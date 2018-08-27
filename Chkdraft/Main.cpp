#include "../ChkdraftLib/Chkdraft.h"

#ifdef SHOW_CLI
CLI cli; // Open a command line interface that works with cin and cout
#endif

Chkdraft chkd; // The main instance of Chkdraft

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    int result = chkd.Run(lpCmdLine, nCmdShow); // Forward the run command line params and the show options
    return result;
}
