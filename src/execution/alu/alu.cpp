#include "alu.h"
#include <stdexcept>

uint32_t ALU::execute(uint32_t a, uint32_t b, ALUoperation operation) const{
    switch (operation){
        case ALUoperation::ADD:
            return a + b;
        case ALUoperation::SUB:
            return a - b;
        case ALUoperation::MUL:
            return a * b;
        case ALUoperation::DIV:
            if (b == 0) {
                throw std::invalid_argument("Division by zero");
            }
            return a / b;
        case ALUoperation::AND:
            return a & b;
        case ALUoperation::OR:
            return a | b;
        case ALUoperation::XOR:
            return a ^ b;
        case ALUoperation::SLL:
            return a << (b & 0x1F); // RISC V is a 32-bit arch, so we mask the shift amount to 5 bits
        case ALUoperation::SRL:
            return a >> (b & 0x1F); // RISC V is a 32-bit arch, so we mask the shift amount to 5 bits
        case ALUoperation::SRA:
            return static_cast<uint32_t> (static_cast<int32_t>(a) >> (b & 0x1F));
        case ALUoperation::SLT:
            return (static_cast<int32_t>(a) < static_cast<int32_t>(b)) ? 1 : 0;
        default:
            throw std::invalid_argument("Invalid ALU operation");
    }
}
