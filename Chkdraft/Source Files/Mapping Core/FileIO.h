#ifndef FILEIO_H
#define FILEIO_H
#include "Buffer.h"
#include "SFmpqapi.h"
#include <fstream>

bool FindFile(const char* filePath);

bool PatientFindFile(const char* filePath, int numWaitTimes, int* waitTimes);

bool OpenArchive(const char* szFilePath, const char* FileName, MPQHANDLE &hMpq);

bool CloseArchive(MPQHANDLE mpq);

bool FileToBuffer(MPQHANDLE &hMpq, const char* fileName, buffer &buf);

bool FileToBuffer(MPQHANDLE &hStarDat, MPQHANDLE &hBrooDat, MPQHANDLE &hPatchRt, MPQHANDLE &hPriority, const char* fileName, buffer& buf);

bool FileToBuffer(MPQHANDLE &hStarDat, MPQHANDLE &hBrooDat, MPQHANDLE &hPatchRt, const char* fileName, buffer& buf);

bool FileToString(string fileName, string &str);

void RemoveFile(string fileName);

void RemoveFiles(string firstFileName, string secondFileName);

void RemoveFiles(string firstFileName, string secondFileName, string thirdFileName);

OPENFILENAME GetOfn(char* szFileName, char* filter, int initFilter);

OPENFILENAME GetScSaveOfn(char* szFileName);

bool FileToBuffer(const char* FileName, buffer &buf);

DWORD GetSubKeyString(HKEY hParentKey, const char* subKey, const char* valueName, char* data, DWORD* dataSize);

bool GetRegString(char* dest, u32 destSize, const char* subKey, const char* valueName);

bool GetRegScPath(char* dest, u32 destSize);

#endif