#ifndef CUWPINPUT_H
#define CUWPINPUT_H
#include "MappingCore/MappingCore.h"
#include "WindowsUI/WindowsUI.h"

class CuwpInputDialog : public ClassDialog
{
    public:
        static bool GetCuwp(ChkCuwp &cuwp, const ChkCuwp &initialCuwp, HWND hParent);

    protected:
        CuwpInputDialog();
        void ClearStoredCuwps();
        bool InternalGetCuwp(ChkCuwp &cuwp, const ChkCuwp &initialCuwp, HWND hParent);
        BOOL DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
        BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        bool gotCuwp;
        ChkCuwp newCuwp;
        ChkCuwp initialCuwp;

        EditControl editHitpointPercent, editManaPercent, editShieldPercent, editResources, editHanger;
        CheckBoxControl checkInvincible, checkBurrowed, checkLifted, checkHallucinated, checkCloaked;
};

#endif