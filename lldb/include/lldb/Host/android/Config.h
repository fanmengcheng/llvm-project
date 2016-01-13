//===-- Config.h -----------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//----------------------------------------------------------------------
// LLDB currently doesn't have a dynamic configuration mechanism, so we
// are going to hardcode things for now. Eventually these files will
// be auto generated by some configuration script that can detect
// platform functionality availability.
//----------------------------------------------------------------------

#ifndef liblldb_Platform_Config_h_
#define liblldb_Platform_Config_h_

#define LLDB_CONFIG_TERMIOS_SUPPORTED 1

//#define LLDB_CONFIG_TILDE_RESOLVES_TO_USER 1

//#define LLDB_CONFIG_DLOPEN_RTLD_FIRST_SUPPORTED 1

//#define LLDB_CONFIG_FCNTL_GETPATH_SUPPORTED 1

#endif // #ifndef liblldb_Platform_Config_h_
