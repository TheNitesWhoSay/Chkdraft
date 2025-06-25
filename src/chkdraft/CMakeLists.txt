# Compile the chkdraft static library
cmake_minimum_required (VERSION 3.20)

cmake_policy(SET CMP0091 NEW)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(VCPKG_TARGET_TRIPLET x64-windows-static)
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

set(This ChkdraftLib)

project(${This})

set(ChkdraftLibPath ${CMAKE_CURRENT_SOURCE_DIR})
cmake_path(GET ChkdraftLibPath PARENT_PATH SrcPath)
cmake_path(GET SrcPath PARENT_PATH ChkdraftRootPath)
include_directories(
    ${ChkdraftRootPath}/src
    ${ChkdraftRootPath}/src/chkdraft
    ${ChkdraftRootPath}/src/chkdraft/mapping/opengl
    ${ChkdraftRootPath}/src/chkdraft/mapping/opengl/glad
)

include(FetchContent)
FetchContent_Declare(
  rarecpp
  GIT_REPOSITORY https://github.com/TheNitesWhoSay/RareCpp.git
  GIT_TAG 328e0e0 # release-2.5.0TODO
)
FetchContent_MakeAvailable(rarecpp)
include_directories(${rarecpp_SOURCE_DIR}/include)

if ( PROJECT_IS_TOP_LEVEL )
  set(CMAKE_BUILD_PARALLEL_LEVEL)
  set(CMAKE_C_STANDARD 99)
  set(CMAKE_CXX_STANDARD 20)
  add_compile_options(/permissive-)
  add_compile_definitions(_UNICODE UNICODE NOMINMAX)
endif()

find_package(Freetype REQUIRED)
get_target_property(FREETYPE_INCLUDE_DIRS freetype INTERFACE_INCLUDE_DIRECTORIES)
include_directories(${FREETYPE_INCLUDE_DIRS})

find_package(harfbuzz CONFIG REQUIRED)
get_target_property(HARFBUZZ_INCLUDE_DIRS harfbuzz::harfbuzz INTERFACE_INCLUDE_DIRECTORIES)
include_directories(${HARFBUZZ_INCLUDE_DIRS})

find_package(glm CONFIG REQUIRED)

