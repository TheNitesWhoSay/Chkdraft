#include "CascArchive.h"
#include "../CascLib/src/CascLib.h"
#include <SimpleIcu.h>
#include <fstream>

CascArchive::CascArchive() : ArchiveFile(false), filePath(""), hCasc(NULL)
{

}

CascArchive::~CascArchive()
{
    close();
}

const std::string & CascArchive::getFilePath() const
{
    return filePath;
}

inline bool CascArchive::isOpen() const
{
    return hCasc != NULL;
}


inline bool CascArchive::isOpen(const std::string & filePath) const
{
    return hCasc != NULL && filePath == this->filePath;
}

bool CascArchive::isValid(const std::string & filePath) const
{
    if ( isOpen(filePath) )
        return true;

    HANDLE cascCheck = NULL;
    bool opened = CascOpenStorage(icux::toFilestring(filePath).c_str(), 0, &cascCheck);
    if ( opened )
        CascCloseStorage(cascCheck);

    return opened;
}

bool CascArchive::open(const std::string & filePath, bool readOnly, bool createIfNotFound)
{
    if ( !readOnly || createIfNotFound )
        throw std::logic_error("CascArchives must be readOnly and cannot be created");

    if ( isOpen(filePath) )
        return true;

    close();
    if ( CascOpenStorage(icux::toFilestring(filePath).c_str(), 0, &hCasc) )
    {
        this->filePath = filePath;
        return true;
    }
    return false;
}

void CascArchive::close()
{
    if ( isOpen() )
    {
        CascCloseStorage(hCasc);
        hCasc = NULL;
        filePath = "";
    }
}

bool CascArchive::findFile(const std::string & cascPath) const
{
    if ( isOpen() )
    {
        u32 bytesRead = 0;
        HANDLE openFile = NULL;
        if ( CascOpenFile(hCasc, cascPath.c_str(), 0, CASC_OPEN_BY_NAME, &openFile) )
        {
            CascCloseFile(openFile);
            return true;
        }
    }
    return false;
}

bool CascArchive::findFile(const std::string & filePath, const std::string & cascPath) const
{
    bool success = false;
    if ( isOpen(filePath) )
        success = findFile(cascPath);
    else
    {
        HANDLE tempCasc = NULL;
        HANDLE foundFile = NULL;
        if ( CascOpenStorage(icux::toFilestring(filePath).c_str(), 0, &tempCasc) )
        {
            if ( CascOpenFile(hCasc, filePath.c_str(), 0, CASC_OPEN_BY_NAME, &foundFile) )
            {
                CascCloseFile(foundFile);
                success = true;
            }
            CascCloseStorage(tempCasc);
        }
    }
    return success;
}

std::optional<std::vector<u8>> CascArchive::getFile(const std::string & cascPath) const
{
    if ( isOpen() )
    {
        u32 bytesRead = 0;
        HANDLE openFile = NULL;
        if ( CascOpenFile(hCasc, cascPath.c_str(), 0, CASC_OPEN_BY_NAME, &openFile) )
        {
            size_t fileSize = (size_t)CascGetFileSize(openFile, NULL);
            auto fileData = std::make_optional<std::vector<u8>>(fileSize);
            bool success = CascReadFile(openFile, (void*)&fileData.value()[0], (DWORD)fileSize, (LPDWORD)(&bytesRead));
            CascCloseFile(openFile);
            return success ? fileData : std::nullopt;
        }
    }
    return std::nullopt;
}

bool CascArchive::extractFile(const std::string & cascPath, const std::string & systemFilePath) const
{
    bool success = false;
    if ( auto fileData = getFile(cascPath) )
    {
        std::ofstream extractedFile(systemFilePath);
        extractedFile.write(reinterpret_cast<const char*>(&fileData.value()[0]), std::streamsize(fileData->size()));
        success = extractedFile.good();
        extractedFile.close();
    }
    return success;
}
