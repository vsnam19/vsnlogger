cmake_minimum_required(VERSION 3.10)
project(vsnlogger VERSION 1.0.0 LANGUAGES CXX)

# Find required dependencies
find_package(spdlog REQUIRED)

# Add library target
add_library(vsnlogger
    src/logger.cpp
    src/config.cpp
    src/formatters.cpp
    src/sinks.cpp
)

# Define include directories
target_include_directories(vsnlogger
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link dependencies
target_link_libraries(vsnlogger
    PUBLIC
        spdlog::spdlog
)

# Create aliases for use in other components
add_library(VSNLogger::vsnlogger ALIAS vsnlogger)

# Installation rules
install(TARGETS vsnlogger
    EXPORT vsnlogger-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY include/
    DESTINATION include
)

# Export the targets
install(EXPORT vsnlogger-targets
    FILE vsnlogger-targets.cmake
    NAMESPACE VSNLogger::
    DESTINATION lib/cmake/vsnlogger
)

# Generate and install package configuration
include(CMakePackageConfigHelpers)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/vsnlogger-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/vsnlogger-config.cmake
    @ONLY
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/vsnlogger-config.cmake
    DESTINATION lib/cmake/vsnlogger
)