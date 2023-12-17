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
        WinLib::DropdownControl brushWidth;
        WinLib::DropdownControl brushHeight;
        WinLib::CheckBoxControl checkTerrain;
        WinLib::CheckBoxControl checkDoodads;
        WinLib::CheckBoxControl checkSprites;
        WinLib::CheckBoxControl checkUnits;
        WinLib::CheckBoxControl checkFog;

        virtual ~MainToolbar();
        bool CreateThis(HWND hParent, u64 windowId);
        void AutoSize();

    private:

};

#endif