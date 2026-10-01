#pragma once

#include <cstdint>

class SnowflakeClock
{
public:
    static uint64_t get_time();
};