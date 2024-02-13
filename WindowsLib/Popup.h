#ifndef POPUP_H
#define POPUP_H
#include <string>

namespace WinLib {

    enum class PromptResult
    {
        Unknown,
        Yes,
        No,
        Cancel
    };

    void Message(const std::string & text);

    void Message(const std::string & text, const std::string & caption);

    PromptResult GetYesNo(const std::string & text, const std::string & caption);

    PromptResult GetYesNoCancel(const std::string & text, const std::string & caption);

}

#endif