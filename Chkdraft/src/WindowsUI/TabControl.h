#ifndef TABCONTROL_H
#define TABCONTROL_H
#include "WindowControl.h"

class TabControl : public WindowControl
{
    public:

        bool CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, u32 id);

        u32 GetCurSel();
        bool SetCurSel(u32 tabId);

        bool InsertTab(u32 tabId, const char* label);
        
        void ShowTab(u32 windowId);
        void HideTab(u32 windowId);

        void SetImageList(HIMAGELIST hImageList);
        void SetMinTabWidth(int minTabWidth);


    protected:


    private:

};

#endif