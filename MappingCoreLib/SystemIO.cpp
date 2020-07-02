#include "SystemIO.h"
#include <SimpleIcu.h>
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <string>
#include <filesystem>
#include <memory>
#include <thread>
#include <chrono>
#include <iostream>
#ifdef _WIN32
#include <Windows.h>
#endif

constexpr u32 size_1kb = 0x400;
constexpr u32 size_1gb = 0x40000000;

bool hasExtension(const std::string & systemFilePath, const std::string & extension)
{
    const std::string systemExtensionSeparator = ".";
    bool extensionIncludesSeparator = extension.rfind(systemExtensionSeparator) == 0;
    auto found = systemFilePath.rfind(extensionIncludesSeparator ? extension : systemExtensionSeparator + extension);
    if ( extensionIncludesSeparator )
        return found != std::string::npos && found == systemFilePath.length() - extension.length();
    else
        return found != std::string::npos && found == systemFilePath.length() - extension.length() - systemExtensionSeparator.length();
}

std::string getSystemFileSeparator()
{
    std::filesystem::path::value_type separatorChar = std::filesystem::path::preferred_separator;
    icux::codepoint separatorArray[2] = {(icux::codepoint)separatorChar, icux::nullChar};
    return icux::toUtf8(icux::filestring(separatorArray));
}

std::string getSystemFileName(const std::string & systemFilePath)
{
    const std::string systemFileSeparator = getSystemFileSeparator();
    size_t lastSeparator = systemFilePath.rfind(systemFileSeparator);
    if ( lastSeparator == std::string::npos )
        return systemFilePath;
    else if ( !systemFilePath.empty() && lastSeparator == systemFilePath.length()-systemFileSeparator.length() )
        return "";
    else if ( lastSeparator >= 0 && lastSeparator < systemFilePath.length()-systemFileSeparator.length() )
        return systemFilePath.substr(lastSeparator+systemFileSeparator.length(), systemFilePath.length());
    else
        return systemFilePath;
}

std::string getSystemFileExtension(const std::string & systemFilePath, bool includeExtensionSeparator)
{
    const std::string systemExtensionSeparator = ".";
    size_t lastExtensionSeparator = systemFilePath.rfind(systemExtensionSeparator);
    if ( lastExtensionSeparator != std::string::npos && lastExtensionSeparator >= 0 &&
        lastExtensionSeparator <= systemFilePath.length()-systemExtensionSeparator.length() )
    {
        size_t lastPathSeparator = systemFilePath.rfind(getSystemFileSeparator());
        if ( lastPathSeparator == std::string::npos || lastExtensionSeparator > lastPathSeparator )
        {
            if ( includeExtensionSeparator )
                return systemFilePath.substr(lastExtensionSeparator, systemFilePath.length() - lastExtensionSeparator);
            else
                return systemFilePath.substr(lastExtensionSeparator + systemExtensionSeparator.length(), systemFilePath.length() - lastExtensionSeparator - systemExtensionSeparator.length());
        }
    }
    return "";
}

std::string getSystemFileDirectory(const std::string & systemFilePath, bool includeTrailingSeparator)
{
    const std::string systemFileSeparator = getSystemFileSeparator();
    size_t lastSeparator = systemFilePath.rfind(systemFileSeparator);
    if ( lastSeparator == std::string::npos )
        return "";
    else if ( lastSeparator >= 0 && lastSeparator < systemFilePath.length() )
    {
        if ( includeTrailingSeparator )
            return systemFilePath.substr(0, lastSeparator+systemFileSeparator.length());
        else
            return systemFilePath.substr(0, lastSeparator);
    }
    return systemFilePath;
}

std::string makeSystemFilePath(const std::string & systemDirectory, const std::string & fileName)
{
    const std::string systemFileSeparator = getSystemFileSeparator();
    size_t lastSeparator = systemDirectory.rfind(systemFileSeparator);
    if ( lastSeparator == systemDirectory.length() - systemFileSeparator.length() )
        return systemDirectory + fileName;
    else
        return systemDirectory + systemFileSeparator + fileName;
}

std::string makeExtSystemFilePath(const std::string & systemFilePath, const std::string & extension)
{
    const std::string systemExtensionSeparator = ".";
    const bool extensionIncludesSeparator = extension.find(systemExtensionSeparator) == 0;
    if ( extensionIncludesSeparator )
        return systemFilePath + extension;
    else
        return systemFilePath + systemExtensionSeparator + extension;
}

