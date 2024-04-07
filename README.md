
## TLFloat - C++ template library for floating point operations

Original distribution site : https://github.com/shibatch/tlfloat


### Features

* Truly constexpr functions
  * Compilable with C++20 standard
  * Most of the functions are implemented as templates
    * Completely inlinable functions
    * The functions can be evaluated at compile time
  * No malloc required
  * Works without libc++

* IEEE 754 compliant
  * Supports subnormal numbers, NaN, inf, and signed zero.

* Supports a wide range of precisions
  * Half, float, double, quad, octuple
  * Returns correctly rounded results for arithmetic oprations, fma and sqrt
  * Returns 1-ulp accuracy results for other math.h functions
    * All functions, including trigonometric functions, return 1ulp-accuracy results for all input range

* Portable
  * Compatible with Linux, Windows, microcontrollers, wasm, CUDA (version 12 or later)

* Moderately optimized
  * Optimized for each architecture using intrinsics, etc.
  * Library design allows compilers to fully inline operations

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
* I/O functions

* The library provides BigInt template classes in addition to the FP classes
  * It provides operations for integers of artibrary length (2^N bits)
  * They can be used in the similar way to the ordinary int/uint types
  * Data formats are the same as ordinary int/uint
  * These classes are internally used to implement the TLFloat FP classes


### Development status

* This software is currently at the alpha development stage, and thus there may be major changes to the API.

* The following features will be added in future releases
  * Proper documentation
  * Add testers for certain unimplemented items
    * Testers for I/O functions
  * Complete printf functionality
  * C API / pre-C++20 API
  * Interoperability with SLEEF
  * Big endian support
  * Remaining math functions in math.h
  * BF16 support
  * Further optimization
  * Benchmarking tools


### License

The software is distributed under the Boost Software License, Version 1.0.
See accompanying file LICENSE.txt or copy at :
http://www.boost.org/LICENSE_1_0.txt.

Contributions to this project are accepted under the same license.

Copyright Naoki Shibata 2024.
