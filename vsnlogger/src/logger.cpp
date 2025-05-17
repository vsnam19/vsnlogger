/**
 * @file logger.cpp
 * @brief Implementation of Logger class
 */

#include "vsnlogger/logger.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <atomic>
#include <filesystem>
#include <iostream>
#include <mutex>

#include "vsnlogger/config.h"
#include "vsnlogger/formatters.h"
#include "vsnlogger/sinks.h"

namespace vsn {
namespace logger {

/* Initialize static members */
std::shared_ptr<Logger> Logger::ms_defaultInstance = nullptr;
std::uint32_t Logger::ms_allocationCount = 0U;

/* Thread synchronization for singleton access */
static std::mutex g_loggerMutex;

Logger::Logger(const std::string& name) {
    try {
        /* Check allocation limits */
        if (Logger::ms_allocationCount >= 32U) {
            throw std::runtime_error("Maximum logger instances exceeded");
        }

        /* Try to retrieve existing logger first */
        m_logger = spdlog::get(name);

        if (!m_logger) {
            /* Create console sink using sinks utility */
            auto consoleSink = sinks::CreateConsoleSink(true);
            if (!consoleSink) {
                throw std::runtime_error("Failed to create console sink");
            }

            m_logger = std::make_shared<spdlog::logger>(name, consoleSink);
            if (!m_logger) {
                throw std::runtime_error("Failed to create logger instance");
            }

            spdlog::register_logger(m_logger);
            ++Logger::ms_allocationCount;
        }
    } catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "Logger initialization failed: " << ex.what() << std::endl;
        /* Fallback to console-only */
        m_logger = spdlog::stdout_color_mt(name);
        ++Logger::ms_allocationCount;
    }
}

Logger::Logger(const std::string& name, const std::string& logFilePath) {
    try {
        /* Check allocation limits */
        if (Logger::ms_allocationCount >= 32U) {
            throw std::runtime_error("Maximum logger instances exceeded");
        }

        /* Try to retrieve existing logger first */
        m_logger = spdlog::get(name);

        if (!m_logger) {
            /* Use the create_multi_sink function instead of direct creation */
            std::vector<std::shared_ptr<spdlog::sinks::sink>> sinksVec =
                sinks::CreateMultiSink(true, logFilePath, false);

            if (sinksVec.empty()) {
                throw std::runtime_error("Failed to create sinks");
            }

            /* Enforce sink count limit */
            const std::size_t sinkCount =
                (sinksVec.size() <= Logger::k_maxSinks) ? sinksVec.size()
                                                        : Logger::k_maxSinks;

            m_logger = std::make_shared<spdlog::logger>(
                name, sinksVec.begin(),
                sinksVec.begin() + static_cast<int32_t>(sinkCount));
            if (!m_logger) {
                throw std::runtime_error("Failed to create logger instance");
            }

            spdlog::register_logger(m_logger);
            ++Logger::ms_allocationCount;
        }
    } catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "Logger initialization failed: " << ex.what() << std::endl;
        /* Fallback to console-only */
        m_logger = spdlog::stdout_color_mt(name);
        ++Logger::ms_allocationCount;
    }
}

/* Create a non-registering constructor for use in initialize */
Logger::Logger(std::shared_ptr<spdlog::logger> existingLogger) {
    if (!existingLogger) {
        throw std::invalid_argument("Null logger instance provided");
    }

    m_logger = existingLogger;
    /* Note: We don't increment allocation count for wrapper instances */
}

