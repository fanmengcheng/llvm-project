#!/usr/bin/python

#----------------------------------------------------------------------
# Be sure to add the python path that points to the LLDB shared library.
#
# To use this in the embedded python interpreter using "lldb":
#
#   cd /path/containing/crashlog.py
#   lldb
#   (lldb) script import crashlog
#   "crashlog" command installed, type "crashlog --help" for detailed help
#   (lldb) crashlog ~/Library/Logs/DiagnosticReports/a.crash
#
# The benefit of running the crashlog command inside lldb in the 
# embedded python interpreter is when the command completes, there 
# will be a target with all of the files loaded at the locations
# described in the crash log. Only the files that have stack frames
# in the backtrace will be loaded unless the "--load-all" option
# has been specified. This allows users to explore the program in the
# state it was in right at crash time. 
#
# On MacOSX csh, tcsh:
#   ( setenv PYTHONPATH /path/to/LLDB.framework/Resources/Python ; ./crashlog.py ~/Library/Logs/DiagnosticReports/a.crash )
#
# On MacOSX sh, bash:
#   PYTHONPATH=/path/to/LLDB.framework/Resources/Python ./crashlog.py ~/Library/Logs/DiagnosticReports/a.crash
#----------------------------------------------------------------------

import lldb
import commands
import optparse
import os
import plistlib
import re
import shlex
import sys
import time
import uuid

class Address:
    """Class that represents an address that will be symbolicated"""
    def __init__(self, target, load_addr):
        self.target = target
        self.load_addr = load_addr # The load address that this object represents
        self.so_addr = None # the resolved lldb.SBAddress (if any), named so_addr for section/offset address
        self.sym_ctx = None # The cached symbol context for this address
        self.description = None # Any original textual description of this address to be used as a backup in case symbolication fails
        self.symbolication = None # The cached symbolicated string that describes this address
        self.inlined = False
    def __str__(self):
        s = "%#16.16x" % (self.load_addr)
        if self.symbolication:
            s += " %s" % (self.symbolication)
        elif self.description:
            s += " %s" % (self.description)
        elif self.so_addr:
            s += " %s" % (self.so_addr)
        return s

    def resolve_addr(self):
        if self.so_addr == None:
            self.so_addr = self.target.ResolveLoadAddress (self.load_addr)
        return self.so_addr

    def is_inlined(self):
        return self.inlined
    
    def get_symbol_context(self):
        if self.sym_ctx == None:
            sb_addr = self.resolve_addr()
            if sb_addr:
                self.sym_ctx = self.target.ResolveSymbolContextForAddress (sb_addr, lldb.eSymbolContextEverything)
            else:
                self.sym_ctx = lldb.SBSymbolContext()
        return self.sym_ctx

    def get_instructions(self):
        sym_ctx = self.get_symbol_context()
        if sym_ctx:
            function = sym_ctx.GetFunction()
            if function:
                return function.GetInstructions(self.target)
            return sym_ctx.GetSymbol().GetInstructions(self.target)
        return None
    
    def symbolicate(self):
        if self.symbolication == None:
            self.symbolication = ''
            self.inlined = False
            sym_ctx = self.get_symbol_context()
            if sym_ctx:
                module = sym_ctx.GetModule()
                if module:
                    self.symbolication += module.GetFileSpec().GetFilename() + '`'
                    function_start_load_addr = -1
                    function = sym_ctx.GetFunction()
                    block = sym_ctx.GetBlock()
                    line_entry = sym_ctx.GetLineEntry()
                    symbol = sym_ctx.GetSymbol()
                    inlined_block = block.GetContainingInlinedBlock();
                    if function:
                        self.symbolication += function.GetName()

                        if inlined_block:
                            self.inlined = True
                            self.symbolication += ' [inlined] ' + inlined_block.GetInlinedName();
                            block_range_idx = inlined_block.GetRangeIndexForBlockAddress (self.so_addr)
                            if block_range_idx < lldb.UINT32_MAX:
                                block_range_start_addr = inlined_block.GetRangeStartAddress (block_range_idx)
                                function_start_load_addr = block_range_start_addr.GetLoadAddress (self.target)
                        if function_start_load_addr == -1:
                            function_start_load_addr = function.GetStartAddress().GetLoadAddress (self.target)
                    elif symbol:
                        self.symbolication += symbol.GetName()
                        function_start_load_addr = symbol.GetStartAddress().GetLoadAddress (self.target)
                    else:
                        self.symbolication = ''
                        return False

                    # Dump the offset from the current function or symbol if it is non zero
                    function_offset = self.load_addr - function_start_load_addr
                    if function_offset > 0:
                        self.symbolication += " + %u" % (function_offset)
                    elif function_offset < 0:
                        self.symbolication += " %i (invalid negative offset, file a bug) " % function_offset

                    # Print out any line information if any is available
                    if line_entry.GetFileSpec():
                            self.symbolication += ' at %s' % line_entry.GetFileSpec().GetFilename()
                            self.symbolication += ':%u' % line_entry.GetLine ()
                            column = line_entry.GetColumn()
                            if column > 0:
                                self.symbolication += ':%u' % column
                    return True
        return False

