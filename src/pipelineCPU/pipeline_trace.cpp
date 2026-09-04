#include "pipeline_trace.h"

#include <iomanip>

PipelineTrace::PipelineTrace()
{
}

PipelineTrace::~PipelineTrace()
{
    close();
}


// ============================================================
// OPEN
// ============================================================

void PipelineTrace::open(const std::string& filename)
{
    close();

    file.open(
        filename,
        std::ios::out | std::ios::trunc
    );

    if (file.is_open())
    {
        writeHeader();
    }
}


// ============================================================
// CLOSE
// ============================================================

void PipelineTrace::close()
{
    if (file.is_open())
    {
        file.close();
    }
}


// ============================================================
// IS OPEN
// ============================================================

bool PipelineTrace::isOpen() const
{
    return file.is_open();
}


// ============================================================
// HEADER
// ============================================================

void PipelineTrace::writeHeader()
{
    file
        << "===============================================================================================\n";

    file
        << "                              RISC-V 5-STAGE PIPELINE TRACE\n";

    file
        << "===============================================================================================\n\n";

    file
        << std::left
        << std::setw(7)  << "Cycle"
        << "| "
        << std::setw(12) << "PC"
        << "| "
        << std::setw(20) << "IF"
        << "| "
        << std::setw(20) << "ID"
        << "| "
        << std::setw(20) << "EX"
        << "| "
        << std::setw(20) << "MEM"
        << "| "
        << std::setw(20) << "WB"
        << "\n";

    file
        << "-------+--------------+----------------------+----------------------+----------------------+----------------------+----------------------\n";
}


// ============================================================
// LOG ONE PIPELINE CYCLE
// ============================================================

void PipelineTrace::logCycle(
    uint64_t cycle,
    uint32_t pc,
    uint32_t instruction,

    const std::string& if_stage,
    const std::string& id_stage,
    const std::string& ex_stage,
    const std::string& mem_stage,
    const std::string& wb_stage,

    uint32_t rs1,
    uint32_t rs1_value,
    uint32_t rs2,
    uint32_t rs2_value,

    int32_t immediate,

    uint32_t alu_result,

    const std::string& activity
)
{
    if (!file.is_open())
    {
        return;
    }

    // --------------------------------------------------------
    // Main pipeline table
    // --------------------------------------------------------

    file
        << std::left
        << std::dec
        << std::setw(7)
        << cycle
        << "| ";

    // --------------------------------------------------------
    // PC
    // --------------------------------------------------------

    file
        << "0x"
        << std::right
        << std::setfill('0')
        << std::setw(8)
        << std::hex
        << pc
        << std::setfill(' ')
        << "    | ";

    // --------------------------------------------------------
    // IF
    // --------------------------------------------------------

    file
        << std::left
        << std::setw(20)
        << if_stage
        << "| ";

    // --------------------------------------------------------
    // ID
    // --------------------------------------------------------

    file
        << std::setw(20)
        << id_stage
        << "| ";

    // --------------------------------------------------------
    // EX
    // --------------------------------------------------------

    file
        << std::setw(20)
        << ex_stage
        << "| ";

    // --------------------------------------------------------
    // MEM
    // --------------------------------------------------------

    file
        << std::setw(20)
        << mem_stage
        << "| ";

    // --------------------------------------------------------
    // WB
    // --------------------------------------------------------

    file
        << std::setw(20)
        << wb_stage
        << "\n";


    // --------------------------------------------------------
    // No additional debug information here.
    //
    // This file is intentionally a clean visualization of
    // instruction movement through the five pipeline stages.
    // --------------------------------------------------------

    file.flush();
}