#ifndef CONTROL_SIGNALS_H
#define CONTROL_SIGNALS_H

#include <cstdint>

enum class ALUSource
{
    REGISTER,
    IMMEDIATE
};

enum class ALUoperation
{
    ADD,
    SUB,
    MUL,
    DIV,
    AND,
    OR,
    XOR,
    SLT,
    SLL,
    SRL,
    SRA,
    NONE
};

struct ControlSignals
{
    bool reg_write = false;

    bool mem_read = false;
    bool mem_write = false;

    bool mem_to_reg = false;

    bool branch = false;
    bool jump = false;

    ALUSource alu_source = ALUSource::REGISTER;

    ALUoperation alu_operation = ALUoperation::NONE;
};

#endif