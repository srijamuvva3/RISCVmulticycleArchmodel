#include "hazard_unit.h"


// ============================================================
// Does instruction use rs1?
// ============================================================

bool HazardUnit::usesRS1(
    const Instruction& instruction
) const
{
    switch (instruction.opcode)
    {
        // R-type
        case 0x33:

        // I-type ALU
        case 0x13:

        // Load
        case 0x03:

        // Store
        case 0x23:

        // Branch
        case 0x63:

        // JALR
        case 0x67:
            return true;

        default:
            return false;
    }
}


// ============================================================
// Does instruction use rs2?
// ============================================================

bool HazardUnit::usesRS2(
    const Instruction& instruction
) const
{
    switch (instruction.opcode)
    {
        // R-type
        case 0x33:

        // Store
        case 0x23:

        // Branch
        case 0x63:
            return true;

        default:
            return false;
    }
}


// ============================================================
// Does this instruction write a register?
// ============================================================

bool HazardUnit::writesRegister(
    const ControlSignals& control,
    const Instruction& instruction
) const
{
    if (!control.reg_write)
        return false;

    if (instruction.rd == 0)
        return false;

    return true;
}


// ============================================================
// LOAD-USE HAZARD
// ============================================================
//
// Example:
//
//     LW   x1, 0(x2)
//     ADD  x3, x1, x4
//
// ADD needs x1 before LW has reached WB.
//
// Therefore:
//
//     stall IF
//     stall ID
//     insert bubble into EX
//
// ============================================================

bool HazardUnit::detectLoadUseHazard(
    const ID_EX_Register& id_ex,
    const IF_ID_Register& if_id
) const
{
    if (!id_ex.valid)
        return false;

    if (!id_ex.control.mem_read)
        return false;

    if (id_ex.rd == 0)
        return false;

    if (!if_id.valid)
        return false;

    const uint32_t raw_instruction = if_id.instruction;
    const uint32_t opcode = raw_instruction & 0x7Fu;
    const uint32_t rs1 = (raw_instruction >> 15) & 0x1Fu;
    const uint32_t rs2 = (raw_instruction >> 20) & 0x1Fu;

    const bool uses_rs1 =
        opcode == 0x33 ||
        opcode == 0x13 ||
        opcode == 0x03 ||
        opcode == 0x23 ||
        opcode == 0x63 ||
        opcode == 0x67;

    const bool uses_rs2 =
        opcode == 0x33 ||
        opcode == 0x23 ||
        opcode == 0x63;

    const bool rs1_hazard = uses_rs1 && (rs1 == id_ex.rd);
    const bool rs2_hazard = uses_rs2 && (rs2 == id_ex.rd);

    return rs1_hazard || rs2_hazard;
}


// ============================================================
// FORWARDING A
// ============================================================

ForwardingSelect HazardUnit::forwardingA(
    const ID_EX_Register& id_ex,
    const EX_MEM_Register& ex_mem,
    const MEM_WB_Register& mem_wb
) const
{
    if (!id_ex.valid)
        return ForwardingSelect::FROM_REGISTER;


    // EX/MEM forwarding has priority.
    if (ex_mem.valid &&
        ex_mem.control.reg_write &&
        ex_mem.rd != 0 &&
        ex_mem.rd == id_ex.rs1)
    {
        // Do not forward a load result from EX/MEM.
        if (!ex_mem.control.mem_read)
        {
            return ForwardingSelect::FROM_EX_MEM;
        }
    }


    // MEM/WB forwarding
    if (mem_wb.valid &&
        mem_wb.control.reg_write &&
        mem_wb.rd != 0 &&
        mem_wb.rd == id_ex.rs1)
    {
        return ForwardingSelect::FROM_MEM_WB;
    }


    return ForwardingSelect::FROM_REGISTER;
}


// ============================================================
// FORWARDING B
// ============================================================

ForwardingSelect HazardUnit::forwardingB(
    const ID_EX_Register& id_ex,
    const EX_MEM_Register& ex_mem,
    const MEM_WB_Register& mem_wb
) const
{
    if (!id_ex.valid)
        return ForwardingSelect::FROM_REGISTER;


    // EX/MEM forwarding
    if (ex_mem.valid &&
        ex_mem.control.reg_write &&
        ex_mem.rd != 0 &&
        ex_mem.rd == id_ex.rs2)
    {
        if (!ex_mem.control.mem_read)
        {
            return ForwardingSelect::FROM_EX_MEM;
        }
    }


    // MEM/WB forwarding
    if (mem_wb.valid &&
        mem_wb.control.reg_write &&
        mem_wb.rd != 0 &&
        mem_wb.rd == id_ex.rs2)
    {
        return ForwardingSelect::FROM_MEM_WB;
    }


    return ForwardingSelect::FROM_REGISTER;
}