#include "TextTrig.h"
#include "Common Files/CommonFiles.h"
#include "Maps.h"

extern MAPS maps;
extern HWND hMain;

bool CompareStrTblNode(StringTableNode first, StringTableNode second); // Defined in Structs.h

#define MAX_CONDITIONS 16
#define MAX_ACTIONS	   64

WNDPROC wpTextTrigEdit; // Used to redefine default edit window input handling

LRESULT CALLBACK TextTrigEditProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_KEYDOWN:
			if ( GetKeyState(VK_CONTROL) & 0x8000 && wParam == 'A' ) // Select all
				SendMessage(hWnd, EM_SETSEL, 0, -1);
			break;

		case WM_CHAR:
			if ( GetKeyState(VK_CONTROL) & 0x8000 && wParam == 1 ) // Prevent ctrl + key from causing beeps
				return 0;
			break;
	}
	return CallWindowProc(wpTextTrigEdit, hWnd, msg, wParam, lParam);
}

BOOL CALLBACK TextTrigProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg )
	{
		case WM_INITDIALOG:
			{
				HWND hEdit = GetDlgItem(hWnd, IDC_EDIT_TRIGTEXT);
				wpTextTrigEdit = (WNDPROC)SetWindowLong(hEdit, GWL_WNDPROC, (LONG)&TextTrigEditProc);
				SendMessage(hEdit, EM_SETLIMITTEXT, 0x7FFFFFFE, NULL);
				 
				TextTrigGenerator textTrigs(maps.curr);
				buffer output("TeOu");
				textTrigs.GenerateTextTrigs(maps.curr->TRIG(), output);
				SetDlgItemText(hWnd, IDC_EDIT_TRIGTEXT, (const char*)output.getPtr(0));
			}
			break;

		case WM_MOUSEWHEEL:
			PostMessage(GetDlgItem(hWnd, IDC_EDIT_TRIGTEXT), WM_MOUSEWHEEL, wParam, lParam);
			break;

		case WM_SIZE:
			{
				RECT rcClient, rcCompile, rcCompSave;

				HWND hEdit	   = GetDlgItem(hWnd, IDC_EDIT_TRIGTEXT ),
					 hCompile  = GetDlgItem(hWnd, ID_COMPILE_TRIGS),
					 hCompSave = GetDlgItem(hWnd, IDC_COMPSAVE);

				GetClientRect(hWnd, &rcClient);
				GetWindowRect(hCompile, &rcCompile);
				GetWindowRect(hCompSave, &rcCompSave);

				SetWindowPos(hCompSave, hWnd, rcClient.right-rcClient.left-(rcCompSave.right-rcCompSave.left)-10, rcClient.bottom - (rcCompSave.bottom-rcCompSave.top) - 10, (rcCompSave.right-rcCompSave.left), (rcCompSave.bottom-rcCompSave.top), SWP_NOZORDER|SWP_NOACTIVATE);
				SetWindowPos(hCompile, hWnd, (rcClient.right-rcClient.left-(rcCompSave.right-rcCompSave.left)-10)-(rcCompile.right-rcCompile.left+5), rcClient.bottom - (rcCompile.bottom-rcCompile.top) - 10, (rcCompile.right-rcCompile.left), (rcCompile.bottom-rcCompile.top), SWP_NOZORDER|SWP_NOACTIVATE);
				SetWindowPos(hEdit, hWnd, 0, 0, rcClient.right, (rcClient.bottom - (rcCompile.bottom-rcCompile.top) - 20), SWP_NOZORDER|SWP_NOACTIVATE);
			}
			break;

		case WM_COMMAND:
			switch ( LOWORD(wParam) )
			{
				case IDC_COMPSAVE:
					if ( maps.curr != nullptr )
					{
						if ( CompileEditText(maps.curr, hWnd) )
						{
							if ( maps.SaveCurr(false) )
								MessageBox(NULL, "Success", "Compiler", MB_OK);
							else
							{
								MessageBox(NULL, "Compile Succeeded, Save Failed", "Compiler", MB_OK);
								maps.curr->notifyChange(false);
							}
						}
					}
					else
						Error("No map open!");
					break;
				case ID_COMPILE_TRIGS:
					if ( maps.curr != nullptr )
					{
						if ( CompileEditText(maps.curr, hWnd) )
						{
							MessageBox(NULL, "Success", "Compiler", MB_OK);
							maps.curr->notifyChange(false);
						}
					}
					else
						Error("No map open!");
					break;
			}
			break;

		case WM_CLOSE:
			DestroyWindow(hWnd);
			return false;
			break;

		default:
			return false;
	}
	return true;
}

bool CompileEditText(Scenario* map, HWND hWnd)
{
	if ( map != nullptr )
	{
		int length = GetWindowTextLength(GetDlgItem(hWnd, IDC_EDIT_TRIGTEXT)) + 1;
		char* editText = nullptr;
		try {
			editText = new char[length];
		} catch ( std::bad_alloc ) {
			MessageBox(NULL, "Compilation failed, couldn't allocate space for input text.", "Error!", MB_OK);
			return false;
		}
		GetDlgItemText(hWnd, IDC_EDIT_TRIGTEXT, editText, length);
	
		buffer text("TxTr"), output("TRIG");
		text.addStr(editText, length);
		delete[] editText;

		bool success;
		TextTrigCompiler compiler(map, success); // All data for compilation is gathered on-the-fly, no need to check for updates
		if ( success == false )
			MessageBox(NULL, "Compilation failed, couldn't allocate space for metadata.", "Error!", MB_OK);

		return success && compiler.CompileText(text, map);
	}
	else
		return false;
}


// Begin StaticComponenetParser class

bool StaticTrigComponentParser::ParseNumericComparison(char* text, u8 &dest, u32 pos, u32 end)
{
	int size = end-pos;
	if ( text[pos] == '\"' ) // Quoted argument, ignore the quotes
	{
		pos ++;
		end --;
		size -= 2;
	}

	text = &text[pos];

	if ( size < 1 || size > 8 )
		return false;

	char comparison[12] = { };

	// Take uppercase copy of argument
	int numSkipped = 0;
	for ( int i=0; i<size; i++ )
	{
		if ( text[i] >= 'a' && text[i] <= 'z' )
			comparison[i-numSkipped] = text[i] - 32;
		else if ( text[i] != ' ' ) // also ignore spacing
			comparison[i-numSkipped] = text[i];
		else
			numSkipped ++;
	}
	comparison[size] = '\0';
	bool success = false;
	switch ( comparison[0] )
	{
		case 'A':
			if		( strcmp(&comparison[1], "TLEAST") == 0 ) { dest = 0; success = true; }
			else if ( strcmp(&comparison[1], "TMOST" ) == 0 ) { dest = 1; success = true; }
			break;
		case 'E':
			if ( strcmp(&comparison[1], "XACTLY") == 0 ) { dest = 10; success = true; }
			break;
	}
	return success;
}

bool StaticTrigComponentParser::ParseSwitchState(char* text, u8 &dest, u32 pos, u32 end)
{
	int size = end-pos;
	if ( text[pos] == '\"' ) // Quoted argument, ignore the quotes
	{
		pos ++;
		end --;
		size -= 2;
	}

	if ( size < 1 || size > 7 )
		return false;

	char comparison[8] = { };
	char* srcStr = &text[pos];

	// Take uppercase copy of argument
	int numSkipped = 0;
	for ( int i=0; i<size; i++ )
	{
		if ( srcStr[i] >= 'a' && srcStr[i] <= 'z' )
			comparison[i-numSkipped] = srcStr[i] - 32;
		else if ( srcStr[i] != ' ' ) // also ignore spacing
			comparison[i-numSkipped] = srcStr[i];
		else
			numSkipped ++;
	}

	comparison[size] = '\0';
	bool success = false;
	switch ( comparison[0] )
	{
		case 'C':
			if ( strcmp(&comparison[1], "LEARED") == 0 ) { dest = 3; success = true; }
			break;
		case 'N':
			if ( strcmp(&comparison[1], "OTSET") == 0 ) { dest = 3; success = true; }
			break;
		case 'S':
			if ( strcmp(&comparison[1], "ET") == 0 ) { dest = 2; success = true; }
			break;
	}
	return success;
}

bool StaticTrigComponentParser::ParseSpecialUnitAmount(char* text, u8 &dest, u32 pos, u32 end)
	// All
{
	int size = end-pos;
	if ( text[pos] == '\"' ) // Quoted argument, ignore the quotes
	{
		pos ++;
		end --;
		size -= 2;
	}

	if ( size < 1 || size > 3 )
		return false;

	char comparison[4] = { };
	char* srcStr = &text[pos];

	// Take uppercase copy of argument
	int numSkipped = 0;
	for ( int i=0; i<size; i++ )
	{
		if ( srcStr[i] >= 'a' && srcStr[i] <= 'z' )
			comparison[i-numSkipped] = srcStr[i] - 32;
		else if ( srcStr[i] != ' ' ) // also ignore spacing
			comparison[i-numSkipped] = srcStr[i];
		else
			numSkipped ++;
	}

	comparison[size] = '\0';
	if ( strcmp(comparison, "ALL") == 0 )
	{
		dest = 0;
		return true;
	}
	else
		return false;
}

bool StaticTrigComponentParser::ParseAllianceStatus(char* text, u16 &dest, u32 pos, u32 end)
	// Ally, Enemy, Allied Victory
{
	int size = end-pos;
	if ( text[pos] == '\"' ) // Quoted argument, ignore the quotes
	{
		pos ++;
		end --;
		size -= 2;
	}

	if ( size < 1 || size > 14 )
		return false;

	char comparison[16] = { };
	char* srcStr = &text[pos];

	// Take uppercase copy of argument
	int numSkipped = 0;
	for ( int i=0; i<size; i++ )
	{
		if ( srcStr[i] >= 'a' && srcStr[i] <= 'z' )
			comparison[i-numSkipped] = srcStr[i] - 32;
		else if ( srcStr[i] != ' ' ) // also ignore spacing
			comparison[i-numSkipped] = srcStr[i];
		else
			numSkipped ++;
	}

	comparison[size] = '\0';
	bool success = false;
	switch ( comparison[0] )
	{
		case 'A':
			if		( strcmp(&comparison[1], "LLIEDVICTORY") == 0 ) { dest = 2; success = true; }
			else if ( strcmp(&comparison[1], "LLY"		   ) == 0 ) { dest = 1; success = true; }
			break;
		case 'E':
			if ( strcmp(&comparison[1], "NEMY") == 0 ) { dest = 0; success = true; }
			break;
	}
	return success;
}

bool StaticTrigComponentParser::ParseResourceType(char* text, u8 &dest, u32 pos, u32 end)
{
	int size = end-pos;
	if ( text[pos] == '\"' ) // Quoted argument, ignore the quotes
	{
		pos ++;
		end --;
		size -= 2;
	}
	
	if ( size < 1 || size > 11 )
		return false;

	char resource[12] = { };
	char* srcStr = &text[pos];

	// Take uppercase copy of argument
	int numSkipped = 0;
	for ( int i=0; i<size; i++ )
	{
		if ( srcStr[i] >= 'a' && srcStr[i] <= 'z' )
			resource[i-numSkipped] = srcStr[i] - 32;
		else if ( srcStr[i] != ' ' ) // also ignore spacing
			resource[i-numSkipped] = srcStr[i];
		else
			numSkipped ++;
	}

	resource[size] = '\0';
	char temp = text[end+1]; // temporarily NUL terminate the src str
	text[end+1] = '\0';
	bool success = false;
	switch ( resource[0] )
	{
		case 'O':
			if		( strcmp(&resource[1], "RE"		 ) == 0 ) { dest = 0; success = true; }
			else if ( strcmp(&resource[1], "REANDGAS") == 0 ) { dest = 2; success = true; }
			break;
		case 'G':
			if ( strcmp(&resource[1], "AS") == 0 ) { dest = 1; success = true; }
			break;
	}
	text[end+1] = temp;
	return success;
}

bool StaticTrigComponentParser::ParseScoreType(char* text, u8 &dest, u32 pos, u32 end)
{
	int size = end-pos;
	if ( text[pos] == '\"' ) // Quoted argument, ignore the quotes
	{
		pos ++;
		end --;
		size -= 2;
	}
	
	if ( size < 1 || size > 19 )
		return false;

	char score[20] = { };
	char* srcStr = &text[pos];

	// Take uppercase copy of argument
	int numSkipped = 0;
	for ( int i=0; i<20; i++ )
	{
		if ( srcStr[i] >= 'a' && srcStr[i] <= 'z' )
			score[i-numSkipped] = srcStr[i] - 32;
		else if ( srcStr[i] != ' ' ) // also ignore spacing
			score[i-numSkipped] = srcStr[i];
		else
			numSkipped ++;
	}

	score[size] = '\0';
	char temp = text[end+1]; // temporarily NUL terminate the src str
	text[end+1] = '\0';
	bool success = false;
	switch ( score[0] )
	{
		case 'B':
			if ( strcmp(&score[1], "UILDINGS") == 0 ) { dest = 2; success = true; }
			break;
		case 'C':
			if ( strcmp(&score[1], "USTOM") == 0 ) { dest = 7; success = true; }
			break;
		case 'K':
			if		( strcmp(&score[1], "ILLS"		   ) == 0 ) { dest = 4; success = true; }
			else if ( strcmp(&score[1], "ILLSANDRAZINGS") == 0 ) { dest = 6; success = true; }
			break;
		case 'R':
			if ( strcmp(&score[1], "AZINGS") == 0 ) { dest = 5; success = true; }
			break;
		case 'T':
			if ( strcmp(&score[1], "OTAL") == 0 ) { dest = 0; success = true; }
			break;
		case 'U':
			if		( strcmp(&score[1], "NITS"			 ) == 0 ) { dest = 1; success = true; }
			else if ( strcmp(&score[1], "NITSANDBUILDINGS") == 0 ) { dest = 3; success = true; }
			break;
	}
	text[end+1] = temp;
	return success;
}

bool StaticTrigComponentParser::ParseTextDisplayFlag(char* text, u8 &dest, u32 pos, u32 end)
	// Always Display, Don't Always Display
{
	int size = end-pos;
	if ( text[pos] == '\"' ) // Quoted argument, ignore the quotes
	{
		pos ++;
		end --;
		size -= 2;
	}

	if ( size < 1 || size > 20 )
		return false;

	char comparison[24] = { };
	char* srcStr = &text[pos];

	// Take uppercase copy of argument
	int numSkipped = 0;
	for ( int i=0; i<size; i++ )
	{
		if ( srcStr[i] >= 'a' && srcStr[i] <= 'z' )
			comparison[i-numSkipped] = srcStr[i] - 32;
		else if ( srcStr[i] != ' ' ) // also ignore spacing
			comparison[i-numSkipped] = srcStr[i];
		else
			numSkipped ++;
	}

	comparison[size] = '\0';
	bool success = false;
	switch ( comparison[0] )
	{
		case 'A':
			if ( strcmp(&comparison[1], "LWAYSDISPLAY") == 0 ) { dest |= ACTION_FLAG_ALWAYS_DISPLAY; success = true; }
			break;
		case 'D':
			if ( strcmp(&comparison[1], "ON'TALWAYSDISPLAY") == 0 ) { dest &= ACTION_FLAG_DONT_ALWAYS_DISPLAY; success = true; }
			break;
	}
	return success;
}

bool StaticTrigComponentParser::ParseNumericModifier(char* text, u8 &dest, u32 pos, u32 end)
	// Add, subtract, set to
{
	int size = end-pos;
	if ( text[pos] == '\"' ) // Quoted argument, ignore the quotes
	{
		pos ++;
		end --;
		size -= 2;
	}

	if ( size < 1 || size > 8 )
	{
		return false;
	}

	char comparison[12] = { };
	char* srcStr = &text[pos];

	// Take uppercase copy of argument
	int numSkipped = 0;
	for ( int i=0; i<size; i++ )
	{
		if ( srcStr[i] >= 'a' && srcStr[i] <= 'z' )
			comparison[i-numSkipped] = srcStr[i] - 32;
		else if ( srcStr[i] != ' ' ) // also ignore spacing
			comparison[i-numSkipped] = srcStr[i];
		else
			numSkipped ++;
	}

	comparison[size] = '\0';
	bool success = false;
	switch ( comparison[0] )
	{
		case 'A':
			if ( strcmp(&comparison[1], "DD") == 0 ) { dest = 8; success = true; }
			break;
		case 'S':
			if		( strcmp(&comparison[1], "ETTO"	  ) == 0 ) { dest = 7; success = true; }
			else if ( strcmp(&comparison[1], "UBTRACT") == 0 ) { dest = 9; success = true; }
			break;
	}
	return success;
}

bool StaticTrigComponentParser::ParseSwitchMod(char* text, u8 &dest, u32 pos, u32 end)
	// Set, clear, toggle, randomize
{
	int size = end-pos;
	if ( text[pos] == '\"' ) // Quoted argument, ignore the quotes
	{
		pos ++;
		end --;
		size -= 2;
	}

	if ( size < 1 || size > 9 )
		return false;

	char comparison[10] = { };
	char* srcStr = &text[pos];

	// Take uppercase copy of argument
	int numSkipped = 0;
	for ( int i=0; i<size; i++ )
	{
		if ( srcStr[i] >= 'a' && srcStr[i] <= 'z' )
			comparison[i-numSkipped] = srcStr[i] - 32;
		else if ( srcStr[i] != ' ' ) // also ignore spacing
			comparison[i-numSkipped] = srcStr[i];
		else
			numSkipped ++;
	}

	comparison[size] = '\0';
	bool success = false;
	switch ( comparison[0] )
	{
		case 'C':
			if ( strcmp(&comparison[1], "LEAR") == 0 ) { dest = 5; success = true; }
			break;
		case 'R':
			if ( strcmp(&comparison[1], "ANDOMIZE") == 0 ) { dest = 11; success = true; }
			break;
		case 'S':
			if ( strcmp(&comparison[1], "ET") == 0 ) { dest = 4; success = true; }
			break;
		case 'T':
			if ( strcmp(&comparison[1], "OGGLE") == 0 ) { dest = 6; success = true; }
			break;
	}
	return success;
}

bool StaticTrigComponentParser::ParseStateMod(char* text, u8 &dest, u32 pos, u32 end)
	// Disable, Disabled, Enable, Enabled, Toggle
{
	int size = end-pos;
	if ( text[pos] == '\"' ) // Quoted argument, ignore the quotes
	{
		pos ++;
		end --;
		size -= 2;
	}

	if ( size < 1 || size > 8 )
		return false;

	char comparison[12] = { };
	char* srcStr = &text[pos];

	// Take uppercase copy of argument
	int numSkipped = 0;
	for ( int i=0; i<size; i++ )
	{
		if ( srcStr[i] >= 'a' && srcStr[i] <= 'z' )
			comparison[i-numSkipped] = srcStr[i] - 32;
		else if ( srcStr[i] != ' ' ) // also ignore spacing
			comparison[i-numSkipped] = srcStr[i];
		else
			numSkipped ++;
	}

	comparison[size] = '\0';
	bool success = false;
	switch ( comparison[0] )
	{
		case 'D':
			if		( strcmp(&comparison[1], "ISABLE" ) == 0 ) { dest = 5; success = true; }
			else if ( strcmp(&comparison[1], "ISABLED") == 0 ) { dest = 5; success = true; }
			break;
		case 'E':
			if		( strcmp(&comparison[1], "NABLE" ) == 0 ) { dest = 4; success = true; }
			else if ( strcmp(&comparison[1], "NABLED") == 0 ) { dest = 4; success = true; }
			break;
		case 'T':
			if ( strcmp(&comparison[1], "OGGLE") == 0 ) { dest = 6; success = true; }
			break;
	}
	return success;
}

bool StaticTrigComponentParser::ParseOrder(char* text, u8 &dest, u32 pos, u32 end)
	// Attack, move, patrol
{
	int size = end-pos;
	if ( text[pos] == '\"' ) // Quoted argument, ignore the quotes
	{
		pos ++;
		end --;
		size -= 2;
	}

	if ( size < 1 || size > 6 )
		return false;

	char comparison[8] = { };
	char* srcStr = &text[pos];

	// Take uppercase copy of argument
	int numSkipped = 0;
	for ( int i=0; i<size; i++ )
	{
		if ( srcStr[i] >= 'a' && srcStr[i] <= 'z' )
			comparison[i-numSkipped] = srcStr[i] - 32;
		else if ( srcStr[i] != ' ' ) // also ignore spacing
			comparison[i-numSkipped] = srcStr[i];
		else
			numSkipped ++;
	}

	comparison[size] = '\0';
	bool success = false;
	switch ( comparison[0] )
	{
		case 'A':
			if ( strcmp(&comparison[1], "TTACK") == 0 ) { dest = 2; success = true; }
			break;
		case 'M':
			if ( strcmp(&comparison[1], "OVE") == 0 ) { dest = 0; success = true; }
			break;
		case 'P':
			if ( strcmp(&comparison[1], "ATROL") == 0 ) { dest = 1; success = true; }
			break;
	}
	return success;
}

bool StaticTrigComponentParser::ParseScript(char* text, u32 &dest, u32 pos, u32 end)
{
	/** With scripts, the exact ascii characters entered are the exact bytes out.
		As a consequence, if the script name is not quoted and is comprised entirely
		of numbers, it must be considered a script number, and this method should
		return false so ParseByte can be called. */

	int size = end-pos;
	bool isQuoted = ( text[pos] == '\"' );
	if ( isQuoted ) // Quoted argument, ignore the quotes
	{
		pos ++;
		end --;
		size -= 2;

		if ( size == 4 )
		{
			dest = (u32 &)(text[pos]);
			return true;
		}
	}
	else // was not quoted
	{
		if ( size == 4 )
		{
			// If at least one character is not a number, treat it regularly, else, return false
			for ( int i=0; true; i++ )
			{
				if ( i == 5 ) // All characters were numbers, return false
					return false;
				else if ( text[pos+i] < '0' || text[pos+i] > '9' ) // The current character is not a number
				{
					dest = (u32 &)(text[pos]);
					return true;
				}
			}
		}
	}

	return false;
}

bool StaticTrigComponentParser::ParseResourceType(char* text, u16 &dest, u32 pos, u32 end)
{
	u8 temp = 0;
	bool success = ParseResourceType(text, temp, pos, end);
	dest = temp;
	return success;
}

bool StaticTrigComponentParser::ParseScoreType(char* text, u16 &dest, u32 pos, u32 end)
{
	u8 temp = 0;
	bool success = ParseScoreType(text, temp, pos, end);
	dest = temp;
	return success;
}

bool StaticTrigComponentParser::ParseLong(char* text, u32& dest, u32 pos, u32 end)
{
	int size = end - pos;
	if ( size < 12 )
	{
		if ( size == 1 && text[pos] == '0' )
		{
			dest = 0;
			return true;
		}
		else
		{
			char potentialLong[12] = { };
			memcpy(potentialLong, &text[pos], size);
			potentialLong[size] = '\0';
			return ( (dest = atoi(potentialLong)) > 0 );
		}
	}
	return false;
}

bool StaticTrigComponentParser::ParseTriplet(char* text, u8* dest, u32 pos, u32 end)
{
	u32 val;
	bool isNegative;

	if ( text[pos] == '-' ) // isNegative
	{
		pos ++;
		isNegative = true;
	}

	if ( ParseLong(text, val, pos, end) )
	{
		if ( isNegative )
			val = 0x1000000 - val;
		
		dest[2] = u8(val/0x10000);
		val /= 0x10000;
		dest[1] = u8(val/0x100);
		val /= 0x100;
		dest[0] = u8(val);
		return true;
	}
	return false;
}

bool StaticTrigComponentParser::ParseShort(char* text, u16& dest, u32 pos, u32 end)
{
	int size = end - pos;
	if ( size < 7 )
	{
		if ( size == 1 && text[pos] == '0' )
		{
			dest = 0;
			return true;
		}
		char potentialShort[7] = { };
		memcpy(potentialShort, &text[pos], size);
		potentialShort[size] = '\0';
		return ( (dest = atoi(potentialShort)) > 0 );
	}
	return false;
}

