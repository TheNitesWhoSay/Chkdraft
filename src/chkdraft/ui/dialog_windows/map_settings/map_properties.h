#ifndef MAPPROPERTIES_H
#define MAPPROPERTIES_H
#include <common_files/common_files.h>
#include <windows/windows_ui.h>
#include <mapping_core/mapping_core.h>

class MapPropertiesWindow : public WinLib::ClassWindow
{
    public:
        MapPropertiesWindow();
        virtual ~MapPropertiesWindow();
        bool CreateThis(HWND hParent, u64 windowId);
        bool DestroyThis();
        void RefreshWindow();

    protected:
        LRESULT Command(HWND hWnd, WPARAM wParam, LPARAM lParam);
        LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
        
        virtual void NotifyButtonClicked(int idFrom, HWND hWndFrom); // Sent when a button or checkbox is clicked

    private:
        bool refreshing;
        void CheckReplaceMapTitle();
        void CheckReplaceMapDescription();

        bool possibleTitleUpdate;
        bool possibleDescriptionUpdate;

        WinLib::ButtonControl buttonApply;
        WinLib::EditControl editMapTitle;
        WinLib::EditControl editMapDescription;
        WinLib::EditControl editMapWidth;
        WinLib::EditControl editMapHeight;
        WinLib::GroupBoxControl groupMapPlayers[12];
        WinLib::TextControl textMapTitle;
        WinLib::TextControl textMapDescription;
        WinLib::TextControl textMapTileset;
        WinLib::DropdownControl dropMapTileset;
        WinLib::TextControl textNewMapTerrain;
        WinLib::DropdownControl dropNewMapTerrain;
        WinLib::TextControl textNewMapWidth;
        WinLib::TextControl textNewMapHeight;
        WinLib::TextControl textPlayerOwner[12];
        WinLib::DropdownControl dropPlayerOwner[12];
        WinLib::TextControl textPlayerRace[12];
        WinLib::DropdownControl dropPlayerRaces[12];
        WinLib::TextControl textPlayerColor[8];
        WinLib::DropdownControl dropPlayerColor[8];
        Chk::Rgb lastCustomColor[8];
        WinLib::ButtonControl buttonColorProperties[8];
};

#endif