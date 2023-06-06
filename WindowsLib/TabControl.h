#ifndef TABCONTROL_H
#define TABCONTROL_H
#include "WindowControl.h"
#include <CommCtrl.h>

namespace WinLib {

    class TabControl : public WindowControl
    {
        public:
            virtual ~TabControl();

            bool CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, u64 id);

            u32 GetCurSel();
            bool SetCurSel(u32 tabId);

            bool InsertTab(u32 tabId, const std::string & label);
        
            void ShowTab(u32 windowId);
            void HideTab(u32 windowId);

            void SetImageList(HIMAGELIST hImageList);
            void SetMinTabWidth(int minTabWidth);


        protected:


        private:

    };

}

#endif