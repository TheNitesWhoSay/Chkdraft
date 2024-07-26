#ifndef DATAFILEBROWSERS_H
#define DATAFILEBROWSERS_H
#include "../../MappingCoreLib/MappingCore.h"
#include "Settings.h"

class ChkdDataFileBrowser : public Sc::DataFile::Browser
{
    public:
        ChkdDataFileBrowser();

        virtual ArchiveClusterPtr openScDataFiles(
            bool & includesRemastered,
            const std::vector<Sc::DataFile::Descriptor> & dataFiles = getDataFileDescriptors(),
            const std::string & expectedStarCraftDirectory = Settings::starCraftPath,
            FileBrowserPtr<> starCraftBrowser = Sc::DataFile::Browser::getDefaultStarCraftBrowser()) override;

        virtual bool findStarCraftDirectory(std::string & starCraftDirectory, bool & isRemastered, bool & declinedBrowse, const std::string & expectedStarCraftDirectory = "", FileBrowserPtr<> starCraftBrowser = nullptr) override;

        virtual ArchiveFilePtr openDataFile(const std::string & dataFilePath, const Sc::DataFile::Descriptor & dataFileDescriptor) override;

        static std::vector<Sc::DataFile::Descriptor> getDataFileDescriptors();

        static std::string getExpectedStarCraftDirectory();

    private:
        bool updatedPaths;
};

#endif