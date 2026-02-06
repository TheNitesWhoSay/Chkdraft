#include "folder_archive.h"
#include <filesystem>
#include <fstream>

FolderArchive::FolderArchive(bool deleteOnClose) : ArchiveFile(deleteOnClose)
{

}

FolderArchive::~FolderArchive()
{
    close();
}

const std::string & FolderArchive::getFilePath() const
{
    return this->filePath;
}

bool FolderArchive::isOpen() const
{
    return !this->filePath.empty() && isDirectory(this->filePath);
}

bool FolderArchive::isOpen(const std::string & filePath) const
{
    return !this->filePath.empty() && this->filePath == filePath && isDirectory(this->filePath);
}

bool FolderArchive::isValid(const std::string & filePath) const
{
    return !filePath.empty() && isDirectory(filePath);
}

bool FolderArchive::create(const std::string & filePath)
{
    if ( this->filePath == filePath )
        return true;
    else if ( filePath.empty() )
        return false;

    if ( isSamePath(this->filePath, filePath) ) // Existing directory effectively the same as the previous directory, but with a new path string
    {
        if ( isDirectory(filePath) )
        {
            this->filePath = filePath;
            return true;
        }
        else
        {
            if ( makeDirectory(filePath) )
            {
                this->filePath = filePath;
                return true;
            }
            else
                return false;
        }
    }
    else if ( isDirectory(filePath) ) // Existing directory different from previous referred directory
    {
        if ( isOpen() ) // Close the current directory
            close();

        this->filePath = filePath;
        return true;
    }
    else if ( makeDirectory(filePath) ) // Non-existing directory
    {
        this->filePath = filePath;
        return true;
    }
    else
        return false;
}

bool FolderArchive::open(const std::string & filePath, bool readOnly, bool createIfNotFound)
{
    if ( isOpen(filePath) )
        return true;

    close();
    bool isDir = ::isDirectory(filePath);
    if ( createIfNotFound && !isDir )
        return create(filePath);
    else if ( isDir )
    {
        this->filePath = filePath;
        return true;
    }
    return false;
}

void FolderArchive::close()
{
    if ( isOpen() )
        removeFile(this->filePath);
}

bool FolderArchive::findFile(const std::string & subFolderPath) const
{
    return isOpen() && ::findFile(makeSystemFilePath(this->filePath, subFolderPath));
}

bool FolderArchive::findFile(const std::string & filePath, const std::string & subFolderPath) const
{
    return ::findFile(makeSystemFilePath(this->filePath, subFolderPath));
}

size_t FolderArchive::getFileSize(const std::string & subFolderPath) const
{
    if ( isOpen() )
    {
        try {
            return std::filesystem::file_size(makeSystemFilePath(this->filePath, subFolderPath));
        } catch ( ... ) {}
    }
    return 0;
}

bool FolderArchive::getFile(const std::string & subFolderPath, ByteBuffer & fileData, bool) const
{
    if ( isOpen() && this->findFile(subFolderPath) )
    {
        size_t fileSize = getFileSize(subFolderPath);
        fileData.expand(fileSize);
        if ( fileSize == 0 )
            return true;
        else
        {
            std::ifstream inFile(std::filesystem::path(asUtf8(makeSystemFilePath(this->filePath, subFolderPath))), std::ios_base::binary|std::ios_base::in);
            inFile.read((char*)&fileData[0], std::streamsize(fileSize));
            return !inFile.fail();
        }
    }
    return false;
}

std::optional<std::vector<u8>> FolderArchive::getFile(const std::string & subFolderPath, bool) const
{
    return isOpen() ? fileToBuffer(makeSystemFilePath(this->filePath, subFolderPath)) : std::optional<std::vector<u8>>(std::nullopt);
}

bool FolderArchive::extractFile(const std::string & subFolderPath, const std::string & systemFilePath, bool) const
{
    return isOpen() && makeFileCopy(makeSystemFilePath(this->filePath, subFolderPath), systemFilePath);
}

bool FolderArchive::addFile(const std::string & subFolderPath, std::stringstream & fileData)
{
    if ( isOpen() )
    {
        fileData.unsetf(std::ios_base::skipws);
        auto start = std::istream_iterator<u8>(fileData);
        std::vector<u8> fileBytes(start, std::istream_iterator<u8>());
        return addFile(subFolderPath, fileData);
    }
    return false;
}

bool FolderArchive::addFile(const std::string & subFolderPath, const std::vector<u8> & fileData)
{
    return isOpen() && bufferToFile(makeSystemFilePath(this->filePath, subFolderPath), fileData);
}

bool FolderArchive::addFile(const std::string & subFolderPath, const std::string & filePath)
{
    return isOpen() && makeFileCopy(filePath, makeSystemFilePath(this->filePath, subFolderPath));
}

bool FolderArchive::renameFile(const std::string & subFolderPath, const std::string & newSubFolderPath)
{
    if ( isOpen() )
    {
        if ( subFolderPath == newSubFolderPath )
            return false;

        std::string oldPath = makeSystemFilePath(this->filePath, subFolderPath);
        std::string newPath = makeSystemFilePath(this->filePath, newSubFolderPath);
        if ( isSamePath(oldPath, newPath) ) // Different strings but same effective path
            return false;
        else if ( !::findFile(oldPath) )
            return false;
        else
            return makeFileCopy(oldPath, newPath) && removeFile(oldPath);
    }
    return false;
}

bool FolderArchive::removeFile(const std::string & subFolderPath)
{
    if ( isOpen() )
    {
        std::string fullPath = makeSystemFilePath(this->filePath, subFolderPath);
        if ( ::findFile(fullPath) )
            return ::removeFile(fullPath);
    }
    return false;
}
