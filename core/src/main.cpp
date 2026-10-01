#include <iostream>
#include <chrono>
#include <ctime>

#include <snowflake/snowflake.hpp>
#include <snowflake/snow_flake_config.hpp>

int main()
{
    Snowflake generator(1);

    std::cout << "Snowflake ID Generator\n";
    std::cout << "======================\n\n";

    // Generate an ID
    uint64_t id = generator.generate();

    std::cout << "Generated ID: "
              << id << "\n\n";

    // Decode the ID
    auto data = generator.decode(id);

    std::cout << "Decoded ID\n";
    std::cout << "Worker ID: "
              << data.worker_id << '\n';

    std::cout << "Sequence: "
              << data.sequence << '\n';

    std::cout << "Timestamp: "
              << data.timestamp << '\n';

    // Convert Snowflake timestamp back to real time
    std::chrono::system_clock::time_point time_point{
        std::chrono::milliseconds(
            data.timestamp + SnowflakeConfig::EPOCH
        )
    };

    std::time_t time =
        std::chrono::system_clock::to_time_t(time_point);

    std::cout << "Generated at: "
              << std::ctime(&time);

    return 0;
}