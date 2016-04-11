#include "mcc/tac/triple.h"

#include <cassert>
#include <iostream>
#include <typeinfo>

#include "mcc/tac/variable.h"

namespace mcc {
  namespace tac {
    unsigned Triple::nextId = 0;

    Triple::Triple(std::shared_ptr<Operand> arg) :
    Triple(Operator(OperatorName::NOP), arg) {
      // check if it is a terminal
      assert(arg.get()->isLeaf() && "Operand is non-terminal!");
    }

    // constructor needed for Label
    Triple::Triple(OperatorName op) : Triple(Operator(op), nullptr) {
    }

    Triple::Triple(Operator op, std::shared_ptr<Operand> arg) :
    Triple(op, arg, nullptr) {
      // check if operator is UNARY or LINE (NOP)
      assert((
              op.getType() == OperatorType::LINE ||
              op.getType() == OperatorType::UNARY
             ) && "Operator not unary!");
    }

    Triple::Triple(Operator op, std::shared_ptr<Operand> arg1,
            std::shared_ptr<Operand> arg2) :
    Operand(),
    arg1(arg1),
    arg2(arg2),
    op(op),
    basicBlockId(0),
    targetVar(nullptr),
    id(0)
     {
      if (arg1 != nullptr) {
        this->setType(arg1->getType());
      }

      this->updateResultType(op);

      if (arg2 != nullptr) {
        assert((
                op.getName() == OperatorName::JUMPFALSE ||
                arg1->getType() == arg2->getType()
               ) && 
               "Type mismatch");
      }

      switch(op.getName()) {
          case OperatorName::ASSIGN:
          case OperatorName::JUMPFALSE:
          case OperatorName::JUMP:
          case OperatorName::NOP:
              break;
          default:
            id = ++nextId;
            this->name = "$t" + std::to_string(id);
      }
    }

    bool Triple::isLeaf() const {
      return false;
    }

    unsigned Triple::getId() const {
      return id;
    }

    std::string Triple::getName() const {
      return name;
    }

    void Triple::setName(const std::string name) {
      this->name = name;
    }

    std::string Triple::getValue() const {
      return getName();
    }
    
     unsigned Triple::getBasicBlockId() const {
         return basicBlockId;
     }
     
     void Triple::setBasicBlockId(unsigned blockId) {
         basicBlockId = blockId;
     }

    std::string Triple::toString() const {
      std::string output;

      switch (op.getName()) {
        case OperatorName::JUMPFALSE:
        case OperatorName::JUMP:
          output.append(op.toString());
          output.append(arg1.get()->getValue());
          if(arg2 != nullptr) {   
            output.append(" ");
            output.append(arg2.get()->getValue());
          }

          return output;

        case OperatorName::LABEL:
          output.append(op.toString());
          output.append(getName());

          return output;
          
        case OperatorName::NOP:
            output.append(arg1.get()->getValue());

            return output;
        
        default:
            if (op.getName() != OperatorName::ASSIGN) {   
                output.append(name);
                output.append(" = ");
            }
            
            switch(op.getType()) {
                case OperatorType::UNARY:
                    
                    output.append(op.toString());
                    output.append(arg1.get()->getValue());
                    
                    break;
                default:
                    output.append(arg1.get()->getValue());
                    output.append(op.toString());
                    if(arg2 != nullptr) {
                      output.append(arg2.get()->getValue());
                    }
            }
            return output;
      }
    }
    
    void Triple::setTargetVariable(std::shared_ptr<Variable> var) {
        targetVar = var;
    }
    
    std::shared_ptr<Variable> Triple::getTargetVariable() {
        return targetVar;
    }
  }
}
