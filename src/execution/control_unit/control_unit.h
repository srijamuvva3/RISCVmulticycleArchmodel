#ifndef CONTROL_UNIT_H
#define CONTROL_UNIT_H

#include "../common/instruction.h"
#include "../common/control_signals.h"

class ControlUnit
{
public:
    ControlSignals generate(const Instruction& instruction) const;
};

#endif