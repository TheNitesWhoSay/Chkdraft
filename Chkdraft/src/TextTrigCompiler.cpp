#include "TextTrigCompiler.h"
#include "Mapping Core/MappingCore.h"
#include <cstdio>
#include <cstring>
#include <exception>
#include <string>
#include <utility>
#include <vector>

#define MAX_ERROR_MESSAGE_SIZE 256

TextTrigCompiler::TextTrigCompiler()
{

}

bool TextTrigCompiler::CompileTriggers(std::string trigText, ScenarioPtr chk, ScData &scData)
{
	buffer text("TxTr");
	return text.addStr(trigText.c_str(), trigText.length()+1) && CompileTriggers(text, chk, scData);
}

bool TextTrigCompiler::CompileTriggers(buffer& text, ScenarioPtr chk, ScData &scData)
{
	if ( !LoadCompiler(chk, scData) )
		return false;

	try
	{
		CleanText(text);
		Section TRIG(new buffer((u32)SectionId::TRIG));
		char error[MAX_ERROR_MESSAGE_SIZE];

		if ( ParseTriggers(text, *TRIG, error) )
		{
			if ( BuildNewStringTable(chk) )
			{
				if ( !chk->TRIG().exists() )
				{
					if ( chk->AddSection(TRIG) )
						return true;
					else
						std::snprintf(error, sizeof(error), "No text errors, but compilation must abort due to low memory.\n\n%s", LastError);
				}
				else if ( chk->TRIG().takeAllData(*TRIG) )
					return true;
				else
					std::snprintf(error, sizeof(error), "No text errors, but TRIG could not be overwritten.\n\n%s", LastError);
			}
			else
				std::snprintf(error, sizeof(error), "No text errors, but compilation must abort due to low memory.\n\n%s", LastError);
		}

		MessageBox(NULL, error, "Error!", MB_OK | MB_ICONEXCLAMATION);
	}
	catch ( std::bad_alloc ) { MessageBox(NULL, "Compilation aborted due to low memory.", "Error!", MB_OK | MB_ICONEXCLAMATION); }
	return false;
}

bool TextTrigCompiler::CompileTrigger(std::string trigText, Trigger* trigger, ScenarioPtr chk, ScData &scData)
{
	buffer text("TxTr");
	return text.addStr(trigText.c_str(), trigText.length()+1) && CompileTrigger(text, trigger, chk, scData);
}

bool TextTrigCompiler::CompileTrigger(buffer& text, Trigger* trigger, ScenarioPtr chk, ScData &scData)
{
	if ( !LoadCompiler(chk, scData) )
		return false;

	try
	{
		CleanText(text);
		Section TRIG(new buffer((u32)SectionId::TRIG));
		char error[MAX_ERROR_MESSAGE_SIZE];

		if ( ParseTriggers(text, *TRIG, error) )
		{
			if ( BuildNewStringTable(chk) )
			{
				if ( !chk->TRIG().exists() )
				{
					if ( chk->AddSection(TRIG) )
						return true;
					else
						std::snprintf(error, sizeof(error), "No text errors, but compilation must abort due to low memory.\n\n%s", LastError);
				}
				else
				{
					Trigger* trig;
					if ( TRIG->getPtr<Trigger>(trig, 0, TRIG_STRUCT_SIZE) )
					{
						trigger->internalData = trig->internalData;
						for ( u8 i = 0; i < NUM_TRIG_PLAYERS; i++ )
							trigger->players[i] = trig->players[i];
						for ( u8 i = 0; i < NUM_TRIG_CONDITIONS; i++ )
							trigger->conditions[i] = trig->conditions[i];
						for ( u8 i = 0; i < NUM_TRIG_ACTIONS; i++ )
							trigger->actions[i] = trig->actions[i];

						return true;
					}
					else
						std::snprintf(error, sizeof(error), "No text errors, but trigger could not be transferred.\n\n%s", LastError);
				}
			}
			else
				std::snprintf(error, sizeof(error), "No text errors, but compilation must abort due to low memory.\n\n%s", LastError);
		}

		MessageBox(NULL, error, "Error!", MB_OK | MB_ICONEXCLAMATION);
	}
	catch ( std::bad_alloc ) { MessageBox(NULL, "Compilation aborted due to low memory.", "Error!", MB_OK | MB_ICONEXCLAMATION); }
	return false;
}

bool TextTrigCompiler::ParseConditionName(std::string text, u8 &ID)
{
	buffer txcd("TxCd");
	if ( txcd.addStr(text.c_str(), text.size()) )
	{
		CleanText(txcd);
		u32 CID = CID_NO_CONDITION;
		if ( ParseConditionName(txcd, CID) && CID != CID_CUSTOM )
		{
			if ( ((s32)CID) < 0 )
				ID = ExtendedToRegularCID((s32)CID);
			else
				ID = (u8)CID;

			return true;
		}
		else if ( ParseByte((char*)txcd.getPtr(0), ID, 0, txcd.size()) )
			return true;
	}
	return false;
}

