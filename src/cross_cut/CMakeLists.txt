# Compile the cross-cut static library
cmake_minimum_required (VERSION 3.20)

cmake_policy(SET CMP0091 NEW)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(VCPKG_TARGET_TRIPLET x64-windows-static)
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

set(This CrossCut)

project(${This})

set(Headers
  commander.h
  command_types.h
  error_handler.h
  generic_command.h
  logger.h
  simple_icu.h
  test_commands.h
  updater.h
)
set(Sources
  commander.cpp
  error_handler.cpp
  generic_command.cpp
  logger.cpp
  simple_icu.cpp
  test_commands.cpp
  updater.cpp
)

add_library(${This} STATIC ${Sources} ${Headers})

find_package(ICU COMPONENTS uc i18n REQUIRED)

target_link_libraries(${This} ICU::uc ICU::i18n)
