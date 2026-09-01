#ifndef PIPELINE_REGISTERS_H
#define PIPELINE_REGISTERS_H

#include <cstdint>

#include "../common/instruction.h"
#include "../common/control_signals.h"


// ============================================================
// IF / ID PIPELINE REGISTER
// ============================================================

struct IF_ID_Register
{
    bool valid = false;

    uint32_t pc = 0;
    uint32_t instruction = 0;
};


// ============================================================
// ID / EX PIPELINE REGISTER
// ============================================================

struct ID_EX_Register
{
    bool valid = false;

    uint32_t pc = 0;

    Instruction instruction{};

    uint32_t rs1_value = 0;
    uint32_t rs2_value = 0;

    int32_t immediate = 0;

    uint32_t rd = 0;
    uint32_t rs1 = 0;
    uint32_t rs2 = 0;

    ControlSignals control{};

    // Used by AUIPC/JAL
    uint32_t pc_value = 0;
};


// ============================================================
// EX / MEM PIPELINE REGISTER
// ============================================================

struct EX_MEM_Register
{
    bool valid = false;

    uint32_t pc = 0;

    Instruction instruction{};

    uint32_t alu_result = 0;

    uint32_t rs2_value = 0;

    uint32_t rd = 0;

    int32_t immediate = 0;

    ControlSignals control{};

    // Value to be written for JAL/JALR
    uint32_t writeback_value = 0;

    // Branch information
    bool branch_taken = false;
    uint32_t branch_target = 0;
};


// ============================================================
// MEM / WB PIPELINE REGISTER
// ============================================================

struct MEM_WB_Register
{
    bool valid = false;

    uint32_t pc = 0;

    Instruction instruction{};

    uint32_t alu_result = 0;

    uint32_t memory_data = 0;

    uint32_t rd = 0;

    ControlSignals control{};

    uint32_t writeback_value = 0;
};


// ============================================================
// RESET HELPERS
// ============================================================

void resetIF_ID(IF_ID_Register& reg);
void resetID_EX(ID_EX_Register& reg);
void resetEX_MEM(EX_MEM_Register& reg);
void resetMEM_WB(MEM_WB_Register& reg);

#endif