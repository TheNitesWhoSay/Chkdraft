#ifndef DATAFILEBROWSERS_H
#define DATAFILEBROWSERS_H
#include "../../MappingCoreLib/MappingCore.h"
#include "Settings.h"

class ChkdDataFileBrowser : public Sc::DataFile::Browser
{
    public:
        ChkdDataFileBrowser();

        virtual std::vector<MpqFilePtr> openScDataFiles(
            const std::unordered_map<Sc::DataFile::Priority, Sc::DataFile::Descriptor> & dataFiles = getDataFileDescriptors(),
            const std::string & expectedStarCraftDirectory = Settings::starCraftPath,
            FileBrowserPtr<> starCraftBrowser = Sc::DataFile::Browser::getDefaultStarCraftBrowser());

        virtual bool findStarCraftDirectory(std::string & starCraftDirectory, bool & declinedBrowse, const std::string & expectedStarCraftDirectory = "", FileBrowserPtr<> starCraftBrowser = nullptr);

        virtual MpqFilePtr openDataFile(const std::string & dataFilePath, const Sc::DataFile::Descriptor & dataFileDescriptor);

        static std::unordered_map<Sc::DataFile::Priority, Sc::DataFile::Descriptor> getDataFileDescriptors();

        static std::string getExpectedStarCraftDirectory();

    private:
        bool updatedPaths;
};

#endif