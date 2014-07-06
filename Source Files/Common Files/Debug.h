#ifndef DEBUG_H
#define DEBUG_H
#include "Constants.h"

#ifdef CHKD_DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#include <iostream>
#include <Windows.h>
class CLI
{
	public: 
		CLI()
		{ 
			AllocConsole();
			freopen_s(&console, "CONOUT$", "w", stdout);
		}

		~CLI()
		{
			fclose(console);
		}

	private:
		FILE* console;
};
void CheckInvariant(bool condition, const char* file, int line);
#define Invariant(condition) ( CheckInvariant(condition, __FILE__, __LINE__) ) // States something that must be true at this point in execution
#else
#define DEBUG_NEW new
#define Invariant(condition) ; // States something that must be true at this point in execution
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
void MessageInt(int integer, const char* caption);
void MessageChar(char character, int pos);
void FindLeaks();
bool RetryError(const char* text);

#endif