cmake_minimum_required(VERSION 3.20)

project(RISCVProcessor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# ============================================================
# Common source files
# ============================================================

set(COMMON_SOURCES

    src/common/instruction.h

    src/ISA/decoder.cpp

    src/execution/alu/alu.cpp
    src/execution/control_unit/control_unit.cpp

    src/execution/register_file/register_file.cpp
    src/execution/register_file/pc.cpp

    src/memory/memory.cpp
)


# ============================================================
# MULTICYCLE CPU
# ============================================================

add_executable(riscv_multicycle

    tests/multicycle/test_multicycle.cpp

    src/multicycle/cpu.cpp
    src/multicycle/control_fsm.cpp

    src/trace/trace.cpp
    src/trace/logger.cpp

    ${COMMON_SOURCES}
)


target_include_directories(riscv_multicycle PRIVATE

    ${CMAKE_CURRENT_SOURCE_DIR}/src
)


# ============================================================
# PIPELINE CPU
# ============================================================

add_executable(riscv_pipeline

    tests/pipeline/test_pipeline.cpp

    src/pipelineCPU/pipeline_cpu.cpp
    src/pipelineCPU/pipeline_registers.cpp
    src/pipelineCPU/hazard_unit.cpp
    src/pipelineCPU/pipeline_trace.cpp

    ${COMMON_SOURCES}

    src/trace/trace.cpp
    src/trace/logger.cpp
)


target_include_directories(riscv_pipeline PRIVATE

    ${CMAKE_CURRENT_SOURCE_DIR}/src
)