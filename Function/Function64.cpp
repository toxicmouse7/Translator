//
// Created by Aleksej on 06.07.2023.
//

#include "Function64.hpp"

#include <stack>
#include <stdexcept>
#include <Requestor/Requestor.hpp>

#include "Requests/InstructionTranslationRequest.hpp"
#include "Requests/InstructionToTextRequest.hpp"

Function64::Function64(std::byte* entryPoint)
{
    FillAdjacencyList(entryPoint);
    FillInstructionsFromAdjacencyList();
}

std::shared_ptr<std::list<uint64_t>> Function64::GetAddressesOfReturn()
{
    auto addressesOfReturn = std::make_shared<std::list<uint64_t>>();

    for (auto& adjacency: *adjacencyList)
    {
        if (adjacency.second.empty())
            addressesOfReturn->push_back((*instructions)[adjacency.first].back().runtime_address);
    }

    return addressesOfReturn;
}

void Function64::FillAdjacencyList(std::byte* entryPoint)
{
    auto address = reinterpret_cast<uint32_t>(entryPoint);
    adjacencyList = std::make_shared<std::map<uint32_t, std::set<uint32_t>>>();
    std::stack<uint32_t> toVisit;
    toVisit.push(address);

    (*adjacencyList)[address] = {};

    while (!toVisit.empty())
    {
        auto currentAddress = toVisit.top();
        toVisit.pop();
        ZydisDisassembledInstruction instruction;
        DisassembleInstruction(currentAddress, &instruction, ZYDIS_MACHINE_MODE_LONG_64);

        if (IsConditionalJumpInstruction(instruction))
        {
            auto newAddress = currentAddress + instruction.info.length;
            auto newConditionalAddress = GetJumpOffsetFromInstruction(instruction);

            if (!adjacencyList->contains(newConditionalAddress))
            {
                (*adjacencyList)[newConditionalAddress] = {};
                toVisit.push(newConditionalAddress);
            }
            if (!adjacencyList->contains(newAddress))
            {
                (*adjacencyList)[newAddress] = {};
                toVisit.push(newAddress);
            }
        }
        else if (IsJumpInstruction(instruction))
        {
            auto newAddress = GetJumpOffsetFromInstruction(instruction);
            if (!adjacencyList->contains(newAddress))
            {
                (*adjacencyList)[newAddress] = {};
                toVisit.push(newAddress);
            }
        }
        else if (instruction.info.mnemonic != ZYDIS_MNEMONIC_RET)
        {
            auto newAddress = currentAddress + instruction.info.length;
            toVisit.push(newAddress);
        }
    }
}

bool Function64::IsConditionalJumpInstruction(const ZydisDisassembledInstruction& instruction)
{
    switch (instruction.info.mnemonic)
    {
        case ZYDIS_MNEMONIC_JZ:
        case ZYDIS_MNEMONIC_JB:
        case ZYDIS_MNEMONIC_JBE:
        case ZYDIS_MNEMONIC_JL:
        case ZYDIS_MNEMONIC_JLE:
        case ZYDIS_MNEMONIC_JNB:
        case ZYDIS_MNEMONIC_JNBE:
        case ZYDIS_MNEMONIC_JNL:
        case ZYDIS_MNEMONIC_JNLE:
        case ZYDIS_MNEMONIC_JNO:
        case ZYDIS_MNEMONIC_JNP:
        case ZYDIS_MNEMONIC_JNS:
        case ZYDIS_MNEMONIC_JNZ:
        case ZYDIS_MNEMONIC_JO:
        case ZYDIS_MNEMONIC_JP:
        case ZYDIS_MNEMONIC_JS:
            return true;
        default:
            return false;
    }

}

bool Function64::IsJumpInstruction(const ZydisDisassembledInstruction& instruction)
{
    return instruction.info.mnemonic == ZYDIS_MNEMONIC_JMP;
}

void Function64::DisassembleInstruction(
        uint32_t address, ZydisDisassembledInstruction* instruction, ZydisMachineMode machineMode)
{
    std::byte buffer[15];

    memcpy(buffer, (char*) address, 15);

    ZydisDisassembleIntel(machineMode, address, buffer, 15, instruction);
}

uint32_t Function64::GetJumpOffsetFromInstruction(const ZydisDisassembledInstruction& instruction)
{
    if (!IsJumpInstruction(instruction) && !IsConditionalJumpInstruction(instruction))
        throw std::runtime_error("Instruction is not a jump instruction");

    switch (instruction.operands[0].type)
    {
        case ZYDIS_OPERAND_TYPE_IMMEDIATE:
        {
            return instruction.runtime_address + instruction.info.length + instruction.operands[0].imm.value.s;
        }
        case ZYDIS_OPERAND_TYPE_MEMORY:
        {
            return *(uint32_t*) instruction.operands[0].mem.disp.value;
        }
        default:
            throw std::runtime_error("Unknown jump operand type");
    }
}

void Function64::FillInstructionsFromAdjacencyList()
{
    instructions = std::make_shared<std::map<uint32_t, std::vector<ZydisDisassembledInstruction>>>();

    for (auto& adjacencyEntry: *adjacencyList)
    {
        auto pieceEntry = adjacencyEntry.first;
        while (true)
        {
            ZydisDisassembledInstruction instruction;
            DisassembleInstruction(pieceEntry, &instruction, ZYDIS_MACHINE_MODE_LONG_64);

            (*instructions)[adjacencyEntry.first].push_back(instruction);
            pieceEntry += instruction.info.length;

            if (IsConditionalJumpInstruction(instruction) || IsJumpInstruction(instruction))
            {
                adjacencyEntry.second.insert(GetJumpOffsetFromInstruction(instruction));
                if (IsJumpInstruction(instruction)) break;
            }

            if (adjacencyList->contains(pieceEntry))
            {
                adjacencyEntry.second.insert(pieceEntry);
                break;
            }

            if (instruction.info.mnemonic == ZYDIS_MNEMONIC_RET)
            {
                break;
            }
        }
    }
}

std::vector<ZyanU8> Function64::TranslateToX86()
{
    if (instructions->size() > 1)
        throw std::runtime_error("Not implemented yet");

    auto& requestor = Requestor::Instance();
    std::vector<ZyanU8> function86;

    for (auto& instructionPlacement : *instructions)
    {
        for (auto& instruction : instructionPlacement.second)
        {
            auto request = InstructionTranslationRequest(instruction);
            auto result = requestor.Handle<std::vector<ZyanU8>>(request);
            auto debugRequest = InstructionToTextRequest(result, ZYDIS_MACHINE_MODE_LEGACY_32);
            auto translatedInstruction = requestor.Handle<std::string>(debugRequest);
            std::cout << "\x1B[33m" << instruction.text << std::endl;
            std::cout << "\x1B[32m" << translatedInstruction << std::endl;
            std::ranges::copy(result, std::back_inserter(function86));
        }
    }

    return function86;
}
