#include "MapProperties.h"
#include "ColorProperties.h"
#include "../../../CommonFiles/CommonFiles.h"
#include "../../../Chkdraft.h"
#include "../../../Mapping/Undos/ChkdUndos/DimensionChange.h"
#include <string>

extern Logger logger;

enum_t(Id, u32, {
    EDIT_MAPTITLE = ID_FIRST,
    EDIT_MAPDESCRIPTION,
    CB_MAPTILESET,
    CB_NEWMAPTERRAIN,
    EDIT_NEWMAPWIDTH,
    EDIT_NEWMAPHEIGHT,
    BUTTON_APPLY,
    CB_P1OWNER,
    CB_P2OWNER,
    CB_P3OWNER,
    CB_P4OWNER,
    CB_P5OWNER,
    CB_P6OWNER,
    CB_P7OWNER,
    CB_P8OWNER,
    CB_P9OWNER,
    CB_P10OWNER,
    CB_P11OWNER,
    CB_P12OWNER,
    CB_P1RACE,
    CB_P2RACE,
    CB_P3RACE,
    CB_P4RACE,
    CB_P5RACE,
    CB_P6RACE,
    CB_P7RACE,
    CB_P8RACE,
    CB_P9RACE,
    CB_P10RACE,
    CB_P11RACE,
    CB_P12RACE,
    CB_P1COLOR,
    CB_P2COLOR,
    CB_P3COLOR,
    CB_P4COLOR,
    CB_P5COLOR,
    CB_P6COLOR,
    CB_P7COLOR,
    CB_P8COLOR,
    BUTTON_P1COLOR,
    BUTTON_P2COLOR,
    BUTTON_P3COLOR,
    BUTTON_P4COLOR,
    BUTTON_P5COLOR,
    BUTTON_P6COLOR,
    BUTTON_P7COLOR,
    BUTTON_P8COLOR
});

MapPropertiesWindow::MapPropertiesWindow() : possibleTitleUpdate(false), possibleDescriptionUpdate(false), refreshing(false)
{

}

MapPropertiesWindow::~MapPropertiesWindow()
{

}

