//===- X86ISelDAGToDAG.cpp - A DAG pattern matching inst selector for X86 -===//
//
//                     The LLVM Compiler Infrastructure
//
// This file was developed by the Evan Cheng and is distributed under
// the University of Illinois Open Source License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines a DAG pattern matching instruction selector for X86,
// converting from a legalized dag to a X86 dag.
//
//===----------------------------------------------------------------------===//

#include "X86.h"
#include "X86Subtarget.h"
#include "X86ISelLowering.h"
#include "llvm/GlobalValue.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Support/Debug.h"
#include "llvm/ADT/Statistic.h"
using namespace llvm;

//===----------------------------------------------------------------------===//
//                      Pattern Matcher Implementation
//===----------------------------------------------------------------------===//

namespace {
  /// X86ISelAddressMode - This corresponds to X86AddressMode, but uses
  /// SDOperand's instead of register numbers for the leaves of the matched
  /// tree.
  struct X86ISelAddressMode {
    enum {
      RegBase,
      FrameIndexBase,
      ConstantPoolBase
    } BaseType;

    struct {            // This is really a union, discriminated by BaseType!
      SDOperand Reg;
      int FrameIndex;
    } Base;

    unsigned Scale;
    SDOperand IndexReg; 
    unsigned Disp;
    GlobalValue *GV;

    X86ISelAddressMode()
      : BaseType(RegBase), Scale(1), IndexReg(), Disp(0), GV(0) {
    }
  };
}

namespace {
  Statistic<>
  NumFPKill("x86-codegen", "Number of FP_REG_KILL instructions added");

  //===--------------------------------------------------------------------===//
  /// ISel - X86 specific code to select X86 machine instructions for
  /// SelectionDAG operations.
  ///
  class X86DAGToDAGISel : public SelectionDAGISel {
    /// ContainsFPCode - Every instruction we select that uses or defines a FP
    /// register should set this to true.
    bool ContainsFPCode;

    /// X86Lowering - This object fully describes how to lower LLVM code to an
    /// X86-specific SelectionDAG.
    X86TargetLowering X86Lowering;

    /// Subtarget - Keep a pointer to the X86Subtarget around so that we can
    /// make the right decision when generating code for different targets.
    const X86Subtarget *Subtarget;
  public:
    X86DAGToDAGISel(TargetMachine &TM)
      : SelectionDAGISel(X86Lowering), X86Lowering(TM) {
      Subtarget = &TM.getSubtarget<X86Subtarget>();
    }

    virtual const char *getPassName() const {
      return "X86 DAG->DAG Instruction Selection";
    }

    /// InstructionSelectBasicBlock - This callback is invoked by
    /// SelectionDAGISel when it has created a SelectionDAG for us to codegen.
    virtual void InstructionSelectBasicBlock(SelectionDAG &DAG);

// Include the pieces autogenerated from the target description.
#include "X86GenDAGISel.inc"

  private:
    SDOperand Select(SDOperand N);

    bool MatchAddress(SDOperand N, X86ISelAddressMode &AM);
    bool SelectAddr(SDOperand N, SDOperand &Base, SDOperand &Scale,
                    SDOperand &Index, SDOperand &Disp);
    bool SelectLEAAddr(SDOperand N, SDOperand &Base, SDOperand &Scale,
                       SDOperand &Index, SDOperand &Disp);

    /// getI8Imm - Return a target constant with the specified value, of type
    /// i8.
    inline SDOperand getI8Imm(unsigned Imm) {
      return CurDAG->getTargetConstant(Imm, MVT::i8);
    }

    /// getI16Imm - Return a target constant with the specified value, of type
    /// i16.
    inline SDOperand getI16Imm(unsigned Imm) {
      return CurDAG->getTargetConstant(Imm, MVT::i16);
    }

    /// getI32Imm - Return a target constant with the specified value, of type
    /// i32.
    inline SDOperand getI32Imm(unsigned Imm) {
      return CurDAG->getTargetConstant(Imm, MVT::i32);
    }
  };
}

