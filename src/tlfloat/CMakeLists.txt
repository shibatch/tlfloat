add_library(tlfloat capi.cpp)
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
