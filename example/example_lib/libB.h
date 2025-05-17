#pragma once

#include <string>

namespace libB {

/**
 * Generate a report with specified frequency
 * @param frequency The report frequency (daily, weekly, monthly, etc.)
 * @return Success status
 */
bool generate_report(const std::string& frequency);

/**
 * Process configuration with validation
 * @param config_file Path to the configuration file
 * @return Success status
 */
bool process_config(const std::string& config_file);

}  // namespace libB