#include <cstdint>
#include <mutex>
#include <chrono>

#include <snowflake/snow_flake_config.hpp>

#define private public
#include <snowflake/snowflake.hpp>
#undef private

#include <snowflake/clock.hpp>

#include <iostream>

int main()
{
    Snowflake snowflake(1);

    snowflake.sequence_ = SnowflakeConfig::MAX_SEQUENCE;
    snowflake.last_timestamp_ = SnowflakeClock::get_time();

    uint64_t id = snowflake.generate();

    auto data = snowflake.decode(id);

    std::cout << "Before overflow sequence: "
              << SnowflakeConfig::MAX_SEQUENCE << '\n';

    std::cout << "After generation sequence: "
              << data.sequence << '\n';

    if (data.sequence == 0)
    {
        std::cout << "[PASS] Sequence overflow handling\n";
        return 0;
    }

    std::cout << "[FAIL] Sequence did not reset\n";
    return 1;
}