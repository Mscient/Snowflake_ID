cmake_minimum_required(VERSION 3.20)

project(SnowflakeID
    VERSION 1.0
    LANGUAGES CXX
)

 
option(SNOWFLAKE_BUILD_TESTS "Build the test suite"          ON)
option(SNOWFLAKE_BUILD_DEMO  "Build the demo executable"     ON)

 
add_library(snowflake
    src/snow_flake.cpp
    src/clock.cpp
     
    include/snowflake/snowflake.hpp
    include/snowflake/clock.hpp
    include/snowflake/snow_flake_config.hpp
)

 
target_compile_features(snowflake PUBLIC cxx_std_20)

target_include_directories(snowflake
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_options(snowflake PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

 
if(SNOWFLAKE_BUILD_DEMO)
    add_executable(snowflake_demo src/main.cpp)
    target_link_libraries(snowflake_demo PRIVATE snowflake)
    target_compile_features(snowflake_demo PRIVATE cxx_std_20)
endif()
 
if(SNOWFLAKE_BUILD_TESTS)
    enable_testing()

    add_executable(test_multiple_ids tests/multiple_ids.cpp)
    target_link_libraries(test_multiple_ids PRIVATE snowflake)
    target_compile_features(test_multiple_ids PRIVATE cxx_std_20)

    add_executable(test_overflow tests/overflow.cpp)
    target_link_libraries(test_overflow PRIVATE snowflake)
    target_compile_features(test_overflow PRIVATE cxx_std_20)

    add_executable(test_concurrency tests/concurrency.cpp)
    target_link_libraries(test_concurrency PRIVATE snowflake)
    target_compile_features(test_concurrency PRIVATE cxx_std_20)

    add_test(NAME multiple_ids COMMAND test_multiple_ids)
    add_test(NAME overflow      COMMAND test_overflow)
    add_test(NAME concurrency   COMMAND test_concurrency)
endif()
 
include(GNUInstallDirs)

install(
    TARGETS snowflake
    EXPORT  SnowflakeIDTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    EXPORT      SnowflakeIDTargets
    FILE        SnowflakeIDTargets.cmake
    NAMESPACE   SnowflakeID::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SnowflakeID
)

 
include(CMakePackageConfigHelpers)

configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/SnowflakeIDConfig.cmake.in"
    "${PROJECT_BINARY_DIR}/SnowflakeIDConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/SnowflakeID"
)

write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/SnowflakeIDConfigVersion.cmake"
    VERSION      ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(
    FILES
        "${PROJECT_BINARY_DIR}/SnowflakeIDConfig.cmake"
        "${PROJECT_BINARY_DIR}/SnowflakeIDConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/SnowflakeID"
)