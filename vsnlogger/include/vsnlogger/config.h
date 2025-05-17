#pragma once

#include <map>
#include <memory>
#include <string>

namespace vsn::logger {

/**
 * Configuration options for the logging system
 */
class Config {
   public:
    /**
     * Singleton instance accessor
     */
    static Config& instance();

    /**
     * Load configuration from file
     */
    bool load_from_file(const std::string& config_file = "/etc/vsnlogger.conf");

    /**
     * Load configuration from environment variables
     */
    bool load_from_env();

    /**
     * Get string configuration value
     */
    std::string get_string(const std::string& section, const std::string& key,
                           const std::string& default_value = "");

    /**
     * Get integer configuration value
     */
    int get_int(const std::string& section, const std::string& key,
                int default_value = 0);

    /**
     * Get boolean configuration value
     */
    bool get_bool(const std::string& section, const std::string& key,
                  bool default_value = false);

    /**
     * Set configuration value
     */
    void set(const std::string& section, const std::string& key,
             const std::string& value);

   private:
    // Private constructor for singleton
    Config();

    // Disable copy and assignment
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    // Configuration storage
    std::map<std::string, std::map<std::string, std::string>> config_data_;
};

}  // namespace vsn::logger
