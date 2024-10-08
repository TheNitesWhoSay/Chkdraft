# Compile the mapping-core static library
cmake_minimum_required (VERSION 3.20)

cmake_policy(SET CMP0091 NEW)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(VCPKG_TARGET_TRIPLET x64-windows-static)
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

set(This MappingCore)

project(${This})

set(MappingCoreLibPath ${CMAKE_CURRENT_SOURCE_DIR})
cmake_path(GET MappingCoreLibPath PARENT_PATH SrcPath)
cmake_path(GET SrcPath PARENT_PATH ChkdraftRootPath)
include_directories(${ChkdraftRootPath}/src)

if ( PROJECT_IS_TOP_LEVEL )
  set(CMAKE_BUILD_PARALLEL_LEVEL)
  set(CMAKE_C_STANDARD 99)
  set(CMAKE_CXX_STANDARD 20)
  add_compile_options(/permissive-)
  add_compile_definitions(_UNICODE UNICODE NOMINMAX)
  add_subdirectory(${ChkdraftRootPath}/src/cross_cut ${CMAKE_CURRENT_BINARY_DIR}/cross_cut_lib)
endif()

include(FetchContent)
FetchContent_Declare(
  rarecpp
  GIT_REPOSITORY https://github.com/TheNitesWhoSay/RareCpp.git
  GIT_TAG 21ba359 # release-2.3.3
)
FetchContent_MakeAvailable(rarecpp)
include_directories(${rarecpp_SOURCE_DIR}/include)

set(Headers
  archive_cluster.h
  archive_file.h
  basics.h
  casc_archive.h
  chk.h
  escape_strings.h
  file_browser.h
  map_file.h
  mapping_core.h
  mpq_file.h
  sc.h
  scenario.h
  sha256.h
  system_io.h
  text_trig_compiler.h
  text_trig_generator.h
)
set(Sources
  archive_cluster.cpp
  archive_file.cpp
  basics.cpp
  casc_archive.cpp
  chk.cpp
  escape_strings.cpp
  file_browser.cpp
  map_file.cpp
  mpq_file.cpp
  sc.cpp
  scenario.cpp
  sha256.cpp
  system_io.cpp
  text_trig_compiler.cpp
  text_trig_generator.cpp
)

add_compile_definitions(_UNICODE UNICODE CASCLIB_UNICODE STORMLIB_NO_AUTO_LINK)
add_library(${This} STATIC ${Sources} ${Headers})

find_package(BZip2 REQUIRED)
find_package(CascLib CONFIG REQUIRED)
add_definitions(-DUNICODE -D_UNICODE)
find_package(StormLib CONFIG REQUIRED)


target_link_libraries(${This} PRIVATE BZip2::BZip2 CascLib::casc_static StormLib::storm)
