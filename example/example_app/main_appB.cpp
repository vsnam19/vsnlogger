#include <algorithm>
#include <iostream>
#include <vector>

#include "libA.h"
#include "libB.h"
#include "vsnlogger/logger.h"
#include "vsnlogger/macros.h"

// Function to demonstrate logging in algorithms
void process_vector(const std::vector<int>& data) {
    VSN_INFO("Processing vector with {} elements", data.size());

    int sum = 0;
    int count = 0;

    for (const auto& value : data) {
        VSN_DEBUG("Processing element {}: {}", count++, value);
        sum += value;
    }

    double average = data.empty() ? 0 : static_cast<double>(sum) / data.size();
    VSN_INFO("Vector processing complete. Sum: {}, Average: {:.2f}", sum,
             average);
}

int main() {
    try {
        // Initialize logging with JSON format for structured logging
        vsn::logger::Logger::Initialize("app_b", "/var/log/app_b",
                                        vsn::logger::E_LogLevel::E_INFO);

        // Set pattern for JSON formatted logs
        auto logger = vsn::logger::Logger::GetDefaultLogger()->GetNativeHandle();

        VSN_INFO("Application B initialized with JSON logging");

        // Create test data
        std::vector<int> test_data = {42, 17, 8, 94, 23, 61};

        // Process data with logging
        process_vector(test_data);

        // Use library functions
        libA::process_data(100);
        libB::generate_report("quarterly");

        // Simulated error handling
        try {
            VSN_INFO("Attempting risky operation");
            if (test_data.empty()) {
                throw std::runtime_error("Empty data set");
            }

            // Simulate error condition
            if (test_data[0] > 30) {
                throw std::out_of_range("Value exceeds threshold");
            }
        } catch (const std::exception& e) {
            VSN_ERROR("Error during data processing: {}", e.what());
        }

        VSN_INFO("Application B shutting down");
        vsn::logger::Logger::Shutdown();

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Fatal error in Application B: " << e.what() << std::endl;
        return 1;
    }
}