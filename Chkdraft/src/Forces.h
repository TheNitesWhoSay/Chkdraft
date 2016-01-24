#ifndef FORCES_H
#define FORCES_H
#include "CommonFiles/CommonFiles.h"
#include "MappingCore/MappingCore.h"
#include "WindowsUI/WindowsUI.h"

class ForcesWindow : public ClassWindow
{
	public:
		ForcesWindow();
		bool CreateThis(HWND hParent, u32 windowId);
		bool DestroyThis();
		void RefreshWindow();
		LRESULT Command(HWND hWnd, WPARAM wParam, LPARAM lParam);
		LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:
		void CheckReplaceForceName(int force);

		bool possibleForceNameUpdate[4];
		u8 playerBeingDragged;

		EditControl editForceName[4];
		GroupBoxControl groupForce[4];
		TextControl textAboutForces;
		CheckBoxControl checkAllied[4];
		CheckBoxControl checkSharedVision[4];
		CheckBoxControl checkRandomizeStart[4];
		CheckBoxControl checkAlliedVictory[4];
		DragListControl dragForces[4];
};

#endif