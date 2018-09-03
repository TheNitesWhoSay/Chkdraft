#ifndef FILEBROWSER_H
#define FILEBROWSER_H
#include "Basics.h"
#include <memory>
#include <string>
#include <map>

class FileBrowser;
typedef std::shared_ptr<FileBrowser> FileBrowserPtr;

class FileBrowser
{
    public:
        FileBrowser();
        FileBrowser(const std::vector<std::pair<std::string, std::string>> &filtersAndLabels, const std::string &initialDirectory);
        FileBrowser(const std::vector<std::pair<std::string, std::string>> &filtersAndLabels, const std::string &title, bool pathMustExist, bool provideOverwritePrompt);

        virtual bool browseForOpenPath(inout_param std::string &filePath, inout_param u32 &filterIndex) const;
        virtual bool browseForSavePath(inout_param std::string &filePath, inout_param u32 &filterIndex) const;
        virtual bool promptTryBrowse(const std::string &tryBrowseMessage) const;
        virtual bool promptOpenRetry(const std::string &openRetryMessage) const;
        virtual bool promptSaveRetry(const std::string &saveRetryMessage) const;

        std::vector<std::pair<std::string, std::string>> getFiltersAndLabels() const;
        std::string getInitialDirectory() const;
        std::string getTitle() const;
        bool getPathMustExist() const;
        bool getProvideOverwritePrompt() const;

        void setFiltersAndLabels(const std::vector<std::pair<std::string, std::string>> &filtersAndLabels);
        void setInitialDirectory(const std::string &initialDirectory);
        void setTitle(const std::string &title);
        void setPathMustExist(bool pathMustExist);
        void setProvideOverwritePrompt(bool provideOverwritePrompt);

    private:
        std::vector<std::pair<std::string, std::string>> filtersAndLabels;
        std::string initialDirectory;
        std::string title;
        bool pathMustExist;
        bool provideOverwritePrompt;
};

#endif