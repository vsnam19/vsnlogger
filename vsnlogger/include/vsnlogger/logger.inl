/**
 * @file logger.inl
 * @brief Template implementations for the Logger class
 */

#pragma once

// Include full spdlog definitions needed for template implementations
#include <spdlog/spdlog.h>

namespace vsn {
namespace logger {

/* Template method implementations */
template <typename... Args>
E_Result Logger::LogWithLocation(SourceLocation_t loc, E_LogLevel level,
                                 const char* fmt, const Args&... args) {
    try {
        if (!m_logger) {
            return E_Result::E_NOT_INITIALIZED;
        }

        if (!fmt) {
            return E_Result::E_INVALID_PARAMETER;
        }

        /* Format string length check */
        const std::size_t fmtLen = std::strlen(fmt);
        if (fmtLen > Logger::k_maxMessageLength) {
            /* Format string too long */
            return E_Result::E_INVALID_PARAMETER;
        }

        /* Convert to spdlog source location */
        spdlog::source_loc spdlogLoc{
            loc.m_filename, static_cast<int>(loc.m_line), loc.m_function};

        /* Log message with level */
        m_logger->log(spdlogLoc, static_cast<spdlog::level::level_enum>(level),
                      fmt, args...);

        return E_Result::E_SUCCESS;
    } catch (...) {
        return E_Result::E_UNKNOWN_ERROR;
    }
}

/* Level-specific logging method implementations */
template <typename... Args>
E_Result Logger::Trace(SourceLocation_t loc, const char* fmt,
                       const Args&... args) {
    return LogWithLocation(loc, E_LogLevel::E_TRACE, fmt, args...);
}

template <typename... Args>
E_Result Logger::Debug(SourceLocation_t loc, const char* fmt,
                       const Args&... args) {
    return LogWithLocation(loc, E_LogLevel::E_DEBUG, fmt, args...);
}

template <typename... Args>
E_Result Logger::Info(SourceLocation_t loc, const char* fmt,
                      const Args&... args) {
    return LogWithLocation(loc, E_LogLevel::E_INFO, fmt, args...);
}

template <typename... Args>
E_Result Logger::Warn(SourceLocation_t loc, const char* fmt,
                      const Args&... args) {
    return LogWithLocation(loc, E_LogLevel::E_WARN, fmt, args...);
}

template <typename... Args>
E_Result Logger::Error(SourceLocation_t loc, const char* fmt,
                       const Args&... args) {
    return LogWithLocation(loc, E_LogLevel::E_ERROR, fmt, args...);
}

template <typename... Args>
E_Result Logger::Critical(SourceLocation_t loc, const char* fmt,
                          const Args&... args) {
    return LogWithLocation(loc, E_LogLevel::E_CRITICAL, fmt, args...);
}

} /* namespace logger */
} /* namespace vsn */
