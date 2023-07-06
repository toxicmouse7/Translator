//
// Created by Aleksej on 06.07.2023.
//

#ifndef TRANSLATOR_FUNCTION64_HPP
#define TRANSLATOR_FUNCTION64_HPP

#include <memory>
#include <set>
#include <map>
#include <vector>
#include <list>
#include <cstddef>

#include <Zydis/Zydis.h>

class Function32;

class Function64
{
private:
    std::shared_ptr<std::map<uint32_t, std::set<uint32_t>>> adjacencyList;
    std::shared_ptr<std::map<uint32_t, std::vector<ZydisDisassembledInstruction>>> instructions;
    std::shared_ptr<std::map<uint32_t, uint32_t>> functionCalls;

    static void DisassembleInstruction(
            uint32_t address, ZydisDisassembledInstruction* instruction,
            ZydisMachineMode machineMode);

    static bool IsConditionalJumpInstruction(const ZydisDisassembledInstruction& instruction);

    static uint32_t GetJumpOffsetFromInstruction(const ZydisDisassembledInstruction& instruction);

    static bool IsJumpInstruction(const ZydisDisassembledInstruction& instruction);

    void FillAdjacencyList(std::byte* entryPoint);

    void FillInstructionsFromAdjacencyList();

    void FindRelativeFunctionsCalls();

    friend Function32;

public:
    const uint32_t functionAddress;

    explicit Function64(std::byte* entryPoint);

    std::shared_ptr<std::list<uint64_t>> GetAddressesOfReturn();

    std::vector<ZyanU8> TranslateToX86();

    const std::shared_ptr<std::map<uint32_t, uint32_t>>& GetRelativeFunctionCalls();
};


#endif //TRANSLATOR_FUNCTION64_HPP
