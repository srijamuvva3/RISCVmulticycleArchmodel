cmake_minimum_required(VERSION 3.16)

project(riscv_cpu)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Main CPU executable
add_executable(riscv_cpu
    src/main.cpp

    src/cpu/cpu.cpp
    src/cpu/control_unit/control_unit.cpp
    src/ISA/decoder.cpp
    src/cpu/alu/alu.cpp
    src/cpu/register_file/register_file.cpp
    src/cpu/register_file/pc.cpp

    src/memory/memory.cpp
)

# Arithmetic test
add_executable(test_arithmetic
    tests/test_arithmetic.cpp

    src/cpu/cpu.cpp
    src/cpu/control_unit/control_unit.cpp
    src/cpu/decoder/decoder.cpp
    src/cpu/alu/alu.cpp
    src/cpu/register_file/register_file.cpp
    src/cpu/register_file/pc.cpp

    src/memory/memory.cpp
)