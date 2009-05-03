//===-- MSP430AsmPrinter.cpp - MSP430 LLVM assembly writer ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to the MSP430 assembly language.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "asm-printer"
#include "MSP430.h"
#include "MSP430InstrInfo.h"
#include "MSP430TargetMachine.h"
#include "llvm/Constants.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Module.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/DwarfWriter.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/Target/TargetAsmInfo.h"
#include "llvm/Target/TargetData.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/Mangler.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

STATISTIC(EmittedInsts, "Number of machine instrs printed");

namespace {
  class VISIBILITY_HIDDEN MSP430AsmPrinter : public AsmPrinter {
  public:
    MSP430AsmPrinter(raw_ostream &O, MSP430TargetMachine &TM,
                    const TargetAsmInfo *TAI, bool Fast, bool Verbose)
      : AsmPrinter(O, TM, TAI, Fast, Verbose) {}

    virtual const char *getPassName() const {
      return "MSP430 Assembly Printer";
    }

    void printOperand(const MachineInstr *MI, int OpNum,
                      const char* Modifier = 0);
    void printSrcMemOperand(const MachineInstr *MI, int OpNum,
                            const char* Modifier = 0);
    void printCCOperand(const MachineInstr *MI, int OpNum);
    bool printInstruction(const MachineInstr *MI);  // autogenerated.
    void printMachineInstruction(const MachineInstr * MI);
    bool runOnMachineFunction(MachineFunction &F);
    bool doInitialization(Module &M);
    bool doFinalization(Module &M);

    void getAnalysisUsage(AnalysisUsage &AU) const {
      AsmPrinter::getAnalysisUsage(AU);
      AU.setPreservesAll();
    }
  };
} // end of anonymous namespace

#include "MSP430GenAsmWriter.inc"

/// createMSP430CodePrinterPass - Returns a pass that prints the MSP430
/// assembly code for a MachineFunction to the given output stream,
/// using the given target machine description.  This should work
/// regardless of whether the function is in SSA form.
///
FunctionPass *llvm::createMSP430CodePrinterPass(raw_ostream &o,
                                                MSP430TargetMachine &tm,
                                                bool fast, bool verbose) {
  return new MSP430AsmPrinter(o, tm, tm.getTargetAsmInfo(), fast, verbose);
}

bool MSP430AsmPrinter::doInitialization(Module &M) {
  Mang = new Mangler(M, "", TAI->getPrivateGlobalPrefix());
  return false; // success
}


bool MSP430AsmPrinter::doFinalization(Module &M) {
  return AsmPrinter::doFinalization(M);
}

bool MSP430AsmPrinter::runOnMachineFunction(MachineFunction &MF) {
  // Print out code for the function.
  for (MachineFunction::const_iterator I = MF.begin(), E = MF.end();
       I != E; ++I) {
    // Print a label for the basic block.
    if (I != MF.begin()) {
      printBasicBlockLabel(I, true , true);
      O << '\n';
    }

    for (MachineBasicBlock::const_iterator II = I->begin(), E = I->end();
         II != E; ++II) {
      // Print the assembly for the instruction.
      O << "\t";
      printMachineInstruction(II);
    }

    // Each Basic Block is separated by a newline
    O << '\n';
  }

  // We didn't modify anything
  return false;
}

void MSP430AsmPrinter::printMachineInstruction(const MachineInstr *MI) {
  ++EmittedInsts;

  // Call the autogenerated instruction printer routines.
  if (printInstruction(MI))
    return;

  assert(0 && "Should not happen");
}

void MSP430AsmPrinter::printOperand(const MachineInstr *MI, int OpNum,
                                    const char* Modifier) {
  const MachineOperand &MO = MI->getOperand(OpNum);
  switch (MO.getType()) {
  case MachineOperand::MO_Register:
    assert (TargetRegisterInfo::isPhysicalRegister(MO.getReg()) &&
            "Virtual registers should be already mapped!");
    O << TM.getRegisterInfo()->get(MO.getReg()).AsmName;
    return;
  case MachineOperand::MO_Immediate:
    if (!Modifier || strcmp(Modifier, "nohash"))
      O << '#';
    O << MO.getImm();
    return;
  case MachineOperand::MO_MachineBasicBlock:
    printBasicBlockLabel(MO.getMBB());
    return;
  case MachineOperand::MO_GlobalAddress: {
    bool isMemOp  = Modifier && !strcmp(Modifier, "mem");
    bool isCallOp = Modifier && !strcmp(Modifier, "call");
    std::string Name = Mang->getValueName(MO.getGlobal());
    assert(MO.getOffset() == 0 && "No offsets allowed!");

    if (isCallOp)
      O << '#';
    else if (isMemOp)
      O << '&';

    O << Name;

    return;
  }
  case MachineOperand::MO_ExternalSymbol: {
    bool isCallOp = Modifier && !strcmp(Modifier, "call");
    std::string Name(TAI->getGlobalPrefix());
    Name += MO.getSymbolName();
    if (isCallOp)
      O << '#';
    O << Name;
    return;
  }
  default:
    assert(0 && "Not implemented yet!");
  }
}

void MSP430AsmPrinter::printSrcMemOperand(const MachineInstr *MI, int OpNum,
                                          const char* Modifier) {
  const MachineOperand &Base = MI->getOperand(OpNum);
  const MachineOperand &Disp = MI->getOperand(OpNum+1);

  if (Base.isGlobal())
    printOperand(MI, OpNum, "mem");
  else if (Disp.isImm() && !Base.getReg())
    printOperand(MI, OpNum);
  else if (Base.getReg()) {
    if (Disp.getImm()) {
      printOperand(MI, OpNum + 1, "nohash");
      O << '(';
      printOperand(MI, OpNum);
      O << ')';
    } else {
      O << '@';
      printOperand(MI, OpNum);
    }
  } else
    assert(0 && "Unsupported memory operand");
}

void MSP430AsmPrinter::printCCOperand(const MachineInstr *MI, int OpNum) {
  unsigned CC = MI->getOperand(OpNum).getImm();

  switch (CC) {
  default:
   assert(0 && "Unsupported CC code");
   break;
  case MSP430::COND_E:
   O << 'e';
   break;
  case MSP430::COND_NE:
   O << "ne";
   break;
  case MSP430::COND_HS:
   O << "hs";
   break;
  case MSP430::COND_LO:
   O << "lo";
   break;
  case MSP430::COND_GE:
   O << "ge";
   break;
  case MSP430::COND_L:
   O << 'l';
   break;
  }
}
