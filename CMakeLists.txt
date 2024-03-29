cmake_minimum_required(VERSION 3.25)
project(Translator)

set(CMAKE_CXX_STANDARD 20)

# Register Zydis dependency.
# Disable build of tools and examples.
option(ZYDIS_BUILD_TOOLS "" OFF)
option(ZYDIS_BUILD_EXAMPLES "" OFF)
#set(ZYAN_ZYCORE_PATH "D:\\ClionProjects\\Translator\\deps\\zydis\\dependencies\\zycore")
add_subdirectory("deps/zydis")
add_subdirectory("deps/requestor")

add_executable(Translator Handlers/MovTranslationHandler.hpp main.cpp Requests/InstructionTranslationRequest.hpp Handlers/RegisterDownArchitectureHandler.hpp Requests/RegisterDownArchitectureRequest.hpp Handlers/InstructionTranslationHandler.hpp Requests/MovTranslationRequest.hpp Requests/InstructionToTextRequest.hpp Handlers/InstructionToTextHandler.hpp AdditionalContext.hpp InstructionBuilder.hpp OperandsRelation.hpp Requests/DetermineOperandsRelationRequest.hpp Handlers/DetermineOperandsRelationHandler.hpp Requests/LeaTranslationRequest.hpp Handlers/LeaTranslationHandler.hpp Handlers/LeaTranslationHandler.hpp Requests/AddTranslationRequest.hpp Handlers/AddTranslationHandler.hpp Requests/CallTranslationRequest.hpp Handlers/CallTranslationHandler.hpp Handlers/SubTranslationHandler.hpp Requests/SubTranslationRequest.hpp Utils.hpp Requests/XorTranslationRequest.hpp Handlers/XorTranslationHandler.hpp PE64Processing.cpp PE64Processing.hpp Function/Function64.cpp Function/Function64.hpp Requests/RetTranslationRequest.hpp Handlers/RetTranslationHandler.hpp Function/Function32.cpp Function/Function32.hpp)

target_include_directories(Translator PUBLIC .)

find_package(fmt)

target_link_libraries(Translator PRIVATE "Zydis" "Requestor" fmt::fmt)
