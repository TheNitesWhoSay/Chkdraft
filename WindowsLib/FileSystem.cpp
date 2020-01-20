#include "FileSystem.h"
#include <SimpleIcu.h>
#include <Windows.h>

namespace WinLib
{

    std::string GetCurrentExePath()
    {
        icux::codepoint currExePath[MAX_PATH] = {};
        if ( GetModuleFileName(NULL, currExePath, MAX_PATH) != MAX_PATH )
            return std::string(icux::toUtf8(currExePath));
        else
            return std::string("");
    }

    bool executeOpen(const std::string & filePath, int & resultCode)
    {
        resultCode = (int)(s64)ShellExecute(NULL, icux::toFilestring("open").c_str(), icux::toFilestring(filePath).c_str(), NULL, NULL, SW_SHOWDEFAULT) > 32;
        return resultCode > 32;
    }

    bool executeOpen(const std::string & file, const std::string & directory, int & resultCode)
    {
        resultCode = (int)(s64)ShellExecute(NULL, icux::toFilestring("open").c_str(), icux::toFilestring(file).c_str(), NULL, icux::toFilestring(directory).c_str(), SW_SHOWDEFAULT);
        return resultCode > 32;
    }

}
