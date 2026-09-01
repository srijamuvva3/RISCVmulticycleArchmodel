#include "pipeline_cpu.h"

#include <iomanip>
#include <sstream>


// ============================================================
// CONSTRUCTOR
// ============================================================

PipelineCPU::PipelineCPU(
    Memory& memory
)
    : memory(memory),
      pipeline_trace(pipeline_trace),
      cycle(0),
      status(PipelineStatus::RUNNING)
{
    reset();
}

// ============================================================
// RESET
// ============================================================

void PipelineCPU::reset()
{
    pc.reset();

    registers.reset();

    resetIF_ID(if_id);
    resetID_EX(id_ex);
    resetEX_MEM(ex_mem);
    resetMEM_WB(mem_wb);

    cycle = 0;

    status = PipelineStatus::RUNNING;
}


// ============================================================
// GETTERS
// ============================================================

uint32_t PipelineCPU::getPC() const
{
    return pc.get();
}


uint32_t PipelineCPU::getRegister(
    uint32_t index
) const
{
    return registers.read(index);
}


uint64_t PipelineCPU::getCycle() const
{
    return cycle;
}


PipelineStatus PipelineCPU::getStatus() const
{
    return status;
}


// ============================================================
// CHECK VALID INSTRUCTION
// ============================================================

bool PipelineCPU::isValidInstruction(
    uint32_t raw
) const
{
    if (raw == 0)
        return false;

    Instruction instruction =
        decoder.decode(raw);

    return instruction.format !=
           InstructionFormat::UNKNOWN;
}


// ============================================================
// INSTRUCTION TYPE HELPERS
// ============================================================

bool PipelineCPU::isBranch(
    const Instruction& instruction
) const
{
    return instruction.opcode == 0x63;
}


bool PipelineCPU::isJAL(
    const Instruction& instruction
) const
{
    return instruction.opcode == 0x6F;
}


bool PipelineCPU::isJALR(
    const Instruction& instruction
) const
{
    return instruction.opcode == 0x67;
}


bool PipelineCPU::isControlTransfer(
    const Instruction& instruction
) const
{
    return isBranch(instruction) ||
           isJAL(instruction) ||
           isJALR(instruction);
}


// ============================================================
// MEM/WB WRITEBACK VALUE
// ============================================================

uint32_t PipelineCPU::getMEMWBWritebackValue() const
{
    if (mem_wb.control.mem_to_reg)
    {
        return mem_wb.memory_data;
    }

    return mem_wb.alu_result;
}


// ============================================================
// FORWARDING VALUE
// ============================================================

uint32_t PipelineCPU::getForwardedValue(
    ForwardingSelect select,
    uint32_t register_value
) const
{
    switch (select)
    {
        case ForwardingSelect::FROM_EX_MEM:

            return ex_mem.writeback_value;


        case ForwardingSelect::FROM_MEM_WB:

            return getMEMWBWritebackValue();


        case ForwardingSelect::FROM_REGISTER:

        default:

            return register_value;
    }
}


// ============================================================
// BRANCH EVALUATION
// ============================================================

bool PipelineCPU::evaluateBranch(
    const Instruction& instruction,
    uint32_t a,
    uint32_t b
) const
{
    switch (instruction.funct3)
    {
        // BEQ
        case 0x0:
            return a == b;

        // BNE
        case 0x1:
            return a != b;

        // BLT
        case 0x4:
            return static_cast<int32_t>(a) <
                   static_cast<int32_t>(b);

        // BGE
        case 0x5:
            return static_cast<int32_t>(a) >=
                   static_cast<int32_t>(b);

        // BLTU
        case 0x6:
            return a < b;

        // BGEU
        case 0x7:
            return a >= b;

        default:
            return false;
    }
}


// ============================================================
// WRITEBACK STAGE
// ============================================================

void PipelineCPU::writeBackStage()
{
    if (!mem_wb.valid)
        return;


    if (!mem_wb.control.reg_write)
        return;


    if (mem_wb.rd == 0)
        return;


    uint32_t value;


    if (mem_wb.control.mem_to_reg)
    {
        value = mem_wb.memory_data;
    }
    else
    {
        value = mem_wb.alu_result;
    }


    registers.write(
        mem_wb.rd,
        value
    );
}


