#pragma once

#include <spdlog/fmt/chrono.h>  // For chrono types
#include <spdlog/fmt/ostr.h>    // For custom types with operator

#include <algorithm>
#include <string>

#include "vsnlogger/logger.h"

#define __FILENAME__                              \
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

// Basic logging macros with source location information
#define VSN_TRACE(...)                                               \
    ::vsn::logger::Logger::default_logger()->trace(                  \
        spdlog::source_loc{__FILENAME__, __LINE__, SPDLOG_FUNCTION}, \
        __VA_ARGS__)

#define VSN_DEBUG(...)                                               \
    ::vsn::logger::Logger::default_logger()->debug(                  \
        spdlog::source_loc{__FILENAME__, __LINE__, SPDLOG_FUNCTION}, \
        __VA_ARGS__)

#define VSN_INFO(...)                                                \
    ::vsn::logger::Logger::default_logger()->info(                   \
        spdlog::source_loc{__FILENAME__, __LINE__, SPDLOG_FUNCTION}, \
        __VA_ARGS__)

#define VSN_WARN(...)                                                \
    ::vsn::logger::Logger::default_logger()->warn(                   \
        spdlog::source_loc{__FILENAME__, __LINE__, SPDLOG_FUNCTION}, \
        __VA_ARGS__)

#define VSN_ERROR(...)                                               \
    ::vsn::logger::Logger::default_logger()->error(                  \
        spdlog::source_loc{__FILENAME__, __LINE__, SPDLOG_FUNCTION}, \
        __VA_ARGS__)

#define VSN_CRITICAL(...)                                            \
    ::vsn::logger::Logger::default_logger()->critical(               \
        spdlog::source_loc{__FILENAME__, __LINE__, SPDLOG_FUNCTION}, \
        __VA_ARGS__)

// Component-specific logging macros (adds component name)
#define VSN_COMPONENT_TRACE(component, ...)                          \
    ::vsn::logger::Logger::default_logger()->trace(                  \
        spdlog::source_loc{__FILENAME__, __LINE__, SPDLOG_FUNCTION}, \
        "[{}] " __VA_ARGS__, component)
#define VSN_COMPONENT_DEBUG(component, ...)                          \
    ::vsn::logger::Logger::default_logger()->debug(                  \
        spdlog::source_loc{__FILENAME__, __LINE__, SPDLOG_FUNCTION}, \
        "[{}] " __VA_ARGS__, component)
#define VSN_COMPONENT_INFO(component, ...)                           \
    ::vsn::logger::Logger::default_logger()->info(                   \
        spdlog::source_loc{__FILENAME__, __LINE__, SPDLOG_FUNCTION}, \
        "[{}] " __VA_ARGS__, component)
#define VSN_COMPONENT_WARN(component, ...)                           \
    ::vsn::logger::Logger::default_logger()->warn(                   \
        spdlog::source_loc{__FILENAME__, __LINE__, SPDLOG_FUNCTION}, \
        "[{}] " __VA_ARGS__, component)
#define VSN_COMPONENT_ERROR(component, ...)                          \
    ::vsn::logger::Logger::default_logger()->error(                  \
        spdlog::source_loc{__FILENAME__, __LINE__, SPDLOG_FUNCTION}, \
        "[{}] " __VA_ARGS__, component)
#define VSN_COMPONENT_CRITICAL(component, ...)                       \
    ::vsn::logger::Logger::default_logger()->critical(               \
        spdlog::source_loc{__FILENAME__, __LINE__, SPDLOG_FUNCTION}, \
        "[{}] " __VA_ARGS__, component)

// Initialize logging system
#define VSN_INIT_LOGGING(app_name) ::vsn::logger::Logger::initialize(app_name)
#define VSN_INIT_LOGGING_WITH_PATH(app_name, log_path) \
    ::vsn::logger::Logger::initialize(app_name, log_path)
#define VSN_INIT_LOGGING_WITH_LEVEL(app_name, log_level) \
    ::vsn::logger::Logger::initialize(app_name, "/var/log", log_level)
#define VSN_INIT_LOGGING_FULL(app_name, log_path, log_level) \
    ::vsn::logger::Logger::initialize(app_name, log_path, log_level)
#define VSN_INIT_LOGGING_WITH_CONFIG(app_name, config_file) \
    ::vsn::logger::Logger::initialize_with_config(app_name, config_file)

// Flush and shutdown
#define VSN_FLUSH_LOGS() ::vsn::logger::Logger::default_logger()->flush()
#define VSN_SHUTDOWN_LOGGING() ::vsn::logger::Logger::shutdown()