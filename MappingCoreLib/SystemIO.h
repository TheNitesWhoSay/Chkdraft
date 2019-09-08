#ifndef SYSTEMIO_H
#define SYSTEMIO_H
#include <fstream>
#include <direct.h>
#include <algorithm>
#include <vector>
#include <map>

/**
    SystemIO provides methods to interact directly with the system, all methods shall either use only C++ standard code
    or wrap all system specific code in compiler checks (e.g. #ifdef _WIN32) for that specific system, and return false/Unknown/Unsupported for any unsupported system
*/

#ifdef output_param
#undef output_param
#endif
#define output_param /* Syntactic sugar denoting an output parameter - unless a function indicates that there has been an error it's obligated to set out params before returning */

#ifdef inout_param
#undef inout_param
#endif
#define inout_param /* Syntactic sugar denoting a parameter that may optionally be used for input, and unless a function indicates that there's been an error it's obligated to set inout params before returning */

using u8 = std::uint8_t;
using s8 = std::int8_t;
using u16 = std::uint16_t;
using s16 = std::int16_t;
using u32 = std::uint32_t;
using s32 = std::int32_t;
using u64 = std::uint64_t;
using s64 = std::int64_t;

bool hasExtension(const std::string &systemFilePath, const std::string &extension);
std::string GetSystemFileSeparator(); // Gets the file separator on the current system, usually \ or /
std::string GetSystemFileName(const std::string &systemFilePath); // Extracts the file name from a system file path
std::string GetSystemFileExtension(const std::string &systemFilePath); // Extracts the file extension from a system file path, this will include the dot
std::string GetSystemFileDirectory(const std::string &systemFilePath, bool includeTrailingSeparator = true); // Extracts the directory from a system file path
std::string MakeSystemFilePath(const std::string &systemDirectory, const std::string &fileName); // Creates a file path ensuring the appropriate separator is used
std::string MakeExtSystemFilePath(const std::string &systemFilePath, const std::string &extension); // Creates a file path ensuring the appropriate extension is used
std::string MakeExtSystemFilePath(const std::string &systemDirectory, const std::string &fileName, const std::string &extension); // Creates a file path ensuring the appropriate separator and extension is used

std::string GetMpqFileSeparator(); // Gets the file separator used inside MPQ files (always \)
std::string GetMpqFileName(const std::string &mpqFilePath); // Extracts the file name from a MPQ file path
std::string MakeMpqFilePath(const std::string &mpqDirectory, const std::string &fileName); // Creates a mpq file path ensuring the appropriate separator is used

bool FindFile(const std::string &filePath);
bool PatientFindFile(const std::string &filePath, int numWaitTimes, int* waitTimes);

bool FileToString(const std::string &fileName, std::string &str);

bool MakeFileCopy(const std::string &inFilePath, const std::string &outFilePath);
bool MakeDirectory(const std::string &directory);

bool RemoveFile(const std::string &filePath);
bool RemoveFiles(const std::string &firstFileName, const std::string &secondFileName);
bool RemoveFiles(const std::string &firstFileName, const std::string &secondFileName, const std::string &thirdFileName);

bool GetModuleDirectory(output_param std::string &moduleDirectory, bool includeTrailingSeparator = false);

bool GetDefaultScPath(output_param std::string &data);
std::string GetDefaultScPath();

enum class PromptResult
{
    Unknown,
    Yes,
    No,
    Cancel
};
PromptResult GetYesNo(const std::string &text, const std::string &caption);
PromptResult GetYesNoCancel(const std::string &text, const std::string &caption);

bool BrowseForFile(inout_param std::string &filePath, inout_param u32 &filterIndex, const std::vector<std::pair<std::string, std::string>> &filtersAndLabels,
    const std::string &initialDirectory, const std::string &title, bool pathMustExist, bool provideOverwritePrompt);

bool BrowseForSave(inout_param std::string &filePath, inout_param u32 &filterIndex, const std::vector<std::pair<std::string, std::string>> &filtersAndLabels,
    const std::string &initialDirectory, const std::string &title, bool pathMustExist, bool provideOverwritePrompt);

#endif