// ============================================================================
// File:    tests/test.h
// Layer:   test
// Purpose: The test program's small framework: check macros that record
//          failures without stopping, the shared fixtures and the entry
//          points of the test suites.
// ============================================================================

#ifndef TESTS_TEST_H
#define TESTS_TEST_H

#include "app/population.h"
#include "app/world.h"
#include "core/types.h"

// ----------------------------------------------------------------------------
// Macro:   TEST_CHECK, TEST_CHECK_S32, TEST_CHECK_F32
// Purpose: Record the outcome of one expectation, tagged with the source
//          line. TEST_CHECK takes a condition; TEST_CHECK_S32 compares two
//          integers and reports both on failure; TEST_CHECK_F32 compares
//          two floating point values within a tolerance. A failed check
//          does not stop the test, so one run reports every failure.
// ----------------------------------------------------------------------------
#define TEST_CHECK(condition) test_check((condition) ? true : false, #condition, __FILE__, __LINE__)
#define TEST_CHECK_S32(actual, expected) test_check_s32((s32)(actual), (s32)(expected), #actual, __FILE__, __LINE__)
#define TEST_CHECK_F32(actual, expected, tolerance) test_check_f32((f32)(actual), (f32)(expected), (f32)(tolerance), #actual, __FILE__, __LINE__)

// ----------------------------------------------------------------------------
// Type:    TestSuiteFunction
// Purpose: Signature of a test suite: a function that runs the checks of
//          one area of the code base.
// ----------------------------------------------------------------------------
typedef void (*TestSuiteFunction)(void);

// ----------------------------------------------------------------------------
// Function: test_check
// Purpose:  Records the outcome of one expectation and reports a failure.
// Params:   passed     - true when the expectation held
//           expression - source text of the expectation
//           file       - source file of the check
//           line       - source line of the check
// Returns:  nothing
// Reads:    arguments only
// Writes:   the check counters; stderr on failure
// ----------------------------------------------------------------------------
void test_check(int passed, const char* expression, const char* file, int line);

// ----------------------------------------------------------------------------
// Function: test_check_s32
// Purpose:  Records the outcome of an integer comparison.
// Params:   actual     - value computed by the code under test
//           expected   - value it should have
//           expression - source text of the actual value
//           file       - source file of the check
//           line       - source line of the check
// Returns:  nothing
// Reads:    arguments only
// Writes:   the check counters; stderr on failure
// ----------------------------------------------------------------------------
void test_check_s32(s32 actual, s32 expected, const char* expression, const char* file, int line);

// ----------------------------------------------------------------------------
// Function: test_check_f32
// Purpose:  Records the outcome of a floating point comparison.
// Params:   actual     - value computed by the code under test
//           expected   - value it should have
//           tolerance  - largest acceptable absolute difference
//           expression - source text of the actual value
//           file       - source file of the check
//           line       - source line of the check
// Returns:  nothing
// Reads:    arguments only
// Writes:   the check counters; stderr on failure
// ----------------------------------------------------------------------------
void test_check_f32(f32 actual, f32 expected, f32 tolerance, const char* expression, const char* file, int line);

// ----------------------------------------------------------------------------
// Function: test_hash_bytes
// Purpose:  Folds a run of bytes into a 64-bit FNV-1a hash, used to compare
//           whole table columns against recorded values.
// Params:   hash - hash so far; start with test_hash_begin()
//           data - bytes to fold in
//           size - number of bytes
// Returns:  The updated hash.
// Reads:    data
// Writes:   nothing
// ----------------------------------------------------------------------------
u64 test_hash_bytes(u64 hash, const void* data, size_t size);

// ----------------------------------------------------------------------------
// Function: test_hash_begin
// Purpose:  Produces the initial value of an FNV-1a hash.
// Params:   none
// Returns:  The FNV-1a offset basis.
// Reads:    nothing
// Writes:   nothing
// ----------------------------------------------------------------------------
u64 test_hash_begin(void);

// ----------------------------------------------------------------------------
// Function: test_make_u64
// Purpose:  Builds a 64-bit constant from two 32-bit halves; ANSI C89 has
//           no 64-bit integer literals.
// Params:   high - most significant 32 bits
//           low  - least significant 32 bits
// Returns:  The combined value.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
u64 test_make_u64(u32 high, u32 low);

// ----------------------------------------------------------------------------
// Function: test_get_world
// Purpose:  Provides the generated world shared by the suites. Generating
//           the world takes about a second, so it is done once, on first
//           use.
// Params:   none
// Returns:  The world as world_init left it. It is not const because the
//           work functions take a mutable world, but suites must not
//           modify it.
// Reads:    the shared world
// Writes:   the shared world, on first use
// ----------------------------------------------------------------------------
World* test_get_world(void);

// ----------------------------------------------------------------------------
// Function: test_get_asset_directory
// Purpose:  Provides the location of the game's assets for suites that
//           load files.
// Params:   none
// Returns:  The asset directory given on the command line, ending with a
//           separator, or "assets/" when none was given.
// Reads:    the command line, as recorded by main
// Writes:   nothing
// ----------------------------------------------------------------------------
const char* test_get_asset_directory(void);

// ----------------------------------------------------------------------------
// Function: test_core_run
// Purpose:  Runs the checks of the core layer: table primitives, random
//           numbers and geometry.
// Params:   none
// Returns:  nothing
// Reads:    nothing
// Writes:   the check counters
// ----------------------------------------------------------------------------
void test_core_run(void);

// ----------------------------------------------------------------------------
// Function: test_data_run
// Purpose:  Runs the checks of the data layer: the runtime tables, their
//           indexes and the reference tables.
// Params:   none
// Returns:  nothing
// Reads:    the reference tables
// Writes:   the check counters
// ----------------------------------------------------------------------------
void test_data_run(void);

// ----------------------------------------------------------------------------
// Function: test_world_run
// Purpose:  Runs the checks of world generation and of the world queries.
// Params:   none
// Returns:  nothing
// Reads:    the shared world
// Writes:   the check counters
// ----------------------------------------------------------------------------
void test_world_run(void);

// ----------------------------------------------------------------------------
// Function: test_simulation_run
// Purpose:  Runs the checks of the simulation: population, physics, the
//           work schedule and player control.
// Params:   none
// Returns:  nothing
// Reads:    the shared world
// Writes:   the check counters
// ----------------------------------------------------------------------------
void test_simulation_run(void);

// ----------------------------------------------------------------------------
// Function: test_presentation_run
// Purpose:  Runs the checks of the CPU side of presentation: screen labels,
//           the debug overlay, sector meshing and model loading.
// Params:   none
// Returns:  nothing
// Reads:    the shared world, the asset files
// Writes:   the check counters
// ----------------------------------------------------------------------------
void test_presentation_run(void);

#endif