class Section:
    """Class that represents an load address range"""
    sect_info_regex = re.compile('(?P<name>[^=]+)=(?P<range>.*)')
    addr_regex = re.compile('^\s*(?P<start>0x[0-9A-Fa-f]+)\s*$')
    range_regex = re.compile('^\s*(?P<start>0x[0-9A-Fa-f]+)\s*(?P<op>[-+])\s*(?P<end>0x[0-9A-Fa-f]+)\s*$')

    def __init__(self, start_addr = None, end_addr = None, name = None):
        self.start_addr = start_addr
        self.end_addr = end_addr
        self.name = name
    
    def contains(self, addr):
        return self.start_addr <= addr and addr < self.end_addr;
    
    def set_from_string(self, s):
        match = self.sect_info_regex.match (s)
        if match:
            self.name = match.group('name')
            range_str = match.group('range')
            addr_match = self.addr_regex.match(range_str)
            if addr_match:
                self.start_addr = int(addr_match.group('start'), 16)
                self.end_addr = None
                return True

            range_match = self.range_regex.match(range_str)
            if range_match:
                self.start_addr = int(range_match.group('start'), 16)
                self.end_addr = int(range_match.group('end'), 16)
                op = range_match.group('op')
                if op == '+':
                    self.end_addr += self.start_addr
                return True
        print 'error: invalid section info string "%s"' % s
        print 'Valid section info formats are:'
        print 'Format                Example                    Description'
        print '--------------------- -----------------------------------------------'
        print '<name>=<base>        __TEXT=0x123000             Section from base address only'
        print '<name>=<base>-<end>  __TEXT=0x123000-0x124000    Section from base address and end address'
        print '<name>=<base>+<size> __TEXT=0x123000+0x1000      Section from base address and size'
        return False
        
    def __str__(self):
        if self.name:
            if self.end_addr != None:
                if self.start_addr != None:
                    return "%s=[0x%16.16x - 0x%16.16x)" % (self.name, self.start_addr, self.end_addr)
            else:
                if self.start_addr != None:
                    return "%s=0x%16.16x" % (self.name, self.start_addr)
            return self.name
        return "<invalid>"
            
