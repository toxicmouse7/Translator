//
// Created by Алексей Гладков on 07.05.2023.
//

#ifndef TRANSLATOR_INSTRUCTIONTOTEXTHANDLER_HPP
#define TRANSLATOR_INSTRUCTIONTOTEXTHANDLER_HPP

#include <iostream>

#include <Requestor/Interfaces/IRequestHandler.hpp>

#include <Zydis/Zydis.h>

#include "Requests/InstructionToTextRequest.hpp"


class InstructionToTextHandler : public IRequestHandler<InstructionToTextRequest, void>
{
public:
    void Handle(const InstructionToTextRequest& request) override
    {
        int offset = 0;
        uintptr_t runtimeAddress = 0;
        ZydisDisassembledInstruction instruction;
        memset(&instruction, 0, sizeof(instruction));

        while (ZYAN_SUCCESS(ZydisDisassembleIntel(
                /* machine_mode:    */ request.GetMode(),
                /* runtime_address: */ 0,
                /* buffer:          */ request.GetBytes().data() + offset,
                /* length:          */ request.GetBytes().size() - offset,
                /* instruction:     */ &instruction
        ))) {
            std::cout << instruction.text << std::endl;
            offset += instruction.info.length;
            runtimeAddress += instruction.info.length;
        }
    }
};


#endif //TRANSLATOR_INSTRUCTIONTOTEXTHANDLER_HPP
