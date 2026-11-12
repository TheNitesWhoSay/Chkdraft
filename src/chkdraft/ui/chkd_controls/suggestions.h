#ifndef SUGGESTIONS_H
#define SUGGESTIONS_H
#include <windows/windows_ui.h>
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

        enum class Type
        {
            FreeformText, // Scroll towards closest text match, if there's no exact match, display "New String: ..." as the current suggestion
                          // If text is numeric, display "String Index: ##" *above* the current suggestion
            Amount, // If text is numeric, display "Amount: ##" as the current suggestion, else use closest text match as the current suggestion
            FixedSet // Display closest text match as the current suggestion, if numeric, display "Raw Value: ##" as the current suggestion
        };

        class Toolbar : public WinLib::ToolbarControl
        {
            public:
                virtual ~Toolbar();
                bool CreateThis(HWND hParent, std::uint64_t windowId);
                void AutoSize();
                void EnableContextualPropertiesButton(const std::string & hoverText);
                void DisableContextualPropertiesButton();

            private:
                icux::uistring contextualProperties = icux::toUistring("(Disabled)");
                icux::uistring cancel = icux::toUistring("Cancel (Esc)");
        };

        Suggestions();
        virtual ~Suggestions();
        bool CreateThis(HWND hParent, int x, int y, int width, int height);
        void ClearItems();

        void AddItems(const std::vector<SuggestionItem> & items); // Adds items to the stored list but does not yet display them
        int AddItem(const SuggestionItem & item); // Adds an item to the stored list but does not yet display it
        void SetItems(); // Sets all the items in the stored list to the display
        void SetItems(const std::vector<SuggestionItem> & strings); // Adds items to the stored list then sets all stored items up for display
        void Show(Type suggestionType, std::function<std::string(std::uint32_t)> numberDecorator, std::function<std::string(const std::string &)> stringDecorator = {});
        void Hide();
        void EnableContextualPropertiesButton(const std::string & hoverText);
        void DisableContextualPropertiesButton();
        bool SuggestIndex(int index);
        void SuggestNear(const std::string & str);
        void OpenContextualProperties();
        void CancelSuggestion();
        bool HasSelection();
        void SelectFirst();
        void ArrowUp();
        void ArrowDown();
        SuggestionItem Take();
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
        Toolbar toolbar;
        std::optional<WinLib::DeviceContext> listDc;
        bool isShown;
        bool isActive;
        Type suggestionType;
        int currentNumberItemIndex;
        int currentTextItemIndex;
        bool modifying;

        int KeyDown(WPARAM wParam);
        std::list<SuggestionItem> items;
        SuggestionItem currNumberItem;
        SuggestionItem currTextItem;
        std::string undecoratedCurrentText;
        std::function<std::string(std::uint32_t)> currentNumberDecorator;
        std::function<std::string(const std::string &)> currentStringDecorator;
};

namespace decorate
{
    std::string unitId(std::uint32_t number);
    std::string locationId(std::uint32_t number);
    std::string player(std::uint32_t number);
    std::string numericComparison(std::uint32_t number);
    std::string resourceType(std::uint32_t number);
    std::string scoreType(std::uint32_t number);
    std::string switchIndex(std::uint32_t number);
    std::string switchState(std::uint32_t number);
    std::string conditionType(std::uint32_t number);

    std::string stringIndex(std::uint32_t number);
    std::string string(const std::string & str);
    std::string numUnits(std::uint32_t number);
    std::string cuwp(std::uint32_t number);
    std::string stateMod(std::uint32_t number);
    std::string percent(std::uint32_t number);
    std::string order(std::uint32_t number);
    std::string soundStringIndex(std::uint32_t number);
    std::string soundPathString(const std::string & str);
    std::string script(std::uint32_t number);
    std::string allyState(std::uint32_t number);
    std::string numericMod(std::uint32_t number);
    std::string switchMod(std::uint32_t number);
    std::string actionType(std::uint32_t number);

    std::string briefingSlot(std::uint32_t number);
    std::string briefingActionType(std::uint32_t number);
}

#endif