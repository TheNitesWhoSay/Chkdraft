#ifndef MAINTOOLBAR_H
#define MAINTOOLBAR_H
#include "WindowsUI/WindowsUI.h"
#include "CommonFiles/CommonFiles.h"
#include "PlayerDropdown.h"

class MainToolbar : public ToolbarControl
{
    public:
        DropdownControl layerBox;
        DropdownControl zoomBox;
        PlayerDropdown playerBox;
        DropdownControl terrainBox;

        bool CreateThis(HWND hParent, u32 windowId);
        void AutoSize();

    private:

};

#endif
