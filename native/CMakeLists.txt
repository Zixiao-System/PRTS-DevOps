cmake_minimum_required(VERSION 3.20)
project(prts_native
    VERSION 0.1.0
    DESCRIPTION "PRTS DevOps Platform - Native Performance Module"
    LANGUAGES C
)

# C Standard
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Wpedantic")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")

# Platform detection
if(APPLE)
    set(PLATFORM_SOURCES src/platform/darwin.c)
    set(PLATFORM_LIBS "-framework CoreFoundation")
elseif(UNIX)
    set(PLATFORM_SOURCES src/platform/linux.c)
    set(PLATFORM_LIBS pthread)
elseif(WIN32)
    set(PLATFORM_SOURCES src/platform/windows.c)
    set(PLATFORM_LIBS ws2_32)
endif()

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/core/memory_pool.c
    src/core/thread_pool.c
    src/core/ring_buffer.c
    src/metrics/collector.c
    src/metrics/aggregator.c
    src/log/parser.c
    src/log/indexer.c
    ${PLATFORM_SOURCES}
)

# Shared library
add_library(prts_native SHARED ${SOURCES})
target_link_libraries(prts_native ${PLATFORM_LIBS})

# Static library
add_library(prts_native_static STATIC ${SOURCES})
target_link_libraries(prts_native_static ${PLATFORM_LIBS})

# Python extension (optional)
option(BUILD_PYTHON_EXTENSION "Build Python extension" ON)
if(BUILD_PYTHON_EXTENSION)
    find_package(Python3 COMPONENTS Development)
    if(Python3_FOUND)
        add_library(prts_native_py SHARED
            src/bindings/python_bindings.c
            ${SOURCES}
        )
        target_include_directories(prts_native_py PRIVATE ${Python3_INCLUDE_DIRS})
        target_link_libraries(prts_native_py ${Python3_LIBRARIES} ${PLATFORM_LIBS})
        set_target_properties(prts_native_py PROPERTIES
            PREFIX ""
            SUFFIX ".so"
        )
    endif()
endif()

# Tests
option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_executable(test_memory_pool tests/test_memory_pool.c)
    target_link_libraries(test_memory_pool prts_native_static)
    add_test(NAME test_memory_pool COMMAND test_memory_pool)

    add_executable(test_thread_pool tests/test_thread_pool.c)
    target_link_libraries(test_thread_pool prts_native_static)
    add_test(NAME test_thread_pool COMMAND test_thread_pool)
endif()

# Install
install(TARGETS prts_native prts_native_static
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
install(DIRECTORY include/ DESTINATION include)

# CPack
set(CPACK_PACKAGE_NAME "prts-native")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
include(CPack)
