#include "MapFile.h"
#include "FileIO.h"
#include <cstdio>
#include <cstdarg>

MapFile::MapFile() : saveType(SaveType::Unknown)
{
    filePath[0] = '\0';
}

bool MapFile::LoadFile(const char* &path)
{
    if ( path != nullptr )
        std::strncpy(filePath, path, MAX_PATH); // Copy the given path to the map's stored filePath
    else if ( GetPath() )
        path = filePath; // Have the pointer point to the new filePath

    return filePath != nullptr && OpenFile();
}

bool MapFile::LoadFile()
{
    return GetPath() && OpenFile();
}

bool MapFile::SaveFile(bool SaveAs)
{
    std::string prevFilePath(filePath);

    if ( isProtected() )
        MessageBox(NULL, "Cannot save protected maps!", "Error!", MB_OK|MB_ICONEXCLAMATION);
    else
    {
        if ( SaveAs || filePath[0] == '\0' ) // SaveAs specified or filePath not yet determined
        {
            OPENFILENAME ofn = GetScSaveOfn(filePath);
            if ( GetSaveFileName(&ofn) )
            {
                saveType = (SaveType)ofn.nFilterIndex;

                char* ext = std::strrchr(filePath, '.'); // Find the last occurrence of '.'
                if ( ext == nullptr ) // No extension specified, need to add
                {
                    if ( saveType == SaveType::StarCraftScm || saveType == SaveType::HybridScm )
                        std::strcat(filePath, ".scm");
                    else if ( saveType == SaveType::ExpansionScx || saveType == SaveType::AllMaps )
                        std::strcat(filePath, ".scx");
                    else if ( saveType == SaveType::StarCraftChk || saveType == SaveType::HybridChk ||
                        saveType == SaveType::ExpansionChk )
                    {
                        std::strcat(filePath, ".chk");
                    }
                }
                else // Extension specified, give it precedence over filterIndex
                {
                    if ( std::strcmp(ext, ".chk") == 0 && (saveType == SaveType::StarCraftScm || saveType == SaveType::HybridScm ||
                        saveType == SaveType::ExpansionScx) )
                    {
                        saveType = SaveType::HybridChk;
                    }
                    else if ( std::strcmp(ext, ".scm") == 0 && saveType != SaveType::StarCraftScm )
                        saveType = SaveType::HybridScm;
                    else if ( std::strcmp(ext, ".scx") == 0 )
                        saveType = SaveType::ExpansionScx;
                    else if ( saveType == SaveType::AllMaps )
                        saveType = SaveType::ExpansionScx;
                }
            }
        }

        if ( filePath[0] != '\0' ) // Map for sure has a path
        {
            std::FILE* pFile(nullptr);

            if ( saveType == SaveType::StarCraftScm || saveType == SaveType::StarCraftChk ) // StarCraft Map, edit to match
                ChangeToScOrig();
            else if ( saveType == SaveType::HybridScm || saveType == SaveType::HybridChk ) // Hybrid Map, edit to match
                ChangeToHybrid();
            else if ( saveType == SaveType::ExpansionScx || saveType == SaveType::ExpansionChk ||
                saveType == SaveType::AllMaps ) // BroodWar Map, edit to match
            {
                ChangeToScExp();
            }

            if ( (saveType == SaveType::StarCraftScm || saveType == SaveType::HybridScm || saveType == SaveType::ExpansionScx)
                || saveType == SaveType::AllMaps ) // Must be packed into an MPQ
            {
                if ( !SaveAs || (SaveAs && MakeFileCopy(prevFilePath, filePath)) )
                {
                    DeleteFileA("chk.tmp"); // Remove any existing chk.tmp files
                    pFile = std::fopen("chk.tmp", "wb");
                    WriteFile(pFile);
                    std::fclose(pFile);

                    MPQHANDLE hMpq = MpqOpenArchiveForUpdate(filePath, MOAU_OPEN_EXISTING | MOAU_MAINTAIN_LISTFILE, 1000);
                    if ( hMpq != NULL && hMpq != INVALID_HANDLE_VALUE )
                    {
                        BOOL addedFile = MpqAddFileToArchive(hMpq, "chk.tmp", "staredit\\scenario.chk", MAFA_COMPRESS | MAFA_REPLACE_EXISTING);
                        MpqCloseUpdatedArchive(hMpq, 0);
                        if ( addedFile == TRUE )
                        {
                            DeleteFileA("chk.tmp");
                            return true;
                        }
                        else
                            MessageBox(NULL, "Failed to add file!", "Error!", MB_OK | MB_ICONEXCLAMATION);
                    }
                    else
                        MessageBox(NULL, std::string(std::string("Failed to open for updates!\n\nThe file may be in use elsewhere. ") + std::to_string(GetLastError())).c_str(), "Error!", MB_OK | MB_ICONEXCLAMATION);
                }
                MessageBox(NULL, "Failed to create the new MPQ file!", "Error!", MB_OK | MB_ICONEXCLAMATION);

                DeleteFileA("chk.tmp");
            }
            else // Is a chk file or unrecognized format, write out chk file
            {
                DeleteFileA(filePath); // Remove any existing files of the same name
                pFile = std::fopen(filePath, "wb");
                WriteFile(pFile);
                std::fclose(pFile);
                return true;
            }
        }
    }
    return false;
}

