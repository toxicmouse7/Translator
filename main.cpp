#include <Requestor.hpp>

#include "Handlers/InstructionTranslationHandler.hpp"
#include "Handlers/MovTranslationHandler.hpp"

#include "Requests/InstructionTranslationRequest.hpp"

int main()
{
    auto& requestor = Requestor::Instance();

    try
    {
        requestor.RegisterHandler<InstructionTranslationHandler>();
        requestor.RegisterHandler<MovTranslationHandler>();

        auto request = InstructionTranslationRequest({.info = {.mnemonic = ZYDIS_MNEMONIC_MOV}});

        auto result = requestor.Handle<std::vector<ZyanU8>>(request);
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
