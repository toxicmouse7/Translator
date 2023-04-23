cmake_minimum_required(VERSION 3.25)
project(Translator)

set(CMAKE_CXX_STANDARD 20)

# Register Zydis dependency.
# Disable build of tools and examples.
option(ZYDIS_BUILD_TOOLS "" OFF)
option(ZYDIS_BUILD_EXAMPLES "" OFF)
set(ZYAN_ZYCORE_PATH "D:\\ClionProjects\\Translator\\deps\\zydis\\dependencies\\zycore")
add_subdirectory("deps/zydis")

add_executable(Translator Requestor/Interfaces/IRequestHandler.hpp Handlers/MovHandler.hpp main.cpp Requestor/Interfaces/IRequest.hpp Requests/InstructionTranslationRequest.hpp Handlers/RegisterDownArchitectureHandler.hpp Requests/RegisterDownArchitectureRequest.hpp Requestor/Requestor.hpp Requestor/Concepts.hpp Requestor/RequestHandlerFactory.hpp Requestor/Exceptions/HandlerNotFound.hpp Requestor/Exceptions/HandlerAlreadyRegistered.hpp Requestor/Exceptions/HandlerResultTypeMismatch.hpp)

target_include_directories(Translator PUBLIC .)

target_link_libraries(Translator PRIVATE "Zydis")
