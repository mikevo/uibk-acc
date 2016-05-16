#include "mcc/gas/gas.h"

#include <algorithm>
#include <cassert>
#include <memory>
#include <ostream>
#include <typeinfo>

#include "mcc/gas/x86_instruction_set.h"
#include "mcc/tac/operator.h"
#include "mcc/tac/triple.h"

namespace mcc {

namespace gas {

Gas::Gas(Tac tac) : functionMap(tac.getFunctionMap()) {
  this->functionStackSpaceMap =
      std::make_shared<function_stack_space_map_type>();
  this->variableStackOffsetMap =
      std::make_shared<variable_stack_offset_map_type>();

  this->convertTac(tac);
        }

        void Gas::analyzeTac(Tac &tac) {
          Label::ptr_t currentFunctionLabel = nullptr;
          unsigned stackSpace = 0;
          unsigned currentStackOffset = 0;
          for (auto codeLine : tac.codeLines) {
            auto opName = codeLine->getOperator().getName();
            if (opName == OperatorName::LABEL) {
              auto label = std::static_pointer_cast<Label>(codeLine);
              if (label->isFunctionEntry()) {
                // if new function is entered
                if (currentFunctionLabel) {
                  this->setFunctionStackSpace(currentFunctionLabel, stackSpace);
                  stackSpace = 0;
                  currentStackOffset = 0;
                }

                currentFunctionLabel = label;
              }
            } else if (codeLine->containsTargetVar()) {
              auto targetVar = codeLine->getTargetVariable();
              (*variableStackOffsetMap)[targetVar] = currentStackOffset;
              currentStackOffset += targetVar->getSize();

              // if variable not parameter of function
              if (codeLine->getOperator().getName() != OperatorName::POP) {
                stackSpace += targetVar->getSize();
              }
            }
          }

          // add last function
          this->setFunctionStackSpace(currentFunctionLabel, stackSpace);
        }

        void Gas::setFunctionStackSpace(std::string functionName,
                                        unsigned stackSpace) {
          assert(functionMap->find(functionName) != functionMap->end() &&
                 "Function not declared!");
          (*functionStackSpaceMap)[functionName] = stackSpace;
        }

        void Gas::setFunctionStackSpace(Label::ptr_t functionLabel,
                                        unsigned stackSpace) {
          assert(functionLabel->isFunctionEntry() && "Not a function label!");
          setFunctionStackSpace(functionLabel->getName(), stackSpace);
        }

        std::shared_ptr<function_stack_space_map_type>
        Gas::getFunctionStackSpaceMap() {
          return this->functionStackSpaceMap;
        }

        std::shared_ptr<variable_stack_offset_map_type>
        Gas::getVariableStackOffsetMap() {
          return this->variableStackOffsetMap;
        }

        void Gas::convertTac(Tac &tac) {
          this->analyzeTac(tac);

          for (auto triple : tac.codeLines) {
            auto op = triple->getOperator();

            switch (op.getName()) {
              case OperatorName::NOP:
                break;

              case OperatorName::ADD:
                break;

              case OperatorName::SUB:
                break;

              case OperatorName::MUL:
                break;

              case OperatorName::DIV:
                break;

              case OperatorName::ASSIGN:
                break;

              case OperatorName::LABEL:
                break;

              case OperatorName::JUMP:
                break;

              case OperatorName::JUMPFALSE:
                break;

              case OperatorName::EQ:
              case OperatorName::NE:
              case OperatorName::LE:
              case OperatorName::GE:
              case OperatorName::LT:
              case OperatorName::GT:
                break;

              case OperatorName::MINUS:
                break;

              case OperatorName::NOT:
                break;

              case OperatorName::PUSH:
                break;

              case OperatorName::POP:
                break;

              case OperatorName::CALL:
                break;

              case OperatorName::RET:
                break;
            }
          }
        }
        }
}
