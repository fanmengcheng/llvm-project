//===-- SWIG Interface for SBModule -----------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

namespace lldb {

%feature("docstring",
"Represents an executable image and its associated object and symbol files.

The module is designed to be able to select a single slice of an
executable image as it would appear on disk and during program
execution.

You can retrieve SBModule from SBSymbolContext, which in turn is available
from SBFrame.

SBModule supports symbol iteration, for example,

    for symbol in module:
        name = symbol.GetName()
        saddr = symbol.GetStartAddress()
        eaddr = symbol.GetEndAddress()

and rich comparion methods which allow the API program to use,

    if thisModule == thatModule:
        print 'This module is the same as that module'

to test module equality.  A module also contains object file sections, namely
SBSection.  SBModule supports section iteration through section_iter(), for
example,

    print 'Number of sections: %d' % module.GetNumSections()
    for sec in module.section_iter():
        print sec

And to iterate the symbols within a SBSection, use symbol_in_section_iter(),

    # Iterates the text section and prints each symbols within each sub-section.
    for subsec in text_sec:
        print INDENT + repr(subsec)
        for sym in exe_module.symbol_in_section_iter(subsec):
            print INDENT2 + repr(sym)
            print INDENT2 + 'symbol type: %s' % symbol_type_to_str(sym.GetType())

produces this following output:

    [0x0000000100001780-0x0000000100001d5c) a.out.__TEXT.__text
        id = {0x00000004}, name = 'mask_access(MaskAction, unsigned int)', range = [0x00000001000017c0-0x0000000100001870)
        symbol type: code
        id = {0x00000008}, name = 'thread_func(void*)', range = [0x0000000100001870-0x00000001000019b0)
        symbol type: code
        id = {0x0000000c}, name = 'main', range = [0x00000001000019b0-0x0000000100001d5c)
        symbol type: code
        id = {0x00000023}, name = 'start', address = 0x0000000100001780
        symbol type: code
    [0x0000000100001d5c-0x0000000100001da4) a.out.__TEXT.__stubs
        id = {0x00000024}, name = '__stack_chk_fail', range = [0x0000000100001d5c-0x0000000100001d62)
        symbol type: trampoline
        id = {0x00000028}, name = 'exit', range = [0x0000000100001d62-0x0000000100001d68)
        symbol type: trampoline
        id = {0x00000029}, name = 'fflush', range = [0x0000000100001d68-0x0000000100001d6e)
        symbol type: trampoline
        id = {0x0000002a}, name = 'fgets', range = [0x0000000100001d6e-0x0000000100001d74)
        symbol type: trampoline
        id = {0x0000002b}, name = 'printf', range = [0x0000000100001d74-0x0000000100001d7a)
        symbol type: trampoline
        id = {0x0000002c}, name = 'pthread_create', range = [0x0000000100001d7a-0x0000000100001d80)
        symbol type: trampoline
        id = {0x0000002d}, name = 'pthread_join', range = [0x0000000100001d80-0x0000000100001d86)
        symbol type: trampoline
        id = {0x0000002e}, name = 'pthread_mutex_lock', range = [0x0000000100001d86-0x0000000100001d8c)
        symbol type: trampoline
        id = {0x0000002f}, name = 'pthread_mutex_unlock', range = [0x0000000100001d8c-0x0000000100001d92)
        symbol type: trampoline
        id = {0x00000030}, name = 'rand', range = [0x0000000100001d92-0x0000000100001d98)
        symbol type: trampoline
        id = {0x00000031}, name = 'strtoul', range = [0x0000000100001d98-0x0000000100001d9e)
        symbol type: trampoline
        id = {0x00000032}, name = 'usleep', range = [0x0000000100001d9e-0x0000000100001da4)
        symbol type: trampoline
    [0x0000000100001da4-0x0000000100001e2c) a.out.__TEXT.__stub_helper
    [0x0000000100001e2c-0x0000000100001f10) a.out.__TEXT.__cstring
    [0x0000000100001f10-0x0000000100001f68) a.out.__TEXT.__unwind_info
    [0x0000000100001f68-0x0000000100001ff8) a.out.__TEXT.__eh_frame
"
) SBModule;
class SBModule
{
public:

    SBModule ();

    SBModule (const SBModule &rhs);
    
    ~SBModule ();

    bool
    IsValid () const;

    void
    Clear();

