#ifndef CUWPINPUT_H
#define CUWPINPUT_H
#include <windows/windows_ui.h>
#include <mapping_core/mapping_core.h>

class CuwpInputDialog : public WinLib::ClassDialog
{
    public:
        virtual ~CuwpInputDialog();
        static bool GetCuwp(Chk::Cuwp & cuwp, const Chk::Cuwp & initialCuwp, HWND hParent);

    protected:
        CuwpInputDialog();
        void ClearStoredCuwps();
        bool InternalGetCuwp(Chk::Cuwp & cuwp, const Chk::Cuwp & initialCuwp, HWND hParent);
        BOOL DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
        BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        bool gotCuwp;
        Chk::Cuwp newCuwp;
        Chk::Cuwp initialCuwp;

        WinLib::EditControl editHitpointPercent, editManaPercent, editShieldPercent, editResources, editHangar;
        WinLib::CheckBoxControl checkInvincible, checkBurrowed, checkLifted, checkHallucinated, checkCloaked;
};

#endif