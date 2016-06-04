/*
 * File:   tac_converter.h
 *
 * Created on June 1, 2016, 10:19 PM
 */

#ifndef TAC_CONVERTER_H
#define TAC_CONVERTER_H

#include "mcc/tac/array_access.h"

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

std::vector<
    std::tuple<Label::ptr_t, Array::ptr_t, mcc::gas::Operand::ptr_t>>::iterator
lookupDefinedArray(Label::ptr_t functionLabel, Array::ptr_t array);
void defineArray(Gas *gas, Label::ptr_t functionLabel, ArrayAccess::ptr_t arr);
void computeAndStoreArrayStartAddress(Gas *gas, Label::ptr_t functionLabel,
                                      Array::ptr_t arr);
void cleanUpArray(Gas *gas, Label::ptr_t functionLabel, Array::ptr_t arr,
                  Operand::ptr_t length);
}
}
}

#endif /* TAC_CONVERTER_H */
