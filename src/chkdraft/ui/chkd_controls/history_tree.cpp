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

std::string getActionText(std::size_t actionIndex, const nf::rendered_action<DescriptorIndex> & action)
{
    if ( action.is_elision_marker() )
        return "[" + std::to_string((int)actionIndex-(int)action.elision_count) + "-" + std::to_string(actionIndex) + "] Elided";
    else
    {
        std::string actionText = "[" + std::to_string(actionIndex) + "] ";
        switch ( action.status )
        {
            case nf::action_status::undoable:
                actionText += "Undoable";
                break;
            case nf::action_status::elided_redo:
                actionText += "Elided";
                break;
            case nf::action_status::redoable:
                actionText += "Redoable";
                break;
        }
        if ( action.user_data.descriptorIndex != ActionDescriptor::None )
        {
            actionText += " - ";
            switch ( action.user_data.descriptorIndex )
            {
                // Misc
                case ActionDescriptor::BeginPaste: actionText += "Begin Paste"; break;
                case ActionDescriptor::PasteMisc: actionText += "Paste Misc"; break;
                case ActionDescriptor::DeleteSelection: actionText += "Delete Selection"; break;
                case ActionDescriptor::UpdateMiscSelection: actionText += "Update Misc Selection"; break;
                case ActionDescriptor::ChangeTileset: actionText += "Change Tileset"; break;
                case ActionDescriptor::ResizeMap: actionText += "Resize Map"; break;
                case ActionDescriptor::UpdateSaveType: actionText += "Update Save Type"; break;
                case ActionDescriptor::UpdateSaveSections: actionText += "Update Save Sections"; break;
                case ActionDescriptor::ClearSelections: actionText += "Clear Selections"; break;
                case ActionDescriptor::SelectAll: actionText += "Select All"; break;
                case ActionDescriptor::SetPassword: actionText += "Set Password"; break;
                case ActionDescriptor::AddSaveSection: actionText += "Add Save Section"; break;
                case ActionDescriptor::RemoveSaveSection: actionText += "Remove Save Section"; break;
                case ActionDescriptor::UpdateMapVersion: actionText += "Update Map Version"; break;
                case ActionDescriptor::ResetVcod: actionText += "Reset VCOD"; break;
                // Terrain
                case ActionDescriptor::BrushIsom: actionText += "Brush Isom"; break;
                case ActionDescriptor::UpdateTileSel: actionText += "Update Tile Selection"; break;
                case ActionDescriptor::ClearTileSel: actionText += "Clear Tile Selection"; break;
                case ActionDescriptor::DeleteTiles: actionText += "Delete Tiles"; break;
                case ActionDescriptor::PasteTiles: actionText += "Paste Tiles"; break;
                case ActionDescriptor::FillPasteTiles: actionText += "Fill Paste Tiles"; break;
                case ActionDescriptor::OpenTileProperties: actionText += "Open Tile Properties"; break;
                case ActionDescriptor::UpdateTileValue: actionText += "Update Tile Value"; break;
                case ActionDescriptor::ValidateTileOccupiers: actionText += "Validate Tile Occupiers"; break;
                // Doodads
                case ActionDescriptor::CreateDoodad: actionText += "Create Doodad"; break;
                case ActionDescriptor::UpdateDoodadSel: actionText += "Update Doodad Selection"; break;
                case ActionDescriptor::ClearDoodadSel: actionText += "Clear Doodad Selection"; break;
                case ActionDescriptor::PasteDoodad: actionText += "Paste Doodads"; break;
                case ActionDescriptor::DeleteDoodad: actionText += "Delete Doodads"; break;
                case ActionDescriptor::MoveDoodad: actionText += "Move Doodad"; break;
                case ActionDescriptor::ConvertDoodad: actionText += "Convert Doodad To Terrain"; break;
                case ActionDescriptor::UpdateDoodadPlayer: actionText += "Update Doodad Player"; break;
                // Fog of War
                case ActionDescriptor::BrushFog: actionText += "Brush Fog"; break;
                case ActionDescriptor::PasteFog: actionText += "Paste Fog"; break;
                case ActionDescriptor::DeleteFogTiles: actionText += "Delete Fog Tiles"; break;
                case ActionDescriptor::UpdateTileFog: actionText += "Update Tile Fog"; break;
                // Locations
                case ActionDescriptor::UpdateLocationSel: actionText += "Update Location Selection"; break;
                case ActionDescriptor::ClearLocationSel: actionText += "Clear Location Selection"; break;
                case ActionDescriptor::CreateLocation: actionText += "Create Location"; break;
                case ActionDescriptor::ReplaceLocation: actionText += "Replace Location"; break;
                case ActionDescriptor::MoveLocation: actionText += "Move Location"; break;
                case ActionDescriptor::DeleteLocations: actionText += "Delete Locations"; break;
                case ActionDescriptor::InvertLocationXc: actionText += "Invert Location Xc"; break;
                case ActionDescriptor::InvertLocationYc: actionText += "Invert Location Yc"; break;
                case ActionDescriptor::InvertLocationXcYc: actionText += "Invert Location"; break;
                case ActionDescriptor::ToggleLocationLowGround: actionText += "Toggle Location Low Ground"; break;
                case ActionDescriptor::ToggleLocationMedGround: actionText += "Toggle Location Med Ground"; break;
                case ActionDescriptor::ToggleLocationHighGround: actionText += "Toggle Location High Ground"; break;
                case ActionDescriptor::ToggleLocationLowAir: actionText += "Toggle Location Low Air"; break;
                case ActionDescriptor::ToggleLocationMedAir: actionText += "Toggle Location Med Air"; break;
                case ActionDescriptor::ToggleLocationHighAir: actionText += "Toggle Location High Air"; break;
                case ActionDescriptor::UpdateLocationName: actionText += "Update Location Name"; break;
                case ActionDescriptor::UpdateLocationRawFlags: actionText += "Update Location Raw Flags"; break;
                case ActionDescriptor::AdjustLocationLeft: actionText += "Adjust Location Left"; break;
                case ActionDescriptor::AdjustLocationTop: actionText += "Adjust Location Top"; break;
                case ActionDescriptor::AdjustLocationRight: actionText += "Adjust Location Right"; break;
                case ActionDescriptor::AdjustLocationBottom: actionText += "Adjust Location Bottom"; break;
                case ActionDescriptor::TrimLocationsToOriginal: actionText += "Trim Locations To Original"; break;
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
                case ActionDescriptor::CreateLocationForSprite: actionText += "Create Location For Sprite"; break;
                case ActionDescriptor::CreateInvertedLocationForSprite: actionText += "Create Inverted Location For Sprite"; break;
                case ActionDescriptor::CreateMobileInvertedLocationForSprite: actionText += "Create Mobile Inverted Location For Sprite"; break;
                case ActionDescriptor::UpdateUnitOwner: actionText += "Update Unit Owner"; break;
                case ActionDescriptor::MoveUnits: actionText += "Move Units"; break;
                case ActionDescriptor::ToggleUnitLinked: actionText += "Toggle Unit Linked"; break;
                case ActionDescriptor::ToggleUnitInvincible: actionText += "Toggle Unit Invincible"; break;
                case ActionDescriptor::ToggleUnitHallucinated: actionText += "Toggle Unit Hallucinated"; break;
                case ActionDescriptor::ToggleUnitBurrowed: actionText += "Toggle Unit Burrowed"; break;
                case ActionDescriptor::ToggleUnitCloaked: actionText += "Toggle Unit Cloaked"; break;
                case ActionDescriptor::ToggleUnitLifted: actionText += "Toggle Unit Lifted"; break;
                case ActionDescriptor::UpdateUnitHp: actionText += "Update Unit Hp"; break;
                case ActionDescriptor::UpdateUnitMp: actionText += "Update Unit Mp"; break;
                case ActionDescriptor::UpdateUnitShields: actionText += "Update Unit Shields"; break;
                case ActionDescriptor::UpdateUnitResources: actionText += "Update Unit Resources"; break;
                case ActionDescriptor::UpdateUnitHangar: actionText += "Update Unit Hangar"; break;
                case ActionDescriptor::ChangeUnitType: actionText += "Change Unit Type"; break;
                case ActionDescriptor::AdjustUnitXc: actionText += "Adjust Unit Xc"; break;
                case ActionDescriptor::AdjustUnitYc: actionText += "Adjust Unit Yc"; break;
                case ActionDescriptor::AdjustUnitPosition: actionText += "Adjust Unit Position"; break;
                case ActionDescriptor::ToggleUnitValidFieldOwner: actionText += "Toggle Unit Valid Field Owner"; break;
                case ActionDescriptor::ToggleUnitValidFieldLife: actionText += "Toggle Unit Valid Field Life"; break;
                case ActionDescriptor::ToggleUnitValidFieldMana: actionText += "Toggle Unit Valid Field Mana"; break;
                case ActionDescriptor::ToggleUnitValidFieldShields: actionText += "Toggle Unit Valid Field Shields"; break;
                case ActionDescriptor::ToggleUnitValidFieldResources: actionText += "Toggle Unit Valid Field Resources"; break;
                case ActionDescriptor::ToggleUnitValidFieldHangar: actionText += "Toggle Unit Valid Field Hangar"; break;
                case ActionDescriptor::UpdateUnitValidFieldRawFlags: actionText += "Update Unit Valid Field Raw Flags"; break;
                case ActionDescriptor::ToggleUnitValidStateInvincible: actionText += "Toggle Unit Valid State Invincible"; break;
                case ActionDescriptor::ToggleUnitValidStateBurrowed: actionText += "Toggle Unit Valid State Burrowed"; break;
                case ActionDescriptor::ToggleUnitValidStateHallucinated: actionText += "Toggle Unit Valid State Hallucinated"; break;
                case ActionDescriptor::ToggleUnitValidStateCloaked: actionText += "Toggle Unit Valid State Cloaked"; break;
                case ActionDescriptor::ToggleUnitValidStateLifted: actionText += "Toggle Unit Valid State Lifted"; break;
                case ActionDescriptor::UpdateUnitValidStateRawFlags: actionText += "Update Unit Valid State Raw Flags"; break;
                case ActionDescriptor::UpdateUnitStateRawFlags: actionText += "Update Unit State Raw Flags"; break;
                case ActionDescriptor::UpdateUnitUnused: actionText += "Update Unit Unused"; break;
                case ActionDescriptor::UpdateUnitUniqueId: actionText += "Update Unit Unique Id"; break;
                case ActionDescriptor::UpdateUnitLinkedId: actionText += "Update Unit Linked Id"; break;
                case ActionDescriptor::ToggleUnitNydusLink: actionText += "Toggle Unit Nydus Link"; break;
                case ActionDescriptor::ToggleUnitAddonLink: actionText += "Toggle Unit Addon Link"; break;
                case ActionDescriptor::UpdateUnitRelationRawFlags: actionText += "Update Unit Relation Raw Flags"; break;
                // Sprites
                case ActionDescriptor::UpdateSpriteSel: actionText += "Update Sprite Selection"; break;
                case ActionDescriptor::ClearSpriteSel: actionText += "Clear Sprite Selection"; break;
                case ActionDescriptor::CreateSprite: actionText += "Create Sprite"; break;
                case ActionDescriptor::PasteSprites: actionText += "Paste Sprites"; break;
                case ActionDescriptor::DeleteSprites: actionText += "Delete Sprites"; break;
                case ActionDescriptor::StackSprites: actionText += "Stack Sprites"; break;
                case ActionDescriptor::UpdateSpriteOwner: actionText += "Update Sprite Owner"; break;
                case ActionDescriptor::MoveSprites: actionText += "Move Sprites"; break;
                case ActionDescriptor::UpdateSpriteFlags: actionText += "Update Sprite Flags"; break;
                case ActionDescriptor::ChangeSpriteType: actionText += "Change Sprite Type"; break;
                case ActionDescriptor::UpdateSpriteUnused: actionText += "Update Sprite Unused"; break;
                case ActionDescriptor::AdjustSpriteXc: actionText += "Adjust Sprite Xc"; break;
                case ActionDescriptor::AdjustSpriteYc: actionText += "Adjust Sprite Yc"; break;
                case ActionDescriptor::AdjustSpritePosition: actionText += "Adjust Sprite Position"; break;
                // Cut Copy Paste
                // Scenario
                case ActionDescriptor::SetScenarioName: actionText += "Set Scenario Name"; break;
                case ActionDescriptor::SetScenarioDescription: actionText += "Set Scenario Description"; break;
                // Players
                case ActionDescriptor::SetSlotPlayerType: actionText += "Set Slot Player Type"; break;
                case ActionDescriptor::SetPlayerRace: actionText += "Set Player Race"; break;
                case ActionDescriptor::SetPlayerColor: actionText += "Set Player Color"; break;
                case ActionDescriptor::SetPlayerForce: actionText += "Set Player Force"; break;
                // Forces
                case ActionDescriptor::UpdateForceName: actionText += "Update Force Name"; break;
                case ActionDescriptor::UpdateForceFlags: actionText += "Update Force Flags"; break;
                // Unit Properties
                case ActionDescriptor::UpdateUnitName: actionText += "Update Unit Name"; break;
                case ActionDescriptor::ToggleUnitTypeUsesDefaultSettings: actionText += "Toggle Unit Type Uses Default Settings"; break;
                case ActionDescriptor::UpdateUnitTypeHitpoints: actionText += "Update Unit Type Hitpoints"; break;
                case ActionDescriptor::UpdateUnitTypeShields: actionText += "Update Unit Type Shields"; break;
                case ActionDescriptor::UpdateUnitTypeArmor: actionText += "Update Unit Type Armor"; break;
                case ActionDescriptor::UpdateUnitTypeBuildTime: actionText += "Update Unit Type Build Time"; break;
                case ActionDescriptor::UpdateUnitTypeMineralCost: actionText += "Update Unit Type Mineral Cost"; break;
                case ActionDescriptor::UpdateUnitTypeGasCost: actionText += "Update Unit Type Gas Cost"; break;
                case ActionDescriptor::SetUnitTypeBuildable: actionText += "Set Unit Type Buildable"; break;
                case ActionDescriptor::SetUnitTypeDefaultBuildable: actionText += "Set Unit Type Default Buildable"; break;
                case ActionDescriptor::SetPlayerUsesDefaultUnitBuildability: actionText += "Set Player Uses Default Unit Buildability"; break;
                case ActionDescriptor::ResetAllUnitProperites: actionText += "Reset All Unit Properties"; break;
                // Weapons
                case ActionDescriptor::UpdateWeaponBaseDamage: actionText += "Update Weapon Base Damage"; break;
                case ActionDescriptor::UpdateWeaponUpgradeDamage: actionText += "Update Weapon Upgrade Damage"; break;
                // Upgrades
                case ActionDescriptor::SetUpgradeUsesDefaultCosts: actionText += "Set Upgrade Uses Default Costs"; break;
                case ActionDescriptor::SetUpgradeBaseMineralCost: actionText += "Set Upgrade Base Mineral Cost"; break;
                case ActionDescriptor::SetUpgradeMineralCostFactor: actionText += "Set Upgrade Mineral Cost Factor"; break;
                case ActionDescriptor::SetUpgradeBaseGasCost: actionText += "Set Upgrade Base Gas Cost"; break;
                case ActionDescriptor::SetUpgradeGasCostFactor: actionText += "Set Upgrade Gas Cost Factor"; break;
                case ActionDescriptor::SetUpgradeBaseResearchTime: actionText += "Set Upgrade Base Research Time"; break;
                case ActionDescriptor::SetUpgradeResearchTimeFactor: actionText += "Set Upgrade Research Time Factor"; break;
                case ActionDescriptor::SetStartUpgradeLevel: actionText += "Set Start Upgrade Level"; break;
                case ActionDescriptor::SetMaxUpgradeLevel: actionText += "Set Max Upgrade Level"; break;
                case ActionDescriptor::SetDefaultStartUpgradeLevel: actionText += "Set Default Start Upgrade Level"; break;
                case ActionDescriptor::SetDefaultMaxUpgradeLevel: actionText += "Set Default Max Upgrade Level"; break;
                case ActionDescriptor::SetPlayerUsesDefaultUpgradeLeveling: actionText += "Set Player Uses Default Upgrade Leveling"; break;
                case ActionDescriptor::ResetAllUpgradeProperties: actionText += "Reset All Upgrade Properties"; break;
                // Techs
                case ActionDescriptor::SetTechUsesDefaultSettings: actionText += "Set Tech Uses Default Settings"; break;
                case ActionDescriptor::SetTechMineralCost: actionText += "Set Tech Mineral Cost"; break;
                case ActionDescriptor::SetTechGasCost: actionText += "Set Tech Gas Cost"; break;
                case ActionDescriptor::SetTechResearchTime: actionText += "Set Tech Research Time"; break;
                case ActionDescriptor::SetTechEnergyCost: actionText += "Set Tech Energy Cost"; break;
                case ActionDescriptor::SetTechAvailable: actionText += "Set Tech Available"; break;
                case ActionDescriptor::SetTechResearched: actionText += "Set Tech Researched"; break;
                case ActionDescriptor::SetTechDefaultAvailable: actionText += "Set Tech Default Available"; break;
                case ActionDescriptor::SetTechDefaultResearched: actionText += "Set Tech Default Researched"; break;
                case ActionDescriptor::SetPlayerUsesDefaultTechSettings: actionText += "Set Player Uses Default Tech Settings"; break;
                case ActionDescriptor::ResetAllTechProperties: actionText += "Reset All Tech Properties"; break;
                // Strings
                case ActionDescriptor::SetStringProperties: actionText += "Set String Properties"; break;
                case ActionDescriptor::AddString: actionText += "Add String"; break;
                case ActionDescriptor::ReplaceString: actionText += "Replace String"; break;
                case ActionDescriptor::DeleteString: actionText += "Delete String"; break;
                case ActionDescriptor::MoveString: actionText += "Move String"; break;
                case ActionDescriptor::RescopeString: actionText += "Rescope String"; break;
                // Sounds
                case ActionDescriptor::AddSound: actionText += "Add Sound"; break;
                case ActionDescriptor::RemoveSound: actionText += "Remove Sound"; break;
                // CUWPs
                case ActionDescriptor::AddCuwp: actionText += "Add CUWP"; break;
                case ActionDescriptor::SetCuwp: actionText += "Set CUWP"; break;
                case ActionDescriptor::SetCuwpUsed: actionText += "Set CUWP Used"; break;
                // Switches
                case ActionDescriptor::SetSwitchName: actionText += "Set Switch Name"; break;
                // Triggers
                case ActionDescriptor::AddTrigger: actionText += "Add Trigger"; break;
                case ActionDescriptor::DeleteTrigger: actionText += "Delete Trigger"; break;
                case ActionDescriptor::MoveTrigger: actionText += "Move Trigger"; break;
                case ActionDescriptor::ReplaceTriggerRange: actionText += "Replace Trigger Range"; break;
                case ActionDescriptor::CreateExtendedTriggerData: actionText += "Create Extended Trigger Data"; break;
                case ActionDescriptor::DeleteExtendedTriggerData: actionText += "Delete Extended Trigger Data"; break;
                case ActionDescriptor::ChangeTriggerActionType: actionText += "Change Trigger Action Type"; break;
                case ActionDescriptor::ChangeTriggerActionString: actionText += "Change Trigger Action String"; break;
                case ActionDescriptor::ChangeTriggerActionSound: actionText += "Change Trigger Action Sound"; break;
                case ActionDescriptor::SetTriggerActionUnitProperties: actionText += "Set Trigger Action Unit Properties"; break;
                case ActionDescriptor::ChangeTriggerConditionType: actionText += "Change Trigger Condition Type"; break;
                case ActionDescriptor::UpdateTriggerRawFlags: actionText += "Update Trigger Raw Flags"; break;
                case ActionDescriptor::UpdateTriggerComment: actionText += "Update Trigger Comment"; break;
                case ActionDescriptor::UpdateTriggerNotes: actionText += "Update Trigger Notes"; break;
                case ActionDescriptor::ToggleTriggerOwner: actionText += "Toggle Trigger Owner"; break;
                case ActionDescriptor::UpdateTriggerRawPlayers: actionText += "Update Trigger Raw Players"; break;
                case ActionDescriptor::UpdateTriggerConditionArg: actionText += "Update Trigger Condition Arg"; break;
                case ActionDescriptor::UpdateTriggerActionArg: actionText += "Update Trigger Action Arg"; break;
                case ActionDescriptor::TogglePreserveTriggerFlag: actionText += "Toggle Preserve Trigger Flag"; break;
                case ActionDescriptor::ToggleDisabledFlag: actionText += "Toggle Trigger Disabled Flag"; break;
                case ActionDescriptor::ToggleIgnoreConditionsOnceFlag: actionText += "Toggle Trig Ignore Conditions Once Flag"; break;
                case ActionDescriptor::ToggleIgnoreWaitSkipOnceFlag: actionText += "Toggle Trig Ignore Wait Skip Once Flag"; break;
                case ActionDescriptor::ToggleIgnoreMiscActionsOnceFlag: actionText += "Toggle Trig Ignore Misc Actions Once Flag"; break;
                case ActionDescriptor::ToggleIgnoreDefeatDrawFlag: actionText += "Toggle Trig Ignore Defeat Draw Flag"; break;
                case ActionDescriptor::TogglePausedFlag: actionText += "Toggle Trig Paused Flag"; break;
                case ActionDescriptor::CompileTextTrigs: actionText += "Compile Text Trigs"; break;
                // BriefingTriggers
                case ActionDescriptor::AddBriefingTrigger: actionText += "Add Briefing Trigger"; break;
                case ActionDescriptor::DeleteBriefingTrigger: actionText += "Delete Briefing Trigger"; break;
                case ActionDescriptor::MoveBriefingTrigger: actionText += "Move Briefing Trigger"; break;
                case ActionDescriptor::ReplaceBriefingTriggerRange: actionText += "Replace Briefing Trigger Range"; break;
                case ActionDescriptor::ChangeBriefingActionType: actionText += "Change Briefing Action Type"; break;
                case ActionDescriptor::ChangeBriefingActionString: actionText += "Change Briefing Action String"; break;
                case ActionDescriptor::ChangeBriefingActionSound: actionText += "Change Briefing Action Sound"; break;
                case ActionDescriptor::ChangeBriefingTrigPlayers: actionText += "Change Briefing Trig Players"; break;
                case ActionDescriptor::UpdateBriefingActionArg: actionText += "Update Briefing Action Arg"; break;
                case ActionDescriptor::CompileBriefingTextTrigs: actionText += "Compile Briefing Text Trigs"; break;
                // Plugins/Scripts
                case ActionDescriptor::RepairSounds: actionText += "Repair Sounds"; break;
                case ActionDescriptor::RepairStrings: actionText += "Repair Strings"; break;
                // Default
                default: actionText += "TODO: " + std::to_string(std::underlying_type_t<ActionDescriptor>(action.user_data.descriptorIndex)); break;
            }
        }
        actionText += " (" + getSizeString(action.byte_count) + ")";
        return actionText;
    }
}

