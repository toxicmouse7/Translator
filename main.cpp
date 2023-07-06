#include <fstream>

#include <Requestor/Requestor.hpp>

#include "Handlers/InstructionTranslationHandler.hpp"
#include "Handlers/MovTranslationHandler.hpp"
#include "Handlers/InstructionToTextHandler.hpp"
#include "Handlers/RegisterDownArchitectureHandler.hpp"
#include "Handlers/DetermineOperandsRelationHandler.hpp"
#include "Handlers/LeaTranslationHandler.hpp"
#include "Handlers/AddTranslationHandler.hpp"
#include "Handlers/CallTranslationHandler.hpp"
#include "Handlers/SubTranslationHandler.hpp"
#include "Handlers/XorTranslationHandler.hpp"
#include "Handlers/RetTranslationHandler.hpp"

#include "PE64Processing.hpp"

int main()
{
    auto& requestor = Requestor::Instance();
    requestor.RegisterHandler<InstructionToTextHandler>();
    requestor.RegisterHandler<InstructionTranslationHandler>();
    requestor.RegisterHandler<MovTranslationHandler>();
    requestor.RegisterHandler<RegisterDownArchitectureHandler>();
    requestor.RegisterHandler<DetermineOperandsRelationHandler>();
    requestor.RegisterHandler<LeaTranslationHandler>();
    requestor.RegisterHandler<AddTranslationHandler>();
    requestor.RegisterHandler<CallTranslationHandler>();
    requestor.RegisterHandler<SubTranslationHandler>();
    requestor.RegisterHandler<XorTranslationHandler>();
    requestor.RegisterHandler<RetTranslationHandler>();

    auto peProcessing = new PE64Processing("D:\\ClionProjects\\Translator\\tests\\C\\test5.exe");
    auto retCode = peProcessing->ExecuteX86();

    return 0;
}
