#include "DataFileBrowsers.h"

ChkdDataFileBrowser::ChkdDataFileBrowser() : updatedPaths(false)
{

}


std::vector<MpqFilePtr> ChkdDataFileBrowser::openScDataFiles(const std::unordered_map<Sc::DataFile::Priority, Sc::DataFile::Descriptor> & dataFiles, const std::string & expectedStarCraftDirectory, FileBrowserPtr<> starCraftBrowser)
{
    updatedPaths = false;
    const std::vector<MpqFilePtr> & scDataFiles = Sc::DataFile::Browser::openScDataFiles(dataFiles, expectedStarCraftDirectory, starCraftBrowser);
    if ( updatedPaths )
        Settings::updateSettingsFile();

    return scDataFiles;
}

bool ChkdDataFileBrowser::findStarCraftDirectory(output_param std::string & starCraftDirectory, bool & declinedBrowse, const std::string & expectedStarCraftDirectory, FileBrowserPtr<> starCraftBrowser)
{
    bool foundStarCraftDirectory = Sc::DataFile::Browser::findStarCraftDirectory(starCraftDirectory, declinedBrowse, expectedStarCraftDirectory, starCraftBrowser);
    if ( foundStarCraftDirectory && Settings::starCraftPath != starCraftDirectory )
    {
        Settings::starCraftPath = starCraftDirectory;
        updatedPaths = true;
    }
    return foundStarCraftDirectory;
}

MpqFilePtr ChkdDataFileBrowser::openDataFile(const std::string & dataFilePath, const Sc::DataFile::Descriptor & dataFileDescriptor)
{
    MpqFilePtr mpqFile = Sc::DataFile::Browser::openDataFile(dataFilePath, dataFileDescriptor);
    if ( mpqFile != nullptr )
    {
        const std::string & mpqFilePath = mpqFile->getFilePath();
        if ( dataFileDescriptor.getPriority() == Sc::DataFile::Priority::StarDat && mpqFilePath != Settings::starDatPath )
        {
            Settings::starDatPath = mpqFilePath;
            updatedPaths = true;
        }
        else if ( dataFileDescriptor.getPriority() == Sc::DataFile::Priority::BrooDat && mpqFilePath != Settings::brooDatPath )
        {
            Settings::brooDatPath = mpqFilePath;
            updatedPaths = true;
        }
        else if ( dataFileDescriptor.getPriority() == Sc::DataFile::Priority::PatchRt && mpqFilePath != Settings::patchRtPath )
        {
            Settings::patchRtPath = mpqFilePath;
            updatedPaths = true;
        }
    }
    return mpqFile;
}

std::unordered_map<Sc::DataFile::Priority, Sc::DataFile::Descriptor> ChkdDataFileBrowser::getDataFileDescriptors()
{
    Settings::readSettingsFile();
    std::unordered_map<Sc::DataFile::Priority, Sc::DataFile::Descriptor> dataFiles = Sc::DataFile::getDefaultDataFiles();
    for ( auto dataFile = dataFiles.begin(); dataFile != dataFiles.end(); ++dataFile )
    {
        if ( dataFile->first == Sc::DataFile::Priority::StarDat && !Settings::starDatPath.empty() )
            dataFile->second.setExpectedFilePath(Settings::starDatPath);
        else if ( dataFile->first == Sc::DataFile::Priority::BrooDat && !Settings::brooDatPath.empty() )
            dataFile->second.setExpectedFilePath(Settings::brooDatPath);
        else if ( dataFile->first == Sc::DataFile::Priority::PatchRt && !Settings::patchRtPath.empty() )
            dataFile->second.setExpectedFilePath(Settings::patchRtPath);
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
