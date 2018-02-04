#ifndef TEMPLATES_H
#define TEMPLATES_H
#include "CommonFiles/CommonFiles.h"
#include "MappingCore/MappingCore.h"
#include "WindowsUI/WindowsUI.h"

class TemplatesWindow : public ClassWindow
{
    public:
        virtual ~TemplatesWindow();
        bool CreateThis(HWND hParent, u32 windowId);
        bool DestroyThis();
        void RefreshWindow();
        void DoSize();

    protected:
        void CreateSubWindows(HWND hWnd);

    private:
};

#endif