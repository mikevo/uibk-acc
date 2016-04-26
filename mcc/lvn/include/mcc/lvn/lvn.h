#ifndef MCC_LVN_H
#define MCC_LVN_H

#include "mcc/tac/float_literal.h"
#include "mcc/tac/int_literal.h"
#include "mcc/tac/operand.h"
#include "mcc/tac/operator.h"
#include "mcc/tac/tac.h"
#include <string>

using namespace mcc::tac;

namespace mcc {
namespace lvn {

class LVN {
 public:
  static void transform(mcc::tac::Tac &tac);

 private:
  template <typename T>
  static T evaluateExpression(T arg1, T arg2, mcc::tac::OperatorName op);
  static void updateTriple(Operator op, std::shared_ptr<Operand> arg1,
                           std::shared_ptr<Operand> arg2, Triple &triple);
  static std::shared_ptr<IntLiteral> evaluateInt(Triple &);
  static std::shared_ptr<FloatLiteral> evaluateFloat(Triple &);
};
}
}

#endif /* LVN_H */
