//
// Created by mike on 1/6/20.
//

#ifndef NR_OPENTRACING_CPP_CONFIG_H
#define NR_OPENTRACING_CPP_CONFIG_H

#include <string>
#include <map>
#include <newrelic/libnewrelic.h>
#include <regex>
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
        static bool skipTransaction(std::string txn);
        static std::string filterTransaction(std::string txn);
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
        static const std::string TransactionFilterFileKey;
        static std::vector<std::regex> skipExpressions;
        static std::vector<std::pair<std::regex, std::string>> replaceExpressions;

        static void processTransactionFilters();

    };
}


#endif //NR_OPENTRACING_CPP_CONFIG_H
