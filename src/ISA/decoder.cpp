#include "decoder.h"

int32_t Decoder::decodeImmediate(uint32_t raw, InstructionFormat format) const{
    switch (format)
    {
        case InstructionFormat::I: {
            int32_t immediate = (raw >> 20) & 0xFFF;

            // Sign extend 12-bit immediate
            if (immediate & 0x800)
            {
                immediate |= 0xFFFFF000;
            }

            return immediate;
        }

        case InstructionFormat::S: {
            int32_t imm11_5 = (raw >> 25) & 0x7F;
            int32_t imm4_0  = (raw >> 7) & 0x1F;

            int32_t immediate =
                (imm11_5 << 5) | imm4_0;

            if (immediate & 0x800)
            {
                immediate |= 0xFFFFF000;
            }

            return immediate;
        }

        case InstructionFormat::B: {
            int32_t imm12   = (raw >> 31) & 0x1;
            int32_t imm11   = (raw >> 7) & 0x1;
            int32_t imm10_5 = (raw >> 25) & 0x3F;
            int32_t imm4_1  = (raw >> 8) & 0xF;

            int32_t immediate =
                (imm12 << 12) |
                (imm11 << 11) |
                (imm10_5 << 5) |
                (imm4_1 << 1);

            if (immediate & 0x1000){
                immediate |= 0xFFFFE000;
            }

            return immediate;
        }

        case InstructionFormat::U:  {
            return static_cast<int32_t>(raw & 0xFFFFF000);
        }

        case InstructionFormat::J: {
            int32_t imm20    = (raw >> 31) & 0x1;
            int32_t imm10_1  = (raw >> 21) & 0x3FF;
            int32_t imm11    = (raw >> 20) & 0x1;
            int32_t imm19_12 = (raw >> 12) & 0xFF;

            int32_t immediate =
                (imm20 << 20) |
                (imm19_12 << 12) |
                (imm11 << 11) |
                (imm10_1 << 1);

            if (immediate & 0x100000)
            {
                immediate |= 0xFFE00000;
            }

            return immediate;
        }

        case InstructionFormat::R:
        case InstructionFormat::UNKNOWN:
        default:
            return 0;
    }
}
Instruction Decoder::decode(uint32_t raw_instruction) const
{
    Instruction instruction{};

    instruction.raw = raw_instruction;

    // Common fields
    instruction.opcode = raw_instruction & 0x7F;
    instruction.rd     = (raw_instruction >> 7)  & 0x1F;
    instruction.funct3 = (raw_instruction >> 12) & 0x07;
    instruction.rs1    = (raw_instruction >> 15) & 0x1F;
    instruction.rs2    = (raw_instruction >> 20) & 0x1F;
    instruction.funct7 = (raw_instruction >> 25) & 0x7F;

    // Determine instruction format
    switch (instruction.opcode)
    {
        // R-type instructions
        case 0x33:
            instruction.format = InstructionFormat::R;
            break;

        // I-type ALU instructions
        // ADDI, SLTI, XORI, ORI, ANDI,
        // SLLI, SRLI, SRAI
        case 0x13:
            instruction.format = InstructionFormat::I;
            break;

        // I-type load instructions
        // LB, LH, LW, LBU, LHU
        case 0x03:
            instruction.format = InstructionFormat::I;
            break;

        // I-type JALR
        case 0x67:
            instruction.format = InstructionFormat::I;
            break;

        // S-type store instructions
        // SB, SH, SW
        case 0x23:
            instruction.format = InstructionFormat::S;
            break;

        // B-type branch instructions
        // BEQ, BNE, BLT, BGE, BLTU, BGEU
        case 0x63:
            instruction.format = InstructionFormat::B;
            break;

        // U-type LUI
        case 0x37:
            instruction.format = InstructionFormat::U;
            break;

        // U-type AUIPC
        case 0x17:
            instruction.format = InstructionFormat::U;
            break;

        // J-type JAL
        case 0x6F:
            instruction.format = InstructionFormat::J;
            break;

        default:
            instruction.format = InstructionFormat::UNKNOWN;
            break;
    }

    // Generate immediate according to instruction format
    instruction.immediate =
        decodeImmediate(raw_instruction, instruction.format);

    return instruction;
}