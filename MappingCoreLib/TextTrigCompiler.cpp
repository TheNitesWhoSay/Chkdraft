#include "TextTrigCompiler.h"
#include "EscapeStrings.h"
#include "Math.h"
#include <cstdio>
#include <cstring>
#include <exception>
#include <string>
#include <utility>
#include <vector>
#undef PlaySound

TextTrigCompiler::TextTrigCompiler(bool useAddressesForMemory, u32 deathTableOffset) : useAddressesForMemory(useAddressesForMemory), deathTableOffset(deathTableOffset)
{

}

TextTrigCompiler::~TextTrigCompiler()
{

}

bool TextTrigCompiler::CompileTriggers(std::string trigText, ScenarioPtr chk, ScData & scData)
{
    buffer text("TxTr");
    return text.addStr(trigText.c_str(), trigText.length()) && CompileTriggers(text, chk, scData);
}

bool TextTrigCompiler::CompileTriggers(buffer & text, ScenarioPtr chk, ScData & scData)
{
    if ( !LoadCompiler(chk, scData) )
        return false;

    try
    {
        CleanText(text);

        std::deque<std::shared_ptr<Chk::Trigger>> triggers;
        std::stringstream compilerError;
        std::stringstream stringTableError;

        if ( ParseTriggers(text, triggers, compilerError) )
        {
            if ( BuildNewStringTable(chk, stringTableError) )
            {
                chk->triggers.trig->swap(triggers);
                return true;
            }
            else
                compilerError << "No text errors, but compilation must abort due to errors recompiling strings." << std::endl << std::endl << stringTableError.str();
        }

        CHKD_ERR(compilerError.str());
    }
    catch ( std::bad_alloc ) { CHKD_ERR("Compilation aborted due to low memory."); }
    return false;
}

bool TextTrigCompiler::CompileTrigger(std::string trigText, Chk::Trigger* trigger, ScenarioPtr chk, ScData & scData)
{
    buffer text("TxTr");
    return text.addStr(trigText.c_str(), trigText.length()+1) && CompileTrigger(text, trigger, chk, scData);
}

bool TextTrigCompiler::CompileTrigger(buffer & text, Chk::Trigger* trigger, ScenarioPtr chk, ScData & scData)
{
    if ( !LoadCompiler(chk, scData) )
        return false;

    try
    {
        CleanText(text);
        std::deque<std::shared_ptr<Chk::Trigger>> triggers;
        std::stringstream compilerError;
        std::stringstream stringTableError;

        if ( ParseTriggers(text, triggers, compilerError) )
        {
            if ( BuildNewStringTable(chk, stringTableError) )
            {
                if ( triggers.size() == 0 )
                {
                    *trigger = *triggers[0];
                    return true;
                }
                else
                    compilerError << "No text errors, but trigger could not be transferred.";
            }
            else
                compilerError << "No text errors, but compilation must abort due to errors recompiling strings." << std::endl << std::endl << stringTableError.str();
        }

        CHKD_ERR(compilerError.str());
    }
    catch ( std::bad_alloc ) { CHKD_ERR("Compilation aborted due to low memory."); }
    return false;
}

bool TextTrigCompiler::ParseConditionName(std::string text, Chk::Condition::Type & conditionType)
{
    buffer txcd("TxCd");
    if ( txcd.addStr(text.c_str(), text.size()) )
    {
        CleanText(txcd);
        Chk::Condition::VirtualType newConditionType = Chk::Condition::VirtualType::NoCondition;
        if ( ParseConditionName(txcd, newConditionType) && newConditionType != Chk::Condition::VirtualType::Custom )
        {
            if ( ((s32)newConditionType) < 0 )
                conditionType = ExtendedToRegularCID(newConditionType);
            else
                conditionType = (Chk::Condition::Type)newConditionType;

            return true;
        }
        else
        {
            u32 temp = 0;
            if ( ParseLong((char*)txcd.getPtr(0), temp, 0, txcd.size()) )
            {
                if ( ((s32)temp) < 0 )
                    conditionType = ExtendedToRegularCID((Chk::Condition::VirtualType)temp);
                else
                    conditionType = (Chk::Condition::Type)temp;

                return true;
            }
        }
    }
    return false;
}

bool TextTrigCompiler::ParseConditionArg(std::string conditionArgText, u8 argNum,
    std::vector<u8> & argMap, Chk::Condition & condition, ScenarioPtr chk, ScData & scData)
{
    if ( !LoadCompiler(chk, scData) )
        return false;

    buffer txcd("TxCd");
    if ( argNum < argMap.size() &&
        txcd.addStr(conditionArgText.c_str(), conditionArgText.size()) &&
        txcd.add<u8>(0) )
    {
        std::stringstream argumentError;
        u32 argsLeft = numConditionArgs((Chk::Condition::VirtualType)condition.conditionType) - argMap[argNum];
        if ( ParseConditionArg(txcd, condition, 0, txcd.size()-1, (Chk::Condition::VirtualType)condition.conditionType, argsLeft, argumentError) )
            return true;
        else
        {
            std::stringstream errorMessage;
            errorMessage << "Unable to parse condition. " << argumentError.str();
            CHKD_ERR(errorMessage.str());
        }
    }
    return false;
}

bool TextTrigCompiler::ParseActionName(std::string text, Chk::Action::Type & actionType)
{
    buffer txac("TxAc");
    if ( txac.addStr(text.c_str(), text.size()) )
    {
        CleanText(txac);
        Chk::Action::VirtualType newActionType = Chk::Action::VirtualType::NoAction;
        if ( ParseActionName(txac, newActionType) && newActionType != Chk::Action::VirtualType::Custom )
        {
            if ( ((s32)newActionType) < 0 )
                actionType = ExtendedToRegularAID(newActionType);
            else
                actionType = (Chk::Action::Type)newActionType;

            return true;
        }
        else
        {
            u32 temp = 0;
            if ( ParseLong((char*)txac.getPtr(0), temp, 0, txac.size()) )
            {
                if ( ((s32)temp) < 0 )
                    actionType = ExtendedToRegularAID((Chk::Action::VirtualType)temp);
                else
                    actionType = (Chk::Action::Type)temp;

                return true;
            }
        }
    }
    return false;
}

bool TextTrigCompiler::ParseActionArg(std::string actionArgText, u8 argNum,
    std::vector<u8> & argMap, Chk::Action & action, ScenarioPtr chk, ScData & scData)
{
    if ( !LoadCompiler(chk, scData) )
        return false;

    buffer txac("TxAc");
    if ( argNum < argMap.size() &&
        txac.addStr(actionArgText.c_str(), actionArgText.size()) &&
        txac.add<u8>(0) )
    {
        std::stringstream argumentError;
        u32 argsLeft = numActionArgs((Chk::Action::VirtualType)action.actionType) - argMap[argNum];
        if ( ParseActionArg(txac, action, 0, txac.size() - 1, (Chk::Action::VirtualType)action.actionType, argsLeft, argumentError) )
            return true;
        else
        {
            std::stringstream errorMessage;
            errorMessage << "Unable to parse action. " << argumentError.str();
            CHKD_ERR(errorMessage.str());
        }
    }
    return false;
}

u8 TextTrigCompiler::defaultConditionFlags(Chk::Condition::Type conditionType)
{
    u8 defaultFlags[] = { 0, 0, 16, 16, 0,  16, 16, 16, 16, 0,
        0, 0, 0, 0, 0,    16, 16, 16, 16, 0,
        0, 0, 0, 0 };

    if ( conditionType < sizeof(defaultFlags) / sizeof(const u8) )
        return defaultFlags[conditionType];
    else
        return 0;
}

u8 TextTrigCompiler::defaultActionFlags(Chk::Action::Type actionType)
{
    u8 defaultFlags[] = { 0, 4, 4, 4, 4,        4, 4, 0, 4, 0,
        4, 28, 0, 4, 4,       4, 4, 16, 16, 0,
        16, 0, 20, 20, 20,    20, 4, 4, 4, 20,
        4, 4, 4, 16, 16,      0, 16, 0, 20, 20,
        4, 4, 20, 20, 20,     20, 20, 0, 20, 20,
        20, 20, 4, 20, 4,     4, 4, 4, 0, 0 };

    if ( actionType < sizeof(defaultFlags) / sizeof(const u8) )
        return defaultFlags[actionType];
    else
        return 0;
}

u8 TextTrigCompiler::numConditionArgs(Chk::Condition::VirtualType conditionType)
{
    const u8 conditionNumArgs[] = { 0, 2, 4, 5, 4, 4, 1, 2, 1, 1,
        1, 2, 2, 0, 3, 4, 1, 2, 1, 1,
        1, 4, 0, 0 };

    if ( conditionType >= 0 && conditionType < sizeof(conditionNumArgs) / sizeof(const u8) )
        return conditionNumArgs[conditionType];
    else
        return ExtendedNumConditionArgs(conditionType);
}

u8 TextTrigCompiler::numActionArgs(Chk::Action::VirtualType actionType)
{
    const u8 actionNumArgs[] = { 0, 0, 0, 0, 1,  0, 0, 8, 2, 2,
        1, 5, 1, 2, 2,  1, 2, 2, 3, 2,
        2, 2, 2, 4, 2,  4, 4, 4, 1, 2,
        0, 0, 1, 3, 4,  3, 3, 3, 4, 5,
        1, 1, 4, 4, 4,  4, 5, 1, 5, 5,
        5, 5, 4, 5, 0,  0, 0, 2, 0, 0 };

    if ( actionType >= 0 && actionType < sizeof(actionNumArgs) / sizeof(const u8) )
        return actionNumArgs[actionType];
    else
        return ExtendedNumActionArgs(actionType);
}

// protected

bool TextTrigCompiler::LoadCompiler(ScenarioPtr chk, ScData & scData)
{
    ClearCompiler();
    stringUsed.reset();
    extendedStringUsed.reset();
    chk->strings.markValidUsedStrings(stringUsed, Chk::Scope::Either, Chk::Scope::Game);
    chk->strings.markValidUsedStrings(extendedStringUsed, Chk::Scope::Either, Chk::Scope::Either);

    return
        PrepLocationTable(chk) && PrepUnitTable(chk) && PrepSwitchTable(chk) &&
        PrepGroupTable(chk) && PrepStringTable(chk) && PrepScriptTable(scData);
}

void TextTrigCompiler::ClearCompiler()
{
    locationTable.clear();
    unitTable.clear();
    switchTable.clear();
    groupTable.clear();
    zzStringTable.clear();
    extendedStringTable.clear();

    zzAddedStrings.clear();
    addedExtendedStrings.clear();

    stringUsed.reset();
    extendedStringUsed.reset();
}

void TextTrigCompiler::CleanText(buffer & text)
{
    s64 pos = 0;
    bool inString = false;
    buffer dest("TeCp");
    dest.setSize(text.size()+1);

    while ( pos < text.size() ) 
    {
        u8 character = text.get<u8>(pos);
        pos++;
        switch ( character )
        {
        case ' ': case '\t':
            break; // Ignore (delete) spaces and tabs

        case '\r': // CR (line ending)
            if ( text.get<u8>(pos) == '\n' ) // Followed by LF
                pos++; // Increment position and continue to LF case
                       // Else continue to the LF case
        case '\n': // LF (line ending)
        case '\13': // VT (line ending)
        case '\14': // FF (line ending)
            dest.add<u8>('\r');
            dest.add<u8>('\n');
            break;

        case '/': // Might be a comment
            if ( text.get<u8>(pos) == '/' ) // Found a comment
            {
                s64 newPos = pos;
                if ( text.getNext('\n', pos, newPos) || // Check for nearby LF
                    text.getNext('\r', pos, newPos) || // Check for nearby CR (should be checked after LF)
                    text.getNext('\13', pos, newPos) || // Check for nearby VT
                    text.getNext('\14', pos, newPos) ) // Check for nearby FF
                {
                    pos = newPos; // Skip (delete) contents until line ending
                }
                else // File ended on this line
                    pos = text.size();
            }
            break;

        case '\"': // Found a string
        {
            dest.add<u8>('\"');

            s64 closeQuotePos = pos;
            if ( text.getNextUnescaped('\"', pos, closeQuotePos) )
            {
                while ( pos < closeQuotePos )
                {
                    u8 curr = text.get<u8>(pos);
                    if ( curr == '\\' ) // Escape Character
                    {
                        pos++;
                        if ( text.get<u8>(pos) == 'x' )
                        {
                            pos++;
                            pos++; // First num should always be 0

                            u8 targetVal = 0;

                            if ( text.get<u8>(pos) >= '0' && text.get<u8>(pos) <= '9' )
                                targetVal += 16 * (text.get<u8>(pos) - '0');
                            else if ( text.get<u8>(pos) >= 'A' && text.get<u8>(pos) <= 'F' )
                                targetVal += 16 * (text.get<u8>(pos) - 'A' + 10);

                            pos++;

                            if ( text.get<u8>(pos) >= '0' && text.get<u8>(pos) <= '9' )
                                targetVal += text.get<u8>(pos) - '0';
                            else if ( text.get<u8>(pos) >= 'A' && text.get<u8>(pos) <= 'F' )
                                targetVal += text.get<u8>(pos) - 'A' + 10;

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
                        else if ( text.get<u8>(pos) == '\"' )
                        {
                            dest.add<u8>('\\');
                            dest.add<u8>('\"');
                        }
                        else if ( text.get<u8>(curr, pos) )
                            dest.add<u8>(curr);
                    }
                    else if ( curr == '<' && text.get<u8>(pos + 3) == '>' &&
                        (text.get<u8>(pos+1) >= '0' && text.get<u8>(pos+1) <= '9' ||
                            text.get<u8>(pos+1) >= 'A' && text.get<u8>(pos+1) <= 'F') &&
                            (text.get<u8>(pos + 2) >= '0' && text.get<u8>(pos + 2) <= '9' ||
                                text.get<u8>(pos + 2) >= 'A' && text.get<u8>(pos + 2) <= 'F') )
                    {
                        pos++;

                        u8 targetVal = 0;

                        if ( text.get<u8>(pos) >= '0' && text.get<u8>(pos) <= '9' )
                            targetVal += 16 * (text.get<u8>(pos) - '0');
                        else if ( text.get<u8>(pos) >= 'A' && text.get<u8>(pos) <= 'F' )
                            targetVal += 16 * (text.get<u8>(pos) - 'A' + 10);

                        pos++;

                        if ( text.get<u8>(pos) >= '0' && text.get<u8>(pos) <= '9' )
                            targetVal += text.get<u8>(pos) - '0';
                        else if ( text.get<u8>(pos) >= 'A' && text.get<u8>(pos) <= 'F' )
                            targetVal += text.get<u8>(pos) - 'A' + 10;

                        dest.add<u8>(targetVal);

                        pos++;
                    }
                    else
                        dest.add<u8>(curr);

                    pos++;
                }

                dest.add<u8>('\"');
                pos = closeQuotePos+1;
            }
        }
        break;

        default:
            if ( character >= 'a' && character <= 'z' ) // If lowercase
                dest.add<u8>(character-32); // Captialize
            else
                dest.add<u8>(character); // Add character as normal
            break;
        }
    }
    text.overwrite((const char*)dest.getPtr(0), dest.size());
}

bool TextTrigCompiler::ParseTriggers(buffer & text, std::deque<std::shared_ptr<Chk::Trigger>> & output, std::stringstream & error)
{
    text.add<u8>(0); // Add a terminating null character

    u8 flags;

    s64 pos = 0,
        lineEnd = 0,
        playerEnd = 0,
        conditionEnd = 0,
        actionEnd = 0,
        flagsEnd = 0,
        argEnd = 0;

    u32 expecting = 0,
        line = 1,
        argsLeft = 0,
        numConditions = 0,
        numActions = 0;

    Chk::Condition::VirtualType conditionId;
    Chk::Action::VirtualType actionId;

    while ( pos < text.size() )
    {
        std::shared_ptr<Chk::Trigger> currTrig = std::shared_ptr<Chk::Trigger>(new Chk::Trigger());
        Chk::Condition* currCondition = &currTrig->condition(0);
        Chk::Action* currAction = &currTrig->action(0);

        if ( text.has('\15', pos) ) // Line End
        {
            pos += 2;
            line ++;
        }
        else
        {
            switch ( expecting )
            {
            case 0: //      trigger
                    // or   %NULL
                if ( !ParsePartZero(text, error, pos, line, expecting) )
                    return false;
                break;

            case 1: //      %PlayerName,
                    // or   %PlayerName:Value,
                    // or   %PlayerName:Value)
                    // or   %PlayerName)
                    // or   )
                if ( !ParsePartOne(text, *currTrig, error, pos, line, expecting, playerEnd, lineEnd) )
                    return false;
                break;

            case 2: //      {
                if ( !ParsePartTwo(text, *currTrig, error, pos, line, expecting) )
                    return false;
                break;

            case 3: //      conditions:
                    // or   actions:
                    // or   flags:
                    // or   }
                if ( !ParsePartThree(text, *currTrig, error, pos, line, expecting) )
                    return false;
                break;

            case 4: //      %ConditionName(
                    // or   ;%ConditionName(
                    // or   actions:
                    // or   flags:
                    // or   }
                if ( !ParsePartFour(text, *currTrig, error, pos, line, expecting, conditionEnd, lineEnd, conditionId,
                    flags, argsLeft, numConditions, currCondition) )
                    return false;
                break;

            case 5: //      );
                    // or   %ConditionArg,
                    // or   %ConditionArg);
                if ( !ParsePartFive(text, *currTrig, error, pos, line, expecting, argsLeft, argEnd, currCondition, conditionId) )
                    return false;
                break;

            case 6: //      actions:
                    // or   flags:
                    // or   }
                if ( !ParsePartSix(text, *currTrig, error, pos, line, expecting) )
                    return false;
                break;

            case 7: //      %ActionName(
                    // or   ;%ActionName(
                    // or   flags:
                    // or   }
                if ( !ParsePartSeven(text, *currTrig, error, pos, line, expecting, flags, actionEnd, lineEnd,
                    actionId, argsLeft, numActions, currAction) )
                    return false;
                break;

            case 8: //      );
                    // or   %ActionArg,
                    // or   %ActionArg);
                if ( !ParsePartEight(text, *currTrig, error, pos, line, expecting, argsLeft, argEnd, currAction, actionId) )
                    return false;
                break;

            case 9: //      }
                    // or   flags:,
                if ( !ParsePartNine(text, *currTrig, error, pos, line, expecting) )
                    return false;
                break;

            case 10: //     ;
                     // or  %32BitFlags;
                if ( !ParsePartTen(text, *currTrig, error, pos, line, expecting, flagsEnd) )
                    return false;
                break;

            case 11: //     }
                if ( !ParsePartEleven(text, *currTrig, error, pos, line, expecting) )
                    return false;
                break;

            case 12: // Trigger end was found, reset
            {
                numConditions = 0;
                numActions = 0;

                output.push_back(currTrig);
                expecting = 0;
                if ( text.has('\0', pos) ) // End of Text
                {
                    error << "Success!";
                    return true;
                }
            }
            break;
            }
        }
    }
    return true;
}

inline bool TextTrigCompiler::ParsePartZero(buffer & text, std::stringstream & error, s64 & pos, u32 & line, u32 & expecting)
{
    //      trigger
    // or   %NULL
    if ( text.has("TRIGGER(", pos, 8) )
    {
        pos += 8;
        expecting ++;
    }
    else if ( text.has("TRIGGER", pos, 7) )
    {
        pos += 7;
        while ( text.has('\r', pos) )
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
            error << "Line: " << line << std::endl << std::endl << "Expected: \'(\'";
            return false;
        }
    }
    else if ( text.has('\0', pos) ) // End of text
    {
        pos ++;
    }
    else
    {
        error << "Line: " << line << std::endl << std::endl << "Expected: \"Trigger\" or End of Text";
        return false;
    }
    return true;
}

