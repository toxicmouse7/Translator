//
// Created by Алексей Гладков on 04.07.2023.
//

#ifndef TRANSLATOR_XORTRANSLATIONHANDLER_HPP
#define TRANSLATOR_XORTRANSLATIONHANDLER_HPP

#include <vector>
#include <ranges>

#include <Zydis/Zydis.h>

#include <Requestor/Requestor.hpp>
#include <Requestor/Interfaces/IRequestHandler.hpp>

#include "Requests/XorTranslationRequest.hpp"
#include "Requests/RegisterDownArchitectureRequest.hpp"
#include "Requests/DetermineOperandsRelationRequest.hpp"

#include "AdditionalContext.hpp"
#include "InstructionBuilder.hpp"
#include "OperandsRelation.hpp"


class XorTranslationHandler : public IRequestHandler<XorTranslationRequest, std::vector<ZyanU8>>
{
public:
    std::vector<ZyanU8> Handle(const XorTranslationRequest& request) override
    {
        auto& requestor = Requestor::Instance();
        auto& instruction = request.GetInstruction();

        auto operandsRelationRequest = DetermineOperandsRelationRequest(instruction);
        auto operandsRelation = requestor.Handle<OperandsRelation>(operandsRelationRequest);

        switch (operandsRelation)
        {
            case OperandsRelation::Reg2Reg:
                return XorReg2Reg(instruction);
            case OperandsRelation::Reg2Mem:
                return XorReg2Mem(instruction);
            case OperandsRelation::Imm2Reg:
                return XorImm2Reg(instruction);
            case OperandsRelation::Mem2Reg:
                return XorMem2Reg(instruction);
            case OperandsRelation::Imm2Mem:
                return XorImm2Mem(instruction);
            default:
                throw std::runtime_error("Not implemented yet (xor operands relation)");
        }
    }

