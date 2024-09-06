# Compile the test executables (ChkdraftTest, CrossCutTest, MappingCoreTest, WindowsTest) and their respective libraries if necessary
cmake_minimum_required (VERSION 3.20)

cmake_policy(SET CMP0091 NEW)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(VCPKG_TARGET_TRIPLET x64-windows-static)
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

set(This AllTests)

project(${This})

set(CMAKE_BUILD_PARALLEL_LEVEL)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 20)
add_compile_options(/permissive-)
add_compile_definitions(_UNICODE UNICODE NOMINMAX)

set(TestPath ${CMAKE_CURRENT_SOURCE_DIR})
cmake_path(GET TestPath PARENT_PATH ChkdraftRootPath)
include_directories(${ChkdraftRootPath}/src)

if ( PROJECT_IS_TOP_LEVEL )
  add_subdirectory(${ChkdraftRootPath}/src/cross_cut ${CMAKE_CURRENT_BINARY_DIR}/test_top_level/cross_cut_lib)
  add_subdirectory(${ChkdraftRootPath}/src/mapping_core ${CMAKE_CURRENT_BINARY_DIR}/test_top_level/mapping_core)
  add_subdirectory(${ChkdraftRootPath}/src/windows ${CMAKE_CURRENT_BINARY_DIR}/test_top_level/windows)
  add_subdirectory(${ChkdraftRootPath}/src/chkdraft ${CMAKE_CURRENT_BINARY_DIR}/test_top_level/chkdraft)
endif()

enable_testing()

#find_package(GTest CONFIG REQUIRED)

add_subdirectory(cross_cut)
add_subdirectory(mapping_core)
add_subdirectory(windows)
add_subdirectory(chkdraft)
