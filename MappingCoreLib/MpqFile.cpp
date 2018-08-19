#include "MpqFile.h"
#include "SystemIO.h"
#include <SimpleIcu.h>

MpqFile::MpqFile(bool deleteOnClose) : deleteOnClose(deleteOnClose), madeChanges(false), filePath(""), hMpq(NULL)
{

}

MpqFile::~MpqFile()
{
    close();
}

bool MpqFile::deletingOnClose() const
{
    return deleteOnClose;
}

const std::string &MpqFile::getFilePath() const
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

bool MpqFile::open(const std::string &filePath, bool createIfNotFound)
{
    if ( isOpen(filePath) )
        return true;

    close();
    if ( createIfNotFound && !FindFile(filePath) )
        return create(filePath);
    else if ( SFileOpenArchive(icux::toFilestring(filePath).c_str(), NULL, 0, &hMpq) )
    {
        this->filePath = filePath;
        return true;
    }
    return false;
}

bool MpqFile::open(const std::string &filePath, const FileBrowserPtr fileBrowser)
{
    u32 filterIndex = 0;
    std::string browseFilePath = "";
    if ( !filePath.empty() && FindFile(filePath) )
        return open(filePath);
    else if ( fileBrowser != nullptr && fileBrowser->browseForFilePath(browseFilePath, filterIndex) )
        return open(browseFilePath);
}

bool MpqFile::open(const FileBrowserPtr fileBrowser)
{
    u32 filterIndex = 0;
    std::string browseFilePath = "";
    return fileBrowser != nullptr && fileBrowser->browseForFilePath(browseFilePath, filterIndex) && open(browseFilePath);
}

void MpqFile::close(bool updateListFile)
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

                if ( SFileCompactWithList(hMpq, filestringMpqPaths.get(), numAddedMpqAssets) )
                    addedMpqAssetPaths.clear();
            }
            else
                SFileCompactArchive(hMpq, NULL, false);
        }
        SFileCloseArchive(hMpq);
        hMpq = NULL;

        if ( deleteOnClose )
            remove();

        deleteOnClose = false;
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
                    SFileReadFile(openFile, (LPVOID)fileData.data, fileData.sizeUsed, (LPDWORD)(&bytesRead), NULL);
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

bool MpqFile::addFile(const std::string &mpqPath, const buffer &fileData, WavQuality wavQuality)
{
    bool addedFile = false;
    if ( isOpen() )
    {
        if ( wavQuality == WavQuality::Uncompressed )
            addedFile = SFileAddFileFromBuffer(hMpq, mpqPath.c_str(), (LPBYTE)fileData.getPtr(0), fileData.size(), MPQ_FILE_COMPRESS | MPQ_FILE_REPLACEEXISTING);
        else
            addedFile = SFileAddWaveFromBuffer(hMpq, mpqPath.c_str(), (LPBYTE)fileData.getPtr(0), fileData.size(), MPQ_FILE_COMPRESS | MPQ_FILE_REPLACEEXISTING, (DWORD)wavQuality);

        if ( addedFile )
            addedMpqAssetPaths.push_back(mpqPath);
    }
    return addedFile;
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

bool MpqFile::renameFile(const std::string &mpqPath, const std::string &newMpqPath, WavQuality wavQuality)
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
        remove(sysFilePath.c_str());
#endif
    }
}
