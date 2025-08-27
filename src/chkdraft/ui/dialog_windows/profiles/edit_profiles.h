#ifndef EDITPROFILES_H
#define EDITPROFILES_H
#include <common_files/common_files.h>
#include <string>
#include <windows/windows_ui.h>
#include <ui/dialog_windows/profiles/custom_dat_files.h>
#include <ui/dialog_windows/profiles/profile_editor_settings.h>
#include <ui/dialog_windows/profiles/profile_general.h>

struct ChkdProfile;

class EditProfilesWindow : public WinLib::ClassDialog
{
public:
    enum class Tab : u32 {
        ProfilePaths,
        CustomDatFiles,
        EditorSettings,
    };

    bool CreateThis(HWND hParent);
    void DestroyThis();
    void ChangeTab(Tab tab);
    void RefreshWindow();

protected:
    BOOL DlgNotify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr);
    BOOL DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
    WinLib::GroupBoxControl groupProfiles;
    WinLib::ListBoxControl listProfiles;
    WinLib::ButtonControl buttonAddProfile;
    WinLib::ButtonControl buttonCloneProfile;
    WinLib::ButtonControl buttonFindProfile;
    WinLib::ButtonControl buttonRenameProfile;
    WinLib::ButtonControl buttonDeleteProfile;
    WinLib::CheckBoxControl checkIsDefaultProfile;
    WinLib::CheckBoxControl checkAutoLoadOnStart;

    Tab currTab = Tab::ProfilePaths;
    WinLib::TabControl tabs;

    ProfileGeneralWindow profileGeneralWindow;
    CustomDatFilesWindow customDatFilesWindow;
    ProfileEditorSettingsWindow editorSettingsWindow;

    std::string editProfileName {};
    ChkdProfile & getEditProfile();
};

#endif