bool StaticTrigComponentParser::ParseByte(char* text, u8& dest, u32 pos, u32 end)
{
	int size = end - pos;
	if ( size < 5 )
	{
		if ( size == 1 && text[pos] == '0' )
		{
			dest = 0;
			return true;
		}
		char potentialByte[5] = { };
		memcpy(potentialByte, &text[pos], size);
		potentialByte[size] = '\0';
		return ( (dest = atoi(potentialByte)) > 0 );
	}
	return false;
}


// Begin TextTrigCompiler class

TextTrigCompiler::TextTrigCompiler(Scenario* map, bool& success)
	: strUsage(map, false, success), extendedStrUsage(map, true, success) // Will have to change something here for extended strings to work
{
	success = success && PrepLocationTable(map) && PrepUnitTable(map) && PrepSwitchTable(map)
					  && PrepWavTable(map)	   && PrepGroupTable(map) && PrepStringTable(map);
}

bool TextTrigCompiler::CompileText(buffer& text, Scenario* chk)
{
	buffer output("TxOp");

	CleanText(text);
	char error[MAX_ERROR_MESSAGE_SIZE];

	if ( ParseTriggers(text, output, error) )
	{
		if ( BuildNewStringTable(chk) )
		{
			if ( !chk->TRIG().exists() )
			{
				output.setTitle("TRIG");
				if ( chk->AddSection(output) )
					return true;
				else
					sprintf_s(error, "No text errors, but compilation must abort due to low memory.\n\n%s", LastError);
			}
			else if ( chk->TRIG().takeAllData(output) )
				return true;
			else
				sprintf_s(error, "No text errors, but TRIG could not be overwritten.\n\n%s", LastError);
		}
		else
			sprintf_s(error, "No text errors, but compilation must abort due to low memory.\n\n%s", LastError);
	}

	MessageBox(NULL, error, "Error!", MB_OK|MB_ICONEXCLAMATION);
	return false;
}

// protected

void TextTrigCompiler::CleanText(buffer &text)
{
	u32 pos = 0;
	bool inString = false;
	buffer dest("TeCp");
	dest.setSize(text.size());

	while ( pos < text.size() ) 
	{
		switch ( text.get<u8>(pos) )
		{
			case ' ': // Space
			case '	': // Tab
				pos ++;
				break;
			case '\15': // CR (line ending)
				if ( text.get<u8>(pos+1) == '\12' ) // Has LF
					pos ++;
			case '\12': // LF (line ending)
			case '\13': // VT (line ending)
			case '\14': // FF (line ending)
				dest.add<u8>('\15');
				dest.add<u8>('\12');
				pos ++;
				break;
			
			case '\"': // Found a string
				dest.add<u8>('\"');
				pos ++;

				u32 skipTo;
				if ( text.getNext('\"', pos, skipTo) )
				{
					for ( pos; pos<skipTo; pos++ )
					{
						char curr = text.get<u8>(pos);
						if ( curr == '\\' ) // Escape Character
						{
							pos ++;
							if ( text.get<u8>(pos) == 'x' )
							{
								pos ++;
								pos ++; // First num should always be 0
						
								u8 targetVal = 0;

								if ( text.get<u8>(pos) >= '0' && text.get<u8>(pos) <= '9' )
									targetVal += 16*(text.get<u8>(pos)-'0');
								else if ( text.get<u8>(pos) >= 'A' && text.get<u8>(pos) <= 'F' )
									targetVal += 16*(text.get<u8>(pos)-'A'+10);

								pos ++;

								if ( text.get<u8>(pos) >= '0' && text.get<u8>(pos) <= '9' )
									targetVal += text.get<u8>(pos)-'0';
								else if ( text.get<u8>(pos) >= 'A' && text.get<u8>(pos) <= 'F' )
									targetVal += text.get<u8>(pos)-'A'+10;

								dest.add<u8>(targetVal);
							}
							else if ( text.get<u8>(pos) == 'r' )
							{
								dest.add<u8>('\r');
							}
							else if ( text.get<u8>(pos) == 'n' )
							{
								dest.add<u8>('\n');
							}
							else if ( text.get<u8>(pos) == '\\' )
							{
								dest.add<u8>('\\');
							}
						}
						else
							dest.add<u8>(curr);
					}

					dest.add<u8>('\"');
					pos ++;
				}
				else
					pos ++;
				break;

			case '/': // Might be a comment
				if ( text.get<u8>(pos+1) == '/' ) // Found a comment, skip over it
				{
					u32 lineEnd;

					if ( !text.getNext('\15', pos, lineEnd) ) // Check for CR
					{
						if ( !text.getNext('\12', pos, lineEnd) ) // Check for LF
						{
							if ( !text.getNext('\13', pos, lineEnd ) ) // Check for VT
							{
								if ( !text.getNext('\14', pos, lineEnd) ) // Check for FF
									lineEnd = text.size(); // File probably ended this line
							}
						}
					}
					pos = lineEnd;
				}
				break;

			default:
				UCHAR curr = text.get<u8>(pos);
				if ( curr > 96 && curr < 123 ) // Capitalize letters
					dest.add<u8>(curr-32);
				else
					dest.add<u8>(curr);
				pos ++;
				break;
		}
	}
	text.overwrite((const char*)dest.getPtr(0), dest.size());
}

bool TextTrigCompiler::ParseTriggers(buffer &text, buffer &output, char *error)
{
	u8 flags;

	u32 pos = 0,
		expecting = 0,
		line = 1,
		playerEnd, endPlayers,
		conditionEnd, conditionIndex,
		actionEnd, actionIndex,
		argEnd, argsLeft = 0,
		lineEnd,
		numConditions = 0, numActions = 0;

	Trigger currTrig = { };
	Condition* currCondition = &currTrig.conditions[0];
	Action* currAction = &currTrig.actions[0];

	while ( pos < text.size() )
	{
		if ( text.has('\15', pos) ) // Line End
		{
			pos += 2;
			line ++;
		}
		else
		{
			switch ( expecting )
			{
				case 0:	//		trigger
						// or	%NULL
					{
						if ( text.has("TRIGGER(", pos, 8) )
						{
							pos += 8;
							expecting ++;
						}
						else if ( text.has('\0', pos) ) // End of Text
						{
							strcpy_s(error, MAX_ERROR_MESSAGE_SIZE, "Success!");
							return true;
						}
						else if ( text.has("TRIGGER", pos, 7) )
						{
							pos += 7;
							while ( text.has('\15', pos) )
							{
								pos += 2;
								line ++;
							}
							if ( text.has('(', pos) )
							{
								pos ++;
								expecting ++;
							}
							else
							{
								sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \'(\'", line);
								return false;
							}
						}
						else 
						{
							sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \"Trigger\" or End of Text", line);
							return false;
						}
					}
					break;

				case 1:	//		%PlayerName,
						// or	%PlayerName)
						// or	"%PlayerName"
						// or	)
					{
						if ( text.get<u8>(pos) == '\"' ) // Quoted Player Name
						{
							pos ++;
							if ( text.getNext('\"', pos, playerEnd) )
							{
								if ( !text.getNext('\15', pos, lineEnd) )
									text.getNext(NULL, pos, lineEnd); // Text ends on this line

								if ( playerEnd < lineEnd )
								{
									if ( ParseExecutingPlayer(text, currTrig, pos, playerEnd) )
									{
										pos = playerEnd+1;
										while ( text.has('\15', pos) )
										{
											pos += 2;
											line ++;
										}
										if ( text.has(')', pos) )
											expecting ++;
									}
									else
									{
										sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: Player Identifier", line);
										return false;
									}
									pos ++;
								}
								else
								{
									sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: Closing \'\"\'", line);
									return false;
								}
							}
							else
							{
								sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: Closing \'\"\'", line);
								return false;
							}
						}
						else if ( text.get<u8>(pos) == ')' ) // No players
						{
							pos ++;
							expecting ++;
						}
						else
						{
							if ( text.getNextUnquoted(',', pos, playerEnd) || text.getNextUnquoted(')', pos, playerEnd) )
							{
								if ( !text.getNext('\15', pos, lineEnd) )
									text.getNext(NULL, pos, lineEnd); // Text ends on this line
								
								playerEnd = SmallestOf(playerEnd, endPlayers, lineEnd);

								if ( ParseExecutingPlayer(text, currTrig, pos, playerEnd) )
								{
									pos = playerEnd;
									while ( text.has('\15', pos) )
									{
										pos += 2;
										line ++;
									}
									if ( text.has(')', pos) )
										expecting ++;
									pos ++;
								}
								else
								{
									sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: Player Identifier", line);
									return false;
								}
							}
							else
							{
								sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \',\' or \')\'", line);
								return false;
							}
						}
					}
					break;
	
				case 2: //		{
					{
						if ( text.has('{', pos) )
						{
							pos ++;
							expecting ++;
						}
						else
						{
							sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: '{'", line);
							return false;
						}
					}
					break;
	
				case 3: //		conditions:
						// or	actions:
						// or	}
					{
						if ( text.has("CONDITIONS:", pos, 11) )
						{
							pos += 11;
							expecting ++;
						}
						else if ( text.has("ACTIONS:", pos, 8) )
						{
							pos += 8;
							expecting += 4;
						}
						else if ( text.has('}', pos ) )
						{
							pos ++;
							expecting = 10;
						}
						else
						{
							if ( text.has("CONDITIONS", pos, 10) || text.has("ACTIONS", pos, 7) )
							{
								pos += 7;
								while ( text.has('\15', pos) )
								{
									pos += 2;
									line ++;
								}
								sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \':\'", line);
							}
							else
								sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \"Conditions\" or \"Actions\" or \'}\'", line);

							return false;
						}
					}
					break;
	
				case 4:	//		%ConditionName(
						// or	;%ConditionName(
						// or	actions:
						// or	}
					{
						if ( text.has(';', pos) ) // Disabled condition
						{
							while ( text.has('\15', pos) )
							{
								pos += 2;
								line ++;
							}
							pos ++;
							if ( text.getNext('(', pos, conditionEnd) )
							{
								if ( !text.getNext('\15', pos, lineEnd) )
									text.getNext(NULL, pos, lineEnd);

								conditionEnd = std::min(conditionEnd, lineEnd);

								if ( ParseCondition(text, pos, conditionEnd, true, conditionIndex, flags, argsLeft) )
								{
									if ( numConditions > MAX_CONDITIONS )
									{
										sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nCondition Max Exceeded!");
										return false;
									}
									currCondition = &currTrig.conditions[numConditions];
									currCondition->flags = flags | CONDITION_FLAG_DISABLED;
									if ( conditionIndex < 0 )
										currCondition->condition = ExtendedToRegularCID(conditionIndex);
									else
										currCondition->condition = u8(conditionIndex);
									numConditions ++;
									
									pos = conditionEnd;
									while ( text.has('\15', pos) )
									{
										pos += 2;
										line ++;
									}

									if ( text.has('(', pos) )
									{
										pos ++;
										expecting ++;
									}
									else
									{
										sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \'(\'");
										return false;
									}
								}
								else
								{
									sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: Condition Name", line);
									return false;
								}
							}
							else
							{
								sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \'(\'", line);
								return false;
							}
						}
						else if ( text.has('}', pos ) ) // End trigger
						{
							pos ++;
							expecting = 10;
						}
						else if ( text.has("ACTIONS", pos, 7 ) ) // End conditions
						{
							pos += 7;
							if ( text.has(':', pos) )
							{
								pos ++;
								expecting = 7;
							}
							else
							{
								sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \':\'", line);
								return false;
							}
						}
						else if ( text.getNext('(', pos, conditionEnd) ) // Has a condition or an error
						{
							if ( !text.getNext('\15', pos, lineEnd) )
								text.getNext(NULL, pos, lineEnd);

							conditionEnd = std::min(conditionEnd, lineEnd);

							if ( ParseCondition(text, pos, conditionEnd, false, conditionIndex, flags, argsLeft) )
							{
								if ( numConditions > MAX_CONDITIONS )
								{
									sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nCondition Max Exceeded!");
									return false;
								}
								currCondition = &currTrig.conditions[numConditions];
								currCondition->flags = flags;
								if ( conditionIndex < 0 )
									currCondition->condition = ExtendedToRegularCID(conditionIndex);
								else
									currCondition->condition = u8(conditionIndex);
								numConditions ++;

								pos = conditionEnd;
								while ( text.has('\15', pos) )
								{
									pos += 2;
									line ++;
								}

								if ( text.has('(', pos) )
								{
									pos ++;
									expecting ++;
								}
								else
								{
									sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \'(\'", line);
									return false;
								}
							}
							else
							{
								sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: Condition Name", line);
								return false;
							}
						}
						else
						{
							sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \'(\'", line);
							return false;
						}
					}
					break;
	
				case 5: //		);
						// or	%ConditionArg,
						// or	%ConditionArg);
						// or	"%ConditionArg",
						// or	"%ConditionArg");
					{
						if ( text.has(')', pos) ) // Condition End
						{
							if ( argsLeft > 0 )
							{
								sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: Condition Argument", line);
								return false;
							}

							pos ++;
							while ( text.has('\15', pos) )
							{
								pos += 2;
								line ++;
							}

							if ( text.has(';', pos) )
							{
								pos ++;
								expecting --;
							}
							else
							{
								sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \';\'", line);
								return false;
							}
						}
						else if ( argsLeft == 0 )
						{
							sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \')\'", line);
							return false;
						}
						else if ( argsLeft == 1 )
						{
							if ( text.getNextUnquoted(')', pos, argEnd) )
							{
								if ( ParseConditionArg(text, *currCondition, pos, argEnd, conditionIndex, argsLeft, error) )
								{
									pos = argEnd;
									argsLeft --;
								}
								else
								{
									sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\n%s", line, LastError);
									return false;
								}
							}
							else
							{
								sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \')\'", line);
								return false;
							}
						}
						else if ( text.has('\"', pos) ) // Quoted Argument
						{
							pos ++;
							if ( text.getNext('\"', pos, argEnd) )
							{
								if ( !text.getNext('\15', pos, lineEnd) )
									text.getNext(NULL, pos, lineEnd);

								if ( argEnd < lineEnd )
								{
									if ( ParseConditionArg(text, *currCondition, pos, argEnd, conditionIndex, argsLeft, error) )
									{
										pos = argEnd+1;
										while ( text.has('\15', pos) )
										{
											pos += 2;
											line ++;
										}

										if ( text.has(',', pos) )
											pos ++;

										argsLeft --;
									}
									else
									{
										sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\n%s", line, LastError);
										return false;
									}
								}
								else
								{
									sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: Closing \'\"\'", line);
									return false;
								}
							}
							else
							{
								sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: Closing '\"'", line);
								return false;
							}
						}
						else if ( text.getNextUnquoted(',', pos, argEnd) ) // Has argument
						{
							if ( ParseConditionArg(text, *currCondition, pos, argEnd, conditionIndex, argsLeft, error) )
							{
								pos = argEnd+1;
								argsLeft --;
							}
							else
							{
								sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\n%s", line, LastError);
								return false;
							}
						}
						else
						{
							sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \',\'", line);
							return false;
						}
						 
					}
					break;
	
				case 6: //		actions:
						// or	}
					{
						if ( text.has('}', pos) )
						{
							pos ++;
							expecting = 10;
						}
						else if ( text.has("ACTIONS:", pos, 8) )
						{
							pos += 8;
							expecting ++;
						}
						else
						{
							if ( text.has("ACTIONS", pos, 7) )
								sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \':\'", line);
							else
								sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \"Actions\" or '}'", line);
							return false;
						}
					}
					break;
	
				case 7: //		%ActionName(
						// or	;%ActionName(
						// or	}
					{
						if ( text.has(';', pos ) )
						{
							pos ++;
							if ( text.getNext('(', pos, actionEnd) )
							{
								if ( !text.getNext('\15', pos, lineEnd) )
									text.getNext(NULL, pos, lineEnd);

								actionEnd = std::min(actionEnd, lineEnd);

								if ( ParseAction(text, pos, actionEnd, true, actionIndex, flags, argsLeft) )
								{
									if ( numActions > MAX_ACTIONS )
									{
										sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nAction Max Exceeded!");
										return false;
									}

									currAction = &currTrig.actions[numActions];
									currAction->flags = flags | ACTION_FLAG_DISABLED;
									if ( actionIndex < 0 )
										currAction->action = ExtendedToRegularAID(actionIndex);
									else
										currAction->action = u8(actionIndex);
									currAction->action = u8(actionIndex);
									numActions ++;

									pos = actionEnd+1;
									expecting ++;
								}
								else
								{
									sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: Action Name or \'}\'", line);
									return false;
								}
							}
							else
							{
								sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: '('", line);
								return false;
							}
						}
						else if ( text.has('}', pos) )
						{
							pos ++;
							expecting = 10;
						}
						else if ( text.getNext('(', pos, actionEnd) )
						{
							if ( !text.getNext('\15', pos, lineEnd) )
								text.getNext(NULL, pos, lineEnd);

							actionEnd = std::min(actionEnd, lineEnd);

							if ( ParseAction(text, pos, actionEnd, false, actionIndex, flags, argsLeft) )
							{
								if ( numActions > MAX_ACTIONS )
								{
									sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nAction Max Exceeded!");
									return false;
								}
								currAction = &currTrig.actions[numActions];
								currAction->flags = flags;
								if ( actionIndex < 0 )
									currAction->action = ExtendedToRegularAID(actionIndex);
								else
									currAction->action = u8(actionIndex);
								numActions ++;

								pos = actionEnd+1;
								expecting ++;
							}
							else
							{
								sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: Action Name or \'}\'", line);
								return false;
							}
						}
						else
						{
							sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \'(\'", line);
							return false;
						}
					}
					break;
	
				case 8: //		);
						// or	%ActionArg,
						// or	%ActionArg);
						// or	"%ActionArg",
						// or	"%ActionArg");
					{
						if ( text.has(')', pos) ) // Action End
						{
							if ( argsLeft > 0 )
							{
								sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: Action Argument", line);
								return false;
							}

							pos ++;
							while ( text.has('\15', pos) )
							{
								pos += 2;
								line ++;
							}

							if ( text.has(';', pos) )
							{
								pos ++;
								expecting --;
							}
							else
							{
								sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \';\'", line);
								return false;
							}
						}
						else if ( argsLeft == 0 )
						{
							sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \')\'", line);
							return false;
						}
						else if ( argsLeft == 1 )
						{
							if ( text.getNextUnquoted(')', pos, argEnd) )
							{
								if ( ParseActionArg(text, *currAction, pos, argEnd, actionIndex, argsLeft, error) )
								{
									pos = argEnd;
									argsLeft --;
								}
								else
								{
									sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: Action Argument\n\n%s", line, LastError);
									return false;
								}
							}
							else
							{
								sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \')\'", line);
								return false;
							}
						}
						else if ( text.getNextUnquoted(',', pos, argEnd) ) // Has argument
						{
							if ( ParseActionArg(text, *currAction, pos, argEnd, actionIndex, argsLeft, error) )
							{
								pos = argEnd+1;
								argsLeft --;
							}
							else
							{
								sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: Action Argument\n\n%s", line, LastError);
								return false;
							}
						}
					}
					break;
	
				case 9: //		}
					{
						if ( text.has('}', pos) )
						{
							pos ++;
							expecting = 10;
						}
						else
						{
							sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \'}\'", line);
							return false;
						}
					}
					break;

				case 10: // Trigger end was found, reset
					{
						numConditions = 0;
						numActions = 0;

						if ( !output.add<Trigger&>(currTrig) )
						{
							sprintf_s(error, MAX_ERROR_MESSAGE_SIZE, "Failed to add trigger.\n\n%s", LastError);
							return false;
						}

						memset(&currTrig, 0, sizeof(Trigger));
						expecting = 0;
					}
					break;
			}
		}
	}
	return true;
}

bool TextTrigCompiler::ParseExecutingPlayer(buffer &text, Trigger &currTrig, u32 pos, u32 end)
{
	u32 group;
	if ( ParsePlayer(text, group, pos, end) &&
		 group < 28 )
	{
		currTrig.players[group] = 1;
		return true;
	}
	return false;
}

bool TextTrigCompiler::ParseCondition(buffer &text, u32 pos, u32 end, bool disabled, u32 &ID, u8& flags, u32 &argsLeft)
{
	ID = CID_NO_CONDITION;
	u16 number = 0;

	u32 size = end - pos;
	buffer arg;

	for ( u32 i=0; i<size; i++ ) // Copy argument to arg buffer
	{
		arg.add<u8>(text.get<u8>(i+pos));
		if ( arg.get<u8>(i) > 96 && arg.get<u8>(i) < 123 ) // If lower-case
			arg.replace<char>(i, arg.get<u8>(i)-32); // Capitalize
	}

	for ( u32 i=0; i<arg.size(); i++ )
	{
		if ( arg.get<u8>(i) == ' ' ) // Del spacing
			arg.del<u8>(i);
		else if ( arg.get<u8>(i) == '	' ) // Del tabbing
			arg.del<u8>(i);
	}

	char currChar = arg.get<u8>(0);

	switch ( currChar )
	{
		case 'A':
			{
				if ( arg.has("CCUMULATE", 1, 9) )
					ID = CID_ACCUMULATE;
				else if ( arg.has("LWAYS", 1, 5) )
					ID = CID_ALWAYS;
			}
			break;
		
		case 'B':
			if ( arg.has("RING", 1, 4) )
				ID = CID_BRING;
			break;

		case 'C':
			if ( arg.has("OMMAND", 1, 6) )
			{
				if ( arg.has("THELEAST", 7, 8) )
				{
					if ( arg.has("AT", 15, 2) )
						ID = CID_COMMAND_THE_LEAST_AT;
					else
						ID = CID_COMMAND_THE_LEAST;
				}
				else if ( arg.has("THEMOST", 7, 7) )
				{
					if ( arg.has("AT", 14, 2) )
						ID = CID_COMMAND_THE_MOST_AT;
					else
						ID = CID_COMMAND_THE_MOST;
				}
				else if ( arg.has("STHEMOSTAT", 7, 10) ) // command'S', added for backwards compatibility
					ID = CID_COMMAND_THE_MOST_AT;
				else
					ID = CID_COMMAND;
			}
			else if ( arg.has("OUNTDOWNTIMER", 1, 13) )
				ID = CID_COUNTDOWN_TIMER;
			else if ( arg.has("USTOM", 1, 5) )
			{
				ID = CID_CUSTOM;
				argsLeft = 8;
			}
			break;

		case 'D':
			if ( arg.has("EATHS", 1, 5) )
				ID = CID_DEATHS;
			break;

		case 'E':
			if ( arg.has("LAPSEDTIME", 1, 10) )
				ID = CID_ELAPSED_TIME;
			break;

		case 'H':
			if ( arg.has("IGHESTSCORE", 1, 11) )
				ID = CID_HIGHEST_SCORE;
			break;

		case 'K':
			if ( arg.has("ILL", 1, 3) )
				ID = CID_KILL;
			break;

		case 'L':
			if ( arg.has("EAST", 1, 4) )
			{
				if ( arg.has("KILLS", 5, 5) )
					ID = CID_LEAST_KILLS;
				else if ( arg.has("RESOURCES", 5, 9) )
					ID = CID_LEAST_RESOURCES;
			}
			else if ( arg.has("OWESTSCORE", 1, 10) )
				ID = CID_LOWEST_SCORE;
			break;

		case 'M':
			if ( arg.has("EMORY", 1, 5) )
				ID = CID_MEMORY;
			else if ( arg.has("OST", 1, 3) )
			{
				if ( arg.has("KILLS", 4, 5) )
					ID = CID_MOST_KILLS;
				else if ( arg.has("RESOURCES", 4, 9) )
					ID = CID_MOST_RESOURCES;
			}
			break;

		case 'N':
			if ( arg.has("EVER", 1, 4) )
				ID = CID_NEVER;
			break;

		case 'O':
			if ( arg.has("PPONENTS", 1, 8) )
				ID = CID_OPPONENTS;
			break;

		case 'S':
			if ( arg.has("CORE", 1, 4) )
				ID = CID_SCORE;
			else if ( arg.has("WITCH", 1, 5) )
				ID = CID_SWITCH;
			break;
	}

	const u8 conditionNumArgs[] = { 0, 2, 4, 5, 4, 4, 1, 2, 1, 1,
									1, 2, 2, 0, 3, 4, 1, 2, 1, 1,
									1, 4, 0, 0 };



	u8 defaultFlags[] = { 0, 0, 16, 16, 0,  16, 16, 16, 16, 0,
						  0, 0, 0, 0, 0,	16, 16, 16, 16, 0,
						  0, 0, 0, 0 };

	if ( ID >= 0 && ID < sizeof(conditionNumArgs)/sizeof(const u8) )
	{
		argsLeft = conditionNumArgs[ID];
		flags = defaultFlags[ID];
	}
	else
	{
		argsLeft = ExtendedNumConditionArgs(ID);
		flags = 0;
	}

	return ID != CID_NO_CONDITION;
}

