#ifndef SELECTPROFILE_H
#define SELECTPROFILE_H
#include <windows/windows_ui.h>

class SelectProfile : public WinLib::ClassDialog
{
    public:
        virtual ~SelectProfile();
        void CreateThis(HWND hParent);

    protected:
        BOOL DlgNotify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr) override;
        BOOL DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam) override;
        BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) override;
        void LoadProfile(const std::string & profileName);
        void FindProfile();
        void EditProfiles();
        void ExitPressed();

    private:
        WinLib::ListBoxControl listBoxProfiles;
};

#endif