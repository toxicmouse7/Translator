//
// Created by Алексей Гладков on 03.07.2023.
//

#ifndef TRANSLATOR_CALLTRANSLATIONREQUEST_HPP
#define TRANSLATOR_CALLTRANSLATIONREQUEST_HPP

#include "InstructionTranslationRequest.hpp"

class CallTranslationRequest : public InstructionTranslationRequest
{
public:
    explicit CallTranslationRequest(ZydisDisassembledInstruction disassembledInstruction)
            : InstructionTranslationRequest(disassembledInstruction)
    {
    }
};


#endif //TRANSLATOR_CALLTRANSLATIONREQUEST_HPP