HistAction* HistoryTree::InsertAction(std::size_t actionIndex, const nf::rendered_action<DescriptorIndex> & action, HTREEITEM parent)
{
    if ( (u32)actionIndex <= TreeDataPortion )
    {
        std::string text = getActionText(actionIndex, action);
        HTREEITEM hActionRoot = InsertTreeItem(parent, text, actionIndex|TreeTypeAction, !action.is_elision_marker());
        HistAction histAction {
            .lParam = LPARAM(actionIndex)|TreeTypeAction,
            .hItem = hActionRoot,
            .actionStatus = action.status,
            .elisionCount = action.elision_count,
            .text = text
        };

        const auto & changeEvents = action.change_events;
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
        if ( action.user_data.descriptorIndex == ActionDescriptor::CompileTextTrigs ||
            action.user_data.descriptorIndex == ActionDescriptor::CompileBriefingTextTrigs )
        {
            std::string text = "  (Event rendering skipped)\n";
            InsertTreeItem(hActionRoot, text, TreeTypeEvent);
            histAction.subEvents.push_back(HistEvent{.lParam = TreeTypeEvent, .text = text});
        }

        if ( actionIndex <= 1 )
            ExpandItem(parent);

        return &(actionTree.insert(std::make_pair(actionIndex, histAction)).first->second);
    }
    else
        return nullptr;
}

