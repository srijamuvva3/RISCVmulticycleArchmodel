#ifndef ALU_H
#define ALU_H

#include <cstdint>

#include "../../common/control_signals.h"

class ALU
{
public:

    uint32_t execute(
        uint32_t a,
        uint32_t b,
        ALUoperation operation
    ) const;
};

#endif