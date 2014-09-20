#ifndef FORCES_H
#define FORCES_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"
#include "Windows UI/WindowsUI.h"

class ForcesWindow : public ClassWindow
{
	public:
		ForcesWindow();
		bool CreateThis(HWND hParent);
		bool DestroyThis();

	private:
		LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		void CheckReplaceForceName(int force);

		bool possibleForceNameUpdate[4];
		u8 playerBeingDragged;
};

#endif