    static std::vector<ZyanU8> XorReg2Reg(const ZydisDisassembledInstruction& instruction)
    {
        std::vector<ZyanU8> result;

        auto& requestor = Requestor::Instance();
        auto& firstOperand = instruction.operands[0];
        auto& secondOperand = instruction.operands[1];
        auto& additionalContext = AdditionalContext::GetInstance();

        auto firstDownRegisterRequest = RegisterDownArchitectureRequest(firstOperand.reg.value);
        auto secondDownRegisterRequest = RegisterDownArchitectureRequest(secondOperand.reg.value);

        auto firstDownRegister = requestor.Handle<ZydisRegister>(firstDownRegisterRequest);
        auto secondDownRegister = requestor.Handle<ZydisRegister>(secondDownRegisterRequest);

        if (firstDownRegister == secondDownRegister && (firstDownRegister == ZYDIS_REGISTER_R9D || firstDownRegister == ZYDIS_REGISTER_R8D))
        {
            std::ranges::copy(InstructionBuilder::Builder()
            .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
            .Mnemonic(ZYDIS_MNEMONIC_MOV)
            .Operand(ZYDIS_OPERAND_TYPE_MEMORY)
            .Mem((uint32_t)&additionalContext.GetRegister(firstDownRegister), 4)
            .FinishOperand()
            .Operand(ZYDIS_OPERAND_TYPE_IMMEDIATE)
            .Imm(0)
            .FinishOperand()
            .Build(), std::back_inserter(result));

            return result;
        }

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_PUSH)
                                  .Operand(ZYDIS_OPERAND_TYPE_REGISTER)
                                  .Reg(firstDownRegister)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_MOV)
                                  .Operand(ZYDIS_OPERAND_TYPE_REGISTER)
                                  .Reg(firstDownRegister)
                                  .FinishOperand()
                                  .Operand(ZYDIS_OPERAND_TYPE_MEMORY)
                                  .Mem((uint32_t)(uintptr_t)(&additionalContext
                                          .GetRegister(secondDownRegister)), 4)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_XOR)
                                  .Operand(ZYDIS_OPERAND_TYPE_MEMORY)
                                  .Mem((uint32_t)(uintptr_t)(&additionalContext
                                          .GetRegister(firstDownRegister)), 4)
                                  .FinishOperand()
                                  .Operand(ZYDIS_OPERAND_TYPE_REGISTER)
                                  .Reg(firstDownRegister)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_POP)
                                  .Operand(ZYDIS_OPERAND_TYPE_REGISTER)
                                  .Reg(firstDownRegister)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_XOR)
                                  .Operand(ZYDIS_OPERAND_TYPE_REGISTER)
                                  .Reg(firstDownRegister)
                                  .FinishOperand()
                                  .Operand(ZYDIS_OPERAND_TYPE_REGISTER)
                                  .Reg(secondDownRegister)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        return result;
    }

    static std::vector<ZyanU8> XorReg2Mem(const ZydisDisassembledInstruction& instruction)
    {
        std::vector<ZyanU8> result;
        auto& requestor = Requestor::Instance();
        auto& additionalContext = AdditionalContext::GetInstance();
        auto& firstOperand = instruction.operands[0];
        auto& secondOperand = instruction.operands[1];

        auto secondDownRegisterRequest = RegisterDownArchitectureRequest(secondOperand.reg.value);
        auto secondDownRegister = requestor.Handle<ZydisRegister>(secondDownRegisterRequest);

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_PUSH)
                                  .Operand(ZYDIS_OPERAND_TYPE_REGISTER)
                                  .Reg(secondDownRegister)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        auto base = firstOperand.mem.base;
        auto downBaseRequest = RegisterDownArchitectureRequest(base);
        base = requestor.Handle<ZydisRegister>(downBaseRequest);


        auto index = firstOperand.mem.index;
        auto downIndexRequest = RegisterDownArchitectureRequest(index);
        index = requestor.Handle<ZydisRegister>(downIndexRequest);

        auto scale = firstOperand.mem.scale;

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_MOV)
                                  .Operand(ZYDIS_OPERAND_TYPE_REGISTER)
                                  .Reg(secondDownRegister)
                                  .FinishOperand()
                                  .Operand(ZYDIS_OPERAND_TYPE_MEMORY)
                                  .Mem((uint32_t)(uintptr_t)(&additionalContext
                                          .GetRegister(secondDownRegister)), 4)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_XOR)
                                  .Operand(ZYDIS_OPERAND_TYPE_MEMORY)
                                  .Mem(firstOperand.mem.disp.value + 4, 4,
                                       base, instruction.runtime_address + instruction.info.length,
                                       index, scale)
                                  .FinishOperand()
                                  .Operand(ZYDIS_OPERAND_TYPE_REGISTER)
                                  .Reg(secondDownRegister)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_POP)
                                  .Operand(ZYDIS_OPERAND_TYPE_REGISTER)
                                  .Reg(secondDownRegister)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_XOR)
                                  .Operand(ZYDIS_OPERAND_TYPE_MEMORY)
                                  .Mem(firstOperand.mem.disp.value, 4,
                                       base, instruction.runtime_address + instruction.info.length,
                                       index, scale)
                                  .FinishOperand()
                                  .Operand(ZYDIS_OPERAND_TYPE_REGISTER)
                                  .Reg(secondDownRegister)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        return result;
    }

    static std::vector<ZyanU8> XorImm2Reg(const ZydisDisassembledInstruction& instruction)
    {
        std::vector<ZyanU8> result;
        auto& requestor = Requestor::Instance();
        auto& additionalContext = AdditionalContext::GetInstance();
        auto& firstOperand = instruction.operands[0];
        auto& secondOperand = instruction.operands[1];

        auto firstDownRegisterRequest = RegisterDownArchitectureRequest(firstOperand.reg.value);
        auto firstDownRegister = requestor.Handle<ZydisRegister>(firstDownRegisterRequest);

        if (secondOperand.imm.value.s > INT32_MAX || secondOperand.imm.value.s < INT32_MIN)
        {
            std::ranges::copy(InstructionBuilder::Builder()
                                      .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                      .Mnemonic(ZYDIS_MNEMONIC_XOR)
                                      .Operand(ZYDIS_OPERAND_TYPE_MEMORY)
                                      .Mem((int32_t)(ZyanI64)&additionalContext
                                              .GetRegister(firstDownRegister), 4)
                                      .FinishOperand()
                                      .Operand(ZYDIS_OPERAND_TYPE_IMMEDIATE)
                                      .Imm((int32_t)((secondOperand.imm.value.s & 0xFFFFFFFF00000000) >> 32))
                                      .FinishOperand()
                                      .Build(), std::back_inserter(result));
        }

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_XOR)
                                  .Operand(ZYDIS_OPERAND_TYPE_REGISTER)
                                  .Reg(firstDownRegister)
                                  .FinishOperand()
                                  .Operand(ZYDIS_OPERAND_TYPE_IMMEDIATE)
                                  .Imm((int32_t)secondOperand.imm.value.s)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));


        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_XOR)
                                  .Operand(ZYDIS_OPERAND_TYPE_MEMORY)
                                  .Mem((int32_t)(ZyanI64)&additionalContext
                                          .GetRegister(firstDownRegister), 4)
                                  .FinishOperand()
                                  .Operand(ZYDIS_OPERAND_TYPE_IMMEDIATE)
                                  .Imm(0)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));


        return result;
    }

    static std::vector<ZyanU8> XorMem2Reg(const ZydisDisassembledInstruction& instruction)
    {
        std::vector<ZyanU8> result;
        auto& requestor = Requestor::Instance();
        auto& additionalContext = AdditionalContext::GetInstance();
        auto& firstOperand = instruction.operands[0];
        auto& secondOperand = instruction.operands[1];

        auto firstDownRegisterRequest = RegisterDownArchitectureRequest(firstOperand.reg.value);
        auto firstDownRegister = requestor.Handle<ZydisRegister>(firstDownRegisterRequest);

        auto base = secondOperand.mem.base;
        auto index = secondOperand.mem.index;
        auto scale = secondOperand.mem.scale;

        auto downBaseRequest = RegisterDownArchitectureRequest(base);
        auto downIndexRequest = RegisterDownArchitectureRequest(index);

        base = requestor.Handle<ZydisRegister>(downBaseRequest);
        index = requestor.Handle<ZydisRegister>(downIndexRequest);

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_XOR)
                                  .Operand(ZYDIS_OPERAND_TYPE_REGISTER)
                                  .Reg(firstDownRegister)
                                  .FinishOperand()
                                  .Operand(ZYDIS_OPERAND_TYPE_MEMORY)
                                  .Mem(secondOperand.mem.disp.value, 4,
                                       base, instruction.info.length + instruction.runtime_address,
                                       index, scale)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_PUSH)
                                  .Operand(ZYDIS_OPERAND_TYPE_REGISTER)
                                  .Reg(firstDownRegister)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_MOV)
                                  .Operand(ZYDIS_OPERAND_TYPE_REGISTER)
                                  .Reg(firstDownRegister)
                                  .FinishOperand()
                                  .Operand(ZYDIS_OPERAND_TYPE_MEMORY)
                                  .Mem(secondOperand.mem.disp.value + 4, 4,
                                       base, instruction.info.length + instruction.runtime_address,
                                       index, scale)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_XOR)
                                  .Operand(ZYDIS_OPERAND_TYPE_MEMORY)
                                  .Mem((int32_t)(ZyanI64)&additionalContext
                                          .GetRegister(firstDownRegister), 4)
                                  .FinishOperand()
                                  .Operand(ZYDIS_OPERAND_TYPE_REGISTER)
                                  .Reg(firstDownRegister)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_POP)
                                  .Operand(ZYDIS_OPERAND_TYPE_REGISTER)
                                  .Reg(firstDownRegister)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        return result;
    }

    static std::vector<ZyanU8> XorImm2Mem(const ZydisDisassembledInstruction& instruction)
    {
        std::vector<ZyanU8> result;

        auto& mem = instruction.operands[0].mem;
        auto& imm = instruction.operands[1].imm;

        if (imm.value.u > UINT32_MAX)
        {
            std::ranges::copy(InstructionBuilder::Builder()
                                      .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                      .Mnemonic(ZYDIS_MNEMONIC_XOR)
                                      .Operand(ZYDIS_OPERAND_TYPE_MEMORY)
                                      .Mem((ZyanI64)(mem.disp.value
                                                     + instruction.info.length
                                                     + instruction.runtime_address
                                                     + 4), 4)
                                      .FinishOperand()
                                      .Operand(ZYDIS_OPERAND_TYPE_IMMEDIATE)
                                      .Imm((int32_t)((imm.value.s & 0xFFFFFFFF00000000) >> 32))
                                      .FinishOperand()
                                      .Build(), std::back_inserter(result));
        }

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_XOR)
                                  .Operand(ZYDIS_OPERAND_TYPE_MEMORY)
                                  .Mem((ZyanI64)(mem.disp.value
                                                 + instruction.info.length
                                                 + instruction.runtime_address), 4)
                                  .FinishOperand()
                                  .Operand(ZYDIS_OPERAND_TYPE_IMMEDIATE)
                                  .Imm((int32_t)imm.value.s)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        return result;
    }
};


#endif //TRANSLATOR_XORTRANSLATIONHANDLER_HPP
