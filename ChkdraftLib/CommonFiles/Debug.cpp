#include "Debug.h"
#include "CommonFiles.h"
#include "../../WindowsLib/WindowsUi.h"
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <string>
#include <crtdbg.h>
#include <SimpleIcu.h>

bool debugging = false;

const u32 MAX_ERROR_LENGTH = 512;
char LastError[MAX_ERROR_LENGTH]; // Purposed for user-based errors
char LastErrorLoc[MAX_ERROR_LENGTH]; // Purposed for debugging internal errors

void PrintError(const std::string &file, unsigned int line, const std::string msg, ...)
{
    //std::cout << LastErrorLoc << " | " << LastError << std::endl;
    va_list args;
#ifdef WINDOWS_UTF16
    const icux::uistring sysMsg = icux::toUistring(msg);
    const icux::codepoint sysLastError[MAX_ERROR_LENGTH] = L"";
    const icux::codepoint sysLastErrorLoc[MAX_ERROR_LENGTH] = L"";
    va_start(args, msg);
    _vsnwprintf((wchar_t*)sysLastError, MAX_ERROR_LENGTH, sysMsg.c_str(), args);
    _snwprintf((wchar_t*)sysLastErrorLoc, MAX_ERROR_LENGTH, L"File: %s\nLine: %u", icux::toUistring(file).c_str(), line);
    strcpy(LastError, icux::toUtf8(sysLastError).c_str());
    strcpy(LastErrorLoc, icux::toUtf8(sysLastErrorLoc).c_str());
#else
    va_start(args, msg);
    std::vsnprintf(LastError, MAX_ERROR_LENGTH, msg.c_str(), args);
    std::snprintf(LastErrorLoc, MAX_ERROR_LENGTH, "File: %s\nLine: %u", file.c_str(), line);
#endif
    va_end(args);
}

void ShoutError(const std::string &file, unsigned int line, const std::string msg, ...)
{
    //std::cout << LastErrorLoc << " | " << LastError << std::endl;
    va_list args;
#ifdef WINDOWS_UTF16
    const icux::uistring sysMsg = icux::toUistring(msg);
    const icux::codepoint sysLastError[MAX_ERROR_LENGTH] = L"";
    const icux::codepoint sysLastErrorLoc[MAX_ERROR_LENGTH] = L"";
    va_start(args, msg);
    _vsnwprintf((wchar_t*)sysLastError, MAX_ERROR_LENGTH, sysMsg.c_str(), args);
    _snwprintf((wchar_t*)sysLastErrorLoc, MAX_ERROR_LENGTH, L"File: %s\nLine: %u", icux::toUistring(file).c_str(), line);
    strcpy(LastError, icux::toUtf8(sysLastError).c_str());
    strcpy(LastErrorLoc, icux::toUtf8(sysLastErrorLoc).c_str());
#else
    va_start(args, msg);
    std::vsnprintf(LastError, MAX_ERROR_LENGTH, msg.c_str(), args);
    std::snprintf(LastErrorLoc, MAX_ERROR_LENGTH, "File: %s\nLine: %u", file.c_str(), line);
#endif
    va_end(args);
}

#ifdef CHKD_DEBUG
    void CheckInvariant(bool condition, const char* file, int line)
    {
        char invarError[MAX_ERROR_LENGTH];
        std::snprintf(invarError, MAX_ERROR_LENGTH, "Invariant Check Failed!\n\nFile: %s\nLine: %i", file, line);

        if ( condition == false )
            MessageBox(NULL, invarError, "Error!", MB_OK|MB_ICONEXCLAMATION);
    }
#endif

void Error(const std::string &errorMessage)
{
    WinLib::Message(errorMessage, "Error!");
}

void Coord(s32 x, s32 y, const std::string &title = "Coordinates")
{
    std::string message = "(x, y) --> (" + std::to_string(x) + ", " + std::to_string(y) + ")";
    WinLib::Message(message, title);
}

void mb(const std::string &text)
{
    WinLib::Message(text);
}

void mb(const std::string &text, const std::string &title)
{
    WinLib::Message(text, title);
}

void mb(int i, const std::string &text)
{
    MessageInt(i, text);
}

void Debug()
{
    debugging = true;
}

void DebugIf(bool condition)
{
    debugging = condition;
}

void NoDebug()
{
    debugging = false;
}

void db(const std::string &text)
{
    if ( debugging )
        WinLib::Message(text);
}

void db(int i, const std::string &text)
{
    if ( debugging )
        MessageInt(i, text);
}

void MessageInt(int integer, const std::string &caption)
{
    WinLib::Message(std::to_string(integer), caption);
}

void FindLeaks()
{
    #ifdef CHKD_DEBUG
    if ( _CrtDumpMemoryLeaks() )
        MessageBox(NULL, "CrtDumpMemoryLeaks Activated", "Message", MB_OK);
    else
        MessageBox(NULL, "No Leaks!", "Message", MB_OK);
    #endif
}
