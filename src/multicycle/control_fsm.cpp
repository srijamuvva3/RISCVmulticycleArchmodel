#include <cstdint>
#include "state.h"
#include "control_fsm.h"

class ControlFSM {
public:
    MultiCycleState nextState(MultiCycleState current_state, uint32_t opcode) const {
        switch(current_state){
            case MultiCycleState::FETCH:
                return MultiCycleState::DECODE;

            case MultiCycleState::DECODE:
                switch(opcode){
                    case 0x33: // R-type
                        return MultiCycleState::EXECUTE_R;
                    case 0x13: // I-type
                        return MultiCycleState::EXECUTE_I;
                    case 0x03: // Load
                        return MultiCycleState::EXECUTE_ADDRESS;
                    case 0x23: // Store
                        return MultiCycleState::EXECUTE_ADDRESS;
                    case 0x63: // Branch
                        return MultiCycleState::BRANCH;
                    case 0x6F: // JAL
                        return MultiCycleState::JUMP;
                    case 0x67: // JALR
                        return MultiCycleState::JALR;
                    case 0x37: // LUI
                        return MultiCycleState::LUI;
                    case 0x17: // AUIPC
                        return MultiCycleState::AUIPC;
                    default:
                        return MultiCycleState::HALT; // Unknown opcode, halt the CPU
                }

            case MultiCycleState::EXECUTE_R:
                return MultiCycleState::WRITE_BACK_R;

            case MultiCycleState::EXECUTE_I:
                return MultiCycleState::WRITE_BACK_I;

            case MultiCycleState::EXECUTE_ADDRESS:
                if (opcode == 0x03) // Load
                    return MultiCycleState::MEMORY_READ;
                else if (opcode == 0x23) // Store
                    return MultiCycleState::MEMORY_WRITE;
            case MultiCycleState::MEMORY_READ:
                return MultiCycleState::WRITE_BACK_LOAD;
            
            case MultiCycleState::MEMORY_WRITE:
                return MultiCycleState::FETCH;
            
            case MultiCycleState::WRITE_BACK_R:
                return MultiCycleState::FETCH;

            case MultiCycleState::WRITE_BACK_I:
                return MultiCycleState::FETCH;

            case MultiCycleState::WRITE_BACK_LOAD:
                return MultiCycleState::FETCH;

            case MultiCycleState::BRANCH:
                return MultiCycleState::FETCH;

            case MultiCycleState::JUMP:
                return MultiCycleState::FETCH;

            case MultiCycleState::JALR:
                return MultiCycleState::FETCH;
            
            case MultiCycleState::LUI:
                return MultiCycleState::FETCH;
            
            case MultiCycleState::AUIPC:
                return MultiCycleState::FETCH;
            
            default:
                return MultiCycleState::HALT;
        }

    }

};

