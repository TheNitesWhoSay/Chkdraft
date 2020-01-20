#include "Popup.h"
#include <SimpleIcu.h>
#include <Windows.h>

namespace WinLib {

    void Message(const std::string & text)
    {
        MessageBox(NULL, icux::toUistring(text).c_str(), icux::toUistring("Message").c_str(), MB_OK);
    }

    void Message(const std::string & text, const std::string & caption)
    {
        MessageBox(NULL, icux::toUistring(text).c_str(), icux::toUistring(caption).c_str(), MB_OK);
    }

    PromptResult GetYesNo(const std::string & text, const std::string & caption)
    {
        int result = MessageBox(NULL, icux::toUistring(text).c_str(), icux::toUistring(caption).c_str(), MB_YESNO);
        switch ( result ) {
            case IDYES: return PromptResult::Yes;
            case IDNO: return PromptResult::No;
            default: return PromptResult::Unknown;
        }
    }

    PromptResult GetYesNoCancel(const std::string & text, const std::string & caption)
    {
        int result = MessageBox(NULL, icux::toUistring(text).c_str(), icux::toUistring(caption).c_str(), MB_YESNOCANCEL);
        switch ( result ) {
            case IDYES: return PromptResult::Yes;
            case IDNO: return PromptResult::No;
            case IDCANCEL: return PromptResult::Cancel;
            default: return PromptResult::Unknown;
        }
    }

}
