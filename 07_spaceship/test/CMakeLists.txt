
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(GTest REQUIRED)

set(TESTS
    test_point
    test_vector
    test_move
    test_rotate
)

include(CTest)
include(GoogleTest)

foreach(TEST ${TESTS})
    add_executable(${TEST} ${TEST}.cpp)

    target_include_directories(${TEST} PRIVATE ../src)
    target_link_libraries(${TEST} PRIVATE spaceship GTest::gmock_main)
    target_compile_features(${TEST} PUBLIC cxx_std_20)

    gtest_discover_tests(${TEST})
endforeach()

