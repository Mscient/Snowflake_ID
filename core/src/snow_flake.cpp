 
#include <snowflake/snowflake.hpp>
#include <snowflake/clock.hpp>
#include <stdexcept>

Snowflake::Snowflake(uint16_t worker_id)
    : worker_id_(worker_id),
      sequence_(0),
      last_timestamp_(0)
{
    if (worker_id > SnowflakeConfig::MAX_WORKER_ID)
    {
        throw std::invalid_argument(
            "Worker ID exceeds maximum allowed value"
        );
    }
}

Snowflake::DecodedID Snowflake::decode(uint64_t id)
{
    DecodedID data;

    data.timestamp =
        id >> SnowflakeConfig::TIMESTAMP_SHIFT;

    data.worker_id =
        (id >> SnowflakeConfig::WORKER_ID_SHIFT)
        & SnowflakeConfig::MAX_WORKER_ID;

    data.sequence =
        (id >> SnowflakeConfig::SEQUENCE_SHIFT)
        & SnowflakeConfig::MAX_SEQUENCE;

    return data;
}

uint64_t Snowflake::generate()
{
    std::lock_guard<std::mutex> lock(mutex_);

    uint64_t timestamp = SnowflakeClock::get_time();

    if (timestamp > last_timestamp_)
    {
        sequence_ = 0;
    }
    else if (timestamp == last_timestamp_)
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
    else
    {
        throw std::runtime_error(
            "Clock moved backwards"
        );
    }

    last_timestamp_ = timestamp;

    return
        (timestamp << SnowflakeConfig::TIMESTAMP_SHIFT)
        | (static_cast<uint64_t>(worker_id_)
           << SnowflakeConfig::WORKER_ID_SHIFT)
        | (static_cast<uint64_t>(sequence_)
           << SnowflakeConfig::SEQUENCE_SHIFT);
}