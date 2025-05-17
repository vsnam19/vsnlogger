#include "vsnlogger/config.h"

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <regex>

namespace vsn::logger {

Config::Config() {
    // Initialize with default global section
    config_data_["global"] = std::map<std::string, std::string>();
}

Config& Config::instance() {
    static Config instance;
    return instance;
}

bool Config::load_from_file(const std::string& config_file) {
    std::ifstream file(config_file);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    std::string current_section = "global";
    std::regex section_regex(R"(\[(.*)\])");
    std::regex key_value_regex(R"(([^=]+)=([^=]*))");

    while (std::getline(file, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }

        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        // Parse section header
        std::smatch section_match;
        if (std::regex_match(line, section_match, section_regex)) {
            current_section = section_match[1].str();
            if (config_data_.find(current_section) == config_data_.end()) {
                config_data_[current_section] =
                    std::map<std::string, std::string>();
            }
            continue;
        }

        // Parse key-value pair
        std::smatch key_value_match;
        if (std::regex_match(line, key_value_match, key_value_regex)) {
            std::string key = key_value_match[1].str();
            std::string value = key_value_match[2].str();

            // Trim key and value
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);

            config_data_[current_section][key] = value;
        }
    }

    return true;
}
bool Config::load_from_env() {
    bool found_any = false;
    std::regex env_regex(R"(^VSNLOG_([^_]+)_([^=]+)$)");

    // Common environment variable prefixes to check
    std::vector<std::string> prefixes = {"VSNLOG_GLOBAL_", "VSNLOG_APP_"};

    // Check known environment variables
    for (const auto& prefix : prefixes) {
        // Try common configuration options
        std::vector<std::string> options = {"LOG_LEVEL", "FORMAT", "FILE_PATH",
                                            "MAX_SIZE"};

        for (const auto& option : options) {
            std::string env_var = prefix + option;
            const char* value = std::getenv(env_var.c_str());

            if (value != nullptr) {
                // Extract section and key using regex
                std::string key = env_var;
                std::smatch env_match;

                if (std::regex_search(key, env_match, env_regex) &&
                    env_match.size() > 2) {
                    std::string section = env_match[1].str();
                    std::string config_key = env_match[2].str();

                    // Convert to lowercase
                    std::transform(
                        section.begin(), section.end(), section.begin(),
                        [](unsigned char c) { return std::tolower(c); });
                    std::transform(config_key.begin(), config_key.end(),
                                   config_key.begin(), [](unsigned char c) {
                                       return std::tolower(c);
                                   });

                    config_data_[section][config_key] = value;
                    found_any = true;
                }
            }
        }
    }

    return found_any;
}

std::string Config::get_string(const std::string& section,
                               const std::string& key,
                               const std::string& default_value) {
    auto section_it = config_data_.find(section);
    if (section_it != config_data_.end()) {
        auto key_it = section_it->second.find(key);
        if (key_it != section_it->second.end()) {
            return key_it->second;
        }
    }

    // Check global section as fallback
    if (section != "global") {
        auto global_it = config_data_.find("global");
        if (global_it != config_data_.end()) {
            auto key_it = global_it->second.find(key);
            if (key_it != global_it->second.end()) {
                return key_it->second;
            }
        }
    }

    return default_value;
}

int Config::get_int(const std::string& section, const std::string& key,
                    int default_value) {
    std::string value = get_string(section, key, "");
    if (!value.empty()) {
        try {
            return std::stoi(value);
        } catch (const std::exception& e) {
            // Conversion failed, return default
            return default_value;
        }
    }
    return default_value;
}

bool Config::get_bool(const std::string& section, const std::string& key,
                      bool default_value) {
    std::string value = get_string(section, key, "");
    if (!value.empty()) {
        std::transform(value.begin(), value.end(), value.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        return (value == "true" || value == "yes" || value == "1" ||
                value == "on");
    }
    return default_value;
}

void Config::set(const std::string& section, const std::string& key,
                 const std::string& value) {
    config_data_[section][key] = value;
}

}  // namespace vsn::logger