#include "pc.h"

PC::PC()
{
    reset();
}

uint32_t PC::get() const
{
    return value;
}

void PC::set(uint32_t new_value)
{
    value = new_value;
}

void PC::reset()
{
    value = 0;
}