# Compile the windows test executable
cmake_minimum_required (VERSION 3.20)

cmake_policy(SET CMP0091 NEW)

set(This WindowsTest)

project(${This})

set(WindowsTestPath ${CMAKE_CURRENT_SOURCE_DIR})
cmake_path(GET WindowsTestPath PARENT_PATH TestPath)
cmake_path(GET TestPath PARENT_PATH ChkdraftRootPath)
include_directories(${ChkdraftRootPath}/src)

if ( PROJECT_IS_TOP_LEVEL )
  set(CMAKE_BUILD_PARALLEL_LEVEL)
  set(CMAKE_C_STANDARD 99)
  set(CMAKE_CXX_STANDARD 20)
  add_compile_options(/permissive- ${ASAN})
  add_compile_definitions(_UNICODE UNICODE NOMINMAX)
  add_subdirectory(${ChkdraftRootPath}/src/cross_cut ${CMAKE_CURRENT_BINARY_DIR}/cross_cut_lib)
  add_subdirectory(${ChkdraftRootPath}/src/windows ${CMAKE_CURRENT_BINARY_DIR}/windows)
endif()

set(Headers
  
)
set(Sources
  file_system_test.cpp
  windows_test_main.cpp
)

add_executable(${This} ${Sources} ${Headers})

enable_testing()

find_package(GTest CONFIG REQUIRED)
include(GoogleTest)
target_link_libraries(${This} PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)

if ( WIN64_DEBUG_ASAN ) # The asan dll must be made available for gtest_discover_tests to use the test exe
    configure_file("$ENV{VCToolsInstallDir}/bin/Hostx64/x64/clang_rt.asan_dynamic-x86_64.dll"
        "${CMAKE_CURRENT_BINARY_DIR}/clang_rt.asan_dynamic-x86_64.dll" COPYONLY)
endif ()

gtest_discover_tests(${This} DISCOVERY_MODE PRE_TEST)