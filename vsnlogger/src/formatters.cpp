/**
 * @file formatters_t.cpp
 * @brief Implementation of message formatting utilities for VSNLogger
 *
 * @details
 * This component provides format processing functionality with deterministic
 * memory utilization and comprehensive bounds checking suitable for
 * automotive embedded systems.
 *
 * @author VSNLogger Contributors
 * @version 1.0.0
 */

#include "vsnlogger/formatters.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace vsn {
namespace logger {
namespace formatters {

/* Helper function to get current timestamp with bounds checking */
static E_Result GetCurrentTimestamp(std::string& result) {
    try {
        auto now = std::chrono::system_clock::now();
        auto timeT = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      now.time_since_epoch()) %
                  1000;

        std::tm timeInfo;

#ifdef _WIN32
        /* Windows-specific thread-safe version */
        if (0 != gmtime_s(&timeInfo, &timeT)) {
            return E_Result::E_UNKNOWN_ERROR;
        }
#else
        /* POSIX thread-safe version */
        if (nullptr == gmtime_r(&timeT, &timeInfo)) {
            return E_Result::E_UNKNOWN_ERROR;
        }
#endif

        /* Format timestamp with stringstream for bounds safety */
        std::stringstream ss;
        ss << std::put_time(&timeInfo, "%Y-%m-%dT%H:%M:%S");
        ss << '.' << std::setfill('0') << std::setw(3) << ms.count() << 'Z';

        result = ss.str();
        return E_Result::E_SUCCESS;
    } catch (...) {
        return E_Result::E_UNKNOWN_ERROR;
    }
}

/* Helper function to JSON escape a string with bounds checking */
static E_Result JsonEscapeString(const std::string& input,
                                 std::string& output) {
    try {
        /* Reserve space to reduce allocations */
        output.clear();
        output.reserve(input.length() + 16U); /* Allow for some escaping */

        for (std::size_t i = 0U; i < input.length(); ++i) {
            const char c = input[i];
            if (c == '\"') {
                output += "\\\"";
            } else if (c == '\\') {
                output += "\\\\";
            } else if (c == '\n') {
                output += "\\n";
            } else if (c == '\r') {
                output += "\\r";
            } else if (c == '\t') {
                output += "\\t";
            } else if (c == '\b') {
                output += "\\b";
            } else if (c == '\f') {
                output += "\\f";
            } else if (static_cast<unsigned char>(c) < 32U) {
                /* Control characters as Unicode escapes */
                char hexBuf[8];
                std::snprintf(hexBuf, sizeof(hexBuf), "\\u%04x",
                              static_cast<unsigned int>(c));
                output += hexBuf;
            } else {
                output += c;
            }
        }

        return E_Result::E_SUCCESS;
    } catch (...) {
        return E_Result::E_UNKNOWN_ERROR;
    }
}

E_Result ToJson(const std::string& message, const std::string& level,
                const std::string& component, std::string& result) {
    std::map<std::string, std::string> empty;
    return ToJson(message, level, component, empty, result);
}

E_Result ToJson(const std::string& message, const std::string& level,
                const std::string& component,
                const std::map<std::string, std::string>& additionalFields,
                std::string& result) {
    try {
        /* Parameter validation */
        if (message.empty() || level.empty()) {
            return E_Result::E_INVALID_PARAMETER;
        }

        /* Get timestamp */
        std::string timestamp;
        E_Result timestampResult = GetCurrentTimestamp(timestamp);
        if (E_Result::E_SUCCESS != timestampResult) {
            return timestampResult;
        }

        /* Prepare JSON output with stringstream for bounds safety */
        std::stringstream json;

        /* Start JSON object */
        json << "{";

        /* Add standard fields */
        json << "\"timestamp\":\"" << timestamp << "\",";
        json << "\"level\":\"";

        /* Escape level string */
        std::string escapedLevel;
        E_Result levelResult = JsonEscapeString(level, escapedLevel);
        if (E_Result::E_SUCCESS != levelResult) {
            return levelResult;
        }
        json << escapedLevel << "\",";

        /* Add component if provided */
        if (!component.empty()) {
            json << "\"component\":\"";

            /* Escape component string */
            std::string escapedComponent;
            E_Result componentResult =
                JsonEscapeString(component, escapedComponent);
            if (E_Result::E_SUCCESS != componentResult) {
                return componentResult;
            }
            json << escapedComponent << "\",";
        }

        /* Add message */
        json << "\"message\":\"";

        /* Escape message string */
        std::string escapedMessage;
        E_Result messageResult = JsonEscapeString(message, escapedMessage);
        if (E_Result::E_SUCCESS != messageResult) {
            return messageResult;
        }
        json << escapedMessage << "\"";

        /* Add additional fields with strict bounds checking */
        if (!additionalFields.empty()) {
            const std::size_t k_maxFields = 32U;
            const std::size_t fieldCount =
                (additionalFields.size() <= k_maxFields)
                    ? additionalFields.size()
                    : k_maxFields;

            std::size_t processedFields = 0U;

            for (const auto& field : additionalFields) {
                if (processedFields >= fieldCount) {
                    break;
                }

                json << ",\"";

                /* Escape key string */
                std::string escapedKey;
                E_Result keyResult = JsonEscapeString(field.first, escapedKey);
                if (E_Result::E_SUCCESS != keyResult) {
                    continue;
                }
                json << escapedKey << "\":\"";

                /* Escape value string */
                std::string escapedValue;
                E_Result valueResult =
                    JsonEscapeString(field.second, escapedValue);
                if (E_Result::E_SUCCESS != valueResult) {
                    continue;
                }
                json << escapedValue << "\"";

                ++processedFields;
            }
        }

        /* Close JSON object */
        json << "}";

        /* Set result */
        result = json.str();

        return E_Result::E_SUCCESS;
    } catch (...) {
        return E_Result::E_UNKNOWN_ERROR;
    }
}

E_Result ToSyslog(const std::string& message, const std::string& level,
                  const std::string& component, std::string& result) {
    try {
        /* Parameter validation */
        if (message.empty() || level.empty()) {
            return E_Result::E_INVALID_PARAMETER;
        }

        /* Get timestamp */
        std::string timestamp;
        E_Result timestampResult = GetCurrentTimestamp(timestamp);
        if (E_Result::E_SUCCESS != timestampResult) {
            return timestampResult;
        }

        /* Prepare syslog output with stringstream for bounds safety */
        std::stringstream ss;

        /* Convert level to syslog priority */
        std::string priority = "6"; /* Default to INFO */

        if (level == "trace" || level == "debug") {
            priority = "7"; /* DEBUG */
        } else if (level == "info") {
            priority = "6"; /* INFO */
        } else if (level == "warn") {
            priority = "4"; /* WARNING */
        } else if (level == "error") {
            priority = "3"; /* ERROR */
        } else if (level == "critical") {
            priority = "2"; /* CRITICAL */
        }

        /* Format: <priority>timestamp component: message */
        ss << "<" << priority << ">" << timestamp << " ";

        /* Add component if provided */
        if (!component.empty()) {
            /* Limit component length for syslog */
            const std::size_t k_maxComponentLength = 32U;
            if (component.length() <= k_maxComponentLength) {
                ss << component;
            } else {
                ss << component.substr(0, k_maxComponentLength);
            }
        } else {
            ss << "vsnlogger";
        }

        ss << ": " << message;

        /* Set result */
        result = ss.str();

        return E_Result::E_SUCCESS;
    } catch (...) {
        return E_Result::E_UNKNOWN_ERROR;
    }
}

E_Result ToConsole(const std::string& message, const std::string& level,
                   const std::string& component, std::string& result) {
    try {
        /* Parameter validation */
        if (message.empty() || level.empty()) {
            return E_Result::E_INVALID_PARAMETER;
        }

        /* Get timestamp */
        std::string timestamp;
        E_Result timestampResult = GetCurrentTimestamp(timestamp);
        if (E_Result::E_SUCCESS != timestampResult) {
            return timestampResult;
        }

        /* Prepare console output with stringstream for bounds safety */
        std::stringstream ss;

        /* Format: [timestamp] [level] [component] message */
        ss << "[" << timestamp << "] ";
        ss << "[" << level << "] ";

        /* Add component if provided */
        if (!component.empty()) {
            ss << "[" << component << "] ";
        }

        ss << message;

        /* Set result */
        result = ss.str();

        return E_Result::E_SUCCESS;
    } catch (...) {
        return E_Result::E_UNKNOWN_ERROR;
    }
}

E_Result GetPattern(E_FormatType formatType, std::string& pattern) {
    try {
        /* Convert enum to string pattern based on type */
        switch (formatType) {
            case E_FormatType::E_JSON:
                pattern =
                    "{\"timestamp\":\"%Y-%m-%dT%H:%M:%S.%fZ\",\"level\":\"%^%l%"
                    "$\","
                    "\"logger\":\"%n\",\"thread\":\"%t\",\"message\":\"%v\"}";
                break;

            case E_FormatType::E_CONSOLE:
                pattern = "%Y-%m-%d %H:%M:%S.%f %z [%^%l%$] [%n] [%t] %v";
                break;

            case E_FormatType::E_SIMPLE:
                pattern = "[%Y-%m-%d %H:%M:%S.%f] [%^%l%$] %v";
                break;

            case E_FormatType::E_MINIMAL:
                pattern = "%^%l%$ %v";
                break;

            case E_FormatType::E_COLORED:
                pattern =
                    "%Y-%m-%d %H:%M:%S.%f %z [%^%-8l%$] [%-10n] [%-5P %-5t] "
                    "[%g:%#] %v";
                break;

            case E_FormatType::E_DETAILED:
                pattern =
                    "%Y-%m-%d %H:%M:%S.%f %z [%^%-8l%$] [%-10n] [%-5P %-5t] "
                    "[%g:%#:%!()] %v";
                break;

            case E_FormatType::E_DEFAULT:
            default:
                pattern = "%Y-%m-%d %H:%M:%S.%f %z [%^%l%$] [%n] [%t] %v";
                break;
        }

        return E_Result::E_SUCCESS;
    } catch (...) {
        return E_Result::E_UNKNOWN_ERROR;
    }
}

E_Result GetPattern(const std::string& formatName, std::string& pattern) {
    try {
        /* Parameter validation */
        if (formatName.empty()) {
            return E_Result::E_INVALID_PARAMETER;
        }

        /* Convert string to enum */
        E_FormatType formatType;

        if (formatName == "json") {
            formatType = E_FormatType::E_JSON;
        } else if (formatName == "console") {
            formatType = E_FormatType::E_CONSOLE;
        } else if (formatName == "simple") {
            formatType = E_FormatType::E_SIMPLE;
        } else if (formatName == "minimal") {
            formatType = E_FormatType::E_MINIMAL;
        } else if (formatName == "colored") {
            formatType = E_FormatType::E_COLORED;
        } else if (formatName == "detailed") {
            formatType = E_FormatType::E_DETAILED;
        } else {
            formatType = E_FormatType::E_DEFAULT;
        }

        /* Get pattern from enum */
        return GetPattern(formatType, pattern);
    } catch (...) {
        return E_Result::E_UNKNOWN_ERROR;
    }
}

} /* namespace formatters */
} /* namespace logger */
} /* namespace vsn */