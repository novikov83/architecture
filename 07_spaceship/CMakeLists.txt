add_subdirectory(generator)
add_subdirectory(src)

if(BUILD_TEST)
    add_subdirectory(test)
endif()
