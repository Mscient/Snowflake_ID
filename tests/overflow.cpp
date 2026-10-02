
#include <snowflake/snowflake.hpp>
#include <snowflake/clock.hpp>
#include <snowflake/snow_flake_config.hpp>

#include <iostream>
#include <stdexcept>

 
struct SnowflakeTestHook
{
    static void set_sequence(Snowflake& sf, uint16_t s)
    {
        sf.sequence_ = s;
    }

    static void set_last_timestamp(Snowflake& sf, uint64_t t)
    {
        sf.last_timestamp_ = t;
    }
};
 
static bool passed = true;

static void expect(bool condition, const char* description)
{
    if (condition)
        std::cout << "[PASS] " << description << '\n';
    else
    {
        std::cout << "[FAIL] " << description << '\n';
        passed = false;
    }
}

 
static void test_sequence_overflow()
{
    Snowflake sf(1);
    uint64_t ts_before = SnowflakeClock::get_time();

    SnowflakeTestHook::set_sequence(sf, SnowflakeConfig::MAX_SEQUENCE);
    SnowflakeTestHook::set_last_timestamp(sf, ts_before);

    uint64_t id = sf.generate();
    auto data   = Snowflake::decode(id);

    expect(data.sequence == 0,
           "Sequence overflow: sequence resets to 0");

    expect(data.timestamp > ts_before,
           "Sequence overflow: new timestamp strictly greater than previous");
}

static void test_rollback_within_tolerance()
{
    Snowflake sf(2);
    uint64_t now = SnowflakeClock::get_time();

  
    SnowflakeTestHook::set_last_timestamp(sf, now + 2);

    bool threw = false;
    uint64_t id = 0;
    try { id = sf.generate(); }
    catch (const std::runtime_error&) { threw = true; }

    expect(!threw,
           "Rollback within tolerance (2 ms): no exception");

    if (!threw)
    {
        auto data = Snowflake::decode(id);
        expect(data.timestamp >= now,
               "Rollback within tolerance: returned timestamp is sane");
    }
}

 
static void test_rollback_beyond_tolerance()
{
    Snowflake sf(3);
    uint64_t now = SnowflakeClock::get_time();

    SnowflakeTestHook::set_last_timestamp(
        sf, now + SnowflakeConfig::ROLLBACK_TOLERANCE_MS + 10
    );

    bool threw = false;
    try { [[maybe_unused]] auto _ = sf.generate(); }
    catch (const std::runtime_error&) { threw = true; }

    expect(threw,
           "Rollback beyond tolerance: exception thrown");
}

 
static void test_invalid_worker_id()
{
    bool threw = false;
    try
    {
        Snowflake sf(
            static_cast<uint16_t>(SnowflakeConfig::MAX_WORKER_ID + 1)
        );
    }
    catch (const std::invalid_argument&) { threw = true; }

    expect(threw, "Invalid worker ID: exception thrown");
}

 
int main()
{
    test_sequence_overflow();
    test_rollback_within_tolerance();
    test_rollback_beyond_tolerance();
    test_invalid_worker_id();

    return passed ? 0 : 1;
}