bool TextTrigCompiler::ParseConditionArg(std::string conditionArgText, u8 argNum,
										 std::vector<u8> &argMap, Condition& condition, ScenarioPtr chk, ScData &scData)
{
	if ( !LoadCompiler(chk, scData) )
		return false;
	
	buffer txcd("TxCd");
	if ( argNum < argMap.size() &&
		 txcd.addStr(conditionArgText.c_str(), conditionArgText.size()) &&
		 txcd.add<u8>(0) )
	{
		char error[256] = { };
		u32 argsLeft = numConditionArgs(condition.condition) - argMap[argNum];
		if ( ParseConditionArg(txcd, condition, 0, txcd.size()-1, condition.condition, argsLeft, error) )
			return true;
		else
			CHKD_ERR("Unable to parse condition.\n\n%s", error);
	}
	return false;
}

bool TextTrigCompiler::ParseActionName(std::string text, u8 &ID)
{
	buffer txac("TxAc");
	if ( txac.addStr(text.c_str(), text.size()) )
	{
		CleanText(txac);
		u32 AID = AID_NO_ACTION;
		if ( ParseActionName(txac, AID) && AID != AID_CUSTOM )
		{
			if ( ((s32)AID) < 0 )
				ID = ExtendedToRegularAID((s32)AID);
			else
				ID = (u8)AID;

			return true;
		}
		else if ( ParseByte((char*)txac.getPtr(0), ID, 0, txac.size()) )
			return true;
	}
	return false;
}

bool TextTrigCompiler::ParseActionArg(std::string actionArgText, u8 argNum,
									  std::vector<u8> &argMap, Action &action, ScenarioPtr chk, ScData &scData)
{
	if ( !LoadCompiler(chk, scData) )
		return false;

	buffer txac("TxAc");
	if ( argNum < argMap.size() &&
		txac.addStr(actionArgText.c_str(), actionArgText.size()) &&
		txac.add<u8>(0) )
	{
		char error[256] = {};
		u32 argsLeft = numActionArgs(action.action) - argMap[argNum];
		if ( ParseActionArg(txac, action, 0, txac.size() - 1, action.action, argsLeft, error) )
			return true;
		else
			CHKD_ERR("Unable to parse action.\n\n%s", error);
	}
	return false;
}

u8 TextTrigCompiler::defaultConditionFlags(u8 CID)
{
	u8 defaultFlags[] = { 0, 0, 16, 16, 0,  16, 16, 16, 16, 0,
						  0, 0, 0, 0, 0,	16, 16, 16, 16, 0,
						  0, 0, 0, 0 };

	if ( CID >= 0 && CID < sizeof(defaultFlags)/sizeof(const u8) )
		return defaultFlags[CID];
	else
		return 0;
}

u8 TextTrigCompiler::defaultActionFlags(u8 AID)
{
	u8 defaultFlags[] = { 0, 4, 4, 4, 4,		4, 4, 0, 4, 0,
						  4, 28, 0, 4, 4,		4, 4, 16, 16, 0,
						  16, 0, 20, 20, 20,	20, 4, 4, 4, 20,
						  4, 4, 4, 16, 16,		0, 16, 0, 20, 20,
						  4, 4, 20, 20, 20,		20, 20, 0, 20, 20,
						  20, 20, 4, 20, 4,		4, 4, 4, 0, 0 };

	if ( AID >= 0 && AID < sizeof(defaultFlags) / sizeof(const u8) )
		return defaultFlags[AID];
	else
		return 0;
}

u8 TextTrigCompiler::numConditionArgs(s32 CID)
{
	const u8 conditionNumArgs[] = { 0, 2, 4, 5, 4, 4, 1, 2, 1, 1,
	1, 2, 2, 0, 3, 4, 1, 2, 1, 1,
	1, 4, 0, 0 };

	if ( CID >= 0 && CID < sizeof(conditionNumArgs) / sizeof(const u8) )
		return conditionNumArgs[CID];
	else
		return 0;
}

u8 TextTrigCompiler::numActionArgs(s32 AID)
{
	const u8 actionNumArgs[] = { 0, 0, 0, 0, 1,  0, 0, 8, 2, 2,
		1, 5, 1, 2, 2,  1, 2, 2, 3, 2,
		2, 2, 2, 4, 2,  4, 4, 4, 1, 2,
		0, 0, 1, 3, 4,  3, 3, 3, 4, 5,
		1, 1, 4, 4, 4,  4, 5, 1, 5, 5,
		5, 5, 4, 5, 0,  0, 0, 2, 0, 0 };

	if ( AID >= 0 && AID < sizeof(actionNumArgs) / sizeof(const u8) )
		return actionNumArgs[AID];
	else
		return ExtendedNumActionArgs(AID);
}

// protected

bool TextTrigCompiler::LoadCompiler(ScenarioPtr chk, ScData &scData)
{
	ClearCompiler();
	return strUsage.populateTable(chk.get(), false) && extendedStrUsage.populateTable(chk.get(), true) &&
		   PrepLocationTable(chk) && PrepUnitTable(chk) && PrepSwitchTable(chk) &&
		   PrepGroupTable(chk) && PrepStringTable(chk) && PrepScriptTable(scData);
}

