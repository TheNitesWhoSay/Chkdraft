#include "MpqFile.h"
#include "SystemIO.h"
#include <SimpleIcu.h>
#include <iterator>
#include <vector>

MpqFile::MpqFile(bool deleteOnClose, bool updateListFile) : ArchiveFile(deleteOnClose), updateListFile(updateListFile), madeChanges(false), filePath(""), hMpq(NULL)
{

}

MpqFile::~MpqFile()
{
    close();
}

const std::string & MpqFile::getFilePath() const
{
    return filePath;
}

inline bool MpqFile::isOpen() const
{
    return hMpq != NULL;
}


inline bool MpqFile::isOpen(const std::string &filePath) const
{
    return hMpq != NULL && filePath == this->filePath;
}

inline bool MpqFile::isUpdatingListFile() const
{
    return updateListFile;
}

bool MpqFile::isValid(const std::string &filePath) const
{
    if ( isOpen(filePath) )
        return true;

    HANDLE mpqCheck = NULL;
    bool opened = SFileOpenArchive(icux::toFilestring(filePath).c_str(), NULL, STREAM_FLAG_READ_ONLY, &mpqCheck);
    if ( opened )
        SFileCloseArchive(mpqCheck);

    return opened;
}

bool MpqFile::create(const std::string &filePath)
{
    if ( isOpen(filePath) )
        return true;

    close();
    if ( SFileCreateArchive(icux::toFilestring(filePath).c_str(), NULL, 1000, &hMpq) )
    {
        this->filePath = filePath;
        return true;
    }
    return false;
}

bool MpqFile::open(const std::string &filePath, bool readOnly, bool createIfNotFound)
{
    if ( isOpen(filePath) )
        return true;

    close();
    if ( createIfNotFound && !FindFile(filePath) )
        return create(filePath);
    else if ( SFileOpenArchive(icux::toFilestring(filePath).c_str(), NULL, (readOnly ? MPQ_OPEN_READ_ONLY : 0), &hMpq) )
    {
        this->filePath = filePath;
        return true;
    }
    return false;
}

void MpqFile::setUpdatingListFile(bool updateListFile)
{
    this->updateListFile = updateListFile;
}

void MpqFile::save()
{
    if ( isOpen() )
    {
        if ( madeChanges )
        {
            size_t numAddedMpqAssets = addedMpqAssetPaths.size();
            if ( updateListFile && numAddedMpqAssets > 0 )
            {
                std::unique_ptr<const char*[]> filestringMpqPaths = std::unique_ptr<const char*[]>(new const char*[numAddedMpqAssets]);
                for ( size_t assetIndex = 0; assetIndex < numAddedMpqAssets; assetIndex ++ )
                    filestringMpqPaths[assetIndex] = addedMpqAssetPaths[assetIndex].c_str();

                if ( SFileCompactWithList(hMpq, filestringMpqPaths.get(), (DWORD)numAddedMpqAssets) )
                    addedMpqAssetPaths.clear();
            }
            else
                SFileCompactArchive(hMpq, NULL, false);
        }
        SFileFlushArchive(hMpq);
        madeChanges = false;
    }
}

void MpqFile::close()
{
    if ( isOpen() )
    {
        if ( madeChanges )
        {
            size_t numAddedMpqAssets = addedMpqAssetPaths.size();
            if ( updateListFile && numAddedMpqAssets > 0 )
            {
                std::unique_ptr<const char*[]> filestringMpqPaths = std::unique_ptr<const char*[]>(new const char*[numAddedMpqAssets]);
                for ( size_t assetIndex = 0; assetIndex < numAddedMpqAssets; assetIndex ++ )
                    filestringMpqPaths[assetIndex] = addedMpqAssetPaths[assetIndex].c_str();

                if ( SFileCompactWithList(hMpq, filestringMpqPaths.get(), (DWORD)numAddedMpqAssets) )
                    addedMpqAssetPaths.clear();
            }
            else
                SFileCompactArchive(hMpq, NULL, false);
        }
        SFileCloseArchive(hMpq);
        hMpq = NULL;

        if ( ArchiveFile::deletingOnClose() )
            remove();

        setDeleteOnClose(false);
        madeChanges = false;
        filePath = "";
    }
}

bool MpqFile::findFile(const std::string &mpqPath) const
{
    if ( isOpen() )
    {
        u32 bytesRead = 0;
        HANDLE openFile = NULL;
        if ( SFileOpenFileEx(hMpq, mpqPath.c_str(), SFILE_OPEN_FROM_MPQ, &openFile) == TRUE )
        {
            SFileCloseFile(openFile);
            return true;
        }
    }
    return false;
}

