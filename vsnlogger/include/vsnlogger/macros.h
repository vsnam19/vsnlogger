/**
 * @file macros.h
 * @brief Logging macro definitions for VSNLogger
 *
 * @details
 * This component provides user-facing macros for simplified logging
 * with MISRA C++ compliance for safety-critical applications.
 *
 * @author VSNLogger Contributors
 * @version 1.0.0
 */

#pragma once

#include <spdlog/fmt/chrono.h> /* For chrono types */
#include <spdlog/fmt/ostr.h>   /* For custom types with operator */

#include "logger.h"

/**
 * @brief Extract filename from full path at compile time
 */
#define VSN_FILENAME                              \
    ([]() {                                       \
        static const char* const result = []() {  \
            const char* path = __FILE__;          \
            const char* filename = path;          \
            for (const char* p = path; *p; ++p) { \
                if (*p == '/' || *p == '\\') {    \
                    filename = p + 1;             \
                }                                 \
            }                                     \
            return filename;                      \
        }();                                      \
        return result;                            \
    }())

/**
 * @brief Create source location structure for current position
 */
#define VSN_SRC_LOC                                                  \
    ::vsn::logger::SourceLocation_t {                                \
        VSN_FILENAME, static_cast<std::uint32_t>(__LINE__), __func__ \
    }

/**
 * @brief Basic logging macros with source location information
 */
#define VSN_TRACE(...) \
    ::vsn::logger::Logger::GetDefaultLogger()->Trace(VSN_SRC_LOC, __VA_ARGS__)

#define VSN_DEBUG(...) \
    ::vsn::logger::Logger::GetDefaultLogger()->Debug(VSN_SRC_LOC, __VA_ARGS__)

#define VSN_INFO(...) \
    ::vsn::logger::Logger::GetDefaultLogger()->Info(VSN_SRC_LOC, __VA_ARGS__)

#define VSN_WARN(...) \
    ::vsn::logger::Logger::GetDefaultLogger()->Warn(VSN_SRC_LOC, __VA_ARGS__)

#define VSN_ERROR(...) \
    ::vsn::logger::Logger::GetDefaultLogger()->Error(VSN_SRC_LOC, __VA_ARGS__)

#define VSN_CRITICAL(...)                                            \
    ::vsn::logger::Logger::GetDefaultLogger()->Critical(VSN_SRC_LOC, \
                                                        __VA_ARGS__)

/**
 * @brief Component-specific logging macros (adds component name)
 */
#define VSN_COMPONENT_TRACE(component, ...)           \
    ::vsn::logger::Logger::GetDefaultLogger()->Trace( \
        VSN_SRC_LOC, "[{}] " __VA_ARGS__, component)

#define VSN_COMPONENT_DEBUG(component, ...)           \
    ::vsn::logger::Logger::GetDefaultLogger()->Debug( \
        VSN_SRC_LOC, "[{}] " __VA_ARGS__, component)

#define VSN_COMPONENT_INFO(component, ...)           \
    ::vsn::logger::Logger::GetDefaultLogger()->Info( \
        VSN_SRC_LOC, "[{}] " __VA_ARGS__, component)

#define VSN_COMPONENT_WARN(component, ...)           \
    ::vsn::logger::Logger::GetDefaultLogger()->Warn( \
        VSN_SRC_LOC, "[{}] " __VA_ARGS__, component)

#define VSN_COMPONENT_ERROR(component, ...)           \
    ::vsn::logger::Logger::GetDefaultLogger()->Error( \
        VSN_SRC_LOC, "[{}] " __VA_ARGS__, component)

#define VSN_COMPONENT_CRITICAL(component, ...)           \
    ::vsn::logger::Logger::GetDefaultLogger()->Critical( \
        VSN_SRC_LOC, "[{}] " __VA_ARGS__, component)

/**
 * @brief Initialize logging system
 */
#define VSN_INIT_LOGGING(app_name)                          \
    ::vsn::logger::Logger::Initialize(app_name, "/var/log", \
                                      ::vsn::logger::E_LogLevel::E_INFO)

#define VSN_INIT_LOGGING_WITH_PATH(app_name, log_path)    \
    ::vsn::logger::Logger::Initialize(app_name, log_path, \
                                      ::vsn::logger::E_LogLevel::E_INFO)

#define VSN_INIT_LOGGING_WITH_LEVEL(app_name, log_level) \
    ::vsn::logger::Logger::Initialize(app_name, "/var/log", log_level)

#define VSN_INIT_LOGGING_FULL(app_name, log_path, log_level) \
    ::vsn::logger::Logger::Initialize(app_name, log_path, log_level)

#define VSN_INIT_LOGGING_WITH_CONFIG(app_name, config_file) \
    ::vsn::logger::Logger::InitializeWithConfig(app_name, config_file)

/**
 * @brief Flush and shutdown operations
 */
#define VSN_FLUSH_LOGS() ::vsn::logger::Logger::GetDefaultLogger()->Flush()

#define VSN_SHUTDOWN_LOGGING() ::vsn::logger::Logger::Shutdown()
