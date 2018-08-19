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
        FileBrowser(const std::map<std::string, std::string> &filterToLabels, const std::string &initialDirectory);
        FileBrowser(const std::map<std::string, std::string> &filterToLabels, const std::string &title, bool pathMustExist, bool provideOverwritePrompt);

        virtual bool browseForFilePath(inout_param std::string &filePath, inout_param u32 &filterIndex) const;
        virtual bool promptTryBrowse(const std::string &tryBrowseMessage) const;
        virtual bool promptOpenRetry(const std::string &openRetryMessage) const;
        virtual bool promptSaveRetry(const std::string &saveRetryMessage) const;

        std::map<std::string, std::string> getFilterToLabels() const;
        std::string getInitialDirectory() const;
        std::string getTitle() const;
        bool getPathMustExist() const;
        bool getProvideOverwritePrompt() const;

        void setFilterToLabels(const std::map<std::string, std::string> &filterToLabels);
        void setInitialDirectory(const std::string &initialDirectory);
        void setTitle(const std::string &title);
        void setPathMustExist(bool pathMustExist);
        void setProvideOverwritePrompt(bool provideOverwritePrompt);

    private:
        std::map<std::string, std::string> filterToLabels;
        std::string initialDirectory;
        std::string title;
        bool pathMustExist;
        bool provideOverwritePrompt;
};

#endif