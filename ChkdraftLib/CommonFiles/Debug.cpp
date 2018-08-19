#include "Debug.h"
#include "CommonFiles.h"
#include "../../WindowsLib/WindowsUi.h"
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <string>
#include <crtdbg.h>

bool debugging = false;

const u32 MAX_ERROR_LENGTH = 512;
char LastError[MAX_ERROR_LENGTH]; // Purposed for user-based errors
char LastErrorLoc[MAX_ERROR_LENGTH]; // Purposed for debugging internal errors

void PrintError(const std::string &file, unsigned int line, const std::string &msg, ...)
{
//    const icux::codepoint* cMsg = icux::toUistring(msg).c_str();
    va_list args;
    va_start(args, msg.c_str());
    std::vsnprintf(LastError, MAX_ERROR_LENGTH, msg.c_str(), args);
    std::snprintf(LastErrorLoc, MAX_ERROR_LENGTH, "File: %s\nLine: %u", file, line);
    //std::cout << LastErrorLoc << " | " << LastError << std::endl;
    va_end(args);
}

void ShoutError(const std::string &file, unsigned int line, const std::string &msg, ...)
{
//    const icux::codepoint* cMsg = icux::toUistring(msg).c_str();
    va_list args;
    va_start(args, msg.c_str());
    std::vsnprintf(LastError, MAX_ERROR_LENGTH, msg.c_str(), args);
    std::snprintf(LastErrorLoc, MAX_ERROR_LENGTH, "File: %s\nLine: %u", file.c_str(), line);
    //std::cout << LastErrorLoc << " | " << LastError << std::endl;
    //MessageBox(NULL, LastError, LastErrorLoc, MB_OK | MB_ICONEXCLAMATION);
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
