/*

  ShadowFlare MPQ API Library. (c) ShadowFlare Software 2002-2010
  License information for this code is in license.txt and
  included in this file at the end of this comment.

  All functions below are actual functions that are part of this
  library and do not need any additional dll files.  It does not
  even require Storm to be able to decompress or compress files.

  This library emulates the interface of Lmpqapi and Storm MPQ
  functions, so it may be used as a replacement for them in
  MPQ extractors/archivers without even needing to recompile
  the program that uses Lmpqapi or Storm.  It has a few features
  not included in Lmpqapi and Storm, such as extra flags for some
  functions, setting the locale ID of existing files, and adding
  files without having to write them somewhere else first.  Also,
  MPQ handles used by functions prefixed with "SFile" and "Mpq"
  can be used interchangably; all functions use the same type
  of MPQ handles.  You cannot, however, use handles from this
  library with storm or lmpqapi or vice-versa.  Doing so will
  most likely result in a crash.

  Revision History:
  (Release date) 1.08 (ShadowFlare)
  - Fixed a buffer overflow that would occur when reading files
    if neither using a buffer that is large enough to contain the
    entire file nor has a size that is a multiple of 4096
  - Added SFileOpenFileAsArchive which opens an archive that is
    contained within an already open archive
  - Added MpqRenameAndSetFileLocale and MpqDeleteFileWithLocale.
    These have extra parameters that allow you to use them with
    files having language codes other than what was last set
    using SFileSetLocale
  - Fixed a bug that caused (listfile) to get cleared if adding
    files with a locale ID other than 0
  - Added MpqOpenArchiveForUpdateEx which allows creating
    archives with different block sizes
  - SFileListFiles can list the contents of bncache.dat without
    needing an external list

  06/12/2002 1.07 (ShadowFlare)
  - No longer requires Storm.dll to compress or decompress
    Warcraft III files
  - Added SFileListFiles for getting names and information
    about all of the files in an archive
  - Fixed a bug with renaming and deleting files
  - Fixed a bug with adding wave compressed files with
    low compression setting
  - Added a check in MpqOpenArchiveForUpdate for proper
    dwMaximumFilesInArchive values (should be a number that
    is a power of 2).  If it is not a proper value, it will
    be rounded up to the next higher power of 2

  05/09/2002 1.06 (ShadowFlare)
  - Compresses files without Storm.dll!
  - If Warcraft III is installed, this library will be able to
    find Storm.dll on its own. (Storm.dll is needed to
    decompress Warcraft III files)
  - Fixed a bug where an embedded archive and the file that
    contains it would be corrupted if the archive was modified
  - Able to open all .w3m maps now

  29/06/2002 1.05 (ShadowFlare)
  - Supports decompressing files from Warcraft III MPQ archives
    if using Storm.dll from Warcraft III
  - Added MpqAddFileToArchiveEx and MpqAddFileFromBufferEx for
    using extra compression types

  29/05/2002 1.04 (ShadowFlare)
  - Files can be compressed now!
  - Fixed a bug in SFileReadFile when reading data not aligned
    to the block size
  - Optimized some of SFileReadFile's code.  It can read files
    faster now
  - SFile functions may now be used to access files not in mpq
    archives as you can with the real storm functions
  - MpqCompactArchive will no longer corrupt files with the
    MODCRYPTKEY flag as long as the file is either compressed,
    listed in "(listfile)", is "(listfile)", or is located in
    the same place in the compacted archive; so it is safe
    enough to use it on almost any archive
  - Added MpqAddWaveFromBuffer
  - Better handling of archives with no files
  - Fixed compression with COMPRESS2 flag

  15/05/2002 1.03 (ShadowFlare)
  - Supports adding files with the compression attribute (does
    not actually compress files).  Now archives created with
    this dll can have files added to them through lmpqapi
    without causing staredit to crash
  - SFileGetBasePath and SFileSetBasePath work more like their
    Storm equivalents now
  - Implemented MpqCompactArchive, but it is not finished yet.
    In its current state, I would recommend against using it
    on archives that contain files with the MODCRYPTKEY flag,
    since it will corrupt any files with that flag
  - Added SFMpqGetVersionString2 which may be used in Visual
    Basic to get the version string

  07/05/2002 1.02 (ShadowFlare)
  - SFileReadFile no longer passes the lpOverlapped parameter it
    receives to ReadFile.  This is what was causing the function
    to fail when used in Visual Basic
  - Added support for more Storm MPQ functions
  - GetLastError may now be used to get information about why a
    function failed

  01/05/2002 1.01 (ShadowFlare)
  - Added ordinals for Storm MPQ functions
  - Fixed MPQ searching functionality of SFileOpenFileEx
  - Added a check for whether a valid handle is given when
    SFileCloseArchive is called
  - Fixed functionality of SFileSetArchivePriority when multiple
    files are open
  - File renaming works for all filenames now
  - SFileReadFile no longer reallocates the buffer for each block
    that is decompressed.  This should make SFileReadFile at least
    a little faster

  30/04/2002 1.00 (ShadowFlare)
  - First version.
  - Compression not yet supported
  - Does not use SetLastError yet, so GetLastError will not return any
    errors that have to do with this library
  - MpqCompactArchive not implemented

  Any comments or suggestions are accepted at blakflare@hotmail.com (ShadowFlare)

  License information:

  Copyright (c) 2002-2010, ShadowFlare <blakflare@hotmail.com>
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
  SUCH DAMAGE.
*/

