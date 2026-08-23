#ifndef STATE_H
#define STATE_H

enum class MultiCycleState{
    FETCH,
    DECODE,

    EXECUTE_R,
    EXECUTE_I,
    EXECUTE_ADDRESS,

    MEMORY_READ,
    MEMORY_WRITE,

    WRITE_BACK_R,
    WRITE_BACK_I,
    WRITE_BACK_LOAD,

    BRANCH,
    JUMP,
    JALR,

    LUI,
    AUIPC,

    HALT
};

#endif // STATE_H