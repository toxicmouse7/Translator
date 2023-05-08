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

class MovTranslationHandler : public IRequestHandler<MovTranslationRequest, std::vector<ZyanU8>>
{
public:
    std::vector<ZyanU8> Handle(const MovTranslationRequest& request) override
    {
        auto& requestor = Requestor::Instance();
        std::vector<ZyanU8> result(ZYDIS_MAX_INSTRUCTION_LENGTH);

        auto& instruction = request.GetInstruction();
        ZydisEncoderRequest encoderRequest;
        memset(&encoderRequest, 0, sizeof(encoderRequest));
        encoderRequest.machine_mode = ZYDIS_MACHINE_MODE_LEGACY_32;
        encoderRequest.mnemonic = ZYDIS_MNEMONIC_MOV;
        encoderRequest.operand_count = 2;

        auto& firstOperand = instruction.operands[0];
        auto& secondOperand = instruction.operands[1];

        if (firstOperand.type == ZYDIS_OPERAND_TYPE_REGISTER)
        {
            auto downRegisterRequest = RegisterDownArchitectureRequest(instruction.operands[0].reg.value);
            auto downRegister = requestor.Handle<ZydisRegister>(downRegisterRequest);
            encoderRequest.operands[0].type = ZYDIS_OPERAND_TYPE_REGISTER;
            encoderRequest.operands[0].reg.value = downRegister;

            if (secondOperand.type == ZYDIS_OPERAND_TYPE_IMMEDIATE)
            {
                encoderRequest.operands[1].type = ZYDIS_OPERAND_TYPE_IMMEDIATE;
                if (secondOperand.imm.value.u <= UINT32_MAX)
                {
                    encoderRequest.operands[1].imm.s = (int32_t)secondOperand.imm.value.s;
                }
            }
        }

        ZyanUSize instructionSize = result.size();
        auto status = ZydisEncoderEncodeInstruction(&encoderRequest, result.data(), &instructionSize);
        if (ZYAN_FAILED(status))
            exit(-1);

        result.resize(instructionSize);

        return result;
    }
};
