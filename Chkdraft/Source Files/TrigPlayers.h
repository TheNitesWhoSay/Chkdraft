#ifndef TRIGPLAYERS_H
#define TRIGPLAYERS_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"
#include "Windows UI/WindowsUI.h"

class TrigPlayersWindow : public ClassWindow
{
	public:
		TrigPlayersWindow();
		bool CreateThis(HWND hParent, u32 windowId);
		bool DestroyThis();
		void RefreshWindow(u32 trigIndex);
		void DoSize();

	protected:
		void CreateSubWindows(HWND hWnd);
		void CheckBoxUpdated(u16 checkId);
		void OnLeave();
		void ParseRawPlayers();

	private:
		bool refreshing;
		u32 trigIndex;
		LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		GroupBoxControl groupExecutingPlayers;
		CheckBoxControl checkMainPlayers[8];
		CheckBoxControl checkForces[4];
		CheckBoxControl checkAllPlayers;
		TextControl textPlayerStats;

		GroupBoxControl groupNonExecutingPlayers;
		CheckBoxControl checkNonExecutingPlayers[15]; // Starting at p9//ID:8, Skip ID:17-21, Ending at ID:27

		GroupBoxControl groupRawEdit;
		CheckBoxControl checkAllowRawEdit;
		TextControl textExtendedData;
		EditControl editRawPlayers; // 56 hex characters // 28 bytes
};

#endif