void TextTrigCompiler::ClearCompiler()
{
	stringTable.clear();
	locationTable.clear();
	unitTable.clear();
	switchTable.clear();
	groupTable.clear();

	addedStrings.clear();

	strUsage.clearTable();
	extendedStrUsage.clearTable();
}

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
				if ( text.getNextUnescaped('\"', pos, skipTo) )
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
							else if ( text.get<u8>(pos) == 't' )
							{
								dest.add<u8>('\t');
							}
							else if ( text.get<char>(curr, pos) )
								dest.add<u8>(curr);
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
				u8 curr = text.get<u8>(pos);
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
		playerEnd,
		conditionEnd, conditionIndex,
		actionEnd, actionIndex,
		flagsEnd,
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
					if ( !ParsePartZero(text, output, error, pos, line, expecting) )
						return false;
					break;

				case 1:	//		%PlayerName,
						// or	%PlayerName:Value,
						// or	%PlayerName:Value)
						// or	%PlayerName)
						// or	)
					if ( !ParsePartOne(text, output, error, pos, line, expecting, playerEnd, lineEnd, currTrig) )
						return false;
					break;
	
				case 2: //		{
					if ( !ParsePartTwo(text, output, error, pos, line, expecting) )
						return false;
					break;
	
				case 3: //		conditions:
						// or	actions:
						// or	flags:
						// or	}
					if ( !ParsePartThree(text, output, error, pos, line, expecting) )
						return false;
					break;
	
				case 4:	//		%ConditionName(
						// or	;%ConditionName(
						// or	actions:
						// or	flags:
						// or	}
					if ( !ParsePartFour(text, output, error, pos, line, expecting, conditionEnd, lineEnd, conditionIndex,
							flags, argsLeft, numConditions, currCondition, currTrig) )
						return false;
					break;
	
				case 5: //		);
						// or	%ConditionArg,
						// or	%ConditionArg);
					if ( !ParsePartFive(text, output, error, pos, line, expecting, argsLeft, argEnd, currCondition, conditionIndex) )
						return false;
					break;
	
				case 6: //		actions:
						// or	flags:
						// or	}
					if ( !ParsePartSix(text, output, error, pos, line, expecting) )
						return false;
					break;
	
				case 7: //		%ActionName(
						// or	;%ActionName(
						// or	flags:
						// or	}
					if ( !ParsePartSeven(text, output, error, pos, line, expecting, flags, actionEnd, lineEnd,
										 actionIndex, argsLeft, numActions, currAction, currTrig) )
						return false;
					break;
	
				case 8: //		);
						// or	%ActionArg,
						// or	%ActionArg);
					if ( !ParsePartEight(text, output, error, pos, line, expecting, argsLeft, argEnd, currAction, actionIndex) )
						return false;
					break;
	
				case 9: //		}
						// or	flags:,
					if ( !ParsePartNine(text, output, error, pos, line, expecting) )
						return false;
					break;

				case 10: //		;
						 //	or	%32BitFlags;
					if ( !ParsePartTen(text, output, error, pos, line, expecting, flagsEnd, currTrig) )
						return false;
					break;

				case 11: //		}
					if ( !ParsePartEleven(text, output, error, pos, line, expecting) )
						return false;
					break;

				case 12: // Trigger end was found, reset
					{
						numConditions = 0;
						numActions = 0;

						if ( !output.add<Trigger&>(currTrig) )
						{
							std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Failed to add trigger.\n\n%s", LastError);
							return false;
						}

						std::memset(&currTrig, 0, sizeof(Trigger));
						expecting = 0;
						if ( text.has('\0', pos) ) // End of Text
						{
							std::strcpy(error, "Success!");
							return true;
						}
					}
					break;
			}
		}
	}
	return true;
}

inline bool TextTrigCompiler::ParsePartZero(buffer &text, buffer &output, char* error, u32 &pos, u32 &line, u32 &expecting)
{
	//		trigger
	// or	%NULL
	if ( text.has("TRIGGER(", pos, 8) )
	{
		pos += 8;
		expecting ++;
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
			std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \'(\'", line);
			return false;
		}
	}
	else 
	{
		std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \"Trigger\" or End of Text", line);
		return false;
	}
	return true;
}

inline bool TextTrigCompiler::ParsePartOne(buffer &text, buffer &output, char* error, u32 &pos, u32 &line, u32 &expecting, u32 &playerEnd, u32 &lineEnd, Trigger &currTrig)
{
	//		%PlayerName,
	// or	%PlayerName:Value,
	// or	%PlayerName)
	// or	%PlayerName:Value)
	// or	)
	if ( text.get<u8>(pos) == ')' ) // No players
	{
		pos ++;
		expecting ++;
	}
	else
	{
		if ( text.getNextUnquoted(',', pos, playerEnd, '{') || text.getNextUnquoted(')', pos, playerEnd, '{') )
		{
			lineEnd = u32_max;
			if ( !text.getNext('\15', pos, lineEnd) )
				text.getNext('\0', pos, lineEnd); // Text ends on this line
			
			playerEnd = SmallestOf(playerEnd, lineEnd);

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
				std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: Player Identifier", line);
				return false;
			}
		}
		else
		{
			std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \',\' or \')\'", line);
			return false;
		}
	}
	return true;
}

