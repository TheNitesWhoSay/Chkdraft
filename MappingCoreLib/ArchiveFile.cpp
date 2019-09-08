#include "ArchiveFile.h"

ArchiveFile::ArchiveFile(bool deleteOnClose) : deleteOnClose(deleteOnClose)
{

}

ArchiveFile::~ArchiveFile()
{
    close();
}

bool ArchiveFile::deletingOnClose() const
{
    return deleteOnClose;
}

void ArchiveFile::close()
{
    // Override and ensure everything is closed and cleaned up
}

void ArchiveFile::setDeleteOnClose(bool deleteOnClose)
{
    this->deleteOnClose = deleteOnClose;
}
