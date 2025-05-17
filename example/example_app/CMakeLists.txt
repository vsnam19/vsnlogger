# example_app/CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(example_apps)

# Add executable targets
add_executable(app_a
    main_appA.cpp
)

add_executable(app_b
    main_appB.cpp
)

# Link against example libraries and logger
target_link_libraries(app_a
    PRIVATE
        libA
        libB
        vsnlogger
)

target_link_libraries(app_b
    PRIVATE
        libA
        libB
        vsnlogger
)

# Create log directories during installation
install(CODE "
    file(MAKE_DIRECTORY \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/var/log/app_a)
    file(MAKE_DIRECTORY \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/var/log/app_b)
    ")

# Install executables
install(TARGETS app_a app_b
    RUNTIME DESTINATION bin
)