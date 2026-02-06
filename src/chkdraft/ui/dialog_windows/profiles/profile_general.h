#ifndef PROFILEGENERAL_H
#define PROFILEGENERAL_H
#include <chkdraft/mapping/chkd_profiles.h>
#include <common_files/common_files.h>
#include <windows/windows_ui.h>

class ProfileGeneralWindow : public WinLib::ClassWindow
{
public:
    ProfileGeneralWindow(std::string & editProfileName) : editProfileName(editProfileName) {}
    bool CreateThis(HWND hParent, u64 windowId);
    void DestroyThis();
    void DisableRemasteredGroup();
    void EnableRemasteredGroup();
    void DisableClassicGroup();
    void EnableClassicGroup();
    void RefreshWindow();

protected:
    void ToggleUseRemastered();
    void RemoveDataFilePath(bool isRemastered, const std::string & path);
    void AddOrReplaceDataFilePath(bool isRemastered, const std::string & oldPath, const std::string & newPath);
    void BrowseScrPath();
    void BrowseCascPath();
    void BrowseScPath();
    void BrowseStarDatPath();
    void BrowseBrooDatPath();
    void BrowsePatchRtDatPath();
    void ToggleUseDefaultDeathTableStart();
    void ToggleUseAddressesForMemory();
    void ScrSkinSelUpdated();
    void ScSkinSelUpdated();
    void LogLevelSelUpdated();

    void NotifyButtonClicked(int idFrom, HWND hWndFrom) override; // Sent when a button or checkbox is clicked
    void NotifyEditUpdated(int idFrom, HWND hWndFrom) override; // Sent when edit text changes, before redraw
    void NotifyEditFocusLost(int idFrom, HWND hWndFrom) override; // Sent when focus changes or the window is hidden
    void NotifyComboSelChanged(int idFrom, HWND hWndFrom) override; // Sent when combo is changed by selection
    void NotifyComboEditUpdated(int idFrom, HWND hWndFrom) override; // Sent when combo is changed by text edits
    void NotifyComboEditFocusLost(int idFrom, HWND hWndFrom) override; // Sent when focus changes or the window is hidden
    void NotifyWindowHidden() override; // Sent when the window is hidden
    LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
    void CreateSubWindows(HWND hWnd);
    ChkdProfile & getEditProfile();
    std::string & editProfileName;

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

    void CheckReplaceScrPath();
    void CheckReplaceCascPath();
    void CheckReplaceScPath();
    void CheckReplaceStarDatPath();
    void CheckReplaceBrooDatPath();
    void CheckReplacePatchRtPath();
    void CheckReplaceLogLevel();
    void CheckReplaceMaxHistMemory();
    void CheckReplaceMaxHistActions();
    void CheckReplaceDeathTableStart();

    void CheckEditUpdates();

    bool possibleScrPathUpdate = false;
    bool possibleCascPathUpdate = false;
    bool possibleScPathUpdate = false;
    bool possibleStarDatPathUpdate = false;
    bool possibleBrooDatPathUpdate = false;
    bool possiblePatchRtPathUpdate = false;
    bool possibleLogLevelUpdate = false;
    bool possibleMaxHistMemoryUpdate = false;
    bool possibleMaxHistActionsUpdate = false;
    bool possibleDeathTableStartUpdate = false;

    bool refreshing = false;
};

#endif