bool MpqFile::findFile(const std::string &filePath, const std::string &mpqPath) const
{
    bool success = false;
    if ( isOpen(filePath) )
        success = findFile(mpqPath);
    else
    {
        HANDLE tempMpq = NULL;
        HANDLE foundFile = NULL;
        if ( SFileOpenArchive(icux::toFilestring(filePath).c_str(), NULL, STREAM_FLAG_READ_ONLY, &tempMpq) )
        {
            if ( SFileOpenFileEx(tempMpq, mpqPath.c_str(), SFILE_OPEN_FROM_MPQ, &foundFile) == TRUE )
            {
                SFileCloseFile(foundFile);
                success = true;
            }
            SFileCloseArchive(tempMpq);
        }
    }
    return success;
}

/**
    This function is a friend of buffer and MpqFile and can be used to write in data directly
*/
bool getFile(void* source, const std::string &mpqPath, buffer &fileData)
{
    if ( source != nullptr )
    {
        MpqFile* mpqFile = (MpqFile*)source;
        if ( mpqFile->isOpen() )
        {
            u32 bytesRead = 0;
            HANDLE openFile = NULL;
            if ( SFileOpenFileEx(mpqFile->hMpq, mpqPath.c_str(), SFILE_OPEN_FROM_MPQ, &openFile) )
            {
                u32 fileSize = (u32)SFileGetFileSize(openFile, NULL);
                if ( fileData.setSize(fileSize) )
                {
                    fileData.sizeUsed = fileSize;
                    SFileReadFile(openFile, (LPVOID)fileData.data, (DWORD)fileData.sizeUsed, (LPDWORD)(&bytesRead), NULL);
                    SFileCloseFile(openFile);

                    if ( fileData.sizeUsed == bytesRead )
                        return true;
                }
                else
                {
                    SFileCloseFile(openFile);
                    fileData.flush();
                }
            }
        }
    }
    return false;
}

bool MpqFile::getFile(const std::string &mpqPath, buffer &fileData)
{
    return ::getFile(this, mpqPath, fileData);
}

bool MpqFile::getFile(const std::string &mpqPath, std::vector<u8> & fileData)
{
    bool success = false;
    if ( isOpen() )
    {
        u32 bytesRead = 0;
        HANDLE openFile = NULL;
        if ( SFileOpenFileEx(hMpq, mpqPath.c_str(), SFILE_OPEN_FROM_MPQ, &openFile) )
        {
            size_t fileSize = (size_t)SFileGetFileSize(openFile, NULL);
            fileData.assign(fileSize, u8(0));
            success = SFileReadFile(openFile, (LPVOID)&fileData[0], (DWORD)fileSize, (LPDWORD)(&bytesRead), NULL);
            SFileCloseFile(openFile);
        }
    }
    return success;
}

bool MpqFile::extractFile(const std::string & mpqPath, const std::string & systemFilePath)
{
    if ( isOpen() )
    {
        u32 bytesRead = 0;
        HANDLE openFile = NULL;
        if ( SFileOpenFileEx(hMpq, mpqPath.c_str(), SFILE_OPEN_FROM_MPQ, &openFile) )
            return SFileExtractFile(hMpq, mpqPath.c_str(), icux::toFilestring(systemFilePath).c_str(), 0);
    }
    return false;
}

bool MpqFile::addFile(const std::string &mpqPath, std::stringstream &fileData)
{
    fileData.unsetf(std::ios_base::skipws);
    auto start = std::istream_iterator<u8>(fileData);
    std::vector<u8> fileBytes(start, std::istream_iterator<u8>());
    if ( isOpen() && SFileAddFileFromBuffer(hMpq, mpqPath.c_str(), (LPBYTE)&fileBytes[0], (DWORD)fileBytes.size(), MPQ_FILE_COMPRESS | MPQ_FILE_REPLACEEXISTING) )
    {
        addedMpqAssetPaths.push_back(mpqPath);
        return true;
    }
    return false;
}

bool MpqFile::addFile(const std::string &mpqPath, std::stringstream &fileData, WavQuality wavQuality)
{
    fileData.unsetf(std::ios_base::skipws);
    auto start = std::istream_iterator<u8>(fileData);
    std::vector<u8> fileBytes(start, std::istream_iterator<u8>());
    bool addedFile = false;
    if ( isOpen() )
    {
        if ( wavQuality == WavQuality::Uncompressed )
            addedFile = SFileAddFileFromBuffer(hMpq, mpqPath.c_str(), (LPBYTE)&fileBytes[0], (DWORD)fileBytes.size(), MPQ_FILE_COMPRESS | MPQ_FILE_REPLACEEXISTING);
        else
            addedFile = SFileAddWaveFromBuffer(hMpq, mpqPath.c_str(), (LPBYTE)&fileBytes[0], (DWORD)fileBytes.size(), MPQ_FILE_COMPRESS | MPQ_FILE_REPLACEEXISTING, (DWORD)wavQuality);

        if ( addedFile )
            addedMpqAssetPaths.push_back(mpqPath);
    }
    return addedFile;
}

