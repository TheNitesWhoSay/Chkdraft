#include "Settings.h"

bool GetChkdPath(std::string &outChkdPath)
{
    std::string moduleDirectory;
    if ( GetModuleDirectory(moduleDirectory) )
    {
        MakeDirectory(moduleDirectory + "\\chkd");
        outChkdPath = moduleDirectory + std::string("\\chkd");
        return true;
    }
    return false;
}

bool GetPreSavePath(std::string &outPreSavePath)
{
    std::string chkdPath;
    if ( GetChkdPath(chkdPath) )
    {
        MakeDirectory(chkdPath + "\\Pre-Save");
        outPreSavePath = chkdPath + std::string("\\Pre-Save\\");
        return true;
    }
    return false;
}

bool GetLoggerPath(std::string &outLoggerPath)
{
    std::string chkdPath;
    if ( GetChkdPath(chkdPath) )
    {
        MakeDirectory(chkdPath + "\\Logs");
        outLoggerPath = chkdPath + std::string("\\Logs\\");
        return true;
    }
    return false;
}
