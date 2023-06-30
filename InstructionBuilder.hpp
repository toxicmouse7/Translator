//
// Created by Алексей Гладков on 08.05.2023.
//

#ifndef TRANSLATOR_INSTRUCTIONBUILDER_HPP
#define TRANSLATOR_INSTRUCTIONBUILDER_HPP

#include <vector>
#include <optional>

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

        explicit OperandBuilder(
                ZydisOperandType type, ZydisEncoderOperand& operandRef, InstructionBuilder& instructionBuilder)
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

        OperandBuilder& Mem(
                ZyanI64 displacement,
                ZyanU16 size,
                std::optional<ZydisRegister> base = std::nullopt,
                std::optional<ZyanI64> ripOffset = std::nullopt,
                std::optional<ZydisRegister> index = std::nullopt,
                std::optional<ZyanU8> scale = std::nullopt)
        {
            operand.mem.displacement = displacement;
            operand.mem.size = size;

            if (base.has_value())
            {
                if (base == ZYDIS_REGISTER_RIP && ripOffset.has_value())
                    operand.mem.displacement += *ripOffset;
                else
                    operand.mem.base = *base;
            }


            if (index.has_value())
                operand.mem.index = index.value();

            if (scale.has_value())
                operand.mem.scale = scale.value();

            return *this;
        }

        OperandBuilder& Reg(ZydisRegister reg)
        {
            operand.reg.value = reg;
            return *this;
        }

        OperandBuilder& Ptr(ZyanU32 segment, ZyanU64 offset)
        {
            operand.mem.displacement = offset;
            operand.mem.size = 4;
//            operand.ptr.offset = offset;
//            operand.ptr.segment = segment;
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
