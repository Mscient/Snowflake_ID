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

    
    auto data = Snowflake::decode(id);

    std::cout << "Decoded ID\n";
    std::cout << "Worker ID:  " << data.worker_id  << '\n';
    std::cout << "Sequence:   " << data.sequence   << '\n';
    std::cout << "Timestamp:  " << data.timestamp
              << " ms (since Snowflake epoch)\n";
 
    auto tp = std::chrono::system_clock::time_point{
        std::chrono::milliseconds(data.unix_ms())
    };

    std::time_t t = std::chrono::system_clock::to_time_t(tp);

    
    std::cout << "Generated at: " << std::ctime(&t);

    return 0;
}