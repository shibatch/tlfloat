if (BUILD_LIBS)
  add_library(tlfloat_inline OBJECT arith.cpp misc.cpp trig.cpp logexp.cpp invtrig.cpp hyp.cpp erfgamma.cpp)
  if (ENABLE_INLINING AND "${LC_CMAKE_BUILD_TYPE}" STREQUAL "release")
    target_compile_options(tlfloat_inline PRIVATE ${INLINE_CXX_FLAGS})
    target_compile_definitions(tlfloat_inline PRIVATE TLFLOAT_ENABLE_INLINING=1)
  endif()

  add_library(tlfloat printf.cpp)
  target_link_libraries(tlfloat tlfloat_inline)

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

else(BUILD_LIBS)

  set(INCLUDEDIR "${PROJECT_SOURCE_DIR}/src/include")
  set(VERFILE "${PROJECT_BINARY_DIR}/include/tlfloat/tlfloatversion.hpp")
  install(
    FILES "${INCLUDEDIR}/tlfloat/bigint.hpp" "${INCLUDEDIR}/tlfloat/rpitab.hpp" "${INCLUDEDIR}/tlfloat/tlfloat.hpp" "${INCLUDEDIR}/tlfloat/tlmath.hpp" "${INCLUDEDIR}/tlfloat/tlmathcoef.hpp" "${INCLUDEDIR}/tlfloat/tlfloat.h" "${VERFILE}"
    DESTINATION "${CMAKE_INSTALL_PREFIX}/include/tlfloat"
    )

endif(BUILD_LIBS)


