#include "libB.h"

#include <chrono>
#include <fstream>
#include <thread>

#include "vsnlogger/macros.h"

namespace libB {

bool generate_report(const std::string& frequency) {
    VSN_COMPONENT_INFO("LibB", "Generating {} report", frequency);

    // Simulate report generation (takes time)
    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    // Validate frequency parameter
    if (frequency != "daily" && frequency != "weekly" &&
        frequency != "monthly" && frequency != "quarterly") {
        VSN_COMPONENT_WARN("LibB", "Unusual report frequency requested: {}",
                           frequency);
    }

    // Simulate successful report generation
    VSN_COMPONENT_INFO("LibB", "Report generation complete");
    return true;
}

bool process_config(const std::string& config_file) {
    VSN_COMPONENT_INFO("LibB", "Processing configuration file: {}",
                       config_file);

    // Check if file exists
    std::ifstream file(config_file);
    if (!file.good()) {
        VSN_COMPONENT_ERROR("LibB", "Configuration file not found: {}",
                            config_file);
        return false;
    }

    // Simulate configuration processing
    VSN_COMPONENT_DEBUG("LibB", "Parsing configuration parameters");

    // Count lines as a simple metric
    int line_count = 0;
    std::string line;
    while (std::getline(file, line)) {
        line_count++;
    }

    VSN_COMPONENT_INFO("LibB", "Processed {} configuration parameters",
                       line_count);
    return true;
}

}  // namespace libB