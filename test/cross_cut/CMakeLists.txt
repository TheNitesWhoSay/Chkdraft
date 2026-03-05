# Compile the cross-cut test executable
cmake_minimum_required (VERSION 3.20)

cmake_policy(SET CMP0091 NEW)

set(This CrossCutTest)

project(${This})

set(CrossCutTestPath ${CMAKE_CURRENT_SOURCE_DIR})
cmake_path(GET CrossCutTestPath PARENT_PATH TestPath)
cmake_path(GET TestPath PARENT_PATH ChkdraftRootPath)
include_directories(${ChkdraftRootPath}/src)

if ( PROJECT_IS_TOP_LEVEL )
  add_subdirectory(${ChkdraftRootPath}/src/cross_cut ${CMAKE_CURRENT_BINARY_DIR}/cross_cut_lib)
endif()

set(Headers
  
)
set(Sources
  cross_cut_test_main.cpp
  logger_test.cpp
)

add_executable(${This} ${Sources} ${Headers})

enable_testing()

find_package(GTest CONFIG REQUIRED)
include(GoogleTest)
target_link_libraries(${This} PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main CrossCut)

if ( WIN64_DEBUG_ASAN ) # The asan dll must be made available for gtest_discover_tests to use the test exe
    configure_file("$ENV{VCToolsInstallDir}/bin/Hostx64/x64/clang_rt.asan_dynamic-x86_64.dll"
        "${CMAKE_CURRENT_BINARY_DIR}/clang_rt.asan_dynamic-x86_64.dll" COPYONLY)
endif ()

gtest_discover_tests(${This} DISCOVERY_MODE PRE_TEST)