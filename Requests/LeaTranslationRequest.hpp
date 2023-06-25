//
// Created by Aleksej on 25.06.2023.
//

#ifndef TRANSLATOR_LEATRANSLATIONREQUEST_HPP
#define TRANSLATOR_LEATRANSLATIONREQUEST_HPP

#include "InstructionTranslationRequest.hpp"

class LeaTranslationRequest : public InstructionTranslationRequest
{
public:
    explicit LeaTranslationRequest(ZydisDisassembledInstruction disassembledInstruction)
            : InstructionTranslationRequest(disassembledInstruction)
    {}
};


#endif //TRANSLATOR_LEATRANSLATIONREQUEST_HPP
