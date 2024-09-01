
## TLFloat - C++ template library for floating point operations

Original distribution site : https://github.com/shibatch/tlfloat

Doxygen-generated reference : https://shibatch.sourceforge.net/tlfloat-doxygen/


### Introduction

This library implements C++ classes with which half, single, double,
quadruple and octuple precision IEEE 754 floating point numbers can be
operated.

Internally, these classes are implemented as class templates on top of
arbitrary-precision integer class templates so that the templates are
expanded as arbitrary precision floating-point operations by just
changing the template parameters, rather than implementing each
floating-point operation for each precision. The arbitrary-precision
integer class templates are also included in this library.


### Features

* Truly constexpr functions
  * Compilable with C++20 standard
  * Most of the functions are implemented as templates
    * Completely inlinable functions
    * The functions can be evaluated at compile time
  * No malloc required
  * Works without libstdc++

* IEEE 754 compliant
  * Supports subnormal numbers, NaN, infinity, and signed zero

* Supports a wide range of precisions
  * Half, float, double, quad, and octuple precisions
  * Returns correctly rounded results for arithmetic oprations, fma and sqrt
  * Returns 1-ulp accuracy results for other math.h functions
    * All functions, including trigonometric functions, return 1ulp-accuracy results for all input range

* Portable
  * Compatible with Linux, Windows, microcontrollers, wasm, CUDA (version 12 or later)
  * Constexpr functions can be called from CUDA devices with --expt-relaxed-constexpr compiler option

* C/C++11 API with libquadmath emulation
  * Most of libquadmath functions can be used on x86_64 clang and MSVC
  * 128-bit integer types can be used on MSVC
  * C++11 FP and int classes with overloaded operators are provided
    * C++11 functions in TLFloat are not constexpr

* Moderately optimized
  * Optimized for each architecture using intrinsics, etc.
  * Library design allows compilers to fully inline operations
  * All functions are thread-safe and reentrant

* Implements most of the math.h functions
  * Arithmetic operations, comparison, cast operations
  * fma, sqrt, hypot, cbrt, fmod, remainder, remquo
  * sin, cos, tan, asin, acos, atan, atan2
  * log, log2, log10, log1p, exp, exp2, exp10, expm1, pow
  * sinh, cosh, tanh, asinh, acosh, atanh
  * erf, erfc, tgamma
  * trunc, floor, ceil, round, rint
  * fabs, copysign, fmax, fmin, fdim
  * ldexp, frexp, modf, nextafter
  * isnan, isinf, finite, fpclassify, signbit

* Implements I/O functions
  * Conversion to/from C strings
  * printf-family functions

* Provides BigInt template classes in addition to the FP classes
  * It provides operations for integers of artibrary length (2^N bits)
  * They can be used in the similar way to the ordinary int/uint types
  * Data formats are the same as ordinary int/uint
  * These classes are internally used to implement the FP classes in TLFloat


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
#include <iomanip>
#include <tlfloat/tlmath.hpp>

using namespace tlfloat;

Octuple machin() {
  return 4 * (4 * atan(1 / Octuple(5)) - atan(1 / Octuple(239)));
}

int main(int argc, char **argv) {
  std::cout << std::setprecision(70) << machin() << std::endl;
}
```

To compile this source code, use the following command.

`g++ -std=c++20 -I./install/include hello.cpp`

You have to specify C++20 standard. Note that you do not need to link
any library in this example. This program computes PI in octuple
precision and shows it.

```
$ ./a.out
3.141592653589793238462643383279502884197169399375105820974944592307816
```


### Libquadmath emulation

In gcc/g++ on x86_64 architecture, libquadmath provides math functions
for quadruple precision floating point numbers.  However, libquadmath
is not available with clang or Visual Studio.  By using the
libquadmath emulation feature of TLFloat library, it is possible to
use most of the features of libquadmath with clang and Visual Studio.

Below is a simple C source code utilizing this feature.

```
#include <stdio.h>
#include <stdlib.h>

#define TLFLOAT_LIBQUADMATH_EMULATION
#include <tlfloat/tlfloat.h>

