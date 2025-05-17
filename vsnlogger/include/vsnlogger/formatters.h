/**
 * @file formatters.h
 * @brief Message formatting utilities for VSNLogger
 *
 * @details
 * This component provides formatting functions for log messages
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
namespace formatters {

/**
 * @brief Format types supported by the logger
 */
enum class E_FormatType : std::uint8_t {
    E_JSON = 0U,
    E_CONSOLE = 1U,
    E_SIMPLE = 2U,
    E_MINIMAL = 3U,
    E_COLORED = 4U,
    E_DETAILED = 5U,
    E_DEFAULT = 6U
};

/**
 * @brief Format log entry as JSON
 *
 * @param[in] message Log message content
 * @param[in] level Severity level string
 * @param[in] component Component identifier
 * @param[out] result Formatted output string
 * @return Operation result code
 */
E_Result ToJson(const std::string& message, const std::string& level,
                const std::string& component, std::string& result);

/**
 * @brief Format log entry as JSON with additional fields
 *
 * @param[in] message Log message content
 * @param[in] level Severity level string
 * @param[in] component Component identifier
 * @param[in] additionalFields Additional key-value pairs to include
 * @param[out] result Formatted output string
 * @return Operation result code
 */
E_Result ToJson(const std::string& message, const std::string& level,
                const std::string& component,
                const std::map<std::string, std::string>& additionalFields,
                std::string& result);

/**
 * @brief Format log entry for syslog
 *
 * @param[in] message Log message content
 * @param[in] level Severity level string
 * @param[in] component Component identifier
 * @param[out] result Formatted output string
 * @return Operation result code
 */
E_Result ToSyslog(const std::string& message, const std::string& level,
                  const std::string& component, std::string& result);

/**
 * @brief Format log entry for console output
 *
 * @param[in] message Log message content
 * @param[in] level Severity level string
 * @param[in] component Component identifier
 * @param[out] result Formatted output string
 * @return Operation result code
 */
E_Result ToConsole(const std::string& message, const std::string& level,
                   const std::string& component, std::string& result);

/**
 * @brief Get spdlog pattern string for specified format
 *
 * @param[in] formatType Format type identifier
 * @param[out] pattern Pattern string for spdlog
 * @return Operation result code
 */
E_Result GetPattern(E_FormatType formatType, std::string& pattern);

/**
 * @brief Get spdlog pattern string for specified format name
 *
 * @param[in] formatName Format name string
 * @param[out] pattern Pattern string for spdlog
 * @return Operation result code
 */
E_Result GetPattern(const std::string& formatName, std::string& pattern);

} /* namespace formatters */
} /* namespace logger */
} /* namespace vsn */
