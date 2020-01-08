//
// Created by mike on 1/7/20.
//

#ifndef NR_OPENTRACING_CPP_STRING_UTILS_H
#define NR_OPENTRACING_CPP_STRING_UTILS_H


#include <string>
#include <algorithm>
#include <vector>
#include <sstream>

namespace newrelic {
    class StringUtils {
    public:
        static std::string &trim(std::string &str) {
            return ltrim(rtrim(str));
        }

        static std::string &ltrim(std::string &str) {
            auto it2 = std::find_if(str.begin(), str.end(), [](char ch) { return !std::isspace<char>(ch, std::locale::classic()); });
            str.erase(str.begin(), it2);
            return str;
        }

        static std::string &rtrim(std::string &str) {
            auto it1 = std::find_if(str.rbegin(), str.rend(), [](char ch) { return !std::isspace<char>(ch, std::locale::classic()); });
            str.erase(it1.base(), str.end());
            return str;
        }

        static std::string trim_copy(std::string const &str) {
            auto s = str;
            return ltrim(rtrim(s));
        }

        static std::vector<std::string> split(std::string strToSplit, char delimeter) {
            std::stringstream ss(strToSplit);
            std::string item;
            std::vector<std::string> splittedStrings;
            while (std::getline(ss, item, delimeter)) {
                splittedStrings.push_back(item);
            }
            return splittedStrings;
        }

        static std::vector<std::string> split(std::string stringToBeSplitted, std::string delimeter) {
            std::vector<std::string> splittedString;
            int startIndex = 0;
            int endIndex = 0;
            while ((endIndex = stringToBeSplitted.find(delimeter, startIndex)) < stringToBeSplitted.size()) {

                std::string val = stringToBeSplitted.substr(startIndex, endIndex - startIndex);
                splittedString.push_back(val);
                startIndex = endIndex + delimeter.size();

            }
            if (startIndex < stringToBeSplitted.size()) {
                std::string val = stringToBeSplitted.substr(startIndex);
                splittedString.push_back(val);
            }
            return splittedString;

        }

        static std::string toLower(std::string source) {
            auto target{source};
            std::transform(source.begin(), source.end(), target.begin(), ::tolower);
            return target;
        }

        static std::string toUpper(std::string source) {
            auto target{source};
            std::transform(source.begin(), source.end(), target.begin(), ::toupper);
            return target;
        }

        static std::string chomp(std::string source, std::string delim){
            return split(source, delim).front();
        }
    };
}


#endif //NR_OPENTRACING_CPP_STRING_UTILS_H
