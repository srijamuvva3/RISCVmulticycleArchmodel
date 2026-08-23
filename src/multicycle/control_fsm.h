#ifndef CONTROL_FSM_H
#define CONTROL_FSM_H

#include "state.h"
#include <cstdint>

class ControlFSM
{
public:

    MultiCycleState nextState(MultiCycleState current,uint32_t opcode) const;
};

#endif