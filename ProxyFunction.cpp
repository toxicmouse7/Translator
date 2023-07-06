//
// Created by Aleksej on 07.07.2023.
//

#include "ProxyFunction.hpp"
#include "PE64Processing.hpp"
#include "Function/Function64.hpp"
#include "Function/Function32.hpp"

bool ProxyFunction(char* address)
{
    if ((uint32_t)address < (uint32_t)PE64Processing::sections[0] && (uint32_t)address > (uint32_t)PE64Processing::sections[1])
        return true;

    void* executionBuffer = PE64Processing::executionBuffer;
    auto function = Function64((std::byte*)address);

    auto translationResult = function.TranslateToX86();
    memcpy((char*) executionBuffer + PE64Processing::offset, translationResult.data(), translationResult.size());
    auto newFunctionAddress = (char*) executionBuffer + PE64Processing::offset;
    PE64Processing::relations[function.functionAddress] = (uint32_t) newFunctionAddress;
    PE64Processing::offset += translationResult.size();

    auto function32 = Function32((std::byte*)newFunctionAddress,
                                 std::shared_ptr<Function64>(&function));

    function32.SynchronizeCalls(PE64Processing::relations);
    return false;
}
