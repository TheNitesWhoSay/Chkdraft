#include "ArchiveCluster.h"

ArchiveCluster::ArchiveCluster(std::vector<ArchiveFilePtr> orderedSourceFiles) : ArchiveFile(false), orderedSourceFiles(std::move(orderedSourceFiles)) {}

ArchiveCluster::~ArchiveCluster()
{
    close();
}

bool ArchiveCluster::isOpen() const
{
    for ( auto & archive : this->orderedSourceFiles )
    {
        if ( archive->isOpen() )
            return true;
    }
    return false;
}

void ArchiveCluster::close()
{
    for ( auto & archive : this->orderedSourceFiles )
        archive->close();
}

bool ArchiveCluster::findFile(const std::string & filePath) const
{
    for ( auto & archive : this->orderedSourceFiles )
    {
        if ( archive->findFile(filePath) )
            return true;
    }
    return false;
}

size_t ArchiveCluster::getFileSize(const std::string & filePath) const
{
    for ( auto & archive : this->orderedSourceFiles )
    {
        if ( archive->findFile(filePath) )
            return archive->getFileSize(filePath);
    }
    return 0;
}

bool ArchiveCluster::getFile(const std::string & filePath, ByteBuffer & fileData) const
{
    for ( auto & archive : this->orderedSourceFiles )
    {
        if ( archive->findFile(filePath) )
            return archive->getFile(filePath, fileData);
    }
    return false;
}

std::optional<std::vector<u8>> ArchiveCluster::getFile(const std::string & filePath) const
{
    for ( auto & archive : this->orderedSourceFiles )
    {
        if ( archive->findFile(filePath) )
            return archive->getFile(filePath);
    }
    return std::nullopt;
}

bool ArchiveCluster::extractFile(const std::string & filePath, const std::string & systemFilePath) const
{
    for ( auto & archive : this->orderedSourceFiles )
    {
        if ( archive->findFile(filePath) )
            return archive->extractFile(filePath, systemFilePath);
    }
    return false;
}
