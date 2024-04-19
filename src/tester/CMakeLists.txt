if (COMPILER_SUPPORTS_INT128)
  add_executable(test_bigint3 test_bigint3.cpp)
  target_link_libraries(test_bigint3 tlfloat)
  target_compile_definitions(test_bigint3 PRIVATE TLFLOAT_COMPILER_SUPPORTS_INT128=1)
  add_test(NAME test_bigint3 COMMAND test_bigint3)
endif()

if (NOT WIN32)
   add_executable(test_printf test_printf.cpp)
   target_link_libraries(test_printf tlfloat)
   add_test(NAME test_printf COMMAND test_printf)
endif()

add_executable(test_bigint2 test_bigint2.cpp)
target_link_libraries(test_bigint2 tlfloat)
target_compile_definitions(test_bigint2 PRIVATE TLFLOAT_SUPPRESS_WARNINGS=1)
add_test(NAME test_bigint2 COMMAND test_bigint2)

if(ENABLE_CUDA_TEST)
  add_executable(test_cuda test_cuda.cu)
  target_link_libraries(test_cuda tlfloat)
  target_compile_options(test_cuda PRIVATE "--maxrregcount;128;--expt-relaxed-constexpr;--std;c++20")
  add_test(NAME test_cuda COMMAND test_cuda)
endif()

add_executable(test_arith test_arith.cpp)
target_link_libraries(test_arith tlfloat)
if (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT WIN32)
  target_link_libraries(test_arith quadmath)
endif()
if (COMPILER_SUPPORTS_INT128)
  target_compile_definitions(test_arith PRIVATE TLFLOAT_COMPILER_SUPPORTS_INT128=1)
endif()
target_compile_definitions(test_arith PRIVATE TLFLOAT_SUPPRESS_WARNINGS=1)
add_test(NAME test_arith COMMAND test_arith)

if (INSTALL_CONTINUOUS_TESTERS)
  install(
    TARGETS test_arith
    DESTINATION "${INSTALL_BINDIR}"
    COMPONENT tester
    )
endif()

add_executable(test_hash test_hash.cpp)
target_link_libraries(test_hash tlfloat)
add_test(NAME test_hash COMMAND test_hash 975ad94306bbd3b8ceb6a946403379cff54591b498edb56d5a587980d82a1488)

add_executable(test_printf_hash test_printf_hash.cpp)
target_link_libraries(test_printf_hash tlfloat)
add_test(NAME test_printf_hash COMMAND test_printf_hash fd393c2ce5cf95bebbafc7966b5ab3ff80f56dec1c61125b774f5ad1e6a28452)

if (LIB_MPFR)
  add_executable(test_mpfr test_mpfr.cpp)
  target_link_libraries(test_mpfr tlfloat ${LIB_MPFR})
  if (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT WIN32)
    target_link_libraries(test_mpfr quadmath)
  endif()
  add_test(NAME test_mpfr COMMAND test_mpfr)

  if (INSTALL_CONTINUOUS_TESTERS)
    install(
      TARGETS test_mpfr
      DESTINATION "${INSTALL_BINDIR}"
      COMPONENT tester
      )
  endif()

  add_executable(tester1 tester1.cpp)
  target_link_libraries(tester1 ${LIB_MPFR})
  if (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT WIN32)
    target_link_libraries(tester1 quadmath)
  endif()
  target_compile_definitions(tester1 PRIVATE TLFLOAT_SUPPRESS_WARNINGS=1)
  add_test(NAME tester1 COMMAND tester1)
endif()

#

if (LIB_MPFR AND (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_SYSTEM_PROCESSOR MATCHES "(arm64|aarch64)"))
  add_executable(test_exhaustive32 test_exhaustive32.cpp)
  target_link_libraries(test_exhaustive32 ${LIB_MPFR})
  if (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT WIN32)
    target_link_libraries(test_exhaustive32 quadmath)
  endif()
  if (OpenMP_CXX_FOUND)
    target_compile_options(test_exhaustive32 PRIVATE ${OpenMP_CXX_FLAGS})
    target_link_options(test_exhaustive32 PRIVATE ${OpenMP_CXX_FLAGS})
    target_include_directories(test_exhaustive32 PRIVATE ${OpenMP_CXX_INCLUDE_DIRS})
  endif()

  if (INSTALL_CONTINUOUS_TESTERS)
    install(
      TARGETS test_exhaustive32
      DESTINATION "${INSTALL_BINDIR}"
      COMPONENT tester
      )
  endif()
endif()

#

if (COMPILER_SUPPORTS_FP16 AND LIB_MPFR)
  add_executable(test_exhaustive16 test_exhaustive16.cpp)
  target_link_libraries(test_exhaustive16 tlfloat ${LIB_MPFR})
  target_compile_options(test_exhaustive16 PRIVATE "-march=armv8.2-a+fp16")
  if (OpenMP_CXX_FOUND)
    target_compile_options(test_exhaustive16 PRIVATE ${OpenMP_CXX_FLAGS})
    target_link_options(test_exhaustive16 PRIVATE ${OpenMP_CXX_FLAGS})
    target_include_directories(test_exhaustive16 PRIVATE ${OpenMP_CXX_INCLUDE_DIRS})
  endif()

  if (INSTALL_CONTINUOUS_TESTERS)
    install(
      TARGETS test_exhaustive16
      DESTINATION "${INSTALL_BINDIR}"
      COMPONENT tester
      )
  endif()
endif()

#

add_library(test_octuple_obj OBJECT test_octuple.cpp)
target_compile_definitions(test_octuple_obj PRIVATE TLFLOAT_NO_LIBSTDCXX=1)
set_target_properties(test_octuple_obj PROPERTIES LINKER_LANGUAGE C)

add_executable(test_octuple test_octuple_dummy.c)
target_link_libraries(test_octuple test_octuple_obj tlfloat ${LIBM})
set(CMAKE_CXX_IMPLICIT_LINK_LIBRARIES "")
set_target_properties(test_octuple PROPERTIES LINKER_LANGUAGE C)
add_test(NAME test_octuple COMMAND test_octuple)

add_executable(test_capi test_capi.c)
target_link_libraries(test_capi tlfloat ${LIBM})
if (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT WIN32)
  target_link_libraries(test_capi quadmath)
endif()
set_target_properties(test_capi PROPERTIES LINKER_LANGUAGE C)
add_test(NAME test_capi COMMAND test_capi)
