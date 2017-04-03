#include "Settings.h"

bool GetPreSavePath(std::string &outPreSavePath)
{
    std::string moduleDirectory;
    if ( GetModuleDirectory(moduleDirectory) )
    {
        MakeDirectory(moduleDirectory + "\\chkd");
        MakeDirectory(moduleDirectory + "\\chkd\\Pre-Save");
        outPreSavePath = moduleDirectory + std::string("\\chkd\\Pre-Save\\");
        return true;
    }
    return false;
}
