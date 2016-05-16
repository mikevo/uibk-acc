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
  // Data instructions
  MOV,
  PUSH,
  POP,
  LEA,

  // Arithmetic
  ADD,
  SUB,
  IMUL,
  IDIV,
  FADD,
  FSUB,
  FMUL,
  FDIV,

  // Logic
  AND,
  OR,
  XOR,
  NOT,

  // Control flow
  JMP,
  JNE,
  CMP,
  CALL,
  RET,

};

static std::map<Instruction, std::string> InstructionName{
    // Data instructions
    {Instruction::MOV, "mov"},
    {Instruction::PUSH, "push"},
    {Instruction::POP, "pop"},
    {Instruction::LEA, "lea"},

    // Arithmetic
    {Instruction::ADD, "add"},
    {Instruction::SUB, "sub"},
    {Instruction::IMUL, "imul"},
    {Instruction::IDIV, "idiv"},
    {Instruction::FADD, "fadd"},
    {Instruction::FSUB, "fsub"},
    {Instruction::FMUL, "fmul"},
    {Instruction::FDIV, "fdiv"},

    // Logic
    {Instruction::AND, "and"},
    {Instruction::OR, "or"},
    {Instruction::XOR, "xor"},
    {Instruction::NOT, "not"},

    // Control flow
    {Instruction::JMP, "jmp"},
    {Instruction::JNE, "jne"},
    {Instruction::CMP, "cmp"},
    {Instruction::CALL, "call"},
    {Instruction::RET, "ret"},
};

enum class Register {
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
    {Register::ESP, "esp"}, {Register::EBP, "ebp"}};

enum class OperandType {
  REGISTER,
  ADDRESS,
  CONSTANT,
};
}
}

#endif /* X86_INSTRUCTION_SET_H */
