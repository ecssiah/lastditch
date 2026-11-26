// ============================================================================
// File:    tests/test_main.c
// Layer:   test
// Purpose: Entry point and framework of the test program. Runs every suite
//          listed in TEST_SUITE_TABLE and reports the number of failed
//          checks through the exit status.
// ============================================================================

#include "test.h"

#include <stdio.h>
#include <string.h>

#include "core/geometry.h"

// ----------------------------------------------------------------------------
// Constants: test program limits
// Purpose:   Fixed sizes used by the test framework.
// ----------------------------------------------------------------------------
#define TEST_SUITE_COUNT 5                  // Rows of TEST_SUITE_TABLE.
#define TEST_PATH_CAPACITY 1024             // Longest asset directory, with terminator.

// ----------------------------------------------------------------------------
// Record:  TestSuiteRecord
// Purpose: Row type of TEST_SUITE_TABLE.
// ----------------------------------------------------------------------------
typedef struct TestSuiteRecord
{
    const char* name;               // Name printed when the suite runs.
    TestSuiteFunction run;          // The suite.
} TestSuiteRecord;

// ----------------------------------------------------------------------------
// State:   TestState
// Purpose: What the framework remembers across checks and suites.
// ----------------------------------------------------------------------------
typedef struct TestState
{
    s32 check_count;                            // Checks evaluated so far.
    s32 failure_count;                          // Checks that failed so far.
    int world_is_generated;                     // Whether the shared world has been generated.
    World world;                                // The shared world fixture.
    char asset_directory[TEST_PATH_CAPACITY];   // Asset directory, ending with a separator.
} TestState;

// ----------------------------------------------------------------------------
// Reference: TEST_SUITE_TABLE
// Purpose:   Every test suite, in the order in which they run.
// Key:       name
// Order:     execution order: lower layers first
// Lifetime:  compiled in, read-only
// ----------------------------------------------------------------------------
static const TestSuiteRecord TEST_SUITE_TABLE[TEST_SUITE_COUNT] =
{
    { "core",         test_core_run },
    { "data",         test_data_run },
    { "world",        test_world_run },
    { "simulation",   test_simulation_run },
    { "presentation", test_presentation_run }
};

// ----------------------------------------------------------------------------
// State:   test_state
// Purpose: The one instance of the framework state.
// ----------------------------------------------------------------------------
static TestState test_state;

// Documented in tests/test.h.
void
test_check(const int passed, const char* expression, const char* file, const int line)
{
    test_state.check_count++;

    if (!passed)
    {
        test_state.failure_count++;

        fprintf(stderr, "FAILED %s:%d: %s\n", file, line, expression);
    }
}

// Documented in tests/test.h.
void
test_check_s32(const s32 actual, const s32 expected, const char* expression, const char* file, const int line)
{
    test_state.check_count++;

    if (actual != expected)
    {
        test_state.failure_count++;

        fprintf(stderr, "FAILED %s:%d: %s is %ld, expected %ld\n", file, line, expression, (long)actual, (long)expected);
    }
}

// Documented in tests/test.h.
void
test_check_f32(const f32 actual, const f32 expected, const f32 tolerance, const char* expression, const char* file, const int line)
{
    test_state.check_count++;

    // Written so that a NaN fails the check.
    if (!(f32_abs(actual - expected) <= tolerance))
    {
        test_state.failure_count++;

        fprintf(stderr, "FAILED %s:%d: %s is %.9g, expected %.9g\n", file, line, expression, (double)actual, (double)expected);
    }
}

// Documented in tests/test.h.
u64
test_make_u64(const u32 high, const u32 low)
{
    return ((u64)high << 32) | (u64)low;
}

// Documented in tests/test.h.
u64
test_hash_begin(void)
{
    return test_make_u64(0xCBF29CE4u, 0x84222325u);
}

// Documented in tests/test.h.
u64
test_hash_bytes(u64 hash, const void* data, const size_t size)
{
    const u64 prime = test_make_u64(0x00000100u, 0x000001B3u);
    const u8* bytes = (const u8*)data;
    size_t index;

    for (index = 0; index < size; ++index)
    {
        hash ^= bytes[index];
        hash *= prime;
    }

    return hash;
}

// Documented in tests/test.h.
World*
test_get_world(void)
{
    if (!test_state.world_is_generated)
    {
        world_init(&test_state.world);

        test_state.world_is_generated = true;
    }

    return &test_state.world;
}

// Documented in tests/test.h.
const char*
test_get_asset_directory(void)
{
    return test_state.asset_directory;
}

// ----------------------------------------------------------------------------
// Function: main
// Purpose:  Runs every test suite.
// Params:   argc - number of command line arguments
//           argv - command line; argv[1], when present, is the directory
//                  that holds the game's assets
// Returns:  0 when every check passed, 1 otherwise.
// Reads:    TEST_SUITE_TABLE
// Writes:   test_state; stdout and stderr
// ----------------------------------------------------------------------------
int
main(int argc, char** argv)
{
    const char* asset_directory = argc > 1 ? argv[1] : "assets";
    size_t length = strlen(asset_directory);
    s32 row;

    if (length + 2 > sizeof(test_state.asset_directory))
    {
        fprintf(stderr, "asset directory path is too long\n");

        return 1;
    }

    strcpy(test_state.asset_directory, asset_directory);

    if (length > 0 && asset_directory[length - 1] != '/' && asset_directory[length - 1] != '\\')
    {
        strcat(test_state.asset_directory, "/");
    }

    for (row = 0; row < TEST_SUITE_COUNT; ++row)
    {
        const s32 failures_before = test_state.failure_count;
        const s32 checks_before = test_state.check_count;

        TEST_SUITE_TABLE[row].run();

        printf(
            "suite %-12s %6ld checks, %ld failed\n",
            TEST_SUITE_TABLE[row].name,
            (long)(test_state.check_count - checks_before),
            (long)(test_state.failure_count - failures_before)
        );
    }

    if (test_state.world_is_generated)
    {
        world_quit(&test_state.world);
    }

    printf("total %ld checks, %ld failed\n", (long)test_state.check_count, (long)test_state.failure_count);

    return test_state.failure_count == 0 ? 0 : 1;
}