inline bool TextTrigCompiler::ParsePartTwo(buffer& text, buffer& output, char* error, u32 &pos, u32 &line, u32 &expecting)
{
	//		{
	if ( text.has('{', pos) )
	{
		pos ++;
		expecting ++;
	}
	else
	{
		std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: '{'", line);
		return false;
	}
	return true;
}

inline bool TextTrigCompiler::ParsePartThree(buffer& text, buffer& output, char* error, u32 &pos, u32 &line, u32 &expecting)
{
	//		conditions:
	// or	actions:
	// or	flags:
	// or	}
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
	else if ( text.has("FLAGS:", pos, 6) )
	{
		pos += 6;
		expecting += 7;
	}
	else if ( text.has('}', pos ) )
	{
		pos ++;
		expecting = 12;
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
			std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \':\'", line);
		}
		else
			std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \"Conditions\" or \"Actions\" or \"Flags\" or \'}\'", line);

		return false;
	}
	return true;
}

inline bool TextTrigCompiler::ParsePartFour(buffer& text, buffer& output, char* error, u32 &pos, u32 &line, u32 &expecting,
	u32 &conditionEnd, u32 &lineEnd, u32 &conditionIndex, u8 &flags, u32 &argsLeft, u32 &numConditions,
	Condition*& currCondition, Trigger &currTrig)
{
	//		%ConditionName(
	// or	;%ConditionName(
	// or	actions:
	// or	flags:
	// or	}
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
				if ( numConditions > NUM_TRIG_CONDITIONS )
				{
					std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nCondition Max Exceeded!", line);
					return false;
				}
				currCondition = &currTrig.conditions[numConditions];
				currCondition->flags = flags | CONDITION_FLAG_DISABLED;
				if ( ((s32)conditionIndex) < 0 )
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
					std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \'(\'", line);
					return false;
				}
			}
			else
			{
				std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: Condition Name", line);
				return false;
			}
		}
		else
		{
			std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \'(\'", line);
			return false;
		}
	}
	else if ( text.has('}', pos ) ) // End trigger
	{
		pos ++;
		expecting = 12;
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
			std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \':\'", line);
			return false;
		}
	}
	else if ( text.has("FLAGS", pos, 5) ) // End conditions, no actions
	{
		pos += 5;
		if ( text.has(':', pos) )
		{
			pos ++;
			expecting = 10;
		}
		else
		{
			std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \':\'", line);
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
			if ( numConditions > NUM_TRIG_CONDITIONS )
			{
				std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nCondition Max Exceeded!", line);
				return false;
			}
			currCondition = &currTrig.conditions[numConditions];
			currCondition->flags = flags;
			if ( ((s32)conditionIndex) < 0 )
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
				std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \'(\'", line);
				return false;
			}
		}
		else
		{
			std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: Condition Name", line);
			return false;
		}
	}
	else
	{
		std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \'(\'", line);
		return false;
	}
	return true;
}

inline bool TextTrigCompiler::ParsePartFive(buffer& text, buffer& output, char* error, u32 &pos, u32 &line, u32 &expecting, u32 &argsLeft, u32 &argEnd,
											Condition*& currCondition, u32 &conditionIndex)
{
	//		);
	// or	%ConditionArg,
	// or	%ConditionArg);
	if ( text.has(')', pos) ) // Condition End
	{
		if ( argsLeft > 0 )
		{
			std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: Condition Argument", line);
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
			std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \';\'", line);
			return false;
		}
	}
	else if ( argsLeft == 0 )
	{
		std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \'(\' or \'}\'", line);
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
				std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\n%s", line, LastError);
				return false;
			}
		}
		else
		{
			std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \')\'", line);
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
			std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\n%s", line, LastError);
			return false;
		}
	}
	else
	{
		std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \',\'", line);
		return false;
	}
	return true;
}

inline bool TextTrigCompiler::ParsePartSix(buffer& text, buffer& output, char* error, u32 &pos, u32 &line, u32 &expecting)
{
	//		actions:
	// or	flags:
	// or	}
	if ( text.has('}', pos) )
	{
		pos ++;
		expecting = 12;
	}
	else if ( text.has("ACTIONS:", pos, 8) )
	{
		pos += 8;
		expecting ++;
	}
	else if ( text.has("FLAGS:", pos, 6) )
	{
		pos += 6;
		expecting = 10;
	}
	else
	{
		if ( text.has("ACTIONS", pos, 7) || text.has("FLAGS", pos, 5) )
			std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \':\'", line);
		else
			std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \"Actions\" or \"Flags\" or '}'", line);
		return false;
	}
	return true;
}

