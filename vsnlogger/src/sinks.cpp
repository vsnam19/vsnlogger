/**
 * @file sinks_t.cpp
 * @brief Implementation of output destination management for VSNLogger
 *
 * @details
 * This component provides factory functions for creating logging output
 * destinations with deterministic allocation characteristics and explicit
 * resource constraints suitable for automotive embedded systems.
 *
 * @author VSNLogger Contributors
 * @version 1.0.0
 */

#include "vsnlogger/sinks.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/sinks/syslog_sink.h>

#include <atomic>
#include <filesystem>
#include <mutex>

namespace vsn {
namespace logger {
namespace sinks {

/* Thread synchronization for allocation tracking */
static std::mutex g_sinkMutex;

/* Allocation tracking */
static std::atomic<std::uint32_t> g_sinkAllocationCount(0U);

/* Maximum number of sink allocations allowed */
static constexpr std::uint32_t k_maxSinkAllocations = 64U;

std::shared_ptr<spdlog::sinks::sink> CreateConsoleSink(bool colored) {
    /* Thread synchronization for allocation tracking */
    std::lock_guard<std::mutex> lock(g_sinkMutex);

    /* Check allocation limit */
    if (g_sinkAllocationCount >= k_maxSinkAllocations) {
        return nullptr;
    }

    try {
        std::shared_ptr<spdlog::sinks::sink> result;

        if (colored) {
            result = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        } else {
            result = std::make_shared<spdlog::sinks::stdout_sink_mt>();
        }

        if (result) {
            ++g_sinkAllocationCount;
        }

        return result;
    } catch (...) {
        return nullptr;
    }
}

std::shared_ptr<spdlog::sinks::sink> CreateFileSink(const std::string& filename,
                                                    bool rotate,
                                                    std::size_t maxSize,
                                                    std::size_t maxFiles) {
    /* Parameter validation */
    if (filename.empty()) {
        return nullptr;
    }

    /* Thread synchronization for allocation tracking */
    std::lock_guard<std::mutex> lock(g_sinkMutex);

    /* Check allocation limit */
    if (g_sinkAllocationCount >= k_maxSinkAllocations) {
        return nullptr;
    }

    try {
        /* Create parent directory if it doesn't exist */
        std::filesystem::path dirPath =
            std::filesystem::path(filename).parent_path();

        if (!dirPath.empty() && !std::filesystem::exists(dirPath)) {
            std::filesystem::create_directories(dirPath);
        }

        std::shared_ptr<spdlog::sinks::sink> result;

        /* Apply sensible defaults for limits */
        if (0U == maxSize) {
            maxSize = 10U * 1024U * 1024U; /* 10 MB */
        }

        if (0U == maxFiles) {
            maxFiles = 5U;
        }

        /* Enforce upper bounds */
        const std::size_t k_maxFileSizeLimit = 1024U * 1024U * 1024U; /* 1 GB */
        const std::size_t k_maxFileCountLimit = 100U;

        if (maxSize > k_maxFileSizeLimit) {
            maxSize = k_maxFileSizeLimit;
        }

        if (maxFiles > k_maxFileCountLimit) {
            maxFiles = k_maxFileCountLimit;
        }

        if (rotate) {
            result = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                filename, maxSize, maxFiles);
        } else {
            result =
                std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename);
        }

        if (result) {
            ++g_sinkAllocationCount;
        }

        return result;
    } catch (...) {
        return nullptr;
    }
}

std::shared_ptr<spdlog::sinks::sink> CreateSyslogSink(
    std::string ident, std::int32_t syslogOption, std::int32_t syslogFacility,
    bool enableFormatting) {
    /* Thread synchronization for allocation tracking */
    std::lock_guard<std::mutex> lock(g_sinkMutex);

    /* Check allocation limit */
    if (g_sinkAllocationCount >= k_maxSinkAllocations) {
        return nullptr;
    }

    try {
        /* Default identifier if empty */
        if (ident.empty()) {
            ident = "vsnlogger";
        }

        /* Check identifier length */
        const std::size_t k_maxIdentLength = 32U;
        if (ident.length() > k_maxIdentLength) {
            ident = ident.substr(0, k_maxIdentLength);
        }

        std::shared_ptr<spdlog::sinks::sink> result =
            std::make_shared<spdlog::sinks::syslog_sink_mt>(
                ident, syslogOption, syslogFacility, enableFormatting);

        if (result) {
            ++g_sinkAllocationCount;
        }

        return result;
    } catch (...) {
        return nullptr;
    }
}

std::shared_ptr<spdlog::sinks::sink> CreateNullSink(void) {
    /* Thread synchronization for allocation tracking */
    std::lock_guard<std::mutex> lock(g_sinkMutex);

    /* Check allocation limit */
    if (g_sinkAllocationCount >= k_maxSinkAllocations) {
        return nullptr;
    }

    try {
        std::shared_ptr<spdlog::sinks::sink> result =
            std::make_shared<spdlog::sinks::null_sink_mt>();

        if (result) {
            ++g_sinkAllocationCount;
        }

        return result;
    } catch (...) {
        return nullptr;
    }
}

std::vector<std::shared_ptr<spdlog::sinks::sink>> CreateMultiSink(
    bool console, const std::string& logFile, bool syslog) {
    std::vector<std::shared_ptr<spdlog::sinks::sink>> sinks;

    /* Maximum number of sinks to create */
    const std::size_t k_maxSinks = 8U;

    try {
        if (console) {
            auto consoleSink = CreateConsoleSink(true);
            if (consoleSink && sinks.size() < k_maxSinks) {
                sinks.push_back(consoleSink);
            }
        }

        if (!logFile.empty()) {
            auto fileSink = CreateFileSink(logFile, 0, 0, true);
            if (fileSink && sinks.size() < k_maxSinks) {
                sinks.push_back(fileSink);
            }
        }

        if (syslog) {
            auto syslogSink = CreateSyslogSink("vsnlogger", 0, 0, true);
            if (syslogSink && sinks.size() < k_maxSinks) {
                sinks.push_back(syslogSink);
            }
        }

        /* If no sinks specified, add console sink as default */
        if (sinks.empty()) {
            auto defaultSink = CreateConsoleSink(true);
            if (defaultSink) {
                sinks.push_back(defaultSink);
            }
        }
    } catch (...) {
        /* Return whatever sinks were successfully created */
    }

    return sinks;
}

std::uint32_t GetSinkAllocationCount(void) {
    return g_sinkAllocationCount.load(std::memory_order_relaxed);
}

} /* namespace sinks */
} /* namespace logger */
} /* namespace vsn */