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
  MOVSS,
  FLD,
  FSTP,

  // Arithmetic
  // int
  ADD,
  SUB,
  IMUL,
  IDIV,
  CDQ,
  FADD,
  FSUB,
  FMUL,
  FDIV,

  // float
  ADDSS,
  SUBSS,
  MULSS,
  DIVSS,

  // Logic
  // int
  AND,
  OR,
  XOR,
  NOT,

  // float
  ANDPS,
  ORPS,
  XORPS,

  // Control flow
  JMP,
  JNE,
  JE,
  JG,
  JGE,
  JL,
  JLE,
  CMP,
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
    {Instruction::MOVSS, "movss"},
    {Instruction::FLD, "fld"},
    {Instruction::FSTP, "fstp"},

    // Arithmetic
    // int
    {Instruction::ADD, "add"},
    {Instruction::SUB, "sub"},
    {Instruction::IMUL, "imul"},
    {Instruction::IDIV, "idiv"},
    {Instruction::CDQ, "cdq"},
    {Instruction::FADD, "fadd"},
    {Instruction::FSUB, "fsub"},
    {Instruction::FMUL, "fmul"},
    {Instruction::FDIV, "fdiv"},

    // float
    {Instruction::ADDSS, "addss"},
    {Instruction::SUBSS, "subss"},
    {Instruction::MULSS, "mulss"},
    {Instruction::DIVSS, "divss"},

    // Logic
    // int
    {Instruction::AND, "and"},
    {Instruction::OR, "or"},
    {Instruction::XOR, "xor"},
    {Instruction::NOT, "not"},

    // float
    {Instruction::ANDPS, "andps"},
    {Instruction::ORPS, "orps"},
    {Instruction::XORPS, "xorps"},

    // Control flow
    {Instruction::JMP, "jmp"},
    {Instruction::JNE, "jne"},
    {Instruction::JE, "je"},
    {Instruction::JG, "jg"},
    {Instruction::JGE, "jge"},
    {Instruction::JL, "jl"},
    {Instruction::JLE, "jle"},
    {Instruction::CMP, "cmp"},
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

  // float registers
  XMM0,
  XMM1,
  XMM2,
  XMM3,
  XMM4,
  XMM5,
  XMM6,
  XMM7,
};

static std::map<Register, std::string> RegisterName{
    {Register::EAX, "eax"},   {Register::EBX, "ebx"},
    {Register::ECX, "ecx"},   {Register::EDX, "edx"},
    {Register::ESI, "esi"},   {Register::EDI, "edi"},
    {Register::ESP, "esp"},   {Register::EBP, "ebp"},
    {Register::XMM0, "xmm0"}, {Register::XMM1, "xmm1"},
    {Register::XMM2, "xmm2"}, {Register::XMM3, "xmm3"},
    {Register::XMM4, "xmm4"}, {Register::XMM5, "xmm5"},
    {Register::XMM6, "xmm6"}, {Register::XMM7, "xmm7"},
};

enum class OperandType {
  REGISTER,
  ADDRESS,
  CONSTANT,
  LABEL,
};
}
}

#endif /* X86_INSTRUCTION_SET_H */
