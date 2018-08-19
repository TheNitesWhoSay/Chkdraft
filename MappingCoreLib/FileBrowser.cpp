#include "FileBrowser.h"
#include "SystemIO.h"

FileBrowser::FileBrowser() : filterToLabels(), initialDirectory(""), title(""), pathMustExist(false), provideOverwritePrompt(false)
{

}

FileBrowser::FileBrowser(const std::map<std::string, std::string> &filterToLabels, const std::string &initialDirectory)
    : filterToLabels(filterToLabels), initialDirectory(initialDirectory), title(""), pathMustExist(false), provideOverwritePrompt(false)
{

}

FileBrowser::FileBrowser(const std::map<std::string, std::string> &filterToLabels, const std::string &title, bool pathMustExist, bool provideOverwritePrompt)
    : filterToLabels(filterToLabels), initialDirectory(""), title(title), pathMustExist(pathMustExist), provideOverwritePrompt(provideOverwritePrompt)
{

}

bool FileBrowser::browseForFilePath(inout_param std::string &filePath, inout_param u32 &filterIndex) const
{
    return BrowseForFile(filePath, filterIndex, getFilterToLabels(), getInitialDirectory(), getTitle(), getPathMustExist(), getProvideOverwritePrompt());
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

std::map<std::string, std::string> FileBrowser::getFilterToLabels() const
{
    return filterToLabels;
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

void FileBrowser::setFilterToLabels(const std::map<std::string, std::string> &filterToLabels)
{
    this->filterToLabels = filterToLabels;
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
