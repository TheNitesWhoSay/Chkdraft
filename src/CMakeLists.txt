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
add_compile_definitions(_UNICODE UNICODE)

find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

include(FetchContent)
FetchContent_Declare(
  rarecpp
  GIT_REPOSITORY https://github.com/TheNitesWhoSay/RareCpp.git
  GIT_TAG d437771 # release-2.3.2
)
FetchContent_MakeAvailable(rarecpp)
include_directories(${rarecpp_SOURCE_DIR}/include)

if ( PROJECT_IS_TOP_LEVEL )
  set(CMAKE_BUILD_PARALLEL_LEVEL)
  set(CMAKE_C_STANDARD 99)
  set(CMAKE_CXX_STANDARD 20)
  add_compile_options(/permissive-)
  add_compile_definitions(_UNICODE UNICODE NOMINMAX CHKDRAFT)
endif()

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
target_link_libraries(Chkdraft glad::glad glm::glm CrossCut MappingCore WindowsLib ChkdraftLib opengl32.lib Comctl32.lib Winmm.lib Msimg32.lib)
