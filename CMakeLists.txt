cmake_minimum_required(VERSION 3.16)

project(riscv_cpu)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Common include directory
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ============================================================
# Single-cycle CPU
# ============================================================

add_executable(riscv_cpu

    src/main.cpp

    src/isa/decoder.cpp

    src/execution/alu/alu.cpp

    src/execution/register_file/register_file.cpp
    src/execution/register_file/pc.cpp

    src/execution/control_unit/control_unit.cpp

    src/memory/memory.cpp

    src/trace/trace.cpp
    src/trace/logger.cpp

    src/single_cycle/cpu.cpp
)


# ============================================================
# Multi-cycle CPU
# ============================================================

add_executable(riscv_multicycle
    src/main.cpp

    src/common/...

    src/memory/memory.cpp

    src/isa/decoder.cpp

    src/execution/alu/alu.cpp

    src/execution/register_file/register_file.cpp
    src/execution/register_file/pc.cpp

    src/execution/control_unit/control_unit.cpp

    src/memory/memory.cpp

    src/trace/trace.cpp
    src/trace/logger.cpp

    src/multicycle/cpu.cpp
    src/multicycle/control_fsm.cpp
)
add_executable(test_multicycle

    tests/multicycle/test_multicycle.cpp

    src/multicycle/cpu.cpp
    src/multicycle/control_fsm.cpp

    src/isa/decoder.cpp

    src/execution/alu/alu.cpp

    src/execution/control_unit/control_unit.cpp

    src/execution/register_file/register_file.cpp
    src/execution/register_file/pc.cpp

    src/memory/memory.cpp

    src/trace/trace.cpp
    src/trace/logger.cpp
)