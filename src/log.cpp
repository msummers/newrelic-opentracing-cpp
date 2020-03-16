//
// Created by mike on 1/9/20.
//
#include <string>
#include <iostream>
#include "log.h"

namespace newrelic {
    Log::LogLevels Log::level{Log::LogLevels::INFO};

    void Log::setLevel(Log::LogLevels logLevel) {
        Log::level = logLevel;
    }

    void Log::fmtMessage(std::string format, fmt::format_args args) {
        try {
            fmt::vprint(stderr, format, args);
        }catch(fmt::format_error& e){
            fmt::print(stderr, "[ERROR] fmt error: {} format: {}\n", e.what(), format);
        }
    }

    std::string Log::prefix(const char *logLevel, const char *format) {
        std::string result;
        result += "[";
        result += logLevel;
        result += "] ";
        result += format;
        result += "\n";
        return result;
    }
}
