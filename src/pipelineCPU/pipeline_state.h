#ifndef PIPELINE_STATE_H
#define PIPELINE_STATE_H

#include <cstdint>

enum class PipelineStage
{
    IF,
    ID,
    EX,
    MEM,
    WB
};

enum class PipelineStatus
{
    RUNNING,
    HALTED
};

#endif