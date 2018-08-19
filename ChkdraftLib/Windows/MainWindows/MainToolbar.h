#ifndef MAINTOOLBAR_H
#define MAINTOOLBAR_H
#include "../../CommonFiles/CommonFiles.h"
#include "../../../WindowsLib/WindowsUI.h"
#include "../ChkdControls/PlayerDropdown.h"

class MainToolbar : public WinLib::ToolbarControl
{
    public:
        WinLib::DropdownControl layerBox;
        WinLib::DropdownControl zoomBox;
        PlayerDropdown playerBox;
        WinLib::DropdownControl terrainBox;

        virtual ~MainToolbar();
        bool CreateThis(HWND hParent, u32 windowId);
        void AutoSize();

    private:

};

#endif