//
// Created by Aleksej on 23.04.2023.
//

#ifndef TRANSLATOR_MOVTRANSLATIONREQUEST_HPP
#define TRANSLATOR_MOVTRANSLATIONREQUEST_HPP

#include <Requestor/Interfaces/IRequest.hpp>

#include "InstructionTranslationRequest.hpp"

class MovTranslationRequest : public InstructionTranslationRequest
{
public:
    explicit MovTranslationRequest(ZydisDisassembledInstruction disassembledInstruction)
            : InstructionTranslationRequest(disassembledInstruction)
    {}
};


#endif //TRANSLATOR_MOVTRANSLATIONREQUEST_HPP
