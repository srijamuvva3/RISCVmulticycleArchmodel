#ifndef CPU_H
#define CPU_H

#include <cstdint>

#include "alu/alu.h"
#include "control_unit/control_unit.h"
#include "decoder/decoder.h"
#include "register_file/register_file.h"
#include "register_file/pc.h"

#include "../memory/memory.h"

class CPU
{
private:
    PC pc;
    RegisterFile registers;
    ALU alu;
    Decoder decoder;
    ControlUnit control_unit;
    Memory& memory;

public:
    explicit CPU(Memory& memory);

    void reset();

    void step();

    uint32_t getPC() const;

    uint32_t getRegister(uint8_t index) const;
};

#endif