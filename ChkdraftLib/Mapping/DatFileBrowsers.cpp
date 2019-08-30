#include "DatFileBrowsers.h"

ChkdDatFileBrowser::ChkdDatFileBrowser() : updatedPaths(false)
{

}

std::vector<MpqFilePtr> ChkdDatFileBrowser::openScDatFiles(const std::unordered_map<DatFilePriority, DatFileDescriptor> &datFiles, const std::string &expectedStarCraftDirectory, FileBrowserPtr<> starCraftBrowser)
{
    updatedPaths = false;
    const std::vector<MpqFilePtr> &scDatFiles = DatFileBrowser::openScDatFiles(datFiles, expectedStarCraftDirectory, starCraftBrowser);
    if ( updatedPaths )
        Settings::updateSettingsFile();

    return scDatFiles;
}

bool ChkdDatFileBrowser::findStarCraftDirectory(output_param std::string &starCraftDirectory, bool &declinedBrowse, const std::string &expectedStarCraftDirectory, FileBrowserPtr<> starCraftBrowser)
{
    bool foundStarCraftDirectory = DatFileBrowser::findStarCraftDirectory(starCraftDirectory, declinedBrowse, expectedStarCraftDirectory, starCraftBrowser);
    if ( foundStarCraftDirectory && Settings::starCraftPath != starCraftDirectory )
    {
        Settings::starCraftPath = starCraftDirectory;
        updatedPaths = true;
    }
    return foundStarCraftDirectory;
}

MpqFilePtr ChkdDatFileBrowser::openDatFile(const std::string &datFilePath, const DatFileDescriptor &datFileDescriptor)
{
    MpqFilePtr mpqFile = DatFileBrowser::openDatFile(datFilePath, datFileDescriptor);
    if ( mpqFile != nullptr )
    {
        const std::string &mpqFilePath = mpqFile->getFilePath();
        if ( datFileDescriptor.getDatFilePriority() == DatFilePriority::StarDat && mpqFilePath != Settings::starDatPath )
        {
            Settings::starDatPath = mpqFilePath;
            updatedPaths = true;
        }
        else if ( datFileDescriptor.getDatFilePriority() == DatFilePriority::BrooDat && mpqFilePath != Settings::brooDatPath )
        {
            Settings::brooDatPath = mpqFilePath;
            updatedPaths = true;
        }
        else if ( datFileDescriptor.getDatFilePriority() == DatFilePriority::PatchRt && mpqFilePath != Settings::patchRtPath )
        {
            Settings::patchRtPath = mpqFilePath;
            updatedPaths = true;
        }
    }
    return mpqFile;
}

std::unordered_map<DatFilePriority, DatFileDescriptor> ChkdDatFileBrowser::getDatFileDescriptors()
{
    Settings::readSettingsFile();
    std::unordered_map<DatFilePriority, DatFileDescriptor> datFiles = ScData::getDefaultDatFiles();
    for ( auto datFile = datFiles.begin(); datFile != datFiles.end(); ++datFile )
    {
        if ( datFile->first == DatFilePriority::StarDat && !Settings::starDatPath.empty() )
            datFile->second.setExpectedFilePath(Settings::starDatPath);
        else if ( datFile->first == DatFilePriority::BrooDat && !Settings::brooDatPath.empty() )
            datFile->second.setExpectedFilePath(Settings::brooDatPath);
        else if ( datFile->first == DatFilePriority::PatchRt && !Settings::patchRtPath.empty() )
            datFile->second.setExpectedFilePath(Settings::patchRtPath);
    }
    return datFiles;
}

std::string ChkdDatFileBrowser::getExpectedStarCraftDirectory()
{
    Settings::readSettingsFile();
    if ( !Settings::starCraftPath.empty() )
        return Settings::starCraftPath;
    else
        return ::GetDefaultScPath();
}
