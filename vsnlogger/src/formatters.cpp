#include "vsnlogger/formatters.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <map>
#include <sstream>

namespace vsn::logger {
namespace formatters {

std::string get_pattern(const std::string& format_name) {
    if (format_name == "json") {
        return "{\"timestamp\":\"%Y-%m-%dT%H:%M:%S.%fZ\",\"level\":\"%^%l%$\","
               "\"logger\":\"%n\",\"thread\":\"%t\",\"message\":\"%v\"}";
    } else if (format_name == "console") {
        return "%Y-%m-%d %H:%M:%S.%f %z [%^%l%$] [%n] [%t] %v";
    } else if (format_name == "simple") {
        return "[%Y-%m-%d %H:%M:%S.%f] [%^%l%$] %v";
    } else if (format_name == "minimal") {
        return "%^%l%$ %v";
    } else if (format_name == "colored") {
        // New colored format
        return "%Y-%m-%d %H:%M:%S.%f %z [%^%-8l%$] [%-10n] [%-5P %-5t] "
               "[%g:%#] %v";
    } else if (format_name == "detailed") {
        return "%Y-%m-%d %H:%M:%S.%f %z [%^%-8l%$] [%-10n] [%-5P %-5t] "
               "[%g:%#:%!()] %v";
    } else {
        // Default format
        return "%Y-%m-%d %H:%M:%S.%f %z [%^%l%$] [%n] [%t] %v";
    }
}
// Helper function to get current timestamp
std::string get_current_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                  now.time_since_epoch()) %
              1000;

    std::stringstream ss;
    ss << std::put_time(std::gmtime(&time_t_now), "%Y-%m-%dT%H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count() << 'Z';

    return ss.str();
}

std::string to_json(const std::string& message, const std::string& level,
                    const std::string& component) {
    std::map<std::string, std::string> empty;
    return to_json(message, level, component, empty);
}

std::string to_json(
    const std::string& message, const std::string& level,
    const std::string& component,
    const std::map<std::string, std::string>& additional_fields) {
    std::stringstream json;

    json << "{";
    json << "\"timestamp\":\"" << get_current_timestamp() << "\",";
    json << "\"level\":\"" << level << "\",";
    json << "\"component\":\"" << component << "\",";
    json << "\"message\":\"";

    // Escape quotation marks in message
    for (char c : message) {
        if (c == '\"')
            json << "\\\"";
        else if (c == '\\')
            json << "\\\\";
        else
            json << c;
    }
    json << "\"";

    // Add additional fields
    for (const auto& field : additional_fields) {
        json << ",\"" << field.first << "\":\"";
        // Escape quotation marks in value
        for (char c : field.second) {
            if (c == '\"')
                json << "\\\"";
            else if (c == '\\')
                json << "\\\\";
            else
                json << c;
        }
        json << "\"";
    }

    json << "}";

    return json.str();
}

std::string to_syslog(const std::string& message, const std::string& level,
                      const std::string& component) {
    std::stringstream ss;

    // Convert level to syslog priority
    std::string priority;
    if (level == "trace" || level == "debug")
        priority = "7";  // DEBUG
    else if (level == "info")
        priority = "6";  // INFO
    else if (level == "warn")
        priority = "4";  // WARNING
    else if (level == "error")
        priority = "3";  // ERROR
    else if (level == "critical")
        priority = "2";  // CRITICAL
    else
        priority = "6";  // Default to INFO

    // <priority>timestamp component: message
    ss << "<" << priority << ">" << get_current_timestamp() << " " << component
       << ": " << message;

    return ss.str();
}

std::string to_console(const std::string& message, const std::string& level,
                       const std::string& component) {
    std::stringstream ss;

    ss << "[" << get_current_timestamp() << "] "
       << "[" << level << "] "
       << "[" << component << "] " << message;

    return ss.str();
}

}  // namespace formatters
}  // namespace vsn::logger