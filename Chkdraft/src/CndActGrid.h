#ifndef CNDACTGRID_H
#define CNDACTGRID_H
#include "WindowsUI/WindowsUI.h"
#include "CommonFiles/CommonFiles.h"
#include "MappingCore/MappingCore.h"
#include "Suggestions.h"

class ICndActGridUser // ~Interface
{
    public:
        virtual void CndActEnableToggled(u8 cndActId) = 0;
        virtual void CutSelection() = 0;
        virtual void CopySelection() = 0;
        virtual void Paste() = 0;
};

enum class ArgumentEnd { TwoCharLineBreak, OneCharLineBreak, Tab, StringEnd };

class CndActGrid : public GridViewControl
{
    public:
        CheckBoxControl checkEnabled[64];

        CndActGrid(ICndActGridUser &user, int numUsedRows);
        bool CreateThis(HWND hParent, int x, int y, int width, int height, u32 id);
        void SetEnabledCheck(u8 cndActId, bool enabled);

        Suggestions &GetSuggestions();
        bool HasUpDownedThisEdit();

        virtual bool allowKeyNavDuringEdit(GVKey key); // Checks if navigation is allowed by the specified key
        virtual bool allowSel(int x, int y);
        virtual bool allowSel(int xStart, int xEnd, int yStart, int yEnd);
        virtual void adjustSel(int &xStart, int &xEnd, int &yStart, int &yEnd);
        virtual void CellClicked(int x, int y);

        size_t FindArgEnd(std::string &str, size_t argStart, ArgumentEnd &outEndsBy);
        void EscapeString(std::string &str);
        bool BuildSelectionString(std::string &str);
        virtual void RedrawThis();

    protected:
        bool CreateSubWindows(HWND hWnd);
        virtual void StartEditing(int xClick, int yClick, char initChar);
        virtual void KeyDown(WPARAM wParam);
        virtual void EditTextChanged(std::string &str);

        virtual LRESULT Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr);
        virtual LRESULT Command(HWND hWnd, WPARAM wParam, LPARAM lParam);
        virtual LRESULT ControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        Suggestions suggestions;
        ICndActGridUser& user;
        bool hasUpDownedThisEdit;
        bool startedByClick;
        bool ignoreChange;
        int numUsedRows;

        BOOL ProcessTrack(NMHEADER* nmHeader);
        BOOL ProcessTrackW(NMHEADERW* nmHeader);

        CndActGrid(); // Disallow ctor
};

#endif