#include "FileIO.h"
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <string>

void MakeDirectory(std::string directory)
{
    _mkdir(directory.c_str());
}

bool GetModuleDirectory(std::string &outModuleDirectory)
{
    char cModulePath[MAX_PATH] = {};
    if ( GetModuleFileName(NULL, cModulePath, MAX_PATH) != MAX_PATH )
    {
        std::string modulePath(cModulePath);
        auto lastBackslashPos = modulePath.find_last_of('\\');
        if ( lastBackslashPos != std::string::npos && lastBackslashPos < modulePath.size() )
        {
            outModuleDirectory = modulePath.substr(0, lastBackslashPos);
            return true;
        }
    }
    return false;
}

bool FindFile(const char* filePath)
{
    if ( filePath != nullptr )
    {
        FILE* file = std::fopen(filePath, "r");
        if ( file != nullptr )
        {
            std::fclose(file);
            return true;
        }
    }
    return false;
}

bool FindFileInMpq(MPQHANDLE mpq, const char* fileName)
{
    if ( mpq == nullptr )
        CHKD_ERR("NULL MPQ file specified for opening %s", fileName);
    else
    {
        u32 bytesRead = 0;
        HANDLE openFile = NULL;
        if ( SFileGetFileInfo(mpq, SFILE_INFO_NUM_FILES) != 0xFFFFFFFF )
        {
            if ( SFileOpenFileEx(mpq, fileName, SFILE_SEARCH_CURRENT_ONLY, &openFile) )
                SFileCloseFile(openFile);
            else
                CHKD_ERR("Failed to get %s from MPQ file", fileName);
        }
        else
            CHKD_ERR("File is already open", fileName);
    }
    return false;
}

bool FindFileInMpq(const char* mpqPath, const char* fileName)
{
    bool success = false;
    MPQHANDLE mpq = NULL;
    if ( OpenArchive(mpqPath, mpq) )
    {
        success = FindFileInMpq(mpq, fileName);
        CloseArchive(mpq);
    }
    return success;
}

bool PatientFindFile(const char* filePath, int numWaitTimes, int* waitTimes)
{
    if ( FindFile(filePath) )
        return true;

    for ( int i = 0; i < numWaitTimes; i++ )
    {
        Sleep(waitTimes[i]);
        if ( FindFile(filePath) )
            return true;
    }

    return false;
}

