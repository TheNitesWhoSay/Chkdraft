#ifndef FORCES_H
#define FORCES_H
#include <common_files/common_files.h>
#include <windows/windows_ui.h>
#include <mapping_core/mapping_core.h>

class ForcesWindow : public WinLib::ClassWindow
{
    public:
        ForcesWindow();
        virtual ~ForcesWindow();
        bool CreateThis(HWND hParent, u64 windowId);
        bool DestroyThis();
        void RefreshWindow();
        LRESULT Command(HWND hWnd, WPARAM wParam, LPARAM lParam);
        LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        void CheckReplaceForceName(Chk::Force force);

        bool possibleForceNameUpdate[4];
        u8 playerBeingDragged;

        WinLib::EditControl editForceName[4];
        WinLib::GroupBoxControl groupForce[4];
        WinLib::TextControl textAboutForces;
        WinLib::CheckBoxControl checkAllied[4];
        WinLib::CheckBoxControl checkSharedVision[4];
        WinLib::CheckBoxControl checkRandomizeStart[4];
        WinLib::CheckBoxControl checkAlliedVictory[4];
        WinLib::DragListControl dragForces[4];
};

#endif