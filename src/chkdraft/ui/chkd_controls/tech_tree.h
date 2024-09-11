#ifndef TECHTREE_H
#define TECHTREE_H
#include <windows/windows_ui.h>
#include <common_files/common_files.h>

class TechTree : public WinLib::TreeViewControl
{
    public:
        virtual ~TechTree();
        bool CreateThis(HWND hParent, int x, int y, int width, int height, bool hasButtons, u32 id);

    private:
        void InsertTechs();
};

#endif