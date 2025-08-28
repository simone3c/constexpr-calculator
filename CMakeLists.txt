cmake_minimum_required(VERSION 3.28)
project(calc)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options("-Wextra;-Wall;-Wfloat-equal;-Wconversion;-Wshadow;-Wpointer-arith;-Wcast-align;-Wwrite-strings;")

include_directories(src)
include_directories(external)



option(BUILD_TESTS "Build the unit tests" ON)
if(BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )

    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()

    add_executable(
        calc_test
        test/calc_test.cpp
    )
    target_link_libraries(
        calc_test
        GTest::gtest_main
    )
    include(GoogleTest)
    gtest_discover_tests(calc_test)
endif()


add_executable(
  main
  src/main.cpp
)
