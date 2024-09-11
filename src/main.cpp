#include <chkdraft/chkdraft.h>
#include <chkdraft/mapping/settings.h>

#ifdef _DEBUG
Logger logger(LogLevel::All); // The primary logger
#else
Logger logger((LogLevel)Settings::getLogLevel()); // The primary logger
#endif


Chkdraft chkd; // The main instance of Chkdraft

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#ifdef _DEBUG
    return chkd.Run(lpCmdLine, nCmdShow); // Forward the run command line params and the show options
#else
    int result = 0;
    try {
        result = chkd.Run(lpCmdLine, nCmdShow); // Forward the run command line params and the show options
    } catch ( std::exception & e ) {
        logger.fatal() << "Unhandled exception: " << e.what() << std::endl;
    }
    return result;
#endif
}
