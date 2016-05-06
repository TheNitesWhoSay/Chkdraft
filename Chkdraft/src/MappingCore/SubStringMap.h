#ifndef SUBSTRINGMAP_H
#define SUBSTRINGMAP_H
#include "StringTableNode.h"
#include <vector>
#include <memory>

class SubString;
using SubStringPtr = std::shared_ptr<SubString>;

class SubString
{
    public:
        static std::vector<SubStringPtr> GetSubStrings(const std::vector<StringTableNode> &strList);
        const StringTableNode* GetString();
        const SubStringPtr GetParent();
        void* userData; // Is whatever the client code makes it

    private:
        const StringTableNode* str;
        SubStringPtr parent;
        std::vector<SubStringPtr> children;

        SubString(const StringTableNode* str);
        static SubStringPtr Make(const StringTableNode* str);
        void MinimizeDependencies();
};

#endif