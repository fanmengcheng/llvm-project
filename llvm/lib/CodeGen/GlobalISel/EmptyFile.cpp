//===-- llvm/CodeGen/GlobalISel/EmptyFile.cpp ------ EmptyFile ---*- C++ -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
/// \file
/// The purpose of this file is to please cmake by not creating an
/// empty library when we do not build GlobalISel.
/// \todo This file should be removed when GlobalISel is not optional anymore.
//===----------------------------------------------------------------------===//

#include "llvm/Support/Compiler.h"


namespace llvm {
// Export a global symbol so that ranlib does not complain
// about the TOC being empty for the global-isel library when
// we do not build global-isel.
LLVM_ATTRIBUTE_UNUSED void DummyFunctionToSilenceRanlib(void) {
}
}
