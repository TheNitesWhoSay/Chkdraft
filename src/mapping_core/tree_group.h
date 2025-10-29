#ifndef TREEGROUP_H
#define TREEGROUP_H
#include <rarecpp/json.h>
#include <rarecpp/reflect.h>
#include <cstdint>
#include <string>
#include <vector>

struct TreeGroup
{
    std::string label;
    std::vector<std::int32_t> items;
    std::vector<TreeGroup> subGroups;

    void parse(const Json::OrderedObject & rawGroup);
    void serialize(Json::OrderedObject & rawGroup);

    REFLECT(TreeGroup, label, subGroups, items)
};

#endif