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

add_executable(Translator Handlers/MovTranslationHandler.hpp main.cpp Requests/InstructionTranslationRequest.hpp Handlers/RegisterDownArchitectureHandler.hpp Requests/RegisterDownArchitectureRequest.hpp Handlers/InstructionTranslationHandler.hpp Requests/MovTranslationRequest.hpp Requests/InstructionToTextRequest.hpp Handlers/InstructionToTextHandler.hpp)

target_include_directories(Translator PUBLIC .)

find_package(fmt)

target_link_libraries(Translator PRIVATE "Zydis" "Requestor" fmt::fmt)
