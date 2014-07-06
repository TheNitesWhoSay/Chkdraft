#include "StringUsage.h"

StringUsageTable::StringUsageTable(Scenario* chk, bool extendedTable, bool& success)
{
	buffer* strings = &chk->STR();
	if ( extendedTable )
		strings = &chk->KSTR();

	if ( strings->exists() )
	{
		u32 currMaxStrings;
		if ( extendedTable)
			currMaxStrings = strings->get<u32>(4);
		else
			currMaxStrings = chk->numStrings();

		if ( stringUsed.add<u8>(0, 1+currMaxStrings) ) // Add 1 unused byte and 1 byte for each potential string
		{
			stringUsed.replace<u8>(0, 1); // Mark the unused byte as a 'used' string

			buffer& MRGN = chk->MRGN(),
				  & TRIG = chk->TRIG(),
				  & MBRF = chk->MBRF(),
				  & SPRP = chk->SPRP(),
				  & FORC = chk->FORC(),
				  & WAV	 = chk->WAV (),
				  & UNIS = chk->UNIS(),
				  & SWNM = chk->SWNM(),
				  & UNIx = chk->UNIx();

			#define MarkIfOverZero(index)						  \
				if ( index > 0 )								  \
				{												  \
					if ( extendedTable )						  \
						stringUsed.replace<u8>(65536-index, 1);	  \
					else										  \
						stringUsed.replace<u8>(index, 1);		  \
				}

			// MRGN - location strings
			ChkLocation* loc;
			for ( u32 i=0; i<MRGN.size()/CHK_LOCATION_SIZE; i++ )
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
					MarkIfOverZero( trig->actions[i].stringNum );
			
				trigNum ++;
			}

			// MBRF - briefing strings
			trigNum = 0;

			while ( chk->getBriefingTrigger(trig, trigNum) )
			{
				for ( int i=0; i<NUM_TRIG_ACTIONS; i++ )
					MarkIfOverZero( trig->actions[i].stringNum );

				trigNum ++;
			}
	
			// SPRP - scenario property strings
			u16 strIndex = SPRP.get<u16>(0); // Scenario Name
			MarkIfOverZero(strIndex);
			strIndex = SPRP.get<u16>(2); // Scenario Description
			MarkIfOverZero(strIndex);

			// FORC - force strings
			for ( int i=0; i<4; i++ )
				MarkIfOverZero( chk->getForceStringNum(i) );

			// WAV  - sound strings
			for ( u32 i=0; i<WAV.size()/4; i++ )
				MarkIfOverZero( WAV.get<u32>(i*4) );

			// UNIS - unit settings strings (vanilla)
			for ( int i=0; i<228; i++ )
				MarkIfOverZero( UNIS.get<u16>(UNIT_SETTINGS_STRING_IDS+i*2) );

			// SWNM - switch strings
			for ( int i=0; i<256; i++ )
				MarkIfOverZero( SWNM.get<u32>(i*4) );

			// UNIx - unit settings strings (brood war)
			for ( int i=0; i<228; i++ )
				MarkIfOverZero( UNIx.get<u16>(UNIT_SETTINGS_STRING_IDS+i*2) );

			success = true;
		}
		else
		{
			CHKD_ERR("Failed to allocate string usage table.\n\n%s", LastError);
			success = false;
		}
	}
	else // The string section did not exist, convey that information
	{
		if ( stringUsed.add<u8>(1) )// Mark the unused byte as a 'used' string
			success = true;
		else
			success = false;
	}
}

StringUsageTable::StringUsageTable(std::list<StringTableNode> strList, u32 numStrs, bool& success)
{
	if ( !stringUsed.add<u8>(0, numStrs+1) ) // Room for all strings + first unused byte
		success = false;
	else
		success = true;

	strList.sort(CompareStrTblNode);
	for ( auto it = strList.begin(); it != strList.end(); it++ )
		stringUsed.replace<u8>(it->stringNum, 1);
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