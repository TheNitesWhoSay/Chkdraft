# Compile the cross-cut static library
cmake_minimum_required (VERSION 3.20)

cmake_policy(SET CMP0091 NEW)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(VCPKG_TARGET_TRIPLET x64-windows-static)
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

set(This CrossCut)

project(${This})

set(Headers
  logger.h
  simple_icu.h
)
set(Sources
  logger.cpp
  simple_icu.cpp
)

add_library(${This} STATIC ${Sources} ${Headers})

find_package(ICU COMPONENTS uc i18n REQUIRED)

target_link_libraries(${This} ICU::uc ICU::i18n)
