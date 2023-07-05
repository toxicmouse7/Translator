//
// Created by Алексей Гладков on 04.07.2023.
//

#ifndef TRANSLATOR_UTILS_HPP
#define TRANSLATOR_UTILS_HPP

#include <Zydis/Zydis.h>

class Utils
{
public:
    static bool IsTranslationNeed(const ZydisDisassembledInstruction& instruction)
    {
        for (int i = 0; i < instruction.info.operand_count; ++i)
        {
            switch (instruction.operands[i].type)
            {
                case ZYDIS_OPERAND_TYPE_REGISTER:
                {
                    auto reg = instruction.operands[i].reg.value;
                    if (ZydisRegisterGetWidth(ZYDIS_MACHINE_MODE_LONG_64, reg) == 64
                        || reg == ZYDIS_REGISTER_R8D || reg == ZYDIS_REGISTER_R9D)
                        return true;
                    break;
                }
                case ZYDIS_OPERAND_TYPE_MEMORY:
                {
                    auto base = instruction.operands[i].mem.base;
                    auto index = instruction.operands[i].mem.index;
                    if (ZydisRegisterGetWidth(ZYDIS_MACHINE_MODE_LONG_64, base) == 64
                        || ZydisRegisterGetWidth(ZYDIS_MACHINE_MODE_LONG_64, index) == 64)
                        return true;
                    break;
                }
                default:
                    continue;
            }
        }

        return false;
    }
};


#endif //TRANSLATOR_UTILS_HPP
