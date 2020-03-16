//
// Created by mike on 1/9/20.
//

#ifndef NR_OPENTRACING_CPP_LOG_H
#define NR_OPENTRACING_CPP_LOG_H
#define FMT_HEADER_ONLY
#include "fmt/format.h"
#include <iostream>

namespace newrelic {
    class Log {
    public:
        enum class LogLevels {
            ALL, TRACE, DEBUG, INFO, WARN, ERROR, FATAL, OFF
        };

        static void setLevel(LogLevels logLevel);

        // Templated definitions go in the header to give the compiler the info it needs.

        template<typename S, typename... Args>
        static void trace(const S &format, Args &&... args) {
            try {
                if (Log::LogLevels::TRACE >= Log::level) {
                    fmtMessage(prefix("TRACE", format), fmt::make_format_args(args...));
                }
            } catch (...) {
                auto e = std::current_exception();
                std::cerr << e.__cxa_exception_type()->name() << " " << format << std::endl;
            }
        }

        template<typename S, typename... Args>
        static void debug(const S &format, Args &&... args) {
            try {
                if (Log::LogLevels::DEBUG >= Log::level) {
                    fmtMessage(prefix("DEBUG", format), fmt::make_format_args(args...));
                }
            } catch (...) {
                auto e = std::current_exception();
                std::cerr << e.__cxa_exception_type()->name() << " " << format << std::endl;
            }
        }

        template<typename S, typename... Args>
        static void info(const S &format, Args &&... args) {
            if (Log::LogLevels::INFO >= Log::level) {
                fmtMessage(prefix("INFO ", format), fmt::make_format_args(args...));
            }
        }

        template<typename S, typename... Args>
        static void warn(const S &format, Args &&... args) {
            if (Log::LogLevels::WARN >= Log::level) {
                fmtMessage(prefix("WARN ", format), fmt::make_format_args(args...));
            }
        }

        template<typename S, typename... Args>
        static void error(const S &format, Args &&... args) {
            if (Log::LogLevels::ERROR >= Log::level) {
                fmtMessage(prefix("ERROR", format), fmt::make_format_args(args...));
            }
        }

        template<typename S, typename... Args>
        static void fatal(const S &format, Args &&... args) {
            if (Log::LogLevels::FATAL >= Log::level) {
                fmtMessage(prefix("FATAL", format), fmt::make_format_args(args...));
                std::terminate();
            }
        }

    private:
        static void fmtMessage(std::string format, fmt::format_args args);

        static std::string prefix(const char *logLevel, const char *format);

        static LogLevels level;
    };
}
#endif //NR_OPENTRACING_CPP_LOG_H
