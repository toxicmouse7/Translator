//
// Created by Алексей Гладков on 07.05.2023.
//

#ifndef TRANSLATOR_INSTRUCTIONTOTEXTREQUEST_HPP
#define TRANSLATOR_INSTRUCTIONTOTEXTREQUEST_HPP

#include <vector>

#include <Requestor/Interfaces/IRequest.hpp>

#include <Zydis/Zydis.h>

class InstructionToTextRequest : public IRequest
{
private:
    std::vector<ZyanU8> bytes;
    ZydisMachineMode machineMode;
public:
    explicit InstructionToTextRequest(const std::vector<ZyanU8>& bytes, ZydisMachineMode machineMode)
    {
        this->bytes = bytes;
        this->machineMode = machineMode;
    }

    [[nodiscard]] const std::vector<ZyanU8>& GetBytes() const
    {
        return bytes;
    }

    [[nodiscard]] ZydisMachineMode GetMode() const
    {
        return machineMode;
    }
};


#endif //TRANSLATOR_INSTRUCTIONTOTEXTREQUEST_HPP