#ifndef SHADOWFLARE_MPQ_API_INCLUDED
#define SHADOWFLARE_MPQ_API_INCLUDED

#include <windows.h>

#ifndef SFMPQ_STATIC

#ifdef SFMPQAPI_EXPORTS
#define SFMPQAPI __declspec(dllexport)
#else
#define SFMPQAPI __declspec(dllimport)
#endif

#else
#define SFMPQAPI
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	WORD Major;
	WORD Minor;
	WORD Revision;
	WORD Subrevision;
} SFMPQVERSION;

// MpqInitialize does nothing.  It is only provided for
// compatibility with MPQ archivers that use lmpqapi.
BOOL   SFMPQAPI WINAPI MpqInitialize();

LPCSTR SFMPQAPI WINAPI MpqGetVersionString();
float  SFMPQAPI WINAPI MpqGetVersion();

void SFMPQAPI WINAPI SFMpqDestroy(); // This no longer needs to be called.  It is only provided for compatibility with older versions

void SFMPQAPI WINAPI AboutSFMpq(); // Displays an about page in a web browser (this has only been tested in Internet Explorer). This is only for the dll version of SFmpq

// SFMpqGetVersionString2's return value is the required length of the buffer plus
// the terminating null, so use SFMpqGetVersionString2(0, 0); to get the length.
LPCSTR SFMPQAPI WINAPI SFMpqGetVersionString();
DWORD  SFMPQAPI WINAPI SFMpqGetVersionString2(LPSTR lpBuffer, DWORD dwBufferLength);
SFMPQVERSION SFMPQAPI WINAPI SFMpqGetVersion();

// Returns 0 if the dll version is equal to the version your program was compiled
// with, 1 if the dll is newer, -1 if the dll is older.
long SFMPQAPI __inline SFMpqCompareVersion();

// General error codes
#define MPQ_ERROR_MPQ_INVALID      0x85200065
#define MPQ_ERROR_FILE_NOT_FOUND   0x85200066
#define MPQ_ERROR_DISK_FULL        0x85200068 //Physical write file to MPQ failed
#define MPQ_ERROR_HASH_TABLE_FULL  0x85200069
#define MPQ_ERROR_ALREADY_EXISTS   0x8520006A
#define MPQ_ERROR_BAD_OPEN_MODE    0x8520006C //When MOAU_READ_ONLY is used without MOAU_OPEN_EXISTING

