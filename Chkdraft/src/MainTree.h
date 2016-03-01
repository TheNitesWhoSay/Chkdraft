#ifndef MAINTREE_H
#define MAINTREE_H
#include "WindowsUI/WindowsUI.h"
#include "MappingCore/MappingCore.h"
#include "UnitTree.h"
#include "LocationTree.h"

class MainTree : public TreeViewControl
{
    public:

        UnitTree unitTree;
        LocationTree locTree;

        void BuildMainTree();


    private:

};

#endif