bool MapPropertiesWindow::CreateThis(HWND hParent, u64 windowId)
{
    if ( getHandle() != NULL )
        return SetParent(hParent);

    if ( ClassWindow::RegisterWindowClass(0, NULL, NULL, NULL, NULL, "MapProperties", NULL, false) &&
         ClassWindow::CreateClassWindow(0, "MapProperties", WS_VISIBLE|WS_CHILD, 4, 22, 592, 524, hParent, (HMENU)windowId) )
    {
        refreshing = true;
        HWND hMapProperties = getHandle();

        std::optional<ChkdString> mapTitle, mapDescription;
        u16 currTileset = 0,
            currWidth = 0,
            currHeight = 0;

        if ( CM != nullptr )
        {
            mapTitle = CM->getScenarioName<ChkdString>();
            mapDescription = CM->getScenarioDescription<ChkdString>();
            currTileset = CM->getTileset() % Sc::Terrain::NumTilesets,
            currWidth = (u16)CM->getTileWidth(),
            currHeight = (u16)CM->getTileHeight();
        }

        textMapTitle.CreateThis(hMapProperties, 5, 5, 50, 20, "Map Title", 0);
        editMapTitle.CreateThis(hMapProperties, 5, 25, 582, 20, false, Id::EDIT_MAPTITLE);
        editMapTitle.SetText(mapTitle ? *mapTitle : "");

        textMapDescription.CreateThis(hMapProperties, 5, 60, 100, 20, "Map Description", 0);
        editMapDescription.CreateThis(hMapProperties, 5, 80, 582, 100, true, Id::EDIT_MAPDESCRIPTION);
        editMapDescription.SetText(mapDescription ? *mapDescription : "");

        std::string sCurrWidth(std::to_string(currWidth));
        std::string sCurrHeight(std::to_string(currHeight));

        textMapTileset.CreateThis(hMapProperties, 5, 185, 100, 20, "Map Tileset", 0);
        dropMapTileset.CreateThis(hMapProperties, 5, 205, 185, 400, false, false, Id::CB_MAPTILESET, tilesetNames);
        textNewMapTerrain.CreateThis(hMapProperties, 195, 185, 100, 20, "[New] Terrain", 0);

        const auto & tileset = chkd.scData.terrain.get(Sc::Terrain::Tileset(currTileset));
        dropNewMapTerrain.CreateThis(hMapProperties, 195, 205, 185, 400, false, false, Id::CB_NEWMAPTERRAIN, {});
        for ( const auto & brushType : tileset.brushes )
            dropNewMapTerrain.AddItem(std::string(brushType.name), brushType.index);

        dropNewMapTerrain.SetSel(tileset.defaultBrush.brushSortOrder);
        textNewMapWidth.CreateThis(hMapProperties, 385, 185, 50, 20, "Width", 0);
        editMapWidth.CreateThis(hMapProperties, 385, 205, 50, 20, false, Id::EDIT_NEWMAPWIDTH);
        editMapWidth.SetText(sCurrWidth);
        textNewMapHeight.CreateThis(hMapProperties, 440, 185, 50, 20, "Height", 0);
        editMapHeight.CreateThis(hMapProperties, 440, 205, 50, 20, false, Id::EDIT_NEWMAPHEIGHT);
        editMapHeight.SetText(sCurrHeight);
        buttonApply.CreateThis(hMapProperties, 494, 205, 91, 20, "Apply", Id::BUTTON_APPLY);

        const char* sPlayers[] = { "Player 1", "Player 2" , "Player 3" , "Player 4",
                                   "Player 5", "Player 6" , "Player 7" , "Player 8",
                                   "Player 9", "Player 10", "Player 11", "Player 12" };

        for ( int yBox=0; yBox<3; yBox++ )
        {
            for ( int xBox=0; xBox<4; xBox++ )
            {
                int player = yBox*4+xBox;
                Chk::Race race = Chk::Race::Zerg;
                Chk::PlayerColor color = Chk::PlayerColor::Azure_NeutralColor;
                if ( CM != nullptr )
                    race = CM->getPlayerRace(player);

                groupMapPlayers[yBox*4+xBox].CreateThis(hMapProperties, 5+146*xBox, 242+95*yBox, 141, 91, sPlayers[yBox*4+xBox], 0);
                textPlayerOwner[yBox*4+xBox].CreateThis(hMapProperties, 13+146*xBox, 257+95*yBox, 50, 20, "Owner", 0);
                dropPlayerOwner[yBox*4+xBox].CreateThis(hMapProperties, 60+146*xBox, 257+95*yBox, 80, 140, false, false, Id::CB_P1OWNER+player, playerOwners);
                textPlayerRace[yBox*4+xBox].CreateThis(hMapProperties, 13+146*xBox, 282+95*yBox, 50, 20, "Race", 0);
                dropPlayerRaces[yBox*4+xBox].CreateThis(hMapProperties, 60+146*xBox, 282+95*yBox, 80, 110, false, false, Id::CB_P1RACE+player, playerRaces);

                if ( yBox < 2 )
                {
                    if ( CM != nullptr )
                        color = CM->getPlayerColor(player);

                    std::vector<std::string> colorSelections{};
                    colorSelections.insert(colorSelections.end(), specialColors.begin(), specialColors.end());
                    colorSelections.insert(colorSelections.end(), playerColors.begin(), playerColors.end());

                    textPlayerColor[player].CreateThis(hMapProperties, 13+146*xBox, 307+95*yBox, 50, 20, "Color", 0);
                    dropPlayerColor[player].CreateThis(hMapProperties, 60+146*xBox, 307+95*yBox, 80, 140, true, false, Id::CB_P1COLOR+player, colorSelections);
                    buttonColorProperties[player].CreateThis(hMapProperties, dropPlayerColor[player].Left()-20, 307+95*yBox, 20, 19, "", Id::BUTTON_P1COLOR+player, true);
                    buttonColorProperties[player].SetImageFromResourceId(IDB_PROPERTIES);
                }
            }
        }
        refreshing = false;
        return true;
    }
    else
        return false;
}

bool MapPropertiesWindow::DestroyThis()
{
    ClassWindow::DestroyThis();
    this->possibleTitleUpdate = false;
    this->possibleDescriptionUpdate = false;
    this->refreshing = false;
    return true;
}

