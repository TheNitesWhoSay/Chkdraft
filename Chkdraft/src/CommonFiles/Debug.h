#ifndef DEBUG_H
#define DEBUG_H
#include "Constants.h"

// If defined, a standard output window will be shown
#define SHOW_CLI

/** CHKD_DEBUG is usually only defined if visual studios is set
    to compile in debug mode, however it can ocassionally be
    useful to define in release mode to asses/debug parts of the
    program without the performance overhead of full debug mode */
//#define CHKD_DEBUG

#ifdef CHKD_DEBUG
#ifndef SHOW_CLI
#define SHOW_CLI
#endif
#define DEBUG_NEW new (_NORMAL_BLOCK, __FILE__, __LINE__)

void ShoutError(const char* file, unsigned int line, const char* msg, ...);
void CheckInvariant(bool condition, const char* file, int line);
#define Invariant(condition) ( CheckInvariant(condition, __FILE__, __LINE__) ) // States something that must be true at this point in execution
#endif

#ifdef SHOW_CLI
#include <iostream>
#include <Windows.h>
class CLI
{
public:
    CLI()
    {
        AllocConsole();
        freopen_s(&console, "CONOUT$", "w", stdout);
        freopen_s(&console, "CONIN$", "r", stdin);
        std::cout.clear();
        std::cin.clear();
    }

    ~CLI()
    {
        fclose(console);
        FreeConsole();
    }

private:
    FILE* console;
};
#endif

extern char LastError[];
extern char LastErrorLoc[];

extern bool Break; // break point
extern const u32 MAX_ERROR_LENGTH;

void Error(const char* ErrorMessage);
void Coord(s32 x, s32 y, const char* title);
void Debug();
void DebugIf(bool condition);
void NoDebug();
void mb(const char* text);
void mb(const char* text, const char* title);
void mb(int i, const char* text);
void mb(int i, const char character);
void db(const char *text);
void db(int i, const char* text);
void Message(std::string text);
void Message(std::string text, std::string caption);
void MessageInt(int integer, const char* caption);
void MessageChar(char character, int pos);
void FindLeaks();
bool RetryError(const char* text);

#endif