    %feature("docstring", "
    //------------------------------------------------------------------
    /// Get const accessor for the module file specification.
    ///
    /// This function returns the file for the module on the host system
    /// that is running LLDB. This can differ from the path on the 
    /// platform since we might be doing remote debugging.
    ///
    /// @return
    ///     A const reference to the file specification object.
    //------------------------------------------------------------------
    ") GetFileSpec;
    lldb::SBFileSpec
    GetFileSpec () const;

    %feature("docstring", "
    //------------------------------------------------------------------
    /// Get accessor for the module platform file specification.
    ///
    /// Platform file refers to the path of the module as it is known on
    /// the remote system on which it is being debugged. For local 
    /// debugging this is always the same as Module::GetFileSpec(). But
    /// remote debugging might mention a file '/usr/lib/liba.dylib'
    /// which might be locally downloaded and cached. In this case the
    /// platform file could be something like:
    /// '/tmp/lldb/platform-cache/remote.host.computer/usr/lib/liba.dylib'
    /// The file could also be cached in a local developer kit directory.
    ///
    /// @return
    ///     A const reference to the file specification object.
    //------------------------------------------------------------------
    ") GetPlatformFileSpec;
    lldb::SBFileSpec
    GetPlatformFileSpec () const;

    bool
    SetPlatformFileSpec (const lldb::SBFileSpec &platform_file);

    %feature("docstring", "Returns the UUID of the module as a Python string."
    ) GetUUIDString;
    const char *
    GetUUIDString () const;

    lldb::SBSection
    FindSection (const char *sect_name);

    lldb::SBAddress
    ResolveFileAddress (lldb::addr_t vm_addr);

    lldb::SBSymbolContext
    ResolveSymbolContextForAddress (const lldb::SBAddress& addr, 
                                    uint32_t resolve_scope);

    bool
    GetDescription (lldb::SBStream &description);

    size_t
    GetNumSymbols ();
    
    lldb::SBSymbol
    GetSymbolAtIndex (size_t idx);

    size_t
    GetNumSections ();

    lldb::SBSection
    GetSectionAtIndex (size_t idx);


    %feature("docstring", "
    //------------------------------------------------------------------
    /// Find functions by name.
    ///
    /// @param[in] name
    ///     The name of the function we are looking for.
    ///
    /// @param[in] name_type_mask
    ///     A logical OR of one or more FunctionNameType enum bits that
    ///     indicate what kind of names should be used when doing the
    ///     lookup. Bits include fully qualified names, base names,
    ///     C++ methods, or ObjC selectors. 
    ///     See FunctionNameType for more details.
    ///
    /// @param[in] append
    ///     If true, any matches will be appended to \a sc_list, else
    ///     matches replace the contents of \a sc_list.
    ///
    /// @param[out] sc_list
    ///     A symbol context list that gets filled in with all of the
    ///     matches.
    ///
    /// @return
    ///     The number of matches added to \a sc_list.
    //------------------------------------------------------------------
    ") FindFunctions;
    uint32_t
    FindFunctions (const char *name, 
                   uint32_t name_type_mask, // Logical OR one or more FunctionNameType enum bits
                   bool append, 
                   lldb::SBSymbolContextList& sc_list);
    
    lldb::SBType
    FindFirstType (const char* name);

    lldb::SBTypeList
    FindTypes (const char* type);


    %feature("docstring", "
    //------------------------------------------------------------------
    /// Find global and static variables by name.
    ///
    /// @param[in] target
    ///     A valid SBTarget instance representing the debuggee.
    ///
    /// @param[in] name
    ///     The name of the global or static variable we are looking
    ///     for.
    ///
    /// @param[in] max_matches
    ///     Allow the number of matches to be limited to \a max_matches.
    ///
    /// @return
    ///     A list of matched variables in an SBValueList.
    //------------------------------------------------------------------
    ") FindGlobalVariables;
    lldb::SBValueList
    FindGlobalVariables (lldb::SBTarget &target, 
                         const char *name, 
                         uint32_t max_matches);
    
    lldb::ByteOrder
    GetByteOrder ();
    
    uint32_t
    GetAddressByteSize();
    
    const char *
    GetTriple ();

    %pythoncode %{
        class symbols_access(object):
            re_compile_type = type(re.compile('.'))
            '''A helper object that will lazily hand out lldb.SBModule objects for a target when supplied an index, or by full or partial path.'''
            def __init__(self, sbmodule):
                self.sbmodule = sbmodule
        
            def __len__(self):
                if self.sbmodule:
                    return self.sbmodule.GetNumSymbols()
                return 0
        
            def __getitem__(self, key):
                count = len(self)
                if type(key) is int:
                    if key < count:
                        return self.sbmodule.GetSymbolAtIndex(key)
                elif type(key) is str:
                    matches = []
                    for idx in range(count):
                        symbol = self.sbmodule.GetSymbolAtIndex(idx)
                        if symbol.name == key or symbol.mangled == key:
                            matches.append(symbol)
                    return matches
                elif isinstance(key, self.re_compile_type):
                    matches = []
                    for idx in range(count):
                        symbol = self.sbmodule.GetSymbolAtIndex(idx)
                        added = False
                        name = symbol.name
                        if name:
                            re_match = key.search(name)
                            if re_match:
                                matches.append(symbol)
                                added = True
                        if not added:
                            mangled = symbol.mangled
                            if mangled:
                                re_match = key.search(mangled)
                                if re_match:
                                    matches.append(symbol)
                    return matches
                else:
                    print "error: unsupported item type: %s" % type(key)
                return None
        
        def get_symbols_access_object(self):
            '''An accessor function that returns a symbols_access() object which allows lazy symbol access from a lldb.SBModule object.'''
            return self.symbols_access (self)
        
        def get_symbols_array(self):
            '''An accessor function that returns a list() that contains all symbols in a lldb.SBModule object.'''
            symbols = []
            for idx in range(self.num_symbols):
                symbols.append(self.GetSymbolAtIndex(idx))
            return symbols

        class sections_access(object):
            re_compile_type = type(re.compile('.'))
            '''A helper object that will lazily hand out lldb.SBModule objects for a target when supplied an index, or by full or partial path.'''
            def __init__(self, sbmodule):
                self.sbmodule = sbmodule
        
            def __len__(self):
                if self.sbmodule:
                    return self.sbmodule.GetNumSections()
                return 0
        
            def __getitem__(self, key):
                count = len(self)
                if type(key) is int:
                    if key < count:
                        return self.sbmodule.GetSectionAtIndex(key)
                elif type(key) is str:
                    for idx in range(count):
                        section = self.sbmodule.GetSectionAtIndex(idx)
                        if section.name == key:
                            return section
                elif isinstance(key, self.re_compile_type):
                    matches = []
                    for idx in range(count):
                        section = self.sbmodule.GetSectionAtIndex(idx)
                        name = section.name
                        if name:
                            re_match = key.search(name)
                            if re_match:
                                matches.append(section)
                    return matches
                else:
                    print "error: unsupported item type: %s" % type(key)
                return None
        
        def get_sections_access_object(self):
            '''An accessor function that returns a sections_access() object which allows lazy section array access.'''
            return self.sections_access (self)
        
        def get_sections_array(self):
            '''An accessor function that returns an array object that contains all sections in this module object.'''
            if not hasattr(self, 'sections'):
                self.sections = []
                for idx in range(self.num_sections):
                    self.sections.append(self.GetSectionAtIndex(idx))
            return self.sections

        __swig_getmethods__["symbols"] = get_symbols_array
        if _newclass: x = property(get_symbols_array, None)

        __swig_getmethods__["symbol"] = get_symbols_access_object
        if _newclass: x = property(get_symbols_access_object, None)

        __swig_getmethods__["sections"] = get_sections_array
        if _newclass: x = property(get_sections_array, None)
        
        __swig_getmethods__["section"] = get_sections_access_object
        if _newclass: x = property(get_sections_access_object, None)

        def get_uuid(self):
            return uuid.UUID (self.GetUUIDString())
        
        __swig_getmethods__["uuid"] = get_uuid
        if _newclass: x = property(get_uuid, None)
        
        __swig_getmethods__["file"] = GetFileSpec
        if _newclass: x = property(GetFileSpec, None)
        
        __swig_getmethods__["platform_file"] = GetPlatformFileSpec
        if _newclass: x = property(GetPlatformFileSpec, None)
        
        __swig_getmethods__["byte_order"] = GetByteOrder
        if _newclass: x = property(GetByteOrder, None)
        
        __swig_getmethods__["addr_size"] = GetAddressByteSize
        if _newclass: x = property(GetAddressByteSize, None)
        
        __swig_getmethods__["triple"] = GetTriple
        if _newclass: x = property(GetTriple, None)

        __swig_getmethods__["num_symbols"] = GetNumSymbols
        if _newclass: x = property(GetNumSymbols, None)
        
        __swig_getmethods__["num_sections"] = GetNumSections
        if _newclass: x = property(GetNumSections, None)
        
    %}

};

} // namespace lldb