void MapPropertiesWindow::RefreshWindow()
{
    refreshing = true;
    if ( CM != nullptr )
    {
        auto mapTitle = CM->getScenarioName<ChkdString>();
        auto mapDescription = CM->getScenarioDescription<ChkdString>();
        u16 tilesetIndex = CM->getTileset(),
            currWidth = (u16)CM->getTileWidth(),
            currHeight = (u16)CM->getTileHeight();

        std::string sCurrWidth(std::to_string(currWidth));
        std::string sCurrHeight(std::to_string(currHeight));
        
        if ( mapTitle )
            editMapTitle.SetText(*mapTitle);

        if ( mapDescription )
            editMapDescription.SetText(*mapDescription);

        possibleTitleUpdate = false;
        possibleDescriptionUpdate = false;
        dropMapTileset.SetSel(tilesetIndex);
        dropMapTileset.ClearEditSel();

        std::vector<std::string> initTerrains {};
        const auto & tileset = chkd.scData.terrain.get(Sc::Terrain::Tileset(tilesetIndex));
        dropNewMapTerrain.ClearItems();
        for ( const auto & brushType : tileset.brushes )
            dropNewMapTerrain.AddItem(std::string(brushType.name), brushType.index);
        
        dropNewMapTerrain.SetSel(tileset.defaultBrush.brushSortOrder);
        dropNewMapTerrain.ClearEditSel();
        editMapWidth.SetText(sCurrWidth);
        editMapHeight.SetText(sCurrHeight);
                    
        for ( int player=0; player<12; player++ )
        {
            u8 displayOwner(CM->GetPlayerOwnerStringId(player));
            Chk::Race race = CM->getPlayerRace(player);
            dropPlayerOwner[player].SetSel(displayOwner);
            dropPlayerOwner[player].ClearEditSel();
            dropPlayerRaces[player].SetSel(race);
            dropPlayerRaces[player].ClearEditSel();

            if ( player < 8 )
            {
                if ( CM->isUsingRemasteredColors() )
                {
                    auto colorSetting = CM->getPlayerColorSetting(player);
                    switch ( colorSetting )
                    {
                        case Chk::PlayerColorSetting::RandomPredefined:
                            dropPlayerColor[player].SetSel(0);
                            break;
                        case Chk::PlayerColorSetting::PlayerChoice:
                            dropPlayerColor[player].SetSel(1);
                            break;
                        case Chk::PlayerColorSetting::Custom:
                        {
                            auto customColor = CM->getPlayerCustomColor(player);
                            if ( size_t(dropPlayerColor[player].GetNumItems()) > ::playerColors.size() + ::specialColors.size() )
                                dropPlayerColor[player].RemoveItem(dropPlayerColor[player].GetNumItems()-1); // Remove the custom color element
                            auto rgb = RGB(customColor.red, customColor.green, customColor.blue);
                            int customIndex = dropPlayerColor[player].AddItem(to_hex_string(rgb));
                            if ( customIndex >= 0 )
                                dropPlayerColor[player].SetSel(customIndex);
                        }
                        break;
                        case Chk::PlayerColorSetting::UseId:
                            dropPlayerColor[player].SetSel(CM->getPlayerCustomColor(player).blue+int(specialColors.size()));
                            break;
                    }
                }
                else // Using COLR section
                {
                    Chk::PlayerColor color = CM->getPlayerColor(player);
                    dropPlayerColor[player].SetSel(int(color)+int(specialColors.size()));
                    dropPlayerColor[player].ClearEditSel();
                }
            }
        }
    }
    refreshing = false;
}