#define MPQ_ERROR_COMPACT_ERROR    0x85300001

// MpqOpenArchiveForUpdate flags
#define MOAU_CREATE_NEW          0x00 //If archive does not exist, it will be created. If it exists, the function will fail
#define MOAU_CREATE_ALWAYS       0x08 //Will always create a new archive
#define MOAU_OPEN_EXISTING       0x04 //If archive exists, it will be opened. If it does not exist, the function will fail
#define MOAU_OPEN_ALWAYS         0x20 //If archive exists, it will be opened. If it does not exist, it will be created
#define MOAU_READ_ONLY           0x10 //Must be used with MOAU_OPEN_EXISTING. Archive will be opened without write access
#define MOAU_MAINTAIN_ATTRIBUTES 0x02 //Will be used in a future version to create the (attributes) file
#define MOAU_MAINTAIN_LISTFILE   0x01 //Creates and maintains a list of files in archive when they are added, replaced, or deleted

// MpqOpenArchiveForUpdateEx constants
#define DEFAULT_BLOCK_SIZE 3 // 512 << number = block size
#define USE_DEFAULT_BLOCK_SIZE 0xFFFFFFFF // Use default block size that is defined internally

// MpqAddFileToArchive flags
#define MAFA_EXISTS           0x80000000 //This flag will be added if not present
#define MAFA_UNKNOWN40000000  0x40000000 //Unknown flag
#define MAFA_SINGLEBLOCK      0x01000000 //File is stored as a single unit rather than being split by the block size
#define MAFA_MODCRYPTKEY      0x00020000 //Used with MAFA_ENCRYPT. Uses an encryption key based on file position and size
#define MAFA_ENCRYPT          0x00010000 //Encrypts the file. The file is still accessible when using this, so the use of this has depreciated
#define MAFA_COMPRESS         0x00000200 //File is to be compressed when added. This is used for most of the compression methods
#define MAFA_COMPRESS2        0x00000100 //File is compressed with standard compression only (was used in Diablo 1)
#define MAFA_REPLACE_EXISTING 0x00000001 //If file already exists, it will be replaced

// MpqAddFileToArchiveEx compression flags
#define MAFA_COMPRESS_STANDARD 0x08 //Standard PKWare DCL compression
#define MAFA_COMPRESS_DEFLATE  0x02 //ZLib's deflate compression
#define MAFA_COMPRESS_BZIP2    0x10 //bzip2 compression
#define MAFA_COMPRESS_WAVE     0x81 //Stereo wave compression
#define MAFA_COMPRESS_WAVE2    0x41 //Mono wave compression

// Flags for individual compression types used for wave compression
#define MAFA_COMPRESS_WAVECOMP1 0x80 //Main compressor for stereo wave compression
#define	MAFA_COMPRESS_WAVECOMP2 0x40 //Main compressor for mono wave compression
#define MAFA_COMPRESS_WAVECOMP3 0x01 //Secondary compressor for wave compression

// ZLib deflate compression level constants (used with MpqAddFileToArchiveEx and MpqAddFileFromBufferEx)
#define Z_NO_COMPRESSION         0
#define Z_BEST_SPEED             1
#define Z_BEST_COMPRESSION       9
#define Z_DEFAULT_COMPRESSION  (-1) //Default level is 6 with current ZLib version

// MpqAddWaveToArchive quality flags
#define MAWA_QUALITY_HIGH    1 //Higher compression, lower quality
#define MAWA_QUALITY_MEDIUM  0 //Medium compression, medium quality
#define MAWA_QUALITY_LOW     2 //Lower compression, higher quality

