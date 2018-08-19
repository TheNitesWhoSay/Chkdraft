#ifndef MAINTREE_H
#define MAINTREE_H
#include "../../../WindowsLib/WindowsUi.h"
#include "../../../MappingCoreLib/MappingCore.h"
#include "LocationTree.h"
#include "UnitTree.h"

class MainTree : public WinLib::TreeViewControl
{
    public:
        virtual ~MainTree();
        UnitTree unitTree;
        LocationTree locTree;

        void BuildMainTree();


    private:

};

#endif