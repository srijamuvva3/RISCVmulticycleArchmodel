#include "register_file.h"
#include <stdexcept>

RegisterFile::RegisterFile() {
    reset();
}

void RegisterFile::reset() {
    for (int i = 0; i < 32; ++i) {
        registers[i] = 0;
    }
}
uint32_t RegisterFile::read(uint32_t reg_num) const {
    if(reg_num == 0) {
        return 0; // Register 0 is always zero
    }
    if(reg_num >= 32) {
        throw std::out_of_range("Register number out of range");
    }
    return registers[reg_num];
}
void RegisterFile::write(uint32_t reg_num, uint32_t value) {
    if(reg_num != 0) { // Register 0 is read-only
        registers[reg_num] = value;
    }
    return;
}