// SFileGetFileInfo flags
#define SFILE_INFO_BLOCK_SIZE      0x01 //Block size in MPQ
#define SFILE_INFO_HASH_TABLE_SIZE 0x02 //Hash table size in MPQ
#define SFILE_INFO_NUM_FILES       0x03 //Number of files in MPQ
#define SFILE_INFO_TYPE            0x04 //Is MPQHANDLE a file or an MPQ?
#define SFILE_INFO_SIZE            0x05 //Size of MPQ or uncompressed file
#define SFILE_INFO_COMPRESSED_SIZE 0x06 //Size of compressed file
#define SFILE_INFO_FLAGS           0x07 //File flags (compressed, etc.), file attributes if a file not in an archive
#define SFILE_INFO_PARENT          0x08 //Handle of MPQ that file is in
#define SFILE_INFO_POSITION        0x09 //Position of file pointer in files
#define SFILE_INFO_LOCALEID        0x0A //Locale ID of file in MPQ
#define SFILE_INFO_PRIORITY        0x0B //Priority of open MPQ
#define SFILE_INFO_HASH_INDEX      0x0C //Hash table index of file in MPQ
#define SFILE_INFO_BLOCK_INDEX     0x0D //Block table index of file in MPQ

// Return values of SFileGetFileInfo when SFILE_INFO_TYPE flag is used
#define SFILE_TYPE_MPQ  0x01
#define SFILE_TYPE_FILE 0x02

// SFileListFiles flags
#define SFILE_LIST_MEMORY_LIST  0x01 // Specifies that lpFilelists is a file list from memory, rather than being a list of file lists
#define SFILE_LIST_ONLY_KNOWN   0x02 // Only list files that the function finds a name for
#define SFILE_LIST_ONLY_UNKNOWN 0x04 // Only list files that the function does not find a name for
#define SFILE_LIST_FLAG_UNKNOWN 0x08 // Use without SFILE_LIST_ONLY_KNOWN or SFILE_LIST_FLAG_UNKNOWN to list all files, but will set dwFileExists to 3 if file's name is not found

// SFileOpenArchive flags
#define SFILE_OPEN_HARD_DISK_FILE 0x0000 //Open archive without regard to the drive type it resides on
#define SFILE_OPEN_CD_ROM_FILE    0x0001 //Open the archive only if it is on a CD-ROM
#define SFILE_OPEN_ALLOW_WRITE    0x8000 //Open file with write access

// SFileOpenFileEx search scopes
#define SFILE_SEARCH_CURRENT_ONLY 0x00 //Used with SFileOpenFileEx; only the archive with the handle specified will be searched for the file
#define SFILE_SEARCH_ALL_OPEN     0x01 //SFileOpenFileEx will look through all open archives for the file. This flag also allows files outside the archive to be used

typedef HANDLE MPQHANDLE;

#include <pshpack1.h>

struct FILELISTENTRY {
	DWORD dwFileExists; // Nonzero if this entry is used
	LCID lcLocale; // Locale ID of file
	DWORD dwCompressedSize; // Compressed size of file
	DWORD dwFullSize; // Uncompressed size of file
	DWORD dwFlags; // Flags for file
	char szFileName[260];
};

struct MPQARCHIVE;
struct MPQFILE;
struct MPQHEADER;
struct BLOCKTABLEENTRY;
struct HASHTABLEENTRY;

struct MPQHEADER {
	DWORD dwMPQID; //"MPQ\x1A" for mpq's, "BN3\x1A" for bncache.dat
	DWORD dwHeaderSize; // Size of this header
	DWORD dwMPQSize; //The size of the mpq archive
	WORD wUnused0C; // Seems to always be 0
	WORD wBlockSize; // Size of blocks in files equals 512 << wBlockSize
	DWORD dwHashTableOffset; // Offset to hash table
	DWORD dwBlockTableOffset; // Offset to block table
	DWORD dwHashTableSize; // Number of entries in hash table
	DWORD dwBlockTableSize; // Number of entries in block table
};

#include <poppack.h>

