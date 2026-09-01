#include "pipeline_registers.h"


void resetIF_ID(IF_ID_Register& reg)
{
    reg.valid = false;

    reg.pc = 0;
    reg.instruction = 0;
}


void resetID_EX(ID_EX_Register& reg)
{
    reg.valid = false;

    reg.pc = 0;

    reg.instruction = Instruction{};

    reg.rs1_value = 0;
    reg.rs2_value = 0;

    reg.immediate = 0;

    reg.rd = 0;
    reg.rs1 = 0;
    reg.rs2 = 0;

    reg.control = ControlSignals{};

    reg.pc_value = 0;
}


void resetEX_MEM(EX_MEM_Register& reg)
{
    reg.valid = false;

    reg.pc = 0;

    reg.instruction = Instruction{};

    reg.alu_result = 0;

    reg.rs2_value = 0;

    reg.rd = 0;

    reg.immediate = 0;

    reg.control = ControlSignals{};

    reg.writeback_value = 0;

    reg.branch_taken = false;
    reg.branch_target = 0;
}


void resetMEM_WB(MEM_WB_Register& reg)
{
    reg.valid = false;

    reg.pc = 0;

    reg.instruction = Instruction{};

    reg.alu_result = 0;

    reg.memory_data = 0;

    reg.rd = 0;

    reg.control = ControlSignals{};

    reg.writeback_value = 0;
}