LRESULT MapPropertiesWindow::Command(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch ( LOWORD(wParam) )
    {
    case Id::EDIT_MAPTITLE:
        if ( HIWORD(wParam) == EN_CHANGE && !refreshing )
            possibleTitleUpdate = true;
        else if ( HIWORD(wParam) == EN_KILLFOCUS )
            CheckReplaceMapTitle();
        break;

    case Id::EDIT_MAPDESCRIPTION:
        if ( HIWORD(wParam) == EN_CHANGE && !refreshing )
            possibleDescriptionUpdate = true;
        else if ( HIWORD(wParam) == EN_KILLFOCUS )
            CheckReplaceMapDescription();
        break;

    case Id::BUTTON_APPLY:
    {
        if ( HIWORD(wParam) == BN_CLICKED )
        {
            auto dimensionsUndo = DimensionChange::Make((void*)CM.get());
            Sc::Terrain::Tileset newTileset = (Sc::Terrain::Tileset)SendMessage(GetDlgItem(hWnd, Id::CB_MAPTILESET), CB_GETCURSEL, 0, 0);
            size_t newMapTerrain = dropNewMapTerrain.GetSelData();
            CM->setTileset(newTileset);
            u16 newWidth, newHeight;
            if ( editMapWidth.GetEditNum<u16>(newWidth) && editMapHeight.GetEditNum<u16>(newHeight) )
                CM->setDimensions(newWidth, newHeight, Scenario::SizeValidationFlag::Default, 0, 0, newMapTerrain);
            
            CM->AddUndo(dimensionsUndo);
            CM->refreshScenario();
            CM->Redraw(true);
        }
    }
    break;

    case Id::CB_MAPTILESET:
        if ( HIWORD(wParam) == CBN_SELCHANGE )
        {
            HWND hMapTileset = GetDlgItem(hWnd, Id::CB_MAPTILESET), hMapNewTerrain = GetDlgItem(hWnd, Id::CB_NEWMAPTERRAIN);
            LRESULT currTileset = SendMessage(hMapTileset, CB_GETCURSEL, 0, 0);
            if ( currTileset != CB_ERR && currTileset < (LRESULT)tilesetNames.size())
            {
                while ( SendMessage(hMapNewTerrain, CB_DELETESTRING, 0, 0) != CB_ERR );

                const auto & tileset = chkd.scData.terrain.get(Sc::Terrain::Tileset(currTileset));
                for ( const auto & brushType : tileset.brushes )
                    SendMessage(hMapNewTerrain, CB_ADDSTRING, 0, (LPARAM)icux::toUistring(std::string(brushType.name)).c_str());

                SendMessage(hMapNewTerrain, WM_SETFONT, (WPARAM)WinLib::ResourceManager::getDefaultFont(), MAKELPARAM(TRUE, 0));
                SendMessage(hMapNewTerrain, CB_SETCURSEL, (WPARAM)tileset.defaultBrush.brushSortOrder, NULL);
                PostMessage(hMapNewTerrain, CB_SETEDITSEL, 0, MAKELPARAM(-1, 0));
            }
        }
        break;
    case Id::CB_P1OWNER: case Id::CB_P2OWNER: case Id::CB_P3OWNER: case Id::CB_P4OWNER:
    case Id::CB_P5OWNER: case Id::CB_P6OWNER: case Id::CB_P7OWNER: case Id::CB_P8OWNER:
    case Id::CB_P9OWNER: case Id::CB_P10OWNER: case Id::CB_P11OWNER: case Id::CB_P12OWNER:
        if ( HIWORD(wParam) == CBN_SELCHANGE )
        {
            u32 player = LOWORD(wParam) - Id::CB_P1OWNER; // 0 based player
            Sc::Player::SlotType newType = Sc::Player::SlotType::Inactive;
            LRESULT sel = SendMessage((HWND)lParam, CB_GETCURSEL, 0, 0);
            if ( player >= 0 && player < 12 && sel != CB_ERR )
            {
                switch ( sel )
                {
                case 0: newType = Sc::Player::SlotType::Unused; break;
                case 1: newType = Sc::Player::SlotType::RescuePassive; break;
                case 2: newType = Sc::Player::SlotType::Computer; break;
                case 3: newType = Sc::Player::SlotType::Human; break;
                case 4: newType = Sc::Player::SlotType::Neutral; break;
                }
                if ( newType != Sc::Player::SlotType::Inactive )
                {
                    CM->setSlotType(player, newType);
                    CM->notifyChange(false);
                }
            }
        }
        break;

    case Id::CB_P1RACE: case Id::CB_P2RACE: case Id::CB_P3RACE: case Id::CB_P4RACE:
    case Id::CB_P5RACE: case Id::CB_P6RACE: case Id::CB_P7RACE: case Id::CB_P8RACE:
    case Id::CB_P9RACE: case Id::CB_P10RACE: case Id::CB_P11RACE: case Id::CB_P12RACE:
        if ( HIWORD(wParam) == CBN_SELCHANGE )
        {
            u32 player = LOWORD(wParam) - Id::CB_P1RACE; // 0 based player
            LRESULT newRace = SendMessage((HWND)lParam, CB_GETCURSEL, 0, 0);
            if ( player >= 0 && player < 12 && newRace != CB_ERR && newRace >= 0 && newRace < 8 )
            {
                CM->setPlayerRace(player, (Chk::Race)newRace);
                chkd.maps.UpdatePlayerStatus();
                CM->Redraw(true);
                CM->notifyChange(false);
            }
        }
        break;
        
    case Id::CB_P1COLOR: case Id::CB_P2COLOR: case Id::CB_P3COLOR: case Id::CB_P4COLOR:
    case Id::CB_P5COLOR: case Id::CB_P6COLOR: case Id::CB_P7COLOR: case Id::CB_P8COLOR:
        switch ( HIWORD(wParam) )
        {
        case CBN_SELCHANGE:
        {
            u32 player = LOWORD(wParam) - Id::CB_P1COLOR; // 0 based player
            LRESULT newColor = SendMessage((HWND)lParam, CB_GETCURSEL, 0, 0);
            LRESULT maxColorIndex = ::specialColors.size() + ::playerColors.size();
            if ( player >= 0 && player < 12 && newColor != CB_ERR && newColor >= 0 && newColor < maxColorIndex )
            {
                switch ( newColor )
                {
                case 0: CM->setPlayerColorSetting(player, Chk::PlayerColorSetting::RandomPredefined); break;
                case 1: CM->setPlayerColorSetting(player, Chk::PlayerColorSetting::PlayerChoice); break;
                default: CM->setPlayerColor(player, Chk::PlayerColor(newColor-LRESULT(::specialColors.size()))); break;
                }
                chkd.maps.UpdatePlayerStatus();
                CM->Redraw(true);
                CM->notifyChange(false);
            }
        }
        break;
        case CBN_EDITCHANGE:
        {
            u32 player = LOWORD(wParam) - Id::CB_P1COLOR; // 0 based player
            u8 newColor;
            if ( dropPlayerColor[player].GetEditNum<u8>(newColor) )
            {
                CM->setPlayerColor(player, (Chk::PlayerColor)newColor);
                chkd.maps.UpdatePlayerStatus();
                CM->Redraw(true);
                CM->notifyChange(false);
            }
        }

        }
        break;
    }
    return ClassWindow::Command(hWnd, wParam, lParam);
}

