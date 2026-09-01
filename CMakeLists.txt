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

    src/trace/trace.cpp
    src/trace/logger.cpp
)

# ============================================================
# Pipeline CPU
# ============================================================

add_executable(riscv_pipeline

    tests/pipeline/test_pipeline.cpp

    src/pipelineCpu/pipeline_cpu.cpp
    src/pipelineCpu/pipeline_registers.cpp
    src/pipelineCpu/hazard_unit.cpp
    src/pipelineCPU/pipeline_trace.cpp

    ${COMMON_SOURCES}
)

# ============================================================
# IMPORTANT:
# src is an include directory
# ============================================================

target_include_directories(riscv_pipeline PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)