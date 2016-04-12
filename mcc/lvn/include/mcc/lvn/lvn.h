#ifndef MCC_LVN_H
#define MCC_LVN_H

#include <string>
#include "mcc/tac/tac.h"
#include "mcc/tac/operator.h"
#include "mcc/tac/operand.h"

using namespace mcc::tac;

namespace mcc {
  namespace lvn {
      
      class LVN {
      public:
          static void transform(mcc::tac::Tac& tac);
          
      private:
          template<typename T> static T evaluateExpression(T arg1, T arg2, mcc::tac::OperatorName op);
          static void updateTriple(Operator op, std::shared_ptr<Operand> arg1,
                    std::shared_ptr<Operand> arg2, Triple& triple);
      };

  }
}

#endif /* LVN_H */

