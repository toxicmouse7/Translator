//
// Created by Aleksej on 22.04.2023.
//

#pragma once

#include <vector>

#include <Zydis/Zydis.h>

#include <Requestor/Requestor.hpp>
#include <Requestor/Interfaces/IRequestHandler.hpp>

#include "Requests/MovTranslationRequest.hpp"
#include "Requests/RegisterDownArchitectureRequest.hpp"

#include "AdditionalContext.hpp"
#include "InstructionBuilder.hpp"

class MovTranslationHandler : public IRequestHandler<MovTranslationRequest, std::vector<ZyanU8>>
{
public:
    std::vector<ZyanU8> Handle(const MovTranslationRequest& request) override
    {
        auto& requestor = Requestor::Instance();
        std::vector<ZyanU8> result;

        auto& instruction = request.GetInstruction();
        auto builder = InstructionBuilder::Builder()
                .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                .Mnemonic(ZYDIS_MNEMONIC_MOV);

        auto& firstOperand = instruction.operands[0];
        auto& secondOperand = instruction.operands[1];

        if (firstOperand.type == ZYDIS_OPERAND_TYPE_REGISTER)
        {
            auto downRegisterRequest = RegisterDownArchitectureRequest(firstOperand.reg.value);
            auto downRegister = requestor.Handle<ZydisRegister>(downRegisterRequest);
            builder.Operand(ZYDIS_OPERAND_TYPE_REGISTER).Reg(downRegister);

            if (secondOperand.type == ZYDIS_OPERAND_TYPE_IMMEDIATE)
            {
                builder.Operand(ZYDIS_OPERAND_TYPE_IMMEDIATE)
                        .Imm((int32_t)secondOperand.imm.value.s);

                if (secondOperand.imm.value.u > UINT32_MAX)
                {
                    auto& additionalContext = AdditionalContext::GetInstance();

                    auto additionalInstruction = InstructionBuilder::Builder()
                            .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                            .Mnemonic(ZYDIS_MNEMONIC_MOV)
                            .Operand(ZYDIS_OPERAND_TYPE_MEMORY)
                            .Mem((int32_t)(ZyanI64)&additionalContext.GetRegister(downRegister), 4)
                            .FinishOperand()
                            .Operand(ZYDIS_OPERAND_TYPE_IMMEDIATE)
                            .Imm((int32_t)((secondOperand.imm.value.s & 0xFFFFFFFF00000000) >> 32))
                            .FinishOperand()
                            .Build();

                    std::copy(additionalInstruction.begin(), additionalInstruction.end(),
                              std::back_inserter(result));
                }
            }
            else if (secondOperand.type == ZYDIS_OPERAND_TYPE_REGISTER)
            {
                auto downSecondRegisterRequest = RegisterDownArchitectureRequest(secondOperand.reg.value);
                auto downSecondRegister = requestor.Handle<ZydisRegister>(downSecondRegisterRequest);
                builder.Operand(ZYDIS_OPERAND_TYPE_REGISTER)
                        .Reg(downSecondRegister);
            }
        }

        auto mainInstruction = builder.Build();
        std::copy(mainInstruction.begin(), mainInstruction.end(),
                  std::back_inserter(result));

        return result;
    }
};
