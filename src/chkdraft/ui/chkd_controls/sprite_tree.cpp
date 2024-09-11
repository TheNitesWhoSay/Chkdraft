#include "sprite_tree.h"
#include "chkdraft/chkdraft.h"

SpriteTree::~SpriteTree()
{

}

bool SpriteTree::AddThis(HWND hTree, HTREEITEM hParent)
{
    hSpriteRoot = hParent;
    if ( TreeViewControl::SetHandle(hTree) )
        return true;
    else
        return false;
}

void SpriteTree::addToTree(HTREEITEM parent, const Sc::Sprite::SpriteGroup & spriteGroup)
{
    auto groupRoot = InsertTreeItem(parent, spriteGroup.groupName, TreeTypeCategory | (LPARAM)Layer::Sprites);
    for ( const auto & subGroup : spriteGroup.subGroups )
        addToTree(groupRoot, subGroup);
    
    for ( auto memberSprite : spriteGroup.memberSprites )
    {
        const auto & spriteDat = chkd.scData.sprites.getSprite(memberSprite.spriteIndex);
        const auto & imageDat = chkd.scData.sprites.getImage(spriteDat.imageFile);
        const auto & imageFileStr = chkd.scData.sprites.imagesTbl->getString(imageDat.grpFile);
        std::string imageFileName = "[" + std::to_string(memberSprite.spriteIndex) + "] " + memberSprite.spriteName;
        InsertTreeItem(groupRoot, imageFileName, memberSprite.spriteIndex|TreeTypeSprite);
    }
}

void SpriteTree::UpdateSpriteTree()
{
    EmptySubTree(hSpriteRoot);

    for ( auto & spriteGroup : chkd.scData.sprites.spriteGroups )
        addToTree(hSpriteRoot, spriteGroup);
}