inline bool TextTrigCompiler::ParsePartSeven(buffer& text, buffer& output, char* error, u32 &pos, u32 &line, u32 &expecting,
	u8 &flags, u32 &actionEnd, u32 &lineEnd, u32 &actionIndex, u32 &argsLeft, u32 &numActions,
	Action*& currAction, Trigger &currTrig)
{
	//		%ActionName(
	// or	;%ActionName(
	// or	flags:
	// or	}
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
				if ( numActions > NUM_TRIG_ACTIONS )
				{
					std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nAction Max Exceeded!", line);
					return false;
				}

				currAction = &currTrig.actions[numActions];
				currAction->flags = flags | ACTION_FLAG_DISABLED;
				if ( ((s32)actionIndex) < 0 )
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
				std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: Action Name or \'}\'", line);
				return false;
			}
		}
		else
		{
			std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: '('", line);
			return false;
		}
	}
	else if ( text.has('}', pos) )
	{
		pos ++;
		expecting = 12;
	}
	else if ( text.has("FLAGS", pos, 5) ) // End actions
	{
		pos += 5;
		if ( text.has(':', pos) )
		{
			pos ++;
			expecting = 10;
		}
		else
		{
			std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \':\'", line);
			return false;
		}
	}
	else if ( text.getNext('(', pos, actionEnd) )
	{
		if ( !text.getNext('\15', pos, lineEnd) )
			text.getNext(NULL, pos, lineEnd);

		actionEnd = std::min(actionEnd, lineEnd);

		if ( ParseAction(text, pos, actionEnd, false, actionIndex, flags, argsLeft) )
		{
			if ( numActions > NUM_TRIG_ACTIONS )
			{
				std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nAction Max Exceeded!", line);
				return false;
			}
			currAction = &currTrig.actions[numActions];
			currAction->flags = flags;
			if ( ((s32)actionIndex) < 0 )
				currAction->action = ExtendedToRegularAID(actionIndex);
			else
				currAction->action = u8(actionIndex);
			numActions ++;

			pos = actionEnd+1;
			expecting ++;
		}
		else
		{
			std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: Action Name or \'}\'", line);
			return false;
		}
	}
	else
	{
		std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \'(\'", line);
		return false;
	}
	return true;
}

inline bool TextTrigCompiler::ParsePartEight(buffer& text, buffer& output, char* error, u32 &pos, u32 &line, u32 &expecting,
	u32 &argsLeft, u32 &argEnd, Action*& currAction, u32 &actionIndex)
{
	//		);
	// or	%ActionArg,
	// or	%ActionArg);
	if ( text.has(')', pos) ) // Action End
	{
		if ( argsLeft > 0 )
		{
			std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: Action Argument", line);
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
			std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \';\'", line);
			return false;
		}
	}
	else if ( argsLeft == 0 )
	{
		std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \')\'", line);
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
				std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: Action Argument\n\n%s", line, LastError);
				return false;
			}
		}
		else
		{
			std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \')\'", line);
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
			std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: Action Argument\n\n%s", line, LastError);
			return false;
		}
	}
	else
	{
		std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: Additional Arguments\n\n%s", line, LastError);
		return false;
	}

	return true;
}

inline bool TextTrigCompiler::ParsePartNine(buffer& text, buffer& output, char* error, u32 &pos, u32 &line, u32 &expecting)
{
	//		}
	// or	flags:,
	if ( text.has('}', pos) )
	{
		pos ++;
		expecting = 12;
	}
	else if ( text.has("FLAGS:", pos, 6) )
	{
		pos += 6;
		expecting ++;
	}
	else
	{
		if ( text.has("FLAGS", pos, 5) )
			std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \':\'", line);
		else
			std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \"Flags\" or \'}\'", line);
		return false;
	}
	return true;
}

inline bool TextTrigCompiler::ParsePartTen(buffer& text, buffer& output, char*error, u32 &pos, u32 &line, u32 &expecting,
	u32 &flagsEnd, Trigger& currTrig)
{
	//		;
	//	or	%32BitFlags;
	if ( text.has(';', pos) )
	{
		pos ++;
		expecting ++;
	}
	else if ( text.getNext(';', pos, flagsEnd) )
	{
		if ( ParseExecutionFlags(text, pos, flagsEnd, currTrig.internalData) )
		{
			pos = flagsEnd+1;
			expecting ++;
		}
		else
		{
			std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: Binary Execution Flags (32-bit max).", line);
			return false;
		}
	}
	else
	{
		std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \';\'", line);
		return false;
	}
	return true;
}

inline bool TextTrigCompiler::ParsePartEleven(buffer& text, buffer& output, char* error, u32 &pos, u32 &line, u32 &expecting)
{
	//		}
	if ( text.has('}', pos) )
	{
		pos ++;
		expecting ++;
	}
	else
	{
		std::snprintf(error, MAX_ERROR_MESSAGE_SIZE, "Line: %u\n\nExpected: \'}\'", line);
		return false;
	}
	return true;
}

bool TextTrigCompiler::ParseExecutingPlayer(buffer &text, Trigger &currTrig, u32 pos, u32 end)
{
	u32 group;
	u32 separator;
	if ( text.getNextUnquoted(':', pos, end, separator) &&
		 ParsePlayer(text, group, pos, separator) &&
		 group < 28 )
	{
		u8 appendedValue;
		if ( !ParseByte((char*)text.getPtr(0), appendedValue, separator+1, end) )
			appendedValue = 1;

		currTrig.players[group] = appendedValue;
		return true;
	}
	else if ( ParsePlayer(text, group, pos, end) && group < 28 )
	{
		currTrig.players[group] = 1;
		return true;
	}
	return false;
}

