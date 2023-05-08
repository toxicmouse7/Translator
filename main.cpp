#include <fstream>

#include <Requestor/Requestor.hpp>

#include "Handlers/InstructionTranslationHandler.hpp"
#include "Handlers/MovTranslationHandler.hpp"
#include "Handlers/InstructionToTextHandler.hpp"
#include "Handlers/RegisterDownArchitectureHandler.hpp"

int main()
{
    auto& requestor = Requestor::Instance();

    try
    {
        requestor.RegisterHandler<InstructionToTextHandler>();
        requestor.RegisterHandler<InstructionTranslationHandler>();
        requestor.RegisterHandler<MovTranslationHandler>();
        requestor.RegisterHandler<RegisterDownArchitectureHandler>();

        std::ifstream testFile("tests/test1.bin", std::ios::binary);
        std::string binaryData;
        testFile >> binaryData;
        testFile.close();

        int offset = 0;
        uintptr_t runtimeAddress = 0;
        ZydisDisassembledInstruction instruction;
        memset(&instruction, 0, sizeof(instruction));
        while (ZYAN_SUCCESS(ZydisDisassembleIntel(
                ZYDIS_MACHINE_MODE_LONG_64,
                runtimeAddress,
                binaryData.data() + offset,
                binaryData.size() - offset,
                &instruction)))
        {
            auto request = InstructionTranslationRequest(instruction);

            auto result = requestor.Handle<std::vector<ZyanU8>>(request);

            auto debugRequest = InstructionToTextRequest(result, ZYDIS_MACHINE_MODE_LEGACY_32);
            requestor.Handle(debugRequest);

            offset += instruction.info.length;
            runtimeAddress += instruction.info.length;
        }
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
