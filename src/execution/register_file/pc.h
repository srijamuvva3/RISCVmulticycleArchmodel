#ifndef PC_H
#define PC_H

#include <cstdint>

class PC{
    private:
        uint32_t value;
    public:
        PC();
        uint32_t get() const;
        void set(uint32_t new_value);
        void reset();
};

#endif