LRESULT MapPropertiesWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        case WM_SHOWWINDOW:
            if ( wParam == TRUE )
            {
                RefreshWindow();
                chkd.mapSettingsWindow.SetWinText("Map Settings");
            }
            else if ( !refreshing )
            {
                CheckReplaceMapTitle();
                CheckReplaceMapDescription();
            }
            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;

        default:
            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;
    }
    return 0;
}

void MapPropertiesWindow::NotifyButtonClicked(int idFrom, HWND hWndFrom)
{
    if ( u32(idFrom) >= Id::BUTTON_P1COLOR && u32(idFrom) <= Id::BUTTON_P8COLOR )
    {
        size_t playerIndex = size_t(idFrom)-Id::BUTTON_P1COLOR;
        if ( auto playerColor = ColorPropertiesDialog::GetCrgbColor(getHandle(), CM->getPlayerCustomColor(playerIndex)) )
        {
            CM->setPlayerColorSetting(playerIndex, Chk::PlayerColorSetting::Custom);
            CM->setPlayerCustomColor(playerIndex, *playerColor);
            chkd.maps.UpdatePlayerStatus();
            CM->Redraw(true);
            CM->notifyChange(false);
            this->RefreshWindow();
        }
    }
}

void MapPropertiesWindow::CheckReplaceMapTitle()
{
    if ( possibleTitleUpdate == true )
    {
        if ( auto newMapTitle = editMapTitle.GetWinText() )
        {
            CM->setScenarioName<ChkdString>(*newMapTitle);
            CM->deleteUnusedStrings(Chk::StrScope::Both);
            CM->notifyChange(false);
            possibleTitleUpdate = false;
        }
    }
}

void MapPropertiesWindow::CheckReplaceMapDescription()
{
    if ( possibleDescriptionUpdate == true )
    {
        if ( auto newMapDescription = editMapDescription.GetWinText() )
        {
            CM->setScenarioDescription<ChkdString>(*newMapDescription);
            CM->deleteUnusedStrings(Chk::StrScope::Both);
            CM->notifyChange(false);
            possibleDescriptionUpdate = false;
        }
    }
}