bool TextTrigCompiler::ParseAction(buffer &text, u32 pos, u32 end, bool diabled, u32 &ID, u8& flags, u32 &argsLeft)
{
	ID = AID_NO_ACTION;
	u16 number = 0;

	u32 size = end - pos;
	buffer arg;

	for ( u32 i=0; i<size; i++ ) // Copy argument to arg buffer
	{
		arg.add<u8>(text.get<u8>(i+pos));
		if ( arg.get<u8>(i) > 96 && arg.get<u8>(i) < 123 ) // If lower-case
			arg.replace<char>(i, arg.get<u8>(i)-32); // Capitalize
	}

	for ( u32 i=0; i<arg.size(); i++ )
	{
		if ( arg.get<u8>(i) == ' ' ) // Del spacing
			arg.del<u8>(i);
		else if ( arg.get<u8>(i) == '	' ) // Del tabbing
			arg.del<u8>(i);
	}

	char currChar = arg.get<u8>(0);

	switch ( currChar )
	{
		case 'C':
			if ( arg.has("OMMENT", 1, 6) )
				ID = AID_COMMENT;
			else if ( arg.has("REATEUNIT", 1, 9) )
			{
				if ( arg.has("WITHPROPERTIES", 10, 14) )
					ID = AID_CREATE_UNIT_WITH_PROPERTIES;
				else
					ID = AID_CREATE_UNIT;
			}
			else if ( arg.has("ENTERVIEW", 1, 9) )
				ID = AID_CENTER_VIEW;
			else if ( arg.has("USTOM", 1, 5) )
				ID = AID_CUSTOM;
			break;

		case 'D':
			if ( arg.has("ISPLAYTEXTMESSAGE", 1, 17) )
				ID = AID_DISPLAY_TEXT_MESSAGE;
			else if ( arg.has("EFEAT", 1, 5) )
				ID = AID_DEFEAT;
			else if ( arg.has("RAW", 1, 3) )
				ID = AID_DRAW;
			break;

		case 'G':
			if ( arg.has("IVEUNITSTOPLAYER", 1, 16) )
				ID = AID_GIVE_UNITS_TO_PLAYER;
			break;

		case 'K':
			if ( arg.has("ILLUNIT", 1, 7) )
			{
				if ( arg.has("ATLOCATION", 8, 10) )
					ID = AID_KILL_UNIT_AT_LOCATION;
				else
					ID = AID_KILL_UNIT;
			}
			break;

		case 'L':
			if ( arg.has("EADERBOARD", 1, 10) )
			{
				if ( arg.has("GOAL", 11, 4) )
				{
					if ( arg.has("CONTROL", 15, 7) )
					{
						if ( arg.has("ATLOCATION", 22, 10) )
							ID = AID_LEADERBOARD_GOAL_CONTROL_AT_LOCATION;
						else
							ID = AID_LEADERBOARD_GOAL_CONTROL;
					}
					else if ( arg.has("KILLS", 15, 5) )
						ID = AID_LEADERBOARD_GOAL_KILLS;
					else if ( arg.has("POINTS", 15, 6) )
						ID = AID_LEADERBOARD_GOAL_POINTS;
					else if ( arg.has("RESOURCES", 15, 9) )
						ID = AID_LEADERBOARD_GOAL_RESOURCES;
				}
				else
				{
					if ( arg.has("CONTROL", 11, 7) )
					{
						if ( arg.has("ATLOCATION", 18, 10) )
							ID = AID_LEADERBOARD_CONTROL_AT_LOCATION;
						else
							ID = AID_LEADERBOARD_CONTROL;
					}
					else if ( arg.has("GREED", 11, 5) )
						ID = AID_LEADERBOARD_GREED;
					else if ( arg.has("KILLS", 11, 5) )
						ID = AID_LEADERBOARD_KILLS;
					else if ( arg.has("POINTS", 11, 6) )
						ID = AID_LEADERBOARD_POINTS;
					else if ( arg.has("RESOURCES", 11, 9) )
						ID = AID_LEADERBOARD_RESOURCES;
					else if ( arg.has("COMPUTERPLAYERS", 11, 15) )
						ID = AID_LEADERBOARD_COMPUTER_PLAYERS;
				}
			}
			break;

		case 'M':
			if ( arg.has("EMORY", 1, 5) )
				ID = AID_MEMORY;
			else if ( arg.has("OVE", 1, 3) )
			{
				if ( arg.has("UNIT", 4, 4) )
					ID = AID_MOVE_UNIT;
				else if ( arg.has("LOCATION", 4, 8) )
					ID = AID_MOVE_LOCATION;
			}
			else if ( arg.has("ODIFYUNIT", 1, 9) )
			{
				if ( arg.has("ENERGY", 10, 6) )
					ID = AID_MODIFY_UNIT_ENERGY;
				else if ( arg.has("HANGERCOUNT", 10, 11) )
					ID = AID_MODIFY_UNIT_HANGER_COUNT;
				else if ( arg.has("HITPOINTS", 10, 9) )
					ID = AID_MODIFY_UNIT_HITPOINTS;
				else if ( arg.has("RESOURCEAMOUNT", 10, 14) )
					ID = AID_MODIFY_UNIT_RESOURCE_AMOUNT;
				else if ( arg.has("SHIELDPOINTS", 10, 12) )
					ID = AID_MODIFY_UNIT_SHIELD_POINTS;
			}
			else if ( arg.has("INIMAPPING", 1, 10) )
				ID = AID_MINIMAP_PING;
			else if ( arg.has("UTEUNITSPEECH", 1, 13) )
				ID = AID_MUTE_UNIT_SPEECH;
			break;

		case 'O':
			if ( arg.has("RDER", 1, 4) )
				ID = AID_ORDER;
			break;

		case 'P':
			if ( arg.has("RESERVETRIGGER", 1, 14) )
				ID = AID_PRESERVE_TRIGGER;
			else if ( arg.has("LAYWAV", 1, 6) )
				ID = AID_PLAY_WAV;
			else if ( arg.has("AUSE", 1, 4) )
			{
				if ( arg.has("GAME", 5, 4) )
					ID = AID_PAUSE_GAME;
				else if ( arg.has("TIMER", 5, 5) )
					ID = AID_PAUSE_TIMER;
			}
			break;

		case 'R':
			if ( arg.has("EMOVEUNIT", 1, 9) )
			{
				if ( arg.has("ATLOCATION", 10, 10) )
					ID = AID_REMOVE_UNIT_AT_LOCATION;
				else
					ID = AID_REMOVE_UNIT;
			}
			else if ( arg.has("UNAISCRIPT", 1, 10) )
			{
				if ( arg.has("ATLOCATION", 11, 10) )
					ID = AID_RUN_AI_SCRIPT_AT_LOCATION;
				else
					ID = AID_RUN_AI_SCRIPT;
			}
			break;

		case 'S':
			if ( arg.has("ET", 1, 2) )
			{
				if ( arg.has("DEATHS", 3, 6) )
					ID = AID_SET_DEATHS;
				else if ( arg.has("SWITCH", 3, 6) )
					ID = AID_SET_SWITCH;
				else if ( arg.has("RESOURCES", 3, 9) )
					ID = AID_SET_RESOURCES;
				else if ( arg.has("SCORE", 3, 5) )
					ID = AID_SET_SCORE;
				else if ( arg.has("ALLIANCESTATUS", 3, 14) )
					ID = AID_SET_ALLIANCE_STATUS;
				else if ( arg.has("COUNTDOWNTIMER", 3, 14) )
					ID = AID_SET_COUNTDOWN_TIMER;
				else if ( arg.has("DOODADSTATE", 3, 11) )
					ID = AID_SET_DOODAD_STATE;
				else if ( arg.has("INVINCIBILITY", 3, 13) )
					ID = AID_SET_INVINCIBILITY;
				else if ( arg.has("MISSIONOBJECTIVES", 3, 17) )
					ID = AID_SET_MISSION_OBJECTIVES;
				else if ( arg.has("NEXTSCENARIO", 3, 12) )
					ID  = AID_SET_NEXT_SCENARIO;
			}
			break;

		case 'T':
			if ( arg.has("ALKINGPORTRAIT", 1, 14) )
				ID = AID_TALKING_PORTRAIT;
			else if ( arg.has("RANSMISSION", 1, 11) )
				ID = AID_TRANSMISSION;
			break;

		case 'U':
			if ( arg.has("NPAUSE", 1, 6) )
			{
				if ( arg.has("TIMER", 7, 5) )
					ID = AID_UNPAUSE_TIMER;
				else if ( arg.has("GAME", 7, 4) )
					ID = AID_UNPAUSE_GAME;
			}
			else if ( arg.has("NMUTEUNITSPEECH", 1, 15) )
				ID = AID_UNMUTE_UNIT_SPEECH;
			break;

		case 'V':
			if ( arg.has("ICTORY", 1, 6) )
				ID = AID_VICTORY;
			break;

		case 'W':
			if ( arg.has("AIT", 1, 3) )
				ID = AID_WAIT;
			break;
	}

	const u8 actionNumArgs[] = { 0, 0, 0, 0, 1,  0, 0, 8, 2, 2,
								 1, 5, 1, 2, 2,  1, 2, 2, 3, 2,
								 2, 2, 2, 4, 2,  4, 4, 4, 1, 2,
								 0, 0, 1, 3, 4,  3, 3, 3, 4, 5,
								 1, 1, 4, 4, 4,  4, 5, 1, 5, 5,
								 5, 5, 4, 5, 0,  0, 0, 2, 0, 0 };

	u8 defaultFlags[] = { 0, 4, 4, 4, 4,		4, 4, 0, 4, 0,
						  4, 28, 0, 4, 4,		4, 4, 16, 16, 0,
						  16, 0, 20, 20, 20,	20, 4, 4, 4, 20,
						  4, 4, 4, 16, 16,		0, 16, 0, 20, 20,
						  4, 4, 20, 20, 20,		20, 20, 0, 20, 20,
						  20, 20, 4, 20, 4,		4, 4, 4, 0, 0 };

	if ( ID >= 0 && ID < sizeof(actionNumArgs)/sizeof(const u8) )
	{
		argsLeft = actionNumArgs[ID];
		flags = defaultFlags[ID];
	}
	else
	{
		argsLeft = ExtendedNumActionArgs(ID);
		flags = 0;
	}

	return ID != AID_NO_ACTION;
}

bool TextTrigCompiler::ParseConditionArg(buffer &text, Condition& currCondition, u32 pos, u32 end, u32 CID, u32 argsLeft, char *error)
{
	char* textPtr = (char*)text.getPtr(0);

	// Search for condition ID
	switch ( CID )
	{
		case CID_CUSTOM:
			switch ( argsLeft ) {
				case 9: goto ParseConditionLocationField	; break;
				case 8: goto ParseConditionPlayerField		; break;
				case 7: goto ParseConditionAmountField		; break;
				case 6: goto ParseConditionUnitField		; break;
				case 5: goto ParseConditionComparisonField  ; break;
				case 4: goto ParseConditionConditionField   ; break;
				case 3: goto ParseConditionTypeIndexField   ; break;
				case 2: goto ParseConditionFlagsField		; break;
				case 1: goto ParseConditionInternalDataField; break;
			}
			break;
		case CID_MEMORY: // deathTable+, mod, num
			switch ( argsLeft ) {
				case 3: goto ParseConditionDeathOffsetField		 ; break;
				case 2: goto ParseConditionNumericComparisonField; break;
				case 1: goto ParseConditionAmountField			 ; break;
			}
			break;
		case CID_ACCUMULATE: // player, mod, num, resouce
			switch ( argsLeft ) {
				case 4: goto ParseConditionPlayerField			 ; break;
				case 3: goto ParseConditionNumericComparisonField; break;
				case 2: goto ParseConditionAmountField			 ; break;
				case 1: goto ParseConditionResourceTypeField	 ; break;
			}
			break;
		case CID_BRING: // player, mod, num, unit, location
			switch ( argsLeft ) {
				case 5: goto ParseConditionPlayerField			 ; break;
				case 4: goto ParseConditionUnitField			 ; break;
				case 3: goto ParseConditionLocationField		 ; break;
				case 2: goto ParseConditionNumericComparisonField; break;
				case 1: goto ParseConditionAmountField			 ; break;
			}
			break;
		case CID_COMMAND: // Player, Unit, NumericComparison, Amount
		case CID_DEATHS:  // Player, Unit, NumericComparison, Amount
		case CID_KILL:	  // Player, Unit, NumericComparison, Amount
			switch ( argsLeft ) {
				case 4: goto ParseConditionPlayerField			 ; break;
				case 3: goto ParseConditionUnitField			 ; break;
				case 2: goto ParseConditionNumericComparisonField; break;
				case 1: goto ParseConditionAmountField			 ; break;
				
			}
			break;
		case CID_COMMAND_THE_LEAST: // unit
		case CID_COMMAND_THE_MOST:	// unit
		case CID_LEAST_KILLS:		// unit
		case CID_MOST_KILLS:		// unit
			if ( argsLeft == 1 ) goto ParseConditionUnitField;
			break;
		case CID_COMMAND_THE_LEAST_AT: // unit, location
		case CID_COMMAND_THE_MOST_AT:  // unit, location
			switch ( argsLeft ) {
				case 2: goto ParseConditionUnitField	; break;
				case 1: goto ParseConditionLocationField; break;
			}
			break;
		case CID_COUNTDOWN_TIMER: // NumericComparison, Amount
		case CID_ELAPSED_TIME: // NumericComparison, Amount
			switch ( argsLeft ) {
				case 2: goto ParseConditionNumericComparisonField; break;
				case 1: goto ParseConditionAmountField			 ; break;
			}
			break;
		case CID_HIGHEST_SCORE: // scoreType
		case CID_LOWEST_SCORE: // scoreType
			if ( argsLeft == 1 ) goto ParseConditionScoreTypeField;
			break;
		case CID_LEAST_RESOURCES: // resource
		case CID_MOST_RESOURCES: // resource
			if ( argsLeft == 1 ) goto ParseConditionResourceTypeField;
			break;
		case CID_OPPONENTS: // Player, NumericComparison, Amount
			switch ( argsLeft ) {
				case 3: goto ParseConditionPlayerField			 ; break;
				case 2: goto ParseConditionNumericComparisonField; break;
				case 1: goto ParseConditionAmountField			 ; break;
			}
			break;
		case CID_SCORE: // Player, ScoreType, NumericComparison, Amount
			switch ( argsLeft ) {
				case 4: goto ParseConditionPlayerField			 ; break;
				case 3: goto ParseConditionScoreTypeField		 ; break;
				case 2: goto ParseConditionNumericComparisonField; break;
				case 1: goto ParseConditionAmountField			 ; break;
			}
			break;
		case CID_SWITCH: // Switch, SwitchState
			switch ( argsLeft ) {
				case 2: goto ParseConditionSwitchField	   ; break;
				case 1: goto ParseConditionSwitchStateField; break;
			}
			break;
	}

	Error("INTERNAL ERROR: Invalid args left or argument unhandled, report this");
	return false;

// returns whether the condition was true and prints msg to the error message if false
#define returnMsg(condition, msg)					   \
	if ( condition )								   \
		return true;								   \
	else {											   \
		sprintf_s(LastError, MAX_ERROR_LENGTH, msg);   \
		return false;								   \
	}

ParseConditionLocationField: // 4 bytes
	returnMsg( ParseLocationName(text, currCondition.locationNum, pos, end) ||
			   ParseLong(textPtr, currCondition.locationNum, pos, end),
			   "Expected: Location name or 4-byte locationNum" );

ParseConditionPlayerField: // 4 bytes
	returnMsg( ParsePlayer(text, currCondition.players, pos, end) ||
			   ParseLong(textPtr, currCondition.players, pos, end),
			   "Expected: Player/group name or 4-byte id" );

ParseConditionAmountField: // 4 bytes
	returnMsg( ParseLong(textPtr, currCondition.amount, pos, end),
			   "Expected: 4-byte amount" );

ParseConditionUnitField: // 2 bytes
	returnMsg( ParseUnitName(text, currCondition.unitID, pos, end) ||
			   ParseShort(textPtr, currCondition.unitID, pos, end),
			   "Expected: Unit name or 2-byte unitID" );

ParseConditionNumericComparisonField: // 1 byte
	returnMsg( ParseNumericComparison(textPtr, currCondition.comparison, pos, end) ||
			   ParseByte(textPtr, currCondition.comparison, pos, end),
			   "Expected: Numeric comparison or 1-byte comparisonID" );

ParseConditionSwitchStateField: // 1 byte
	returnMsg( ParseSwitchState(textPtr, currCondition.comparison, pos, end) ||
			   ParseByte(textPtr, currCondition.comparison, pos, end),
			   "Expected: Switch state or 1-byte comparisonID" );

ParseConditionComparisonField: // 1 byte, comparison type or switch state
	returnMsg( ParseByte(textPtr, currCondition.comparison, pos, end) ||
			   ParseNumericComparison(textPtr, currCondition.comparison, pos, end) ||
			   ParseSwitchState(textPtr, currCondition.comparison, pos, end),
			   "Expected: 1-byte comparison" );

ParseConditionConditionField: // 1 byte, only used by custom
	returnMsg( ParseByte(textPtr, currCondition.condition, pos, end),
			   "Expected: 1-byte conditionID" );

ParseConditionResourceTypeField: // 1 byte
	returnMsg( ParseResourceType(textPtr, currCondition.typeIndex, pos, end) ||
			   ParseByte(textPtr, currCondition.typeIndex, pos, end),
			   "Expected: Resource type or 1-byte resourceID" );

ParseConditionScoreTypeField: // 1 byte
	returnMsg( ParseScoreType(textPtr, currCondition.typeIndex, pos, end) ||
			   ParseByte(textPtr, currCondition.typeIndex, pos, end),
			   "Expected: Score type or 1-byte scoreID" );

ParseConditionSwitchField: // 1 byte
	returnMsg( ParseSwitch(text, currCondition.typeIndex, pos, end) ||
			   ParseByte(textPtr, currCondition.typeIndex, pos, end),
			   "Expected: Switch name or 1-byte switchID" );

ParseConditionTypeIndexField: // 1 byte, resource type, score type, switch num
	returnMsg( ParseByte(textPtr, currCondition.typeIndex, pos, end) ||
			   ParseResourceType(textPtr, currCondition.typeIndex, pos, end) ||
			   ParseScoreType(textPtr, currCondition.typeIndex, pos, end) ||
			   ParseSwitch(text, currCondition.typeIndex, pos, end),
			   "Expected: 1-byte typeId, resource type, score type, or switch name" );

ParseConditionFlagsField: // 1 byte
	returnMsg( ParseByte(textPtr, currCondition.flags, pos, end),
			   "Expected: 1-byte flags" );

ParseConditionInternalDataField: // 2 bytes
	returnMsg( ParseShort(textPtr, currCondition.internalData, pos, end),
			   "Expected: 2-byte internal data" );

ParseConditionDeathOffsetField: // 4 bytes
	returnMsg( ParseLong(textPtr, currCondition.players, pos, end),
			   "Expected: 4-byte death table offset" );
}

