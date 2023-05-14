//
// Created by Aleksej on 14.05.2023.
//

#ifndef TRANSLATOR_DETERMINEOPERANDSRELATIONHANDLER_HPP
#define TRANSLATOR_DETERMINEOPERANDSRELATIONHANDLER_HPP

#include <stdexcept>

#include <Requestor/Interfaces/IRequestHandler.hpp>

#include "Requests/DetermineOperandsRelationRequest.hpp"

#include "OperandsRelation.hpp"

class DetermineOperandsRelationHandler : public IRequestHandler<DetermineOperandsRelationRequest, OperandsRelation>
{
public:
    OperandsRelation Handle(const DetermineOperandsRelationRequest& request) override
    {
        auto& instruction = request.GetInstruction();
        auto& firstOperand = instruction.operands[0];
        auto& secondOperand = instruction.operands[1];

        if (instruction.info.operand_count != 2)
            throw std::runtime_error("Not implemented yet (operand count != 2)");

        switch (firstOperand.type)
        {
            case ZYDIS_OPERAND_TYPE_REGISTER:
            {
                if (secondOperand.type == ZYDIS_OPERAND_TYPE_REGISTER)
                    return OperandsRelation::Reg2Reg;
                if (secondOperand.type == ZYDIS_OPERAND_TYPE_IMMEDIATE)
                    return OperandsRelation::Imm2Reg;
                if (secondOperand.type == ZYDIS_OPERAND_TYPE_MEMORY)
                    return OperandsRelation::Mem2Reg;
                break;
            }
            case ZYDIS_OPERAND_TYPE_MEMORY:
            {
                if (secondOperand.type == ZYDIS_OPERAND_TYPE_REGISTER)
                    return OperandsRelation::Reg2Mem;
                if (secondOperand.type == ZYDIS_OPERAND_TYPE_IMMEDIATE)
                    return OperandsRelation::Imm2Mem;
                break;
            }
            default:
                throw std::runtime_error("Invalid first argument type");
        }

        throw std::runtime_error("Invalid first argument type");
    }
};


#endif //TRANSLATOR_DETERMINEOPERANDSRELATIONHANDLER_HPP