/// InstructionSelectBasicBlock - This callback is invoked by SelectionDAGISel
/// when it has created a SelectionDAG for us to codegen.
void X86DAGToDAGISel::InstructionSelectBasicBlock(SelectionDAG &DAG) {
  DEBUG(BB->dump());

  // Codegen the basic block.
  DAG.setRoot(Select(DAG.getRoot()));
  DAG.RemoveDeadNodes();

  // Emit machine code to BB. 
  ScheduleAndEmitDAG(DAG);
}

/// FIXME: copied from X86ISelPattern.cpp
/// MatchAddress - Add the specified node to the specified addressing mode,
/// returning true if it cannot be done.  This just pattern matches for the
/// addressing mode
bool X86DAGToDAGISel::MatchAddress(SDOperand N, X86ISelAddressMode &AM) {
  switch (N.getOpcode()) {
  default: break;
  case ISD::FrameIndex:
    if (AM.BaseType == X86ISelAddressMode::RegBase && AM.Base.Reg.Val == 0) {
      AM.BaseType = X86ISelAddressMode::FrameIndexBase;
      AM.Base.FrameIndex = cast<FrameIndexSDNode>(N)->getIndex();
      return false;
    }
    break;

  case ISD::ConstantPool:
    if (AM.BaseType == X86ISelAddressMode::RegBase && AM.Base.Reg.Val == 0) {
      if (ConstantPoolSDNode *CP = dyn_cast<ConstantPoolSDNode>(N)) {
        AM.BaseType = X86ISelAddressMode::ConstantPoolBase;
        AM.Base.Reg = CurDAG->getTargetConstantPool(CP->get(), MVT::i32);
        return false;
      }
    }
    break;

  case ISD::GlobalAddress:
    if (AM.GV == 0) {
      GlobalValue *GV = cast<GlobalAddressSDNode>(N)->getGlobal();
      // For Darwin, external and weak symbols are indirect, so we want to load
      // the value at address GV, not the value of GV itself.  This means that
      // the GlobalAddress must be in the base or index register of the address,
      // not the GV offset field.
      if (Subtarget->getIndirectExternAndWeakGlobals() &&
          (GV->hasWeakLinkage() || GV->isExternal())) {
        break;
      } else {
        AM.GV = GV;
        return false;
      }
    }
    break;

  case ISD::Constant:
    AM.Disp += cast<ConstantSDNode>(N)->getValue();
    return false;

  case ISD::SHL:
    if (AM.IndexReg.Val == 0 && AM.Scale == 1)
      if (ConstantSDNode *CN = dyn_cast<ConstantSDNode>(N.Val->getOperand(1))) {
        unsigned Val = CN->getValue();
        if (Val == 1 || Val == 2 || Val == 3) {
          AM.Scale = 1 << Val;
          SDOperand ShVal = N.Val->getOperand(0);

          // Okay, we know that we have a scale by now.  However, if the scaled
          // value is an add of something and a constant, we can fold the
          // constant into the disp field here.
          if (ShVal.Val->getOpcode() == ISD::ADD && ShVal.hasOneUse() &&
              isa<ConstantSDNode>(ShVal.Val->getOperand(1))) {
            AM.IndexReg = ShVal.Val->getOperand(0);
            ConstantSDNode *AddVal =
              cast<ConstantSDNode>(ShVal.Val->getOperand(1));
            AM.Disp += AddVal->getValue() << Val;
          } else {
            AM.IndexReg = ShVal;
          }
          return false;
        }
      }
    break;

  case ISD::MUL:
    // X*[3,5,9] -> X+X*[2,4,8]
    if (AM.IndexReg.Val == 0 && AM.BaseType == X86ISelAddressMode::RegBase &&
        AM.Base.Reg.Val == 0)
      if (ConstantSDNode *CN = dyn_cast<ConstantSDNode>(N.Val->getOperand(1)))
        if (CN->getValue() == 3 || CN->getValue() == 5 || CN->getValue() == 9) {
          AM.Scale = unsigned(CN->getValue())-1;

          SDOperand MulVal = N.Val->getOperand(0);
          SDOperand Reg;

          // Okay, we know that we have a scale by now.  However, if the scaled
          // value is an add of something and a constant, we can fold the
          // constant into the disp field here.
          if (MulVal.Val->getOpcode() == ISD::ADD && MulVal.hasOneUse() &&
              isa<ConstantSDNode>(MulVal.Val->getOperand(1))) {
            Reg = MulVal.Val->getOperand(0);
            ConstantSDNode *AddVal =
              cast<ConstantSDNode>(MulVal.Val->getOperand(1));
            AM.Disp += AddVal->getValue() * CN->getValue();
          } else {
            Reg = N.Val->getOperand(0);
          }

          AM.IndexReg = AM.Base.Reg = Reg;
          return false;
        }
    break;

  case ISD::ADD: {
    X86ISelAddressMode Backup = AM;
    if (!MatchAddress(N.Val->getOperand(0), AM) &&
        !MatchAddress(N.Val->getOperand(1), AM))
      return false;
    AM = Backup;
    if (!MatchAddress(N.Val->getOperand(1), AM) &&
        !MatchAddress(N.Val->getOperand(0), AM))
      return false;
    AM = Backup;
    break;
  }
  }

  // Is the base register already occupied?
  if (AM.BaseType != X86ISelAddressMode::RegBase || AM.Base.Reg.Val) {
    // If so, check to see if the scale index register is set.
    if (AM.IndexReg.Val == 0) {
      AM.IndexReg = N;
      AM.Scale = 1;
      return false;
    }

    // Otherwise, we cannot select it.
    return true;
  }

  // Default, generate it as a register.
  AM.BaseType = X86ISelAddressMode::RegBase;
  AM.Base.Reg = N;
  return false;
}