bool TextTrigCompiler::ParseActionArg(buffer &text, Action& currAction, u32 pos, u32 end, u32 AID, u32 argsLeft, char *error)
{
	char* textPtr = (char*)text.getPtr(0);

	switch ( AID )
	{
		case AID_CUSTOM: // bytes: 4, 4, 4, 4, 4, 4, 2, 1, 1, 1, 3
			switch ( argsLeft ) {
				case 11: goto ParseActionLocationField	  ; break;
				case 10: goto ParseActionTextField		  ; break;
				case  9: goto ParseActionWavField		  ; break;
				case  8: goto ParseActionDurationField	  ; break;
				case  7: goto ParseActionFirstGroupField  ; break;
				case  6: goto ParseActionNumberField	  ; break;
				case  5: goto ParseActionTypeIndexField	  ; break;
				case  4: goto ParseActionActionField	  ; break;
				case  3: goto ParseActionTypeIndexField2  ; break;
				case  2: goto ParseActionFlagsField		  ; break;
				case  1: goto ParseActionInternalDataField; break;
			}
			break;
		case AID_MEMORY: // deathTable+, mod, num
			switch ( argsLeft ) {
				case 3: goto ParseActionDeathOffsetField	; break;
				case 2: goto ParseActionNumericModifierField; break;
				case 1: goto ParseActionAmountField			; break;
			}
			break;
		case AID_CENTER_VIEW:  // Location
		case AID_MINIMAP_PING: // Location
			if ( argsLeft == 1 ) goto ParseActionLocationField;
			break;
		case AID_COMMENT:				 // String
		case AID_SET_MISSION_OBJECTIVES: // String
		case AID_SET_NEXT_SCENARIO:		 // String
			if ( argsLeft == 1 ) goto ParseActionTextField;
			break;
		case AID_CREATE_UNIT:			// Player, Unit, NumUnits, Location
		case AID_KILL_UNIT_AT_LOCATION: // Player, Unit, NumUnits, Location
			switch ( argsLeft ) {
				case 4: goto ParseActionFirstGroupField; break;
				case 3: goto ParseActionUnitField; break;
				case 2: goto ParseActionNumUnitsField; break;
				case 1: goto ParseActionLocationField; break;
			}
			break;
		case AID_CREATE_UNIT_WITH_PROPERTIES: // Player, Unit, NumUnits, Location, Amount
			switch ( argsLeft ) {
				case 5: goto ParseActionFirstGroupField; break;
				case 4: goto ParseActionUnitField; break;
				case 3: goto ParseActionNumUnitsField; break;
				case 2: goto ParseActionLocationField; break;
				case 1: goto ParseActionAmountField; break;
			}
			break;
		case AID_DISPLAY_TEXT_MESSAGE: // TextFlags, String
			switch ( argsLeft ) {
				case 2: goto ParseActionTextFlagField; break;
				case 1: goto ParseActionTextField; break;
			}
			break;
		case AID_GIVE_UNITS_TO_PLAYER: // Player, SecondPlayer, Unit, NumUnits, Location
			switch ( argsLeft ) {
				case 5: goto ParseActionFirstGroupField; break;
				case 4: goto ParseActionSecondGroupField; break;
				case 3: goto ParseActionUnitField; break;
				case 2: goto ParseActionNumUnitsField; break;
				case 1: goto ParseActionLocationField; break;
			}
			break;
		case AID_KILL_UNIT:	  // Player, Unit
		case AID_REMOVE_UNIT: // Player, Unit
			switch ( argsLeft ) {
				case 2: goto ParseActionFirstGroupField; break;
				case 1: goto ParseActionUnitField; break;
			}
			break;
		case AID_LEADERBOARD_CONTROL_AT_LOCATION: // String, Unit, Location
			switch ( argsLeft ) {
				case 3: goto ParseActionTextField; break;
				case 2: goto ParseActionUnitField; break;
				case 1: goto ParseActionLocationField; break;
			}
			break;
		case AID_LEADERBOARD_CONTROL: // String, Unit
		case AID_LEADERBOARD_KILLS:	  // String, Unit
			switch ( argsLeft ) {
				case 2: goto ParseActionTextField; break;
				case 1: goto ParseActionUnitField; break;
			}
			break;
		case AID_LEADERBOARD_GREED: // Amount
			if ( argsLeft == 1 ) goto ParseActionAmountField;
			break;
		case AID_LEADERBOARD_POINTS: // String, ScoreType
			switch ( argsLeft ) {
				case 2: goto ParseActionTextField; break;
				case 1: goto ParseActionScoreTypeField; break;
			}
			break;
		case AID_LEADERBOARD_RESOURCES: // String, ResourceType
			switch ( argsLeft ) {
				case 2: goto ParseActionTextField; break;
				case 1: goto ParseActionResourceTypeField; break;
			}
			break;
		case AID_LEADERBOARD_COMPUTER_PLAYERS: // StateModifier
			if ( argsLeft == 1 ) goto ParseActionStateModifierField;
			break;
		case AID_LEADERBOARD_GOAL_CONTROL_AT_LOCATION: // String, Unit, Amount, Location
			switch ( argsLeft ) {
				case 4: goto ParseActionTextField	 ; break;
				case 3: goto ParseActionUnitField	 ; break;
				case 2: goto ParseActionAmountField	 ; break;
				case 1: goto ParseActionLocationField; break;
			}
			break;
		case AID_LEADERBOARD_GOAL_CONTROL: // String, Unit, Amount
		case AID_LEADERBOARD_GOAL_KILLS:   // String, Unit, Amount
			switch ( argsLeft ) {
				case 3: goto ParseActionTextField; break;
				case 2: goto ParseActionUnitField; break;
				case 1: goto ParseActionAmountField; break;
			}
			break;
		case AID_LEADERBOARD_GOAL_POINTS: // String, ScoreType, Amount
			switch ( argsLeft ) {
				case 3: goto ParseActionTextField	  ; break;
				case 2: goto ParseActionScoreTypeField; break;
				case 1: goto ParseActionAmountField	  ; break;
			}
			break;
		case AID_LEADERBOARD_GOAL_RESOURCES: // String, Amount, ResourceType
			switch ( argsLeft ) {
				case 3: goto ParseActionTextField; break;
				case 2: goto ParseActionAmountField; break;
				case 1: goto ParseActionResourceTypeField; break;
			}
			break;
		case AID_MODIFY_UNIT_ENERGY:		// Player, Unit, Amount, NumUnits, Location
		case AID_MODIFY_UNIT_HANGER_COUNT:	// Player, Unit, Amount, NumUnits, Location
		case AID_MODIFY_UNIT_HITPOINTS:		// Player, Unit, Amount, NumUnits, Location
		case AID_MODIFY_UNIT_SHIELD_POINTS:	// Player, Unit, Amount, NumUnits, Location
			switch ( argsLeft ) {
				case 5: goto ParseActionFirstGroupField; break;
				case 4: goto ParseActionUnitField; break;
				case 3: goto ParseActionAmountField; break;
				case 2: goto ParseActionNumUnitsField; break;
				case 1: goto ParseActionLocationField; break;
			}
			break;
		case AID_MODIFY_UNIT_RESOURCE_AMOUNT: // Player, Amount, NumUnits, Location
			switch ( argsLeft ) {
				case 4: goto ParseActionFirstGroupField; break;
				case 3: goto ParseActionAmountField; break;
				case 2: goto ParseActionNumUnitsField; break;
				case 1: goto ParseActionLocationField; break;
			}
			break;
		case AID_MOVE_LOCATION: // Player, Unit, LocDest, Location
			switch ( argsLeft ) {
				case 4: goto ParseActionFirstGroupField; break;
				case 3: goto ParseActionUnitField; break;
				case 2: goto ParseActionLocationField; break;
				case 1: goto ParseActionSecondLocationField; break;
			}
			break;
		case AID_MOVE_UNIT: // Player, Unit, NumUnits, Location, LocDest
			switch ( argsLeft ) {
				case 5: goto ParseActionFirstGroupField; break;
				case 4: goto ParseActionUnitField; break;
				case 3: goto ParseActionNumUnitsField; break;
				case 2: goto ParseActionLocationField; break;
				case 1: goto ParseActionSecondLocationField; break;
			}
			break;
		case AID_ORDER: // Player, Unit, Location, LocDest, OrderType
			switch ( argsLeft ) {
				case 5: goto ParseActionFirstGroupField; break;
				case 4: goto ParseActionUnitField; break;
				case 3: goto ParseActionLocationField; break;
				case 2: goto ParseActionSecondLocationField; break;
				case 1: goto ParseActionOrderField; break;
			}
			break;
		case AID_PLAY_WAV: // Wav, Duration
			switch ( argsLeft ) {
				case 2: goto ParseActionWavField; break;
				case 1: goto ParseActionDurationField; break;
			}
			break;
		case AID_REMOVE_UNIT_AT_LOCATION: // Player, Unit, NumUnits, Location
			switch ( argsLeft ) {
				case 4: goto ParseActionFirstGroupField; break;
				case 3: goto ParseActionUnitField; break;
				case 2: goto ParseActionNumUnitsField; break;
				case 1: goto ParseActionLocationField; break;
			}
			break;
		case AID_RUN_AI_SCRIPT: // Script
			if ( argsLeft == 1 ) goto ParseActionScriptField;
			break;
		case AID_RUN_AI_SCRIPT_AT_LOCATION: // Script, Location
			switch ( argsLeft ) {
				case 2: goto ParseActionScriptField	 ; break;
				case 1: goto ParseActionLocationField; break;
			}
			break;
		case AID_SET_ALLIANCE_STATUS: // Player, AllyState
			switch ( argsLeft ) {
				case 2: goto ParseActionFirstGroupField	   ; break;
				case 1: goto ParseActionAllianceStatusField; break;
			}
			break;
		case AID_SET_COUNTDOWN_TIMER: // NumericModifier, Duration
			switch ( argsLeft ) {
				case 2: goto ParseActionNumericModifierField; break;
				case 1: goto ParseActionDurationField; break;
			}
			break;
		case AID_SET_DEATHS: // Player, Unit, NumericModifier, Amount
			switch ( argsLeft ) {
				case 4: goto ParseActionFirstGroupField; break;
				case 3: goto ParseActionUnitField; break;
				case 2: goto ParseActionNumericModifierField; break;
				case 1: goto ParseActionAmountField; break;
			}
		case AID_SET_DOODAD_STATE:	// Player, Unit, Location, StateMod
		case AID_SET_INVINCIBILITY: // Player, Unit, Location, StateMod
			switch ( argsLeft ) {
				case 4: goto ParseActionFirstGroupField; break;
				case 3: goto ParseActionUnitField; break;
				case 2: goto ParseActionLocationField; break;
				case 1: goto ParseActionStateModifierField; break;
			}
			break;
		case AID_SET_RESOURCES: // Player, NumericModifier, Amount, ResourceType
			switch ( argsLeft ) {
				case 4: goto ParseActionFirstGroupField		; break;
				case 3: goto ParseActionNumericModifierField; break;
				case 2: goto ParseActionAmountField			; break;
				case 1: goto ParseActionResourceTypeField	; break;
			}
		case AID_SET_SCORE:	// Player, NumericModifier, Amount, ScoreType
			switch ( argsLeft ) {
				case 4: goto ParseActionFirstGroupField		; break;
				case 3: goto ParseActionNumericModifierField; break;
				case 2: goto ParseActionAmountField			; break;
				case 1: goto ParseActionScoreTypeField		; break;
			}
			break;
		case AID_SET_SWITCH: // Switch, SwitchMod
			switch ( argsLeft ) {
				case 2: goto ParseActionSwitchField; break;
				case 1: goto ParseActionSwitchModifierField; break;
			}
			break;
		case AID_TALKING_PORTRAIT: // Unit, Duration
			switch ( argsLeft ) {
				case 2: goto ParseActionUnitField	 ; break;
				case 1: goto ParseActionDurationField; break;
			}
			break;
		case AID_TRANSMISSION: // TextFlags, String, Unit, Location, NumericModifier, SecondAmount, Wav, Duration
			switch ( argsLeft ) {
				case 8: goto ParseActionTextFlagField; break;
				case 7: goto ParseActionTextField; break;
				case 6: goto ParseActionUnitField; break;
				case 5: goto ParseActionLocationField; break;
				case 4: goto ParseActionNumericModifierField; break;
				case 3: goto ParseActionAmountField; break;
				case 2: goto ParseActionWavField; break;
				case 1: goto ParseActionDurationField; break;
			}
			break;
		case AID_WAIT: // Duration
			if ( argsLeft == 1 ) goto ParseActionDurationField;
			break;
	}

ParseActionLocationField: // 4 bytes
	returnMsg( ParseLocationName(text, currAction.location, pos, end) ||
			   ParseLong(textPtr, currAction.location, pos, end),
			   "Expected: Location name or 4-byte locationNum" );

ParseActionTextField: // 4 bytes
	returnMsg( ParseString(text, currAction.stringNum, pos, end) ||
			   ParseLong(textPtr, currAction.stringNum, pos, end),
			   "Expected: String or stringNum" );

ParseActionWavField: // 4 bytes
	returnMsg( ParseWavName(text, currAction.wavID, pos, end) ||
			   ParseLong(textPtr, currAction.wavID, pos, end),
			   "Expected: Wav name or 4-byte wavID" );

ParseActionDurationField: // 4 bytes
	returnMsg( ParseLong(textPtr, currAction.time, pos, end),
			   "Expected: 4-byte duration" );

ParseActionFirstGroupField: // 4 bytes
	returnMsg( ParsePlayer(text, currAction.group, pos, end) ||
			   ParseLong(textPtr, currAction.group, pos, end),
			   "Expected: 4-byte groupID" );

ParseActionNumberField: // 4 bytes
	returnMsg( ParsePlayer(text, currAction.number, pos, end) ||
			   ParseLocationName(text, currAction.number, pos, end) ||
			   ParseScript(textPtr, currAction.number, pos, end) ||
			   ParseSwitch(text, currAction.number, pos, end) ||
			   ParseLong(textPtr, currAction.number, pos, end),
			   "Expected: Group, location, script, switch, or 4-byte number" );

ParseActionSecondGroupField: // 4 bytes
	returnMsg( ParsePlayer(text, currAction.number, pos, end) ||
			   ParseLong(textPtr, currAction.number, pos, end),
			   "Expected: Group name or 4-byte number" );

ParseActionSecondLocationField: // 4 bytes
	returnMsg( ParseLocationName(text, currAction.number, pos, end) ||
			   ParseLong(textPtr, currAction.number, pos, end),
			   "Expected: Location name or 4-byte number" );

ParseActionScriptField: // 4 bytes
	returnMsg ( ParseScript(textPtr, currAction.number, pos, end) ||
				ParseLong(textPtr, currAction.number, pos, end),
				"Expected: Script name or 4-byte script num" );

ParseActionSwitchField: // 4 bytes
	returnMsg ( ParseSwitch(text, currAction.number, pos, end) ||
				ParseLong(textPtr, currAction.number, pos, end),
				"Expected: Switch name or 4-byte number" );

ParseActionAmountField: // 4 bytes
	returnMsg( ParseLong(textPtr, currAction.number, pos, end),
			   "Expected: 4-byte number" );

ParseActionTypeIndexField: // 2 bytes
	returnMsg( ParseUnitName(text, currAction.type, pos, end) ||
			   ParseScoreType(textPtr, currAction.type, pos, end) ||
			   ParseResourceType(textPtr, currAction.type, pos, end) ||
			   ParseAllianceStatus(textPtr, currAction.type, pos, end) ||
			   ParseShort(textPtr, currAction.type, pos, end),
			   "Expected: Unit, score type, resource type, alliance status, or 2-byte typeID" );

ParseActionUnitField: // 2 bytes
	returnMsg( ParseUnitName(text, currAction.type, pos, end) ||
			   ParseShort(textPtr, currAction.type, pos, end),
			   "Expected: Unit name or 2-byte unitID" );

ParseActionScoreTypeField: // 2 bytes
	returnMsg( ParseScoreType(textPtr, currAction.type, pos, end) ||
			   ParseShort(textPtr, currAction.type, pos, end),
			   "Expected: Score type or 1-byte scoreID" );

ParseActionResourceTypeField:
	returnMsg( ParseResourceType(textPtr, currAction.type, pos, end) ||
			   ParseShort(textPtr, currAction.type, pos, end),
			   "Expected: Resource type or 2-byte number" );

ParseActionAllianceStatusField:
	returnMsg( ParseAllianceStatus(textPtr, currAction.type, pos, end) ||
			   ParseShort(textPtr, currAction.type, pos, end),
			   "Expected: Alliance status or 2-byte number" );

ParseActionActionField: // 1 byte, only used by custom
	returnMsg( ParseByte(textPtr, currAction.action, pos, end),
			   "Expected: 1-byte actionID" );

ParseActionTypeIndexField2: // 1 byte
	returnMsg( ParseSwitchMod(textPtr, currAction.type2, pos, end) ||
			   ParseOrder(textPtr, currAction.type2, pos, end) ||
			   ParseNumericModifier(textPtr, currAction.type2, pos, end) ||
			   ParseStateMod(textPtr, currAction.type2, pos, end) ||
			   ParseByte(textPtr, currAction.type2, pos, end),
			   "Expected: Switch modifier, order, numeric modifier, state modifier, or 1-byte number" );

ParseActionSwitchModifierField: // 1 byte
	returnMsg ( ParseSwitchMod(textPtr, currAction.type2, pos, end) ||
				ParseByte(textPtr, currAction.type2, pos, end),
				"Expected: Switch modifier or 1-byte number" );

ParseActionOrderField:
	returnMsg ( ParseOrder(textPtr, currAction.type2, pos, end) ||
				ParseByte(textPtr, currAction.type2, pos, end),
				"Expected: Order or 1-byte number" );

ParseActionNumericModifierField: // 1 byte
	returnMsg( ParseNumericModifier(textPtr, currAction.type2, pos, end) ||
			   ParseByte(textPtr, currAction.type2, pos, end),
			   "Expected: Numeric modifier or 1-byte number" );

ParseActionStateModifierField: // 1 byte
	returnMsg( ParseStateMod(textPtr, currAction.type2, pos, end) ||
			   ParseByte(textPtr, currAction.type2, pos, end),
			   "Expected: State modifier or 1-byte number" );

ParseActionNumUnitsField: // 1 byte
	returnMsg( ParseSpecialUnitAmount(textPtr, currAction.type2, pos, end) ||
			   ParseByte(textPtr, currAction.type2, pos, end),
			   "Expected: 1-byte number" );

ParseActionFlagsField: // 1 byte
	returnMsg( ParseTextDisplayFlag(textPtr, currAction.flags, pos, end) ||
			   ParseByte(textPtr, currAction.flags, pos, end),
			   "Expected: Always display text flags or 1-byte flag data" );

ParseActionTextFlagField: // 1 byte
	returnMsg( ParseTextDisplayFlag(textPtr, currAction.flags, pos, end) ||
			   ParseByte(textPtr, currAction.flags, pos, end),
			   "Expected: Always display text flags or 1-byte flag data" );

ParseActionInternalDataField: // 3 bytes
	returnMsg( ParseTriplet(textPtr, currAction.internalData, pos, end),
			   "Expected: 3-byte internal data" );

ParseActionDeathOffsetField: // 4 bytes
	returnMsg( ParseLong(textPtr, currAction.group, pos, end),
			   "Expected: 4-byte death table offset" );
}

bool TextTrigCompiler::ParseString(buffer &text, u32& dest, u32 pos, u32 end)
{
	bool isExtended = false; // Will need to do something with this for extended strings

	if ( text.get<u8>(pos) == '\"' )
	{
		pos ++;
		end --;
	}
	else if ( text.get<u8>(pos) == 'K' )
	{
		if ( text.get<u8>(pos+1) == '\"' )
		{
			pos += 2;
			end --;

			isExtended = true;
		}
		else
			return false;
	}
	else
		return false;

	int size = end-pos;

	if ( size < 0 )
		return false;

	char* stringPtr;
	bool success = false;

	if ( text.getPtr<char>(stringPtr, pos, size+1) )
	{
		char temp = stringPtr[size];
		stringPtr[size] = '\0';
		std::string str(stringPtr);
		stringPtr[size] = temp;

		u32 hash = strHash(str);
		int numMatching = stringTable.count(hash);
		if ( numMatching == 1 )
		{ // Should guarentee that you can find at least one entry
			StringTableNode &node = stringTable.find(hash)->second;
			if ( node.string.compare(str) == 0 && isExtended == node.isExtended )
			{
				dest = node.stringNum;
				success = true;
			}
		}
		else if ( numMatching > 1 )
		{
			auto range = stringTable.equal_range(hash);
			for ( auto it = range.first; it != range.second; it ++ )
			{
				StringTableNode &node = it->second;
				if ( node.string.compare(str) == 0 && isExtended == node.isExtended )
				{
					if ( success == false ) // If no matches have previously been found
					{
						dest = node.stringNum;
						success = true;
					}
					else // If matches have previously been found
					{
						if ( node.stringNum < dest )
						{ // Replace if stringNum < prevStringNum
							dest = 65536-node.stringNum;
							dest = node.stringNum;
						}
					}
				}
			}
		}

		if ( success == false ) // No string matches have been found
			// New string, try to add it to the map
		{
			StringTableNode node;
			node.string = str;
			node.isExtended = isExtended;

			if ( ( isExtended && extendedStrUsage.useNext(node.stringNum) ) ||
				 ( !isExtended && strUsage.useNext(node.stringNum) ) )
			{
				addedStrings.push_back(node); // Add to the addedStrings list so it can be added to the map after compiling
				stringTable.insert( pair<u32, StringTableNode>(strHash(node.string), node) ); // Add to search tree for recycling
				if ( isExtended )
					dest = 65536 - node.stringNum;
				else
					dest = node.stringNum;

				success = true;
			}
		}
	}
	return success;
}

bool TextTrigCompiler::ParseLocationName(buffer &text, u32 &dest, u32 pos, u32 end)
{
	int size = end-pos;
	if ( text.get<u8>(pos) == '\"' )
	{
		pos ++;
		end --;
		size -= 2;
	}

	if ( size < 1 )
		return false;

	char* locStringPtr;
	bool success = false;

	if ( text.getPtr<char>(locStringPtr, pos, size+1) )
	{ // At least one character must come after the location string
		// Temporarily replace next char with NUL char
		char temp = locStringPtr[size];
		locStringPtr[size] = '\0';
		std::string str(locStringPtr);

		// Grab the string hash
		u32 hash = strHash(str);
		int numMatching = locationTable.count(hash);
		if ( numMatching == 1 )
		{ // Should guarentee that you can find at least one entry
			LocationTableNode &node = locationTable.find(hash)->second;
			if ( node.locationName.compare(locStringPtr) == 0 )
			{
				dest = node.locationNum;
				success = true;
			}
		}
		else if ( numMatching > 1 )
		{
			auto range = locationTable.equal_range(hash);
			for ( auto it = range.first; it != range.second; it ++ )
			{
				LocationTableNode &node = it->second;
				if ( node.locationName.compare(locStringPtr) == 0 )
				{
					if ( success == false ) // If no matches have previously been found
					{
						dest = node.locationNum;
						success = true;
					}
					else // If matches have previously been found
					{
						if ( u32(node.locationNum) < dest )
							dest = node.locationNum; // Replace if locationNum < previous locationNum
					}
				}
			}
		}
		locStringPtr[size] = temp;
	}
	return success;
}

