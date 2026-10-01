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
        // Milliseconds elapsed since SnowflakeConfig::EPOCH.
        // Add EPOCH to convert to a Unix timestamp in milliseconds.
        uint64_t timestamp;

        // Returns the absolute Unix timestamp in milliseconds.
        [[nodiscard]] uint64_t unix_ms() const noexcept
        {
            return timestamp + SnowflakeConfig::EPOCH;
        }
    };

    // Throws std::invalid_argument if worker_id > SnowflakeConfig::MAX_WORKER_ID.
    // The accepted range is [0, 1023].
    explicit Snowflake(uint16_t worker_id);

    // Generates a unique, time-ordered 64-bit Snowflake ID.
    // Thread-safe; may spin briefly on sequence overflow or a small clock drift.
    // Throws std::runtime_error on clock rollback beyond ROLLBACK_TOLERANCE_MS,
    // or if the current timestamp exceeds the 41-bit range (~year 2095).
    //
    // The class is non-copyable because of the internal mutex.
    [[nodiscard]] uint64_t generate();

    // Decodes a previously generated ID into its components.
    // Does not need an instance; declared static.
    static DecodedID decode(uint64_t id) noexcept;

private:

    // White-box test seam.  SnowflakeTestHook (defined in tests/overflow.cpp)
    // can read and write private fields to set up specific test scenarios.
    // It is never instantiated in production code.
    friend struct SnowflakeTestHook;

    uint16_t worker_id_;
    uint16_t sequence_;
    uint64_t last_timestamp_;

    std::mutex mutex_;
};