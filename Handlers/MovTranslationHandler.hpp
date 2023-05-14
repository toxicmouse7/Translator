//
// Created by Aleksej on 22.04.2023.
//

#pragma once

#include <vector>
#include <ranges>

#include <Zydis/Zydis.h>

#include <Requestor/Requestor.hpp>
#include <Requestor/Interfaces/IRequestHandler.hpp>

#include "Requests/MovTranslationRequest.hpp"
#include "Requests/RegisterDownArchitectureRequest.hpp"
#include "Requests/DetermineOperandsRelationRequest.hpp"

#include "AdditionalContext.hpp"
#include "InstructionBuilder.hpp"
#include "OperandsRelation.hpp"

class MovTranslationHandler : public IRequestHandler<MovTranslationRequest, std::vector<ZyanU8>>
{
public:
    std::vector<ZyanU8> Handle(const MovTranslationRequest& request) override
    {
        auto& requestor = Requestor::Instance();
        auto& instruction = request.GetInstruction();

        auto operandsRelationRequest = DetermineOperandsRelationRequest(instruction);
        auto operandsRelation = requestor.Handle<OperandsRelation>(operandsRelationRequest);

        switch (operandsRelation)
        {

            case OperandsRelation::Reg2Reg:
                return MovReg2Reg(instruction);
            case OperandsRelation::Reg2Mem:
                return MovReg2Mem(instruction);
            case OperandsRelation::Imm2Reg:
                return MovImm2Reg(instruction);
            case OperandsRelation::Mem2Reg:
                return MovMem2Reg(instruction);
            case OperandsRelation::Imm2Mem:
                break;
            default:
                throw std::runtime_error("Not implemented yet (mov operands relation)");
        }

        return {};
    }

