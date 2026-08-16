#include "control_unit.h"

ControlSignals ControlUnit::generate(
    const Instruction& instruction) const
{
    ControlSignals signals;

    switch (instruction.opcode){
        case 0x33:{ //R-type: opcode is 0110011 ->0x33
            signals.reg_write = true;
            signals.alu_source = ALUSource::REGISTER;

            switch (instruction.funct3)
            {
                case 0x0:
                    if (instruction.funct7 == 0x00)
                        signals.alu_operation = ALUoperation::ADD;
                    else if (instruction.funct7 == 0x20)
                        signals.alu_operation = ALUoperation::SUB;
                    break;

                case 0x7:
                    signals.alu_operation = ALUoperation::AND;
                    break;

                case 0x6:
                    signals.alu_operation = ALUoperation::OR;
                    break;

                case 0x4:
                    signals.alu_operation = ALUoperation::XOR;
                    break;

                case 0x2:
                    signals.alu_operation = ALUoperation::SLT;
                    break;

                case 0x1:
                    signals.alu_operation = ALUoperation::SLL;
                    break;

                case 0x5:
                    if (instruction.funct7 == 0x00)
                        signals.alu_operation = ALUoperation::SRL;
                    else if (instruction.funct7 == 0x20)
                        signals.alu_operation = ALUoperation::SRA;
                    break;
            }

            break;
        }

        case 0x13:
        {
            signals.reg_write = true;
            signals.alu_source = ALUSource::IMMEDIATE;

            switch (instruction.funct3)
            {
                case 0x0:
                    signals.alu_operation = ALUoperation::ADD;
                    break;

                case 0x7:
                    signals.alu_operation = ALUoperation::AND;
                    break;

                case 0x6:
                    signals.alu_operation = ALUoperation::OR;
                    break;

                case 0x4:
                    signals.alu_operation = ALUoperation::XOR;
                    break;

                case 0x2:
                    signals.alu_operation = ALUoperation::SLT;
                    break;
            }

            break;
        }

        case 0x03:  // LW
        {
            signals.reg_write = true;
            signals.alu_source = ALUSource::IMMEDIATE;
            signals.alu_operation = ALUoperation::ADD;
            signals.mem_read = true;
            signals.mem_to_reg = true;
            break;
        }

        case 0x23:  // SW
        {
            signals.alu_source = ALUSource::IMMEDIATE;
            signals.alu_operation = ALUoperation::ADD;
            signals.mem_write = true;
            break;
        }

        case 0x63:  // Branch
        {
            signals.branch = true;
            signals.alu_source = ALUSource::REGISTER;
            signals.alu_operation = ALUoperation::SUB;
            break;
        }

        case 0x6F:  // JAL
        {
            signals.jump = true;
            signals.reg_write = true;
            break;
        }

        case 0x67:  // JALR
        {
            signals.jump = true;
            signals.reg_write = true;
            signals.alu_source = ALUSource::IMMEDIATE;
            signals.alu_operation = ALUoperation::ADD;
            break;
        }

        default:
            break;
    }

    return signals;
}