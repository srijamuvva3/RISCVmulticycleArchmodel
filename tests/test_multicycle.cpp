#include <iostream>
#include <cassert>

#include "../src/multicycle/cpu.h"
#include "../src/memory/memory.h"


int main()
{
    Memory memory;

    MultiCycleCPU cpu(memory);

    cpu.reset();


    // ------------------------------------------------
    // Program
    //
    // ADDI x1, x0, 10
    // ADDI x2, x0, 20
    // ADD  x3, x1, x2
    // ------------------------------------------------

    memory.write32(0x00, 0x00A00093);
    memory.write32(0x04, 0x01400113);
    memory.write32(0x08, 0x002081B3);


    // ------------------------------------------------
    // Run enough cycles
    // ------------------------------------------------

    for (int i = 0; i < 15; ++i)
    {
        cpu.step();
    }


    // ------------------------------------------------
    // Check results
    // ------------------------------------------------

    std::cout << "x1 = "
              << cpu.getRegister(1)
              << std::endl;

    std::cout << "x2 = "
              << cpu.getRegister(2)
              << std::endl;

    std::cout << "x3 = "
              << cpu.getRegister(3)
              << std::endl;


    assert(cpu.getRegister(1) == 10);
    assert(cpu.getRegister(2) == 20);
    assert(cpu.getRegister(3) == 30);


    std::cout << "MULTICYCLE ARITHMETIC TEST PASSED"
              << std::endl;

    return 0;
}