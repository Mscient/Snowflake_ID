#include <snowflake/clock.hpp>
#include <snowflake/snow_flake_config.hpp>

#include <chrono>
#include <stdexcept>

uint64_t SnowflakeClock::get_time()
{
    auto now = std::chrono::system_clock::now();

    // Use a signed count to detect pre-epoch timestamps before casting.
    auto ms_since_unix_epoch =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()
        ).count();   // int64_t

    // The custom epoch must lie in the past relative to the Unix epoch.
    auto epoch_signed =
        static_cast<int64_t>(SnowflakeConfig::EPOCH);

    if (ms_since_unix_epoch < epoch_signed)
    {
        throw std::runtime_error(
            "System clock is before the Snowflake epoch "
            "(2026-01-01 00:00:00 UTC)"
        );
    }

    return static_cast<uint64_t>(ms_since_unix_epoch - epoch_signed);
}