#include "FileBrowser.h"
#include "SystemIO.h"

FileBrowser::FileBrowser() : filtersAndLabels(), initialDirectory(""), title(""), pathMustExist(false), provideOverwritePrompt(false)
{

}

FileBrowser::FileBrowser(const std::vector<std::pair<std::string, std::string>> &filtersAndLabels, const std::string &initialDirectory)
    : filtersAndLabels(filtersAndLabels), initialDirectory(initialDirectory), title(""), pathMustExist(false), provideOverwritePrompt(false)
{

}

FileBrowser::FileBrowser(const std::vector<std::pair<std::string, std::string>> &filtersAndLabels, const std::string &title, bool pathMustExist, bool provideOverwritePrompt)
    : filtersAndLabels(filtersAndLabels), initialDirectory(""), title(title), pathMustExist(pathMustExist), provideOverwritePrompt(provideOverwritePrompt)
{

}

bool FileBrowser::browseForOpenPath(inout_param std::string &filePath, inout_param u32 &filterIndex) const
{
    return BrowseForFile(filePath, filterIndex, getFiltersAndLabels(), getInitialDirectory(), getTitle(), getPathMustExist(), getProvideOverwritePrompt());
}

bool FileBrowser::browseForSavePath(inout_param std::string &filePath, inout_param u32 &filterIndex) const
{
    return BrowseForSave(filePath, filterIndex, getFiltersAndLabels(), getInitialDirectory(), getTitle(), getPathMustExist(), getProvideOverwritePrompt());
}

bool FileBrowser::promptTryBrowse(const std::string &tryBrowseMessage) const
{
    return GetYesNo(tryBrowseMessage, "Browse") == PromptResult::Yes;
}

bool FileBrowser::promptOpenRetry(const std::string &openRetryMessage) const
{
    return GetYesNo(openRetryMessage, "Open") == PromptResult::Yes;
}

bool FileBrowser::promptSaveRetry(const std::string &saveRetryMessage) const
{
    return GetYesNo(saveRetryMessage, "Save") == PromptResult::Yes;
}

std::vector<std::pair<std::string, std::string>> FileBrowser::getFiltersAndLabels() const
{
    return filtersAndLabels;
}

std::string FileBrowser::getInitialDirectory() const
{
    return initialDirectory;
}

std::string FileBrowser::getTitle() const
{
    return title;
}

bool FileBrowser::getPathMustExist() const
{
    return pathMustExist;
}

bool FileBrowser::getProvideOverwritePrompt() const
{
    return provideOverwritePrompt;
}

void FileBrowser::setFiltersAndLabels(const std::vector<std::pair<std::string, std::string>> &filtersAndLabels)
{
    this->filtersAndLabels = filtersAndLabels;
}

void FileBrowser::setInitialDirectory(const std::string &initialDirectory)
{
    this->initialDirectory = initialDirectory;
}

void FileBrowser::setTitle(const std::string &title)
{
    this->title = title;
}

void FileBrowser::setPathMustExist(bool pathMustExist)
{
    this->pathMustExist = pathMustExist;
}

void FileBrowser::setProvideOverwritePrompt(bool provideOverwritePrompt)
{
    this->provideOverwritePrompt = provideOverwritePrompt;
}
