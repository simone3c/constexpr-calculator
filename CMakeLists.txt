cmake_minimum_required(VERSION 3.28)
project(calc)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


add_compile_options("-Wextra;-Wall;-Wfloat-equal;-Wconversion;-Wshadow;-Wpointer-arith;-Wcast-align;-Wwrite-strings;")


add_library(calc INTERFACE)
target_include_directories(calc 
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src/include
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/external  
)

target_compile_features(calc INTERFACE cxx_std_23)

option(BUILD_TESTS "Build the unit tests" OFF)
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
        calc
    )

    include(GoogleTest)
    gtest_discover_tests(calc_test)
endif()

