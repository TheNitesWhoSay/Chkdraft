#ifndef SUGGESTIONS_H
#define SUGGESTIONS_H
#include "../../../WindowsLib/WindowsUi.h"
#include <string>
#include <vector>

struct SuggestionItem
{
    std::optional<uint32_t> data; // If present, indicates the data that should be set in the target field
    std::string str; // The displayed string value, if data is not present then str gives the value to be set in the target field
};

class Suggestions : public WinLib::ClassWindow
{
    public:
        Suggestions();
        virtual ~Suggestions();
        bool CreateThis(HWND hParent, int x, int y, int width, int height);
        void ClearItems();

        void AddItems(const std::vector<SuggestionItem> & items); // Adds items to the stored list but does not yet display them
        int AddItem(const SuggestionItem & item); // Adds an item to the stored list but does not yet display it
        void SetItems(); // Sets all the items in the stored list to the display
        void SetItems(const std::vector<SuggestionItem> & strings); // Adds items to the stored list then sets all stored items up for display
        void Show();
        void Hide();
        bool SuggestIndex(int index);
        void SuggestNear(const std::string & str);
        bool HasSelection();
        void SelectFirst();
        void ArrowUp();
        void ArrowDown();
        std::string Take();
        bool HasItems();
        bool IsShown();

    protected:
        void DoSize();
        void EraseBackground(HDC hDC);
        void SuggestFirstStartingWith(const std::string & str);

        virtual LRESULT Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr);
        virtual LRESULT Command(HWND hWnd, WPARAM wParam, LPARAM lParam);
        virtual LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        HWND suggestParent;
        WinLib::ListBoxControl listSuggestions;
        bool isShown;
        bool isActive;

        int KeyDown(WPARAM wParam);
        std::list<SuggestionItem> items;
};

#endif