cmake_minimum_required(VERSION 3.16)

option(BUILD_LIBS "Build and install the library" ON)
option(BUILD_TESTS "Build tests" ON)
option(BUILD_UTILS "Build utilities" ON)
option(BUILD_BENCH "Build benchmarking tools" OFF)
option(ENABLE_INTSQRT "Enable fully integer sqrt" OFF)
option(ENABLE_INLINING "Enable extensive function inlining" OFF)
option(ENABLE_LTO "Enable LTO" OFF)
option(ENABLE_ASAN "Enable address sanitizer" OFF)
option(ENABLE_CUDA_TEST "Build CUDA tester" OFF)
option(INSTALL_CONTINUOUS_TESTERS "Install continuous testers" OFF)
option(ENABLE_DOXYGEN "Enable generating documents with Doxygen" OFF)
option(ENABLE_COVERAGE "Enable generating coverage data" OFF)
option(ENABLE_ARCH_OPTIMIZATION "Enable architecture-specific optimization upon building library" ON)
option(BUILD_EXHAUSTIVE_TESTING "Build exhaustive testing for 16-bit and 32-bit arguments" OFF)
option(ENABLE_EXHAUSTIVE_TESTING "Enable exhaustive testing for 16-bit and 32-bit arguments" OFF)
set(SET_COMPILER_SUPPORTS_INT128 "auto" CACHE STRING "Set whether the compiler supports __int128 type")
set(SET_COMPILER_SUPPORTS_FLOAT128 "auto" CACHE STRING "Set whether the compiler supports __float128 type")
set(SET_LONGDOUBLE_IS_FLOAT128 "auto" CACHE STRING "Set whether long double is IEEE binary128")

#

set(TLFLOAT_VERSION_MAJOR 1)
set(TLFLOAT_VERSION_MINOR 11)
set(TLFLOAT_VERSION_PATCH 2)
set(TLFLOAT_VERSION ${TLFLOAT_VERSION_MAJOR}.${TLFLOAT_VERSION_MINOR}.${TLFLOAT_VERSION_PATCH})
set(TLFLOAT_SOVERSION ${TLFLOAT_VERSION_MAJOR})

message(STATUS "Configuring TLFloat ${TLFLOAT_VERSION_MAJOR}.${TLFLOAT_VERSION_MINOR}.${TLFLOAT_VERSION_PATCH}")

project(tlfloat)

#

enable_language(CXX)
set(CMAKE_CXX_STANDARD 20)

if(ENABLE_CUDA_TEST)
  enable_language(CUDA)
  set(CMAKE_CUDA_ARCHITECTURES 50)
endif()

# CMake configuration

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "In-source build not supported")
endif()

set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")

include_directories("${PROJECT_SOURCE_DIR}/src/include")
include_directories("${PROJECT_BINARY_DIR}/include")

set(TMPDIR "${CMAKE_BINARY_DIR}/tmp")
file(MAKE_DIRECTORY "${TMPDIR}")

if (NOT EXISTS ${CMAKE_BINARY_DIR}/CMakeCache.txt)
  if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
  endif()
endif()

include(GNUInstallDirs)

# Include packages

if (NOT CMAKE_CROSSCOMPILING)
  find_program(COMMAND_LLD lld)
  message(STATUS "COMMAND_LLD : " ${COMMAND_LLD})

  find_program(COMMAND_GOLD gold)
  message(STATUS "COMMAND_GOLD : " ${COMMAND_GOLD})
endif()

find_library(LIB_MPFR mpfr)
message(STATUS "LIB_MPFR : " ${LIB_MPFR})
if (LIB_MPFR)
  find_path(MPFR_INCLUDE_DIR mpfr.h)
  message(STATUS "MPFR_INCLUDE_DIR : " ${MPFR_INCLUDE_DIR})
  if (MPFR_INCLUDE_DIR)
    include_directories("${MPFR_INCLUDE_DIR}")
  endif()
endif()

find_library(LIBM m)
if (NOT LIBM)
  set(LIBM "")
endif()

find_package(OpenMP)

# Detect compiler capabilities

include(CheckCXXSourceCompiles)