bool MpqFile::addFile(const std::string &mpqPath, const buffer &fileData)
{
    if ( isOpen() && SFileAddFileFromBuffer(hMpq, mpqPath.c_str(), (LPBYTE)fileData.getPtr(0), (DWORD)fileData.size(), MPQ_FILE_COMPRESS | MPQ_FILE_REPLACEEXISTING) )
    {
        addedMpqAssetPaths.push_back(mpqPath);
        return true;
    }
    return false;
}

bool MpqFile::addFile(const std::string &mpqPath, const buffer &fileData, WavQuality wavQuality)
{
    bool addedFile = false;
    if ( isOpen() )
    {
        if ( wavQuality == WavQuality::Uncompressed )
            addedFile = SFileAddFileFromBuffer(hMpq, mpqPath.c_str(), (LPBYTE)fileData.getPtr(0), (DWORD)fileData.size(), MPQ_FILE_COMPRESS | MPQ_FILE_REPLACEEXISTING);
        else
            addedFile = SFileAddWaveFromBuffer(hMpq, mpqPath.c_str(), (LPBYTE)fileData.getPtr(0), (DWORD)fileData.size(), MPQ_FILE_COMPRESS | MPQ_FILE_REPLACEEXISTING, (DWORD)wavQuality);

        if ( addedFile )
            addedMpqAssetPaths.push_back(mpqPath);
    }
    return addedFile;
}

bool MpqFile::addFile(const std::string &mpqPath, const std::string &filePath)
{
    if ( isOpen() && SFileAddFile(hMpq, icux::toFilestring(filePath).c_str(), mpqPath.c_str(), MPQ_FILE_COMPRESS | MPQ_FILE_REPLACEEXISTING) )
    {
        addedMpqAssetPaths.push_back(mpqPath);
        return true;
    }
    return false;
}

bool MpqFile::addFile(const std::string &mpqPath, const std::string &filePath, WavQuality wavQuality)
{
    bool addedFile = false;
    if ( isOpen() )
    {
        if ( wavQuality == WavQuality::Uncompressed )
            addedFile = SFileAddFile(hMpq, icux::toFilestring(filePath).c_str(), mpqPath.c_str(), MPQ_FILE_COMPRESS | MPQ_FILE_REPLACEEXISTING);
        else
            addedFile = SFileAddWave(hMpq, icux::toFilestring(filePath).c_str(), mpqPath.c_str(), MPQ_FILE_COMPRESS | MPQ_FILE_REPLACEEXISTING, (DWORD)wavQuality);

        if ( addedFile )
            addedMpqAssetPaths.push_back(mpqPath);
    }
    return addedFile;
}

bool MpqFile::renameFile(const std::string &mpqPath, const std::string &newMpqPath)
{
    return isOpen() && SFileRenameFile(hMpq, mpqPath.c_str(), newMpqPath.c_str());
}

bool MpqFile::removeFile(const std::string &mpqPath)
{
    bool removed = SFileRemoveFile(hMpq, mpqPath.c_str(), 0);
    if ( removed )
    {
        auto toRemove = std::find(addedMpqAssetPaths.begin(), addedMpqAssetPaths.end(), mpqPath);
        if ( toRemove != addedMpqAssetPaths.end() )
            addedMpqAssetPaths.erase(toRemove);
    }
    return removed;
}

bool MpqFile::remove()
{
    if ( !filePath.empty() )
    {
        icux::filestring sysFilePath = icux::toFilestring(filePath);
        // Return whether the file with the given filePath is not on the system
#ifdef WINDOWS_UTF16
        _wremove(sysFilePath.c_str());
#else
        ::remove(sysFilePath.c_str());
#endif
        return true;
    }
    return false;
}

u64 ModifiedAsset::nextAssetId(0);

ModifiedAsset::ModifiedAsset(const std::string &assetMpqPath, AssetAction actionTaken, WavQuality wavQualitySelected)
    : assetMpqPath(assetMpqPath), wavQualitySelected(WavQuality::Uncompressed), actionTaken(actionTaken)
{
    assetTempMpqPath = std::to_string(nextAssetId);
    nextAssetId ++;
}

ModifiedAsset::~ModifiedAsset()
{

}

std::vector<FilterEntry<u32>> getMpqFilter()
{
    return std::vector<FilterEntry<u32>> { FilterEntry<u32>("*.mpq", "MoPaQ Archive(*.mpq)", ".mpq") };
}
