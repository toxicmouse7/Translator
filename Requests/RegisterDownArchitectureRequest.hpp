//
// Created by Aleksej on 23.04.2023.
//

#pragma once

#include <Zydis/Zydis.h>

#include "Requestor/Interfaces/IRequest.hpp"

class RegisterDownArchitectureRequest : public IRequest
{
private:
    ZydisRegister reg{};
public:
    explicit RegisterDownArchitectureRequest(ZydisRegister reg)
    {
        this->reg = reg;
    }

    [[nodiscard]] const auto& GetRegister() const
    {
        return reg;
    }
};
