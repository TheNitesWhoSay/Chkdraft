#ifndef WINDOWSUI_H
#define WINDOWSUI_H

/**
    The WindowsLib acts as a wrapper between windows functionality and the other projects in this solution.
    All public methods within WindowsLib take UTF-8 strings as input, and returns UTF-8 strings as output.
    Internally much is converted and handled as UTF-16 strings.
*/

// Controls
#include "ButtonControl.h"
#include "CheckBoxControl.h"
#include "DropdownControl.h"
#include "DragListControl.h"
#include "EditControl.h"
#include "GridView.h"
#include "GroupBoxControl.h"
#include "ListBoxControl.h"
#include "ListViewControl.h"
#include "RadioButtonControl.h"
#include "RichEditControl.h"
#include "StatusControl.h"
#include "TabControl.h"
#include "TextControl.h"
#include "ToolbarControl.h"
#include "TreeViewControl.h"
#include "WindowControl.h"


// Menus
#include "WindowMenu.h"

// Windows
#include "ClassWindow.h"
#include "ClassDialog.h"
#include "MdiClient.h"

// Misc
#include "Popup.h"
#include "FileSystem.h"
#include "DeviceContext.h"
#include "ResourceManager.h"

#endif