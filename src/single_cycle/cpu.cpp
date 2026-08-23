#include "cpu.h"

SingleCycleCPU::SingleCycleCPU(Memory& memory, Logger* logger) : memory(memory), logger(logger), cycle(0) {
    reset();
}

void SingleCycleCPU::reset(){
    pc.reset();
    registers.reset();

    cycle = 0;
}

uint32_t SingleCycleCPU::getPC() const{
    return pc.get();
}

uint32_t SingleCycleCPU::getRegister(uint8_t index) const{
    return registers.read(index);
}

uint64_t SingleCycleCPU::getCycle() const {
    return cycle;
}

void SingleCycleCPU::step()
{
    //// FETCH ////
    uint32_t current_pc = pc.get();
    uint32_t raw_instruction = memory.read32(current_pc);

    //// DECODE ////
    Instruction instruction = decoder.decode(raw_instruction);

    //GENERATE CONTROL SIGNALS
    ControlSignals signals = control_unit.generate(instruction);

    // Default PC = PC + 4
    uint32_t next_pc = current_pc + 4;

    // Read registers rs1,rs2 
    uint32_t rs1_value = registers.read(instruction.rs1);
    uint32_t rs2_value = registers.read(instruction.rs2);

    //// EXECUTE ////
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

    //// MEMORY ACCESS ////
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

    //// WRITE BACK ////
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
        switch(instruction.funct3) {
            case 0x0: // BEQ
                if (rs1_value == rs2_value) {
                    next_pc = current_pc + instruction.immediate;
                }
                break;
            case 0x1: // BNE
                if (rs1_value != rs2_value) {
                    next_pc = current_pc + instruction.immediate;
                }
                break;
            case 0x4: // BLT
                if (static_cast<int32_t>(rs1_value) < static_cast<int32_t>(rs2_value)) {
                    next_pc = current_pc + instruction.immediate;
                }
                break;
            case 0x5: // BGE
                if (static_cast<int32_t>(rs1_value) >= static_cast<int32_t>(rs2_value)) {
                    next_pc = current_pc + instruction.immediate;
                }
                break;
            case 0x6: // BLTU
                if (rs1_value < rs2_value) {
                    next_pc = current_pc + instruction.immediate;
                }
                break; 
            case 0x7: // BGEU
                if (rs1_value >= rs2_value) {
                    next_pc = current_pc + instruction.immediate;
                }
                break;
            default:
                // Unsupported branch type
                break;
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


    //// TRACE LOGGING ////
    if (logger!=nullptr){
        TraceEntry entry;
        entry.cycle = cycle;
        entry.pc = current_pc;
        entry.instruction = raw_instruction;
        entry.stage = "SingleCycle";
        entry.alu_result = alu_result;
        entry.reg_write = signals.reg_write;
        entry.rd = instruction.rd;
        entry.write_data = signals.reg_write ? (signals.mem_to_reg ? memory_data : alu_result) : 0;
        entry.mem_read = signals.mem_read;
        entry.mem_write = signals.mem_write;
        entry.mem_address = signals.mem_read || signals.mem_write ? alu_result : 0;
        entry.mem_data = signals.mem_read ? memory_data : (signals.mem_write ? rs2_value : 0);
        entry.next_pc = next_pc;

        logger->log(entry);
    }

    // Increment cycle count
    cycle++;


}