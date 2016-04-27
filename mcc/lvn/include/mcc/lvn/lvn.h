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
  static void transform(Tac &tac);

 private:
  static std::map<unsigned, std::shared_ptr<Triple>> tempVarAssignments;   
     
  template <typename T>
  static T evaluateExpression(T arg1, T arg2, mcc::tac::OperatorName op);
  static void updateTriple(Operator op, std::shared_ptr<Operand> arg1,
                           std::shared_ptr<Operand> arg2, Triple &triple);
  static std::shared_ptr<IntLiteral> evaluateInt(Triple &);
  static std::shared_ptr<FloatLiteral> evaluateFloat(Triple &);
  static std::shared_ptr<Variable> addTempVarAssignment(unsigned position, 
                                           std::shared_ptr<Variable> var);
  static void updateTAC(Tac &tac);
};
}
}

#endif /* LVN_H */
