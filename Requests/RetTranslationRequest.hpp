//
// Created by Aleksej on 06.07.2023.
//

#ifndef TRANSLATOR_RETTRANSLATIONREQUEST_HPP
#define TRANSLATOR_RETTRANSLATIONREQUEST_HPP


#include "InstructionTranslationRequest.hpp"

class RetTranslationRequest : public InstructionTranslationRequest
{
public:
    explicit RetTranslationRequest(ZydisDisassembledInstruction disassembledInstruction)
            : InstructionTranslationRequest(disassembledInstruction)
    {}
};


#endif //TRANSLATOR_RETTRANSLATIONREQUEST_HPP
