#ifndef ARCHIVEFILE_H
#define ARCHIVEFILE_H
#include "Basics.h"
#include "FileBrowser.h"
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

/**
    This file defines a standard interface for archive files, e.g. zip, casc or MoPaQ (mpq)
    Strings for archiveFileName input must be UTF8 and UTF8 should be used for creation and modifying whenever possible

    ArchiveFile should not contain any system specific code
*/

class ArchiveFile;
using ArchiveFilePtr = std::shared_ptr<ArchiveFile>;

class ArchiveFile
{
public:
    ArchiveFile(bool deleteOnClose = false);

    virtual ~ArchiveFile();

    // Checks whether this archive will be deleted when close() is called
    virtual bool deletingOnClose() const;

    // Gets the filePath, the filePath will be empty unless an archive is open
    virtual const std::string & getFilePath() const = 0;

    // Checks whether this archive is open
    virtual bool isOpen() const = 0;

    // Checks whether an archive with the given filePath is open
    virtual bool isOpen(const std::string & filePath) const = 0;

    // Checks whether an openable archive exists at filePath
    virtual bool isValid(const std::string & filePath) const = 0;

    // Creates and opens an archive at the given filePath
    // If this archive is already open with the given filePath, no operation occurs and the method returns true
    // If this archive is already open with a filePath not matching the given filePath, it is closed before attempting to create the new archive
    // If a file already exists at filePath it is replaced with the new archive
    virtual bool create(const std::string & filePath) = 0;

    // Attempts to open an archive at filePath
    // If createIfNotFound is specified and no file can be found at filePath, the archive will be automatically created
    // If this archive is already open with the given filePath, no operation occurs and the method returns true
    // If this archive is already open with a filePath not matching the given filePath, it is closed before attempting to open the new archive
    virtual bool open(const std::string & filePath, bool readOnly = true, bool createIfNotFound = true) = 0;

    // Attempts to open an archive at filePath, if filePath is empty or no file can be found at filePath, and a fileBrowser is provided, attempts to resolve the path using fileBrowser->browseForFilePath("", 0)
    // If this archive is already open with the given filePath or the filePath browsed, no operation occurs and the method returns true
    // If this archive is already open with a filePath not matching the given filePath or the filePath browsed, it is closed before attempting to open the new archive
    template <typename FilterId>
    bool open(const std::string & filePath, const FileBrowserPtr<FilterId> fileBrowser) { return virtualizableOpen(filePath, fileBrowser); }

    // Attempts to open an archive if a fileBrowser is provided and fileBrowser->browseForFilePath("", 0) succeeds in finding a path
    // If this archive is already open with the browsed filePath, no operation occurs and the method returns true
    // If this archive is already open with a filePath not matching the filePath browsed, it is closed before attempting to open the new archive
    // Calling this method automatically closes the current archive if open
    template <typename FilterId>
    bool open(const FileBrowserPtr<FilterId> fileBrowser) { return virtualizableOpen(fileBrowser); }

    // Saves an archive, if changes have been made then the archive is saved
    // If no archive is open calling this method has no affect
    virtual void save() = 0;

    // Closes an archive, if changes have been made then the archive is saved
    // If no archive is open calling this method has no affect
    // If the deleteOnClose flag was specified earlier the archive is removed from disk after being closed
    virtual void close();

    // Checks whether a file exists within the archive at the given archivedFilePath
    // Cannot be used unless the archive is already open
    virtual bool findFile(const std::string & archivedFilePath) const = 0;

    // Checks whether a file exists within the archive at the given archivedFilePath
    // If an archive is already open with a filePath matching the provided filePath, the already opened archive is searched
    // Else attempts to find and search an archive at filePath
    virtual bool findFile(const std::string & filePath, const std::string & archivedFilePath) const = 0;

    // Attempts to get a file from this archive at archivedFilePath and place the data within the fileData buffer
    // Cannot be used unless the archive is already open
    virtual std::optional<std::vector<u8>> getFile(const std::string & archivedFilePath) const = 0;

    // Attempts to copy a file from this archive at archivePath to a new file at systemFilePath
    // Cannot be used unless the archive is already open
    virtual bool extractFile(const std::string & archivedFilePath, const std::string & systemFilePath) const = 0;

    // Attempts to add a file to this archive at archivedFilePath with the data from the fileData buffer
    // Cannot be used unless the archive is already open
    virtual bool addFile(const std::string & archivedFilePath, std::stringstream & fileData) = 0;

    // Attempts to add a file to this archive at archivedFilePath with the data from the fileData buffer
    // Cannot be used unless the archive is already open
    virtual bool addFile(const std::string & archivedFilePath, const std::vector<u8> & fileData) = 0;

    // Attempts to add the file at filePath to this archive at archivedFilePath
    // Cannot be used unless the archive is already open
    virtual bool addFile(const std::string & archivedFilePath, const std::string & filePath) = 0;

    // Attempts to rename the file in this archive at archivedFilePath to newArchivedFilePath
    // Cannot be used unless the archive is already open
    virtual bool renameFile(const std::string & archivedFilePath, const std::string & newArchivedFilePath) = 0;

    // Attempts to remove a file at archivedFilePath within this archive
    // Cannot be used unless the archive is already open
    virtual bool removeFile(const std::string & archivedFilePath) = 0;

protected:
    void setDeleteOnClose(bool deleteOnClose);

private:
    bool deleteOnClose;

    ArchiveFile(); // disallow ctor 
};

#endif