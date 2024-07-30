#include "preserved_sprite_stats.h"
#include "chkdraft/chkdraft.h"
#include "mapping/undos/chkd_undos/sprite_change.h"

PreservedSpriteStats::PreservedSpriteStats() : field(Chk::Sprite::Field::Type)
{

}

PreservedSpriteStats::~PreservedSpriteStats()
{

}

void PreservedSpriteStats::Clear()
{
    field = Chk::Sprite::Field::Type;
    preservedValues.clear();
}

void PreservedSpriteStats::AddStats(Selections & sel, Chk::Sprite::Field statField)
{
    Clear();
    field = statField;
    auto & spriteIndexes = sel.getSprites();
    for ( size_t & spriteIndex : spriteIndexes )
    {
        const Chk::Sprite & sprite = CM->getSprite(spriteIndex);
        switch ( field )
        {
            case Chk::Sprite::Field::Type: preservedValues.push_back(sprite.type); break;
            case Chk::Sprite::Field::Xc: preservedValues.push_back(sprite.xc); break;
            case Chk::Sprite::Field::Yc: preservedValues.push_back(sprite.yc); break;
            case Chk::Sprite::Field::Owner: preservedValues.push_back(sprite.owner); break;
            case Chk::Sprite::Field::Unused: preservedValues.push_back(sprite.unused); break;
            case Chk::Sprite::Field::Flags: preservedValues.push_back(sprite.flags); break;
        }
    }
}

void PreservedSpriteStats::convertToUndo()
{
    if ( preservedValues.size() > 0 )
    {
        // For each selected sprite, add the corresponding undo from values
        u32 i = 0;

        Selections & selections = CM->GetSelections();
        auto spriteChanges = ReversibleActions::Make();
        auto & selectedSpriteIndexes = selections.getSprites();
        for ( size_t spriteIndex : selectedSpriteIndexes )
        {
            Chk::Sprite oldSprite = CM->getSprite(spriteIndex);
            switch ( field ) {
                case Chk::Sprite::Field::Type: oldSprite.type = Sc::Sprite::Type(preservedValues[i]); break;
                case Chk::Sprite::Field::Xc: oldSprite.xc = preservedValues[i]; break;
                case Chk::Sprite::Field::Yc: oldSprite.yc = preservedValues[i]; break;
                case Chk::Sprite::Field::Owner: oldSprite.owner = preservedValues[i]; break;
                case Chk::Sprite::Field::Unused: oldSprite.unused = preservedValues[i]; break;
                case Chk::Sprite::Field::Flags: oldSprite.flags = preservedValues[i]; break;
            }
            spriteChanges->Insert(SpriteChange::Make(spriteIndex, oldSprite));
            i++;
        }
        CM->AddUndo(spriteChanges);
    }
    Clear();
}
