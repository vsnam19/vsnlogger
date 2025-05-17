# vsnlogger/CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(vsnlogger VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add compile options for MISRA C++ compliance
add_compile_options(
    -Wall
    -Wextra
    -Werror
    -Wconversion
    -Wsign-conversion
    -Wcast-align
    -Wcast-qual
    -Wformat=2
    -Wold-style-cast
    -Wredundant-decls
    -Wshadow
    -Wstrict-overflow=5
    -Wswitch-default
    -Wundef
    -Wunused
    -Woverloaded-virtual
    -Wpedantic
)

# Find required dependencies with version constraints
find_package(spdlog 1.9.0 REQUIRED)

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
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/vsnlogger-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/vsnlogger-config.cmake
    INSTALL_DESTINATION lib/cmake/vsnlogger
)

# Version information file
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/vsnlogger-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/vsnlogger-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/vsnlogger-config-version.cmake
    DESTINATION lib/cmake/vsnlogger
)