inline bool TextTrigCompiler::ParsePartOne(buffer & text, Chk::Trigger & output, std::stringstream & error, s64 & pos, u32 & line, u32 & expecting, s64 & playerEnd, s64 & lineEnd)
{
    //      %PlayerName,
    // or   %PlayerName:Value,
    // or   %PlayerName)
    // or   %PlayerName:Value)
    // or   )
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
            if ( !text.getNextUnquoted('\15', pos, lineEnd) )
                text.getNext('\0', pos, lineEnd); // Text ends on this line

            playerEnd = std::min(playerEnd, lineEnd);

            if ( ParseExecutingPlayer(text, output, pos, playerEnd) )
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
                error << "Line: " << line << std::endl << std::endl << "Expected: Player Identifier";
                return false;
            }
        }
        else
        {
            error << "Line: " << line << std::endl << std::endl << "Expected: \',\' or \')\'";
            return false;
        }
    }
    return true;
}

inline bool TextTrigCompiler::ParsePartTwo(buffer & text, Chk::Trigger & output, std::stringstream & error, s64 & pos, u32 & line, u32 & expecting)
{
    //      {
    if ( text.has('{', pos) )
    {
        pos ++;
        expecting ++;
    }
    else
    {
        error << "Line: " << line << std::endl << std::endl << "Expected: '{'";
        return false;
    }
    return true;
}

inline bool TextTrigCompiler::ParsePartThree(buffer & text, Chk::Trigger & output, std::stringstream & error, s64 & pos, u32 & line, u32 & expecting)
{
    //      conditions:
    // or   actions:
    // or   flags:
    // or   }
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
        bool hasConditions = text.has("CONDITIONS", pos, 10);
        if ( hasConditions || text.has("ACTIONS", pos, 7) )
        {
            pos += hasConditions ? 10 : 7;
            while ( text.has('\15', pos) )
            {
                pos += 2;
                line ++;
            }

            if ( text.has(':', pos) ) 
            {
                pos ++;
                expecting += hasConditions ? 1 : 4;
            }
            else
            {
                error << "Line: " << line << std::endl << std::endl << "Expected: \':\'";
                return false;
            }
        }
        else
        {
            error << "Line: " << line << std::endl << std::endl << "Expected: \"Conditions\" or \"Actions\" or \"Flags\" or \'}\'";
            return false;
        }
    }
    return true;
}

inline bool TextTrigCompiler::ParsePartFour(buffer & text, Chk::Trigger & output, std::stringstream & error, s64 & pos, u32 & line, u32 & expecting,
    s64 & conditionEnd, s64 & lineEnd, Chk::Condition::VirtualType & conditionId, u8 & flags, u32 & argsLeft, u32 & numConditions,
    Chk::Condition* & currCondition)
{
    //      %ConditionName(
    // or   ;%ConditionName(
    // or   actions:
    // or   flags:
    // or   }
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
            if ( !text.getNextUnquoted('\r', pos, lineEnd) )
                text.getNext('\0', pos, lineEnd);

            conditionEnd = std::min(conditionEnd, lineEnd);

            if ( ParseCondition(text, pos, conditionEnd, true, conditionId, flags, argsLeft) )
            {
                if ( numConditions > Chk::Trigger::MaxConditions )
                {
                    error << "Line: " << line << std::endl << std::endl << "Condition Max Exceeded!";
                    return false;
                }
                currCondition = &output.condition(numConditions);
                currCondition->flags = flags | Chk::Condition::Flags::Disabled;
                if ( (s32)conditionId < 0 )
                    currCondition->conditionType = ExtendedToRegularCID(conditionId);
                else
                    currCondition->conditionType = (Chk::Condition::Type)conditionId;
                numConditions ++;

                pos = conditionEnd;
                while ( text.has('\r', pos) )
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
                    error << "Line: " << line << std::endl << std::endl << "Expected: \'(\'";
                    return false;
                }
            }
            else
            {
                error << "Line: " << line << std::endl << std::endl << "Expected: Condition Name";
                return false;
            }
        }
        else
        {
            error << "Line: " << line << std::endl << std::endl << "Expected: \'(\'";
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
        while ( text.has('\r', pos) )
        {
            pos += 2;
            line ++;
        }
        if ( text.has(':', pos) )
        {
            pos ++;
            expecting = 7;
        }
        else
        {
            error << "Line: " << line << std::endl << std::endl << "Expected: \':\'";
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
            error << "Line: " << line << std::endl << std::endl << "Expected: \':\'";
            return false;
        }
    }
    else if ( text.getNext('(', pos, conditionEnd) ) // Has a condition or an error
    {
        if ( !text.getNextUnquoted('\15', pos, lineEnd) )
            text.getNext('\0', pos, lineEnd);

        conditionEnd = std::min(conditionEnd, lineEnd);

        if ( ParseCondition(text, pos, conditionEnd, false, conditionId, flags, argsLeft) )
        {
            if ( numConditions > Chk::Trigger::MaxConditions )
            {
                error << "Line: " << line << std::endl << std::endl << "Condition Max Exceeded!";
                return false;
            }
            currCondition = &output.condition(numConditions);
            currCondition->flags = flags;
            if ( (s32)conditionId < 0 )
                currCondition->conditionType = ExtendedToRegularCID(conditionId);
            else
                currCondition->conditionType = (Chk::Condition::Type)conditionId;
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
                error << "Line: " << line << std::endl << std::endl << "Expected: \'(\'";
                return false;
            }
        }
        else
        {
            error << "Line: " << line << std::endl << std::endl << "Expected: Condition Name";
            return false;
        }
    }
    else
    {
        error << "Line: " << line << std::endl << std::endl << "Expected: \'(\'";
        return false;
    }
    return true;
}

inline bool TextTrigCompiler::ParsePartFive(buffer & text, Chk::Trigger & output, std::stringstream & error, s64 & pos, u32 & line, u32 & expecting, u32 & argsLeft, s64 & argEnd,
    Chk::Condition* & currCondition, Chk::Condition::VirtualType & conditionId)
{
    //      );
    // or   %ConditionArg,
    // or   %ConditionArg);
    if ( text.has(')', pos) ) // Condition End
    {
        if ( argsLeft > 0 )
        {
            error << "Line: " << line << std::endl << std::endl << "Expected: Condition Argument";
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
            error << "Line: " << line << std::endl << std::endl << "Expected: \';\'";
            return false;
        }
    }
    else if ( argsLeft == 0 )
    {
        error << "Line: " << line << std::endl << std::endl << "Expected: \')\'";
        return false;
    }
    else if ( argsLeft == 1 )
    {
        if ( text.getNextUnquoted(')', pos, argEnd) )
        {
            std::stringstream argumentError;
            if ( ParseConditionArg(text, *currCondition, pos, argEnd, conditionId, argsLeft, argumentError) )
            {
                pos = argEnd;
                argsLeft --;
            }
            else
            {
                error << "Line: " << line << std::endl << std::endl << argumentError.str();
                return false;
            }
        }
        else
        {
            error << "Line: " << line << std::endl << std::endl << "Expected: \')\'";
            return false;
        }
    }
    else if ( text.getNextUnquoted(',', pos, argEnd) ) // Has argument
    {
        std::stringstream argumentError;
        if ( ParseConditionArg(text, *currCondition, pos, argEnd, conditionId, argsLeft, argumentError) )
        {
            pos = argEnd+1;
            argsLeft --;
        }
        else
        {
            error << "Line: " << line << std::endl << std::endl << argumentError.str();
            return false;
        }
    }
    else
    {
        error << "Line: " << line << std::endl << std::endl << "Expected: \',\'";
        return false;
    }
    return true;
}

inline bool TextTrigCompiler::ParsePartSix(buffer & text, Chk::Trigger & output, std::stringstream & error, s64 & pos, u32 & line, u32 & expecting)
{
    //      actions:
    // or   flags:
    // or   }
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
            error << "Line: " << line << std::endl << std::endl << "Expected: \':\'";
        else
            error << "Line: " << line << std::endl << std::endl << "Expected: \"Actions\" or \"Flags\" or '}'";
        return false;
    }
    return true;
}

inline bool TextTrigCompiler::ParsePartSeven(buffer & text, Chk::Trigger & output, std::stringstream & error, s64 & pos, u32 & line, u32 & expecting,
    u8 & flags, s64 & actionEnd, s64 & lineEnd, Chk::Action::VirtualType & actionId, u32 & argsLeft, u32 & numActions,
    Chk::Action* & currAction)
{
    //      %ActionName(
    // or   ;%ActionName(
    // or   flags:
    // or   }
    if ( text.has(';', pos ) )
    {
        pos ++;
        if ( text.getNext('(', pos, actionEnd) )
        {
            if ( !text.getNextUnquoted('\15', pos, lineEnd) )
                text.getNext('\0', pos, lineEnd);

            actionEnd = std::min(actionEnd, lineEnd);

            if ( ParseAction(text, pos, actionEnd, true, actionId, flags, argsLeft) )
            {
                if ( numActions > Chk::Trigger::MaxActions )
                {
                    error << "Line: " << line << std::endl << std::endl << "Action Max Exceeded!";
                    return false;
                }
                currAction = &output.action(numActions);
                currAction->flags = flags | Chk::Action::Flags::Disabled;
                if ( actionId < 0 )
                    currAction->actionType = ExtendedToRegularAID(actionId);
                else
                    currAction->actionType = (Chk::Action::Type)actionId;
                numActions ++;

                pos = actionEnd+1;
                expecting ++;
            }
            else
            {
                error << "Line: " << line << std::endl << std::endl << "Expected: Action Name or \'}\'";
                return false;
            }
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
            error << "Line: " << line << std::endl << std::endl << "Expected: \':\'";
            return false;
        }
    }
    else if ( text.getNext('(', pos, actionEnd) )
    {
        if ( !text.getNextUnquoted('\15', pos, lineEnd) )
            text.getNext('\0', pos, lineEnd);

        actionEnd = std::min(actionEnd, lineEnd);

        if ( ParseAction(text, pos, actionEnd, false, actionId, flags, argsLeft) )
        {
            if ( numActions > Chk::Trigger::MaxActions )
            {
                error << "Line: " << line << std::endl << std::endl << "Action Max Exceeded!";
                return false;
            }
            currAction = &output.action(numActions);
            currAction->flags = flags;
            if ( (s32)actionId < 0 )
                currAction->actionType = ExtendedToRegularAID(actionId);
            else
                currAction->actionType = (Chk::Action::Type)actionId;
            numActions ++;

            pos = actionEnd+1;
            expecting ++;
        }
        else
        {
            error << "Line: " << line << std::endl << std::endl << "Expected: Action Name or \'}\'";
            return false;
        }
    }
    else
    {
        error << "Line: " << line << std::endl << std::endl << "Expected: \'(\'";
        return false;
    }
    return true;
}

inline bool TextTrigCompiler::ParsePartEight(buffer & text, Chk::Trigger & output, std::stringstream & error, s64 & pos, u32 & line, u32 & expecting,
    u32 & argsLeft, s64 & argEnd, Chk::Action* & currAction, Chk::Action::VirtualType & actionId)
{
    //      );
    // or   %ActionArg,
    // or   %ActionArg);
    if ( text.has(')', pos) ) // Action End
    {
        if ( argsLeft > 0 )
        {
            error << "Line: " << line << std::endl << std::endl << "Expected: Action Argument";
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
            error << "Line: " << line << std::endl << std::endl << "Expected: \';\'";
            return false;
        }
    }
    else if ( argsLeft == 0 )
    {
        error << "Line: " << line << std::endl << std::endl << "Expected: \')\'";
        return false;
    }
    else if ( argsLeft == 1 )
    {
        if ( text.getNextUnquoted(')', pos, argEnd) )
        {
            std::stringstream argumentError;
            if ( ParseActionArg(text, *currAction, pos, argEnd, actionId, argsLeft, argumentError) )
            {
                pos = argEnd;
                argsLeft --;
            }
            else
            {
                error << "Line: " << line << std::endl << std::endl << "Expected: Action Argument" << std::endl << std::endl << argumentError.str();
                return false;
            }
        }
        else
        {
            error << "Line: " << line << std::endl << std::endl << "Expected: \')\'";
            return false;
        }
    }
    else if ( text.getNextUnquoted(',', pos, argEnd) ) // Has argument
    {
        std::stringstream argumentError;
        if ( ParseActionArg(text, *currAction, pos, argEnd, actionId, argsLeft, argumentError) )
        {
            pos = argEnd+1;
            argsLeft --;
        }
        else
        {
            error << "Line: " << line << std::endl << std::endl << "Expected: Action Argument" << std::endl << std::endl << argumentError.str();
            return false;
        }
    }
    else
    {
        error << "Line: " << line << std::endl << std::endl << "Expected: Additional Arguments";
        return false;
    }

    return true;
}

inline bool TextTrigCompiler::ParsePartNine(buffer & text, Chk::Trigger & output, std::stringstream & error, s64 & pos, u32 & line, u32 & expecting)
{
    //      }
    // or   flags:,
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
            error << "Line: " << line << std::endl << std::endl << "Expected: \':\'";
        else
            error << "Line: " << line << std::endl << std::endl << "Expected: \"Flags\" or \'}\'";
        return false;
    }
    return true;
}

inline bool TextTrigCompiler::ParsePartTen(buffer & text, Chk::Trigger & output, std::stringstream & error, s64 & pos, u32 & line, u32 & expecting,
    s64 & flagsEnd)
{
    //      ;
    // or  %32BitFlags;
    if ( text.has(';', pos) )
    {
        pos ++;
        expecting ++;
    }
    else if ( text.getNext(';', pos, flagsEnd) )
    {
        if ( ParseExecutionFlags(text, pos, flagsEnd, output.flags) )
        {
            pos = flagsEnd+1;
            expecting ++;
        }
        else
        {
            error << "Line: " << line << std::endl << std::endl << "Expected: Binary Execution Flags (32-bit max).";
            return false;
        }
    }
    else
    {
        error << "Line: " << line << std::endl << std::endl << "Expected: \';\'";
        return false;
    }
    return true;
}

inline bool TextTrigCompiler::ParsePartEleven(buffer & text, Chk::Trigger & output, std::stringstream & error, s64 & pos, u32 & line, u32 & expecting)
{
    //      }
    if ( text.has('}', pos) )
    {
        pos ++;
        expecting ++;
    }
    else
    {
        error << "Line: " << line << std::endl << std::endl << "Expected: \'}\'";
        return false;
    }
    return true;
}

bool TextTrigCompiler::ParseExecutingPlayer(buffer & text, Chk::Trigger & currTrig, s64 pos, s64 end)
{
    u32 group;
    s64 separator;
    if ( text.getNextUnquoted(':', pos, end, separator) &&
        ParsePlayer(text, group, pos, separator) &&
        group < 28 )
    {
        u8 appendedValue;
        if ( !ParseByte((char*)text.getPtr(0), appendedValue, separator+1, end) )
            appendedValue = 1;

        currTrig.owners[group] = (Chk::Trigger::Owned)appendedValue;
        return true;
    }
    else if ( ParsePlayer(text, group, pos, end) && group < 28 )
    {
        currTrig.owners[group] = Chk::Trigger::Owned::Yes;
        return true;
    }
    return false;
}

bool TextTrigCompiler::ParseConditionName(buffer & arg, Chk::Condition::VirtualType & conditionType)
{
    char currChar = arg.get<u8>(0);
    switch ( currChar )
    {
    case 'A':
    {
        if ( arg.has("CCUMULATE", 1, 9) )
            conditionType = Chk::Condition::VirtualType::Accumulate;
        else if ( arg.has("LWAYS", 1, 5) )
            conditionType = Chk::Condition::VirtualType::Always;
    }
    break;

    case 'B':
        if ( arg.has("RING", 1, 4) )
            conditionType = Chk::Condition::VirtualType::Bring;
        break;

    case 'C':
        if ( arg.has("OMMAND", 1, 6) )
        {
            if ( arg.has("THELEAST", 7, 8) )
            {
                if ( arg.has("AT", 15, 2) )
                    conditionType = Chk::Condition::VirtualType::CommandTheLeastAt;
                else if ( arg.size() == 15 )
                    conditionType = Chk::Condition::VirtualType::CommandTheLeast;
            }
            else if ( arg.has("THEMOST", 7, 7) )
            {
                if ( arg.has("AT", 14, 2) )
                    conditionType = Chk::Condition::VirtualType::CommandTheMostAt;
                else if ( arg.size() == 14 )
                    conditionType = Chk::Condition::VirtualType::CommandTheMost;
            }
            else if ( arg.has("STHEMOSTAT", 7, 10) ) // command'S', added for backwards compatibility
                conditionType = Chk::Condition::VirtualType::CommandTheMostAt;
            else if ( arg.size() == 7 )
                conditionType = Chk::Condition::VirtualType::Command;
        }
        else if ( arg.has("OUNTDOWNTIMER", 1, 13) )
            conditionType = Chk::Condition::VirtualType::CountdownTimer;
        else if ( arg.has("USTOM", 1, 5) )
            conditionType = Chk::Condition::VirtualType::Custom;
        break;

    case 'D':
        if ( arg.has("EATHS", 1, 5) )
            conditionType = Chk::Condition::VirtualType::Deaths;
        break;

    case 'E':
        if ( arg.has("LAPSEDTIME", 1, 10) )
            conditionType = Chk::Condition::VirtualType::ElapsedTime;
        break;

    case 'H':
        if ( arg.has("IGHESTSCORE", 1, 11) )
            conditionType = Chk::Condition::VirtualType::HighestScore;
        break;

    case 'K':
        if ( arg.has("ILL", 1, 3) )
            conditionType = Chk::Condition::VirtualType::Kill;
        break;

    case 'L':
        if ( arg.has("EAST", 1, 4) )
        {
            if ( arg.has("KILLS", 5, 5) )
                conditionType = Chk::Condition::VirtualType::LeastKills;
            else if ( arg.has("RESOURCES", 5, 9) )
                conditionType = Chk::Condition::VirtualType::LeastResources;
        }
        else if ( arg.has("OWESTSCORE", 1, 10) )
            conditionType = Chk::Condition::VirtualType::LowestScore;
        break;

    case 'M':
        if ( arg.has("EMORY", 1, 5) )
            conditionType = Chk::Condition::VirtualType::Memory;
        else if ( arg.has("OST", 1, 3) )
        {
            if ( arg.has("KILLS", 4, 5) )
                conditionType = Chk::Condition::VirtualType::MostKills;
            else if ( arg.has("RESOURCES", 4, 9) )
                conditionType = Chk::Condition::VirtualType::MostResources;
        }
        break;

    case 'N':
        if ( arg.has("EVER", 1, 4) )
            conditionType = Chk::Condition::VirtualType::Never;
        break;

    case 'O':
        if ( arg.has("PPONENTS", 1, 8) )
            conditionType = Chk::Condition::VirtualType::Opponents;
        break;

    case 'S':
        if ( arg.has("CORE", 1, 4) )
            conditionType = Chk::Condition::VirtualType::Score;
        else if ( arg.has("WITCH", 1, 5) )
            conditionType = Chk::Condition::VirtualType::Switch;
        break;
    }

    return conditionType != Chk::Condition::VirtualType::NoCondition;
}

