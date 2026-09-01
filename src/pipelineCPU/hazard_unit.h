#ifndef HAZARD_UNIT_H
#define HAZARD_UNIT_H

#include <cstdint>

#include "pipeline_registers.h"


enum class ForwardingSelect
{
    FROM_REGISTER,
    FROM_EX_MEM,
    FROM_MEM_WB
};


class HazardUnit
{
public:

    // --------------------------------------------------------
    // Load-use hazard
    // --------------------------------------------------------

    bool detectLoadUseHazard(
        const ID_EX_Register& id_ex,
        const IF_ID_Register& if_id
    ) const;


    // --------------------------------------------------------
    // Forwarding for EX-stage rs1
    // --------------------------------------------------------

    ForwardingSelect forwardingA(
        const ID_EX_Register& id_ex,
        const EX_MEM_Register& ex_mem,
        const MEM_WB_Register& mem_wb
    ) const;


    // --------------------------------------------------------
    // Forwarding for EX-stage rs2
    // --------------------------------------------------------

    ForwardingSelect forwardingB(
        const ID_EX_Register& id_ex,
        const EX_MEM_Register& ex_mem,
        const MEM_WB_Register& mem_wb
    ) const;


private:

    bool usesRS1(const Instruction& instruction) const;

    bool usesRS2(const Instruction& instruction) const;

    bool writesRegister(
        const ControlSignals& control,
        const Instruction& instruction
    ) const;
};

#endif