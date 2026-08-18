#ifndef CPU_STATE_H
#define CPU_STATE_H

#include "types.h"
#include <cstdint>

struct CPUState {
    Word pc;
    Word registers[32];
    Cycle cycle;
};

#endif // CPU_STATE_H