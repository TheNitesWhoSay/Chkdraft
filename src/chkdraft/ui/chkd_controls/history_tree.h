#ifndef HISTORYTREE_H
#define HISTORYTREE_H
#include <windows/windows_ui.h>
#include <common_files/common_files.h>
#include <mapping_core/scenario.h>
#include <rarecpp/editor.h>

struct HistEvent
{
    LPARAM lParam = 0;
    std::string text {};
};

struct HistAction
{
    LPARAM lParam = 0;
    HTREEITEM hItem = NULL;
    RareEdit::ActionStatus actionStatus {};
    std::size_t elisionCount = 0;
    std::string text {};
    std::vector<HistAction*> subActions {};
    std::vector<HistEvent> subEvents {};

    void moveUnder(WinLib::TreeViewControl & treeView, HistAction & newItemParent);
};

class HistoryTree : public WinLib::TreeViewControl
{
    public:
        virtual ~HistoryTree();
        void createRoot();
        HistAction* InsertAction(std::size_t actionIndex, const RareEdit::RenderAction<DescriptorIndex> & action, HTREEITEM parent);
        void InsertAction(std::size_t actionIndex, const RareEdit::RenderAction<DescriptorIndex> & action);
        std::size_t RebuildHistoryTree(); // Returns total byte count
        void RefreshActionHeaders(std::optional<std::size_t> excludeIndex = std::nullopt);
        
        std::unordered_map<std::size_t, HistAction> actionTree {};

    protected:
        LRESULT ControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        HTREEITEM hHistoryRoot = NULL;
};

#endif