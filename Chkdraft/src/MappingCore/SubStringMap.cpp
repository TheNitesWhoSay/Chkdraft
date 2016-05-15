#include "SubStringMap.h"

std::vector<SubStringPtr> SubString::GetSubStrings(const std::vector<StringTableNode> &strList)
{
    std::vector<SubStringPtr> subStrings;
    for ( auto &str : strList )
        subStrings.push_back(SubString::Make(&str));

    for ( auto &subString : subStrings )
    {
        for ( auto &potentialParent : subStrings )
        {
            const RawString &childString = subString->GetString()->string;
            const RawString &parentString = potentialParent->GetString()->string;

            size_t childLength = childString.length();
            size_t parentLength = parentString.length();
            size_t childStartPoint = parentLength - childLength;

            if ( parentLength > childLength &&
                parentString.compare(childStartPoint, childLength, childString) == 0 )
            {
                subString->parent = potentialParent;
                potentialParent->children.push_back(subString);
            }
        }
    }

    for ( auto &subString : subStrings )
        subString->MinimizeDependencies();
    
    return subStrings;
}

const StringTableNode* SubString::GetString()
{
    return str;
}

const SubStringPtr SubString::GetParent()
{
    return parent;
}

SubString::SubString(const StringTableNode* str) : str(str), parent(nullptr)
{

}

SubStringPtr SubString::Make(const StringTableNode* str)
{
    return SubStringPtr(new SubString(str));
}

void SubString::MinimizeDependencies()
{
    if ( parent != nullptr && children.size() > 0 )
    {
        for ( auto &child : children )
        {
            child->parent = parent;
            child->MinimizeDependencies();
        }
    }
    children.clear();
}
