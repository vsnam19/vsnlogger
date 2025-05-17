#include "libA.h"

#include <cmath>
#include <stdexcept>

#include "vsnlogger/macros.h"

namespace libA {

int process_data(int value) {
    // Use component-specific logging for the library
    VSN_COMPONENT_INFO("LibA", "Processing data with value: {}", value);

    try {
        // Simulate data processing
        if (value < 0) {
            VSN_COMPONENT_WARN("LibA", "Received negative value: {}", value);
            value = std::abs(value);
        }

        int result = value * 2;
        VSN_COMPONENT_DEBUG("LibA", "Calculated result: {}", result);

        return result;
    } catch (const std::exception& e) {
        VSN_COMPONENT_ERROR("LibA", "Error processing data: {}", e.what());
        throw;  // Re-throw the exception
    }
}

bool analyze_statistics(int data_points) {
    VSN_COMPONENT_INFO("LibA", "Analyzing statistics with {} data points",
                       data_points);

    if (data_points <= 0) {
        VSN_COMPONENT_ERROR("LibA", "Invalid number of data points: {}",
                            data_points);
        return false;
    }

    // Simulate analysis
    VSN_COMPONENT_DEBUG("LibA", "Running statistical analysis");

    // Log completion
    VSN_COMPONENT_INFO("LibA", "Statistical analysis complete");

    return true;
}

}  // namespace libA