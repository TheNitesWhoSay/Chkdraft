#ifndef FILEBROWSER_H
#define FILEBROWSER_H
#include "SystemIO.h"
#include "Basics.h"
#include <memory>
#include <string>
#include <vector>

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
template <typename FilterId = u32> using FileBrowserPtr = std::shared_ptr<FileBrowser<FilterId>>;

template <typename FilterId>
class FileBrowser
{
public:
    FileBrowser() = default;
    FileBrowser(const std::vector<FilterEntry<FilterId>> & filters, const std::string & initialDirectory)
        : filters(filters), initialDirectory(initialDirectory), title(""), pathMustExist(false), provideOverwritePrompt(false) {}
    FileBrowser(const std::vector<FilterEntry<FilterId>> & filters, const std::string & title, bool pathMustExist, bool provideOverwritePrompt)
        : filters(filters), initialDirectory(""), title(title), pathMustExist(pathMustExist), provideOverwritePrompt(provideOverwritePrompt) {}

    virtual bool browseForOpenPath(std::string & filePath, FilterId & filterIndex) const
    {
        u32 u32FilterIndex = static_cast<u32>(filterIndex);
        if ( virtualizableBrowseForOpenPath(filePath, u32FilterIndex) )
        {
            filterIndex = static_cast<FilterId>(filters[u32FilterIndex].filterId);
            return true;
        }
        return false;
    }
    virtual bool browseForSavePath(std::string & filePath, FilterId & filterIndex) const
    {
        u32 u32FilterIndex = static_cast<u32>(filterIndex);
        if ( virtualizableBrowseForSavePath(filePath, u32FilterIndex) )
        {
            filterIndex = static_cast<FilterId>(filters[u32FilterIndex].filterId);
            return true;
        }
        return false;
    }
    virtual bool promptTryBrowse(const std::string & tryBrowseMessage) const
    {
        return getYesNo(tryBrowseMessage, "Browse") == PromptResult::Yes;
    }
    virtual bool promptOpenRetry(const std::string & openRetryMessage) const
    {
        return getYesNo(openRetryMessage, "Open") == PromptResult::Yes;
    }
    virtual bool promptSaveRetry(const std::string & saveRetryMessage) const
    {
        return getYesNo(saveRetryMessage, "Save") == PromptResult::Yes;
    }
    virtual bool confirmOverwrite(const std::string & confirmOverwriteMessage) const
    {
        return getYesNo(confirmOverwriteMessage, "Confirm Save As") == PromptResult::Yes;
    }

    std::vector<FilterEntry<FilterId>> getFilters() const { return filters; }
    std::string getInitialDirectory() const { return initialDirectory; }
    std::string getTitle() const { return title; }
    bool getPathMustExist() const { return pathMustExist; }
    bool getProvideOverwritePrompt() const { return provideOverwritePrompt; }

    void setFilters(const std::vector<FilterEntry<FilterId>> & filters) { this->filters = filters; }
    void setInitialDirectory(const std::string & initialDirectory) { this->initialDirectory = initialDirectory; }
    void setTitle(const std::string & title) { this->title = title; }
    void setPathMustExist(bool pathMustExist) { this->pathMustExist = pathMustExist; }
    void setProvideOverwritePrompt(bool provideOverwritePrompt) { this->provideOverwritePrompt = provideOverwritePrompt; }

protected:
    virtual std::vector<std::pair<std::string, std::string>> getFiltersAndLabels() const
    {
        std::vector<std::pair<std::string, std::string>> filtersAndLabels;
        for ( auto & filterEntry : filters )
            filtersAndLabels.push_back(std::make_pair<std::string, std::string>(std::string(filterEntry.filterString), std::string(filterEntry.filterLabel)));

        return filtersAndLabels;
    }

public:
    // Classes cannot have virtual template methods, this alternate method together with type erasure form a workaround for client classes
    virtual bool virtualizableBrowseForOpenPath(std::string & filePath, u32 & filterIndex) const {
        return browseForFile(filePath, filterIndex, getFiltersAndLabels(), getInitialDirectory(), getTitle(), getPathMustExist(), getProvideOverwritePrompt());
    }
    // Classes cannot have virtual template methods, this alternate method together with type erasure form a workaround for client classes
    virtual bool virtualizableBrowseForSavePath(std::string & filePath, u32 & filterIndex) const {
        bool checkingForOverwrite = getProvideOverwritePrompt();
        bool rejectedOverwrite = false;
        std::vector<std::pair<std::string, std::string>> filtersAndLabels = getFiltersAndLabels();
        u32 newFilterIndex = filterIndex;
        std::string newFilePath(filePath);
        do
        {
            rejectedOverwrite = false;
            if ( browseForSave(newFilePath, newFilterIndex, filtersAndLabels, getInitialDirectory(), getTitle(), getPathMustExist(), checkingForOverwrite) )
            {
                if ( getSystemFileExtension(newFilePath).empty() && newFilterIndex > 0 && newFilterIndex <= filters.size() ) // No extension specified, but a valid filter was selected
                { // Check there is a default extension for the given filter
                    const std::string & defaultExtension = filters.at(newFilterIndex).defaultExtension;
                    if ( !defaultExtension.empty() )
                    { // There was a default extension, add it to the path
                        newFilePath += defaultExtension;
                        if ( checkingForOverwrite && findFile(newFilePath) && !confirmOverwrite(getSystemFileName(newFilePath) + " already exists.\nDo you want to replace it?") )
                            rejectedOverwrite = true; // Checking for overwrite and the file exists and the user rejects overwriting the existing file
                    }
                }
                if ( !rejectedOverwrite )
                {
                    filePath = newFilePath;
                    filterIndex = newFilterIndex;
                    return true;
                }
            }
        }
        while ( rejectedOverwrite );
        return false;
    }

private:
    std::string initialDirectory {};
    std::string title {};
    bool pathMustExist {false};
    bool provideOverwritePrompt {false};
    std::vector<FilterEntry<FilterId>> filters {};
};

template <typename FilterId>
class FilterEntry
{
public:
    // This constructor will set the filterId to zero and is only defined when the FilterId type is u32
    FilterEntry(const std::string & filterString, const std::string & filterLabel)
        : filterId(0), filterString(filterString), filterLabel(filterLabel), defaultExtension("") {}
    // This constructor will set the filterId to zero and is only defined when the FilterId type is u32
    FilterEntry(const std::string & filterString, const std::string & filterLabel, const std::string & defaultExtension)
        : filterId(0), filterString(filterString), filterLabel(filterLabel), defaultExtension(defaultExtension) {}
    FilterEntry(const FilterId & filterId, const std::string & filterString, const std::string & filterLabel, const std::string & defaultExtension = "")
        : filterId(filterId), filterString(filterString), filterLabel(filterLabel), defaultExtension(defaultExtension) {}

    const std::string filterString;
    const std::string filterLabel;
    const std::string defaultExtension;
    const FilterId filterId;
};

#endif