//Archive handles may be typecasted to this struct so you can access
//some of the archive's properties and the decrypted hash table and
//block table directly.  This struct is based on Storm's internal
//struct for archive handles.
struct MPQARCHIVE {
	// Arranged according to priority with lowest priority first
	MPQARCHIVE * lpNextArc; //0// Pointer to the next MPQARCHIVE struct. Pointer to addresses of first and last archives if last archive
	MPQARCHIVE * lpPrevArc; //4// Pointer to the previous MPQARCHIVE struct. 0xEAFC5E23 if first archive
	char szFileName[260]; //8// Filename of the archive
	HANDLE hFile; //10C// The archive's file handle
	DWORD dwFlags1; //110// Some flags, bit 0 seems to be set when opening an archive from a hard drive if bit 1 in the flags for SFileOpenArchive is set, bit 1 (0 based) seems to be set when opening an archive from a CD
	DWORD dwPriority; //114// Priority of the archive set when calling SFileOpenArchive
	MPQFILE * lpLastReadFile; //118// Pointer to the last read file's MPQFILE struct. This is cleared when finished reading a block
	DWORD dwUnk; //11C// Seems to always be 0
	DWORD dwBlockSize; //120// Size of file blocks in bytes
	BYTE * lpLastReadBlock; //124// Pointer to the read buffer for archive. This is cleared when finished reading a block
	DWORD dwBufferSize; //128// Size of the read buffer for archive. This is cleared when finished reading a block
	DWORD dwMPQStart; //12C// The starting offset of the archive
	DWORD dwMPQEnd; //130// The ending offset of the archive
	MPQHEADER * lpMPQHeader; //134// Pointer to the archive header
	BLOCKTABLEENTRY * lpBlockTable; //138// Pointer to the start of the block table
	HASHTABLEENTRY * lpHashTable; //13C// Pointer to the start of the hash table
	DWORD dwReadOffset; //140// Offset to the data for a file
	DWORD dwRefCount; //144// Count of references to this open archive.  This is incremented for each file opened from the archive, and decremented for each file closed
	// Extra struct members used by SFmpq
	MPQHEADER MpqHeader;
	DWORD dwFlags; //The only flags that should be changed are MOAU_MAINTAIN_LISTFILE and MOAU_MAINTAIN_ATTRIBUTES, changing any others can have unpredictable effects
	LPSTR lpFileName;
	DWORD dwExtraFlags;
};

//Handles to files in the archive may be typecasted to this struct
//so you can access some of the file's properties directly.  This
//struct is based on Storm's internal struct for file handles.
struct MPQFILE {
	MPQFILE * lpNextFile; //0// Pointer to the next MPQFILE struct. Pointer to addresses of first and last files if last file
	MPQFILE * lpPrevFile; //4// Pointer to the previous MPQFILE struct. 0xEAFC5E13 if first file
	char szFileName[260]; //8// Filename of the file
	HANDLE hFile; //10C// Always INVALID_HANDLE_VALUE for files in MPQ archives. For files not in MPQ archives, this is the file handle for the file and the rest of this struct is filled with zeros except for dwRefCount
	MPQARCHIVE * lpParentArc; //110// Pointer to the MPQARCHIVE struct of the archive in which the file is contained
	BLOCKTABLEENTRY * lpBlockEntry; //114// Pointer to the file's block table entry
	DWORD dwCryptKey; //118// Decryption key for the file
	DWORD dwFilePointer; //11C// Position of file pointer in the file
	DWORD dwUnk; //120// Seems to always be 0
	DWORD dwBlockCount; //124// Number of blocks in file
	DWORD * lpdwBlockOffsets; //128// Offsets to blocks in file. There are 1 more of these than the number of blocks. The values for this are set after the first read
	DWORD dwReadStarted; //12C// Set to 1 after first read
	BOOL bStreaming; //130// 1 when streaming a WAVE
	BYTE * lpLastReadBlock; //134// Pointer to the read buffer for file. This starts at the position specified in the last SFileSetFilePointer call. This is cleared when SFileSetFilePointer is called or when finished reading the block
	DWORD dwBytesRead; //138// Total bytes read from the current block in the open file. This is cleared when SFileSetFilePointer is called or when finished reading the block
	DWORD dwBufferSize; //13C// Size of the read buffer for file. This is cleared when SFileSetFilePointer is called or when finished reading the block
	DWORD dwRefCount; //140// Count of references to this open file
	// Extra struct members used by SFmpq
	HASHTABLEENTRY *lpHashEntry;
	LPSTR lpFileName;
};

