#ifndef LoGGER_H
#define LOGGER_H

#include "trace.h"
#include <fstream>
#include <string>

class Logger {
    private:
        std::ofstream log_file;
        
public:
    Logger()=default; //creates a logger object w/o opening a file
    explicit Logger(const std::string& filename); //
    ~Logger();
    void open(const std::string& filename);
    void close();
    void log(const TraceEntry& entry);
    void logHeader();
};

#endif // LOGGER_H