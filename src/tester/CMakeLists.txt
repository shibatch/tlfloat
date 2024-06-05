if (TLFLOAT_COMPILER_SUPPORTS_INT128)
  add_executable(test_bigint3 test_bigint3.cpp)
  target_link_libraries(test_bigint3 tlfloat)
  add_test(NAME test_bigint3 COMMAND test_bigint3)
endif()

add_executable(test_bigint4 test_bigint4.cpp)
target_link_libraries(test_bigint4 tlfloat)
add_test(NAME test_bigint4 COMMAND test_bigint4)

if (BUILDING_WITH_GLIBC)
   add_executable(test_printf test_printf.cpp)
   target_link_libraries(test_printf tlfloat)
   add_test(NAME test_printf COMMAND test_printf)
endif()

add_executable(test_bigint2 test_bigint2.cpp)
target_link_libraries(test_bigint2 tlfloat)
add_test(NAME test_bigint2 COMMAND test_bigint2)

if(ENABLE_CUDA_TEST)
  add_executable(test_cuda test_cuda.cu)
  target_link_libraries(test_cuda tlfloat)
  target_compile_options(test_cuda PRIVATE "--maxrregcount;128;--expt-relaxed-constexpr;--std;c++20")
  add_test(NAME test_cuda COMMAND test_cuda)
  set_tests_properties(test_cuda PROPERTIES COST "4")
endif()

add_executable(test_arith test_arith.cpp)
add_executable(test_arith_noopt test_arith.cpp)
target_compile_definitions(test_arith_noopt PRIVATE TLFLOAT_DISABLE_ARCH_OPTIMIZATION=1)
target_link_libraries(test_arith tlfloat)
target_link_libraries(test_arith_noopt tlfloat)
if (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT WIN32)
  target_link_libraries(test_arith quadmath)
  target_link_libraries(test_arith_noopt quadmath)
endif()
add_test(NAME test_arith COMMAND test_arith)
add_test(NAME test_arith_noopt COMMAND test_arith_noopt)

if (INSTALL_CONTINUOUS_TESTERS)
  install(
    TARGETS test_arith test_arith_noopt
    DESTINATION "${INSTALL_BINDIR}"
    COMPONENT tester
    )
endif()

add_executable(test_hash test_hash.cpp)
target_link_libraries(test_hash tlfloat)
add_test(NAME test_hash COMMAND test_hash 214e4df164eae527fe434c92869baf0420ee1573d0e914c43dd615b0cb4cf45c)
set_tests_properties(test_hash PROPERTIES COST "3")

add_executable(test_hash_noopt test_hash.cpp)
target_compile_definitions(test_hash_noopt PRIVATE TLFLOAT_DISABLE_ARCH_OPTIMIZATION=1)
target_link_libraries(test_hash_noopt tlfloat)
add_test(NAME test_hash_noopt COMMAND test_hash_noopt 214e4df164eae527fe434c92869baf0420ee1573d0e914c43dd615b0cb4cf45c)
set_tests_properties(test_hash_noopt PROPERTIES COST "3")

add_executable(test_printf_hash test_printf_hash.cpp)
target_link_libraries(test_printf_hash tlfloat)
add_test(NAME test_printf_hash COMMAND test_printf_hash 19afa79b7328d55c90a0f2a885d1b5cb304a094c312551815fa43ff94488e277)
set_tests_properties(test_printf_hash PROPERTIES COST "2")

if (LIB_MPFR)
  add_executable(test_mpfr test_mpfr.cpp)
  target_link_libraries(test_mpfr tlfloat ${LIB_MPFR})
  if (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT WIN32)
    target_link_libraries(test_mpfr quadmath)
  endif()
  add_test(NAME test_mpfr COMMAND test_mpfr)
  set_tests_properties(test_mpfr PROPERTIES COST "3")

  add_executable(test_mpfr_noopt test_mpfr.cpp)
  target_compile_definitions(test_mpfr_noopt PRIVATE TLFLOAT_DISABLE_ARCH_OPTIMIZATION=1)
  target_link_libraries(test_mpfr_noopt tlfloat ${LIB_MPFR})
  if (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT WIN32)
    target_link_libraries(test_mpfr_noopt quadmath)
  endif()
  add_test(NAME test_mpfr_noopt COMMAND test_mpfr_noopt)
  set_tests_properties(test_mpfr_noopt PROPERTIES COST "3")

  if (INSTALL_CONTINUOUS_TESTERS)
    install(
      TARGETS test_mpfr test_mpfr_noopt
      DESTINATION "${INSTALL_BINDIR}"
      COMPONENT tester
      )
  endif()

  add_executable(tester1 tester1.cpp)
  target_link_libraries(tester1 ${LIB_MPFR})
  if (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT WIN32)
    target_link_libraries(tester1 quadmath)
  endif()
  add_test(NAME tester1 COMMAND tester1)
  set_tests_properties(tester1 PROPERTIES COST "1")

  add_executable(tester1_noopt tester1.cpp)
  target_compile_definitions(tester1_noopt PRIVATE TLFLOAT_DISABLE_ARCH_OPTIMIZATION=1)
  target_link_libraries(tester1_noopt ${LIB_MPFR})
  if (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT WIN32)
    target_link_libraries(tester1_noopt quadmath)
  endif()
  add_test(NAME tester1_noopt COMMAND tester1_noopt)
  set_tests_properties(tester1_noopt PROPERTIES COST "1")
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

  if (ENABLE_EXHAUSTIVE_TESTING)
    add_test(NAME test_exhaustive32 COMMAND test_exhaustive32)
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

if (TLFLOAT_COMPILER_SUPPORTS_FP16 AND LIB_MPFR)
  add_executable(test_exhaustive16 test_exhaustive16.cpp)
  target_link_libraries(test_exhaustive16 tlfloat ${LIB_MPFR})
  target_compile_options(test_exhaustive16 PRIVATE "-march=armv8.2-a+fp16")
  if (OpenMP_CXX_FOUND)
    target_compile_options(test_exhaustive16 PRIVATE ${OpenMP_CXX_FLAGS})
    target_link_options(test_exhaustive16 PRIVATE ${OpenMP_CXX_FLAGS})
    target_include_directories(test_exhaustive16 PRIVATE ${OpenMP_CXX_INCLUDE_DIRS})
  endif()
  if (ENABLE_EXHAUSTIVE_TESTING)
    add_test(NAME test_exhaustive16 COMMAND test_exhaustive16)
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

add_executable(test_cpp11api test_cpp11api.cpp)
target_link_libraries(test_cpp11api tlfloat)
set_target_properties(test_cpp11api PROPERTIES CXX_STANDARD 11 CXX_EXTENSIONS off)
add_test(NAME test_cpp11api COMMAND test_cpp11api)

add_executable(test_cpp11api20 test_cpp11api.cpp)
target_link_libraries(test_cpp11api20 tlfloat)
add_test(NAME test_cpp11api20 COMMAND test_cpp11api20)
