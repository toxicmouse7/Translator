//
// Created by Алексей Гладков on 03.07.2023.
//

#ifndef TRANSLATOR_CALLTRANSLATIONHANDLER_HPP
#define TRANSLATOR_CALLTRANSLATIONHANDLER_HPP

#include <Requestor/Requestor.hpp>
#include <Requestor/Interfaces/IRequestHandler.hpp>

#include <vector>

#include "Requests/CallTranslationRequest.hpp"
#include "Requests/RegisterDownArchitectureRequest.hpp"
#include "Requests/DetermineOperandsRelationRequest.hpp"
#include "OperandsRelation.hpp"

class CallTranslationHandler : public IRequestHandler<CallTranslationRequest, std::vector<ZyanU8>>
{
private:
    constexpr static inline int signature = 0x12312312;
public:
    std::vector<ZyanU8> Handle(const CallTranslationRequest& request) override
    {
        auto& requestor = Requestor::Instance();
        auto& instruction = request.GetInstruction();

        auto operandsRelationRequest = DetermineOperandsRelationRequest(instruction);
        auto operandsRelation = requestor.Handle<OperandsRelation>(operandsRelationRequest);

        switch (operandsRelation)
        {
            case OperandsRelation::Reg:
            case OperandsRelation::Mem:
                return CallMem(request.GetInstruction());
            case OperandsRelation::Imm:
                return CallImm(request.GetInstruction());
            default:
                throw std::runtime_error("Incompatible operand types (call)");
        }
    }

