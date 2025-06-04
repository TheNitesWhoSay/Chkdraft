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

void HistoryTree::createRoot()
{
    hHistoryRoot = InsertTreeItem(NULL, "History", 0, true);
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
                // Misc
                case ActionDescriptor::BeginPaste: actionText += "Begin Paste"; break;
                case ActionDescriptor::DeleteSelection: actionText += "Delete Selection"; break;
                case ActionDescriptor::UpdateMiscSelection: actionText += "Update Misc Selection"; break;
                // Terrain
                case ActionDescriptor::BrushIsom: actionText += "Brush Isom"; break;
                case ActionDescriptor::UpdateTileSel: actionText += "Update Tile Selection"; break;
                case ActionDescriptor::ClearTileSel: actionText += "Clear Tile Selection"; break;
                case ActionDescriptor::DeleteTiles: actionText += "Delete Tiles"; break;
                case ActionDescriptor::PasteTiles: actionText += "Paste Tiles"; break;
                case ActionDescriptor::FillPasteTiles: actionText += "Fill Paste Tiles"; break;
                case ActionDescriptor::OpenTileProperties: actionText += "Open Tile Properties"; break;
                // Doodads
                case ActionDescriptor::UpdateDoodadSel: actionText += "Update Doodad Selection"; break;
                case ActionDescriptor::ClearDoodadSel: actionText += "Clear Doodad Selection"; break;
                case ActionDescriptor::PasteDoodad: actionText += "Paste Doodads"; break;
                case ActionDescriptor::DeleteDoodad: actionText += "Delete Doodads"; break;
                case ActionDescriptor::ConvertDoodad: actionText += "Convert Doodad To Terrain"; break;
                // Fog of War
                case ActionDescriptor::BrushFog: actionText += "Brush Fog"; break;
                case ActionDescriptor::PasteFog: actionText += "Paste Fog"; break;
                case ActionDescriptor::DeleteFogTiles: actionText += "Delete Fog Tiles"; break;
                // Locations
                case ActionDescriptor::UpdateLocationSel: actionText += "Update Location Selection"; break;
                case ActionDescriptor::ClearLocationSel: actionText += "Clear Location Selection"; break;
                case ActionDescriptor::CreateLocation: actionText += "Create Location"; break;
                case ActionDescriptor::MoveLocation: actionText += "Move Location"; break;
                case ActionDescriptor::DeleteLocations: actionText += "Delete Locations"; break;
                // Units
                case ActionDescriptor::UpdateUnitSel: actionText += "Update Unit Selection"; break;
                case ActionDescriptor::ClearUnitSel: actionText += "Clear Unit Selection"; break;
                case ActionDescriptor::CreateUnit: actionText += "Create Unit"; break;
                case ActionDescriptor::PasteUnits: actionText += "Paste Units"; break;
                case ActionDescriptor::DeleteUnits: actionText += "Delete Units"; break;
                case ActionDescriptor::StackUnits: actionText += "Stack Units"; break;
                case ActionDescriptor::CreateLocationForUnit: actionText += "Create Location For Unit"; break;
                case ActionDescriptor::CreateInvertedLocationForUnit: actionText += "Create Inverted Location For Unit"; break;
                case ActionDescriptor::CreateMobileInvertedLocationForUnit: actionText += "Create Mobile Inverted Location For Unit"; break;
                // Sprites
                case ActionDescriptor::UpdateSpriteSel: actionText += "Update Sprite Selection"; break;
                case ActionDescriptor::ClearSpriteSel: actionText += "Clear Sprite Selection"; break;
                case ActionDescriptor::CreateSprite: actionText += "Create Sprite"; break;
                case ActionDescriptor::PasteSprites: actionText += "Paste Sprites"; break;
                case ActionDescriptor::DeleteSprites: actionText += "Delete Sprites"; break;
                case ActionDescriptor::StackSprites: actionText += "Stack Sprites"; break;
                // Cut Copy Paste
                // Default
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
    SetRedraw(false);
    EmptySubTree(hHistoryRoot);
    
    this->actionTree.clear();
    auto changeHistory = CM->renderChangeHistory(true);
    for ( std::size_t i=0; i<changeHistory.size(); ++i )
        InsertAction(i, changeHistory[i]);
    
    this->SetItemText(hHistoryRoot, "History");
    SetRedraw(true);
    RedrawThis();
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
        }
    }
    this->SetItemText(hHistoryRoot, "History (" + getSizeString(totalByteCount) + ")");
    if ( cursorIndex == 0 )
        SendMessage(TreeViewControl::getHandle(), WM_VSCROLL, SB_TOP, 0);
    else if ( cursorIndex == actionTree.size() )
        SendMessage(TreeViewControl::getHandle(), WM_VSCROLL, SB_BOTTOM, 0);
    else if ( actionTree.contains(cursorIndex-1) )
        TreeView_EnsureVisible(getHandle(), this->actionTree.find(cursorIndex-1)->second.hItem);

    SetRedraw(true);
    RedrawThis();
}
