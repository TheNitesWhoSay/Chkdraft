#include "briefing_trig_players.h"
#include "chkdraft/chkdraft.h"
#include <sstream>

enum_t(Id, u32, {
    GROUP_EXECUTINGPLAYERS = ID_FIRST,
    CHECK_PLAYER1,
    CHECK_PLAYER8 = CHECK_PLAYER1+7,
    CHECK_PLAYER9,
    CHECK_PLAYER12 = CHECK_PLAYER9+3,
    CHECK_PLAYERNONE,
    CHECK_CURRENTPLAYER,
    CHECK_FOES,
    CHECK_ALLIES,
    CHECK_NEUTRALPLAYERS,
    CHECK_ALL_PLAYERS,
    CHECK_FORCE1,
    CHECK_FORCE4 = CHECK_FORCE1+3,
    CHECK_UNUSED1,
    CHECK_UNUSED4 = CHECK_UNUSED1+3,
    CHECK_NONAVPLAYERS,
    TEXT_STATS,
    GROUP_UNUSEDPLAYERS,
    GROUP_RAWEDIT,
    TEXT_EXTENDEDDATA,
    EDIT_RAWPLAYERS,
    CHECK_ALLOWRAWEDIT,
    BUTTON_ADVANCED
});

BriefingTrigPlayersWindow::BriefingTrigPlayersWindow() : briefingTrigIndex(0), refreshing(true), advancedMode(true)
{

}

BriefingTrigPlayersWindow::~BriefingTrigPlayersWindow()
{

}

bool BriefingTrigPlayersWindow::CreateThis(HWND hParent, u64 windowId)
{
    if ( getHandle() != NULL )
        return SetParent(hParent);

    RECT rcCli;
    if ( GetWindowRect(hParent, &rcCli) &&
         ClassWindow::RegisterWindowClass(0, NULL, NULL, NULL, NULL, "BriefingTrigPlayers", NULL, false) &&
         ClassWindow::CreateClassWindow(0, "BriefingTrigPlayers", WS_CHILD,
            5, 62, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top,
            hParent, (HMENU)windowId) )
    {
        CreateSubWindows(getHandle());
        return true;
    }
    else
        return false;
}

bool BriefingTrigPlayersWindow::DestroyThis()
{
    ClassWindow::DestroyThis();
    this->briefingTrigIndex = 0;
    this->refreshing = true;
    this->advancedMode = true;
    return true;
}

