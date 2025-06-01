#ifndef HISTORYTREE_H
#define HISTORYTREE_H
#include <windows/windows_ui.h>
#include <common_files/common_files.h>
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
        bool AddThis(HWND hTree, HTREEITEM hParent);
        HistAction* InsertAction(std::size_t actionIndex, const RareEdit::Action & action, HTREEITEM parent);
        void InsertAction(std::size_t actionIndex, const RareEdit::Action & action);
        void RebuildHistoryTree();
        void RefreshActionHeaders(std::optional<std::size_t> excludeIndex = std::nullopt);

    private:
        HTREEITEM hHistoryRoot = NULL;
        std::unordered_map<std::size_t, HistAction> actionTree {};
};

#endif