// ============================================================
// MEMORY STAGE
// ============================================================

void PipelineCPU::memoryStage(
    MEM_WB_Register& next_mem_wb
)
{
    resetMEM_WB(next_mem_wb);


    if (!ex_mem.valid)
        return;


    next_mem_wb.valid = true;

    next_mem_wb.pc =
        ex_mem.pc;

    next_mem_wb.instruction =
        ex_mem.instruction;

    next_mem_wb.alu_result =
        ex_mem.alu_result;

    next_mem_wb.rd =
        ex_mem.rd;

    next_mem_wb.control =
        ex_mem.control;

    next_mem_wb.writeback_value =
        ex_mem.writeback_value;


    // --------------------------------------------------------
    // LOAD
    // --------------------------------------------------------

    if (ex_mem.control.mem_read)
    {
        next_mem_wb.memory_data =
            memory.read32(
                ex_mem.alu_result
            );
    }


    // --------------------------------------------------------
    // STORE
    // --------------------------------------------------------

    if (ex_mem.control.mem_write)
    {
        memory.write32(
            ex_mem.alu_result,
            ex_mem.rs2_value
        );
    }
}


// ============================================================
// EXECUTE STAGE
// ============================================================

void PipelineCPU::executeStage(
    EX_MEM_Register& next_ex_mem,
    bool& control_transfer,
    uint32_t& new_pc
)
{
    resetEX_MEM(next_ex_mem);

    control_transfer = false;
    new_pc = pc.get();


    if (!id_ex.valid)
        return;


    next_ex_mem.valid = true;

    next_ex_mem.pc =
        id_ex.pc;

    next_ex_mem.instruction =
        id_ex.instruction;

    next_ex_mem.rd =
        id_ex.rd;

    next_ex_mem.immediate =
        id_ex.immediate;

    next_ex_mem.control =
        id_ex.control;


    // --------------------------------------------------------
    // FORWARDING
    // --------------------------------------------------------

    ForwardingSelect forwardA =
        hazard_unit.forwardingA(
            id_ex,
            ex_mem,
            mem_wb
        );


    ForwardingSelect forwardB =
        hazard_unit.forwardingB(
            id_ex,
            ex_mem,
            mem_wb
        );


    uint32_t operandA =
        getForwardedValue(
            forwardA,
            id_ex.rs1_value
        );


    uint32_t operandB =
        getForwardedValue(
            forwardB,
            id_ex.rs2_value
        );


    // Keep forwarded rs2 for stores.
    next_ex_mem.rs2_value =
        operandB;


    // --------------------------------------------------------
    // JAL
    // --------------------------------------------------------

    if (isJAL(id_ex.instruction))
    {
        next_ex_mem.writeback_value =
            id_ex.pc + 4;

        next_ex_mem.alu_result =
            id_ex.pc +
            static_cast<uint32_t>(
                id_ex.immediate
            );

        control_transfer = true;

        new_pc =
            id_ex.pc +
            static_cast<uint32_t>(
                id_ex.immediate
            );

        return;
    }


    // --------------------------------------------------------
    // JALR
    // --------------------------------------------------------

    if (isJALR(id_ex.instruction))
    {
        next_ex_mem.writeback_value =
            id_ex.pc + 4;

        uint32_t target =
            operandA +
            static_cast<uint32_t>(
                id_ex.immediate
            );

        target &= ~1u;

        next_ex_mem.alu_result =
            target;

        control_transfer = true;

        new_pc = target;

        return;
    }


    // --------------------------------------------------------
    // BRANCH
    // --------------------------------------------------------

    if (isBranch(id_ex.instruction))
    {
        bool taken =
            evaluateBranch(
                id_ex.instruction,
                operandA,
                operandB
            );


        uint32_t target =
            id_ex.pc +
            static_cast<uint32_t>(
                id_ex.immediate
            );


        next_ex_mem.branch_taken =
            taken;

        next_ex_mem.branch_target =
            target;


        if (taken)
        {
            control_transfer = true;

            new_pc = target;
        }

        return;
    }


    // --------------------------------------------------------
    // LUI
    // --------------------------------------------------------

    if (id_ex.instruction.opcode == 0x37)
    {
        next_ex_mem.alu_result =
            static_cast<uint32_t>(
                id_ex.immediate
            );

        return;
    }


    // --------------------------------------------------------
    // AUIPC
    // --------------------------------------------------------

    if (id_ex.instruction.opcode == 0x17)
    {
        next_ex_mem.alu_result =
            id_ex.pc +
            static_cast<uint32_t>(
                id_ex.immediate
            );

        return;
    }


    // --------------------------------------------------------
    // NORMAL ALU OPERATION
    // --------------------------------------------------------

    uint32_t aluOperandB;


    if (id_ex.control.alu_source ==
        ALUSource::IMMEDIATE)
    {
        aluOperandB =
            static_cast<uint32_t>(
                id_ex.immediate
            );
    }
    else
    {
        aluOperandB =
            operandB;
    }


    next_ex_mem.alu_result =
        alu.execute(
            operandA,
            aluOperandB,
            id_ex.control.alu_operation
        );


    next_ex_mem.writeback_value =
        next_ex_mem.alu_result;
}


