add_subdirectory(src)

if(BUILD_TEST)
    enable_testing()
    add_subdirectory(test)
endif()