#include <pshpack1.h>

struct BLOCKTABLEENTRY {
	DWORD dwFileOffset; // Offset to file
	DWORD dwCompressedSize; // Compressed size of file
	DWORD dwFullSize; // Uncompressed size of file
	DWORD dwFlags; // Flags for file
};

struct HASHTABLEENTRY {
	DWORD dwNameHashA; // First name hash of file
	DWORD dwNameHashB; // Second name hash of file
	LCID lcLocale; // Locale ID of file
	DWORD dwBlockTableIndex; // Index to the block table entry for the file
};

#include <poppack.h>

// Defines for backward compatibility with old lmpqapi function names
#define MpqAddFileToArcive MpqAddFileToArchive
#define MpqOpenArchive     SFileOpenArchive
#define MpqOpenFileEx      SFileOpenFileEx
#define MpqGetFileSize     SFileGetFileSize
#define MpqReadFile        SFileReadFile
#define MpqCloseFile       SFileCloseFile
#define MpqCloseArchive    SFileCloseArchive

// Storm functions implemented by this library
BOOL      SFMPQAPI WINAPI SFileOpenArchive(LPCSTR lpFileName, DWORD dwPriority, DWORD dwFlags, MPQHANDLE *hMPQ);
BOOL      SFMPQAPI WINAPI SFileCloseArchive(MPQHANDLE hMPQ);
BOOL      SFMPQAPI WINAPI SFileOpenFileAsArchive(MPQHANDLE hSourceMPQ, LPCSTR lpFileName, DWORD dwPriority, DWORD dwFlags, MPQHANDLE *hMPQ);
BOOL      SFMPQAPI WINAPI SFileGetArchiveName(MPQHANDLE hMPQ, LPSTR lpBuffer, DWORD dwBufferLength);
BOOL      SFMPQAPI WINAPI SFileOpenFile(LPCSTR lpFileName, MPQHANDLE *hFile);
BOOL      SFMPQAPI WINAPI SFileOpenFileEx(MPQHANDLE hMPQ, LPCSTR lpFileName, DWORD dwSearchScope, MPQHANDLE *hFile);
BOOL      SFMPQAPI WINAPI SFileCloseFile(MPQHANDLE hFile);
DWORD     SFMPQAPI WINAPI SFileGetFileSize(MPQHANDLE hFile, LPDWORD lpFileSizeHigh);
BOOL      SFMPQAPI WINAPI SFileGetFileArchive(MPQHANDLE hFile, MPQHANDLE *hMPQ);
BOOL      SFMPQAPI WINAPI SFileGetFileName(MPQHANDLE hFile, LPSTR lpBuffer, DWORD dwBufferLength);
DWORD     SFMPQAPI WINAPI SFileSetFilePointer(MPQHANDLE hFile, LONG lDistanceToMove, PLONG lplDistanceToMoveHigh, DWORD dwMoveMethod);
BOOL      SFMPQAPI WINAPI SFileReadFile(MPQHANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped);
LCID      SFMPQAPI WINAPI SFileSetLocale(LCID nNewLocale);
BOOL      SFMPQAPI WINAPI SFileGetBasePath(LPCSTR lpBuffer, DWORD dwBufferLength);
BOOL      SFMPQAPI WINAPI SFileSetBasePath(LPCSTR lpNewBasePath);

