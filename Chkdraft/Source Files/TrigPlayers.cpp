#include "TrigPlayers.h"
#include "Chkdraft.h"

enum ID
{
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
	CHECK_ID27,
	TEXT_STATS,
	GROUP_UNUSEDPLAYERS,
	GROUP_RAWEDIT,
	TEXT_EXTENDEDDATA,
	EDIT_RAWPLAYERS,
	CHECK_ALLOWRAWEDIT
};

TrigPlayersWindow::TrigPlayersWindow() : trigIndex(0), refreshing(true)
{

}

bool TrigPlayersWindow::CreateThis(HWND hParent, u32 windowId)
{
	if ( getHandle() != NULL )
		return SetParent(hParent);

	RECT rcCli;
	if ( GetWindowRect(hParent, &rcCli) &&
		 ClassWindow::RegisterWindowClass(NULL, NULL, NULL, NULL, NULL, "TrigPlayers", NULL, false) &&
		 ClassWindow::CreateClassWindow(NULL, "TrigPlayers", WS_CHILD,
			5, 62, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top,
			hParent, (HMENU)windowId) )
	{
		CreateSubWindows(getHandle());
		return true;
	}
	else
		return false;
}

bool TrigPlayersWindow::DestroyThis()
{
	return false;
}

void TrigPlayersWindow::RefreshWindow(u32 trigIndex)
{
	refreshing = true;
	this->trigIndex = trigIndex;
	Trigger* trig;
	if ( chkd.maps.curr->getTrigger(trig, trigIndex) )
	{
		stringstream ssStats;
		const char* strTimesExecuted[] = { "Never", "Once", "Twice", "Thrice" };
		u8 exec[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
		if ( trig->players[AllPlayers] & 1 )
		{
			for ( u8 player=0; player<8; player++ )
				exec[player] ++;
		}
		for ( u8 force=0; force<4; force++ )
		{
			if ( trig->players[Force1+force] & 1 )
			{
				for ( u8 player=0; player<8; player++ )
				{
					u8 playersForce;
					if ( chkd.maps.curr->getPlayerForce(player, playersForce) && playersForce == force )
						exec[player] ++;
				}
			}
		}
		for ( u8 player=0; player<8; player++ )
		{
			if ( trig->players[Player1+player] & 1 )
				exec[player] ++;
		}
		ssStats << "For each trigger cycle..." << endl
			<< "This trigger will be run by:" << endl << endl
			<< "By Player 1: " << strTimesExecuted[exec[0]] << endl
			<< "By Player 2: " << strTimesExecuted[exec[1]] << endl
			<< "By Player 3: " << strTimesExecuted[exec[2]] << endl
			<< "By Player 4: " << strTimesExecuted[exec[3]] << endl
			<< "By Player 5: " << strTimesExecuted[exec[4]] << endl
			<< "By Player 6: " << strTimesExecuted[exec[5]] << endl
			<< "By Player 7: " << strTimesExecuted[exec[6]] << endl
			<< "By Player 8: " << strTimesExecuted[exec[7]] << endl;
		textPlayerStats.SetText(ssStats.str().c_str());

		for ( u8 i=0; i<8; i++ )
			checkMainPlayers[i].SetCheck(trig->players[Player1+i] & 1);
		for ( u8 i=0; i<4; i++ )
			checkForces[i].SetCheck(trig->players[Force1+i] & 1);
		checkAllPlayers.SetCheck(trig->players[AllPlayers] & 1);
		for ( u8 i=Player9; i<=NeutralPlayers; i++ )
			checkNonExecutingPlayers[i-Player9].SetCheck(trig->players[i] & 1);
		for ( u8 i=Unused1; i<=ID27; i++ )
			checkNonExecutingPlayers[i-Unused1+9].SetCheck(trig->players[i] & 1);

		editRawPlayers.SetHexByteString(&trig->players[0], NUM_TRIG_PLAYERS);
	}
	refreshing = false;
}

void TrigPlayersWindow::DoSize()
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
	}
}

