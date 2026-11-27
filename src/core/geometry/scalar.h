// ============================================================================
// File:    src/core/geometry/scalar.h
// Layer:   core
// Purpose: Scalar arithmetic helpers. ANSI C89 only provides the double
//          precision math library, so every single precision operation the
//          application needs is wrapped here; the wrappers compute in double
//          and round the result to f32.
// ============================================================================

#ifndef CORE_GEOMETRY_SCALAR_H
#define CORE_GEOMETRY_SCALAR_H

#include <float.h>

#include "core/types.h"

// ----------------------------------------------------------------------------
// Constants: scalar limits
// Purpose:   Numeric constants of the f32 type used by the geometry library.
// ----------------------------------------------------------------------------
// Ratio of a circle's circumference to its diameter.
#define F32_PI 3.14159265358979323846f
// Distance from 1.0f to the next larger f32.
#define F32_EPSILON FLT_EPSILON

// ----------------------------------------------------------------------------
// Value:   SinCos
// Purpose: Sine and cosine of one angle, computed together because the
//          callers in this library always need both.
// ----------------------------------------------------------------------------
typedef struct SinCos
{
    // Sine of the angle.
    f32 sin;
    // Cosine of the angle.
    f32 cos;
} SinCos;

// ----------------------------------------------------------------------------
// Function: f32_to_radians
// Purpose:  Converts an angle from degrees to radians.
// Params:   degrees - angle in degrees
// Returns:  The same angle in radians.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
f32 f32_to_radians(f32 degrees);

// ----------------------------------------------------------------------------
// Function: f32_to_degrees
// Purpose:  Converts an angle from radians to degrees.
// Params:   radians - angle in radians
// Returns:  The same angle in degrees.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
f32 f32_to_degrees(f32 radians);

// ----------------------------------------------------------------------------
// Function: f32_square_root
// Purpose:  Computes a square root in single precision.
// Params:   value - number to take the root of; negative values yield NaN
// Returns:  The square root of value.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
f32 f32_square_root(f32 value);

// ----------------------------------------------------------------------------
// Function: f32_sin_cos
// Purpose:  Computes the sine and cosine of an angle.
// Params:   angle - angle in radians
// Returns:  Both values as a SinCos.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
SinCos f32_sin_cos(f32 angle);

// ----------------------------------------------------------------------------
// Function: f32_sin
// Purpose:  Computes the sine of an angle.
// Params:   angle - angle in radians
// Returns:  The sine of angle.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
f32 f32_sin(f32 angle);

// ----------------------------------------------------------------------------
// Function: f32_cos
// Purpose:  Computes the cosine of an angle.
// Params:   angle - angle in radians
// Returns:  The cosine of angle.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
f32 f32_cos(f32 angle);

// ----------------------------------------------------------------------------
// Function: f32_tan
// Purpose:  Computes the tangent of an angle.
// Params:   angle - angle in radians
// Returns:  The tangent of angle.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
f32 f32_tan(f32 angle);

// ----------------------------------------------------------------------------
// Function: f32_acos
// Purpose:  Computes the arc cosine of a value.
// Params:   value - cosine of the wanted angle, in [-1, 1]
// Returns:  The angle in radians, in [0, pi].
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
f32 f32_acos(f32 value);

// ----------------------------------------------------------------------------
// Function: f32_exp
// Purpose:  Computes the natural exponential of a value.
// Params:   value - exponent
// Returns:  e raised to the power value.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
f32 f32_exp(f32 value);

// ----------------------------------------------------------------------------
// Function: f32_floor
// Purpose:  Rounds a value towards negative infinity.
// Params:   value - number to round
// Returns:  The largest whole number that is not greater than value.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
f32 f32_floor(f32 value);

// ----------------------------------------------------------------------------
// Function: f32_ceil
// Purpose:  Rounds a value towards positive infinity.
// Params:   value - number to round
// Returns:  The smallest whole number that is not less than value.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
f32 f32_ceil(f32 value);

// ----------------------------------------------------------------------------
// Function: f32_abs
// Purpose:  Computes the magnitude of a value.
// Params:   value - number to take the magnitude of
// Returns:  value without its sign.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
f32 f32_abs(f32 value);

// ----------------------------------------------------------------------------
// Function: f32_min
// Purpose:  Selects the smaller of two values.
// Params:   left  - first value
//           right - second value
// Returns:  right when it is less than left, otherwise left.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
f32 f32_min(f32 left, f32 right);

// ----------------------------------------------------------------------------
// Function: f32_max
// Purpose:  Selects the larger of two values.
// Params:   left  - first value
//           right - second value
// Returns:  right when it is greater than left, otherwise left.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
f32 f32_max(f32 left, f32 right);

// ----------------------------------------------------------------------------
// Function: f32_clamp
// Purpose:  Limits a value to a closed range.
// Params:   value   - number to limit
//           minimum - lower end of the range
//           maximum - upper end of the range; maximum >= minimum
// Returns:  minimum, maximum or value, whichever lies inside the range.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
f32 f32_clamp(f32 value, f32 minimum, f32 maximum);

// ----------------------------------------------------------------------------
// Function: f32_infinity
// Purpose:  Produces positive infinity, for which C89 has no f32 constant.
// Params:   none
// Returns:  Positive infinity.
// Reads:    nothing
// Writes:   nothing
// ----------------------------------------------------------------------------
f32 f32_infinity(void);

// ----------------------------------------------------------------------------
// Function: s32_min
// Purpose:  Selects the smaller of two integers.
// Params:   left  - first value
//           right - second value
// Returns:  The smaller value.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
s32 s32_min(s32 left, s32 right);

// ----------------------------------------------------------------------------
// Function: s32_max
// Purpose:  Selects the larger of two integers.
// Params:   left  - first value
//           right - second value
// Returns:  The larger value.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
s32 s32_max(s32 left, s32 right);

#endif
