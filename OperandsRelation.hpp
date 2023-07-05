//
// Created by Алексей Гладков on 12.05.2023.
//

#ifndef TRANSLATOR_OPERANDSRELATION_HPP
#define TRANSLATOR_OPERANDSRELATION_HPP


enum class OperandsRelation
{
    Reg2Reg,
    Reg2Mem,
    Imm2Reg,
    Mem2Reg,
    Imm2Mem,
    Reg,
    Mem,
    Imm
};


#endif //TRANSLATOR_OPERANDSRELATION_HPP
