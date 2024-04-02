if (COMPILER_SUPPORTS_INT128)
  add_executable(test_bigint3 test_bigint3.cpp)
  target_link_libraries(test_bigint3 tlfloat)
  add_test(NAME test_bigint3 COMMAND test_bigint3)
endif()

add_executable(test_bigint2 test_bigint2.cpp)
target_link_libraries(test_bigint2 tlfloat)
target_compile_definitions(test_bigint2 PRIVATE SUPPRESS_WARNINGS=1)
add_test(NAME test_bigint2 COMMAND test_bigint2)

if(ENABLE_CUDA_TESTER)
  add_executable(test_tlfloat_cuda test_tlfloat_cuda.cu)
  target_link_libraries(test_tlfloat_cuda tlfloat)
  target_compile_options(test_tlfloat_cuda PRIVATE "--maxrregcount;128;--expt-relaxed-constexpr;--std;c++20")
  add_test(NAME test_tlfloat_cuda COMMAND test_tlfloat_cuda)
endif()

add_executable(test_tlfloat test_tlfloat.cpp)
target_link_libraries(test_tlfloat tlfloat)
if (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT WIN32)
  target_link_libraries(test_tlfloat quadmath)
endif()
target_compile_definitions(test_tlfloat PRIVATE SUPPRESS_WARNINGS=1)
add_test(NAME test_tlfloat COMMAND test_tlfloat)

if (INSTALL_CONTINUOUS_TESTERS)
  install(
    TARGETS test_tlfloat
    DESTINATION "${INSTALL_BINDIR}"
    COMPONENT tester
    )
endif()

add_executable(test_tlfloat_hash test_tlfloat_hash.cpp)
target_link_libraries(test_tlfloat_hash tlfloat)
add_test(NAME test_tlfloat_hash COMMAND test_tlfloat_hash 336b7600e848965d7ce559743a1b83726f4d7b6e85ed276d41c9150b16c3b7cf)

if (LIB_MPFR)
  add_executable(test_tlfloat_mpfr test_tlfloat_mpfr.cpp)
  target_link_libraries(test_tlfloat_mpfr tlfloat ${LIB_MPFR})
  if (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT WIN32)
    target_link_libraries(test_tlfloat_mpfr quadmath)
  endif()
  add_test(NAME test_tlfloat_mpfr COMMAND test_tlfloat_mpfr)

  if (INSTALL_CONTINUOUS_TESTERS)
    install(
      TARGETS test_tlfloat_mpfr
      DESTINATION "${INSTALL_BINDIR}"
      COMPONENT tester
      )
  endif()
endif()

#

if (LIB_MPFR AND (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_SYSTEM_PROCESSOR MATCHES "(arm64|aarch64)"))
  add_executable(test_tlfloat32 test_tlfloat32.cpp)
  target_link_libraries(test_tlfloat32 ${LIB_MPFR})
  if (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT WIN32)
    target_link_libraries(test_tlfloat32 quadmath)
  endif()
  if (OpenMP_CXX_FOUND)
    target_compile_options(test_tlfloat32 PRIVATE ${OpenMP_CXX_FLAGS})
    target_link_options(test_tlfloat32 PRIVATE ${OpenMP_CXX_FLAGS})
    target_include_directories(test_tlfloat32 PRIVATE ${OpenMP_CXX_INCLUDE_DIRS})
  endif()

  if (INSTALL_CONTINUOUS_TESTERS)
    install(
      TARGETS test_tlfloat32
      DESTINATION "${INSTALL_BINDIR}"
      COMPONENT tester
      )
  endif()
endif()

#

if (COMPILER_SUPPORTS_FP16 AND LIB_MPFR)
  add_executable(test_tlfloat16 test_tlfloat16.cpp)
  target_link_libraries(test_tlfloat16 tlfloat ${LIB_MPFR})
  target_compile_options(test_tlfloat16 PRIVATE "-march=armv8.2-a+fp16")
  if (OpenMP_CXX_FOUND)
    target_compile_options(test_tlfloat16 PRIVATE ${OpenMP_CXX_FLAGS})
    target_link_options(test_tlfloat16 PRIVATE ${OpenMP_CXX_FLAGS})
    target_include_directories(test_tlfloat16 PRIVATE ${OpenMP_CXX_INCLUDE_DIRS})
  endif()

  if (INSTALL_CONTINUOUS_TESTERS)
    install(
      TARGETS test_tlfloat16
      DESTINATION "${INSTALL_BINDIR}"
      COMPONENT tester
      )
  endif()
endif()

#

add_library(test_octuple_obj OBJECT test_octuple.cpp)
target_compile_definitions(test_octuple_obj PRIVATE NO_LIBSTDCXX=1)
set_target_properties(test_octuple_obj PROPERTIES LINKER_LANGUAGE C)

add_executable(test_octuple test_octuple_dummy.c)
target_link_libraries(test_octuple test_octuple_obj ${LIBM})
set(CMAKE_CXX_IMPLICIT_LINK_LIBRARIES "")
set_target_properties(test_octuple PROPERTIES LINKER_LANGUAGE C)
add_test(NAME test_octuple COMMAND test_octuple)
