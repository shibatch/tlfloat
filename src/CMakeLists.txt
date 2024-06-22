add_subdirectory("tlfloat")
add_subdirectory("util")
if (BUILD_TESTS)
  add_subdirectory("tester") # tester has to come at last
endif()
