# Compile the windows static library
cmake_minimum_required (VERSION 3.20)

cmake_policy(SET CMP0091 NEW)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(VCPKG_TARGET_TRIPLET x64-windows-static)
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

set(This WindowsLib)

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

set(Headers
  button_control.h
  check_box_control.h
  class_dialog.h
  class_window.h
  context_menu.h
  data_types.h
  device_context.h
  drag_list_control.h
  dropdown_control.h
  edit_control.h
  file_system.h
  grid_item.h
  grid_view.h
  group_box_control.h
  list_box_control.h
  list_view_control.h
  mdi_client.h
  popup.h
  radio_button_control.h
  resource_manager.h
  rich_edit_control.h
  status_control.h
  tab_control.h
  text_control.h
  toolbar_control.h
  tree_view_control.h
  window_control.h
  window_menu.h
  windows_item.h
  windows_ui.h
  win_ui_enums.h
)
set(Sources
  button_control.cpp
  check_box_control.cpp
  class_dialog.cpp
  class_window.cpp
  context_menu.cpp
  drag_list_control.cpp
  dropdown_control.cpp
  edit_control.cpp
  file_system.cpp
  grid_item.cpp
  grid_view.cpp
  group_box_control.cpp
  list_box_control.cpp
  list_view_control.cpp
  mdi_client.cpp
  popup.cpp
  radio_button_control.cpp
  rich_edit_control.cpp
  status_control.cpp
  tab_control.cpp
  text_control.cpp
  toolbar_control.cpp
  tree_view_control.cpp
  window_control.cpp
  window_menu.cpp
  windows_item.cpp
)

add_compile_definitions(_UNICODE UNICODE)
add_library(${This} STATIC ${Sources} ${Headers})