class Image:
    """A class that represents an executable image and any associated data"""
    
    def __init__(self, path, uuid = None):
        self.path = path
        self.resolved_path = None
        self.uuid = uuid
        self.section_infos = list()
        self.identifier = None
        self.version = None
        self.arch = None
        self.module = None
        self.symfile = None
        self.slide = None
    
    def dump(self, prefix):
        print "%s%s" % (prefix, self)
    
    def __str__(self):
        s = "%s %s" % (self.get_uuid(), self.get_resolved_path())
        for section_info in self.section_infos:
            s += ", %s" % (section_info)
        if self.slide != None:
            s += ', slide = 0x%16.16x' % self.slide
        return s        
    
    def add_section(self, section):
        #print "added '%s' to '%s'" % (section, self.path)
        self.section_infos.append (section)
        
    def get_section_containing_load_addr (self, load_addr):
        for section_info in self.section_infos:
            if section_info.contains(load_addr):
                return section_info
        return None

    def get_resolved_path(self):
        if self.resolved_path:
            return self.resolved_path
        elif self.path:
            return self.path
        return None

    def get_resolved_path_basename(self):
        path = self.get_resolved_path()
        if path:
            return os.path.basename(path)
        return None

    def symfile_basename(self):
        if self.symfile:
            return os.path.basename(self.symfile)
        return None
    
    def has_section_load_info(self):
        return self.section_infos or self.slide != None
    
    def load_module(self, target):
        # Load this module into "target" using the section infos to
        # set the section load addresses
        if self.has_section_load_info():
            if target:
                if self.module:
                    if self.section_infos:
                        num_sections_loaded = 0
                        for section_info in self.section_infos:
                            if section_info.name:
                                section = self.module.FindSection (section_info.name)
                                if section:
                                    error = target.SetSectionLoadAddress (section, section_info.start_addr)
                                    if error.Success():
                                        num_sections_loaded += 1
                                    else:
                                        return 'error: %s' % error.GetCString()
                                else:
                                    return 'error: unable to find the section named "%s"' % section_info.name
                            else:
                                return 'error: unable to find "%s" section in "%s"' % (range.name, self.get_resolved_path())
                        if num_sections_loaded == 0:
                            return 'error: no sections were successfully loaded'
                    else:
                        err = target.SetModuleLoadAddress(self.module, self.slide)
                        if err.Fail():
                            return err.GetCString()
                    return None
                else:
                    return 'error: invalid module'
            else:
                return 'error: invalid target'
        else:
            return 'error: no section infos'
        
    def add_module(self, target):
        '''Add the Image described in this object to "target" and load the sections if "load" is True.'''
        if target:
            resolved_path = self.get_resolved_path();
            # Try and find using UUID only first so that paths need not match up
            if self.uuid:
                self.module = target.AddModule (None, None, str(self.uuid))
            if not self.module:
                if self.locate_module_and_debug_symbols ():
                    path_spec = lldb.SBFileSpec (resolved_path)
                    #print 'target.AddModule (path="%s", arch="%s", uuid=%s)' % (resolved_path, self.arch, self.uuid)
                    self.module = target.AddModule (resolved_path, self.arch, self.uuid)
            if not self.module:
                return 'error: unable to get module for (%s) "%s"' % (self.arch, resolved_path)
            if self.has_section_load_info():
                return self.load_module(target)
            else:
                return None # No sections, the module was added to the target, so success
        else:
            return 'error: invalid target'
    
    def locate_module_and_debug_symbols (self):
        # By default, just use the paths that were supplied in:
        # self.path
        # self.resolved_path
        # self.module
        # self.symfile
        # Subclasses can inherit from this class and override this function
        return True
    
    def get_uuid(self):
        if not self.uuid:
            self.uuid = uuid.UUID(self.module.GetUUIDString())
        return self.uuid

    def create_target(self):
        '''Create a target using the information in this Image object.'''
        if self.locate_module_and_debug_symbols ():
            resolved_path = self.get_resolved_path();
            path_spec = lldb.SBFileSpec (resolved_path)
            #result.PutCString ('plist[%s] = %s' % (uuid, self.plist))
            error = lldb.SBError()
            target = lldb.debugger.CreateTarget (resolved_path, self.arch, None, False, error);
            if target:
                self.module = target.FindModule(path_spec)
                if self.has_section_load_info():
                    err = self.load_module(target)
                    if err:
                        print 'ERROR: ', err
                return target
            else:
                print 'error: unable to create a valid target for (%s) "%s"' % (self.arch, self.path)
        else:
            print 'error: unable to locate main executable (%s) "%s"' % (self.arch, self.path)
        return None
    
class Symbolicator:

    def __init__(self):
        """A class the represents the information needed to symbolicate addresses in a program"""
        self.target = None
        self.images = list() # a list of images to be used when symbolicating

    
    def __str__(self):
        s = "Symbolicator:\n"
        if self.target:
            s += "Target = '%s'\n" % (self.target)
            s += "Target modules:'\n"
            for m in self.target.modules:
                print m
        s += "Images:\n"
        for image in self.images:
            s += '    %s\n' % (image)
        return s
    
    def find_images_with_identifier(self, identifier):
        images = list()
        for image in self.images:
            if image.identifier == identifier:
                images.append(image)
        return images
        
    def find_image_containing_load_addr(self, load_addr):
        for image in self.images:
            if image.contains_addr (load_addr):
                return image
        return None
    
    def create_target(self):
        if self.target:
            return self.target

        if self.images:
            for image in self.images:
                self.target = image.create_target ()
                if self.target:
                    return self.target
        return None
    
    def symbolicate(self, load_addr):
        if self.target:
            symbolicated_address = Address(self.target, load_addr)
            if symbolicated_address.symbolicate ():
            
                if symbolicated_address.so_addr:
                    symbolicated_addresses = list()
                    symbolicated_addresses.append(symbolicated_address)
                    # See if we were able to reconstruct anything?
                    while 1:
                        inlined_parent_so_addr = lldb.SBAddress()
                        inlined_parent_sym_ctx = symbolicated_address.sym_ctx.GetParentOfInlinedScope (symbolicated_address.so_addr, inlined_parent_so_addr)
                        if not inlined_parent_sym_ctx:
                            break
                        if not inlined_parent_so_addr:
                            break

                        symbolicated_address = Address(self.target, inlined_parent_so_addr.GetLoadAddress(self.target))
                        symbolicated_address.sym_ctx = inlined_parent_sym_ctx
                        symbolicated_address.so_addr = inlined_parent_so_addr
                        symbolicated_address.symbolicate ()
                
                        # push the new frame onto the new frame stack
                        symbolicated_addresses.append (symbolicated_address)
            
                    if symbolicated_addresses:
                        return symbolicated_addresses
        return None
            
        
