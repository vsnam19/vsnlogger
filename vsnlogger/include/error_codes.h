/**
 * @file error_codes.h
 * @brief Error code definitions for VSNLogger
 *
 * @details
 * This component defines standardized error codes used throughout the VSNLogger
 * framework to ensure consistent error reporting and handling.
 *
 * @author VSNLogger Contributors
 * @version 1.0.0
 */

#pragma once
#include <cstdint>

namespace vsn {
namespace logger {

/**
 * @brief Standard result codes for logger operations
 */
enum class E_Result : std::uint8_t {
    E_SUCCESS = 0U,              /**< Operation completed successfully */
    E_INVALID_PARAMETER = 1U,    /**< One or more parameters were invalid */
    E_RESOURCE_UNAVAILABLE = 2U, /**< Required resource could not be accessed */
    E_ALLOCATION_FAILED = 3U,    /**< Memory allocation operation failed */
    E_PERMISSION_DENIED = 4U,    /**< Insufficient permissions for operation */
    E_PATH_CREATION_FAILED = 5U, /**< Failed to create directory path */
    E_NOT_INITIALIZED = 6U,      /**< Component not properly initialized */
    E_CONFIG_ERROR = 7U,         /**< Configuration error occurred */
    E_INVALID_STATE = 8U,        /**< Operation invalid in current state */
    E_FILE_ERROR = 9U,           /**< File operation failed */
    E_UNKNOWN_ERROR = 255U       /**< Unspecified error occurred */
};

} /* namespace logger */
} /* namespace vsn */
