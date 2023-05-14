//
// Created by Алексей Гладков on 07.05.2023.
//

#ifndef TRANSLATOR_INSTRUCTIONTOTEXTHANDLER_HPP
#define TRANSLATOR_INSTRUCTIONTOTEXTHANDLER_HPP

#include <string>
#include <sstream>

#include <Requestor/Interfaces/IRequestHandler.hpp>

#include <Zydis/Zydis.h>

#include "Requests/InstructionToTextRequest.hpp"


class InstructionToTextHandler : public IRequestHandler<InstructionToTextRequest, std::string>
{
public:
    std::string Handle(const InstructionToTextRequest& request) override
    {
        int offset = 0;
        uintptr_t runtimeAddress = 0;
        ZydisDisassembledInstruction instruction;
        memset(&instruction, 0, sizeof(instruction));

        std::ostringstream ss;

        while (ZYAN_SUCCESS(ZydisDisassembleIntel(
                /* machine_mode:    */ request.GetMode(),
                /* runtime_address: */ 0,
                /* buffer:          */ request.GetBytes().data() + offset,
                /* length:          */ request.GetBytes().size() - offset,
                /* instruction:     */ &instruction
        ))) {
            ss << instruction.text << std::endl;
            offset += instruction.info.length;
            runtimeAddress += instruction.info.length;
        }

        return ss.str();
    }
};


#endif //TRANSLATOR_INSTRUCTIONTOTEXTHANDLER_HPP
