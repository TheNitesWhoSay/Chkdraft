#ifndef CHKDSTRINGINPUT_H
#define CHKDSTRINGINPUT_H
#include "../../../MappingCoreLib/MappingCore.h"
#include "../../../WindowsLib/WindowsUi.h"

class ChkdStringInputDialog : public WinLib::ClassDialog
{
    public:
        enum class Tab : u32 {
            GameString,
            EditorString
        };
        enum class ExitCode : u32 {
            None = 0,
            Ok,
            Cancel
        };
        enum_t(Result, u32, {
            NoChange = 0,
            GameStringChanged = BIT_0,
            EditorStringChanged = BIT_1,
            BothStringsChanged = GameStringChanged | EditorStringChanged
        });

        virtual ~ChkdStringInputDialog();
        static Result GetChkdString(HWND hParent, ChkdStringPtr & gameString, ChkdStringPtr & editorString, Chk::StringUserFlag stringUser = Chk::StringUserFlag::None, size_t stringUserIndex = 0, size_t stringSubUserIndex = 0);
        
        void ChangeTab(Tab tab);
        void ExitDialog(ExitCode exitCode);

    protected:
        ChkdStringInputDialog();
        Result InternalGetChkdString(HWND hParent, ChkdStringPtr & gameString, ChkdStringPtr & editorString, Chk::StringUserFlag stringUser, size_t stringUserIndex, size_t stringSubUserIndex);
        void UpdateWindowText();
        BOOL DlgNotify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr);
        BOOL DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
        BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

        void DoSize();

    private:
        ChkdStringPtr initialGameString;
        ChkdStringPtr initialEditorString;
        ChkdStringPtr newGameString;
        ChkdStringPtr newEditorString;
        Chk::StringUserFlag stringUser;
        size_t stringUserIndex;
        size_t stringSubUserIndex;

        WinLib::TabControl tabStringTypes;
        Tab currTab;
        ExitCode exitCode;
        
        class GameStringWindow : public WinLib::ClassWindow
        {
            public:
                GameStringWindow(ChkdStringInputDialog & parent);
                virtual ~GameStringWindow();
                bool CreateThis(HWND hParent, u64 windowId);
                void DoSize();
                void ApplyChanges();
                void FocusThis();

            protected:
                void CreateSubWindows(HWND hWnd);
                void ButtonOk();
                void ButtonReset();
                void ButtonCancel();
                virtual void NotifyButtonClicked(int idFrom, HWND hWndFrom);
                virtual void NotifyEditUpdated(int idFrom, HWND hWndFrom);

            private:
                ChkdStringInputDialog & parent;
                WinLib::EditControl editString;
                WinLib::ButtonControl buttonReset;
                WinLib::ButtonControl buttonCancel;
                WinLib::ButtonControl buttonOk;
                WinLib::CheckBoxControl checkNoString;
        };

        class EditorStringWindow : public WinLib::ClassWindow
        {
            public:
                EditorStringWindow(ChkdStringInputDialog & parent);
                virtual ~EditorStringWindow();
                bool CreateThis(HWND hParent, u64 windowId);
                void DoSize();
                void ApplyChanges();
                void FocusThis();

            protected:
                void CreateSubWindows(HWND hWnd);
                void ButtonOk();
                void ButtonReset();
                void ButtonCancel();
                virtual void NotifyButtonClicked(int idFrom, HWND hWndFrom);
                virtual void NotifyEditUpdated(int idFrom, HWND hWndFrom);

            private:
                ChkdStringInputDialog & parent;
                WinLib::EditControl editString;
                WinLib::ButtonControl buttonReset;
                WinLib::ButtonControl buttonCancel;
                WinLib::ButtonControl buttonOk;
                WinLib::CheckBoxControl checkNoString;
        };

        GameStringWindow gameStringWindow;
        EditorStringWindow editorStringWindow;
};

#endif