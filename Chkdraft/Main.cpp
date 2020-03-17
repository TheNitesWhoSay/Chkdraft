#include "../ChkdraftLib/Chkdraft.h"
#include "../ChkdraftLib/Mapping/Settings.h"

#ifdef _DEBUG
Logger logger(LogLevel::All); // The primary logger
#else
Logger logger((LogLevel)Settings::getLogLevel()); // The primary logger
#endif


Chkdraft chkd; // The main instance of Chkdraft

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    int result = chkd.Run(lpCmdLine, nCmdShow); // Forward the run command line params and the show options
    return result;
}
