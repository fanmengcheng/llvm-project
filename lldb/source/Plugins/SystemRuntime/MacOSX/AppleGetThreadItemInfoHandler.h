//===-- AppleGetThreadItemInfoHandler.h ----------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef lldb_AppleGetThreadItemInfoHandler_h_
#define lldb_AppleGetThreadItemInfoHandler_h_

// C Includes
// C++ Includes
#include <map>
#include <vector>
// Other libraries and framework includes
// Project includes
#include "lldb/lldb-public.h"
#include "lldb/Core/Error.h"
#include "lldb/Expression/ClangFunction.h"
#include "lldb/Host/Mutex.h"
#include "lldb/Symbol/ClangASTType.h"

// This class will insert a ClangUtilityFunction into the inferior process for
// calling libBacktraceRecording's __introspection_dispatch_thread_get_item_info()
// function.  The function in the inferior will return a struct by value
// with these members:
//
//     struct get_thread_item_info_return_values
//     {
//         introspection_dispatch_item_info_ref *item_buffer;
//         uint64_t item_buffer_size;
//     };
//
// The item_buffer pointer is an address in the inferior program's address
// space (item_buffer_size in size) which must be mach_vm_deallocate'd by
// lldb.  
//
// The AppleGetThreadItemInfoHandler object should persist so that the ClangUtilityFunction
// can be reused multiple times.

namespace lldb_private
{

class AppleGetThreadItemInfoHandler {
public:

    AppleGetThreadItemInfoHandler (lldb_private::Process *process);

    ~AppleGetThreadItemInfoHandler();

    struct GetThreadItemInfoReturnInfo
    {
        lldb::addr_t    item_buffer_ptr;  /* the address of the item buffer from libBacktraceRecording */
        lldb::addr_t    item_buffer_size; /* the size of the item buffer from libBacktraceRecording */

        GetThreadItemInfoReturnInfo() :
            item_buffer_ptr(LLDB_INVALID_ADDRESS),
            item_buffer_size(0)
        {}
    };

    //----------------------------------------------------------
    /// Get the information about a work item by calling
    /// __introspection_dispatch_thread_get_item_info.  If there's a page of
    /// memory that needs to be freed, pass in the address and size and it will
    /// be freed before getting the list of queues.
    ///
    /// @param [in] thread_id
    ///     The thread to get the extended backtrace for.
    ///
    /// @param [in] page_to_free
    ///     An address of an inferior process vm page that needs to be deallocated,
    ///     LLDB_INVALID_ADDRESS if this is not needed.
    ///
    /// @param [in] page_to_free_size
    ///     The size of the vm page that needs to be deallocated if an address was
    ///     passed in to page_to_free.
    ///
    /// @param [out] error
    ///     This object will be updated with the error status / error string from any failures encountered.
    ///
    /// @returns
    ///     The result of the inferior function call execution.  If there was a failure of any kind while getting
    ///     the information, the item_buffer_ptr value will be LLDB_INVALID_ADDRESS.
    //----------------------------------------------------------
    GetThreadItemInfoReturnInfo
    GetThreadItemInfo (Thread &thread, lldb::tid_t thread_id, lldb::addr_t page_to_free, uint64_t page_to_free_size, lldb_private::Error &error);


    void
    Detach ();

private:

    lldb::addr_t
    SetupGetThreadItemInfoFunction (Thread &thread, ValueList &get_thread_item_info_arglist);

    static const char *g_get_thread_item_info_function_name;
    static const char *g_get_thread_item_info_function_code;

    lldb_private::Process *m_process;
    std::unique_ptr<ClangFunction> m_get_thread_item_info_function;
    std::unique_ptr<ClangUtilityFunction> m_get_thread_item_info_impl_code;
    Mutex m_get_thread_item_info_function_mutex;

    lldb::addr_t m_get_thread_item_info_return_buffer_addr;
    Mutex m_get_thread_item_info_retbuffer_mutex;

};

}  // using namespace lldb_private

#endif	// lldb_AppleGetThreadItemInfoHandler_h_
