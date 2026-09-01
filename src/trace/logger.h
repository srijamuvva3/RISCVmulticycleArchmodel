#ifndef LOGGER_H
#define LOGGER_H

#include "trace.h"

#include <fstream>
#include <string>

class Logger
{
public:

    explicit Logger(const std::string& filename);

    ~Logger();

    void log(const TraceEntry& entry);

    void close();

private:

    std::ofstream file;

    void writeHeader();

    void writeHex32(uint32_t value);
};

#endif