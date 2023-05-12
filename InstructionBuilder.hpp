//
// Created by Алексей Гладков on 08.05.2023.
//

#ifndef TRANSLATOR_INSTRUCTIONBUILDER_HPP
#define TRANSLATOR_INSTRUCTIONBUILDER_HPP

#include <vector>

#include <Zydis/Zydis.h>

class InstructionBuilder
{
private:
    int operandIndex = -1;
    ZydisEncoderRequest request{};

    InstructionBuilder()
    {
        memset(&request, 0, sizeof(request));
    }

public:
    static InstructionBuilder Builder()
    {
        return {};
    }

    InstructionBuilder& Mnemonic(ZydisMnemonic mnemonic)
    {
        request.mnemonic = mnemonic;
        return *this;
    }

    InstructionBuilder& Mode(ZydisMachineMode mode)
    {
        request.machine_mode = mode;
        return *this;
    }

    std::vector<ZyanU8> Build()
    {
        ZyanU8 buffer[ZYDIS_MAX_INSTRUCTION_LENGTH];
        ZyanUSize instructionSize = sizeof(buffer);
        ZydisEncoderEncodeInstruction(&request, buffer, &instructionSize);
        return {buffer, buffer + instructionSize};
    }

    class OperandBuilder
    {
    private:
        ZydisEncoderOperand& operand;
        InstructionBuilder& instructionBuilder;

        explicit OperandBuilder(ZydisOperandType type, ZydisEncoderOperand& operandRef, InstructionBuilder& instructionBuilder)
            : operand(operandRef), instructionBuilder(instructionBuilder)
        {
            operand.type = type;
        }
    public:
        friend InstructionBuilder;

        OperandBuilder& Imm(ZyanI64 value)
        {
            operand.imm.s = value;
            return *this;
        }

        OperandBuilder& Mem(ZyanI64 displacement, ZyanU16 size)
        {
            operand.mem.displacement = displacement;
            operand.mem.size = size;
            return *this;
        }

        OperandBuilder& Reg(ZydisRegister reg)
        {
            operand.reg.value = reg;
            return *this;
        }

        InstructionBuilder& FinishOperand()
        {
            return instructionBuilder;
        }
    };

    OperandBuilder Operand(ZydisOperandType type)
    {
        operandIndex++;
        request.operand_count++;
        return OperandBuilder(type, request.operands[operandIndex], *this);
    }

};


#endif //TRANSLATOR_INSTRUCTIONBUILDER_HPP
