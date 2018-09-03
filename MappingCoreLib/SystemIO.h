#ifndef SYSTEMIO_H
#define SYSTEMIO_H
#include "Buffer.h"
#include <fstream>
#include <direct.h>
#include <algorithm>
#include <map>

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

bool FileToBuffer(const std::string &fileName, buffer &buf);
bool FileToString(const std::string &fileName, std::string &str);

bool MakeFileCopy(const std::string &inFilePath, const std::string &outFilePath);
bool MakeDirectory(const std::string &directory);

bool RemoveFile(const std::string &filePath);
bool RemoveFiles(const std::string &firstFileName, const std::string &secondFileName);
bool RemoveFiles(const std::string &firstFileName, const std::string &secondFileName, const std::string &thirdFileName);

bool GetModuleDirectory(output_param std::string &moduleDirectory);

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