#ifndef SPRITETREE_H
#define SPRITETREE_H
#include <mapping_core/sc.h>
#include <windows/windows_ui.h>

struct TreeGroup;

class SpriteTree : public WinLib::TreeViewControl
{
    public:
        virtual ~SpriteTree();
        bool AddThis(HWND hTree, HTREEITEM hParent);
        void UpdateSpriteTree();

    private:
        void addToTree(HTREEITEM parent, const Sc::Sprite::SpriteGroup & spriteGroup);
        void addToTree(const TreeGroup & spriteGroup, HTREEITEM hParent);

        HTREEITEM hSpriteRoot = NULL;
};

#endif