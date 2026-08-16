
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(GTest REQUIRED COMPONENTS gmock)

add_executable(test_solver test_solver.cpp)

target_include_directories(test_solver PRIVATE ../src)
target_link_libraries(test_solver PRIVATE solver GTest::gtest_main GTest::gmock_main)
target_compile_features(test_solver PUBLIC cxx_std_20)

include(CTest)
include(GoogleTest)
gtest_discover_tests(test_solver)
