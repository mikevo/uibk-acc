/*
 * File:   tac_int_converter.h
 *
 * Created on June 2, 2016, 3:35 PM
 */

#ifndef TAC_INT_CONVERTER_H
#define TAC_INT_CONVERTER_H

#include "mcc/tac/array_access.h"

namespace mcc {
namespace gas {
namespace helper {
void convertIntArithmetic(Gas *gas, Triple::ptr_t triple);
void convertIntAssign(Gas *gas, Triple::ptr_t triple);
void convertIntJumpFalse(Gas *gas, Triple::ptr_t triple);
void convertIntLogicOperator(Gas *gas, Triple::ptr_t triple);
}
}
}

#endif /* TAC_INT_CONVERTER_H */
