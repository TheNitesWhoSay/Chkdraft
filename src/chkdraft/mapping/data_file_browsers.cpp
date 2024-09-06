#include "data_file_browsers.h"

ChkdDataFileBrowser::ChkdDataFileBrowser() : updatedPaths(false)
{

}


ArchiveClusterPtr ChkdDataFileBrowser::openScDataFiles(bool & includesRemastered,
    const std::vector<Sc::DataFile::Descriptor> & dataFiles, const std::string & expectedStarCraftDirectory, FileBrowserPtr<> starCraftBrowser)
{
    updatedPaths = false;
    auto scDataFiles = Sc::DataFile::Browser::openScDataFiles(
        includesRemastered, dataFiles, expectedStarCraftDirectory, starCraftBrowser);

    if ( updatedPaths )
        Settings::updateSettingsFile();

    return scDataFiles;
}

bool ChkdDataFileBrowser::findStarCraftDirectory(std::string & starCraftDirectory, bool & isRemastered, bool & declinedBrowse,
    const std::string & expectedStarCraftDirectory, FileBrowserPtr<> starCraftBrowser)
{
    bool foundStarCraftDirectory = Sc::DataFile::Browser::findStarCraftDirectory(starCraftDirectory, isRemastered, declinedBrowse,
        expectedStarCraftDirectory, starCraftBrowser);

    if ( foundStarCraftDirectory && Settings::starCraftPath != starCraftDirectory )
    {
        Settings::starCraftPath = starCraftDirectory;
        updatedPaths = true;
    }
    return foundStarCraftDirectory;
}

ArchiveFilePtr ChkdDataFileBrowser::openDataFile(const std::string & dataFilePath, const Sc::DataFile::Descriptor & dataFileDescriptor)
{
    ArchiveFilePtr archiveFile = Sc::DataFile::Browser::openDataFile(dataFilePath, dataFileDescriptor);
    if ( archiveFile != nullptr )
    {
        const std::string & archiveFilePath = archiveFile->getFilePath();
        if ( dataFileDescriptor.getPriority() == Sc::DataFile::Priority::RemasteredCasc && Settings::isRemastered == false )
        {
            Settings::isRemastered = true;
            updatedPaths = true;
        }
        else if ( dataFileDescriptor.getPriority() == Sc::DataFile::Priority::StarDat && archiveFilePath != Settings::starDatPath )
        {
            Settings::starDatPath = archiveFilePath;
            updatedPaths = true;
        }
        else if ( dataFileDescriptor.getPriority() == Sc::DataFile::Priority::BrooDat && archiveFilePath != Settings::brooDatPath )
        {
            Settings::brooDatPath = archiveFilePath;
            updatedPaths = true;
        }
        else if ( dataFileDescriptor.getPriority() == Sc::DataFile::Priority::PatchRt && archiveFilePath != Settings::patchRtPath )
        {
            Settings::patchRtPath = archiveFilePath;
            updatedPaths = true;
        }
    }
    return archiveFile;
}

std::vector<Sc::DataFile::Descriptor> ChkdDataFileBrowser::getDataFileDescriptors()
{
    Settings::readSettingsFile();
    std::vector<Sc::DataFile::Descriptor> dataFiles = Sc::DataFile::getDefaultDataFiles();
    for ( auto dataFile = dataFiles.begin(); dataFile != dataFiles.end(); ++dataFile )
    {
        if ( dataFile->getPriority() == Sc::DataFile::Priority::StarDat && !Settings::starDatPath.empty() )
            dataFile->setExpectedFilePath(Settings::starDatPath);
        else if ( dataFile->getPriority() == Sc::DataFile::Priority::BrooDat && !Settings::brooDatPath.empty() )
            dataFile->setExpectedFilePath(Settings::brooDatPath);
        else if ( dataFile->getPriority() == Sc::DataFile::Priority::PatchRt && !Settings::patchRtPath.empty() )
            dataFile->setExpectedFilePath(Settings::patchRtPath);
    }
    return dataFiles;
}

std::string ChkdDataFileBrowser::getExpectedStarCraftDirectory()
{
    Settings::readSettingsFile();
    if ( !Settings::starCraftPath.empty() )
        return Settings::starCraftPath;
    else
        return ::getDefaultScPath();
}
