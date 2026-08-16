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
            return a << b;
        case ALUoperation::SRL:
            return a >> b;
        case ALUoperation::SRA:
            return static_cast<int32_t>(a) >> b;
        case ALUoperation::SLT:
            return (static_cast<int32_t>(a) < static_cast<int32_t>(b)) ? 1 : 0;
        default:
            throw std::invalid_argument("Invalid ALU operation");
    }
}
