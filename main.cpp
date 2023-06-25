#include <fstream>

#include <Requestor/Requestor.hpp>

#include "Handlers/InstructionTranslationHandler.hpp"
#include "Handlers/MovTranslationHandler.hpp"
#include "Handlers/InstructionToTextHandler.hpp"
#include "Handlers/RegisterDownArchitectureHandler.hpp"
#include "Handlers/DetermineOperandsRelationHandler.hpp"
#include "Handlers/LeaTranslationHandler.hpp"

#include "tests/cpp/tests.hpp"

int main()
{
    auto& requestor = Requestor::Instance();

    try
    {
        requestor.RegisterHandler<InstructionToTextHandler>();
        requestor.RegisterHandler<InstructionTranslationHandler>();
        requestor.RegisterHandler<MovTranslationHandler>();
        requestor.RegisterHandler<RegisterDownArchitectureHandler>();
        requestor.RegisterHandler<DetermineOperandsRelationHandler>();
        requestor.RegisterHandler<LeaTranslationHandler>();

        auto& binaryData = Test10::data;

        int offset = 0;
        uintptr_t runtimeAddress = 0;
        ZydisDisassembledInstruction instruction;
        memset(&instruction, 0, sizeof(instruction));
        while (ZYAN_SUCCESS(ZydisDisassembleIntel(
                ZYDIS_MACHINE_MODE_LONG_64,
                runtimeAddress,
                binaryData + offset,
                sizeof(binaryData) - offset,
                &instruction)))
        {
            auto request = InstructionTranslationRequest(instruction);

            auto result = requestor.Handle<std::vector<ZyanU8>>(request);

            std::cout << "\x1B[33m" << instruction.text << std::endl << "\x1B[31m->\033[0m" << std::endl;
            auto debugRequest = InstructionToTextRequest(result, ZYDIS_MACHINE_MODE_LEGACY_32);
            auto translatedInstruction = requestor.Handle<std::string>(debugRequest);
            std::cout << "\x1B[32m" << translatedInstruction << std::endl;

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
