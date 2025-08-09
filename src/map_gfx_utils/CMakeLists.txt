# Compile the map-gfx-utils static library
cmake_minimum_required (VERSION 3.20)

cmake_policy(SET CMP0091 NEW)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
if (WIN32)
  set(VCPKG_TARGET_TRIPLET x64-windows-static)
endif ()
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

set(This MapGfxUtils)

project(${This})

set(MapGfxUtilsLibPath ${CMAKE_CURRENT_SOURCE_DIR})
cmake_path(GET MapGfxUtilsLibPath PARENT_PATH SrcPath)
cmake_path(GET SrcPath PARENT_PATH ChkdraftRootPath)
include_directories(
    ${ChkdraftRootPath}/src
    ${ChkdraftRootPath}/src/mapping_core/opengl
    ${ChkdraftRootPath}/src/mapping_core/opengl/glad
)

if ( PROJECT_IS_TOP_LEVEL )
  set(CMAKE_BUILD_PARALLEL_LEVEL)
  set(CMAKE_C_STANDARD 99)
  set(CMAKE_CXX_STANDARD 20)
  if ( "${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC" )
    add_compile_options(/permissive-)
  endif ()
  add_compile_definitions(_UNICODE UNICODE NOMINMAX _CRT_SECURE_NO_WARNINGS)
  add_subdirectory(${ChkdraftRootPath}/src/cross_cut ${CMAKE_CURRENT_BINARY_DIR}/cross_cut_lib)
  add_subdirectory(${ChkdraftRootPath}/src/mapping_core ${CMAKE_CURRENT_BINARY_DIR}/mapping_core_lib)
endif()

find_package(Freetype REQUIRED)
get_target_property(FREETYPE_INCLUDE_DIRS freetype INTERFACE_INCLUDE_DIRECTORIES)
include_directories(${FREETYPE_INCLUDE_DIRS})

find_package(harfbuzz CONFIG REQUIRED)
get_target_property(HARFBUZZ_INCLUDE_DIRS harfbuzz::harfbuzz INTERFACE_INCLUDE_DIRECTORIES)
include_directories(${HARFBUZZ_INCLUDE_DIRS})

find_package(WebP CONFIG REQUIRED)

find_package(ICU COMPONENTS uc i18n REQUIRED)

find_package(glad CONFIG REQUIRED)
get_target_property(GLAD_INCLUDE_DIRS glad::glad INTERFACE_INCLUDE_DIRECTORIES)
include_directories(${GLAD_INCLUDE_DIRS})

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

include(FetchContent)
FetchContent_Declare(
  stb
  GIT_REPOSITORY https://github.com/nothings/stb.git
  GIT_TAG ae721c5 # 2024-02-13
)
FetchContent_MakeAvailable(stb)
include_directories(${stb_SOURCE_DIR}/include)

include(FetchContent)
FetchContent_Declare(
  rarecpp
  GIT_REPOSITORY https://github.com/TheNitesWhoSay/RareCpp.git
  GIT_TAG 4acbf69 # release-2.5.0TODO
)
FetchContent_MakeAvailable(rarecpp)
include_directories(${rarecpp_SOURCE_DIR}/include)

set(Headers
  test.h
)
set(Sources
  test.cpp
)

add_compile_definitions(_UNICODE UNICODE CASCLIB_UNICODE STORMLIB_NO_AUTO_LINK)
add_library(${This}Lib STATIC ${Sources} ${Headers})

add_executable(${This} ${Sources} ${Headers})

find_package(OpenGL REQUIRED)
if (WIN32)
target_link_libraries(${This} PRIVATE CrossCut MappingCore glad::glad glfw glm::glm Freetype::Freetype harfbuzz::harfbuzz harfbuzz::harfbuzz-subset ICU::uc ICU::i18n WebP::webp WebP::webpdecoder WebP::webpdemux ${OPENGL_LIBRARIES} Comctl32.lib Winmm.lib Msimg32.lib)
else ()
target_link_libraries(${This} PRIVATE CrossCut MappingCore glad::glad glfw glm::glm Freetype::Freetype harfbuzz::harfbuzz harfbuzz::harfbuzz-subset ICU::uc ICU::i18n WebP::webp WebP::webpdecoder WebP::webpdemux ${OPENGL_LIBRARIES})
endif ()