#ifndef MAINTREE_H
#define MAINTREE_H
#include "../../../WindowsLib/WindowsUi.h"
#include "../../../MappingCoreLib/MappingCore.h"
#include "LocationTree.h"
#include "UnitTree.h"
#include "IsomTree.h"
#include "DoodadTree.h"

class MainTree : public WinLib::TreeViewControl
{
    public:
        virtual ~MainTree();
        IsomTree isomTree;
        DoodadTree doodadTree;
        UnitTree unitTree;
        LocationTree locTree;

        void BuildMainTree();


    private:

};

#endif