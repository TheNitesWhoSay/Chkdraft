#ifndef FILEBROWSER_H
#define FILEBROWSER_H
#include "Basics.h"
#include "SystemIO.h"
#include <memory>
#include <string>

/**
    This file defines a standard, extensible interface for browsing for file paths, be they a location to save files at or open files from
    FileBrowser has methods which bring up generic system UIs for performing these tasks, but the methods can all be overridden to use a custom UI

    Strings here are all UTF8

    FileBrowser should not contain any system specific code, however it calls functions in SystemIO which does make system-specific calls
    However the system-specific code in SystemIO is wrapped in compiler checks to make them possible to cross-platform/safe to use on other systems
    If no code exists for the system in question to browse for save or open file paths, then the methods simply return false
*/

template <typename FilterId = u32> class FilterEntry;
template <typename FilterId = u32> class FileBrowser;
// TODO: This could very likely be a unique_ptr, generally FileBrowsers are constructed as needed, not stored and shared
template <typename FilterId = u32> using FileBrowserPtr = std::shared_ptr<FileBrowser<FilterId>>;

template <typename FilterId>
class FileBrowser
{
public:
    FileBrowser();
    FileBrowser(const std::vector<FilterEntry<FilterId>> & filters, const std::string & initialDirectory);
    FileBrowser(const std::vector<FilterEntry<FilterId>> & filters, const std::string & title, bool pathMustExist, bool provideOverwritePrompt);

    virtual bool browseForOpenPath(inout_param std::string & filePath, inout_param FilterId & filterIndex) const;
    virtual bool browseForSavePath(inout_param std::string & filePath, inout_param FilterId & filterIndex) const;
    virtual bool promptTryBrowse(const std::string & tryBrowseMessage) const;
    virtual bool promptOpenRetry(const std::string & openRetryMessage) const;
    virtual bool promptSaveRetry(const std::string & saveRetryMessage) const;
    virtual bool confirmOverwrite(const std::string & confirmOverwriteMessage) const;

    std::vector<FilterEntry<FilterId>> getFilters() const;
    std::string getInitialDirectory() const;
    std::string getTitle() const;
    bool getPathMustExist() const;
    bool getProvideOverwritePrompt() const;

    void setFilters(const std::vector<FilterEntry<FilterId>> & filters);
    void setInitialDirectory(const std::string & initialDirectory);
    void setTitle(const std::string & title);
    void setPathMustExist(bool pathMustExist);
    void setProvideOverwritePrompt(bool provideOverwritePrompt);

protected:
    virtual std::vector<std::pair<std::string, std::string>> getFiltersAndLabels() const;

public:
    // Classes cannot have virtual template methods, this alternate method together with type erasure form a workaround for client classes
    virtual bool virtualizableBrowseForOpenPath(inout_param std::string & filePath, inout_param u32 & filterIndex) const;
    // Classes cannot have virtual template methods, this alternate method together with type erasure form a workaround for client classes
    virtual bool virtualizableBrowseForSavePath(inout_param std::string & filePath, inout_param u32 & filterIndex) const;

private:
    std::string initialDirectory;
    std::string title;
    bool pathMustExist;
    bool provideOverwritePrompt;
    std::vector<FilterEntry<FilterId>> filters;
};

template <typename FilterId>
class FilterEntry
{
public:
    // This constructor will set the filterId to zero and is only defined when the FilterId type is u32
    FilterEntry(const std::string & filterString, const std::string & filterLabel);
    // This constructor will set the filterId to zero and is only defined when the FilterId type is u32
    FilterEntry(const std::string & filterString, const std::string & filterLabel, const std::string & defaultExtension);

    FilterEntry(const FilterId & filterId, const std::string & filterString, const std::string & filterLabel, const std::string & defaultExtension = "")
        : filterId(filterId), filterString(filterString), filterLabel(filterLabel), defaultExtension(defaultExtension) {}

    const std::string filterString;
    const std::string filterLabel;
    const std::string defaultExtension;
    const FilterId filterId;
};

#define INCLUDE_TEMPLATES_ONLY
#include "FileBrowser.cpp"
#undef INCLUDE_TEMPLATES_ONLY

#endif