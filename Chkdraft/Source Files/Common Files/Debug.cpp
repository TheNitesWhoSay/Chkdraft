#include "Debug.h"
#include "CommonFiles.h"

bool debugging = false;

const u32 MAX_ERROR_LENGTH = 512;
char LastError[MAX_ERROR_LENGTH]; // Purposed for user-based errors
char LastErrorLoc[MAX_ERROR_LENGTH]; // Purposed for debugging internal errors

void PrintError(const char* file, unsigned int line, const char* msg, ...)
{
	va_list args;
	va_start(args, msg);
	vsprintf_s(LastError, MAX_ERROR_LENGTH, msg, args);
	sprintf_s(LastErrorLoc, MAX_ERROR_LENGTH, "File: %s\nLine: %u", file, line);
	va_end(args);
}

void ShoutError(const char* file, unsigned int line, const char* msg, ...)
{
	char AnError[MAX_ERROR_LENGTH];
	char AnErrorLoc[MAX_ERROR_LENGTH];
	va_list args;
	va_start(args, msg);
	vsprintf_s(AnError, MAX_ERROR_LENGTH, msg, args);
	sprintf_s(AnErrorLoc, MAX_ERROR_LENGTH, "File: %s\nLine: %u", file, line);
	va_end(args);
}

#ifdef CHKD_DEBUG
	void CheckInvariant(bool condition, const char* file, int line)
	{
		char invarError[MAX_ERROR_LENGTH];
		sprintf_s(invarError, MAX_ERROR_LENGTH, "Invariant Check Failed!\n\nFile: %s\nLine: %i", strrchr(file, '\\')+1, line);

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
	sprintf_s(message, 256, "(x, y) --> (%d, %d)", x, y);
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
	char msg[2], cInt[32];
	msg[0] = character;
	msg[1] = '\0';
	_itoa_s(i, cInt, 10);

	MessageBox(NULL, msg, cInt, MB_OK);
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

void MessageInt(int integer, const char* caption)
{
	char cInt[32];
	_itoa_s(integer, cInt, 10);
	MessageBox(NULL, cInt, caption, MB_OK);
}

void MessageChar(char character, int pos)
{
	char posStr[32];
	char chrStr[32];

	_itoa_s(character, chrStr, 32, 10);
	_itoa_s(pos, posStr, 32, 10);

	if ( (unsigned char)character < 32 )
		MessageBox(NULL, chrStr, posStr, MB_OK);
	else
	{
		char str[2] = { character, '\0' };
		MessageBox(NULL, str, posStr, MB_OK);
	}
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
