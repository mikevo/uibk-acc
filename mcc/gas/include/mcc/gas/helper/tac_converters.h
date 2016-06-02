/*
 * File:   tac_converter.h
 *
 * Created on June 1, 2016, 10:19 PM
 */

#ifndef TAC_CONVERTER_H
#define TAC_CONVERTER_H

using namespace mcc::tac;

namespace mcc {
namespace gas {
namespace helper {
void convertTac(Gas *gas, Tac &tac);

void convertArithmetic(Gas *gas, Triple::ptr_t triple);
void convertAssign(Gas *gas, Triple::ptr_t triple);
void convertLabel(Gas *gas, Triple::ptr_t triple);
void convertJump(Gas *gas, Triple::ptr_t triple);
void convertJumpFalse(Gas *gas, Triple::ptr_t triple);
void convertLogicOperator(Gas *gas, Triple::ptr_t triple);
void convertMinus(Gas *gas, Triple::ptr_t triple);
void convertNot(Gas *gas, Triple::ptr_t triple);
void convertPush(Gas *gas, Triple::ptr_t triple);
void convertUnary(Gas *gas, Triple::ptr_t triple, Instruction i);
void convertCall(Gas *gas, Triple::ptr_t triple);
void convertReturn(Gas *gas, Triple::ptr_t triple);

void prepareCall(Gas *gas, Label::ptr_t label);
void cleanUpCall(Gas *gas, Label::ptr_t label);
void createFunctionProlog(Gas *gas, Label::ptr_t label);
void createFunctionEpilog(Gas *gas, Label::ptr_t label);
}
}
}

#endif /* TAC_CONVERTER_H */