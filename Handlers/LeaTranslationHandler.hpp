//
// Created by Aleksej on 25.06.2023.
//

#ifndef TRANSLATOR_LEATRANSLATIONHANDLER_HPP
#define TRANSLATOR_LEATRANSLATIONHANDLER_HPP

#include <Requestor/Interfaces/IRequestHandler.hpp>

#include <vector>

#include "Requests/LeaTranslationRequest.hpp"
#include "Requests/RegisterDownArchitectureRequest.hpp"
#include "Requests/DetermineOperandsRelationRequest.hpp"

class LeaTranslationHandler : public IRequestHandler<LeaTranslationRequest, std::vector<ZyanU8>>
{
public:

    std::vector<ZyanU8> Handle(const LeaTranslationRequest& request) override
    {
        auto& requestor = Requestor::Instance();
        auto& instruction = request.GetInstruction();

        auto operandsRelationRequest = DetermineOperandsRelationRequest(instruction);
        auto operandsRelation = requestor.Handle<OperandsRelation>(operandsRelationRequest);

        switch (operandsRelation)
        {
            case OperandsRelation::Mem2Reg:
                return LeaMem2Reg(instruction);
            default:
                throw std::runtime_error("Not implemented yet (lea operands relation)");
        }
    }

private:

    static std::vector<ZyanU8> LeaMem2Reg(const ZydisDisassembledInstruction& instruction)
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
                                  .Mnemonic(ZYDIS_MNEMONIC_MOV)
                                  .Operand(ZYDIS_OPERAND_TYPE_MEMORY)
                                  .Mem((uint32_t) (&additionalContext
                                          .GetRegister(firstDownRegister)), 4)
                                  .FinishOperand()
                                  .Operand(ZYDIS_OPERAND_TYPE_IMMEDIATE)
                                  .Imm(0)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        std::ranges::copy(InstructionBuilder::Builder()
                                  .Mode(ZYDIS_MACHINE_MODE_LEGACY_32)
                                  .Mnemonic(ZYDIS_MNEMONIC_LEA)
                                  .Operand(ZYDIS_OPERAND_TYPE_REGISTER)
                                  .Reg(firstDownRegister)
                                  .FinishOperand()
                                  .Operand(ZYDIS_OPERAND_TYPE_MEMORY)
                                  .Mem(secondOperand.mem.disp.value, 4,
                                       base, instruction.runtime_address + instruction.info.length,
                                       index, scale)
                                  .FinishOperand()
                                  .Build(), std::back_inserter(result));

        return result;
    }
};


#endif //TRANSLATOR_LEATRANSLATIONHANDLER_HPP
