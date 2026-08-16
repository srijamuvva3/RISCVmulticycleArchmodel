#include "cpu.h"

CPU::CPU(Memory& memory) : memory(memory){
    reset();
}

void CPU::reset(){
    pc.reset();
    registers.reset();
}

uint32_t CPU::getPC() const{
    return pc.get();
}

uint32_t CPU::getRegister(uint8_t index) const{
    return registers.read(index);
}
void CPU::step()
{
    // FETCH
    uint32_t current_pc = pc.get();
    uint32_t raw_instruction = memory.read32(current_pc);

    // DECODE
    Instruction instruction = decoder.decode(raw_instruction);

    // GENERATE CONTROL SIGNALS
    ControlSignals signals = control_unit.generate(instruction);

    // Default PC = PC + 4
    uint32_t next_pc = current_pc + 4;

    // Read registers rs1,rs2 
    uint32_t rs1_value = registers.read(instruction.rs1);
    uint32_t rs2_value = registers.read(instruction.rs2);

    // EXECUTE
    uint32_t alu_input_b;
    if (signals.alu_source == ALUSource::IMMEDIATE){
        alu_input_b = static_cast<uint32_t>(instruction.immediate);
    }
    else{
        alu_input_b = rs2_value;
    }
    uint32_t alu_result = 0;

    // Execute ALU operation if required
    if (signals.alu_operation != ALUoperation::NONE){
        alu_result = alu.execute(rs1_value, alu_input_b, signals.alu_operation);
    }

    // MEMORY ACCESS
    uint32_t memory_data = 0;

    // LOAD
    if (signals.mem_read){
        memory_data =
            memory.read32(alu_result);
    }

    // STORE
    if (signals.mem_write){
        memory.write32(
            alu_result,
            rs2_value
        );
    }

    // WRITE BACK
    if (signals.reg_write){
        uint32_t write_data;
        if (signals.mem_to_reg){
            write_data = memory_data;
        }
        else{
            write_data = alu_result;
        }
        registers.write(instruction.rd,write_data);
    }

    // BRANCH
    if (signals.branch) {
        // Currently implement BEQ
        if (instruction.funct3 == 0x0) {
            if (rs1_value == rs2_value) {
                next_pc = current_pc + instruction.immediate;
            }
        }
    }

    // JUMP
    if (signals.jump) {
        // JAL
        if (instruction.opcode == 0x6F)        {
            registers.write(instruction.rd, current_pc + 4);
            next_pc = current_pc + instruction.immediate;
        }

        // JALR
        else if (instruction.opcode == 0x67) {
            registers.write( instruction.rd, current_pc + 4);
            next_pc = (rs1_value + instruction.immediate) & ~1u;
        }
    }

    // Update PC
    pc.set(next_pc);
}