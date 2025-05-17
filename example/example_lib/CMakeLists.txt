# example_lib/CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(example_libs)

# Add library targets
add_library(libA
    libA.cpp
)

add_library(libB
    libB.cpp
)

# Define include directories
target_include_directories(libA
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(libB
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link against vsnlogger
target_link_libraries(libA
    PRIVATE
        vsnlogger
)

target_link_libraries(libB
    PRIVATE
        vsnlogger
)