#pragma once

#include <spdlog/fmt/chrono.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <string>

namespace vsn::logger {

/**
 * Logging levels matching spdlog levels
 */
enum class LogLevel {
    TRACE = spdlog::level::trace,
    DEBUG = spdlog::level::debug,
    INFO = spdlog::level::info,
    WARN = spdlog::level::warn,
    ERROR = spdlog::level::err,
    CRITICAL = spdlog::level::critical,
    OFF = spdlog::level::off
};

/**
 * Core logger class that wraps spdlog functionality
 */
class Logger {
   private:
    std::shared_ptr<spdlog::logger> logger_;
    static std::shared_ptr<Logger> default_instance_;

   public:
    /**
     * Create new logger with given name
     */
    explicit Logger(const std::string& name);
    /**
     * Create logger wrapper from existing spdlog logger
     */
    Logger(std::shared_ptr<spdlog::logger> existing_logger);
    /**
     * Create logger with given name and specific file path
     */
    Logger(const std::string& name, const std::string& log_file_path);

    /**
     * Initialize the default global logger
     */
    static void initialize(const std::string& app_name,
                           const std::string& log_dir = "/var/log",
                           LogLevel level = LogLevel::INFO);

    /**
     * Get the default logger instance
     */
    static std::shared_ptr<Logger>& default_logger();

    /**
     * Set log pattern from predefined formats
     * @param pattern_name Name of the pattern ("json", "console", "simple",
     * "colored", etc.)
     */
    static void set_pattern(const std::string& pattern_name);

    /**
     * Initialize logging with configuration from file
     * @param app_name Application name
     * @param config_file Path to configuration file
     */
    static void initialize_with_config(
        const std::string& app_name,
        const std::string& config_file = "vsnlogger.conf");

    /**
     * Set global log level
     */
    static void set_level(LogLevel level);

    /**
     * Log with specified level
     */
    template <typename... Args>
    void log(LogLevel level, const char* fmt, const Args&... args) {
        logger_->log(static_cast<spdlog::level::level_enum>(level), fmt,
                     args...);
    }

    /**
     * Log methods for different levels
     */
    template <typename... Args>
    void trace(const char* fmt, const Args&... args) {
        logger_->trace(fmt, args...);
    }

    template <typename... Args>
    void debug(const char* fmt, const Args&... args) {
        logger_->debug(fmt, args...);
    }

    template <typename... Args>
    void info(const char* fmt, const Args&... args) {
        logger_->info(fmt, args...);
    }

    template <typename... Args>
    void warn(const char* fmt, const Args&... args) {
        logger_->warn(fmt, args...);
    }

    template <typename... Args>
    void error(const char* fmt, const Args&... args) {
        logger_->error(fmt, args...);
    }

    template <typename... Args>
    void critical(const char* fmt, const Args&... args) {
        logger_->critical(fmt, args...);
    }
    // Source location overloads - correct implementation
    template <typename... Args>
    void trace(spdlog::source_loc loc, const char* fmt, const Args&... args) {
        logger_->log(loc, spdlog::level::trace, fmt, args...);
    }

    template <typename... Args>
    void debug(spdlog::source_loc loc, const char* fmt, const Args&... args) {
        logger_->log(loc, spdlog::level::debug, fmt, args...);
    }

    template <typename... Args>
    void info(spdlog::source_loc loc, const char* fmt, const Args&... args) {
        logger_->log(loc, spdlog::level::info, fmt, args...);
    }

    template <typename... Args>
    void warn(spdlog::source_loc loc, const char* fmt, const Args&... args) {
        logger_->log(loc, spdlog::level::warn, fmt, args...);
    }

    template <typename... Args>
    void error(spdlog::source_loc loc, const char* fmt, const Args&... args) {
        logger_->log(loc, spdlog::level::err, fmt, args...);
    }

    template <typename... Args>
    void critical(spdlog::source_loc loc, const char* fmt,
                  const Args&... args) {
        logger_->log(loc, spdlog::level::critical, fmt, args...);
    }
    /**
     * Get the underlying spdlog logger
     */
    std::shared_ptr<spdlog::logger> native_handle() { return logger_; }

    /**
     * Flush logger
     */
    void flush() { logger_->flush(); }

    /**
     * Shutdown all loggers
     */
    static void shutdown() { spdlog::shutdown(); }
};

}  // namespace vsn::logger