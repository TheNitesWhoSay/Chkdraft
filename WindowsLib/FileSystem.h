#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#include <string>
#include <map>
#include "DataTypes.h"

namespace WinLib
{
    std::string GetCurrentExePath();

    bool executeOpen(const std::string & filePath, int & resultCode);

    bool executeOpen(const std::string & file, const std::string & directory, int & resultCode);

    bool BrowseForFile(std::string & filePath, const std::map<std::string, std::string> & filterToLabel, u32 filterIndex, const std::string & initialDirectory, const std::string & title,
        bool pathMustExist, bool provideOverwritePrompt);
}

#endif