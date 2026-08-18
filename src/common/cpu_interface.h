#ifndef CPU_INTERFACE_H
#define CPU_INTERFACE_H

#include "cpu_state.h"

class ICPU{

    virtual ~ICPU() = default;
    virtual void reset() = 0;
    virtual void step() = 0;

    virtual CPUState getState() const = 0;

};

#endif // CPU_INTERFACE_H
