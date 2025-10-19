#include "basics.h"
#include "system_io.h"
#include "cross_cut/simple_icu.h"
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <string>
#include <filesystem>
#include <memory>
#include <thread>
#include <chrono>
#include <iostream>
#include <regex>
#ifdef _WIN32
#include <Windows.h>
#include <direct.h>
#elif defined(__linux__)
#include <dlfcn.h>
#include <linux/limits.h>
#include "portable_file_dialogs.h"
#else
#include "portable_file_dialogs.h"
#endif

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

std::string fixSystemPathSeparators(const std::string & systemFilePath)
{
    const std::string defaultSystemPathSeparator = getSystemPathSeparator();
    const std::string doubleSeparator = defaultSystemPathSeparator + defaultSystemPathSeparator;
    const std::regex altSeparatorRegex = std::regex(defaultSystemPathSeparator.compare("\\") == 0 ? "\\/" : "\\\\");
    std::string fixedPath = std::regex_replace(systemFilePath, altSeparatorRegex, defaultSystemPathSeparator);
    size_t found = fixedPath.find(doubleSeparator);
    while ( found != std::string::npos )
    {
        fixedPath.replace(found, doubleSeparator.size(), defaultSystemPathSeparator);
        found = fixedPath.find(doubleSeparator);
    }
    return fixedPath;
}

std::string getSystemPathSeparator()
{
    std::filesystem::path::value_type separatorChar = std::filesystem::path::preferred_separator;
    icux::codepoint separatorArray[2] = {(icux::codepoint)separatorChar, icux::nullChar};
    return icux::toUtf8(icux::filestring(separatorArray));
}

std::string getSystemFileName(const std::string & systemFilePath)
{
    const std::string fixedSystemFilePath = fixSystemPathSeparators(systemFilePath);
    const std::string systemPathSeparator = getSystemPathSeparator();
    size_t lastSeparator = fixedSystemFilePath.rfind(systemPathSeparator);
    if ( lastSeparator == std::string::npos )
        return fixedSystemFilePath;
    else if ( !fixedSystemFilePath.empty() && lastSeparator == fixedSystemFilePath.length()-systemPathSeparator.length() )
        return "";
    else if ( lastSeparator >= 0 && lastSeparator < fixedSystemFilePath.length()-systemPathSeparator.length() )
        return fixedSystemFilePath.substr(lastSeparator+systemPathSeparator.length(), fixedSystemFilePath.length());
    else
        return fixedSystemFilePath;
}

std::string getSystemFileExtension(const std::string & systemFilePath, bool includeExtensionSeparator)
{
    const std::string fixedSystemFilePath = fixSystemPathSeparators(systemFilePath);
    const std::string systemExtensionSeparator = ".";
    size_t lastExtensionSeparator = fixedSystemFilePath.rfind(systemExtensionSeparator);
    if ( lastExtensionSeparator != std::string::npos && lastExtensionSeparator >= 0 &&
        lastExtensionSeparator <= fixedSystemFilePath.length()-systemExtensionSeparator.length() )
    {
        size_t lastPathSeparator = fixedSystemFilePath.rfind(getSystemPathSeparator());
        if ( lastPathSeparator == std::string::npos || lastExtensionSeparator > lastPathSeparator )
        {
            if ( includeExtensionSeparator )
                return fixedSystemFilePath.substr(lastExtensionSeparator, fixedSystemFilePath.length() - lastExtensionSeparator);
            else
                return fixedSystemFilePath.substr(lastExtensionSeparator + systemExtensionSeparator.length(), fixedSystemFilePath.length() - lastExtensionSeparator - systemExtensionSeparator.length());
        }
    }
    return "";
}

std::string getSystemFileDirectory(const std::string & systemFilePath, bool includeTrailingSeparator)
{
    const std::string fixedSystemFilePath = fixSystemPathSeparators(systemFilePath);
    const std::string systemPathSeparator = getSystemPathSeparator();
    size_t lastSeparator = fixedSystemFilePath.rfind(systemPathSeparator);
    if ( lastSeparator == std::string::npos )
        return "";
    else if ( lastSeparator >= 0 && lastSeparator < fixedSystemFilePath.length() )
    {
        if ( includeTrailingSeparator )
            return fixedSystemFilePath.substr(0, lastSeparator+systemPathSeparator.length());
        else
            return fixedSystemFilePath.substr(0, lastSeparator);
    }
    return fixedSystemFilePath;
}

