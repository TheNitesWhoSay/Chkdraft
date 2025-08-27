#include "data_file_browsers.h"
#include <chkdraft/chkdraft.h>
#include <filesystem>

std::string ChkdDataFileBrowser::GetExpectedStarCraftDirectory()
{
    return "TODO"; // TODO: fetch from profiles
}

ChkdDataFileBrowser::ChkdDataFileBrowser() : profileUpdated(false)
{

}


ArchiveClusterPtr ChkdDataFileBrowser::openScDataFiles(bool & includesRemastered,
    const std::vector<Sc::DataFile::Descriptor> & dataFiles, const std::string & expectedStarCraftDirectory, FileBrowserPtr<> starCraftBrowser)
{
    profileUpdated = false;
    auto scDataFiles = Sc::DataFile::Browser::openScDataFiles(
        includesRemastered, dataFiles, expectedStarCraftDirectory, starCraftBrowser);

    if ( profileUpdated )
        chkd.profiles.saveCurrProfile();

    return scDataFiles;
}

bool ChkdDataFileBrowser::findStarCraftDirectory(std::string & starCraftDirectory, bool & isRemastered, bool & declinedBrowse,
    const std::string & expectedStarCraftDirectory, FileBrowserPtr<> starCraftBrowser)
{
    bool foundStarCraftDirectory = Sc::DataFile::Browser::findStarCraftDirectory(starCraftDirectory, isRemastered, declinedBrowse,
        expectedStarCraftDirectory, starCraftBrowser);

    if ( foundStarCraftDirectory )
    {
        if ( !isRemastered && chkd.profiles().classic.starCraftPath != starCraftDirectory )
        {
            chkd.profiles().classic.starCraftPath = starCraftDirectory;
            profileUpdated = true;
        }
        else if ( isRemastered && chkd.profiles().remastered.starCraftPath != starCraftDirectory )
        {
            chkd.profiles().remastered.starCraftPath = starCraftDirectory;
            profileUpdated = true;
        }
    }
    return foundStarCraftDirectory;
}

ArchiveFilePtr ChkdDataFileBrowser::openDataFile(const std::string & dataFilePath, const Sc::DataFile::Descriptor & dataFileDescriptor)
{
    auto & profile = chkd.profiles();
    ArchiveFilePtr archiveFile = Sc::DataFile::Browser::openDataFile(dataFilePath, dataFileDescriptor);
    if ( archiveFile != nullptr )
    {
        const std::string & archiveFilePath = archiveFile->getFilePath();
        if ( dataFileDescriptor.getPriority() == Sc::DataFile::Priority::RemasteredCasc && 
            (profile.useRemastered || profile.remastered.cascPath != dataFilePath) )
        {
            profile.useRemastered = true;
            profile.remastered.cascPath = dataFilePath;
            profile.remastered.dataFiles = {profile.remastered.cascPath};
            profileUpdated = true;
        }
        else if ( dataFileDescriptor.getPriority() == Sc::DataFile::Priority::StarDat && archiveFilePath != profile.classic.starDatPath )
        {
            auto found = std::find(profile.classic.dataFiles.begin(), profile.classic.dataFiles.end(), profile.classic.starDatPath);
            profile.useRemastered = false;
            profile.classic.starDatPath = archiveFilePath;
            if ( found != profile.classic.dataFiles.end() )
                (*found) = profile.classic.starDatPath;
            else
                profile.classic.dataFiles.push_back(profile.classic.starDatPath);

            profileUpdated = true;
        }
        else if ( dataFileDescriptor.getPriority() == Sc::DataFile::Priority::BrooDat && archiveFilePath != profile.classic.brooDatPath )
        {
            auto found = std::find(profile.classic.dataFiles.begin(), profile.classic.dataFiles.end(), profile.classic.brooDatPath);
            profile.useRemastered = false;
            profile.classic.brooDatPath = archiveFilePath;
            if ( found != profile.classic.dataFiles.end() )
                (*found) = profile.classic.brooDatPath;
            else
                profile.classic.dataFiles.push_back(profile.classic.brooDatPath);

            profileUpdated = true;
        }
        else if ( dataFileDescriptor.getPriority() == Sc::DataFile::Priority::PatchRt && archiveFilePath != profile.classic.patchRtPath )
        {
            auto found = std::find(profile.classic.dataFiles.begin(), profile.classic.dataFiles.end(), profile.classic.patchRtPath);
            profile.useRemastered = false;
            profile.classic.patchRtPath = archiveFilePath;
            if ( found != profile.classic.dataFiles.end() )
                (*found) = profile.classic.patchRtPath;
            else
                profile.classic.dataFiles.push_back(profile.classic.patchRtPath);

            profileUpdated = true;
        }
    }
    return archiveFile;
}

