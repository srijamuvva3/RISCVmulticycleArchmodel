#include "pc.h"

PC::PC()
    : value(0)
{
}

void PC::reset()
{
    value = 0;
}

uint32_t PC::get() const
{
    return value;
}

void PC::set(uint32_t new_value)
{
    value = new_value;
}