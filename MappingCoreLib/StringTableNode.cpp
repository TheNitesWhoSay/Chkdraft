#include "StringTableNode.h"

StringTableNode::StringTableNode() : string(""), stringId(0), isExtended(false), propStruct(0)
{

}

StringTableNode::~StringTableNode()
{

}

bool StringTableNode::operator== (StringTableNode other)
{
    return (stringId == other.stringId) &&
         (isExtended == other.isExtended) &&
         (propStruct == other.propStruct) &&
         (string.compare(other.string) == 0);
}

bool StringTableNode::operator< (StringTableNode other)
{
    return stringId < other.stringId;
}

bool CompareStrTblNode(StringTableNode first, StringTableNode second)
{
    return first.stringId < second.stringId;
}

bool strIsInHashTable(RawString &str, std::hash<std::string> &strHash,
    std::unordered_multimap<size_t, StringTableNode> &stringTable)
{
    size_t hash = strHash(str);
    size_t numMatching = stringTable.count(hash);
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
                    currLowest = node.stringId;
                    success = true;
                }
                else if ( node.stringId < currLowest ) // If matches have previously been found
                    currLowest = node.stringId; // Replace if stringNum < previous stringNum
            }
        }
        return success;
    }
    return false; // No matches
}
