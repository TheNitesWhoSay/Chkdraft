#ifndef BRIEFINGTEMPLATES_H
#define BRIEFINGTEMPLATES_H
#include "../../../CommonFiles/CommonFiles.h"
#include "../../../../WindowsLib/WindowsUi.h"
#include "../../../../MappingCoreLib/MappingCore.h"

class BriefingTemplatesWindow : public WinLib::ClassWindow
{
    public:
        virtual ~BriefingTemplatesWindow();
        bool CreateThis(HWND hParent, u64 windowId);
        bool DestroyThis();
        void RefreshWindow();
        void DoSize();

    protected:
        void CreateSubWindows(HWND hWnd);

    private:
};

#endif