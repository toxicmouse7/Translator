//
// Created by Aleksej on 23.04.2023.
//

#pragma once

#include <map>

#include <Requestor/Interfaces/IRequestHandler.hpp>
#include "Requests/RegisterDownArchitectureRequest.hpp"

class RegisterDownArchitectureHandler : public IRequestHandler<RegisterDownArchitectureRequest, ZydisRegister>
{
private:
    inline static const std::map<ZydisRegister, ZydisRegister> registersDowngradeMap =
            {
                    {ZYDIS_REGISTER_RAX, ZYDIS_REGISTER_EAX},
                    {ZYDIS_REGISTER_RBX, ZYDIS_REGISTER_EBX},
                    {ZYDIS_REGISTER_RCX, ZYDIS_REGISTER_ECX},
                    {ZYDIS_REGISTER_RDX, ZYDIS_REGISTER_EDX},
            };
public:

    ZydisRegister Handle(const RegisterDownArchitectureRequest &request) override
    {
        return registersDowngradeMap.at(request.GetRegister());
    }
};
