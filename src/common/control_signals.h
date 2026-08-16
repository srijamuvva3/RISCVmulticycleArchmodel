#ifndef CONTROL_SIGNALS_H
#define CONTROL_SIGNALS_H

#include "../alu/alu.h"

enum class ALUSource
{
    REGISTER,
    IMMEDIATE
};

struct ControlSignals
{
    bool reg_write = false;

    ALUSource alu_source = ALUSource::REGISTER;
    ALUoperation alu_operation = ALUoperation::NONE;

    bool mem_read = false;
    bool mem_write = false;

    bool mem_to_reg = false;

    bool branch = false;
    bool jump = false;
};

#endif