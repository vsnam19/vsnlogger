/**
 * @file sinks.h
 * @brief Output destination management for VSNLogger
 *
 * @details
 * This component provides factory functions for creating logging output
 * destinations with MISRA C++ compliance for safety-critical applications.
 *
 * @author VSNLogger Contributors
 * @version 1.0.0
 */

#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "error_codes.h"

/* Forward declaration of spdlog sink */
namespace spdlog {
namespace sinks {
class sink;
}
}  // namespace spdlog

namespace vsn {
namespace logger {
namespace sinks {

/**
 * @brief Create a console sink
 *
 * @param[in] colored Enable colorized output
 * @return Pointer to created sink or nullptr on failure
 */
std::shared_ptr<spdlog::sinks::sink> CreateConsoleSink(bool colored);

/**
 * @brief Create a file sink
 *
 * @param[in] filename Path to output file
 * @param[in] rotate Enable log rotation
 * @param[in] maxSize Maximum file size before rotation
 * @param[in] maxFiles Maximum number of rotated files to keep
 * @return Pointer to created sink or nullptr on failure
 */
std::shared_ptr<spdlog::sinks::sink> CreateFileSink(const std::string& filename,
                                                    bool rotate,
                                                    std::size_t maxSize,
                                                    std::size_t maxFiles);

/**
 * @brief Create a syslog sink
 *
 * @param[in] ident Application identifier for syslog
 * @param[in] syslogOption Syslog options
 * @param[in] syslogFacility Syslog facility
 * @param[in] enableFormatting Enable message formatting
 * @return Pointer to created sink or nullptr on failure
 */
std::shared_ptr<spdlog::sinks::sink> CreateSyslogSink(
    std::string ident, std::int32_t syslogOption, std::int32_t syslogFacility,
    bool enableFormatting);

/**
 * @brief Create a null sink (discards all messages)
 *
 * @return Pointer to created sink or nullptr on failure
 */
std::shared_ptr<spdlog::sinks::sink> CreateNullSink(void);

/**
 * @brief Create a multi-sink with multiple outputs
 *
 * @param[in] console Enable console output
 * @param[in] logFile Path to log file (empty to disable)
 * @param[in] syslog Enable syslog output
 * @return Vector of created sinks
 */
std::vector<std::shared_ptr<spdlog::sinks::sink>> CreateMultiSink(
    bool console, const std::string& logFile, bool syslog);

/**
 * @brief Get current sink allocation count
 *
 * @return Number of active sink allocations
 */
std::uint32_t GetSinkAllocationCount(void);

} /* namespace sinks */
} /* namespace logger */
} /* namespace vsn */