std::string makeSystemFilePath(const std::string & systemDirectory, const std::string & fileName)
{
    const std::string fixedSystemDirectory = fixSystemPathSeparators(systemDirectory);
    const std::string systemPathSeparator = getSystemPathSeparator();
    size_t lastSeparator = fixedSystemDirectory.rfind(systemPathSeparator);
    if ( lastSeparator == fixedSystemDirectory.length() - systemPathSeparator.length() )
        return fixedSystemDirectory + fileName;
    else
        return fixedSystemDirectory + systemPathSeparator + fileName;
}

std::string makeExtSystemFilePath(const std::string & systemFilePath, const std::string & extension)
{
    const std::string fixedSystemFilePath = fixSystemPathSeparators(systemFilePath);
    const std::string systemExtensionSeparator = ".";
    const bool extensionIncludesSeparator = extension.find(systemExtensionSeparator) == 0;
    if ( extensionIncludesSeparator )
        return fixedSystemFilePath + extension;
    else
        return fixedSystemFilePath + systemExtensionSeparator + extension;
}

std::string makeExtSystemFilePath(const std::string & systemDirectory, const std::string & fileName, const std::string & extension)
{
    const std::string fixedSystemDirectory = fixSystemPathSeparators(systemDirectory);
    const std::string systemPathSeparator = getSystemPathSeparator();
    const std::string systemExtensionSeparator = ".";
    size_t lastSeparator = fixedSystemDirectory.rfind(systemPathSeparator);
    const bool directoryIncludesSeparator = lastSeparator != std::string::npos && lastSeparator == fixedSystemDirectory.length() - systemPathSeparator.length();
    const bool extensionIncludesSeparator = extension.find(systemExtensionSeparator) == 0;
    if ( directoryIncludesSeparator )
    {
        if ( extensionIncludesSeparator )
            return fixedSystemDirectory + fileName + extension;
        else
            return fixedSystemDirectory + fileName + systemExtensionSeparator + extension;
    }
    else
    {
        if ( extensionIncludesSeparator )
            return fixedSystemDirectory + systemPathSeparator + fileName + extension;
        else
            return fixedSystemDirectory + systemPathSeparator + fileName + systemExtensionSeparator + extension;
    }
}

std::string fixArchivePathSeparators(const std::string & archiveFilePath)
{
    const std::string defaultArchivePathSeparator = getArchivePathSeparator();
    const std::string doubleSeparator = defaultArchivePathSeparator + defaultArchivePathSeparator;
    const std::regex altSeparatorRegex = std::regex(defaultArchivePathSeparator.compare("\\") == 0 ? "\\/" : "\\\\");
    std::string fixedPath = std::regex_replace(archiveFilePath, altSeparatorRegex, defaultArchivePathSeparator);
    size_t found = fixedPath.find(doubleSeparator);
    while ( found != std::string::npos )
    {
        fixedPath.replace(found, doubleSeparator.size(), defaultArchivePathSeparator);
        found = fixedPath.find(doubleSeparator);
    }
    return fixedPath;
}

std::string getArchivePathSeparator()
{
    return "\\";
}

std::string getArchiveFileName(const std::string & archiveFilePath)
{
    const std::string fixedArchiveFilePath = fixArchivePathSeparators(archiveFilePath);
    const std::string archivePathSeparator = getArchivePathSeparator();
    size_t lastSeparator = fixedArchiveFilePath.rfind(archivePathSeparator);
    if ( lastSeparator == std::string::npos )
        return fixedArchiveFilePath;
    else if ( !fixedArchiveFilePath.empty() && lastSeparator == fixedArchiveFilePath.length()-archivePathSeparator.length() )
        return "";
    else if ( lastSeparator >= 0 && lastSeparator < fixedArchiveFilePath.length()-archivePathSeparator.length() )
        return fixedArchiveFilePath.substr(lastSeparator+archivePathSeparator.length(), fixedArchiveFilePath.length());
    else
        return fixedArchiveFilePath;
}

std::string getArchiveFileExtension(const std::string & archiveFilePath, bool includeExtensionSeparator)
{
    const std::string fixedArchiveFilePath = fixArchivePathSeparators(archiveFilePath);
    const std::string archiveExtensionSeparator = ".";
    size_t lastExtensionSeparator = fixedArchiveFilePath.rfind(archiveExtensionSeparator);
    if ( lastExtensionSeparator != std::string::npos && lastExtensionSeparator >= 0 &&
        lastExtensionSeparator <= fixedArchiveFilePath.length()-archiveExtensionSeparator.length() )
    {
        size_t lastPathSeparator = fixedArchiveFilePath.rfind(getArchivePathSeparator());
        if ( lastPathSeparator == std::string::npos || lastExtensionSeparator > lastPathSeparator )
        {
            if ( includeExtensionSeparator )
                return fixedArchiveFilePath.substr(lastExtensionSeparator, fixedArchiveFilePath.length() - lastExtensionSeparator);
            else
                return fixedArchiveFilePath.substr(lastExtensionSeparator + archiveExtensionSeparator.length(), fixedArchiveFilePath.length() - lastExtensionSeparator - archiveExtensionSeparator.length());
        }
    }
    return "";
}

