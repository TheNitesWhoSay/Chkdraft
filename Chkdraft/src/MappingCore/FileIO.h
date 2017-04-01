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

void MakeDirectory(std::string directory);

bool GetModuleDirectory(std::string &outModuleDirectory);

bool FindFile(const char* filePath);

bool FindFileInMpq(MPQHANDLE mpq, const char* fileName);

bool FindFileInMpq(const char* mpqPath, const char* fileName);

bool PatientFindFile(const char* filePath, int numWaitTimes, int* waitTimes);

bool OpenArchive(const char* mpqPath, MPQHANDLE &mpq);

bool OpenArchive(const char* directory, const char* mpqName, MPQHANDLE &hMpq, std::string &outPath);

bool CloseArchive(MPQHANDLE mpq);

bool FileToBuffer(MPQHANDLE &hMpq, const char* fileName, buffer &buf);

bool FileToBuffer(MPQHANDLE &hStarDat, MPQHANDLE &hBrooDat, MPQHANDLE &hPatchRt, MPQHANDLE &hPriority, const char* fileName, buffer& buf);

bool FileToBuffer(MPQHANDLE &hStarDat, MPQHANDLE &hBrooDat, MPQHANDLE &hPatchRt, const char* fileName, buffer& buf);

bool StdMpqFileToBuffer(const char* fileName, buffer &buf);

bool FileToString(std::string fileName, std::string &str);

bool MakeFileCopy(const std::string &inFilePath, const std::string &outFilePath);

void RemoveFile(std::string fileName);

void RemoveFiles(std::string firstFileName, std::string secondFileName);

void RemoveFiles(std::string firstFileName, std::string secondFileName, std::string thirdFileName);

OPENFILENAME GetOfn(char* szFileName, const char* filter, int initFilter);

OPENFILENAME GetScSaveOfn(char* szFileName);

bool FileToBuffer(const char* FileName, buffer &buf);

DWORD GetSubKeyString(HKEY hParentKey, const char* subKey, const char* valueName, char* data, DWORD* dataSize);

bool GetRegString(char* dest, u32 destSize, const char* subKey, const char* valueName);

bool GetRegScPath(char* dest, u32 destSize);

#endif