std::vector<Sc::DataFile::Descriptor> ChkdDataFileBrowser::getDataFileDescriptors()
{
    auto & profile = chkd.profiles();
    const auto & dataFilePaths = profile.useRemastered ? profile.remastered.dataFiles : profile.classic.dataFiles;
    const std::string & starCraftPath = profile.useRemastered ? profile.remastered.starCraftPath : profile.classic.starCraftPath;
    Sc::DataFile::Priority priority = Sc::DataFile::Priority::AutoPriorityStart;
    std::vector<Sc::DataFile::Descriptor> dataFiles {};
    for ( auto & dataFilePath : dataFilePaths )
    {
        bool isDirectory = std::filesystem::is_directory(dataFilePath);
        bool isCasc = isDirectory;
        bool isOptionalIfCascFound = false;
        std::string fileName = isCasc ? std::string("Data") : getSystemFileName(dataFilePath);
        std::shared_ptr<FileBrowser<u32>> fileBrowser = nullptr;
        bool isExpectedInScDirectory = false;
        try {
            isExpectedInScDirectory = std::filesystem::equivalent(std::filesystem::path(dataFilePath),
                std::filesystem::path(starCraftPath)) ||
            std::filesystem::equivalent(std::filesystem::path(getSystemFileDirectory(dataFilePath)),
                std::filesystem::path(starCraftPath));
        } catch ( ... ) {}

        dataFiles.emplace_back(priority, isCasc, isOptionalIfCascFound, fileName, dataFilePath, fileBrowser, isExpectedInScDirectory);
        priority = Sc::DataFile::Priority((u32 &)priority + 1);
    }

    if ( dataFilePaths.empty() )
    {
        dataFiles = Sc::DataFile::getDefaultDataFiles(profile.useRemastered ?
            Sc::DataFile::RemasteredDescriptor::Yes : Sc::DataFile::RemasteredDescriptor::No);

        for ( auto dataFile = dataFiles.begin(); dataFile != dataFiles.end(); ++dataFile )
        {
            if ( dataFile->getPriority() == Sc::DataFile::Priority::StarDat && !profile.classic.starDatPath.empty() )
                dataFile->setExpectedFilePath(profile.classic.starDatPath);
            else if ( dataFile->getPriority() == Sc::DataFile::Priority::BrooDat && !profile.classic.brooDatPath.empty() )
                dataFile->setExpectedFilePath(profile.classic.brooDatPath);
            else if ( dataFile->getPriority() == Sc::DataFile::Priority::PatchRt && !profile.classic.patchRtPath.empty() )
                dataFile->setExpectedFilePath(profile.classic.patchRtPath);
        }
    }

    return dataFiles;
}

std::string ChkdDataFileBrowser::getExpectedStarCraftDirectory()
{
    const auto & profile = chkd.profiles();
    if ( profile.useRemastered && !profile.remastered.starCraftPath.empty() )
        return profile.remastered.starCraftPath;
    else if ( !profile.useRemastered && !profile.classic.starCraftPath.empty() )
        return profile.classic.starCraftPath;
    else
    {
        std::string defaultScPath = ::getDefaultScPath();
        if ( !defaultScPath.empty() )
        {
            if ( findFile(makeSystemFilePath(defaultScPath, Sc::DataFile::starCraftFileName)) &&
                !findFile(makeSystemFilePath(defaultScPath, Sc::DataFile::buildInfoFileName)) ) // folder is classic
            {
                return !profile.useRemastered ? defaultScPath : "";
            }
            else if ( isDirectory(makeSystemFilePath(defaultScPath, "Data")) ) // folder is remastered
                return profile.useRemastered ? defaultScPath : "";
            else // Doesn't appear to be either remastered or classic install
                return "";
        }
        else
            return ""; // Empty default path
    }
}
