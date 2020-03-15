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

void ShoutError(const std::string & file, unsigned int line, const std::string msg, ...);
void CheckInvariant(bool condition, const std::string & file, int line);
#define Invariant(condition) ( CheckInvariant(condition, __FILE__, __LINE__) ) // States something that must be true at this point in execution
#endif

extern char LastError[];
extern char LastErrorLoc[];

extern bool Break; // break point
extern const u32 MAX_ERROR_LENGTH;

void Error(const std::string & errorMessage);
void Coord(s32 x, s32 y, const std::string & title);
void Debug();
void DebugIf(bool condition);
void NoDebug();
void mb(const std::string & text);
void mb(const std::string & text, const std::string & title);
void mb(int i, const std::string & text);
void db(const std::string & text);
void db(int i, const std::string & text);
void MessageInt(int integer, const std::string & caption);
void FindLeaks();

#endif