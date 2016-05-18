/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   x86_instruction_set.h
 * Author: Philipp Stiegernigg
 *
 * Created on May 15, 2016, 8:45 PM
 */

#ifndef X86_INSTRUCTION_SET_H
#define X86_INSTRUCTION_SET_H

#include <map>
#include <string>

namespace mcc {
namespace gas {

enum class Instruction {
  NOP,

  // Data instructions
  // int
  MOV,
  PUSH,
  POP,
  LEA,

  // float
  FSTP,
  FXCH,
  FLD,

  // Arithmetic
  // int
  ADD,
  SUB,
  IMUL,
  IDIV,
  CDQ,
  NEG,

  // float
  FADDP,
  FSUBP,
  FMULP,
  FDIVP,
  FCHS,

  // Logic
  AND,
  OR,
  XOR,
  NOT,

  // Control flow
  JMP,
  JNE,
  JE,
  JG,
  JGE,
  JL,
  JLE,
  CMP,
  FCOMIP,
  CALL,
  RET,

};

static std::map<Instruction, std::string> InstructionName{
    {Instruction::NOP, "nop"},

    // Data instructions
    // int
    {Instruction::MOV, "mov"},
    {Instruction::PUSH, "push"},
    {Instruction::POP, "pop"},
    {Instruction::LEA, "lea"},

    // float
    {Instruction::FSTP, "fstp"},
    {Instruction::FXCH, "fxch"},
    {Instruction::FLD, "fld"},

    // Arithmetic
    // int
    {Instruction::ADD, "add"},
    {Instruction::SUB, "sub"},
    {Instruction::IMUL, "imul"},
    {Instruction::IDIV, "idiv"},
    {Instruction::CDQ, "cdq"},
    {Instruction::NEG, "neg"},

    // float
    {Instruction::FADDP, "faddp"},
    {Instruction::FSUBP, "fsubp"},
    {Instruction::FMULP, "fmulp"},
    {Instruction::FDIVP, "fdivp"},
    {Instruction::FCHS, "fchs"},

    // Logic
    {Instruction::AND, "and"},
    {Instruction::OR, "or"},
    {Instruction::XOR, "xor"},
    {Instruction::NOT, "not"},

    // Control flow
    {Instruction::JMP, "jmp"},
    {Instruction::JNE, "jne"},
    {Instruction::JE, "je"},
    {Instruction::JG, "jg"},
    {Instruction::JGE, "jge"},
    {Instruction::JL, "jl"},
    {Instruction::JLE, "jle"},
    {Instruction::CMP, "cmp"},
    {Instruction::FCOMIP, "fcomip"},
    {Instruction::CALL, "call"},
    {Instruction::RET, "ret"},
};

enum class Register {
  // int registers
  EAX,
  EBX,
  ECX,
  EDX,
  ESI,
  EDI,
  ESP,
  EBP,
};

static std::map<Register, std::string> RegisterName{
    {Register::EAX, "eax"}, {Register::EBX, "ebx"}, {Register::ECX, "ecx"},
    {Register::EDX, "edx"}, {Register::ESI, "esi"}, {Register::EDI, "edi"},
    {Register::ESP, "esp"}, {Register::EBP, "ebp"},
};

enum class OperandType {
  REGISTER,
  ADDRESS,
  CONSTANT,
  FLOAT_CONSTANT,
  LABEL,
};
}
}

#endif /* X86_INSTRUCTION_SET_H */
