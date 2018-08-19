#ifndef CUWPINPUT_H
#define CUWPINPUT_H
#include "../../../WindowsLib/WindowsUi.h"
#include "../../../MappingCoreLib/MappingCore.h"

class CuwpInputDialog : public WinLib::ClassDialog
{
    public:
        virtual ~CuwpInputDialog();
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

        WinLib::EditControl editHitpointPercent, editManaPercent, editShieldPercent, editResources, editHanger;
        WinLib::CheckBoxControl checkInvincible, checkBurrowed, checkLifted, checkHallucinated, checkCloaked;
};

#endif