bool TextTrigCompiler::ParseConditionName(buffer &arg, u32 &ID)
{
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
					else if ( arg.size() == 15 )
						ID = CID_COMMAND_THE_LEAST;
				}
				else if ( arg.has("THEMOST", 7, 7) )
				{
					if ( arg.has("AT", 14, 2) )
						ID = CID_COMMAND_THE_MOST_AT;
					else if ( arg.size() == 14 )
						ID = CID_COMMAND_THE_MOST;
				}
				else if ( arg.has("STHEMOSTAT", 7, 10) ) // command'S', added for backwards compatibility
					ID = CID_COMMAND_THE_MOST_AT;
				else if ( arg.size() == 7 )
					ID = CID_COMMAND;
			}
			else if ( arg.has("OUNTDOWNTIMER", 1, 13) )
				ID = CID_COUNTDOWN_TIMER;
			else if ( arg.has("USTOM", 1, 5) )
				ID = CID_CUSTOM;
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

	return ID != CID_NO_CONDITION;
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

	ParseConditionName(arg, ID);

	flags = defaultConditionFlags(ID);
	argsLeft = numConditionArgs(ID);
	
	return ID != CID_NO_CONDITION;
}

bool TextTrigCompiler::ParseActionName(buffer &arg, u32 &ID)
{
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
				else if ( arg.size() == 10 )
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
				else if ( arg.size() == 8 )
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
						else if ( arg.size() == 22 )
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
						else if ( arg.size() == 18 )
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
				else if ( arg.size() == 10 )
					ID = AID_REMOVE_UNIT;
			}
			else if ( arg.has("UNAISCRIPT", 1, 10) )
			{
				if ( arg.has("ATLOCATION", 11, 10) )
					ID = AID_RUN_AI_SCRIPT_AT_LOCATION;
				else if ( arg.size() == 11 )
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
					ID = AID_SET_NEXT_SCENARIO;
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

	return ID != AID_NO_ACTION;
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
				else if ( arg.size() == 10 )
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
				else if ( arg.size() == 8 )
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
						else if ( arg.size() == 22 )
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
						else if ( arg.size() == 18 )
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
				else if ( arg.size() == 10 )
					ID = AID_REMOVE_UNIT;
			}
			else if ( arg.has("UNAISCRIPT", 1, 10) )
			{
				if ( arg.has("ATLOCATION", 11, 10) )
					ID = AID_RUN_AI_SCRIPT_AT_LOCATION;
				else if ( arg.size() == 11 )
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

	flags = defaultActionFlags(ID);
	argsLeft = numActionArgs(ID);

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
#define returnMsg(condition, msg)							\
	if ( condition )										\
		return true;										\
	else {													\
		std::snprintf(LastError, MAX_ERROR_LENGTH, msg);	\
		return false;										\
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

	Error("INTERNAL ERROR: Invalid args left or argument unhandled, report this");
	return false;

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
			   ParseScript(text, currAction.number, pos, end) ||
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
	returnMsg ( ParseScript(text, currAction.number, pos, end) ||
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

bool TextTrigCompiler::ParseExecutionFlags(buffer& text, u32 pos, u32 end, u32& flags)
{
	flags = 0;

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

	char* argData = (char*)arg.getPtr(0);
	return ParseBinaryLong(argData, flags, 0, arg.size());
}

bool TextTrigCompiler::ParseString(buffer &text, u32& dest, u32 pos, u32 end)
{
	if ( text.hasCaseless("No String", pos, 9) || text.hasCaseless("\"No String\"", pos, 11) )
	{
		dest = 0;
		return true;
	}

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
			return ParseLong((char*)text.getPtr(0), dest, pos, end);
	}
	else
		return ParseLong((char*)text.getPtr(0), dest, pos, end);

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
			foreachin(pair, range)
			{
				StringTableNode &node = pair->second;
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

			if ( (isExtended && extendedStrUsage.useNext(node.stringNum)) ||
				(!isExtended && strUsage.useNext(node.stringNum)) )
			{
				addedStrings.push_back(node); // Add to the addedStrings list so it can be added to the map after compiling
				stringTable.insert(std::pair<u32, StringTableNode>(strHash(node.string), node)); // Add to search tree for recycling
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
	else if ( ParseLong((char*)text.getPtr(0), dest, pos, end) )
		return true;

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

		if ( str.compare("anywhere") == 0 ) // Capitalize lower-case anywhere's
		{
			locStringPtr[0] = 'A';
			str[0] = 'A';
		}

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
			foreachin(pair, range)
			{
				LocationTableNode &node = pair->second;
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
	else if ( ParseShort((char*)text.getPtr(0), dest, pos, end) )
		return true;
	
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
				foreachin(pair, range)
				{
					UnitTableNode &node = pair->second;
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
	if ( text.hasCaseless("No WAV", 0, 6) || text.hasCaseless("\"No WAV\"", 0, 8) )
	{
		dest = 0;
		return true;
	}
	else
		return ParseString(text, dest, pos, end);
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
	else if ( ParseLong((char*)text.getPtr(0), dest, pos, end) )
		return true;

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
		else if ( arg.has("ONE", 1, 3) )
		{
			// Do something with 'none' players (ID:12)
			dest = 12;
			return true;
		}
		else if ( arg.has("ONAVPLAYERS", 1, 11) )
		{
			// Do something with non av players
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
		else if ( arg.has("NUSED1", 1, 6) )
		{
			dest = 22;
			return true;
		}
		else if ( arg.has("NUSED2", 1, 6) )
		{
			dest = 23;
			return true;
		}
		else if ( arg.has("NUSED3", 1, 6) )
		{
			dest = 24;
			return true;
		}
		else if ( arg.has("NUSED4", 1, 6) )
		{
			dest = 25;
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
	
	// Might be a defined group name
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
			foreachin(pair, range)
			{
				GroupTableNode &node = pair->second;
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

bool TextTrigCompiler::ParseSwitch(buffer &text, u8 &dest, u32 pos, u32 end)
{
	if ( text.get<u8>(pos) == '\"' ) // If quoted, ignore quotes
	{
		pos ++;
		end --;
	}
	else if ( ParseByte((char*)text.getPtr(0), dest, pos, end) )
		return true;
	
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
				foreachin(pair, range)
				{
					SwitchTableNode &node = pair->second;
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

bool TextTrigCompiler::ParseScript(buffer &text, u32 &dest, u32 pos, u32 end)
{
	if ( text.hasCaseless("NOSCRIPT", pos, 8) || text.hasCaseless("No Script", pos, 9) || text.hasCaseless("\"No Script\"", pos, 11) )
	{
		dest = 0;
		return true;
	}

	int size = end - pos;
	bool isQuoted = text.get<u8>(pos) == '\"';
	if ( isQuoted )
	{
		pos++;
		end--;
		size -= 2;
	}

	if ( size < 1 )
		return false;

	char* scriptStringPtr;
	bool success = false;

	if ( text.getPtr<char>(scriptStringPtr, pos, size + 1) )
	{ // At least one character must come after the script string
	  // Temporarily replace next char with NUL char
		char temp = scriptStringPtr[size];
		scriptStringPtr[size] = '\0';
		std::string str(scriptStringPtr);

		// Grab the string hash
		u32 hash = strHash(str);
		int numMatching = scriptTable.count(hash);
		if ( numMatching == 1 )
		{ // Should guarentee that you can find at least one entry
			ScriptTableNode &node = scriptTable.find(hash)->second;
			if ( node.scriptName.compare(scriptStringPtr) == 0 )
			{
				dest = node.scriptID;
				success = true;
			}
		}
		else if ( numMatching > 1 )
		{
			auto range = scriptTable.equal_range(hash);
			foreachin(pair, range)
			{
				ScriptTableNode &node = pair->second;
				if ( node.scriptName.compare(scriptStringPtr) == 0 && success == false ) // Compare equal and no prev matches
				{
					dest = node.scriptID;
					success = true;
					break;
				}
			}
		}
		scriptStringPtr[size] = temp;
	}

	if ( !success && size == 4 )
	{
		/** With scripts, the exact ascii characters entered can be the exact bytes out.
			As a consequence, if the script name is not quoted and is comprised entirely
			of numbers, it must be considered a script number, and this method should
			return false so ParseByte can be called. */

		bool hasNonNumericCharacter =
			text.get<char>(pos) < '0' || text.get<char>(pos) > '9' ||
			text.get<char>(pos + 1) < '0' || text.get<char>(pos + 1) > '9' ||
			text.get<char>(pos + 2) < '0' || text.get<char>(pos + 2) > '9' ||
			text.get<char>(pos + 3) < '0' || text.get<char>(pos + 3) > '9';

		if ( isQuoted || hasNonNumericCharacter )
		{
			dest = text.get<u32>(pos);
			success = true;
		}
	}
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

bool TextTrigCompiler::PrepLocationTable(ScenarioPtr map)
{
	ChkLocation* loc;
	u16 stringNum;
	LocationTableNode locNode;
	buffer& MRGN = map->MRGN();
	if ( MRGN.exists() && map->STR().exists() )
	{
		locationTable.reserve(MRGN.size()/sizeof(ChkLocation)+1);
		locNode.locationNum = 0;
		locNode.locationName = "No Location";
		locationTable.insert(std::pair<u32, LocationTableNode>(strHash(locNode.locationName), locNode));
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
					locationTable.insert( std::pair<u32, LocationTableNode>(strHash(locNode.locationName), locNode) );
				}
				else if ( loc->stringNum > 0 && map->GetString(locNode.locationName, loc->stringNum) )
				{
					locNode.locationNum = u8(i+1);
					locationTable.insert( std::pair<u32, LocationTableNode>(strHash(locNode.locationName), locNode) );
				}
			}
		}
		locationTable.reserve(locationTable.size());
	}
	return true;
}

bool TextTrigCompiler::PrepUnitTable(ScenarioPtr map)
{
	UnitTableNode unitNode;
	buffer& unitSettings = map->unitSettings();
	if ( unitSettings.exists() && map->STR().exists() )
	{
		u16 stringID;
		for ( int unitID=0; unitID<228; unitID++ )
		{
			if ( unitSettings.get<u16>(stringID, unitID*2+(u32)UnitSettingsDataLoc::StringIds) &&
				 stringID > 0 )
			{
				unitNode.unitID = unitID;
				map->GetString(unitNode.unitName, stringID);
				unitTable.insert( std::pair<u32, UnitTableNode>(strHash(unitNode.unitName), unitNode) );
			}

			RawString regUnitName;
			map->getUnitName(regUnitName, unitID);
			if ( regUnitName.compare(unitNode.unitName) != 0 )
			{
				unitNode.unitName = regUnitName;
				unitTable.insert( std::pair<u32, UnitTableNode>(strHash(unitNode.unitName), unitNode) );
			}
		}
	}
	return true;
}

bool TextTrigCompiler::PrepSwitchTable(ScenarioPtr map)
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
				 map->GetString(switchNode.switchName, stringID) )
			{
				switchNode.switchNum = u8(switchID);
				switchTable.insert( std::pair<u32, SwitchTableNode>(strHash(switchNode.switchName), switchNode) );				
			}
		}
	}
	return true;
}

bool TextTrigCompiler::PrepGroupTable(ScenarioPtr map)
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
				 map->GetString(groupNode.groupName, stringID) )
			{
				groupNode.groupID = i+18;
				groupTable.insert( std::pair<u32, GroupTableNode>(strHash(groupNode.groupName), groupNode) );
			}
		}
	}
	return true;
}

bool TextTrigCompiler::PrepStringTable(ScenarioPtr chk)
{
	if ( chk->STR().exists() )
	{
		StringTableNode node;
#define AddStrIffOverZero(index)															\
			if ( index > 0 && chk->GetString(node.string, index) ) {								\
				node.stringNum = index;																\
				node.isExtended = chk->isExtendedString(node.stringNum);							\
				if ( !strIsInHashTable(node.string, strHash, stringTable) ) {						\
					stringTable.insert( std::pair<u32, StringTableNode>(strHash(node.string), node) );	\
				}																					\
			}

		ChkLocation* loc;
		for ( u32 i=0; i<chk->MRGN().size()/CHK_LOCATION_SIZE; i++ )
		{
			if ( chk->getLocation(loc, u16(i)) )
				AddStrIffOverZero(loc->stringNum);
		}

		Trigger* trig;
		int trigNum = 0;
		while ( chk->getTrigger(trig, trigNum) )
		{
			for ( int i=0; i<NUM_TRIG_ACTIONS; i++ )
			{
				AddStrIffOverZero( trig->actions[i].stringNum );
				AddStrIffOverZero( trig->actions[i].wavID );
			}

			trigNum ++;
		}

		trigNum = 0;
		while ( chk->getBriefingTrigger(trig, trigNum) )
		{
			for ( int i=0; i<NUM_TRIG_ACTIONS; i++ )
			{
				AddStrIffOverZero( trig->actions[i].stringNum );
				AddStrIffOverZero( trig->actions[i].wavID );
			}

			trigNum ++;
		}
	
		AddStrIffOverZero( chk->SPRP().get<u16>(0) ); // Scenario Name
		AddStrIffOverZero( chk->SPRP().get<u16>(2) ); // Scenario Description

		for ( int i=0; i<4; i++ )
			AddStrIffOverZero( chk->getForceStringNum(i) );

		for ( u32 i=0; i<chk->WAV().size()/4; i++ )
			AddStrIffOverZero( chk->WAV().get<u32>(i*4) );

		for ( int i=0; i<228; i++ )
		{
			AddStrIffOverZero( chk->UNIS().get<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds) );
			AddStrIffOverZero( chk->UNIx().get<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds) );
		}

		for ( int i=0; i<256; i++ )
			AddStrIffOverZero( chk->SWNM().get<u32>(i*4) );

	}
	return true;
}

