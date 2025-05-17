#include "vsnlogger/sinks.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/sinks/syslog_sink.h>

#include <filesystem>

namespace vsn::logger {
namespace sinks {

std::shared_ptr<spdlog::sinks::sink> create_console_sink(bool colored) {
    if (colored) {
        return std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    } else {
        return std::make_shared<spdlog::sinks::stdout_sink_mt>();
    }
}

std::shared_ptr<spdlog::sinks::sink> create_file_sink(
    const std::string& filename, bool rotate, size_t max_size,
    size_t max_files) {
    // Create parent directory if it doesn't exist
    std::filesystem::path dir_path =
        std::filesystem::path(filename).parent_path();
    if (!dir_path.empty() && !std::filesystem::exists(dir_path)) {
        std::filesystem::create_directories(dir_path);
    }

    if (rotate) {
        return std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            filename, max_size, max_files);
    } else {
        return std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename);
    }
}

std::shared_ptr<spdlog::sinks::sink> create_syslog_sink(
    std::string ident, int syslog_option, int syslog_facility,
    bool enable_formatting) {
    return std::make_shared<spdlog::sinks::syslog_sink_mt>(
        ident, syslog_option, syslog_facility, enable_formatting);
}

std::shared_ptr<spdlog::sinks::sink> create_null_sink() {
    return std::make_shared<spdlog::sinks::null_sink_mt>();
}

std::vector<std::shared_ptr<spdlog::sinks::sink>> create_multi_sink(
    bool console, const std::string& log_file, bool syslog) {
    std::vector<std::shared_ptr<spdlog::sinks::sink>> sinks;

    if (console) {
        sinks.push_back(create_console_sink(true));
    }

    if (!log_file.empty()) {
        sinks.push_back(create_file_sink(log_file));
    }

    if (syslog) {
        sinks.push_back(create_syslog_sink());
    }

    // If no sinks specified, add console sink as default
    if (sinks.empty()) {
        sinks.push_back(create_console_sink(true));
    }

    return sinks;
}

}  // namespace sinks
}  // namespace vsn::logger