// ============================================================
// DECODE STAGE
// ============================================================

void PipelineCPU::decodeStage(
    ID_EX_Register& next_id_ex
)
{
    resetID_EX(next_id_ex);


    if (!if_id.valid)
        return;


    Instruction instruction =
        decoder.decode(
            if_id.instruction
        );


    next_id_ex.valid = true;

    next_id_ex.pc =
        if_id.pc;

    next_id_ex.instruction =
        instruction;

    next_id_ex.rs1 =
        instruction.rs1;

    next_id_ex.rs2 =
        instruction.rs2;

    next_id_ex.rd =
        instruction.rd;

    next_id_ex.immediate =
        instruction.immediate;

    next_id_ex.rs1_value =
        registers.read(
            instruction.rs1
        );

    next_id_ex.rs2_value =
        registers.read(
            instruction.rs2
        );

    next_id_ex.pc_value =
        if_id.pc;


    next_id_ex.control =
        control_unit.generate(
            instruction
        );
}


// ============================================================
// FETCH STAGE
// ============================================================

void PipelineCPU::fetchStage(
    IF_ID_Register& next_if_id
)
{
    resetIF_ID(next_if_id);


    uint32_t fetchPC =
        pc.get();


    uint32_t raw =
        memory.read32(fetchPC);


    if (!isValidInstruction(raw))
    {
        return;
    }


    next_if_id.valid = true;

    next_if_id.pc =
        fetchPC;

    next_if_id.instruction =
        raw;


    pc.set(
        fetchPC + 4
    );
}


// ============================================================
// TRACE
// ============================================================



// ============================================================
// MAIN PIPELINE STEP
// ============================================================

