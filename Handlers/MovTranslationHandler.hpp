//
// Created by Aleksej on 22.04.2023.
//

#pragma once

#include <vector>

#include <Zydis/Zydis.h>

#include "Requestor/Interfaces/IRequestHandler.hpp"
#include "Requests/MovTranslationRequest.hpp"

class MovTranslationHandler : public IRequestHandler<MovTranslationRequest, std::vector<ZyanU8>>
{
public:
    std::vector<ZyanU8> Handle(const MovTranslationRequest& request) override
    {
        auto& instruction = request.GetInstruction();
        ZydisEncoderRequest encoderRequest;
        memset(&encoderRequest, 0, sizeof(encoderRequest));
        encoderRequest.machine_mode = ZYDIS_MACHINE_MODE_LEGACY_32;
        encoderRequest.mnemonic = ZYDIS_MNEMONIC_MOV;

        if (instruction.operands[0].type == ZYDIS_OPERAND_TYPE_REGISTER)
        {

        }

        return {};
    }
};
