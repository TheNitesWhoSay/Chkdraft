#include "tree_group.h"

void parseTreeGroup(std::vector<TreeGroup> & treeGroups, const std::vector<std::pair<std::string, std::shared_ptr<Json::Value>>> & rawTreeGroups)
{
    treeGroups.assign(rawTreeGroups.size(), TreeGroup{});
    std::size_t i = 0;
    for ( const auto & group : rawTreeGroups )
    {
        const std::string & groupName = group.first;
        const std::shared_ptr<Json::Value> & groupMembers = group.second;
        treeGroups[i].label = groupName;
        if ( groupMembers != nullptr )
        {
            switch ( groupMembers->type() )
            {
                case Json::Value::Type::NumberArray:
                {
                    const std::vector<std::string> & numberArray = group.second->numberArray();
                    for ( const auto & number : numberArray )
                        treeGroups[i].items.push_back(std::stoi(number));
                }
                break;
                case Json::Value::Type::OrderedObject:
                    parseTreeGroup(treeGroups[i].subGroups, group.second->orderedObject());
                    break;
                default:
                    break;
            }
        }
        ++i;
    }
}

void TreeGroup::parse(const Json::OrderedObject & rawGroup)
{
    this->subGroups.clear();
    parseTreeGroup(this->subGroups, rawGroup.orderedObject());
}

void serializeSubGroup(std::vector<std::pair<std::string, std::shared_ptr<Json::Value>>> & rawGroup, const std::vector<TreeGroup> & treeGroups)
{
    for ( const TreeGroup & treeGroup : treeGroups )
    {
        bool hasSubGroups = !treeGroup.subGroups.empty();
        bool hasItems = !treeGroup.items.empty();
        if ( hasSubGroups )
        {
            std::shared_ptr<Json::OrderedObject> object = std::make_shared<Json::OrderedObject>();
            serializeSubGroup(object->orderedObject(), treeGroup.subGroups);
            rawGroup.push_back(std::make_pair(treeGroup.label, std::dynamic_pointer_cast<Json::Value>(object)));
        }
        else if ( hasItems )
        {
            std::shared_ptr<Json::NumberArray> numberArray = std::make_shared<Json::NumberArray>();
            for ( const auto item : treeGroup.items )
                numberArray->numberArray().push_back(std::to_string(item));

            rawGroup.push_back(std::make_pair(treeGroup.label, std::dynamic_pointer_cast<Json::Value>(numberArray)));
        }
        else // Has neither (make an empty object)
        {
            std::shared_ptr<Json::OrderedObject> emptyObject = std::make_shared<Json::OrderedObject>();
            rawGroup.push_back(std::make_pair(treeGroup.label, std::dynamic_pointer_cast<Json::Value>(emptyObject)));
        }
    }
}

void TreeGroup::serialize(Json::OrderedObject & rawGroup)
{
    std::vector<std::pair<std::string, std::shared_ptr<Json::Value>>> & obj = rawGroup.orderedObject();
    obj.clear();
    serializeSubGroup(obj, this->subGroups);
}
