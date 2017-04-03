#include "Debug.h"
#include "CommonFiles.h"
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <string>
#include <crtdbg.h>

bool debugging = false;

const u32 MAX_ERROR_LENGTH = 512;
char LastError[MAX_ERROR_LENGTH]; // Purposed for user-based errors
char LastErrorLoc[MAX_ERROR_LENGTH]; // Purposed for debugging internal errors

void PrintError(const char* file, unsigned int line, const char* msg, ...)
{
    va_list args;
    va_start(args, msg);
    std::vsnprintf(LastError, MAX_ERROR_LENGTH, msg, args);
    std::snprintf(LastErrorLoc, MAX_ERROR_LENGTH, "File: %s\nLine: %u", file, line);
    //std::cout << LastErrorLoc << " | " << LastError << std::endl;
    va_end(args);
}

void ShoutError(const char* file, unsigned int line, const char* msg, ...)
{
    va_list args;
    va_start(args, msg);
    std::vsnprintf(LastError, MAX_ERROR_LENGTH, msg, args);
    std::snprintf(LastErrorLoc, MAX_ERROR_LENGTH, "File: %s\nLine: %u", file, line);
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

void Error(const char* ErrorMessage)
{
    MessageBox(NULL, ErrorMessage, "Error!", MB_OK | MB_ICONEXCLAMATION);
}

void Coord(s32 x, s32 y, const char* title = "Coordinates")
{
    char message[256];
    std::snprintf(message, sizeof(message), "(x, y) --> (%d, %d)", x, y);
    MessageBox(NULL, message, title, MB_OK);
}

void mb(const char* text)
{
    MessageBox(NULL, text, "Message", MB_OK);
}

void mb(const char* text, const char* title)
{
    MessageBox(NULL, text, title, MB_OK);
}

void mb(int i, const char* text)
{
    MessageInt(i, text);
}

void mb(int i, const char character)
{
    char msg[2] = { character, '\0' };
    MessageBox(NULL, msg, std::to_string(i).c_str(), MB_OK);
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

void db(const char* text)
{
    if ( debugging )
        MessageBox(NULL, text, "Message", MB_OK);
}

void db(int i, const char* text)
{
    if ( debugging )
        MessageInt(i, text);
}

void Message(std::string text)
{
    MessageBox(NULL, text.c_str(), "Message", MB_OK);
}

void Message(std::string text, std::string caption)
{
    MessageBox(NULL, text.c_str(), caption.c_str(), MB_OK);
}

void MessageInt(int integer, const char* caption)
{
    MessageBox(NULL, std::to_string(integer).c_str(), caption, MB_OK);
}

void MessageChar(char character, int pos)
{
    std::string posStr(std::to_string(pos));
    std::string chrStr(std::to_string(character));
    MessageBox(NULL, chrStr.c_str(), posStr.c_str(), MB_OK);
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

bool RetryError(const char* text)
{
    return MessageBox(NULL, text, "Error!", MB_YESNO|MB_ICONEXCLAMATION) == IDYES;
}
