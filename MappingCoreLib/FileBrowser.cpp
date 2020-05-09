#include "FileBrowser.h"

/* Templates */ #ifdef INCLUDE_TEMPLATES_ONLY

template <typename FilterId>
FileBrowser<FilterId>::FileBrowser() : filters(), initialDirectory(""), title(""), pathMustExist(false), provideOverwritePrompt(false)
{

}

template <typename FilterId>
FileBrowser<FilterId>::FileBrowser(const std::vector<FilterEntry<FilterId>> & filters, const std::string & initialDirectory)
    : filters(filters), initialDirectory(initialDirectory), title(""), pathMustExist(false), provideOverwritePrompt(false)
{

}

template <typename FilterId>
FileBrowser<FilterId>::FileBrowser(const std::vector<FilterEntry<FilterId>> & filters, const std::string & title, bool pathMustExist, bool provideOverwritePrompt)
    : filters(filters), initialDirectory(""), title(title), pathMustExist(pathMustExist), provideOverwritePrompt(provideOverwritePrompt)
{

}

template <typename FilterId>
bool FileBrowser<FilterId>::browseForOpenPath(inout_param std::string & filePath, inout_param FilterId & filterIndex) const
{
    u32 u32FilterIndex = static_cast<u32>(filterIndex);
    if ( virtualizableBrowseForOpenPath(filePath, u32FilterIndex) )
    {
        filterIndex = static_cast<FilterId>(filters[u32FilterIndex-1].filterId);
        return true;
    }
    return false;
}

template <typename FilterId>
bool FileBrowser<FilterId>::browseForSavePath(inout_param std::string & filePath, inout_param FilterId & filterIndex) const
{
    u32 u32FilterIndex = static_cast<u32>(filterIndex);
    if ( virtualizableBrowseForSavePath(filePath, u32FilterIndex) )
    {
        filterIndex = static_cast<FilterId>(filters[u32FilterIndex-1].filterId);
        return true;
    }
    return false;
}

template <typename FilterId>
bool FileBrowser<FilterId>::promptTryBrowse(const std::string & tryBrowseMessage) const
{
    return getYesNo(tryBrowseMessage, "Browse") == PromptResult::Yes;
}

template <typename FilterId>
bool FileBrowser<FilterId>::promptOpenRetry(const std::string & openRetryMessage) const
{
    return getYesNo(openRetryMessage, "Open") == PromptResult::Yes;
}

template <typename FilterId>
bool FileBrowser<FilterId>::promptSaveRetry(const std::string & saveRetryMessage) const
{
    return getYesNo(saveRetryMessage, "Save") == PromptResult::Yes;
}

template <typename FilterId>
bool FileBrowser<FilterId>::confirmOverwrite(const std::string & confirmOverwriteMessage) const
{
    return getYesNo(confirmOverwriteMessage, "Confirm Save As") == PromptResult::Yes;
}

template <typename FilterId>
std::vector<FilterEntry<FilterId>> FileBrowser<FilterId>::getFilters() const
{
    return filters;
}

template <typename FilterId>
std::string FileBrowser<FilterId>::getInitialDirectory() const
{
    return initialDirectory;
}

template <typename FilterId>
std::string FileBrowser<FilterId>::getTitle() const
{
    return title;
}

template <typename FilterId>
bool FileBrowser<FilterId>::getPathMustExist() const
{
    return pathMustExist;
}

template <typename FilterId>
bool FileBrowser<FilterId>::getProvideOverwritePrompt() const
{
    return provideOverwritePrompt;
}

template <typename FilterId>
void FileBrowser<FilterId>::setFilters(const std::vector<FilterEntry<FilterId>> & filters)
{
    this->filters = filters;
}

template <typename FilterId>
void FileBrowser<FilterId>::setInitialDirectory(const std::string & initialDirectory)
{
    this->initialDirectory = initialDirectory;
}

template <typename FilterId>
void FileBrowser<FilterId>::setTitle(const std::string & title)
{
    this->title = title;
}

template <typename FilterId>
void FileBrowser<FilterId>::setPathMustExist(bool pathMustExist)
{
    this->pathMustExist = pathMustExist;
}

template <typename FilterId>
void FileBrowser<FilterId>::setProvideOverwritePrompt(bool provideOverwritePrompt)
{
    this->provideOverwritePrompt = provideOverwritePrompt;
}

template <typename FilterId>
std::vector<std::pair<std::string, std::string>> FileBrowser<FilterId>::getFiltersAndLabels() const
{
    std::vector<std::pair<std::string, std::string>> filtersAndLabels;
    for ( auto & filterEntry : filters )
        filtersAndLabels.push_back(std::make_pair<std::string, std::string>(std::string(filterEntry.filterString), std::string(filterEntry.filterLabel)));

    return filtersAndLabels;
}

template <typename FilterId>
bool FileBrowser<FilterId>::virtualizableBrowseForOpenPath(inout_param std::string & filePath, inout_param u32 & filterIndex) const
{
    return browseForFile(filePath, filterIndex, getFiltersAndLabels(), getInitialDirectory(), getTitle(), getPathMustExist(), getProvideOverwritePrompt());
}

template <typename FilterId>
bool FileBrowser<FilterId>::virtualizableBrowseForSavePath(inout_param std::string & filePath, inout_param u32 & filterIndex) const
{
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
                const std::string & defaultExtension = filters.at(newFilterIndex-1).defaultExtension;
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

/* End templates */ #else

template <typename FilterId>
FilterEntry<FilterId>::FilterEntry(const std::string & filterString, const std::string & filterLabel)
    : filterId(0), filterString(filterString), filterLabel(filterLabel), defaultExtension("")
{

}
// Explicit instantiation provided only for the u32 type
template FilterEntry<u32>::FilterEntry(const std::string & filterString, const std::string & filterLabel);

template <typename FilterId>
FilterEntry<FilterId>::FilterEntry(const std::string & filterString, const std::string & filterLabel, const std::string & defaultExtension)
    : filterId(0), filterString(filterString), filterLabel(filterLabel), defaultExtension(defaultExtension)
{

}
// Explicit instantiation provided only for the u32 type
template FilterEntry<u32>::FilterEntry(const std::string & filterString, const std::string & filterLabel, const std::string & defaultExtension);

#endif