std::string makeExtSystemFilePath(const std::string & systemDirectory, const std::string & fileName, const std::string & extension)
{
    const std::string systemFileSeparator = getSystemFileSeparator();
    const std::string systemExtensionSeparator = ".";
    size_t lastSeparator = systemDirectory.rfind(systemFileSeparator);
    const bool directoryIncludesSeparator = lastSeparator != std::string::npos && lastSeparator == systemDirectory.length() - systemFileSeparator.length();
    const bool extensionIncludesSeparator = extension.find(systemExtensionSeparator) == 0;
    if ( directoryIncludesSeparator )
    {
        if ( extensionIncludesSeparator )
            return systemDirectory + fileName + extension;
        else
            return systemDirectory + fileName + systemExtensionSeparator + extension;
    }
    else
    {
        if ( extensionIncludesSeparator )
            return systemDirectory + systemFileSeparator + fileName + extension;
        else
            return systemDirectory + systemFileSeparator + fileName + systemExtensionSeparator + extension;
    }
}

std::string getMpqFileSeparator()
{
    return "\\";
}

std::string getMpqFileName(const std::string & mpqFilePath)
{
    const std::string mpqFileSeparator = getMpqFileSeparator();
    size_t lastSeparator = mpqFilePath.rfind(mpqFileSeparator);
    if ( lastSeparator == std::string::npos )
        return mpqFilePath;
    else if ( !mpqFilePath.empty() && lastSeparator == mpqFilePath.length()-mpqFileSeparator.length() )
        return "";
    else if ( lastSeparator >= 0 && lastSeparator < mpqFilePath.length()-mpqFileSeparator.length() )
        return mpqFilePath.substr(lastSeparator+mpqFileSeparator.length(), mpqFilePath.length());
    else
        return mpqFilePath;
}

std::string getMpqFileExtension(const std::string & mpqFilePath, bool includeExtensionSeparator)
{
    const std::string mpqExtensionSeparator = ".";
    size_t lastExtensionSeparator = mpqFilePath.rfind(mpqExtensionSeparator);
    if ( lastExtensionSeparator != std::string::npos && lastExtensionSeparator >= 0 &&
        lastExtensionSeparator <= mpqFilePath.length()-mpqExtensionSeparator.length() )
    {
        size_t lastPathSeparator = mpqFilePath.rfind(getMpqFileSeparator());
        if ( lastPathSeparator == std::string::npos || lastExtensionSeparator > lastPathSeparator )
        {
            if ( includeExtensionSeparator )
                return mpqFilePath.substr(lastExtensionSeparator, mpqFilePath.length() - lastExtensionSeparator);
            else
                return mpqFilePath.substr(lastExtensionSeparator + mpqExtensionSeparator.length(), mpqFilePath.length() - lastExtensionSeparator - mpqExtensionSeparator.length());
        }
    }
    return "";
}

std::string makeMpqFilePath(const std::string & mpqDirectory, const std::string & fileName)
{
    const std::string mpqFileSeparator = getMpqFileSeparator();
    size_t lastSeparator = mpqDirectory.rfind(mpqFileSeparator);
    if ( lastSeparator != std::string::npos && lastSeparator == mpqDirectory.length() - mpqFileSeparator.length() )
        return mpqDirectory + fileName;
    else
        return mpqDirectory + mpqFileSeparator + fileName;
}

std::string makeExtMpqFilePath(const std::string & mpqFilePath, const std::string & extension)
{
    const std::string mpqExtensionSeparator = ".";
    const bool extensionIncludesSeparator = extension.find(mpqExtensionSeparator) == 0;
    if ( extensionIncludesSeparator )
        return mpqFilePath + extension;
    else
        return mpqFilePath + mpqExtensionSeparator + extension;
}

bool findFile(const std::string & filePath)
{
    icux::filestring fFilePath = icux::toFilestring(filePath);
    if ( !filePath.empty() )
    {
#ifdef WINDOWS_UTF16
        FILE* file = NULL;
        errno_t result = _wfopen_s(&file, fFilePath.c_str(), L"r");
        if ( file != NULL )
            fclose(file);
        
        return result == 0 || result == EEXIST || result == EACCES;
#else
#ifdef WINDOWS_MULTIBYTE
        FILE* file = NULL;
        errno_t result = fopen_s(&file, fFilePath.c_str(), "r");
        if ( file != NULL )
            fclose(file);

        return result == 0 || result == EEXIST || result == EACCES;
#else
        FILE* file = fopen(fFilePath.c_str(), "r");
        if ( file != nullptr )
        {
            std::fclose(file);
            return true;
        }
#endif
#endif
    }
    return false;
}

bool patientFindFile(const std::string & filePath, int numWaitTimes, int* waitTimes)
{
    if ( findFile(filePath) )
        return true;

    for ( int i = 0; i < numWaitTimes; i++ )
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(waitTimes[i]));
        if ( findFile(filePath) )
            return true;
    }

    return false;
}

