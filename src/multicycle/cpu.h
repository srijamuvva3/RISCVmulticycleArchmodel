#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include "control_fsm.h"
#include "state.h"

#include "../execution/alu/alu.h"
#include "../execution/control_unit/control_unit.h"
#include "../execution/register_file/register_file.h"
#include "../execution/register_file/pc.h"

#include "../memory/memory.h"
#include "../ISA/decoder.h"

#include "../trace/trace.h"
#include "../trace/logger.h"

class MultiCycleCPU {
    private:
        PC pc;
        RegisterFile registers;
        ALU alu;
        Decoder decoder;
        ControlUnit control_unit;
        Memory& memory;

        Logger* logger; // Pointer to Logger
        uint64_t cycle;
        ControlFSM fsm;

        MultiCycleState current_state;
        //Multicycle temporary registers
        uint32_t IR; // Instruction Register
        uint32_t MDR; // Memory Data Register
        uint32_t A; // Register rs1 value
        uint32_t B; // Register rs2 value
        uint32_t ALUOut; // ALU Output Register
        uint32_t current_pc; // Current Program Counter
        Instruction current_instruction; // Decoded instruction
         // -----------------------------
    // State handlers
    // -----------------------------

        void handleFetch();
        void handleDecode();

        void handleExecuteR();
        void handleExecuteI();

        void handleAddressCalculation();

        void handleMemoryRead();
        void handleMemoryWrite();

        void handleWritebackR();
        void handleWritebackI();
        void handleWritebackLoad();

        void handleBranch();

        void handleJUMP();
        void handleJALR();

        void handleLUI();
        void handleAUIPC();

        void logTrace();

    public:
        explicit MultiCycleCPU(Memory& memory, Logger* logger = nullptr);
        void reset();
        void step();
        uint32_t getPC() const;
        uint32_t getRegister(uint32_t index) const;
        uint64_t getCycle() const;
        MultiCycleState getCurrentState() const;
};

#endif // CPU_H