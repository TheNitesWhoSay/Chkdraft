#ifndef LOCATION_H
#define LOCATION_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"
#include "Windows UI/WindowsUI.h"

class LocationWindow : public ClassWindow
{
	public:
		LocationWindow();
		bool CreateThis(HWND hParent);
		bool DestroyThis();

	protected:
		void RefreshLocationElevationFlags(ChkLocation* locRef, HWND hWnd);
		LRESULT RefreshLocationInfo(HWND hWnd);
		BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:
		bool initializing;
		u32 preservedStat;
		u16 locProcLocIndex;
};

#endif