E_Result Logger::Initialize(const std::string& appName,
                            const std::string& logDir, E_LogLevel level) {
    /* Thread synchronization for singleton initialization */
    std::lock_guard<std::mutex> lock(g_loggerMutex);

    try {
        /* Input validation */
        if (appName.empty() || logDir.empty()) {
            return E_Result::E_INVALID_PARAMETER;
        }

        /* Get configuration instance */
        auto& config = LogConfig::GetInstance();

        /* Load potential environment variables */
        const E_Result envResult = config.LoadFromEnv();
        if (E_Result::E_SUCCESS != envResult &&
            E_Result::E_NOT_INITIALIZED != envResult) {
            /* Non-critical error, continue with defaults */
            std::cerr << "Warning: Failed to load environment variables"
                      << std::endl;
        }

        /* Get configuration parameters, with defaults from the provided
         * arguments */
        const std::string configuredLogDir =
            config.GetString("global", "log_dir", logDir);

        const bool useConsole = config.GetBool(appName, "console_output", true);
        const bool useFile = config.GetBool(appName, "file_output", true);
        const bool useSyslog = config.GetBool(appName, "syslog_output", false);
        const std::string patternName =
            config.GetString(appName, "log_pattern", "colored");
        const bool useColors = config.GetBool(appName, "use_colors", true);

        /* Configure log file size/count */
        const std::int32_t fileMaxSize =
            config.GetInt32(appName, "max_file_size", 10 * 1024 * 1024);
        const std::int32_t fileMaxCount =
            config.GetInt32(appName, "max_files", 5);

        /* Convert level from config if provided */
        const E_LogLevel configuredLevel =
            static_cast<E_LogLevel>(config.GetInt32(
                appName, "log_level", static_cast<std::int32_t>(level)));

        /* Create log file path with bounds checking */
        std::string logFilePath;
        if (useFile) {
            /* Enforce directory + application name + extension length limits */
            const std::size_t maxPathLen = 255;
            const std::size_t dirLen = configuredLogDir.length();
            const std::size_t appNameLen = appName.length();
            const std::size_t extensionLen = 4; /* ".log" */

            if ((dirLen + appNameLen + extensionLen + 2) > maxPathLen) {
                return E_Result::E_INVALID_PARAMETER;
            }

            logFilePath =
                configuredLogDir + "/" + appName + "/" + appName + ".log";
        }

        /* Check if a logger with this name already exists */
        auto existingLogger = spdlog::get(appName);
        if (existingLogger) {
            /* Use the existing logger, just update its configuration */
            ms_defaultInstance = std::make_shared<Logger>(existingLogger);
        } else {
            /* Build a vector of sinks based on configuration */
            std::vector<std::shared_ptr<spdlog::sinks::sink>> sinkVec;

            /* Add console sink if configured */
            if (useConsole) {
                auto consoleSink = sinks::CreateConsoleSink(useColors);
                if (consoleSink) {
                    sinkVec.push_back(consoleSink);
                }
            }

            /* Add file sink if configured */
            if (useFile && !logFilePath.empty()) {
                auto fileSink = sinks::CreateFileSink(
                    logFilePath, true, static_cast<std::size_t>(fileMaxSize),
                    static_cast<std::size_t>(fileMaxCount));

                if (fileSink) {
                    sinkVec.push_back(fileSink);
                }
            }

            /* Add syslog sink if configured */
            if (useSyslog) {
                auto syslogSink =
                    sinks::CreateSyslogSink("vsnlogger", 0, 0, true);
                if (syslogSink) {
                    sinkVec.push_back(syslogSink);
                }
            }

            /* If no sinks were added, add a default console sink */
            if (sinkVec.empty()) {
                auto defaultSink = sinks::CreateConsoleSink(useColors);
                if (defaultSink) {
                    sinkVec.push_back(defaultSink);
                }
            }

            /* Enforce sink count limit */
            const std::size_t sinkCount = (sinkVec.size() <= Logger::k_maxSinks)
                                              ? sinkVec.size()
                                              : Logger::k_maxSinks;

            /* Create logger with the sinks */
            auto logger = std::make_shared<spdlog::logger>(
                appName, sinkVec.begin(),
                sinkVec.begin() + static_cast<int32_t>(sinkCount));

            if (!logger) {
                return E_Result::E_ALLOCATION_FAILED;
            }

            /* Register the logger with spdlog */
            spdlog::register_logger(logger);

            /* Create our wrapper without re-registering */
            ms_defaultInstance = std::make_shared<Logger>(logger);
        }

        /* Set pattern using formatter helper */
        std::string pattern;
        const E_Result patternResult =
            formatters::GetPattern(patternName, pattern);

        if (E_Result::E_SUCCESS == patternResult && !pattern.empty()) {
            spdlog::set_pattern(pattern);
        } else {
            /* Default pattern if formatter failed */
            spdlog::set_pattern(
                "%Y-%m-%d %H:%M:%S.%f %z  [%^%-8l%$] [%-10n] [%-5P %-5t] "
                "[%g:%#] %v");
        }

        /* Set level */
        spdlog::set_level(
            static_cast<spdlog::level::level_enum>(configuredLevel));

        /* Configure colors for console sinks if using an existing logger */
        if (useColors && useConsole) {
            auto sinks = ms_defaultInstance->m_logger->sinks();
            for (auto& sink : sinks) {
                auto consoleSink = std::dynamic_pointer_cast<
                    spdlog::sinks::stdout_color_sink_mt>(sink);

                if (consoleSink) {
                    consoleSink->set_color(spdlog::level::trace,
                                           "\033[36m"); /* Cyan */
                    consoleSink->set_color(spdlog::level::debug,
                                           "\033[92m"); /* Bright Green */
                    consoleSink->set_color(spdlog::level::info,
                                           "\033[97m"); /* Bright White */
                    consoleSink->set_color(spdlog::level::warn,
                                           "\033[93m"); /* Bright Yellow */
                    consoleSink->set_color(spdlog::level::err,
                                           "\033[91m"); /* Bright Red */
                    consoleSink->set_color(spdlog::level::critical,
                                           "\033[97;41m"); /* White on Red */
                    break; /* Only configure the first console sink */
                }
            }
        }

        /* Log initialization message */
        ms_defaultInstance->Info(
            SourceLocation_t{"logger.cpp", __LINE__, __func__},
            "Logging initialized for application: {}", appName);

        return E_Result::E_SUCCESS;
    } catch (const std::exception& ex) {
        std::cerr << "Logger initialization failed: " << ex.what() << std::endl;
        return E_Result::E_UNKNOWN_ERROR;
    }
}

