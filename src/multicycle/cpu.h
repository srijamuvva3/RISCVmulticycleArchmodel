#ifndef MULTICYCLE_CPU_H
#define MULTICYCLE_CPU_H

#include "../common/cpu_interface.h"
#include "../common/instruction.h"
#include "../memory/memory.h"
#include "../ISA/decoder.h"

#include "../execution/alu/alu.h"
#include "../execution/register_file/register_file.h"
#include "../execution/register_file/pc.h"
#include "../execution/control_unit/control_unit.h"

#include "state.h"
#include "control_fsm.h"

#include "../trace/logger.h"
#include "../trace/trace.h"

class MultiCycleCPU
{
public:

    MultiCycleCPU(
        Memory& memory,
        Logger* logger = nullptr
    );

    void reset();
    void step();

    uint32_t getPC() const;
    uint32_t getRegister(uint32_t index) const;

    uint64_t getCycle() const;
    MultiCycleState getCurrentState() const;

private:

    Memory& memory;

    RegisterFile registers;
    PC pc;

    Decoder decoder;
    ALU alu;
    ControlUnit control_unit;
    ControlFSM fsm;

    Logger* logger;

    uint64_t cycle;

    uint32_t current_pc;

    Instruction current_instruction;

    // Multicycle datapath registers
    uint32_t IR;
    uint32_t MDR;

    uint32_t A;
    uint32_t B;

    uint32_t ALUOut;

    MultiCycleState current_state;

    // Handlers
    void handleFetch();
    void handleDecode();

    void handleExecuteR();
    void handleExecuteI();

    void handleWritebackR();
    void handleWritebackI();

    void handleAddressCalculation();

    void handleMemoryRead();
    void handleMemoryWrite();
    void handleWritebackLoad();

    void handleBranch();
    void handleJUMP();
    void handleJALR();

    void handleLUI();
    void handleAUIPC();

    void logTrace(
    MultiCycleState state,
    uint32_t trace_pc,
    uint32_t trace_instruction
);
};

#endif