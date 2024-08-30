if (BUILD_UTILS)
  if (LIB_MPFR)
    add_executable(mkrpitab mkrpitab.cpp)
    target_link_libraries(mkrpitab ${LIB_MPFR})
  endif()

  add_executable(genmathcoef genmathcoef.cpp)
endif()
