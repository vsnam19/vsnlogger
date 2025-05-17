#pragma once

#include <map>
#include <string>

namespace vsn::logger {
namespace formatters {

/**
 * Format log entry as JSON
 */
std::string to_json(const std::string& message, const std::string& level,
                    const std::string& component);

/**
 * Format log entry as JSON with additional fields
 */
std::string to_json(
    const std::string& message, const std::string& level,
    const std::string& component,
    const std::map<std::string, std::string>& additional_fields);

/**
 * Format log entry for syslog
 */
std::string to_syslog(const std::string& message, const std::string& level,
                      const std::string& component);

/**
 * Format log entry for simple console output
 */
std::string to_console(const std::string& message, const std::string& level,
                       const std::string& component);

/**
 * Get spdlog pattern string for specified format
 */
std::string get_pattern(const std::string& format_name);

}  // namespace formatters
}  // namespace vsn::logger
