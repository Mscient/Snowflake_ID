cmake_minimum_required(VERSION 3.20)

project(SnowflakeID
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Library
add_library(snowflake
    src/snow_flake.cpp
    src/clock.cpp
)

target_include_directories(snowflake
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

# Main application
add_executable(snowflake_demo
    src/main.cpp
)

target_link_libraries(snowflake_demo
    PRIVATE snowflake
)

# Tests
add_executable(test_multiple_ids
    tests/multiple_ids.cpp
)

target_link_libraries(test_multiple_ids
    PRIVATE snowflake
)

add_executable(test_overflow
    tests/overflow.cpp
)

target_link_libraries(test_overflow
    PRIVATE snowflake
)

add_executable(test_concurrency
    tests/concurrency.cpp
)

target_link_libraries(test_concurrency
    PRIVATE snowflake
)


enable_testing()

add_test(NAME multiple_ids COMMAND test_multiple_ids)
add_test(NAME overflow COMMAND test_overflow)
add_test(NAME concurrency COMMAND test_concurrency)

# Installation
include(GNUInstallDirs)

install(
    TARGETS snowflake
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)