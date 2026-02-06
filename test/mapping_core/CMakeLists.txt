# Compile the mapping-core test executable
cmake_minimum_required (VERSION 3.20)

cmake_policy(SET CMP0091 NEW)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
if (WIN32)
  set(VCPKG_TARGET_TRIPLET x64-windows-static)
endif ()
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

set(This MappingCoreTest)

project(${This})

set(MappingCoreTestPath ${CMAKE_CURRENT_SOURCE_DIR})
cmake_path(GET MappingCoreTestPath PARENT_PATH TestPath)
cmake_path(GET TestPath PARENT_PATH ChkdraftRootPath)
include_directories(${ChkdraftRootPath}/src)

if ( PROJECT_IS_TOP_LEVEL )
  set(CMAKE_BUILD_PARALLEL_LEVEL)
  set(CMAKE_C_STANDARD 99)
  set(CMAKE_CXX_STANDARD 20)
  if ( "${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC" )
    add_compile_options(/permissive-)
  endif ()
  add_compile_definitions(_UNICODE UNICODE NOMINMAX _CRT_SECURE_NO_WARNINGS STORMLIB_NO_AUTO_LINK DU_STATIC_IMPLEMENTATION)
  add_subdirectory(${ChkdraftRootPath}/src/cross_cut ${CMAKE_CURRENT_BINARY_DIR}/cross_cut_lib)
  add_subdirectory(${ChkdraftRootPath}/src/mapping_core ${CMAKE_CURRENT_BINARY_DIR}/mapping_core_lib)
endif()
  
include(FetchContent)
FetchContent_Declare(
  rarecpp
  GIT_REPOSITORY https://github.com/TheNitesWhoSay/RareCpp.git
  GIT_TAG d5b58a6 # release-2.6.0
)
FetchContent_MakeAvailable(rarecpp)
include_directories(${rarecpp_SOURCE_DIR}/include)

set(Headers
  test_assets.h
)
set(Sources
  basics_test.cpp
  mapping_core_test_main.cpp
  system_io_test.cpp
  test_assets.cpp
  text_trig_compiler_test.cpp
)

add_executable(${This} ${Sources} ${Headers})

enable_testing()

find_package(GTest CONFIG REQUIRED)
find_package(BZip2 REQUIRED)
find_package(CascLib CONFIG REQUIRED)
add_definitions(-DUNICODE -D_UNICODE)
find_package(StormLib CONFIG REQUIRED)
include(GoogleTest)

if (WIN32)
target_link_libraries(${This} PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main CrossCut MappingCore BZip2::BZip2 CascLib::casc_static StormLib::storm winmm.lib comctl32.lib Kernel32.lib)
else ()
target_link_libraries(${This} PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main CrossCut MappingCore BZip2::BZip2 CascLib::casc_static StormLib::storm)
endif ()
gtest_discover_tests(${This} DISCOVERY_MODE PRE_TEST)