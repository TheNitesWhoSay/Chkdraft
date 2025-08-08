#include "debug.h"
#include "common_files.h"
#include <windows/windows_ui.h>
#include "cross_cut/logger.h"
#include "cross_cut/simple_icu.h"
#include <crtdbg.h>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <string>

extern Logger logger;

bool debugging = false;

const u32 MAX_ERROR_LENGTH = 512;
char LastError[MAX_ERROR_LENGTH]; // Purposed for user-based errors
char LastErrorLoc[MAX_ERROR_LENGTH]; // Purposed for debugging internal errors

void PrintError(const std::string & file, unsigned int line, const std::string msg, ...)
{
    va_list args;
#ifdef WINDOWS_UTF16
    const icux::uistring sysMsg = icux::toUistring(msg);
    icux::codepoint sysLastError[MAX_ERROR_LENGTH] = L"";
    icux::codepoint sysLastErrorLoc[MAX_ERROR_LENGTH] = L"";
    va_start(args, msg);
    _vsnwprintf((wchar_t*)sysLastError, MAX_ERROR_LENGTH, sysMsg.c_str(), args);
    _snwprintf((wchar_t*)sysLastErrorLoc, MAX_ERROR_LENGTH, L"File: %s | Line: %u", icux::toUistring(file).c_str(), line);
    sysLastError[sysMsg.length()] = '\0';
    strcpy(LastError, icux::toUtf8(sysLastError).c_str());
    strcpy(LastErrorLoc, icux::toUtf8(sysLastErrorLoc).c_str());
#else
    va_start(args, msg);
    std::vsnprintf(LastError, MAX_ERROR_LENGTH, msg.c_str(), args);
    std::snprintf(LastErrorLoc, MAX_ERROR_LENGTH, "File: %s\nLine: %u", file.c_str(), line);
#endif
    va_end(args);
    logger.error() << LastErrorLoc << " | " << LastError << std::endl;
}

void ShoutError(const std::string & file, unsigned int line, const std::string msg, ...)
{
    va_list args;
#ifdef WINDOWS_UTF16
    const icux::uistring sysMsg = icux::toUistring(msg);
    icux::codepoint sysLastError[MAX_ERROR_LENGTH] = L"";
    icux::codepoint sysLastErrorLoc[MAX_ERROR_LENGTH] = L"";
    va_start(args, msg);
    _vsnwprintf((wchar_t*)sysLastError, MAX_ERROR_LENGTH, sysMsg.c_str(), args);
    _snwprintf((wchar_t*)sysLastErrorLoc, MAX_ERROR_LENGTH, L"File: %s | Line: %u", icux::toUistring(file).c_str(), line);
    sysLastError[sysMsg.length()] = '\0';
    strcpy(LastError, icux::toUtf8(sysLastError).c_str());
    strcpy(LastErrorLoc, icux::toUtf8(sysLastErrorLoc).c_str());
#else
    va_start(args, msg);
    std::vsnprintf(LastError, MAX_ERROR_LENGTH, msg.c_str(), args);
    std::snprintf(LastErrorLoc, MAX_ERROR_LENGTH, "File: %s\nLine: %u", file.c_str(), line);
#endif
    va_end(args);
    logger.error() << LastErrorLoc << " | " << LastError << std::endl;
}

#ifdef CHKD_DEBUG
    void CheckInvariant(bool condition, const std::string & file, int line)
    {
        char invarError[MAX_ERROR_LENGTH];
        std::snprintf(invarError, MAX_ERROR_LENGTH, "Invariant Check Failed!\n\nFile: %s\nLine: %i", file.c_str(), line);

        if ( condition == false )
            WinLib::Message(invarError, "Error!");
    }
#endif

void Error(const std::string & errorMessage)
{
    WinLib::Message(errorMessage, "Error!");
}

void Coord(s32 x, s32 y, const std::string & title = "Coordinates")
{
    std::string message = "(x, y) --> (" + std::to_string(x) + ", " + std::to_string(y) + ")";
    WinLib::Message(message, title);
}

void mb(const std::string & text)
{
    WinLib::Message(text);
}

void mb(const std::string & text, const std::string & title)
{
    WinLib::Message(text, title);
}

void mb(int i, const std::string & text)
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

void db(const std::string & text)
{
    if ( debugging )
        WinLib::Message(text);
}

void db(int i, const std::string & text)
{
    if ( debugging )
        MessageInt(i, text);
}

void MessageInt(int integer, const std::string & caption)
{
    WinLib::Message(std::to_string(integer), caption);
}

void FindLeaks()
{
    #ifdef CHKD_DEBUG
    if ( _CrtDumpMemoryLeaks() )
        WinLib::Message("CrtDumpMemoryLeaks Activated");
    else
        WinLib::Message("No Leaks!");
    #endif
}