std::string makeArchiveFilePath(const std::string & archiveDirectory, const std::string & fileName)
{
    const std::string fixedArchiveDirectory = fixArchivePathSeparators(archiveDirectory);
    const std::string archivePathSeparator = getArchivePathSeparator();
    size_t lastSeparator = fixedArchiveDirectory.rfind(archivePathSeparator);
    if ( lastSeparator != std::string::npos && lastSeparator == fixedArchiveDirectory.length() - archivePathSeparator.length() )
        return fixedArchiveDirectory + fileName;
    else
        return fixedArchiveDirectory + archivePathSeparator + fileName;
}

std::string makeExtArchiveFilePath(const std::string & archiveFilePath, const std::string & extension)
{
    const std::string fixedArchiveFilePath = fixArchivePathSeparators(archiveFilePath);
    const std::string archiveExtensionSeparator = ".";
    const bool extensionIncludesSeparator = extension.find(archiveExtensionSeparator) == 0;
    if ( extensionIncludesSeparator )
        return fixedArchiveFilePath + extension;
    else
        return fixedArchiveFilePath + archiveExtensionSeparator + extension;
}

bool isSamePath(const std::string & left, const std::string & right)
{
    try {
        return left.compare(right) == 0 ||
            std::filesystem::path(left) == std::filesystem::path(right);
    } catch ( ... ) {
        return false;
    }
}

bool isInDirectory(const std::string & path, const std::string & directory)
{
    try {
        return std::filesystem::path(path).parent_path() == std::filesystem::path(directory);
    } catch ( ... ) {
        return false;
    }
}

