#ifndef FILEBROWSER_H
#define FILEBROWSER_H
#include "Basics.h"
#include "SystemIO.h"
#include <memory>
#include <string>
#include <map>

template <typename FilterId = u32> class FilterEntry;
template <typename FilterId = u32> class FileBrowser;
template <typename FilterId = u32> using FileBrowserPtr = std::shared_ptr<FileBrowser<FilterId>>;

template <typename FilterId>
class FileBrowser
{
public:
    FileBrowser();
    FileBrowser(const std::vector<FilterEntry<FilterId>> &filters, const std::string &initialDirectory);
    FileBrowser(const std::vector<FilterEntry<FilterId>> &filters, const std::string &title, bool pathMustExist, bool provideOverwritePrompt);

    virtual bool browseForOpenPath(inout_param std::string &filePath, inout_param FilterId &filterIndex) const;
    virtual bool browseForSavePath(inout_param std::string &filePath, inout_param FilterId &filterIndex) const;
    virtual bool promptTryBrowse(const std::string &tryBrowseMessage) const;
    virtual bool promptOpenRetry(const std::string &openRetryMessage) const;
    virtual bool promptSaveRetry(const std::string &saveRetryMessage) const;
    virtual bool confirmOverwrite(const std::string &confirmOverwriteMessage) const;

    std::vector<FilterEntry<FilterId>> getFilters() const;
    std::string getInitialDirectory() const;
    std::string getTitle() const;
    bool getPathMustExist() const;
    bool getProvideOverwritePrompt() const;

    void setFilters(const std::vector<FilterEntry<FilterId>> &filters);
    void setInitialDirectory(const std::string &initialDirectory);
    void setTitle(const std::string &title);
    void setPathMustExist(bool pathMustExist);
    void setProvideOverwritePrompt(bool provideOverwritePrompt);

protected:
    virtual std::vector<std::pair<std::string, std::string>> getFiltersAndLabels() const;

private:
    std::vector<FilterEntry<FilterId>> filters;
    std::string initialDirectory;
    std::string title;
    bool pathMustExist;
    bool provideOverwritePrompt;
};

template <typename FilterId>
class FilterEntry
{
public:
    // This constructor will set the filterId to zero and is only defined when the FilterId type is u32
    FilterEntry(const std::string &filterString, const std::string &filterLabel);
    // This constructor will set the filterId to zero and is only defined when the FilterId type is u32
    FilterEntry(const std::string &filterString, const std::string &filterLabel, const std::string &defaultExtension);

    FilterEntry(const FilterId &filterId, const std::string &filterString, const std::string &filterLabel, const std::string &defaultExtension = "")
        : filterId(filterId), filterString(filterString), filterLabel(filterLabel), defaultExtension(defaultExtension) {}

    const FilterId filterId;
    const std::string filterString;
    const std::string filterLabel;
    const std::string defaultExtension;
};

#define INCLUDE_TEMPLATES_ONLY
#include "FileBrowser.cpp"
#undef INCLUDE_TEMPLATES_ONLY

#endif