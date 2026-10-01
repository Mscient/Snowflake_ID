#pragma once

#include <chrono>
#include <cstdint>

struct SnowflakeConfig
{
    static constexpr int TIMESTAMP_BITS = 41;
    static constexpr int WORKER_ID_BITS = 10;
    static constexpr int SEQUENCE_BITS  = 12;

    static constexpr uint64_t MAX_WORKER_ID =
        (1ULL << WORKER_ID_BITS) - 1;

    static constexpr uint64_t MAX_SEQUENCE =
        (1ULL << SEQUENCE_BITS) - 1;

    // Maximum value that fits in TIMESTAMP_BITS (~69.7 years from epoch).
    // IDs generated after this point would overflow into the sign bit.
    static constexpr uint64_t MAX_TIMESTAMP =
        (1ULL << TIMESTAMP_BITS) - 1;

    static constexpr int SEQUENCE_SHIFT  = 0;

    static constexpr int WORKER_ID_SHIFT =
        SEQUENCE_BITS;

    static constexpr int TIMESTAMP_SHIFT =
        SEQUENCE_BITS + WORKER_ID_BITS;

    // Custom epoch: 2026-01-01 00:00:00 UTC in milliseconds since Unix epoch.
    static constexpr uint64_t EPOCH =
        1767225600000ULL;

    // Tolerance for small NTP-induced clock rollbacks (milliseconds).
    // If the clock drifts backward by at most this amount the generator
    // spins until it catches up; beyond this it throws.
    static constexpr uint64_t ROLLBACK_TOLERANCE_MS = 5ULL;
};