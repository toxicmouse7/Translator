//
// Created by Aleksej on 05.07.2023.
//

#ifndef TRANSLATOR_PE64PROCESSING_HPP
#define TRANSLATOR_PE64PROCESSING_HPP

#include <string>
#include <vector>
#include <map>
#include <Windows.h>

class PE64Processing
{
private:
    static void LoadPE(const std::string& fileName);
    static void LoadImportTable();

public:
    explicit PE64Processing(const std::string& fileName);
    ~PE64Processing();
    static int ExecuteX86();

    inline static std::vector<char> peBuffer;

    inline static PIMAGE_DOS_HEADER dosHeader = nullptr;
    inline static PIMAGE_NT_HEADERS64 ntHeaders = nullptr;
    inline static std::vector<PIMAGE_SECTION_HEADER> sectionHeaders;
    inline static std::vector<byte*> sections;
    inline static std::vector<u_long> alignedSectionSizes;
    inline static void* executionBuffer;
    inline static uint32_t offset;
    inline static std::map<uint32_t, uint32_t> relations;
};


#endif //TRANSLATOR_PE64PROCESSING_HPP
