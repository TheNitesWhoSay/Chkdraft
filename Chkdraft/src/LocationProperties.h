#ifndef LOCATIONPROPERITES_H
#define LOCATIONPROPERITES_H
#include "CommonFiles/CommonFiles.h"
#include "MappingCore/MappingCore.h"
#include "WindowsUI/WindowsUI.h"

class LocationWindow : public ClassDialog
{
    public:
        LocationWindow();
        bool CreateThis(HWND hParent);
        bool DestroyThis();
        void RefreshLocationInfo();

    protected:
        void RefreshLocationElevationFlags(ChkLocation* locRef, HWND hWnd);
        BOOL DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
        BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        bool initializing;
        u32 preservedStat;
        u16 locProcLocIndex;

        CheckBoxControl checkUseExtended;
        EditControl editLocName;
        EditControl editLocLeft;
        EditControl editLocTop;
        EditControl editLocRight;
        EditControl editLocBottom;
        EditControl editRawFlags;
};

#endif
