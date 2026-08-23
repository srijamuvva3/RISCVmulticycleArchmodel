#include "cpu.h"

MultiCycleCPU::MultiCycleCPU(Memory& memory, Logger* logger)
    : memory(memory), logger(logger), cycle(0), current_pc(0), current_state(MultiCycleState::FETCH), IR(0), MDR(0), A(0), B(0), ALUOut(0)
{
    reset();
}

void MultiCycleCPU::reset() {
    pc.reset();
    registers.reset();
    cycle = 0;
    current_state = MultiCycleState::FETCH;
    IR = 0;
    MDR = 0;
    A = 0;
    B = 0;
    ALUOut = 0;
    current_pc = 0;
    current_instruction = Instruction();
}

uint32_t MultiCycleCPU::getPC() const {
    return pc.get();
}

uint32_t MultiCycleCPU::getRegister(uint32_t index) const {
    return registers.read(index);
}

uint64_t MultiCycleCPU::getCycle() const {
    return cycle;
}

MultiCycleState MultiCycleCPU::getCurrentState() const {
    return current_state;
}

void MultiCycleCPU::handleFetch(){
    current_pc = pc.get();
    IR = memory.read32(current_pc);
    pc.set(current_pc + 4);
    current_state = MultiCycleState::DECODE;
}
void MultiCycleCPU::handleDecode(){
    current_instruction = decoder.decode(IR);
    A = registers.read(current_instruction.rs1);
    B = registers.read(current_instruction.rs2);

    current_state = fsm.nextState(MultiCycleState::DECODE, current_instruction.opcode);
}
void MultiCycleCPU::handleExecuteR(){
    ControlSignals signals = control_unit.generate(current_instruction);

    ALUOut = alu.execute(A,B,signals.alu_operation);

    current_state = MultiCycleState::WRITE_BACK_R;
}
void MultiCycleCPU::handleWritebackR(){
    registers.write(current_instruction.rd,ALUOut);
    current_state = MultiCycleState::FETCH;
}
void MultiCycleCPU::handleExecuteI(){
    ControlSignals signals = control_unit.generate(current_instruction);
    uint32_t immediate = static_cast<uint32_t>(current_instruction.immediate);

    ALUOut = alu.execute(A, immediate, signals.alu_operation);

    current_state = MultiCycleState::WRITE_BACK_I;
}
void MultiCycleCPU::handleWritebackI(){
    registers.write(current_instruction.rd,ALUOut);
    current_state = MultiCycleState::FETCH;
}
void MultiCycleCPU::handleAddressCalculation(){
    uint32_t immediate = static_cast<uint32_t>(current_instruction.immediate);

    ALUOut = alu.execute(A,immediate, ALUoperation::ADD);

    //Load
    if(current_instruction.opcode==0x03){
        current_state = MultiCycleState::MEMORY_READ;
    }

    //Store
    else if (current_instruction.opcode == 0x23){
        current_state = MultiCycleState::MEMORY_WRITE;
    }

    else{
        current_state = MultiCycleState::HALT;
    }
}
// MEMORY READ HANDLER
void MultiCycleCPU::handleMemoryRead(){
    MDR = memory.read32(ALUOut);

    current_state = MultiCycleState::WRITE_BACK_LOAD;
}
// MEMORY WRITE HANDLER
void MultiCycleCPU::handleMemoryWrite(){
    memory.write32(ALUOut,B);

    current_state = MultiCycleState::FETCH;

}
// LOAD WRITEBACK HANDLER
void MultiCycleCPU::handleWritebackLoad(){
    registers.write(current_instruction.rd,MDR);

    current_state = MultiCycleState::FETCH;
}
// BRANCH HANDLER
void MultiCycleCPU::handleBranch(){
    bool taken = false;
    switch(current_instruction.funct3){
        case 0x0: 
            taken = (A==B);
            break;
        case 0x1:
            taken = (A!=B);
            break;
        case 0x4:
            taken = static_cast<int32_t>(A) < static_cast<int32_t>(B);
            break;
        case 0x5:
            taken = static_cast<int32_t>(A) >= static_cast<int32_t>(B);
            break;
        case 0x6:
            taken = (A<B);
            break;
        case 0x7:
            taken = (A>=B);
            break;
        default:
            taken = false;
            break;
    }

    if(taken){
        pc.set(current_pc +current_instruction.immediate);
    }
    current_state = MultiCycleState::FETCH;
}
void MultiCycleCPU::handleJUMP(){
    registers.write(current_instruction.rd, current_pc+4);
    pc.set(current_pc+current_instruction.immediate);

    current_state = MultiCycleState::FETCH;
}
void MultiCycleCPU::handleJALR(){
    registers.write(current_instruction.rd,current_pc+4);
    uint32_t target = A + static_cast<uint32_t>(current_instruction.immediate);
    target &= ~1u;
    pc.set(target);
    current_state = MultiCycleState::FETCH;
}
void MultiCycleCPU::handleLUI(){
    registers.write(current_instruction.rd,static_cast<uint32_t>(current_instruction.immediate));
    current_state = MultiCycleState::FETCH;
}
void MultiCycleCPU::handleAUIPC(){
    uint32_t result = current_pc + static_cast<uint32_t>(current_instruction.immediate);
    registers.write(current_instruction.rd,result);

    current_state = MultiCycleState::FETCH;
}
//trace handler
void MultiCycleCPU::logTrace(){

    if (logger == nullptr){
        return;
    }

    TraceEntry entry;
    entry.cycle = cycle;
    entry.pc = current_pc;
    entry.instruction = IR;
    entry.alu_result = ALUOut;
    entry.next_pc = pc.get();

    // Determine the stage represented by this cycle
    switch (current_state){
        case MultiCycleState::FETCH:
            entry.stage = "FETCH";
            break;

        case MultiCycleState::DECODE:
            entry.stage = "DECODE";
            break;

        case MultiCycleState::EXECUTE_R:
        case MultiCycleState::EXECUTE_I:
        case MultiCycleState::EXECUTE_ADDRESS:
            entry.stage = "EXECUTE";
            break;

        case MultiCycleState::MEMORY_READ:
        case MultiCycleState::MEMORY_WRITE:
            entry.stage = "MEMORY";
            break;

        case MultiCycleState::WRITE_BACK_R:
        case MultiCycleState::WRITE_BACK_I:
        case MultiCycleState::WRITE_BACK_LOAD:
            entry.stage = "WRITEBACK";
            break;

        case MultiCycleState::BRANCH:
            entry.stage = "BRANCH";
            break;

        case MultiCycleState::JUMP:
        case MultiCycleState::JALR:
            entry.stage = "JUMP";
            break;

        case MultiCycleState::LUI:
        case MultiCycleState::AUIPC:
            entry.stage = "WRITEBACK";
            break;

        default:
            entry.stage = "OTHER";
            break;
    }

    logger->log(entry);
}

