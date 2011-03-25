//===-- DisassemblerLLVM.h --------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef liblldb_DisassemblerLLVM_h_
#define liblldb_DisassemblerLLVM_h_


#include "llvm-c/EnhancedDisassembly.h"

#include "lldb/Core/Disassembler.h"
#include "lldb/Host/Mutex.h"

class DisassemblerLLVM : public lldb_private::Disassembler
{
public:
    class InstructionLLVM : public lldb_private::Instruction
    {
    public:
        InstructionLLVM (const lldb_private::Address &addr,
                         EDDisassemblerRef disassembler);

        virtual
        ~InstructionLLVM();

        virtual void
        Dump (lldb_private::Stream *s,
              bool show_address,
              bool show_bytes,
              const lldb_private::ExecutionContext* exe_ctx,
              bool raw);

        virtual bool
        DoesBranch () const;

        virtual size_t
        Extract (const Disassembler &disassembler,
                 const lldb_private::DataExtractor &data,
                 uint32_t data_offset);

    protected:
        EDDisassemblerRef m_disassembler;
        EDInstRef m_inst;
    };

    //------------------------------------------------------------------
    // Static Functions
    //------------------------------------------------------------------
    static void
    Initialize();

    static void
    Terminate();

    static const char *
    GetPluginNameStatic();

    static const char *
    GetPluginDescriptionStatic();

    static lldb_private::Disassembler *
    CreateInstance(const lldb_private::ArchSpec &arch);


    DisassemblerLLVM(const lldb_private::ArchSpec &arch);

    virtual
    ~DisassemblerLLVM();

    size_t
    DecodeInstructions (const lldb_private::Address &base_addr,
                        const lldb_private::DataExtractor& data,
                        uint32_t data_offset,
                        uint32_t num_instructions,
                        bool append);
    
    //------------------------------------------------------------------
    // PluginInterface protocol
    //------------------------------------------------------------------
    virtual const char *
    GetPluginName();

    virtual const char *
    GetShortPluginName();

    virtual uint32_t
    GetPluginVersion();

protected:
    bool
    IsValid() const
    {
        return m_disassembler != NULL;
    }

    EDDisassemblerRef m_disassembler;
    EDDisassemblerRef m_disassembler_thumb;
};

#endif  // liblldb_DisassemblerLLVM_h_
