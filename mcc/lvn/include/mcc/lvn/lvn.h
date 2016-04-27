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
  static std::map<Variable::ptr_t, Variable::ptr_t> tempVarAssign;
  static std::map<Variable::ptr_t, unsigned> tempVarTacPos;
  static Variable::set_t tempVarUsed;
     
  template <typename T>
  static T evaluateExpression(T arg1, T arg2, tac::OperatorName op);
  static void updateTriple(Operator op, Operand::ptr_t arg1,
                           Operand::ptr_t arg2, Triple &triple);
  static IntLiteral::ptr_t evaluateInt(Triple &);
  static FloatLiteral::ptr_t evaluateFloat(Triple &);
  static Variable::ptr_t addTempVar(unsigned position, 
                                           Variable::ptr_t var);
  static bool addTempVarAssignment(Variable::ptr_t, Tac &tac, unsigned offset);
  static void updateTac(Tac &tac);

};
}
}

#endif /* LVN_H */
