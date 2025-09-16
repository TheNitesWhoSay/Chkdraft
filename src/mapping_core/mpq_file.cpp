#include "mpq_file.h"
#include "cross_cut/simple_icu.h"
#include <StormLib.h>
#include "system_io.h"
#include <iterator>
#include <vector>

static_assert(WavQuality::Low == MPQ_WAVE_QUALITY_LOW, "WavQuality::Low has the wrong value!");
static_assert(WavQuality::Med == MPQ_WAVE_QUALITY_MEDIUM, "WavQuality::Med has the wrong value!");
static_assert(WavQuality::High == MPQ_WAVE_QUALITY_HIGH, "WavQuality::High has the wrong value!");
static_assert(WavQuality::Uncompressed == std::max(std::max(MPQ_WAVE_QUALITY_LOW, MPQ_WAVE_QUALITY_MEDIUM), MPQ_WAVE_QUALITY_HIGH)+1,
    "WavQuality::Uncompressed has the wrong value!");

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

inline bool MpqFile::isOpen(const std::string & filePath) const
{
    return hMpq != NULL && filePath == this->filePath;
}

inline bool MpqFile::isUpdatingListFile() const
{
    return updateListFile;
}

bool MpqFile::isValid(const std::string & filePath) const
{
    if ( isOpen(filePath) )
        return true;

    HANDLE mpqCheck = NULL;
    bool opened = SFileOpenArchive(icux::toFilestring(filePath).c_str(), 0, STREAM_FLAG_READ_ONLY, &mpqCheck);
    if ( opened )
        SFileCloseArchive(mpqCheck);

    return opened;
}

bool MpqFile::create(const std::string & filePath)
{
    if ( isOpen(filePath) )
        return true;

    close();
    if ( SFileCreateArchive(icux::toFilestring(filePath).c_str(), 0, 1000, &hMpq) )
    {
        this->filePath = filePath;
        this->madeChanges = true;
        return true;
    }
    return false;
}

bool MpqFile::open(const std::string & filePath, bool readOnly, bool createIfNotFound)
{
    if ( isOpen(filePath) )
        return true;

    close();
    if ( createIfNotFound && !::findFile(filePath) )
        return create(filePath);
    else if ( SFileOpenArchive(icux::toFilestring(filePath).c_str(), 0, (readOnly ? MPQ_OPEN_READ_ONLY : 0), &hMpq) )
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

bool MpqFile::save()
{
    bool success = false;
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

                if ( SFileAddListFileEntries(hMpq, filestringMpqPaths.get(), (DWORD)numAddedMpqAssets) == ERROR_SUCCESS &&
                    SFileCompactArchive(hMpq, NULL, false) )
                {
                    success = true;
                    addedMpqAssetPaths.clear();
                }
            }
            else
                success = SFileCompactArchive(hMpq, NULL, false);

            success = success && SFileFlushArchive(hMpq);
        }
        madeChanges = false;
    }
    return success;
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

               if ( SFileAddListFileEntries(hMpq, filestringMpqPaths.get(), (DWORD)numAddedMpqAssets) == ERROR_SUCCESS &&
                    SFileCompactArchive(hMpq, NULL, false) )
               {
                    addedMpqAssetPaths.clear();
               }
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

bool MpqFile::findFile(const std::string & mpqPath) const
{
    if ( isOpen() )
    {
        u32 bytesRead = 0;
        HANDLE openFile = NULL;
        if ( SFileOpenFileEx(hMpq, mpqPath.c_str(), SFILE_OPEN_FROM_MPQ, &openFile) )
        {
            SFileCloseFile(openFile);
            return true;
        }
    }
    return false;
}

bool MpqFile::findFile(const std::string & filePath, const std::string & mpqPath) const
{
    bool success = false;
    if ( isOpen(filePath) )
        success = findFile(mpqPath);
    else
    {
        HANDLE tempMpq = NULL;
        HANDLE foundFile = NULL;
        if ( SFileOpenArchive(icux::toFilestring(filePath).c_str(), 0, STREAM_FLAG_READ_ONLY, &tempMpq) )
        {
            if ( SFileOpenFileEx(tempMpq, mpqPath.c_str(), SFILE_OPEN_FROM_MPQ, &foundFile) )
            {
                SFileCloseFile(foundFile);
                success = true;
            }
            SFileCloseArchive(tempMpq);
        }
    }
    return success;
}

