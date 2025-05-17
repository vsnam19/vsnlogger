#pragma once

#include <spdlog/sinks/sink.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <string>
#include <vector>

namespace vsn::logger {
namespace sinks {

/**
 * Create a console sink
 */
std::shared_ptr<spdlog::sinks::sink> create_console_sink(bool colored = true);

/**
 * Create a file sink
 */
std::shared_ptr<spdlog::sinks::sink> create_file_sink(
    const std::string& filename, bool rotate = true,
    size_t max_size = 10 * 1024 * 1024,  // 10 MB
    size_t max_files = 5);

/**
 * Create a syslog sink
 */
std::shared_ptr<spdlog::sinks::sink> create_syslog_sink(
    std::string ident = "vsnlogger", int syslog_option = 0,
    int syslog_facility = 0, bool enable_formatting = true);

/**
 * Create a null sink (discards all messages)
 */
std::shared_ptr<spdlog::sinks::sink> create_null_sink();

/**
 * Create a multi-sink
 */
std::vector<std::shared_ptr<spdlog::sinks::sink>> create_multi_sink(
    bool console = true, const std::string& log_file = "", bool syslog = false);

}  // namespace sinks
}  // namespace vsn::logger