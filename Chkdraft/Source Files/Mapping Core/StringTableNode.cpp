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