std::optional<std::vector<std::string>> MpqFile::getListfile() const
{
    if ( isOpen() )
    {
        auto listFile = getFile("(listfile)");
        if ( listFile )
        {
            auto result = std::make_optional<std::vector<std::string>>();
            auto & listFileBytes = *listFile;
            std::size_t entryStart = 0;
            std::size_t size = listFile->size();
            for ( std::size_t i=0; i<size; ++i )
            {
                char c = char(listFileBytes[i]);
                if ( c == '\r' || c == '\n' )
                {
                    if ( i > entryStart )
                        result->emplace_back((const char*)&listFileBytes[entryStart], i-entryStart);
                    
                    entryStart = i+1;
                }
            }
            return result;
        }
    }
    return std::nullopt;
}

size_t MpqFile::getFileSize(const std::string & mpqPath) const
{
    if ( isOpen() )
    {
        u32 bytesRead = 0;
        HANDLE openFile = NULL;
        if ( SFileOpenFileEx(hMpq, mpqPath.c_str(), SFILE_OPEN_FROM_MPQ, &openFile) )
        {
            size_t fileSize = (size_t)SFileGetFileSize(openFile, NULL);
            SFileCloseFile(openFile);
            return fileSize;
        }
    }
    return 0;
}

bool MpqFile::getFile(const std::string & mpqPath, ByteBuffer & fileData) const
{
    if ( isOpen() )
    {
        u32 bytesRead = 0;
        HANDLE openFile = NULL;
        if ( SFileOpenFileEx(hMpq, mpqPath.c_str(), SFILE_OPEN_FROM_MPQ, &openFile) )
        {
            size_t fileSize = (size_t)SFileGetFileSize(openFile, NULL);
            fileData.expand(fileSize);
            bool success = SFileReadFile(openFile, (void*)fileData.data(), (DWORD)fileSize, (LPDWORD)(&bytesRead), NULL);
            SFileCloseFile(openFile);
            return success;
        }
    }
    return false;
}

std::optional<std::vector<u8>> MpqFile::getFile(const std::string & mpqPath) const
{
    if ( isOpen() )
    {
        u32 bytesRead = 0;
        HANDLE openFile = NULL;
        if ( SFileOpenFileEx(hMpq, mpqPath.c_str(), SFILE_OPEN_FROM_MPQ, &openFile) )
        {
            size_t fileSize = (size_t)SFileGetFileSize(openFile, NULL);
            auto fileData = std::make_optional<std::vector<u8>>(fileSize);
            bool success = SFileReadFile(openFile, (void*)&fileData.value()[0], (DWORD)fileSize, (LPDWORD)(&bytesRead), NULL);
            SFileCloseFile(openFile);
            return success ? fileData : std::nullopt;
        }
    }
    return std::nullopt;
}

bool MpqFile::extractFile(const std::string & mpqPath, const std::string & systemFilePath) const
{
    if ( isOpen() )
    {
        u32 bytesRead = 0;
        HANDLE openFile = NULL;
        if ( SFileOpenFileEx(hMpq, mpqPath.c_str(), SFILE_OPEN_FROM_MPQ, &openFile) )
        {
            bool success = SFileExtractFile(hMpq, mpqPath.c_str(), icux::toFilestring(systemFilePath).c_str(), 0);
            SFileCloseFile(openFile);
            return success;
        }
    }
    return false;
}

bool MpqFile::addListFileEntry(const std::string & listFileEntry)
{
    if ( isOpen() )
    {
        addedMpqAssetPaths.push_back(listFileEntry);
        madeChanges = true;
        return true;
    }
    return false;
}

