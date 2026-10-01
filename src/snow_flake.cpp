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
    // Validate before touching member state so the object is never partially
    // constructed with a bad worker ID.
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

// ---------------------------------------------------------------------------
// decode  (static — uses no instance state)
// ---------------------------------------------------------------------------

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

// ---------------------------------------------------------------------------
// generate
// ---------------------------------------------------------------------------

uint64_t Snowflake::generate()
{
    std::lock_guard<std::mutex> lock(mutex_);

    uint64_t timestamp = SnowflakeClock::get_time();

    // ------------------------------------------------------------------
    // Clock rollback handling (item 1)
    // ------------------------------------------------------------------
    // NTP can step the clock backward by a few milliseconds.  A small
    // drift is tolerated by spinning until the clock catches up.  A
    // large drift (beyond ROLLBACK_TOLERANCE_MS) indicates a real problem
    // and is rejected with an exception.
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

        // Spin (or yield) until the clock catches up.  The drift is at
        // most ROLLBACK_TOLERANCE_MS milliseconds, so this loop is
        // bounded and completes quickly.
        do
        {
            std::this_thread::yield();
            timestamp = SnowflakeClock::get_time();
        } while (timestamp < last_timestamp_);
    }

    // ------------------------------------------------------------------
    // Timestamp range guard (item 2)
    // ------------------------------------------------------------------
    // After ~69.7 years from the custom epoch (roughly year 2095),
    // the 41-bit timestamp field overflows.  Detect this early so we
    // never produce a corrupt ID.
    if (timestamp > SnowflakeConfig::MAX_TIMESTAMP)
    {
        throw std::runtime_error(
            "Timestamp " + std::to_string(timestamp)
            + " exceeds the 41-bit maximum ("
            + std::to_string(SnowflakeConfig::MAX_TIMESTAMP)
            + "). The Snowflake epoch has expired."
        );
    }

    // ------------------------------------------------------------------
    // Sequence management
    // ------------------------------------------------------------------
    if (timestamp > last_timestamp_)
    {
        // New millisecond — reset the sequence counter.
        sequence_ = 0;
    }
    else
    {
        // Same millisecond (timestamp == last_timestamp_ after rollback handling).
        if (sequence_ == SnowflakeConfig::MAX_SEQUENCE)
        {
            // All 4096 sequence numbers for this millisecond are exhausted.
            // Spin until the clock advances to the next millisecond.
            //
            // NOTE: this loop busy-waits while holding the mutex, blocking
            // every other thread.  The wait is typically a few hundred
            // microseconds at most; it is acceptable for the common case
            // but worth noting for latency-sensitive workloads.
            do
            {
                timestamp = SnowflakeClock::get_time();
            } while (timestamp <= last_timestamp_);

            // sequence_ is reset once we drop into the timestamp > last_timestamp_
            // branch on the *next* call, but we reset it here immediately so the
            // ID we are about to return has sequence 0 for the new millisecond.
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