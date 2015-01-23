#ifndef COUNTERS_H
#define COUNTERS_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"
#include "Windows UI/WindowsUI.h"

class CountersWindow : public ClassWindow
{
	public:
		bool CreateThis(HWND hParent);
		bool DestroyThis();
		void RefreshWindow();
		void DoSize();

	protected:
		void CreateSubWindows(HWND hWnd);

	private:
};

#endif