bool TextTrigCompiler::ParseUnitName(buffer &text, u16 &dest, u32 pos, u32 end)
{
	if ( text.get<u8>(pos) == '\"' ) // If quoted, ignore quotes
	{
		pos ++;
		end --;
	}
	
	int size = end-pos;
	char* unitNamePtr;
	char unit[40] = { };
	bool success = false;

	if ( text.getPtr<char>(unitNamePtr, pos, size+1) )
	{ // At least one character must come after the unit string
		// Temporarily replace next char with NUL char
		char temp = unitNamePtr[size],
			 currChar;
		unitNamePtr[size] = '\0';

		if ( size < 40 )
		{
			// Take an upper case copy of the name
			for ( int i=0; i<size; i++ )
			{
				currChar = unitNamePtr[i];
				if ( currChar > 96 && currChar < 123 )
					unit[i] = currChar - 32;
				else
					unit[i] = currChar;
			}
			unit[size] = '\0';
		
			switch ( unit[0] ) // First search standard unit names
			{
				case 'A':
					if		( strcmp(&unit[1], "LAN SCHEZAR (GOLIATH)"		   ) == 0 ) { dest =  17; success = true; }
					else if ( strcmp(&unit[1], "LAN SCHEZAR TURRET"			   ) == 0 ) { dest =  18; success = true; }
					else if ( strcmp(&unit[1], "LDARIS (TEMPLAR)"			   ) == 0 ) { dest =  87; success = true; }
					else if ( strcmp(&unit[1], "LEXEI STUKOV (GHOST)"		   ) == 0 ) { dest = 100; success = true; }
					else if ( strcmp(&unit[1], "NY UNIT"					   ) == 0 ) { dest = 229; success = true; }
					else if	( strcmp(&unit[1], "RCTURUS MENGSK (BATTLECRUISER)") == 0 ) { dest =  27; success = true; }
					else if ( strcmp(&unit[1], "RTANIS (SCOUT)"				   ) == 0 ) { dest =  88; success = true; }
					break;
				case 'B':
					if		( strcmp(&unit[1], "ENGALAAS (JUNGLE CRITTER)") == 0 ) { dest =	 90; success = true; }
					else if ( strcmp(&unit[1], "UILDINGS"				  ) == 0 ) { dest = 231; success = true; }
					break;
				case 'C':
					if		( strcmp(&unit[1], "ARGO SHIP (UNUSED)") == 0 ) { dest =  91; success = true; }
					else if ( strcmp(&unit[1], "ATINA (UNUSED)"	   ) == 0 ) { dest = 181; success = true; }
					else if ( strcmp(&unit[1], "AVE (UNUSED)"	   ) == 0 ) { dest = 179; success = true; }
					else if ( strcmp(&unit[1], "AVE-IN (UNUSED)"   ) == 0 ) { dest = 180; success = true; }
					break;
				case 'D':
					if		( strcmp(&unit[1], "ANIMOTH (ARBITER)"		) == 0 ) { dest =  86; success = true; }
					else if ( strcmp(&unit[1], "ARK SWARM"				) == 0 ) { dest = 202; success = true; }
					else if ( strcmp(&unit[1], "ATA DISC"				) == 0 ) { dest = 218; success = true; }
					else if ( strcmp(&unit[1], "EVOURING ONE (ZERGLING)") == 0 ) { dest =  54; success = true; }
					else if ( strcmp(&unit[1], "ISRUPTION WEB"			) == 0 ) { dest = 105; success = true; }
					break;
				case 'E':
					if		( strcmp(&unit[1], "DMUND DUKE (SIEGE MODE)"	   ) == 0 ) { dest = 25; success = true; }
					else if ( strcmp(&unit[1], "DMUND DUKE (TANK MODE)"		   ) == 0 ) { dest = 23; success = true; }
					else if ( strcmp(&unit[1], "DMUND DUKE TURRET (SIEGE MODE)") == 0 ) { dest = 26; success = true; }
					else if ( strcmp(&unit[1], "DMUND DUKE TURRET (TANK MODE)" ) == 0 ) { dest = 24; success = true; }
					break;
				case 'F':
					if		( strcmp(&unit[1], "ACTORIES"			) == 0 ) { dest = 232; success = true; }
					else if	( strcmp(&unit[1], "ENIX (DRAGOON)"		) == 0 ) { dest =  78; success = true; }
					else if ( strcmp(&unit[1], "ENIX (ZEALOT)"		) == 0 ) { dest =  77; success = true; }
					else if ( strcmp(&unit[1], "LAG"				) == 0 ) { dest = 215; success = true; }
					else if ( strcmp(&unit[1], "LOOR GUN TRAP"		) == 0 ) { dest = 209; success = true; }
					else if ( strcmp(&unit[1], "LOOR HATCH (UNUSED)") == 0 ) { dest = 204; success = true; }
					else if ( strcmp(&unit[1], "LOOR MISSILE TRAP"	) == 0 ) { dest = 203; success = true; }
					break;
				case 'G':
					if		( strcmp(&unit[1], "ANTRITHOR (CARRIER)"		  ) == 0 ) { dest =  82; success = true; }
					else if ( strcmp(&unit[1], "ERARD DUGALLE (BATTLECRUISER)") == 0 ) { dest = 102; success = true; }
					else if ( strcmp(&unit[1], "OLIATH TURRET"				  ) == 0 ) { dest =   4; success = true; }
					else if ( strcmp(&unit[1], "UI MONTAG (FIREBAT)"		  ) == 0 ) { dest =  10; success = true; }
					break;
				case 'H':
					if		( strcmp(&unit[1], "UNTER KILLER (HYDRALISK)") == 0 ) { dest = 53; success = true; }
					else if ( strcmp(&unit[1], "YPERION (BATTLECRUISER)" ) == 0 ) { dest = 28; success = true; }
					break;
				case 'I':
					if ( unit[1] == 'D' && unit[2] == ':' )
						success = ParseShort(unit, dest, 3, size);
					else if ( strcmp(&unit[1], "NDEPENDENT COMMAND CENTER (UNUSED)"	 ) == 0 ) { dest = 183; success = true; }
					else if ( strcmp(&unit[1], "NDEPENDENT JUMP GATE (UNUSED)"		 ) == 0 ) { dest = 185; success = true; }
					else if ( strcmp(&unit[1], "NDEPENDENT STARPORT (UNUSED)"		 ) == 0 ) { dest = 184; success = true; }
					else if ( strcmp(&unit[1], "NFESTED COMMAND CENTER"				 ) == 0 ) { dest = 130; success = true; }
					else if ( strcmp(&unit[1], "NFESTED DURAN (INFESTED TERRAN)"	 ) == 0 ) { dest = 104; success = true; }
					else if ( strcmp(&unit[1], "NFESTED KERRIGAN (INFESTED TERRAIN)" ) == 0 ) { dest =  51; success = true; }
					else if ( strcmp(&unit[1], "NFESTED TERRAN"						 ) == 0 ) { dest =  50; success = true; }
					else if ( strcmp(&unit[1], "ON CANNON"							 ) == 0 ) { dest = 127; success = true; }
					break;
				case 'J':
					if		( strcmp(&unit[1], "IM RAYNOR (MARINE)"	) == 0 ) { dest = 20; success = true; }
					else if ( strcmp(&unit[1], "IM RAYNOR (VULTURE)") == 0 ) { dest = 19; success = true; }
					break;
				case 'K':
					if		( strcmp(&unit[1], "AKARU (TWILIGHT CRITTER)"		   ) == 0 ) { dest =  94; success = true; }
					else if ( strcmp(&unit[1], "HADARIN CRYSTAL FORMATION (UNUSED)") == 0 ) { dest = 187; success = true; }
					else if ( strcmp(&unit[1], "HALIS CRYSTAL"					   ) == 0 ) { dest = 129; success = true; }
					else if ( strcmp(&unit[1], "HAYDARIN CRYSTAL"				   ) == 0 ) { dest = 219; success = true; }
					else if ( strcmp(&unit[1], "HAYDARIN CRYSTAL FORMATION"		   ) == 0 ) { dest = 173; success = true; }
					else if ( strcmp(&unit[1], "UKULZA (GUARDIAN)"				   ) == 0 ) { dest =  56; success = true; }
					else if ( strcmp(&unit[1], "UKULZA (MUTALISK)"				   ) == 0 ) { dest =  55; success = true; }
					break;
				case 'L':
					if		( strcmp(&unit[1], "EFT PIT DOOR"		  ) == 0 ) { dest = 207; success = true; }
					else if ( strcmp(&unit[1], "EFT UPPER LEVEL DOOR" ) == 0 ) { dest = 205; success = true; }
					else if ( strcmp(&unit[1], "EFT WALL FLAME TRAP"  ) == 0 ) { dest = 211; success = true; }
					else if ( strcmp(&unit[1], "EFT WALL MISSILE TRAP") == 0 ) { dest = 210; success = true; }
					else if ( strcmp(&unit[1], "URKER EGG"			  ) == 0 ) { dest =	 97; success = true; }
					break;
				case 'M':
					if		( strcmp(&unit[1], "AGELLAN (SCIENCE VESSEL)" ) == 0 ) { dest =  22; success = true; }
					else if ( strcmp(&unit[1], "AP REVEALER"			  ) == 0 ) { dest = 101; success = true; }
					else if ( strcmp(&unit[1], "ATRIARCH (QUEEN)"		  ) == 0 ) { dest =  49; success = true; }
					else if ( strcmp(&unit[1], "ATURE CRYSALIS"			  ) == 0 ) { dest = 150; success = true; }
					else if ( strcmp(&unit[1], "EN"						  ) == 0 ) { dest = 230; success = true; }
					else if ( strcmp(&unit[1], "ERCENARY GUNSHIP (UNUSED)") == 0 ) { dest =  92; success = true; }
					else if ( strcmp(&unit[1], "INERAL CLUSTER TYPE 1"	  ) == 0 ) { dest = 220; success = true; }
					else if ( strcmp(&unit[1], "INERAL CLUSTER TYPE 2"	  ) == 0 ) { dest = 221; success = true; }
					else if ( strcmp(&unit[1], "INERAL FIELD (TYPE 1)"	  ) == 0 ) { dest = 176; success = true; }
					else if ( strcmp(&unit[1], "INERAL FIELD (TYPE 2)"	  ) == 0 ) { dest = 177; success = true; }
					else if ( strcmp(&unit[1], "INERAL FIELD (TYPE 3)"	  ) == 0 ) { dest = 178; success = true; }
					else if ( strcmp(&unit[1], "INING PLATFORM (UNUSED)"  ) == 0 ) { dest = 182; success = true; }
					else if ( strcmp(&unit[1], "OJO (SCOUT)"			  ) == 0 ) { dest =  80; success = true; }
					else if ( strcmp(&unit[1], "UTALISK COCOON"			  ) == 0 ) { dest =  50; success = true; }
					break;
				case 'N':
					if		( strcmp(&unit[1], "ORAD II (BATTLECRUISER)") == 0 ) { dest =  29; success = true; }
					else if ( strcmp(&unit[1], "ORAD II (CRASHED)"		) == 0 ) { dest = 126; success = true; }
					else if ( strcmp(&unit[1], "UCLEAR MISSILE"			) == 0 ) { dest =  14; success = true; }
					break;
				case 'O':
					if		( strcmp(&unit[1], "VERMIND COCOON") == 0 ) { dest = 201; success = true; }
					break;
				case 'P':
					if ( unit[1] == 'R' )
					{
						switch ( unit[8] )
						{
							case 'A':
								if		( strcmp(&unit[2], "OTOSS ARBITER"		   ) == 0 )	{ dest =  71; success = true; }
								else if ( strcmp(&unit[2], "OTOSS ARBITER TRIBUNAL") == 0 ) { dest = 170; success = true; }
								else if ( strcmp(&unit[2], "OTOSS ARCHON"		   ) == 0 ) { dest =  68; success = true; }
								else if ( strcmp(&unit[2], "OTOSS ASSIMILATOR"	   ) == 0 ) { dest = 157; success = true; }
								break;
							case 'B':
								if ( strcmp(&unit[2], "OTOSS BEACON") == 0 ) { dest = 196; success = true; }
								break;
							case 'C':
								if		( strcmp(&unit[2], "OTOSS CARRIER"		   ) == 0 ) { dest =  72; success = true; }
								else if ( strcmp(&unit[2], "OTOSS CITADEL OF ADUN" ) == 0 ) { dest = 163; success = true; }
								else if ( strcmp(&unit[2], "OTOSS CORSAIR"		   ) == 0 ) { dest =  60; success = true; }
								else if ( strcmp(&unit[2], "OTOSS CYBERNETICS CORE") == 0 ) { dest = 164; success = true; }
								break;
							case 'D':
								if		( strcmp(&unit[2], "OTOSS DARK ARCHON"		  ) == 0 ) { dest =  63; success = true; }
								else if ( strcmp(&unit[2], "OTOSS DARK TEMPLAR (HERO)") == 0 ) { dest =  74; success = true; }
								else if ( strcmp(&unit[2], "OTOSS DARK TEMPLAR (UNIT)") == 0 ) { dest =  61; success = true; }
								else if ( strcmp(&unit[2], "OTOSS DRAGOON"			  ) == 0 ) { dest =  66; success = true; }
								break;
							case 'F':
								if		( strcmp(&unit[2], "OTOSS FLAG BEACON" ) == 0 ) { dest = 199; success = true; }
								else if ( strcmp(&unit[2], "OTOSS FLEET BEACON") == 0 ) { dest = 169; success = true; }
								else if ( strcmp(&unit[2], "OTOSS FORGE"	   ) == 0 ) { dest = 166; success = true; }
								break;
							case 'G':
								if ( strcmp(&unit[2], "OTOSS GATEWAY") == 0 ) { dest = 160; success = true; }
								break;
							case 'H':
								if ( strcmp(&unit[2], "OTOSS HIGH TEMPLAR") == 0 ) { dest =  67; success = true; }
								break;
							case 'I':
								if ( strcmp(&unit[2], "OTOSS INTERCEPTOR") == 0 ) { dest =  73; success = true; }
								break;
							case 'M':
								if ( strcmp(&unit[2], "OTOSS MARKER") == 0 ) { dest = 193; success = true; }
								break;
							case 'N':
								if ( strcmp(&unit[2], "OTOSS NEXUS") == 0 ) { dest = 154; success = true; }
								break;
							case 'O':
								if		( strcmp(&unit[2], "OTOSS OBSERVATORY") == 0 ) { dest = 159; success = true; }
								else if ( strcmp(&unit[2], "OTOSS OBSERVER"	  ) == 0 ) { dest =  84; success = true; }
								break;
							case 'P':
								if		( strcmp(&unit[2], "OTOSS PHOTON CANNON") == 0 ) { dest = 162; success = true; }
								else if ( strcmp(&unit[2], "OTOSS PROBE"		) == 0 ) { dest =  64; success = true; }
								else if ( strcmp(&unit[2], "OTOSS PYLON"		) == 0 ) { dest = 156; success = true; }		
								break;
							case 'R':
								if		( strcmp(&unit[2], "OTOSS REAVER"			   ) == 0 ) { dest =  83; success = true; }
								else if ( strcmp(&unit[2], "OTOSS ROBOTICS FACILITY"   ) == 0 ) { dest = 155; success = true; }
								else if ( strcmp(&unit[2], "OTOSS ROBOTICS SUPPORT BAY") == 0 ) { dest = 171; success = true; }
								break;
							case 'S':
								if		( strcmp(&unit[2], "OTOSS SCARAB"		 ) == 0 ) { dest =  85; success = true; }
								else if ( strcmp(&unit[2], "OTOSS SCOUT"		 ) == 0 ) { dest =  70; success = true; }
								else if ( strcmp(&unit[2], "OTOSS SHIELD BATTERY") == 0 ) { dest = 172; success = true; }
								else if ( strcmp(&unit[2], "OTOSS SHUTTLE"		 ) == 0 ) { dest =  69; success = true; }
								else if ( strcmp(&unit[2], "OTOSS STARGATE"		 ) == 0 ) { dest = 167; success = true; }
								break;
							case 'T':
								if		( strcmp(&unit[2], "OTOSS TEMPLAR ARCHIVES") == 0 ) { dest = 165; success = true; }
								else if ( strcmp(&unit[2], "OTOSS TEMPLE"		   ) == 0 ) { dest = 174; success = true; }
								break;
							case 'V':
								if		( strcmp(&unit[2], "OTOSS VESPENE GAS ORB TYPE 1") == 0 ) { dest = 222; success = true; }
								else if ( strcmp(&unit[2], "OTOSS VESPENE GAS ORB TYPE 2") == 0 ) { dest = 223; success = true; }		
								break;
							case 'Z':
								if ( strcmp(&unit[2], "OTOSS ZEALOT") == 0 ) { dest =  65; success = true; }
								break;
						}
					}
					else if ( unit[1] == 'S' )
					{
						if		( strcmp(&unit[2], "I DISRUPTER") == 0 ) { dest = 190; success = true; }
						else if ( strcmp(&unit[2], "I EMITTER"	) == 0 ) { dest = 217; success = true; }
					}
					else if ( strcmp(&unit[1], "OWER GENERATOR") == 0 ) { dest = 200; success = true; }			
					break;
				case 'R':
					if		( strcmp(&unit[1], "AGNASAUR (ASHWORLD CRITTER)") == 0 ) { dest =  95; success = true; }
					else if ( strcmp(&unit[1], "ASZAGAL (CORSAIR)"			) == 0 ) { dest =  98; success = true; }
					else if ( strcmp(&unit[1], "EPAIR BAY (UNUSED)"			) == 0 ) { dest = 121; success = true; }
					else if ( strcmp(&unit[1], "HYNADON (BADLANDS CRITTER)"	) == 0 ) { dest =  89; success = true; }
					else if ( strcmp(&unit[1], "IGHT PIT DOOR"				) == 0 ) { dest = 208; success = true; }
					else if ( strcmp(&unit[1], "IGHT UPPER LEVEL DOOR"		) == 0 ) { dest = 206; success = true; }
					else if ( strcmp(&unit[1], "IGHT WALL FLAME TRAP"		) == 0 ) { dest = 213; success = true; }
					else if ( strcmp(&unit[1], "IGHT WALL MISSILE TRAP"		) == 0 ) { dest = 212; success = true; }
					else if ( strcmp(&unit[1], "UINS (UNUSED)"				) == 0 ) { dest = 186; success = true; }
					break;
				case 'S':
					if		( strcmp(&unit[1], "AMIR DURAN (GHOST)"			  ) == 0 ) { dest =  99; success = true; }
					else if ( strcmp(&unit[1], "ARAH KERRIGAN (GHOST)"		  ) == 0 ) { dest =  16; success = true; }
					else if ( strcmp(&unit[1], "CANNER SWEEP"				  ) == 0 ) { dest =  33; success = true; }
					else if ( strcmp(&unit[1], "CANTID (DESERT CRITTER)"	  ) == 0 ) { dest =  93; success = true; }
					else if ( strcmp(&unit[1], "IEGE TANK TURRET (SIEGE MODE)") == 0 ) { dest =  31; success = true; }
					else if ( strcmp(&unit[1], "IEGE TANK TURRET (TANK MODE)" ) == 0 ) { dest =	  6; success = true; }
					else if ( strcmp(&unit[1], "PIDER MINE"					  ) == 0 ) { dest =  13; success = true; }
					else if ( strcmp(&unit[1], "TARBASE (UNUSED)"			  ) == 0 ) { dest = 119; success = true; }
					else if ( strcmp(&unit[1], "TART LOCATION"				  ) == 0 ) { dest = 214; success = true; }
					else if ( strcmp(&unit[1], "TASIS CELL/PRISON"			  ) == 0 ) { dest = 168; success = true; }
					break;
				case 'T':
					if ( unit[1] == 'E' )
					{
						switch ( unit[7] )
						{
							case 'A':
								if		( strcmp(&unit[2], "RRAN ACADEMY") == 0 ) { dest = 112; success = true; }
								else if ( strcmp(&unit[2], "RRAN ARMORY" ) == 0 ) { dest = 123; success = true; }
								break;
							case 'B':
								if		( strcmp(&unit[2], "RRAN BARRACKS"	   ) == 0 ) { dest = 111; success = true; }
								else if ( strcmp(&unit[2], "RRAN BATTLECRUISER") == 0 ) { dest =  12; success = true; }
								else if ( strcmp(&unit[2], "RRAN BEACON"	   ) == 0 ) { dest = 195; success = true; }
								else if ( strcmp(&unit[2], "RRAN BUNKER"	   ) == 0 ) { dest = 125; success = true; }
								break;
							case 'C':
								if		( strcmp(&unit[2], "RRAN CIVILIAN"		) == 0 ) { dest =  15; success = true; }
								else if ( strcmp(&unit[2], "RRAN COMMAND CENTER") == 0 ) { dest = 106; success = true; }
								else if ( strcmp(&unit[2], "RRAN COMSAT STATION") == 0 ) { dest = 107; success = true; }
								else if ( strcmp(&unit[2], "RRAN CONTROL TOWER"	) == 0 ) { dest = 115; success = true; }
								else if ( strcmp(&unit[2], "RRAN COVERT OPS"	) == 0 ) { dest = 117; success = true; }
								break;
							case 'D':
								if ( strcmp(&unit[2], "RRAN DROPSHIP") == 0 ) { dest =  11; success = true; }
								break;
							case 'E':
								if ( strcmp(&unit[2], "RRAN ENGINEERING BAY") == 0 ) { dest = 122; success = true; }
								break;
							case 'F':
								if		( strcmp(&unit[2], "RRAN FACTORY"	 ) == 0 ) { dest = 113; success = true; }
								else if ( strcmp(&unit[2], "RRAN FIREBAT"	 ) == 0 ) { dest =  32; success = true; }
								else if ( strcmp(&unit[2], "RRAN FLAG BEACON") == 0 ) { dest = 198; success = true; }
								break;
							case 'G':
								if		( strcmp(&unit[2], "RRAN GHOST"	 ) == 0 ) { dest =   1; success = true; }
								else if ( strcmp(&unit[2], "RRAN GOLIATH") == 0 ) { dest =   3; success = true; }
								break;
							case 'M':
								if		( strcmp(&unit[2], "RRAN MACHINE SHOP"	) == 0 ) { dest = 120; success = true; }
								else if ( strcmp(&unit[2], "RRAN MARINE"		) == 0 ) { dest =   0; success = true; }
								else if ( strcmp(&unit[2], "RRAN MARKER"		) == 0 ) { dest = 192; success = true; }
								else if ( strcmp(&unit[2], "RRAN MEDIC"			) == 0 ) { dest =  34; success = true; }
								else if ( strcmp(&unit[2], "RRAN MISSILE TURRET") == 0 ) { dest = 124; success = true; }
								break;
							case 'N':
								if ( strcmp(&unit[2], "RRAN NUCLEAR SILO") == 0 ) { dest = 108; success = true; }
								break;
							case 'P':
								if ( strcmp(&unit[2], "RRAN PHYSICS LAB") == 0 ) { dest = 118; success = true; }
								break;
							case 'R':
								if ( strcmp(&unit[2], "RRAN REFINERY") == 0 ) { dest = 110; success = true; }
								break;
							case 'S':
								if		( strcmp(&unit[2], "RRAN SCIENCE FACILITY"		 ) == 0 ) { dest = 116; success = true; }
								else if ( strcmp(&unit[2], "RRAN SCIENCE VESSEL"		 ) == 0 ) { dest =   9; success = true; }
								else if ( strcmp(&unit[2], "RRAN SCV"					 ) == 0 ) { dest =   7; success = true; }
								else if ( strcmp(&unit[2], "RRAN SIEGE TANK (SIEGE MODE)") == 0 ) { dest =  30; success = true; }
								else if ( strcmp(&unit[2], "RRAN SIEGE TANK (TANK MODE)" ) == 0 ) { dest =   5; success = true; }
								else if ( strcmp(&unit[2], "RRAN STARPORT"				 ) == 0 ) { dest = 114; success = true; }
								else if ( strcmp(&unit[2], "RRAN SUPPLY DEPOT"			 ) == 0 ) { dest = 109; success = true; }
								break;
							case 'V':
								if		( strcmp(&unit[2], "RRAN VALKYRIE"				 ) == 0 ) { dest =  58; success = true; }
								else if ( strcmp(&unit[2], "RRAN VESPENE GAS TANK TYPE 1") == 0 ) { dest = 226; success = true; }
								else if ( strcmp(&unit[2], "RRAN VESPENE GAS TANK TYPE 2") == 0 ) { dest = 227; success = true; }
								else if ( strcmp(&unit[2], "RRAN VULTURE"				 ) == 0 ) { dest =   2; success = true; }
								break;
							case 'W':
								if ( strcmp(&unit[2], "RRAN WRAITH") == 0 ) { dest =   8; success = true; }
								break;
						}
					}
					else if ( unit[1] == 'A' )
					{
						if		( strcmp(&unit[2], "SSADAR (TEMPLAR)"		) == 0 ) { dest =  79; success = true; }
						else if ( strcmp(&unit[2], "SSADAR/ZERATUL (ARCHON)") == 0 ) { dest =  76; success = true; }
					}
					else if ( unit[1] == 'O' )
					{
						if		( strcmp(&unit[2], "M KAZANSKY (WRAITH)") == 0 ) { dest =  21; success = true; }
						else if ( strcmp(&unit[2], "RRASQUE (ULTRALISK)") == 0 ) { dest =  48; success = true; }
					}
					break;
				case 'U':
					if		( strcmp(&unit[1], "NCLEAN ONE (DEFILER)"	   ) == 0 ) { dest =  52; success = true; }
					else if ( strcmp(&unit[1], "NUSED PROTOSS BUILDING 1"  ) == 0 ) { dest = 158; success = true; }
					else if ( strcmp(&unit[1], "NUSED PROTOSS BUILDING 2"  ) == 0 ) { dest = 161; success = true; }
					else if ( strcmp(&unit[1], "NUSED ZERG BUILDING 1"	   ) == 0 ) { dest = 145; success = true; }
					else if ( strcmp(&unit[1], "NUSED ZERG BUILDING 2"	   ) == 0 ) { dest = 153; success = true; }
					else if ( strcmp(&unit[1], "RAJ CRYSTAL"			   ) == 0 ) { dest = 128; success = true; }
					else if ( strcmp(&unit[1], "RSADON (ICE WORLD CRITTER)") == 0 ) { dest =  96; success = true; }
					break;
				case 'V':
					if		( strcmp(&unit[1], "ESPENE GEYSER") == 0 ) { dest = 188; success = true; }
					break;
				case 'W':
					if		( strcmp(&unit[1], "ARBRINGER (REAVER)") == 0 ) { dest =  81; success = true; }
					else if ( strcmp(&unit[1], "ARP GATE"		   ) == 0 ) { dest = 189; success = true; }
					break;
				case 'X':
					if		( strcmp(&unit[1], "EL'NAGA TEMPLE") == 0 ) { dest = 175; success = true; }
					break;
				case 'Y':
					if		( strcmp(&unit[1], "GGDRASILL (OVERLORD)") == 0 ) { dest =  57; success = true; }
					else if ( strcmp(&unit[1], "OUNG CHRYSALIS"		 ) == 0 ) { dest = 216; success = true; }
					break;
				case 'Z':
					if ( unit[3] == 'G' )
					{
						switch ( unit[5] )
						{
							case 'B':
								if		( strcmp(&unit[1], "ERG BEACON"	  ) == 0 ) { dest = 194; success = true; }
								else if ( strcmp(&unit[1], "ERG BROODLING") == 0 ) { dest =  40; success = true; }
								break;
							case 'C':
								if		( strcmp(&unit[1], "ERG CEREBRATE"		  ) == 0 ) { dest = 151; success = true; }
								else if ( strcmp(&unit[1], "ERG CEREBRATE DAGGOTH") == 0 ) { dest = 152; success = true; }
								else if ( strcmp(&unit[1], "ERG CREEP COLONY"	  ) == 0 ) { dest = 143; success = true; }
								break;
							case 'D':
								if		( strcmp(&unit[1], "ERG DEFILER"	  ) == 0 ) { dest =  46; success = true; }
								else if ( strcmp(&unit[1], "ERG DEFILER MOUND") == 0 ) { dest = 136; success = true; }
								else if ( strcmp(&unit[1], "ERG DEVOURER"	  ) == 0 ) { dest =  62; success = true; }
								else if ( strcmp(&unit[1], "ERG DRONE"		  ) == 0 ) { dest =  41; success = true; }
								break;
							case 'E':
								if		( strcmp(&unit[1], "ERG EGG"			  ) == 0 ) { dest =  36; success = true; }
								else if ( strcmp(&unit[1], "ERG EVOLUTION CHAMBER") == 0 ) { dest = 139; success = true; }
								else if ( strcmp(&unit[1], "ERG EXTRACTOR"		  ) == 0 ) { dest = 149; success = true; }
								break;
							case 'F':
								if ( strcmp(&unit[1], "ERG FLAG BEACON") == 0 ) { dest = 197; success = true; }
								break;
							case 'G':
								if		( strcmp(&unit[1], "ERG GREATER SPIRE") == 0 ) { dest = 137; success = true; }
								else if ( strcmp(&unit[1], "ERG GUARDIAN"	  ) == 0 ) { dest =  44; success = true; }
								break;
							case 'H':
								if		( strcmp(&unit[1], "ERG HATCHERY"	  ) == 0 ) { dest = 131; success = true; }
								else if ( strcmp(&unit[1], "ERG HIVE"		  ) == 0 ) { dest = 133; success = true; }
								else if ( strcmp(&unit[1], "ERG HYDRALISK"	  ) == 0 ) { dest =  38; success = true; }
								else if ( strcmp(&unit[1], "ERG HYDRALISK DEN") == 0 ) { dest = 135; success = true; }
								break;
							case 'L':
								if		( strcmp(&unit[1], "ERG LAIR"  ) == 0 ) { dest = 132; success = true; }
								else if ( strcmp(&unit[1], "ERG LARVA" ) == 0 ) { dest =  35; success = true; }
								else if ( strcmp(&unit[1], "ERG LURKER") == 0 ) { dest = 103; success = true; }
								break;
							case 'M':
								if		( strcmp(&unit[1], "ERG MARKER"	 ) == 0 ) { dest = 191; success = true; }
								else if ( strcmp(&unit[1], "ERG MUTALISK") == 0 ) { dest =  43; success = true; }
								break;
							case 'N':
								if ( strcmp(&unit[1], "ERG NYDUS CANAL") == 0 ) { dest = 134; success = true; }
								break;
							case 'O':
								if		( strcmp(&unit[1], "ERG OVERLORD"			  ) == 0 ) { dest =  42; success = true; }
								else if ( strcmp(&unit[1], "ERG OVERMIND"			  ) == 0 ) { dest = 148; success = true; }
								else if ( strcmp(&unit[1], "ERG OVERMIND (WITH SHELL)") == 0 ) { dest = 147; success = true; }
								break;
							case 'Q':
								if		( strcmp(&unit[1], "ERG QUEEN"		 ) == 0 ) { dest =  45; success = true; }
								else if ( strcmp(&unit[1], "ERG QUEEN'S NEST") == 0 ) { dest = 138; success = true; }
								break;
							case 'S':
								if		( strcmp(&unit[1], "ERG SCOURGE"	  ) == 0 ) { dest =  47; success = true; }
								else if ( strcmp(&unit[1], "ERG SPAWNING POOL") == 0 ) { dest = 142; success = true; }
								else if ( strcmp(&unit[1], "ERG SPIRE"		  ) == 0 ) { dest = 141; success = true; }
								else if ( strcmp(&unit[1], "ERG SPORE COLONY" ) == 0 ) { dest = 144; success = true; }
								else if ( strcmp(&unit[1], "ERG SUNKEN COLONY") == 0 ) { dest = 146; success = true; }
								break;
							case 'U':
								if		( strcmp(&unit[1], "ERG ULTRALISK"		 ) == 0 ) { dest =  39; success = true; }
								else if ( strcmp(&unit[1], "ERG ULTRALISK CAVERN") == 0 ) { dest = 140; success = true; }
								break;
							case 'V':
								if		( strcmp(&unit[1], "ERG VESPENE GAS SAC TYPE 1") == 0 ) { dest = 224; success = true; }
								else if ( strcmp(&unit[1], "ERG VESPENE GAS SAC TYPE 2") == 0 ) { dest = 225; success = true; }
								break;
							case 'Z':
								if ( strcmp(&unit[1], "ERG ZERGLING") == 0 ) { dest =  37; success = true; }
								break;
						}
					}
					else if ( strcmp(&unit[1], "ERATUL (DARK TEMPLAR)") == 0 ) { dest = 75; success = true; }
					break;
			}
		}

		if ( !success ) // Now search the unit name table
		{
			std::string str(unitNamePtr);
			u32 hash = strHash(str);
			int numMatching = unitTable.count(hash);
			if ( numMatching == 1 )
			{ // Should guarentee that you can find at least one entry
				UnitTableNode &node = unitTable.find(hash)->second;
				if ( node.unitName.compare(unitNamePtr) == 0 )
				{
					dest = node.unitID;
					success = true;
				}
			}
			else if ( numMatching > 1 )
			{
				auto range = unitTable.equal_range(hash);
				for ( auto it = range.first; it != range.second; it ++ )
				{
					UnitTableNode &node = it->second;
					if ( node.unitName.compare(unitNamePtr) == 0 )
					{
						if ( success == false ) // If no matches have previously been found
						{
							dest = node.unitID;
							success = true;
						}
						else // If matches have previously been found
						{
							if ( u32(node.unitID) < dest )
								dest = node.unitID; // Replace if unitID < previous unitID
						}
					}
				}
			}
		}

		if ( !success && size < 40 ) // Now search legacy names, akas, and shortcut names
		{
			switch ( unit[0] )
			{
				case '[':
					if		( strcmp(&unit[1], "ANY UNIT]" ) == 0 ) { dest = 229; success = true; }
					else if ( strcmp(&unit[1], "BUILDINGS]") == 0 ) { dest = 231; success = true; }
					else if ( strcmp(&unit[1], "FACTORIES]") == 0 ) { dest = 232; success = true; }
					else if ( strcmp(&unit[1], "MEN]"	   ) == 0 ) { dest = 230; success = true; }
					break;
				case 'A':
					if ( strcmp(&unit[1], "LAN TURRET") == 0 ) { dest = 18; success = true; }
					break;
				case 'B':
					if ( strcmp(&unit[1], "ENGALAAS (JUNGLE)") == 0 ) { dest = 90; success = true; }
					break;
				case 'C':
					if		( strcmp(&unit[1], "ANTINA") == 0 ) { dest = 181; success = true; }
					else if ( strcmp(&unit[1], "AVE"   ) == 0 ) { dest = 179; success = true; }
					else if ( strcmp(&unit[1], "AVE-IN") == 0 ) { dest = 180; success = true; }
					else if ( strcmp(&unit[1], "OCOON" ) == 0 ) { dest = 59; success = true; }
					break;
				case 'D':
					if		( strcmp(&unit[1], "ARK TEMPLAR (HERO)") == 0 ) { dest =  74; success = true; }
					else if ( strcmp(&unit[1], "ISRUPTION FIELD"   ) == 0 ) { dest = 105; success = true; }
					else if	( strcmp(&unit[1], "UKE TURRET TYPE 1" ) == 0 ) { dest =  24; success = true; }
					else if ( strcmp(&unit[1], "UKE TURRET TYPE 2" ) == 0 ) { dest =  26; success = true; }
					break;
				case 'E':
					if ( strcmp(&unit[1], "DMUND DUKE (SIEGE TANK)") == 0 ) { dest = 23; success = true; }
					break;
				case 'G':
					if ( strcmp(&unit[1], "ERARD DUGALLE (GHOST)") == 0 ) { dest = 102; success = true; }
					break;
				case 'I':
					if		( strcmp(&unit[1], "NDEPENDENT COMMAND CENTER"		   ) == 0 ) { dest = 183; success = true; }
					else if ( strcmp(&unit[1], "NDEPENDENT STARPORT"			   ) == 0 ) { dest = 184; success = true; }
					else if	( strcmp(&unit[1], "NFESTED DURAN"					   ) == 0 ) { dest = 104; success = true; }
					else if ( strcmp(&unit[1], "NFESTED KERRIGAN (INFESTED TERRAN)") == 0 ) { dest =  51; success = true; }
					break;
				case 'J':
					if ( strcmp(&unit[1], "UMP GATE") == 0 ) { dest = 185; success = true; }
					break;
				case 'K':
					if		( strcmp(&unit[1], "AKARU (TWILIGHT)"		  ) == 0 ) { dest = 94; success = true; }
					else if ( strcmp(&unit[1], "YADARIN CRYSTAL FORMATION") == 0 ) { dest = 187; success = true; }
					break;
				case 'M':
					if		( strcmp(&unit[1], "INING PLATFORM"		  ) == 0 ) { dest = 182; success = true; }
					else if ( strcmp(&unit[1], "INERAL CHUNK (TYPE 1)") == 0 ) { dest = 220; success = true; }
					else if ( strcmp(&unit[1], "INERAL CHUNK (TYPE 2)") == 0 ) { dest = 221; success = true; }
					break;
				case 'N':
					if ( strcmp(&unit[1], "ORAD II (CRASHED BATTLECRUISER)") == 0 ) { dest = 126; success = true; }
					break;
				case 'P':
					if		( strcmp(&unit[1], "ROTOSS DARK TEMPLAR" ) == 0 ) { dest =  61; success = true; }
					else if ( strcmp(&unit[1], "ROTOSS UNUSED TYPE 1") == 0 ) { dest = 158; success = true; }
					else if ( strcmp(&unit[1], "ROTOSS UNUSED TYPE 2") == 0 ) { dest = 161; success = true; }
					break;
				case 'R':
					if		( strcmp(&unit[1], "AGNASAUR (ASH WORLD)"  ) == 0 ) { dest =  95; success = true; }
					else if ( strcmp(&unit[1], "UINS"				   ) == 0 ) { dest = 186; success = true; }
					else if ( strcmp(&unit[1], "HYNADON (BADLANDS)"	   ) == 0 ) { dest =  89; success = true; }
					else if ( strcmp(&unit[1], "ASZAGAL (DARK TEMPLAR)") == 0 ) { dest =  98; success = true; }
				case 'S':
					if ( strcmp(&unit[1], "CANTID (DESERT)") == 0 ) { dest = 93; success = true; }
					break;
				case 'T':
					if		( strcmp(&unit[1], "ANK TURRET TYPE 1") == 0 ) { dest =  6; success = true; }
					else if ( strcmp(&unit[1], "ANK TURRET TYPE 2") == 0 ) { dest = 31; success = true; }
					break;
				case 'U':
					if		( strcmp(&unit[1], "NUSED TERRAN BLDG TYPE 1") == 0 ) { dest = 119; success = true; }
					else if ( strcmp(&unit[1], "NUSED TERRAN BLDG TYPE 2") == 0 ) { dest = 121; success = true; }
					else if	( strcmp(&unit[1], "NUSED TYPE 1"			 ) == 0 ) { dest =  91; success = true; }
					else if ( strcmp(&unit[1], "NUSED TYPE 2"			 ) == 0 ) { dest =  92; success = true; }
					else if ( strcmp(&unit[1], "NUSED ZERG BLDG"		 ) == 0 ) { dest = 145; success = true; }
					else if ( strcmp(&unit[1], "NUSED ZERG BLDG 5"		 ) == 0 ) { dest = 153; success = true; }
					else if ( strcmp(&unit[1], "RSADON (ICE WORLD)"		 ) == 0 ) { dest =  96; success = true; }
					break;
				case 'V':
					if		( strcmp(&unit[1], "ULTURE SPIDER MINE"			) == 0 ) { dest =  13; success = true; }
					else if ( strcmp(&unit[1], "ESPENE TANK (TERRAN TYPE 1)") == 0 ) { dest = 226; success = true; }
					else if ( strcmp(&unit[1], "ESPENE TANK (TERRAN TYPE 2)") == 0 ) { dest = 227; success = true; }
					else if ( strcmp(&unit[1], "ESPENE ORB (PROTOSS TYPE 1)") == 0 ) { dest = 222; success = true; }
					else if ( strcmp(&unit[1], "ESPENE ORB (PROTOSS TYPE 2)") == 0 ) { dest = 223; success = true; }
					else if ( strcmp(&unit[1], "ESPENE SAC (ZERG TYPE 1)"	) == 0 ) { dest = 224; success = true; }
					else if ( strcmp(&unit[1], "ESPENE SAC (ZERG TYPE 2)"	) == 0 ) { dest = 225; success = true; }
					break;
				case 'Z':
					if ( strcmp(&unit[1], "ERG LURKER EGG") == 0 ) { dest = 97; success = true; }
					break;
			}
		}

		// Remove the temporary NUL char
		unitNamePtr[size] = temp;
	}
	return success;
}

