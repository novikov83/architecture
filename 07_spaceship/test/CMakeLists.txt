
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(GTest REQUIRED)

set(TESTS
    test_point
    test_vector
    test_move
    test_rotate
    test_command
    test_fuel
    test_velocity
    test_ioc
    test_adapter
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

message(STATUS "CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "BINARY_DIR: ${CMAKE_BINARY_DIR}")
message(STATUS "GENERATED_DIR: ${GENERATED_DIR}")

target_include_directories(test_adapter PRIVATE
    "${GENERATED_DIR}"
)

add_dependencies(test_adapter generate-adapters)

