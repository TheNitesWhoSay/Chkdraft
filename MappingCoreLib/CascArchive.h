#ifndef CASCARCHIVE_H
#define CASCARCHIVE_H
#include "Basics.h"
#include "FileBrowser.h"
#include "ArchiveFile.h"
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

typedef void* HANDLE;

class CascArchive;
using CascArchivePtr = std::shared_ptr<CascArchive>;

class CascArchive : public ArchiveFile
{
public:
    // Constructs an unopened Casc archive
    CascArchive();

    // Calls close()
    virtual ~CascArchive() override;

    // Gets the filePath, the filePath will be empty unless an Casc is open
    virtual const std::string & getFilePath() const override;

    // Checks whether this Casc is open
    virtual bool isOpen() const override;

    // Checks whether a Casc with the given filePath is open
    virtual bool isOpen(const std::string & filePath) const override;

    // Checks whether an openable Casc exists at filePath
    virtual bool isValid(const std::string & filePath) const override;

    // Attempts to open a Casc at filePath
    // If this Casc is already open with the given filePath, no operation occurs and the method returns true
    // If this Casc is already open with a filePath not matching the given filePath, it is closed before attempting to open the new Casc
    virtual bool open(const std::string & filePath, bool readOnly = true, bool createIfNotFound = false) override;

    // Closes a Casc
    // If no Casc is open calling this method has no effect
    virtual void close() override;

    // Checks whether a file exists within the Casc at the given cascPath
    // Cannot be used unless the Casc is already open
    virtual bool findFile(const std::string & cascPath) const override;

    // Checks whether a file exists within the Casc at the given cascPath
    // If a Casc is already open with a filePath matching the provided filePath, the already opened Casc is searched
    // Else attempts to find and search a Casc at filePath
    virtual bool findFile(const std::string & filePath, const std::string & cascPath) const override;

    // Attempts to get a file from this Casc at cascPath and place the data within the fileData buffer
    // Cannot be used unless the Casc is already open
    virtual std::optional<std::vector<u8>> getFile(const std::string & cascPath) const override;

    // Attempts to copy a file from this Casc at cascPath to a new file at systemFilePath
    // Cannot be used unless the Casc is already open
    virtual bool extractFile(const std::string & cascPath, const std::string & systemFilePath) const override;

    virtual inline bool create(const std::string & filePath) override { return false; }
    virtual inline void save() override {}
    virtual inline bool addFile(const std::string & archivedFilePath, std::stringstream & fileData) override { return false; }
    virtual bool addFile(const std::string & archivedFilePath, const std::vector<u8> & fileData) override { return false; }
    virtual bool addFile(const std::string & archivedFilePath, const std::string & filePath) override { return false; }
    virtual bool renameFile(const std::string & archivedFilePath, const std::string & newArchivedFilePath) override { return false; }
    virtual bool removeFile(const std::string & archivedFilePath) override { return false; }

private:
    std::string filePath;
    HANDLE hCasc;
};

#endif