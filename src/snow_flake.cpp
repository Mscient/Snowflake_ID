#include <snowflake/snowflake.hpp>
#include <snowflake/clock.hpp>
#include <stdexcept>
#include <thread>
#include <chrono>

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------

Snowflake::Snowflake(uint16_t worker_id)
{ 
    if (worker_id > SnowflakeConfig::MAX_WORKER_ID)
    {
        throw std::invalid_argument(
            "Worker ID " + std::to_string(worker_id)
            + " exceeds maximum allowed value "
            + std::to_string(SnowflakeConfig::MAX_WORKER_ID)
        );
    }

    worker_id_      = worker_id;
    sequence_       = 0;
    last_timestamp_ = 0;
}

 
Snowflake::DecodedID Snowflake::decode(uint64_t id) noexcept
{
    DecodedID data;

    data.timestamp =
        id >> SnowflakeConfig::TIMESTAMP_SHIFT;

    data.worker_id =
        static_cast<uint16_t>(
            (id >> SnowflakeConfig::WORKER_ID_SHIFT)
            & SnowflakeConfig::MAX_WORKER_ID
        );

    data.sequence =
        static_cast<uint16_t>(
            (id >> SnowflakeConfig::SEQUENCE_SHIFT)
            & SnowflakeConfig::MAX_SEQUENCE
        );

    return data;
}

 
uint64_t Snowflake::generate()
{
    std::lock_guard<std::mutex> lock(mutex_);

    uint64_t timestamp = SnowflakeClock::get_time();
 
    if (timestamp < last_timestamp_)
    {
        const uint64_t drift = last_timestamp_ - timestamp;

        if (drift > SnowflakeConfig::ROLLBACK_TOLERANCE_MS)
        {
            throw std::runtime_error(
                "Clock moved backward by "
                + std::to_string(drift)
                + " ms, which exceeds the tolerance of "
                + std::to_string(SnowflakeConfig::ROLLBACK_TOLERANCE_MS)
                + " ms"
            );
        }
 
        do
        {
            std::this_thread::yield();
            timestamp = SnowflakeClock::get_time();
        } while (timestamp < last_timestamp_);
    }

     
    if (timestamp > SnowflakeConfig::MAX_TIMESTAMP)
    {
        throw std::runtime_error(
            "Timestamp " + std::to_string(timestamp)
            + " exceeds the 41-bit maximum ("
            + std::to_string(SnowflakeConfig::MAX_TIMESTAMP)
            + "). The Snowflake epoch has expired."
        );
    }
 
    if (timestamp > last_timestamp_)
    { 
        sequence_ = 0;
    }
    else
    {
       
        if (sequence_ == SnowflakeConfig::MAX_SEQUENCE)
        { 
            do
            {
                timestamp = SnowflakeClock::get_time();
            } while (timestamp <= last_timestamp_);

           
            sequence_ = 0;
        }
        else
        {
            sequence_++;
        }
    }

    last_timestamp_ = timestamp;

    return
        (timestamp << SnowflakeConfig::TIMESTAMP_SHIFT)
        | (static_cast<uint64_t>(worker_id_)
           << SnowflakeConfig::WORKER_ID_SHIFT)
        | (static_cast<uint64_t>(sequence_)
           << SnowflakeConfig::SEQUENCE_SHIFT);
}