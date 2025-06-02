#include "history_tree.h"
#include "chkdraft/chkdraft.h"

void HistAction::moveUnder(WinLib::TreeViewControl & treeView, HistAction & newItemParent)
{
    HTREEITEM oldItem = this->hItem;
    this->hItem = treeView.InsertTreeItem(newItemParent.hItem, this->text, this->lParam);
    for ( auto & child : this->subActions )
        child->moveUnder(treeView, *this);

    if ( !this->subEvents.empty() )
    {
        for ( const auto & child : this->subEvents )
            treeView.InsertTreeItem(this->hItem, child.text, child.lParam);
    
        treeView.EmptySubTree(oldItem);
    }

    TreeView_DeleteItem(treeView.getHandle(), oldItem);
    if ( std::find(newItemParent.subActions.begin(), newItemParent.subActions.end(), this) == newItemParent.subActions.end() )
        newItemParent.subActions.push_back(this);
}

HistoryTree::~HistoryTree()
{

}

bool HistoryTree::AddThis(HWND hTree, HTREEITEM hParent)
{
    hHistoryRoot = hParent;
    return TreeViewControl::SetHandle(hTree);
}

std::string getSizeString(std::size_t size)
{
    if ( size >= 1024*1024*10 )
        return std::to_string(size/(1024*1024)) + "mb"; // Show MB
    else if ( size >= 1024*10 )
        return std::to_string(size/1024) + "kb"; // Show KB
    else
        return std::to_string(size) + " bytes";
}

std::string getActionText(std::size_t actionIndex, const RareEdit::RenderAction<DescriptorIndex> & action)
{
    if ( action.isElisionMarker() )
        return "[" + std::to_string((int)actionIndex-(int)action.elisionCount) + "-" + std::to_string(actionIndex) + "] Elided";
    else
    {
        std::string actionText = "[" + std::to_string(actionIndex) + "] ";
        switch ( action.actionStatus )
        {
            case RareEdit::ActionStatus::Undoable:
                actionText += "Undoable";
                break;
            case RareEdit::ActionStatus::ElidedRedo:
                actionText += "Elided";
                break;
            case RareEdit::ActionStatus::Redoable:
                actionText += "Redoable";
                break;
        }
        if ( action.userData.descriptorIndex != ActionDescriptor::None )
        {
            actionText += " - ";
            switch ( action.userData.descriptorIndex )
            {
                case ActionDescriptor::ClearUnitSel: actionText += "Clear Unit Selection"; break;
                case ActionDescriptor::UpdateUnitSel: actionText += "Update Unit Selection"; break;
                case ActionDescriptor::CreateUnit: actionText += "Create Unit"; break;
                case ActionDescriptor::PasteUnits: actionText += "Paste Unit"; break;
                case ActionDescriptor::BrushIsom: actionText += "Brush Isom"; break;
                default: actionText += "TODO: " + std::to_string(std::underlying_type_t<ActionDescriptor>(action.userData.descriptorIndex)); break;
            }
        }
        actionText += " (" + getSizeString(action.byteCount) + ")";
        return actionText;
    }
}

HistAction* HistoryTree::InsertAction(std::size_t actionIndex, const RareEdit::RenderAction<DescriptorIndex> & action, HTREEITEM parent)
{
    if ( (u32)actionIndex <= TreeDataPortion )
    {
        if ( action.actionStatus != RareEdit::ActionStatus::ElidedRedo && action.changeEvents.size() == 0 )
            return nullptr; // Don't insert incomplete actions

        std::string text = getActionText(actionIndex, action);
        HTREEITEM hActionRoot = InsertTreeItem(parent, text, actionIndex|TreeTypeAction, !action.isElisionMarker());
        HistAction histAction {
            .lParam = LPARAM(actionIndex)|TreeTypeAction,
            .hItem = hActionRoot,
            .actionStatus = action.actionStatus,
            .elisionCount = action.elisionCount,
            .text = text
        };

        const auto & changeEvents = action.changeEvents;
        for ( std::size_t i=0; i<changeEvents.size(); ++i )
        {
            LPARAM lParam = LPARAM(i)|TreeTypeEvent;
            std::size_t limit = 2;
            if ( i > limit )
            {
                std::string text = "  (" + std::to_string(int(changeEvents.size())-limit) + " additional events omitted)\n";
                InsertTreeItem(hActionRoot, text, lParam);
                histAction.subEvents.push_back(HistEvent{.lParam = lParam, .text = text});
                break;
            }
            else
            {
                const auto & changeEvent = changeEvents[i];
                InsertTreeItem(hActionRoot, changeEvent.summary, lParam);
                histAction.subEvents.push_back(HistEvent{.lParam = lParam, .text = changeEvent.summary});
            }
        }

        if ( actionIndex <= 1 )
            ExpandItem(parent);

        return &(actionTree.insert(std::make_pair(actionIndex, histAction)).first->second);
    }
    else
        return nullptr;
}

