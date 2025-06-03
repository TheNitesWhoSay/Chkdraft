# Compile the Chkdraft executable, the sub-libraries (Chkdraft, CrossCut, MappingCore, Windows)
cmake_minimum_required (VERSION 3.20)

cmake_policy(SET CMP0091 NEW)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(VCPKG_TARGET_TRIPLET x64-windows-static)
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

set(This Chkdraft)

project(${This})

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/chkdraft
    ${CMAKE_CURRENT_SOURCE_DIR}/chkdraft/mapping/opengl
    ${CMAKE_CURRENT_SOURCE_DIR}/chkdraft/mapping/opengl/glad
)

set(CMAKE_BUILD_PARALLEL_LEVEL)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 20)
add_compile_options(/permissive-)
add_compile_definitions(_UNICODE UNICODE NOMINMAX CHKDRAFT)

include(FetchContent)
FetchContent_Declare(
  rarecpp
  GIT_REPOSITORY https://github.com/TheNitesWhoSay/RareCpp.git
  GIT_TAG 22b142c # release-2.5.0TODO
)
FetchContent_MakeAvailable(rarecpp)
include_directories(${rarecpp_SOURCE_DIR}/include)

find_package(Freetype REQUIRED)
get_target_property(FREETYPE_INCLUDE_DIRS freetype INTERFACE_INCLUDE_DIRECTORIES)
include_directories(${FREETYPE_INCLUDE_DIRS})

find_package(harfbuzz CONFIG REQUIRED)
get_target_property(HARFBUZZ_INCLUDE_DIRS harfbuzz::harfbuzz INTERFACE_INCLUDE_DIRECTORIES)
include_directories(${HARFBUZZ_INCLUDE_DIRS})

find_package(glm CONFIG REQUIRED)

set(Headers
  
)
set(Sources
  main.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(cross_cut)
add_subdirectory(mapping_core)
add_subdirectory(windows)
add_subdirectory(chkdraft)

add_executable(${This} WIN32 ${Sources} ${Headers})
target_sources(${This} PRIVATE ../res/resource.rc)
target_link_libraries(Chkdraft CrossCut MappingCore WindowsLib ChkdraftLib Freetype::Freetype harfbuzz::harfbuzz harfbuzz::harfbuzz-subset glm::glm opengl32.lib Comctl32.lib Winmm.lib Msimg32.lib)
