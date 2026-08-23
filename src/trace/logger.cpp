#include "logger.h"

#include <iomanip>
#include <iostream>

std::ofstream file;

Logger::Logger(const std::string& filename) {
    open(filename);
}

Logger::~Logger() {
    close();
}

void Logger::open(const std::string& filename) {
    file.open(filename);
    if(!file.is_open()) {
        std::cerr << "Error: Could not open log file: " << filename << std::endl;
    } else {
        logHeader();
    }
}

void Logger::logHeader(){
    if(!file.is_open()) {
        std::cerr << "Error: Log file is not open." << std::endl;
        return;
    }
   file << "Cycle,PC,Instruction,Stage,"
         << "ALUResult,RegWrite,RD,WriteData,"
         << "MemRead,MemWrite,MemAddress,MemData,NextPC\n";
}

void Logger::log(const TraceEntry& entry) {
    if(!file.is_open()) {
        std::cerr << "Error: Log file is not open." << std::endl;
        return;
    }
    file << entry.cycle << ","
         << "0x" << std::hex << entry.pc << ","
         << "0x" << entry.instruction << ","
         << entry.stage << ","
         << "0x" << entry.alu_result << ","
         << std::dec << entry.reg_write << ","
         << static_cast<int>(entry.rd) << ","
         << "0x" << std::hex << entry.write_data << ","
         << std::dec << entry.mem_read << ","
         << entry.mem_write << ","
         << "0x" << std::hex << entry.mem_address << ","
         << "0x" << entry.mem_data << ","
         << "0x" << entry.next_pc
         << "\n";
}