    static std::vector<ZyanU8> CallImm(const ZydisDisassembledInstruction& instruction)
    {
        std::vector<ZyanU8> result;
        auto additionalContext = AdditionalContext::GetInstance();

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_PUSH)
                                  .Operand(ZYDIS_OPERAND_TYPE_IMMEDIATE)
                                  .Imm(signature)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_PUSH)
                                  .Operand(ZYDIS_OPERAND_TYPE_MEMORY)
                                  .Mem((int32_t)(uintptr_t)&additionalContext
                                          .GetRegister(ZYDIS_REGISTER_R9D), 4)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_PUSH)
                                  .Operand(ZYDIS_OPERAND_TYPE_MEMORY)
                                  .Mem((int32_t)(uintptr_t)&additionalContext
                                          .GetRegister(ZYDIS_REGISTER_R8D), 4)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_PUSH)
                                  .Operand(ZYDIS_OPERAND_TYPE_REGISTER)
                                  .Reg(ZYDIS_REGISTER_EDX)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_PUSH)
                                  .Operand(ZYDIS_OPERAND_TYPE_REGISTER)
                                  .Reg(ZYDIS_REGISTER_ECX)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_CALL)
                                  .Operand(ZYDIS_OPERAND_TYPE_IMMEDIATE)
                                  .Imm(instruction.operands[0].imm.value.s)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_CMP)
                                  .Operand(ZYDIS_OPERAND_TYPE_MEMORY)
                                  .Mem(0, 4, ZYDIS_REGISTER_ESP)
                                  .FinishOperand()
                                  .Operand(ZYDIS_OPERAND_TYPE_IMMEDIATE)
                                  .Imm(signature)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_JZ)
                                  .Operand(ZYDIS_OPERAND_TYPE_IMMEDIATE)
                                  .Imm(5)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_ADD)
                                  .Operand(ZYDIS_OPERAND_TYPE_MEMORY)
                                  .Mem(0, 4, ZYDIS_REGISTER_ESP)
                                  .FinishOperand()
                                  .Operand(ZYDIS_OPERAND_TYPE_IMMEDIATE)
                                  .Imm(4)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_JMP)
                                  .Operand(ZYDIS_OPERAND_TYPE_IMMEDIATE)
                                  .Imm(-14)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_ADD)
                                  .Operand(ZYDIS_OPERAND_TYPE_MEMORY)
                                  .Mem(0, 4, ZYDIS_REGISTER_ESP)
                                  .FinishOperand()
                                  .Operand(ZYDIS_OPERAND_TYPE_IMMEDIATE)
                                  .Imm(4)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));
        /*
         * m1:
         * cmp dword ptr [esp], 0x12312312
         * je m2
         * add esp, 4
         * jmp m1
         * m2:
         * add esp, 4
         */

        return result;
    }

    static std::vector<ZyanU8> CallMem(const ZydisDisassembledInstruction& instruction)
    {
        std::vector<ZyanU8> result;
        auto additionalContext = AdditionalContext::GetInstance();
        auto& requestor = Requestor::Instance();
        auto& firstOperand = instruction.operands[0];

        auto base = firstOperand.mem.base;
        auto downBaseRequest = RegisterDownArchitectureRequest(base);
        base = requestor.Handle<ZydisRegister>(downBaseRequest);


        auto index = firstOperand.mem.index;
        auto downIndexRequest = RegisterDownArchitectureRequest(index);
        index = requestor.Handle<ZydisRegister>(downIndexRequest);

        auto scale = firstOperand.mem.scale;

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_PUSH)
                                  .Operand(ZYDIS_OPERAND_TYPE_IMMEDIATE)
                                  .Imm(signature)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_PUSH)
                                  .Operand(ZYDIS_OPERAND_TYPE_MEMORY)
                                  .Mem((int32_t)(uintptr_t)&additionalContext
                                          .GetRegister(ZYDIS_REGISTER_R9D), 4)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_PUSH)
                                  .Operand(ZYDIS_OPERAND_TYPE_MEMORY)
                                  .Mem((int32_t)(uintptr_t)&additionalContext
                                          .GetRegister(ZYDIS_REGISTER_R8D), 4)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_PUSH)
                                  .Operand(ZYDIS_OPERAND_TYPE_REGISTER)
                                  .Reg(ZYDIS_REGISTER_EDX)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_PUSH)
                                  .Operand(ZYDIS_OPERAND_TYPE_REGISTER)
                                  .Reg(ZYDIS_REGISTER_ECX)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        ZyanI64 offset = 0;
        if (base == ZYDIS_REGISTER_ESP)
            offset += 5 * 4; // 4 пуша по 4 байта
        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_CALL)
                                  .Operand(ZYDIS_OPERAND_TYPE_MEMORY)
                                  .Mem(firstOperand.mem.disp.value + offset, 4,
                                       base, instruction.runtime_address + instruction.info.length,
                                       index, scale)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_CMP)
                                  .Operand(ZYDIS_OPERAND_TYPE_MEMORY)
                                  .Mem(0, 4, ZYDIS_REGISTER_ESP)
                                  .FinishOperand()
                                  .Operand(ZYDIS_OPERAND_TYPE_IMMEDIATE)
                                  .Imm(signature)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_JZ)
                                  .Operand(ZYDIS_OPERAND_TYPE_IMMEDIATE)
                                  .Imm(5)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_ADD)
                                  .Operand(ZYDIS_OPERAND_TYPE_REGISTER)
                                  .Reg(ZYDIS_REGISTER_ESP)
                                  .FinishOperand()
                                  .Operand(ZYDIS_OPERAND_TYPE_IMMEDIATE)
                                  .Imm(4)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_JMP)
                                  .Operand(ZYDIS_OPERAND_TYPE_IMMEDIATE)
                                  .Imm(-14)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_ADD)
                                  .Operand(ZYDIS_OPERAND_TYPE_REGISTER)
                                  .Reg(ZYDIS_REGISTER_ESP)
                                  .FinishOperand()
                                  .Operand(ZYDIS_OPERAND_TYPE_IMMEDIATE)
                                  .Imm(4)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        return result;
    }
};


#endif //TRANSLATOR_CALLTRANSLATIONHANDLER_HPP
