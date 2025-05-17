#pragma once

namespace libA {

/**
 * Process data with logging
 * @param value The input value to process
 * @return The processed result
 */
int process_data(int value);

/**
 * Analyze data and generate statistics
 * @param data_points Number of data points to analyze
 * @return Success status
 */
bool analyze_statistics(int data_points);

}  // namespace libA