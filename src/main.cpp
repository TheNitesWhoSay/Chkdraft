#include <chkdraft/chkdraft.h>
#include <chkdraft/mapping/chkd_profiles.h>

Chkdraft chkd {}; // The main instance of Chkdraft

#ifdef _DEBUG
Logger logger(LogLevel::Debug); // The primary logger
#else
Logger logger((LogLevel)chkd.profiles().logger.defaultLogLevel); // The primary logger
#endif


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
    } catch ( ... ) {
        logger.fatal() << "Unhandled unknown exception" << std::endl;
    }
    return result;
#endif
}
