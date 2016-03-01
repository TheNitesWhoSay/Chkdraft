#include "StringUsage.h"

StringUsageTable::StringUsageTable()
{

}

bool StringUsageTable::populateTable(Scenario* chk, bool extendedTable)
{
    /*buffer* strings = &chk->STR();
    if ( extendedTable )
        strings = &chk->KSTR();*/

    if ( chk->hasStrSection(extendedTable) )
    {
        u32 currMaxStrings = 0;
        if ( extendedTable )
            currMaxStrings = chk->kstrSectionCapacity();
        else
            currMaxStrings = chk->strSectionCapacity();

        if ( stringUsed.add<u8>(0, 1+currMaxStrings) ) // Add 1 unused byte and 1 byte for each potential string
        {
            stringUsed.replace<u8>(0, 1); // Mark the unused byte as a 'used' string

            /*buffer& MRGN = chk->MRGN(),
                  & MBRF = chk->MBRF(),
                  & SPRP = chk->SPRP(),
                  & WAV  = chk->WAV (),
                  & UNIS = chk->UNIS(),
                  & SWNM = chk->SWNM(),
                  & UNIx = chk->UNIx();*/

            #define MarkIfOverZero(index)                         \
                if ( index > 0 )                                  \
                {                                                 \
                    if ( extendedTable )                          \
                        stringUsed.replace<u8>(65536-index, 1);   \
                    else                                          \
                        stringUsed.replace<u8>(index, 1);         \
                }

            // MRGN - location strings
            ChkLocation* loc;
            for ( u32 i=0; i<chk->locationCapacity(); i++ )
            {
                if ( chk->getLocation(loc, u8(i)) )
                    MarkIfOverZero(loc->stringNum);
            }

            // TRIG - trigger strings
            Trigger* trig;
            int trigNum = 0;

            while ( chk->getTrigger(trig, trigNum) )
            {
                for ( int i=0; i<NUM_TRIG_ACTIONS; i++ )
                {
                    MarkIfOverZero( trig->actions[i].stringNum );
                    MarkIfOverZero( trig->actions[i].wavID );
                }
            
                trigNum ++;
            }

            // MBRF - briefing strings
            trigNum = 0;

            while ( chk->getBriefingTrigger(trig, trigNum) )
            {
                for ( int i=0; i<NUM_TRIG_ACTIONS; i++ )
                {
                    MarkIfOverZero( trig->actions[i].stringNum );
                    MarkIfOverZero( trig->actions[i].wavID );
                }

                trigNum ++;
            }
    
            // SPRP - scenario property strings
            MarkIfOverZero(chk->GetMapTitleStrIndex());
            MarkIfOverZero(chk->GetMapDescriptionStrIndex());

            // FORC - force strings
            for ( int i=0; i<4; i++ )
                MarkIfOverZero( chk->getForceStringNum(i) );

            // WAV  - sound strings
            for ( u32 i = 0; i < chk->WavSectionCapacity(); i++ )
            {
                u32 stringId = 0;
                if ( chk->GetWav(i, stringId) )
                    MarkIfOverZero(stringId);
            }

            // UNIS - unit settings strings (vanilla)
            for ( int i = 0; i < 228; i++ )
            {
                u16 stringId = 0;
                if ( chk->getUnisStringId(i, stringId) )
                       MarkIfOverZero(stringId);
            }

            // SWNM - switch strings
            for ( int i = 0; i < 256; i++ )
            {
                u32 stringId = 0;
                if ( chk->getSwitchStrId(i, stringId) )
                    MarkIfOverZero(stringId);
            }

            // UNIx - unit settings strings (brood war)
            for ( int i = 0; i < 228; i++ )
            {
                u16 stringId = 0;
                if ( chk->getUnixStringId(i, stringId) )
                    MarkIfOverZero(stringId);
            }

            return true;
        }
        else
        {
            CHKD_ERR("Failed to allocate string usage table.");
            return false;
        }
    }
    else // The string section did not exist, convey that information
    {
        if ( stringUsed.add<u8>(1) ) // Mark the unused byte as a 'used' string
            return true;
        else
            return false;
    }
}

bool StringUsageTable::populateTable(std::list<StringTableNode> strList, u32 numStrs)
{
    if ( stringUsed.add<u8>(0, numStrs+1) ) // Room for all strings + first unused byte
    {
        strList.sort(CompareStrTblNode);
        for ( auto &str : strList )
            stringUsed.replace<u8>(str.stringNum, 1);

        return true;
    }
    else
        return false;
}

void StringUsageTable::clearTable()
{
    stringUsed.flush();
}

bool StringUsageTable::useNext(u32 &index)
{
    for ( u32 i=1; i<stringUsed.size(); i++ )
    {
        if ( stringUsed.get<u8>(i) == 0 )
        {
            stringUsed.replace<u8>(i, 1);
            index = i;
            return true;
        }
    }

    // No open slots, add another
    if ( stringUsed.add<u8>(1) )
    {
        index = stringUsed.size()-1;
        return true;
    }
    else // Prolly out of memory
        index = 0;

    return false;
}

bool StringUsageTable::useNext(u16 &index)
{
    for ( u32 i = 1; i<stringUsed.size() && i < 65536; i++ )
    {
        if ( stringUsed.get<u8>(i) == 0 )
        {
            stringUsed.replace<u8>(i, 1);
            index = (u16)i;
            return true;
        }
    }

    // No open slots, add another
    if ( stringUsed.size() < 65536 && stringUsed.add<u8>(1) )
    {
        index = (u16)(stringUsed.size() - 1);
        return true;
    }
    else // Prolly out of memory
        index = 0;

    return false;
}

bool StringUsageTable::isUsed(u32 index)
{
    return stringUsed.get<u8>(index) == 1; // If it can't find, it equals 0 and returns that it's unused
}

u32 StringUsageTable::numStrings()
{
    return stringUsed.size();
}

u32 StringUsageTable::lastUsedString()
{
    u32 max = numStrings();
    u8* info = (u8*)stringUsed.getPtr(0);
    for ( u32 i=max-1; i>=0; i-- )
    {
        if ( info[i] == 1 )
            return i;
    }
    return 0;
}

bool StringUsageTable::popFragmentedString(u32& firstEmpty, u32 &lastFragmented)
{
    u8* info = (u8*)stringUsed.getPtr(0);
    for ( u32 i=numStrings()-1; i>0; i-- )
    {
        if ( info[i] == 1 )
        {
            for ( u32 j=1; j<numStrings(); j++ )
            {
                if ( info[j] == 0 && j < i )
                {
                    info[i] = 0;
                    info[j] = 1;
                    lastFragmented = i;
                    firstEmpty = j;
                    return true;
                }
            }
        }
    }
    lastFragmented = 0;
    firstEmpty = 0;
    return false;
}
