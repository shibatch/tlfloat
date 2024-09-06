cmake_minimum_required(VERSION 3.16)

option(BUILD_LIBS "Build and install the library" ON)
option(BUILD_TESTS "Build tests" ON)
option(BUILD_UTILS "Build utilities" ON)
option(BUILD_BENCH "Build benchmarking tools" OFF)
option(ENABLE_INLINING "Enable extensive function inlining" OFF)
option(ENABLE_ASAN "Enable address sanitizer" OFF)
option(ENABLE_CUDA_TEST "Build CUDA tester" OFF)
option(INSTALL_CONTINUOUS_TESTERS "Install continuous testers" OFF)
option(ENABLE_DOXYGEN "Enable generating documents with Doxygen" OFF)
option(ENABLE_COVERAGE "Enable generating coverage data" OFF)
option(ENABLE_ARCH_OPTIMIZATION "Enable architecture-specific optimization upon building library" ON)
option(ENABLE_EXHAUSTIVE_TESTING "Enable exhaustive testing for 16-bit and 32-bit arguments" OFF)
set(SET_COMPILER_SUPPORTS_INT128 "auto" CACHE STRING "Set whether the compiler supports __int128 type")
set(SET_COMPILER_SUPPORTS_FLOAT128 "auto" CACHE STRING "Set whether the compiler supports __float128 type")
set(SET_LONGDOUBLE_IS_FLOAT128 "auto" CACHE STRING "Set whether long double is IEEE binary128")

#

set(TLFLOAT_VERSION_MAJOR 1)
set(TLFLOAT_VERSION_MINOR 6)
set(TLFLOAT_VERSION_PATCHLEVEL 1)
set(TLFLOAT_VERSION ${TLFLOAT_VERSION_MAJOR}.${TLFLOAT_VERSION_MINOR}.${TLFLOAT_VERSION_PATCHLEVEL})
set(TLFLOAT_SOVERSION ${TLFLOAT_VERSION_MAJOR})

message(STATUS "Configuring TLFloat ${TLFLOAT_VERSION_MAJOR}.${TLFLOAT_VERSION_MINOR}.${TLFLOAT_VERSION_PATCHLEVEL}")

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

set(INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/")
set(INSTALL_BINDIR "${INSTALL_PREFIX}bin")

# Detect compiler capabilities

include(CheckCXXSourceCompiles)

if (SET_COMPILER_SUPPORTS_INT128 STREQUAL "auto")
  CHECK_CXX_SOURCE_COMPILES("
int main(int argc, char **argv) {
  __int128_t i = argc;
  __uint128_t u = argc;
  return (int)(i % 10) + (int)(u % 10);
}" TLFLOAT_COMPILER_SUPPORTS_INT128)
else()
  if (SET_COMPILER_SUPPORTS_INT128)
    set(TLFLOAT_COMPILER_SUPPORTS_INT128 ON)
  else()
    set(TLFLOAT_COMPILER_SUPPORTS_INT128 OFF)
  endif()
  message(STATUS "Manually setting TLFLOAT_COMPILER_SUPPORTS_INT128 : " ${TLFLOAT_COMPILER_SUPPORTS_INT128})
endif()

if (SET_COMPILER_SUPPORTS_FLOAT128 STREQUAL "auto")
  CHECK_CXX_SOURCE_COMPILES("
int main(int argc, char **argv) {
  __float128 i = argc;
  return (int)i;
}" TLFLOAT_COMPILER_SUPPORTS_FLOAT128)
else()
  if (SET_COMPILER_SUPPORTS_FLOAT128)
    set(TLFLOAT_COMPILER_SUPPORTS_FLOAT128 ON)
  else()
    set(TLFLOAT_COMPILER_SUPPORTS_FLOAT128 OFF)
  endif()
  message(STATUS "Manually setting TLFLOAT_COMPILER_SUPPORTS_FLOAT128 : " ${TLFLOAT_COMPILER_SUPPORTS_FLOAT128})
endif()

if (SET_LONGDOUBLE_IS_FLOAT128 STREQUAL "auto")
CHECK_CXX_SOURCE_COMPILES("
int main() {
  static_assert((1 + (long double)0x1p-64) - 1 != 0.0, \"long double is not float128\");
}" TLFLOAT_LONGDOUBLE_IS_FLOAT128)
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
}" TLFLOAT_COMPILER_SUPPORTS_FP16)
  set(CMAKE_REQUIRED_FLAGS)
endif()

CHECK_CXX_SOURCE_COMPILES("
#include <cstdio>
#include <features.h>
#ifndef __GLIBC__
#error GLIBC macro not defined
#endif
int main(int argc, char **argv) {}" BUILDING_WITH_GLIBC)

CHECK_CXX_SOURCE_COMPILES("
#define MPFR_WANT_FLOAT128
#include <mpfr.h>
int main() {}" TLFLOAT_ENABLE_MPFR_WANT_FLOAT128)

set(CMAKE_REQUIRED_LIBRARIES quadmath)
CHECK_CXX_SOURCE_COMPILES("
#define MPFR_WANT_FLOAT128
#include <cstdio>
#include <quadmath.h>
int main(int argc, char **argv) {
__float128 f = atof(argv[1]);
printf(\"%g\", (double)sinq(f));
}" TLFLOAT_ENABLE_LIBQUADMATH)
set(CMAKE_REQUIRED_LIBRARIES)

# Setting up memory-leak checks

include(CTest)
set(MEMORYCHECK_COMMAND_OPTIONS "--leak-check=full --error-exitcode=1")

if (ENABLE_VALGRIND AND NOT CMAKE_CROSSCOMPILING)
  find_program(VALGRIND "valgrind")
  message(STATUS "VALGRIND : " ${VALGRIND})
endif()

# Compiler options

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fcompare-debug-second -fconstexpr-ops-limit=1000000000 --param=max-vartrack-size=1000000000")
  set(BENCH_CXX_FLAGS "-funroll-loops;--param=max-unroll-times=1000000;--param=max-variable-expansions-in-unroller=65536;--inline-limit=1000000")
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
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fconstexpr-steps=1000000000")
  set(BENCH_CXX_FLAGS "-mllvm;-inline-threshold=100000")
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
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /clang:-fconstexpr-steps=1000000000")
  set(BENCH_CXX_FLAGS "/clang:-mllvm;/clang:-inline-threshold=100000")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /constexpr:steps1000000000")
  set(BENCH_CXX_FLAGS "")
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

configure_file("${PROJECT_SOURCE_DIR}/src/include/tlfloat/tlfloatversion.hpp.in"
  "${PROJECT_BINARY_DIR}/include/tlfloat/tlfloatversion.hpp" @ONLY)

configure_file("${PROJECT_SOURCE_DIR}/src/include/tlfloat/detect.h.in"
  "${PROJECT_BINARY_DIR}/include/tlfloat/detect.h" @ONLY)

#

add_subdirectory("src")

# Show status

if(NOT "${CMAKE_BUILD_TYPE}" STREQUAL "")
  message(STATUS "CMAKE_BUILD_TYPE : " ${CMAKE_BUILD_TYPE})
endif()
