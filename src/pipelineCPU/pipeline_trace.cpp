#include "pipeline_trace.h"

#include <iomanip>

PipelineTrace::PipelineTrace()
{
}

PipelineTrace::~PipelineTrace()
{
    close();
}

void PipelineTrace::open(const std::string& filename)
{
    close();

    file.open(filename, std::ios::out | std::ios::trunc);

    if (file.is_open())
    {
        writeHeader();
    }
}

void PipelineTrace::close()
{
    if (file.is_open())
    {
        file.close();
    }
}

bool PipelineTrace::isOpen() const
{
    return file.is_open();
}

void PipelineTrace::writeHeader()
{
    file << "========================================================================================================================\n";
    file << "                                      RISC-V PIPELINE TRACE\n";
    file << "========================================================================================================================\n";

    file
        << "Cycle  | PC           | Instruction  "
        << "| IF/ID       | ID/EX       | EX/MEM      | MEM/WB      "
        << "| rs1  | rs1_val      "
        << "| rs2  | rs2_val      "
        << "| Immediate    "
        << "| ALU_result   "
        << "| Activity\n";

    file << "------------------------------------------------------------------------------------------------------------------------\n";
}

void PipelineTrace::logCycle(
    uint64_t cycle,
    uint32_t pc,
    uint32_t instruction,

    const std::string& if_id_state,
    const std::string& id_ex_state,
    const std::string& ex_mem_state,
    const std::string& mem_wb_state,

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

    file
        << std::left
        << std::setw(6) << cycle
        << " | "
        << "0x" << std::right << std::setfill('0')
        << std::setw(8) << std::hex << pc
        << std::setfill(' ')
        << "   | "
        << "0x" << std::right << std::setfill('0')
        << std::setw(8) << std::hex << instruction
        << std::setfill(' ')
        << " | "
        << std::left << std::setw(11) << if_id_state
        << " | "
        << std::setw(11) << id_ex_state
        << " | "
        << std::setw(11) << ex_mem_state
        << " | "
        << std::setw(11) << mem_wb_state
        << " | "
        << "x" << std::dec << std::setw(3) << rs1
        << " | "
        << "0x" << std::right << std::setfill('0')
        << std::setw(8) << std::hex << rs1_value
        << std::setfill(' ')
        << "   | "
        << "x" << std::dec << std::setw(3) << rs2
        << " | "
        << "0x" << std::right << std::setfill('0')
        << std::setw(8) << std::hex << rs2_value
        << std::setfill(' ')
        << "   | "
        << std::dec << std::setw(11) << immediate
        << " | "
        << "0x" << std::right << std::setfill('0')
        << std::setw(8) << std::hex << alu_result
        << std::setfill(' ')
        << " | "
        << activity
        << "\n";

    file.flush();
}