void MultiCycleCPU::step() {

    switch(current_state) {
    // FETCH //
        case (MultiCycleState::FETCH): {
            handleFetch();
            break;
        }
    //DECODE//
        case(MultiCycleState::DECODE): {
            handleDecode();
            break;
        }
    //EXECUTE R-TYPE//
        case(MultiCycleState::EXECUTE_R): {
            handleExecuteR();
            break;
        }
    //Write Back R-TYPE//
        case(MultiCycleState::WRITE_BACK_R): {
            handleWritebackR();
            break;
        }
    //EXECUTE I Type //
        case(MultiCycleState::EXECUTE_I): {
            handleExecuteI();
            break;
        }
    //Write Back I Type //
        case(MultiCycleState::WRITE_BACK_I):{
            handleWritebackI();
            break;
        }
    //Execute Address //
        case(MultiCycleState::EXECUTE_ADDRESS): {
            handleAddressCalculation();
            break;
        }
        case(MultiCycleState::MEMORY_READ):{
            handleMemoryRead();
            break;
        }
        case(MultiCycleState::MEMORY_WRITE):{
            handleMemoryWrite();
            break;
        }
        case(MultiCycleState::WRITE_BACK_LOAD):{
            handleWritebackLoad();
            break;
        }
        case(MultiCycleState::BRANCH):{
            handleBranch();
            break;
        }
        case(MultiCycleState::JUMP):{
            handleJUMP();
            break;
        }
        case(MultiCycleState::JALR):{
            handleJALR();
            break;
        }
        case(MultiCycleState::LUI):{
            handleLUI();
            break;
        }
        case(MultiCycleState::AUIPC):{
            handleAUIPC();
            break;
        }
        case(MultiCycleState::HALT):{
            return;
        }

        logTrace();
        cycle++;
    }
}