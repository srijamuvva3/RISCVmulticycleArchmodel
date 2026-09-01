#include "logger.h"

#include <iomanip>
#include <sstream>
#include <stdexcept>

Logger::Logger(const std::string& filename)
{
    file.open(filename, std::ios::out | std::ios::trunc);

    if (!file.is_open())
    {
        throw std::runtime_error(
            "Unable to open trace file: " + filename
        );
    }

    writeHeader();
}

Logger::~Logger()
{
    close();
}


// ============================================================
// HEADER
// ============================================================

void Logger::writeHeader()
{
    file
        << "========================================================================================================================\n"
        << "                                      RISC-V MULTI-CYCLE TRACE\n"
        << "========================================================================================================================\n";

    file
        << std::left
        << std::setw(6)  << "Cycle"       << " | "
        << std::setw(12) << "PC"          << " | "
        << std::setw(12) << "Instruction" << " | "
        << std::setw(15) << "State"       << " | "
        << std::setw(4)  << "rs1"         << " | "
        << std::setw(12) << "rs1_val"     << " | "
        << std::setw(4)  << "rs2"         << " | "
        << std::setw(12) << "rs2_val"     << " | "
        << std::setw(12) << "Immediate"   << " | "
        << std::setw(12) << "ALU_result"  << " | "
        << "Activity\n";

    file
        << "------------------------------------------------------------------------------------------------------------------------\n";

    file.flush();
}

// ============================================================
// LOG ONE CYCLE
// ============================================================

void Logger::log(const TraceEntry& entry)
{
    if (!file.is_open())
        return;

    // -------------------------------------------------
    // Helper lambdas
    // -------------------------------------------------

    auto hex32 = [&](uint32_t value)
    {
        std::ostringstream ss;

        ss << "0x"
           << std::uppercase
           << std::hex
           << std::right
           << std::setw(8)
           << std::setfill('0')
           << value;

        return ss.str();
    };

    auto decField = [&](uint64_t value, int width)
    {
        std::ostringstream ss;

        ss << std::left
           << std::setw(width)
           << std::setfill(' ')
           << std::dec
           << value;

        return ss.str();
    };

    auto textField = [&](const std::string& text, int width)
    {
        std::ostringstream ss;

        ss << std::left
           << std::setw(width)
           << std::setfill(' ')
           << text;

        return ss.str();
    };

    // -------------------------------------------------
    // Prepare fields
    // -------------------------------------------------

    std::string rs1 = "-";
    std::string rs1_val = "-";
    std::string rs2 = "-";
    std::string rs2_val = "-";
    std::string imm = "-";
    std::string alu = "-";

    bool show_rs1 =
        entry.stage == "DECODE" ||
        entry.stage == "EXECUTE_R" ||
        entry.stage == "EXECUTE_I" ||
        entry.stage == "EXECUTE_ADDR" ||
        entry.stage == "BRANCH" ||
        entry.stage == "JALR";

    bool show_rs2 =
        entry.stage == "DECODE" ||
        entry.stage == "EXECUTE_R" ||
        entry.stage == "BRANCH";

    bool show_imm =
        entry.stage == "DECODE" ||
        entry.stage == "EXECUTE_I" ||
        entry.stage == "EXECUTE_ADDR" ||
        entry.stage == "JAL" ||
        entry.stage == "JALR" ||
        entry.stage == "LUI" ||
        entry.stage == "AUIPC";

    bool show_alu =
        entry.stage == "EXECUTE_R" ||
        entry.stage == "EXECUTE_I" ||
        entry.stage == "EXECUTE_ADDR" ||
        entry.stage == "WRITE_BACK_R" ||
        entry.stage == "WRITE_BACK_I" ||
        entry.stage == "WRITE_BACK_LD";

    if (show_rs1)
    {
        rs1 = "x" + std::to_string(entry.rs1);
        rs1_val = hex32(entry.rs1_value);
    }

    if (show_rs2)
    {
        rs2 = "x" + std::to_string(entry.rs2);
        rs2_val = hex32(entry.rs2_value);
    }

    if (show_imm)
        imm = std::to_string(entry.immediate);

    if (show_alu)
        alu = hex32(entry.alu_result);

    // -------------------------------------------------
    // Print row
    // -------------------------------------------------

    file
        << decField(entry.cycle, 6) << " | "
        << textField(hex32(entry.pc), 12) << " | "
        << textField(hex32(entry.instruction), 12) << " | "
        << textField(entry.stage, 15) << " | "
        << textField(rs1, 4) << " | "
        << textField(rs1_val, 12) << " | "
        << textField(rs2, 4) << " | "
        << textField(rs2_val, 12) << " | "
        << textField(imm, 12) << " | "
        << textField(alu, 12) << " | "
        << entry.activity;

    file << "\n";

    file.flush();
}


// ============================================================
// CLOSE
// ============================================================

void Logger::close()
{
    if (file.is_open())
    {
        file.flush();
        file.close();
    }
}