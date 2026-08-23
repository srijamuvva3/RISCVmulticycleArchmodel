#ifndef TRACE_H
#define TRACE_H

#include <cstdint>
#include <string>

struct TraceEntry {
    uint64_t cycle =0;
    uint32_t pc =0;
    uint32_t instruction =0;

    std::string stage;

    uint32_t alu_result =0;

    bool reg_write = false;
    uint8_t rd = 0;
    uint32_t write_data = 0;

    bool mem_read = false;
    bool mem_write = false;
    uint32_t mem_address = 0;
    uint32_t mem_data = 0;

    uint32_t next_pc = 0;


};

#endif // TRACE_H