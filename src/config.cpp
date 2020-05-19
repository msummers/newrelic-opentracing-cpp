//
// Created by mike on 1/6/20.
//

#include "config.h"
#include "string_utils.h"
#include <unistd.h>
#include <newrelic/libnewrelic.h>
#include <iostream>
#include <fstream>

namespace newrelic {
    // TODO let envvars override. Default - config file - envvars

    const std::string Config::LicenseKey{"newrelicLicense"};
    const std::string Config::AppNameKey{"applicationName"};
    const std::string Config::LogLevelKey{"logLevel"};
    const std::string Config::SegmentCategoryKey{"segmentCategory"};
    const std::string Config::CSDKLogLocationKey{"csdkLogLocation"};
    const std::string Config::CSDKLogLevelKey{"csdkLogLevel"};
    const std::string Config::TransactionFilterFileKey{"transactionFilterFile"};
    const std::string Config::DaemonAddress{"daemonAddress"};
    const std::string Config::DaemonTimeout{"daemonTimeout"};

    std::map<std::string, std::string> Config::config = {{Config::LicenseKey,               ""},
                                                         {Config::AppNameKey,               ""},
                                                         {Config::LogLevelKey,              "ALL"},
                                                         {Config::SegmentCategoryKey,       "nginx"},
                                                         {Config::TransactionFilterFileKey, "/etc/newrelic/nginxTransactionFilters.txt"},
                                                         {Config::CSDKLogLevelKey,          "INFO"},
                                                         {Config::DaemonAddress,            "/tmp/.newrelic.sock"},
                                                         {Config::DaemonTimeout,            "0"},
                                                         {Config::CSDKLogLocationKey,       "./c_sdk.log"}};

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

    std::vector<std::regex> Config::skipExpressions;
    std::vector<std::pair<std::regex, std::string>> Config::replaceExpressions;

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
        if (appName.empty()) {
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
        if (Config::config[Config::LicenseKey].empty()) {
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
            if (line.empty()) {
                continue;
            }

            auto words = StringUtils::split(line, '=');
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
        processTransactionFilters();
        Log::trace("Config::init exit");
    }

    void Config::processTransactionFilters() {
        // Read the entire regex config file as binary
        try {
            std::streampos size;
            char *memblock;
            std::ifstream file(Config::config[Config::TransactionFilterFileKey], std::ios::in | std::ios::binary | std::ios::ate);
            if (file.is_open()) {
                size = file.tellg();
                memblock = new char[size];
                file.seekg(0, std::ios::beg);
                file.read(memblock, size);
                file.close();
                std::string line;
                for (int i = 0; i < size; i++) {
                    // See if we have a complete line
                    if (memblock[i] == '\n') {
                        // Get rid of comments
                        Log::debug("Config::processTransactionFilters line: {}", line);
                        line = StringUtils::chomp(line, "#");
                        if (line.empty()) {
                            line.clear();
                            continue;
                        }

                        auto regexStart = line.find("regex:");
                        if (regexStart == std::string::npos) {
                            Log::error("Config::processTransactionFilters line must start with regex: Line: {}", line);
                            line.clear();
                            continue;
                        }

                        auto cmdStart = line.find(" replace:");
                        if (cmdStart == std::string::npos) {
                            cmdStart = line.find(" skip:");
                            if (cmdStart == std::string::npos) {
                                Log::error("Config::processTransactionFilters line must valid command- replace: or skip: Line: {}", line);
                                line.clear();
                                continue;
                            }

                            // Process skip:
                            auto regex = line.substr(6, (cmdStart - 6));
                            try {
                                skipExpressions.emplace_back(std::regex{regex});
                                Log::debug("Config::processTransactionFilters regex: {} skip:", regex);
                            } catch (std::regex_error &e) {
                                Log::error("Config::processTransactionFilters error compiling regex. regex: {} skip:", regex);
                            }
                            Log::debug("Config::processTransactionFilters regex: {} skip:", regex);
                        } else {
                            // Process replace:
                            auto regex = line.substr(6, (cmdStart - 6));
                            auto replace = line.substr(cmdStart + 9, line.length() - cmdStart - 8);
                            try {
                                auto value = std::pair<std::regex, std::string>{std::regex{regex}, replace};
                                replaceExpressions.emplace_back(value);
                                Log::debug("Config::processTransactionFilters regex: {} replace: {}", regex, replace);
                            } catch (std::regex_error &e) {
                                Log::error("Config::processTransactionFilters error compiling regex. regex: {} replace: {}", regex, replace);
                            }
                        }
                        line.clear();
                    } else {
                        // Build-up a line
                        line.append(1, memblock[i]);
                    }
                }
                delete[] memblock;
            }
        } catch (...) {
            std::exception_ptr p = std::current_exception();
            Log::error("Config::processTransactionFilters exception: {}", p.__cxa_exception_type()->name());
        }
    }

    bool Config::skipTransaction(std::string txn) {
        std::smatch match;
        for (const auto &regex : skipExpressions) {
            if (std::regex_match(txn, match, regex)) {
                return true;
            }
        }
        return false;
    }

    std::string Config::filterTransaction(std::string txn) {
        std::smatch match;
        for (const auto &pair : replaceExpressions) {
            if (std::regex_search(txn, match, pair.first)) {
                return std::regex_replace(txn, pair.first, pair.second);
            }
        }
        return txn;
    }

    const char *Config::getDaemonSocket() {
        Log::debug("getDaemonSocket: {}", Config::config[Config::DaemonAddress]);
        return Config::config[Config::DaemonAddress].c_str();
    }

    int Config::getConnectionTimeout() {
        Log::debug("getConnectionTimeout: {}", Config::config[Config::DaemonTimeout]);
        try {
            return std::stoi(Config::config[Config::DaemonTimeout]);
        } catch (...) {
            Log::warn("Config::getConnectionTimeout invalid daemonTimeout value {} using 0.", Config::config[Config::DaemonTimeout]);
        }
        return 0;
    }
}