void PipelineCPU::step()
{
    if (status == PipelineStatus::HALTED)
        return;


    // ========================================================
    // 1. WRITEBACK
    // ========================================================

    writeBackStage();


    // ========================================================
    // 2. PREPARE NEXT PIPELINE REGISTERS
    // ========================================================

    MEM_WB_Register next_mem_wb;

    EX_MEM_Register next_ex_mem;

    ID_EX_Register next_id_ex;

    IF_ID_Register next_if_id;


    // ========================================================
    // 3. MEMORY
    // ========================================================

    memoryStage(
        next_mem_wb
    );


    // ========================================================
    // 4. EXECUTE
    // ========================================================

    bool control_transfer =
        false;

    uint32_t new_pc =
        pc.get();


    executeStage(
        next_ex_mem,
        control_transfer,
        new_pc
    );


    // ========================================================
    // 5. HAZARD DETECTION
    // ========================================================

    bool loadUseHazard =
        hazard_unit.detectLoadUseHazard(
            id_ex,
            if_id
        );


    // ========================================================
    // 6. DECODE
    // ========================================================

    if (!loadUseHazard &&
        !control_transfer)
    {
        decodeStage(
            next_id_ex
        );
    }


    // ========================================================
    // 7. FETCH
    // ========================================================

    if (!loadUseHazard &&
        !control_transfer)
    {
        fetchStage(
            next_if_id
        );
    }


    // ========================================================
    // 8. LOAD-USE STALL
    // ========================================================

    if (loadUseHazard)
    {
        // Keep IF/ID instruction.
        next_if_id =
            if_id;


        // Insert bubble into ID/EX.
        resetID_EX(
            next_id_ex
        );
    }


    // ========================================================
    // 9. BRANCH/JUMP FLUSH
    // ========================================================

    if (control_transfer)
    {
        // The instruction currently in ID
        // is wrong-path.
        resetID_EX(
            next_id_ex
        );


        // The instruction currently in IF
        // is also wrong-path.
        resetIF_ID(
            next_if_id
        );


        pc.set(
            new_pc
        );
    }


    // ========================================================
    // 10. COMMIT PIPELINE REGISTERS
    // ========================================================

    mem_wb =
        next_mem_wb;

    ex_mem =
        next_ex_mem;

    id_ex =
        next_id_ex;

    if_id =
        next_if_id;


    // ========================================================
    // 11. HALT WHEN PIPELINE DRAINS
    // ========================================================

    if (!if_id.valid &&
        !id_ex.valid &&
        !ex_mem.valid &&
        !mem_wb.valid)
    {
        uint32_t raw =
            memory.read32(
                pc.get()
            );


        if (!isValidInstruction(raw))
        {
            status =
                PipelineStatus::HALTED;
        }
    }


    cycle++;
}
void PipelineCPU::logPipelineCycle()
{
    if (pipeline_trace == nullptr)
        return;

    std::string if_id_state = "-";
    std::string id_ex_state = "-";
    std::string ex_mem_state = "-";
    std::string mem_wb_state = "-";

    if (if_id.valid)
        if_id_state = "VALID";

    if (id_ex.valid)
        id_ex_state = "VALID";

    if (ex_mem.valid)
        ex_mem_state = "VALID";

    if (mem_wb.valid)
        mem_wb_state = "VALID";


    uint32_t instruction = 0;
    uint32_t trace_pc = pc.get();

    uint32_t rs1 = 0;
    uint32_t rs1_value = 0;

    uint32_t rs2 = 0;
    uint32_t rs2_value = 0;

    int32_t immediate = 0;

    uint32_t alu_result = 0;


    // --------------------------------------------------------
    // Prefer the instruction currently in ID/EX
    // because this gives us decoded operands/immediate.
    // --------------------------------------------------------

    if (id_ex.valid)
    {
        instruction =
            id_ex.instruction.raw;

        trace_pc =
            id_ex.pc;

        rs1 =
            id_ex.rs1;

        rs1_value =
            id_ex.rs1_value;

        rs2 =
            id_ex.rs2;

        rs2_value =
            id_ex.rs2_value;

        immediate =
            id_ex.immediate;
    }
    else if (if_id.valid)
    {
        instruction =
            if_id.instruction;

        trace_pc =
            if_id.pc;
    }
    else if (ex_mem.valid)
    {
        instruction =
            ex_mem.instruction.raw;

        trace_pc =
            ex_mem.pc;

        alu_result =
            ex_mem.alu_result;
    }
    else if (mem_wb.valid)
    {
        instruction =
            mem_wb.instruction.raw;

        trace_pc =
            mem_wb.pc;

        alu_result =
            mem_wb.alu_result;
    }


    // --------------------------------------------------------
    // ALU result
    // --------------------------------------------------------

    if (ex_mem.valid)
    {
        alu_result =
            ex_mem.alu_result;
    }


    // --------------------------------------------------------
    // Activity
    // --------------------------------------------------------

    std::ostringstream activity;

    if (if_id.valid)
        activity << "IF ";

    if (id_ex.valid)
        activity << "ID ";

    if (ex_mem.valid)
        activity << "EX ";

    if (mem_wb.valid)
        activity << "WB ";


    if (mem_wb.valid &&
        mem_wb.control.reg_write)
    {
        activity
            << "| WB: x"
            << mem_wb.rd
            << " <- 0x"
            << std::hex
            << mem_wb.alu_result;
    }


    pipeline_trace->logCycle(
        cycle,
        trace_pc,
        instruction,

        if_id_state,
        id_ex_state,
        ex_mem_state,
        mem_wb_state,

        rs1,
        rs1_value,

        rs2,
        rs2_value,

        immediate,

        alu_result,

        activity.str()
    );
}