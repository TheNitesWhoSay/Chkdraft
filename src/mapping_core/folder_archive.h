#ifndef FOLDERARCHIVE_H
#define FOLDERARCHIVE_H
#include "basics.h"
#include "file_browser.h"
#include "archive_file.h"
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

typedef void* HANDLE;

class FolderArchive;
using FolderArchivePtr = std::shared_ptr<FolderArchive>;

class FolderArchive : public ArchiveFile
{
public:
    // Constructs a folder archive, if deleteOnClose is specified the folder is deleted when close() is called
    FolderArchive(bool deleteOnClose = false);

    // Calls close()
    virtual ~FolderArchive() override;

    // Gets the path of the folder to which this FolderArchive refers, this may be empty if no folder is set
    virtual const std::string & getFilePath() const override;

    // Returns true if this FolderArchive refers to a valid folder path
    virtual bool isOpen() const override;

    // Returns true if this FolderArchive refers to a valid folder path equivalent to filePath
    virtual bool isOpen(const std::string & filePath) const override;

    // Checks whether a valid folder exists at the given path
    virtual bool isValid(const std::string & filePath) const override;

    // Creates a folder at the given filePath and makes it the folder to which this FolderArchive refers
    virtual bool create(const std::string & filePath);

    // Checks whether a folder exists at the given path, and makes it the folder path for this FolderArchive if so
    // If the given folder path is the folder to which this FolderArchive already refers, no operation occurs and the method returns true
    // If the given folder path is a different valid folder from the one currently referred to by this FolderArchive, the current is closed and the new set
    virtual bool open(const std::string & filePath, bool readOnly = false, bool createIfNotFound = false) override;

    // Does nothing except return true
    virtual inline bool save() override { return true; }

    // Clears the folder path to which this archive refers
    // If no folder path has been set calling this method has no effect
    // If the temporary flag was specified the folder is deleted
    virtual void close() override;

    // Checks whether a file exists within the FolderArchive at the given folderPath
    // Cannot be used unless the FolderArchive already refers to a valid folder
    virtual bool findFile(const std::string & subFolderPath) const override;

    // Checks whether a file exists within the folder given by filePath, at the given subFolderPath
    virtual bool findFile(const std::string & filePath, const std::string & subFolderPath) const override;

    // Attempts to get the file size from this FolderArchive at subFolderPath
    // Cannot be used unless the FolderArchive already refers to a valid folder
    virtual size_t getFileSize(const std::string & subFolderPath) const override;

    // Attempts to get a file from this FolderArchive at subFolderPath and place the data within the fileData buffer
    // Cannot be used unless the FolderArchive already refers to a valid folder
    virtual bool getFile(const std::string & subFolderPath, ByteBuffer & fileData, bool = false) const override;

    // Attempts to get a file from this FolderArchive at subFolderPath and return the file data
    // Cannot be used unless the FolderArchive already refers to a valid folder
    virtual std::optional<std::vector<u8>> getFile(const std::string & subFolderPath, bool = false) const override;

    // Attempts to copy a file from this FolderArchive at subFolderPath to a new file at systemFilePath
    // Cannot be used unless the FolderArchive already refers to a valid folder
    virtual bool extractFile(const std::string & subFolderPath, const std::string & systemFilePath, bool = false) const override;

    // Attempts to add a file to this folder at subFolderPath with the data from the fileData buffer
    // Cannot be used unless the FolderArchive already refers to a valid folder
    virtual bool addFile(const std::string & subFolderPath, std::stringstream & fileData) override;

    // Attempts to add a file to this folder at subFolderPath with the data from the fileData buffer
    // Cannot be used unless the FolderArchive already refers to a valid folder
    virtual bool addFile(const std::string & subFolderPath, const std::vector<u8> & fileData) override;

    // Attempts to add the file at filePath to the folder at subFolderPath
    // Cannot be used unless the FolderArchive already refers to a valid folder
    virtual bool addFile(const std::string & subFolderPath, const std::string & filePath) override;

    // Attempts to move the file in this folder at subFolderPath to a sub-folder in this folder matching newSubFolderPath
    // Cannot be used unless the FolderArchive already refers to a valid folder
    virtual bool renameFile(const std::string & subFolderPath, const std::string & newSubFolderPath) override;

    // Attempts to delete the file in this folder at subFolderPath
    // Cannot be used unless the FolderArchive already refers to a valid folder
    virtual bool removeFile(const std::string & subFolderPath) override;

private:
    std::string filePath {};
};

#endif