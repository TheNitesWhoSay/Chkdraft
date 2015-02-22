#ifndef CUWPS_H
#define CUWPS_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"
#include "Windows UI/WindowsUI.h"

class CUWPsWindow : public ClassWindow
{
	public:
		bool CreateThis(HWND hParent, u32 windowId);
		bool DestroyThis();
		void RefreshWindow();
		void DoSize();

	protected:
		void CreateSubWindows(HWND hWnd);

	private:
};

#endif