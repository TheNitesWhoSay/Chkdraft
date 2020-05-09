#ifndef TEMPLATES_H
#define TEMPLATES_H
#include "../../../CommonFiles/CommonFiles.h"
#include "../../../../WindowsLib/WindowsUi.h"
#include "../../../../MappingCoreLib/MappingCore.h"

class TemplatesWindow : public WinLib::ClassWindow
{
    public:
        virtual ~TemplatesWindow();
        bool CreateThis(HWND hParent, u64 windowId);
        bool DestroyThis();
        void RefreshWindow();
        void DoSize();

    protected:
        void CreateSubWindows(HWND hWnd);

    private:
};

#endif