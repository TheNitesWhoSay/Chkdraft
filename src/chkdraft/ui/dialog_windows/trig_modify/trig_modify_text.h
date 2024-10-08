#ifndef TRIG_MODIFY_TEXT
#define TRIG_MODIFY_TEXT
#include <common_files/common_files.h>
#include <windows/windows_ui.h>
#include <mapping_core/mapping_core.h>

class TrigModifyTextWindow : public WinLib::ClassWindow
{
    public:
        TrigModifyTextWindow();
        virtual ~TrigModifyTextWindow();
        bool CreateThis(HWND hParent, u64 windowId);
        bool DestroyThis();
        void SetTrigIndex(u32 trigIndex);
        void RefreshWindow(u32 trigIndex);
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
        u32 trigIndex;
        std::string trigText;
        bool autoCompile;
};

#endif