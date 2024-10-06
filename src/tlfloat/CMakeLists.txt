if (BUILD_LIBS)
  add_library(tlfloat_inline OBJECT arith.cpp misc.cpp trig.cpp logexp.cpp invtrig.cpp hyp.cpp erfgamma.cpp)
  target_compile_options(tlfloat_inline PRIVATE ${NOEXCEPT_CXX_FLAGS})
  if (ENABLE_INLINING AND "${LC_CMAKE_BUILD_TYPE}" STREQUAL "release")
    target_compile_options(tlfloat_inline PRIVATE ${INLINE_CXX_FLAGS})
    target_compile_definitions(tlfloat_inline PRIVATE TLFLOAT_ENABLE_INLINING=1)
  endif()

  add_library(tlfloat printf.cpp)
  target_compile_options(tlfloat PRIVATE ${NOEXCEPT_CXX_FLAGS})
  target_link_libraries(tlfloat PRIVATE tlfloat_inline)

  set(INCLUDEDIR "${PROJECT_SOURCE_DIR}/src/include")
  set(VERFILE "${PROJECT_BINARY_DIR}/include/tlfloat/tlfloatconfig.hpp")
  set_target_properties(tlfloat PROPERTIES
    VERSION ${TLFLOAT_VERSION}
    SOVERSION ${TLFLOAT_SOVERSION}
    PUBLIC_HEADER "${INCLUDEDIR}/tlfloat/bigint.hpp;${INCLUDEDIR}/tlfloat/rpitab.hpp;${INCLUDEDIR}/tlfloat/tlfloat.hpp;${INCLUDEDIR}/tlfloat/tlmath.hpp;${INCLUDEDIR}/tlfloat/tlmathcoef.hpp;${INCLUDEDIR}/tlfloat/tlfloat.h;${VERFILE}"
    )
  install(
    TARGETS tlfloat tlfloat_inline
    EXPORT tlfloatTargets
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/tlfloat"
    COMPONENT tlfloat_Development
    )

else(BUILD_LIBS)

  set(INCLUDEDIR "${PROJECT_SOURCE_DIR}/src/include")
  set(VERFILE "${PROJECT_BINARY_DIR}/include/tlfloat/tlfloatconfig.hpp")
  install(
    FILES "${INCLUDEDIR}/tlfloat/bigint.hpp" "${INCLUDEDIR}/tlfloat/rpitab.hpp" "${INCLUDEDIR}/tlfloat/tlfloat.hpp" "${INCLUDEDIR}/tlfloat/tlmath.hpp" "${INCLUDEDIR}/tlfloat/tlmathcoef.hpp" "${INCLUDEDIR}/tlfloat/tlfloat.h" "${VERFILE}"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/tlfloat"
    )

endif(BUILD_LIBS)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/tlfloat.pc.in" "${CMAKE_CURRENT_BINARY_DIR}/tlfloat.pc" @ONLY)
install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/tlfloat.pc"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig"
    COMPONENT tlfloat_Development
)

if (FALSE)
  # Currently disabled because I still don't fully understand how this works

  configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/tlfloatConfig.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/tlfloatConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/tlfloat"
    )

  install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/tlfloatConfig.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/tlfloat"
    COMPONENT tlfloat_Development
    )
endif()
