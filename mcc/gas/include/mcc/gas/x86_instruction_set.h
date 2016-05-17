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
  FSTP,
  DD,

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
  NEG,

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
    {Instruction::FSTP, "fstp"},
    {Instruction::DD, "dd"},

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
    {Instruction::NEG, "neg"},

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
  XMM8,
  XMM9,
  XMM10,
  XMM11,
  XMM12,
  XMM13,
  XMM14,
  XMM15,
  RSP,
  RBP,
};

static std::map<Register, std::string> RegisterName{
    {Register::EAX, "eax"},     {Register::EBX, "ebx"},
    {Register::ECX, "ecx"},     {Register::EDX, "edx"},
    {Register::ESI, "esi"},     {Register::EDI, "edi"},
    {Register::ESP, "esp"},     {Register::EBP, "ebp"},
    {Register::XMM0, "xmm0"},   {Register::XMM1, "xmm1"},
    {Register::XMM2, "xmm2"},   {Register::XMM3, "xmm3"},
    {Register::XMM4, "xmm4"},   {Register::XMM5, "xmm5"},
    {Register::XMM6, "xmm6"},   {Register::XMM7, "xmm7"},
    {Register::XMM8, "xmm8"},   {Register::XMM9, "xmm9"},
    {Register::XMM10, "xmm10"}, {Register::XMM11, "xmm11"},
    {Register::XMM12, "xmm12"}, {Register::XMM13, "xmm13"},
    {Register::XMM14, "xmm14"}, {Register::XMM15, "xmm15"},
    {Register::RSP, "rsp"},     {Register::RBP, "rbp"},
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
