#include <snowflake/clock.hpp>
#include <snowflake/snow_flake_config.hpp>

#include <chrono>

uint64_t SnowflakeClock::get_time()
{
    auto now = std::chrono::system_clock::now();

    auto milliseconds =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()
        ).count();

    return static_cast<uint64_t>(
        milliseconds - SnowflakeConfig::EPOCH
    );
}