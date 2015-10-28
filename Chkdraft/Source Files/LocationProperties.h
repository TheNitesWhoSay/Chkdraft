#ifndef LOCATIONPROPERITES_H
#define LOCATIONPROPERITES_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"
#include "Windows UI/WindowsUI.h"

class LocationWindow : public ClassDialog
{
	public:
		LocationWindow();
		bool CreateThis(HWND hParent);
		bool DestroyThis();
		void RefreshLocationInfo();

	protected:
		void RefreshLocationElevationFlags(ChkLocation* locRef, HWND hWnd);
		BOOL DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
		BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:
		bool initializing;
		u32 preservedStat;
		u16 locProcLocIndex;

		EditControl editLocName;
		EditControl editLocLeft;
		EditControl editLocTop;
		EditControl editLocRight;
		EditControl editLocBottom;
		EditControl editRawFlags;
};

#endif