int main(int argc, char **argv) {
  if (argc < 3) exit(-1);

  __float128 q1 = strtoflt128(argv[1], NULL);
  __float128 q2 = strtoflt128(argv[2], NULL);

  char str[256];
  quadmath_snprintf(str, sizeof(str), "%.30Qg", powq(q1, q2));
  puts(str);
}
```

To compile this source code, use the following command.

`clang quad.c -I./install/include -L./install/lib -ltlfloat -lm`

Below is an example of executing this program.

```
$ ./a.out 1.234 2.345
1.63732181977903314975233575019
```

In order to use the libquadmath emulation feature, define
TLFLOAT_LIBQUADMATH_EMULATION macro, include tlfloat/tlfloat.h instead
of quadmath.h, and link with -ltlfloat -lm. If you need portability,
replace __float128 with tlfloat_quad.


### C++11 API

Besides the C++20 API, TLFloat provides classes that can be used with
C++11 standard.

Below is a simple C++ source code utilizing this feature.

```
#include <iostream>
#include <tlfloat/tlfloat.h>

tlfloat_octuple AGM(int N) {
  tlfloat_octuple y = tlfloat_sqrto(2) - 1;
  tlfloat_octuple a = y * y * 2;

  for(int k=0;k<N;k++) {
    y = 1.0 - tlfloat_powo(y, 4);
    y = tlfloat_powo(y, 1.0/4);
    y = (1 - y) / (1 + y);
    a *= tlfloat_powo(1 + y, 4);
    a -= tlfloat_ldexpo(((y + 1) * y + 1) * y, 2 * k + 3);
  }

  return 1 / a;
}

int main(int argc, char **argv) {
  std::cout << tlfloat::to_string(AGM(3), 70) << std::endl;
}
```

To compile this source code, use the following command.

`g++ cpp11.cpp -std=c++11 -I./install/include -L./install/lib -ltlfloat`

Below is an example of executing this program.

```
$ ./a.out
3.141592653589793238462643383279502884197169399375105820974944592307818
```

### Benchmark results

This software package includes a benchmark tool. This can be built by
specifying `-DBUILD_BENCH=True` cmake option. Below are some results
of the benchmarks.

CPU: AMD Ryzen 9 7950X (turbo disabled)

Compiler: gcc version 12.3.0 (Ubuntu 12.3.0-1ubuntu1~22.04)

TLFloat Quad

```
Config               : tlfloat quad
Measurement time     : 10 sec
Addition             : 126.276 Mops/second
Multiplication       : 121.558 Mops/second
Division             : 21.3083 Mops/second
Cast to/from double  : 210.811 Mops/second
Compare              : 349.309 Mops/second
FMA                  : 79.3782 Mops/second
Square root          : 35.2694 Mops/second
Rint                 : 416.364 Mops/second
Sin                  : 2.44763 Mops/second
Atan                 : 1.30991 Mops/second
Exp                  : 1.40848 Mops/second
Pow                  : 0.946258 Mops/second
```

libquadmath

```
Config               : Libquadmath
Measurement time     : 10 sec
Addition             : 90.2234 Mops/second
Multiplication       : 74.2182 Mops/second
Division             : 83.3925 Mops/second
Cast to/from double  : 78.1327 Mops/second
Compare              : 272.084 Mops/second
FMA                  : 2.24491 Mops/second
Square root          : 12.1595 Mops/second
Rint                 : 36.0805 Mops/second
Sin                  : 4.17695 Mops/second
Atan                 : 2.43314 Mops/second
Exp                  : 1.97359 Mops/second
Pow                  : 1.38477 Mops/second
```


### Development plan

* The following features will be added in future releases
  * Further documentation
  * More testing
    * Add more testers for I/O functions
  * Interoperability with SLEEF
  * Big endian support
  * Remaining math functions in math.h
  * BF16 support
  * Further optimization


### License

The software is distributed under the Boost Software License, Version 1.0.
See accompanying file LICENSE.txt or copy at :
http://www.boost.org/LICENSE_1_0.txt.

Contributions to this project are accepted under the same license.

The fact that this software is released under an open source license
only means that you can use the current version of the software for
free. If you want this software to be maintained, you need to
financially support the project.

Copyright Naoki Shibata 2024.
