#include "Chkdraft.h" // Under 'Windows' filter

#ifdef CHKD_DEBUG // If compiling in Debug mode...
CLI cli; // Open a command line interface that works with cin and cout
#endif

Chkdraft chkd; // The main instance of Chkdraft

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	return chkd.Run(lpCmdLine, nCmdShow); // Forward the run command line params and the show options
}
