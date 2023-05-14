//
// Created by Алексей Гладков on 12.05.2023.
//

#ifndef TRANSLATOR_DETERMINEOPERANDSRELATIONREQUEST_HPP
#define TRANSLATOR_DETERMINEOPERANDSRELATIONREQUEST_HPP

#include <Requestor/Interfaces/IRequest.hpp>

#include <Zydis/Zydis.h>

class DetermineOperandsRelationRequest : IRequest
{
private:
    ZydisDisassembledInstruction instruction;
public:
    explicit DetermineOperandsRelationRequest(const ZydisDisassembledInstruction& instruction)
            : instruction(instruction)
    {
    }

    const ZydisDisassembledInstruction& GetInstruction()
    {
        return instruction;
    }
};


#endif //TRANSLATOR_DETERMINEOPERANDSRELATIONREQUEST_HPP
