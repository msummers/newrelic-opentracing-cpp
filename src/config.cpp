//
// Created by mike on 1/6/20.
//

#include <fstream>
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
                                                         {Config::LogLevelKey,        "INFO"},
                                                         {Config::SegmentCategoryKey, "nginx"},
                                                         {Config::CSDKLogLevelKey,    "INFO"},
                                                         {Config::CSDKLogLocationKey, "./c_sdk.log"}};

    std::map<std::string, newrelic_loglevel_t> Config::newrelicLogLevel = {{"INFO",    NEWRELIC_LOG_INFO},
                                                                           {"DEBUG",   NEWRELIC_LOG_DEBUG},
                                                                           {"ERROR",   NEWRELIC_LOG_ERROR},
                                                                           {"WARNING", NEWRELIC_LOG_WARNING}};

    std::string Config::getSegmentCategory() {
        return Config::config[Config::SegmentCategoryKey];
    }

    std::string Config::getLogLevel() {
        return Config::config[Config::LogLevelKey];
    }

    newrelic_loglevel_t Config::getCSDKLogLevel() {
        return Config::newrelicLogLevel[Config::config[Config::CSDKLogLevelKey]];
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
        if( Config::config[Config::LicenseKey] == ""){
           std::cerr << "New Relic license key not configured! Terminating." << std::endl;
            std::terminate();
        }
        return Config::config[Config::LicenseKey];
    }

    void Config::init(const char *configuration) {
        std::cerr << "Config::init enter configuration string: " << configuration << std::endl;
        //std::fstream file(configuration);
        std::istringstream stream{configuration};
        std::string str;
        for (std::string line; std::getline(stream, line);) {
            // Deal with comments
            line = StringUtils::chomp(line, "#");
            if(line == ""){
                continue;
            }

            auto words = StringUtils::split(line, ':');
            std::cerr << "Config::init loading key: " << words.front() << " value: <" << words.back() << ">" << std::endl;
            // Key is in the conf file
            if (Config::config.count(words.front()) > 0) {
                Config::config[words.front()] = StringUtils::trim(words.back());
            } else {
                std::cerr << "Config::init unknown key: " << words.front() << std::endl;
            }
        }
        for (std::pair<std::string, std::string> element : Config::config) {
            std::cerr << "Config::init loaded key: " << element.first << " value: <" << element.second << ">" << std::endl;
        }
        std::cerr << "Config::init exit" << std::endl;
    }
}