bool TextTrigCompiler::PrepScriptTable(ScData &scData)
{
	std::string aiName;
	int numScripts = scData.aiScripts.GetNumAiScripts();
	for ( int i = 0; i < numScripts; i++ )
	{
		ScriptTableNode scriptNode;
		if ( scData.aiScripts.GetAiIdAndName(i, scriptNode.scriptID, scriptNode.scriptName) )
			scriptTable.insert(std::pair<u32, ScriptTableNode>(strHash(scriptNode.scriptName), scriptNode));
	}
	return true;
}

bool TextTrigCompiler::BuildNewStringTable(ScenarioPtr map)
{
	std::vector<StringTableNode> standardStrList;
	std::vector<StringTableNode> extendedStrList;

	try { // Include all strings added by text trigs
		for ( auto &str : addedStrings )
		{
			if ( str.isExtended )
				extendedStrList.push_back(str);
			else
				standardStrList.push_back(str);
		}
	} catch ( std::bad_alloc ) {
		std::snprintf(LastError, MAX_ERROR_MESSAGE_SIZE, "Out of memory!");
		return false;
	}

	if ( map->addAllUsedStrings(standardStrList, true, false) &&
		 map->rebuildStringTable(standardStrList, false) )
	{
		if ( map->KSTR().exists() && extendedStrList.size() > 0 ) // Has extended strings
		{
			return map->addAllUsedStrings(extendedStrList, false, true) && 
				   map->rebuildStringTable(extendedStrList, true);
		}
		else
			return true;
	}
	else
		return false;
}