/// SelectAddr - returns true if it is able pattern match an addressing mode.
/// It returns the operands which make up the maximal addressing mode it can
/// match by reference.
bool X86DAGToDAGISel::SelectAddr(SDOperand N, SDOperand &Base, SDOperand &Scale,
                                 SDOperand &Index, SDOperand &Disp) {
  X86ISelAddressMode AM;
  if (!MatchAddress(N, AM)) {
    if (AM.BaseType == X86ISelAddressMode::RegBase) {
      if (AM.Base.Reg.Val)
        AM.Base.Reg = Select(AM.Base.Reg);
      else
        AM.Base.Reg = CurDAG->getRegister(0, MVT::i32);
    }
    if (AM.IndexReg.Val)
      AM.IndexReg = Select(AM.IndexReg);
    else
      AM.IndexReg = CurDAG->getRegister(0, MVT::i32);

    Base  = (AM.BaseType == X86ISelAddressMode::FrameIndexBase) ?
      CurDAG->getTargetFrameIndex(AM.Base.FrameIndex, MVT::i32) : AM.Base.Reg;
    Scale = getI8Imm (AM.Scale);
    Index = AM.IndexReg;
    Disp  = AM.GV ? CurDAG->getTargetGlobalAddress(AM.GV, MVT::i32, AM.Disp)
                  : getI32Imm(AM.Disp);
    return true;
  }
  return false;
}

static bool isRegister0(SDOperand Op)
{
  if (RegisterSDNode *R = dyn_cast<RegisterSDNode>(Op))
    return (R->getReg() == 0);
  return false;
}

/// SelectLEAAddr - it calls SelectAddr and determines if the maximal addressing
/// mode it matches can be cost effectively emitted as an LEA instruction.
/// For X86, it always is unless it's just a (Reg + const).
bool X86DAGToDAGISel::SelectLEAAddr(SDOperand N, SDOperand &Base, SDOperand &Scale,
                                    SDOperand &Index, SDOperand &Disp) {
  if (SelectAddr(N, Base, Scale, Index, Disp)) {
    if (!isRegister0(Base)) {
      unsigned Complexity = 0;
      if ((unsigned)cast<ConstantSDNode>(Scale)->getValue() > 1)
        Complexity++;
      if (!isRegister0(Index))
        Complexity++;
      if (ConstantSDNode *CN = dyn_cast<ConstantSDNode>(Disp)) {
        if (!CN->isNullValue()) Complexity++;
      } else {
        Complexity++;
      }
      return (Complexity > 1);
    }
    return true;
  } else {
    return false;
  }
}