def disassemble_instructions (target, instructions, pc, insts_before_pc, insts_after_pc, non_zeroeth_frame):
    lines = list()
    pc_index = -1
    comment_column = 50
    for inst_idx, inst in enumerate(instructions):
        inst_pc = inst.GetAddress().GetLoadAddress(target);
        if pc == inst_pc:
            pc_index = inst_idx
        mnemonic = inst.GetMnemonic (target)
        operands =  inst.GetOperands (target)
        comment =  inst.GetComment (target)
        #data = inst.GetData (target)
        lines.append ("%#16.16x: %8s %s" % (inst_pc, mnemonic, operands))
        if comment:
            line_len = len(lines[-1])
            if line_len < comment_column:
                lines[-1] += ' ' * (comment_column - line_len)
                lines[-1] += "; %s" % comment

    if pc_index >= 0:
        # If we are disassembling the non-zeroeth frame, we need to backup the PC by 1
        if non_zeroeth_frame and pc_index > 0:
            pc_index = pc_index - 1
        if insts_before_pc == -1:
            start_idx = 0
        else:
            start_idx = pc_index - insts_before_pc
        if start_idx < 0:
            start_idx = 0
        if insts_before_pc == -1:
            end_idx = inst_idx
        else:
            end_idx = pc_index + insts_after_pc
        if end_idx > inst_idx:
            end_idx = inst_idx
        for i in range(start_idx, end_idx+1):
            if i == pc_index:
                print ' -> ', lines[i]
            else:
                print '    ', lines[i]

def print_module_section_data (section):
    print section
    section_data = section.GetSectionData()
    if section_data:
        ostream = lldb.SBStream()
        section_data.GetDescription (ostream, section.GetFileAddress())
        print ostream.GetData()

def print_module_section (section, depth):
    print section
    if depth > 0:
        num_sub_sections = section.GetNumSubSections()
        for sect_idx in range(num_sub_sections):
            print_module_section (section.GetSubSectionAtIndex(sect_idx), depth - 1)

def print_module_sections (module, depth):
    for sect in module.section_iter():
        print_module_section (sect, depth)

def print_module_symbols (module):
    for sym in module:
        print sym

def Symbolicate(command_args):
    
    usage = "usage: %prog [options] <addr1> [addr2 ...]"
    description='''Symbolicate one or more addresses using LLDB's python scripting API..'''
    parser = optparse.OptionParser(description=description, prog='crashlog.py',usage=usage)
    parser.add_option('-v', '--verbose', action='store_true', dest='verbose', help='display verbose debug info', default=False)
    parser.add_option('-p', '--platform', type='string', metavar='platform', dest='platform', help='specify one platform by name')
    parser.add_option('-f', '--file', type='string', metavar='file', dest='file', help='Specify a file to use when symbolicating')
    parser.add_option('-a', '--arch', type='string', metavar='arch', dest='arch', help='Specify a architecture to use when symbolicating')
    parser.add_option('-s', '--slide', type='int', metavar='slide', dest='slide', help='Specify the slide to use on the file specified with the --file option', default=None)
    parser.add_option('--section', type='string', action='append', dest='section_strings', help='specify <sect-name>=<start-addr> or <sect-name>=<start-addr>-<end-addr>')
    try:
        (options, args) = parser.parse_args(command_args)
    except:
        return
    symbolicator = Symbolicator()
    images = list();
    if options.file:
        image = Image(options.file);
        image.arch = options.arch
        # Add any sections that were specified with one or more --section options
        if options.section_strings:
            for section_str in options.section_strings:
                section = Section()
                if section.set_from_string (section_str):
                    image.add_section (section)
                else:
                    sys.exit(1)
        if options.slide != None:
            image.slide = options.slide
        symbolicator.images.append(image)
    
    target = symbolicator.create_target()
    if options.verbose:
        print symbolicator
    if target:
        for addr_str in args:
            addr = int(addr_str, 0)
            symbolicated_addrs = symbolicator.symbolicate(addr)
            for symbolicated_addr in symbolicated_addrs:
                print symbolicated_addr
            print
    else:
        print 'error: no target for %s' % (symbolicator)
        
if __name__ == '__main__':
    # Create a new debugger instance
    lldb.debugger = lldb.SBDebugger.Create()
    Symbolicate (sys.argv[1:])