bool fileToString(const std::string & fileName, std::string & str)
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

bool makeFileCopy(const std::string & inFilePath, const std::string & outFilePath)
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

bool makeDirectory(const std::string & directory)
{
    icux::filestring directoryPath = icux::toFilestring(directory);
#ifdef WINDOWS_UTF16
    return _wmkdir(directoryPath.c_str()) == 0;
#else
    return _mkdir(directoryPath.c_str()) == 0;
#endif
}

bool removeFile(const std::string & filePath)
{
    icux::filestring sysFilePath = icux::toFilestring(filePath);
    // Return whether the file with the given filePath is not on the system
#ifdef WINDOWS_UTF16
    return filePath.empty() || _wremove(sysFilePath.c_str()) == 0 || !findFile(filePath);
#else
    return sysFilePath.empty() || remove(sysFilePath.c_str()) == 0 || !FindFile(filePath);
#endif
}

bool removeFiles(const std::string & firstFileName, const std::string & secondFileName)
{
    bool success = removeFile(firstFileName) && removeFile(secondFileName);
    return success;
}

bool removeFiles(const std::string & firstFileName, const std::string & secondFileName, const std::string & thirdFileName)
{
    bool success = removeFile(firstFileName) && removeFile(secondFileName) && removeFile(thirdFileName);
    return success;
}

bool getModuleDirectory(output_param std::string & moduleDirectory, bool includeTrailingSeparator)
{
#ifdef _WIN32
    icux::codepoint cModulePath[MAX_PATH] = {};
    if ( GetModuleFileName(NULL, cModulePath, MAX_PATH) != MAX_PATH )
    {
        icux::filestring modulePath(cModulePath);
        auto lastBackslashPos = modulePath.rfind('\\');
        if ( lastBackslashPos != std::string::npos && lastBackslashPos < modulePath.size() )
        {
            moduleDirectory = icux::toUtf8(modulePath.substr(0, lastBackslashPos)) + (includeTrailingSeparator ? getSystemFileSeparator() : "");
            return true;
        }
    }
#endif
    return false;
}

bool getDefaultScPath(output_param std::string & data)
{
    return ""; // TODO: Implement me
}

std::string getDefaultScPath()
{
    return ""; // TODO: Implement me
}

// Windows registry functions
#ifdef _WIN32
DWORD getSubKeyString(HKEY hParentKey, const std::string & subKey, const std::string & valueName, std::string & data)
{
    icux::filestring sysSubKey = icux::toFilestring(subKey);
    icux::filestring sysValueName = icux::toFilestring(valueName);

    HKEY hKey = NULL;

    DWORD errorCode = RegOpenKeyEx(hParentKey, sysSubKey.c_str(), 0, KEY_QUERY_VALUE, &hKey); // Open key
    if ( errorCode == ERROR_SUCCESS ) // Open key success
    {
        DWORD bufferSize = size_1kb;
        do
        {
            std::unique_ptr<char> buffer(new char[bufferSize]);
            bufferSize *= 2;

            errorCode = RegQueryValueEx(hKey, sysValueName.c_str(), NULL, NULL, (LPBYTE)buffer.get(), &bufferSize); // Read Key
            if ( errorCode == ERROR_SUCCESS )
                data = std::string(buffer.get());
        }
        while ( errorCode == ERROR_MORE_DATA && bufferSize <= size_1gb );
        RegCloseKey(hKey); // Close key
    }
    return errorCode; // Return success/error message
}

bool getRegString(const std::string & subKey, const std::string & valueName, std::string & data)
{
    DWORD errorCode = getSubKeyString(HKEY_CURRENT_USER, subKey, valueName, data); // Try HKCU
    if ( errorCode != ERROR_SUCCESS ) // Not found in HKCU
        errorCode = getSubKeyString(HKEY_LOCAL_MACHINE, subKey, valueName, data); // Try HKLM

    return errorCode == ERROR_SUCCESS;
}
#endif

PromptResult getYesNo(const std::string & text, const std::string & caption)
{
#ifdef _WIN32
    int result = MessageBox(NULL, icux::toUistring(text).c_str(), icux::toUistring(caption).c_str(), MB_YESNO);
    switch ( result ) {
        case IDYES: return PromptResult::Yes;
        case IDNO: return PromptResult::No;
        default: return PromptResult::Unknown;
    }
#else
    return PromptResult::Unknown;
#endif
}

