//
// Created by Aleksej on 25.06.2023.
//

#ifndef TRANSLATOR_ADDTRANSLATIONREQUEST_HPP
#define TRANSLATOR_ADDTRANSLATIONREQUEST_HPP

#include "InstructionTranslationRequest.hpp"

class AddTranslationRequest : public InstructionTranslationRequest
{
public:
    explicit AddTranslationRequest(ZydisDisassembledInstruction disassembledInstruction)
            : InstructionTranslationRequest(disassembledInstruction)
    {}
};


#endif //TRANSLATOR_ADDTRANSLATIONREQUEST_HPP
