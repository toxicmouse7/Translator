//
// Created by Aleksej on 05.07.2023.
//

#include "PE64Processing.hpp"
#include "Function/Function64.hpp"

#include <fstream>
#include <winnt.h>

PE64Processing::PE64Processing(const std::string& fileName)
{
    LoadPE(fileName);
    LoadImportTable();
}

PE64Processing::~PE64Processing() = default;

void PE64Processing::LoadImportTable()
{
    auto& importTableDirectory = ntHeaders->OptionalHeader
                                          .DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];

    auto countOfEntries = importTableDirectory.Size / sizeof(IMAGE_IMPORT_DESCRIPTOR);

    auto* importTable = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(
            peBuffer.data() + importTableDirectory.VirtualAddress);

    for (auto i = 0; i < countOfEntries - 1; ++i)
    {
        auto* entry = &importTable[i];
        auto* dllName = (char*) (peBuffer.data() + entry->Name);

        auto* libraryAddress = LoadLibraryA(dllName);
        if (libraryAddress == nullptr)
            throw std::runtime_error("Library not found");

        auto* originalFirstThunk = reinterpret_cast<PIMAGE_THUNK_DATA64>(entry->OriginalFirstThunk + peBuffer.data());
        auto* firstThunk = reinterpret_cast<PIMAGE_THUNK_DATA64>(entry->FirstThunk + peBuffer.data());

        while (originalFirstThunk->u1.AddressOfData != 0)
        {
            auto importByName = reinterpret_cast<PIMAGE_IMPORT_BY_NAME>(
                    peBuffer.data() + originalFirstThunk->u1.AddressOfData);
            auto functionName = (char*) importByName->Name;
            auto* functionAddress = GetProcAddress(libraryAddress, functionName);

            firstThunk->u1.Function = reinterpret_cast<ULONGLONG>(functionAddress);

            ++originalFirstThunk;
            ++firstThunk;
        }
    }
}

void PE64Processing::LoadPE(const std::string& fileName)
{
    std::ifstream file(fileName, std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    file.read(buffer.data(), size);
    file.close();

    char* bufferPtr = buffer.data();
    auto* ntHeader = reinterpret_cast<IMAGE_NT_HEADERS*>(bufferPtr +
                                                         reinterpret_cast<IMAGE_DOS_HEADER*>(bufferPtr)->e_lfanew);
    auto* sectionHeader = IMAGE_FIRST_SECTION(ntHeader);
    DWORD headersSize = (ntHeader->OptionalHeader.SizeOfHeaders + ntHeader->OptionalHeader.FileAlignment - 1) /
                        ntHeader->OptionalHeader.FileAlignment * ntHeader->OptionalHeader.FileAlignment;
    DWORD alignedSize = headersSize;
    for (int i = 0; i < ntHeader->FileHeader.NumberOfSections; ++i)
    {
        DWORD sectionAlignment = ntHeader->OptionalHeader.SectionAlignment;
        DWORD virtualSize =
                (sectionHeader[i].Misc.VirtualSize + sectionAlignment - 1) / sectionAlignment * sectionAlignment;
        alignedSectionSizes.push_back(virtualSize);
        alignedSize = max(alignedSize, sectionHeader[i].VirtualAddress + virtualSize);
    }

    peBuffer.resize(alignedSize, 0);
    memcpy(peBuffer.data(), bufferPtr, ntHeader->OptionalHeader.SizeOfHeaders);
    char* alignedBufferPtr = peBuffer.data();

    dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(alignedBufferPtr);
    ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS64>(alignedBufferPtr + dosHeader->e_lfanew);

    for (int i = 0; i < ntHeader->FileHeader.NumberOfSections; ++i)
    {
        DWORD virtualOffset = sectionHeader[i].VirtualAddress;
        DWORD rawDataOffset = sectionHeader[i].PointerToRawData;
        auto rawDataSize = sectionHeader[i].SizeOfRawData;
        memcpy(alignedBufferPtr + virtualOffset, bufferPtr + rawDataOffset, rawDataSize);

        sectionHeaders.push_back(&IMAGE_FIRST_SECTION(ntHeaders)[i]);
        sections.push_back((byte*) (alignedBufferPtr + virtualOffset));
    }

}

int PE64Processing::ExecuteX86()
{
    typedef int (*entryFunctionProto)();

    auto* entryPoint = ntHeaders->OptionalHeader.AddressOfEntryPoint + peBuffer.data();
    auto entryFunctionObject = new Function64(reinterpret_cast<std::byte*>(entryPoint));
    auto executionBuffer = VirtualAlloc(nullptr, alignedSectionSizes.front(),
                                        MEM_COMMIT, PAGE_EXECUTE_READWRITE);

    auto translationResult = entryFunctionObject->TranslateToX86();
    memcpy(executionBuffer, translationResult.data(), translationResult.size());

    auto entryFunction = (entryFunctionProto)executionBuffer;
    auto result = entryFunction();
    VirtualFree(executionBuffer, 0, MEM_RELEASE);

    return result;
}
