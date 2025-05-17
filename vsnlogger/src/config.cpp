/**
 * @file config_t.cpp
 * @brief Implementation of configuration management for VSNLogger
 *
 * @details
 * This component provides robust configuration storage and retrieval mechanisms
 * with thread-safety guarantees and explicit resource constraints suitable
 * for safety-critical applications.
 *
 * @author VSNLogger Contributors
 * @version 1.0.0
 */

#include "vsnlogger/config.h"

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <mutex>
#include <regex>

namespace vsn {
namespace logger {

/* Thread synchronization for singleton access */
static std::mutex g_configMutex;

/* Allocation tracking */
static std::uint32_t g_sectionCount = 0U;

LogConfig::LogConfig(void) {
    /* Initialize with default global section */
    m_configData["global"] = std::map<std::string, std::string>();
    g_sectionCount = 1U;
}

LogConfig& LogConfig::GetInstance(void) {
    /* Thread-safe singleton implementation using C++11 static initialization */
    static LogConfig instance;
    return instance;
}

E_Result LogConfig::LoadFromFile(const std::string& configFile) {
    /* Parameter validation */
    if (configFile.empty()) {
        return E_Result::E_INVALID_PARAMETER;
    }

    /* Open configuration file */
    std::ifstream file(configFile);
    if (!file.is_open()) {
        return E_Result::E_FILE_ERROR;
    }

    /* Thread synchronization for data access */
    std::lock_guard<std::mutex> lock(g_configMutex);

    std::string line;
    std::string currentSection = "global";
    std::regex sectionRegex(R"(\[(.*)\])");
    std::regex keyValueRegex(R"(([^=]+)=([^=]*))");

    while (std::getline(file, line)) {
        /* Skip comments and empty lines */
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }

        /* Trim whitespace */
        const std::size_t firstNonSpace = line.find_first_not_of(" \t");
        const std::size_t lastNonSpace = line.find_last_not_of(" \t");

        if (firstNonSpace == std::string::npos) {
            /* Line contains only whitespace */
            continue;
        }

        line = line.substr(firstNonSpace, lastNonSpace - firstNonSpace + 1);

        /* Parse section header */
        std::smatch sectionMatch;
        if (std::regex_match(line, sectionMatch, sectionRegex)) {
            /* Check section count limit */
            if (g_sectionCount >= k_maxSections) {
                return E_Result::E_RESOURCE_UNAVAILABLE;
            }

            currentSection = sectionMatch[1].str();
            if (m_configData.find(currentSection) == m_configData.end()) {
                m_configData[currentSection] =
                    std::map<std::string, std::string>();
                ++g_sectionCount;
            }
            continue;
        }

        /* Parse key-value pair */
        std::smatch keyValueMatch;
        if (std::regex_match(line, keyValueMatch, keyValueRegex)) {
            std::string key = keyValueMatch[1].str();
            std::string value = keyValueMatch[2].str();

            /* Trim key and value */
            const std::size_t keyFirstNonSpace = key.find_first_not_of(" \t");
            const std::size_t keyLastNonSpace = key.find_last_not_of(" \t");
            const std::size_t valueFirstNonSpace =
                value.find_first_not_of(" \t");
            const std::size_t valueLastNonSpace = value.find_last_not_of(" \t");

            /* Apply bounds checking */
            if (keyFirstNonSpace != std::string::npos &&
                keyLastNonSpace != std::string::npos) {
                key = key.substr(keyFirstNonSpace,
                                 keyLastNonSpace - keyFirstNonSpace + 1);
            } else {
                /* Key is empty or whitespace only */
                continue;
            }

            if (valueFirstNonSpace != std::string::npos &&
                valueLastNonSpace != std::string::npos) {
                value =
                    value.substr(valueFirstNonSpace,
                                 valueLastNonSpace - valueFirstNonSpace + 1);
            } else {
                /* Value is empty or whitespace only, which is still valid */
                value = "";
            }

            /* Check key length limit */
            if (key.length() > k_maxKeyLength) {
                key = key.substr(0, k_maxKeyLength);
            }

            /* Check value length limit */
            if (value.length() > k_maxValueLength) {
                value = value.substr(0, k_maxValueLength);
            }

            /* Check entry count limit for this section */
            if (m_configData[currentSection].size() >= k_maxEntriesPerSection) {
                continue;
            }

            m_configData[currentSection][key] = value;
        }
    }

