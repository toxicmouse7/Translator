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
                    {ZYDIS_REGISTER_RAX,  ZYDIS_REGISTER_EAX},
                    {ZYDIS_REGISTER_RBX,  ZYDIS_REGISTER_EBX},
                    {ZYDIS_REGISTER_RCX,  ZYDIS_REGISTER_ECX},
                    {ZYDIS_REGISTER_RDX,  ZYDIS_REGISTER_EDX},
                    {ZYDIS_REGISTER_RIP,  ZYDIS_REGISTER_RIP},
                    {ZYDIS_REGISTER_RSP,  ZYDIS_REGISTER_ESP},
                    {ZYDIS_REGISTER_NONE, ZYDIS_REGISTER_NONE},
                    {ZYDIS_REGISTER_R8, ZYDIS_REGISTER_R8D},
                    {ZYDIS_REGISTER_R8D, ZYDIS_REGISTER_R8D},
                    {ZYDIS_REGISTER_R9, ZYDIS_REGISTER_R9D},
                    {ZYDIS_REGISTER_R9D, ZYDIS_REGISTER_R9D},
                    {ZYDIS_REGISTER_EAX, ZYDIS_REGISTER_EAX},
                    {ZYDIS_REGISTER_EBX, ZYDIS_REGISTER_EBX},
                    {ZYDIS_REGISTER_ECX, ZYDIS_REGISTER_ECX},
                    {ZYDIS_REGISTER_EDX, ZYDIS_REGISTER_EDX},
                    {ZYDIS_REGISTER_ESP, ZYDIS_REGISTER_ESP}
            };
public:

    ZydisRegister Handle(const RegisterDownArchitectureRequest& request) override
    {
        return registersDowngradeMap.at(request.GetRegister());
    }
};
