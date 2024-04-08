add_library(tlfloat capi.cpp)
target_compile_definitions(tlfloat PRIVATE SUPPRESS_WARNINGS=1)
set(INCLUDEDIR "${CMAKE_SOURCE_DIR}/src/include")
set_target_properties(tlfloat PROPERTIES
  VERSION ${TLFLOAT_VERSION}
  SOVERSION ${TLFLOAT_SOVERSION}
  PUBLIC_HEADER "${INCLUDEDIR}/bigint.hpp;${INCLUDEDIR}/rpitab.hpp;${INCLUDEDIR}/suppress.hpp;${INCLUDEDIR}/tlfloat.hpp;${INCLUDEDIR}/tlmath.hpp;${INCLUDEDIR}/tlmathcoef.hpp;${INCLUDEDIR}/tlfloat.h"
  )

install(
    TARGETS tlfloat
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT tlfloat_Development
)
