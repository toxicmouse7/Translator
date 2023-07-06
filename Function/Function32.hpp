//
// Created by Aleksej on 06.07.2023.
//

#ifndef TRANSLATOR_FUNCTION32_HPP
#define TRANSLATOR_FUNCTION32_HPP

#include <memory>
#include <set>
#include <map>
#include <vector>
#include <list>
#include <cstddef>

#include <Zydis/Zydis.h>

#include "Function64.hpp"

class Function32
{
private:
    std::shared_ptr<std::map<uint32_t, std::set<uint32_t>>> adjacencyList;
    std::shared_ptr<std::map<uint32_t, std::vector<ZydisDisassembledInstruction>>> instructions;
    std::shared_ptr<Function64> parentFunction = nullptr;

    void FillAdjacencyList(std::byte* entryPoint);

    void FillInstructionsFromAdjacencyList();

public:
    const uint32_t functionAddress;

    explicit Function32(std::byte* entryPoint);
    explicit Function32(std::byte* entryPoint, const std::shared_ptr<Function64>& parentFunction);

    void SynchronizeCalls(const std::map<uint32_t, uint32_t>& relations);
};


#endif //TRANSLATOR_FUNCTION32_HPP
