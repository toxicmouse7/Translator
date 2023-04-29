//
// Created by Aleksej on 23.04.2023.
//

#ifndef TRANSLATOR_INSTRUCTIONTRANSLATIONHANDLER_HPP
#define TRANSLATOR_INSTRUCTIONTRANSLATIONHANDLER_HPP

#include <vector>

#include <Zydis/Zydis.h>

#include <Requestor/Requestor.hpp>
#include <Requestor/Interfaces/IRequestHandler.hpp>

#include "Requests/InstructionTranslationRequest.hpp"
#include "Requests/MovTranslationRequest.hpp"

class InstructionTranslationHandler : public IRequestHandler<InstructionTranslationRequest, std::vector<ZyanU8>>
{
public:
    std::vector<ZyanU8> Handle(const InstructionTranslationRequest& request) override
    {
        auto& requestor = Requestor::Instance();

        switch (request.GetInstruction().info.mnemonic)
        {
            case ZYDIS_MNEMONIC_MOV:
            {
                auto translationRequest = MovTranslationRequest(request.GetInstruction());
                return requestor.Handle<std::vector<ZyanU8>>(translationRequest);
            }
            default:
                return {};
        }
    }
};


#endif //TRANSLATOR_INSTRUCTIONTRANSLATIONHANDLER_HPP
