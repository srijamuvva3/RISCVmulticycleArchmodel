#ifndef ALU_H
#define ALU_H

#include <cstdint>

enum class ALUoperation {
    ADD,
    SUB,
    MUL,
    DIV,
    AND,
    OR,
    XOR,
    SLL,
    SRL,
    SRA,
    SLT,
    NONE
};

class ALU {
public:
    uint32_t execute(uint32_t a, uint32_t b, ALUoperation operation) const;
};

#endif // ALU_H