bool MapFile::GetPath()
{
    OPENFILENAME ofn = { };
    char szFileName[MAX_PATH] = { };
    char initPath[MAX_PATH] = { };

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.lpstrFilter = "All Maps\0*.scm;*.scx;*.chk\0Starcraft Maps\0*.scm\0Broodwar Maps\0*.scx\0Raw Scenario Files\0*.chk\0";
    ofn.lpstrFile = szFileName;

    if ( GetRegScPath(initPath, MAX_PATH) )
    {
        std::strcat(initPath, "\\Maps");
        ofn.lpstrInitialDir = initPath;
    }
    else
        ofn.lpstrInitialDir = NULL;

    ofn.hwndOwner = NULL;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

    if ( GetOpenFileName(&ofn) == TRUE )
    {
        std::strncpy(filePath, szFileName, MAX_PATH);
        return true;
    }
    else if ( CommDlgExtendedError() != 0 )
        CHKD_ERR("Error Retrieving File Name.");

    return false;
}

bool MapFile::OpenFile()
{
    if ( filePath != nullptr )
    {
        buffer chk("oMAP");

        char* ext = strrchr(filePath, '.'); // Find the last occurrence of '.'
        if ( ext != nullptr )
        {
            if ( strcmp(ext, ".scm") == 0 || strcmp(ext, ".scx") == 0 )
            {
                MPQHANDLE hMpq = MpqOpenArchiveForUpdate(filePath, MOAU_OPEN_EXISTING|MOAU_READ_ONLY, 1000);
                if ( hMpq != NULL && hMpq != INVALID_HANDLE_VALUE )
                {
                    if ( !FileToBuffer(hMpq, "staredit\\scenario.chk", chk) )
                        CHKD_ERR("Failed to get scenario file from MPQ.");

                    MpqCloseUpdatedArchive(hMpq, 0);
                    

                    if ( chk.size() > 0 && ParseScenario(chk) )
                    {
                        if ( HasVerSection() )
                        {
                            if ( IsScOrig() )
                                saveType = SaveType::StarCraftScm; // Vanilla
                            else if ( IsHybrid() )
                                saveType = SaveType::HybridScm; // Hybrid
                            else
                                saveType = SaveType::ExpansionScx; // Expansion
                        }
                        else if ( strcmp(ext, ".scx") == 0 )
                            saveType = SaveType::ExpansionScx; // Expansion
                        else if ( true ) // Could search for clues to map version here
                            saveType = SaveType::ExpansionChk; // Otherwise set to expansion to prevent data loss

                        return true;
                    }
                    else
                        CHKD_ERR("Invalid or missing Scenario file.");
                }
                else if ( GetLastError() == ERROR_FILE_NOT_FOUND )
                    CHKD_ERR("File Not Found");
                else if ( GetLastError() == MPQ_ERROR_MPQ_INVALID )
                    CHKD_ERR("Invalid or corrupt MPQ file");
                else
                    CHKD_ERR("%d", GetLastError());
            }
            else if ( strcmp(ext, ".chk") == 0 )
            {
                if ( FileToBuffer(filePath, chk) )
                {
                    if ( ParseScenario(chk) )
                    {
                        if ( HasVerSection() )
                        {
                            if ( IsScOrig() )
                                saveType = SaveType::StarCraftChk; // Vanilla chk
                            else if ( IsHybrid() )
                                saveType = SaveType::HybridChk; // Hybrid chk
                            else
                                saveType = SaveType::ExpansionChk; // Expansion chk
                        }
                        else if ( true ) // Could search for clues to map version here
                            saveType = SaveType::ExpansionChk; // Otherwise set to expansion to prevent data loss

                        return true;
                    }
                    else
                        CHKD_ERR("Parsing Failed!");
                }
                else
                    CHKD_ERR("Error Reading CHK File");
            }
        }
    }
    return false;
}

bool MapFile::SetPath(const char* newPath)
{
    if ( std::strlen(newPath) < FILENAME_MAX )
    {
        std::strcpy(filePath, newPath);
        return true;
    }
    return false;
}

const char* MapFile::FilePath()
{
    return filePath;
}

void MapFile::SetSaveType(SaveType newSaveType)
{
    saveType = newSaveType;
}

std::string MapFile::GetFileName()
{
    std::string sFilePath(filePath);
    auto lastBackslashPos = sFilePath.find_last_of('\\');

    if ( lastBackslashPos != std::string::npos && lastBackslashPos+1 < sFilePath.size() )
        return std::string(sFilePath.substr(lastBackslashPos+1, sFilePath.size() - lastBackslashPos+1));
    else
        return std::string("");
}

std::string MapFile::GetFilePath()
{
    return std::string(filePath);
}
