set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

if(APPLE)
    execute_process(
        COMMAND xcrun --show-sdk-path
        OUTPUT_VARIABLE PLATFORM_SYSROOT
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE SDK_RESULT
    )
    if(NOT SDK_RESULT EQUAL 0 OR NOT PLATFORM_SYSROOT)
        message(FATAL_ERROR "Cannot find macOS SDK")
    endif()

    # Clang именно из того LLVM, с которым линкуется generator.
    execute_process(
        COMMAND ${LLVM_TOOLS_BINARY_DIR}/clang -print-resource-dir
        OUTPUT_VARIABLE CLANG_RESOURCE_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE RES_RESULT
    )
    if(NOT RES_RESULT EQUAL 0 OR NOT EXISTS "${CLANG_RESOURCE_DIR}/include/stdarg.h")
        message(FATAL_ERROR "Bad clang resource dir: '${CLANG_RESOURCE_DIR}'")
    endif()
    message(STATUS "Clang resource dir: ${CLANG_RESOURCE_DIR}")

    set(CLANG_PLATFORM_ARGS
        -stdlib=libc++
        -isysroot ${PLATFORM_SYSROOT}
        -resource-dir ${CLANG_RESOURCE_DIR}
        -Wno-pragma-once-outside-header
    )
elseif(UNIX)
    set(CLANG_PLATFORM_ARGS
        -Wno-pragma-once-outside-header
    )
endif()

message(STATUS "LLVM version: ${LLVM_PACKAGE_VERSION}")

add_executable(generator
    main.cpp
    ast_analyzer.hpp
    ast_analyzer.cpp
    generator.hpp
    generator.cpp
)

target_include_directories(generator PRIVATE ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})

target_link_libraries(generator PRIVATE clang-cpp LLVM)

target_compile_features(generator PUBLIC cxx_std_20)

# Директория с интерфейсами
set(INTERFACES_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/../src"
    # ../src
)

# Рекурсивно ищем заголовочные файлы
file(GLOB_RECURSE INTERFACE_HEADERS
    CONFIGURE_DEPENDS

    ${INTERFACES_DIR}/*.h
    ${INTERFACES_DIR}/*.hpp
    ${INTERFACES_DIR}/*.hh
    ${INTERFACES_DIR}/*.hxx
)

# Проверяем, что файлы найдены.
if(NOT INTERFACE_HEADERS)
    message(FATAL_ERROR
        "No interface headers found in ${INTERFACES_DIR}"
    )
endif()

message(STATUS "Found interface headers:")

foreach(HEADER ${INTERFACE_HEADERS})
    message(STATUS "  ${HEADER}")
endforeach()


# Генерация адаптеров.
# set(GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
# set(GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated" PARENT_SCOPE)
set(GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(GENERATED_DIR "${GENERATED_DIR}" PARENT_SCOPE)
add_custom_target(generate-adapters ALL
    # Удаление старых файлов
    COMMAND ${CMAKE_COMMAND} -E rm -rf "${GENERATED_DIR}"

    # Создание директории для генерации файлов
    COMMAND ${CMAKE_COMMAND} -E make_directory ${GENERATED_DIR}


    # COMMAND $<TARGET_FILE:generator>
    # -p "${CMAKE_BINARY_DIR}"
    # ${INTERFACE_HEADERS}
    # --output-dir=${GENERATED_DIR}

    COMMAND $<TARGET_FILE:generator>
        -p "${CMAKE_BINARY_DIR}"
        ${INTERFACE_HEADERS}
        --output-dir=${GENERATED_DIR}
        --
        -x c++
        -std=c++20
        ${CLANG_PLATFORM_ARGS}
        -I${INTERFACES_DIR}

    DEPENDS generator ${INTERFACE_HEADERS}

    COMMENT "Scanning interfaces and generating adapters"

    COMMAND_EXPAND_LISTS
    VERBATIM
)
