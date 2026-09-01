#ifndef PC_H
#define PC_H

#include <cstdint>

class PC
{
public:

    PC();

    void reset();

    uint32_t get() const;

    void set(uint32_t value);

private:

    uint32_t value;
};

#endif