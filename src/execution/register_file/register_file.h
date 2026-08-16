#ifndef REGISTER_FILE_H
#define REGISTER_FILE_H

#include <cstdint>
class RegisterFile
{
    private:
        uint32_t registers[32];
    public:
        RegisterFile();
        uint32_t read(uint32_t reg_num) const;
        void write(uint32_t reg_num, uint32_t value);
        void reset();
};

#endif