void BriefingTrigPlayersWindow::RefreshWindow(u32 briefingTrigIndex)
{
    refreshing = true;
    this->briefingTrigIndex = briefingTrigIndex;
    const Chk::Trigger & trig = CM->getBriefingTrigger(briefingTrigIndex);
    std::stringstream ssStats;
    const char* strTimesExecuted[] = { "Never", "Once", "Twice", "Thrice" };
    u8 exec[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    if ( trig.owners[Sc::Player::Id::AllPlayers] & Chk::Trigger::Owned::Yes )
    {
        for ( u8 player=0; player<8; player++ )
            exec[player] ++;
    }
    for ( u8 force=0; force<4; force++ )
    {
        if ( trig.owners[Sc::Player::Id::Force1+force] & Chk::Trigger::Owned::Yes )
        {
            for ( u8 player=0; player<8; player++ )
            {
                if ( CM->getPlayerForce(player) == force )
                    exec[player] ++;
            }
        }
    }
    for ( u8 player=0; player<8; player++ )
    {
        if ( trig.owners[Sc::Player::Id::Player1+player] & Chk::Trigger::Owned::Yes )
            exec[player] ++;
    }
    ssStats << "This briefing trigger will be run by:" << std::endl << std::endl
        << "By Player 1: " << strTimesExecuted[exec[0]] << std::endl
        << "By Player 2: " << strTimesExecuted[exec[1]] << std::endl
        << "By Player 3: " << strTimesExecuted[exec[2]] << std::endl
        << "By Player 4: " << strTimesExecuted[exec[3]] << std::endl
        << "By Player 5: " << strTimesExecuted[exec[4]] << std::endl
        << "By Player 6: " << strTimesExecuted[exec[5]] << std::endl
        << "By Player 7: " << strTimesExecuted[exec[6]] << std::endl
        << "By Player 8: " << strTimesExecuted[exec[7]] << std::endl;
    textPlayerStats.SetText(ssStats.str());

    for ( u8 i=0; i<8; i++ )
        checkMainPlayers[i].SetCheck(trig.owners[i+Sc::Player::Id::Player1] & Chk::Trigger::Owned::Yes);
    for ( u8 i=0; i<4; i++ )
        checkForces[i].SetCheck(trig.owners[i+Sc::Player::Id::Force1] & Chk::Trigger::Owned::Yes);
    checkAllPlayers.SetCheck(trig.owners[Sc::Player::Id::AllPlayers] & Chk::Trigger::Owned::Yes);
    for ( u8 i=Sc::Player::Id::Player9; i<=Sc::Player::Id::NeutralPlayers; i++ )
        checkNonExecutingPlayers[i-Sc::Player::Id::Player9].SetCheck(trig.owners[i] & Chk::Trigger::Owned::Yes);
    for ( u8 i=Sc::Player::Id::Unused1; i<=Sc::Player::Id::NonAlliedVictoryPlayers; i++ )
        checkNonExecutingPlayers[i-Sc::Player::Id::Unused1+9].SetCheck(trig.owners[i] & Chk::Trigger::Owned::Yes);

    editRawPlayers.SetHexByteString((u8*)&trig.owners[0], Chk::Trigger::MaxOwners);
    refreshing = false;
}

void BriefingTrigPlayersWindow::DoSize()
{
    RECT rect;
    if ( getClientRect(rect) )
    {
        long cliWidth = rect.right-rect.left,
             cliHeight = rect.bottom-rect.top;

        if ( cliWidth > 340 )
            groupNonExecutingPlayers.SetPos(340, 5, cliWidth-345, 190);

        if ( cliHeight > 200 )
            groupRawEdit.SetPos(5, 200, cliWidth-10, cliHeight-205);

        buttonAdvanced.MoveTo(rect.right-buttonAdvanced.Width()-8, rect.bottom-buttonAdvanced.Height()-8);
    }
}

void BriefingTrigPlayersWindow::CreateSubWindows(HWND hWnd)
{
    for ( u8 i=0; i<8; i++ )
    {
        std::stringstream ssPlayer;
        ssPlayer << "Player " << (i+1);
        checkMainPlayers[i].CreateThis(hWnd, 12, 24+18*i, 75, 17, false, ssPlayer.str(), Id::CHECK_PLAYER1+i);
    }
    int yPos = 24;
    for ( u8 i=0; i<4; i++ )
    {
        std::stringstream ssPlayer;
        ssPlayer << "Force " << (i+1);
        checkForces[i].CreateThis(hWnd, 110, 24+18*i, 75, 17, false, ssPlayer.str(), Id::CHECK_FORCE1+i);
        yPos += 18;
    }
    checkAllPlayers.CreateThis(hWnd, 110, yPos, 75, 17, false, "All Players", Id::CHECK_ALL_PLAYERS);
    textPlayerStats.CreateThis(hWnd, 200, 24, 120, 150, "", Id::TEXT_STATS);
    
    groupExecutingPlayers.CreateThis(hWnd, 5, 5, 330, 190, "Executing Players", Id::GROUP_EXECUTINGPLAYERS);
    groupNonExecutingPlayers.CreateThis(hWnd, 340, 5, 210, 190, "Non-Executing Players", Id::GROUP_UNUSEDPLAYERS);
    for ( u8 i=0; i<9; i++ )
        checkNonExecutingPlayers[i].CreateThis(hWnd, 347, 24+18*i, 90, 17, false, triggerPlayers.at(8+i), Id::CHECK_PLAYER9+i);

    for ( u8 i=0; i<5; i++ )
        checkNonExecutingPlayers[i+9].CreateThis(hWnd, 450, 24+18*i, 92, 17, false, triggerPlayers.at(22+i), Id::CHECK_UNUSED1+i);

    groupRawEdit.CreateThis(hWnd, 5, 200, 545, 160, "Raw Data", Id::GROUP_RAWEDIT);
    textExtendedData.CreateThis(hWnd, 12, 219, 220, 60,
        "While not used in StarCraft modifying these bytes can be useful with 3rd party programs. Bytes 22-25 are used by Chkdraft and should not be manually altered. Numbers are in hex.",
        Id::TEXT_EXTENDEDDATA);
    checkAllowRawEdit.CreateThis(hWnd, 240, 219, 110, 20, false, "Enable Raw Edit", Id::CHECK_ALLOWRAWEDIT);
    editRawPlayers.CreateThis(hWnd, 12, 284, 455, 20, false, Id::EDIT_RAWPLAYERS);
    buttonAdvanced.CreateThis(hWnd, 12, 310, 75, 20, "Advanced", Id::BUTTON_ADVANCED);
    editRawPlayers.DisableThis();

    ToggleAdvancedMode();
}

void BriefingTrigPlayersWindow::CheckBoxUpdated(u16 checkId)
{
    Chk::Trigger & trig = CM->getBriefingTrigger(briefingTrigIndex);
    if ( checkId >= Id::CHECK_PLAYER1 && checkId <= Id::CHECK_PLAYER8 )
    {
        u8 player = u8(checkId-Id::CHECK_PLAYER1);
        if ( checkMainPlayers[player].isChecked() )
            trig.owners[player+Sc::Player::Id::Player1] = Chk::Trigger::Owned::Yes;
        else
            trig.owners[player+Sc::Player::Id::Player1] = Chk::Trigger::Owned::No;
    }
    else if ( checkId >= Id::CHECK_FORCE1 && checkId <= Id::CHECK_FORCE4 )
    {
        u8 force = u8(checkId-Id::CHECK_FORCE1);
        if ( checkForces[force].isChecked() )
            trig.owners[force+Sc::Player::Id::Force1] = Chk::Trigger::Owned::Yes;
        else
            trig.owners[force+Sc::Player::Id::Force1] = Chk::Trigger::Owned::No;
    }
    else if ( checkId == Id::CHECK_ALL_PLAYERS )
    {
        if ( checkAllPlayers.isChecked() )
            trig.owners[Sc::Player::Id::AllPlayers] = Chk::Trigger::Owned::Yes;
        else
            trig.owners[Sc::Player::Id::AllPlayers] = Chk::Trigger::Owned::No;
    }
    else if ( checkId >= Id::CHECK_PLAYER9 && checkId <= Id::CHECK_NEUTRALPLAYERS )
    {
        u8 lowerNonExecutingPlayersId = u8(checkId-Id::CHECK_PLAYER9);
        if ( checkNonExecutingPlayers[lowerNonExecutingPlayersId].isChecked() )
            trig.owners[checkId-Id::CHECK_PLAYER1] = Chk::Trigger::Owned::Yes;
        else
            trig.owners[checkId-Id::CHECK_PLAYER1] = Chk::Trigger::Owned::No;
    }
    else if ( checkId >= Id::CHECK_UNUSED1 && checkId <= Id::CHECK_NONAVPLAYERS )
    {
        u8 upperNonExecutingPlayersId = u8(checkId-Id::CHECK_UNUSED1+9);
        if ( checkNonExecutingPlayers[upperNonExecutingPlayersId].isChecked() )
        {
            if ( trig.getExtendedDataIndex() != Chk::ExtendedTrigDataIndex::None )
                (u8 &)trig.owners[checkId-Id::CHECK_PLAYER1] |= Chk::Trigger::Owned::Yes;
            else
                trig.owners[checkId-Id::CHECK_PLAYER1] = Chk::Trigger::Owned::Yes;
        }
        else
        {
            if ( trig.getExtendedDataIndex() != Chk::ExtendedTrigDataIndex::None )
                (u8 &)trig.owners[checkId-Id::CHECK_PLAYER1] &= ~Chk::Trigger::Owned::Yes;
            else
                trig.owners[checkId-Id::CHECK_PLAYER1] = Chk::Trigger::Owned::No;
        }
    }
    else if ( checkId == Id::CHECK_ALLOWRAWEDIT )
    {
        if ( checkAllowRawEdit.isChecked() )
            editRawPlayers.EnableThis();
        else
            editRawPlayers.DisableThis();
    }
    RefreshWindow(briefingTrigIndex);
}

void BriefingTrigPlayersWindow::OnLeave()
{
    ParseRawPlayers();
}

void BriefingTrigPlayersWindow::ParseRawPlayers()
{
    Chk::Trigger & trigger = CM->getBriefingTrigger(briefingTrigIndex);
    if ( editRawPlayers.GetHexByteString((u8*)&trigger.owners[0], 27) )
        CM->notifyChange(false);
        
    RefreshWindow(briefingTrigIndex);
}

void BriefingTrigPlayersWindow::ToggleAdvancedMode()
{
    advancedMode = !advancedMode;
    if ( advancedMode ) // Now in advanced mode
    {
        buttonAdvanced.SetText("Standard");
        groupNonExecutingPlayers.Show();
        for ( u8 i=0; i<9; i++ )
            checkNonExecutingPlayers[i].Show();

        for ( u8 i=0; i<5; i++ )
            checkNonExecutingPlayers[i+9].Show();

        groupRawEdit.Show();
        checkAllowRawEdit.Show();
        textExtendedData.Show();
        editRawPlayers.Show();
    }
    else // Now in standard mode
    {
        buttonAdvanced.SetText("Advanced");
        groupNonExecutingPlayers.Hide();
        for ( u8 i=0; i<14; i++ )
            checkNonExecutingPlayers[i].Hide();

        groupRawEdit.Hide();
        checkAllowRawEdit.Hide();
        textExtendedData.Hide();
        editRawPlayers.Hide();
        Hide(); Show(); // Dirty fix to issues with re-hiding controls
    }
}

LRESULT BriefingTrigPlayersWindow::Command(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if ( !refreshing )
    {
        switch ( LOWORD(wParam) )
        {
        case Id::EDIT_RAWPLAYERS:
            if ( HIWORD(wParam) == EN_KILLFOCUS )
                ParseRawPlayers();
            break;
        }
        switch ( HIWORD(wParam) )
        {
        case BN_CLICKED:
            if ( LOWORD(wParam) == Id::BUTTON_ADVANCED )
                ToggleAdvancedMode();
            else
                CheckBoxUpdated(LOWORD(wParam));
            break;
        }
    }
    return ClassWindow::Command(hWnd, wParam, lParam);
}

LRESULT BriefingTrigPlayersWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        case WM_SHOWWINDOW:
            if ( wParam == FALSE )
                OnLeave();
            else if ( wParam == TRUE )
                RefreshWindow(briefingTrigIndex);
            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;

        case WM_LBUTTONDOWN:
            SetFocus(getHandle());
            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;

        case WM_CLOSE:
            OnLeave();
            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;

        default:
            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}