PromptResult getYesNoCancel(const std::string & text, const std::string & caption)
{
#ifdef _WIN32
    int result = MessageBox(NULL, icux::toUistring(text).c_str(), icux::toUistring(caption).c_str(), MB_YESNOCANCEL);
    switch ( result ) {
        case IDYES: return PromptResult::Yes;
        case IDNO: return PromptResult::No;
        case IDCANCEL: return PromptResult::Cancel;
        default: return PromptResult::Unknown;
    }
#else
    return PromptResult::Unknown;
#endif
}

bool browseForFile(std::string & filePath, uint32_t & filterIndex, const std::vector<std::pair<std::string, std::string>> & filtersAndLabels,
    const std::string & initialDirectory, const std::string & title, bool pathMustExist, bool provideOverwritePrompt)
{
#ifdef _WIN32
    OPENFILENAME ofn = {};
    icux::codepoint fileNameBuffer[FILENAME_MAX] = {};
    if ( !initialDirectory.empty() )
    {
        icux::filestring initFileNameBuf = icux::toFilestring(filePath);
        if ( initFileNameBuf.length() < FILENAME_MAX )
        {
            #ifdef UTF16_FILESYSTEM
            wcscpy(fileNameBuffer, initFileNameBuf.c_str());
            #else
            strcpy(fileNameBuffer, initFileNameBuf.c_str());
            #endif
        }
    }

    std::string filterString = "";
    for ( auto filterAndLabel : filtersAndLabels )
    {
        const std::string & filter = filterAndLabel.first;
        const std::string & label = filterAndLabel.second;
        filterString += label + '\0' + filter + '\0';
    }

    icux::uistring sysFilterString = icux::toUistring(filterString);
    icux::filestring sysInitialDir = icux::toFilestring(initialDirectory);
    icux::uistring sysTitle = icux::toUistring(title);

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.lpstrFilter = sysFilterString.c_str();
    ofn.lpstrFile = fileNameBuffer;

    if ( initialDirectory.empty() )
        ofn.lpstrInitialDir = NULL;
    else
        ofn.lpstrInitialDir = sysInitialDir.c_str();

    if ( title.empty() )
        ofn.lpstrTitle = NULL;
    else
        ofn.lpstrTitle = sysTitle.c_str();

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
    {
        filePath = icux::toUtf8(fileNameBuffer);
        filterIndex = ofn.nFilterIndex;
    }

    return success;
#else
    return false;
#endif
}

bool browseForSave(std::string & filePath, uint32_t & filterIndex, const std::vector<std::pair<std::string, std::string>> & filtersAndLabels,
    const std::string & initialDirectory, const std::string & title, bool pathMustExist, bool provideOverwritePrompt)
{
#ifdef _WIN32
    OPENFILENAME ofn = {};
    icux::codepoint fileNameBuffer[FILENAME_MAX] = {};
    if ( !filePath.empty() )
    {
        icux::filestring initFileNameBuf = icux::toFilestring(filePath);
        if ( initFileNameBuf.length() < FILENAME_MAX )
        {
            #ifdef UTF16_FILESYSTEM
            wcscpy(fileNameBuffer, initFileNameBuf.c_str());
            #else
            strcpy(fileNameBuffer, initFileNameBuf.c_str());
            #endif
        }
    }

    std::string filterString = "";
    for ( auto filtersAndLabel : filtersAndLabels )
    {
        const std::string & filter = filtersAndLabel.first;
        const std::string & label = filtersAndLabel.second;
        filterString += label + '\0' + filter + '\0';
    }

    icux::uistring sysFilterString = icux::toUistring(filterString);
    icux::filestring sysInitialDir = icux::toFilestring(initialDirectory);
    icux::uistring sysTitle = icux::toUistring(title);

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.lpstrFilter = sysFilterString.c_str();
    ofn.lpstrFile = fileNameBuffer;

    if ( initialDirectory.empty() )
        ofn.lpstrInitialDir = NULL;
    else
        ofn.lpstrInitialDir = sysInitialDir.c_str();

    if ( title.empty() )
        ofn.lpstrTitle = NULL;
    else
        ofn.lpstrTitle = sysTitle.c_str();

    ofn.hwndOwner = NULL;
    ofn.nMaxFile = FILENAME_MAX;
    DWORD flags = OFN_HIDEREADONLY;
    if ( pathMustExist )
        flags |= OFN_PATHMUSTEXIST;
    if ( provideOverwritePrompt )
        flags |= OFN_OVERWRITEPROMPT;

    ofn.Flags = flags;
    ofn.nFilterIndex = filterIndex;

    bool success = GetSaveFileName(&ofn) == TRUE;
    if ( success )
    {
        filePath = icux::toUtf8(fileNameBuffer);
        filterIndex = ofn.nFilterIndex;
    }

    return success;
#else
    return false;
#endif
}
