#ifndef INSTRUCTION_H //used to include the header file only once per translation unit - .cpp file
#define INSTRUCTION_H

#include <cstdint>

enum class InstructionFormat{ //Instruction types in RISC-V architecture
    R,
    I,
    S,
    B,
    U,
    J,
    UNKNOWN
};


struct Instruction
{
    InstructionFormat format;
    uint32_t raw;
    uint32_t opcode;
    uint32_t rd;
    uint32_t rs1;
    uint32_t rs2;
    uint32_t funct3;
    uint32_t funct7;
    int32_t immediate;
};

#endif