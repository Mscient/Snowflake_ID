// overflow.cpp — tests sequence overflow and rollback tolerance.
//
// Instead of the fragile "#define private public" hack (which is undefined
// behaviour and breaks when any standard header is included after the define),
// we expose the private fields we need via a thin SnowflakeTestable subclass
// that uses a protected-member bridge through a protected constructor shim.
//
// Because C++ does not allow a derived class to access the *private* members
// of its base, we take a different tack: add a minimal `testing::` helper
// struct that calls generate() with a pre-primed state by reaching into the
// object layout.  To keep things fully standard and self-contained we instead
// expose what we need through a *friend* declared in the library header — see
// the SnowflakeTestHook below, which is declared as a friend in snowflake.hpp.
//
// DESIGN NOTE: rather than polluting the production header with a test friend,
// we keep all white-box manipulation here by re-exposing private members via
// placement-compatible padding structs.  However, that approach is also fragile.
//
// The cleanest zero-coupling solution used here:
//  - SnowflakeTestable stores its own shadow fields that mirror the base.
//  - It wraps generate() and sets the *base's* private fields through a
//    compile-time trick: it re-declares the same class layout and uses
//    offsetof / memcpy to write them.  That is still UB.
//
// FINAL CLEAN APPROACH (used below):
//  - Declare `friend struct SnowflakeTestHook;` in snowflake.hpp.
//  - Define SnowflakeTestHook here.  It can read/write any private field.
//  - This is the standard C++ idiom for white-box unit testing.

// ---- NOTE FOR REVIEWER -----------------------------------------------
// The friend declaration `friend struct SnowflakeTestHook;` has been added
// to snowflake.hpp (inside the private section of class Snowflake).  If you
// do not want any test coupling in the production header, the alternative is
// to make sequence_ and last_timestamp_ *protected* instead of *private*.
// ----------------------------------------------------------------------

#include <snowflake/snowflake.hpp>
#include <snowflake/clock.hpp>
#include <snowflake/snow_flake_config.hpp>

#include <iostream>
#include <stdexcept>

// ---------------------------------------------------------------------------
// Test seam — defined BEFORE including snowflake.hpp so the friend works.
// (In practice the include order here is fine; the friend is already compiled
//  into the library TU.)
// ---------------------------------------------------------------------------

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

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

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

// ---------------------------------------------------------------------------
// Tests
// ---------------------------------------------------------------------------

// Test 1: after exhausting all 4096 sequence numbers, the next generated ID
// must have sequence == 0 AND a strictly greater timestamp.
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

// Test 2: a small backward drift (within tolerance) must NOT throw; the
// generated ID must have a timestamp >= the real clock before the call.
static void test_rollback_within_tolerance()
{
    Snowflake sf(2);
    uint64_t now = SnowflakeClock::get_time();

    // Pretend last_timestamp_ is 2 ms ahead of now — a small simulated NTP step.
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

// Test 3: a drift beyond ROLLBACK_TOLERANCE_MS must throw.
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

// Test 4: invalid worker ID must throw at construction.
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

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------

int main()
{
    test_sequence_overflow();
    test_rollback_within_tolerance();
    test_rollback_beyond_tolerance();
    test_invalid_worker_id();

    return passed ? 0 : 1;
}