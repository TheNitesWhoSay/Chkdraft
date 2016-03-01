#ifndef STRINGUSAGE_H
#define STRINGUSAGE_H
#include "Scenario.h"
#include "Basics.h"
#include <list>

class StringUsageTable
{
    public:

        StringUsageTable();

        // Populates stringUsed table using the map, success if table could be populated
        bool populateTable(Scenario* chk, bool extendedTable);

        // Populates the stringUsed table using a list of strings, success if table could be populated
        bool populateTable(std::list<StringTableNode> strList, u32 numStrs);

        void clearTable(); // Clears the table if populated

        bool useNext(u32 &index); /** Next avaliable string index stored in 'index', marking 'index' as unavaliable
                                        returns false if no index can plausibly be used. */
        bool useNext(u16 &index);

        bool isUsed(u32 index); // Returns if a given index is in use

        u32 numStrings(); // Returns number of strings present in this table

        u32 lastUsedString(); // Returns the highest used string in this table

        bool popFragmentedString(u32& firstEmpty, u32 &lastFragmented); // Returns the last fragmented string and where it should go

    private:

        buffer stringUsed; /** Array of bytes, byte at stringNum is 1 if used, 0 if not
                               First byte is always unused/set to 1 */
};

#endif