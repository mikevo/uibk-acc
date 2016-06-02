/*
 * File:   tac_float_converter.h
 *
 * Created on June 2, 2016, 3:35 PM
 */

#ifndef TAC_FLOAT_CONVERTER_H
#define TAC_FLOAT_CONVERTER_H

namespace mcc {
namespace gas {
namespace helper {
void convertFloatArithmetic(Gas *gas, Triple::ptr_t triple);
void convertFloatAssign(Gas *gas, Triple::ptr_t triple);
void convertFloatJumpFalse(Gas *gas, Triple::ptr_t triple);
void convertFloatLogicOperator(Gas *gas, Triple::ptr_t triple);
void convertFloatMinus(Gas *gas, Triple::ptr_t triple);
}
}
}

#endif /* TAC_FLOAT_CONVERTER_H */
