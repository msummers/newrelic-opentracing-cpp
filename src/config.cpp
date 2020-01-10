//
// Created by mike on 1/6/20.
//

#include "config.h"
#include "string_utils.h"
#include <unistd.h>
#include <newrelic/libnewrelic.h>
#include <iostream>

namespace newrelic {
    const std::string Config::LicenseKey{"newrelicLicense"};
    const std::string Config::AppNameKey{"applicationName"};
    const std::string Config::LogLevelKey{"logLevel"};
    const std::string Config::SegmentCategoryKey{"segmentCategory"};
    const std::string Config::CSDKLogLocationKey{"csdkLogLocation"};
    const std::string Config::CSDKLogLevelKey{"csdkLogLevel"};

    std::map<std::string, std::string> Config::config = {{Config::LicenseKey,         ""},
                                                         {Config::AppNameKey,         ""},
                                                         {Config::LogLevelKey,        "ALL"},
                                                         {Config::SegmentCategoryKey, "nginx"},
                                                         {Config::CSDKLogLevelKey,    "INFO"},
                                                         {Config::CSDKLogLocationKey, "./c_sdk.log"}};

    std::map<std::string, newrelic_loglevel_t> Config::newrelicLogLevels = {{"INFO",  NEWRELIC_LOG_INFO},
                                                                            {"DEBUG", NEWRELIC_LOG_DEBUG},
                                                                            {"ERROR", NEWRELIC_LOG_ERROR},
                                                                            {"WARN",  NEWRELIC_LOG_WARNING}};

    std::map<std::string, Log::LogLevels> Config::logLevels = {{"TRACE",    Log::LogLevels::TRACE},
                                                               {"DEBUG",    Log::LogLevels::DEBUG},
                                                               {"INFO",     Log::LogLevels::INFO},
                                                               {"WARN",     Log::LogLevels::WARN},
                                                               {"ERROR",    Log::LogLevels::ERROR},
                                                               {"CRITICAL", Log::LogLevels::FATAL},
                                                               {"ALL",      Log::LogLevels::ALL},
                                                               {"OFF",      Log::LogLevels::OFF}};

    Log::LogLevels Config::getLogLevel() {
        return Config::logLevels[StringUtils::toUpper(Config::config[Config::LogLevelKey])];
    }

    std::string Config::getSegmentCategory() {
        return Config::config[Config::SegmentCategoryKey];
    }

    newrelic_loglevel_t Config::getCSDKLogLevel() {
        return Config::newrelicLogLevels[StringUtils::toUpper(Config::config[Config::CSDKLogLevelKey])];
    }

    std::string Config::getCSDKLogLocation() {
        return Config::config[Config::CSDKLogLocationKey];
    }

    std::string Config::getApplicationName() {
        auto appName = Config::config[Config::AppNameKey];
        if (appName == "") {
            const std::string Suffix{"-nginx"};
            std::string Hostname{"unknown"};
            char hostname[1024];
            auto success = gethostname(hostname, sizeof(hostname));
            if (success == 0) {
                Hostname = hostname;
            }
            appName = Hostname + Suffix;
        }
        return appName;
    }

    std::string Config::getLicense() {
        if (Config::config[Config::LicenseKey] == "") {
            Log::fatal("Config::getLicense New Relic license key not configured");
        }
        return Config::config[Config::LicenseKey];
    }

    void Config::init(const char *configuration) {
        Log::trace("Config::init enter configuration string: {}", configuration);
        std::istringstream stream{configuration};
        std::string str;
        for (std::string line; std::getline(stream, line);) {
            // Deal with comments
            line = StringUtils::chomp(line, "#");
            if (line == "") {
                continue;
            }

            auto words = StringUtils::split(line, ':');
            Log::debug("Config::init loading key: {} value: {}", words.front(), words.back());
            // Key is in the conf file
            if (Config::config.count(words.front()) > 0) {
                Config::config[words.front()] = StringUtils::trim(words.back());
            } else {
                Log::warn("Config::init unknown key: {}", words.front());
            }
        }
        for (std::pair<std::string, std::string> element : Config::config) {
            Log::debug("Config::init loaded key: {}  value: <{}>", element.first, element.second);
        }
        Log::trace("Config::init exit");
    }
}