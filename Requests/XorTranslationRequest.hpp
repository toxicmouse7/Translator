//
// Created by Алексей Гладков on 04.07.2023.
//

#ifndef TRANSLATOR_XORTRANSLATIONREQUEST_HPP
#define TRANSLATOR_XORTRANSLATIONREQUEST_HPP

#include "InstructionTranslationRequest.hpp"

class XorTranslationRequest : public InstructionTranslationRequest
{
public:
    explicit XorTranslationRequest(const ZydisDisassembledInstruction& instruction)
        : InstructionTranslationRequest(instruction)
    {}
};


#endif //TRANSLATOR_XORTRANSLATIONREQUEST_HPP
