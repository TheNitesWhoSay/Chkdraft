#ifndef UNITTREE_H
#define UNITTREE_H
#include <windows/windows_ui.h>
#include <common_files/common_files.h>
#include <mapping_core/mapping_core.h>

struct TreeGroup;

class UnitTree : public WinLib::TreeViewControl
{
    public:
        virtual ~UnitTree();
        bool AddThis(HWND hTree, HTREEITEM hRoot);
        bool CreateThis(HWND hParent, int x, int y, int width, int height, bool hasButtons, u32 id);

        void UpdateUnitTree();
        void UpdateUnitNames(const std::vector<std::string> & displayNames);
        void UpdateUnitName(int unitId);
        
    private:
        void InsertAllUnits(HTREEITEM hParent);
        HTREEITEM hUnitRoot = NULL;
        std::vector<HTREEITEM> unitItems {};
        std::vector<std::string> unitDisplayNames {};
        void insertUnits(const Sc::Unit::UnitGroup & unitGroup, HTREEITEM hParent);
        void insertUnits(const TreeGroup & unitGroup, HTREEITEM hParent);
        void AddUnitItems(HTREEITEM hParent, const std::vector<u16> & unitIds);
        void AddUnitItems(HTREEITEM hParent, const std::vector<u32> & unitIds);
};

#endif