void TrigPlayersWindow::CreateSubWindows(HWND hWnd)
{
	groupExecutingPlayers.CreateThis(hWnd, 5, 5, 330, 190, "Executing Players", GROUP_EXECUTINGPLAYERS);
	for ( u8 i=0; i<8; i++ )
	{
		stringstream ssPlayer;
		ssPlayer << "Player " << (i+1);
		checkMainPlayers[i].CreateThis(hWnd, 12, 24+18*i, 75, 17, false, ssPlayer.str().c_str(), CHECK_PLAYER1+i);
	}
	int yPos = 24;
	for ( u8 i=0; i<4; i++ )
	{
		stringstream ssPlayer;
		ssPlayer << "Force " << (i+1);
		checkForces[i].CreateThis(hWnd, 110, 24+18*i, 75, 17, false, ssPlayer.str().c_str(), CHECK_FORCE1+i);
		yPos += 18;
	}
	checkAllPlayers.CreateThis(hWnd, 110, yPos, 75, 17, false, "All Players", CHECK_ALL_PLAYERS);
	textPlayerStats.CreateThis(hWnd, 200, 24, 120, 150, "", TEXT_STATS);
	
	groupNonExecutingPlayers.CreateThis(hWnd, 340, 5, 210, 190, "Non-Executing Players", GROUP_UNUSEDPLAYERS);
	for ( u8 i=0; i<9; i++ )
		checkNonExecutingPlayers[i].CreateThis(hWnd, 347, 24+18*i, 90, 17, false, triggerPlayers[8+i], CHECK_PLAYER9+i);

	for ( u8 i=0; i<6; i++ )
		checkNonExecutingPlayers[i+9].CreateThis(hWnd, 450, 24+18*i, 92, 17, false, triggerPlayers[22+i], CHECK_UNUSED1+i);

	groupRawEdit.CreateThis(hWnd, 5, 200, 545, 160, "Raw Data", GROUP_RAWEDIT);
	textExtendedData.CreateThis(hWnd, 12, 219, 220, 60, "While not used in StarCraft modifying these bytes can be useful with 3rd party programs. Bytes 22-25 are used by Chkdraft and should not be manually altered. Numbers are in hex.", TEXT_EXTENDEDDATA);
	checkAllowRawEdit.CreateThis(hWnd, 240, 219, 110, 20, false, "Enable Raw Edit", CHECK_ALLOWRAWEDIT);
	editRawPlayers.CreateThis(hWnd, 12, 284, 455, 20, false, EDIT_RAWPLAYERS);
	editRawPlayers.DisableThis();
}

void TrigPlayersWindow::CheckBoxUpdated(u16 checkId)
{
	Trigger* trig;
	if ( chkd.maps.curr->getTrigger(trig, trigIndex) )
	{
		if ( checkId >= CHECK_PLAYER1 && checkId <= CHECK_PLAYER8 )
		{
			u8 player = u8(checkId-CHECK_PLAYER1);
			if ( checkMainPlayers[player].isChecked() )
				trig->players[player+Player1] = 1;
			else
				trig->players[player+Player1] = 0;
		}
		else if ( checkId >= CHECK_FORCE1 && checkId <= CHECK_FORCE4 )
		{
			u8 force = u8(checkId-CHECK_FORCE1);
			if ( checkForces[force].isChecked() )
				trig->players[force+Force1] = 1;
			else
				trig->players[force+Force1] = 0;
		}
		else if ( checkId == CHECK_ALL_PLAYERS )
		{
			if ( checkAllPlayers.isChecked() )
				trig->players[AllPlayers] = 1;
			else
				trig->players[AllPlayers] = 0;
		}
		else if ( checkId >= CHECK_PLAYER9 && checkId <= CHECK_NEUTRALPLAYERS )
		{
			u8 lowerNonExecutingPlayersId = u8(checkId-CHECK_PLAYER9);
			if ( checkNonExecutingPlayers[lowerNonExecutingPlayersId].isChecked() )
				trig->players[checkId-CHECK_PLAYER1] = 1;
			else
				trig->players[checkId-CHECK_PLAYER1] = 0;
		}
		else if ( checkId >= CHECK_UNUSED1 && checkId <= CHECK_ID27 )
		{
			u8 upperNonExecutingPlayersId = u8(checkId-CHECK_UNUSED1+9);
			if ( checkNonExecutingPlayers[upperNonExecutingPlayersId].isChecked() )
				trig->players[checkId-CHECK_PLAYER1] = 1;
			else
				trig->players[checkId-CHECK_PLAYER1] = 0;
		}
		else if ( checkId == CHECK_ALLOWRAWEDIT )
		{
			if ( checkAllowRawEdit.isChecked() )
				editRawPlayers.EnableThis();
			else
				editRawPlayers.DisableThis();
		}
		
	}
	RefreshWindow(trigIndex);
}

void TrigPlayersWindow::OnLeave()
{
	ParseRawPlayers();
}

void TrigPlayersWindow::ParseRawPlayers()
{
	Trigger* trigger;
	if ( chkd.maps.curr->getTrigger(trigger, trigIndex) )
	{
		if ( editRawPlayers.GetHexByteString(&trigger->players[0], 28) )
			chkd.maps.curr->notifyChange(false);
		
		RefreshWindow(trigIndex);
	}
}

LRESULT TrigPlayersWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_SHOWWINDOW:
			if ( wParam == FALSE )
				OnLeave();
			else if ( wParam == TRUE )
				RefreshWindow(trigIndex);
			break;

		case WM_LBUTTONDOWN:
			SetFocus(getHandle());
			break;

		case WM_COMMAND:
			if ( !refreshing )
			{
				switch ( LOWORD(wParam) )
				{
					case EDIT_RAWPLAYERS:
						if ( HIWORD(wParam) == EN_KILLFOCUS )
							ParseRawPlayers();
						break;
				}
				switch ( HIWORD(wParam) )
				{
					case BN_CLICKED:
						CheckBoxUpdated(LOWORD(wParam));
						break;
				}
			}
			break;

		case WM_CLOSE:
			OnLeave();
			return DefWindowProc(hWnd, msg, wParam, lParam);
			break;

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}