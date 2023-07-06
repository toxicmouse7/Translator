//
// Created by Aleksej on 06.07.2023.
//

#include "Function32.hpp"

#include <stack>
#include <stdexcept>

Function32::Function32(std::byte* entryPoint)
        : functionAddress((uint32_t) entryPoint)
{
    FillAdjacencyList(entryPoint);
    FillInstructionsFromAdjacencyList();
}

void Function32::FillAdjacencyList(std::byte* entryPoint)
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
        Function64::DisassembleInstruction(currentAddress, &instruction, ZYDIS_MACHINE_MODE_LEGACY_32);

        if (Function64::IsConditionalJumpInstruction(instruction))
        {
            auto newAddress = currentAddress + instruction.info.length;
            auto newConditionalAddress = Function64::GetJumpOffsetFromInstruction(instruction);

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
        else if (Function64::IsJumpInstruction(instruction))
        {
            auto newAddress = Function64::GetJumpOffsetFromInstruction(instruction);
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

void Function32::FillInstructionsFromAdjacencyList()
{
    instructions = std::make_shared<std::map<uint32_t, std::vector<ZydisDisassembledInstruction>>>();

    for (auto& adjacencyEntry: *adjacencyList)
    {
        auto pieceEntry = adjacencyEntry.first;
        while (true)
        {
            ZydisDisassembledInstruction instruction;
            Function64::DisassembleInstruction(pieceEntry, &instruction, ZYDIS_MACHINE_MODE_LEGACY_32);

            (*instructions)[adjacencyEntry.first].push_back(instruction);
            pieceEntry += instruction.info.length;

            if (Function64::IsConditionalJumpInstruction(instruction) || Function64::IsJumpInstruction(instruction))
            {
                adjacencyEntry.second.insert(Function64::GetJumpOffsetFromInstruction(instruction));
                if (Function64::IsJumpInstruction(instruction)) break;
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

Function32::Function32(std::byte* entryPoint, const std::shared_ptr<Function64>& parentFunction)
        : Function32(entryPoint)
{
    this->parentFunction = parentFunction;
}

void Function32::SynchronizeCalls(const std::map<uint32_t, uint32_t>& relations)
{
    if (parentFunction->functionCalls->empty())
        return;

    auto& parentInstructions = *parentFunction->instructions->begin();
    auto findFunction = [](const ZydisDisassembledInstruction& i)
    {
        return i.info.mnemonic == ZYDIS_MNEMONIC_CALL && i.operands[0].type == ZYDIS_OPERAND_TYPE_IMMEDIATE;
    };

    auto x = std::ranges::find_if(parentInstructions.second, findFunction);

    for (auto& instructionPlacement: *instructions)
    {
        for (auto& instruction: instructionPlacement.second)
        {
            if (instruction.info.mnemonic != ZYDIS_MNEMONIC_CALL ||
                instruction.operands[0].type != ZYDIS_OPERAND_TYPE_IMMEDIATE)
                continue;

            auto previousCallAddress = (uint32_t) (x->runtime_address + x->info.length + x->operands[0].imm.value.u);
            auto& newCallAddress = relations.at(previousCallAddress);
            auto offset = (int32_t) (newCallAddress - instruction.runtime_address - 5);
            auto* oldOffsetPtr = (uint32_t*) (instruction.runtime_address + 1);
            *oldOffsetPtr = offset;

            ++x;
            x = std::find_if(x, parentInstructions.second.end(), findFunction);
            if (x == parentInstructions.second.end())
                return;
        }
    }
}

void Function32::SynchronizeAddressLoads(const std::map<uint32_t, uint32_t>& relations)
{
    auto& parentInstructions = *parentFunction->instructions->begin();
    auto findFunction = [](const ZydisDisassembledInstruction& i)
    {
        return i.info.mnemonic == ZYDIS_MNEMONIC_LEA && i.operands[1].mem.base == ZYDIS_REGISTER_RIP;
    };

    auto x = std::ranges::find_if(parentInstructions.second, findFunction);

    for (auto& instructionPlacement: *instructions)
    {
        for (auto& instruction: instructionPlacement.second)
        {
            if (instruction.info.mnemonic != ZYDIS_MNEMONIC_LEA ||
                instruction.operands[1].mem.base != ZYDIS_REGISTER_NONE)
                continue;

            auto previousLeaAddress = (uint32_t) (x->runtime_address + x->info.length + x->operands[1].mem.disp.value);
            auto& newLeaAddress = relations.at(previousLeaAddress);
            auto offset = (int32_t) (newLeaAddress - instruction.runtime_address - 6);
            auto* oldOffsetPtr = (uint32_t*) (instruction.runtime_address + 2);
            *oldOffsetPtr = offset;

            ++x;
            x = std::find_if(x, parentInstructions.second.end(), findFunction);
            if (x == parentInstructions.second.end())
                return;
        }
    }
}