if (SET_COMPILER_SUPPORTS_INT128 STREQUAL "auto")
  if (CMAKE_CXX_COMPILER_TARGET)
    set(CMAKE_REQUIRED_FLAGS "--target=${CMAKE_CXX_COMPILER_TARGET}")
  endif()
  CHECK_CXX_SOURCE_COMPILES("
int main(int argc, char **argv) {
  __int128_t i = argc;
  __uint128_t u = argc;
  return (int)(i % 10) + (int)(u % 10);
}" TLFLOAT_COMPILER_SUPPORTS_INT128)
  set(CMAKE_REQUIRED_FLAGS)
else()
  if (SET_COMPILER_SUPPORTS_INT128)
    set(TLFLOAT_COMPILER_SUPPORTS_INT128 ON)
  else()
    set(TLFLOAT_COMPILER_SUPPORTS_INT128 OFF)
  endif()
  message(STATUS "Manually setting TLFLOAT_COMPILER_SUPPORTS_INT128 : " ${TLFLOAT_COMPILER_SUPPORTS_INT128})
endif()

if (SET_COMPILER_SUPPORTS_FLOAT128 STREQUAL "auto")
  if (CMAKE_CXX_COMPILER_TARGET)
    set(CMAKE_REQUIRED_FLAGS "--target=${CMAKE_CXX_COMPILER_TARGET}")
  endif()
  CHECK_CXX_SOURCE_COMPILES("
#include <bit>
struct s { long long x, y; };
int main(void) {
  constexpr s a = std::bit_cast<s>(__float128(0.1234)*__float128(56.789));
  static_assert((a.x ^ a.y) == 0xc7d695c93a4e2b71LL);
}
" TLFLOAT_COMPILER_SUPPORTS_FLOAT128)
  set(CMAKE_REQUIRED_FLAGS)
else()
  if (SET_COMPILER_SUPPORTS_FLOAT128)
    set(TLFLOAT_COMPILER_SUPPORTS_FLOAT128 ON)
  else()
    set(TLFLOAT_COMPILER_SUPPORTS_FLOAT128 OFF)
  endif()
  message(STATUS "Manually setting TLFLOAT_COMPILER_SUPPORTS_FLOAT128 : " ${TLFLOAT_COMPILER_SUPPORTS_FLOAT128})
endif()

if (SET_LONGDOUBLE_IS_FLOAT128 STREQUAL "auto")
  if (CMAKE_CXX_COMPILER_TARGET)
    set(CMAKE_REQUIRED_FLAGS "--target=${CMAKE_CXX_COMPILER_TARGET}")
  endif()
  CHECK_CXX_SOURCE_COMPILES("
#include <bit>
struct s { long long x, y; };
int main(void) {
  constexpr s a = std::bit_cast<s>((long double)0.1234*(long double)56.789);
  static_assert((a.x ^ a.y) == 0xc7d695c93a4e2b71LL);
}
" TLFLOAT_LONGDOUBLE_IS_FLOAT128)
  set(CMAKE_REQUIRED_FLAGS)
else()
  if (SET_LONGDOUBLE_IS_FLOAT128)
    set(TLFLOAT_LONGDOUBLE_IS_FLOAT128 ON)
  else()
    set(TLFLOAT_LONGDOUBLE_IS_FLOAT128 OFF)
  endif()
  message(STATUS "Manually setting TLFLOAT_LONGDOUBLE_IS_FLOAT128 : " ${TLFLOAT_LONGDOUBLE_IS_FLOAT128})
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(CMAKE_REQUIRED_FLAGS "-march=armv8.2-a+fp16")
  CHECK_CXX_SOURCE_COMPILES("
#include <cstdlib>
#include <arm_fp16.h>
int main(int argc, char **argv) {
  __fp16 hu = vaddh_f16(__fp16(atof(argv[1])), __fp16(0.2));
  bool b = vceqh_f16(__fp16(atof(argv[1])), __fp16(0.2));
  return (int)hu + (int)b;
}" TLFLOAT_COMPILER_SUPPORTS_ARM_FP16)
  set(CMAKE_REQUIRED_FLAGS)
endif()

if (CMAKE_CXX_COMPILER_TARGET)
  set(CMAKE_REQUIRED_FLAGS "--target=${CMAKE_CXX_COMPILER_TARGET}")
endif()
CHECK_CXX_SOURCE_COMPILES("
#include <cstdlib>
#include <cstdint>
#include <bit>
int main(int argc, char **argv) {
__fp16 a = (__fp16)(float)atof(argv[1]);
__uint16_t u = std::bit_cast<__uint16_t>(a);
a = std::bit_cast<__fp16>(u);
return (float)a; }
" TLFLOAT_COMPILER_SUPPORTS_FP16)

CHECK_CXX_SOURCE_COMPILES("
#include <cstdlib>
#include <cstdint>
#include <bit>
int main(int argc, char **argv) {
__bf16 a = (__bf16)(float)atof(argv[1]);
__uint16_t u = std::bit_cast<__uint16_t>(a);
a = std::bit_cast<__bf16>(u);
return (float)a; }
" TLFLOAT_COMPILER_SUPPORTS_BF16)

CHECK_CXX_SOURCE_COMPILES("
#include <cstdio>
#include <features.h>
#ifndef __GLIBC__
#error GLIBC macro not defined
#endif
int main(int argc, char **argv) {}" BUILDING_WITH_GLIBC)
set(CMAKE_REQUIRED_FLAGS)

if (LIB_MPFR)
  if (CMAKE_CXX_COMPILER_TARGET)
    set(CMAKE_REQUIRED_FLAGS "--target=${CMAKE_CXX_COMPILER_TARGET}")
  endif()
  set(CMAKE_REQUIRED_LIBRARIES ${LIB_MPFR})
  CHECK_CXX_SOURCE_COMPILES("
#define MPFR_WANT_FLOAT128
#include <mpfr.h>
int main() {}" TLFLOAT_ENABLE_MPFR_WANT_FLOAT128)

  CHECK_CXX_SOURCE_COMPILES("
#include <mpfr.h>
int main() {
mpfr_t mx;
mpfr_sinpi(mx, mx, GMP_RNDN);
}"
    TLFLOAT_ENABLE_MPFR_SINPI)
  set(CMAKE_REQUIRED_LIBRARIES)
  set(CMAKE_REQUIRED_FLAGS)
endif()

if (TLFLOAT_COMPILER_SUPPORTS_FLOAT128)
  if (CMAKE_CXX_COMPILER_TARGET)
    set(CMAKE_REQUIRED_FLAGS "--target=${CMAKE_CXX_COMPILER_TARGET}")
  endif()
  set(CMAKE_REQUIRED_LIBRARIES quadmath)
  CHECK_CXX_SOURCE_COMPILES("
#include <cstdio>
#include <quadmath.h>
int main(int argc, char **argv) {
__float128 f = atof(argv[1]);
printf(\"%g\", (double)sinq(f));
}" TLFLOAT_ENABLE_LIBQUADMATH)
  set(CMAKE_REQUIRED_LIBRARIES)
  set(CMAKE_REQUIRED_FLAGS)
endif()

# Setting up memory-leak checks

include(CTest)
set(MEMORYCHECK_COMMAND_OPTIONS "--leak-check=full --error-exitcode=1")

if (ENABLE_VALGRIND AND NOT CMAKE_CROSSCOMPILING)
  find_program(VALGRIND "valgrind")
  message(STATUS "VALGRIND : " ${VALGRIND})
endif()

# Compiler options

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fcompare-debug-second -fconstexpr-ops-limit=1000000000 -fno-math-errno")
  set(INLINE_CXX_FLAGS "--inline-limit=1000000")
  set(NOEXCEPT_CXX_FLAGS "-fno-exceptions;-fno-rtti")
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "i[3-6]86")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpmath=sse -msse2")
  endif()
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -Og")
  if (ENABLE_ASAN)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "(arm64|aarch64)")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=hwaddress -static-libasan -fno-exceptions")
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=hwaddress -static-libasan")
    else()
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -static-libasan -fno-exceptions")
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -static-libasan")
    endif()
  endif()
  if (COMMAND_GOLD)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=gold")
  endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND NOT WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fconstexpr-steps=1000000000 -fno-math-errno")
  set(INLINE_CXX_FLAGS "-mllvm;-inline-threshold=100000")
  set(NOEXCEPT_CXX_FLAGS "-fno-exceptions;-fno-rtti")
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "i[3-6]86")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpmath=sse -msse2")
  endif()
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -Og")
  if (ENABLE_ASAN)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "(arm64|aarch64)")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=hwaddress")
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=hwaddress")
    else()
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
    endif()
  endif()
  if (COMMAND_LLD)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=lld")
  elseif(COMMAND_GOLD)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=gold")
  endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND WIN32)
  get_filename_component(COMPILER_BASENAME "${CMAKE_CXX_COMPILER}" NAME_WE)
  string(TOLOWER "${COMPILER_BASENAME}" LC_COMPILER_BASENAME)

  if ("${LC_COMPILER_BASENAME}" STREQUAL "clang-cl")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /clang:-fconstexpr-steps=1000000000")
    set(INLINE_CXX_FLAGS "/clang:-mllvm;/clang:-inline-threshold=100000")
    set(NOEXCEPT_CXX_FLAGS "")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fconstexpr-steps=1000000000 -fno-math-errno")
    set(INLINE_CXX_FLAGS "-mllvm;-inline-threshold=100000")
    set(NOEXCEPT_CXX_FLAGS "-fno-exceptions;-fno-rtti")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -Og")
  endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /constexpr:steps1000000000")
  set(INLINE_CXX_FLAGS "")
  set(NOEXCEPT_CXX_FLAGS "")
endif()

string(TOLOWER "${CMAKE_BUILD_TYPE}" LC_CMAKE_BUILD_TYPE)

if (WIN32)
  add_compile_definitions(_CRT_SECURE_NO_WARNINGS=1 _CRT_NONSTDC_NO_DEPRECATE=1)
endif()

if (NOT "${LC_CMAKE_BUILD_TYPE}" STREQUAL "release")
  add_compile_definitions(DEBUG=1)
endif()

if (NOT ENABLE_ARCH_OPTIMIZATION)
  add_definitions(-DTLFLOAT_DISABLE_ARCH_OPTIMIZATION)
endif()

if (ENABLE_LTO)
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

# Setting up for coverage report

if (ENABLE_COVERAGE)
  if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND NOT WIN32)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-instr-generate -fcoverage-mapping")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-instr-generate -fcoverage-mapping")
  endif()
endif()

# Setting up Doxygen

if (ENABLE_DOXYGEN AND NOT WIN32)
  find_package(Doxygen)

  if (DOXYGEN_FOUND)
    configure_file("${PROJECT_SOURCE_DIR}/doxygen.conf.in" "${CMAKE_BINARY_DIR}/doxygen.conf" @ONLY)

    add_custom_target(doc_doxygen ALL
      COMMAND "${DOXYGEN_EXECUTABLE}" "${CMAKE_BINARY_DIR}/doxygen.conf"
      WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
      VERBATIM
      )

    install(
      DIRECTORY "${CMAKE_BINARY_DIR}/doc_doxygen/"
      DESTINATION "${INSTALL_PREFIX}doc_doxygen"
      MESSAGE_NEVER
      COMPONENT tlfloat_Documentation
      )
  endif()
endif()

#

if (ENABLE_INTSQRT)
  set(TLFLOAT_ENABLE_INTSQRT True)
endif()

configure_file("${PROJECT_SOURCE_DIR}/src/include/tlfloat/tlfloatconfig.hpp.in"
  "${PROJECT_BINARY_DIR}/include/tlfloat/tlfloatconfig.hpp" @ONLY)

configure_file("${PROJECT_SOURCE_DIR}/src/include/detect.h.in"
  "${PROJECT_BINARY_DIR}/include/detect.h" @ONLY)

# Install CMake package config

if (FALSE)
  # Currently disabled because I still don't fully understand how this works

  include(CMakePackageConfigHelpers)

  write_basic_package_version_file(
    tlfloatConfigVersion.cmake
    VERSION "${TLFLOAT_VERSION}"
    COMPATIBILITY SameMajorVersion
    )

  install(
    EXPORT tlfloatTargets
    FILE tlfloatConfigVersion.cmake
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/tlfloat"
    COMPONENT tlfloat_Development
    )

  install(
    EXPORT tlfloatTargets
    NAMESPACE tlfloat::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/tlfloat"
    COMPONENT tlfloat_Development
    )
endif()

#

add_subdirectory("src")

# Show status

if(NOT "${CMAKE_BUILD_TYPE}" STREQUAL "")
  message(STATUS "CMAKE_BUILD_TYPE : " ${CMAKE_BUILD_TYPE})
endif()
