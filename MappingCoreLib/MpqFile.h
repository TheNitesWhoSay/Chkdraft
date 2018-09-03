#ifndef MPQFILE_H
#define MPQFILE_H
#include "Basics.h"
#include "Buffer.h"
#include "FileBrowser.h"
#include "../StormLib/src/StormLib.h"
#include <cstdint>
#include <algorithm>
#include <vector>
#include <string>
#include <map>

#ifdef max
#undef max
#endif

/**
While the format of strings may vary in the filesystem...
in MPQs they can be treated as UTF8
*/

enum class WavQuality : uint32_t
{
    Low = MPQ_WAVE_QUALITY_LOW,
    Med = MPQ_WAVE_QUALITY_MEDIUM,
    High = MPQ_WAVE_QUALITY_HIGH,
    Uncompressed = std::max(std::max(MPQ_WAVE_QUALITY_LOW, MPQ_WAVE_QUALITY_MEDIUM), MPQ_WAVE_QUALITY_HIGH)+1
};

class MpqFile;
using MpqFilePtr = std::shared_ptr<MpqFile>;

class MpqFile
{
public:
    // Constructs an unopened MPQ file, if delete is specified the MPQ is removed from the disk when close() is called
    MpqFile(bool deleteOnClose = false);

    // Calls close()
    virtual ~MpqFile();

    // Checks whether this MPQ will be deleted when close() is called
    bool deletingOnClose() const;

    // Gets the filePath, the filePath will be empty unless an MPQ is open
    virtual const std::string &getFilePath() const;

    // Checks whether this MPQ is open
    inline bool isOpen() const;

    // Checks whether an MPQ with the given filePath is open
    inline bool isOpen(const std::string &filePath) const;

    // Checks whether an openable MPQ exists at filePath
    bool isValid(const std::string &filePath) const;

    // Creates and opens an MPQ at the given filePath
    // If this MPQ is already open with the given filePath, no operation occurs and the method returns true
    // If this MPQ is already open with a filePath not matching the given filePath, it is closed before attempting to create the new MPQ
    // If a file already exists at filePath it is replaced with the new MPQ
    bool create(const std::string &filePath);

    // Attempts to open an MPQ at filePath
    // If createIfNotFound is specified and no file can be found at filePath, the MPQ will be automatically created
    // If this MPQ is already open with the given filePath, no operation occurs and the method returns true
    // If this MPQ is already open with a filePath not matching the given filePath, it is closed before attempting to open the new MPQ
    bool open(const std::string &filePath, bool createIfNotFound = true);

    // Attempts to open an MPQ at filePath, if filePath is empty or no file can be found at filePath, and a fileBrowser is provided, attempts to resolve the path using fileBrowser->browseForFilePath("", 0)
    // If this MPQ is already open with the given filePath or the filePath browsed, no operation occurs and the method returns true
    // If this MPQ is already open with a filePath not matching the given filePath or the filePath browsed, it is closed before attempting to open the new MPQ
    bool open(const std::string &filePath, const FileBrowserPtr fileBrowser);

    // Attempts to open an MPQ if a fileBrowser is provided and fileBrowser->browseForFilePath("", 0) succeeds in finding a path
    // If this MPQ is already open with the browsed filePath, no operation occurs and the method returns true
    // If this MPQ is already open with a filePath not matching the filePath browsed, it is closed before attempting to open the new MPQ
    // Calling this method automatically closes the current MPQ if open
    bool open(const FileBrowserPtr fileBrowser);

    // Saves an MPQ, if changes have been made then the MPQ is saved, if updateListFile is specified the listFile is updated with all changes made
    // If no MPQ is open calling this method has no affect
    // If the temporary flag was specified the MPQ is removed from disk after being closed
    void save(bool updateListFile = true);

    // Closes an MPQ, if changes have been made then the MPQ is saved, if updateListFile is specified the listFile is updated with all changes made
    // If no MPQ is open calling this method has no affect
    // If the temporary flag was specified the MPQ is removed from disk after being closed
    void close(bool updateListFile = true);

    // Checks whether an file exists within the MPQ at the given mpqPath
    // Cannot be used unless the MPQ is already open
    bool findFile(const std::string &mpqPath) const;

    // Checks whether an file exists within the MPQ at the given mpqPath
    // If an MPQ is already open with a filePath matching the provided filePath, the already opened MPQ is searched
    // Else attempts to find and search an MPQ at filePath
    bool findFile(const std::string &filePath, const std::string &mpqPath) const;

    // Attempts to get a file from this MPQ at mpqPath and place the data within the fileData buffer
    // Cannot be used unless the MPQ is already open
    bool getFile(const std::string &mpqPath, buffer &fileData);

    // Attempts to add a file to this MPQ at mpqPath with the data from the fileData buffer
    // Cannot be used unless the MPQ is already open
    bool addFile(const std::string &mpqPath, const buffer &fileData, WavQuality wavQuality = WavQuality::Uncompressed);

    // Attempts to add the file at filePath to this MPQ at mpqPath
    // Cannot be used unless the MPQ is already open
    bool addFile(const std::string &mpqPath, const std::string &filePath, WavQuality wavQuality = WavQuality::Uncompressed);

    // Attempts to rename the file in this MPQ at mpqPath to newMpqPath
    // Cannot be used unless the MPQ is already open
    bool renameFile(const std::string &mpqPath, const std::string &newMpqPath, WavQuality wavQuality = WavQuality::Uncompressed);

    // Attempts to remove a file at mpqPath within this MPQ
    // Cannot be used unless the MPQ is already open
    bool removeFile(const std::string &mpqPath);

private:
    bool deleteOnClose;
    bool madeChanges;
    std::vector<std::string> addedMpqAssetPaths;
    std::string filePath;
    HANDLE hMpq;

    friend bool getFile(void* source, const std::string &mpqPath, buffer &fileData);

    // Removes this MPQ from the disk, only valid if the MPQ has already been closed but filePath is still set
    bool remove();
};

#endif