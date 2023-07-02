//
// Created by Aleksej on 25.06.2023.
//

#ifndef TRANSLATOR_ADDTRANSLATIONHANDLER_HPP
#define TRANSLATOR_ADDTRANSLATIONHANDLER_HPP

#include <Requestor/Requestor.hpp>
#include <Requestor/Interfaces/IRequestHandler.hpp>

#include <vector>

#include "Requests/AddTranslationRequest.hpp"
#include "Requests/RegisterDownArchitectureRequest.hpp"
#include "Requests/DetermineOperandsRelationRequest.hpp"
#include "OperandsRelation.hpp"

class AddTranslationHandler : public IRequestHandler<AddTranslationRequest, std::vector<ZyanU8>>
{
public:
    std::vector<ZyanU8> Handle(const AddTranslationRequest& request) override
    {
        auto& requestor = Requestor::Instance();
        auto& instruction = request.GetInstruction();

        auto operandsRelationRequest = DetermineOperandsRelationRequest(instruction);
        auto operandsRelation = requestor.Handle<OperandsRelation>(operandsRelationRequest);

        switch (operandsRelation)
        {

            case OperandsRelation::Reg2Reg:
                return AddReg2Reg(request.GetInstruction());
            case OperandsRelation::Reg2Mem:

            case OperandsRelation::Imm2Reg:

            case OperandsRelation::Mem2Reg:

            case OperandsRelation::Imm2Mem:

            default:
                throw std::runtime_error("Not implemented yet (mov operands relation)");
        }
    }

private:
    static std::vector<ZyanU8> AddReg2Reg(const ZydisDisassembledInstruction& instruction)
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
                                  .Mnemonic(ZYDIS_MNEMONIC_ADD)
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
                                  .Mnemonic(ZYDIS_MNEMONIC_ADD)
                                  .Operand(ZYDIS_OPERAND_TYPE_REGISTER)
                                  .Reg(firstDownRegister)
                                  .FinishOperand()
                                  .Operand(ZYDIS_OPERAND_TYPE_REGISTER)
                                  .Reg(secondDownRegister)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        return result;
    }
};


#endif //TRANSLATOR_ADDTRANSLATIONHANDLER_HPP
