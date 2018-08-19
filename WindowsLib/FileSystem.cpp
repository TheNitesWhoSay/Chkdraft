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

    bool executeOpen(const std::string &filePath, int &resultCode)
    {
        resultCode = (int)ShellExecute(NULL, icux::toFilestring("open").c_str(), icux::toFilestring(filePath).c_str(), NULL, NULL, SW_SHOWDEFAULT);
        return resultCode > 32;
    }

    bool executeOpen(const std::string &file, const std::string &directory, int &resultCode)
    {
        resultCode = (int)ShellExecute(NULL, icux::toFilestring("open").c_str(), icux::toFilestring(file).c_str(), NULL, icux::toFilestring(directory).c_str(), SW_SHOWDEFAULT);
        return resultCode > 32;
    }

    bool BrowseForFile(output_param std::string &filePath, const std::map<std::string, std::string> &filterToLabel, u32 filterIndex, const std::string &initialDirectory, const std::string &title,
        bool pathMustExist, bool provideOverwritePrompt)
    {
        OPENFILENAME ofn = { };
        icux::codepoint fileNameBuffer[FILENAME_MAX];

        std::string filterString = "";
        for ( auto filterLabel : filterToLabel )
        {
            const std::string &filter = filterLabel.first;
            const std::string &label = filterLabel.second;
            filterString += label + '\0' + filter + '\0';
        }

        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.lpstrFilter = icux::toUistring(filterString).c_str();
        ofn.lpstrFile = fileNameBuffer;

        if ( initialDirectory.empty() )
            ofn.lpstrInitialDir = NULL;
        else
            ofn.lpstrInitialDir = icux::toFilestring(initialDirectory).c_str();

        if ( title.empty() )
            ofn.lpstrTitle = NULL;
        else
            ofn.lpstrTitle = icux::toUistring(title).c_str();

        ofn.hwndOwner = NULL;
        ofn.nMaxFile = FILENAME_MAX;
        DWORD flags = OFN_HIDEREADONLY;
        if ( pathMustExist )
            flags |= OFN_PATHMUSTEXIST;
        if ( provideOverwritePrompt )
            flags |= OFN_OVERWRITEPROMPT;

        ofn.Flags = flags;
        ofn.nFilterIndex = filterIndex;

        bool success = GetOpenFileName(&ofn) == TRUE;
        if ( success )
            filePath = icux::toUtf8(fileNameBuffer);

        return success;
    }

}
