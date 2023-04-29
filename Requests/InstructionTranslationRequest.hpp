//
// Created by Aleksej on 23.04.2023.
//

#pragma once

#include <Zydis/Zydis.h>

#include <Interfaces/IRequest.hpp>

class InstructionTranslationRequest : public IRequest
{
private:
    ZydisDisassembledInstruction disassembledInstruction{};
public:
    explicit InstructionTranslationRequest(ZydisDisassembledInstruction disassembledInstruction)
    {
        this->disassembledInstruction = disassembledInstruction;
    }

    [[nodiscard]] const auto& GetInstruction() const
    {
        return disassembledInstruction;
    }
};