bool TextTrigCompiler::ParseWavName(buffer &text, u32 &dest, u32 pos, u32 end)
{
	int size = end-pos;
	if ( text.get<u8>(pos) == '\"' )
	{
		pos ++;
		end --;
		size -= 2;
	}

	if ( size < 1 )
		return false;

	char* wavStringPtr;
	bool success = false;

	if ( text.getPtr<char>(wavStringPtr, pos, size+1) )
	{ // At least one character must come after the wav string
		// Temporarily replace next char with NUL char
		char temp = wavStringPtr[size];
		wavStringPtr[size] = '\0';
		std::string str(wavStringPtr);

		// Grab the string hash
		u32 hash = strHash(str);
		int numMatching = wavTable.count(hash);
		if ( numMatching == 1 )
		{ // Should guarentee that you can find at least one entry
			WavTableNode &node = wavTable.find(hash)->second;
			if ( node.wavName.compare(wavStringPtr) == 0 )
			{
				dest = node.wavID;
				success = true;
			}
		}
		else if ( numMatching > 1 )
		{
			auto range = wavTable.equal_range(hash);
			for ( auto it = range.first; it != range.second; it ++ )
			{
				WavTableNode &node = it->second;
				if ( node.wavName.compare(wavStringPtr) == 0 )
				{
					if ( success == false ) // If no matches have previously been found
					{
						dest = node.wavID;
						success = true;
					}
					else // If matches have previously been found
					{
						if ( u32(node.wavID) < dest )
							dest = node.wavID; // Replace if wavNum < previous wavNum
					}
				}
			}
		}
		wavStringPtr[size] = temp;
	}
	return success;
}

bool TextTrigCompiler::ParsePlayer(buffer &text, u32 &dest, u32 pos, u32 end)
{
	u32 size = end - pos,
		number;

	if ( text.get<u8>(pos) == '\"' )
	{
		pos ++;
		end --;
		size -= 2;
	}

	if ( size < 1 )
		return false;

	buffer arg;
	u8 curr;

	for ( u32 i=0; i<size; i++ ) // Copy argument to arg buffer
	{
		curr = text.get<u8>(i+pos);
		if ( curr > 96 && curr < 123 )
			arg.add<u8>(curr-32);
		else
			arg.add(curr);
	}

	for ( u32 i=0; i<arg.size(); i++ )
	{
		if ( arg.get<u8>(i) == ' ' ) // Del Spacing
			arg.del<u8>(i);
		else if ( arg.get<u8>(i) == '	' ) // Del tabbing
			arg.del<u8>(i);
	}
	arg.add<u8>('\0');

	char currChar = arg.get<u8>(0);
	char* argPtr;

	if ( currChar == 'P' )
	{
		currChar = arg.get<u8>(1);
		if ( currChar == 'L' )
		{
			if ( arg.has("AYER", 2, 4) )
			{
				argPtr = (char*)arg.getPtr(6);
				if ( number = atoi(argPtr) )
				{
					// Do something with player number
					if ( number < 13 && number > 0 )
					{
						dest = number-1;
						return true;
					}
				}
			}
		}
		else if ( currChar > 47 && currChar < 58 ) // Number
		{
			argPtr = (char*)arg.getPtr(2);
			if ( number = atoi(argPtr) )
			{
				// Do something with player number
				if ( number < 13 && number > 0 )
				{
					dest = number-1;
					return true;
				}
			}
		}
	}
	else if ( currChar == 'F' )
	{
		currChar = arg.get<u8>(1);
		if ( currChar == 'O' )
		{
			if ( arg.has("RCE", 2, 3) )
			{
				argPtr = (char*)arg.getPtr(5);

				if ( number = atoi(argPtr) )
				{
					// Do something with force number
					if ( number < 5 )
					{
						dest = number+17;
						return true;
					}
				}
			}
			else if ( arg.has("ES", 2, 2) )
			{
				// Do something with foes
				dest = 14;
				return true;
			}
		}
		else if ( currChar > 47 && currChar < 58 ) // Number
		{
			argPtr = (char*)arg.getPtr(2);

			if ( number = atoi(argPtr) )
			{
				// Do something with force number
				if ( number < 5 )
				{
					dest = number+17;
					return true;
				}
			}
		}
	}
	else if ( currChar == 'A' )
	{
		currChar = arg.get<u8>(1);
		if ( currChar == 'L' )
		{
			if ( arg.has("LPLAYERS", 2, 8) )
			{
				// Do something with all players
				dest = 17;
				return true;
			}
			else if ( arg.has("LIES", 2, 4) )
			{
				// Do something with allies
				dest = 15;
				return true;
			}
		}
		else if ( currChar == 'P' )
		{
			// Do something with all players
			dest = 17;
			return true;
		}
	}
	else if ( currChar == 'C' )
	{
		if ( arg.has("URRENTPLAYER", 1, 12) )
		{
			// Do something with current player
			dest = 13;
			return true;
		}
		else if ( arg.has('P', 1) )
		{
			// Do something with current player
			dest = 13;
			return true;
		}
	}
	else if ( currChar == 'I' )
	{
		if ( arg.has("D:", 1, 2) )
		{
			argPtr = (char*)arg.getPtr(3);
			if ( number = atoi(argPtr) )
			{
				// Do something with player number
				dest = number;
				return true;
			}
		}
	}
	else if ( currChar == 'N' )
	{
		if ( arg.has("EUTRALPLAYERS", 1, 13) )
		{
			// Do something with Neutral Players
			dest = 16;
			return true;
		}
		else if ( arg.has("ONALLIEDVICTORYPLAYERS", 1, 22) )
		{
			// Do something with non allied victory players
			dest = 26;
			return true;
		}
	}
	else if ( currChar == 'U' )
	{
		if ( arg.has("NKNOWN/UNUSED", 1, 13) )
		{
			// Do something with Unknown/Unused
			dest = 12;
			return true;
		}
	}
	else if ( currChar > 47 && currChar < 58 ) // pure number
	{
		argPtr = (char*)arg.getPtr(0);
		if ( number = atoi(argPtr) )
		{
			dest = number;
			return true;
		}
	}
	/*else*/ // Might be a defined group name
	{
		char* groupStrPtr;
		bool success = false;

		if ( text.getPtr<char>(groupStrPtr, pos, size+1) )
		{ // At least one character must come after the group string
			// Temporarily replace next char with NUL char
			char temp = groupStrPtr[size];
			groupStrPtr[size] = '\0';

			// Grab the string hash
			std::string str(groupStrPtr);
			u32 hash = strHash(str);
			int numMatching = groupTable.count(hash);
			if ( numMatching == 1 )
			{ // Should guarentee that you can find at least one entry
				GroupTableNode &node = groupTable.find(hash)->second;
				if ( node.groupName.compare(groupStrPtr) == 0 )
				{
					dest = node.groupID;
					success = true;
				}
			}
			else if ( numMatching > 1 )
			{
				auto range = groupTable.equal_range(hash);
				for ( auto it = range.first; it != range.second; it ++ )
				{
					GroupTableNode &node = it->second;
					if ( node.groupName.compare(groupStrPtr) == 0 )
					{
						if ( success == false ) // If no matches have previously been found
						{
							dest = node.groupID;
							success = true;
						}
						else // If matches have previously been found
						{
							if ( u32(node.groupID) < dest )
								dest = node.groupID; // Replace if groupID < previous groupID
						}
					}
				}
			}
			groupStrPtr[size] = temp;
		}
		return success;
	}
	
	return false;
}

bool TextTrigCompiler::ParseSwitch(buffer &text, u8 &dest, u32 pos, u32 end)
{
	if ( text.get<u8>(pos) == '\"' ) // If quoted, ignore quotes
	{
		pos ++;
		end --;
	}
	
	int size = end-pos;
	char* switchNamePtr;
	bool success = false;

	if ( text.getPtr<char>(switchNamePtr, pos, size+1) )
	{ // At least one character must come after the switch string
		// Temporarily replace next char with NUL char
		char temp = switchNamePtr[size],
			 currChar;
		switchNamePtr[size] = '\0';

		if ( size < 12 )
		{
			// Take an upper case copy of the name
			char sw[12] = { };
			int numSkipped = 0;
			for ( int i=0; i<size; i++ )
			{
				currChar = switchNamePtr[i];
				if ( currChar > 96 && currChar < 123 )
					sw[i-numSkipped] = currChar - 32;
				else if ( currChar != ' ' )
					sw[i-numSkipped] = currChar;
				else
					numSkipped ++;
			}
			sw[size] = '\0';

			// Check if it's a standard switch name
			if ( sw[0] == 'S' && sw[1] == 'W' && sw[2] == 'I' &&
				 sw[3] == 'T' && sw[4] == 'C' && sw[5] == 'H' &&
				 ( dest = atoi(&sw[6]) ) )
			{
				dest --; // 0 based
				success = true;
			}
		}

		if ( !success ) // Otherwise search switch name table
		{
			std::string str(switchNamePtr);
			u32 hash = strHash(str);
			int numMatching = switchTable.count(hash);
			if ( numMatching == 1 )
			{ // Should guarentee that you can find at least one entry
				SwitchTableNode &node = switchTable.find(hash)->second;
				if ( node.switchName.compare(switchNamePtr) == 0 )
				{
					dest = node.switchNum;
					success = true;
				}
			}
			else if ( numMatching > 1 )
			{
				auto range = switchTable.equal_range(hash);
				for ( auto it = range.first; it != range.second; it ++ )
				{
					SwitchTableNode &node = it->second;
					if ( node.switchName.compare(switchNamePtr) == 0 )
					{
						if ( success == false ) // If no matches have previously been found
						{
							dest = node.switchNum;
							success = true;
						}
						else // If matches have previously been found
						{
							if ( u32(node.switchNum) < dest )
								dest = node.switchNum; // Replace if switchID < previous switchID
						}
					}
				}
			}
		}

		// Remove the temporary NUL char
		switchNamePtr[size] = temp;
	}
	return success;
}

bool TextTrigCompiler::ParseSwitch(buffer &text, u32 &dest, u32 pos, u32 end)
{
	u8 temp = 0;
	bool success = ParseSwitch(text, temp, pos, end);
	dest = temp;
	return success;
}

u8 TextTrigCompiler::ExtendedToRegularCID(s32 extendedCID)
{
	switch ( extendedCID )
	{
		// Don't include CID_CUSTOM, that is set while parsing args
		case CID_MEMORY:
			return CID_MEMORY_BASE;
			break;
	}
	return (u8)extendedCID;
}

u8 TextTrigCompiler::ExtendedToRegularAID(s32 extendedAID)
{
	switch ( extendedAID )
	{
		// Don't include AID_CUSTOM, that is set while parsing args
		case AID_MEMORY:
			return AID_MEMORY_BASE;
			break;
	}
	return 0;
}

s32 TextTrigCompiler::ExtendedNumConditionArgs(s32 extendedCID)
{
	switch ( extendedCID )
	{
		case CID_CUSTOM:
			return 8;
		case CID_MEMORY:
			return 3;
	}
	return 0;
}

s32 TextTrigCompiler::ExtendedNumActionArgs(s32 extendedAID)
{
	switch ( extendedAID )
	{
		case AID_CUSTOM:
			return 11;
		case AID_MEMORY:
			return 3;
	}
	return 0;
}

// private

