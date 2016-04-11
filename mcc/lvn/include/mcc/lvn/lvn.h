#ifndef MCC_LVN_H
#define MCC_LVN_H

#include <string>
#include "mcc/tac/tac.h"

namespace mcc {
  namespace lvn {
      
      class LVN {
      public:
          static void transform(mcc::tac::Tac& tac);
          
      private:
          template<typename T> static T evaluateExpression(T arg1, T arg2, mcc::tac::OperatorName op);  
      };

  }
}

#endif /* LVN_H */