    static std::vector<ZyanU8> MovReg2Reg(const ZydisDisassembledInstruction& instruction)
    {
        std::vector<ZyanU8> result;
        auto& requestor = Requestor::Instance();
        auto& additionalContext = AdditionalContext::GetInstance();
        auto& firstOperand = instruction.operands[0];
        auto& secondOperand = instruction.operands[1];

        auto firstDownRegisterRequest = RegisterDownArchitectureRequest(firstOperand.reg.value);
        auto secondDownRegisterRequest = RegisterDownArchitectureRequest(secondOperand.reg.value);

        auto firstDownRegister = requestor.Handle<ZydisRegister>(firstDownRegisterRequest);
        auto secondDownRegister = requestor.Handle<ZydisRegister>(secondDownRegisterRequest);

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_MOV)
                                  .Operand(ZYDIS_OPERAND_TYPE_REGISTER)
                                  .Reg(firstDownRegister)
                                  .FinishOperand()
                                  .Operand(ZYDIS_OPERAND_TYPE_MEMORY)
                                  .Mem((int32_t) (ZyanI64) &additionalContext
                                          .GetRegister(secondDownRegister), 4)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_MOV)
                                  .Operand(ZYDIS_OPERAND_TYPE_MEMORY)
                                  .Mem((int32_t) (ZyanI64) &additionalContext
                                          .GetRegister(firstDownRegister), 4)
                                  .FinishOperand()
                                  .Operand(ZYDIS_OPERAND_TYPE_REGISTER)
                                  .Reg(secondDownRegister)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_MOV)
                                  .Operand(ZYDIS_OPERAND_TYPE_REGISTER)
                                  .Reg(firstDownRegister)
                                  .FinishOperand()
                                  .Operand(ZYDIS_OPERAND_TYPE_REGISTER)
                                  .Reg(secondDownRegister)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        return result;
    }

    static std::vector<ZyanU8> MovImm2Reg(const ZydisDisassembledInstruction& instruction)
    {
        std::vector<ZyanU8> result;
        auto& requestor = Requestor::Instance();
        auto& additionalContext = AdditionalContext::GetInstance();
        auto& firstOperand = instruction.operands[0];
        auto& secondOperand = instruction.operands[1];

        auto firstDownRegisterRequest = RegisterDownArchitectureRequest(firstOperand.reg.value);
        auto firstDownRegister = requestor.Handle<ZydisRegister>(firstDownRegisterRequest);

        if (secondOperand.imm.value.u > UINT32_MAX)
        {
            std::ranges::copy(InstructionBuilder::Builder()
                                      .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                      .Mnemonic(ZYDIS_MNEMONIC_MOV)
                                      .Operand(ZYDIS_OPERAND_TYPE_MEMORY)
                                      .Mem((int32_t) (ZyanI64) &additionalContext
                                              .GetRegister(firstDownRegister), 4)
                                      .FinishOperand()
                                      .Operand(ZYDIS_OPERAND_TYPE_IMMEDIATE)
                                      .Imm((int32_t) ((secondOperand.imm.value.s & 0xFFFFFFFF00000000) >> 32))
                                      .FinishOperand()
                                      .Build(), std::back_inserter(result));
        }

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_MOV)
                                  .Operand(ZYDIS_OPERAND_TYPE_REGISTER)
                                  .Reg(firstDownRegister)
                                  .FinishOperand()
                                  .Operand(ZYDIS_OPERAND_TYPE_IMMEDIATE)
                                  .Imm((int32_t) secondOperand.imm.value.s)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        return result;
    }

    static std::vector<ZyanU8> MovMem2Reg(const ZydisDisassembledInstruction& instruction)
    {
        std::vector<ZyanU8> result;
        auto& requestor = Requestor::Instance();
        auto& additionalContext = AdditionalContext::GetInstance();
        auto& firstOperand = instruction.operands[0];
        auto& secondOperand = instruction.operands[1];

        auto firstDownRegisterRequest = RegisterDownArchitectureRequest(firstOperand.reg.value);
        auto firstDownRegister = requestor.Handle<ZydisRegister>(firstDownRegisterRequest);

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_MOV)
                                  .Operand(ZYDIS_OPERAND_TYPE_REGISTER)
                                  .Reg(firstDownRegister)
                                  .FinishOperand()
                                  .Operand(ZYDIS_OPERAND_TYPE_MEMORY)
                                  .Mem((ZyanI64) (secondOperand.mem.disp.value
                                                  + instruction.info.length
                                                  + instruction.runtime_address
                                                  + 4), 4)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_MOV)
                                  .Operand(ZYDIS_OPERAND_TYPE_MEMORY)
                                  .Mem((int32_t) (ZyanI64) &additionalContext
                                          .GetRegister(firstDownRegister), 4)
                                  .FinishOperand()
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
                                  .Mem((ZyanI64) (secondOperand.mem.disp.value
                                                  + instruction.info.length
                                                  + instruction.runtime_address), 4)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        return result;
    }

    static std::vector<ZyanU8> MovReg2Mem(const ZydisDisassembledInstruction& instruction)
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

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_MOV)
                                  .Operand(ZYDIS_OPERAND_TYPE_REGISTER)
                                  .Reg(secondDownRegister)
                                  .FinishOperand()
                                  .Operand(ZYDIS_OPERAND_TYPE_MEMORY)
                                  .Mem((ZyanI64) (&additionalContext
                                          .GetRegister(secondDownRegister)), 4)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_MOV)
                                  .Operand(ZYDIS_OPERAND_TYPE_MEMORY)
                                  .Mem((ZyanI64) (firstOperand.mem.disp.value
                                                  + instruction.info.length
                                                  + instruction.runtime_address
                                                  + 4), 4)
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
                                  .Mnemonic(ZYDIS_MNEMONIC_MOV)
                                  .Operand(ZYDIS_OPERAND_TYPE_REGISTER)
                                  .Reg(secondDownRegister)
                                  .FinishOperand()
                                  .Operand(ZYDIS_OPERAND_TYPE_MEMORY)
                                  .Mem((ZyanI64) (firstOperand.mem.disp.value
                                                  + instruction.info.length
                                                  + instruction.runtime_address), 4)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        return result;
    }
};