set(Headers
  chkdraft.h
  common_files/common_files.h
  common_files/constants.h
  common_files/debug.h
  common_files/resource.h
  common_files/structs.h
  common_files/version.h
  mapping/map_actor.cpp
  mapping/map_animations.h
  mapping/map_animations.cpp
  mapping/map_image.cpp
  mapping/chkd_plugins.h
  mapping/clipboard.h
  mapping/color_cycler.h
  mapping/data_file_browsers.h
  mapping/game_clock.h
  mapping/memory_tiered_tasks.h
  mapping/sc_graphics.h
  mapping/scr_graphics.h
  mapping/selections.h
  mapping/settings.h
  mapping/wgl_render_context.h
  mapping/opengl/gl/base.h
  mapping/opengl/gl/context_semaphore.h
  mapping/opengl/gl/font.h
  mapping/opengl/gl/fps.h
  mapping/opengl/gl/palette.h
  mapping/opengl/gl/program.h
  mapping/opengl/gl/shader.h
  mapping/opengl/gl/texture.h
  mapping/opengl/gl/uniform.h
  mapping/opengl/gl/unique_resource.h
  mapping/opengl/gl/vertices.h
  mapping/opengl/glad/glad/gl.h
  mapping/opengl/glad/KHR/khrplatform.h
  ui/chkd_controls/chkd_string_input.h
  ui/chkd_controls/cnd_act_grid.h
  ui/chkd_controls/cuwp_input.h
  ui/chkd_controls/doodad_tree.h
  ui/chkd_controls/history_tree.h
  ui/chkd_controls/isom_tree.h
  ui/chkd_controls/location_tree.h
  ui/chkd_controls/main_tree.h
  ui/chkd_controls/move_to.h
  ui/chkd_controls/peer_list_box.h
  ui/chkd_controls/player_dropdown.h
  ui/chkd_controls/sprite_tree.h
  ui/chkd_controls/suggestions.h
  ui/chkd_controls/tech_tree.h
  ui/chkd_controls/unit_tree.h
  ui/chkd_controls/upgrade_tree.h
  ui/dialog_windows/about.h
  ui/dialog_windows/dialog_windows.h
  ui/dialog_windows/dimensions.h
  ui/dialog_windows/briefing_editor/briefing_editor.h
  ui/dialog_windows/briefing_editor/briefing_templates.h
  ui/dialog_windows/briefing_editor/briefing_trig_actions.h
  ui/dialog_windows/briefing_editor/briefing_triggers.h
  ui/dialog_windows/briefing_editor/briefing_trig_modify.h
  ui/dialog_windows/briefing_editor/briefing_trig_modify_text.h
  ui/dialog_windows/briefing_editor/briefing_trig_players.h
  ui/dialog_windows/layers/location_properties.h
  ui/dialog_windows/layers/terrain_palette.h
  ui/dialog_windows/layers/tile_properties.h
  ui/dialog_windows/layers/sprite_properties.h
  ui/dialog_windows/layers/unit_properties.h
  ui/dialog_windows/map_protection/change_password.h
  ui/dialog_windows/map_protection/enter_password.h
  ui/dialog_windows/map_settings/color_properties.h
  ui/dialog_windows/map_settings/forces.h
  ui/dialog_windows/map_settings/map_properties.h
  ui/dialog_windows/map_settings/map_settings.h
  ui/dialog_windows/map_settings/sound_editor.h
  ui/dialog_windows/map_settings/tech_settings.h
  ui/dialog_windows/map_settings/unit_settings.h
  ui/dialog_windows/map_settings/upgrade_settings.h
  ui/dialog_windows/map_settings/string_editor/string_editor.h
  ui/dialog_windows/map_settings/string_editor/string_guide.h
  ui/dialog_windows/map_settings/string_editor/string_preview.h
  ui/dialog_windows/new_map/new_map.h
  ui/dialog_windows/text_trig/briefing_text_trig.h
  ui/dialog_windows/text_trig/lit.h
  ui/dialog_windows/text_trig/text_trig.h
  ui/dialog_windows/trig_editor/counters.h
  ui/dialog_windows/trig_editor/cuwps.h
  ui/dialog_windows/trig_editor/switches.h
  ui/dialog_windows/trig_editor/templates.h
  ui/dialog_windows/trig_editor/trig_editor.h
  ui/dialog_windows/trig_editor/triggers.h
  ui/dialog_windows/trig_modify/trig_actions.h
  ui/dialog_windows/trig_modify/trig_conditions.h
  ui/dialog_windows/trig_modify/trig_general.h
  ui/dialog_windows/trig_modify/trig_modify.h
  ui/dialog_windows/trig_modify/trig_modify_text.h
  ui/dialog_windows/trig_modify/trig_players.h
  ui/main_windows/gui_map.h
  ui/main_windows/left_bar.h
  ui/main_windows/logger_window.h
  ui/main_windows/main_plot.h
  ui/main_windows/main_toolbar.h
  ui/main_windows/maps.h
  ui/main_windows/mini_map.h
)
set(Sources
  chkdraft.cpp
  common_files/constants.cpp
  common_files/debug.cpp
  common_files/version.cpp
  mapping/chkd_plugins.cpp
  mapping/clipboard.cpp
  mapping/data_file_browsers.cpp
  mapping/sc_graphics.cpp
  mapping/scr_graphics.cpp
  mapping/selections.cpp
  mapping/settings.cpp
  mapping/opengl/glad/glad/gl.c
  ui/chkd_controls/chkd_string_input.cpp
  ui/chkd_controls/cnd_act_grid.cpp
  ui/chkd_controls/cuwp_input.cpp
  ui/chkd_controls/doodad_tree.cpp
  ui/chkd_controls/history_tree.cpp
  ui/chkd_controls/isom_tree.cpp
  ui/chkd_controls/location_tree.cpp
  ui/chkd_controls/main_tree.cpp
  ui/chkd_controls/move_to.cpp
  ui/chkd_controls/peer_list_box.cpp
  ui/chkd_controls/player_dropdown.cpp
  ui/chkd_controls/sprite_tree.cpp
  ui/chkd_controls/suggestions.cpp
  ui/chkd_controls/tech_tree.cpp
  ui/chkd_controls/unit_tree.cpp
  ui/chkd_controls/upgrade_tree.cpp
  ui/dialog_windows/about.cpp
  ui/dialog_windows/dimensions.cpp
  ui/dialog_windows/briefing_editor/briefing_editor.cpp
  ui/dialog_windows/briefing_editor/briefing_templates.cpp
  ui/dialog_windows/briefing_editor/briefing_trig_actions.cpp
  ui/dialog_windows/briefing_editor/briefing_triggers.cpp
  ui/dialog_windows/briefing_editor/briefing_trig_modify.cpp
  ui/dialog_windows/briefing_editor/briefing_trig_modify_text.cpp
  ui/dialog_windows/briefing_editor/briefing_trig_players.cpp
  ui/dialog_windows/layers/location_properties.cpp
  ui/dialog_windows/layers/terrain_palette.cpp
  ui/dialog_windows/layers/tile_properties.cpp
  ui/dialog_windows/layers/sprite_properties.cpp
  ui/dialog_windows/layers/unit_properties.cpp
  ui/dialog_windows/map_protection/change_password.cpp
  ui/dialog_windows/map_protection/enter_password.cpp
  ui/dialog_windows/map_settings/color_properties.cpp
  ui/dialog_windows/map_settings/forces.cpp
  ui/dialog_windows/map_settings/map_properties.cpp
  ui/dialog_windows/map_settings/map_settings.cpp
  ui/dialog_windows/map_settings/sound_editor.cpp
  ui/dialog_windows/map_settings/tech_settings.cpp
  ui/dialog_windows/map_settings/unit_settings.cpp
  ui/dialog_windows/map_settings/upgrade_settings.cpp
  ui/dialog_windows/map_settings/string_editor/string_editor.cpp
  ui/dialog_windows/map_settings/string_editor/string_guide.cpp
  ui/dialog_windows/map_settings/string_editor/string_preview.cpp
  ui/dialog_windows/new_map/new_map.cpp
  ui/dialog_windows/text_trig/briefing_text_trig.cpp
  ui/dialog_windows/text_trig/lit.cpp
  ui/dialog_windows/text_trig/text_trig.cpp
  ui/dialog_windows/trig_editor/counters.cpp
  ui/dialog_windows/trig_editor/cuwps.cpp
  ui/dialog_windows/trig_editor/switches.cpp
  ui/dialog_windows/trig_editor/templates.cpp
  ui/dialog_windows/trig_editor/trig_editor.cpp
  ui/dialog_windows/trig_editor/triggers.cpp
  ui/dialog_windows/trig_modify/trig_actions.cpp
  ui/dialog_windows/trig_modify/trig_conditions.cpp
  ui/dialog_windows/trig_modify/trig_general.cpp
  ui/dialog_windows/trig_modify/trig_modify.cpp
  ui/dialog_windows/trig_modify/trig_modify_text.cpp
  ui/dialog_windows/trig_modify/trig_players.cpp
  ui/main_windows/gui_map.cpp
  ui/main_windows/gui_map_printer.cpp
  ui/main_windows/left_bar.cpp
  ui/main_windows/logger_window.cpp
  ui/main_windows/main_plot.cpp
  ui/main_windows/main_toolbar.cpp
  ui/main_windows/maps.cpp
  ui/main_windows/mini_map.cpp
)

add_library(${This} STATIC ${Sources} ${Headers})
