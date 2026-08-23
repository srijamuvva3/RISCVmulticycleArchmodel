#ifndef CPU_H
#define CPU_H

#include <cstdint>

#include "../execution/alu/alu.h"
#include "../execution/control_unit/control_unit.h"
#include "../ISA/decoder.h"
#include "../execution/register_file/register_file.h"
#include "../execution/register_file/pc.h"

#include "../memory/memory.h"
#include "trace/trace.h"
#include "trace/logger.h"

class SingleCycleCPU
{
private:
    PC pc;
    RegisterFile registers;
    ALU alu;
    Decoder decoder;
    ControlUnit control_unit;
    Memory& memory;

    Logger* logger; // Pointer to Logger
    uint64_t cycle;

public:
    SingleCycleCPU(Memory& memory, Logger* logger = nullptr);

    void reset();

    void step();

    uint32_t getPC() const;

    uint32_t getRegister(uint8_t index) const;
    
    uint64_t getCycle() const;
};

#endif