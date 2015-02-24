#ifndef BASICS_H
#define BASICS_H
#include <string>

#ifdef CHKDRAFT // Globally defined in project properties if this is used in CHKDraft
	void Debug();
	void DebugIf(bool condition);
	void NoDebug();
	void mb(const char* text); // Basic message box message
	void mb(int i, const char* text);
	void Error(const char* ErrorMessage); // Basic error message box
	bool RetryError(const char* text); // Error box with yes/no confirm
	extern void PrintError(const char* file, unsigned int line, const char* msg, ...); // Prints to LastError and LastErrorLoc
	#define CHKD_ERR(msg, ...) PrintError(__FILE__, __LINE__, msg, __VA_ARGS__) // Prints a detailed error
#else
	void IgnoreErr(const char* file, unsigned int line, const char* msg, ...); // Ignores an error message
	#define CHKD_ERR(msg, ...) IgnoreErr(__FILE__, __LINE__, msg, __VA_ARGS__) /* Would print to error messages if CHKDRAFT was defined */
#endif

#ifndef u64
#define u64 unsigned long long
#endif
#ifndef s64
#define s64 signed long long
#endif
#ifndef u32
#define u32 unsigned long
#endif
#ifndef s32
#define s32 signed long
#endif
#ifndef u16
#define u16 unsigned short
#endif
#ifndef s16
#define s16 signed short
#endif
#ifndef u8
#define u8 unsigned char
#endif
#ifndef s8
#define s8 signed char
#endif

#define BIT_0  0x1
#define BIT_1  0x2
#define BIT_2  0x4
#define BIT_3  0x8
#define BIT_4  0x10
#define BIT_5  0x20
#define BIT_6  0x40
#define BIT_7  0x80

#define BIT_8  0x100
#define BIT_9  0x200
#define BIT_10 0x400
#define BIT_11 0x800
#define BIT_12 0x1000
#define BIT_13 0x2000
#define BIT_14 0x4000
#define BIT_15 0x8000

#define BIT_16 0x10000
#define BIT_17 0x20000
#define BIT_18 0x40000
#define BIT_19 0x80000
#define BIT_20 0x100000
#define BIT_21 0x200000
#define BIT_22 0x400000
#define BIT_23 0x800000

#define BIT_24 0x1000000
#define BIT_25 0x2000000
#define BIT_26 0x4000000
#define BIT_27 0x8000000
#define BIT_28 0x10000000
#define BIT_29 0x20000000
#define BIT_30 0x40000000
#define BIT_31 0x80000000

const u32 u32Bits[] = { BIT_0, BIT_1, BIT_2, BIT_3, BIT_4, BIT_5, BIT_6, BIT_7,
						BIT_8, BIT_9, BIT_10, BIT_11, BIT_12, BIT_13, BIT_14, BIT_15,
						BIT_16, BIT_17, BIT_18, BIT_19, BIT_20, BIT_21, BIT_22, BIT_23,
						BIT_24, BIT_25, BIT_26, BIT_27, BIT_28, BIT_29, BIT_30, BIT_31 };

const u16 u16Bits[] = { BIT_0, BIT_1, BIT_2, BIT_3, BIT_4, BIT_5, BIT_6, BIT_7,
						BIT_8, BIT_9, BIT_10, BIT_11, BIT_12, BIT_13, BIT_14, BIT_15 };

extern const char* DefaultUnitDisplayName[233];

extern const char* LegacyTextTrigDisplayName[233];

#define NUM_UNIT_NAMES 233

#define NUM_REAL_UNITS 227

#endif