/**
 * @file logger_t.h
 * @brief Thread-safe logging interface for embedded applications
 *
 * @details
 * This component implements a MISRA C++ compliant logging framework
 * with deterministic resource utilization characteristics suitable
 * for safety-critical automotive applications.
 *
 * @author VSNLogger Contributors
 * @version 1.0.0
 *
 * @requirements
 * - REQ-LOG-001: Thread-safe logging operations
 * - REQ-LOG-002: Deterministic memory allocation
 * - REQ-LOG-003: Configurable output destinations
 *
 * @limitations
 * - Limited to 8 concurrent output destinations
 * - Maximum 256 character message length
 * - Non-reentrant from interrupt contexts
 */

#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "error_codes.h"

/* Forward declaration of spdlog logger */
namespace spdlog {
class logger;
}

namespace vsn {
namespace logger {

/**
 * @brief Logging severity levels
 */
enum class E_LogLevel : std::uint8_t {
    E_TRACE = 0U,
    E_DEBUG = 1U,
    E_INFO = 2U,
    E_WARN = 3U,
    E_ERROR = 4U,
    E_CRITICAL = 5U,
    E_OFF = 6U
};

/**
 * @brief Source location information structure
 */
struct SourceLocation_t {
    const char* const m_filename;
    const std::uint32_t m_line;
    const char* const m_function;
};

/**
 * @brief Core logger class that wraps spdlog functionality with MISRA compliant
 * interface
 */
class Logger {
   public:
    /**
     * @brief Create new logger with given name
     *
     * @param[in] name Logger instance identifier
     * @return Newly created logger instance
     */
    explicit Logger(const std::string& name);

    /**
     * @brief Create logger with given name and specific file path
     *
     * @param[in] name Logger instance identifier
     * @param[in] logFilePath Path for log file output
     * @return Newly created logger instance
     */
    Logger(const std::string& name, const std::string& logFilePath);

    /**
     * @brief Create logger wrapper from existing spdlog logger
     *
     * @param[in] existingLogger Pointer to existing spdlog logger
     * @return Newly created logger instance
     */
    explicit Logger(std::shared_ptr<spdlog::logger> existingLogger);

    /**
     * @brief Initialize the default global logger
     *
     * @param[in] appName Application identifier
     * @param[in] logDir Directory for log file storage
     * @param[in] level Minimum severity level to log
     * @return Operation result code
     */
    static E_Result Initialize(const std::string& appName,
                               const std::string& logDir, E_LogLevel level);

    /**
     * @brief Get the default logger instance
     *
     * @return Reference to default logger
     */
    static std::shared_ptr<Logger>& GetDefaultLogger(void);

    /**
     * @brief Set global log pattern from predefined formats
     *
     * @param[in] patternName Name of pattern template
     * @return Operation result code
     */
    static E_Result SetPattern(const std::string& patternName);

    /**
     * @brief Set global log level
     *
     * @param[in] level Minimum severity level to log
     * @return Operation result code
     */
    static E_Result SetLevel(E_LogLevel level);

    /**
     * @brief Initialize logging with configuration from file
     *
     * @param[in] appName Application identifier
     * @param[in] configFile Path to configuration file
     * @return Operation result code
     */
    static E_Result InitializeWithConfig(const std::string& appName,
                                         const std::string& configFile);

    /**
     * @brief Log with specified level and source location
     *
     * @param[in] loc Source code location information
     * @param[in] level Severity level for message
     * @param[in] fmt Format string (must be valid for lifetime of call)
     * @return Operation result code
     */
    template <typename... Args>
    E_Result LogWithLocation(SourceLocation_t loc, E_LogLevel level,
                             const char* fmt, const Args&... args);

    /**
     * @brief Trace level logging
     *
     * @param[in] loc Source code location information
     * @param[in] fmt Format string (must be valid for lifetime of call)
     * @return Operation result code
     */
    template <typename... Args>
    E_Result Trace(SourceLocation_t loc, const char* fmt, const Args&... args);

    /**
     * @brief Debug level logging
     *
     * @param[in] loc Source code location information
     * @param[in] fmt Format string (must be valid for lifetime of call)
     * @return Operation result code
     */
    template <typename... Args>
    E_Result Debug(SourceLocation_t loc, const char* fmt, const Args&... args);

    /**
     * @brief Info level logging
     *
     * @param[in] loc Source code location information
     * @param[in] fmt Format string (must be valid for lifetime of call)
     * @return Operation result code
     */
    template <typename... Args>
    E_Result Info(SourceLocation_t loc, const char* fmt, const Args&... args);

    /**
     * @brief Warning level logging
     *
     * @param[in] loc Source code location information
     * @param[in] fmt Format string (must be valid for lifetime of call)
     * @return Operation result code
     */
    template <typename... Args>
    E_Result Warn(SourceLocation_t loc, const char* fmt, const Args&... args);

    /**
     * @brief Error level logging
     *
     * @param[in] loc Source code location information
     * @param[in] fmt Format string (must be valid for lifetime of call)
     * @return Operation result code
     */
    template <typename... Args>
    E_Result Error(SourceLocation_t loc, const char* fmt, const Args&... args);

    /**
     * @brief Critical level logging
     *
     * @param[in] loc Source code location information
     * @param[in] fmt Format string (must be valid for lifetime of call)
     * @return Operation result code
     */
    template <typename... Args>
    E_Result Critical(SourceLocation_t loc, const char* fmt,
                      const Args&... args);

    /**
     * @brief Get the underlying spdlog logger
     *
     * @return Pointer to native logger implementation
     */
    std::shared_ptr<spdlog::logger> GetNativeHandle(void);

    /**
     * @brief Flush logger to ensure all messages are written
     *
     * @return Operation result code
     */
    E_Result Flush(void);

    /**
     * @brief Shutdown all loggers
     *
     * @return Operation result code
     */
    static E_Result Shutdown(void);

   private:
    /** Maximum number of sinks allowed per logger instance */
    static constexpr std::uint8_t k_maxSinks = 8U;

    /** Maximum message length in characters */
    static constexpr std::uint16_t k_maxMessageLength = 256U;

    /** Underlying spdlog logger instance */
    std::shared_ptr<spdlog::logger> m_logger;

    /** Default logger instance for global access */
    static std::shared_ptr<Logger> ms_defaultInstance;

    /** Allocation counter for resource tracking */
    static std::uint32_t ms_allocationCount;
};

} /* namespace logger */
} /* namespace vsn */