bool TextTrigCompiler::PrepLocationTable(Scenario* map)
{
	ChkLocation* loc;
	u16 stringNum;
	LocationTableNode locNode;
	buffer& MRGN = map->MRGN();
	if ( MRGN.exists() && map->STR().exists() )
	{
		locationTable.reserve(MRGN.size()/sizeof(ChkLocation));
		for ( u32 i=0; i<MRGN.size()/sizeof(ChkLocation); i++ )
		{
			if ( MRGN.getPtr(loc, i*sizeof(ChkLocation), sizeof(ChkLocation)) )
			{
				locNode.locationName = "";
				stringNum = loc->stringNum;

				if ( i == 63 )
				{
					locNode.locationNum = 64;
					locNode.locationName = "Anywhere";
					locationTable.insert( pair<u32, LocationTableNode>(strHash(locNode.locationName), locNode) );
				}
				else if ( loc->stringNum > 0 && map->getRawString(locNode.locationName, loc->stringNum) )
				{
					locNode.locationNum = u8(i+1);
					locationTable.insert( pair<u32, LocationTableNode>(strHash(locNode.locationName), locNode) );
				}
			}
		}
		locationTable.reserve(locationTable.size());
	}
	return true;
}

bool TextTrigCompiler::PrepUnitTable(Scenario* map)
{
	UnitTableNode unitNode;
	buffer& unitSettings = map->unitSettings();
	if ( unitSettings.exists() && map->STR().exists() )
	{
		u16 stringID;
		for ( int unitID=0; unitID<228; unitID++ )
		{
			if ( unitSettings.get<u16>(stringID, UNIT_SETTINGS_STRING_IDS+unitID*2) &&
				 stringID > 0 )
			{
				unitNode.unitID = unitID;
				map->getRawString(unitNode.unitName, stringID);
				unitTable.insert( pair<u32, UnitTableNode>(strHash(unitNode.unitName), unitNode) );
			}
		}
	}
	return true;
}

bool TextTrigCompiler::PrepSwitchTable(Scenario* map)
{
	SwitchTableNode switchNode;
	buffer& SWNM = map->SWNM();
	if ( SWNM.exists() && map->STR().exists() )
	{
		u32 stringID;
		for ( u32 switchID=0; switchID<256; switchID++ )
		{
			if ( SWNM.get<u32>(stringID, switchID*4) &&
				 stringID > 0 &&
				 map->getRawString(switchNode.switchName, stringID) )
			{
				switchNode.switchNum = u8(switchID);
				switchTable.insert( pair<u32, SwitchTableNode>(strHash(switchNode.switchName), switchNode) );				
			}
		}
	}
	return true;
}

bool TextTrigCompiler::PrepWavTable(Scenario* map)
{
	WavTableNode wavNode;
	buffer& WAV = map->WAV();
	if ( WAV.exists() && map->STR().exists() )
	{
		for ( u32 i=0; i<512; i++ )
		{
			u32 stringID;
			if ( WAV.get<u32>(stringID, i*4) &&
				 stringID > 0 &&
				 map->getRawString(wavNode.wavName, stringID) )
			{
				wavNode.wavID = stringID;
				wavTable.insert( pair<u32, WavTableNode>(strHash(wavNode.wavName), wavNode) );
			}
		}
	}
	return true;
}

bool TextTrigCompiler::PrepGroupTable(Scenario* map)
{
	GroupTableNode groupNode;
	buffer& FORC = map->FORC();
	if ( FORC.exists() && map->STR().exists() )
	{
		for ( u32 i=0; i<4; i++ )
		{
			u16 stringID;
			if ( FORC.get<u16>(stringID, 8+i*2) &&
				 stringID > 0 &&
				 map->getRawString(groupNode.groupName, stringID) )
			{
				groupNode.groupID = i+18;
				groupTable.insert( pair<u32, GroupTableNode>(strHash(groupNode.groupName), groupNode) );
			}
		}
	}
	return true;
}

bool TextTrigCompiler::PrepStringTable(Scenario* chk)
{
	if ( chk->STR().exists() )
	{
		StringTableNode node;
		#define AddStrIffOverZero(index)																	\
			if ( index > 0 && chk->getRawString(node.string, index) ) {										\
				node.stringNum = index;																		\
				node.isExtended = chk->isExtendedString(node.stringNum);									\
				if ( !strIsInTable(node.string) ) {															\
					stringTable.insert( pair<u32, StringTableNode>(strHash(node.string), node) );	\
				}																							\
			}

		ChkLocation* loc;
		for ( u32 i=0; i<chk->MRGN().size()/CHK_LOCATION_SIZE; i++ )
		{
			if ( chk->getLocation(loc, u8(i)) )
				AddStrIffOverZero(loc->stringNum);
		}

		Trigger* trig;
		int trigNum = 0;
		while ( chk->getTrigger(trig, trigNum) )
		{
			for ( int i=0; i<NUM_TRIG_ACTIONS; i++ )
				AddStrIffOverZero( trig->actions[i].stringNum );
			
			trigNum ++;
		}

		trigNum = 0;
		while ( chk->getBriefingTrigger(trig, trigNum) )
		{
			for ( int i=0; i<NUM_TRIG_ACTIONS; i++ )
				AddStrIffOverZero( trig->actions[i].stringNum );

			trigNum ++;
		}
	
		AddStrIffOverZero( chk->SPRP().get<u16>(0) ); // Scenario Name
		AddStrIffOverZero( chk->SPRP().get<u16>(2) ); // Scenario Description

		for ( int i=0; i<4; i++ )
			AddStrIffOverZero( chk->getForceStringNum(i) );

		for ( u32 i=0; i<chk->WAV().size()/4; i++ )
			AddStrIffOverZero( chk->WAV().get<u32>(i*4) );

		buffer &unitSettings = chk->unitSettings();
		for ( int i=0; i<228; i++ )
			AddStrIffOverZero( unitSettings.get<u16>(UNIT_SETTINGS_STRING_IDS+i*2) );

		for ( int i=0; i<256; i++ )
			AddStrIffOverZero( chk->SWNM().get<u32>(i*4) );

	}
	return true;
}

bool TextTrigCompiler::strIsInTable(string str)
{
	u32 hash = strHash(str);
	int numMatching = stringTable.count(hash);
	if ( numMatching == 1 )
	{ // Should guarentee that you can find at least one entry
		StringTableNode &node = stringTable.find(hash)->second;
		if ( node.string.compare(str) == 0 )
			return true;
	}
	else if ( numMatching > 1 )
	{
		u32 currLowest = 0;
		bool success = false;
		auto range = stringTable.equal_range(hash);
		for ( auto it = range.first; it != range.second; it ++ )
		{
			StringTableNode &node = it->second;
			if ( node.string.compare(str) == 0 )
			{
				if ( success == false ) // If no matches have previously been found
				{
					currLowest = node.stringNum;
					success = true;
				}
				else // If matches have previously been found
				{
					if ( node.stringNum < currLowest )
						currLowest = node.stringNum; // Replace if stringNum < previous stringNum
				}
			}
		}
		return success;
	}
	return false;
}

bool TextTrigCompiler::BuildNewStringTable(Scenario* map)
{
	std::list<StringTableNode> standardStrList;
	std::list<StringTableNode> extendedStrList;

	// Include all strings added by text trigs
	try {
		for ( auto it = addedStrings.begin(); it != addedStrings.end(); it ++ )
		{
			if ( it->isExtended )
				extendedStrList.push_back(*it);
			else
				standardStrList.push_back(*it);
		}
	} catch ( std::bad_alloc ) {
		sprintf_s(LastError, MAX_ERROR_MESSAGE_SIZE, "Out of memory!");
		return false;
	}

	bool success = true;

	if ( !( map->addAllUsedStrings(standardStrList, STRADD_INCLUDE_STANDARD) && map->rebuildStringTable(standardStrList, false) ) )
		success = false;
	else if ( ( map->KSTR().exists() || extendedStrList.size() > 0 ) &&
			  !(map->addAllUsedStrings(extendedStrList, STRADD_INCLUDE_EXTENDED) && map->rebuildStringTable(extendedStrList, true) ) )
	{
		success = false;
	}

	return success;
}


// Begin TextTrigGenerator class

TextTrigGenerator::TextTrigGenerator(Scenario* map)
{
	PrepLocationTable(map);
	PrepUnitTable(map);
	PrepSwitchTable(map);
	PrepWavTable(map);
	PrepGroupTable(map);
	PrepStringTable(map);
	PrepConditionTable();
	PrepActionTable();
}

bool TextTrigGenerator::GenerateTextTrigs(buffer& TRIG, buffer& output)
{
	u32 numTrigs = TRIG.size()/TRIG_STRUCT_SIZE;
	Trigger* currTrig;
	Condition* conditions;
	Action* actions;
	u8* players;
	u8 CID;
	u8 AID;

	int numArgs;

	const u8 conditionNumArgs[] = { 0, 2, 4, 5, 4, 4, 1, 2, 1, 1,
									1, 2, 2, 0, 3, 4, 1, 2, 1, 1,
									1, 4, 0, 0 };

	const u8 actionNumArgs[] = { 0, 0, 0, 0, 1, 0, 0, 8, 2, 2,
								 1, 5, 1, 2, 2, 1, 2, 2, 3, 2,
								 2, 2, 2, 4, 2, 4, 4, 4, 1, 2,
								 0, 0, 1, 3, 4, 3, 3, 3, 4, 5,
								 1, 1, 4, 4, 4, 4, 5, 1, 5, 5,
								 5, 5, 4, 5, 0, 0, 0, 2, 0, 0 };

	const char* textFlags[] = { "Don't Always Display", "Always Display" };
	const char* scoreTypes[] = { "Total", "Units", "Buildings", "Units and buildings", "Kills", "Razings", "Kills and razings", "Custom" };
	const char* resourceTypes[] = { "ore", "gas", "ore and gas" };
	const char* orderTypes[] = { "move", "patrol", "attack" };
	const char* stateModifiers[] = { "0", "1", "2", "3", "enabled", "disabled", "toggle" };
	const char* switchStates[] = { "0", "1", "set", "not set" };
	const char* switchModifiers[] = { "0", "1", "2", "3", "set", "clear", "toggle", "7", "8", "9", "10", "randomize" };
	const char* allyStates[] = { "Enemy", "Ally", "Allied Victory" };
	const char* numericComparisons[] = { "At least", "At most", "2", "3", "4", "5", "6", "7", "8", "9", "Exactly" };
	const char* numericModifiers[] = { "0", "1", "2", "3", "4", "5", "6", "Set To", "Add", "Subtract" };

	char number[12] = { };

	#define ADD_TEXTTRIG_LOCATION(src) {											\
		if ( src >= 0 && src < locationTable.size() )								\
			output.addStr(locationTable[src].c_str(), locationTable[src].size());	\
		else { _itoa_s(src, number, 10); output.addStr(number, strlen(number)); } }

	#define ADD_TEXTTRIG_STRING(src) {																							\
		if ( src >= 0 && (src < stringTable.size() || (65536-src) < extendedStringTable.size() ) )								\
		{																														\
			if ( src < stringTable.size() )																						\
				output.addStr(stringTable[src].c_str(), stringTable[src].size());												\
			else																												\
				output.addStr(string("k" + extendedStringTable[65536-src]).c_str(), extendedStringTable[65536-src].size()+1);	\
		}																														\
		else { _itoa_s(src, number, 10); output.addStr(number, strlen(number)); } }

	#define ADD_TEXTTRIG_PLAYER(src) {												\
		if ( src >= 0 && src < groupTable.size() )									\
			output.addStr(groupTable[src].c_str(), groupTable[src].size());			\
		else { _itoa_s(src, number, 10); output.addStr(number, strlen(number)); } }

	#define ADD_TEXTTRIG_UNIT(src) {												\
		if ( src >= 0 && src < unitTable.size() )									\
			output.addStr(unitTable[src].c_str(), unitTable[src].size());			\
		else { _itoa_s(src, number, 10); output.addStr(number, strlen(number)); } }

	#define ADD_TEXTTRIG_SWITCH(src) {												\
		if ( src >= 0 && src < switchTable.size() )									\
			output.addStr(switchTable[src].c_str(), switchTable[src].size());		\
		else { _itoa_s( src, number, 10); output.addStr(number, strlen(number)); } }

	#define ADD_TEXTTRIG_SCORE_TYPE(src) {											\
		if ( src >= 0 && src < sizeof(scoreTypes)/sizeof(const char*) )				\
			output.addStr(scoreTypes[src], strlen(scoreTypes[src]));				\
		else { _itoa_s(src, number, 10); output.addStr(number, strlen(number)); } }

	#define ADD_TEXTTRIG_RESOURCE_TYPE(src) {										\
		if ( src >= 0 && src < sizeof(resourceTypes)/sizeof(const char*) )			\
			output.addStr(resourceTypes[src], strlen(resourceTypes[src]));			\
		else { _itoa_s(src, number, 10); output.addStr(number, strlen(number)); } }

	#define ADD_TEXTTRIG_ORDER(src) {												\
		if ( src >= 0 && src < sizeof(orderTypes)/sizeof(const char*) )				\
			output.addStr(orderTypes[src], strlen(orderTypes[src]));				\
		else { _itoa_s(src, number, 10); output.addStr(number, strlen(number)); } }

	#define ADD_TEXTTRIG_STATE_MODIFIER(src) {										\
		if ( src >= 0 && src < sizeof(stateModifiers)/sizeof(const char*) )			\
			output.addStr(stateModifiers[src], strlen(stateModifiers[src]));		\
		else { _itoa_s(src, number, 10); output.addStr(number, strlen(number)); } } 

	#define ADD_TEXTTRIG_SWITCH_STATE(src) {										\
		if ( src >= 0 && src < sizeof(switchStates)/sizeof(const char*) )			\
			output.addStr(switchStates[src], strlen(switchStates[src]));			\
		else { _itoa_s(src, number, 10); output.addStr(number, strlen(number)); } }

	#define ADD_TEXTTRIG_SWITCH_MODIFIER(src) {										\
		if ( src >= 0 && src < sizeof(switchModifiers)/sizeof(const char*) )		\
			output.addStr(switchModifiers[src], strlen(switchModifiers[src]));		\
		else { _itoa_s(src, number, 10); output.addStr(number, strlen(number)); } }

	#define ADD_TEXTTRIG_ALLY_STATE(src) {											\
		if ( src >= 0 && src < sizeof(allyStates)/sizeof(const char*) )				\
			output.addStr(allyStates[src], strlen(allyStates[src]));				\
		else { _itoa_s(src, number, 10); output.addStr(number, strlen(number)); } }

	#define ADD_TEXTTRIG_NUMERIC_COMPARISON(src) {										\
		if ( src >= 0 && src < sizeof(numericComparisons)/sizeof(const char*) )			\
			output.addStr(numericComparisons[src], strlen(numericComparisons[src]));	\
		else { _itoa_s(src, number, 10); output.addStr(number, strlen(number)); } }

	#define ADD_TEXTTRIG_NUMERIC_MODIFIER(src) {									\
		if ( src >= 0 && src < sizeof(numericModifiers)/sizeof(const char*) )		\
			output.addStr(numericModifiers[src], strlen(numericModifiers[src]));	\
		else { _itoa_s(src, number, 10); output.addStr(number, strlen(number)); } }

	#define ADD_TEXTTRIG_SCRIPT(src) {												\
		output.add('\"'); output.addStr(src, 4); output.add('\"'); }

	#define ADD_TEXTTRIG_NUM_UNITS(src) {											\
		if ( src == 0 ) output.addStr("All", 3);									\
		else { _itoa_s(src, number, 10); output.addStr(number, strlen(number)); } }

	#define ADD_TEXTTRIG_NUMBER(src) {												\
		_itoa_s((int)src, number, 10); output.addStr(number, strlen(number)); }

	#define ADD_TEXTTRIG_TEXT_FLAGS(src) {											\
		if		( (src&ACTION_FLAG_ALWAYS_DISPLAY) == 0 )							\
			output.addStr(textFlags[0], strlen(textFlags[0]));						\
		else if ( (src&ACTION_FLAG_ALWAYS_DISPLAY) == ACTION_FLAG_ALWAYS_DISPLAY )	\
			output.addStr(textFlags[1], strlen(textFlags[1])); }

	for ( u32 trigNum=0; trigNum<numTrigs; trigNum++ )
	{
		if ( TRIG.getPtr<Trigger>(currTrig, trigNum*TRIG_STRUCT_SIZE, TRIG_STRUCT_SIZE) )
		{
			output.addStr("Trigger(", 8);

			// Add players
			players = currTrig->players;
			bool hasPrevious = false;
			for ( int groupNum=0; groupNum<NUM_TRIG_PLAYERS; groupNum++ )
			{
				if ( players[groupNum] == EXECUTED_BY_PLAYER )
				{
					if ( hasPrevious )
						output.add<char>(',');
					else
						hasPrevious = true;

					string groupName = groupTable[groupNum];
					output.addStr(groupName.c_str(), groupName.size());
				}
			}

			output.addStr("){\nConditions:", 14);

			// Add conditions
			conditions = currTrig->conditions;
			for ( int i=0; i<NUM_TRIG_CONDITIONS; i++ )
			{
				Condition& condition = conditions[i];
				CID = condition.condition;

				if ( CID > CID_NO_CONDITION )
				{
					if ( (condition.flags&CONDITION_FLAG_DISABLED) == CONDITION_FLAG_DISABLED )
						output.addStr("\n;	", 3);
					else
						output.addStr("\n	", 2);

					// Add condition name
					if ( CID == CID_DEATHS && condition.players > 28 ) // Memory condition
						output.addStr("Memory", 6);
					else if ( CID < conditionTable.size() )
						output.addStr(conditionTable[CID].c_str(), conditionTable[CID].size());
					else
						output.addStr("Custom", 6);

					output.add<char>('(');
					// Add condition args
					if ( CID == CID_DEATHS && condition.players > 28 ) // Memory condition
					{
						CID = CID_MEMORY;
						numArgs = 3;
					}
					else if ( CID < sizeof(conditionNumArgs) )
						numArgs = conditionNumArgs[CID];
					else
						numArgs = 9; // custom

					for ( int i=0; i<numArgs; i++ )
					{
						if ( i > 0 )
							output.addStr(", ", 2);

						switch ( CID )
						{
							case CID_ACCUMULATE: // Player, NumericComparison, Amount, ResourceType
								switch ( i ) {
									case 0: ADD_TEXTTRIG_PLAYER(condition.players) break;
									case 1: ADD_TEXTTRIG_NUMERIC_COMPARISON(condition.comparison) break;
									case 2: ADD_TEXTTRIG_NUMBER(condition.amount) break;
									case 3: ADD_TEXTTRIG_RESOURCE_TYPE(condition.typeIndex) break;
								}
								break;
							case CID_BRING: // Player, Unit, Location, NumericComparison, Amount
								switch ( i ) {
									case 0: ADD_TEXTTRIG_PLAYER(condition.players) break;
									case 1: ADD_TEXTTRIG_UNIT(condition.unitID) break;
									case 2: ADD_TEXTTRIG_LOCATION(condition.locationNum) break;
									case 3: ADD_TEXTTRIG_NUMERIC_COMPARISON(condition.comparison) break;
									case 4: ADD_TEXTTRIG_NUMBER(condition.amount) break;
								}
								break;
							case CID_COMMAND: // Player, Unit, NumericComparison, Amount
							case CID_DEATHS:  // Player, Unit, NumericComparison, Amount
							case CID_KILL:	  // Player, Unit, NumericComparison, Amount
								switch ( i ) {
									case 0: ADD_TEXTTRIG_PLAYER(condition.players) break;
									case 1: ADD_TEXTTRIG_UNIT(condition.unitID) break;
									case 2: ADD_TEXTTRIG_NUMERIC_COMPARISON(condition.comparison) break;
									case 3: ADD_TEXTTRIG_NUMBER(condition.amount) break;
								}
								break;
							case CID_COMMAND_THE_LEAST: // Unit
							case CID_COMMAND_THE_MOST:	// Unit
							case CID_LEAST_KILLS:		// Unit
							case CID_MOST_KILLS:		// Unit
								if ( i == 0 ) ADD_TEXTTRIG_UNIT(condition.unitID);
								break;
							case CID_COMMAND_THE_LEAST_AT: // Unit, Location
							case CID_COMMAND_THE_MOST_AT: // Unit, Location
								switch ( i ) {
									case 0: ADD_TEXTTRIG_UNIT(condition.unitID) break;
									case 1: ADD_TEXTTRIG_LOCATION(condition.locationNum) break;
								}
								break;
							case CID_COUNTDOWN_TIMER: // NumericComparison, Amount
							case CID_ELAPSED_TIME: // NumericComparison, Amount
								switch ( i ) {
									case 0: ADD_TEXTTRIG_NUMERIC_COMPARISON(condition.comparison) break;
									case 1: ADD_TEXTTRIG_NUMBER(condition.amount) break;
								}
								break;
							case CID_HIGHEST_SCORE: // ScoreType
							case CID_LOWEST_SCORE: // ScoreType
								if ( i == 0 ) ADD_TEXTTRIG_SCORE_TYPE(condition.typeIndex);
								break;
							case CID_LEAST_RESOURCES: // ResourceType
							case CID_MOST_RESOURCES: // ResourceType
								if ( i == 0 ) ADD_TEXTTRIG_RESOURCE_TYPE(condition.typeIndex);
								break;
							case CID_OPPONENTS: // Player, NumericComparison, Amount
								switch ( i ) {
									case 0: ADD_TEXTTRIG_PLAYER(condition.players) break;
									case 1: ADD_TEXTTRIG_NUMERIC_COMPARISON(condition.comparison) break;
									case 2: ADD_TEXTTRIG_NUMBER(condition.amount) break;
								}
								break;
							case CID_SCORE: // Player, ScoreType, NumericComparison, Amount
								switch ( i ) {
									case 0: ADD_TEXTTRIG_PLAYER(condition.players) break;
									case 1: ADD_TEXTTRIG_SCORE_TYPE(condition.typeIndex) break;
									case 2: ADD_TEXTTRIG_NUMERIC_COMPARISON(condition.comparison) break;
									case 3: ADD_TEXTTRIG_NUMBER(condition.amount) break;
								}
								break;
							case CID_SWITCH: // Switch, SwitchState
								switch ( i ) {
									case 0: ADD_TEXTTRIG_SWITCH(condition.typeIndex) break;
									case 1: ADD_TEXTTRIG_SWITCH_STATE(condition.comparison) break;
								}
								break;
							case (u8)CID_MEMORY: // MemOffset, NumericComparison, Amount
								switch ( i ) {
									case 0: ADD_TEXTTRIG_NUMBER(condition.players) break;
									case 1: ADD_TEXTTRIG_NUMERIC_COMPARISON(condition.comparison) break;
									case 2: ADD_TEXTTRIG_NUMBER(condition.amount) break;
								}
								break;
							default: // Location, Player, Amount, Unit, NumericComparison, Condition, TypeIndex, Flags, Internal
								switch ( i ) {
									case 0: ADD_TEXTTRIG_LOCATION(condition.locationNum) break;
									case 1: ADD_TEXTTRIG_PLAYER(condition.players) break;
									case 2: ADD_TEXTTRIG_NUMBER(condition.amount) break;
									case 3: ADD_TEXTTRIG_UNIT(condition.unitID) break;
									case 4: ADD_TEXTTRIG_NUMERIC_COMPARISON(condition.comparison) break;
									case 5: ADD_TEXTTRIG_NUMBER(condition.condition) break;
									case 6: ADD_TEXTTRIG_NUMBER(condition.typeIndex) break;
									case 7: ADD_TEXTTRIG_NUMBER(condition.flags) break;
									case 8: ADD_TEXTTRIG_NUMBER(condition.internalData) break;
								}
						}
					}

					output.addStr(");", 2);
				}
			}

			output.addStr("\n\nActions:", 10);

			// Add actions
			actions = currTrig->actions;
			for ( int i=0; i<NUM_TRIG_ACTIONS; i++ )
			{
				Action& action = actions[i];
				AID = action.action;
				if ( AID > AID_NO_ACTION )
				{
					if ( (action.flags&ACTION_FLAG_DISABLED) == ACTION_FLAG_DISABLED )
						output.addStr("\n;	", 3);
					else
						output.addStr("\n	", 2);

					// Add action name
					if ( AID < actionTable.size() )
						output.addStr(actionTable[AID].c_str(), actionTable[AID].size());
					else
						output.addStr("Custom", 6);

					output.add<char>('(');
					// Add action args
					if ( AID < sizeof(actionNumArgs) )
						numArgs = actionNumArgs[AID];
					else
						numArgs = 11; // custom

					for ( int i=0; i<numArgs; i++ )
					{
						if ( i > 0 )
							output.addStr(", ", 2);

						switch ( AID )
						{
							case AID_CENTER_VIEW:  // Location
							case AID_MINIMAP_PING: // Location
								if ( i == 0 ) ADD_TEXTTRIG_LOCATION(action.location)
								break;
							case AID_COMMENT:				 // String
							case AID_SET_MISSION_OBJECTIVES: // String
							case AID_SET_NEXT_SCENARIO:		 // String
								if ( i == 0 ) ADD_TEXTTRIG_STRING(action.stringNum)
								break;
							case AID_CREATE_UNIT:			// Player, Unit, NumUnits, Location
							case AID_KILL_UNIT_AT_LOCATION: // Player, Unit, NumUnits, Location
								switch ( i ) {
									case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
									case 1: ADD_TEXTTRIG_UNIT(action.type) break;
									case 2: ADD_TEXTTRIG_NUM_UNITS(action.type2) break;
									case 3: ADD_TEXTTRIG_LOCATION(action.location) break;
								}
								break;
							case AID_CREATE_UNIT_WITH_PROPERTIES: // Player, Unit, NumUnits, Location, Properties
								switch ( i ) {
									case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
									case 1: ADD_TEXTTRIG_UNIT(action.type) break;
									case 2: ADD_TEXTTRIG_NUM_UNITS(action.type2) break;
									case 3: ADD_TEXTTRIG_LOCATION(action.location) break;
									case 4: ADD_TEXTTRIG_NUMBER(action.number) break;
								}
								break;
							case AID_DISPLAY_TEXT_MESSAGE: // TextFlags, String
								switch ( i ) {
									case 0: ADD_TEXTTRIG_TEXT_FLAGS(action.flags) break;
									case 1: ADD_TEXTTRIG_STRING(action.stringNum) break;
								}
								break;
							case AID_GIVE_UNITS_TO_PLAYER: // Player, SecondPlayer, Unit, NumUnits, Location
								switch ( i ) {
									case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
									case 1: ADD_TEXTTRIG_PLAYER(action.number) break;
									case 2: ADD_TEXTTRIG_UNIT(action.type) break;
									case 3: ADD_TEXTTRIG_NUM_UNITS(action.type2) break;
									case 4: ADD_TEXTTRIG_LOCATION(action.location) break;
								}
								break;
							case AID_KILL_UNIT:	  // Player, Unit
							case AID_REMOVE_UNIT: // Player, Unit
								switch ( i ) {
									case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
									case 1: ADD_TEXTTRIG_UNIT(action.type) break;
								}
								break;
							case AID_LEADERBOARD_CONTROL_AT_LOCATION: // String, Unit, Location
								switch ( i ) {
									case 0: ADD_TEXTTRIG_STRING(action.stringNum) break;
									case 1: ADD_TEXTTRIG_UNIT(action.type) break;
									case 2: ADD_TEXTTRIG_LOCATION(action.location) break;
								}
								break;
							case AID_LEADERBOARD_CONTROL: // String, Unit
							case AID_LEADERBOARD_KILLS: // String, Unit
								switch ( i ) {
									case 0: ADD_TEXTTRIG_STRING(action.stringNum) break;
									case 1: ADD_TEXTTRIG_UNIT(action.type) break;
								}
								break;
							case AID_LEADERBOARD_GREED: // Amount
								if ( i == 0 ) ADD_TEXTTRIG_NUMBER(action.number);
								break;
							case AID_LEADERBOARD_POINTS: // String, ScoreType
								switch ( i ) {
									case 0: ADD_TEXTTRIG_STRING(action.stringNum) break;
									case 1: ADD_TEXTTRIG_SCORE_TYPE(action.type) break;
								}
								break;
							case AID_LEADERBOARD_RESOURCES: // String, ResourceType
								switch ( i ) {
									case 0: ADD_TEXTTRIG_STRING(action.stringNum) break;
									case 1: ADD_TEXTTRIG_RESOURCE_TYPE(action.type) break;
								}
								break;
							case AID_LEADERBOARD_COMPUTER_PLAYERS: // StateModifier
								if ( i == 0 ) ADD_TEXTTRIG_STATE_MODIFIER(action.type2);
								break;
							case AID_LEADERBOARD_GOAL_CONTROL_AT_LOCATION: // String, Unit, Amount, Location
								switch ( i ) {
									case 0: ADD_TEXTTRIG_STRING(action.stringNum) break;
									case 1: ADD_TEXTTRIG_UNIT(action.type) break;
									case 2: ADD_TEXTTRIG_NUMBER(action.number) break;
									case 3: ADD_TEXTTRIG_LOCATION(action.location) break;
								}
								break;
							case AID_LEADERBOARD_GOAL_CONTROL: // String, Unit, Amount
							case AID_LEADERBOARD_GOAL_KILLS: // String, Unit, Amount
								switch ( i ) {
									case 0: ADD_TEXTTRIG_STRING(action.stringNum) break;
									case 1: ADD_TEXTTRIG_UNIT(action.type) break;
									case 2: ADD_TEXTTRIG_NUMBER(action.number) break;
								}
								break;
							case AID_LEADERBOARD_GOAL_POINTS: // String, ScoreType, Amount
								switch ( i ) {
									case 0: ADD_TEXTTRIG_STRING(action.stringNum) break;
									case 1: ADD_TEXTTRIG_SCORE_TYPE(action.type) break;
									case 2: ADD_TEXTTRIG_NUMBER(action.number) break;
								}
								break;
							case AID_LEADERBOARD_GOAL_RESOURCES: // String, Amount, ResourceType
								switch ( i ) {
									case 0: ADD_TEXTTRIG_STRING(action.stringNum) break;
									case 1: ADD_TEXTTRIG_NUMBER(action.number) break;
									case 2: ADD_TEXTTRIG_RESOURCE_TYPE(action.type) break;
								}
								break;
							case AID_MODIFY_UNIT_ENERGY: // Player, Unit, Amount, NumUnits, Location
							case AID_MODIFY_UNIT_HANGER_COUNT: // Player, Unit, Amount, NumUnits, Location
							case AID_MODIFY_UNIT_HITPOINTS: // Player, Unit, Amount, NumUnits, Location
							case AID_MODIFY_UNIT_SHIELD_POINTS: // Player, Unit, Amount, NumUnits, Location
								switch ( i ) {
									case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
									case 1: ADD_TEXTTRIG_UNIT(action.type) break;
									case 2: ADD_TEXTTRIG_NUMBER(action.number) break;
									case 3: ADD_TEXTTRIG_NUM_UNITS(action.type2) break;
									case 4: ADD_TEXTTRIG_LOCATION(action.location) break;
								}
								break;
							case AID_MODIFY_UNIT_RESOURCE_AMOUNT: // Player, Amount, NumUnits, Location
								switch ( i ) {
									case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
									case 1: ADD_TEXTTRIG_NUMBER(action.number) break;
									case 2: ADD_TEXTTRIG_NUM_UNITS(action.type2) break;
									case 3: ADD_TEXTTRIG_LOCATION(action.location) break;
								}
								break;
							case AID_MOVE_LOCATION: // Player, Unit, LocDest, Location
								switch ( i ) {
									case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
									case 1: ADD_TEXTTRIG_UNIT(action.type) break;
									case 2: ADD_TEXTTRIG_LOCATION(action.location) break;
									case 3: ADD_TEXTTRIG_LOCATION(action.number) break;
								}
								break;
							case AID_MOVE_UNIT: // Player, Unit, NumUnits, Location, LocDest
								switch ( i ) {
									case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
									case 1: ADD_TEXTTRIG_UNIT(action.type) break;
									case 2: ADD_TEXTTRIG_NUM_UNITS(action.type2) break;
									case 3: ADD_TEXTTRIG_LOCATION(action.location) break;
									case 4: ADD_TEXTTRIG_LOCATION(action.number) break;
								}
								break;
							case AID_ORDER: // Player, Unit, Location, LocDest, OrderType
								switch ( i ) {
									case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
									case 1: ADD_TEXTTRIG_UNIT(action.type) break;
									case 2: ADD_TEXTTRIG_LOCATION(action.location) break;
									case 3: ADD_TEXTTRIG_LOCATION(action.number) break;
									case 4: ADD_TEXTTRIG_ORDER(action.type2) break;
								}
								break;
							case AID_PLAY_WAV: // Wav, Duration
								switch ( i ) {
									case 0: ADD_TEXTTRIG_STRING(action.wavID) break;
									case 1: ADD_TEXTTRIG_NUMBER(action.time) break;
								}
								break;
							case AID_REMOVE_UNIT_AT_LOCATION: // Player, Unit, NumUnits, Location
								switch ( i ) {
									case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
									case 1: ADD_TEXTTRIG_UNIT(action.type) break;
									case 2: ADD_TEXTTRIG_NUM_UNITS(action.type2) break;
									case 3: ADD_TEXTTRIG_LOCATION(action.location) break;
								}
								break;
							case AID_RUN_AI_SCRIPT: // Script
								if ( i == 0 ) ADD_TEXTTRIG_SCRIPT((char*)&action.number);
								break;
							case AID_RUN_AI_SCRIPT_AT_LOCATION: // Script, Location
								switch ( i ) {
									case 0: ADD_TEXTTRIG_SCRIPT((char*)&action.number) break;
									case 1: ADD_TEXTTRIG_LOCATION(action.location) break;
								}
								break;
							case AID_SET_ALLIANCE_STATUS: // Player, AllyState
								switch ( i ) {
									case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
									case 1: ADD_TEXTTRIG_ALLY_STATE(action.type) break;
								}
								break;
							case AID_SET_COUNTDOWN_TIMER: // NumericModifier, Amount
								switch ( i ) {
									case 0: ADD_TEXTTRIG_NUMERIC_MODIFIER(action.type2) break;
									case 1: ADD_TEXTTRIG_NUMBER(action.time) break;
								}
								break;
							case AID_SET_DEATHS: // Player, Unit, NumericModifier, Amount
								switch ( i ) {
									case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
									case 1: ADD_TEXTTRIG_UNIT(action.type) break;
									case 2: ADD_TEXTTRIG_NUMERIC_MODIFIER(action.type2) break;
									case 3: ADD_TEXTTRIG_NUMBER(action.number) break;
								}
								break;
							case AID_SET_DOODAD_STATE:	// Player, Unit, Location, StateMod
							case AID_SET_INVINCIBILITY: // Player, Unit, Location, StateMod
								switch ( i ) {
									case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
									case 1: ADD_TEXTTRIG_UNIT(action.type) break;
									case 2: ADD_TEXTTRIG_LOCATION(action.location) break;
									case 3: ADD_TEXTTRIG_STATE_MODIFIER(action.type2) break;
								}
								break;
							case AID_SET_RESOURCES: // Player, NumericModifier, Amount, ResourceType
								switch ( i ) {
									case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
									case 1: ADD_TEXTTRIG_NUMERIC_MODIFIER(action.type2) break;
									case 2: ADD_TEXTTRIG_NUMBER(action.number) break;
									case 3: ADD_TEXTTRIG_RESOURCE_TYPE(action.type) break;
								}
								break;
							case AID_SET_SCORE: // Player, NumericModifier, Amount, ScoreType
								switch ( i ) {
									case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
									case 1: ADD_TEXTTRIG_NUMERIC_MODIFIER(action.type2) break;
									case 2: ADD_TEXTTRIG_NUMBER(action.number) break;
									case 3: ADD_TEXTTRIG_SCORE_TYPE(action.type) break;
								}
								break;
							case AID_SET_SWITCH: // Switch, SwitchMod
								switch ( i ) {
									case 0: ADD_TEXTTRIG_SWITCH(action.number) break;
									case 1: ADD_TEXTTRIG_SWITCH_MODIFIER(action.type2) break;
								}
								break;
							case AID_TALKING_PORTRAIT: // Unit, Duration
								switch ( i ) {
									case 0: ADD_TEXTTRIG_UNIT(action.type) break;
									case 1: ADD_TEXTTRIG_NUMBER(action.time) break;
								}
								break;
							case AID_TRANSMISSION: // TextFlags, String, Unit, Location, NumericModifier, DurationMod, Wav, Duration
								switch ( i ) {
									case 0: ADD_TEXTTRIG_TEXT_FLAGS(action.flags) break;
									case 1: ADD_TEXTTRIG_STRING(action.stringNum) break;
									case 2: ADD_TEXTTRIG_UNIT(action.type) break;
									case 3: ADD_TEXTTRIG_LOCATION(action.location) break;
									case 4: ADD_TEXTTRIG_NUMERIC_MODIFIER(action.type2) break;
									case 5: ADD_TEXTTRIG_NUMBER(action.number) break;
									case 6: ADD_TEXTTRIG_STRING(action.wavID) break;
									case 7: ADD_TEXTTRIG_NUMBER(action.time) break;
								}
								break;
							case AID_WAIT: // Duration
								if ( i == 0 ) ADD_TEXTTRIG_NUMBER(action.time);
								break;
							default: // Location, String, Wav, Duration, Player, Number, Type, Action, Type2, Flags, Internal
								switch ( i ) {
									case 0: ADD_TEXTTRIG_LOCATION(action.location) break;
									case 1: ADD_TEXTTRIG_STRING(action.stringNum) break;
									case 2: ADD_TEXTTRIG_STRING(action.wavID) break;
									case 3: ADD_TEXTTRIG_NUMBER(action.time) break;
									case 4: ADD_TEXTTRIG_PLAYER(action.group) break;
									case 5: ADD_TEXTTRIG_NUMBER(action.number) break;
									case 6: ADD_TEXTTRIG_NUMBER(action.type) break;
									case 7: ADD_TEXTTRIG_NUMBER(action.action) break;
									case 8: ADD_TEXTTRIG_NUMBER(action.type2) break;
									case 9: ADD_TEXTTRIG_NUMBER(action.flags) break;
									case 10: ADD_TEXTTRIG_NUMBER(TripletToInt(&action.internalData[0])) break;
								}
								break;
						}
					}

					output.addStr(");", 2);
				}
			}

			output.addStr("\n}\n\n//-----------------------------------------------------------------//\n\n", 75);
		}
	}
	// Add NUL
	output.add<char>('\0');
	CorrectLineEndings(output);

	return true;
}

