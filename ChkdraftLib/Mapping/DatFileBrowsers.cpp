#include "DatFileBrowsers.h"

ChkdDatFileBrowser::ChkdDatFileBrowser() : updatedPaths(false)
{

}

std::vector<MpqFilePtr> ChkdDatFileBrowser::openScDatFiles(const std::unordered_map<DatFilePriority, DatFileDescriptor> &datFiles, const std::string &expectedStarCraftDirectory, FileBrowserPtr starCraftBrowser)
{
    updatedPaths = false;
    const std::vector<MpqFilePtr> &scDatFiles = DatFileBrowser::openScDatFiles(datFiles, expectedStarCraftDirectory, starCraftBrowser);
    if ( updatedPaths )
        Settings::updateSettingsFile();

    return scDatFiles;
}

bool ChkdDatFileBrowser::findStarCraftDirectory(output_param std::string &starCraftDirectory, const std::string &expectedStarCraftDirectory, FileBrowserPtr starCraftBrowser)
{
    bool foundStarCraftDirectory = DatFileBrowser::findStarCraftDirectory(starCraftDirectory, expectedStarCraftDirectory, starCraftBrowser);
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
        if ( datFileDescriptor.datFilePriority == DatFilePriority::StarDat && mpqFilePath != Settings::starDatPath )
        {
            Settings::starDatPath = mpqFilePath;
            updatedPaths = true;
        }
        else if ( datFileDescriptor.datFilePriority == DatFilePriority::BrooDat && mpqFilePath != Settings::brooDatPath )
        {
            Settings::brooDatPath = mpqFilePath;
            updatedPaths = true;
        }
        else if ( datFileDescriptor.datFilePriority == DatFilePriority::PatchRt && mpqFilePath != Settings::patchRtPath )
        {
            Settings::patchRtPath = mpqFilePath;
            updatedPaths = true;
        }
    }
    return mpqFile;
}
