cmake_minimum_required(VERSION 3.10)

project (Cpp VERSION 1.0)

configure_file(
        "${PROJECT_SOURCE_DIR}/config.h.in"
        "${PROJECT_BINARY_DIR}/config.h"
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

include_directories(header)
# config.h is generated in binary dir
include_directories("${PROJECT_BINARY_DIR}")

add_executable(Unique test/unique.cc)
add_executable(Lazy test/lazy.cc)
add_executable(Variant test/variant.cc)

enable_testing()

add_test(NAME test_unique
        COMMAND Unique a)
set_tests_properties(test_unique
        PROPERTIES PASS_REGULAR_EXPRESSION 1)