bool MpqFile::addFile(const std::string & mpqPath, std::stringstream & fileData)
{
    fileData.unsetf(std::ios_base::skipws);
    auto start = std::istream_iterator<u8>(fileData);
    std::vector<u8> fileBytes(start, std::istream_iterator<u8>());
    return addFile(mpqPath, fileBytes, WavQuality::Uncompressed);
}

bool MpqFile::addFile(const std::string & mpqPath, std::stringstream & fileData, WavQuality wavQuality)
{
    fileData.unsetf(std::ios_base::skipws);
    auto start = std::istream_iterator<u8>(fileData);
    std::vector<u8> fileBytes(start, std::istream_iterator<u8>());
    bool addedFile = false;
    return addFile(mpqPath, fileBytes, wavQuality);
}

bool MpqFile::addFile(const std::string & mpqPath, const std::vector<u8> & fileData)
{
    return addFile(mpqPath.c_str(), fileData, WavQuality::Uncompressed);
}

bool MpqFile::addFile(const std::string & mpqPath, const std::vector<u8> & fileData, WavQuality wavQuality)
{
    bool addedFile = false;
    if ( isOpen() )
    {
        HANDLE hFile = NULL;
        if ( SFileCreateFile(hMpq, mpqPath.c_str(), 0, (DWORD)fileData.size(), 0, MPQ_FILE_COMPRESS | MPQ_FILE_REPLACEEXISTING, &hFile) )
        {
            DWORD compression = wavQuality == MPQ_WAVE_QUALITY_LOW || wavQuality == MPQ_WAVE_QUALITY_MEDIUM ?
                (MPQ_COMPRESSION_ADPCM_STEREO | MPQ_COMPRESSION_HUFFMANN) : MPQ_COMPRESSION_PKWARE;
            addedFile = SFileWriteFile(hFile, &fileData[0], (DWORD)fileData.size(), compression);
            SFileFinishFile(hFile);
        }

        if ( addedFile )
        {
            addedMpqAssetPaths.push_back(mpqPath);
            madeChanges = true;
        }
    }
    return addedFile;
}

bool MpqFile::addFile(const std::string & mpqPath, const std::string & filePath)
{
    if ( isOpen() && SFileAddFile(hMpq, icux::toFilestring(filePath).c_str(), mpqPath.c_str(), MPQ_FILE_COMPRESS | MPQ_FILE_REPLACEEXISTING) )
    {
        addedMpqAssetPaths.push_back(mpqPath);
        madeChanges = true;
        return true;
    }
    return false;
}

bool MpqFile::addFile(const std::string & mpqPath, const std::string & filePath, WavQuality wavQuality)
{
    bool addedFile = false;
    if ( isOpen() )
    {
        if ( wavQuality == WavQuality::Uncompressed )
            addedFile = SFileAddFile(hMpq, icux::toFilestring(filePath).c_str(), mpqPath.c_str(), MPQ_FILE_COMPRESS | MPQ_FILE_REPLACEEXISTING);
        else
            addedFile = SFileAddWave(hMpq, icux::toFilestring(filePath).c_str(), mpqPath.c_str(), MPQ_FILE_COMPRESS | MPQ_FILE_REPLACEEXISTING, (DWORD)wavQuality);

        if ( addedFile )
        {
            addedMpqAssetPaths.push_back(mpqPath);
            madeChanges = true;
        }
    }
    return addedFile;
}

bool MpqFile::renameFile(const std::string & mpqPath, const std::string & newMpqPath)
{
    bool renamed = isOpen() && SFileRenameFile(hMpq, mpqPath.c_str(), newMpqPath.c_str());
    madeChanges = renamed || madeChanges;
    return renamed;
}

bool MpqFile::removeFile(const std::string & mpqPath)
{
    bool removed = SFileRemoveFile(hMpq, mpqPath.c_str(), 0);
    if ( removed )
    {
        madeChanges = true;
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

std::vector<FilterEntry<u32>> getMpqFilter()
{
    return std::vector<FilterEntry<u32>> { FilterEntry<u32>("*.mpq", "MoPaQ Archive(*.mpq)", ".mpq") };
}