void HistoryTree::InsertAction(std::size_t actionIndex, const nf::rendered_action<DescriptorIndex> & action)
{
    SetRedraw(false);
    auto* insertedAction = InsertAction(actionIndex, action, hHistoryRoot);
    SetRedraw(true);
    SendMessage(TreeViewControl::getHandle(), WM_VSCROLL, SB_BOTTOM, 0);
}

std::size_t HistoryTree::RebuildHistoryTree()
{
    SetRedraw(false);
    EmptySubTree(hHistoryRoot);
    
    this->actionTree.clear();
    auto changeHistory = CM->render_change_history(true);
    u64 totalByteCount = 0;
    for ( std::size_t i=0; i<changeHistory.size(); ++i )
    {
        InsertAction(i, changeHistory[i]);
        totalByteCount += static_cast<u64>(changeHistory[i].byte_count);
    }
    
    this->SetItemText(hHistoryRoot, "History (" + getSizeString(totalByteCount) + ")");
    SetRedraw(true);
    RedrawThis();
    return totalByteCount;
}

void HistoryTree::RefreshActionHeaders(std::optional<std::size_t> excludeIndex)
{
    SetRedraw(false);
    std::size_t excludedIndex = excludeIndex ? *excludeIndex : std::numeric_limits<std::size_t>::max();
    auto changeHistory = CM->render_change_history(false);
    auto cursorIndex = CM->get_cursor_index();
    std::size_t totalByteCount = 0;
    for ( std::size_t actionIndex=0; actionIndex < changeHistory.size(); ++actionIndex )
    {
        const auto & action = changeHistory[actionIndex];
        totalByteCount += action.byte_count;
        if ( actionIndex == excludedIndex )
            continue;

        std::size_t distance = cursorIndex > actionIndex ? cursorIndex-actionIndex : actionIndex-cursorIndex;
        
        HistAction* histAction = nullptr;
        auto found = actionTree.find(actionIndex);
        if ( found != actionTree.end() )
            histAction = &found->second;
        else
        {
            if ( action.is_elided() )
                histAction = InsertAction(actionIndex, action, hHistoryRoot);
            else if ( action.is_elision_marker() )
            { // Insert elision marker and elided redo children
                histAction = InsertAction(actionIndex, action, hHistoryRoot);
                std::size_t first = static_cast<std::size_t>(static_cast<std::ptrdiff_t>(actionIndex)-static_cast<std::ptrdiff_t>(action.elision_count));
                for ( std::size_t i=first; i<actionIndex; ++i )
                {
                    auto & subAction = actionTree[i]; // Should always exist
                    subAction.moveUnder(*this, *histAction);
                }
                auto elisionMarkerText = action.elision_count == 1 ? "[" + std::to_string(actionIndex) + "] Elides previous action" :
                    "[" + std::to_string(actionIndex) + "] Elides previous " + std::to_string(action.elision_count) + " actions";
                InsertTreeItem(histAction->hItem, elisionMarkerText, actionIndex|TreeTypeAction);
                histAction->subEvents.push_back(HistEvent{.lParam = (LPARAM)actionIndex|TreeTypeAction, .text=elisionMarkerText});
            }
        }

        if ( histAction != nullptr && histAction->hItem != NULL && !action.is_elision_marker() )
        {
            icux::uistring sysNewText = icux::toUistring(getActionText(actionIndex, action));

            TVITEM item = { };
            item.mask = TVIF_TEXT|TVIF_STATE;
            item.stateMask |= TVIS_BOLD;
            item.hItem = histAction->hItem;
            item.pszText = (LPTSTR)sysNewText.c_str();
            if ( action.is_redoable() )
                item.state = TVIS_BOLD;
            else if ( action.is_elided() || distance > 3 )
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

LRESULT HistoryTree::ControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        case WM_SIZING:
        {
            int yBorder = GetSystemMetrics(SM_CYSIZEFRAME);
            RECT rcLeftBar {};
            RECT rcHistTree {};
            RECT rcMainTree {};
            WinLib::TreeViewControl::getWindowRect(rcHistTree);
            chkd.mainPlot.leftBar.getClientRect(rcLeftBar);
            LONG leftBarHeightMinusMinimap = rcLeftBar.bottom-rcLeftBar.top-145;
            LONG histTreeHeight = rcHistTree.bottom-rcHistTree.top;
            chkd.mainPlot.leftBar.historyTreeSize = float(double(histTreeHeight)/double(leftBarHeightMinusMinimap));
            if ( chkd.mainPlot.leftBar.historyTreeSize < 0.1f )
                chkd.mainPlot.leftBar.historyTreeSize = 0.1f;
            else if ( chkd.mainPlot.leftBar.historyTreeSize > 0.9f )
                chkd.mainPlot.leftBar.historyTreeSize = 0.9f;
            
            int totalTreeHeight = rcLeftBar.bottom-rcLeftBar.top-146;
            int mainTreeHeight = totalTreeHeight-histTreeHeight;

            // Fit the main tree to the middle part of the left bar
            GetClientRect(chkd.mainPlot.leftBar.getHandle(), &rcLeftBar);
            SetWindowPos(chkd.mainPlot.leftBar.mainTree.getHandle(), NULL, -2, 145, rcLeftBar.right-rcLeftBar.left+2, mainTreeHeight+yBorder+6, SWP_NOZORDER);
            return 0;
        }
        break;
        case WM_NCHITTEST:
        {
            auto result = WinLib::TreeViewControl::ControlProc(hWnd, msg, wParam, lParam);
            switch ( result )
            {
                case HTTOPLEFT: case HTTOP: case HTTOPRIGHT: return HTTOP;
                case HTLEFT: case HTRIGHT: case HTBOTTOMRIGHT: case HTBOTTOM: case HTBOTTOMLEFT:
                    return HTCLIENT; // Prevent use of any sizing border except top as a sizing border
                default: return result;
            }
        }
        break;
    }
    return WinLib::TreeViewControl::ControlProc(hWnd, msg, wParam, lParam);
}
