
TLFloat - template library for floating point operations

* Truly constexpr
  * Compilable with C++20 standard
  * Most of the library is implemented as templates
  * No malloc required
  * It works without libc++

* Portable
  * Compatible with Linux, Windows, microcontrollers, wasm, CUDA (version 12)
* Moderately optimized
  * Optimized for each architecture using intrinsics, etc.

* IEEE 754 compliant
  * Supports subnormal numbers, NaN, Inf, negative zero, etc.
* Supports a wide range or precisions
  * Half, float, double, quad, octuple
* Implements most of the math.h functions
  * Arithmetic operations, comparison, cast operations
  * fma, sqrt, hypot, cbrt, fmod, remainder
  * sin, cos, tan, asin, acos, atan, atan2
  * log, log2, log10, log1p, exp, exp2, exp10, expm1, pow
  * sinh, cosh, tanh, asinh, acosh, atanh
  * trunc, floor, ceil, round, rint
  * fabs, copysign, fmax, fmin, fdim
  * ldexp, frexp, modf, nextafter
  * isnan, isinf, finite, signbit
* I/O function
* High precision
  * Returns correctly rounded results for arithmetic oprations, fma and sqrt
  * Returns 1-ulp precision results for other math.h functions
    * All functions, including trigonometric functions, return 1ulp-precision results for all input range

* The library provides BigInt class in addition to the FP classes
  * It provides operations for integers of artibrary length (2^N bits)
  * They can be used in the similar way to the ordinary int/uint types
  * Data formats are the same as ordinary int/uint


* This software is currently at the alpha development stage, and thus there may be major changes to the API.

* The following features will be added in future releases
  * Proper documentation
  * Add testers for some unimplemented items
    * Testers for I/O functions
  * Complete printf functionality
  * C API / old C++ API
  * Big endian support
  * Support for other math functions
  * Further optimization