SDOperand X86DAGToDAGISel::Select(SDOperand Op) {
  SDNode *N = Op.Val;
  MVT::ValueType OpVT = N->getValueType(0);
  unsigned Opc;

  if (N->getOpcode() >= ISD::BUILTIN_OP_END)
    return Op;   // Already selected.
  
  switch (N->getOpcode()) {
    default: break;

    case ISD::SHL:
      if (ConstantSDNode *CN = dyn_cast<ConstantSDNode>(N->getOperand(1))) {
        if (CN->getValue() == 1) {
          // X = SHL Y, 1  -> X = ADD Y, Y
          switch (OpVT) {
            default: assert(0 && "Cannot shift this type!");
            case MVT::i8:  Opc = X86::ADD8rr; break;
            case MVT::i16: Opc = X86::ADD16rr; break;
            case MVT::i32: Opc = X86::ADD32rr; break;
          }
          SDOperand Tmp0 = Select(N->getOperand(0));
          return CurDAG->SelectNodeTo(N, Opc, MVT::i32, Tmp0, Tmp0);
        }
      }
      break;

    case ISD::RET: {
      SDOperand Chain = Select(N->getOperand(0));     // Token chain.
      switch (N->getNumOperands()) {
        default:
          assert(0 && "Unknown return instruction!");
        case 3:
          assert(0 && "Not yet handled return instruction!");
          break;
        case 2: {
          SDOperand Val = Select(N->getOperand(1));
          switch (N->getOperand(1).getValueType()) {
            default:
              assert(0 && "All other types should have been promoted!!");
            case MVT::i32:
              Chain = CurDAG->getCopyToReg(Chain, X86::EAX, Val);
              break;
            case MVT::f32:
            case MVT::f64:
              assert(0 && "Not yet handled return instruction!");
              break;
          }
        }
        case 1:
          break;
      }
      if (X86Lowering.getBytesToPopOnReturn() == 0)
        return CurDAG->SelectNodeTo(N, X86::RET, MVT::Other, Chain);
      else
        return CurDAG->SelectNodeTo(N, X86::RET, MVT::Other,
                              getI16Imm(X86Lowering.getBytesToPopOnReturn()),
                                    Chain);
    }

    case ISD::STORE: {
      SDOperand Chain = Select(N->getOperand(0));     // Token chain.
      SDOperand Tmp1 = Select(N->getOperand(1));
      Opc = 0;
      if (ConstantSDNode *CN = dyn_cast<ConstantSDNode>(N->getOperand(1))) {
        switch (CN->getValueType(0)) {
          default: assert(0 && "Invalid type for operation!");
          case MVT::i1:
          case MVT::i8:  Opc = X86::MOV8mi;  break;
          case MVT::i16: Opc = X86::MOV16mi; break;
          case MVT::i32: Opc = X86::MOV32mi; break;
        }
      }

      if (!Opc) {
        switch (N->getOperand(1).getValueType()) {
          default: assert(0 && "Cannot store this type!");
          case MVT::i1:
          case MVT::i8:  Opc = X86::MOV8mr;  break;
          case MVT::i16: Opc = X86::MOV16mr; break;
          case MVT::i32: Opc = X86::MOV32mr; break;
          case MVT::f32: Opc = X86::MOVSSmr; break;
          case MVT::f64: Opc = X86::FST64m;  break;
        }
      }

      SDOperand Base, Scale, Index, Disp;
      SelectAddr(N->getOperand(2), Base, Scale, Index, Disp);
      return CurDAG->SelectNodeTo(N, Opc, MVT::Other,
                                  Base, Scale, Index, Disp, Tmp1, Chain)
        .getValue(Op.ResNo);
    }
  }

  return SelectCode(Op);
}

/// createX86ISelDag - This pass converts a legalized DAG into a 
/// X86-specific DAG, ready for instruction scheduling.
///
FunctionPass *llvm::createX86ISelDag(TargetMachine &TM) {
  return new X86DAGToDAGISel(TM);
}
