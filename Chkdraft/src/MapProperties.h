#ifndef MAPPROPERTIES_H
#define MAPPROPERTIES_H
#include "CommonFiles/CommonFiles.h"
#include "MappingCore/MappingCore.h"
#include "WindowsUI/WindowsUI.h"

class MapPropertiesWindow : public ClassWindow
{
	public:
		MapPropertiesWindow();
		bool CreateThis(HWND hParent, u32 windowId);
		void RefreshWindow();

	protected:
		LRESULT Command(HWND hWnd, WPARAM wParam, LPARAM lParam);
		LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:
		bool refreshing;
		void CheckReplaceMapTitle();
		void CheckReplaceMapDescription();

		bool possibleTitleUpdate;
		bool possibleDescriptionUpdate;

		ButtonControl buttonApply;
		EditControl editMapTitle;
		EditControl editMapDescription;
		EditControl editMapWidth;
		EditControl editMapHeight;
		GroupBoxControl groupMapPlayers[12];
		TextControl textMapTitle;
		TextControl textMapDescription;
		TextControl textMapTileset;
		DropdownControl dropMapTileset;
		TextControl textNewMapTerrain;
		DropdownControl dropNewMapTerrain;
		TextControl textNewMapWidth;
		TextControl textNewMapHeight;
		TextControl textPlayerOwner[12];
		DropdownControl dropPlayerOwner[12];
		TextControl textPlayerRace[12];
		DropdownControl dropPlayerRaces[12];
		TextControl textPlayerColor[8];
		DropdownControl dropPlayerColor[8];
};

#endif