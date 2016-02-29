#include "StringTableNode.h"

StringTableNode::StringTableNode() : string(""), stringNum(0), isExtended(false), propStruct(0)
{

}

bool StringTableNode::operator== (StringTableNode other)
{
    return (stringNum == other.stringNum) &&
         (isExtended == other.isExtended) &&
         (propStruct == other.propStruct) &&
         (string.compare(other.string) == 0);
}

bool StringTableNode::operator< (StringTableNode other)
{
    return stringNum < other.stringNum;
}

bool CompareStrTblNode(StringTableNode first, StringTableNode second)
{
    return first.stringNum < second.stringNum;
}

bool strIsInHashTable(RawString &str, std::hash<std::string> &strHash,
    std::unordered_multimap<u32, StringTableNode> &stringTable)
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
                else if ( node.stringNum < currLowest ) // If matches have previously been found
                    currLowest = node.stringNum; // Replace if stringNum < previous stringNum
            }
        }
        return success;
    }
    return false; // No matches
}
