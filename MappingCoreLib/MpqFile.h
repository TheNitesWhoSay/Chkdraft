#ifndef MPQFILE_H
#define MPQFILE_H
#include "Basics.h"
#include "FileBrowser.h"
#include "ArchiveFile.h"
#include "../StormLib/src/StormLib.h"
#include <algorithm>
#include <memory>
#include <sstream>

/**
    An MPQ file is nothing more than an archive format (like .zip) specialized for StarCraft

    An MPQ file may be created or deleted, files may be added or removed from the archive, and files within may be enumerated (not always with their original file names) and their contents read

    MpqFile is in turn built on top of a version of StormLib that has been modified for use in Mapping Core
*/

enum class AssetAction;
class ModifiedAsset;
using ModifiedAssetPtr = std::shared_ptr<ModifiedAsset>;

enum_t(WavQuality, uint32_t, {
    Low = MPQ_WAVE_QUALITY_LOW,
    Med = MPQ_WAVE_QUALITY_MEDIUM,
    High = MPQ_WAVE_QUALITY_HIGH,
    Uncompressed = std::max(std::max(MPQ_WAVE_QUALITY_LOW, MPQ_WAVE_QUALITY_MEDIUM), MPQ_WAVE_QUALITY_HIGH)+1
});

class MpqFile;
using MpqFilePtr = std::shared_ptr<MpqFile>;

class MpqFile : public ArchiveFile
{
public:
    // Constructs an unopened MPQ file, if delete is specified the MPQ is removed from the disk when close() is called
    MpqFile(bool deleteOnClose = false, bool updateListFile = true);

    // Calls close()
    virtual ~MpqFile();

    // Gets the filePath, the filePath will be empty unless an MPQ is open
    virtual const std::string & getFilePath() const;

    // Checks whether this MPQ is open
    virtual bool isOpen() const;

    // Checks whether an MPQ with the given filePath is open
    virtual bool isOpen(const std::string & filePath) const;

    // Checks whether the listfile will be opened upon save
    virtual bool isUpdatingListFile() const;

    // Checks whether an openable MPQ exists at filePath
    virtual bool isValid(const std::string & filePath) const;

    // Creates and opens an MPQ at the given filePath
    // If this MPQ is already open with the given filePath, no operation occurs and the method returns true
    // If this MPQ is already open with a filePath not matching the given filePath, it is closed before attempting to create the new MPQ
    // If a file already exists at filePath it is replaced with the new MPQ
    virtual bool create(const std::string & filePath);

    // Attempts to open an MPQ at filePath
    // If createIfNotFound is specified and no file can be found at filePath, the MPQ will be automatically created
    // If this MPQ is already open with the given filePath, no operation occurs and the method returns true
    // If this MPQ is already open with a filePath not matching the given filePath, it is closed before attempting to open the new MPQ
    virtual bool open(const std::string & filePath, bool readOnly = true, bool createIfNotFound = true);

    virtual void setUpdatingListFile(bool updateListFile);

    // Saves an MPQ, if changes have been made then the MPQ is saved, if updateListFile was specified the listFile is updated with all changes made
    // If no MPQ is open calling this method has no affect
    virtual void save();

    // Closes an MPQ, if changes have been made then the MPQ is saved, if updateListFile was specified the listFile is updated with all changes made
    // If no MPQ is open calling this method has no affect
    // If the temporary flag was specified the MPQ is removed from disk after being closed
    virtual void close();

    // Checks whether a file exists within the MPQ at the given mpqPath
    // Cannot be used unless the MPQ is already open
    virtual bool findFile(const std::string & mpqPath) const;

    // Checks whether a file exists within the MPQ at the given mpqPath
    // If an MPQ is already open with a filePath matching the provided filePath, the already opened MPQ is searched
    // Else attempts to find and search an MPQ at filePath
    virtual bool findFile(const std::string & filePath, const std::string & mpqPath) const;

    // Attempts to get a file from this MPQ at mpqPath and place the data within the fileData buffer
    // Cannot be used unless the MPQ is already open
    virtual bool getFile(const std::string & mpqPath, std::vector<u8> & fileData) const;

    // Attempts to copy a file from this MPQ at mpqPath to a new file at systemFilePath
    // Cannot be used unless the MPQ is already open
    virtual bool extractFile(const std::string & mpqPath, const std::string & systemFilePath) const;

    // Adds an entry to the list file
    // Cannot be used unless the MPQ is already open
    virtual bool addListFileEntry(const std::string & listFileEntry);

    // Attempts to add a file to this MPQ at mpqPath with the data from the fileData buffer
    // Cannot be used unless the MPQ is already open
    virtual bool addFile(const std::string & mpqPath, std::stringstream & fileData);

    // Attempts to add a file to this MPQ at mpqPath with the data from the fileData buffer
    // Cannot be used unless the MPQ is already open
    virtual bool addFile(const std::string & mpqPath, std::stringstream & fileData, WavQuality wavQuality);

    // Attempts to add a file to this MPQ at mpqPath with the data from the fileData buffer
    // Cannot be used unless the MPQ is already open
    virtual bool addFile(const std::string & mpqPath, const std::vector<u8> & fileData);

    // Attempts to add a file to this MPQ at mpqPath with the data from the fileData buffer
    // Cannot be used unless the MPQ is already open
    virtual bool addFile(const std::string & mpqPath, const std::vector<u8> & fileData, WavQuality wavQuality);

    // Attempts to add the file at filePath to this MPQ at mpqPath
    // Cannot be used unless the MPQ is already open
    virtual bool addFile(const std::string & mpqPath, const std::string & filePath);

    // Attempts to add the file at filePath to this MPQ at mpqPath
    // Cannot be used unless the MPQ is already open
    virtual bool addFile(const std::string & mpqPath, const std::string & filePath, WavQuality wavQuality);

    // Attempts to rename the file in this MPQ at mpqPath to newMpqPath
    // Cannot be used unless the MPQ is already open
    virtual bool renameFile(const std::string & mpqPath, const std::string & newMpqPath);

    // Attempts to remove a file at mpqPath within this MPQ
    // Cannot be used unless the MPQ is already open
    virtual bool removeFile(const std::string & mpqPath);

private:
    bool updateListFile;
    bool madeChanges;
    std::vector<std::string> addedMpqAssetPaths;
    std::string filePath;
    HANDLE hMpq;

    // Removes this MPQ from the disk, only valid if the MPQ has already been closed but filePath is still set
    bool remove();
};

enum class AssetAction {
    Add, Remove
};

// Describes a file modified in an MPQ
class ModifiedAsset
{
public:
    std::string assetMpqPath;
    std::string assetTempMpqPath; // Unique path auto-generated by constructor
    WavQuality wavQualitySelected;
    AssetAction actionTaken;

    ModifiedAsset(const std::string & assetMpqPath, AssetAction actionTaken, WavQuality wavQualitySelected = WavQuality::Uncompressed);
    virtual ~ModifiedAsset();

private:
    static u64 nextAssetId; // Changes are needed if this is accessed in a multi-threaded environment

    ModifiedAsset(); // Disallow ctor
};

extern std::vector<FilterEntry<u32>> getMpqFilter();

#endif