bool TextTrigCompiler::ParseCondition(buffer & text, s64 pos, s64 end, bool disabled, Chk::Condition::VirtualType & conditionType, u8 & flags, u32 & argsLeft)
{
    conditionType = Chk::Condition::VirtualType::NoCondition;
    u16 number = 0;

    s64 size = end - pos;
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
        else if ( arg.get<u8>(i) == '\t' ) // Del tabbing
            arg.del<u8>(i);
    }

    ParseConditionName(arg, conditionType);

    flags = defaultConditionFlags(ExtendedToRegularCID(conditionType));
    argsLeft = numConditionArgs(conditionType);

    return conditionType != Chk::Condition::VirtualType::NoCondition;
}

bool TextTrigCompiler::ParseActionName(buffer & arg, Chk::Action::VirtualType & actionType)
{
    char currChar = arg.get<u8>(0);
    switch ( currChar )
    {
    case 'C':
        if ( arg.has("OMMENT", 1, 6) )
            actionType = Chk::Action::VirtualType::Comment;
        else if ( arg.has("REATEUNIT", 1, 9) )
        {
            if ( arg.has("WITHPROPERTIES", 10, 14) )
                actionType = Chk::Action::VirtualType::CreateUnitWithProperties;
            else if ( arg.size() == 10 )
                actionType = Chk::Action::VirtualType::CreateUnit;
        }
        else if ( arg.has("ENTERVIEW", 1, 9) )
            actionType = Chk::Action::VirtualType::CenterView;
        else if ( arg.has("USTOM", 1, 5) )
            actionType = Chk::Action::VirtualType::Custom;
        break;

    case 'D':
        if ( arg.has("ISPLAYTEXTMESSAGE", 1, 17) )
            actionType = Chk::Action::VirtualType::DisplayTextMessage;
        else if ( arg.has("EFEAT", 1, 5) )
            actionType = Chk::Action::VirtualType::Defeat;
        else if ( arg.has("RAW", 1, 3) )
            actionType = Chk::Action::VirtualType::Draw;
        break;

    case 'G':
        if ( arg.has("IVEUNITSTOPLAYER", 1, 16) )
            actionType = Chk::Action::VirtualType::GiveUnitsToPlayer;
        break;

    case 'K':
        if ( arg.has("ILLUNIT", 1, 7) )
        {
            if ( arg.has("ATLOCATION", 8, 10) )
                actionType = Chk::Action::VirtualType::KillUnitAtLocation;
            else if ( arg.size() == 8 )
                actionType = Chk::Action::VirtualType::KillUnit;
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
                        actionType = Chk::Action::VirtualType::LeaderboardGoalCtrlAtLoc;
                    else if ( arg.size() == 22 )
                        actionType = Chk::Action::VirtualType::LeaderboardGoalCtrl;
                }
                else if ( arg.has("KILLS", 15, 5) )
                    actionType = Chk::Action::VirtualType::LeaderboardGoalKills;
                else if ( arg.has("POINTS", 15, 6) )
                    actionType = Chk::Action::VirtualType::LeaderboardGoalPoints;
                else if ( arg.has("RESOURCES", 15, 9) )
                    actionType = Chk::Action::VirtualType::LeaderboardGoalResources;
            }
            else
            {
                if ( arg.has("CONTROL", 11, 7) )
                {
                    if ( arg.has("ATLOCATION", 18, 10) )
                        actionType = Chk::Action::VirtualType::LeaderboardCtrlAtLoc;
                    else if ( arg.size() == 18 )
                        actionType = Chk::Action::VirtualType::LeaderboardCtrl;
                }
                else if ( arg.has("GREED", 11, 5) )
                    actionType = Chk::Action::VirtualType::LeaderboardGreed;
                else if ( arg.has("KILLS", 11, 5) )
                    actionType = Chk::Action::VirtualType::LeaderboardKills;
                else if ( arg.has("POINTS", 11, 6) )
                    actionType = Chk::Action::VirtualType::LeaderboardPoints;
                else if ( arg.has("RESOURCES", 11, 9) )
                    actionType = Chk::Action::VirtualType::LeaderboardResources;
                else if ( arg.has("COMPUTERPLAYERS", 11, 15) )
                    actionType = Chk::Action::VirtualType::LeaderboardCompPlayers;
            }
        }
        break;

    case 'M':
        if ( arg.has("EMORY", 1, 5) )
            actionType = Chk::Action::VirtualType::SetMemory;
        else if ( arg.has("OVE", 1, 3) )
        {
            if ( arg.has("UNIT", 4, 4) )
                actionType = Chk::Action::VirtualType::MoveUnit;
            else if ( arg.has("LOCATION", 4, 8) )
                actionType = Chk::Action::VirtualType::MoveLocation;
        }
        else if ( arg.has("ODIFYUNIT", 1, 9) )
        {
            if ( arg.has("ENERGY", 10, 6) )
                actionType = Chk::Action::VirtualType::ModifyUnitEnergy;
            else if ( arg.has("HANGERCOUNT", 10, 11) )
                actionType = Chk::Action::VirtualType::ModifyUnitHangerCount;
            else if ( arg.has("HITPOINTS", 10, 9) )
                actionType = Chk::Action::VirtualType::ModifyUnitHitpoints;
            else if ( arg.has("RESOURCEAMOUNT", 10, 14) )
                actionType = Chk::Action::VirtualType::ModifyUnitResourceAmount;
            else if ( arg.has("SHIELDPOINTS", 10, 12) )
                actionType = Chk::Action::VirtualType::ModifyUnitShieldPoints;
        }
        else if ( arg.has("INIMAPPING", 1, 10) )
            actionType = Chk::Action::VirtualType::MinimapPing;
        else if ( arg.has("UTEUNITSPEECH", 1, 13) )
            actionType = Chk::Action::VirtualType::MuteUnitSpeech;
        break;

    case 'O':
        if ( arg.has("RDER", 1, 4) )
            actionType = Chk::Action::VirtualType::Order;
        break;

    case 'P':
        if ( arg.has("RESERVETRIGGER", 1, 14) )
            actionType = Chk::Action::VirtualType::PreserveTrigger;
        else if ( arg.has("LAYWAV", 1, 6) )
            actionType = Chk::Action::VirtualType::PlaySound;
        else if ( arg.has("AUSE", 1, 4) )
        {
            if ( arg.has("GAME", 5, 4) )
                actionType = Chk::Action::VirtualType::PauseGame;
            else if ( arg.has("TIMER", 5, 5) )
                actionType = Chk::Action::VirtualType::PauseTimer;
        }
        break;

    case 'R':
        if ( arg.has("EMOVEUNIT", 1, 9) )
        {
            if ( arg.has("ATLOCATION", 10, 10) )
                actionType = Chk::Action::VirtualType::RemoveUnitAtLocation;
            else if ( arg.size() == 10 )
                actionType = Chk::Action::VirtualType::RemoveUnit;
        }
        else if ( arg.has("UNAISCRIPT", 1, 10) )
        {
            if ( arg.has("ATLOCATION", 11, 10) )
                actionType = Chk::Action::VirtualType::RunAiScriptAtLocation;
            else if ( arg.size() == 11 )
                actionType = Chk::Action::VirtualType::RunAiScript;
        }
        break;

    case 'S':
        if ( arg.has("ET", 1, 2) )
        {
            if ( arg.has("DEATHS", 3, 6) )
                actionType = Chk::Action::VirtualType::SetDeaths;
            else if ( arg.has("SWITCH", 3, 6) )
                actionType = Chk::Action::VirtualType::SetSwitch;
            else if ( arg.has("RESOURCES", 3, 9) )
                actionType = Chk::Action::VirtualType::SetResources;
            else if ( arg.has("SCORE", 3, 5) )
                actionType = Chk::Action::VirtualType::SetScore;
            else if ( arg.has("ALLIANCESTATUS", 3, 14) )
                actionType = Chk::Action::VirtualType::SetAllianceStatus;
            else if ( arg.has("COUNTDOWNTIMER", 3, 14) )
                actionType = Chk::Action::VirtualType::SetCountdownTimer;
            else if ( arg.has("DOODADSTATE", 3, 11) )
                actionType = Chk::Action::VirtualType::SetDoodadState;
            else if ( arg.has("INVINCIBILITY", 3, 13) )
                actionType = Chk::Action::VirtualType::SetInvincibility;
            else if ( arg.has("MISSIONOBJECTIVES", 3, 17) )
                actionType = Chk::Action::VirtualType::SetMissionObjectives;
            else if ( arg.has("NEXTSCENARIO", 3, 12) )
                actionType = Chk::Action::VirtualType::SetNextScenario;
            else if ( arg.has("MEMORY", 3, 6) )
                actionType = Chk::Action::VirtualType::SetMemory;
        }
        break;

    case 'T':
        if ( arg.has("ALKINGPORTRAIT", 1, 14) )
            actionType = Chk::Action::VirtualType::TalkingPortrait;
        else if ( arg.has("RANSMISSION", 1, 11) )
            actionType = Chk::Action::VirtualType::Transmission;
        break;

    case 'U':
        if ( arg.has("NPAUSE", 1, 6) )
        {
            if ( arg.has("TIMER", 7, 5) )
                actionType = Chk::Action::VirtualType::UnpauseTimer;
            else if ( arg.has("GAME", 7, 4) )
                actionType = Chk::Action::VirtualType::UnpauseGame;
        }
        else if ( arg.has("NMUTEUNITSPEECH", 1, 15) )
            actionType = Chk::Action::VirtualType::MuteUnitSpeech;
        break;

    case 'V':
        if ( arg.has("ICTORY", 1, 6) )
            actionType = Chk::Action::VirtualType::Victory;
        break;

    case 'W':
        if ( arg.has("AIT", 1, 3) )
            actionType = Chk::Action::VirtualType::Wait;
        break;
    }

    return actionType != Chk::Action::VirtualType::NoAction;
}

bool TextTrigCompiler::ParseAction(buffer & text, s64 pos, s64 end, bool diabled, Chk::Action::VirtualType & actionType, u8 & flags, u32 & argsLeft)
{
    actionType = Chk::Action::VirtualType::NoAction;
    u16 number = 0;

    s64 size = end - pos;
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
        else if ( arg.get<u8>(i) == '\t' ) // Del tabbing
            arg.del<u8>(i);
    }

    char currChar = arg.get<u8>(0);

    switch ( currChar )
    {
    case 'C':
        if ( arg.has("OMMENT", 1, 6) )
            actionType = Chk::Action::VirtualType::Comment;
        else if ( arg.has("REATEUNIT", 1, 9) )
        {
            if ( arg.has("WITHPROPERTIES", 10, 14) )
                actionType = Chk::Action::VirtualType::CreateUnitWithProperties;
            else if ( arg.size() == 10 )
                actionType = Chk::Action::VirtualType::CreateUnit;
        }
        else if ( arg.has("ENTERVIEW", 1, 9) )
            actionType = Chk::Action::VirtualType::CenterView;
        else if ( arg.has("USTOM", 1, 5) )
            actionType = Chk::Action::VirtualType::Custom;
        break;

    case 'D':
        if ( arg.has("ISPLAYTEXTMESSAGE", 1, 17) )
            actionType = Chk::Action::VirtualType::DisplayTextMessage;
        else if ( arg.has("EFEAT", 1, 5) )
            actionType = Chk::Action::VirtualType::Defeat;
        else if ( arg.has("RAW", 1, 3) )
            actionType = Chk::Action::VirtualType::Draw;
        break;

    case 'G':
        if ( arg.has("IVEUNITSTOPLAYER", 1, 16) )
            actionType = Chk::Action::VirtualType::GiveUnitsToPlayer;
        break;

    case 'K':
        if ( arg.has("ILLUNIT", 1, 7) )
        {
            if ( arg.has("ATLOCATION", 8, 10) )
                actionType = Chk::Action::VirtualType::KillUnitAtLocation;
            else if ( arg.size() == 8 )
                actionType = Chk::Action::VirtualType::KillUnit;
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
                        actionType = Chk::Action::VirtualType::LeaderboardGoalCtrlAtLoc;
                    else if ( arg.size() == 22 )
                        actionType = Chk::Action::VirtualType::LeaderboardGoalCtrl;
                }
                else if ( arg.has("KILLS", 15, 5) )
                    actionType = Chk::Action::VirtualType::LeaderboardGoalKills;
                else if ( arg.has("POINTS", 15, 6) )
                    actionType = Chk::Action::VirtualType::LeaderboardGoalPoints;
                else if ( arg.has("RESOURCES", 15, 9) )
                    actionType = Chk::Action::VirtualType::LeaderboardGoalResources;
            }
            else
            {
                if ( arg.has("CONTROL", 11, 7) )
                {
                    if ( arg.has("ATLOCATION", 18, 10) )
                        actionType = Chk::Action::VirtualType::LeaderboardCtrlAtLoc;
                    else if ( arg.size() == 18 )
                        actionType = Chk::Action::VirtualType::LeaderboardCtrl;
                }
                else if ( arg.has("GREED", 11, 5) )
                    actionType = Chk::Action::VirtualType::LeaderboardGreed;
                else if ( arg.has("KILLS", 11, 5) )
                    actionType = Chk::Action::VirtualType::LeaderboardKills;
                else if ( arg.has("POINTS", 11, 6) )
                    actionType = Chk::Action::VirtualType::LeaderboardPoints;
                else if ( arg.has("RESOURCES", 11, 9) )
                    actionType = Chk::Action::VirtualType::LeaderboardResources;
                else if ( arg.has("COMPUTERPLAYERS", 11, 15) )
                    actionType = Chk::Action::VirtualType::LeaderboardCompPlayers;
            }
        }
        break;

    case 'M':
        if ( arg.has("EMORY", 1, 5) )
            actionType = Chk::Action::VirtualType::SetMemory;
        else if ( arg.has("OVE", 1, 3) )
        {
            if ( arg.has("UNIT", 4, 4) )
                actionType = Chk::Action::VirtualType::MoveUnit;
            else if ( arg.has("LOCATION", 4, 8) )
                actionType = Chk::Action::VirtualType::MoveLocation;
        }
        else if ( arg.has("ODIFYUNIT", 1, 9) )
        {
            if ( arg.has("ENERGY", 10, 6) )
                actionType = Chk::Action::VirtualType::ModifyUnitEnergy;
            else if ( arg.has("HANGERCOUNT", 10, 11) )
                actionType = Chk::Action::VirtualType::ModifyUnitHangerCount;
            else if ( arg.has("HITPOINTS", 10, 9) )
                actionType = Chk::Action::VirtualType::ModifyUnitHitpoints;
            else if ( arg.has("RESOURCEAMOUNT", 10, 14) )
                actionType = Chk::Action::VirtualType::ModifyUnitResourceAmount;
            else if ( arg.has("SHIELDPOINTS", 10, 12) )
                actionType = Chk::Action::VirtualType::ModifyUnitShieldPoints;
        }
        else if ( arg.has("INIMAPPING", 1, 10) )
            actionType = Chk::Action::VirtualType::MinimapPing;
        else if ( arg.has("UTEUNITSPEECH", 1, 13) )
            actionType = Chk::Action::VirtualType::MuteUnitSpeech;
        break;

    case 'O':
        if ( arg.has("RDER", 1, 4) )
            actionType = Chk::Action::VirtualType::Order;
        break;

    case 'P':
        if ( arg.has("RESERVETRIGGER", 1, 14) )
            actionType = Chk::Action::VirtualType::PreserveTrigger;
        else if ( arg.has("LAYWAV", 1, 6) )
            actionType = Chk::Action::VirtualType::PlaySound;
        else if ( arg.has("AUSE", 1, 4) )
        {
            if ( arg.has("GAME", 5, 4) )
                actionType = Chk::Action::VirtualType::PauseGame;
            else if ( arg.has("TIMER", 5, 5) )
                actionType = Chk::Action::VirtualType::PauseTimer;
        }
        break;

    case 'R':
        if ( arg.has("EMOVEUNIT", 1, 9) )
        {
            if ( arg.has("ATLOCATION", 10, 10) )
                actionType = Chk::Action::VirtualType::RemoveUnitAtLocation;
            else if ( arg.size() == 10 )
                actionType = Chk::Action::VirtualType::RemoveUnit;
        }
        else if ( arg.has("UNAISCRIPT", 1, 10) )
        {
            if ( arg.has("ATLOCATION", 11, 10) )
                actionType = Chk::Action::VirtualType::RunAiScriptAtLocation;
            else if ( arg.size() == 11 )
                actionType = Chk::Action::VirtualType::RunAiScript;
        }
        break;

    case 'S':
        if ( arg.has("ET", 1, 2) )
        {
            if ( arg.has("DEATHS", 3, 6) )
                actionType = Chk::Action::VirtualType::SetDeaths;
            else if ( arg.has("SWITCH", 3, 6) )
                actionType = Chk::Action::VirtualType::SetSwitch;
            else if ( arg.has("RESOURCES", 3, 9) )
                actionType = Chk::Action::VirtualType::SetResources;
            else if ( arg.has("SCORE", 3, 5) )
                actionType = Chk::Action::VirtualType::SetScore;
            else if ( arg.has("ALLIANCESTATUS", 3, 14) )
                actionType = Chk::Action::VirtualType::SetAllianceStatus;
            else if ( arg.has("COUNTDOWNTIMER", 3, 14) )
                actionType = Chk::Action::VirtualType::SetCountdownTimer;
            else if ( arg.has("DOODADSTATE", 3, 11) )
                actionType = Chk::Action::VirtualType::SetDoodadState;
            else if ( arg.has("INVINCIBILITY", 3, 13) )
                actionType = Chk::Action::VirtualType::SetInvincibility;
            else if ( arg.has("MISSIONOBJECTIVES", 3, 17) )
                actionType = Chk::Action::VirtualType::SetMissionObjectives;
            else if ( arg.has("NEXTSCENARIO", 3, 12) )
                actionType = Chk::Action::VirtualType::SetNextScenario;
            else if ( arg.has("MEMORY", 3, 6) )
                actionType = Chk::Action::VirtualType::SetMemory;
        }
        break;

    case 'T':
        if ( arg.has("ALKINGPORTRAIT", 1, 14) )
            actionType = Chk::Action::VirtualType::TalkingPortrait;
        else if ( arg.has("RANSMISSION", 1, 11) )
            actionType = Chk::Action::VirtualType::Transmission;
        break;

    case 'U':
        if ( arg.has("NPAUSE", 1, 6) )
        {
            if ( arg.has("TIMER", 7, 5) )
                actionType = Chk::Action::VirtualType::UnpauseTimer;
            else if ( arg.has("GAME", 7, 4) )
                actionType = Chk::Action::VirtualType::UnpauseGame;
        }
        else if ( arg.has("NMUTEUNITSPEECH", 1, 15) )
            actionType = Chk::Action::VirtualType::UnmuteUnitSpeech;
        break;

    case 'V':
        if ( arg.has("ICTORY", 1, 6) )
            actionType = Chk::Action::VirtualType::Victory;
        break;

    case 'W':
        if ( arg.has("AIT", 1, 3) )
            actionType = Chk::Action::VirtualType::Wait;
        break;
    }

    flags = defaultActionFlags(ExtendedToRegularAID(actionType));
    argsLeft = numActionArgs(actionType);

    return actionType != Chk::Action::VirtualType::NoAction;
}

