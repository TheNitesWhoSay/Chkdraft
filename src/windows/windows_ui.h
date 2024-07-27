#ifndef WINDOWSUI_H
#define WINDOWSUI_H

/**
    The WindowsLib acts as a wrapper between windows functionality and the other projects in this solution.
    All public methods within WindowsLib take UTF-8 strings as input, and returns UTF-8 strings as output.
    Internally much is converted and handled as UTF-16 strings.
*/

// Controls
#include "button_control.h"
#include "check_box_control.h"
#include "context_menu.h"
#include "dropdown_control.h"
#include "drag_list_control.h"
#include "edit_control.h"
#include "grid_view.h"
#include "group_box_control.h"
#include "list_box_control.h"
#include "list_view_control.h"
#include "radio_button_control.h"
#include "rich_edit_control.h"
#include "status_control.h"
#include "tab_control.h"
#include "text_control.h"
#include "toolbar_control.h"
#include "tree_view_control.h"
#include "window_control.h"


// Menus
#include "window_menu.h"

// Windows
#include "class_window.h"
#include "class_dialog.h"
#include "mdi_client.h"

// Misc
#include "popup.h"
#include "file_system.h"
#include "device_context.h"
#include "resource_manager.h"

#endif