#ifndef SUGGESTIONS_H
#define SUGGESTIONS_H
#include "../../../WindowsLib/WindowsUi.h"
#include <string>
#include <vector>

class Suggestions : public WinLib::ClassWindow
{
    public:
        Suggestions();
        virtual ~Suggestions();
        bool CreateThis(HWND hParent, int x, int y, int width, int height);
        void ClearStrings();

        void AddStrings(const std::vector<std::string> &strings);
        void AddString(const std::string &string); // Adds a string to the stored list but does not yet display it
        void SetStrings(); // Sets all the strings in the stored list to the display
        void SetStrings(const std::vector<std::string> &strings);
        void Show();
        void Hide();
        void SuggestNear(const std::string &str);
        void ArrowUp();
        void ArrowDown();
        std::string Take();

    protected:
        void DoSize();
        void EraseBackground(HDC hDC);
        void SuggestFirstStartingWith(const std::string &str);

        virtual LRESULT Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr);
        virtual LRESULT Command(HWND hWnd, WPARAM wParam, LPARAM lParam);
        virtual LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        HWND suggestParent;
        WinLib::ListBoxControl listSuggestions;
        bool isShown;

        void KeyDown(WPARAM wParam);
        std::list<std::string> strList;
};

#endif