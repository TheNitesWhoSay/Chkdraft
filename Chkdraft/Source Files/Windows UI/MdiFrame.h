#ifndef MDIFRAME_H
#define MDIFRAME_H
#include <Windows.h>

class MdiFrame
{
	public:
		MdiFrame();
		bool CreateMdiFrame( HANDLE hWindowMenu, UINT idFirstChild,
							 DWORD dwExStyle, LPCSTR lpWindowName, DWORD dwStyle,
							 int X, int Y, int nWidth, int nHeight, HWND hParent, HMENU hMenu );
		HWND getHandle();
		HWND getActive();
		void cascade();
		void tileHorizontally();
		void tileVertically();
		void destroyActive();
		void nextMdi();

	private:
		HWND hWnd;
};

#endif