void HistoryTree::InsertAction(std::size_t actionIndex, const RareEdit::RenderAction<DescriptorIndex> & action)
{
    SetRedraw(false);
    auto* insertedAction = InsertAction(actionIndex, action, hHistoryRoot);
    SetRedraw(true);
    SendMessage(TreeViewControl::getHandle(), WM_VSCROLL, SB_BOTTOM, 0);
}

void HistoryTree::RebuildHistoryTree()
{
    /*EmptySubTree(hHistoryRoot);

    HTREEITEM selected = NULL;
    for ( size_t i = 1; i < 10; i++ )
        ;//InsertAction("asdf", (u32)i);

    ExpandItem(hHistoryRoot);

    RedrawThis();*/
}

void HistoryTree::RefreshActionHeaders(std::optional<std::size_t> excludeIndex)
{
    SetRedraw(false);
    std::size_t excludedIndex = excludeIndex ? *excludeIndex : std::numeric_limits<std::size_t>::max();
    auto changeHistory = CM->renderChangeHistory(false);
    auto cursorIndex = CM->getCursorIndex();
    std::size_t totalByteCount = 0;
    for ( std::size_t actionIndex=0; actionIndex < changeHistory.size(); ++actionIndex )
    {
        const auto & action = changeHistory[actionIndex];
        totalByteCount += action.byteCount;
        if ( actionIndex == excludedIndex )
            continue;

        std::size_t distance = cursorIndex > actionIndex ? cursorIndex-actionIndex : actionIndex-cursorIndex;

        //logger.info() << getActionText(actionIndex, action) << '\n';
        
        HistAction* histAction = nullptr;
        auto found = actionTree.find(actionIndex);
        if ( found != actionTree.end() )
            histAction = &found->second;
        else
        {
            if ( action.isElided() )
                histAction = InsertAction(actionIndex, action, hHistoryRoot);
            else if ( action.isElisionMarker() )
            { // Insert elision marker and elided redo children
                histAction = InsertAction(actionIndex, action, hHistoryRoot);
                std::size_t first = static_cast<std::size_t>(static_cast<std::ptrdiff_t>(actionIndex)-static_cast<std::ptrdiff_t>(action.elisionCount));
                for ( std::size_t i=first; i<actionIndex; ++i )
                {
                    auto & subAction = actionTree[i]; // Should always exist
                    subAction.moveUnder(*this, *histAction);
                }
                auto elisionMarkerText = action.elisionCount == 1 ? "[" + std::to_string(actionIndex) + "] Elides previous action" :
                    "[" + std::to_string(actionIndex) + "] Elides previous " + std::to_string(action.elisionCount) + " actions";
                InsertTreeItem(histAction->hItem, elisionMarkerText, actionIndex|TreeTypeAction);
                histAction->subEvents.push_back(HistEvent{.lParam = (LPARAM)actionIndex|TreeTypeAction, .text=elisionMarkerText});
            }
        }

        if ( histAction != nullptr && histAction->hItem != NULL && !action.isElisionMarker() )
        {
            icux::uistring sysNewText = icux::toUistring(getActionText(actionIndex, action));

            TVITEM item = { };
            item.mask = TVIF_TEXT|TVIF_STATE;
            item.stateMask |= TVIS_BOLD;
            item.hItem = histAction->hItem;
            item.pszText = (LPTSTR)sysNewText.c_str();
            if ( action.isRedoable() )
                item.state = TVIS_BOLD;
            else if ( action.isElided() || distance > 3 )
                item.stateMask |= TVIS_EXPANDED;
            else
                item.state = 0;

            SendMessage(getHandle(), TVM_SETITEM, 0, (LPARAM)&item);
            //SetItemText(hActionItem, getActionText(actionIndex, action));
        }
    }
    this->SetItemText(hHistoryRoot, "History (" + getSizeString(totalByteCount) + ")");
    SetRedraw(true);
    RedrawThis();
}
