#ifndef MAINTOOLBAR_H
#define MAINTOOLBAR_H
#include "Windows UI/WindowsUI.h"
#include "Common Files/CommonFiles.h"

class MainToolbar : public ToolbarControl
{
	public:
		DropdownControl layerBox;
		DropdownControl zoomBox;
		DropdownControl playerBox;
		DropdownControl terrainBox;

		bool CreateThis(HWND hParent);
		void AutoSize();

	private:

};

#endif