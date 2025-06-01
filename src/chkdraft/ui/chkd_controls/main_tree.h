#ifndef MAINTREE_H
#define MAINTREE_H
#include <windows/windows_ui.h>
#include <mapping_core/mapping_core.h>
#include "history_tree.h"
#include "location_tree.h"
#include "unit_tree.h"
#include "isom_tree.h"
#include "doodad_tree.h"
#include "sprite_tree.h"

class MainTree : public WinLib::TreeViewControl
{
    public:
        virtual ~MainTree();
        IsomTree isomTree;
        DoodadTree doodadTree;
        UnitTree unitTree;
        LocationTree locTree;
        SpriteTree spriteTree;
        HistoryTree historyTree;

        void BuildMainTree();


    private:

};

#endif