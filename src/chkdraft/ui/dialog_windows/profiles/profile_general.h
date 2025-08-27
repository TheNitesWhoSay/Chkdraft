#ifndef PROFILEGENERAL_H
#define PROFILEGENERAL_H
#include <common_files/common_files.h>
#include <windows/windows_ui.h>

class ProfileGeneralWindow : public WinLib::ClassWindow
{
public:
    bool CreateThis(HWND hParent, u64 windowId);
    void DestroyThis();
    void RefreshWindow();

protected:
    LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
    void CreateSubWindows(HWND hWnd);

    WinLib::CheckBoxControl checkUseRemastered;

    WinLib::GroupBoxControl groupRemastered;
    WinLib::TextControl textScrPath;
    WinLib::EditControl editScrPath;
    WinLib::ButtonControl browseScrPath;
    WinLib::TextControl textCascPath;
    WinLib::EditControl editCascPath;
    WinLib::ButtonControl browseCascPath;
    WinLib::TextControl textScrDefaultSkin;
    WinLib::DropdownControl dropScrDefaultSkin;
    
    WinLib::GroupBoxControl groupClassic;
    WinLib::TextControl textScPath;
    WinLib::EditControl editScPath;
    WinLib::ButtonControl browseScPath;
    WinLib::TextControl textStarDatPath;
    WinLib::EditControl editStarDatPath;
    WinLib::ButtonControl browseStarDatPath;
    WinLib::TextControl textBrooDatPath;
    WinLib::EditControl editBrooDatPath;
    WinLib::ButtonControl browseBrooDatPath;
    WinLib::TextControl textPatchRtDatPath;
    WinLib::EditControl editPatchRtDatPath;
    WinLib::ButtonControl browsePatchRtDatPath;
    WinLib::TextControl textScDefaultSkin;
    WinLib::DropdownControl dropScDefaultSkin;

    WinLib::GroupBoxControl groupMisc;
    WinLib::TextControl textLogLevel;
    WinLib::DropdownControl dropLogLevel;
    WinLib::TextControl textDeathTableStart;
    WinLib::EditControl editDeathTableStart;
    WinLib::CheckBoxControl checkUseDefaultDeathTableStart;
    WinLib::CheckBoxControl checkUseAddressesForMemory;
    WinLib::TextControl textMaxHistMemoryUseMb;
    WinLib::EditControl editMaxHistMemoryUseMb;
    WinLib::TextControl textMaxHistActions;
    WinLib::EditControl editMaxHistActions;

};

#endif