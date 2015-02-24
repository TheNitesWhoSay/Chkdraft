#ifndef FORCES_H
#define FORCES_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"
#include "Windows UI/WindowsUI.h"

class ForcesWindow : public ClassWindow
{
	public:
		ForcesWindow();
		bool CreateThis(HWND hParent, u32 windowId);
		bool DestroyThis();
		void RefreshWindow();

	private:
		LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
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