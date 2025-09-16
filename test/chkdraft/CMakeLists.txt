# Compile the chkdraft test executable
cmake_minimum_required (VERSION 3.20)

cmake_policy(SET CMP0091 NEW)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(VCPKG_TARGET_TRIPLET x64-windows-static)
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

set(This ChkdraftTest)

project(${This})

set(ChkdraftTestPath ${CMAKE_CURRENT_SOURCE_DIR})
cmake_path(GET ChkdraftTestPath PARENT_PATH TestPath)
cmake_path(GET TestPath PARENT_PATH ChkdraftRootPath)
include_directories(${ChkdraftRootPath}/src)

set(CMAKE_BUILD_PARALLEL_LEVEL)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 20)
add_compile_options(/permissive-)
add_compile_definitions(_UNICODE UNICODE NOMINMAX)

if ( PROJECT_IS_TOP_LEVEL )
  add_subdirectory(${ChkdraftRootPath}/src/cross_cut ${CMAKE_CURRENT_BINARY_DIR}/cross_cut)
  add_subdirectory(${ChkdraftRootPath}/src/chkdraft ${CMAKE_CURRENT_BINARY_DIR}/chkdraft)
endif()

set(Headers
  
)
set(Sources
  chkdraft_test_main.cpp
  chkd_profiles_test.cpp
  chkd_skin_test.cpp
  constants_test.cpp
)

add_executable(${This} ${Sources} ${Headers})

enable_testing()

find_package(GTest CONFIG REQUIRED)
include(GoogleTest)
target_link_libraries(${This} PRIVATE GTest::gtest_main CrossCut)

gtest_discover_tests(${This} DISCOVERY_MODE PRE_TEST)