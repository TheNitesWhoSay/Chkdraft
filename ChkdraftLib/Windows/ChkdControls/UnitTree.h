#ifndef UNITTREE_H
#define UNITTREE_H
#include "../../../WindowsLib/WindowsUi.h"
#include "../../CommonFiles/CommonFiles.h"

class UnitTree : public WinLib::TreeViewControl
{
    public:
        virtual ~UnitTree();
        bool AddThis(HWND hTree, HTREEITEM hRoot);
        bool CreateThis(HWND hParent, int x, int y, int width, int height, bool hasButtons, u32 id);

        void UpdateUnitNames(const std::vector<std::string> & displayNames);
        void UpdateUnitName(int unitId);

    protected:
        void InsertAllUnits(HTREEITEM hParent);
        void InsertZergItems(HTREEITEM hParent);
        void InsertTerranItems(HTREEITEM hParent);
        void InsertProtossItems(HTREEITEM hParent);
        void InsertNeutralItems(HTREEITEM hParent);
        void InsertUndefinedItems(HTREEITEM hParent);

    private:
        HTREEITEM UnitItems[228];
        std::vector<std::string> unitDisplayNames;
        void AddUnitItems(HTREEITEM hParent, const std::vector<u16> & unitIds);
};

#endif