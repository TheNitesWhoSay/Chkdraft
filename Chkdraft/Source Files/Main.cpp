#include "Chkdraft.h"

#ifdef CHKD_DEBUG
CLI cli; // Open a command line interface that works with cin and cout
#endif

Chkdraft chkd;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	return chkd.Run(lpCmdLine, nCmdShow);
}
