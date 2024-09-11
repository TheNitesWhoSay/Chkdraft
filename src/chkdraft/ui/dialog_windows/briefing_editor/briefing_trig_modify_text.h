#ifndef BRIEFINGTRIGMODIFYTEXT_H
#define BRIEFINGTRIGMODIFYTEXT_H
#include <common_files/common_files.h>
#include <windows/windows_ui.h>
#include <mapping_core/mapping_core.h>

class BriefingTrigModifyTextWindow : public WinLib::ClassWindow
{
    public:
        BriefingTrigModifyTextWindow();
        virtual ~BriefingTrigModifyTextWindow();
        bool CreateThis(HWND hParent, u64 windowId);
        bool DestroyThis();
        void SetTrigIndex(u32 briefingTrigIndex);
        void RefreshWindow(u32 briefingTrigIndex);
        void DoSize();
        void ParentHidden();

    protected:
        void CreateSubWindows(HWND hWnd);
        bool unsavedChanges();
        void Compile(bool silent, bool saveAfter);
        bool CompileEditText(std::string & newText);
        void OnLeave();
        LRESULT Command(HWND hWnd, WPARAM wParam, LPARAM lParam);
        LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
        
    private:
        WinLib::EditControl editText;
        WinLib::CheckBoxControl checkAutoCompile;
        WinLib::ButtonControl buttonCompile, buttonCompileAndSave, buttonReload;
        u32 briefingTrigIndex;
        std::string briefingTrigText;
        bool autoCompile;
};

#endif