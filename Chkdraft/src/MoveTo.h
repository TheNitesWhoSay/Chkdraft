#ifndef MOVETODIALOG_H
#define MOVETODIALOG_H
#include "CommonFiles/CommonFiles.h"
#include "MappingCore/MappingCore.h"
#include "WindowsUI/WindowsUI.h"

template <typename indexType> // Allowed types: u8, s8, u16, s16, u32, s32/int
class MoveToDialog : public ClassDialog
{
    public:
        virtual ~MoveToDialog();
        static bool GetIndex(indexType &index, HWND hParent);

    protected:
        MoveToDialog();
        bool InternalGetMoveTo(indexType &index, HWND hParent);
        BOOL DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
        BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        bool gotIndex;
        indexType indexMovedTo;
        EditControl editMoveTo;
};

#endif