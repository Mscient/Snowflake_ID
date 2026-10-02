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

     
    static constexpr uint64_t MAX_TIMESTAMP =
        (1ULL << TIMESTAMP_BITS) - 1;

    static constexpr int SEQUENCE_SHIFT  = 0;

    static constexpr int WORKER_ID_SHIFT =
        SEQUENCE_BITS;

    static constexpr int TIMESTAMP_SHIFT =
        SEQUENCE_BITS + WORKER_ID_BITS;

   
    static constexpr uint64_t EPOCH =
        1767225600000ULL;

 
    static constexpr uint64_t ROLLBACK_TOLERANCE_MS = 5ULL;
};