// protected

bool TextTrigGenerator::CorrectLineEndings(buffer& buf)
{
	u8 curr;
	u32 pos = 0;
	buffer dest(buf.title());
	dest.setSize(buf.size());

	while ( pos < buf.size() ) 
	{
		curr = buf.get<u8>(pos);
		switch ( curr )
		{
			case '\15': // CR (line ending)
				if ( buf.get<u8>(pos+1) == '\12' ) // Has LF
					pos ++;
			case '\12': // LF (line ending)
			case '\13': // VT (line ending)
			case '\14': // FF (line ending)
				dest.add<u8>('\15');
				dest.add<u8>('\12');
				pos ++;
				break;
			default:
				dest.add<u8>(curr);
				pos ++;
				break;
		}

	}

	buf.takeAllData(dest);
	return true;
}

// private

bool TextTrigGenerator::PrepLocationTable(Scenario* map)
{
	ChkLocation* loc;
	u16 stringNum;
	string locationName;
	buffer& MRGN = map->MRGN();

	locationTable.push_back( string("0") );

	if ( MRGN.exists() && map->STR().exists() )
	{
		int numLocations = MRGN.size()/sizeof(ChkLocation);
		for ( int i=0; i<numLocations; i++ )
		{
			Invariant( locationTable.size() == i+1 );
			if ( MRGN.getPtr(loc, i*sizeof(ChkLocation), sizeof(ChkLocation)) )
			{
				locationName = "";
				stringNum = loc->stringNum;

				if ( i == 63 )
				{
					locationName = "\"Anywhere\"";
					locationTable.push_back( locationName );
				}
				else if ( loc->stringNum > 0 && map->getEscapedString(locationName, loc->stringNum) )
				{
					locationTable.push_back( "\"" + locationName + "\"" );
				}
			}

			if ( locationTable.size() == i+1 )
			{
				char locNum[12];
				_itoa_s(i+1, locNum, 10);
				locationName = locNum;
				locationTable.push_back( locationName );
			}
		}
	}
	return true;
}

bool TextTrigGenerator::PrepUnitTable(Scenario* map)
{
	string unitName;
	buffer& unitSettings = map->unitSettings();
	if ( unitSettings.exists() && map->STR().exists() )
	{
		for ( int unitID=0; unitID<232; unitID++ )
		{
			Invariant( unitTable.size() == unitID );
			unitName = "\"" + string(LegacyTextTrigDisplayName[unitID]) + "\"";
			unitTable.push_back( unitName );
		}
	}
	return true;
}

bool TextTrigGenerator::PrepSwitchTable(Scenario* map)
{
	string switchName;
	buffer& SWNM = map->SWNM();
	if ( SWNM.exists() && map->STR().exists() )
	{
		u32 stringID;
		for ( u32 switchID=0; switchID<256; switchID++ )
		{
			Invariant( switchTable.size() == switchID );
			if ( SWNM.get<u32>(stringID, switchID*4) &&
				 stringID > 0 &&
				 map->getEscapedString(switchName, stringID) )
			{
				switchTable.push_back( "\"" + switchName + "\"" );				
			}
			else
			{
				char swDefault[20];
				sprintf_s(swDefault, 20, "\"Switch%i\"", switchID+1);
				switchName = swDefault;
				switchTable.push_back( switchName );
			}
		}
	}
	return true;
}

bool TextTrigGenerator::PrepWavTable(Scenario* map)
{
	string wavName;
	buffer& WAV = map->WAV();
	if ( WAV.exists() && map->STR().exists() )
	{
		for ( u32 i=0; i<512; i++ )
		{
			Invariant( wavTable.size() == i );
			u32 stringID;
			if ( WAV.get<u32>(stringID, i*4) &&
				 stringID > 0 &&
				 map->getEscapedString(wavName, stringID) )
			{
				wavTable.push_back( "\"" + wavName + "\"" );
			}
			else
			{
				char wavNum[12];
				_itoa_s(i, wavNum, 10);
				wavName = wavNum;
				wavTable.push_back( wavName );
			}
		}
	}
	return true;
}

bool TextTrigGenerator::PrepGroupTable(Scenario* map)
{
	string groupName;
	buffer& FORC = map->FORC();
	bool hasForcStrings = FORC.exists() && map->STR().exists();

	const char* legacyLowerGroups[] = { "\"Player 1\"", "\"Player 2\"", "\"Player 3\"", "\"Player 4\"", "\"Player 5\"", "\"Player 6\"",
										"\"Player 7\"", "\"Player 8\"", "\"Player 9\"", "\"Player 10\"", "\"Player 11\"", "\"Player 12\"",
										"\"unknown/unused\"", "\"Current Player\"", "\"Foes\"", "\"Allies\"", "\"Neutral Players\"",
										"\"All players\"" };
	const char* legacyUpperGroups[] = { "\"unknown/unused\"", "\"unknown/unused\"", "\"unknown/unused\"", "\"unknown/unused\"",
										"\"Non Allied Victory Players\"", "\"unknown/unused\"" };

	const char** lowerGroups = legacyLowerGroups;
	const char** upperGroups = legacyUpperGroups;

	for ( u32 i=0; i<18; i++ )
	{
		groupName = lowerGroups[i];
		groupTable.push_back( groupName );
	}

	for ( u32 i=0; i<4; i++ )
	{
		DebugIf( i == 3 );
		Invariant( groupTable.size() == i+18 );
		u16 stringID;
		if ( hasForcStrings &&
			 FORC.get<u16>(stringID, 8+i*2) &&
			 stringID > 0 &&
			 map->getEscapedString(groupName, stringID) )
		{
			groupName = "\"" + groupName + "\"";
			groupTable.push_back( groupName );
		}
		else
		{
			char forceName[12];
			sprintf_s(forceName, 12, "\"Force %i\"", i);
			groupName = forceName;
			groupTable.push_back( groupName );
		}
	}

	for ( u32 i=22; i<28; i++ )
	{
		groupName = upperGroups[i-22];
		groupTable.push_back( groupName );
	}

	return true;
}

bool TextTrigGenerator::PrepStringTable(Scenario* map)
{
	if ( map->STR().exists() || map->KSTR().exists() )
	{
		bool gotStandardUsage, gotExtendedUsage;
		StringUsageTable standardStringUsage(map, false, gotStandardUsage);
		StringUsageTable extendedStringUsage(map, true, gotExtendedUsage);
		if ( gotStandardUsage || gotExtendedUsage )
		{
			string str;

			u32 numStrings = standardStringUsage.numStrings();
			for ( u32 i=0; i<numStrings; i++ )
			{
				str = "";

				if ( standardStringUsage.isUsed(i) )
					map->getEscapedString(str, i);

				stringTable.push_back( "\"" + str + "\"" );
			}

			numStrings = extendedStringUsage.numStrings();
			for ( u32 i=0; i<numStrings; i++ )
			{
				str = "";

				if ( extendedStringUsage.isUsed(i) )
					map->getEscapedString(str, (65536-i));

				extendedStringTable.push_back( "\"" + str + "\"" );
			}

			return true;
		}
		else
			return false;
	}
	else
		return false;
}

bool TextTrigGenerator::PrepConditionTable()
{
	const char* legacyConditionNames[] = { "Always", "Countdown Timer", "Command", "Bring", "Accumulate", "Kill", "Command the Most", 
										   "Commands the Most At", "Most Kills", "Highest Score", "Most Resources", "Switch", "Elapsed Time", 
										   "Never", "Opponents", "Deaths", "Command the Least", "Command the Least At", "Least Kills", 
										   "Lowest Score", "Least Resources", "Score", "Always", "Never" };

	const char** conditionNames = legacyConditionNames;

	for ( int i=0; i<24; i++ )
		conditionTable.push_back(string(conditionNames[i]));

	string custom("Custom");
	for ( int i=24; i<256; i++ )
		conditionTable.push_back(custom);

	return true;
}

bool TextTrigGenerator::PrepActionTable()
{
	const char* legacyActionNames[] = { "No Action", "Victory", "Defeat", "Preserve Trigger", "Wait", "Pause Game", "Unpause Game", "Transmission", 
										"Play WAV", "Display Text Message", "Center View", "Create Unit with Properties", "Set Mission Objectives", 
										"Set Switch", "Set Countdown Timer", "Run AI Script", "Run AI Script At Location", "Leader Board Control", 
										"Leader Board Control At Location","Leader Board Resources", "Leader Board Kills", "Leader Board Points", 
										"Kill Unit", "Kill Unit At Location", "Remove Unit", "Remove Unit At Location", "Set Resources", "Set Score", 
										"Minimap Ping", "Talking Portrait", "Mute Unit Speech", "Unmute Unit Speech", "Leaderboard Computer Players", 
										"Leaderboard Goal Control", "Leaderboard Goal Control At Location", "Leaderboard Goal Resources", 
										"Leaderboard Goal Kills", "Leaderboard Goal Points", "Move Location", "Move Unit", "Leaderboard Greed", 
										"Set Next Scenario", "Set Doodad State", "Set Invincibility", "Create Unit", "Set Deaths", "Order", "Comment", 
										"Give Units to Player", "Modify Unit Hit Points", "Modify Unit Energy", "Modify Unit Shield Points", 
										"Modify Unit Resource Amount", "Modify Unit Hanger Count", "Pause Timer", "Unpause Timer", "Draw", 
										"Set Alliance Status", "Disable Debug Mode", "Enable Debug Mode" };

	const char** actionNames = legacyActionNames;

	for ( int i=0; i<60; i++ )
		actionTable.push_back(string(actionNames[i]));

	string custom("Custom");
	for ( int i=60; i<256; i++ )
		actionTable.push_back(custom);

	return true;
}
