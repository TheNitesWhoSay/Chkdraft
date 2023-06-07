#ifndef SYSTEMIO_H
#define SYSTEMIO_H
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

/**
    SystemIO provides methods to interact directly with the system, all methods shall either use only C++ standard code
    or wrap all system specific code in compiler checks (e.g. #ifdef _WIN32) for that specific system, and return false/Unknown/Unsupported for any unsupported system
*/

using u8 = std::uint8_t;
using s8 = std::int8_t;
using u16 = std::uint16_t;
using s16 = std::int16_t;
using u32 = std::uint32_t;
using s32 = std::int32_t;
using u64 = std::uint64_t;
using s64 = std::int64_t;

bool hasExtension(const std::string & systemFilePath, const std::string & extension);
std::string fixSystemPathSeparators(const std::string & systemFilePath); // Replace all supported path separator chars with default separator chars, replace all sequential separators with a single separator
std::string getSystemPathSeparator(); // Gets the file separator on the current system, usually \ or /
std::string getSystemFileName(const std::string & systemFilePath); // Extracts the file name from a system file path
std::string getSystemFileExtension(const std::string & systemFilePath, bool includeExtensionSeparator = true); // Extracts the file extension from a system file path, this will include the dot
std::string getSystemFileDirectory(const std::string & systemFilePath, bool includeTrailingSeparator = true); // Extracts the directory from a system file path
std::string makeSystemFilePath(const std::string & systemDirectory, const std::string & fileName); // Creates a file path ensuring the appropriate separator is used
std::string makeExtSystemFilePath(const std::string & systemFilePath, const std::string & extension); // Creates a file path ensuring the appropriate extension is used
std::string makeExtSystemFilePath(const std::string & systemDirectory, const std::string & fileName, const std::string & extension); // Creates a file path ensuring the appropriate separator and extension is used

std::string fixMpqPathSeparators(const std::string & mpqFilePath); // Replace all supported path separator chars with default separator chars, replace all sequential separators with a single separator
std::string getMpqPathSeparator(); // Gets the file separator used inside MPQ files (always \)
std::string getMpqFileName(const std::string & mpqFilePath); // Extracts the file name from a MPQ file path
std::string getMpqFileExtension(const std::string & mpqFilePath, bool includeExtensionSeparator = true); // Extracts the file extension from a mpq file path, this will include the dot
std::string makeMpqFilePath(const std::string & mpqDirectory, const std::string & fileName); // Creates a mpq file path ensuring the appropriate separator is used
std::string makeExtMpqFilePath(const std::string & mpqFilePath, const std::string & extension); // Creates a mpq file path ensuring the appropriate separator and extension is used

bool isDirectory(const std::string & directory);
bool findFile(const std::string & filePath);
bool patientFindFile(const std::string & filePath, int numWaitTimes, int* waitTimes);

std::optional<std::string> fileToString(const std::string & fileName);
std::optional<std::vector<u8>> fileToBuffer(const std::string & systemFilePath);
bool bufferToFile(const std::string & systemFilePath, const std::vector<u8> & buffer);

bool makeFileCopy(const std::string & inFilePath, const std::string & outFilePath);
bool makeDirectory(const std::string & directory);

bool removeFile(const std::string & filePath);
bool removeFiles(const std::string & firstFileName, const std::string & secondFileName);
bool removeFiles(const std::string & firstFileName, const std::string & secondFileName, const std::string & thirdFileName);

std::optional<std::string> getModuleDirectory(bool includeTrailingSeparator = false);

bool getDefaultScPath(std::string & data);
std::string getDefaultScPath();

enum class PromptResult
{
    Unknown,
    Yes,
    No,
    Cancel
};
PromptResult getYesNo(const std::string & text, const std::string & caption);
PromptResult getYesNoCancel(const std::string & text, const std::string & caption);

bool browseForFile(std::string & filePath, u32 & filterIndex, const std::vector<std::pair<std::string, std::string>> & filtersAndLabels,
    const std::string & initialDirectory, const std::string & title, bool pathMustExist, bool provideOverwritePrompt);

bool browseForSave(std::string & filePath, u32 & filterIndex, const std::vector<std::pair<std::string, std::string>> & filtersAndLabels,
    const std::string & initialDirectory, const std::string & title, bool pathMustExist, bool provideOverwritePrompt);

bool lastErrorIndicatedFileNotFound();

unsigned long getLastError();

#endif