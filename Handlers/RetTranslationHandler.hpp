//
// Created by Aleksej on 06.07.2023.
//

#ifndef TRANSLATOR_RETTRANSLATIONHANDLER_HPP
#define TRANSLATOR_RETTRANSLATIONHANDLER_HPP

#include <vector>

#include <Zydis/Zydis.h>

#include <Requestor/Interfaces/IRequestHandler.hpp>

#include "Requests/RetTranslationRequest.hpp"
#include "InstructionBuilder.hpp"

class RetTranslationHandler : public IRequestHandler<RetTranslationRequest, std::vector<ZyanU8>>
{
public:
    std::vector<ZyanU8> Handle(const RetTranslationRequest& retTranslationHandler) override
    {
        return InstructionBuilder::Builder()
                .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                .Mnemonic(ZYDIS_MNEMONIC_RET)
                .Build();
    }
};


#endif //TRANSLATOR_RETTRANSLATIONHANDLER_HPP
