#ifndef ARCHIVECLUSTER_H
#define ARCHIVECLUSTER_H
#include "ArchiveFile.h"

class ArchiveCluster;
using ArchiveClusterPtr = std::shared_ptr<ArchiveCluster>;

/**
    An ArchiveCluster has the interface of a singular read-only ArchiveFile, but inside contains one or more source ArchiveFiles
    The source ArchiveFiles have an order by which they'll be tried to get a file or file details
*/
class ArchiveCluster : public ArchiveFile
{
public:
    // Constructs an unopened ArchiveCluster
    ArchiveCluster(std::vector<ArchiveFilePtr> orderedSourceFiles);

    // Calls close()
    virtual ~ArchiveCluster() override;

    // Checks whether any contained source ArchiveFiles are open
    virtual bool isOpen() const override;

    // Closes all contained source ArchiveFile
    // This has no effect on source ArchiveFiles that are not already ope
    virtual void close() override;

    // Checks whether a file exists within the cluster at the given filePath
    virtual bool findFile(const std::string & filePath) const override;

    // Attempts to get the file size from the file in the cluster at filePath
    // Cannot be used unless the cluster is already open
    virtual size_t getFileSize(const std::string & filePath) const override;

    // Attempts to get a file from this cluster at filePath and place the data within the fileData buffer
    // Cannot be used unless the cluster is already open
    virtual bool getFile(const std::string & filePath, ByteBuffer & fileData) const override;

    // Attempts to get a file from this cluster at filePath and place the data within the fileData buffer
    // Cannot be used unless the cluster is already open
    virtual std::optional<std::vector<u8>> getFile(const std::string & filePath) const override;

    // Attempts to copy a file from this cluster at filePath to a new file at systemFilePath
    // Cannot be used unless the cluster is already open
    virtual bool extractFile(const std::string & filePath, const std::string & systemFilePath) const override;
    
    virtual const std::string & getFilePath() const override { throw std::logic_error("ArchiveClusters have no singular file path"); }
    virtual bool isOpen(const std::string & filePath) const override { return false; }
    virtual bool isValid(const std::string & filePath) const override { return false; }
    virtual bool open(const std::string & filePath, bool readOnly = true, bool createIfNotFound = false) override { return false; }
    virtual bool findFile(const std::string & filePath, const std::string & clusterFilePath) const override { return false; }
    virtual inline bool create(const std::string & filePath) override { return false; }
    virtual inline void save() override {}
    virtual inline bool addFile(const std::string & archivedFilePath, std::stringstream & fileData) override { return false; }
    virtual bool addFile(const std::string & archivedFilePath, const std::vector<u8> & fileData) override { return false; }
    virtual bool addFile(const std::string & archivedFilePath, const std::string & filePath) override { return false; }
    virtual bool renameFile(const std::string & archivedFilePath, const std::string & newArchivedFilePath) override { return false; }
    virtual bool removeFile(const std::string & archivedFilePath) override { return false; }

private:
    std::vector<ArchiveFilePtr> orderedSourceFiles {};
};

#endif