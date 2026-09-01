#include "cpu.h"
#include "../trace/logger.h"
#include <iomanip>
#include <sstream>

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
void MultiCycleCPU::handleExecuteI()
{
    ControlSignals signals = control_unit.generate(current_instruction);

    uint32_t immediate =
        static_cast<uint32_t>(current_instruction.immediate);

    uint32_t alu_input_b = immediate;

    // SLLI, SRLI, SRAI use shamt = imm[4:0]
    if (current_instruction.funct3 == 0x1 ||
        current_instruction.funct3 == 0x5)
    {
        alu_input_b = immediate & 0x1F;
    }

    ALUOut = alu.execute(
        A,
        alu_input_b,
        signals.alu_operation
    );

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
void MultiCycleCPU::logTrace(MultiCycleState state,
    uint32_t trace_pc,
    uint32_t trace_instruction)
{
    if (logger == nullptr)
        return;

    TraceEntry entry{};

    entry.cycle = cycle;
    entry.pc = trace_pc;
    entry.instruction = trace_instruction;
    entry.immediate = current_instruction.immediate;

    // Register information
    if (current_state == MultiCycleState::DECODE ||
    current_state == MultiCycleState::EXECUTE_R ||
    current_state == MultiCycleState::EXECUTE_I)
    {
    entry.rs1 = current_instruction.rs1;
    entry.rs1_value = A;
    }

    if (current_state == MultiCycleState::DECODE ||
    current_state == MultiCycleState::EXECUTE_R)
    {
    entry.rs2 = current_instruction.rs2;
    entry.rs2_value = B;
    }

    entry.alu_result = ALUOut;

    switch (state)
    {
        // -------------------------------------------------
        // FETCH
        // -------------------------------------------------

        case MultiCycleState::FETCH:

            entry.stage = "FETCH";

            entry.activity =
                "IR <- MEM[0x";

            {
                std::ostringstream ss;

                ss << std::hex
                   << std::setw(8)
                   << std::setfill('0')
                   << current_pc;

                entry.activity += ss.str();
            }

            entry.activity += "]";

            break;


        // -------------------------------------------------
        // DECODE
        // -------------------------------------------------

        case MultiCycleState::DECODE:

            entry.stage = "DECODE";

            entry.rs1 = current_instruction.rs1;
            entry.rs2 = current_instruction.rs2;

            entry.rs1_value = A;
            entry.rs2_value = B;

            entry.activity =
                "Decode | A <- x" +
                std::to_string(current_instruction.rs1) +
                ", B <- x" +
                std::to_string(current_instruction.rs2);

            break;


        // -------------------------------------------------
        // EXECUTE R
        // -------------------------------------------------

        case MultiCycleState::EXECUTE_R:

            entry.stage = "EXECUTE_R";

            entry.activity =
                "ALU execute R-type";

            break;


        // -------------------------------------------------
        // EXECUTE I
        // -------------------------------------------------

        case MultiCycleState::EXECUTE_I:

            entry.stage = "EXECUTE_I";

            entry.activity =
                "ALU execute I-type | imm = " +
                std::to_string(current_instruction.immediate);

            break;


        // -------------------------------------------------
        // ADDRESS CALCULATION
        // -------------------------------------------------

        case MultiCycleState::EXECUTE_ADDRESS:

            entry.stage = "EXECUTE_ADDR";

            entry.activity =
                "ALU: base + immediate";

            break;


        // -------------------------------------------------
        // MEMORY READ
        // -------------------------------------------------

        case MultiCycleState::MEMORY_READ:

            entry.stage = "MEM_READ";

            entry.mem_read = true;
            entry.mem_address = ALUOut;
            entry.mem_data = MDR;

            entry.activity =
                "MDR <- MEM[0x";

            {
                std::ostringstream ss;

                ss << std::hex
                   << std::setw(8)
                   << std::setfill('0')
                   << ALUOut;

                entry.activity += ss.str();
            }

            entry.activity += "]";

            break;


        // -------------------------------------------------
        // MEMORY WRITE
        // -------------------------------------------------

        case MultiCycleState::MEMORY_WRITE:

            entry.stage = "MEM_WRITE";

            entry.mem_write = true;
            entry.mem_address = ALUOut;
            entry.mem_data = B;

            entry.activity =
                "MEM[0x";

            {
                std::ostringstream ss;

                ss << std::hex
                   << std::setw(8)
                   << std::setfill('0')
                   << ALUOut;

                entry.activity += ss.str();
            }

            entry.activity += "] <- B";

            break;


        // -------------------------------------------------
        // WRITE BACK R
        // -------------------------------------------------

        case MultiCycleState::WRITE_BACK_R:

            entry.stage = "WRITE_BACK_R";

            entry.reg_write = true;
            entry.rd = current_instruction.rd;
            entry.reg_write_value = ALUOut;

            entry.activity =
                "REG[x" +
                std::to_string(current_instruction.rd) +
                "] <- ALUOut";

            break;


        // -------------------------------------------------
        // WRITE BACK I
        // -------------------------------------------------

        case MultiCycleState::WRITE_BACK_I:

            entry.stage = "WRITE_BACK_I";

            entry.reg_write = true;
            entry.rd = current_instruction.rd;
            entry.reg_write_value = ALUOut;

            entry.activity =
                "REG[x" +
                std::to_string(current_instruction.rd) +
                "] <- ALUOut";

            break;


        // -------------------------------------------------
        // WRITE BACK LOAD
        // -------------------------------------------------

        case MultiCycleState::WRITE_BACK_LOAD:

            entry.stage = "WRITE_BACK_LD";

            entry.reg_write = true;
            entry.rd = current_instruction.rd;
            entry.reg_write_value = MDR;

            entry.activity =
                "REG[x" +
                std::to_string(current_instruction.rd) +
                "] <- MDR";

            break;


        // -------------------------------------------------
        // BRANCH
        // -------------------------------------------------

        case MultiCycleState::BRANCH:

            entry.stage = "BRANCH";

            entry.activity =
                "Compare registers | PC <- branch target if taken";

            break;


        // -------------------------------------------------
        // JAL
        // -------------------------------------------------

        case MultiCycleState::JUMP:

            entry.stage = "JAL";

            entry.reg_write = true;
            entry.rd = current_instruction.rd;
            entry.reg_write_value = current_pc + 4;

            entry.activity =
                "REG[x" +
                std::to_string(current_instruction.rd) +
                "] <- PC+4 | PC <- PC+imm";

            break;


        // -------------------------------------------------
        // JALR
        // -------------------------------------------------

        case MultiCycleState::JALR:

            entry.stage = "JALR";

            entry.reg_write = true;
            entry.rd = current_instruction.rd;
            entry.reg_write_value = current_pc + 4;

            entry.activity =
                "REG[x" +
                std::to_string(current_instruction.rd) +
                "] <- PC+4 | PC <- rs1+imm";

            break;


        // -------------------------------------------------
        // LUI
        // -------------------------------------------------

        case MultiCycleState::LUI:

            entry.stage = "LUI";

            entry.reg_write = true;
            entry.rd = current_instruction.rd;
            entry.reg_write_value =
                static_cast<uint32_t>(
                    current_instruction.immediate
                );

            entry.activity =
                "REG[x" +
                std::to_string(current_instruction.rd) +
                "] <- imm";

            break;


        // -------------------------------------------------
        // AUIPC
        // -------------------------------------------------

        case MultiCycleState::AUIPC:

            entry.stage = "AUIPC";

            entry.reg_write = true;
            entry.rd = current_instruction.rd;

            entry.reg_write_value =
                current_pc +
                static_cast<uint32_t>(
                    current_instruction.immediate
                );

            entry.activity =
                "REG[x" +
                std::to_string(current_instruction.rd) +
                "] <- PC+imm";

            break;


        default:

            entry.stage = "HALT";
            entry.activity = "No operation";

            break;
    }

    logger->log(entry);
}

void MultiCycleCPU::step()
{
    if (current_state == MultiCycleState::HALT)
        return;

    MultiCycleState state_before = current_state;

    // --------------------------------------------------
    // Capture values belonging to this cycle
    // --------------------------------------------------

    uint32_t trace_pc = current_pc;
    uint32_t trace_instruction = IR;

    // FETCH is special:
    // current_pc/IR haven't been updated yet.
    if (state_before == MultiCycleState::FETCH)
    {
        trace_pc = pc.get();
        trace_instruction = memory.read32(trace_pc);
    }

    switch (current_state)
    {
        case MultiCycleState::FETCH:
            handleFetch();
            break;

        case MultiCycleState::DECODE:
            handleDecode();
            break;

        case MultiCycleState::EXECUTE_R:
            handleExecuteR();
            break;

        case MultiCycleState::WRITE_BACK_R:
            handleWritebackR();
            break;

        case MultiCycleState::EXECUTE_I:
            handleExecuteI();
            break;

        case MultiCycleState::WRITE_BACK_I:
            handleWritebackI();
            break;

        case MultiCycleState::EXECUTE_ADDRESS:
            handleAddressCalculation();
            break;

        case MultiCycleState::MEMORY_READ:
            handleMemoryRead();
            break;

        case MultiCycleState::MEMORY_WRITE:
            handleMemoryWrite();
            break;

        case MultiCycleState::WRITE_BACK_LOAD:
            handleWritebackLoad();
            break;

        case MultiCycleState::BRANCH:
            handleBranch();
            break;

        case MultiCycleState::JUMP:
            handleJUMP();
            break;

        case MultiCycleState::JALR:
            handleJALR();
            break;

        case MultiCycleState::LUI:
            handleLUI();
            break;

        case MultiCycleState::AUIPC:
            handleAUIPC();
            break;

        case MultiCycleState::HALT:
            return;
    }

    // --------------------------------------------------
    // Override the trace values with captured values
    // --------------------------------------------------

    logTrace(
    state_before,
    trace_pc,
    trace_instruction
);

    cycle++;
}