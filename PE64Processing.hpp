//
// Created by Aleksej on 05.07.2023.
//

#ifndef TRANSLATOR_PE64PROCESSING_HPP
#define TRANSLATOR_PE64PROCESSING_HPP

#include <string>
#include <vector>
#include <Windows.h>

class PE64Processing
{
private:
    std::vector<char> peBuffer;

    PIMAGE_DOS_HEADER dosHeader = nullptr;
    PIMAGE_NT_HEADERS64 ntHeaders = nullptr;
    std::vector<PIMAGE_SECTION_HEADER> sectionHeaders;
    std::vector<byte*> sections;
    std::vector<u_long> alignedSectionSizes;


    void LoadPE(const std::string& fileName);
    void LoadImportTable();
public:
    explicit PE64Processing(const std::string& fileName);
    ~PE64Processing();
    int ExecuteX86();
};


#endif //TRANSLATOR_PE64PROCESSING_HPP
