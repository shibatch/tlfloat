if (BUILD_LIBS)
  add_library(tlfloat capi.cpp)
  if ("${LC_CMAKE_BUILD_TYPE}" STREQUAL "release" AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND NOT WIN32)
    target_compile_options(tlfloat PRIVATE "-mllvm" "-inline-threshold=10000")
  endif()
  set(INCLUDEDIR "${PROJECT_SOURCE_DIR}/src/include")
  set(VERFILE "${PROJECT_BINARY_DIR}/include/tlfloat/tlfloatversion.hpp")
  set_target_properties(tlfloat PROPERTIES
    VERSION ${TLFLOAT_VERSION}
    SOVERSION ${TLFLOAT_SOVERSION}
    PUBLIC_HEADER "${INCLUDEDIR}/tlfloat/bigint.hpp;${INCLUDEDIR}/tlfloat/rpitab.hpp;${INCLUDEDIR}/tlfloat/tlfloat.hpp;${INCLUDEDIR}/tlfloat/tlmath.hpp;${INCLUDEDIR}/tlfloat/tlmathcoef.hpp;${INCLUDEDIR}/tlfloat/tlfloat.h;${VERFILE}"
    )
  install(
    TARGETS tlfloat
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_PREFIX}/include/tlfloat"
    COMPONENT tlfloat_Development
    )
else()
  set(INCLUDEDIR "${PROJECT_SOURCE_DIR}/src/include")
  set(VERFILE "${PROJECT_BINARY_DIR}/include/tlfloat/tlfloatversion.hpp")
  install(
    FILES "${INCLUDEDIR}/tlfloat/bigint.hpp" "${INCLUDEDIR}/tlfloat/rpitab.hpp" "${INCLUDEDIR}/tlfloat/tlfloat.hpp" "${INCLUDEDIR}/tlfloat/tlmath.hpp" "${INCLUDEDIR}/tlfloat/tlmathcoef.hpp" "${INCLUDEDIR}/tlfloat/tlfloat.h" "${VERFILE}"
    DESTINATION "${CMAKE_INSTALL_PREFIX}/include/tlfloat"
    )
endif()