// Extra storm-related functions
DWORD     SFMPQAPI WINAPI SFileGetFileInfo(MPQHANDLE hFile, DWORD dwInfoType);
BOOL      SFMPQAPI WINAPI SFileSetArchivePriority(MPQHANDLE hMPQ, DWORD dwPriority);
DWORD     SFMPQAPI WINAPI SFileFindMpqHeader(HANDLE hFile);
BOOL      SFMPQAPI WINAPI SFileListFiles(MPQHANDLE hMPQ, LPCSTR lpFileLists, FILELISTENTRY *lpListBuffer, DWORD dwFlags);

// Archive editing functions implemented by this library
MPQHANDLE SFMPQAPI WINAPI MpqOpenArchiveForUpdate(LPCSTR lpFileName, DWORD dwFlags, DWORD dwMaximumFilesInArchive);
DWORD     SFMPQAPI WINAPI MpqCloseUpdatedArchive(MPQHANDLE hMPQ, DWORD dwUnknown2);
BOOL      SFMPQAPI WINAPI MpqAddFileToArchive(MPQHANDLE hMPQ, LPCSTR lpSourceFileName, LPCSTR lpDestFileName, DWORD dwFlags);
BOOL      SFMPQAPI WINAPI MpqAddWaveToArchive(MPQHANDLE hMPQ, LPCSTR lpSourceFileName, LPCSTR lpDestFileName, DWORD dwFlags, DWORD dwQuality);
BOOL      SFMPQAPI WINAPI MpqRenameFile(MPQHANDLE hMPQ, LPCSTR lpcOldFileName, LPCSTR lpcNewFileName);
BOOL      SFMPQAPI WINAPI MpqDeleteFile(MPQHANDLE hMPQ, LPCSTR lpFileName);
BOOL      SFMPQAPI WINAPI MpqCompactArchive(MPQHANDLE hMPQ);

// Extra archive editing functions
MPQHANDLE SFMPQAPI WINAPI MpqOpenArchiveForUpdateEx(LPCSTR lpFileName, DWORD dwFlags, DWORD dwMaximumFilesInArchive, DWORD dwBlockSize);
BOOL      SFMPQAPI WINAPI MpqAddFileToArchiveEx(MPQHANDLE hMPQ, LPCSTR lpSourceFileName, LPCSTR lpDestFileName, DWORD dwFlags, DWORD dwCompressionType, DWORD dwCompressLevel);
BOOL      SFMPQAPI WINAPI MpqAddFileFromBufferEx(MPQHANDLE hMPQ, LPVOID lpBuffer, DWORD dwLength, LPCSTR lpFileName, DWORD dwFlags, DWORD dwCompressionType, DWORD dwCompressLevel);
BOOL      SFMPQAPI WINAPI MpqAddFileFromBuffer(MPQHANDLE hMPQ, LPVOID lpBuffer, DWORD dwLength, LPCSTR lpFileName, DWORD dwFlags);
BOOL      SFMPQAPI WINAPI MpqAddWaveFromBuffer(MPQHANDLE hMPQ, LPVOID lpBuffer, DWORD dwLength, LPCSTR lpFileName, DWORD dwFlags, DWORD dwQuality);
BOOL      SFMPQAPI WINAPI MpqRenameAndSetFileLocale(MPQHANDLE hMPQ, LPCSTR lpcOldFileName, LPCSTR lpcNewFileName, LCID nOldLocale, LCID nNewLocale);
BOOL      SFMPQAPI WINAPI MpqDeleteFileWithLocale(MPQHANDLE hMPQ, LPCSTR lpFileName, LCID nLocale);
BOOL      SFMPQAPI WINAPI MpqSetFileLocale(MPQHANDLE hMPQ, LPCSTR lpFileName, LCID nOldLocale, LCID nNewLocale);

// These functions do nothing.  They are only provided for
// compatibility with MPQ extractors that use storm.
BOOL      SFMPQAPI WINAPI SFileDestroy();
void      SFMPQAPI WINAPI StormDestroy();

#ifdef __cplusplus
};  // extern "C" 
#endif

#endif

