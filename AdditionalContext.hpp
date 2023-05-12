//
// Created by Алексей Гладков on 08.05.2023.
//

#ifndef TRANSLATOR_ADDITIONALCONTEXT_HPP
#define TRANSLATOR_ADDITIONALCONTEXT_HPP

#include <cstdint>
#include <map>

#include <Zydis/Zydis.h>

struct AdditionalContext
{
private:
    inline static AdditionalContext* instance = nullptr;
    uint32_t highEax;
    uint32_t highEbx;
    uint32_t highEcx;
    uint32_t highEdx;
    std::map<ZydisRegister, uint32_t&> regRefs {
            {ZYDIS_REGISTER_EAX, highEax},
            {ZYDIS_REGISTER_EBX, highEbx},
            {ZYDIS_REGISTER_ECX, highEcx},
            {ZYDIS_REGISTER_EDX, highEdx},
    };
public:

    static AdditionalContext& GetInstance()
    {
        if (instance == nullptr)
            instance = new AdditionalContext;

        return *instance;
    }

    uint32_t& GetRegister(ZydisRegister reg)
    {
        return regRefs.at(reg);
    }
};


#endif //TRANSLATOR_ADDITIONALCONTEXT_HPP
