//===-- SystemZSubtarget.h - SystemZ subtarget information -----*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the SystemZ specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_SYSTEMZ_SYSTEMZSUBTARGET_H
#define LLVM_LIB_TARGET_SYSTEMZ_SYSTEMZSUBTARGET_H

#include "SystemZFrameLowering.h"
#include "SystemZISelLowering.h"
#include "SystemZInstrInfo.h"
#include "SystemZRegisterInfo.h"
#include "SystemZSelectionDAGInfo.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Target/TargetSubtargetInfo.h"
#include <string>

#define GET_SUBTARGETINFO_HEADER
#include "SystemZGenSubtargetInfo.inc"

namespace llvm {
class GlobalValue;
class StringRef;

class SystemZSubtarget : public SystemZGenSubtargetInfo {
  virtual void anchor();
protected:
  bool HasDistinctOps;
  bool HasLoadStoreOnCond;
  bool HasHighWord;
  bool HasFPExtension;
  bool HasPopulationCount;
  bool HasFastSerialization;
  bool HasInterlockedAccess1;
  bool HasMiscellaneousExtensions;
  bool HasTransactionalExecution;
  bool HasProcessorAssist;
  bool HasVector;
  bool HasLoadStoreOnCond2;
  bool HasLoadAndZeroRightmostByte;

private:
  Triple TargetTriple;
  SystemZInstrInfo InstrInfo;
  SystemZTargetLowering TLInfo;
  SystemZSelectionDAGInfo TSInfo;
  SystemZFrameLowering FrameLowering;

  SystemZSubtarget &initializeSubtargetDependencies(StringRef CPU,
                                                    StringRef FS);
public:
  SystemZSubtarget(const Triple &TT, const std::string &CPU,
                   const std::string &FS, const TargetMachine &TM);

  const TargetFrameLowering *getFrameLowering() const override {
    return &FrameLowering;
  }
  const SystemZInstrInfo *getInstrInfo() const override { return &InstrInfo; }
  const SystemZRegisterInfo *getRegisterInfo() const override {
    return &InstrInfo.getRegisterInfo();
  }
  const SystemZTargetLowering *getTargetLowering() const override {
    return &TLInfo;
  }
  const SelectionDAGTargetInfo *getSelectionDAGInfo() const override {
    return &TSInfo;
  }

  // This is important for reducing register pressure in vector code.
  bool useAA() const override { return true; }

  // Always enable the early if-conversion pass.
  bool enableEarlyIfConversion() const override { return true; }

  // Automatically generated by tblgen.
  void ParseSubtargetFeatures(StringRef CPU, StringRef FS);

  // Return true if the target has the distinct-operands facility.
  bool hasDistinctOps() const { return HasDistinctOps; }

  // Return true if the target has the load/store-on-condition facility.
  bool hasLoadStoreOnCond() const { return HasLoadStoreOnCond; }

  // Return true if the target has the load/store-on-condition facility 2.
  bool hasLoadStoreOnCond2() const { return HasLoadStoreOnCond2; }

  // Return true if the target has the high-word facility.
  bool hasHighWord() const { return HasHighWord; }

  // Return true if the target has the floating-point extension facility.
  bool hasFPExtension() const { return HasFPExtension; }

  // Return true if the target has the population-count facility.
  bool hasPopulationCount() const { return HasPopulationCount; }

  // Return true if the target has the fast-serialization facility.
  bool hasFastSerialization() const { return HasFastSerialization; }

  // Return true if the target has interlocked-access facility 1.
  bool hasInterlockedAccess1() const { return HasInterlockedAccess1; }

  // Return true if the target has the miscellaneous-extensions facility.
  bool hasMiscellaneousExtensions() const {
    return HasMiscellaneousExtensions;
  }

  // Return true if the target has the transactional-execution facility.
  bool hasTransactionalExecution() const { return HasTransactionalExecution; }

  // Return true if the target has the processor-assist facility.
  bool hasProcessorAssist() const { return HasProcessorAssist; }

  // Return true if the target has the load-and-zero-rightmost-byte facility.
  bool hasLoadAndZeroRightmostByte() const {
    return HasLoadAndZeroRightmostByte;
  }

  // Return true if the target has the vector facility.
  bool hasVector() const { return HasVector; }

  // Return true if GV can be accessed using LARL for reloc model RM
  // and code model CM.
  bool isPC32DBLSymbol(const GlobalValue *GV, CodeModel::Model CM) const;

  bool isTargetELF() const { return TargetTriple.isOSBinFormatELF(); }
};
} // end namespace llvm

#endif
