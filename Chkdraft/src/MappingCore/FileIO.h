#ifndef FILEIO_H
#define FILEIO_H
#include "Buffer.h"
#ifdef USESTATICSFMPQ
#include "../../SFmpq/SFmpq_static.h"
#else
#include "../../SFmpq/SFmpqapi.h"
#endif
#include <fstream>
#include <direct.h>
#include <algorithm>

enum class WavQuality
{
    Low = MAWA_QUALITY_LOW,
    Med = MAWA_QUALITY_MEDIUM,
    High = MAWA_QUALITY_HIGH,
    Uncompressed = std::max(std::max(MAWA_QUALITY_LOW, MAWA_QUALITY_MEDIUM), MAWA_QUALITY_HIGH)+1
};

enum class WavStatus
{
    PendingMatch,
    CurrentMatch,
    VirtualFile,
    NoMatch,
    NoMatchExtended,
    FileInUse,
    Unknown
};

std::string GetSystemFileSeparator(); // Gets the file separator on the current system, usually \ or /
std::string GetSystemFileName(const std::string &systemFilePath); // Extracts the file name from a system file path
std::string GetSystemFileDirectory(const std::string &systemFilePath, bool includeSeparator); // Extracts the directory from a system file path
std::string MakeSystemFilePath(const std::string &systemDirectory, const std::string &fileName); // Creates a file path ensuring the appropriate separator is used

std::string GetMpqFileSeparator(); // Gets the file separator used inside MPQ files (always \)
std::string GetMpqFileName(const std::string &mpqFilePath); // Extracts the file name from a MPQ file path
std::string MakeMpqFilePath(const std::string &mpqDirectory, const std::string &fileName); // Creates a mpq file path ensuring the appropriate separator is used

bool MakeDirectory(std::string directory);

bool GetModuleDirectory(std::string &outModuleDirectory);

bool FindFile(const char* filePath);

bool RemoveFile(const char* filePath);

bool FindFileInMpq(MPQHANDLE mpq, const char* fileName);

bool FindFileInMpq(const char* mpqPath, const char* fileName);

bool PatientFindFile(const char* filePath, int numWaitTimes, int* waitTimes);

bool OpenArchive(const char* mpqPath, MPQHANDLE &mpq);

bool OpenArchive(const char* directory, const char* mpqName, MPQHANDLE &hMpq, std::string &outPath);

bool CloseArchive(MPQHANDLE mpq);

bool BufferToArchive(MPQHANDLE &hMpq, const buffer &buf, const std::string &mpqFilePath);

bool WavBufferToArchive(MPQHANDLE &hMpq, const buffer &buf, const std::string &mpqFilePath, WavQuality wavQuality);

bool FileToBuffer(MPQHANDLE &hMpq, const std::string &fileName, buffer &buf);

bool FileToBuffer(MPQHANDLE &hStarDat, MPQHANDLE &hBrooDat, MPQHANDLE &hPatchRt, MPQHANDLE &hPriority, const std::string &fileName, buffer& buf);

bool FileToBuffer(MPQHANDLE &hStarDat, MPQHANDLE &hBrooDat, MPQHANDLE &hPatchRt, const std::string &fileName, buffer& buf);

bool FileToBuffer(const std::string &fileName, buffer &buf);

bool FileToString(const std::string &fileName, std::string &str);

bool MakeFileCopy(const std::string &inFilePath, const std::string &outFilePath);

void RemoveFile(std::string fileName);

void RemoveFiles(std::string firstFileName, std::string secondFileName);

void RemoveFiles(std::string firstFileName, std::string secondFileName, std::string thirdFileName);

OPENFILENAME GetOfn(char* szFileName, const char* filter, int initFilter);

OPENFILENAME GetScSaveOfn(char* szFileName);

OPENFILENAME GetWavSaveOfn(char* szFileName);

DWORD GetSubKeyString(HKEY hParentKey, const char* subKey, const char* valueName, char* data, DWORD* dataSize);

bool GetRegString(char* dest, u32 destSize, const char* subKey, const char* valueName);

bool GetRegScPath(char* dest, u32 destSize);

#endif