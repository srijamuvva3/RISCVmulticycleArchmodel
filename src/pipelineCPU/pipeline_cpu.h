#ifndef PIPELINE_CPU_H
#define PIPELINE_CPU_H

#include <cstdint>

#include "../memory/memory.h"
#include "../ISA/decoder.h"

#include "../execution/alu/alu.h"
#include "../execution/register_file/register_file.h"
#include "../execution/register_file/pc.h"
#include "../execution/control_unit/control_unit.h"

#include "pipeline_trace.h"

#include "pipeline_state.h"
#include "pipeline_registers.h"
#include "hazard_unit.h"


class PipelineCPU
{
public:

    PipelineCPU(
        Memory& memory
    );


    void reset();

    void step();


    uint32_t getPC() const;

    uint32_t getRegister(uint32_t index) const;

    uint64_t getCycle() const;

    PipelineStatus getStatus() const;


private:

    // ========================================================
    // Hardware
    // ========================================================

    Memory& memory;

    RegisterFile registers;

    PC pc;

    Decoder decoder;

    ALU alu;

    ControlUnit control_unit;

    HazardUnit hazard_unit;

    PipelineTrace* pipeline_trace;


    // ========================================================
    // Pipeline registers
    // ========================================================

    IF_ID_Register if_id;

    ID_EX_Register id_ex;

    EX_MEM_Register ex_mem;

    MEM_WB_Register mem_wb;


    // ========================================================
    // State
    // ========================================================

    uint64_t cycle;

    PipelineStatus status;


    // ========================================================
    // Pipeline stages
    // ========================================================

    void writeBackStage();

    void memoryStage(
        MEM_WB_Register& next_mem_wb
    );

    void executeStage(
        EX_MEM_Register& next_ex_mem,
        bool& control_transfer,
        uint32_t& new_pc
    );

    void decodeStage(
        ID_EX_Register& next_id_ex
    );

    void fetchStage(
        IF_ID_Register& next_if_id
    );


    // ========================================================
    // Helpers
    // ========================================================

    uint32_t getForwardedValue(
        ForwardingSelect select,
        uint32_t register_value
    ) const;


    uint32_t getMEMWBWritebackValue() const;


    bool isControlTransfer(
        const Instruction& instruction
    ) const;


    bool isValidInstruction(
        uint32_t raw
    ) const;


    bool isBranch(
        const Instruction& instruction
    ) const;


    bool isJAL(
        const Instruction& instruction
    ) const;


    bool isJALR(
        const Instruction& instruction
    ) const;


    bool evaluateBranch(
        const Instruction& instruction,
        uint32_t a,
        uint32_t b
    ) const;


    void logPipelineTrace(
        const char* stage,
        const Instruction& instruction,
        uint32_t stage_pc
    );

    void logPipelineCycle();
};

#endif