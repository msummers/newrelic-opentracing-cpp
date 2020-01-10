//
// Created by mike on 1/6/20.
//

#ifndef NR_OPENTRACING_CPP_CONFIG_H
#define NR_OPENTRACING_CPP_CONFIG_H

#include <string>
#include <map>
#include <newrelic/libnewrelic.h>
#include "log.h"

namespace newrelic {
    class Config {
    public:
        static void init(const char* configuration);
        static std::string getLicense();
        static std::string getApplicationName();
        static Log::LogLevels getLogLevel();
        static newrelic_loglevel_t getCSDKLogLevel();
        static std::string getCSDKLogLocation();
        static std::string getSegmentCategory();
    private:
        static std::map<std::string, std::string> config;
        static std::map<std::string, Log::LogLevels> logLevels;
        static std::map<std::string, newrelic_loglevel_t> newrelicLogLevels;
        static const std::string LicenseKey;
        static const std::string AppNameKey;
        static const std::string LogLevelKey;
        static const std::string SegmentCategoryKey;
        static const std::string CSDKLogLocationKey;
        static const std::string CSDKLogLevelKey;
    };
}


#endif //NR_OPENTRACING_CPP_CONFIG_H
