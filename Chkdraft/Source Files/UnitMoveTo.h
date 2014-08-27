#ifndef UNITMOVETO_H
#define UNITMOVETO_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"
#include "Windows UI/WindowsUI.h"

class UnitMoveTo : public ClassWindow
{
	public:
		UnitMoveTo();
		s32 GetMoveToIndex(HWND hParent);

	protected:
		BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:
		s32 unitMoveTo;
};

#endif