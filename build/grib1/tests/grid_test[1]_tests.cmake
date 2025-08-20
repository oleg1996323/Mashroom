add_test([=[Serialization.Albers]=]  /home/oster/Mashroom/build/Debug/bin/grid_test [==[--gtest_filter=Serialization.Albers]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[Serialization.Albers]=]  PROPERTIES DEF_SOURCE_LINE /home/oster/Mashroom/grib1/tests/grid_test.cpp:4 WORKING_DIRECTORY /home/oster/Mashroom/build/grib1/tests SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set(  grid_test_TESTS Serialization.Albers)
