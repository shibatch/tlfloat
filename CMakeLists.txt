cmake_minimum_required(VERSION 3.16)

option(ENABLE_VALGRIND "Enable testing with valgrind" ON)
option(ENABLE_ASAN "Enable address sanitizer" OFF)
option(ENABLE_CUDA_TESTER "Build CUDA tester" OFF)
option(INSTALL_CONTINUOUS_TESTERS "Install continuous testers" OFF)

#

project(tlfloat)

set(TLFLOAT_VERSION_MAJOR 0)
set(TLFLOAT_VERSION_MINOR 0)
set(TLFLOAT_VERSION_PATCHLEVEL 0)
set(TLFLOAT_VERSION ${TLFLOAT_VERSION_MAJOR}.${TLFLOAT_VERSION_MINOR}.${TLFLOAT_VERSION_PATCHLEVEL})
set(TLFLOAT_SOVERSION ${TLFLOAT_VERSION_MAJOR})

#

enable_language(CXX)
set(CMAKE_CXX_STANDARD 20)

if(ENABLE_CUDA_TESTER)
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

set(TMPDIR "${CMAKE_BINARY_DIR}/tmp")
file(MAKE_DIRECTORY "${TMPDIR}")

if (NOT EXISTS ${CMAKE_BINARY_DIR}/CMakeCache.txt)
  if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "" FORCE)
  endif()
endif()

# Include packages

find_program(COMMAND_LLD lld)
message(STATUS "COMMAND_LLD : " ${COMMAND_LLD})

find_program(COMMAND_GOLD gold)
message(STATUS "COMMAND_GOLD : " ${COMMAND_GOLD})

find_library(LIB_MPFR mpfr)
message(STATUS "LIB_MPFR : " ${LIB_MPFR})

find_library(LIBM m)
if (NOT LIBM)
  set(LIBM "")
endif()

find_package(OpenMP)

set(INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/")
set(INSTALL_BINDIR "${INSTALL_PREFIX}bin")

# Detect compiler capabilities

include(CheckCXXSourceCompiles)

CHECK_CXX_SOURCE_COMPILES("
int main(int argc, char **argv) {
  __int128_t i = argc;
  __uint128_t u = argc;
  return (int)(i % 10) + (int)(u % 10);
}" COMPILER_SUPPORTS_INT128)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(CMAKE_REQUIRED_FLAGS "-march=armv8.2-a+fp16")
  CHECK_CXX_SOURCE_COMPILES("
#include <cstdlib>
#include <arm_fp16.h>
int main(int argc, char **argv) {
  __fp16 hu = vaddh_f16(__fp16(atof(argv[1])), __fp16(0.2));
  return (int)hu;
}" COMPILER_SUPPORTS_FP16)
endif()

# Reset used flags
set(CMAKE_REQUIRED_FLAGS)

# Setting up memory-leak checks

include(CTest)
set(MEMORYCHECK_COMMAND_OPTIONS "--leak-check=full --error-exitcode=1")

if (ENABLE_VALGRIND)
  find_program(VALGRIND "valgrind")
  message(STATUS "VALGRIND : " ${VALGRIND})
endif()

# Compiler options

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fconstexpr-ops-limit=1000000000 --param=max-vartrack-size=1000000000")
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
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xclang -fconstexpr-steps -Xclang 1000000000")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /constexpr:steps1000000000")
endif()

string(TOLOWER "${CMAKE_BUILD_TYPE}" LC_CMAKE_BUILD_TYPE)

if (WIN32)
  add_compile_definitions(_CRT_SECURE_NO_WARNINGS=1 _CRT_NONSTDC_NO_DEPRECATE=1)
endif()

if (NOT "${LC_CMAKE_BUILD_TYPE}" STREQUAL "release")
  add_compile_definitions(DEBUG=1)
endif()

#

add_subdirectory("src")

# Show status

if(NOT "${CMAKE_BUILD_TYPE}" STREQUAL "")
  message(STATUS "CMAKE_BUILD_TYPE : " ${CMAKE_BUILD_TYPE})
endif()
