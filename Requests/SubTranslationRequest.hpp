//
// Created by Aleksej on 25.06.2023.
//

#ifndef TRANSLATOR_SUBTRANSLATIONREQUEST_HPP
#define TRANSLATOR_SUBTRANSLATIONREQUEST_HPP

#include "InstructionTranslationRequest.hpp"

class SubTranslationRequest : public InstructionTranslationRequest
{
public:
    explicit SubTranslationRequest(ZydisDisassembledInstruction disassembledInstruction)
            : InstructionTranslationRequest(disassembledInstruction)
    {}
};


#endif //TRANSLATOR_SUBTRANSLATIONREQUEST_HPP
