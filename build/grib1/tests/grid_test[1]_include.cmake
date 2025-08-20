if(EXISTS "/home/oster/Mashroom/build/Debug/bin/grid_test")
  if(NOT EXISTS "/home/oster/Mashroom/build/grib1/tests/grid_test[1]_tests.cmake" OR
     NOT "/home/oster/Mashroom/build/grib1/tests/grid_test[1]_tests.cmake" IS_NEWER_THAN "/home/oster/Mashroom/build/Debug/bin/grid_test" OR
     NOT "/home/oster/Mashroom/build/grib1/tests/grid_test[1]_tests.cmake" IS_NEWER_THAN "${CMAKE_CURRENT_LIST_FILE}")
    include("/home/oster/CMake/Modules/GoogleTestAddTests.cmake")
    gtest_discover_tests_impl(
      TEST_EXECUTABLE [==[/home/oster/Mashroom/build/Debug/bin/grid_test]==]
      TEST_EXECUTOR [==[]==]
      TEST_WORKING_DIR [==[/home/oster/Mashroom/build/grib1/tests]==]
      TEST_EXTRA_ARGS [==[]==]
      TEST_PROPERTIES [==[]==]
      TEST_PREFIX [==[]==]
      TEST_SUFFIX [==[]==]
      TEST_FILTER [==[]==]
      NO_PRETTY_TYPES [==[FALSE]==]
      NO_PRETTY_VALUES [==[FALSE]==]
      TEST_LIST [==[grid_test_TESTS]==]
      CTEST_FILE [==[/home/oster/Mashroom/build/grib1/tests/grid_test[1]_tests.cmake]==]
      TEST_DISCOVERY_TIMEOUT [==[5]==]
      TEST_DISCOVERY_EXTRA_ARGS [==[]==]
      TEST_XML_OUTPUT_DIR [==[]==]
    )
  endif()
  include("/home/oster/Mashroom/build/grib1/tests/grid_test[1]_tests.cmake")
else()
  add_test(grid_test_NOT_BUILT grid_test_NOT_BUILT)
endif()
