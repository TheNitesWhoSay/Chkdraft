#include "FileIO.h"
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <string>
#include <experimental/filesystem>

std::string GetSystemFileSeparator()
{
    wchar_t wideCharSeparator = std::experimental::filesystem::path::preferred_separator;
    char multiByteSeparator = (char)wideCharSeparator;
    char multiByteSeparatorArray[2] = {multiByteSeparator, '\0'};
    return std::string(multiByteSeparatorArray);
}

std::string GetSystemFileName(const std::string &systemFilePath)
{
    std::string systemFileSeparator = GetSystemFileSeparator();
    size_t lastSeparator = systemFilePath.find_last_of(systemFileSeparator);
    if ( lastSeparator >= 0 && lastSeparator < systemFilePath.length()-systemFileSeparator.length() )
        return systemFilePath.substr(lastSeparator+systemFileSeparator.length(), systemFilePath.length());
    else
        return systemFilePath;
}

std::string GetSystemFileDirectory(const std::string &systemFilePath, bool includeSeparator)
{
    std::string systemFileSeparator = GetSystemFileSeparator();
    size_t lastSeparator = systemFilePath.find_last_of(systemFileSeparator);
    if ( lastSeparator >= 0 && lastSeparator < systemFilePath.length() )
    {
        if ( includeSeparator )
            return systemFilePath.substr(0, lastSeparator+systemFileSeparator.length());
        else
            return systemFilePath.substr(0, lastSeparator);
    }
    return systemFilePath;
}

std::string MakeSystemFilePath(const std::string &systemDirectory, const std::string &fileName)
{
    std::string systemFileSeparator = GetSystemFileSeparator();
    size_t lastSeparator = systemDirectory.find_last_of(systemFileSeparator);
    if ( lastSeparator == systemDirectory.length() - systemFileSeparator.length() )
        return systemDirectory + fileName;
    else
        return systemDirectory + systemFileSeparator + fileName;
}

std::string GetMpqFileSeparator()
{
    return "\\";
}

std::string GetMpqFileName(const std::string &mpqFilePath)
{
    std::string mpqFileSeparator = GetMpqFileSeparator();
    size_t lastSeparator = mpqFilePath.find_last_of(mpqFileSeparator);
    if ( lastSeparator >= 0 && lastSeparator < mpqFilePath.length()-mpqFileSeparator.length() )
        return mpqFilePath.substr(lastSeparator+mpqFileSeparator.length(), mpqFilePath.length());
    else
        return mpqFilePath;
}

std::string MakeMpqFilePath(const std::string &mpqDirectory, const std::string &fileName)
{
    std::string mpqFileSeparator = GetMpqFileSeparator();
    size_t lastSeparator = mpqDirectory.find_last_of(mpqFileSeparator);
    if ( lastSeparator == mpqDirectory.length() - mpqFileSeparator.length() )
        return mpqDirectory + fileName;
    else
        return mpqDirectory + mpqFileSeparator + fileName;
}

bool MakeDirectory(std::string directory)
{
    return _mkdir(directory.c_str()) == 0;
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

bool RemoveFile(const char* filePath)
{
    // Return whether the file with the given filePath is not on the system
    return filePath == nullptr || strlen(filePath) == 0 || remove(filePath) == 0 || !FindFile(filePath);
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
            if ( SFileOpenFileEx(mpq, fileName, SFILE_SEARCH_CURRENT_ONLY, &openFile) == TRUE )
            {
                SFileCloseFile(openFile);
                return true;
            }
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

    std::string separator = GetSystemFileSeparator();
    std::snprintf(filePath, MAX_PATH, "%s%s%s", directory, separator.c_str(), fileName);
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

bool BufferToArchive(MPQHANDLE &hMpq, const buffer &buf, const std::string &mpqFilePath)
{
    if ( hMpq == nullptr )
        CHKD_ERR("NULL MPQ file specified for writing buffer");
    else
    {
        DWORD dataSize = (DWORD)buf.size();
        LPVOID dataPointer = (LPVOID)buf.getPtr(0);
        if ( MpqAddFileFromBuffer(hMpq, dataPointer, dataSize, mpqFilePath.c_str(), MAFA_COMPRESS | MAFA_REPLACE_EXISTING) == TRUE )
            return true;
        else
            CHKD_ERR("Failed to add buffered file to archive");
    }
    return false;
}

bool WavBufferToArchive(MPQHANDLE &hMpq, const buffer &buf, const std::string &mpqFilePath, WavQuality wavQuality)
{
    if ( hMpq == nullptr )
        CHKD_ERR("NULL MPQ file specified for writing WAV buffer");
    else
    {
        DWORD dwWavQuality = (DWORD)wavQuality;
        DWORD dataSize = (DWORD)buf.size();
        LPVOID dataPointer = (LPVOID)buf.getPtr(0);
        if ( MpqAddWaveFromBuffer(hMpq, dataPointer, dataSize, mpqFilePath.c_str(), MAFA_COMPRESS | MAFA_REPLACE_EXISTING, dwWavQuality) == TRUE )
            return true;
        else
            CHKD_ERR("Failed to add buffered file to archive");
    }
    return false;
}

bool FileToBuffer(MPQHANDLE &hMpq, const std::string &fileName, buffer &buf)
{
    if ( hMpq == nullptr )
        CHKD_ERR("NULL MPQ file specified for opening %s", fileName.c_str());
    else
    {
        u32 bytesRead = 0;
        HANDLE openFile = NULL;
        if ( SFileGetFileInfo(hMpq, SFILE_INFO_NUM_FILES) != 0xFFFFFFFF )
        {
            if ( SFileOpenFileEx(hMpq, fileName.c_str(), SFILE_SEARCH_CURRENT_ONLY, &openFile) )
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
                CHKD_ERR("Failed to get %s from MPQ file", fileName.c_str());
        }
        else
            CHKD_ERR("File is already open", fileName.c_str());
    }
    return false;
}

bool FileToBuffer(MPQHANDLE &hStarDat, MPQHANDLE &hBrooDat, MPQHANDLE &hPatchRt, MPQHANDLE &hPriority, const std::string &fileName, buffer &buf)
{
    return ( hPriority != nullptr && FileToBuffer(hPriority, fileName, buf) )
           || FileToBuffer(hPatchRt, fileName, buf)
           || FileToBuffer(hBrooDat, fileName, buf)
           || FileToBuffer(hStarDat, fileName, buf);
}

bool FileToBuffer(MPQHANDLE &hStarDat, MPQHANDLE &hBrooDat, MPQHANDLE &hPatchRt, const std::string &fileName, buffer &buf)
{
    return    FileToBuffer(hPatchRt, fileName, buf)
           || FileToBuffer(hBrooDat, fileName, buf)
           || FileToBuffer(hStarDat, fileName, buf);
}

bool FileToBuffer(const std::string &fileName, buffer &buf)
{
    bool success = false;
    if ( fileName.length() > 0 )
    {
        FILE* pFile = std::fopen(fileName.c_str(), "rb");
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

bool FileToString(const std::string &fileName, std::string &str)
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

OPENFILENAME GetWavSaveOfn(char* szFileName)
{
    return GetOfn(
        szFileName,
        "WAV File\0*.wav\0All Files\0*.*\0",
        1 );
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