bool OpenArchive(const char* mpqPath, MPQHANDLE &mpq)
{
    BOOL success = FALSE;
    char lpStrMpqPath[MAX_PATH + 100],
         locateError[MAX_PATH + 60],
         retryError[MAX_PATH + 76];

    strncpy(lpStrMpqPath, mpqPath, strlen(mpqPath));

    std::snprintf(locateError, MAX_PATH + 60, "Could not find %s!\n\nWould you like to locate it manually?", mpqPath);
    std::snprintf(retryError, MAX_PATH + 76, "Failed to open %s! The file may be in use.\n\nWould you like to try again?", mpqPath);

    if ( FindFile(mpqPath) ) // File found
    {
        do { success = SFileOpenArchive((LPCSTR)mpqPath, 0, 0, &mpq); }
        while ( success == FALSE && MessageBox(NULL, retryError, "Error!", MB_YESNO | MB_ICONEXCLAMATION) == IDYES );
    }
    else // File not found
    {
        if ( MessageBox(NULL, locateError, "Error!", MB_YESNO | MB_ICONEXCLAMATION) == IDYES )
        {
            OPENFILENAME ofn;
            memset(&ofn, 0, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = NULL;
            ofn.lpstrFilter = "MPQ Files\0*.mpq\0All Files\0*.*\0";
            ofn.lpstrFile = lpStrMpqPath;
            ofn.nMaxFile = MAX_PATH;
            ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

            if ( GetOpenFileName(&ofn) )
            {
                do { success = SFileOpenArchive(mpqPath, 0, 0, &mpq); }
                while ( success == FALSE && MessageBox(NULL, retryError, "Error!", MB_YESNO | MB_ICONEXCLAMATION) == IDYES );
            }
            else
                return OpenArchive(mpqPath, mpq);
        }
    }
    return success == TRUE;
}

bool OpenArchive(const char* directory, const char* fileName, MPQHANDLE &hMpq, std::string &outPath)
{
    BOOL success = FALSE;
    char filePath[MAX_PATH],
        locateError[MAX_PATH + 60],
        retryError[MAX_PATH + 76];

    std::snprintf(filePath, MAX_PATH, "%s\\%s", directory, fileName);
    std::snprintf(locateError, MAX_PATH+60, "Could not find %s!\n\nWould you like to locate it manually?", fileName);
    std::snprintf(retryError, MAX_PATH+76, "Failed to open %s! The file may be in use.\n\nWould you like to try again?", fileName);

    if ( FindFile(filePath) ) // File found
    {
        do { success = SFileOpenArchive((LPCSTR)filePath, 0, 0, &hMpq); }
        while ( success == FALSE && MessageBox(NULL, retryError, "Error!", MB_YESNO|MB_ICONEXCLAMATION) == IDYES );
        if ( success == TRUE )
            outPath = filePath;
    }
    else // File not found
    {
        if ( MessageBox(NULL, locateError, "Error!", MB_YESNO|MB_ICONEXCLAMATION) == IDYES )
        {
            OPENFILENAME ofn;
            memset(&ofn, 0, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = NULL;
            ofn.lpstrFilter = "MPQ Files\0*.mpq\0All Files\0*.*\0";
            ofn.lpstrFile = filePath;
            ofn.nMaxFile = MAX_PATH;
            ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
    
            if ( GetOpenFileName(&ofn) )
            {
                do { success = SFileOpenArchive(filePath, 0, 0, &hMpq); }
                while ( success == FALSE && MessageBox(NULL, retryError, "Error!", MB_YESNO|MB_ICONEXCLAMATION) == IDYES );
                if ( success == TRUE )
                    outPath = filePath;
            }
            else
                return OpenArchive(directory, fileName, hMpq, outPath);
        }
    }
    return success == TRUE;
}

bool CloseArchive(MPQHANDLE mpq)
{
    return SFileCloseArchive(mpq) == TRUE;
}

bool FileToBuffer(HANDLE &hMpq, const char* fileName, buffer &buf)
{
    if ( hMpq == nullptr )
        CHKD_ERR("NULL MPQ file specified for opening %s", fileName);
    else
    {
        u32 bytesRead = 0;
        HANDLE openFile = NULL;
        if ( SFileGetFileInfo(hMpq, SFILE_INFO_NUM_FILES) != 0xFFFFFFFF )
        {
            if ( SFileOpenFileEx(hMpq, fileName, SFILE_SEARCH_CURRENT_ONLY, &openFile) )
            {
                u32 fileSize = (u32)SFileGetFileSize(openFile, NULL);
                if ( buf.setSize(fileSize) )
                {
                    buf.sizeUsed = fileSize;
                    SFileReadFile(openFile, (LPVOID)buf.data, buf.sizeUsed, (LPDWORD)(&bytesRead), NULL);
                    SFileCloseFile(openFile);

                    if ( buf.sizeUsed == bytesRead )
                        return true;
                }
                else
                    SFileCloseFile(openFile);
            }
            else
                CHKD_ERR("Failed to get %s from MPQ file", fileName);
        }
        else
            CHKD_ERR("File is already open", fileName);
    }
    return false;
}

bool FileToBuffer(MPQHANDLE &hStarDat, MPQHANDLE &hBrooDat, MPQHANDLE &hPatchRt, MPQHANDLE &hPriority, const char* fileName, buffer &buf)
{
    return ( hPriority != nullptr && FileToBuffer(hPriority, fileName, buf) )
           || FileToBuffer(hPatchRt, fileName, buf)
           || FileToBuffer(hBrooDat, fileName, buf)
           || FileToBuffer(hStarDat, fileName, buf);
}

bool FileToBuffer(MPQHANDLE &hStarDat, MPQHANDLE &hBrooDat, MPQHANDLE &hPatchRt, const char* fileName, buffer &buf)
{
    return    FileToBuffer(hPatchRt, fileName, buf)
           || FileToBuffer(hBrooDat, fileName, buf)
           || FileToBuffer(hStarDat, fileName, buf);
}

bool FileToString(std::string fileName, std::string &str)
{
    try {
        str.clear();
        std::ifstream file(fileName, std::ifstream::in | std::ifstream::ate); // Open at ending characters position
        if ( file.is_open() )
        {
            auto size = file.tellg(); // Grab size via current position
            str.reserve((size_t)size); // Set string size to file size
            file.seekg(0); // Move reader to beggining of file
            while ( !file.eof() )
                str += file.get();

            if ( str.length() > 0 && str[str.length() - 1] == (char)-1 )
                str[str.length() - 1] = '\0';

            return true;
        }
    }
    catch ( std::exception ) { }
    return false;
}

bool MakeFileCopy(const std::string &inFilePath, const std::string &outFilePath)
{
    bool success = false;
    std::ifstream inFile;
    std::ofstream outFile;
    try
    {
        inFile.open(inFilePath, std::fstream::binary);
        if ( inFile.is_open() )
        {
            outFile.open(outFilePath, std::fstream::out|std::fstream::binary);
            if ( outFile.is_open() )
            {
                outFile << inFile.rdbuf();
                success = true;
                outFile.close();
            }
            inFile.close();
        }
    }
    catch ( std::exception ) { }

    if ( inFile.is_open() )
        inFile.close();

    if ( outFile.is_open() )
        outFile.close();

    return success;
}

void RemoveFile(std::string fileName)
{
    std::remove(fileName.c_str());
}

void RemoveFiles(std::string firstFileName, std::string secondFileName)
{
    std::remove(firstFileName.c_str());
    std::remove(secondFileName.c_str());
}

void RemoveFiles(std::string firstFileName, std::string secondFileName, std::string thirdFileName)
{
    std::remove(firstFileName.c_str());
    std::remove(secondFileName.c_str());
    std::remove(thirdFileName.c_str());
}

OPENFILENAME GetOfn(char* szFileName, const char* filter, int initFilter)
{
    OPENFILENAME ofn = { };
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFilter = filter;
    ofn.lpstrFile = szFileName;
    ofn.nFilterIndex = initFilter;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
    return ofn;
}

OPENFILENAME GetScSaveOfn(char* szFileName)
{
    return GetOfn(
        szFileName,
        "Starcraft Map(*.scm)\0*.scm\0Starcraft Hybrid Map(*.scm)\0*.scm\0Broodwar Map(*.scx)\0*.scx\0Raw Starcraft Map(*.chk)\0*.chk\0Raw Starcraft Hybrid Map(*.chk)\0*.chk\0Raw Broodwar Map(*.chk)\0*.chk\0All Maps\0*.scm;*.scx;*.chk\0",
        7 );
}

bool FileToBuffer(const char* FileName, buffer &buf)
{
    bool success = false;
    if ( FileName != nullptr )
    {
        FILE* pFile = std::fopen(FileName, "rb");
        if ( pFile != nullptr )
        {
            buf.flush();
            std::fseek(pFile, 0, SEEK_END);
            u32 fileSize = (u32)std::ftell(pFile);
            if ( buf.setSize(fileSize) )
            {
                buf.sizeUsed = fileSize;
                std::rewind(pFile);
                size_t lengthRead = std::fread(buf.data, 1, buf.sizeUsed, pFile);
                success = (lengthRead == buf.sizeUsed);
            }
            std::fclose(pFile);
        }
    }
    return success;
}

DWORD GetSubKeyString(HKEY hParentKey, const char* subKey, const char* valueName, char* data, DWORD* dataSize)
{
    HKEY hKey = NULL;

    DWORD errorCode = RegOpenKeyEx(hParentKey, subKey, 0, KEY_QUERY_VALUE, &hKey); // Open key
    if ( errorCode == ERROR_SUCCESS ) // Open key success
    {
        errorCode = RegQueryValueEx(hKey, valueName, NULL, NULL, (LPBYTE)data, dataSize); // Read Key
        RegCloseKey(hKey); // Close key
    }
    return errorCode; // Return success/error message
}

bool GetRegString(char* dest, u32 destSize, const char* subKey, const char* valueName)
{
    DWORD lpcbData = (DWORD)destSize;

    DWORD errorCode = GetSubKeyString(HKEY_CURRENT_USER, subKey, valueName, dest, &lpcbData); // Try HKCU
    if ( errorCode != ERROR_SUCCESS ) // Not found in HKCU
        errorCode = GetSubKeyString(HKEY_LOCAL_MACHINE, subKey, valueName, dest, &lpcbData); // Try HKLM

    return errorCode == ERROR_SUCCESS;
}

bool GetRegScPath(char* dest, u32 destSize)
{
    return false; // TODO remove me
    return GetRegString(dest, destSize, "SOFTWARE\\Blizzard Entertainment\\Starcraft", "InstallPath");
}