    return E_Result::E_SUCCESS;
}

E_Result LogConfig::LoadFromEnv(void) {
    /* Thread synchronization for data access */
    std::lock_guard<std::mutex> lock(g_configMutex);

    bool foundAny = false;
    std::regex envRegex(R"(^VSNLOG_([^_]+)_([^=]+)$)");

    /* Common environment variable prefixes to check */
    std::vector<std::string> prefixes = {"VSNLOG_GLOBAL_", "VSNLOG_APP_"};

    /* Try common configuration options */
    std::vector<std::string> options = {"LOG_LEVEL", "FORMAT", "FILE_PATH",
                                        "MAX_SIZE"};

    for (std::size_t i = 0U; i < prefixes.size(); ++i) {
        const auto& prefix = prefixes[i];

        for (std::size_t j = 0U; j < options.size(); ++j) {
            const auto& option = options[j];

            std::string envVar = prefix + option;
            const char* value = std::getenv(envVar.c_str());

            if (value != nullptr) {
                /* Extract section and key using regex */
                std::string key = envVar;
                std::smatch envMatch;

                if (std::regex_search(key, envMatch, envRegex) &&
                    envMatch.size() > 2) {
                    std::string section = envMatch[1].str();
                    std::string configKey = envMatch[2].str();

                    /* Convert to lowercase */
                    std::transform(
                        section.begin(), section.end(), section.begin(),
                        [](unsigned char c) {
                            return static_cast<char>(std::tolower(c));
                        });

                    std::transform(
                        configKey.begin(), configKey.end(), configKey.begin(),
                        [](unsigned char c) {
                            return static_cast<char>(std::tolower(c));
                        });

                    /* Check section count limit */
                    if (m_configData.find(section) == m_configData.end()) {
                        if (g_sectionCount >= k_maxSections) {
                            return E_Result::E_RESOURCE_UNAVAILABLE;
                        }

                        m_configData[section] =
                            std::map<std::string, std::string>();
                        ++g_sectionCount;
                    }

                    /* Check entry count limit for this section */
                    if (m_configData[section].size() >=
                        k_maxEntriesPerSection) {
                        continue;
                    }

                    /* Check key and value length limits */
                    if (configKey.length() > k_maxKeyLength) {
                        configKey = configKey.substr(0, k_maxKeyLength);
                    }

                    std::string valueStr = value;
                    if (valueStr.length() > k_maxValueLength) {
                        valueStr = valueStr.substr(0, k_maxValueLength);
                    }

                    m_configData[section][configKey] = valueStr;
                    foundAny = true;
                }
            }
        }
    }

    return foundAny ? E_Result::E_SUCCESS : E_Result::E_NOT_INITIALIZED;
}

std::string LogConfig::GetString(const std::string& section,
                                 const std::string& key,
                                 const std::string& defaultValue) {
    /* Thread synchronization for data access */
    std::lock_guard<std::mutex> lock(g_configMutex);

    /* Validate parameters */
    if (section.empty() || key.empty()) {
        return defaultValue;
    }

    /* Check if section exists */
    auto sectionIt = m_configData.find(section);
    if (sectionIt != m_configData.end()) {
        /* Check if key exists in section */
        auto keyIt = sectionIt->second.find(key);
        if (keyIt != sectionIt->second.end()) {
            return keyIt->second;
        }
    }

    /* Check global section as fallback */
    if (section != "global") {
        auto globalIt = m_configData.find("global");
        if (globalIt != m_configData.end()) {
            auto keyIt = globalIt->second.find(key);
            if (keyIt != globalIt->second.end()) {
                return keyIt->second;
            }
        }
    }

    return defaultValue;
}

std::int32_t LogConfig::GetInt32(const std::string& section,
                                 const std::string& key,
                                 std::int32_t defaultValue) {
    /* Get value as string */
    std::string value = GetString(section, key, "");
    if (value.empty()) {
        return defaultValue;
    }

    /* Convert to integer */
    try {
        return std::stoi(value);
    } catch (...) {
        /* Conversion failed, return default */
        return defaultValue;
    }
}

bool LogConfig::GetBool(const std::string& section, const std::string& key,
                        bool defaultValue) {
    /* Get value as string */
    std::string value = GetString(section, key, "");
    if (value.empty()) {
        return defaultValue;
    }

    /* Convert to lowercase for case-insensitive comparison */
    std::transform(
        value.begin(), value.end(), value.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    /* Check for common boolean true values */
    return (value == "true" || value == "yes" || value == "1" || value == "on");
}

E_Result LogConfig::Set(const std::string& section, const std::string& key,
                        const std::string& value) {
    /* Thread synchronization for data access */
    std::lock_guard<std::mutex> lock(g_configMutex);

    /* Validate parameters */
    if (section.empty() || key.empty()) {
        return E_Result::E_INVALID_PARAMETER;
    }

    /* Check key and value length limits */
    if (key.length() > k_maxKeyLength || value.length() > k_maxValueLength) {
        return E_Result::E_INVALID_PARAMETER;
    }

    /* Check if section exists, create if not */
    if (m_configData.find(section) == m_configData.end()) {
        /* Check section count limit */
        if (g_sectionCount >= k_maxSections) {
            return E_Result::E_RESOURCE_UNAVAILABLE;
        }

        m_configData[section] = std::map<std::string, std::string>();
        ++g_sectionCount;
    }

    /* Check entry count limit for this section */
    if (m_configData[section].find(key) == m_configData[section].end() &&
        m_configData[section].size() >= k_maxEntriesPerSection) {
        return E_Result::E_RESOURCE_UNAVAILABLE;
    }

    /* Store the value */
    m_configData[section][key] = value;

    return E_Result::E_SUCCESS;
}

} /* namespace logger */
} /* namespace vsn */