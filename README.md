
## TLFloat - C++ template library for floating point operations

Original distribution site : https://github.com/shibatch/tlfloat

Doxygen-generated reference : https://shibatch.sourceforge.net/tlfloat-doxygen/


### Features

* Truly constexpr functions
  * Compilable with C++20 standard
  * Most of the functions are implemented as templates
    * Completely inlinable functions
    * The functions can be evaluated at compile time
  * No malloc required
  * Works without libc++

* IEEE 754 compliant
  * Supports subnormal numbers, NaN, inf, and signed zero

* Supports a wide range of precisions
  * Half, float, double, quad, octuple
  * Returns correctly rounded results for arithmetic oprations, fma and sqrt
  * Returns 1-ulp accuracy results for other math.h functions
    * All functions, including trigonometric functions, return 1ulp-accuracy results for all input range

* Portable
  * Compatible with Linux, Windows, microcontrollers, wasm, CUDA (version 12 or later)
  * Constexpr functions can be called from CUDA devices with --expt-relaxed-constexpr compiler option

* C language API / libquadmath emulation
  * Most of libquadmath functions can be used with x86_64 clang

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
  * Conversion to/from C strings
  * printf-family functions

* The library provides BigInt template classes in addition to the FP classes
  * It provides operations for integers of artibrary length (2^N bits)
  * They can be used in the similar way to the ordinary int/uint types
  * Data formats are the same as ordinary int/uint
  * These classes are internally used to implement the TLFloat FP classes



### How to build

1. Check out the source code from our GitHub repository :
`git clone https://github.com/shibatch/tlfloat`

2. Make a separate directory to create an out-of-source build :
`cd tlfloat && mkdir build && cd build`

3. Run cmake to configure the project :
`cmake .. -DCMAKE_INSTALL_PREFIX=../../install`

4. Run make to build and install the project :
`make && make install`



### Compiling hello world example

Below is a simple C++ source code utilizing TLFloat.

```
#include <iostream>
#include <tlfloat/tlmath.hpp>

using namespace tlfloat;

Octuple machin() {
  return 4 * (4 * atan(1 / Octuple(5)) - atan(1 / Octuple(239)));
}

int main(int argc, char **argv) {
  std::cout << to_string(machin(), 70) << std::endl;
}
```

To compile this source code, use the following command.

`g++ -std=c++20 -I./install/include hello.cpp`

You have to specify C++20 standard. Note that you do not need to link any library in this example. This program computes PI in octuple precision and shows it.

```
$ ./a.out
3.141592653589793238462643383279502884197169399375105820974944592307816
```


### Development status

* This software is currently at the alpha development stage

* The following features will be added in future releases
  * Further documentation
  * More testing
    * Add more testers for I/O functions
  * Pre-C++20 API
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
