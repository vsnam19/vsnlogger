#include <chrono>
#include <iostream>
#include <thread>

#include "libA.h"
#include "libB.h"
#include "vsnlogger/logger.h"
#include "vsnlogger/macros.h"

// Thread function for demonstrating multi-threaded logging
void worker_thread(int id, int iterations) {
    (void)id;
    for (int i = 0; i < iterations; i++) {
        VSN_COMPONENT_INFO("Worker", "Thread {} processing iteration {}", id,
                           i);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

int main(int argc, char* argv[]) {
    try {
        // Initialize logging system with file and console output
        VSN_INIT_LOGGING_FULL("app_a", "/var/log/app_a",
                              vsn::logger::LogLevel::TRACE);

        VSN_INFO("Application A starting up");

        // Log command-line arguments
        for (int i = 0; i < argc; i++) {
            VSN_DEBUG("Command line argument [{}] = {}", i, argv[i]);
        }

        // Use library functions that also log
        VSN_INFO("Calling library functions");
        libA::process_data(42);
        libB::generate_report("monthly");

        // Demonstrate multi-threaded logging
        VSN_INFO("Starting worker threads");
        std::thread t1(worker_thread, 1, 3);
        std::thread t2(worker_thread, 2, 3);

        // Join threads
        t1.join();
        t2.join();

        // Log different severity levels
        VSN_TRACE("This is a trace message with very detailed info");
        VSN_DEBUG("This is a debug message with troubleshooting info");
        VSN_INFO("This is an informational message about normal operation");
        VSN_WARN("This is a warning about something unusual");
        VSN_ERROR("This is an error that needs attention");
        VSN_CRITICAL("This is a critical error that requires immediate action");

        // Demonstrate component-specific logging
        VSN_COMPONENT_INFO("Database", "Connected to main database");
        VSN_COMPONENT_INFO("Network", "Listening on port 8080");
        VSN_COMPONENT_WARN("Security",
                           "Failed login attempt from 192.168.1.100");

        VSN_INFO("Application A shutting down normally");
        VSN_FLUSH_LOGS();
        VSN_SHUTDOWN_LOGGING();

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}