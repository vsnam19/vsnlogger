/**
 * @file config.h
 * @brief Configuration management for VSNLogger
 *
 * @details
 * This component provides configuration storage and retrieval mechanisms
 * with MISRA C++ compliance for safety-critical applications.
 *
 * @author VSNLogger Contributors
 * @version 1.0.0
 */

#pragma once

#include <cstdint>
#include <map>
#include <string>

#include "error_codes.h"

namespace vsn {
namespace logger {

/**
 * @brief Configuration options for the logging system
 */
class LogConfig {
   public:
    /**
     * @brief Singleton instance accessor
     *
     * @return Reference to configuration instance
     */
    static LogConfig& GetInstance(void);

    /**
     * @brief Load configuration from file
     *
     * @param[in] configFile Path to configuration file
     * @return Operation result code
     */
    E_Result LoadFromFile(const std::string& configFile);

    /**
     * @brief Load configuration from environment variables
     *
     * @return Operation result code
     */
    E_Result LoadFromEnv(void);

    /**
     * @brief Get string configuration value
     *
     * @param[in] section Configuration section identifier
     * @param[in] key Configuration key
     * @param[in] defaultValue Value to return if key not found
     * @return Configuration value or default
     */
    std::string GetString(const std::string& section, const std::string& key,
                          const std::string& defaultValue);

    /**
     * @brief Get integer configuration value
     *
     * @param[in] section Configuration section identifier
     * @param[in] key Configuration key
     * @param[in] defaultValue Value to return if key not found
     * @return Configuration value or default
     */
    std::int32_t GetInt32(const std::string& section, const std::string& key,
                          std::int32_t defaultValue);

    /**
     * @brief Get boolean configuration value
     *
     * @param[in] section Configuration section identifier
     * @param[in] key Configuration key
     * @param[in] defaultValue Value to return if key not found
     * @return Configuration value or default
     */
    bool GetBool(const std::string& section, const std::string& key,
                 bool defaultValue);

    /**
     * @brief Set configuration value
     *
     * @param[in] section Configuration section identifier
     * @param[in] key Configuration key
     * @param[in] value Configuration value to set
     * @return Operation result code
     */
    E_Result Set(const std::string& section, const std::string& key,
                 const std::string& value);

   private:
    /** Maximum number of configuration sections */
    static constexpr std::uint16_t k_maxSections = 32U;

    /** Maximum number of configuration entries per section */
    static constexpr std::uint16_t k_maxEntriesPerSection = 64U;

    /** Maximum key length in characters */
    static constexpr std::uint16_t k_maxKeyLength = 64U;

    /** Maximum value length in characters */
    static constexpr std::uint16_t k_maxValueLength = 256U;

    /**
     * @brief Private constructor for singleton
     */
    LogConfig(void);

    /* Disable copy and assignment */
    LogConfig(const LogConfig&) = delete;
    LogConfig& operator=(const LogConfig&) = delete;

    /** Configuration storage */
    std::map<std::string, std::map<std::string, std::string>> m_configData;
};

} /* namespace logger */
} /* namespace vsn */
