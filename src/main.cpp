#include <iostream>
#include <cstdint>

#include "multicycle/cpu.h"
#include "memory/memory.h"

int main()
{
    // Create 64 KB memory
    Memory memory;

    // Create CPU connected to that memory
    MultiCycleCPU cpu(memory);

    // Reset CPU
    cpu.reset();


    memory.write32(0x00, 0x00A00093);
    memory.write32(0x04, 0x01400113);
    memory.write32(0x08, 0x002081B3);

    // Execute 3 instructions
    cpu.step();
    cpu.step();
    cpu.step();

    // Check results
    std::cout << "x1 = " << cpu.getRegister(1) << std::endl;
    std::cout << "x2 = " << cpu.getRegister(2) << std::endl;
    std::cout << "x3 = " << cpu.getRegister(3) << std::endl;

    std::cout << "PC = 0x"
              << std::hex
              << cpu.getPC()
              << std::dec
              << std::endl;

    return 0;
}