std::shared_ptr<Logger>& Logger::GetDefaultLogger(void) {
    /* Thread synchronization for singleton access */
    std::lock_guard<std::mutex> lock(g_loggerMutex);

    if (!ms_defaultInstance) {
        /* If not initialized yet, create a temporary default logger */
        try {
            ms_defaultInstance = std::make_shared<Logger>("default");
            ms_defaultInstance->Warn(
                SourceLocation_t{"logger.cpp", __LINE__, __func__},
                "Using uninitialized default logger. Call VSN_INIT_LOGGING "
                "first.");
        } catch (...) {
            /* Last resort emergency fallback */
            static std::shared_ptr<Logger> emergencyLogger =
                std::make_shared<Logger>("emergency");
            return emergencyLogger;
        }
    }
    return ms_defaultInstance;
}

E_Result Logger::SetPattern(const std::string& patternName) {
    try {
        std::string pattern;
        const E_Result result = formatters::GetPattern(patternName, pattern);

        if (E_Result::E_SUCCESS != result) {
            return result;
        }

        if (pattern.empty()) {
            return E_Result::E_INVALID_PARAMETER;
        }

        spdlog::set_pattern(pattern);
        return E_Result::E_SUCCESS;
    } catch (...) {
        return E_Result::E_UNKNOWN_ERROR;
    }
}

E_Result Logger::SetLevel(E_LogLevel level) {
    try {
        spdlog::set_level(static_cast<spdlog::level::level_enum>(level));
        return E_Result::E_SUCCESS;
    } catch (...) {
        return E_Result::E_UNKNOWN_ERROR;
    }
}

E_Result Logger::InitializeWithConfig(const std::string& appName,
                                      const std::string& configFile) {
    /* Thread synchronization for singleton initialization */
    std::lock_guard<std::mutex> lock(g_loggerMutex);

    try {
        /* Input validation */
        if (appName.empty()) {
            return E_Result::E_INVALID_PARAMETER;
        }

        /* Load configuration from file */
        const E_Result fileResult =
            LogConfig::GetInstance().LoadFromFile(configFile);
        if (E_Result::E_SUCCESS != fileResult) {
            std::cerr << "Warning: Failed to load configuration from file"
                      << std::endl;
        }

        /* Load potential environment variables (can override file settings) */
        const E_Result envResult = LogConfig::GetInstance().LoadFromEnv();
        if (E_Result::E_SUCCESS != envResult &&
            E_Result::E_NOT_INITIALIZED != envResult) {
            /* Non-critical error, continue with defaults */
            std::cerr << "Warning: Failed to load environment variables"
                      << std::endl;
        }

        std::string logDir =
            LogConfig::GetInstance().GetString("global", "log_dir", "/var/log");

        E_LogLevel level = static_cast<E_LogLevel>(
            LogConfig::GetInstance().GetInt32("global", "log_level", 1));

        /* Initialize with the loaded config */
        return Initialize(appName, logDir, level);
    } catch (...) {
        return E_Result::E_UNKNOWN_ERROR;
    }
}

/* Template specialization implementations would go here */

std::shared_ptr<spdlog::logger> Logger::GetNativeHandle(void) {
    return m_logger;
}

E_Result Logger::Flush(void) {
    try {
        if (!m_logger) {
            return E_Result::E_NOT_INITIALIZED;
        }

        m_logger->flush();
        return E_Result::E_SUCCESS;
    } catch (...) {
        return E_Result::E_UNKNOWN_ERROR;
    }
}

E_Result Logger::Shutdown(void) {
    try {
        spdlog::shutdown();
        ms_defaultInstance = nullptr;
        return E_Result::E_SUCCESS;
    } catch (...) {
        return E_Result::E_UNKNOWN_ERROR;
    }
}

/* Source location logging method implementations */
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

template E_Result Logger::Info(SourceLocation_t, const char*);
template E_Result Logger::Debug(SourceLocation_t, const char*);
template E_Result Logger::Warn(SourceLocation_t, const char*);
template E_Result Logger::Error(SourceLocation_t, const char*);
template E_Result Logger::Critical(SourceLocation_t, const char*);
template E_Result Logger::Trace(SourceLocation_t, const char*);

// Common parameter combinations
template E_Result Logger::Info<int>(SourceLocation_t, const char*, const int&);
template E_Result Logger::Info<std::string>(SourceLocation_t, const char*,
                                            const std::string&);
template E_Result Logger::Debug<int>(SourceLocation_t, const char*, const int&);
template E_Result Logger::Error<std::string>(SourceLocation_t, const char*,
                                             const std::string&);

// Specialized versions with common patterns
template E_Result Logger::Info<int, int>(SourceLocation_t, const char*,
                                         const int&, const int&);
template E_Result Logger::Info<std::string, std::string>(SourceLocation_t,
                                                         const char*,
                                                         const std::string&,
                                                         const std::string&);

// Component-specific patterns
template E_Result Logger::Info<char[5]>(SourceLocation_t, const char*,
                                        const char (&)[5]);
template E_Result Logger::Info<char[7]>(SourceLocation_t, const char*,
                                        const char (&)[7]);
template E_Result Logger::Info<char[9]>(SourceLocation_t, const char*,
                                        const char (&)[9]);

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