#pragma once

#include <cstdint>
#include <string>
#include <fstream>

class PipelineTrace
{
public:
    PipelineTrace();
    ~PipelineTrace();

    // Open/create the trace file and write the header
    void open(const std::string& filename = "pipeline_trace.log");

    // Close the trace file
    void close();

    // Write one pipeline-cycle entry
    void logCycle(
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
    );

    // Check whether trace file is open
    bool isOpen() const;

private:
    std::ofstream file;

    void writeHeader();
};