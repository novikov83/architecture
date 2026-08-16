cmake_minimum_required(VERSION 3.20)

project(
    tdd
    VERSION 1.0.0
    DESCRIPTION "Homework 04 TDD"
    LANGUAGES CXX
)

option(BUILD_TEST "Build tests" ON)

add_subdirectory(src)

if(BUILD_TEST)
    enable_testing()
    add_subdirectory(test)
endif()
