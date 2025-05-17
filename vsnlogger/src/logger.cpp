#include "vsnlogger/logger.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <filesystem>
#include <iostream>

#include "vsnlogger/config.h"
#include "vsnlogger/formatters.h"
#include "vsnlogger/sinks.h"

namespace vsn::logger {

// Initialize static member
std::shared_ptr<Logger> Logger::default_instance_ = nullptr;

Logger::Logger(const std::string& name) {
    try {
        // Try to retrieve existing logger first
        logger_ = spdlog::get(name);

        if (!logger_) {
            // Create console sink using sinks utility
            auto console_sink = sinks::create_console_sink(true);
            logger_ = std::make_shared<spdlog::logger>(name, console_sink);
            spdlog::register_logger(logger_);
        }
    } catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "Logger initialization failed: " << ex.what() << std::endl;
        // Fallback to console-only
        logger_ = spdlog::stdout_color_mt(name);
    }
}

Logger::Logger(const std::string& name, const std::string& log_file_path) {
    try {
        // Try to retrieve existing logger first
        logger_ = spdlog::get(name);

        if (!logger_) {
            // Use the create_multi_sink function instead of direct creation
            std::vector<std::shared_ptr<spdlog::sinks::sink>> sinks_vec =
                sinks::create_multi_sink(true, log_file_path, false);

            logger_ = std::make_shared<spdlog::logger>(name, sinks_vec.begin(),
                                                       sinks_vec.end());
            spdlog::register_logger(logger_);
        }
    } catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "Logger initialization failed: " << ex.what() << std::endl;
        // Fallback to console-only
        logger_ = spdlog::stdout_color_mt(name);
    }
}

// Create a non-registering constructor for use in initialize
Logger::Logger(std::shared_ptr<spdlog::logger> existing_logger) {
    logger_ = existing_logger;
}

void Logger::initialize(const std::string& app_name, const std::string& log_dir,
                        LogLevel level) {
    // Get configuration instance
    auto& config = Config::instance();

    // Load potential environment variables
    config.load_from_env();

    // Get configuration parameters, with defaults from the provided arguments
    std::string configured_log_dir =
        config.get_string("global", "log_dir", log_dir);
    bool use_console = config.get_bool(app_name, "console_output", true);
    bool use_file = config.get_bool(app_name, "file_output", true);
    bool use_syslog = config.get_bool(app_name, "syslog_output", false);
    std::string pattern_name =
        config.get_string(app_name, "log_pattern", "colored");
    bool use_colors = config.get_bool(app_name, "use_colors", true);

    // Configure log file size/count
    int file_max_size =
        config.get_int(app_name, "max_file_size", 10 * 1024 * 1024);
    int file_max_count = config.get_int(app_name, "max_files", 5);

    // Convert level from config if provided
    LogLevel configured_level = static_cast<LogLevel>(
        config.get_int(app_name, "log_level", static_cast<int>(level)));

    // Create log file path
    std::string log_file_path;
    if (use_file) {
        log_file_path =
            configured_log_dir + "/" + app_name + "/" + app_name + ".log";
    }

    // Check if a logger with this name already exists
    auto existing_logger = spdlog::get(app_name);
    if (existing_logger) {
        // Use the existing logger, just update its configuration
        default_instance_ = std::make_shared<Logger>(existing_logger);
    } else {
        // Build a vector of sinks based on configuration
        std::vector<std::shared_ptr<spdlog::sinks::sink>> sink_vec;

        // Add console sink if configured
        if (use_console) {
            sink_vec.push_back(sinks::create_console_sink(use_colors));
        }

        // Add file sink if configured - use the max file size and count
        // parameters
        if (use_file && !log_file_path.empty()) {
            sink_vec.push_back(sinks::create_file_sink(
                log_file_path, true, file_max_size, file_max_count));
        }

        // Add syslog sink if configured
        if (use_syslog) {
            sink_vec.push_back(sinks::create_syslog_sink());
        }

        // If no sinks were added, add a default console sink
        if (sink_vec.empty()) {
            sink_vec.push_back(sinks::create_console_sink(use_colors));
        }

        // Create logger with the sinks
        auto logger = std::make_shared<spdlog::logger>(
            app_name, sink_vec.begin(), sink_vec.end());

        // Register the logger with spdlog
        spdlog::register_logger(logger);

        // Create our wrapper without re-registering
        default_instance_ = std::make_shared<Logger>(logger);
    }

    // Set pattern using formatter helper
    std::string pattern = formatters::get_pattern(pattern_name);
    if (pattern_name == "colored") {
        // Special colored pattern
        pattern =
            "%Y-%m-%d %H:%M:%S.%f %z  [%^%-8l%$] [%-10n] [%-5P %-5t] "
            "[%g:%#] %v";
    }
    spdlog::set_pattern(pattern);

    // Set level
    spdlog::set_level(static_cast<spdlog::level::level_enum>(configured_level));

    // Configure colors for console sinks if using an existing logger
    if (use_colors && use_console) {
        auto sinks = default_instance_->logger_->sinks();
        for (auto& sink : sinks) {
            auto console_sink =
                std::dynamic_pointer_cast<spdlog::sinks::stdout_color_sink_mt>(
                    sink);
            if (console_sink) {
                console_sink->set_color(spdlog::level::trace,
                                        "\033[36m");  // Cyan
                console_sink->set_color(spdlog::level::debug,
                                        "\033[92m");  // Bright Green
                console_sink->set_color(spdlog::level::info,
                                        "\033[97m");  // Bright White
                console_sink->set_color(spdlog::level::warn,
                                        "\033[93m");  // Bright Yellow
                console_sink->set_color(spdlog::level::err,
                                        "\033[91m");  // Bright Red
                console_sink->set_color(spdlog::level::critical,
                                        "\033[97;41m");  // White on Red
                break;  // Only configure the first console sink
            }
        }
    }

    // Log initialization message
    default_instance_->info("Logging initialized for application: {}",
                            app_name);
}

std::shared_ptr<Logger>& Logger::default_logger() {
    if (!default_instance_) {
        // If not initialized yet, create a temporary default logger
        default_instance_ = std::make_shared<Logger>("default");
        default_instance_->warn(
            "Using uninitialized default logger. Call VSN_INIT_LOGGING first.");
    }
    return default_instance_;
}

void Logger::set_level(LogLevel level) {
    spdlog::set_level(static_cast<spdlog::level::level_enum>(level));
}

void Logger::set_pattern(const std::string& pattern_name) {
    std::string pattern = formatters::get_pattern(pattern_name);
    if (pattern_name == "colored") {
        // Special colored pattern
        pattern =
            "%Y-%m-%d %H:%M:%S.%f %z  [%^%-8l%$] [%-10n] [%-5P %-5t] "
            "[%g:%#] %v";
    }
    spdlog::set_pattern(pattern);
}

void Logger::initialize_with_config(const std::string& app_name,
                                    const std::string& config_file) {
    // Load configuration from file
    Config::instance().load_from_file(config_file);

    // Load potential environment variables (can override file settings)
    Config::instance().load_from_env();

    // Initialize with the loaded config
    initialize(app_name);
}

}  // namespace vsn::logger