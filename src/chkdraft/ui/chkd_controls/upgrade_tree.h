#ifndef UPGRADETREE_H
#define UPGRADETREE_H
#include <windows/windows_ui.h>
#include <common_files/common_files.h>

class UpgradeTree : public WinLib::TreeViewControl
{
    public:
        virtual ~UpgradeTree();
        bool CreateThis(HWND hParent, int x, int y, int width, int height, bool hasButtons, u32 id);

    private:
        void InsertUpgrades();

};

#endif