bool isDirectory(const std::string & directory)
{
    return std::filesystem::is_directory(directory);
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
#elif defined(WINDOWS_MULTIBYTE)
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

std::optional<std::string> fileToString(const std::string & fileName)
{
    try {
        std::ifstream file(std::filesystem::path(asUtf8(fileName)), std::ifstream::in | std::ifstream::ate); // Open at ending characters position
        if ( file.is_open() )
        {
            auto size = file.tellg(); // Grab size via current position
            file.seekg(0); // Move reader to beggining of file
            auto str = std::make_optional<std::string>(); // Set string size to file size
            str->reserve(size_t(size));
            while ( !file.eof() )
                *str += file.get();

            if ( str->length() > 0 && str.value()[str->length() - 1] == (char)-1 )
                str.value()[str->length() - 1] = '\0';

            bool success = file.good() || file.eof();
            file.close();
            return success ? str : std::nullopt;
        }
    }
    catch ( ... ) { }
    return std::nullopt;
}

std::optional<std::vector<u8>> fileToBuffer(const std::string & systemFilePath)
{
    std::ifstream inFile(std::filesystem::path(asUtf8(systemFilePath)), std::ios_base::binary|std::ios_base::in);
    auto buffer = std::make_optional<std::vector<u8>>(std::istreambuf_iterator<char>(inFile), std::istreambuf_iterator<char>());
    bool success = inFile.good();
    inFile.close();
    return success ? buffer : std::nullopt;
}

bool bufferToFile(const std::string & systemFilePath, const std::vector<u8> & buffer)
{
    std::ofstream outFile(std::filesystem::path(asUtf8(systemFilePath)), std::ios_base::binary|std::ios_base::out);
    outFile.write(reinterpret_cast<const char*>(&buffer[0]), std::streamsize(buffer.size()));
    bool success = outFile.good();
    outFile.close();
    return success;
}

std::vector<std::string> collectLineSeparatedStrings(const std::vector<u8> & buffer)
{
    std::vector<std::string> result {};
    std::size_t entryStart = 0;
    std::size_t size = buffer.size();
    for ( std::size_t i=0; i<size; ++i )
    {
        char c = char(buffer[i]);
        if ( c == '\r' || c == '\n' )
        {
            if ( i > entryStart )
                result.emplace_back((const char*)&buffer[entryStart], i-entryStart);
                    
            entryStart = i+1;
        }
    }
    return result;
}

bool makeFileCopy(const std::string & inFilePath, const std::string & outFilePath)
{
    bool success = false;
    std::ifstream inFile;
    std::ofstream outFile;
    try
    {
        inFile.open(std::filesystem::path(asUtf8(inFilePath)), std::fstream::binary);
        if ( inFile.is_open() )
        {
            outFile.open(std::filesystem::path(asUtf8(outFilePath)), std::fstream::out|std::fstream::binary);
            if ( outFile.is_open() )
            {
                outFile << inFile.rdbuf();
                success = true;
                outFile.close();
            }
            inFile.close();
        }
    }
    catch ( ... ) { }

    if ( inFile.is_open() )
        inFile.close();

    if ( outFile.is_open() )
        outFile.close();

    return success;
}

bool makeDirectory(const std::string & directory)
{
    icux::filestring directoryPath = icux::toFilestring(directory);
    return std::filesystem::create_directories(directoryPath.c_str()) || std::filesystem::exists(directoryPath.c_str());
}

bool removeFile(const std::string & filePath)
{
    icux::filestring sysFilePath = icux::toFilestring(filePath);
    // Return whether the file with the given filePath is not on the system
#ifdef WINDOWS_UTF16
    return filePath.empty() || _wremove(sysFilePath.c_str()) == 0 || !findFile(filePath);
#else
    return filePath.empty() || remove(sysFilePath.c_str()) == 0 || !findFile(filePath);
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

std::optional<std::string> getModuleDirectory(bool includeTrailingSeparator)
{
#ifdef _WIN32
    icux::codepoint cModulePath[MAX_PATH] = {};
    if ( GetModuleFileName(NULL, cModulePath, MAX_PATH) != MAX_PATH )
    {
        icux::filestring modulePath(cModulePath);
        auto lastBackslashPos = modulePath.rfind('\\');
        if ( lastBackslashPos != std::string::npos && lastBackslashPos < modulePath.size() )
            return icux::toUtf8(modulePath.substr(0, lastBackslashPos)) + (includeTrailingSeparator ? getSystemPathSeparator() : "");
    }
#elif defined (__linux__)
    icux::codepoint cModulePath[PATH_MAX] = {};
    Dl_info dlInfo {};
    if ( dladdr((const void*)&getModuleDirectory, &dlInfo) != 0 )
        return icux::toUtf8(dlInfo.dli_fname);
#endif
    return std::nullopt;
}

bool getDefaultScPath(std::string & data)
{
    data = getDefaultScPath();
    return true;
}

std::string getDefaultScPath()
{
#ifdef _WIN32
    return "C:\\Program Files (x86)\\StarCraft"; // TODO: More logic
#else
    return "";
#endif
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
    if ( !pfd::settings::available() )
        return PromptResult::Unknown;

    auto button = pfd::message(caption, text, pfd::choice::yes_no).result();
    switch ( button ) {
        case pfd::button::yes: return PromptResult::Yes;
        case pfd::button::no: return PromptResult::No;
        default: return PromptResult::Unknown;
    }
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
    if ( !pfd::settings::available() )
        return PromptResult::Unknown;

    auto button = pfd::message(caption, text, pfd::choice::yes_no_cancel).result();
    switch ( button ) {
        case pfd::button::yes: return PromptResult::Yes;
        case pfd::button::no: return PromptResult::No;
        case pfd::button::cancel: return PromptResult::Cancel;
        default: return PromptResult::Unknown;
    }
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
    ofn.nFilterIndex = filterIndex+1;

    bool success = GetOpenFileName(&ofn) == TRUE;
    if ( success )
    {
        filePath = icux::toUtf8(fileNameBuffer);
        filterIndex = ofn.nFilterIndex-1;
    }

    return success;
#else
    if ( !pfd::settings::available() )
        return false;

    std::vector<std::string> flatFilters {};
    for ( auto & filterAndLabel : filtersAndLabels )
    {
        flatFilters.push_back(filterAndLabel.second);
        flatFilters.push_back(filterAndLabel.first);
    }
    auto selection = pfd::open_file(title, initialDirectory, flatFilters, pfd::opt::none).result();
    if ( selection.empty() )
        return false;
    else
    {
        filePath = selection[0];
        return true;
    }
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
    ofn.nFilterIndex = filterIndex+1;

    bool success = GetSaveFileName(&ofn) == TRUE;
    if ( success )
    {
        filePath = icux::toUtf8(fileNameBuffer);
        filterIndex = ofn.nFilterIndex-1;
    }

    return success;
#else
    return false;
#endif
}

bool lastErrorIndicatedFileNotFound()
{
#ifdef _WIN32
    return ::GetLastError() == ERROR_FILE_NOT_FOUND;
#else
    return false;
#endif
}

bool lastErrorIndicatedBadFormat()
{
#ifdef _WIN32
    return ::GetLastError() == ERROR_BAD_FORMAT;
#else
    return false;
#endif
}

unsigned long getLastError()
{
#ifdef _WIN32
    return ::GetLastError();
#else
    return 0;
#endif
}
