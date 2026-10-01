#pragma once

#include <cstdint>
#include <mutex>

#include "snow_flake_config.hpp"

class Snowflake
{
public:

    struct DecodedID
    {
        uint16_t worker_id;
        uint16_t sequence;
        uint64_t timestamp;
    };

    explicit Snowflake(uint16_t worker_id);

    uint64_t generate();

    DecodedID decode(uint64_t id);

private:

    uint16_t worker_id_;
    uint16_t sequence_;
    uint64_t last_timestamp_;

    std::mutex mutex_;
};