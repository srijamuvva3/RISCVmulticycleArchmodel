#include "memory.h"
#include <stdexcept>
Memory::Memory() {
    reset();
}
void Memory::reset() {
    for (uint32_t i = 0; i < MEMORY_SIZE; ++i) {
        data[i] = 0;
    }
}

uint8_t Memory::read8(uint32_t address) const {//for MDR
    if (address >= MEMORY_SIZE) {
        throw std::out_of_range("Memory read8 address out of range");
    }
    return data[address];
}
void Memory::write8(uint32_t address, uint8_t value) { //for MDR
    if (address >= MEMORY_SIZE) {
        throw std::out_of_range("Memory write8 address out of range");
    }
    data[address] = value;
}

uint32_t Memory::read32(uint32_t address) const
{
    return static_cast<uint32_t>(data[address]) |
           (static_cast<uint32_t>(data[address + 1]) << 8) |
           (static_cast<uint32_t>(data[address + 2]) << 16) |
           (static_cast<uint32_t>(data[address + 3]) << 24);
}

void Memory::write32(uint32_t address, uint32_t value)
{
    data[address]     = static_cast<uint8_t>(value & 0xFF);
    data[address + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
    data[address + 2] = static_cast<uint8_t>((value >> 16) & 0xFF);
    data[address + 3] = static_cast<uint8_t>((value >> 24) & 0xFF);
}