bool TextTrigCompiler::ParseConditionArg(buffer & text, Chk::Condition & currCondition, s64 pos, s64 end, Chk::Condition::VirtualType conditionType, u32 argsLeft, std::stringstream & error)
{
    char* textPtr = (char*)text.getPtr(0);

    // Search for condition ID
    switch ( conditionType )
    {
    case Chk::Condition::VirtualType::Custom:
        switch ( argsLeft ) {
        case 9: goto ParseConditionLocationField    ; break;
        case 8: goto ParseConditionPlayerField      ; break;
        case 7: goto ParseConditionAmountField      ; break;
        case 6: goto ParseConditionUnitField        ; break;
        case 5: goto ParseConditionComparisonField  ; break;
        case 4: goto ParseConditionConditionField   ; break;
        case 3: goto ParseConditionTypeIndexField   ; break;
        case 2: goto ParseConditionFlagsField       ; break;
        case 1: goto ParseConditionInternalDataField; break;
        }
        break;
    case Chk::Condition::VirtualType::Memory: // deathTable+, mod, num
        switch ( argsLeft ) {
        case 3: goto ParseConditionDeathOffsetField      ; break;
        case 2: goto ParseConditionNumericComparisonField; break;
        case 1: goto ParseConditionAmountField           ; break;
        }
        break;
    case Chk::Condition::VirtualType::Accumulate: // player, mod, num, resouce
        switch ( argsLeft ) {
        case 4: goto ParseConditionPlayerField           ; break;
        case 3: goto ParseConditionNumericComparisonField; break;
        case 2: goto ParseConditionAmountField           ; break;
        case 1: goto ParseConditionResourceTypeField     ; break;
        }
        break;
    case Chk::Condition::VirtualType::Bring: // player, mod, num, unit, location
        switch ( argsLeft ) {
        case 5: goto ParseConditionPlayerField           ; break;
        case 4: goto ParseConditionUnitField             ; break;
        case 3: goto ParseConditionLocationField         ; break;
        case 2: goto ParseConditionNumericComparisonField; break;
        case 1: goto ParseConditionAmountField           ; break;
        }
        break;
    case Chk::Condition::VirtualType::Command: // Player, Unit, NumericComparison, Amount
    case Chk::Condition::VirtualType::Deaths:  // Player, Unit, NumericComparison, Amount
    case Chk::Condition::VirtualType::Kill:    // Player, Unit, NumericComparison, Amount
        switch ( argsLeft ) {
        case 4: goto ParseConditionPlayerField           ; break;
        case 3: goto ParseConditionUnitField             ; break;
        case 2: goto ParseConditionNumericComparisonField; break;
        case 1: goto ParseConditionAmountField           ; break;

        }
        break;
    case Chk::Condition::VirtualType::CommandTheLeast: // unit
    case Chk::Condition::VirtualType::CommandTheMost:  // unit
    case Chk::Condition::VirtualType::LeastKills:      // unit
    case Chk::Condition::VirtualType::MostKills:       // unit
        if ( argsLeft == 1 ) goto ParseConditionUnitField;
        break;
    case Chk::Condition::VirtualType::CommandTheLeastAt: // unit, location
    case Chk::Condition::VirtualType::CommandTheMostAt:  // unit, location
        switch ( argsLeft ) {
        case 2: goto ParseConditionUnitField    ; break;
        case 1: goto ParseConditionLocationField; break;
        }
        break;
    case Chk::Condition::VirtualType::CountdownTimer: // NumericComparison, Amount
    case Chk::Condition::VirtualType::ElapsedTime: // NumericComparison, Amount
        switch ( argsLeft ) {
        case 2: goto ParseConditionNumericComparisonField; break;
        case 1: goto ParseConditionAmountField           ; break;
        }
        break;
    case Chk::Condition::VirtualType::HighestScore: // scoreType
    case Chk::Condition::VirtualType::LowestScore: // scoreType
        if ( argsLeft == 1 ) goto ParseConditionScoreTypeField;
        break;
    case Chk::Condition::VirtualType::LeastResources: // resource
    case Chk::Condition::VirtualType::MostResources: // resource
        if ( argsLeft == 1 ) goto ParseConditionResourceTypeField;
        break;
    case Chk::Condition::VirtualType::Opponents: // Player, NumericComparison, Amount
        switch ( argsLeft ) {
        case 3: goto ParseConditionPlayerField           ; break;
        case 2: goto ParseConditionNumericComparisonField; break;
        case 1: goto ParseConditionAmountField           ; break;
        }
        break;
    case Chk::Condition::VirtualType::Score: // Player, ScoreType, NumericComparison, Amount
        switch ( argsLeft ) {
        case 4: goto ParseConditionPlayerField           ; break;
        case 3: goto ParseConditionScoreTypeField        ; break;
        case 2: goto ParseConditionNumericComparisonField; break;
        case 1: goto ParseConditionAmountField           ; break;
        }
        break;
    case Chk::Condition::VirtualType::Switch: // Switch, SwitchState
        switch ( argsLeft ) {
        case 2: goto ParseConditionSwitchField     ; break;
        case 1: goto ParseConditionSwitchStateField; break;
        }
        break;
    }

    CHKD_ERR("INTERNAL ERROR: Invalid args left or argument unhandled, report this");
    return false;

    // returns whether the condition was true and prints msg to the error message if false
#define returnMsg(condition, msg)                           \
    if ( condition )                                        \
        return true;                                        \
    else {                                                  \
        error << msg;                                       \
        return false;                                       \
    }

ParseConditionLocationField: // 4 bytes
    returnMsg( ParseLocationName(text, currCondition.locationId, pos, end) ||
        ParseLong(textPtr, currCondition.locationId, pos, end),
        "Expected: Location name or 4-byte locationNum" );

ParseConditionPlayerField: // 4 bytes
    returnMsg( ParsePlayer(text, currCondition.player, pos, end) ||
        ParseLong(textPtr, currCondition.player, pos, end),
        "Expected: Player/group name or 4-byte id" );

ParseConditionAmountField: // 4 bytes
    returnMsg( ParseLong(textPtr, currCondition.amount, pos, end),
        "Expected: 4-byte amount" );

ParseConditionUnitField: // 2 bytes
    returnMsg( ParseUnitName(text, currCondition.unitType, pos, end) ||
        ParseShort(textPtr, (u16 &)currCondition.unitType, pos, end),
        "Expected: Unit name or 2-byte unitID" );

ParseConditionNumericComparisonField: // 1 byte
    returnMsg( ParseNumericComparison(textPtr, currCondition.comparison, pos, end) ||
        ParseByte(textPtr, (u8 &)currCondition.comparison, pos, end),
        "Expected: Numeric comparison or 1-byte comparisonID" );

ParseConditionSwitchStateField: // 1 byte
    returnMsg( ParseSwitchState(textPtr, currCondition.comparison, pos, end) ||
        ParseByte(textPtr, (u8 &)currCondition.comparison, pos, end),
        "Expected: Switch state or 1-byte comparisonID" );

ParseConditionComparisonField: // 1 byte, comparison type or switch state
    returnMsg( ParseByte(textPtr, (u8 &)currCondition.comparison, pos, end) ||
        ParseNumericComparison(textPtr, currCondition.comparison, pos, end) ||
        ParseSwitchState(textPtr, currCondition.comparison, pos, end),
        "Expected: 1-byte comparison" );

ParseConditionConditionField: // 1 byte, only used by custom
    returnMsg( ParseByte(textPtr, (u8 &)currCondition.conditionType, pos, end),
        "Expected: 1-byte conditionID" );

ParseConditionResourceTypeField: // 1 byte
    returnMsg( ParseResourceType(textPtr, currCondition.typeIndex, pos, end) ||
        ParseByte(textPtr, currCondition.typeIndex, pos, end),
        "Expected: Resource type or 1-byte resourceID" );

ParseConditionScoreTypeField: // 1 byte
    returnMsg( ParseScoreType(textPtr,currCondition.typeIndex, pos, end) ||
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
    returnMsg( ParseShort(textPtr, (u16 &)currCondition.maskFlag, pos, end),
        "Expected: 2-byte internal data" );

ParseConditionDeathOffsetField: // 4 bytes
    returnMsg( (useAddressesForMemory && ParseMemoryAddress(textPtr, currCondition.player, pos, end, deathTableOffset) ||
        !useAddressesForMemory && ParseLong(textPtr, currCondition.player, pos, end)),
        (useAddressesForMemory ? "Expected: 4-byte address" : "Expected: 4-byte death table offset") );
}

bool TextTrigCompiler::ParseActionArg(buffer & text, Chk::Action & currAction, s64 pos, s64 end, Chk::Action::VirtualType actionType, u32 argsLeft, std::stringstream & error)
{
    char* textPtr = (char*)text.getPtr(0);

    switch ( actionType )
    {
    case Chk::Action::VirtualType::Custom: // bytes: 4, 4, 4, 4, 4, 4, 2, 1, 1, 1, 3
        switch ( argsLeft ) {
        case 11: goto ParseActionLocationField    ; break;
        case 10: goto ParseActionTextField        ; break;
        case  9: goto ParseActionWavField         ; break;
        case  8: goto ParseActionDurationField    ; break;
        case  7: goto ParseActionFirstGroupField  ; break;
        case  6: goto ParseActionNumberField      ; break;
        case  5: goto ParseActionTypeIndexField   ; break;
        case  4: goto ParseActionActionField      ; break;
        case  3: goto ParseActionTypeIndexField2  ; break;
        case  2: goto ParseActionFlagsField       ; break;
        case  1: goto ParseActionInternalDataField; break;
        }
        break;
    case Chk::Action::VirtualType::SetMemory: // deathTable+, mod, num
        switch ( argsLeft ) {
        case 3: goto ParseActionDeathOffsetField    ; break;
        case 2: goto ParseActionNumericModifierField; break;
        case 1: goto ParseActionAmountField         ; break;
        }
        break;
    case Chk::Action::VirtualType::CenterView:  // Location
    case Chk::Action::VirtualType::MinimapPing: // Location
        if ( argsLeft == 1 ) goto ParseActionLocationField;
        break;
    case Chk::Action::VirtualType::Comment:              // String
    case Chk::Action::VirtualType::SetMissionObjectives: // String
    case Chk::Action::VirtualType::SetNextScenario:      // String
        if ( argsLeft == 1 ) goto ParseActionTextField;
        break;
    case Chk::Action::VirtualType::CreateUnit:         // Player, Unit, NumUnits, Location
    case Chk::Action::VirtualType::KillUnitAtLocation: // Player, Unit, NumUnits, Location
        switch ( argsLeft ) {
        case 4: goto ParseActionFirstGroupField; break;
        case 3: goto ParseActionUnitField; break;
        case 2: goto ParseActionNumUnitsField; break;
        case 1: goto ParseActionLocationField; break;
        }
        break;
    case Chk::Action::VirtualType::CreateUnitWithProperties: // Player, Unit, NumUnits, Location, Amount
        switch ( argsLeft ) {
        case 5: goto ParseActionFirstGroupField; break;
        case 4: goto ParseActionUnitField; break;
        case 3: goto ParseActionNumUnitsField; break;
        case 2: goto ParseActionLocationField; break;
        case 1: goto ParseActionAmountField; break;
        }
        break;
    case Chk::Action::VirtualType::DisplayTextMessage: // TextFlags, String
        switch ( argsLeft ) {
        case 2: goto ParseActionTextFlagField; break;
        case 1: goto ParseActionTextField; break;
        }
        break;
    case Chk::Action::VirtualType::GiveUnitsToPlayer: // Player, SecondPlayer, Unit, NumUnits, Location
        switch ( argsLeft ) {
        case 5: goto ParseActionFirstGroupField; break;
        case 4: goto ParseActionSecondGroupField; break;
        case 3: goto ParseActionUnitField; break;
        case 2: goto ParseActionNumUnitsField; break;
        case 1: goto ParseActionLocationField; break;
        }
        break;
    case Chk::Action::VirtualType::KillUnit:   // Player, Unit
    case Chk::Action::VirtualType::RemoveUnit: // Player, Unit
        switch ( argsLeft ) {
        case 2: goto ParseActionFirstGroupField; break;
        case 1: goto ParseActionUnitField; break;
        }
        break;
    case Chk::Action::VirtualType::LeaderboardCtrlAtLoc: // String, Unit, Location
        switch ( argsLeft ) {
        case 3: goto ParseActionTextField; break;
        case 2: goto ParseActionUnitField; break;
        case 1: goto ParseActionLocationField; break;
        }
        break;
    case Chk::Action::VirtualType::LeaderboardCtrl:  // String, Unit
    case Chk::Action::VirtualType::LeaderboardKills: // String, Unit
        switch ( argsLeft ) {
        case 2: goto ParseActionTextField; break;
        case 1: goto ParseActionUnitField; break;
        }
        break;
    case Chk::Action::VirtualType::LeaderboardGreed: // Amount
        if ( argsLeft == 1 ) goto ParseActionAmountField;
        break;
    case Chk::Action::VirtualType::LeaderboardPoints: // String, ScoreType
        switch ( argsLeft ) {
        case 2: goto ParseActionTextField; break;
        case 1: goto ParseActionScoreTypeField; break;
        }
        break;
    case Chk::Action::VirtualType::LeaderboardResources: // String, ResourceType
        switch ( argsLeft ) {
        case 2: goto ParseActionTextField; break;
        case 1: goto ParseActionResourceTypeField; break;
        }
        break;
    case Chk::Action::VirtualType::LeaderboardCompPlayers: // StateModifier
        if ( argsLeft == 1 ) goto ParseActionStateModifierField;
        break;
    case Chk::Action::VirtualType::LeaderboardGoalCtrlAtLoc: // String, Unit, Amount, Location
        switch ( argsLeft ) {
        case 4: goto ParseActionTextField    ; break;
        case 3: goto ParseActionUnitField    ; break;
        case 2: goto ParseActionAmountField  ; break;
        case 1: goto ParseActionLocationField; break;
        }
        break;
    case Chk::Action::VirtualType::LeaderboardGoalCtrl: // String, Unit, Amount
    case Chk::Action::VirtualType::LeaderboardGoalKills:   // String, Unit, Amount
        switch ( argsLeft ) {
        case 3: goto ParseActionTextField; break;
        case 2: goto ParseActionUnitField; break;
        case 1: goto ParseActionAmountField; break;
        }
        break;
    case Chk::Action::VirtualType::LeaderboardGoalPoints: // String, ScoreType, Amount
        switch ( argsLeft ) {
        case 3: goto ParseActionTextField     ; break;
        case 2: goto ParseActionScoreTypeField; break;
        case 1: goto ParseActionAmountField   ; break;
        }
        break;
    case Chk::Action::VirtualType::LeaderboardGoalResources: // String, Amount, ResourceType
        switch ( argsLeft ) {
        case 3: goto ParseActionTextField; break;
        case 2: goto ParseActionAmountField; break;
        case 1: goto ParseActionResourceTypeField; break;
        }
        break;
    case Chk::Action::VirtualType::ModifyUnitEnergy:       // Player, Unit, Amount, NumUnits, Location
    case Chk::Action::VirtualType::ModifyUnitHangerCount:  // Player, Unit, Amount, NumUnits, Location
    case Chk::Action::VirtualType::ModifyUnitHitpoints:    // Player, Unit, Amount, NumUnits, Location
    case Chk::Action::VirtualType::ModifyUnitShieldPoints: // Player, Unit, Amount, NumUnits, Location
        switch ( argsLeft ) {
        case 5: goto ParseActionFirstGroupField; break;
        case 4: goto ParseActionUnitField; break;
        case 3: goto ParseActionAmountField; break;
        case 2: goto ParseActionNumUnitsField; break;
        case 1: goto ParseActionLocationField; break;
        }
        break;
    case Chk::Action::VirtualType::ModifyUnitResourceAmount: // Player, Amount, NumUnits, Location
        switch ( argsLeft ) {
        case 4: goto ParseActionFirstGroupField; break;
        case 3: goto ParseActionAmountField; break;
        case 2: goto ParseActionNumUnitsField; break;
        case 1: goto ParseActionLocationField; break;
        }
        break;
    case Chk::Action::VirtualType::MoveLocation: // Player, Unit, LocDest, Location
        switch ( argsLeft ) {
        case 4: goto ParseActionFirstGroupField; break;
        case 3: goto ParseActionUnitField; break;
        case 2: goto ParseActionLocationField; break;
        case 1: goto ParseActionSecondLocationField; break;
        }
        break;
    case Chk::Action::VirtualType::MoveUnit: // Player, Unit, NumUnits, Location, LocDest
        switch ( argsLeft ) {
        case 5: goto ParseActionFirstGroupField; break;
        case 4: goto ParseActionUnitField; break;
        case 3: goto ParseActionNumUnitsField; break;
        case 2: goto ParseActionLocationField; break;
        case 1: goto ParseActionSecondLocationField; break;
        }
        break;
    case Chk::Action::VirtualType::Order: // Player, Unit, Location, LocDest, OrderType
        switch ( argsLeft ) {
        case 5: goto ParseActionFirstGroupField; break;
        case 4: goto ParseActionUnitField; break;
        case 3: goto ParseActionLocationField; break;
        case 2: goto ParseActionSecondLocationField; break;
        case 1: goto ParseActionOrderField; break;
        }
        break;
    case Chk::Action::VirtualType::PlaySound: // Wav, Duration
        switch ( argsLeft ) {
        case 2: goto ParseActionWavField; break;
        case 1: goto ParseActionDurationField; break;
        }
        break;
    case Chk::Action::VirtualType::RemoveUnitAtLocation: // Player, Unit, NumUnits, Location
        switch ( argsLeft ) {
        case 4: goto ParseActionFirstGroupField; break;
        case 3: goto ParseActionUnitField; break;
        case 2: goto ParseActionNumUnitsField; break;
        case 1: goto ParseActionLocationField; break;
        }
        break;
    case Chk::Action::VirtualType::RunAiScript: // Script
        if ( argsLeft == 1 ) goto ParseActionScriptField;
        break;
    case Chk::Action::VirtualType::RunAiScriptAtLocation: // Script, Location
        switch ( argsLeft ) {
        case 2: goto ParseActionScriptField  ; break;
        case 1: goto ParseActionLocationField; break;
        }
        break;
    case Chk::Action::VirtualType::SetAllianceStatus: // Player, AllyState
        switch ( argsLeft ) {
        case 2: goto ParseActionFirstGroupField    ; break;
        case 1: goto ParseActionAllianceStatusField; break;
        }
        break;
    case Chk::Action::VirtualType::SetCountdownTimer: // NumericModifier, Duration
        switch ( argsLeft ) {
        case 2: goto ParseActionNumericModifierField; break;
        case 1: goto ParseActionDurationField; break;
        }
        break;
    case Chk::Action::VirtualType::SetDeaths: // Player, Unit, NumericModifier, Amount
        switch ( argsLeft ) {
        case 4: goto ParseActionFirstGroupField; break;
        case 3: goto ParseActionUnitField; break;
        case 2: goto ParseActionNumericModifierField; break;
        case 1: goto ParseActionAmountField; break;
        }
    case Chk::Action::VirtualType::SetDoodadState:   // Player, Unit, Location, StateMod
    case Chk::Action::VirtualType::SetInvincibility: // Player, Unit, Location, StateMod
        switch ( argsLeft ) {
        case 4: goto ParseActionFirstGroupField; break;
        case 3: goto ParseActionUnitField; break;
        case 2: goto ParseActionLocationField; break;
        case 1: goto ParseActionStateModifierField; break;
        }
        break;
    case Chk::Action::VirtualType::SetResources: // Player, NumericModifier, Amount, ResourceType
        switch ( argsLeft ) {
        case 4: goto ParseActionFirstGroupField     ; break;
        case 3: goto ParseActionNumericModifierField; break;
        case 2: goto ParseActionAmountField         ; break;
        case 1: goto ParseActionResourceTypeField   ; break;
        }
    case Chk::Action::VirtualType::SetScore: // Player, NumericModifier, Amount, ScoreType
        switch ( argsLeft ) {
        case 4: goto ParseActionFirstGroupField     ; break;
        case 3: goto ParseActionNumericModifierField; break;
        case 2: goto ParseActionAmountField         ; break;
        case 1: goto ParseActionScoreTypeField      ; break;
        }
        break;
    case Chk::Action::VirtualType::SetSwitch: // Switch, SwitchMod
        switch ( argsLeft ) {
        case 2: goto ParseActionSwitchField; break;
        case 1: goto ParseActionSwitchModifierField; break;
        }
        break;
    case Chk::Action::VirtualType::TalkingPortrait: // Unit, Duration
        switch ( argsLeft ) {
        case 2: goto ParseActionUnitField    ; break;
        case 1: goto ParseActionDurationField; break;
        }
        break;
    case Chk::Action::VirtualType::Transmission: // TextFlags, String, Unit, Location, NumericModifier, SecondAmount, Wav, Duration
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
    case Chk::Action::VirtualType::Wait: // Duration
        if ( argsLeft == 1 ) goto ParseActionDurationField;
        break;
    }

    CHKD_ERR("INTERNAL ERROR: Invalid args left or argument unhandled, report this");
    return false;

ParseActionLocationField: // 4 bytes
    returnMsg( ParseLocationName(text, currAction.locationId, pos, end) ||
        ParseLong(textPtr, currAction.locationId, pos, end),
        "Expected: Location name or 4-byte locationNum" );

ParseActionTextField: // 4 bytes
    returnMsg( zzParseString(text, currAction.stringId, pos, end) ||
        ParseLong(textPtr, currAction.stringId, pos, end),
        "Expected: String or stringNum" );

ParseActionWavField: // 4 bytes
    returnMsg( ParseWavName(text, currAction.soundStringId, pos, end) ||
        ParseLong(textPtr, currAction.soundStringId, pos, end),
        "Expected: Wav name or 4-byte wavID" );

ParseActionDurationField: // 4 bytes
    returnMsg( ParseLong(textPtr, currAction.time, pos, end),
        "Expected: 4-byte duration" );

ParseActionFirstGroupField: // 4 bytes
    returnMsg( ParsePlayer(text, currAction.group, pos, end) ||
        ParseLong(textPtr, currAction.group, pos, end),
        "Expected: Group name or 4-byte groupID" );

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
    returnMsg( ParseUnitName(text, (Sc::Unit::Type &)currAction.type, pos, end) ||
        ParseScoreType(textPtr, currAction.type, pos, end) ||
        ParseResourceType(textPtr, currAction.type, pos, end) ||
        ParseAllianceStatus(textPtr, currAction.type, pos, end) ||
        ParseShort(textPtr, currAction.type, pos, end),
        "Expected: Unit, score type, resource type, alliance status, or 2-byte typeID" );

ParseActionUnitField: // 2 bytes
    returnMsg( ParseUnitName(text, (Sc::Unit::Type &)currAction.type, pos, end) ||
        ParseShort(textPtr, (u16 &)currAction.type, pos, end),
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
    returnMsg( ParseByte(textPtr, (u8 &)currAction.actionType, pos, end),
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
    returnMsg( ParseShort(textPtr, (u16 &)currAction.maskFlag, pos, end),
        "Expected: 2-byte mask flag" );

ParseActionDeathOffsetField: // 4 bytes
    returnMsg( (useAddressesForMemory && ParseMemoryAddress(textPtr, currAction.group, pos, end, deathTableOffset) ||
        !useAddressesForMemory && ParseLong(textPtr, currAction.group, pos, end)),
        (useAddressesForMemory ? "Expected: 4-byte address" : "Expected: 4-byte death table offset") );
}

bool TextTrigCompiler::ParseExecutionFlags(buffer & text, s64 pos, s64 end, u32 & flags)
{
    flags = 0;

    s64 size = end - pos;
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
        else if ( arg.get<u8>(i) == '\t' ) // Del tabbing
            arg.del<u8>(i);
    }

    char* argData = (char*)arg.getPtr(0);
    return ParseBinaryLong(argData, flags, 0, arg.size());
}

bool TextTrigCompiler::zzParseString(buffer & text, u32 & dest, s64 pos, s64 end)
{
    if ( text.hasCaseless("No String", pos, 9) || text.hasCaseless("\"No String\"", pos, 11) )
    {
        dest = 0;
        return true;
    }

    if ( text.get<u8>(pos) == '\"' )
    {
        pos ++;
        end --;
    }
    else
        return ParseLong((char*)text.getPtr(0), dest, pos, end);

    s64 size = end-pos;

    if ( size < 0 )
        return false;

    char* stringPtr;
    bool success = false;

    if ( text.getPtr<char>(stringPtr, pos, size+1) )
    {
        char temp = stringPtr[size];
        stringPtr[size] = '\0';
        EscString escStr(stringPtr);
        stringPtr[size] = temp;

        RawString str;
        ConvertStr(escStr, str);

        size_t hash = strHash(str);
        size_t numMatching = zzStringTable.count(hash);
        if ( numMatching == 1 )
        { // Should guarentee that you can find at least one entry
            zzStringTableNode & node = zzStringTable.find(hash)->second;
            if ( node.scStr->compare<RawString>(str) == 0 )
            {
                dest = node.stringId;
                success = true;
            }
        }
        else if ( numMatching > 1 )
        {
            auto range = zzStringTable.equal_range(hash);
            foreachin(pair, range)
            {
                zzStringTableNode & node = pair->second;
                if ( node.scStr->compare<RawString>(str) == 0 )
                {
                    if ( success == false ) // If no matches have previously been found
                    {
                        dest = node.stringId;
                        success = true;
                    }
                    else // If matches have previously been found
                    {
                        if ( node.stringId < dest )
                        { // Replace if stringNum < prevStringNum
                            dest = node.stringId;
                        }
                    }
                }
            }
        }

        if ( success == false ) // No string matches have been found
                                // New string, try to add it to the map
        {
            zzStringTableNode node;
            node.scStr = ScStrPtr(new ScStr(str));

            if ( useNextString(node.stringId) )
            {
                zzAddedStrings.push_back(node); // Add to the addedStrings list so it can be added to the map after compiling
                zzStringTable.insert(std::pair<size_t, zzStringTableNode>(strHash(str), node)); // Add to search tree for recycling
                dest = node.stringId;
                success = true;
            }
        }
    }
    return success;
}

bool TextTrigCompiler::ParseLocationName(buffer & text, u32 & dest, s64 pos, s64 end)
{
    s64 size = end-pos;
    if ( text.get<u8>(pos) == '\"' )
    {
        pos ++;
        end --;
        size -= 2;
    }
    else if ( ParseLong((char*)text.getPtr(0), dest, pos, end) )
        return true;
    else if ( text.has("NOLOCATION", pos, size) )
    {
        dest = 0;
        return true;
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

        if ( str.compare("anywhere") == 0 ) // Capitalize lower-case anywhere's
        {
            locStringPtr[0] = 'A';
            str[0] = 'A';
        }

        // Grab the string hash
        size_t hash = strHash(str);
        size_t numMatching = locationTable.count(hash);
        if ( numMatching == 1 )
        { // Should guarentee that you can find at least one entry
            LocationTableNode & node = locationTable.find(hash)->second;
            if ( node.locationName.compare(locStringPtr) == 0 )
            {
                dest = node.locationId;
                success = true;
            }
        }
        else if ( numMatching > 1 )
        {
            auto range = locationTable.equal_range(hash);
            foreachin(pair, range)
            {
                LocationTableNode & node = pair->second;
                if ( node.locationName.compare(locStringPtr) == 0 )
                {
                    if ( success == false ) // If no matches have previously been found
                    {
                        dest = node.locationId;
                        success = true;
                    }
                    else // If matches have previously been found
                    {
                        if ( u32(node.locationId) < dest )
                            dest = node.locationId; // Replace if locationNum < previous locationNum
                    }
                }
            }
        }
        locStringPtr[size] = temp;
    }
    return success;
}

bool TextTrigCompiler::ParseUnitName(buffer & text, Sc::Unit::Type & dest, s64 pos, s64 end)
{
    if ( text.get<u8>(pos) == '\"' ) // If quoted, ignore quotes
    {
        pos ++;
        end --;
    }
    else if ( ParseShort((char*)text.getPtr(0), (u16 &)dest, pos, end) )
        return true;

    s64 size = end-pos;
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
                if      ( strcmp(&unit[1], "LAN SCHEZAR (GOLIATH)"         ) == 0 ) { dest = Sc::Unit::Type::AlanSchezar_Goliath; success = true; }
                else if ( strcmp(&unit[1], "LAN SCHEZAR TURRET"            ) == 0 ) { dest = Sc::Unit::Type::AlanTurret; success = true; }
                else if ( strcmp(&unit[1], "LDARIS (TEMPLAR)"              ) == 0 ) { dest = Sc::Unit::Type::Aldaris_Templar; success = true; }
                else if ( strcmp(&unit[1], "LEXEI STUKOV (GHOST)"          ) == 0 ) { dest = Sc::Unit::Type::AlexeiStukov_Ghost; success = true; }
                else if ( strcmp(&unit[1], "NY UNIT"                       ) == 0 ) { dest = Sc::Unit::Type::AnyUnit; success = true; }
                else if ( strcmp(&unit[1], "RCTURUS MENGSK (BATTLECRUISER)") == 0 ) { dest = Sc::Unit::Type::ArcturusMengsk_Battlecruiser; success = true; }
                else if ( strcmp(&unit[1], "RTANIS (SCOUT)"                ) == 0 ) { dest = Sc::Unit::Type::Artanis_Scout; success = true; }
                break;
            case 'B':
                if      ( strcmp(&unit[1], "ENGALAAS (JUNGLE CRITTER)") == 0 ) { dest = Sc::Unit::Type::Bengalaas_Jungle; success = true; }
                else if ( strcmp(&unit[1], "UILDINGS"                 ) == 0 ) { dest = Sc::Unit::Type::Buildings; success = true; }
                break;
            case 'C':
                if      ( strcmp(&unit[1], "ARGO SHIP (UNUSED)") == 0 ) { dest = Sc::Unit::Type::CargoShip_Unused; success = true; }
                else if ( strcmp(&unit[1], "ATINA (UNUSED)"    ) == 0 ) { dest = Sc::Unit::Type::Cantina; success = true; }
                else if ( strcmp(&unit[1], "AVE (UNUSED)"      ) == 0 ) { dest = Sc::Unit::Type::Cave; success = true; }
                else if ( strcmp(&unit[1], "AVE-IN (UNUSED)"   ) == 0 ) { dest = Sc::Unit::Type::CaveIn; success = true; }
                break;
            case 'D':
                if      ( strcmp(&unit[1], "ANIMOTH (ARBITER)"      ) == 0 ) { dest = Sc::Unit::Type::Danimoth_Arbiter; success = true; }
                else if ( strcmp(&unit[1], "ARK SWARM"              ) == 0 ) { dest = Sc::Unit::Type::DarkSwarm; success = true; }
                else if ( strcmp(&unit[1], "ATA DISC"               ) == 0 ) { dest = Sc::Unit::Type::DataDisc; success = true; }
                else if ( strcmp(&unit[1], "EVOURING ONE (ZERGLING)") == 0 ) { dest = Sc::Unit::Type::DevouringOne_Zergling; success = true; }
                else if ( strcmp(&unit[1], "ISRUPTION WEB"          ) == 0 ) { dest = Sc::Unit::Type::DisruptionField; success = true; }
                break;
            case 'E':
                if      ( strcmp(&unit[1], "DMUND DUKE (SIEGE MODE)"       ) == 0 ) { dest = Sc::Unit::Type::EdmundDuke_SiegeMode; success = true; }
                else if ( strcmp(&unit[1], "DMUND DUKE (TANK MODE)"        ) == 0 ) { dest = Sc::Unit::Type::EdmundDuke_SiegeTank; success = true; }
                else if ( strcmp(&unit[1], "DMUND DUKE TURRET (SIEGE MODE)") == 0 ) { dest = Sc::Unit::Type::DukeTurretType2; success = true; }
                else if ( strcmp(&unit[1], "DMUND DUKE TURRET (TANK MODE)" ) == 0 ) { dest = Sc::Unit::Type::DukeTurretType1; success = true; }
                break;
            case 'F':
                if      ( strcmp(&unit[1], "ACTORIES"           ) == 0 ) { dest = Sc::Unit::Type::Factories; success = true; }
                else if ( strcmp(&unit[1], "ENIX (DRAGOON)"     ) == 0 ) { dest = Sc::Unit::Type::Fenix_Dragoon; success = true; }
                else if ( strcmp(&unit[1], "ENIX (ZEALOT)"      ) == 0 ) { dest = Sc::Unit::Type::Fenix_Zealot; success = true; }
                else if ( strcmp(&unit[1], "LAG"                ) == 0 ) { dest = Sc::Unit::Type::Flag; success = true; }
                else if ( strcmp(&unit[1], "LOOR GUN TRAP"      ) == 0 ) { dest = Sc::Unit::Type::FloorGunTrap; success = true; }
                else if ( strcmp(&unit[1], "LOOR HATCH (UNUSED)") == 0 ) { dest = Sc::Unit::Type::FloorHatch_Unused; success = true; }
                else if ( strcmp(&unit[1], "LOOR MISSILE TRAP"  ) == 0 ) { dest = Sc::Unit::Type::FloorMissileTrap; success = true; }
                break;
            case 'G':
                if      ( strcmp(&unit[1], "ANTRITHOR (CARRIER)"          ) == 0 ) { dest = Sc::Unit::Type::Gantrithor_Carrier; success = true; }
                else if ( strcmp(&unit[1], "ERARD DUGALLE (BATTLECRUISER)") == 0 ) { dest = Sc::Unit::Type::GerardDuGalle_BattleCruiser; success = true; }
                else if ( strcmp(&unit[1], "OLIATH TURRET"                ) == 0 ) { dest = Sc::Unit::Type::GoliathTurret; success = true; }
                else if ( strcmp(&unit[1], "UI MONTAG (FIREBAT)"          ) == 0 ) { dest = Sc::Unit::Type::GuiMontag_Firebat; success = true; }
                break;
            case 'H':
                if      ( strcmp(&unit[1], "UNTER KILLER (HYDRALISK)") == 0 ) { dest = Sc::Unit::Type::HunterKiller_Hydralisk; success = true; }
                else if ( strcmp(&unit[1], "YPERION (BATTLECRUISER)" ) == 0 ) { dest = Sc::Unit::Type::Hyperion_Battlecruiser; success = true; }
                break;
            case 'I':
                if ( unit[1] == 'D' && unit[2] == ':' )
                    success = ParseShort(unit, (u16 &)dest, 3, size);
                else if ( strcmp(&unit[1], "NDEPENDENT COMMAND CENTER (UNUSED)"  ) == 0 ) { dest = Sc::Unit::Type::IndependentCommandCenter_Unused; success = true; }
                else if ( strcmp(&unit[1], "NDEPENDENT JUMP GATE (UNUSED)"       ) == 0 ) { dest = Sc::Unit::Type::IndependentJumpGate_Unused; success = true; }
                else if ( strcmp(&unit[1], "NDEPENDENT STARPORT (UNUSED)"        ) == 0 ) { dest = Sc::Unit::Type::IndependentStarport_Unused; success = true; }
                else if ( strcmp(&unit[1], "NFESTED COMMAND CENTER"              ) == 0 ) { dest = Sc::Unit::Type::InfestedCommandCenter; success = true; }
                else if ( strcmp(&unit[1], "NFESTED DURAN (INFESTED TERRAN)"     ) == 0 ) { dest = Sc::Unit::Type::InfestedDuran; success = true; }
                else if ( strcmp(&unit[1], "NFESTED KERRIGAN (INFESTED TERRAIN)" ) == 0 ) { dest = Sc::Unit::Type::InfestedKerrigan_InfestedTerran; success = true; }
                else if ( strcmp(&unit[1], "NFESTED TERRAN"                      ) == 0 ) { dest = Sc::Unit::Type::InfestedTerran; success = true; }
                else if ( strcmp(&unit[1], "ON CANNON"                           ) == 0 ) { dest = Sc::Unit::Type::IonCannon; success = true; }
                break;
            case 'J':
                if      ( strcmp(&unit[1], "IM RAYNOR (MARINE)" ) == 0 ) { dest = Sc::Unit::Type::JimRaynor_Marine; success = true; }
                else if ( strcmp(&unit[1], "IM RAYNOR (VULTURE)") == 0 ) { dest = Sc::Unit::Type::JimRaynor_Vulture; success = true; }
                break;
            case 'K':
                if      ( strcmp(&unit[1], "AKARU (TWILIGHT CRITTER)"          ) == 0 ) { dest = Sc::Unit::Type::Kakaru_TwilightCritter; success = true; }
                else if ( strcmp(&unit[1], "HADARIN CRYSTAL FORMATION (UNUSED)") == 0 ) { dest = Sc::Unit::Type::KhadarinCrystalFormation_Unused; success = true; }
                else if ( strcmp(&unit[1], "HALIS CRYSTAL"                     ) == 0 ) { dest = Sc::Unit::Type::KhalisCrystal; success = true; }
                else if ( strcmp(&unit[1], "HAYDARIN CRYSTAL"                  ) == 0 ) { dest = Sc::Unit::Type::KhaydarinCrystal; success = true; }
                else if ( strcmp(&unit[1], "HAYDARIN CRYSTAL FORMATION"        ) == 0 ) { dest = Sc::Unit::Type::KhaydarinCrystalFormation; success = true; }
                else if ( strcmp(&unit[1], "UKULZA (GUARDIAN)"                 ) == 0 ) { dest = Sc::Unit::Type::Kukulza_Guardian; success = true; }
                else if ( strcmp(&unit[1], "UKULZA (MUTALISK)"                 ) == 0 ) { dest = Sc::Unit::Type::Kukulza_Mutalisk; success = true; }
                break;
            case 'L':
                if      ( strcmp(&unit[1], "EFT PIT DOOR"         ) == 0 ) { dest = Sc::Unit::Type::LeftPitDoor; success = true; }
                else if ( strcmp(&unit[1], "EFT UPPER LEVEL DOOR" ) == 0 ) { dest = Sc::Unit::Type::LeftUpperLevelDoor; success = true; }
                else if ( strcmp(&unit[1], "EFT WALL FLAME TRAP"  ) == 0 ) { dest = Sc::Unit::Type::LeftWallFlameTrap; success = true; }
                else if ( strcmp(&unit[1], "EFT WALL MISSILE TRAP") == 0 ) { dest = Sc::Unit::Type::LeftWallMissileTrap; success = true; }
                else if ( strcmp(&unit[1], "URKER EGG"            ) == 0 ) { dest = Sc::Unit::Type::LurkerEgg; success = true; }
                break;
            case 'M':
                if      ( strcmp(&unit[1], "AGELLAN (SCIENCE VESSEL)" ) == 0 ) { dest = Sc::Unit::Type::Magellan_ScienceVessel; success = true; }
                else if ( strcmp(&unit[1], "AP REVEALER"              ) == 0 ) { dest = Sc::Unit::Type::MapRevealer; success = true; }
                else if ( strcmp(&unit[1], "ATRIARCH (QUEEN)"         ) == 0 ) { dest = Sc::Unit::Type::Matriarch_Queen; success = true; }
                else if ( strcmp(&unit[1], "ATURE CRYSALIS"           ) == 0 ) { dest = Sc::Unit::Type::MatureCrysalis; success = true; }
                else if ( strcmp(&unit[1], "EN"                       ) == 0 ) { dest = Sc::Unit::Type::Men; success = true; }
                else if ( strcmp(&unit[1], "ERCENARY GUNSHIP (UNUSED)") == 0 ) { dest = Sc::Unit::Type::MercenaryGunship_Unused; success = true; }
                else if ( strcmp(&unit[1], "INERAL CLUSTER TYPE 1"    ) == 0 ) { dest = Sc::Unit::Type::MineralClusterType1; success = true; }
                else if ( strcmp(&unit[1], "INERAL CLUSTER TYPE 2"    ) == 0 ) { dest = Sc::Unit::Type::MineralClusterType2; success = true; }
                else if ( strcmp(&unit[1], "INERAL FIELD (TYPE 1)"    ) == 0 ) { dest = Sc::Unit::Type::MineralFieldType1; success = true; }
                else if ( strcmp(&unit[1], "INERAL FIELD (TYPE 2)"    ) == 0 ) { dest = Sc::Unit::Type::MineralFieldType2; success = true; }
                else if ( strcmp(&unit[1], "INERAL FIELD (TYPE 3)"    ) == 0 ) { dest = Sc::Unit::Type::MineralFieldType3; success = true; }
                else if ( strcmp(&unit[1], "INING PLATFORM (UNUSED)"  ) == 0 ) { dest = Sc::Unit::Type::MiningPlatform_Unused; success = true; }
                else if ( strcmp(&unit[1], "OJO (SCOUT)"              ) == 0 ) { dest = Sc::Unit::Type::Mojo_Scout; success = true; }
                else if ( strcmp(&unit[1], "UTALISK COCOON"           ) == 0 ) { dest = Sc::Unit::Type::Cocoon; success = true; }
                break;
            case 'N':
                if      ( strcmp(&unit[1], "ORAD II (BATTLECRUISER)") == 0 ) { dest = Sc::Unit::Type::NoradII_Battlecruiser; success = true; }
                else if ( strcmp(&unit[1], "ORAD II (CRASHED)"      ) == 0 ) { dest = Sc::Unit::Type::NoradII_Crashed; success = true; }
                else if ( strcmp(&unit[1], "UCLEAR MISSILE"         ) == 0 ) { dest = Sc::Unit::Type::NuclearMissile; success = true; }
                break;
            case 'O':
                if      ( strcmp(&unit[1], "VERMIND COCOON") == 0 ) { dest = Sc::Unit::Type::OvermindCocoon; success = true; }
                break;
            case 'P':
                if ( unit[1] == 'R' )
                {
                    switch ( unit[8] )
                    {
                    case 'A':
                        if      ( strcmp(&unit[2], "OTOSS ARBITER"         ) == 0 ) { dest = Sc::Unit::Type::ProtossArbiter; success = true; }
                        else if ( strcmp(&unit[2], "OTOSS ARBITER TRIBUNAL") == 0 ) { dest = Sc::Unit::Type::ProtossArbiterTribunal; success = true; }
                        else if ( strcmp(&unit[2], "OTOSS ARCHON"          ) == 0 ) { dest = Sc::Unit::Type::ProtossArchon; success = true; }
                        else if ( strcmp(&unit[2], "OTOSS ASSIMILATOR"    ) == 0 ) { dest = Sc::Unit::Type::ProtossAssimilator; success = true; }
                        break;
                    case 'B':
                        if ( strcmp(&unit[2], "OTOSS BEACON") == 0 ) { dest = Sc::Unit::Type::ProtossBeacon; success = true; }
                        break;
                    case 'C':
                        if      ( strcmp(&unit[2], "OTOSS CARRIER"         ) == 0 ) { dest = Sc::Unit::Type::ProtossCarrier; success = true; }
                        else if ( strcmp(&unit[2], "OTOSS CITADEL OF ADUN" ) == 0 ) { dest = Sc::Unit::Type::ProtossCitadelOfAdum; success = true; }
                        else if ( strcmp(&unit[2], "OTOSS CORSAIR"         ) == 0 ) { dest = Sc::Unit::Type::ProtossCorsair; success = true; }
                        else if ( strcmp(&unit[2], "OTOSS CYBERNETICS CORE") == 0 ) { dest = Sc::Unit::Type::ProtossCyberneticsCore; success = true; }
                        break;
                    case 'D':
                        if      ( strcmp(&unit[2], "OTOSS DARK ARCHON"        ) == 0 ) { dest = Sc::Unit::Type::ProtossDarkArchon; success = true; }
                        else if ( strcmp(&unit[2], "OTOSS DARK TEMPLAR (HERO)") == 0 ) { dest = Sc::Unit::Type::DarkTemplar_Hero; success = true; }
                        else if ( strcmp(&unit[2], "OTOSS DARK TEMPLAR (UNIT)") == 0 ) { dest = Sc::Unit::Type::ProtossDarkTemplar; success = true; }
                        else if ( strcmp(&unit[2], "OTOSS DRAGOON"            ) == 0 ) { dest = Sc::Unit::Type::ProtossDragoon; success = true; }
                        break;
                    case 'F':
                        if      ( strcmp(&unit[2], "OTOSS FLAG BEACON" ) == 0 ) { dest = Sc::Unit::Type::ProtossFlagBeacon; success = true; }
                        else if ( strcmp(&unit[2], "OTOSS FLEET BEACON") == 0 ) { dest = Sc::Unit::Type::ProtossFleetBeacon; success = true; }
                        else if ( strcmp(&unit[2], "OTOSS FORGE"       ) == 0 ) { dest = Sc::Unit::Type::ProtossForge; success = true; }
                        break;
                    case 'G':
                        if ( strcmp(&unit[2], "OTOSS GATEWAY") == 0 ) { dest = Sc::Unit::Type::ProtossGateway; success = true; }
                        break;
                    case 'H':
                        if ( strcmp(&unit[2], "OTOSS HIGH TEMPLAR") == 0 ) { dest = Sc::Unit::Type::ProtossHighTemplar; success = true; }
                        break;
                    case 'I':
                        if ( strcmp(&unit[2], "OTOSS INTERCEPTOR") == 0 ) { dest = Sc::Unit::Type::ProtossInterceptor; success = true; }
                        break;
                    case 'M':
                        if ( strcmp(&unit[2], "OTOSS MARKER") == 0 ) { dest = Sc::Unit::Type::ProtossMarker; success = true; }
                        break;
                    case 'N':
                        if ( strcmp(&unit[2], "OTOSS NEXUS") == 0 ) { dest = Sc::Unit::Type::ProtossNexus; success = true; }
                        break;
                    case 'O':
                        if      ( strcmp(&unit[2], "OTOSS OBSERVATORY") == 0 ) { dest = Sc::Unit::Type::ProtossObservatory; success = true; }
                        else if ( strcmp(&unit[2], "OTOSS OBSERVER"   ) == 0 ) { dest = Sc::Unit::Type::ProtossObserver; success = true; }
                        break;
                    case 'P':
                        if      ( strcmp(&unit[2], "OTOSS PHOTON CANNON") == 0 ) { dest = Sc::Unit::Type::ProtossPhotonCannon; success = true; }
                        else if ( strcmp(&unit[2], "OTOSS PROBE"        ) == 0 ) { dest = Sc::Unit::Type::ProtossProbe; success = true; }
                        else if ( strcmp(&unit[2], "OTOSS PYLON"        ) == 0 ) { dest = Sc::Unit::Type::ProtossPylon; success = true; }
                        break;
                    case 'R':
                        if      ( strcmp(&unit[2], "OTOSS REAVER"              ) == 0 ) { dest = Sc::Unit::Type::ProtossReaver; success = true; }
                        else if ( strcmp(&unit[2], "OTOSS ROBOTICS FACILITY"   ) == 0 ) { dest = Sc::Unit::Type::ProtossRoboticsFacility; success = true; }
                        else if ( strcmp(&unit[2], "OTOSS ROBOTICS SUPPORT BAY") == 0 ) { dest = Sc::Unit::Type::ProtossRoboticsSupportBay; success = true; }
                        break;
                    case 'S':
                        if      ( strcmp(&unit[2], "OTOSS SCARAB"        ) == 0 ) { dest = Sc::Unit::Type::ProtossScarab; success = true; }
                        else if ( strcmp(&unit[2], "OTOSS SCOUT"         ) == 0 ) { dest = Sc::Unit::Type::ProtossScout; success = true; }
                        else if ( strcmp(&unit[2], "OTOSS SHIELD BATTERY") == 0 ) { dest = Sc::Unit::Type::ProtossShieldBattery; success = true; }
                        else if ( strcmp(&unit[2], "OTOSS SHUTTLE"       ) == 0 ) { dest = Sc::Unit::Type::ProtossShuttle; success = true; }
                        else if ( strcmp(&unit[2], "OTOSS STARGATE"      ) == 0 ) { dest = Sc::Unit::Type::ProtossStargate; success = true; }
                        break;
                    case 'T':
                        if      ( strcmp(&unit[2], "OTOSS TEMPLAR ARCHIVES") == 0 ) { dest = Sc::Unit::Type::ProtossTemplarArchives; success = true; }
                        else if ( strcmp(&unit[2], "OTOSS TEMPLE"          ) == 0 ) { dest = Sc::Unit::Type::ProtossTemple; success = true; }
                        break;
                    case 'V':
                        if      ( strcmp(&unit[2], "OTOSS VESPENE GAS ORB TYPE 1") == 0 ) { dest = Sc::Unit::Type::ProtossVespeneGasOrbType1; success = true; }
                        else if ( strcmp(&unit[2], "OTOSS VESPENE GAS ORB TYPE 2") == 0 ) { dest = Sc::Unit::Type::ProtossVespeneGasOrbType2; success = true; }
                        break;
                    case 'Z':
                        if ( strcmp(&unit[2], "OTOSS ZEALOT") == 0 ) { dest = Sc::Unit::Type::ProtossZealot; success = true; }
                        break;
                    }
                }
                else if ( unit[1] == 'S' )
                {
                    if      ( strcmp(&unit[2], "I DISRUPTER") == 0 ) { dest = Sc::Unit::Type::PsiDistrupter; success = true; }
                    else if ( strcmp(&unit[2], "I EMITTER"  ) == 0 ) { dest = Sc::Unit::Type::PsiEmitter; success = true; }
                }
                else if ( strcmp(&unit[1], "OWER GENERATOR") == 0 ) { dest = Sc::Unit::Type::PowerGenerator; success = true; }
                break;
            case 'R':
                if      ( strcmp(&unit[1], "AGNASAUR (ASHWORLD CRITTER)") == 0 ) { dest = Sc::Unit::Type::Ragnasaur_AshworldCritter; success = true; }
                else if ( strcmp(&unit[1], "ASZAGAL (CORSAIR)"          ) == 0 ) { dest = Sc::Unit::Type::Raszagal_Corsair; success = true; }
                else if ( strcmp(&unit[1], "EPAIR BAY (UNUSED)"         ) == 0 ) { dest = Sc::Unit::Type::RepairBay_Unused; success = true; }
                else if ( strcmp(&unit[1], "HYNADON (BADLANDS CRITTER)" ) == 0 ) { dest = Sc::Unit::Type::Rhynadon_BadlandsCritter; success = true; }
                else if ( strcmp(&unit[1], "IGHT PIT DOOR"              ) == 0 ) { dest = Sc::Unit::Type::RightPitDoor; success = true; }
                else if ( strcmp(&unit[1], "IGHT UPPER LEVEL DOOR"      ) == 0 ) { dest = Sc::Unit::Type::RightUpperLevelDoor; success = true; }
                else if ( strcmp(&unit[1], "IGHT WALL FLAME TRAP"       ) == 0 ) { dest = Sc::Unit::Type::RightWallFlameTrap; success = true; }
                else if ( strcmp(&unit[1], "IGHT WALL MISSILE TRAP"     ) == 0 ) { dest = Sc::Unit::Type::RightWallMissileTrap; success = true; }
                else if ( strcmp(&unit[1], "UINS (UNUSED)"              ) == 0 ) { dest = Sc::Unit::Type::Ruins_Unused; success = true; }
                break;
            case 'S':
                if      ( strcmp(&unit[1], "AMIR DURAN (GHOST)"           ) == 0 ) { dest = Sc::Unit::Type::SamirDuran_Ghost; success = true; }
                else if ( strcmp(&unit[1], "ARAH KERRIGAN (GHOST)"        ) == 0 ) { dest = Sc::Unit::Type::SarahKerrigan_Ghost; success = true; }
                else if ( strcmp(&unit[1], "CANNER SWEEP"                 ) == 0 ) { dest = Sc::Unit::Type::ScannerSweep; success = true; }
                else if ( strcmp(&unit[1], "CANTID (DESERT CRITTER)"      ) == 0 ) { dest = Sc::Unit::Type::Scantid_DesertCritter; success = true; }
                else if ( strcmp(&unit[1], "IEGE TANK TURRET (SIEGE MODE)") == 0 ) { dest = Sc::Unit::Type::SiegeTankTurret_SiegeMode; success = true; }
                else if ( strcmp(&unit[1], "IEGE TANK TURRET (TANK MODE)" ) == 0 ) { dest = Sc::Unit::Type::SiegeTankTurret_TankMode; success = true; }
                else if ( strcmp(&unit[1], "PIDER MINE"                   ) == 0 ) { dest = Sc::Unit::Type::SpiderMine; success = true; }
                else if ( strcmp(&unit[1], "TARBASE (UNUSED)"             ) == 0 ) { dest = Sc::Unit::Type::Starbase_Unused; success = true; }
                else if ( strcmp(&unit[1], "TART LOCATION"                ) == 0 ) { dest = Sc::Unit::Type::StartLocation; success = true; }
                else if ( strcmp(&unit[1], "TASIS CELL/PRISON"            ) == 0 ) { dest = Sc::Unit::Type::StasisCellPrison; success = true; }
                break;
            case 'T':
                if ( unit[1] == 'E' )
                {
                    switch ( unit[7] )
                    {
                    case 'A':
                        if      ( strcmp(&unit[2], "RRAN ACADEMY") == 0 ) { dest = Sc::Unit::Type::TerranAcademy; success = true; }
                        else if ( strcmp(&unit[2], "RRAN ARMORY" ) == 0 ) { dest = Sc::Unit::Type::TerranArmory; success = true; }
                        break;
                    case 'B':
                        if      ( strcmp(&unit[2], "RRAN BARRACKS"     ) == 0 ) { dest = Sc::Unit::Type::TerranBarracks; success = true; }
                        else if ( strcmp(&unit[2], "RRAN BATTLECRUISER") == 0 ) { dest = Sc::Unit::Type::TerranBattlecruiser; success = true; }
                        else if ( strcmp(&unit[2], "RRAN BEACON"       ) == 0 ) { dest = Sc::Unit::Type::TerranBeacon; success = true; }
                        else if ( strcmp(&unit[2], "RRAN BUNKER"       ) == 0 ) { dest = Sc::Unit::Type::TerranBunker; success = true; }
                        break;
                    case 'C':
                        if      ( strcmp(&unit[2], "RRAN CIVILIAN"      ) == 0 ) { dest = Sc::Unit::Type::TerranCivilian; success = true; }
                        else if ( strcmp(&unit[2], "RRAN COMMAND CENTER") == 0 ) { dest = Sc::Unit::Type::TerranCommandCenter; success = true; }
                        else if ( strcmp(&unit[2], "RRAN COMSAT STATION") == 0 ) { dest = Sc::Unit::Type::TerranComsatStation; success = true; }
                        else if ( strcmp(&unit[2], "RRAN CONTROL TOWER" ) == 0 ) { dest = Sc::Unit::Type::TerranControlTower; success = true; }
                        else if ( strcmp(&unit[2], "RRAN COVERT OPS"    ) == 0 ) { dest = Sc::Unit::Type::TerranCovertOps; success = true; }
                        break;
                    case 'D':
                        if ( strcmp(&unit[2], "RRAN DROPSHIP") == 0 ) { dest = Sc::Unit::Type::TerranDropship; success = true; }
                        break;
                    case 'E':
                        if ( strcmp(&unit[2], "RRAN ENGINEERING BAY") == 0 ) { dest = Sc::Unit::Type::TerranEngineeringBay; success = true; }
                        break;
                    case 'F':
                        if      ( strcmp(&unit[2], "RRAN FACTORY"    ) == 0 ) { dest = Sc::Unit::Type::TerranFactory; success = true; }
                        else if ( strcmp(&unit[2], "RRAN FIREBAT"    ) == 0 ) { dest = Sc::Unit::Type::TerranFirebat; success = true; }
                        else if ( strcmp(&unit[2], "RRAN FLAG BEACON") == 0 ) { dest = Sc::Unit::Type::TerranFlagBeacon; success = true; }
                        break;
                    case 'G':
                        if      ( strcmp(&unit[2], "RRAN GHOST"  ) == 0 ) { dest = Sc::Unit::Type::TerranGhost; success = true; }
                        else if ( strcmp(&unit[2], "RRAN GOLIATH") == 0 ) { dest = Sc::Unit::Type::TerranGoliath; success = true; }
                        break;
                    case 'M':
                        if      ( strcmp(&unit[2], "RRAN MACHINE SHOP"  ) == 0 ) { dest = Sc::Unit::Type::TerranMachineShop; success = true; }
                        else if ( strcmp(&unit[2], "RRAN MARINE"        ) == 0 ) { dest = Sc::Unit::Type::TerranMarine; success = true; }
                        else if ( strcmp(&unit[2], "RRAN MARKER"        ) == 0 ) { dest = Sc::Unit::Type::TerranMarker; success = true; }
                        else if ( strcmp(&unit[2], "RRAN MEDIC"         ) == 0 ) { dest = Sc::Unit::Type::TerranMedic; success = true; }
                        else if ( strcmp(&unit[2], "RRAN MISSILE TURRET") == 0 ) { dest = Sc::Unit::Type::TerranMissileTurret; success = true; }
                        break;
                    case 'N':
                        if ( strcmp(&unit[2], "RRAN NUCLEAR SILO") == 0 ) { dest = Sc::Unit::Type::TerranNuclearSilo; success = true; }
                        break;
                    case 'P':
                        if ( strcmp(&unit[2], "RRAN PHYSICS LAB") == 0 ) { dest = Sc::Unit::Type::TerranPhysicsLab; success = true; }
                        break;
                    case 'R':
                        if ( strcmp(&unit[2], "RRAN REFINERY") == 0 ) { dest = Sc::Unit::Type::TerranRefinery; success = true; }
                        break;
                    case 'S':
                        if      ( strcmp(&unit[2], "RRAN SCIENCE FACILITY"       ) == 0 ) { dest = Sc::Unit::Type::TerranScienceFacility; success = true; }
                        else if ( strcmp(&unit[2], "RRAN SCIENCE VESSEL"         ) == 0 ) { dest = Sc::Unit::Type::TerranScienceVessel; success = true; }
                        else if ( strcmp(&unit[2], "RRAN SCV"                    ) == 0 ) { dest = Sc::Unit::Type::TerranScv; success = true; }
                        else if ( strcmp(&unit[2], "RRAN SIEGE TANK (SIEGE MODE)") == 0 ) { dest = Sc::Unit::Type::TerranSiegeTank_SiegeMode; success = true; }
                        else if ( strcmp(&unit[2], "RRAN SIEGE TANK (TANK MODE)" ) == 0 ) { dest = Sc::Unit::Type::TerranSiegeTank_TankMode; success = true; }
                        else if ( strcmp(&unit[2], "RRAN STARPORT"               ) == 0 ) { dest = Sc::Unit::Type::TerranStarport; success = true; }
                        else if ( strcmp(&unit[2], "RRAN SUPPLY DEPOT"           ) == 0 ) { dest = Sc::Unit::Type::TerranSupplyDepot; success = true; }
                        break;
                    case 'V':
                        if      ( strcmp(&unit[2], "RRAN VALKYRIE"               ) == 0 ) { dest = Sc::Unit::Type::TerranValkrie; success = true; }
                        else if ( strcmp(&unit[2], "RRAN VESPENE GAS TANK TYPE 1") == 0 ) { dest = Sc::Unit::Type::TerranVespeneGasTankType1; success = true; }
                        else if ( strcmp(&unit[2], "RRAN VESPENE GAS TANK TYPE 2") == 0 ) { dest = Sc::Unit::Type::TerranVespeneGasTankType2; success = true; }
                        else if ( strcmp(&unit[2], "RRAN VULTURE"                ) == 0 ) { dest = Sc::Unit::Type::TerranVulture; success = true; }
                        break;
                    case 'W':
                        if ( strcmp(&unit[2], "RRAN WRAITH") == 0 ) { dest = Sc::Unit::Type::TerranWraith; success = true; }
                        break;
                    }
                }
                else if ( unit[1] == 'A' )
                {
                    if      ( strcmp(&unit[2], "SSADAR (TEMPLAR)"       ) == 0 ) { dest = Sc::Unit::Type::Tassadar_Templar; success = true; }
                    else if ( strcmp(&unit[2], "SSADAR/ZERATUL (ARCHON)") == 0 ) { dest = Sc::Unit::Type::TassadarZeratul_Archon; success = true; }
                }
                else if ( unit[1] == 'O' )
                {
                    if      ( strcmp(&unit[2], "M KAZANSKY (WRAITH)") == 0 ) { dest = Sc::Unit::Type::TomKazansky_Wraith; success = true; }
                    else if ( strcmp(&unit[2], "RRASQUE (ULTRALISK)") == 0 ) { dest = Sc::Unit::Type::Torrasque_Ultralisk; success = true; }
                }
                break;
            case 'U':
                if      ( strcmp(&unit[1], "NCLEAN ONE (DEFILER)"      ) == 0 ) { dest = Sc::Unit::Type::UncleanOne_Defiler; success = true; }
                else if ( strcmp(&unit[1], "NUSED PROTOSS BUILDING 1"  ) == 0 ) { dest = Sc::Unit::Type::UnusedProtossBuilding1; success = true; }
                else if ( strcmp(&unit[1], "NUSED PROTOSS BUILDING 2"  ) == 0 ) { dest = Sc::Unit::Type::UnusedProtossBuilding2; success = true; }
                else if ( strcmp(&unit[1], "NUSED ZERG BUILDING 1"     ) == 0 ) { dest = Sc::Unit::Type::UnusedZergBuilding1; success = true; }
                else if ( strcmp(&unit[1], "NUSED ZERG BUILDING 2"     ) == 0 ) { dest = Sc::Unit::Type::UnusedZergBuilding1; success = true; }
                else if ( strcmp(&unit[1], "RAJ CRYSTAL"               ) == 0 ) { dest = Sc::Unit::Type::UrajCrystal; success = true; }
                else if ( strcmp(&unit[1], "RSADON (ICE WORLD CRITTER)") == 0 ) { dest = Sc::Unit::Type::Ursadon_IceWorldCritter; success = true; }
                break;
            case 'V':
                if      ( strcmp(&unit[1], "ESPENE GEYSER") == 0 ) { dest = Sc::Unit::Type::VespeneGeyser; success = true; }
                break;
            case 'W':
                if      ( strcmp(&unit[1], "ARBRINGER (REAVER)") == 0 ) { dest = Sc::Unit::Type::Warbringer_Reaver; success = true; }
                else if ( strcmp(&unit[1], "ARP GATE"          ) == 0 ) { dest = Sc::Unit::Type::WarpGate; success = true; }
                break;
            case 'X':
                if      ( strcmp(&unit[1], "EL'NAGA TEMPLE") == 0 ) { dest = Sc::Unit::Type::XelNagaTemple; success = true; }
                break;
            case 'Y':
                if      ( strcmp(&unit[1], "GGDRASILL (OVERLORD)") == 0 ) { dest = Sc::Unit::Type::Yggdrasill_Overlord; success = true; }
                else if ( strcmp(&unit[1], "OUNG CHRYSALIS"      ) == 0 ) { dest = Sc::Unit::Type::YoungChrysalis; success = true; }
                break;
            case 'Z':
                if ( unit[3] == 'G' )
                {
                    switch ( unit[5] )
                    {
                    case 'B':
                        if      ( strcmp(&unit[1], "ERG BEACON"   ) == 0 ) { dest = Sc::Unit::Type::ZergBeacon; success = true; }
                        else if ( strcmp(&unit[1], "ERG BROODLING") == 0 ) { dest = Sc::Unit::Type::ZergBroodling; success = true; }
                        break;
                    case 'C':
                        if      ( strcmp(&unit[1], "ERG CEREBRATE"        ) == 0 ) { dest = Sc::Unit::Type::ZergCerebrate; success = true; }
                        else if ( strcmp(&unit[1], "ERG CEREBRATE DAGGOTH") == 0 ) { dest = Sc::Unit::Type::ZergCerebrateDaggoth; success = true; }
                        else if ( strcmp(&unit[1], "ERG CREEP COLONY"     ) == 0 ) { dest = Sc::Unit::Type::ZergCreepColony; success = true; }
                        break;
                    case 'D':
                        if      ( strcmp(&unit[1], "ERG DEFILER"      ) == 0 ) { dest = Sc::Unit::Type::ZergDefiler; success = true; }
                        else if ( strcmp(&unit[1], "ERG DEFILER MOUND") == 0 ) { dest = Sc::Unit::Type::ZergDefilerMound; success = true; }
                        else if ( strcmp(&unit[1], "ERG DEVOURER"     ) == 0 ) { dest = Sc::Unit::Type::ZergDevourer; success = true; }
                        else if ( strcmp(&unit[1], "ERG DRONE"        ) == 0 ) { dest = Sc::Unit::Type::ZergDrone; success = true; }
                        break;
                    case 'E':
                        if      ( strcmp(&unit[1], "ERG EGG"              ) == 0 ) { dest = Sc::Unit::Type::ZergEgg; success = true; }
                        else if ( strcmp(&unit[1], "ERG EVOLUTION CHAMBER") == 0 ) { dest = Sc::Unit::Type::ZergEvolutionChamber; success = true; }
                        else if ( strcmp(&unit[1], "ERG EXTRACTOR"        ) == 0 ) { dest = Sc::Unit::Type::ZergExtractor; success = true; }
                        break;
                    case 'F':
                        if ( strcmp(&unit[1], "ERG FLAG BEACON") == 0 ) { dest = Sc::Unit::Type::ZergFlagBeacon; success = true; }
                        break;
                    case 'G':
                        if      ( strcmp(&unit[1], "ERG GREATER SPIRE") == 0 ) { dest = Sc::Unit::Type::ZergGreaterSpire; success = true; }
                        else if ( strcmp(&unit[1], "ERG GUARDIAN"     ) == 0 ) { dest = Sc::Unit::Type::ZergGuardian; success = true; }
                        break;
                    case 'H':
                        if      ( strcmp(&unit[1], "ERG HATCHERY"     ) == 0 ) { dest = Sc::Unit::Type::ZergHatchery; success = true; }
                        else if ( strcmp(&unit[1], "ERG HIVE"         ) == 0 ) { dest = Sc::Unit::Type::ZergHive; success = true; }
                        else if ( strcmp(&unit[1], "ERG HYDRALISK"    ) == 0 ) { dest = Sc::Unit::Type::ZergHydralisk; success = true; }
                        else if ( strcmp(&unit[1], "ERG HYDRALISK DEN") == 0 ) { dest = Sc::Unit::Type::ZergHydraliskDen; success = true; }
                        break;
                    case 'L':
                        if      ( strcmp(&unit[1], "ERG LAIR"  ) == 0 ) { dest = Sc::Unit::Type::ZergLair; success = true; }
                        else if ( strcmp(&unit[1], "ERG LARVA" ) == 0 ) { dest = Sc::Unit::Type::ZergLarva; success = true; }
                        else if ( strcmp(&unit[1], "ERG LURKER") == 0 ) { dest = Sc::Unit::Type::ZergLurker; success = true; }
                        break;
                    case 'M':
                        if      ( strcmp(&unit[1], "ERG MARKER"  ) == 0 ) { dest = Sc::Unit::Type::ZergMarker; success = true; }
                        else if ( strcmp(&unit[1], "ERG MUTALISK") == 0 ) { dest = Sc::Unit::Type::ZergMutalisk; success = true; }
                        break;
                    case 'N':
                        if ( strcmp(&unit[1], "ERG NYDUS CANAL") == 0 ) { dest = Sc::Unit::Type::ZergNydusCanal; success = true; }
                        break;
                    case 'O':
                        if      ( strcmp(&unit[1], "ERG OVERLORD"             ) == 0 ) { dest = Sc::Unit::Type::ZergOverlord; success = true; }
                        else if ( strcmp(&unit[1], "ERG OVERMIND"             ) == 0 ) { dest = Sc::Unit::Type::ZergOvermind; success = true; }
                        else if ( strcmp(&unit[1], "ERG OVERMIND (WITH SHELL)") == 0 ) { dest = Sc::Unit::Type::ZergOvermind_WithShell; success = true; }
                        break;
                    case 'Q':
                        if      ( strcmp(&unit[1], "ERG QUEEN"       ) == 0 ) { dest = Sc::Unit::Type::ZergQueen; success = true; }
                        else if ( strcmp(&unit[1], "ERG QUEEN'S NEST") == 0 ) { dest = Sc::Unit::Type::ZergQueensNest; success = true; }
                        break;
                    case 'S':
                        if      ( strcmp(&unit[1], "ERG SCOURGE"      ) == 0 ) { dest = Sc::Unit::Type::ZergScourge; success = true; }
                        else if ( strcmp(&unit[1], "ERG SPAWNING POOL") == 0 ) { dest = Sc::Unit::Type::ZergSpawningPool; success = true; }
                        else if ( strcmp(&unit[1], "ERG SPIRE"        ) == 0 ) { dest = Sc::Unit::Type::ZergSpire; success = true; }
                        else if ( strcmp(&unit[1], "ERG SPORE COLONY" ) == 0 ) { dest = Sc::Unit::Type::ZergSporeColony; success = true; }
                        else if ( strcmp(&unit[1], "ERG SUNKEN COLONY") == 0 ) { dest = Sc::Unit::Type::ZergSunkenColony; success = true; }
                        break;
                    case 'U':
                        if      ( strcmp(&unit[1], "ERG ULTRALISK"       ) == 0 ) { dest = Sc::Unit::Type::ZergUltralisk; success = true; }
                        else if ( strcmp(&unit[1], "ERG ULTRALISK CAVERN") == 0 ) { dest = Sc::Unit::Type::ZergUltraliskCavern; success = true; }
                        break;
                    case 'V':
                        if      ( strcmp(&unit[1], "ERG VESPENE GAS SAC TYPE 1") == 0 ) { dest = Sc::Unit::Type::ZergVespeneGasSacType1; success = true; }
                        else if ( strcmp(&unit[1], "ERG VESPENE GAS SAC TYPE 2") == 0 ) { dest = Sc::Unit::Type::ZergVespeneGasSacType2; success = true; }
                        break;
                    case 'Z':
                        if ( strcmp(&unit[1], "ERG ZERGLING") == 0 ) { dest = Sc::Unit::Type::ZergZergling; success = true; }
                        break;
                    }
                }
                else if ( strcmp(&unit[1], "ERATUL (DARK TEMPLAR)") == 0 ) { dest = Sc::Unit::Type::Zeratul_DarkTemplar; success = true; }
                break;
            }
        }

        if ( !success ) // Now search the unit name table
        {
            std::string str(unitNamePtr);
            size_t hash = strHash(str);
            size_t numMatching = unitTable.count(hash);
            if ( numMatching == 1 )
            { // Should guarentee that you can find at least one entry
                UnitTableNode & node = unitTable.find(hash)->second;
                if ( node.unitName.compare(unitNamePtr) == 0 )
                {
                    dest = node.unitType;
                    success = true;
                }
            }
            else if ( numMatching > 1 )
            {
                auto range = unitTable.equal_range(hash);
                foreachin(pair, range)
                {
                    UnitTableNode & node = pair->second;
                    if ( node.unitName.compare(unitNamePtr) == 0 )
                    {
                        if ( success == false ) // If no matches have previously been found
                        {
                            dest = node.unitType;
                            success = true;
                        }
                        else // If matches have previously been found
                        {
                            if ( node.unitType < dest )
                                dest = node.unitType; // Replace if unitID < previous unitID
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
                if      ( strcmp(&unit[1], "ANY UNIT]" ) == 0 ) { dest = Sc::Unit::Type::AnyUnit; success = true; }
                else if ( strcmp(&unit[1], "BUILDINGS]") == 0 ) { dest = Sc::Unit::Type::Buildings; success = true; }
                else if ( strcmp(&unit[1], "FACTORIES]") == 0 ) { dest = Sc::Unit::Type::Factories; success = true; }
                else if ( strcmp(&unit[1], "MEN]"      ) == 0 ) { dest = Sc::Unit::Type::Men; success = true; }
                break;
            case 'A':
                if ( strcmp(&unit[1], "LAN TURRET") == 0 ) { dest = Sc::Unit::Type::AlanTurret; success = true; }
                break;
            case 'B':
                if ( strcmp(&unit[1], "ENGALAAS (JUNGLE)") == 0 ) { dest = Sc::Unit::Type::Bengalaas_Jungle; success = true; }
                break;
            case 'C':
                if      ( strcmp(&unit[1], "ANTINA") == 0 ) { dest = Sc::Unit::Type::Cantina; success = true; }
                else if ( strcmp(&unit[1], "AVE"   ) == 0 ) { dest = Sc::Unit::Type::Cave; success = true; }
                else if ( strcmp(&unit[1], "AVE-IN") == 0 ) { dest = Sc::Unit::Type::CaveIn; success = true; }
                else if ( strcmp(&unit[1], "OCOON" ) == 0 ) { dest = Sc::Unit::Type::Cocoon; success = true; }
                break;
            case 'D':
                if      ( strcmp(&unit[1], "ARK TEMPLAR (HERO)") == 0 ) { dest = Sc::Unit::Type::DarkTemplar_Hero; success = true; }
                else if ( strcmp(&unit[1], "ISRUPTION FIELD"   ) == 0 ) { dest = Sc::Unit::Type::DisruptionField; success = true; }
                else if ( strcmp(&unit[1], "UKE TURRET TYPE 1" ) == 0 ) { dest = Sc::Unit::Type::DukeTurretType1; success = true; }
                else if ( strcmp(&unit[1], "UKE TURRET TYPE 2" ) == 0 ) { dest = Sc::Unit::Type::DukeTurretType2; success = true; }
                break;
            case 'E':
                if ( strcmp(&unit[1], "DMUND DUKE (SIEGE TANK)") == 0 ) { dest = Sc::Unit::Type::EdmundDuke_SiegeTank; success = true; }
                break;
            case 'G':
                if ( strcmp(&unit[1], "ERARD DUGALLE (GHOST)") == 0 ) { dest = Sc::Unit::Type::GerardDuGalle_BattleCruiser; success = true; }
                break;
            case 'I':
                if      ( strcmp(&unit[1], "NDEPENDENT COMMAND CENTER"         ) == 0 ) { dest = Sc::Unit::Type::IndependentCommandCenter_Unused; success = true; }
                else if ( strcmp(&unit[1], "NDEPENDENT STARPORT"               ) == 0 ) { dest = Sc::Unit::Type::IndependentStarport_Unused; success = true; }
                else if ( strcmp(&unit[1], "NFESTED DURAN"                     ) == 0 ) { dest = Sc::Unit::Type::InfestedDuran; success = true; }
                else if ( strcmp(&unit[1], "NFESTED KERRIGAN (INFESTED TERRAN)") == 0 ) { dest = Sc::Unit::Type::InfestedKerrigan_InfestedTerran; success = true; }
                break;
            case 'J':
                if ( strcmp(&unit[1], "UMP GATE") == 0 ) { dest = Sc::Unit::Type::IndependentJumpGate_Unused; success = true; }
                break;
            case 'K':
                if      ( strcmp(&unit[1], "AKARU (TWILIGHT)"         ) == 0 ) { dest = Sc::Unit::Type::Kakaru_TwilightCritter; success = true; }
                else if ( strcmp(&unit[1], "YADARIN CRYSTAL FORMATION") == 0 ) { dest = Sc::Unit::Type::KhaydarinCrystalFormation; success = true; }
                break;
            case 'M':
                if      ( strcmp(&unit[1], "INING PLATFORM"       ) == 0 ) { dest = Sc::Unit::Type::MiningPlatform_Unused; success = true; }
                else if ( strcmp(&unit[1], "INERAL CHUNK (TYPE 1)") == 0 ) { dest = Sc::Unit::Type::MineralClusterType1; success = true; }
                else if ( strcmp(&unit[1], "INERAL CHUNK (TYPE 2)") == 0 ) { dest = Sc::Unit::Type::MineralClusterType2; success = true; }
                break;
            case 'N':
                if ( strcmp(&unit[1], "ORAD II (CRASHED BATTLECRUISER)") == 0 ) { dest = Sc::Unit::Type::NoradII_Crashed; success = true; }
                break;
            case 'P':
                if      ( strcmp(&unit[1], "ROTOSS DARK TEMPLAR" ) == 0 ) { dest = Sc::Unit::Type::ProtossDarkTemplar; success = true; }
                else if ( strcmp(&unit[1], "ROTOSS UNUSED TYPE 1") == 0 ) { dest = Sc::Unit::Type::UnusedProtossBuilding1; success = true; }
                else if ( strcmp(&unit[1], "ROTOSS UNUSED TYPE 2") == 0 ) { dest = Sc::Unit::Type::UnusedProtossBuilding2; success = true; }
                break;
            case 'R':
                if      ( strcmp(&unit[1], "AGNASAUR (ASH WORLD)"  ) == 0 ) { dest = Sc::Unit::Type::Ragnasaur_AshworldCritter; success = true; }
                else if ( strcmp(&unit[1], "UINS"                  ) == 0 ) { dest = Sc::Unit::Type::Ruins_Unused; success = true; }
                else if ( strcmp(&unit[1], "HYNADON (BADLANDS)"    ) == 0 ) { dest = Sc::Unit::Type::Rhynadon_BadlandsCritter; success = true; }
                else if ( strcmp(&unit[1], "ASZAGAL (DARK TEMPLAR)") == 0 ) { dest = Sc::Unit::Type::Raszagal_Corsair; success = true; }
            case 'S':
                if ( strcmp(&unit[1], "CANTID (DESERT)") == 0 ) { dest = Sc::Unit::Type::Scantid_DesertCritter; success = true; }
                break;
            case 'T':
                if      ( strcmp(&unit[1], "ANK TURRET TYPE 1") == 0 ) { dest = Sc::Unit::Type::SiegeTankTurret_TankMode; success = true; }
                else if ( strcmp(&unit[1], "ANK TURRET TYPE 2") == 0 ) { dest = Sc::Unit::Type::SiegeTankTurret_SiegeMode; success = true; }
                break;
            case 'U':
                if      ( strcmp(&unit[1], "NUSED TERRAN BLDG TYPE 1") == 0 ) { dest = Sc::Unit::Type::Starbase_Unused; success = true; }
                else if ( strcmp(&unit[1], "NUSED TERRAN BLDG TYPE 2") == 0 ) { dest = Sc::Unit::Type::RepairBay_Unused; success = true; }
                else if ( strcmp(&unit[1], "NUSED TYPE 1"            ) == 0 ) { dest = Sc::Unit::Type::CargoShip_Unused; success = true; }
                else if ( strcmp(&unit[1], "NUSED TYPE 2"            ) == 0 ) { dest = Sc::Unit::Type::MercenaryGunship_Unused; success = true; }
                else if ( strcmp(&unit[1], "NUSED ZERG BLDG"         ) == 0 ) { dest = Sc::Unit::Type::UnusedZergBuilding1; success = true; }
                else if ( strcmp(&unit[1], "NUSED ZERG BLDG 5"       ) == 0 ) { dest = Sc::Unit::Type::UnusedZergBuilding2; success = true; }
                else if ( strcmp(&unit[1], "RSADON (ICE WORLD)"      ) == 0 ) { dest = Sc::Unit::Type::Ursadon_IceWorldCritter; success = true; }
                break;
            case 'V':
                if      ( strcmp(&unit[1], "ULTURE SPIDER MINE"         ) == 0 ) { dest = Sc::Unit::Type::SpiderMine; success = true; }
                else if ( strcmp(&unit[1], "ESPENE TANK (TERRAN TYPE 1)") == 0 ) { dest = Sc::Unit::Type::TerranVespeneGasTankType1; success = true; }
                else if ( strcmp(&unit[1], "ESPENE TANK (TERRAN TYPE 2)") == 0 ) { dest = Sc::Unit::Type::TerranVespeneGasTankType2; success = true; }
                else if ( strcmp(&unit[1], "ESPENE ORB (PROTOSS TYPE 1)") == 0 ) { dest = Sc::Unit::Type::ProtossVespeneGasOrbType1; success = true; }
                else if ( strcmp(&unit[1], "ESPENE ORB (PROTOSS TYPE 2)") == 0 ) { dest = Sc::Unit::Type::ProtossVespeneGasOrbType2; success = true; }
                else if ( strcmp(&unit[1], "ESPENE SAC (ZERG TYPE 1)"   ) == 0 ) { dest = Sc::Unit::Type::ZergVespeneGasSacType1; success = true; }
                else if ( strcmp(&unit[1], "ESPENE SAC (ZERG TYPE 2)"   ) == 0 ) { dest = Sc::Unit::Type::ZergVespeneGasSacType2; success = true; }
                break;
            case 'Z':
                if ( strcmp(&unit[1], "ERG LURKER EGG") == 0 ) { dest = Sc::Unit::Type::LurkerEgg; success = true; }
                break;
            }
        }

        // Remove the temporary NUL char
        unitNamePtr[size] = temp;
    }
    return success;
}

bool TextTrigCompiler::ParseWavName(buffer & text, u32 & dest, s64 pos, s64 end)
{
    if ( text.hasCaseless("No WAV", 0, 6) || text.hasCaseless("\"No WAV\"", 0, 8) )
    {
        dest = 0;
        return true;
    }
    else
        return zzParseString(text, dest, pos, end);
}

bool TextTrigCompiler::ParsePlayer(buffer & text, u32 & dest, s64 pos, s64 end)
{
    s64 size = end - pos;
    u32 number;

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
        else if ( arg.get<u8>(i) == '\t' ) // Del tabbing
            arg.del<u8>(i);
    }
    arg.add<u8>('\0');

    char currChar = toupper(arg.get<u8>(0));
    char* argPtr;

    if ( currChar == 'P' )
    {
        currChar = arg.get<u8>(1);
        if ( currChar == 'L' )
        {
            if ( arg.hasCaseless("AYER", 2, 4) )
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
        currChar = toupper(arg.get<u8>(1));
        if ( currChar == 'O' )
        {
            if ( arg.hasCaseless("RCE", 2, 3) )
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
            else if ( arg.hasCaseless("ES", 2, 2) )
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
        currChar = toupper(arg.get<u8>(1));
        if ( currChar == 'L' )
        {
            if ( arg.hasCaseless("LPLAYERS", 2, 8) )
            {
                // Do something with all players
                dest = 17;
                return true;
            }
            else if ( arg.hasCaseless("LIES", 2, 4) )
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
        if ( arg.hasCaseless("URRENTPLAYER", 1, 12) )
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
        size_t hash = strHash(str);
        size_t numMatching = groupTable.count(hash);
        if ( numMatching == 1 )
        { // Should guarentee that you can find at least one entry
            GroupTableNode & node = groupTable.find(hash)->second;
            if ( node.groupName.compare(groupStrPtr) == 0 )
            {
                dest = node.groupId;
                success = true;
            }
        }
        else if ( numMatching > 1 )
        {
            auto range = groupTable.equal_range(hash);
            foreachin(pair, range)
            {
                GroupTableNode & node = pair->second;
                if ( node.groupName.compare(groupStrPtr) == 0 )
                {
                    if ( success == false ) // If no matches have previously been found
                    {
                        dest = node.groupId;
                        success = true;
                    }
                    else // If matches have previously been found
                    {
                        if ( u32(node.groupId) < dest )
                            dest = node.groupId; // Replace if groupID < previous groupID
                    }
                }
            }
        }
        groupStrPtr[size] = temp;
    }
    return success;
}

bool TextTrigCompiler::ParseSwitch(buffer & text, u8 & dest, s64 pos, s64 end)
{
    if ( text.get<u8>(pos) == '\"' ) // If quoted, ignore quotes
    {
        pos ++;
        end --;
    }
    else if ( ParseByte((char*)text.getPtr(0), dest, pos, end) )
        return true;

    s64 size = end-pos;
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
            size_t hash = strHash(str);
            size_t numMatching = switchTable.count(hash);
            if ( numMatching == 1 )
            { // Should guarentee that you can find at least one entry
                SwitchTableNode & node = switchTable.find(hash)->second;
                if ( node.switchName.compare(switchNamePtr) == 0 )
                {
                    dest = node.switchId;
                    success = true;
                }
            }
            else if ( numMatching > 1 )
            {
                auto range = switchTable.equal_range(hash);
                foreachin(pair, range)
                {
                    SwitchTableNode & node = pair->second;
                    if ( node.switchName.compare(switchNamePtr) == 0 )
                    {
                        if ( success == false ) // If no matches have previously been found
                        {
                            dest = node.switchId;
                            success = true;
                        }
                        else // If matches have previously been found
                        {
                            if ( node.switchId < dest )
                                dest = node.switchId; // Replace if switchID < previous switchId
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

bool TextTrigCompiler::ParseSwitch(buffer & text, u32 & dest, s64 pos, s64 end)
{
    u8 temp = 0;
    bool success = ParseSwitch(text, temp, pos, end);
    dest = temp;
    return success;
}

bool TextTrigCompiler::ParseScript(buffer & text, u32 & dest, s64 pos, s64 end)
{
    if ( text.hasCaseless("NOSCRIPT", pos, 8) || text.hasCaseless("No Script", pos, 9) || text.hasCaseless("\"No Script\"", pos, 11) )
    {
        dest = 0;
        return true;
    }

    s64 size = end - pos;
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
        size_t hash = strHash(str);
        size_t numMatching = scriptTable.count(hash);
        if ( numMatching == 1 )
        { // Should guarentee that you can find at least one entry
            ScriptTableNode & node = scriptTable.find(hash)->second;
            if ( node.scriptName.compare(scriptStringPtr) == 0 )
            {
                dest = node.scriptId;
                success = true;
            }
        }
        else if ( numMatching > 1 )
        {
            auto range = scriptTable.equal_range(hash);
            foreachin(pair, range)
            {
                ScriptTableNode & node = pair->second;
                if ( node.scriptName.compare(scriptStringPtr) == 0 && success == false ) // Compare equal and no prev matches
                {
                    dest = node.scriptId;
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

Chk::Condition::Type TextTrigCompiler::ExtendedToRegularCID(Chk::Condition::VirtualType conditionType)
{
    switch ( conditionType )
    {
        // Don't include CID_CUSTOM, that is set while parsing args
    case Chk::Condition::VirtualType::Memory:
        return (Chk::Condition::Type)Chk::Condition::ExtendedBaseType::Memory;
        break;
    }
    return (Chk::Condition::Type)conditionType;
}

Chk::Action::Type TextTrigCompiler::ExtendedToRegularAID(Chk::Action::VirtualType actionType)
{
    switch ( actionType )
    {
        // Don't include AID_CUSTOM, that is set while parsing args
    case Chk::Action::VirtualType::SetMemory:
        return (Chk::Action::Type)Chk::Action::ExtendedBaseType::SetMemory;
        break;
    }
    return (Chk::Action::Type)actionType;
}

s32 TextTrigCompiler::ExtendedNumConditionArgs(Chk::Condition::VirtualType conditionType)
{
    switch ( conditionType )
    {
    case Chk::Condition::VirtualType::Custom:
        return 9;
    case Chk::Condition::VirtualType::Memory:
        return 3;
    }
    return 0;
}

s32 TextTrigCompiler::ExtendedNumActionArgs(Chk::Action::VirtualType actionType)
{
    switch ( actionType )
    {
    case Chk::Action::VirtualType::Custom:
        return 11;
    case Chk::Action::VirtualType::SetMemory:
        return 3;
    }
    return 0;
}

// private

bool TextTrigCompiler::useNextString(u32 & index)
{
    for ( size_t i=1; i>0 && i<Chk::MaxStrings; i++ )
    {
        if ( !stringUsed[i] )
        {
            index = (u32)i;
            return true;
        }
    }
    return false;
}

bool TextTrigCompiler::useNextExtendedString(u32 & index)
{
    for ( size_t i=1; i>0 && i<Chk::MaxStrings; i++ )
    {
        if ( !extendedStringUsed[i] )
        {
            index = (u32)i;
            return true;
        }
    }
    return false;
}

bool TextTrigCompiler::PrepLocationTable(ScenarioPtr map)
{
    LocationTableNode locNode;
    locationTable.reserve(map->layers.numLocations()+1);
    locNode.locationId = 0;
    locNode.locationName = "No Location";
    locationTable.insert(std::pair<size_t, LocationTableNode>(strHash(locNode.locationName), locNode));
    for ( u32 i=1; i<=map->layers.numLocations(); i++ )
    {
        Chk::LocationPtr loc = map->layers.getLocation(i);
        locNode.locationName = "";

        if ( i == Chk::LocationId::Anywhere )
        {
            locNode.locationId = Chk::LocationId::Anywhere;
            locNode.locationName = "Anywhere";
            locationTable.insert( std::pair<size_t, LocationTableNode>(strHash(locNode.locationName), locNode) );
        }
        else if ( loc->stringId > 0 )
        {
            RawStringPtr locationName = map->strings.getString<RawString>(loc->stringId);
            if ( locationName != nullptr )
            {
                locNode.locationId = u8(i+1);
                locNode.locationName = *locationName;
                locationTable.insert( std::pair<size_t, LocationTableNode>(strHash(locNode.locationName), locNode) );
            }
        }
    }
    locationTable.reserve(locationTable.size());
    return true;
}

bool TextTrigCompiler::PrepUnitTable(ScenarioPtr map)
{
    UnitTableNode unitNode;
    u16 stringId = 0;
    for ( u16 unitId=0; unitId<Sc::Unit::TotalTypes; unitId++ )
    {
        unitNode.unitType = (Sc::Unit::Type)unitId;
        RawStringPtr unitName = map->strings.getUnitName<RawString>((Sc::Unit::Type)unitId, true);
        if ( unitName != nullptr )
            unitNode.unitName = *unitName;
        else
            unitNode.unitName = Sc::Unit::defaultDisplayNames[unitId];
        
        unitTable.insert( std::pair<size_t, UnitTableNode>(strHash(unitNode.unitName), unitNode) );
    }
    return true;
}

bool TextTrigCompiler::PrepSwitchTable(ScenarioPtr map)
{
    SwitchTableNode switchNode;
    size_t stringId = 0;
    for ( size_t switchIndex=0; switchIndex<256; switchIndex++ )
    {
        size_t switchNameStringId = map->triggers.getSwitchNameStringId(switchIndex);
        if ( switchNameStringId != Chk::StringId::NoString )
        {
            RawStringPtr switchName = map->strings.getString<RawString>(switchNameStringId);
            if ( switchName != nullptr )
            {
                switchNode.switchId = u8(switchIndex);
                switchNode.switchName = *switchName;
                switchTable.insert( std::pair<size_t, SwitchTableNode>(strHash(switchNode.switchName), switchNode) );
            }
        }
    }
    return true;
}

bool TextTrigCompiler::PrepGroupTable(ScenarioPtr map)
{
    GroupTableNode groupNode;
    for ( u32 i=0; i<4; i++ )
    {
        ChkdStringPtr forceName = map->strings.getForceName<ChkdString>((Chk::Force)i);
        if ( forceName != nullptr )
        {
            groupNode.groupId = i + 18;
            groupNode.groupName = *forceName;
            groupTable.insert(std::pair<size_t, GroupTableNode>(strHash(groupNode.groupName), groupNode));
        }
    }
    return true;
}

bool TextTrigCompiler::PrepStringTable(ScenarioPtr map)
{
    map->strings.markValidUsedStrings(stringUsed, Chk::Scope::Either, Chk::Scope::Game);
    size_t stringCapacity = map->strings.getCapacity(Chk::Scope::Game);
    for ( size_t stringId=1; stringId<stringCapacity; stringId++ )
    {
        if ( stringUsed[stringId] )
        {
            RawStringPtr rawString = map->strings.getString<RawString>(stringId, Chk::Scope::Game);
            if ( rawString != nullptr )
            {
                zzStringTableNode node;
                node.scStr = ScStrPtr(new ScStr(*rawString));
                node.stringId = (u32)stringId;
                zzStringTable.insert(std::pair<size_t, zzStringTableNode>(strHash(*rawString), node));
            }
        }
    }
    return true;
}

bool TextTrigCompiler::PrepExtendedStringTable(ScenarioPtr map)
{
    map->strings.markValidUsedStrings(extendedStringUsed, Chk::Scope::Either, Chk::Scope::Editor);
    size_t extendedStringCapacity = map->strings.getCapacity(Chk::Scope::Editor);
    for ( size_t stringId=1; stringId<extendedStringCapacity; stringId++ )
    {
        if ( stringUsed[stringId] )
        {
            RawStringPtr rawString = map->strings.getString<RawString>(stringId, Chk::Scope::Editor);
            if ( rawString != nullptr )
            {
                zzStringTableNode node;
                node.scStr = ScStrPtr(new ScStr(*rawString));
                node.stringId = (u32)stringId;
                extendedStringTable.insert(std::pair<size_t, zzStringTableNode>(strHash(*rawString), node));
            }
        }
    }
    return true;
}

bool TextTrigCompiler::PrepScriptTable(ScData & scData)
{
    std::string aiName;
    size_t numScripts = scData.ai.numEntries();
    for ( size_t i = 0; i < numScripts; i++ )
    {
        ScriptTableNode scriptNode;
        const Sc::Ai::Entry & entry = scData.ai.getEntry(i);
        if ( scData.ai.getName(i, aiName) )
            scriptTable.insert(std::pair<size_t, ScriptTableNode>(strHash(scriptNode.scriptName), scriptNode));
    }
    return true;
}

bool TextTrigCompiler::BuildNewStringTable(ScenarioPtr map, std::stringstream & error)
{
    return map->strings.addStrings(zzAddedStrings, Chk::Scope::Game, true) && map->strings.addStrings(addedExtendedStrings, Chk::Scope::Editor, true);
}
