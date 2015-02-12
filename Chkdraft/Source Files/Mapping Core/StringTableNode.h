#ifndef STRINGTABLENODE_H
#define STRINGTABLENODE_H
#include "Basics.h"

class StringTableNode // An object comprehensively representing a StarCraft string
{
	public:

		std::string string; // A given string from the map
		u32 stringNum; // The ID associated with this string
		bool isExtended; // Whether or not the given string is extended
		u32 propStruct; // propStruct (extended strings only)

		StringTableNode(); // ctor
		bool operator== (StringTableNode other); // Check for complete equivalency
		bool operator< (StringTableNode other); // Check if one string num is less than another
};

bool CompareStrTblNode(StringTableNode first, StringTableNode second); // Returns whether the first should be sorted below the second

#endif