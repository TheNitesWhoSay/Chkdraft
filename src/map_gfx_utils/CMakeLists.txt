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
  GIT_TAG 421d534 # release-2.5.0TODO
)
FetchContent_MakeAvailable(rarecpp)
include_directories(${rarecpp_SOURCE_DIR}/include)

set(Headers
  gfx_util.h
  map_timings.h
  renderer.h
  sc_map.h
  screenshot.h
  ../mapping_core/opengl/glfw/context.h
  ../mapping_core/opengl/glfw/window.h
  ../mapping_core/opengl/glad/utils.h
  ../mapping_core/opengl/gl/base.h
  ../mapping_core/opengl/gl/camera.h
  ../mapping_core/opengl/gl/context_semaphore.h
  ../mapping_core/opengl/gl/font.h
  ../mapping_core/opengl/gl/fps.h
  ../mapping_core/opengl/gl/palette.h
  ../mapping_core/opengl/gl/program.h
  ../mapping_core/opengl/gl/shader.h
  ../mapping_core/opengl/gl/texture.h
  ../mapping_core/opengl/gl/uniform.h
  ../mapping_core/opengl/gl/unique_resource.h
  ../mapping_core/opengl/gl/utils.h
  ../mapping_core/opengl/gl/vertices.h
  ../mapping_core/render/color_cycler.h
  ../mapping_core/render/game_clock.h
  ../mapping_core/render/memory_tiered_tasks.h
  ../mapping_core/render/map_actor.h
  ../mapping_core/render/map_animations.h
  ../mapping_core/render/map_image.h
  ../mapping_core/render/sc_gl_graphics.h
)
set(Sources
  gfx_util.cpp
  lato.ttf.cpp
  main.cpp
  renderer.cpp
  sc_map.cpp
  screenshot.cpp
  ../mapping_core/opengl/stb/stb.cpp
  ../mapping_core/render/map_actor.cpp
  ../mapping_core/render/map_animations.cpp
  ../mapping_core/render/map_image.cpp
  ../mapping_core/render/sc_gl_graphics.cpp
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