cmake_minimum_required(VERSION 3.20)

project(
    architecture
    VERSION 1.0.0
    DESCRIPTION "Homework of course Architecture and design patterns"
    LANGUAGES CXX
)

option(BUILD_TEST "Build tests" ON)

add_subdirectory(04_tdd)
