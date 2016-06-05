/*
 * File:   int_literal.h
 * Author: Michael Voetter <Michael.Voetter@student.uibk.ac.at>
 *
 * Created on March 19, 2016, 2:50 PM
 */

#ifndef MCC_TAC_FLOAT_LITERAL_H
#define MCC_TAC_FLOAT_LITERAL_H

#include "mcc/tac/operand.h"

#include <memory>

namespace mcc {
namespace tac {
class FloatLiteral : public Operand {
 public:
  typedef std::shared_ptr<FloatLiteral> ptr_t;

  FloatLiteral(float value, Scope::ptr_t scope);
  virtual std::string getValue() const override final;
  virtual bool isLeaf() const override final;
  float value;
};
}
}

#endif /* MCC_TAC_FLOAT_LITERAL_H */
