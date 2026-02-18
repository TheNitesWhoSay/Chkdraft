#include "scenario.h"
#include "sha256.h"
#include "math.h"
#include "cross_cut/logger.h"
#include <algorithm>
#include <cstdio>
#include <exception>
#include <functional>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <chrono>

extern Logger logger;

void Scenario::EditCondition::toggleDisabled()
{
    if ( (read.flags & Chk::Condition::Flags::Disabled) == Chk::Condition::Flags::Disabled )
        edit.flags = read.flags & Chk::Condition::Flags::xDisabled;
    else
        edit.flags = read.flags | Chk::Condition::Flags::Disabled;
}

void Scenario::EditCondition::remapLocationIds(const std::map<u32, u32> & locationIdRemappings)
{
    if ( read.conditionType < Chk::Condition::NumConditionTypes )
    {
        if ( read.conditionUsesLocationArg[read.conditionType] )
        {
            auto found = locationIdRemappings.find(read.locationId);
            if ( found != locationIdRemappings.end() )
                edit.locationId = found->second;
        }
    }
}

void Scenario::EditCondition::deleteLocation(size_t locationId)
{
    if ( read.conditionType < Chk::Condition::NumConditionTypes && Chk::Condition::conditionUsesLocationArg[read.conditionType] && read.locationId == locationId )
        edit.locationId = Chk::LocationId::NoLocation;
}

void Scenario::EditAction::toggleDisabled()
{
    if ( (read.flags & Chk::Action::Flags::Disabled) == Chk::Action::Flags::Disabled )
        edit.flags = read.flags & Chk::Action::Flags::xDisabled;
    else
        edit.flags = read.flags | Chk::Action::Flags::Disabled;
}

void Scenario::EditAction::remapLocationIds(const std::map<u32, u32> & locationIdRemappings)
{
    if ( read.actionType < Chk::Action::NumActionTypes )
    {
        if ( Chk::Action::actionUsesLocationArg[read.actionType] )
        {
            auto found = locationIdRemappings.find(read.locationId);
            if ( found != locationIdRemappings.end() )
                edit.locationId = found->second;
        }

        if ( Chk::Action::actionUsesSecondaryLocationArg[read.actionType] )
        {
            auto found = locationIdRemappings.find(read.locationId);
            if ( found != locationIdRemappings.end() )
                edit.locationId = found->second;
        }
    }
}

void Scenario::EditAction::remapStringIds(const std::map<u32, u32> & stringIdRemappings)
{
    if ( read.actionType < Chk::Action::NumActionTypes )
    {
        if ( Chk::Action::actionUsesStringArg[read.actionType] )
        {
            auto found = stringIdRemappings.find(read.stringId);
            if ( found != stringIdRemappings.end() )
                edit.stringId = found->second;
        }

        if ( Chk::Action::actionUsesSoundArg[read.actionType] )
        {
            auto found = stringIdRemappings.find(read.soundStringId);
            if ( found != stringIdRemappings.end() )
                edit.soundStringId = found->second;
        }
    }
}

void Scenario::EditAction::deleteLocation(size_t locationId)
{
    if ( read.actionType < Chk::Action::NumActionTypes )
    {
        if ( read.actionUsesLocationArg[read.actionType] && read.locationId == locationId )
            edit.locationId = Chk::LocationId::NoLocation;

        if ( read.actionUsesSecondaryLocationArg[read.actionType] && read.number == locationId )
            edit.number = Chk::LocationId::NoLocation;
    }
}

void Scenario::EditAction::deleteString(size_t stringId)
{
    if ( read.actionType < Chk::Action::NumActionTypes )
    {
        if ( Chk::Action::actionUsesStringArg[read.actionType] && read.stringId == stringId )
            edit.stringId = 0;

        if ( Chk::Action::actionUsesSoundArg[read.actionType] && read.soundStringId == stringId )
            edit.soundStringId = 0;
    }
}

void Scenario::EditAction::deleteSoundReferences(size_t stringId)
{
    if ( read.actionType < Chk::Action::NumActionTypes )
    {
        if ( Chk::Action::actionUsesSoundArg[read.actionType] && read.soundStringId == stringId )
            edit.soundStringId = 0;
    }
}

Scenario::EditCondition Scenario::EditTrigger::editCondition(std::size_t conditionIndex)
{
    return EditCondition(this, view.conditions[conditionIndex]);
}

Scenario::EditAction Scenario::EditTrigger::editAction(std::size_t actionIndex)
{
    return EditAction(this, view.actions[actionIndex]);
}

void Scenario::EditTrigger::deleteAction(size_t actionIndex, bool alignTop)
{
    if ( actionIndex < Chk::Trigger::MaxActions )
    {
        edit.actions[actionIndex] = Chk::Action {};

        if ( alignTop )
            alignActionsTop();
    }
}

void Scenario::EditTrigger::deleteCondition(size_t conditionIndex, bool alignTop)
{
    if ( conditionIndex < Chk::Trigger::MaxConditions )
    {
        edit.conditions[conditionIndex] = Chk::Condition {};
    
        if ( alignTop )
            alignConditionsTop();
    }
}

void Scenario::EditTrigger::setPreserveTriggerFlagged(bool preserved)
{
    if ( preserved )
        edit.flags = read.flags | Chk::Trigger::Flags::PreserveTrigger;
    else
        edit.flags = read.flags & (~Chk::Trigger::Flags::PreserveTrigger);
}

void Scenario::EditTrigger::setDisabled(bool disabled)
{
    if ( disabled )
        edit.flags = read.flags | Chk::Trigger::Flags::Disabled;
    else
        edit.flags = read.flags & (~Chk::Trigger::Flags::Disabled);
}

void Scenario::EditTrigger::setIgnoreConditionsOnce(bool ignoreConditionsOnce)
{
    if ( ignoreConditionsOnce )
        edit.flags = read.flags | Chk::Trigger::Flags::IgnoreConditionsOnce;
    else
        edit.flags = read.flags & (~Chk::Trigger::Flags::IgnoreConditionsOnce);
}

void Scenario::EditTrigger::setIgnoreWaitSkipOnce(bool ignoreWaitSkipOnce)
{
    if ( ignoreWaitSkipOnce )
        edit.flags = read.flags | Chk::Trigger::Flags::IgnoreWaitSkipOnce;
    else
        edit.flags = read.flags & (~Chk::Trigger::Flags::IgnoreWaitSkipOnce);
}

void Scenario::EditTrigger::setIgnoreMiscActionsOnce(bool ignoreMiscActionsOnce)
{
    if ( ignoreMiscActionsOnce )
        edit.flags = read.flags | Chk::Trigger::Flags::IgnoreMiscActions;
    else
        edit.flags = read.flags & (~Chk::Trigger::Flags::IgnoreMiscActions);
}

void Scenario::EditTrigger::setIgnoreDefeatDraw(bool ignoreDefeatDraw)
{
    if ( ignoreDefeatDraw )
        edit.flags = read.flags | Chk::Trigger::Flags::IgnoreDefeatDraw;
    else
        edit.flags = read.flags & (~Chk::Trigger::Flags::IgnoreDefeatDraw);
}

void Scenario::EditTrigger::setPauseFlagged(bool pauseFlagged)
{
    if ( pauseFlagged )
        edit.flags = read.flags | Chk::Trigger::Flags::Paused;
    else
        edit.flags = read.flags & (~Chk::Trigger::Flags::Paused);
}

void Scenario::EditTrigger::setExtendedDataIndex(size_t extendedDataIndex)
{
    u32 sizedExtendedDataIndex = (u32)extendedDataIndex;
    if ( sizedExtendedDataIndex == 0 || sizedExtendedDataIndex == 1 || sizedExtendedDataIndex == 256 ||
        sizedExtendedDataIndex == 257 || sizedExtendedDataIndex == 65536 || sizedExtendedDataIndex == 65537 ||
        sizedExtendedDataIndex == 65792 || sizedExtendedDataIndex == 65793 || sizedExtendedDataIndex > 0x00FFFFFF ||
        sizedExtendedDataIndex > Chk::ExtendedTrigDataIndex::MaxIndex )
    {
        throw std::invalid_argument("Extended trigger data index " + std::to_string(sizedExtendedDataIndex) + " is invalid!");
    }
    else
        edit.owners[22] = static_cast<Chk::Trigger::Owned>((((u32 &)read.owners[22]) & 0x1010101) | (u32)extendedDataIndex);
}

void Scenario::EditTrigger::clearExtendedDataIndex()
{
    edit.owners[22] = static_cast<Chk::Trigger::Owned>(0);
}

void Scenario::EditTrigger::remapLocationIds(const std::map<u32, u32>& locationIdRemappings)
{
    for ( size_t i=0; i<Chk::Trigger::MaxConditions; i++ )
        editCondition(i).remapLocationIds(locationIdRemappings);

    for ( size_t i=0; i<Chk::Trigger::MaxActions; i++ )
        editAction(i).remapLocationIds(locationIdRemappings);
}

void Scenario::EditTrigger::remapStringIds(const std::map<u32, u32>& stringIdRemappings)
{
    for ( size_t i=0; i<Chk::Trigger::MaxActions; i++ )
        editAction(i).remapStringIds(stringIdRemappings);
}

void Scenario::EditTrigger::deleteLocation(size_t locationId)
{
    for ( size_t i=0; i<Chk::Trigger::MaxConditions; i++ )
        editCondition(i).deleteLocation(locationId);

    for ( size_t i=0; i<Chk::Trigger::MaxActions; i++ )
        editAction(i).deleteLocation(locationId);
}

void Scenario::EditTrigger::deleteString(size_t stringId)
{
    for ( size_t i=0; i<Chk::Trigger::MaxActions; i++ )
        editAction(i).deleteString(stringId);
}

void Scenario::EditTrigger::deleteSoundReferences(size_t stringId)
{
    for ( size_t i=0; i<Chk::Trigger::MaxActions; i++ )
        editAction(i).deleteSoundReferences(stringId);
}

void Scenario::EditTrigger::alignConditionsTop()
{
    for ( u8 i=0; i<Chk::Trigger::MaxConditions; i++ )
    {
        if ( read.conditions[i].conditionType == Chk::Condition::Type::NoCondition )
        {
            u8 firstUsedAfter = u8(-1);
            for ( u8 j=i+1; j<Chk::Trigger::MaxConditions; j++ )
            {
                if ( read.conditions[j].conditionType != Chk::Condition::Type::NoCondition )
                {
                    firstUsedAfter = j;
                    break;
                }
            }

            if ( firstUsedAfter != u8(-1) )
            {
                for ( u8 j=i; j<Chk::Trigger::MaxConditions; j++ )
                    edit.conditions[j] = read.conditions[firstUsedAfter+(j-i)];
            }
            else
                break;
        }
    }
}

void Scenario::EditTrigger::alignActionsTop()
{
    for ( u8 i=0; i<Chk::Trigger::MaxActions; i++ )
    {
        if ( read.actions[i].actionType == Chk::Action::Type::NoAction )
        {
            u8 firstUsedAfter = u8(-1);
            for ( u8 j=i+1; j<Chk::Trigger::MaxActions; j++ )
            {
                if ( read.actions[j].actionType != Chk::Action::Type::NoAction )
                {
                    firstUsedAfter = j;
                    break;
                }
            }

            if ( firstUsedAfter != u8(-1) )
            {
                for ( u8 j=i; j<Chk::Trigger::MaxActions; j++ )
                    edit.actions[j] = read.actions[firstUsedAfter+(j-i)];
            }
            else
                break;
        }
    }
}

void Scenario::EditBriefingAction::toggleDisabled()
{
    if ( (read.flags & Chk::Action::Flags::Disabled) == Chk::Action::Flags::Disabled )
        edit.flags = read.flags & Chk::Action::Flags::xDisabled;
    else
        edit.flags = read.flags | Chk::Action::Flags::Disabled;
}

void Scenario::EditBriefingAction::remapBriefingStringIds(const std::map<u32, u32> & stringIdRemappings)
{
    if ( read.actionType < Chk::Action::NumBriefingActionTypes )
    {
        if ( Chk::Action::briefingActionUsesStringArg[read.actionType] )
        {
            auto found = stringIdRemappings.find(read.stringId);
            if ( found != stringIdRemappings.end() )
                edit.stringId = found->second;
        }

        if ( Chk::Action::briefingActionUsesSoundArg[read.actionType] )
        {
            auto found = stringIdRemappings.find(read.soundStringId);
            if ( found != stringIdRemappings.end() )
                edit.soundStringId = found->second;
        }
    }
}

void Scenario::EditBriefingAction::deleteBriefingString(size_t stringId)
{
    if ( read.actionType < Chk::Action::NumBriefingActionTypes )
    {
        if ( Chk::Action::briefingActionUsesStringArg[read.actionType] && read.stringId == stringId )
            edit.stringId = 0;

        if ( Chk::Action::briefingActionUsesSoundArg[read.actionType] && read.soundStringId == stringId )
            edit.soundStringId = 0;
    }
}

void Scenario::EditBriefingAction::deleteBriefingSoundReferences(size_t stringId)
{
    if ( read.actionType < Chk::Action::NumBriefingActionTypes )
    {
        if ( Chk::Action::briefingActionUsesSoundArg[read.actionType] && read.soundStringId == stringId )
            edit.soundStringId = 0;
    }
}

Scenario::EditBriefingAction Scenario::EditBriefingTrigger::editBriefingAction(std::size_t actionIndex)
{
    return EditBriefingAction(this, view.actions[actionIndex]);
}


void Scenario::EditBriefingTrigger::deleteAction(size_t actionIndex, bool alignTop)
{
    if ( actionIndex < Chk::Trigger::MaxActions )
    {
        edit.actions[actionIndex] = Chk::Action {};

        if ( alignTop )
            alignActionsTop();
    }
}

void Scenario::EditBriefingTrigger::remapBriefingStringIds(const std::map<u32, u32>& stringIdRemappings)
{
    for ( size_t i=0; i<Chk::Trigger::MaxActions; i++ )
        editBriefingAction(i).remapBriefingStringIds(stringIdRemappings);
}

void Scenario::EditBriefingTrigger::deleteBriefingString(size_t stringId)
{
    for ( size_t i=0; i<Chk::Trigger::MaxActions; i++ )
        editBriefingAction(i).deleteBriefingString(stringId);
}

void Scenario::EditBriefingTrigger::deleteBriefingSoundReferences(size_t stringId)
{
    for ( size_t i=0; i<Chk::Trigger::MaxActions; i++ )
        editBriefingAction(i).deleteBriefingSoundReferences(stringId);
}

void Scenario::EditBriefingTrigger::alignActionsTop()
{
    for ( u8 i=0; i<Chk::Trigger::MaxActions; i++ )
    {
        if ( read.actions[i].actionType == Chk::Action::Type::NoAction )
        {
            u8 firstUsedAfter = u8(-1);
            for ( u8 j=i+1; j<Chk::Trigger::MaxActions; j++ )
            {
                if ( read.actions[j].actionType != Chk::Action::Type::NoAction )
                {
                    firstUsedAfter = j;
                    break;
                }
            }

            if ( firstUsedAfter != u8(-1) )
            {
                for ( u8 j=i; j<Chk::Trigger::MaxActions; j++ )
                    edit.actions[j] = read.actions[firstUsedAfter+(j-i)];
            }
            else
                break;
        }
    }
}

using Chk::StrCompressFlag;
using Member = RareTs::IndexOf<MapData>;

std::unordered_map<Chk::SectionName, size_t> sectionMemberIndex {
    {SectionName::TYPE, Member::mapType}, {SectionName::VER, Member::version}, {SectionName::IVER, Member::iVersion}, {SectionName::IVE2, Member::i2Version},
    {SectionName::VCOD, Member::validation}, {SectionName::IOWN, Member::iownSlotTypes}, {SectionName::OWNR, Member::slotTypes}, {SectionName::ERA, Member::tileset},
    {SectionName::DIM, Member::dimensions}, {SectionName::SIDE, Member::playerRaces}, {SectionName::MTXM, Member::tiles}, {SectionName::PUNI, Member::unitAvailability},
    {SectionName::UPGR, Member::origUpgradeLeveling}, {SectionName::PTEC, Member::origTechnologyAvailability}, {SectionName::UNIT, Member::units}, {SectionName::ISOM, Member::isomRects},
    {SectionName::TILE, Member::editorTiles}, {SectionName::DD2, Member::doodads}, {SectionName::THG2, Member::sprites}, {SectionName::MASK, Member::tileFog},
    {SectionName::STR, Member::strings}, {SectionName::UPRP, Member::createUnitProperties}, {SectionName::UPUS, Member::createUnitPropertiesUsed}, {SectionName::MRGN, Member::locations},
    {SectionName::TRIG, Member::triggers}, {SectionName::MBRF, Member::briefingTriggers}, {SectionName::FORC, Member::forces}, {SectionName::SPRP, Member::scenarioProperties},
    {SectionName::WAV, Member::soundPaths}, {SectionName::UNIS, Member::origUnitSettings}, {SectionName::UPGS, Member::origUpgradeCosts}, {SectionName::TECS, Member::origTechnologyCosts},
    {SectionName::SWNM, Member::switchNames}, {SectionName::COLR, Member::playerColors}, {SectionName::PUPx, Member::upgradeLeveling}, {SectionName::PTEx, Member::techAvailability},
    {SectionName::UNIx, Member::unitSettings}, {SectionName::UPGx, Member::upgradeCosts}, {SectionName::TECx, Member::techCosts}, {SectionName::STRx, Member::strings},
    {SectionName::CRGB, Member::customColors}, {SectionName::OSTR, Member::editorStringOverrides}, {SectionName::KSTR, Member::editorStrings}, {SectionName::KTRG, Member::triggerExtensions},
    {SectionName::KTGP, Member::triggerGroupings}
};

void updateTileFromIsom(Chk::IsomDiamond isomDiamond, Chk::IsomCache & cache, std::size_t tileWidth, std::size_t tileHeight,
    const std::vector<Chk::IsomRect> & isomRects, const std::vector<u16> & editorTiles)
{
    if ( isomDiamond.x+1 >= cache.isomWidth || isomDiamond.y+1 >= cache.isomHeight )
        return;

    auto getTileValue = [&](auto x, auto y) {
        return editorTiles[y*tileWidth + x];
    };

    size_t leftTileX = 2*isomDiamond.x;
    size_t rightTileX = leftTileX+1;

    size_t totalConnections = cache.tileGroups.size();

    std::size_t isomWidth = cache.isomWidth;
    const auto & isomRect = isomRects[isomDiamond.y*isomWidth+isomDiamond.x];
    uint32_t isomRectHash = isomRect.getHash(cache.isomLinks);
    auto foundPotentialGroups = cache.hashToTileGroup->find(isomRectHash);
    if ( foundPotentialGroups != cache.hashToTileGroup->end() )
    {
        const std::vector<uint16_t> & potentialGroups = foundPotentialGroups->second;
        uint16_t destTileGroup = potentialGroups[0];

        // Lookup the isom group for this row using the above rows stack-bottom connection
        if ( isomDiamond.y > 0 )
        {
            auto aboveTileGroup = Sc::Terrain::getTileGroup(getTileValue(leftTileX, isomDiamond.y-1));
            if ( aboveTileGroup < cache.tileGroups.size() )
            {
                uint16_t tileGroupBottom = cache.tileGroups[aboveTileGroup].stackConnections.bottom;
                for ( size_t i=0; i<potentialGroups.size(); ++i )
                {
                    if ( cache.tileGroups[potentialGroups[i]].stackConnections.top == tileGroupBottom )
                    {
                        destTileGroup = potentialGroups[i];
                        break;
                    }
                }
            }
        }

        uint16_t destSubTile = cache.getRandomSubtile(destTileGroup) % 16;
        cache.setTileValue(leftTileX, isomDiamond.y, 16*destTileGroup + destSubTile);
        cache.setTileValue(rightTileX, isomDiamond.y, 16*(destTileGroup+1) + destSubTile);

        // Find the top row of the tile-group stack (note: this is a tad performance sensitive, consider pre-linking stacks)
        size_t stackTopY = isomDiamond.y;
        auto curr = Sc::Terrain::getTileGroup(getTileValue(leftTileX, stackTopY));
        for ( ; stackTopY > 0 && curr < totalConnections && cache.tileGroups[curr].stackConnections.top != 0; --stackTopY )
        {
            auto above = Sc::Terrain::getTileGroup(getTileValue(leftTileX, stackTopY-1));
            if ( above >= totalConnections || cache.tileGroups[curr].stackConnections.top != cache.tileGroups[above].stackConnections.bottom )
                break;

            curr = above;
        }

        cache.setTileValue(leftTileX, stackTopY, 16*Sc::Terrain::getTileGroup(getTileValue(leftTileX, stackTopY)) + destSubTile);
        cache.setTileValue(rightTileX, stackTopY, 16*Sc::Terrain::getTileGroup(getTileValue(rightTileX, stackTopY)) + destSubTile);

        // Set tile values for the rest of the stack
        for ( size_t y=stackTopY+1; y<tileHeight; ++y )
        {
            auto tileGroup = Sc::Terrain::getTileGroup(getTileValue(leftTileX, y-1));
            auto nextTileGroup = Sc::Terrain::getTileGroup(getTileValue(leftTileX, y));

            if ( tileGroup >= cache.tileGroups.size() || nextTileGroup >= cache.tileGroups.size() ||
                cache.tileGroups[tileGroup].stackConnections.bottom == 0 || cache.tileGroups[nextTileGroup].stackConnections.top == 0 )
            {
                break;
            }

            uint16_t bottomConnection = cache.tileGroups[tileGroup].stackConnections.bottom;
            uint16_t leftTileGroup = Sc::Terrain::getTileGroup(getTileValue(leftTileX, y));
            uint16_t rightTileGroup = Sc::Terrain::getTileGroup(getTileValue(rightTileX, y));
            if ( bottomConnection != cache.tileGroups[nextTileGroup].stackConnections.top )
            {
                const auto & isomRect = isomRects[y*isomWidth+isomDiamond.x];
                isomRectHash = isomRect.getHash(cache.isomLinks);

                auto foundPotentialGroups = cache.hashToTileGroup->find(isomRectHash);
                if ( foundPotentialGroups != cache.hashToTileGroup->end() )
                {
                    const std::vector<uint16_t> & potentialGroups = foundPotentialGroups->second;
                    for ( size_t i=0; i<potentialGroups.size(); ++i )
                    {
                        if ( cache.tileGroups[potentialGroups[i]].stackConnections.top == bottomConnection )
                        {
                            leftTileGroup = potentialGroups[i];
                            rightTileGroup = leftTileGroup + 1;
                            break;
                        }
                    }
                }
            }

            cache.setTileValue(leftTileX, y, 16*leftTileGroup + destSubTile);
            cache.setTileValue(rightTileX, y, 16*rightTileGroup + destSubTile);
        }
    }
    else
    {
        cache.setTileValue(leftTileX, isomDiamond.y, 0);
        cache.setTileValue(rightTileX, isomDiamond.y, 0);
    }
}

struct IsomInitializerCache : Chk::IsomCache
{
    IsomInitializerCache(
        Sc::Terrain::Tileset tileset, size_t tileWidth, size_t tileHeight, const Sc::Terrain::Tiles & tilesetData, MapData & mapData)
        : Chk::IsomCache(tileset, tileWidth, tileHeight, tilesetData), mapData(mapData) {}

    MapData & mapData;

    inline void setTileValue(size_t tileX, size_t tileY, uint16_t tileValue) override
    {
        mapData.editorTiles[tileY*mapData.dimensions.tileWidth + tileX] = tileValue;
        mapData.tiles[tileY*mapData.dimensions.tileWidth + tileX] = tileValue;
    }
};

Scenario::Scenario() : tracked{this} {}

Scenario::Scenario(Sc::Terrain::Tileset tileset, u16 width, u16 height, size_t terrainTypeIndex, Chk::DefaultTriggers defaultTriggers, SaveType saveType,
    const Sc::Terrain::Tiles* tilesetData) : tracked{this}
{
    auto mapData = std::make_unique<::MapData>();
    mapData->saveType = saveType;

    mapData->dimensions = Chk::DIM{.tileWidth = width, .tileHeight = height};
    mapData->tileset = tileset;
    mapData->tiles.assign(size_t(width)*size_t(height), u16(0));
    mapData->editorTiles.assign(size_t(width)*size_t(height), u16(0));
    mapData->tileFog.assign(size_t(width)*size_t(height), u8(0));

    if ( tilesetData != nullptr )
    {
        std::size_t tileWidth = size_t(width);
        std::size_t tileHeight = size_t(height);
        std::size_t isomWidth = tileWidth/2 + 1;
        std::size_t isomHeight = tileHeight + 1;
        IsomInitializerCache cache(tileset, width, height, *tilesetData, *mapData);
        uint16_t val = ((cache.getTerrainTypeIsomValue(terrainTypeIndex) << 4) | Chk::IsomRect::EditorFlag::Modified);
        mapData->isomRects.assign(isomWidth*isomHeight, Chk::IsomRect{val, val, val, val});
        cache.setAllChanged();
        
        for ( size_t y=cache.changedArea.top; y<=cache.changedArea.bottom; ++y )
        {
            for ( size_t x=cache.changedArea.left; x<=cache.changedArea.right; ++x )
            {
                Chk::IsomRect isomRect = mapData->isomRects[y*isomWidth+x];
                if ( isomRect.isLeftOrRightModified() )
                    updateTileFromIsom({x, y}, cache, tileWidth, tileHeight, mapData->isomRects, mapData->editorTiles);

                isomRect.clearEditorFlags();
                mapData->isomRects[y*isomWidth+x] = isomRect;
            }
        }
    }
    else
        mapData->isomRects.assign((size_t(width) / size_t(2) + size_t(1)) * (size_t(height) + size_t(1)), Chk::IsomRect{});

    mapData->strings.reserve(1024);
    mapData->strings.push_back(std::nullopt); // 0 (always unused)
    mapData->strings.push_back("Untitled Scenario"); // 1
    mapData->strings.push_back("Destroy all enemy buildings."); // 2
    mapData->strings.push_back("Anywhere"); // 3
    mapData->strings.push_back("Force 1"); // 4
    mapData->strings.push_back("Force 2"); // 5
    mapData->strings.push_back("Force 3"); // 6
    mapData->strings.push_back("Force 4"); // 7
    for ( size_t i=mapData->strings.size(); i<=1024; ++i )
        mapData->strings.push_back(std::nullopt);

     // Note: Location index 0 is unused
    mapData->locations.assign(this->isHybridOrAbove() ? 1+Chk::TotalLocations : 1+Chk::TotalOriginalLocations, Chk::Location{});
    mapData->locations[Chk::LocationId::Anywhere] = Chk::Location{.right = (u32)width*32, .bottom = (u32)height*32, .stringId = 3};

    if ( this->isHybridOrAbove() )
        mapData->saveSections.push_back(::MapData::Section{Chk::SectionName::TYPE});
    
    mapData->saveSections.push_back(::MapData::Section{Chk::SectionName::VER});

    if ( !this->isHybridOrAbove() )
        mapData->saveSections.push_back(::MapData::Section{Chk::SectionName::IVER});

    mapData->saveSections.insert(mapData->saveSections.end(), {
        {Chk::SectionName::IVE2}, {Chk::SectionName::VCOD}, {Chk::SectionName::IOWN}, {Chk::SectionName::OWNR},
        {Chk::SectionName::ERA }, {Chk::SectionName::DIM }, {Chk::SectionName::SIDE}, {Chk::SectionName::MTXM},
        {Chk::SectionName::PUNI}, {Chk::SectionName::UPGR}, {Chk::SectionName::PTEC}, {Chk::SectionName::UNIT},
        {Chk::SectionName::ISOM}, {Chk::SectionName::TILE}, {Chk::SectionName::DD2 }, {Chk::SectionName::THG2},
        {Chk::SectionName::MASK}, {Chk::SectionName::STR }, {Chk::SectionName::UPRP}, {Chk::SectionName::UPUS},
        {Chk::SectionName::MRGN}, {Chk::SectionName::TRIG}, {Chk::SectionName::MBRF}, {Chk::SectionName::SPRP},
        {Chk::SectionName::FORC}, {Chk::SectionName::WAV }, {Chk::SectionName::UNIS}, {Chk::SectionName::UPGS},
        {Chk::SectionName::TECS}, {Chk::SectionName::SWNM}, {Chk::SectionName::COLR}, {Chk::SectionName::PUPx},
        {Chk::SectionName::PTEx}, {Chk::SectionName::UNIx}, {Chk::SectionName::UPGx}, {Chk::SectionName::TECx}
    });

    auto addSetRes = [&](Sc::Player::Id player) {
        Chk::Trigger setRes {};
        setRes.owners[player] = Chk::Trigger::Owned::Yes;
        setRes.conditions[0].conditionType = Chk::Condition::Type::Always;
        setRes.actions[0].actionType = Chk::Action::Type::SetResources;
        setRes.actions[0].group = Sc::Player::Id::CurrentPlayer;
        setRes.actions[0].type2 = Chk::Trigger::ValueModifier::SetTo;
        setRes.actions[0].number = 50;
        setRes.actions[0].type = Chk::Trigger::ResourceType::Ore;
        mapData->triggers.push_back(setRes);
    };
    auto addDefeat = [&](Sc::Player::Id player) {
        Chk::Trigger defeat {};
        defeat.owners[player] = Chk::Trigger::Owned::Yes;
        defeat.conditions[0].conditionType = Chk::Condition::Type::Command;
        defeat.conditions[0].player = Sc::Player::Id::CurrentPlayer;
        defeat.conditions[0].comparison = Chk::Condition::Comparison::AtMost;
        defeat.conditions[0].amount = 0;
        defeat.conditions[0].unitType = Sc::Unit::Type::Buildings;
        defeat.actions[0].actionType = Chk::Action::Type::Defeat;
        mapData->triggers.push_back(defeat);
    };
    auto addVictory = [&](Sc::Player::Id player) {
        Chk::Trigger victory {};
        victory.owners[player] = Chk::Trigger::Owned::Yes;
        victory.conditions[0].conditionType = Chk::Condition::Type::Command;
        victory.conditions[0].player = Sc::Player::Id::NonAlliedVictoryPlayers;
        victory.conditions[0].comparison = Chk::Condition::Comparison::AtMost;
        victory.conditions[0].amount = 0;
        victory.conditions[0].unitType = Sc::Unit::Type::Buildings;
        victory.actions[0].actionType = Chk::Action::Type::Victory;
        mapData->triggers.push_back(victory);
    };
    auto addSharedVision = [&](Sc::Player::Id player) {
        Chk::Trigger addSharedVision {};
        addSharedVision.owners[player] = Chk::Trigger::Owned::Yes;
        addSharedVision.conditions[0].conditionType = Chk::Condition::Type::Always;
        addSharedVision.actions[0].actionType = Chk::Action::Type::RunAiScript;
        addSharedVision.actions[1].actionType = Chk::Action::Type::RunAiScript;
        addSharedVision.actions[2].actionType = Chk::Action::Type::RunAiScript;
        addSharedVision.actions[3].actionType = Chk::Action::Type::RunAiScript;
        addSharedVision.actions[4].actionType = Chk::Action::Type::RunAiScript;
        addSharedVision.actions[5].actionType = Chk::Action::Type::RunAiScript;
        addSharedVision.actions[6].actionType = Chk::Action::Type::RunAiScript;
        addSharedVision.actions[7].actionType = Chk::Action::Type::RunAiScript;
        addSharedVision.actions[0].number = u32(Sc::Ai::Script::TurnONSharedVisionforPlayer1);
        addSharedVision.actions[1].number = u32(Sc::Ai::Script::TurnONSharedVisionforPlayer2);
        addSharedVision.actions[2].number = u32(Sc::Ai::Script::TurnONSharedVisionforPlayer3);
        addSharedVision.actions[3].number = u32(Sc::Ai::Script::TurnONSharedVisionforPlayer4);
        addSharedVision.actions[4].number = u32(Sc::Ai::Script::TurnONSharedVisionforPlayer5);
        addSharedVision.actions[5].number = u32(Sc::Ai::Script::TurnONSharedVisionforPlayer6);
        addSharedVision.actions[6].number = u32(Sc::Ai::Script::TurnONSharedVisionforPlayer7);
        addSharedVision.actions[7].number = u32(Sc::Ai::Script::TurnONSharedVisionforPlayer8);
        mapData->triggers.push_back(addSharedVision);
    };

    switch ( defaultTriggers )
    {
    case Chk::DefaultTriggers::NoTriggers: break;
    case Chk::DefaultTriggers::DefaultMelee:
        addSetRes(Sc::Player::Id::AllPlayers);
        addDefeat(Sc::Player::Id::AllPlayers);
        addVictory(Sc::Player::Id::AllPlayers);
        break;
    case Chk::DefaultTriggers::TwoPlayerMeleeWithObs:
    case Chk::DefaultTriggers::ThreePlayerMeleeWithObs:
    case Chk::DefaultTriggers::FourPlayerMeleeWithObs:
    case Chk::DefaultTriggers::FivePlayerMeleeWithObs:
    case Chk::DefaultTriggers::SixPlayerMeleeWithObs:
    case Chk::DefaultTriggers::SevenPlayerMeleeWithObs:
        for ( size_t slot = size_t(defaultTriggers)-size_t(Chk::DefaultTriggers::TwoPlayerMeleeWithObs)+2; slot < Sc::Player::TotalSlots; ++slot )
            mapData->forces.playerForce[slot] = Chk::Force::Force2;
            
        addSetRes(Sc::Player::Id::Force1);
        addDefeat(Sc::Player::Id::Force1);
        addVictory(Sc::Player::Id::Force1);
        addSharedVision(Sc::Player::Id::Force2);
        break;
    }

    init_data(*mapData);
}

bool Scenario::empty() const
{
    return read.saveSections.empty() && read.tailLength == 0;
}

bool Scenario::isProtected() const
{
    return mapIsProtected;
}

bool Scenario::setProtected()
{
    if ( mapIsProtected )
        return false;
    else
    {
        mapIsProtected = true;
        return true;
    }
}

bool Scenario::hasPassword() const
{
    return read.tailLength == 7;
}

bool Scenario::isPassword(const std::string & password) const
{
    if ( hasPassword() )
    {
        SHA256 sha256;
        std::string hashStr = sha256(password);
        if ( hashStr.length() >= 7 )
        {
            u64 eightHashBytes = std::stoull(hashStr.substr(0, 8), nullptr, 16);
            u8* hashBytes = (u8*)&eightHashBytes;

            for ( u8 i = 0; i < read.tailLength && i < 8; i++ )
            {
                if ( read.tailData[i] != hashBytes[i] )
                    return false;
            }
            return true;
        }
        else
            return false;
    }
    else // No password
        return false;
}

bool Scenario::setPassword(const std::string & oldPass, const std::string & newPass)
{
    auto edit = create_action(ActionDescriptor::SetPassword);
    if ( !hasPassword() || isPassword(oldPass) )
    {
        if ( newPass == "" )
        {
            std::size_t zeroedLength = std::min(int(read.tailLength), 7);
            for ( std::size_t i = 0; i < zeroedLength; i++ )
                edit->tailData[i] = 0;

            edit->tailLength = 0;
            return true;
        }
        else
        {
            SHA256 sha256;
            std::string hashStr = sha256(newPass);
            if ( hashStr.length() >= 7 )
            {
                u64 eightHashBytes = stoull(hashStr.substr(0, 8), nullptr, 16);
                u8* hashBytes = (u8*)&eightHashBytes;

                edit->tailLength = 7;
                std::size_t assignedLength = std::min(int(read.tailLength), 7);
                for ( u8 i = 0; i < assignedLength; i++ )
                    edit->tailData[i] = hashBytes[i];

                return true;
            }
        }
    }
    return false;
}

bool Scenario::login(const std::string & password)
{
    if ( isPassword(password) )
    {
        mapIsProtected = false;
        return true;
    }
    return false;
}

template <typename T>
constexpr size_t size()
{
    if constexpr ( RareTs::is_static_array_v<T> )
        return ::size<RareTs::element_type_t<T>>() * std::extent_v<T>;
    else if constexpr ( RareTs::is_iterable_v<T> )
        throw std::logic_error("Cannot get a static size for a non-static-array iterable type");
    else if constexpr ( std::is_enum_v<T> )
        return sizeof(std::underlying_type_t<T>);
    else if constexpr ( RareTs::is_reflected_v<T> )
    {
        return RareTs::Members<T>::pack([](auto ... member) {
            return (size<typename std::remove_reference_t<decltype(member)>::type>() + ...);
        });
    }
    else
        return sizeof(T);
}

template <typename T, typename Value>
constexpr s32 size(const Value & value)
{
    if constexpr ( RareTs::has_begin_end_v<T> )
        return s32(::size<RareTs::element_type_t<T>>() * value.size());
    else
        return s32(::size<T>());
}

template <typename T, typename Value>
void read(std::istream & is, Value & value, std::streamsize & sectionSize)
{
    if constexpr ( RareTs::is_static_array_v<T> )
    {
        using Element = RareTs::element_type_t<T>;
        for ( size_t i=0; i<std::extent_v<T>; i++ )
            ::read<Element>(is, value[i], sectionSize);
        //is.read(reinterpret_cast<char*>(&value[0]), std::streamsize(std::extent_v<T>*sizeof(Element))); // Performs better, TODO: static check for safety then use
    }
    else if constexpr ( RareTs::is_specialization_v<T, std::vector> )
    {
        using Element = RareTs::element_type_t<T>;
        size_t count = size_t(sectionSize)/sizeof(Element) + (size_t(sectionSize)%sizeof(Element) > 0 ? 1 : 0);
        value = std::vector<Element>(count);
        for ( size_t i=0; i<count; ++i )
            ::read<Element>(is, value[i], sectionSize);
        //is.read(reinterpret_cast<char*>(&value[0]), std::streamsize(wholeElements*sizeof(Element))); // Performs better, TODO: static check for safety then use
    }
    else if constexpr ( RareTs::is_in_class_reflected_v<T> )
    {
        return RareTs::Members<T>::pack([&](auto ... member) {
            (::read<typename std::remove_reference_t<decltype(member)>::type>(is, member.value(value), sectionSize), ...);
        });
    }
    else if constexpr ( std::is_enum_v<T> )
    {
        auto bytesRead = std::min(std::streamsize(sizeof(std::underlying_type_t<T>)), sectionSize);
        if ( bytesRead > 0 )
        {
            is.read(reinterpret_cast<char*>(&value), bytesRead);
            sectionSize -= bytesRead;
        }
    }
    else
    {
        auto bytesRead = std::min(std::streamsize(sizeof(T)), sectionSize);
        if ( bytesRead > 0 )
        {
            is.read(reinterpret_cast<char*>(&value), std::min(std::streamsize(sizeof(T)), sectionSize));
            sectionSize -= bytesRead;
        }
    }
}

void Scenario::parse(std::istream & is, ::MapData & mapData, Chk::SectionName sectionName, Chk::SectionSize sectionSize)
{
    auto & section = mapData.addSaveSection(::MapData::Section{sectionName});
    switch ( sectionName )
    {
        case SectionName::MRGN: // Manual deserialization to account for zeroth location being unused
        {
            size_t numLocations = size_t(sectionSize) / ::size<Chk::Location>();
            mapData.locations.assign(numLocations+1, Chk::Location{});
            std::streamsize bytesRemaining = sectionSize;
            for ( size_t i=1; i<=numLocations; ++i )
                ::read<Chk::Location>(is, mapData.locations[i], bytesRemaining);
        }
        break;
        case SectionName::MTXM:
        {
            size_t totalTiles = sectionSize/2 + (sectionSize % 2 > 0 ? 1 : 0);
            if ( mapData.tiles.size() == 0 )
                mapData.tiles.assign(totalTiles, 0);
            else if ( totalTiles > mapData.tiles.size() )
            {
                mapData.tiles.resize(totalTiles, 0);
                this->mapIsProtected = true;
            }
            is.read(reinterpret_cast<char*>(&mapData.tiles[0]), sectionSize);
        }
        break;
        case SectionName::STR:
        {
            std::vector<u8> bytes(sectionSize);
            is.read(reinterpret_cast<char*>(&bytes[0]), sectionSize);
            if ( !mapData.hasSection(SectionName::STRx) )
                syncBytesToStrings(bytes, mapData.strings, mapData.strTailData);
        }
        break;
        case SectionName::STRx:
        {
            std::vector<u8> bytes(sectionSize);
            is.read(reinterpret_cast<char*>(&bytes[0]), sectionSize);
            syncRemasteredBytesToStrings(bytes, mapData.strings, mapData.strTailData);
        }
        break;
        case SectionName::KSTR:
        {
            std::vector<u8> bytes(sectionSize);
            is.read(reinterpret_cast<char*>(&bytes[0]), sectionSize);
            syncBytesToKstrings(bytes, mapData.editorStrings, mapData.editorStringsVersion);
        }
        break;
        default:
        {
            auto memberIndex = sectionMemberIndex.find(sectionName);
            if ( memberIndex != sectionMemberIndex.end() )
            {
                RareTs::Members<::MapData>::at(memberIndex->second, mapData, [&](auto member, auto & value) {
                    std::streamsize bytesRemaining = std::streamsize(sectionSize);
                    ::read<typename decltype(member)::type>(is, value, bytesRemaining);
                });
            }
            else
            {
                logger.info() << "Encountered unknown section: \"" << Chk::getNameString(sectionName) << "\"" << std::endl;
                section.sectionData = std::make_optional<std::vector<u8>>(std::size_t(sectionSize));
                if ( sectionSize > 0 )
                    is.read((char*)&section.sectionData.value()[0], std::streamsize(sectionSize));
            }
        }
        break;
    }
}


template <typename T>
std::vector<T> resizeAndOffset(const std::vector<T> & tiles, u16 newTileWidth, u16 newTileHeight, u16 oldTileWidth, u16 oldTileHeight, std::ptrdiff_t leftEdge, std::ptrdiff_t topEdge);

void fixTerrainToDimensions(::MapData & mapData)
{
    auto tileWidth = mapData.dimensions.tileWidth;
    auto tileHeight = mapData.dimensions.tileHeight;
    auto expectedIsomSize = (size_t(mapData.dimensions.tileWidth)/2 + 1)*(size_t(mapData.dimensions.tileHeight) + 1);
    if ( mapData.isomRects.size() != expectedIsomSize )
        mapData.isomRects.insert(mapData.isomRects.end(), std::size_t(expectedIsomSize-mapData.isomRects.size()), Chk::IsomRect{});
    if ( mapData.editorTiles.size() != size_t(tileWidth)*size_t(tileHeight) )
        mapData.editorTiles = resizeAndOffset(mapData.editorTiles, tileWidth, tileHeight, tileWidth, tileHeight, 0, 0);
    if ( mapData.tiles.size() != size_t(tileWidth)*size_t(tileHeight) )
        mapData.tiles = resizeAndOffset(mapData.tiles, tileWidth, tileHeight, tileWidth, tileHeight, 0, 0);
}

void fixTriggerExtensions(::MapData & mapData)
{
    auto deleteMapDataTriggerExtension = [&](size_t triggerExtensionIndex) {
        if ( triggerExtensionIndex < mapData.triggerExtensions.size() && triggerExtensionIndex != Chk::ExtendedTrigDataIndex::None )
        {
            size_t i = mapData.triggerExtensions.size();
            for ( ; i > 0 && (((i-1) & Chk::UnusedExtendedTrigDataIndexCheck) == 0 || i-1 >= mapData.triggerExtensions.size()); i-- );

            if ( i == 0 )
                mapData.triggerExtensions.clear();
            else if ( i < mapData.triggerExtensions.size() )
            {
                auto firstErased = std::next(mapData.triggerExtensions.begin(), i);
                mapData.triggerExtensions.erase(firstErased, mapData.triggerExtensions.end());
            }
        }
    };

    std::set<size_t> usedExtendedTrigDataIndexes;
    size_t numTriggers = mapData.triggers.size();
    for ( size_t i=0; i<numTriggers; i++ )
    {
        auto & trigger = mapData.triggers[i];
        size_t extendedDataIndex = trigger.getExtendedDataIndex();
        if ( extendedDataIndex != Chk::ExtendedTrigDataIndex::None )
        {
            if ( extendedDataIndex >= mapData.triggerExtensions.size() )
                trigger.clearExtendedDataIndex();
            else if ( usedExtendedTrigDataIndexes.find(extendedDataIndex) == usedExtendedTrigDataIndexes.end() ) // Valid extension
            {
                mapData.triggerExtensions[extendedDataIndex].trigNum = (u32)i; // Ensure the trigNum is correct
                usedExtendedTrigDataIndexes.insert(extendedDataIndex);
            }
            else // Same extension used by multiple triggers
                trigger.clearExtendedDataIndex();
        }
    }

    size_t numTriggerExtensions = mapData.triggerExtensions.size();
    for ( size_t i=0; i<numTriggerExtensions; i++ )
    {
        const Chk::ExtendedTrigData & extension = mapData.triggerExtensions[i];
        if ( usedExtendedTrigDataIndexes.find(i) == usedExtendedTrigDataIndexes.end() ) // Extension exists, but no trigger uses it
        {
            if ( extension.trigNum != Chk::ExtendedTrigData::TrigNum::None ) // Refers to a trigger
            {
                if ( extension.trigNum < mapData.triggers.size() && // this trigger exists without an extension
                    mapData.triggers[extension.trigNum].getExtendedDataIndex() == Chk::ExtendedTrigDataIndex::None )
                {
                    mapData.triggers[extension.trigNum].setExtendedDataIndex(i); // Link up extension to the trigger
                }
                else // Trigger does not exist
                    deleteMapDataTriggerExtension(i); // Delete the extension
            }
            else if ( extension.trigNum == Chk::ExtendedTrigData::TrigNum::None ) // Does not refer to a trigger
                deleteMapDataTriggerExtension(i); // Delete the extension
        }
    }
}

void removeUnusedBlankStrings(::MapData & mapData)
{
    std::bitset<Chk::MaxStrings> stringIdUsed {};
    mapData.markUsedStrings(stringIdUsed, Chk::Scope::Both, Chk::Scope::Game);
    for ( std::size_t i=0; i<mapData.strings.size(); ++i )
    {
        auto & string = mapData.strings[i];
        if ( string && string->empty() && !stringIdUsed[i] )
            string = std::nullopt;
    }

    stringIdUsed.reset();
    mapData.markUsedStrings(stringIdUsed, Chk::Scope::Both, Chk::Scope::Editor);
    for ( std::size_t i=0; i<mapData.editorStrings.size(); ++i )
    {
        auto & editorString = mapData.editorStrings[i];
        if ( editorString && editorString->empty() && !stringIdUsed[i] )
            editorString = std::nullopt;
    }
}

bool Scenario::parse(std::istream & is, bool fromMpq)
{
    auto parsingFailed = [&](const std::string & error)
    {
        logger.error(error);
        return false;
    };

    bool hasLegacyKstr = false;

    // First read contents of "is" to "chk", this will allow jumping backwards when reading chks with jump sections
    std::stringstream chkCopy(std::ios_base::binary|std::ios_base::in|std::ios_base::out);
    if ( !fromMpq ) // Input not from MPQ may be a filestream requiring a copy first, if it was from mpq it's already an appropriate sstream
        chkCopy << is.rdbuf();

    std::istream & chk = fromMpq ? is : chkCopy;
    if ( !is.good() && !is.eof() )
    {
        logger.error("Unexpected failure reading scenario contents!");
        return false; // Read error on "is"
    }

    chk.seekg(std::ios_base::beg); // Move to start of chk
    auto streamStart = chk.tellg();

    bool makeProtected = false;
    ::MapData mapData {};

    do
    {
        Chk::SectionHeader sectionHeader = {};
        chk.read((char*)&sectionHeader, sizeof(Chk::SectionHeader));
        std::streamsize headerBytesRead = chk.good() ? sizeof(Chk::SectionHeader) : chk.eof() ? chk.gcount() : 0;
        if ( headerBytesRead == 0 && chk.eof() )
            break;
        else if ( headerBytesRead == 0 )
            return parsingFailed("Unexpected failure reading chk section header!");

        if ( headerBytesRead == sizeof(Chk::SectionHeader) ) // Valid section header
        {
            if ( sectionHeader.sizeInBytes > 0 ) // Regular section
            {
                auto begin = chk.tellg();
                parse(chk, mapData, sectionHeader.name, sectionHeader.sizeInBytes);

                if ( chk.good() || chk.eof() )
                {
                    if ( Chk::SectionSize(chk.tellg() - begin) != sectionHeader.sizeInBytes ) // Undersized section
                        makeProtected = true;
                }
                else
                    return parsingFailed("Unexpected error reading chk section contents!");
            }
            else if ( sectionHeader.sizeInBytes < 0 ) // Jump section
            {
                if ( sectionHeader.sizeInBytes < s32(streamStart)-s32(chk.tellg()) )
                {
                    makeProtected = true;
                    break;
                }

                chk.seekg(sectionHeader.sizeInBytes, std::ios_base::cur);
                if ( !chk.good() )
                    return parsingFailed("Unexpected error processing chk jump section!");

                mapData.jumpCompress = true;
            }
            else // Zero-size section
                mapData.addSaveSection(::MapData::Section{sectionHeader.name});
        }
        else // if ( bytesRead < sizeof(Chk::SectionHeader) ) // Partial section header
        {
            for ( std::streamsize i=0; i<headerBytesRead; i++ )
                mapData.tailData[size_t(i)] = ((u8*)&sectionHeader)[size_t(i)];
            for ( size_t i=size_t(headerBytesRead); i<mapData.tailData.size(); i++ )
                mapData.tailData[i] = u8(0);

            mapData.tailLength = (u8)headerBytesRead;
            makeProtected = true;
        }
    }
    while ( chk.good() );

    if ( !mapData.hasSection(SectionName::VER) )
        return parsingFailed("Map was missing the VER section!");
    else if ( mapData.version > Chk::Version::StarCraft_Remastered )
    {
        logger.error() << "Map has a newer CHK version than Chkdraft supports: " << mapData.version << std::endl;
        makeProtected = true;
    }
    
    tracked::clear_history();
    this->mapIsProtected = makeProtected;

    if ( mapData.version < Chk::Version::StarCraft_Hybrid ) // Scenario::isOriginal()
        mapData.saveType = fromMpq ? SaveType::StarCraftScm : SaveType::StarCraftChk; // Vanilla
    else if ( mapData.version >= Chk::Version::StarCraft_Hybrid && mapData.version < Chk::Version::StarCraft_BroodWar ) // Scenario::isHybrid()
        mapData.saveType = fromMpq ? SaveType::HybridScm : SaveType::HybridChk; // Hybrid
    else if ( mapData.version >= Chk::Version::StarCraft_BroodWar && mapData.version < Chk::Version::StarCraft_Remastered ) // Scenario::isExpansion()
        mapData.saveType = fromMpq ? SaveType::ExpansionScx : SaveType::ExpansionChk; // Expansion
    else if ( mapData.version >= Chk::Version::StarCraft_Remastered ) // Scenario::isRemastered()
        mapData.saveType = fromMpq ? SaveType::RemasteredScx : SaveType::RemasteredChk; // Remastered

    ::fixTerrainToDimensions(mapData);
    ::fixTriggerExtensions(mapData);
    ::removeUnusedBlankStrings(mapData);
    init_data<false>(mapData);
    if ( this->upgradeKstrToCurrent() ) // Ideally not tracked in the first place but for the excessive amounts of code duplication required
        tracked::clear_history(); // Clear the KSTR upgrade out of hist in the rare case a KSTR upgrade occurs

    return true;
}

bool Scenario::hasSection(SectionName sectionName) const
{
    return read.hasSection(sectionName);
}

template <typename T, typename Value>
void write(std::ostream & os, const Value & value)
{
    if constexpr ( std::is_array_v<T> )
    {
        using Element = RareTs::element_type_t<T>;
        for ( size_t i=0; i<std::extent_v<T>; i++ )
            write<Element>(os, value[i]);
    }
    else if constexpr ( RareTs::has_begin_end_v<T> )
    {
        using Element = RareTs::element_type_t<T>;
        for ( const auto & element : value )
            write<Element>(os, element);
    }
    else if constexpr ( RareTs::is_reflected_v<T> )
    {
        return RareTs::Members<T>::pack([&](auto ... member) {
            (write<typename decltype(member)::type>(os, member.value(value)), ...);
        });
    }
    else if constexpr ( std::is_enum_v<T> )
        os.write(reinterpret_cast<const char*>(&value), sizeof(std::underlying_type_t<T>));
    else
        os.write(reinterpret_cast<const char*>(&value), sizeof(T));
}

template <typename Member, typename Value>
void writeData(std::ostream & os, const Value & value)
{
    s32 size = ::size<typename Member::type>(value);
    os.write(reinterpret_cast<const char*>(&size), sizeof(size));
    write<typename Member::type>(os, value);
}

void Scenario::writeSection(std::ostream & os, const ::MapData::Section & section, bool includeHeader)
{
    SectionName sectionName = section.sectionName;
    if ( includeHeader )
        os.write(reinterpret_cast<const char*>(&sectionName), sizeof(std::underlying_type_t<SectionName>)); // Write section name

    switch ( sectionName )
    {
        case SectionName::MRGN: // Manual serialization to account for zeroth location being unused
        {
            if ( includeHeader )
            {
                s32 size = s32((read.locations.size()-1) * ::size<Chk::Location>());
                os.write(reinterpret_cast<const char*>(&size), std::streamsize(sizeof(size)));
            }

            for ( size_t i=1; i<read.locations.size(); ++i )
                ::write<Chk::Location>(os, read.locations[i]);
        }
        break;
        case SectionName::STR:
        {
            std::vector<u8> bytes;
            syncStringsToBytes(bytes);
            s32 size = s32(bytes.size());
            if ( includeHeader )
                os.write(reinterpret_cast<const char*>(&size), std::streamsize(sizeof(size)));

            os.write(reinterpret_cast<const char*>(&bytes[0]), std::streamsize(size));
        }
        break;
        case SectionName::STRx:
        {
            std::vector<u8> bytes;
            syncRemasteredStringsToBytes(bytes);
            s32 size = s32(bytes.size());
            if ( includeHeader )
                os.write(reinterpret_cast<const char*>(&size), std::streamsize(sizeof(size)));

            os.write(reinterpret_cast<const char*>(&bytes[0]), std::streamsize(size));
        }
        break;
        case SectionName::KSTR:
        {
            std::vector<u8> bytes;
            syncKstringsToBytes(bytes);
            s32 size = s32(bytes.size());
            if ( includeHeader )
                os.write(reinterpret_cast<const char*>(&size), std::streamsize(sizeof(size)));

            os.write(reinterpret_cast<const char*>(&bytes[0]), std::streamsize(size));
        }
        break;
        default:
        {
            auto memberIndex = sectionMemberIndex.find(sectionName);
            if ( memberIndex != sectionMemberIndex.end() ) // This is a section that can be auto-serialized using reflection
            {
                RareTs::Members<::MapData>::at(memberIndex->second, this->read, [&](auto member, auto & value) {
                    ::writeData<decltype(member)>(os, value);
                });
            }
            else // This is an unknown/custom section
            {
                logger.info() << "[" << Chk::getNameString(sectionName) << "] unknown/custom" << std::endl;
                if ( section.sectionData )
                {
                    s32 size = s32(section.sectionData->size());
                    os.write(reinterpret_cast<const char*>(&size), std::streamsize(sizeof(size)));
                    if ( size > 0 )
                        os.write(reinterpret_cast<const char*>(&section.sectionData.value()[0]), std::streamsize(size));
                }
                else
                {
                    s32 size = 0;
                    os.write(reinterpret_cast<const char*>(&size), std::streamsize(sizeof(size)));
                }
            }
        }
        break;
    }
}

void Scenario::writeSection(std::ostream & os, Chk::SectionName sectionName, bool includeHeader)
{
    auto memberIndex = sectionMemberIndex.find(sectionName);
    if ( memberIndex != sectionMemberIndex.end() ) // This is a section that can be auto-serialized using reflection
    {
        RareTs::Members<::MapData>::at(memberIndex->second, this->read, [&](auto member, auto & value) {
            ::writeData<decltype(member)>(os, value);
        });
    }
    else // This may be a section with special serialization, a custom, or an unknown section
    {
        bool found = false;
        for ( const auto & section : read.saveSections )
        {
            if ( sectionName == section.sectionName )
            {
                found = true;
                writeSection(os, section, includeHeader);
                break;
            }
        }
        if ( !found )
            throw std::invalid_argument("The given section was not present in the scenario file!");
    }
}

void Scenario::write(std::ostream & os)
{
    try
    {
        for ( const auto & section : read.saveSections )
            writeSection(os, section, true);

        if ( read.tailLength > 0 )
            os.write(reinterpret_cast<const char*>(&read.tailData[0]), std::streamsize(read.tailLength < 8 ? read.tailLength : 7));
    }
    catch ( std::exception & e )
    {
        os.setstate(std::ios_base::failbit);
        logger.error("Error writing scenario file ", e);
    }
}

std::vector<u8> Scenario::serialize()
{
    Chk::Size size = 0;
    std::stringstream chk(std::ios_base::in|std::ios_base::out|std::ios_base::binary);
    chk.write((const char*)&Chk::CHK, sizeof(Chk::CHK)); // Header
    chk.write((const char*)&size, sizeof(size)); // Size

    write(chk);
    chk.unsetf(std::ios_base::skipws);
    auto start = std::istream_iterator<u8>(chk);
    std::vector<u8> chkBytes(start, std::istream_iterator<u8>());

    (Chk::Size &)chkBytes[sizeof(Chk::CHK)] = Chk::Size(chkBytes.size() - sizeof(Chk::CHK) - sizeof(size));

    return chkBytes;
}

bool Scenario::deserialize(Chk::SerializedChk* data)
{
    if ( data->header.name == Chk::CHK )
    {
        Chk::Size size = data->header.sizeInBytes;
        std::stringstream chk(std::ios_base::in|std::ios_base::out|std::ios_base::binary);
        std::copy(&data->data[0], &data->data[size], std::ostream_iterator<u8>(chk));
    }
    return false;
}

std::size_t Scenario::addSaveSection(::MapData::Section section)
{
    if ( hasSection(section.sectionName) )
    {
        for ( std::size_t i=0; i<read.saveSections.size(); ++i )
        {
            if ( section.sectionName == read.saveSections[i].sectionName )
                return i;
        }
        throw std::logic_error("An internal error occured");
    }
    else
    {
        create_action(ActionDescriptor::AddSaveSection)->saveSections.append(section);
        return read.saveSections.size()-1;
    }
}

void Scenario::addSaveSection(Chk::SectionName sectionName)
{
    if ( !hasSection(sectionName) )
        Scenario::addSaveSection(::MapData::Section{sectionName, std::nullopt});
}

void Scenario::removeSaveSection(Chk::SectionName sectionName)
{
    if ( hasSection(sectionName) )
    {
        std::vector<std::size_t> removedSectionIndexes {};
        for ( std::size_t i=0; i<read.saveSections.size(); ++i )
        {
            if ( read.saveSections[i].sectionName == sectionName )
                removedSectionIndexes.push_back(i);
        }
        create_action(ActionDescriptor::RemoveSaveSection)->saveSections.remove(removedSectionIndexes);
    }
}

void Scenario::updateSaveSections()
{
    std::vector<::MapData::Section> addedSections {};
    if ( this->hasExtendedStrings() )
    {
        addedSections.push_back(::MapData::Section{SectionName::OSTR});
        addedSections.push_back(::MapData::Section{SectionName::KSTR});
    }

    if ( !read.triggerExtensions.empty() )
        addedSections.push_back(::MapData::Section{SectionName::KTRG});

    if ( !read.triggerGroupings.empty() )
        addedSections.push_back(::MapData::Section{SectionName::KTGP});

    if ( !addedSections.empty() )
        create_action(ActionDescriptor::UpdateSaveSections)->saveSections.append(addedSections);
}

bool Scenario::changeVersionTo(Chk::Version version, bool lockAnywhere, bool autoDefragmentLocations)
{
    auto edit = create_action(ActionDescriptor::UpdateMapVersion);
    auto oldVersion = read.version;

    if ( version < Chk::Version::StarCraft_BroodWar ) // Original or Hybrid: include all original properties
    {
        if ( version < Chk::Version::StarCraft_Hybrid ) // Original: No COLR, TYPE, IVE2, or expansion properties
        {
            if ( !this->trimLocationsToOriginal(lockAnywhere, autoDefragmentLocations) )
            {
                logger.error("Cannot save as original with over 64 locations in use!");
                return false;
            }
            if ( read.mapType != Chk::Type::RAWS )
                edit->mapType = Chk::Type::RAWS;
            if ( read.iVersion != Chk::IVersion::Current )
                edit->iVersion = Chk::IVersion::Current;
            removeSaveSection(SectionName::TYPE);
            removeSaveSection(SectionName::PUPx);
            removeSaveSection(SectionName::PTEx);
            removeSaveSection(SectionName::UNIx);
            removeSaveSection(SectionName::TECx);
            removeSaveSection(SectionName::COLR);
        }
        else
        {
            if ( read.mapType != Chk::Type::RAWS )
                edit->mapType = Chk::Type::RAWS;
            if ( read.iVersion != Chk::IVersion::Current )
                edit->iVersion = Chk::IVersion::Current;
            if ( read.i2Version != Chk::I2Version::StarCraft_1_04 )
                edit->i2Version = Chk::I2Version::StarCraft_1_04;
            expandToScHybridOrExpansion();
        }
        addSaveSection(::MapData::Section{SectionName::IVER});
        addSaveSection(::MapData::Section{SectionName::UPGR});
        addSaveSection(::MapData::Section{SectionName::PTEC});
        addSaveSection(::MapData::Section{SectionName::UNIS});
        addSaveSection(::MapData::Section{SectionName::UPGS});
        addSaveSection(::MapData::Section{SectionName::TECS});
    }
    else // if ( version >= Chk::Version::StarCraft_BroodWar ) // Broodwar: No IVER or original properties
    {
        if ( read.mapType != Chk::Type::RAWB )
            edit->mapType = Chk::Type::RAWB;
        if ( read.iVersion != Chk::IVersion::Current )
            edit->iVersion = Chk::IVersion::Current;
        if ( read.i2Version != Chk::I2Version::StarCraft_1_04 )
            edit->i2Version = Chk::I2Version::StarCraft_1_04;
        this->expandToScHybridOrExpansion();

        removeSaveSection(SectionName::IVER);
        removeSaveSection(SectionName::UPGR);
        removeSaveSection(SectionName::PTEC);
        removeSaveSection(SectionName::UNIS);
        removeSaveSection(SectionName::UPGS);
        removeSaveSection(SectionName::TECS);
    }
        
    if ( version >= Chk::Version::StarCraft_Hybrid ) // Hybrid or BroodWar: Include TYPE, IVE2, COLR, and all expansion properties
    {
        if ( read.mapType != (version >= Chk::Version::StarCraft_BroodWar ? Chk::Type::RAWB : Chk::Type::RAWS) )
            edit->mapType = version >= Chk::Version::StarCraft_BroodWar ? Chk::Type::RAWB : Chk::Type::RAWS;
        addSaveSection(::MapData::Section{SectionName::TYPE});
        addSaveSection(::MapData::Section{SectionName::PUPx});
        addSaveSection(::MapData::Section{SectionName::PTEx});
        addSaveSection(::MapData::Section{SectionName::UNIx});
        addSaveSection(::MapData::Section{SectionName::UPGx});
        addSaveSection(::MapData::Section{SectionName::TECx});

        if ( version >= Chk::Version::StarCraft_Remastered )
            upgradeToRemasteredColors();
        else if ( !hasSection(SectionName::COLR) )
        {
            addSaveSection(::MapData::Section{SectionName::COLR});
            for ( size_t i=size_t(Chk::PlayerColor::Red); i<=Chk::PlayerColor::Yellow; ++i )
                this->setPlayerColor(i, Chk::PlayerColor(i));
        }
    }
    
    if ( read.version != version )
        edit->version = version;

    this->deleteUnusedStrings(Chk::Scope::Both);
    return true;
}

Chk::Version Scenario::getVersion() const
{
    return read.version;
}

bool Scenario::isVersion(Chk::Version version) const
{
    return version == read.version;
}

bool Scenario::isOriginal() const
{
    return read.version < Chk::Version::StarCraft_Hybrid;
}

bool Scenario::isHybrid() const
{
    return read.version >= Chk::Version::StarCraft_Hybrid && read.version < Chk::Version::StarCraft_BroodWar;
}

bool Scenario::isExpansion() const
{
    return read.version >= Chk::Version::StarCraft_BroodWar && read.version < Chk::Version::StarCraft_Remastered;
}

bool Scenario::isHybridOrAbove() const
{
    return read.version >= Chk::Version::StarCraft_Hybrid;
}

bool Scenario::isRemastered() const
{
    return read.version >= Chk::Version::StarCraft_Remastered;
}

bool Scenario::hasDefaultValidation() const
{
    Chk::VCOD vcod{};
    for ( size_t i=0; i<Chk::TotalValidationSeeds; ++i )
    {
        if ( read.validation.seedValues[i] != vcod.seedValues[i] )
            return false;
    }
    for ( size_t i=0; i<Chk::TotalValidationOpCodes; ++i )
    {
        if ( read.validation.opCodes[i] != vcod.opCodes[i] )
            return false;
    }
    return true;
}

void Scenario::setToDefaultValidation()
{
    create_action(ActionDescriptor::ResetVcod)->validation = Chk::VCOD{};
}

bool Scenario::hasExtendedStrings() const
{
    return !read.editorStrings.empty();
}

size_t Scenario::getCapacity(Chk::Scope storageScope) const
{
    if ( storageScope == Chk::Scope::Game )
        return read.strings.size();
    else if ( storageScope == Chk::Scope::Editor )
        return read.editorStrings.size();
    else
        return 0;
}

size_t Scenario::getBytesUsed(Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Game )
    {
        std::vector<u8> bytes {};
        if ( hasSection(SectionName::STRx) )
            syncRemasteredStringsToBytes(bytes);
        else
            syncStringsToBytes(bytes);

        return bytes.size();
    }
    else if ( storageScope == Chk::Scope::Editor )
    {
        std::vector<u8> bytes {};
        syncKstringsToBytes(bytes);
        return bytes.size();
    }
    else
        return 0;
}

bool Scenario::stringStored(size_t stringId, Chk::Scope storageScope) const
{
    if ( (storageScope & Chk::Scope::Game) == Chk::Scope::Game )
        return stringId < read.strings.size() && read.strings[stringId];
    else if ( (storageScope & Chk::Scope::Editor) == Chk::Scope::Editor )
        return stringId < read.editorStrings.size() && read.editorStrings[stringId];

    return false;
}

void Scenario::appendUsage(size_t stringId, std::vector<Chk::StringUser> & stringUsers, Chk::Scope storageScope, u32 userMask) const
{
    if ( stringId == Chk::StringId::NoString )
        return;

    if ( storageScope == Chk::Scope::Game )
    {
        if ( stringId < Chk::MaxStrings ) // 16 or 32-bit stringId
        {
            if ( (userMask & Chk::StringUserFlag::ScenarioProperties) != Chk::StringUserFlag::None )
            {
                if ( (userMask & Chk::StringUserFlag::ScenarioName) == Chk::StringUserFlag::ScenarioName && read.scenarioProperties.scenarioNameStringId == (u16)stringId )
                    stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::ScenarioName));

                if ( (userMask & Chk::StringUserFlag::ScenarioDescription) == Chk::StringUserFlag::ScenarioDescription && read.scenarioProperties.scenarioDescriptionStringId == (u16)stringId )
                    stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::ScenarioDescription));
            }
            if ( (userMask & Chk::StringUserFlag::Force) != Chk::StringUserFlag::None )
            {
                appendForceStrUsage(stringId, stringUsers, userMask);
            }
            if ( (userMask & Chk::StringUserFlag::BothUnitSettings) != Chk::StringUserFlag::None )
            {
                appendUnitStrUsage(stringId, stringUsers, userMask);
            }
            if ( (userMask & Chk::StringUserFlag::Location) != Chk::StringUserFlag::None )
            {
                appendLocationStrUsage(stringId, stringUsers, userMask);
            }
            if ( (userMask & Chk::StringUserFlag::AnyTrigger) != Chk::StringUserFlag::None )
            {
                appendTriggerStrUsage(stringId, stringUsers, storageScope, userMask);
            }
        }
        else if ( (userMask & Chk::StringUserFlag::AnyTrigger) != Chk::StringUserFlag::None ) // stringId >= Chk::MaxStrings // 32-bit stringId
        {
            appendTriggerStrUsage(stringId, stringUsers, storageScope, userMask);
        }
    }
    else if ( storageScope == Chk::Scope::Editor )
    {
        if ( read.editorStringOverrides.scenarioName == stringId )
            stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::ScenarioName));

        if ( read.editorStringOverrides.scenarioDescription == stringId )
            stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::ScenarioDescription));

        for ( size_t i=0; i<Chk::TotalForces; i++ )
        {
            if ( read.editorStringOverrides.forceName[i] == stringId )
                stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::Force, i));
        }

        for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
        {
            if ( read.editorStringOverrides.unitName[i] == stringId )
                stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::OriginalUnitSettings, i));
        }

        for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
        {
            if ( read.editorStringOverrides.expUnitName[i] == stringId )
                stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::ExpansionUnitSettings, i));
        }

        for ( size_t i=0; i<Chk::TotalSounds; i++ )
        {
            if ( read.editorStringOverrides.soundPath[i] == stringId )
                stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::Sound, i));
        }

        for ( size_t i=0; i<Chk::TotalSwitches; i++ )
        {
            if ( read.editorStringOverrides.switchName[i] == stringId )
                stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::Switch, i));
        }

        for ( size_t i=0; i<Chk::TotalLocations; i++ )
        {
            if ( read.editorStringOverrides.locationName[i] == stringId )
                stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::Location, i));
        }
    }
}

bool Scenario::stringUsed(size_t stringId, Chk::Scope usageScope, Chk::Scope storageScope, u32 userMask, bool ensureStored) const
{
    if ( storageScope == Chk::Scope::Game && ((stringId < read.strings.size() && read.strings[stringId]) || !ensureStored) )
    {
        if ( stringId < Chk::MaxStrings ) // 16 or 32-bit stringId
        {
            if ( usageScope == Chk::Scope::Editor )
                return locationStringUsed(stringId, storageScope, userMask) || triggerEditorStringUsed(stringId, storageScope, userMask);
            else if ( usageScope == Chk::Scope::Game )
            {
                return ((userMask & Chk::StringUserFlag::ScenarioName) == Chk::StringUserFlag::ScenarioName && read.scenarioProperties.scenarioNameStringId == (u16)stringId ) ||
                    ((userMask & Chk::StringUserFlag::ScenarioDescription) == Chk::StringUserFlag::ScenarioDescription && read.scenarioProperties.scenarioDescriptionStringId == (u16)stringId ) ||
                    forceStringUsed(stringId, userMask) || unitStringUsed(stringId, userMask) || triggerGameStringUsed(stringId, userMask);
            }
            else // if ( usageScope == Chk::Scope::Either )
            {
                return ((userMask & Chk::StringUserFlag::ScenarioName) == Chk::StringUserFlag::ScenarioName && read.scenarioProperties.scenarioNameStringId == (u16)stringId ) ||
                    ((userMask & Chk::StringUserFlag::ScenarioDescription) == Chk::StringUserFlag::ScenarioDescription && read.scenarioProperties.scenarioDescriptionStringId == (u16)stringId ) ||
                    forceStringUsed(stringId, userMask) || unitStringUsed(stringId, userMask) ||
                    locationStringUsed(stringId, storageScope, userMask) || triggerStringUsed(stringId, storageScope, userMask);
            }
        }
        else // stringId >= Chk::MaxStrings // 32-bit stringId
        {
            return usageScope == Chk::Scope::Either && triggerStringUsed(stringId, storageScope, userMask) ||
                usageScope == Chk::Scope::Game && triggerGameStringUsed(stringId, userMask) ||
                usageScope == Chk::Scope::Editor && triggerEditorStringUsed(stringId, storageScope, userMask);
        }
    }
    else if ( storageScope == Chk::Scope::Editor && ((stringId < read.editorStrings.size() && read.editorStrings[stringId]) || !ensureStored) )
    {
        if ( read.editorStringOverrides.scenarioName == stringId || read.editorStringOverrides.scenarioDescription == stringId )
            return true;

        for ( size_t i=0; i<Chk::TotalForces; i++ )
        {
            if ( read.editorStringOverrides.forceName[i] == stringId )
                return true;
        }

        for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
        {
            if ( read.editorStringOverrides.unitName[i] == stringId )
                return true;
        }

        for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
        {
            if ( read.editorStringOverrides.expUnitName[i] == stringId )
                return true;
        }

        for ( size_t i=0; i<Chk::TotalSounds; i++ )
        {
            if ( read.editorStringOverrides.soundPath[i] == stringId )
                return true;
        }

        for ( size_t i=0; i<Chk::TotalSwitches; i++ )
        {
            if ( read.editorStringOverrides.switchName[i] == stringId )
                return true;
        }

        for ( size_t i=0; i<Chk::TotalLocations; i++ )
        {
            if ( read.editorStringOverrides.locationName[i] == stringId )
                return true;
        }
    }
    return false;
}

void Scenario::markUsedStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, Chk::Scope usageScope, Chk::Scope storageScope, u32 userMask) const
{
    return read.markUsedStrings(stringIdUsed, usageScope, storageScope, userMask);
}

void Scenario::markValidUsedStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, Chk::Scope usageScope, Chk::Scope storageScope, u32 userMask) const
{
    markUsedStrings(stringIdUsed, usageScope, storageScope, userMask);
    switch ( storageScope )
    {
        case Chk::Scope::Game:
        {
            size_t limit = std::min((size_t)Chk::MaxStrings, read.strings.size());
            size_t stringId = 1;
            for ( ; stringId < limit; ++stringId )
            {
                if ( stringIdUsed[stringId] && !read.strings[stringId])
                    stringIdUsed[stringId] = false;
            }
            for ( ; stringId < Chk::MaxStrings; stringId++ )
            {
                if ( stringIdUsed[stringId] )
                    stringIdUsed[stringId] = false;
            }
        }
        break;
        case Chk::Scope::Editor:
        {
            size_t limit = std::min((size_t)Chk::MaxKStrings, read.editorStrings.size());
            size_t stringId = 1;
            for ( ; stringId < limit; ++stringId )
            {
                if ( stringIdUsed[stringId] && !read.editorStrings[stringId] )
                    stringIdUsed[stringId] = false;
            }
            for ( ; stringId < Chk::MaxStrings; stringId++ )
            {
                if ( stringIdUsed[stringId] )
                    stringIdUsed[stringId] = false;
            }
        }
        break;
        case Chk::Scope::Either:
        {
            size_t limit = std::min(std::min((size_t)Chk::MaxStrings, getCapacity(Chk::Scope::Game)), getCapacity(Chk::Scope::Editor));
            size_t stringId = 1;
            for ( ; stringId < limit; stringId++ )
            {
                if ( stringIdUsed[stringId] && !(stringId < read.strings.size() && read.strings[stringId]) && !(stringId < read.editorStrings.size() && read.editorStrings[stringId]) )
                    stringIdUsed[stringId] = false;
            }

            if ( getCapacity(Chk::Scope::Game) > getCapacity(Chk::Scope::Editor) )
            {
                for ( ; stringId < getCapacity(Chk::Scope::Game); stringId++ )
                {
                    if ( stringIdUsed[stringId] && !(stringId < read.strings.size() && read.strings[stringId]) )
                        stringIdUsed[stringId] = false;
                }
            }
            else if ( getCapacity(Chk::Scope::Editor) > getCapacity(Chk::Scope::Game) )
            {
                for ( ; stringId < getCapacity(Chk::Scope::Editor); stringId++ )
                {
                    if ( stringIdUsed[stringId] && !(stringId < read.editorStrings.size() && read.editorStrings[stringId]) )
                        stringIdUsed[stringId] = false;
                }
            }

            for ( ; stringId < Chk::MaxStrings; stringId++ )
            {
                if ( stringIdUsed[stringId] )
                    stringIdUsed[stringId] = false;
            }
        
        }
        break;
        default: break;
    }
}

StrProp Scenario::getProperties(size_t editorStringId) const
{
    return hasSection(Chk::SectionName::KSTR) && editorStringId < read.editorStrings.size() ? read.editorStrings[editorStringId]->properties : StrProp{};
}

void Scenario::setProperties(size_t editorStringId, const StrProp & strProp)
{
    if ( hasSection(Chk::SectionName::KSTR) )
    {
        if ( editorStringId < read.editorStrings.size() && read.editorStrings[editorStringId] )
            create_action(ActionDescriptor::SetStringProperties)->editorStrings[editorStringId]->properties = strProp;
    }
}

template <typename StringType>
std::optional<StringType> Scenario::getString(size_t stringId, Chk::Scope storageScope) const
{
    auto getGameString = [&](){
        return stringId < read.strings.size() && read.strings[stringId] ?
            std::optional<StringType>(read.strings[stringId]->toString<StringType>()) : std::nullopt;
    };
    auto getEditorString = [&](){
        return stringId < read.editorStrings.size() && read.editorStrings[stringId] ?
            std::optional<StringType>(read.editorStrings[stringId]->toString<StringType>()) : std::nullopt;
    };
    switch ( storageScope )
    {
        case Chk::Scope::Either:
        case Chk::Scope::EditorOverGame:
        {
            auto editorResult = getEditorString();
            return editorResult ? editorResult : getGameString();
        }
        case Chk::Scope::Game: return getGameString();
        case Chk::Scope::Editor: return getEditorString();
        case Chk::Scope::GameOverEditor:
        {
            auto gameResult = getGameString();
            return gameResult ? gameResult : getEditorString();
        }
        default: return std::nullopt;
    }
}
template std::optional<RawString> Scenario::getString<RawString>(size_t stringId, Chk::Scope storageScope) const;
template std::optional<EscString> Scenario::getString<EscString>(size_t stringId, Chk::Scope storageScope) const;
template std::optional<ChkdString> Scenario::getString<ChkdString>(size_t stringId, Chk::Scope storageScope) const;
template std::optional<SingleLineChkdString> Scenario::getString<SingleLineChkdString>(size_t stringId, Chk::Scope storageScope) const;

template <typename StringType>
size_t Scenario::findString(const StringType & str, Chk::Scope storageScope) const
{
    auto findGameString = [&](){
        for ( size_t stringId=1; stringId<read.strings.size(); stringId++ )
        {
            if ( read.strings[stringId] && read.strings[stringId]->compare<StringType>(str) == 0 )
                return stringId;
        }
        return size_t(Chk::StringId::NoString);
    };
    auto findEditorString = [&](){
        for ( size_t stringId=1; stringId<read.editorStrings.size(); stringId++ )
        {
            if ( read.editorStrings[stringId] && read.editorStrings[stringId]->compare<StringType>(str) == 0 )
                return stringId;
        }
        return size_t(Chk::StringId::NoString);
    };
    switch ( storageScope )
    {
        case Chk::Scope::Game: return findGameString();
        case Chk::Scope::Editor: return findEditorString();
        case Chk::Scope::GameOverEditor:
        case Chk::Scope::Either:
            {
                size_t gameResult = findGameString();
                return gameResult != Chk::StringId::NoString ? gameResult : findEditorString();
            }
        case Chk::Scope::EditorOverGame:
            {
                size_t editorResult = findEditorString();
                return editorResult != Chk::StringId::NoString ? editorResult : findGameString();
            }
        default: break;
    }
    return size_t(Chk::StringId::NoString);
}
template size_t Scenario::findString<RawString>(const RawString & str, Chk::Scope storageScope) const;
template size_t Scenario::findString<EscString>(const EscString & str, Chk::Scope storageScope) const;
template size_t Scenario::findString<ChkdString>(const ChkdString & str, Chk::Scope storageScope) const;
template size_t Scenario::findString<SingleLineChkdString>(const SingleLineChkdString & str, Chk::Scope storageScope) const;

bool Scenario::setCapacity(size_t stringCapacity, Chk::Scope storageScope, bool autoDefragment)
{
    if ( storageScope == Chk::Scope::Game )
    {
        if ( stringCapacity > Chk::MaxStrings )
            throw Chk::MaximumStringsExceeded();

        std::bitset<Chk::MaxStrings> stringIdUsed;
        markValidUsedStrings(stringIdUsed, Chk::Scope::Either, Chk::Scope::Game);
        size_t numValidUsedStrings = 0;
        size_t highestValidUsedStringId = 0;
        for ( size_t stringId = 1; stringId<Chk::MaxStrings; stringId++ )
        {
            if ( stringIdUsed[stringId] )
            {
                numValidUsedStrings ++;
                highestValidUsedStringId = stringId;
            }
        }

        if ( numValidUsedStrings > stringCapacity )
            throw Chk::InsufficientStringCapacity(Chk::getNameString(SectionName::STR), numValidUsedStrings, stringCapacity, autoDefragment);
        else if ( highestValidUsedStringId > stringCapacity )
        {
            if ( autoDefragment && numValidUsedStrings <= stringCapacity )
                defragment(Chk::Scope::Game, false);
            else
                throw Chk::InsufficientStringCapacity(Chk::getNameString(SectionName::STR), numValidUsedStrings, stringCapacity, autoDefragment);
        }
        
        if ( read.strings.size() <= stringCapacity )
            create_action()->strings.append(std::vector<std::optional<ScStr>>(1+stringCapacity-read.strings.size(), std::nullopt));
        else if ( read.strings.size() > stringCapacity )
        {
            std::vector<std::size_t> indexesRemoved(read.strings.size()-stringCapacity);
            std::iota(indexesRemoved.begin(), indexesRemoved.end(), stringCapacity);
            create_action()->strings.remove(indexesRemoved);
        }

        return true;
    }
    else if ( storageScope == Chk::Scope::Editor )
    {
        if ( stringCapacity > Chk::MaxKStrings )
            throw Chk::MaximumStringsExceeded();

        std::bitset<Chk::MaxStrings> stringIdUsed;
        markValidUsedStrings(stringIdUsed, Chk::Scope::Either, Chk::Scope::Editor);
        size_t numValidUsedStrings = 0;
        size_t highestValidUsedStringId = 0;
        for ( size_t stringId = 1; stringId<Chk::MaxStrings; stringId++ )
        {
            if ( stringIdUsed[stringId] )
            {
                numValidUsedStrings ++;
                highestValidUsedStringId = stringId;
            }
        }

        if ( numValidUsedStrings > stringCapacity )
            throw Chk::InsufficientStringCapacity(Chk::getNameString(SectionName::KSTR), numValidUsedStrings, stringCapacity, autoDefragment);
        else if ( highestValidUsedStringId > stringCapacity )
        {
            if ( autoDefragment && numValidUsedStrings <= stringCapacity )
                defragment(Chk::Scope::Editor, false);
            else
                throw Chk::InsufficientStringCapacity(Chk::getNameString(SectionName::KSTR), numValidUsedStrings, stringCapacity, autoDefragment);
        }
        
        if ( read.editorStrings.size() <= stringCapacity )
            create_action()->editorStrings.append(std::vector<std::optional<ScStr>>(1+stringCapacity-read.editorStrings.size(), std::nullopt));
        else if ( read.strings.size() > stringCapacity )
        {
            std::vector<std::size_t> indexesRemoved(read.editorStrings.size()-stringCapacity);
            std::iota(indexesRemoved.begin(), indexesRemoved.end(), stringCapacity);
            create_action()->editorStrings.remove(indexesRemoved);
        }

        return true;
    }
    return false;
}

template <typename StringType>
size_t Scenario::addString(const StringType & str, Chk::Scope storageScope, bool autoDefragment)
{
    auto edit = create_action(ActionDescriptor::AddString);
    if ( storageScope == Chk::Scope::Game )
    {
        RawString rawString;
        convertStr<StringType, RawString>(str, rawString);

        size_t stringId = findString<StringType>(str);
        if ( stringId != (size_t)Chk::StringId::NoString )
            return stringId; // String already exists, return the id

        std::bitset<Chk::MaxStrings> stringIdUsed;
        markUsedStrings(stringIdUsed, Chk::Scope::Either, Chk::Scope::Game);
        size_t nextUnusedStringId = Chk::MaxStrings;
        size_t limit = Chk::MaxStrings;
        for ( size_t i=1; i<limit; i++ )
        {
            if ( !stringIdUsed[i] )
            {
                nextUnusedStringId = i;
                break;
            }
        }

        if ( nextUnusedStringId == Chk::MaxStrings )
            throw Chk::MaximumStringsExceeded();
        else if ( nextUnusedStringId >= read.strings.size() )
            setCapacity(nextUnusedStringId+1, Chk::Scope::Game, autoDefragment);

        edit->strings[nextUnusedStringId] = rawString;
        return nextUnusedStringId;
    }
    else if ( storageScope == Chk::Scope::Editor )
    {
        RawString rawString;
        convertStr<StringType, RawString>(str, rawString);

        size_t stringId = findString<StringType>(str);
        if ( stringId != (size_t)Chk::StringId::NoString )
            return stringId; // String already exists, return the id

        std::bitset<Chk::MaxStrings> stringIdUsed;
        markUsedStrings(stringIdUsed, Chk::Scope::Either, Chk::Scope::Editor);
        size_t nextUnusedStringId = Chk::MaxKStrings;
        size_t limit = Chk::MaxStrings;
        for ( size_t i=1; i<limit; i++ )
        {
            if ( !stringIdUsed[i] )
            {
                nextUnusedStringId = i;
                break;
            }
        }
        
        if ( nextUnusedStringId == Chk::MaxKStrings )
            throw Chk::MaximumStringsExceeded();
        else if ( nextUnusedStringId >= read.editorStrings.size() )
            setCapacity(nextUnusedStringId+1, Chk::Scope::Editor, autoDefragment);

        edit->editorStrings[nextUnusedStringId] = rawString;
        return nextUnusedStringId;
    }
    return (size_t)Chk::StringId::NoString;
}
template size_t Scenario::addString<RawString>(const RawString & str, Chk::Scope storageScope, bool autoDefragment);
template size_t Scenario::addString<EscString>(const EscString & str, Chk::Scope storageScope, bool autoDefragment);
template size_t Scenario::addString<ChkdString>(const ChkdString & str, Chk::Scope storageScope, bool autoDefragment);
template size_t Scenario::addString<SingleLineChkdString>(const SingleLineChkdString & str, Chk::Scope storageScope, bool autoDefragment);

template <typename StringType>
void Scenario::replaceString(size_t stringId, const StringType & str, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Game )
    {
        RawString rawString;
        convertStr<StringType, RawString>(str, rawString);

        if ( stringId < read.strings.size() && read.strings[stringId]->str != rawString )
            create_action(ActionDescriptor::ReplaceString)->strings[stringId] = rawString;
    }
    else if ( storageScope == Chk::Scope::Editor )
    {
        RawString rawString;
        convertStr<StringType, RawString>(str, rawString);

        if ( stringId < read.editorStrings.size() && read.editorStrings[stringId]->str != rawString )
            create_action(ActionDescriptor::ReplaceString)->editorStrings[stringId] = rawString;
    }
}
template void Scenario::replaceString<RawString>(size_t stringId, const RawString & str, Chk::Scope storageScope);
template void Scenario::replaceString<EscString>(size_t stringId, const EscString & str, Chk::Scope storageScope);
template void Scenario::replaceString<ChkdString>(size_t stringId, const ChkdString & str, Chk::Scope storageScope);
template void Scenario::replaceString<SingleLineChkdString>(size_t stringId, const SingleLineChkdString & str, Chk::Scope storageScope);

void Scenario::deleteUnusedStrings(Chk::Scope storageScope)
{
    auto edit = create_action();
    auto deleteUnusedGameStrings = [&]() {
        std::bitset<65536> stringIdUsed;
        markUsedStrings(stringIdUsed, Chk::Scope::Either, Chk::Scope::Game);
        for ( size_t i=0; i<read.strings.size(); i++ )
        {
            if ( !stringIdUsed[i] && read.strings[i] && !read.strings[i]->empty() )
                edit->strings[i] = std::nullopt;
        }
    };
    auto deleteUnusedEditorStrings = [&]() {
        std::bitset<65536> stringIdUsed;
        markUsedStrings(stringIdUsed, Chk::Scope::Either, Chk::Scope::Editor);
        for ( size_t i=0; i<read.editorStrings.size(); i++ )
        {
            if ( !stringIdUsed[i] && read.editorStrings[i] && !read.editorStrings[i]->empty() )
                edit->editorStrings[i] = std::nullopt;
        }
    };
    switch ( storageScope )
    {
        case Chk::Scope::Game: deleteUnusedGameStrings(); break;
        case Chk::Scope::Editor: deleteUnusedEditorStrings(); break;
        case Chk::Scope::Both: deleteUnusedGameStrings(); deleteUnusedEditorStrings(); break;
        default: break;
    }
}

void Scenario::deleteString(size_t stringId, Chk::Scope storageScope, bool deleteOnlyIfUnused)
{
    auto edit = create_action(ActionDescriptor::DeleteString);
    if ( (storageScope & Chk::Scope::Game) == Chk::Scope::Game )
    {
        if ( !deleteOnlyIfUnused || !stringUsed(stringId, Chk::Scope::Game) )
        {
            if ( stringId < read.strings.size() )
                edit->strings[stringId] = std::nullopt;

            if ( read.scenarioProperties.scenarioNameStringId == stringId )
                edit->scenarioProperties.scenarioNameStringId = 0;

            if ( read.scenarioProperties.scenarioDescriptionStringId == stringId )
                edit->scenarioProperties.scenarioDescriptionStringId = 0;

            deleteForceString(stringId);
            deleteUnitString(stringId);
            deleteLocationString(stringId);
            deleteTriggerString(stringId, Chk::Scope::Game);
        }
    }
    
    if ( (storageScope & Chk::Scope::Editor) == Chk::Scope::Editor )
    {
        if ( !deleteOnlyIfUnused || !stringUsed(stringId, Chk::Scope::Either, Chk::Scope::Editor, Chk::StringUserFlag::All, true) )
        {
            if ( stringId < read.editorStrings.size() )
                edit->editorStrings[stringId] = std::nullopt;

            if ( read.editorStringOverrides.scenarioName == stringId )
                edit->editorStringOverrides.scenarioName = 0;
    
            if ( read.editorStringOverrides.scenarioDescription == stringId )
                edit->editorStringOverrides.scenarioDescription = 0;

            for ( size_t i=0; i<Chk::TotalForces; i++ )
            {
                if ( read.editorStringOverrides.forceName[i] == stringId )
                    edit->editorStringOverrides.forceName[i] = 0;
            }

            for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
            {
                if ( read.editorStringOverrides.unitName[i] == stringId )
                    edit->editorStringOverrides.unitName[i] = 0;
            }

            for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
            {
                if ( read.editorStringOverrides.expUnitName[i] == stringId )
                    edit->editorStringOverrides.expUnitName[i] = 0;
            }

            for ( size_t i=0; i<Chk::TotalSounds; i++ )
            {
                if ( read.editorStringOverrides.soundPath[i] == stringId )
                    edit->editorStringOverrides.soundPath[i] = 0;
            }

            for ( size_t i=0; i<Chk::TotalSwitches; i++ )
            {
                if ( read.editorStringOverrides.switchName[i] == stringId )
                    edit->editorStringOverrides.switchName[i] = 0;
            }

            for ( size_t i=0; i<Chk::TotalLocations; i++ )
            {
                if ( read.editorStringOverrides.locationName[i] == stringId )
                    edit->editorStringOverrides.locationName[i] = 0;
            }
            deleteTriggerString(stringId, Chk::Scope::Editor);
        }
    }
}

void Scenario::moveString(size_t stringIdFrom, size_t stringIdTo, Chk::Scope storageScope)
{
    auto edit = create_action(ActionDescriptor::MoveString);
    if ( storageScope == Chk::Scope::Game )
    {
        size_t stringIdMin = std::min(stringIdFrom, stringIdTo);
        size_t stringIdMax = std::max(stringIdFrom, stringIdTo);
        if ( stringIdMin > 0 && stringIdMax <= read.strings.size() && stringIdFrom != stringIdTo )
        {
            std::bitset<Chk::MaxStrings> stringIdUsed;
            markUsedStrings(stringIdUsed, Chk::Scope::Game);
            auto selected = read.strings[stringIdFrom];
            stringIdUsed[stringIdFrom] = false;
            std::map<u32, u32> stringIdRemappings;
            if ( stringIdTo < stringIdFrom ) // Move to a lower stringId, if there are strings in the way, cascade towards stringIdFrom
            {
                while ( stringIdUsed[stringIdTo] ) // There is a block of one or more strings where the selected string needs to go
                {
                    for ( size_t stringId = stringIdTo+1; stringId <= stringIdFrom; stringId ++ ) // Find the lowest available stringId past the block
                    {
                        if ( !stringIdUsed[stringId] ) // Move the highest stringId remaining in the block to the next available stringId
                        {
                            auto highestString = read.strings[stringId-1];
                            edit->strings[stringId-1] = std::nullopt;
                            stringIdUsed[stringId-1] = false;
                            edit->strings[stringId] = highestString;
                            stringIdUsed[stringId] = true;
                            stringIdRemappings.insert(std::pair<u32, u32>(u32(stringId-1), (u32)stringId));
                            break;
                        }
                    }
                }
                stringIdRemappings.insert(std::pair<u32, u32>((u32)stringIdFrom, (u32)stringIdTo));
            }
            else if ( stringIdTo > stringIdFrom ) // Move to a higher stringId, if there are strings in the way, cascade towards stringIdTo
            {
                while ( stringIdUsed[stringIdTo] ) // There is a block of one or more strings where the selected string needs to go
                {
                    for ( size_t stringId = stringIdTo-1; stringId >= stringIdFrom; stringId -- ) // Find the highest available stringId past the block
                    {
                        if ( !stringIdUsed[stringId] ) // Move the lowest stringId in the block to the available stringId
                        {
                            auto lowestString = read.strings[stringId+1];
                            edit->strings[stringId+1] = std::nullopt;
                            stringIdUsed[stringId+1] = false;
                            edit->strings[stringId] = lowestString;
                            stringIdUsed[stringId] = true;
                            stringIdRemappings.insert(std::pair<u32, u32>(u32(stringId+1), (u32)stringId));
                            break;
                        }
                    }
                }
            }
            edit->strings[stringIdTo] = selected;
            stringIdRemappings.insert(std::pair<u32, u32>((u32)stringIdFrom, (u32)stringIdTo));
            remapStringIds(stringIdRemappings, Chk::Scope::Game);
        }
    }
    else if ( storageScope == Chk::Scope::Editor )
    {
        size_t stringIdMin = std::min(stringIdFrom, stringIdTo);
        size_t stringIdMax = std::max(stringIdFrom, stringIdTo);
        if ( stringIdMin > 0 && stringIdMax <= read.editorStrings.size() && stringIdFrom != stringIdTo )
        {
            std::bitset<Chk::MaxStrings> stringIdUsed;
            markUsedStrings(stringIdUsed, Chk::Scope::Editor);
            auto selected = read.editorStrings[stringIdFrom];
            stringIdUsed[stringIdFrom] = false;
            std::map<u32, u32> stringIdRemappings;
            if ( stringIdTo < stringIdFrom ) // Move to a lower stringId, if there are strings in the way, cascade towards stringIdFrom
            {
                while ( stringIdUsed[stringIdTo] ) // There is a block of one or more strings where the selected string needs to go
                {
                    for ( size_t stringId = stringIdTo+1; stringId <= stringIdFrom; stringId ++ ) // Find the lowest available stringId past the block
                    {
                        if ( !stringIdUsed[stringId] ) // Move the highest stringId remaining in the block to the next available stringId
                        {
                            auto highestString = read.editorStrings[stringId-1];
                            edit->editorStrings[stringId-1] = std::nullopt;
                            stringIdUsed[stringId-1] = false;
                            edit->editorStrings[stringId] = highestString;
                            stringIdUsed[stringId] = true;
                            stringIdRemappings.insert(std::pair<u32, u32>(u32(stringId-1), (u32)stringId));
                            break;
                        }
                    }
                }
                stringIdRemappings.insert(std::pair<u32, u32>((u32)stringIdFrom, (u32)stringIdTo));
            }
            else if ( stringIdTo > stringIdFrom ) // Move to a higher stringId, if there are strings in the way, cascade towards stringIdTo
            {
                while ( stringIdUsed[stringIdTo] ) // There is a block of one or more strings where the selected string needs to go
                {
                    for ( size_t stringId = stringIdTo-1; stringId >= stringIdFrom; stringId -- ) // Find the highest available stringId past the block
                    {
                        if ( !stringIdUsed[stringId] ) // Move the lowest stringId in the block to the available stringId
                        {
                            auto lowestString = read.editorStrings[stringId+1];
                            edit->editorStrings[stringId+1] = std::nullopt;
                            stringIdUsed[stringId+1] = false;
                            edit->editorStrings[stringId] = lowestString;
                            stringIdUsed[stringId] = true;
                            stringIdRemappings.insert(std::pair<u32, u32>(u32(stringId+1), (u32)stringId));
                            break;
                        }
                    }
                }
            }
            edit->editorStrings[stringIdTo] = selected;
            stringIdRemappings.insert(std::pair<u32, u32>((u32)stringIdFrom, (u32)stringIdTo));
            remapStringIds(stringIdRemappings, Chk::Scope::Editor);
        }
    }
}

size_t Scenario::rescopeString(size_t stringId, Chk::Scope changeStorageScopeTo, bool autoDefragment)
{
    auto edit = create_action(ActionDescriptor::RescopeString);
    if ( changeStorageScopeTo == Chk::Scope::Editor && stringUsed(stringId, Chk::Scope::Either, Chk::Scope::Game, Chk::StringUserFlag::All, true) )
    {
        RawString toRescope = getString<RawString>(stringId, Chk::Scope::Game).value();
        size_t newStringId = addString<RawString>(toRescope, Chk::Scope::Editor, autoDefragment);
        if ( newStringId != 0 )
        {
            std::set<u32> stringIdsReplaced;
            if ( stringId == read.scenarioProperties.scenarioNameStringId )
            {
                stringIdsReplaced.insert(read.editorStringOverrides.scenarioName);
                edit->editorStringOverrides.scenarioName = u32(newStringId);
            }
            if ( stringId == read.scenarioProperties.scenarioDescriptionStringId )
            {
                stringIdsReplaced.insert(read.editorStringOverrides.scenarioDescription);
                edit->editorStringOverrides.scenarioDescription = u32(newStringId);
            }
            for ( size_t i=0; i<Chk::TotalForces; i++ )
            {
                if ( stringId == read.forces.forceString[i] )
                {
                    stringIdsReplaced.insert(read.editorStringOverrides.forceName[i]);
                    edit->editorStringOverrides.forceName[i] = u32(newStringId);
                }
            }
            for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
            {
                if ( stringId == read.origUnitSettings.nameStringId[i] )
                {
                    stringIdsReplaced.insert(read.editorStringOverrides.unitName[i]);
                    edit->editorStringOverrides.unitName[i] = u32(newStringId);
                }
            }
            for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
            {
                if ( stringId == read.unitSettings.nameStringId[i] )
                {
                    stringIdsReplaced.insert(read.editorStringOverrides.expUnitName[i]);
                    edit->editorStringOverrides.expUnitName[i] = u32(newStringId);
                }
            }
            for ( size_t i=0; i<Chk::TotalSounds; i++ )
            {
                if ( stringId == read.soundPaths[i] )
                {
                    stringIdsReplaced.insert(read.editorStringOverrides.soundPath[i]);
                    edit->editorStringOverrides.soundPath[i] = u32(newStringId);
                }
            }
            for ( size_t i=0; i<Chk::TotalSwitches; i++ )
            {
                if ( stringId == read.switchNames[i] )
                {
                    stringIdsReplaced.insert(read.editorStringOverrides.switchName[i]);
                    edit->editorStringOverrides.switchName[i] = u32(newStringId);
                }
            }
            size_t numLocations = this->numLocations();
            for ( size_t i=1; i<=numLocations; i++ )
            {
                if ( stringId == read.locations[i].stringId )
                {
                    stringIdsReplaced.insert(read.editorStringOverrides.locationName[i]);
                    edit->editorStringOverrides.locationName[i] = u32(newStringId);
                }
            }

            deleteString(stringId, Chk::Scope::Game, false);
            for ( auto stringIdReplaced : stringIdsReplaced )
                deleteString(stringIdReplaced, Chk::Scope::Editor, true);
        }
    }
    else if ( changeStorageScopeTo == Chk::Scope::Game && stringUsed(stringId, Chk::Scope::Either, Chk::Scope::Editor, Chk::StringUserFlag::All, true) )
    {
        RawString toRescope = getString<RawString>(stringId, Chk::Scope::Editor).value();
        size_t newStringId = addString<RawString>(toRescope, Chk::Scope::Game, autoDefragment);
        if ( newStringId != 0 )
        {
            std::set<u32> stringIdsReplaced;
            if ( stringId == read.editorStringOverrides.scenarioName )
            {
                stringIdsReplaced.insert((u32)read.scenarioProperties.scenarioNameStringId);
                edit->scenarioProperties.scenarioNameStringId = u16(newStringId);
            }
            if ( stringId == read.editorStringOverrides.scenarioDescription )
            {
                stringIdsReplaced.insert((u32)read.scenarioProperties.scenarioDescriptionStringId);
                edit->scenarioProperties.scenarioDescriptionStringId = u16(newStringId);
            }
            for ( size_t i=0; i<Chk::TotalForces; i++ )
            {
                if ( stringId == read.editorStringOverrides.forceName[i] )
                {
                    stringIdsReplaced.insert(u32(read.forces.forceString[i]));
                    edit->forces.forceString[i] = u16(newStringId);
                }
            }
            for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
            {
                if ( stringId == read.editorStringOverrides.unitName[i] )
                {
                    stringIdsReplaced.insert(u32(read.origUnitSettings.nameStringId[i]));
                    edit->origUnitSettings.nameStringId[i] = u16(newStringId);
                }
            }
            for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
            {
                if ( stringId == read.editorStringOverrides.expUnitName[i] )
                {
                    stringIdsReplaced.insert(u32(read.unitSettings.nameStringId[i]));
                    edit->unitSettings.nameStringId[i] = u16(newStringId);
                }
            }
            for ( size_t i=0; i<Chk::TotalSounds; i++ )
            {
                if ( stringId == read.editorStringOverrides.soundPath[i] )
                {
                    stringIdsReplaced.insert((u32)read.soundPaths[i]);
                    edit->soundPaths[i] = u32(newStringId);
                }
            }
            for ( size_t i=0; i<Chk::TotalSwitches; i++ )
            {
                if ( stringId == read.editorStringOverrides.switchName[i] )
                {
                    stringIdsReplaced.insert((u32)read.switchNames[i]);
                    edit->switchNames[i] = u32(newStringId);
                }
            }
            size_t numLocations = this->numLocations();
            for ( size_t i=1; i<=numLocations; i++ )
            {
                if ( stringId == read.editorStringOverrides.locationName[i] )
                {
                    stringIdsReplaced.insert(read.locations[i].stringId);
                    edit->locations[i].stringId = (u16)newStringId;
                }
            }

            deleteString(stringId, Chk::Scope::Editor, false);
            for ( auto stringIdReplaced : stringIdsReplaced )
                deleteString(stringIdReplaced, Chk::Scope::Game, true);
        }
    }
    return 0;
}

const std::vector<u8> & Scenario::getStrTailData() const
{
    return read.strTailData;
}

size_t Scenario::getStrTailDataOffset()
{
    std::vector<u8> stringBytes {};
    if ( hasSection(SectionName::STRx) )
        syncRemasteredStringsToBytes(stringBytes);
    else
        syncStringsToBytes(stringBytes);

    return stringBytes.size();
}

size_t Scenario::getScenarioNameStringId(Chk::Scope storageScope) const
{
    return storageScope == Chk::Scope::Editor ? read.editorStringOverrides.scenarioName : read.scenarioProperties.scenarioNameStringId;
}

size_t Scenario::getScenarioDescriptionStringId(Chk::Scope storageScope) const
{
    return storageScope == Chk::Scope::Editor ? read.editorStringOverrides.scenarioDescription : read.scenarioProperties.scenarioDescriptionStringId;
}

size_t Scenario::getForceNameStringId(Chk::Force force, Chk::Scope storageScope) const
{
    return storageScope == Chk::Scope::Editor ? read.editorStringOverrides.forceName[force] : read.forces.forceString[force];;
}

size_t Scenario::getUnitNameStringId(Sc::Unit::Type unitType, Chk::UseExpSection useExp, Chk::Scope storageScope) const
{
    if ( unitType >= Sc::Unit::TotalTypes )
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS/UNIx section!");

    if ( storageScope == Chk::Scope::Game )
        return useExpansionUnitSettings(useExp) ? read.unitSettings.nameStringId[unitType] : read.origUnitSettings.nameStringId[unitType];
    else if ( storageScope == Chk::Scope::Editor )
    {
        switch ( useExp )
        {
            case Chk::UseExpSection::Auto: return isHybridOrAbove() ? read.editorStringOverrides.expUnitName[unitType] : read.editorStringOverrides.unitName[unitType];
            case Chk::UseExpSection::Yes: return read.editorStringOverrides.expUnitName[unitType];
            case Chk::UseExpSection::No: return read.editorStringOverrides.unitName[unitType];
            case Chk::UseExpSection::YesIfAvailable: return read.editorStringOverrides.expUnitName[unitType] != 0 ? read.editorStringOverrides.expUnitName[unitType] : read.editorStringOverrides.unitName[unitType];
            case Chk::UseExpSection::NoIfOrigAvailable: return read.editorStringOverrides.unitName[unitType] != 0 ? read.editorStringOverrides.unitName[unitType] : read.editorStringOverrides.expUnitName[unitType];
            default: break;
        }
    }
    return 0;
}

size_t Scenario::getSoundPathStringId(size_t soundIndex, Chk::Scope storageScope) const
{
    return storageScope == Chk::Scope::Editor ? read.editorStringOverrides.soundPath[soundIndex] : read.soundPaths[soundIndex];
}

size_t Scenario::getSwitchNameStringId(size_t switchIndex, Chk::Scope storageScope) const
{
    if ( switchIndex < Chk::TotalSwitches )
    {
        if ( storageScope == Chk::Scope::Game )
            return read.switchNames[switchIndex];
        else
            return read.editorStringOverrides.switchName[switchIndex];
    }
    else
        throw std::out_of_range(std::string("switchIndex: ") + std::to_string((u32)switchIndex) + " is out of range for the SWNM section!");
}

size_t Scenario::getLocationNameStringId(size_t locationId, Chk::Scope storageScope) const
{
    if ( storageScope == Chk::Scope::Editor )
        return read.editorStringOverrides.locationName[locationId];
    else if ( locationId < numLocations() )
        return read.locations[locationId].stringId;
    else
        return 0;
}

void Scenario::setScenarioNameStringId(size_t scenarioNameStringId, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Editor )
        create_action(ActionDescriptor::SetScenarioName)->editorStringOverrides.scenarioName = u32(scenarioNameStringId);
    else
        create_action(ActionDescriptor::SetScenarioName)->scenarioProperties.scenarioNameStringId = u16(scenarioNameStringId);
}

void Scenario::setScenarioDescriptionStringId(size_t scenarioDescriptionStringId, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Editor )
        create_action(ActionDescriptor::SetScenarioDescription)->editorStringOverrides.scenarioDescription = u32(scenarioDescriptionStringId);
    else
        create_action(ActionDescriptor::SetScenarioDescription)->scenarioProperties.scenarioDescriptionStringId = u16(scenarioDescriptionStringId);
}

void Scenario::setForceNameStringId(Chk::Force force, size_t forceNameStringId, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Editor )
        create_action(ActionDescriptor::UpdateForceName)->editorStringOverrides.forceName[force] = u32(forceNameStringId);
    else
        create_action(ActionDescriptor::UpdateForceName)->forces.forceString[force] = u16(forceNameStringId);
}

void Scenario::setUnitNameStringId(Sc::Unit::Type unitType, size_t unitNameStringId, Chk::UseExpSection useExp, Chk::Scope storageScope)
{
    auto edit = create_action(ActionDescriptor::UpdateUnitName);
    if ( unitType >= Sc::Unit::TotalTypes )
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS/UNIx section!");

    if ( storageScope == Chk::Scope::Game )
    {
        switch ( useExp )
        {
            case Chk::UseExpSection::Auto:
            case Chk::UseExpSection::Both: edit->unitSettings.nameStringId[unitType] = u16(unitNameStringId); edit->origUnitSettings.nameStringId[unitType] = u16(unitNameStringId); break;
            case Chk::UseExpSection::Yes: edit->unitSettings.nameStringId[unitType] = u16(unitNameStringId); break;
            case Chk::UseExpSection::No: edit->origUnitSettings.nameStringId[unitType] = u16(unitNameStringId); break;
            case Chk::UseExpSection::YesIfAvailable:
                if ( this->hasSection(Chk::SectionName::UNIx) )
                    edit->unitSettings.nameStringId[unitType] = u16(unitNameStringId);
                else
                    edit->origUnitSettings.nameStringId[unitType] = u16(unitNameStringId);
                break;
            case Chk::UseExpSection::NoIfOrigAvailable:
                if ( this->hasSection(Chk::SectionName::UNIS) )
                    edit->origUnitSettings.nameStringId[unitType] = u16(unitNameStringId);
                else
                    edit->unitSettings.nameStringId[unitType] = u16(unitNameStringId);
                break;
        }
    }
    else
    {
        switch ( useExp )
        {
            case Chk::UseExpSection::Auto:
            case Chk::UseExpSection::Both: edit->editorStringOverrides.unitName[unitType] = u32(unitNameStringId); edit->editorStringOverrides.expUnitName[unitType] = u32(unitNameStringId); break;
            case Chk::UseExpSection::YesIfAvailable:
            case Chk::UseExpSection::Yes: edit->editorStringOverrides.expUnitName[unitType] = u32(unitNameStringId); break;
            case Chk::UseExpSection::NoIfOrigAvailable:
            case Chk::UseExpSection::No: edit->editorStringOverrides.unitName[unitType] = u32(unitNameStringId); break;
        }
    }
}

void Scenario::setSoundPathStringId(size_t soundIndex, size_t soundPathStringId, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Editor )
        create_action()->editorStringOverrides.soundPath[soundIndex] = u32(soundPathStringId);
    else
        create_action()->soundPaths[soundIndex] = u32(soundPathStringId);
}

void Scenario::setSwitchNameStringId(size_t switchIndex, size_t switchNameStringId, Chk::Scope storageScope)
{
    if ( switchIndex < Chk::TotalSwitches )
    {
        if ( storageScope == Chk::Scope::Game )
            create_action(ActionDescriptor::SetSwitchName)->switchNames[switchIndex] = u32(switchNameStringId);
        else
            create_action(ActionDescriptor::SetSwitchName)->editorStringOverrides.switchName[switchIndex] = u32(switchNameStringId);
    }
    else
        throw std::out_of_range(std::string("switchIndex: ") + std::to_string((u32)switchIndex) + " is out of range for the SWNM section!");
}

void Scenario::setLocationNameStringId(size_t locationId, size_t locationNameStringId, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Editor )
        create_action(ActionDescriptor::UpdateLocationName)->editorStringOverrides.locationName[locationId] = u32(locationNameStringId);
    else if ( locationId < numLocations() )
        create_action(ActionDescriptor::UpdateLocationName)->locations[locationId].stringId = u16(locationNameStringId);
}

template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
std::optional<StringType> Scenario::getString(size_t gameStringId, size_t editorStringId, Chk::Scope storageScope) const
{
    switch ( storageScope )
    {
        case Chk::Scope::Game: return getString<StringType>(gameStringId, Chk::Scope::Game);
        case Chk::Scope::Editor: return getString<StringType>(editorStringId, Chk::Scope::Editor);
        case Chk::Scope::GameOverEditor: return gameStringId != 0 ? getString<StringType>(gameStringId, Chk::Scope::Game) : getString<StringType>(editorStringId, Chk::Scope::Editor);
        case Chk::Scope::Either:
        case Chk::Scope::EditorOverGame: return editorStringId != 0 ? getString<StringType>(editorStringId, Chk::Scope::Editor) : getString<StringType>(gameStringId, Chk::Scope::Game);
        default: break;
    }
    return std::nullopt;
}
template std::optional<RawString> Scenario::getString<RawString>(size_t gameStringId, size_t editorStringId, Chk::Scope storageScope) const;
template std::optional<EscString> Scenario::getString<EscString>(size_t gameStringId, size_t editorStringId, Chk::Scope storageScope) const;
template std::optional<ChkdString> Scenario::getString<ChkdString>(size_t gameStringId, size_t editorStringId, Chk::Scope storageScope) const;
template std::optional<SingleLineChkdString> Scenario::getString<SingleLineChkdString>(size_t gameStringId, size_t editorStringId, Chk::Scope storageScope) const;

template <typename StringType>
std::optional<StringType> Scenario::getScenarioName(Chk::Scope storageScope) const
{
    return getString<StringType>(read.scenarioProperties.scenarioNameStringId, read.editorStringOverrides.scenarioName, storageScope);
}
template std::optional<RawString> Scenario::getScenarioName<RawString>(Chk::Scope storageScope) const;
template std::optional<EscString> Scenario::getScenarioName<EscString>(Chk::Scope storageScope) const;
template std::optional<ChkdString> Scenario::getScenarioName<ChkdString>(Chk::Scope storageScope) const;
template std::optional<SingleLineChkdString> Scenario::getScenarioName<SingleLineChkdString>(Chk::Scope storageScope) const;

template <typename StringType>
std::optional<StringType> Scenario::getScenarioDescription(Chk::Scope storageScope) const
{
    return getString<StringType>(read.scenarioProperties.scenarioDescriptionStringId, read.editorStringOverrides.scenarioDescription, storageScope);
}
template std::optional<RawString> Scenario::getScenarioDescription<RawString>(Chk::Scope storageScope) const;
template std::optional<EscString> Scenario::getScenarioDescription<EscString>(Chk::Scope storageScope) const;
template std::optional<ChkdString> Scenario::getScenarioDescription<ChkdString>(Chk::Scope storageScope) const;
template std::optional<SingleLineChkdString> Scenario::getScenarioDescription<SingleLineChkdString>(Chk::Scope storageScope) const;

template <typename StringType>
std::optional<StringType> Scenario::getForceName(Chk::Force force, Chk::Scope storageScope) const
{
    return getString<StringType>(read.forces.forceString[force], read.editorStringOverrides.forceName[force], storageScope);
}
template std::optional<RawString> Scenario::getForceName<RawString>(Chk::Force force, Chk::Scope storageScope) const;
template std::optional<EscString> Scenario::getForceName<EscString>(Chk::Force force, Chk::Scope storageScope) const;
template std::optional<ChkdString> Scenario::getForceName<ChkdString>(Chk::Force force, Chk::Scope storageScope) const;
template std::optional<SingleLineChkdString> Scenario::getForceName<SingleLineChkdString>(Chk::Force force, Chk::Scope storageScope) const;

template <typename StringType>
std::optional<StringType> Scenario::getUnitName(Sc::Unit::Type unitType, const Sc::Data* scData, bool defaultIfNull, Chk::UseExpSection useExp, Chk::Scope storageScope) const
{
    auto mapUnitName = unitType < Sc::Unit::TotalTypes ? getString<StringType>(
        this->useExpansionUnitSettings(useExp) ? read.unitSettings.nameStringId[unitType] : read.origUnitSettings.nameStringId[unitType],
        this->useExpansionUnitSettings(useExp) ? read.editorStringOverrides.expUnitName[unitType] : read.editorStringOverrides.unitName[unitType],
        storageScope) : std::nullopt;

    if ( mapUnitName )
        return mapUnitName;
    else if ( scData != nullptr && unitType < scData->units.displayNames.size() )
        return std::optional<StringType>(scData->units.displayNames[unitType]);
    else if ( unitType < Sc::Unit::TotalTypes )
        return std::optional<StringType>(Sc::Unit::defaultDisplayNames[unitType]);
    else
        return std::optional<StringType>("ID:" + std::to_string(unitType));
}
template std::optional<RawString> Scenario::getUnitName<RawString>(Sc::Unit::Type unitTyp, const Sc::Data* scDatae, bool defaultIfNull, Chk::UseExpSection useExp, Chk::Scope storageScope) const;
template std::optional<EscString> Scenario::getUnitName<EscString>(Sc::Unit::Type unitType, const Sc::Data* scData, bool defaultIfNull, Chk::UseExpSection useExp, Chk::Scope storageScope) const;
template std::optional<ChkdString> Scenario::getUnitName<ChkdString>(Sc::Unit::Type unitType, const Sc::Data* scData, bool defaultIfNull, Chk::UseExpSection useExp, Chk::Scope storageScope) const;
template std::optional<SingleLineChkdString> Scenario::getUnitName<SingleLineChkdString>(Sc::Unit::Type unitType, const Sc::Data* scData, bool defaultIfNull, Chk::UseExpSection useExp, Chk::Scope storageScope) const;

template <typename StringType>
std::optional<StringType> Scenario::getSoundPath(size_t soundIndex, Chk::Scope storageScope) const
{
    return getString<StringType>(read.soundPaths[soundIndex], read.editorStringOverrides.soundPath[soundIndex], storageScope);
}
template std::optional<RawString> Scenario::getSoundPath<RawString>(size_t soundIndex, Chk::Scope storageScope) const;
template std::optional<EscString> Scenario::getSoundPath<EscString>(size_t soundIndex, Chk::Scope storageScope) const;
template std::optional<ChkdString> Scenario::getSoundPath<ChkdString>(size_t soundIndex, Chk::Scope storageScope) const;
template std::optional<SingleLineChkdString> Scenario::getSoundPath<SingleLineChkdString>(size_t soundIndex, Chk::Scope storageScope) const;

template <typename StringType>
std::optional<StringType> Scenario::getSwitchName(size_t switchIndex, Chk::Scope storageScope) const
{
    return getString<StringType>(read.switchNames[switchIndex], read.editorStringOverrides.switchName[switchIndex], storageScope);
}
template std::optional<RawString> Scenario::getSwitchName<RawString>(size_t switchIndex, Chk::Scope storageScope) const;
template std::optional<EscString> Scenario::getSwitchName<EscString>(size_t switchIndex, Chk::Scope storageScope) const;
template std::optional<ChkdString> Scenario::getSwitchName<ChkdString>(size_t switchIndex, Chk::Scope storageScope) const;
template std::optional<SingleLineChkdString> Scenario::getSwitchName<SingleLineChkdString>(size_t switchIndex, Chk::Scope storageScope) const;

template <typename StringType>
std::optional<StringType> Scenario::getLocationName(size_t locationId, Chk::Scope storageScope) const
{
    return getString<StringType>((locationId > 0 && locationId <= numLocations() ? getLocation(locationId).stringId : 0), read.editorStringOverrides.locationName[locationId], storageScope);
}
template std::optional<RawString> Scenario::getLocationName<RawString>(size_t locationId, Chk::Scope storageScope) const;
template std::optional<EscString> Scenario::getLocationName<EscString>(size_t locationId, Chk::Scope storageScope) const;
template std::optional<ChkdString> Scenario::getLocationName<ChkdString>(size_t locationId, Chk::Scope storageScope) const;
template std::optional<SingleLineChkdString> Scenario::getLocationName<SingleLineChkdString>(size_t locationId, Chk::Scope storageScope) const;

template <typename StringType>
std::optional<StringType> Scenario::getComment(size_t triggerIndex) const
{
    return getString<StringType>(getCommentStringId(triggerIndex), Chk::Scope::Game);
}
template std::optional<RawString> Scenario::getComment<RawString>(size_t triggerIndex) const;
template std::optional<EscString> Scenario::getComment<EscString>(size_t triggerIndex) const;
template std::optional<ChkdString> Scenario::getComment<ChkdString>(size_t triggerIndex) const;
template std::optional<SingleLineChkdString> Scenario::getComment<SingleLineChkdString>(size_t triggerIndex) const;

template <typename StringType>
std::optional<StringType> Scenario::getExtendedComment(size_t triggerIndex) const
{
    return getString<StringType>(getExtendedCommentStringId(triggerIndex), Chk::Scope::Editor);
}
template std::optional<RawString> Scenario::getExtendedComment<RawString>(size_t triggerIndex) const;
template std::optional<EscString> Scenario::getExtendedComment<EscString>(size_t triggerIndex) const;
template std::optional<ChkdString> Scenario::getExtendedComment<ChkdString>(size_t triggerIndex) const;
template std::optional<SingleLineChkdString> Scenario::getExtendedComment<SingleLineChkdString>(size_t triggerIndex) const;

template <typename StringType>
std::optional<StringType> Scenario::getExtendedNotes(size_t triggerIndex) const
{
    return getString<StringType>(getExtendedNotesStringId(triggerIndex), Chk::Scope::Editor);
}
template std::optional<RawString> Scenario::getExtendedNotes<RawString>(size_t triggerIndex) const;
template std::optional<EscString> Scenario::getExtendedNotes<EscString>(size_t triggerIndex) const;
template std::optional<ChkdString> Scenario::getExtendedNotes<ChkdString>(size_t triggerIndex) const;
template std::optional<SingleLineChkdString> Scenario::getExtendedNotes<SingleLineChkdString>(size_t triggerIndex) const;

template <typename StringType>
void Scenario::setScenarioName(const StringType & scenarioNameString, Chk::Scope storageScope, bool autoDefragment)
{
    if ( storageScope == Chk::Scope::Game || storageScope == Chk::Scope::Editor )
    {
        size_t newStringId = addString<StringType>(scenarioNameString, storageScope, autoDefragment);
        if ( newStringId != (size_t)Chk::StringId::NoString )
        {
            if ( storageScope == Chk::Scope::Game )
                create_action(ActionDescriptor::SetScenarioName)->scenarioProperties.scenarioNameStringId = u16(newStringId);
            else if ( storageScope == Chk::Scope::Editor )
                create_action(ActionDescriptor::SetScenarioName)->editorStringOverrides.scenarioName = u32(newStringId);
        }
    }
}
template void Scenario::setScenarioName<RawString>(const RawString & scenarioNameString, Chk::Scope storageScope, bool autoDefragment);
template void Scenario::setScenarioName<EscString>(const EscString & scenarioNameString, Chk::Scope storageScope, bool autoDefragment);
template void Scenario::setScenarioName<ChkdString>(const ChkdString & scenarioNameString, Chk::Scope storageScope, bool autoDefragment);
template void Scenario::setScenarioName<SingleLineChkdString>(const SingleLineChkdString & scenarioNameString, Chk::Scope storageScope, bool autoDefragment);

template <typename StringType>
void Scenario::setScenarioDescription(const StringType & scenarioDescription, Chk::Scope storageScope, bool autoDefragment)
{
    if ( storageScope == Chk::Scope::Game || storageScope == Chk::Scope::Editor )
    {
        size_t newStringId = addString<StringType>(scenarioDescription, storageScope, autoDefragment);
        if ( newStringId != (size_t)Chk::StringId::NoString )
        {
            if ( storageScope == Chk::Scope::Game )
                create_action(ActionDescriptor::SetScenarioDescription)->scenarioProperties.scenarioDescriptionStringId = u16(newStringId);
            else if ( storageScope == Chk::Scope::Editor )
                create_action(ActionDescriptor::SetScenarioDescription)->editorStringOverrides.scenarioDescription = u32(newStringId);
        }
    }
}
template void Scenario::setScenarioDescription<RawString>(const RawString & scenarioNameString, Chk::Scope storageScope, bool autoDefragment);
template void Scenario::setScenarioDescription<EscString>(const EscString & scenarioNameString, Chk::Scope storageScope, bool autoDefragment);
template void Scenario::setScenarioDescription<ChkdString>(const ChkdString & scenarioNameString, Chk::Scope storageScope, bool autoDefragment);
template void Scenario::setScenarioDescription<SingleLineChkdString>(const SingleLineChkdString & scenarioNameString, Chk::Scope storageScope, bool autoDefragment);

template <typename StringType>
void Scenario::setForceName(Chk::Force force, const StringType & forceName, Chk::Scope storageScope, bool autoDefragment)
{
    if ( (storageScope == Chk::Scope::Game || storageScope == Chk::Scope::Editor) && (u32)force < Chk::TotalForces )
    {
        size_t newStringId = addString<StringType>(forceName, storageScope, autoDefragment);
        if ( newStringId != (size_t)Chk::StringId::NoString )
        {
            if ( storageScope == Chk::Scope::Game && read.forces.forceString[force] != u16(newStringId) )
                create_action(ActionDescriptor::UpdateForceName)->forces.forceString[force] = u16(newStringId);
            else if ( storageScope == Chk::Scope::Editor && read.editorStringOverrides.forceName[force] != u32(newStringId) )
                create_action(ActionDescriptor::UpdateForceName)->editorStringOverrides.forceName[force] = u32(newStringId);
        }
    }
}
template void Scenario::setForceName<RawString>(Chk::Force force, const RawString & forceName, Chk::Scope storageScope, bool autoDefragment);
template void Scenario::setForceName<EscString>(Chk::Force force, const EscString & forceName, Chk::Scope storageScope, bool autoDefragment);
template void Scenario::setForceName<ChkdString>(Chk::Force force, const ChkdString & forceName, Chk::Scope storageScope, bool autoDefragment);
template void Scenario::setForceName<SingleLineChkdString>(Chk::Force force, const SingleLineChkdString & forceName, Chk::Scope storageScope, bool autoDefragment);

template <typename StringType>
void Scenario::setUnitName(Sc::Unit::Type unitType, const StringType & unitName, Chk::UseExpSection useExp, Chk::Scope storageScope, bool autoDefragment)
{
    if ( (storageScope == Chk::Scope::Game || storageScope == Chk::Scope::Editor) && unitType < Sc::Unit::TotalTypes )
    {
        size_t newStringId = addString<StringType>(unitName, storageScope, autoDefragment);
        if ( newStringId != (size_t)Chk::StringId::NoString )
        {
            auto edit = create_action(ActionDescriptor::UpdateUnitName);
            if ( storageScope == Chk::Scope::Game )
            {
                if ( this->useExpansionUnitSettings(useExp) )
                    edit->unitSettings.nameStringId[unitType] = u16(newStringId);
                else
                    edit->origUnitSettings.nameStringId[unitType] = u16(newStringId);
            }
            else if ( storageScope == Chk::Scope::Editor )
            {
                switch ( useExp )
                {
                    case Chk::UseExpSection::Auto:
                        if ( this->isHybridOrAbove() )
                            edit->editorStringOverrides.expUnitName[unitType] = u32(newStringId);
                        else
                            edit->editorStringOverrides.unitName[unitType] = u32(newStringId);
                        break;
                    case Chk::UseExpSection::Yes: edit->editorStringOverrides.expUnitName[unitType] = u32(newStringId); break;
                    case Chk::UseExpSection::No: edit->editorStringOverrides.unitName[unitType] = u32(newStringId); break;
                    default:
                        edit->editorStringOverrides.unitName[unitType] = u32(newStringId);
                        edit->editorStringOverrides.expUnitName[unitType] = u32(newStringId);
                        break;
                }
            }
        }
    }
}
template void Scenario::setUnitName<RawString>(Sc::Unit::Type unitType, const RawString & unitName, Chk::UseExpSection useExp, Chk::Scope storageScope, bool autoDefragment);
template void Scenario::setUnitName<EscString>(Sc::Unit::Type unitType, const EscString & unitName, Chk::UseExpSection useExp, Chk::Scope storageScope, bool autoDefragment);
template void Scenario::setUnitName<ChkdString>(Sc::Unit::Type unitType, const ChkdString & unitName, Chk::UseExpSection useExp, Chk::Scope storageScope, bool autoDefragment);
template void Scenario::setUnitName<SingleLineChkdString>(Sc::Unit::Type unitType, const SingleLineChkdString & unitName, Chk::UseExpSection useExp, Chk::Scope storageScope, bool autoDefragment);

template <typename StringType>
void Scenario::setSoundPath(size_t soundIndex, const StringType & soundPath, Chk::Scope storageScope, bool autoDefragment)
{
    if ( storageScope == Chk::Scope::Game || storageScope == Chk::Scope::Editor && soundIndex < Chk::TotalSounds )
    {
        size_t newStringId = addString<StringType>(soundPath, storageScope, autoDefragment);
        if ( newStringId != (size_t)Chk::StringId::NoString )
        {
            if ( storageScope == Chk::Scope::Game )
                setSoundStringId(soundIndex, newStringId);
            else if ( storageScope == Chk::Scope::Editor )
                create_action(ActionDescriptor::AddSound)->editorStringOverrides.soundPath[soundIndex] = u32(newStringId);
        }
    }
}
template void Scenario::setSoundPath<RawString>(size_t soundIndex, const RawString & soundPath, Chk::Scope storageScope, bool autoDefragment);
template void Scenario::setSoundPath<EscString>(size_t soundIndex, const EscString & soundPath, Chk::Scope storageScope, bool autoDefragment);
template void Scenario::setSoundPath<ChkdString>(size_t soundIndex, const ChkdString & soundPath, Chk::Scope storageScope, bool autoDefragment);
template void Scenario::setSoundPath<SingleLineChkdString>(size_t soundIndex, const SingleLineChkdString & soundPath, Chk::Scope storageScope, bool autoDefragment);

template <typename StringType>
void Scenario::setSwitchName(size_t switchIndex, const StringType & switchName, Chk::Scope storageScope, bool autoDefragment)
{
    if ( storageScope == Chk::Scope::Game || storageScope == Chk::Scope::Editor && switchIndex < Chk::TotalSwitches )
    {
        size_t newStringId = addString<StringType>(switchName, storageScope, autoDefragment);
        if ( newStringId != (size_t)Chk::StringId::NoString )
        {
            if ( storageScope == Chk::Scope::Game )
                create_action(ActionDescriptor::SetSwitchName)->switchNames[switchIndex] = u32(newStringId);
            else if ( storageScope == Chk::Scope::Editor )
                create_action(ActionDescriptor::SetSwitchName)->editorStringOverrides.switchName[switchIndex] = u32(newStringId);
        }
    }
}
template void Scenario::setSwitchName<RawString>(size_t switchIndex, const RawString & switchName, Chk::Scope storageScope, bool autoDefragment);
template void Scenario::setSwitchName<EscString>(size_t switchIndex, const EscString & switchName, Chk::Scope storageScope, bool autoDefragment);
template void Scenario::setSwitchName<ChkdString>(size_t switchIndex, const ChkdString & switchName, Chk::Scope storageScope, bool autoDefragment);
template void Scenario::setSwitchName<SingleLineChkdString>(size_t switchIndex, const SingleLineChkdString & switchName, Chk::Scope storageScope, bool autoDefragment);

template <typename StringType>
void Scenario::setLocationName(size_t locationId, const StringType & locationName, Chk::Scope storageScope, bool autoDefragment)
{
    if ( storageScope == Chk::Scope::Game || storageScope == Chk::Scope::Editor && locationId > 0 && locationId <= numLocations() )
    {
        size_t newStringId = addString<StringType>(locationName, storageScope, autoDefragment);
        if ( newStringId != (size_t)Chk::StringId::NoString )
        {
            if ( storageScope == Chk::Scope::Game )
                create_action(ActionDescriptor::UpdateLocationName)->locations[locationId].stringId = (u16)newStringId;
            else if ( storageScope == Chk::Scope::Editor )
                create_action(ActionDescriptor::UpdateLocationName)->editorStringOverrides.locationName[locationId] = u32(newStringId);
        }
    }
}
template void Scenario::setLocationName<RawString>(size_t locationId, const RawString & locationName, Chk::Scope storageScope, bool autoDefragment);
template void Scenario::setLocationName<EscString>(size_t locationId, const EscString & locationName, Chk::Scope storageScope, bool autoDefragment);
template void Scenario::setLocationName<ChkdString>(size_t locationId, const ChkdString & locationName, Chk::Scope storageScope, bool autoDefragment);
template void Scenario::setLocationName<SingleLineChkdString>(size_t locationId, const SingleLineChkdString & locationName, Chk::Scope storageScope, bool autoDefragment);

template <typename StringType>
void Scenario::setExtendedComment(size_t triggerIndex, const StringType & comment, bool autoDefragment)
{
    auto edit = create_action(ActionDescriptor::UpdateTriggerComment);
    auto extensionIndex = getTriggerExtension(triggerIndex, true);
    size_t newStringId = addString<StringType>(comment, Chk::Scope::Editor, autoDefragment);
    if ( newStringId != (size_t)Chk::StringId::NoString )
        edit->triggerExtensions[extensionIndex].commentStringId = (u32)newStringId;
}
template void Scenario::setExtendedComment<RawString>(size_t triggerIndex, const RawString & comment, bool autoDefragment);
template void Scenario::setExtendedComment<EscString>(size_t triggerIndex, const EscString & comment, bool autoDefragment);
template void Scenario::setExtendedComment<ChkdString>(size_t triggerIndex, const ChkdString & comment, bool autoDefragment);
template void Scenario::setExtendedComment<SingleLineChkdString>(size_t triggerIndex, const SingleLineChkdString & comment, bool autoDefragment);

template <typename StringType>
void Scenario::setExtendedNotes(size_t triggerIndex, const StringType & notes, bool autoDefragment)
{
    auto edit = create_action(ActionDescriptor::UpdateTriggerNotes);
    auto extensionIndex = getTriggerExtension(triggerIndex, true);
    size_t newStringId = addString<StringType>(notes, Chk::Scope::Editor, autoDefragment);
    if ( newStringId != (size_t)Chk::StringId::NoString )
        edit->triggerExtensions[extensionIndex].notesStringId = (u32)newStringId;
}
template void Scenario::setExtendedNotes<RawString>(size_t triggerIndex, const RawString & notes, bool autoDefragment);
template void Scenario::setExtendedNotes<EscString>(size_t triggerIndex, const EscString & notes, bool autoDefragment);
template void Scenario::setExtendedNotes<ChkdString>(size_t triggerIndex, const ChkdString & notes, bool autoDefragment);
template void Scenario::setExtendedNotes<SingleLineChkdString>(size_t triggerIndex, const SingleLineChkdString & notes, bool autoDefragment);

void Scenario::syncStringsToBytes(std::vector<u8> & stringBytes, u32 requestedCompressionFlags, u32 allowedCompressionFlags)
{
    /**
        Uses the basic, staredit standard, STR section format, and not allowing section sizes over 65536

        u16 numStrings;
        u16 stringOffsets[numStrings]; // Offset of the start of the string within the section
        void[] stringData; // Character data, first comes initial NUL character... then all strings, in order, each NUL terminated
    */
    
    constexpr size_t maxStrings = (size_t(u16_max) - sizeof(u16))/sizeof(u16);
    size_t numStrings = read.strings.size() > 0 ? read.strings.size()-1 : 0; // Exclude string at index 0
    if ( numStrings > maxStrings )
        throw Chk::MaximumStringsExceeded(Chk::getNameString(SectionName::STR), numStrings, maxStrings);
        
    size_t sizeAndOffsetSpaceAndNulSpace = sizeof(u16) + sizeof(u16)*numStrings + 1;
    size_t sectionSize = sizeAndOffsetSpaceAndNulSpace;
    for ( size_t i=1; i<=numStrings; i++ )
    {
        if ( read.strings[i] )
            sectionSize += read.strings[i]->length();
    }

    constexpr size_t maxStandardSize = u16_max;
    if ( sectionSize > maxStandardSize )
        throw Chk::MaximumCharactersExceeded(Chk::getNameString(SectionName::STR), sectionSize-sizeAndOffsetSpaceAndNulSpace, maxStandardSize);

    stringBytes.assign(sizeof(u16)+sizeof(u16)*numStrings, u8(0));
    (u16 &)stringBytes[0] = (u16)numStrings;
    u16 initialNulOffset = u16(stringBytes.size());
    stringBytes.push_back(u8('\0')); // Add initial NUL character
    for ( size_t i=1; i<=numStrings; i++ )
    {
        if ( !read.strings[i] )
            (u16 &)stringBytes[sizeof(u16)*i] = initialNulOffset;
        else
        {
            (u16 &)stringBytes[sizeof(u16)*i] = u16(stringBytes.size());
            stringBytes.insert(stringBytes.end(), read.strings[i]->str.c_str(), read.strings[i]->str.c_str()+(read.strings[i]->length()+1));
        }
    }
}

void Scenario::syncRemasteredStringsToBytes(std::vector<u8> & stringBytes)
{
    /**
        Uses the standard SC:R STRx section format

        u32 numStrings;
        u32 stringOffsets[numStrings]; // Offset of the start of the string within the section
        void[] stringData; // Character data, first comes initial NUL character... then all strings, in order, each NUL terminated
    */
    constexpr size_t maxStrings = (size_t(u32_max) - sizeof(u32))/sizeof(u32);
    size_t numStrings = read.strings.size() > 0 ? read.strings.size()-1 : 0; // Exclude string at index 0
    if ( numStrings > maxStrings )
        throw Chk::MaximumStringsExceeded(Chk::getNameString(SectionName::STRx), numStrings, maxStrings);
        
    size_t sizeAndOffsetSpaceAndNulSpace = sizeof(u32) + sizeof(u32)*numStrings + 1;
    size_t sectionSize = sizeAndOffsetSpaceAndNulSpace;
    for ( size_t i=1; i<=numStrings; i++ )
    {
        if ( read.strings[i] )
            sectionSize += read.strings[i]->length();
    }
    constexpr size_t maxStandardSize = u32_max;
    if ( sectionSize > maxStandardSize )
        throw Chk::MaximumCharactersExceeded(Chk::getNameString(SectionName::STRx), sectionSize-sizeAndOffsetSpaceAndNulSpace, maxStandardSize);
    stringBytes.assign(sizeof(u32)+sizeof(u32)*numStrings, u8(0));
    (u32 &)stringBytes[0] = (u32)numStrings;
    u32 initialNulOffset = u32(stringBytes.size());
    stringBytes.push_back(u8('\0')); // Add initial NUL character
    for ( size_t i=1; i<=numStrings; i++ )
    {
        if ( read.strings[i] )
        {
            (u32 &)stringBytes[sizeof(u32)*i] = u32(stringBytes.size());
            stringBytes.insert(stringBytes.end(), read.strings[i]->str.c_str(), read.strings[i]->str.c_str()+(read.strings[i]->length()+1));
        }
        else
            (u32 &)stringBytes[sizeof(u32)*i] = initialNulOffset;
    }
}

void Scenario::syncKstringsToBytes(std::vector<u8> & stringBytes, u32 requestedCompressionFlags, u32 allowedCompressionFlags)
{
    /**
        Uses the standard KSTR format
        
        u32 version; // Current version: 2
        u32 numStrings; // Number of strings in the section
        u32 stringOffsets[0]; // u32 stringOffsets[numStrings]; // Offsets to each string within the string section (not within stringData, but within the whole section)
        StringProperties[numStrings] stringProperties; // String properties
        void[] stringData; // List of strings, each null terminated
    */

    constexpr size_t maxStrings = (size_t(s32_max) - 2*sizeof(u32))/sizeof(u32);
    size_t numStrings = read.editorStrings.size() > 0 ? read.editorStrings.size()-1 : 0; // Exclude string at index 0
    if ( numStrings > maxStrings )
        throw Chk::MaximumStringsExceeded(Chk::getNameString(SectionName::KSTR), numStrings, maxStrings);

    size_t stringPropertiesStart = 2*sizeof(u32)+2*numStrings;
    size_t versionAndSizeAndOffsetAndStringPropertiesAndNulSpace = 2*sizeof(u32) + 2*sizeof(u32)*numStrings + 1;
    size_t sectionSize = versionAndSizeAndOffsetAndStringPropertiesAndNulSpace;
    for ( size_t i=1; i<=numStrings; i++ )
    {
        if ( read.editorStrings[i] )
            sectionSize += read.editorStrings[i]->length();
    }

    constexpr size_t maxStandardSize = s32_max;
    if ( sectionSize > maxStandardSize )
        throw Chk::MaximumCharactersExceeded(Chk::getNameString(SectionName::KSTR), sectionSize-versionAndSizeAndOffsetAndStringPropertiesAndNulSpace, maxStandardSize);

    stringBytes.assign(2*sizeof(u32)+2*sizeof(u32)*numStrings, u8(0));
    (u32 &)stringBytes[0] = Chk::KSTR::CurrentVersion;
    (u32 &)stringBytes[sizeof(u32)] = (u32)numStrings;
    u32 initialNulOffset = u32(stringBytes.size());
    stringBytes.push_back(u8('\0')); // Add initial NUL character
    for ( size_t i=1; i<=numStrings; i++ )
    {
        if ( !read.editorStrings[i] )
            (u32 &)stringBytes[sizeof(u32)+sizeof(u32)*i] = initialNulOffset;
        else
        {
            auto prop = read.editorStrings[i]->properties;
            (u32 &)stringBytes[stringPropertiesStart+sizeof(u32)*i] = (u32)Chk::StringProperties(prop.red, prop.green, prop.blue, prop.isUsed, prop.hasPriority, prop.isBold, prop.isUnderlined, prop.isItalics, prop.size);
            (u32 &)stringBytes[sizeof(u32)+sizeof(u32)*i] = u32(stringBytes.size());
            stringBytes.insert(stringBytes.end(), read.editorStrings[i]->str.c_str(), read.editorStrings[i]->str.c_str()+read.editorStrings[i]->length()+1);
        }
    }
}

void Scenario::syncBytesToStrings(const std::vector<u8> & stringBytes, std::vector<std::optional<ScStr>> & strings, std::vector<u8> & strTailData)
{
    size_t numBytes = stringBytes.size();
    u16 rawNumStrings = numBytes >= 2 ? (u16 &)stringBytes[0] : numBytes == 1 ? (u16)stringBytes[0] : 0;
    size_t highestStringWithValidOffset = std::min(size_t(rawNumStrings), numBytes < 4 ? 0 : numBytes/2-1);
    strings.clear();
    strings.push_back(std::nullopt); // Fill the non-existant 0th stringId

    size_t stringId = 1;
    size_t sectionLastCharacter = 0;
    for ( ; stringId <= highestStringWithValidOffset; ++stringId )
    {
        size_t offsetPos = sizeof(u16)*stringId;
        size_t stringOffset = size_t((u16 &)stringBytes[offsetPos]);
        size_t lastCharacter = loadString(stringBytes, stringOffset, numBytes, strings);

        if ( lastCharacter > sectionLastCharacter )
            sectionLastCharacter = lastCharacter;
    }
    if ( highestStringWithValidOffset < size_t(rawNumStrings) ) // Some offsets aren't within bounds
    {
        if ( numBytes % 2 > 0 ) // Can read one byte of an offset
        {
            stringId ++;
            size_t stringOffset = size_t((u16)stringBytes[numBytes-1]);
            loadString(stringBytes, stringOffset, numBytes, strings);
        }
        for ( ; stringId <= size_t(rawNumStrings); ++stringId ) // Any remaining strings are fully out of bounds
            strings.push_back(std::nullopt);
    }

    size_t offsetsEnd = sizeof(u16) + sizeof(u16)*rawNumStrings;
    size_t charactersEnd = sectionLastCharacter+1;
    size_t regularStrSectionEnd = std::max(offsetsEnd, charactersEnd);
    if ( regularStrSectionEnd < numBytes ) // Tail data exists starting at regularStrSectionEnd
    {
        auto tailStart = std::next(stringBytes.begin(), regularStrSectionEnd);
        auto tailEnd = stringBytes.end();
        strTailData.assign(tailStart, tailEnd);
        logger.info() << "Read " << strTailData.size() << " bytes of tailData after the STR section" << std::endl;
        if ( strTailData.size() > 8 )
        {
            logger.info() << "This is most likely a compiled EUD map, unlike the source map, compiled EUD maps cannot be edited further." << std::endl;
            logger.info() << "Additionally, everything you see in Chkdraft may be radically different from what you see in StarCraft." << std::endl;
        }
    }
    else // No tail data exists
        strTailData.clear();
}

void Scenario::syncRemasteredBytesToStrings(const std::vector<u8> & stringBytes, std::vector<std::optional<ScStr>> & strings, std::vector<u8> & strTailData)
{
    size_t numBytes = stringBytes.size();
    u32 rawNumStrings = numBytes >= 4 ? (u32 &)stringBytes[0] : numBytes == 1 ? u32((u8 &)stringBytes[0]) : 0;
    size_t highestStringWithValidOffset = std::min(size_t(rawNumStrings), numBytes < 4 ? 0 : numBytes/4-1);
    strings.clear();
    strings.push_back(std::nullopt); // Fill the non-existant 0th stringId
    size_t stringId = 1;
    size_t sectionLastCharacter = 0;
    for ( ; stringId <= highestStringWithValidOffset; ++stringId )
    {
        size_t offsetPos = sizeof(u32)*stringId;
        size_t stringOffset = size_t((u32 &)stringBytes[offsetPos]);
        size_t lastCharacter = loadString(stringBytes, stringOffset, numBytes, strings);
        if ( lastCharacter > sectionLastCharacter )
            sectionLastCharacter = lastCharacter;
    }
    if ( highestStringWithValidOffset < size_t(rawNumStrings) ) // Some offsets aren't within bounds
    {
        if ( numBytes % 4 > 0 ) // Can read one byte of an offset
        {
            stringId ++;
            size_t stringOffset = size_t((u32)stringBytes[numBytes-1]);
            loadString(stringBytes, stringOffset, numBytes, strings);
        }
        if ( stringId <= size_t(rawNumStrings) && rawNumStrings > 1024 ) // Downsize the strings rather than insert potentially millions of blanks
            rawNumStrings = 1024;
        else
        {
            for ( ; stringId <= size_t(rawNumStrings); ++stringId ) // Any remaining strings are fully out of bounds
                strings.push_back(std::nullopt);
        }
    }
    size_t offsetsEnd = sizeof(u32) + sizeof(u32)*rawNumStrings;
    size_t charactersEnd = sectionLastCharacter+1;
    size_t regularStrxSectionEnd = std::max(offsetsEnd, charactersEnd);
    if ( regularStrxSectionEnd < numBytes ) // Tail data exists starting at regularStrxSectionEnd
    {
        auto tailStart = std::next(stringBytes.begin(), regularStrxSectionEnd);
        auto tailEnd = stringBytes.end();
        strTailData.assign(tailStart, tailEnd);
        logger.info() << "Read " << strTailData.size() << " bytes of tailData after the STRx section" << std::endl;
        if ( strTailData.size() > 8 )
        {
            logger.info() << "This is most likely a compiled EUD map, unlike the source map, compiled EUD maps cannot be edited further." << std::endl;
            logger.info() << "Additionally, everything you see in Chkdraft may be radically different from what you see in StarCraft." << std::endl;
        }
    }
    else // No tail data exists
        strTailData.clear();
}

void Scenario::syncBytesToKstrings(const std::vector<u8> & stringBytes, std::vector<std::optional<ScStr>> & editorStrings, Chk::KstrVersion & editorStringsVersion)
{
    size_t numBytes = stringBytes.size();
    u32 version = 0;
    if ( numBytes >= 4 )
        version = (u32 &)stringBytes[0];
    else
        version = 0;

    if ( version > Chk::KSTR::CurrentVersion )
        throw Chk::SectionValidationException(Chk::SectionName::KSTR, "Unrecognized KSTR Version: " + std::to_string(version));
    
    editorStringsVersion = Chk::KstrVersion(version);

    u32 rawNumStrings = 0;
    if ( numBytes >= 8 )
        rawNumStrings = (u32 &)stringBytes[4];
    else if ( numBytes == 7 )
    {
        u8 paddedTriplet[4] = { stringBytes[4], stringBytes[5], stringBytes[6], u8(0) };
        rawNumStrings = (u32 &)paddedTriplet[0];
    }
    else if ( numBytes == 6 )
        rawNumStrings = u32((u16 &)stringBytes[4]);
    else if ( numBytes == 5 )
        rawNumStrings = u32(stringBytes[4]);
    
    size_t highestStringWithValidOffset = std::min(size_t(rawNumStrings), numBytes < 12 ? 0 : (numBytes-8)/4);
    size_t highestStringWithValidProperties = std::min(size_t(rawNumStrings), numBytes < 12 ? 0 : (numBytes-8)/8);
    size_t propertiesStartMinusFour = sizeof(u32)+sizeof(u32)*rawNumStrings;
    editorStrings.clear();
    editorStrings.push_back(std::nullopt); // Fill the non-existant 0th stringId

    size_t stringId = 1;
    for ( ; stringId <= highestStringWithValidOffset; ++stringId )
    {
        size_t offsetPos = sizeof(u32)+sizeof(u32)*stringId;
        size_t stringOffset = size_t((u32 &)stringBytes[offsetPos]);
        loadKstring(stringBytes, stringOffset, numBytes, editorStrings);
        if ( stringId <= highestStringWithValidProperties && editorStrings[stringId] )
        {
            size_t propertiesPos = propertiesStartMinusFour + sizeof(u32)*stringId;
            Chk::StringProperties properties = (Chk::StringProperties &)stringBytes[propertiesPos];
            editorStrings[stringId]->properties = StrProp(properties);
        }
    }
    if ( highestStringWithValidOffset < size_t(rawNumStrings) ) // Some offsets aren't within bounds
    {
        if ( numBytes % 4 == 3 ) // Can read three bytes of an offset
        {
            stringId ++;
            u8 paddedTriplet[4] = { stringBytes[numBytes-3], stringBytes[numBytes-2], stringBytes[numBytes-1], u8(0) };
            size_t stringOffset = size_t((u32 &)paddedTriplet[0]);
            loadKstring(stringBytes, stringOffset, numBytes, editorStrings);
        }
        else if ( numBytes % 4 == 2 ) // Can read two bytes of an offset
        {
            stringId ++;
            size_t stringOffset = size_t((u16 &)stringBytes[numBytes]);
            loadKstring(stringBytes, stringOffset, numBytes, editorStrings);
        }
        else if ( numBytes % 4 == 1 ) // Can read one byte of an offset
        {
            stringId ++;
            size_t stringOffset = size_t(stringBytes[sizeof(u32)*highestStringWithValidOffset]);
            loadKstring(stringBytes, stringOffset, numBytes, editorStrings);
        }
        for ( ; stringId <= size_t(rawNumStrings); ++stringId ) // Any remaining editorStrings are fully out of bounds
            editorStrings.push_back(std::nullopt);
    }
}

size_t Scenario::loadString(const std::vector<u8> & stringBytes, const size_t & stringOffset, const size_t & sectionSize, std::vector<std::optional<ScStr>> & strings)
{
    if ( stringOffset < sectionSize )
    {
        auto nextNull = std::find(stringBytes.begin()+stringOffset, stringBytes.end(), u8('\0'));
        if ( nextNull != stringBytes.end() )
        {
            auto nullIndex = std::distance(stringBytes.begin(), nextNull);
            if ( size_t(nullIndex) >= stringOffset ) // Regular string
            {
                strings.push_back(std::make_optional<ScStr>((const char*)&stringBytes[stringOffset]));
                return nullIndex;
            }
            else // String ends where section ends
            {
                strings.push_back(std::string((const char*)&stringBytes[stringOffset], sectionSize-stringOffset));
                return sectionSize-1;
            }
        }
        else if ( sectionSize > stringOffset ) // String ends where section ends
        {
            strings.push_back(std::string((const char*)&stringBytes[stringOffset], sectionSize-stringOffset));
            return sectionSize-1;
        }
        else // Character data would be out of bounds
            strings.push_back(std::nullopt);
    }
    else // Offset is out of bounds
        strings.push_back(std::nullopt);

    return 0;
}

void Scenario::loadKstring(const std::vector<u8> & stringBytes, const size_t & stringOffset, const size_t & sectionSize, std::vector<std::optional<ScStr>> & editorStrings)
{
    if ( stringOffset < sectionSize )
    {
        auto nextNull = std::find(stringBytes.begin()+stringOffset, stringBytes.end(), u8('\0'));
        if ( nextNull != stringBytes.end() )
        {
            auto nullIndex = std::distance(stringBytes.begin(), nextNull);
            if ( size_t(nullIndex) >= stringOffset ) // Regular string
                editorStrings.push_back(std::string((const char*)&stringBytes[stringOffset]));
            else // String ends where section ends
                editorStrings.push_back(std::string((const char*)&stringBytes[stringOffset], sectionSize-stringOffset));
        }
        else if ( sectionSize > stringOffset ) // String ends where section ends
            editorStrings.push_back(std::string((const char*)&stringBytes[stringOffset], sectionSize-stringOffset));
        else // Character data would be out of bounds
            editorStrings.push_back(std::nullopt);
    }
    else // Offset is out of bounds
        editorStrings.push_back(std::nullopt);
}

bool Scenario::upgradeKstrToCurrent()
{
    if ( read.editorStringsVersion >= Chk::KstrVersion::Current )
        return false;

    auto ver = read.editorStringsVersion;
    if ( 0 == ver || 2 == ver )
    {
        auto edit = create_action();
        size_t strCapacity = getCapacity(Chk::Scope::Game);
        for ( size_t triggerIndex=0; triggerIndex<read.triggers.size(); triggerIndex++ )
        {
            const auto & trigger = read.triggers[triggerIndex];
            size_t extendedCommentStringId = getExtendedCommentStringId(triggerIndex);
            size_t extendedNotesStringId = getExtendedNotesStringId(triggerIndex);
            for ( size_t actionIndex=0; actionIndex<Chk::Trigger::MaxActions; actionIndex++ )
            {
                const Chk::Action & action = trigger.actions[actionIndex];
                if ( action.actionType < Chk::Action::NumActionTypes )
                {
                    if ( Chk::Action::actionUsesStringArg[action.actionType] &&
                        action.stringId > strCapacity &&
                        action.stringId != Chk::StringId::NoString &&
                        action.stringId < 65536 &&
                        65536-action.stringId < read.editorStrings.size() )
                    {
                        if ( action.actionType == Chk::Action::Type::Comment &&
                            (extendedCommentStringId == Chk::StringId::NoString ||
                                extendedNotesStringId == Chk::StringId::NoString) )
                        { // Move comment to extended comment or to notes
                            if ( extendedCommentStringId == Chk::StringId::NoString )
                            {
                                setExtendedCommentStringId(triggerIndex, 65536-action.stringId);
                                edit->triggers[triggerIndex].actions[actionIndex].stringId = 0;
                            }
                            else if ( extendedNotesStringId == Chk::StringId::NoString )
                            {
                                setExtendedNotesStringId(triggerIndex, 65536-action.stringId);
                                edit->triggers[triggerIndex].actions[actionIndex].stringId = 0;
                            }
                        }
                        else // Extended string is lost
                        {
                            auto actionString = getString<ChkdString>(65536-action.stringId, Chk::Scope::Editor);
                            logger.warn() << "Trigger #" << triggerIndex << " action #" << actionIndex << " lost extended string: \""
                                << (actionString ? *actionString : "") << "\"" << std::endl;
                            edit->triggers[triggerIndex].actions[actionIndex].stringId = Chk::StringId::NoString;
                        }
                    }

                    if ( Chk::Action::actionUsesSoundArg[action.actionType] &&
                        action.soundStringId > strCapacity &&
                        action.soundStringId != Chk::StringId::NoString &&
                        action.soundStringId < 65536 &&
                        65536-action.soundStringId < read.editorStrings.size() )
                    {
                        edit->triggers[triggerIndex].actions[actionIndex].soundStringId = 65536 - action.soundStringId;
                        auto soundString = getString<ChkdString>(65536 - action.soundStringId);
                        logger.warn() << "Trigger #" << triggerIndex << " action #" << actionIndex << " lost extended sound string: \""
                            << (soundString ? *soundString : "") << "\"" << std::endl;
                        edit->triggers[triggerIndex].actions[actionIndex].soundStringId = Chk::StringId::NoString;
                    }
                }
            }
        }
        
        for ( size_t briefingTriggerIndex=0; briefingTriggerIndex<read.briefingTriggers.size(); briefingTriggerIndex++ )
        {
            const auto & briefingTrigger = read.briefingTriggers[briefingTriggerIndex];
            for ( size_t actionIndex=0; actionIndex<Chk::Trigger::MaxActions; actionIndex++ )
            {
                const Chk::Action & briefingAction = briefingTrigger.actions[actionIndex];
                if ( briefingAction.actionType < Chk::Action::NumBriefingActionTypes )
                {
                    if ( Chk::Action::briefingActionUsesStringArg[briefingAction.actionType] &&
                        briefingAction.stringId > strCapacity &&
                        briefingAction.stringId != Chk::StringId::NoString &&
                        briefingAction.stringId < 65536 &&
                        65536-briefingAction.stringId < read.editorStrings.size() )
                    {
                        auto briefingString = getString<ChkdString>(65536 - briefingAction.stringId);
                        logger.warn() << "Briefing trigger #" << briefingTriggerIndex << " action #" << actionIndex << " lost extended string: \""
                            << (briefingString ? *briefingString : "") << "\"" << std::endl;
                        edit->briefingTriggers[briefingTriggerIndex].actions[actionIndex].stringId = Chk::StringId::NoString;
                    }

                    if ( Chk::Action::briefingActionUsesSoundArg[briefingAction.actionType] &&
                        briefingAction.soundStringId > strCapacity &&
                        briefingAction.soundStringId != Chk::StringId::NoString &&
                        briefingAction.soundStringId < 65536 &&
                        65536-briefingAction.stringId < read.editorStrings.size() )
                    {
                        auto briefingSoundString = getString<ChkdString>(65536 - briefingAction.soundStringId);
                        logger.warn() << "Briefing trigger #" << briefingTriggerIndex << " action #" << actionIndex << " lost extended sound string: \""
                            << (briefingSoundString ? *briefingSoundString : "") << "\"" << std::endl;
                        edit->briefingTriggers[briefingTriggerIndex].actions[actionIndex].soundStringId = Chk::StringId::NoString;
                    }
                }
            }
        }

        for ( size_t locationIndex=0; locationIndex<read.locations.size(); locationIndex++ )
        {
            const auto & location = read.locations[locationIndex];
            if ( location.stringId > strCapacity &&
                 location.stringId != Chk::StringId::NoString &&
                 size_t(65536-location.stringId) < read.editorStrings.size() )
            {
                edit->editorStringOverrides.locationName[locationIndex] = 65536-location.stringId;
                edit->locations[locationIndex].stringId = Chk::StringId::NoString;
            }
        }

        if ( read.scenarioProperties.scenarioNameStringId > strCapacity &&
            read.scenarioProperties.scenarioNameStringId != Chk::StringId::NoString &&
            size_t(65536-read.scenarioProperties.scenarioNameStringId) < read.editorStrings.size() )
        {
            setScenarioNameStringId(65536-read.scenarioProperties.scenarioNameStringId, Chk::Scope::Editor);
            edit->scenarioProperties.scenarioNameStringId = Chk::StringId::NoString;
        }

        if ( read.scenarioProperties.scenarioDescriptionStringId > strCapacity &&
            read.scenarioProperties.scenarioDescriptionStringId != Chk::StringId::NoString &&
            size_t(65536-read.scenarioProperties.scenarioDescriptionStringId) < read.editorStrings.size() )
        {
            setScenarioDescriptionStringId(65536-read.scenarioProperties.scenarioDescriptionStringId, Chk::Scope::Editor);
            edit->scenarioProperties.scenarioDescriptionStringId = Chk::StringId::NoString;
        }

        for ( Chk::Force i=Chk::Force::Force1; i<=Chk::Force::Force4; ((u8 &)i)++ )
        {
            if ( read.forces.forceString[i] > strCapacity &&
                read.forces.forceString[i] != Chk::StringId::NoString &&
                size_t(65536-read.forces.forceString[i]) < read.editorStrings.size() )
            {
                setForceNameStringId(i, 65536-read.forces.forceString[i], Chk::Scope::Editor);
                edit->forces.forceString[i] = Chk::StringId::NoString;
            }
        }
        for ( size_t i=0; i<Chk::TotalSounds; i++ )
        {
            if ( read.soundPaths[i] > strCapacity &&
                read.soundPaths[i] != Chk::StringId::NoString &&
                read.soundPaths[i] < 65536 &&
                65536-read.soundPaths[i] < read.editorStrings.size() )
            {
                setSoundPathStringId(i, 65536-read.soundPaths[i], Chk::Scope::Editor);
                edit->soundPaths[i] = Chk::StringId::NoString;
            }
        }
        for ( size_t i=0; i<Chk::TotalSwitches; i++ )
        {
            if ( read.switchNames[i] > strCapacity &&
                read.switchNames[i] != Chk::StringId::NoString &&
                read.switchNames[i] < 65536 &&
                65536-read.switchNames[i] < read.editorStrings.size() )
            {
                setSwitchNameStringId(i, 65536-read.switchNames[i], Chk::Scope::Editor);
                edit->switchNames[i] = Chk::StringId::NoString;
            }
        }
        for ( Sc::Unit::Type i=Sc::Unit::Type::TerranMarine; i<Sc::Unit::TotalTypes; ((u16 &)i)++ )
        {
            if ( read.origUnitSettings.nameStringId[i] > strCapacity &&
                read.origUnitSettings.nameStringId[i] != Chk::StringId::NoString &&
                size_t(65536-read.origUnitSettings.nameStringId[i]) < read.editorStrings.size() )
            {
                setUnitNameStringId(i, 65536-read.origUnitSettings.nameStringId[i], Chk::UseExpSection::No, Chk::Scope::Editor);
                edit->origUnitSettings.nameStringId[i] = Chk::StringId::NoString;
            }
        }
        for ( Sc::Unit::Type i=Sc::Unit::Type::TerranMarine; i<Sc::Unit::TotalTypes; ((u16 &)i)++ )
        {
            if ( read.unitSettings.nameStringId[i] > strCapacity &&
                read.unitSettings.nameStringId[i] != Chk::StringId::NoString &&
                size_t(65536-read.unitSettings.nameStringId[i]) < read.editorStrings.size() )
            {
                setUnitNameStringId(i, 65536-read.unitSettings.nameStringId[i], Chk::UseExpSection::Yes, Chk::Scope::Editor);
                edit->unitSettings.nameStringId[i] = Chk::StringId::NoString;
            }
        }
        edit->editorStringsVersion = Chk::KstrVersion::Current;
        return true;
    }
    else
        return false;
}

const std::vector<u32> compressionFlagsProgression = {
    StrCompressFlag::None,
    StrCompressFlag::DuplicateStringRecycling,
    StrCompressFlag::LastStringTrick,
    StrCompressFlag::LastStringTrick | StrCompressFlag::DuplicateStringRecycling,
    StrCompressFlag::ReverseStacking,
    StrCompressFlag::ReverseStacking | StrCompressFlag::DuplicateStringRecycling,
    StrCompressFlag::ReverseStacking | StrCompressFlag::LastStringTrick,
    StrCompressFlag::ReverseStacking | StrCompressFlag::LastStringTrick | StrCompressFlag::DuplicateStringRecycling,
    StrCompressFlag::SizeBytesRecycling | StrCompressFlag::ReverseStacking,
    StrCompressFlag::SizeBytesRecycling | StrCompressFlag::ReverseStacking | StrCompressFlag::DuplicateStringRecycling,
    StrCompressFlag::SizeBytesRecycling | StrCompressFlag::ReverseStacking | StrCompressFlag::LastStringTrick,
    StrCompressFlag::SizeBytesRecycling | StrCompressFlag::ReverseStacking | StrCompressFlag::LastStringTrick | StrCompressFlag::DuplicateStringRecycling,
    StrCompressFlag::SizeBytesRecycling | StrCompressFlag::ReverseStacking,
    StrCompressFlag::SizeBytesRecycling | StrCompressFlag::ReverseStacking | StrCompressFlag::DuplicateStringRecycling,
    StrCompressFlag::SizeBytesRecycling | StrCompressFlag::ReverseStacking | StrCompressFlag::LastStringTrick,
    StrCompressFlag::SizeBytesRecycling | StrCompressFlag::ReverseStacking | StrCompressFlag::LastStringTrick | StrCompressFlag::DuplicateStringRecycling,
    StrCompressFlag::SubStringRecycling,
    StrCompressFlag::SubStringRecycling | StrCompressFlag::LastStringTrick,
    StrCompressFlag::SubStringRecycling | StrCompressFlag::ReverseStacking,
    StrCompressFlag::SubStringRecycling | StrCompressFlag::ReverseStacking | StrCompressFlag::LastStringTrick,
    StrCompressFlag::SubStringRecycling | StrCompressFlag::SizeBytesRecycling,
    StrCompressFlag::SubStringRecycling | StrCompressFlag::SizeBytesRecycling | StrCompressFlag::LastStringTrick,
    StrCompressFlag::SubStringRecycling | StrCompressFlag::SizeBytesRecycling | StrCompressFlag::ReverseStacking,
    StrCompressFlag::SubStringRecycling | StrCompressFlag::SizeBytesRecycling | StrCompressFlag::ReverseStacking | StrCompressFlag::LastStringTrick,
    StrCompressFlag::OffsetInterlacing,
    StrCompressFlag::OffsetInterlacing | StrCompressFlag::LastStringTrick,
    StrCompressFlag::OffsetInterlacing | StrCompressFlag::SizeBytesRecycling,
    StrCompressFlag::OffsetInterlacing | StrCompressFlag::SizeBytesRecycling | StrCompressFlag::LastStringTrick,
    StrCompressFlag::OrderShuffledInterlacing,
    StrCompressFlag::OrderShuffledInterlacing | StrCompressFlag::SizeBytesRecycling,
    StrCompressFlag::SpareShuffledInterlacing,
    StrCompressFlag::SpareShuffledInterlacing | StrCompressFlag::LastStringTrick,
    StrCompressFlag::SpareShuffledInterlacing | StrCompressFlag::SizeBytesRecycling,
    StrCompressFlag::SpareShuffledInterlacing | StrCompressFlag::SizeBytesRecycling | StrCompressFlag::LastStringTrick,
    StrCompressFlag::SpareShuffledInterlacing | StrCompressFlag::OrderShuffledInterlacing,
    StrCompressFlag::SpareShuffledInterlacing | StrCompressFlag::OrderShuffledInterlacing | StrCompressFlag::SizeBytesRecycling,
    StrCompressFlag::SubShuffledInterlacing,
    StrCompressFlag::SubShuffledInterlacing | StrCompressFlag::LastStringTrick,
    StrCompressFlag::SubShuffledInterlacing | StrCompressFlag::SizeBytesRecycling,
    StrCompressFlag::SubShuffledInterlacing | StrCompressFlag::SizeBytesRecycling | StrCompressFlag::LastStringTrick,
    StrCompressFlag::SubShuffledInterlacing | StrCompressFlag::OrderShuffledInterlacing,
    StrCompressFlag::SubShuffledInterlacing | StrCompressFlag::OrderShuffledInterlacing | StrCompressFlag::SizeBytesRecycling,
    StrCompressFlag::SubShuffledInterlacing | StrCompressFlag::SpareShuffledInterlacing,
    StrCompressFlag::SubShuffledInterlacing | StrCompressFlag::SpareShuffledInterlacing | StrCompressFlag::LastStringTrick,
    StrCompressFlag::SubShuffledInterlacing | StrCompressFlag::SpareShuffledInterlacing | StrCompressFlag::SizeBytesRecycling,
    StrCompressFlag::SubShuffledInterlacing | StrCompressFlag::SpareShuffledInterlacing | StrCompressFlag::SizeBytesRecycling | StrCompressFlag::LastStringTrick,
    StrCompressFlag::SubShuffledInterlacing | StrCompressFlag::SpareShuffledInterlacing | StrCompressFlag::OrderShuffledInterlacing,
    StrCompressFlag::SubShuffledInterlacing | StrCompressFlag::SpareShuffledInterlacing | StrCompressFlag::OrderShuffledInterlacing
        | StrCompressFlag::SizeBytesRecycling
};

std::vector<std::optional<ScStr>> Scenario::copyStrings(Chk::Scope storageScope) const
{
    std::vector<std::optional<ScStr>> copy;
    if ( storageScope == Chk::Scope::Editor )
    {
        for ( const auto & str : read.editorStrings )
            copy.push_back(str);
    }
    else
    {
        for ( const auto & str : read.strings )
            copy.push_back(str);
    }

    return copy;
}

void Scenario::swapStrings(std::vector<std::optional<ScStr>> & strings, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Editor )
        create_action()->editorStrings = strings;
    else
        create_action()->strings = strings;
}

bool Scenario::defragment(Chk::Scope storageScope, bool matchCapacityToUsage)
{
    auto edit = create_action();
    if ( storageScope & Chk::Scope::Game )
    {
        size_t nextCandidateStringId = 0;
        size_t numStrings = read.strings.size();
        std::map<u32, u32> stringIdRemappings;
        for ( size_t i=0; i<numStrings; i++ )
        {
            if ( !read.strings[i] )
            {
                for ( size_t j = std::max(i+1, nextCandidateStringId); j < numStrings; j++ )
                {
                    if ( read.strings[j] )
                    {
                        edit->strings[i] = read.strings[j];
                        stringIdRemappings.insert(std::pair<u32, u32>((u32)j, (u32)i));
                        break;
                    }
                }
            }
        }

        if ( !stringIdRemappings.empty() )
        {
            remapStringIds(stringIdRemappings, Chk::Scope::Game);
            return true;
        }
    }
    else if ( storageScope & Chk::Scope::Editor )
    {
        size_t nextCandidateStringId = 0;
        size_t numStrings = read.editorStrings.size();
        std::map<u32, u32> stringIdRemappings;
        for ( size_t i=0; i<numStrings; i++ )
        {
            if ( !read.editorStrings[i] )
            {
                for ( size_t j = std::max(i+1, nextCandidateStringId); j < numStrings; j++ )
                {
                    if ( read.editorStrings[j] )
                    {
                        edit->editorStrings[i] = read.editorStrings[j];
                        stringIdRemappings.insert(std::pair<u32, u32>((u32)j, (u32)i));
                        break;
                    }
                }
            }
        }

        if ( !stringIdRemappings.empty() )
        {
            remapStringIds(stringIdRemappings, Chk::Scope::Editor);
            return true;
        }
    }
    return false;
}

void Scenario::remapStringIds(const std::map<u32, u32> & stringIdRemappings, Chk::Scope storageScope)
{
    auto edit = create_action();
    if ( storageScope == Chk::Scope::Game )
    {
        auto scenarioNameRemapping = stringIdRemappings.find(read.scenarioProperties.scenarioNameStringId);
        auto scenarioDescriptionRemapping = stringIdRemappings.find(read.scenarioProperties.scenarioDescriptionStringId);

        if ( scenarioNameRemapping != stringIdRemappings.end() )
            edit->scenarioProperties.scenarioNameStringId = scenarioNameRemapping->second;

        if ( scenarioDescriptionRemapping != stringIdRemappings.end() )
            edit->scenarioProperties.scenarioDescriptionStringId = scenarioDescriptionRemapping->second;

        remapForceStringIds(stringIdRemappings);
        remapUnitStringIds(stringIdRemappings);
        remapLocationStringIds(stringIdRemappings);
        remapTriggerStringIds(stringIdRemappings, storageScope);
    }
    else if ( storageScope == Chk::Scope::Editor )
    {
        auto scenarioNameRemapping = stringIdRemappings.find(read.editorStringOverrides.scenarioName);
        auto scenarioDescriptionRemapping= stringIdRemappings.find(read.editorStringOverrides.scenarioDescription);
        auto forceOneRemapping = stringIdRemappings.find(read.editorStringOverrides.forceName[0]);
        auto forceTwoRemapping = stringIdRemappings.find(read.editorStringOverrides.forceName[1]);
        auto forceThreeRemapping = stringIdRemappings.find(read.editorStringOverrides.forceName[2]);
        auto forceFourRemapping = stringIdRemappings.find(read.editorStringOverrides.forceName[3]);

        if ( scenarioNameRemapping != stringIdRemappings.end() )
            edit->editorStringOverrides.scenarioName = scenarioNameRemapping->second;

        if ( scenarioDescriptionRemapping != stringIdRemappings.end() )
            edit->editorStringOverrides.scenarioDescription = scenarioDescriptionRemapping->second;
    
        if ( forceOneRemapping != stringIdRemappings.end() )
            edit->editorStringOverrides.forceName[0] = forceOneRemapping->second;

        if ( forceTwoRemapping != stringIdRemappings.end() )
            edit->editorStringOverrides.forceName[1] = forceTwoRemapping->second;

        if ( forceThreeRemapping != stringIdRemappings.end() )
            edit->editorStringOverrides.forceName[2] = forceThreeRemapping->second;

        if ( forceFourRemapping != stringIdRemappings.end() )
            edit->editorStringOverrides.forceName[3] = forceFourRemapping->second;
    
        for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
        {
            auto found = stringIdRemappings.find(read.editorStringOverrides.unitName[i]);
            if ( found != stringIdRemappings.end() )
                edit->editorStringOverrides.unitName[i] = found->second;
        }

        for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
        {
            auto found = stringIdRemappings.find(read.editorStringOverrides.expUnitName[i]);
            if ( found != stringIdRemappings.end() )
                edit->editorStringOverrides.expUnitName[i] = found->second;
        }
    
        for ( size_t i=0; i<Chk::TotalSounds; i++ )
        {
            auto found = stringIdRemappings.find(read.editorStringOverrides.soundPath[i]);
            if ( found != stringIdRemappings.end() )
                edit->editorStringOverrides.soundPath[i] = found->second;
        }

        for ( size_t i=0; i<Chk::TotalSwitches; i++ )
        {
            auto found = stringIdRemappings.find(read.editorStringOverrides.switchName[i]);
            if ( found != stringIdRemappings.end() )
                edit->editorStringOverrides.switchName[i] = found->second;
        }

        for ( size_t i=0; i<Chk::TotalLocations; i++ )
        {
            auto found = stringIdRemappings.find(read.editorStringOverrides.locationName[i]);
            if ( found != stringIdRemappings.end() )
                edit->editorStringOverrides.locationName[i] = found->second;
        }
        remapTriggerStringIds(stringIdRemappings, storageScope);
    }
}

Sc::Player::SlotType Scenario::getSlotType(size_t slotIndex, Chk::Scope scope) const
{
    if ( slotIndex >= Sc::Player::Total )
        throw std::out_of_range(std::string("SlotIndex: ") + std::to_string(slotIndex) + " is out of range for the OWNR/IOWN sections!");

    switch ( scope )
    {
        case Chk::Scope::Game: return read.slotTypes[slotIndex];
        case Chk::Scope::Editor: return read.iownSlotTypes[slotIndex];
        case Chk::Scope::EditorOverGame: return this->hasSection(Chk::SectionName::IOWN) ? read.iownSlotTypes[slotIndex] : read.slotTypes[slotIndex];
        default: return read.slotTypes[slotIndex];
    }
    return Sc::Player::SlotType::Inactive;
}

void Scenario::setSlotType(size_t slotIndex, Sc::Player::SlotType slotType, Chk::Scope scope)
{
    auto edit = create_action(ActionDescriptor::SetSlotPlayerType);
    if ( slotIndex >= Sc::Player::Total )
        throw std::out_of_range(std::string("SlotIndex: ") + std::to_string(slotIndex) + " is out of range for the OWNR/IOWN sections!");

    switch ( scope )
    {
        case Chk::Scope::Game: edit->slotTypes[slotIndex] = slotType; break;
        case Chk::Scope::Editor: edit->iownSlotTypes[slotIndex] = slotType; break;
        default: edit->slotTypes[slotIndex] = slotType; edit->iownSlotTypes[slotIndex] = slotType; break;
    }
}

Chk::Race Scenario::getPlayerRace(size_t playerIndex) const
{
    if ( playerIndex < Sc::Player::Total )
        return read.playerRaces[playerIndex];
    else
        throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the SIDE section!");
}

void Scenario::setPlayerRace(size_t playerIndex, Chk::Race race)
{
    if ( playerIndex < Sc::Player::Total )
        create_action(ActionDescriptor::SetPlayerRace)->playerRaces[playerIndex] = race;
    else
        throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the SIDE section!");
}

Chk::PlayerColor Scenario::getPlayerColor(size_t slotIndex) const
{
    if ( slotIndex < Sc::Player::TotalSlots )
        return read.playerColors[slotIndex];
    else
        throw std::out_of_range(std::string("SlotIndex: ") + std::to_string((u32)slotIndex) + " is out of range for the COLR section!");
}

void Scenario::setPlayerColor(size_t slotIndex, Chk::PlayerColor color)
{
    auto edit = create_action(ActionDescriptor::SetPlayerColor);
    if ( slotIndex < Sc::Player::TotalSlots )
    {
        edit->playerColors[slotIndex] = color;
        if ( color > Chk::PlayerColor::Azure_NeutralColor && this->isRemastered() )
            upgradeToRemasteredColors();

        if ( isUsingRemasteredColors() )
        {
            edit->customColors.playerSetting[slotIndex] = Chk::PlayerColorSetting::UseId;
            edit->customColors.playerColor[slotIndex][0] = u8(0); // R
            edit->customColors.playerColor[slotIndex][1] = u8(0); // G
            edit->customColors.playerColor[slotIndex][2] = color; // B (or color index)
        }
    }
    else
        throw std::out_of_range(std::string("SlotIndex: ") + std::to_string((u32)slotIndex) + " is out of range for the COLR section!");
}

Chk::Force Scenario::getPlayerForce(size_t slotIndex) const
{
    if ( slotIndex < Sc::Player::TotalSlots )
        return read.forces.playerForce[slotIndex];
    else
        throw std::out_of_range(std::string("SlotIndex: ") + std::to_string((u32)slotIndex) + " is out of range for the FORC section!");
}

size_t Scenario::getForceStringId(Chk::Force force) const
{
    if ( force < Chk::TotalForces )
        return read.forces.forceString[(size_t)force];
    else
        throw std::out_of_range(std::string("Force: ") + std::to_string(force) + " is out of range for the FORC section!");
}

u8 Scenario::getForceFlags(Chk::Force force) const
{
    if ( force < Chk::TotalForces )
        return read.forces.flags[force];
    else
        throw std::out_of_range(std::string("Force: ") + std::to_string(force) + " is out of range for the FORC section!");
}

void Scenario::setPlayerForce(size_t slotIndex, Chk::Force force)
{
    if ( slotIndex < Sc::Player::TotalSlots )
        create_action(ActionDescriptor::SetPlayerForce)->forces.playerForce[slotIndex] = force;
    else
        throw std::out_of_range(std::string("SlotIndex: ") + std::to_string((u32)slotIndex) + " is out of range for the FORC section!");
}

void Scenario::setForceStringId(Chk::Force force, u16 forceStringId)
{
    if ( force < Chk::TotalForces )
        create_action(ActionDescriptor::UpdateForceName)->forces.forceString[(size_t)force] = forceStringId;
    else
        throw std::out_of_range(std::string("Force: ") + std::to_string(force) + " is out of range for the FORC section!");
}

void Scenario::setForceFlags(Chk::Force force, u8 forceFlags)
{
    if ( force < Chk::TotalForces )
        create_action(ActionDescriptor::UpdateForceFlags)->forces.flags[force] = Chk::ForceFlags(forceFlags);
    else
        throw std::out_of_range(std::string("Force: ") + std::to_string(force) + " is out of range for the FORC section!");
}

void Scenario::appendForceStrUsage(size_t stringId, std::vector<Chk::StringUser> & stringUsers, u32 userMask) const
{
    if ( (userMask & Chk::StringUserFlag::Force) == Chk::StringUserFlag::Force )
    {
        for ( size_t i=0; i<Chk::TotalForces; i++ )
        {
            if ( read.forces.forceString[i] == stringId )
                stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::Force, i));
        }
    }
}

bool Scenario::forceStringUsed(size_t stringId, u32 userMask) const
{
    return (userMask & Chk::StringUserFlag::Force) == Chk::StringUserFlag::Force &&
        read.forces.forceString[0] == (u16)stringId || read.forces.forceString[1] == (u16)stringId ||
        read.forces.forceString[2] == (u16)stringId || read.forces.forceString[3] == (u16)stringId;
}

void Scenario::markUsedForceStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask) const
{
    return read.markUsedForceStrings(stringIdUsed, userMask);
}

void Scenario::remapForceStringIds(const std::map<u32, u32> & stringIdRemappings)
{
    auto edit = create_action();
    auto forceOneRemapping = stringIdRemappings.find(read.forces.forceString[0]);
    auto forceTwoRemapping = stringIdRemappings.find(read.forces.forceString[1]);
    auto forceThreeRemapping = stringIdRemappings.find(read.forces.forceString[2]);
    auto forceFourRemapping = stringIdRemappings.find(read.forces.forceString[3]);

    if ( forceOneRemapping != stringIdRemappings.end() )
        edit->forces.forceString[0] = forceOneRemapping->second;

    if ( forceTwoRemapping != stringIdRemappings.end() )
        edit->forces.forceString[1] = forceTwoRemapping->second;

    if ( forceThreeRemapping != stringIdRemappings.end() )
        edit->forces.forceString[2] = forceThreeRemapping->second;

    if ( forceFourRemapping != stringIdRemappings.end() )
        edit->forces.forceString[3] = forceFourRemapping->second;
}

void Scenario::deleteForceString(size_t stringId)
{
    for ( size_t i=0; i<Chk::TotalForces; i++ )
    {
        if ( read.forces.forceString[i] == stringId )
            create_action(ActionDescriptor::UpdateForceName)->forces.forceString[i] = 0;
    }
}

bool Scenario::isUsingRemasteredColors()
{
    return hasSection(SectionName::CRGB);
}

void Scenario::upgradeToRemasteredColors()
{
    if ( !hasSection(SectionName::CRGB) )
    {
        auto edit = create_action();
        addSaveSection(::MapData::Section{SectionName::CRGB});
        for ( size_t i=0; i<Sc::Player::TotalSlots; ++i )
        {
            edit->customColors.playerColor[i][0] = u8(0); // R
            edit->customColors.playerColor[i][1] = u8(0); // G
            edit->customColors.playerColor[i][2] = read.playerColors[i]; // B (or color index)
            edit->customColors.playerSetting[i] = Chk::PlayerColorSetting::UseId;
        }
    }
}

Chk::PlayerColorSetting Scenario::getPlayerColorSetting(size_t playerIndex)
{
    if ( playerIndex < Sc::Player::TotalSlots )
        return read.customColors.playerSetting[playerIndex];
    else
        throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the CRGB section!");
}

void Scenario::setPlayerColorSetting(size_t playerIndex, Chk::PlayerColorSetting setting)
{
    if ( playerIndex < Sc::Player::TotalSlots )
    {
        upgradeToRemasteredColors();
        create_action(ActionDescriptor::SetPlayerColor)->customColors.playerSetting[playerIndex] = setting;
    }
    else
        throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the CRGB section!");
}

Chk::Rgb Scenario::getPlayerCustomColor(size_t playerIndex)
{
    if ( playerIndex < Sc::Player::TotalSlots )
    {
        return Chk::Rgb { read.customColors.playerColor[playerIndex][0], read.customColors.playerColor[playerIndex][1], read.customColors.playerColor[playerIndex][2] };
    }
    else
        throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the CRGB section!");
}

void Scenario::setPlayerCustomColor(size_t playerIndex, Chk::Rgb rgb)
{
    if ( playerIndex < Sc::Player::TotalSlots )
    {
        auto edit = create_action(ActionDescriptor::SetPlayerColor);
        upgradeToRemasteredColors();
        edit->customColors.playerColor[playerIndex][0] = rgb.red;
        edit->customColors.playerColor[playerIndex][1] = rgb.green;
        edit->customColors.playerColor[playerIndex][2] = rgb.blue;
    }
    else
        throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the CRGB section!");
}

Sc::Terrain::Tileset Scenario::getTileset() const
{
    return read.tileset;
}

void Scenario::setTileset(Sc::Terrain::Tileset tileset)
{
    if ( tileset != read.tileset )
        create_action(ActionDescriptor::ChangeTileset)->tileset = tileset;
}

size_t Scenario::getPixelWidth() const
{
    return read.dimensions.tileWidth * Sc::Terrain::PixelsPerTile;
}

size_t Scenario::getPixelHeight() const
{
    return read.dimensions.tileHeight * Sc::Terrain::PixelsPerTile;
}

std::vector<Chk::IsomRect> resizeAndOffsetIsom(const std::vector<Chk::IsomRect> & isomTiles,
    u16 newTileWidth, u16 newTileHeight, u16 /*oldTileWidth*/, u16 /*oldTileHeight*/, s32 /*leftEdge*/, s32 /*topEdge*/)
{
    std::vector<Chk::IsomRect> newIsomRects = isomTiles;
    size_t oldNumIndices = isomTiles.size();
    size_t newNumIndices = (size_t(newTileWidth) / size_t(2) + size_t(1)) * (size_t(newTileHeight) + size_t(1));
    if ( oldNumIndices < newNumIndices )
    {
        newIsomRects.reserve(newNumIndices);
        for ( size_t i=oldNumIndices; i<newNumIndices; i++ )
            newIsomRects.push_back(Chk::IsomRect());
    }
    else if ( oldNumIndices > newNumIndices )
    {
        auto eraseStart = newIsomRects.begin();
        std::advance(eraseStart, newNumIndices);
        newIsomRects.erase(eraseStart, newIsomRects.end());
    }
    return newIsomRects;
}

template <typename T>
std::vector<T> resizeAndOffset(const std::vector<T> & tiles, u16 newTileWidth, u16 newTileHeight, u16 oldTileWidth, u16 oldTileHeight, std::ptrdiff_t leftEdge, std::ptrdiff_t topEdge)
{
    if ( oldTileWidth == newTileWidth && oldTileHeight == newTileHeight )
    {
        if ( tiles.size() == newTileWidth*newTileHeight )
            return tiles;
        else
        {
            std::vector<T> newTiles(tiles.begin(), tiles.end());
            newTiles.resize(newTileWidth*newTileHeight, T{0});
            return newTiles;
        }
    }

    std::vector<T> newTiles(std::size_t(newTileWidth)*std::size_t(newTileHeight), 0);

    auto srcWidth = static_cast<std::ptrdiff_t>(oldTileWidth);
    auto srcHeight = static_cast<std::ptrdiff_t>(oldTileHeight);
    auto destWidth = static_cast<std::ptrdiff_t>(newTileWidth);
    auto destHeight = static_cast<std::ptrdiff_t>(newTileHeight);

    std::ptrdiff_t srcStartX = leftEdge < 0 ? -leftEdge : 0;
    std::ptrdiff_t destStartX = leftEdge > 0 ? leftEdge : 0;
    std::ptrdiff_t copyWidth = std::min(srcWidth-srcStartX, destWidth-destStartX);
    std::ptrdiff_t srcEndX = srcStartX+copyWidth;
    std::ptrdiff_t destEndX = destStartX+copyWidth;
    
    std::ptrdiff_t srcStartY = topEdge < 0 ? -topEdge : 0;
    std::ptrdiff_t destStartY = topEdge > 0 ? topEdge : 0;
    std::ptrdiff_t copyHeight = std::min(srcHeight-srcStartY, destHeight-destStartY);
    std::ptrdiff_t srcEndY = srcStartY+copyHeight;
    std::ptrdiff_t destEndY = destStartY+copyHeight;

    if ( copyWidth > 0 && copyHeight > 0 )
    {
        if ( copyWidth == srcWidth && copyWidth == destWidth ) // All data is contiguous, single copy
        {
            auto srcStart = srcStartY*srcWidth+srcStartX;
            auto srcEnd = srcStart+srcWidth*copyHeight;
            auto destStart = destStartY*destWidth+destStartY;
            std::copy(std::next(tiles.begin(), srcStart), std::next(tiles.begin(), srcEnd), std::next(newTiles.begin(), destStart));
        }
        else // Data is not contiguous, row-by-row copy
        {
            for ( std::ptrdiff_t i=0; i<copyHeight; ++i )
            {
                auto srcRowStart = (srcStartY+i)*srcWidth+srcStartX;
                auto srcRowEnd = srcRowStart + copyWidth;
                auto destRowStart = (destStartY+i)*destWidth+srcStartY;
                std::copy(std::next(tiles.begin(), srcRowStart), std::next(tiles.begin(), srcRowEnd), std::next(newTiles.begin(), destRowStart));
            }
        }
    }

    return newTiles;
}
template std::vector<u8> resizeAndOffset<u8>(const std::vector<u8> & tiles,
    u16 newTileWidth, u16 newTileHeight, u16 oldTileWidth, u16 oldTileHeight, std::ptrdiff_t leftEdge, std::ptrdiff_t topEdge);
template std::vector<u16> resizeAndOffset<u16>(const std::vector<u16> & tiles,
    u16 newTileWidth, u16 newTileHeight, u16 oldTileWidth, u16 oldTileHeight, std::ptrdiff_t leftEdge, std::ptrdiff_t topEdge);

void Scenario::setTileWidth(u16 newTileWidth, u16 sizeValidationFlags, s32 leftEdge)
{
    auto edit = create_action(ActionDescriptor::ResizeMap);
    u16 tileWidth = read.dimensions.tileWidth;
    u16 tileHeight = read.dimensions.tileHeight;
    // TODO: pull these inside Scenario??
    edit->isomRects = ::resizeAndOffsetIsom(read.isomRects, newTileWidth, tileHeight, tileWidth, tileHeight, leftEdge, 0);
    edit->tiles = ::resizeAndOffset(read.tiles, newTileWidth, tileHeight, tileWidth, tileHeight, leftEdge, 0);
    edit->editorTiles = ::resizeAndOffset(read.editorTiles, newTileWidth, tileHeight, tileWidth, tileHeight, leftEdge, 0);
    edit->dimensions.tileWidth = newTileWidth;
    validateSizes(sizeValidationFlags, tileWidth, tileHeight);
}

void Scenario::setTileHeight(u16 newTileHeight, u16 sizeValidationFlags, s32 topEdge)
{
    auto edit = create_action(ActionDescriptor::ResizeMap);
    u16 tileWidth = read.dimensions.tileWidth;
    u16 tileHeight = read.dimensions.tileHeight;
    edit->isomRects = ::resizeAndOffsetIsom(read.isomRects, tileWidth, newTileHeight, tileWidth, tileHeight, 0, topEdge);
    edit->tiles = ::resizeAndOffset(read.tiles, tileWidth, newTileHeight, tileWidth, tileHeight, 0, topEdge);
    edit->editorTiles = ::resizeAndOffset(read.editorTiles, tileWidth, newTileHeight, tileWidth, tileHeight, 0, topEdge);
    edit->dimensions.tileHeight = newTileHeight;
    validateSizes(sizeValidationFlags, tileWidth, tileHeight);
}

void Scenario::setDimensions(u16 newTileWidth, u16 newTileHeight, u16 sizeValidationFlags, s32 leftEdge, s32 topEdge)
{
    auto edit = create_action(ActionDescriptor::ResizeMap);
    u16 tileWidth = read.dimensions.tileWidth;
    u16 tileHeight = read.dimensions.tileHeight;
    edit->isomRects = ::resizeAndOffsetIsom(read.isomRects, newTileWidth, newTileHeight, tileWidth, tileHeight, leftEdge, topEdge);
    edit->tiles = ::resizeAndOffset(read.tiles, newTileWidth, newTileHeight, tileWidth, tileHeight, leftEdge, topEdge);
    edit->editorTiles = ::resizeAndOffset(read.editorTiles, newTileWidth, newTileHeight, tileWidth, tileHeight, leftEdge, topEdge);
    edit->dimensions.tileWidth = newTileWidth;
    edit->dimensions.tileHeight = newTileHeight;
    validateSizes(sizeValidationFlags, tileWidth, tileHeight);
}

u16 Scenario::getTile(size_t tileXc, size_t tileYc, Chk::Scope scope) const
{
    size_t tileWidth = read.dimensions.tileWidth;
    size_t tileIndex = tileYc*tileWidth + tileXc;
    if ( scope == Chk::Scope::EditorOverGame )
    {
        if ( tileIndex < read.editorTiles.size() )
            return read.editorTiles[tileIndex];
        else if ( tileIndex < read.tiles.size() )
            return read.tiles[tileIndex];
    }
    else if ( scope == Chk::Scope::Editor )
    {
        if ( tileIndex < read.editorTiles.size() )
            return read.editorTiles[tileIndex];
        else
            throw std::out_of_range(std::string("TileIndex: ") + std::to_string(tileIndex) + " is past the end of the TILE section!");
    }
    else if ( scope == Chk::Scope::Game )
    {
        if ( tileIndex < read.tiles.size() )
            return read.tiles[tileIndex];
        else
            throw std::out_of_range(std::string("TileIndex: ") + std::to_string(tileIndex) + " is past the end of the MTXM section!");
    }
    return 0;
}

u16 Scenario::getTilePx(size_t pixelXc, size_t pixelYc, Chk::Scope scope) const
{
    return getTile(pixelXc / Sc::Terrain::PixelsPerTile, pixelYc / Sc::Terrain::PixelsPerTile, scope);
}

void Scenario::setTile(size_t tileXc, size_t tileYc, u16 tileValue, Chk::Scope scope)
{
    size_t tileWidth = read.dimensions.tileWidth;
    size_t tileIndex = tileYc*tileWidth + tileXc;
    if ( scope & Chk::Scope::Game )
    {
        if ( tileIndex < read.tiles.size() )
            create_action(ActionDescriptor::UpdateTileValue)->tiles[tileIndex] = tileValue;
        else
            throw std::out_of_range(std::string("TileIndex: ") + std::to_string(tileIndex) + " is past the end of the MTXM section!");
    }
    if ( scope & Chk::Scope::Editor )
    {
        if ( tileIndex < read.editorTiles.size() )
            create_action(ActionDescriptor::UpdateTileValue)->editorTiles[tileIndex] = tileValue;
        else
            throw std::out_of_range(std::string("TileIndex: ") + std::to_string(tileIndex) + " is past the end of the TILE section!");
    }
}

void Scenario::setTilePx(size_t pixelXc, size_t pixelYc, u16 tileValue, Chk::Scope scope)
{
    setTile(pixelXc / Sc::Terrain::PixelsPerTile, pixelYc / Sc::Terrain::PixelsPerTile, tileValue, scope);
}

const Chk::IsomRect & Scenario::getIsomRect(size_t isomRectIndex) const
{
    if ( isomRectIndex < read.isomRects.size() )
        return read.isomRects[isomRectIndex];
    else
        throw std::out_of_range(std::string("IsomRectIndex: ") + std::to_string(isomRectIndex) + " is past the end of the ISOM section!");
}
    
bool Scenario::placeIsomTerrain(Chk::IsomDiamond isomDiamond, size_t terrainType, size_t brushExtent, Chk::IsomCache & cache)
{
    uint16_t isomValue = cache.getTerrainTypeIsomValue(terrainType);
    if ( isomValue == 0 || !isomDiamond.isValid() || size_t(isomValue) >= cache.isomLinks.size() || cache.isomLinks[size_t(isomValue)].terrainType == 0 )
        return false;

    int brushMin = int(brushExtent) / -2;
    int brushMax = brushMin + int(brushExtent);
    if ( brushExtent%2 == 0 ) {
        ++brushMin;
        ++brushMax;
    }

    cache.resetChangedArea();

    std::deque<Chk::IsomDiamond> diamondsToUpdate {};
    for ( int brushOffsetX=brushMin; brushOffsetX<brushMax; ++brushOffsetX )
    {
        for ( int brushOffsetY=brushMin; brushOffsetY<brushMax; ++brushOffsetY )
        {
            size_t brushX = isomDiamond.x + brushOffsetX - brushOffsetY;
            size_t brushY = isomDiamond.y + brushOffsetX + brushOffsetY;
            if ( isInBounds({brushX, brushY}) )
            {
                setDiamondIsomValues({brushX, brushY}, isomValue, cache);
                if ( brushOffsetX == brushMin || brushOffsetX == brushMax-1 || brushOffsetY == brushMin || brushOffsetY == brushMax-1 )
                { // Mark diamonds on the edge of the brush for radial updates
                    for ( auto i : Chk::IsomDiamond::neighbors )
                    {
                        Chk::IsomDiamond neighbor = Chk::IsomDiamond{brushX, brushY}.getNeighbor(i);
                        if ( diamondNeedsUpdate(neighbor) )
                            diamondsToUpdate.push_back(Chk::IsomDiamond{neighbor.x, neighbor.y});
                    }
                }
            }
        }
    }
    radiallyUpdateTerrain(diamondsToUpdate, cache);
    return true;
}

void Scenario::copyIsomFrom(const Scenario & sourceMap, int32_t xTileOffset, int32_t yTileOffset, Chk::IsomCache & destCache)
{
    auto edit = create_action();
    size_t sourceIsomWidth = sourceMap.getTileWidth()/2 + 1;
    size_t sourceIsomHeight = sourceMap.getTileHeight() + 1;

    int32_t diamondX = xTileOffset / 2;
    int32_t diamondY = yTileOffset;

    Sc::BoundingBox sourceRc { sourceIsomWidth, sourceIsomHeight, destCache.isomWidth, destCache.isomHeight, diamondX, diamondY };
    
    size_t copyWidth = sourceRc.right-sourceRc.left;
    size_t rowLimit = sourceRc.bottom;
    if ( diamondX > 0 )
        copyWidth -= diamondX;
    if ( diamondY > 0 )
        rowLimit -= diamondY;
    
    std::vector<std::size_t> assignedIndexes(copyWidth, 0);
    std::vector<Chk::IsomRect> assignedValues(copyWidth, Chk::IsomRect{});
    for ( size_t y=sourceRc.top; y<rowLimit; ++y )
    {
        auto srcStart = std::next(sourceMap.read.isomRects.begin(), y*sourceIsomWidth + sourceRc.left);
        std::copy(srcStart, std::next(srcStart, copyWidth), assignedValues.begin());
        std::iota(assignedIndexes.begin(), assignedIndexes.end(), (y+diamondY)*destCache.isomWidth + sourceRc.left + diamondX);
        edit->isomRects.set(assignedIndexes, assignedValues);
    }
}

void Scenario::updateTilesFromIsom(Chk::IsomCache & cache)
{
    auto edit = create_action(ActionDescriptor::BrushIsom);
    for ( size_t y=cache.changedArea.top; y<=cache.changedArea.bottom; ++y )
    {
        for ( size_t x=cache.changedArea.left; x<=cache.changedArea.right; ++x )
        {
            Chk::IsomRect isomRect = read.isomRects[y*getIsomWidth()+x];
            if ( read.isomRects[y*getIsomWidth()+x].isLeftOrRightModified() )
                updateTileFromIsom({x, y}, cache, getTileWidth(), getTileHeight(), read.isomRects, read.editorTiles);

            isomRect.clearEditorFlags();
            edit->isomRects[y*getIsomWidth()+x] = isomRect;
        }
    }
    cache.resetChangedArea();
}

bool Scenario::resizeIsom(int32_t xTileOffset, int32_t yTileOffset, size_t oldMapWidth, size_t oldMapHeight, bool fixBorders, Chk::IsomCache & cache)
{
    auto edit = create_action();
    int32_t xDiamondOffset = xTileOffset/2;
    int32_t yDiamondOffset = yTileOffset;
    size_t oldIsomWidth = oldMapWidth/2 + 1;
    size_t oldIsomHeight = oldMapHeight + 1;
    Sc::BoundingBox sourceRc { oldIsomWidth, oldIsomHeight, cache.isomWidth, cache.isomHeight, xDiamondOffset, yDiamondOffset };
    Sc::BoundingBox innerArea {
        sourceRc.left+xDiamondOffset,
        sourceRc.top+yDiamondOffset,
        std::min(cache.isomWidth, sourceRc.right+xDiamondOffset-1),
        std::min(cache.isomHeight, sourceRc.bottom+yDiamondOffset-1)
    };
     
    std::vector<Chk::IsomDiamond> edges {};
    for ( size_t y=innerArea.top; y<=innerArea.bottom; ++y )
    {
        for ( size_t x=innerArea.left+(innerArea.left+y)%2; x<innerArea.right+1; x+= 2 )
        {
            if ( (x+y)%2 != 0 )
                continue;

            bool fullyInside = true;
            bool fullyOutside = true;
            uint16_t isomValue = 0;
            for ( auto i : Sc::Isom::quadrants )
            {
                Chk::IsomRect::Point rectCoords = Chk::IsomDiamond{x, y}.getRectangleCoords(i);
                if ( isInBounds(rectCoords) )
                {
                    if ( rectCoords.x >= innerArea.left && rectCoords.x < innerArea.right &&
                        rectCoords.y >= innerArea.top && rectCoords.y < innerArea.bottom )
                    {
                        isomValue = getIsomRect(rectCoords).getIsomValue(Sc::Isom::ProjectedQuadrant{i}.firstSide) >> 4;
                        fullyOutside = false;
                    }
                    else
                        fullyInside = false;
                }
            }

            if ( fullyOutside ) // Do not update diamonds completely outside the inner area
                continue;

            if ( !fullyInside ) // Update diamonds that are partially inside and mark them for radial updates
            {
                for ( auto i : Sc::Isom::quadrants )
                {
                    Chk::IsomRect::Point rectCoords = Chk::IsomDiamond{x, y}.getRectangleCoords(i);
                    if ( (rectCoords.x < innerArea.left || rectCoords.x >= innerArea.right || // Quadrant is outside inner area
                        rectCoords.y < innerArea.top || rectCoords.y >= innerArea.bottom) )
                    {
                        setIsomValue(rectCoords, Sc::Isom::quadrants[size_t(i)], isomValue, cache);
                    }
                }

                if ( fixBorders )
                {
                    for ( auto i : Chk::IsomDiamond::neighbors )
                    {
                        Chk::IsomDiamond neighbor = Chk::IsomDiamond{x, y}.getNeighbor(i);
                        if ( isInBounds(neighbor) && (
                            neighbor.x < innerArea.left || neighbor.x > innerArea.right || // Neighbor is outside inner area
                            neighbor.y < innerArea.top || neighbor.y > innerArea.bottom) )
                        {
                            edges.push_back(neighbor);
                        }
                    }
                }
            }

            for ( auto i : Sc::Isom::quadrants )
            {
                Chk::IsomRect::Point rectCoords = Chk::IsomDiamond{x, y}.getRectangleCoords(i);
                if ( isInBounds(rectCoords) )
                    setIsomRectModified(edit, rectCoords, i);
            }
        }
    }

    // Order edges by distance from top-left over difference between x&y over x-coordinates
    std::sort(edges.begin(), edges.end(), [](const Chk::IsomDiamond & l, const Chk::IsomDiamond & r) {
        auto lDistance = l.x + l.y;
        auto rDistance = r.x + r.y;
        if ( lDistance != rDistance )
            return lDistance < rDistance; // Order by distance from top-left corner

        lDistance = std::max(l.x, l.y) - std::min(l.x, l.y);
        rDistance = std::max(r.x, r.y) - std::min(r.x, r.y);
        if ( lDistance != rDistance )
            return lDistance < rDistance; // Order by difference between x & y
        else
            return l.x < r.x; // Order by x difference
    });

    // Update all the edges
    std::deque<Chk::IsomDiamond> diamondsToUpdate;
    for ( const auto & edge : edges )
    {
        if ( diamondNeedsUpdate({edge.x, edge.y}) )
            diamondsToUpdate.push_back({edge.x, edge.y});
    }
    radiallyUpdateTerrain(diamondsToUpdate, cache);

    // Clear the changed and visited flags
    for ( size_t y=cache.changedArea.top; y<=cache.changedArea.bottom; ++y )
    {
        for ( size_t x=cache.changedArea.left; x<=cache.changedArea.right; ++x )
            clearIsomRectFlags(edit, {x, y});
    }

    for ( size_t y=innerArea.top; y<=innerArea.bottom; ++y )
    {
        for ( size_t x=innerArea.left+(innerArea.left+y)%2; x<=innerArea.right; x+=2 )
        {
            if ( (x+y)%2 != 0 )
                continue;

            bool fullyOutside = true;
            for ( auto i : Sc::Isom::quadrants )
            {
                Chk::IsomRect::Point rectCoords = Chk::IsomDiamond{x, y}.getRectangleCoords(i);
                if ( isInBounds(rectCoords) &&
                    rectCoords.x >= innerArea.left && rectCoords.x < innerArea.right && // Inside inner area
                    rectCoords.y >= innerArea.top && rectCoords.y < innerArea.bottom )
                {
                    fullyOutside = false;
                    break;
                }
            }

            if ( !fullyOutside ) // Only update diamonds that are at least partially inside
            {
                for ( auto i : Sc::Isom::quadrants )
                {
                    Chk::IsomRect::Point rectCoords = Chk::IsomDiamond{x, y}.getRectangleCoords(i);
                    if ( isInBounds(rectCoords) )
                        setIsomRectModified(edit, {rectCoords}, i);
                }
            }
        }
    }
    diamondsToUpdate.clear();

    cache.setAllChanged();

    // Clear off the changed flags for the inner area
    for ( size_t y=innerArea.top; y<innerArea.bottom; ++y )
    {
        for ( size_t x=innerArea.left; x<innerArea.right; ++x )
            clearIsomRectFlags(edit, {x, y});
    }

    for ( size_t y=0; y<cache.isomHeight; ++y )
    {
        for ( size_t x=y%2; x<cache.isomWidth; x+=2 )
        {
            if ( (x+y)%2 != 0 )
                continue;

            bool fullyInside = true;
            for ( auto i : Sc::Isom::quadrants )
            {
                Chk::IsomRect::Point rectCoords = Chk::IsomDiamond{x, y}.getRectangleCoords(i);
                if ( isInBounds(rectCoords) &&
                    (rectCoords.x < innerArea.left || rectCoords.x >= innerArea.right || // Quadrant is outside the inner area
                        rectCoords.y < innerArea.top || rectCoords.y < innerArea.bottom) )
                {
                    fullyInside = false;
                    break;
                }
            }

            if ( !fullyInside ) // Mark diamonds partially or fully outside the inner area as modified
            {
                for ( auto i : Sc::Isom::quadrants )
                {
                    Chk::IsomRect::Point rectCoords = Chk::IsomDiamond{x, y}.getRectangleCoords(i);
                    if ( isInBounds(rectCoords) )
                        setIsomRectModified(edit, rectCoords, i);
                }
            }
        }
    }

    return true;
}

Chk::TileOccupationCache Scenario::getTileOccupationCache(const Sc::Terrain::Tiles & tileset, const Sc::Unit & unitData) const
{
    std::vector<bool> tileOccupied(size_t(read.dimensions.tileWidth)*size_t(read.dimensions.tileHeight), false);
    for ( const auto & doodad : read.doodads )
    {
        if ( auto doodadGroupIndex = tileset.getDoodadGroupIndex(doodad.type) )
        {
            const auto & doodadDat = (Sc::Terrain::DoodadCv5 &)tileset.tileGroups[*doodadGroupIndex];
            const auto & placability = tileset.doodadPlacibility[doodad.type];
            
            bool evenWidth = doodadDat.tileWidth % 2 == 0;
            bool evenHeight = doodadDat.tileHeight % 2 == 0;
            size_t halfWidth = 16*size_t(doodadDat.tileWidth);
            size_t halfHeight = 16*size_t(doodadDat.tileHeight);
            size_t left = size_t(doodad.xc) < halfWidth ? 0 : (size_t(doodad.xc) - halfWidth)/32;
            size_t top = size_t(doodad.yc) < halfHeight ? 0 : (size_t(doodad.yc) - halfHeight)/32;
            for ( size_t y=0; y<size_t(doodadDat.tileHeight); ++y )
            {
                for ( size_t x=0; x<size_t(doodadDat.tileWidth); ++x )
                {
                    if ( placability.tileGroup[y*doodadDat.tileWidth+x] != 0 )
                        tileOccupied[(top+y)*read.dimensions.tileWidth+(left+x)] = true;
                }
            }
        }
    }
    for ( const auto & unit : read.units )
    {
        if ( unit.type < unitData.numUnitTypes() )
        {
            const auto & unitDat = unitData.getUnit(unit.type);
            s32 xTileMin = (unit.xc - unitDat.unitSizeLeft)/32;
            s32 xTileMax = (unit.xc + unitDat.unitSizeRight)/32+1;
            s32 yTileMin = (unit.yc - unitDat.unitSizeUp)/32;
            s32 yTileMax = (unit.yc + unitDat.unitSizeDown)/32+1;
            for ( s32 y = std::max(0, yTileMin); y<std::min(s32(read.dimensions.tileHeight), yTileMax); ++y )
            {
                for ( s32 x = std::max(0, xTileMin); x<std::min(s32(read.dimensions.tileWidth), xTileMax); ++x )
                    tileOccupied[y*read.dimensions.tileWidth+x] = true;
            }
        }
    }
    return Chk::TileOccupationCache{tileOccupied};
}

void Scenario::validateSizes(u16 sizeValidationFlags, u16 prevWidth, u16 prevHeight)
{
    bool updateAnywhereIfAlreadyStandard = (sizeValidationFlags & SizeValidationFlag::UpdateAnywhereIfAlreadyStandard) == SizeValidationFlag::UpdateAnywhereIfAlreadyStandard;
    bool updateAnywhere = (sizeValidationFlags & SizeValidationFlag::UpdateAnywhere) == SizeValidationFlag::UpdateAnywhere;
    if ( (!updateAnywhereIfAlreadyStandard && updateAnywhere) || (updateAnywhereIfAlreadyStandard && anywhereIsStandardDimensions(prevWidth, prevHeight)) )
        matchAnywhereToDimensions();

    if ( (sizeValidationFlags & SizeValidationFlag::UpdateOutOfBoundsLocations) == SizeValidationFlag::UpdateOutOfBoundsLocations )
        downsizeOutOfBoundsLocations();

    if ( (sizeValidationFlags & SizeValidationFlag::RemoveOutOfBoundsDoodads) == SizeValidationFlag::RemoveOutOfBoundsDoodads )
        removeOutOfBoundsDoodads();

    if ( (sizeValidationFlags & SizeValidationFlag::UpdateOutOfBoundsUnits) == SizeValidationFlag::UpdateOutOfBoundsUnits )
        updateOutOfBoundsUnits();
    else if ( (sizeValidationFlags & SizeValidationFlag::RemoveOutOfBoundsUnits) == SizeValidationFlag::RemoveOutOfBoundsUnits )
        removeOutOfBoundsUnits();

    if ( (sizeValidationFlags & SizeValidationFlag::UpdateOutOfBoundsSprites) == SizeValidationFlag::UpdateOutOfBoundsSprites )
        updateOutOfBoundsSprites();
    else if ( (sizeValidationFlags & SizeValidationFlag::RemoveOutOfBoundsSprites) == SizeValidationFlag::RemoveOutOfBoundsSprites )
        removeOutOfBoundsSprites();
}

u8 Scenario::getFog(size_t tileXc, size_t tileYc) const
{
    size_t tileWidth = read.dimensions.tileWidth;
    size_t tileIndex = tileWidth*tileYc + tileXc;
    if ( tileIndex < read.tileFog.size() )
        return read.tileFog[tileIndex];
    else
        throw std::out_of_range(std::string("TileIndex: ") + std::to_string(tileIndex) + " is past the end of the MASK section!");
}

u8 Scenario::getFogPx(size_t pixelXc, size_t pixelYc) const
{
    return getFog(pixelXc / Sc::Terrain::PixelsPerTile, pixelYc / Sc::Terrain::PixelsPerTile);
}

void Scenario::setFog(size_t tileXc, size_t tileYc, u8 fogOfWarPlayers)
{
    size_t tileWidth = read.dimensions.tileWidth;
    size_t tileIndex = tileWidth*tileYc + tileXc;
    
    if ( tileIndex < read.tileFog.size() )
        create_action(ActionDescriptor::UpdateTileFog)->tileFog[tileIndex] = fogOfWarPlayers;
    else
        throw std::out_of_range(std::string("TileIndex: ") + std::to_string(tileIndex) + " is past the end of the MASK section!");
}

void Scenario::setFogPx(size_t pixelXc, size_t pixelYc, u8 fogOfWarPlayers)
{
    setFog(pixelXc / Sc::Terrain::PixelsPerTile, pixelYc / Sc::Terrain::PixelsPerTile, fogOfWarPlayers);
}

size_t Scenario::numSprites() const
{
    return read.sprites.size();
}

const Chk::Sprite & Scenario::getSprite(size_t spriteIndex) const
{
    return read.sprites[spriteIndex];
}

size_t Scenario::addSprite(const Chk::Sprite & sprite)
{
    create_action(ActionDescriptor::CreateSprite)->sprites.append(sprite);
    return read.sprites.size()-1;
}

void Scenario::insertSprite(size_t spriteIndex, const Chk::Sprite & sprite)
{
    if ( spriteIndex < read.sprites.size() )
        create_action(ActionDescriptor::CreateSprite)->sprites.insert(spriteIndex, sprite);
    else
        create_action(ActionDescriptor::CreateSprite)->sprites.append(sprite);
}

void Scenario::deleteSprite(size_t spriteIndex)
{
    if ( spriteIndex < read.sprites.size() )
        create_action(ActionDescriptor::DeleteSprites)->sprites.remove(spriteIndex);
}

void Scenario::moveSprite(size_t spriteIndexFrom, size_t spriteIndexTo)
{
    create_action(ActionDescriptor::MoveSprites)->sprites.move_to(spriteIndexFrom, spriteIndexTo);
}

void Scenario::updateOutOfBoundsSprites()
{
    auto edit = create_action();
    size_t pixelWidth = this->getPixelWidth();
    size_t pixelHeight = this->getPixelHeight();
    size_t numSprites = this->numSprites();
    for ( size_t i=0; i<numSprites; i++ )
    {
        const Chk::Sprite & sprite = read.sprites[i];
        if ( sprite.xc >= pixelWidth )
            edit->sprites[i].xc = u16(pixelWidth-1);

        if ( sprite.yc >= pixelHeight )
            edit->sprites[i].yc = u16(pixelHeight-1);
    }
}

void Scenario::removeOutOfBoundsSprites()
{
    size_t pixelWidth = this->getPixelWidth();
    size_t pixelHeight = this->getPixelHeight();
    size_t numSprites = this->numSprites();
    for ( size_t i = numSprites-1; i < numSprites; i-- )
    {
        const Chk::Sprite & sprite = this->getSprite(i);
        if ( sprite.xc >= pixelWidth || sprite.yc >= pixelHeight )
            this->deleteSprite(i);
    }
}

size_t Scenario::numDoodads() const
{
    return read.doodads.size();
}

const Chk::Doodad & Scenario::getDoodad(size_t doodadIndex) const
{
    return read.doodads[doodadIndex];
}

size_t Scenario::addDoodad(const Chk::Doodad & doodad)
{
    create_action(ActionDescriptor::CreateDoodad)->doodads.append(doodad);
    return read.doodads.size()-1;
}

void Scenario::insertDoodad(size_t doodadIndex, const Chk::Doodad & doodad)
{
    if ( doodadIndex < read.doodads.size() )
        create_action(ActionDescriptor::CreateDoodad)->doodads.insert(doodadIndex, doodad);
    else
        create_action(ActionDescriptor::CreateDoodad)->doodads.append(doodad);
}

void Scenario::deleteDoodad(size_t doodadIndex)
{
    if ( doodadIndex < read.doodads.size() )
        create_action(ActionDescriptor::DeleteDoodad)->doodads.remove(doodadIndex);
}

void Scenario::moveDoodad(size_t doodadIndexFrom, size_t doodadIndexTo)
{
    create_action(ActionDescriptor::MoveDoodad)->doodads.move_to(doodadIndexFrom, doodadIndexTo);
}

void Scenario::removeOutOfBoundsDoodads()
{
    size_t pixelWidth = this->getPixelWidth();
    size_t pixelHeight = this->getPixelHeight();
    size_t numDoodads = this->numDoodads();
    for ( size_t i = numDoodads-1; i < numDoodads; i-- )
    {
        const Chk::Doodad & doodad = this->getDoodad(i);
        if ( doodad.xc >= pixelWidth || doodad.yc >= pixelHeight )
            this->deleteDoodad(i);
    }
}

size_t Scenario::numUnits() const
{
    return read.units.size();
}

const Chk::Unit & Scenario::getUnit(size_t unitIndex) const
{
    return read.units[unitIndex];
}

size_t Scenario::addUnit(const Chk::Unit & unit)
{
    create_action(ActionDescriptor::CreateUnit)->units.append(unit);
    return read.units.size()-1;
}

void Scenario::insertUnit(size_t unitIndex, const Chk::Unit & unit)
{
    if ( unitIndex < read.units.size() )
        create_action(ActionDescriptor::CreateUnit)->units.insert(unitIndex, unit);
    else
        create_action(ActionDescriptor::CreateUnit)->units.append(unit);
}

void Scenario::deleteUnit(size_t unitIndex)
{
    if ( unitIndex < read.units.size() )
        create_action(ActionDescriptor::DeleteUnits)->units.remove(unitIndex);
}

void Scenario::moveUnit(size_t unitIndexFrom, size_t unitIndexTo)
{
    create_action(ActionDescriptor::MoveUnits)->units.move_to(unitIndexFrom, unitIndexTo);
}

void Scenario::updateOutOfBoundsUnits()
{
    auto edit = create_action();
    size_t pixelWidth = this->getPixelWidth();
    size_t pixelHeight = this->getPixelHeight();
    size_t numUnits = this->numUnits();
    for ( size_t i=0; i<numUnits; i++ )
    {
        const Chk::Unit & currUnit = read.units[i];
        if ( currUnit.xc >= pixelWidth )
            edit->units[i].xc = u16(pixelWidth-1);

        if ( currUnit.yc >= pixelHeight )
            edit->units[i].yc = u16(pixelHeight-1);
    }
}

void Scenario::removeOutOfBoundsUnits()
{
    size_t pixelWidth = this->getPixelWidth();
    size_t pixelHeight = this->getPixelHeight();
    size_t numUnits = this->numUnits();
    for ( size_t i = numUnits-1; i < numUnits; i-- )
    {
        const Chk::Unit & currUnit = this->getUnit(i);
        if ( currUnit.xc >= pixelWidth || currUnit.yc >= pixelHeight )
            this->deleteUnit(i);
    }
}

size_t Scenario::numLocations() const
{
    return read.locations.size() > 0 ? read.locations.size()-1 : 0;
}

const Chk::Location & Scenario::getLocation(size_t locationId) const
{
    return read.locations[locationId];
}

size_t Scenario::addLocation(const Chk::Location & location)
{
    auto edit = create_action(ActionDescriptor::CreateLocation);
    for ( size_t i=1; i<read.locations.size(); i++ )
    {
        if ( isBlank(i) )
        {
            edit->locations[i] = location;
            return i;
        }
    }
    return Chk::LocationId::NoLocation;
}

void Scenario::replaceLocation(size_t locationId, const Chk::Location & location)
{
    if ( locationId > 0 && locationId < read.locations.size() )
    {
        if ( isBlank(locationId) )
            create_action(ActionDescriptor::ReplaceLocation)->locations[locationId] = location;
    }
    else
        throw std::out_of_range(std::string("LocationId: ") + std::to_string((u32)locationId) + " is out of range for the MRGN section!");
}

void Scenario::deleteLocation(size_t locationId, bool deleteOnlyIfUnused)
{
    if ( !deleteOnlyIfUnused || !this->triggerLocationUsed(locationId) )
    {
        if ( locationId > 0 && locationId < read.locations.size() )
        {
            create_action(ActionDescriptor::DeleteLocations)->locations[locationId] = Chk::Location{};
        }
    }
}

bool Scenario::moveLocation(size_t locationIdFrom, size_t locationIdTo, bool lockAnywhere)
{
    auto edit = create_action(ActionDescriptor::MoveLocation);
    size_t locationIdMin = std::min(locationIdFrom, locationIdTo);
    size_t locationIdMax = std::max(locationIdFrom, locationIdTo);
    if ( locationIdFrom > 0 && locationIdTo > 0 && locationIdMax < read.locations.size() && locationIdFrom != locationIdTo &&
         (!lockAnywhere || (locationIdMin != Chk::LocationId::Anywhere && locationIdMax != Chk::LocationId::Anywhere)) )
    {
        if ( locationIdMax-locationIdMin == 1 && locationIdMax < read.locations.size() ) // Move up or down by 1 using swap
        {
            edit->locations.move_up(locationIdMax);
            return true;
        }
        else // Move up or down by more than one, remove from present location, insert in the list at destination
        {
            auto location = read.locations[locationIdFrom];
            edit->locations.remove(locationIdFrom);
            edit->locations.insert(locationIdTo == 0 ? 0 : locationIdTo-1, location);

            if ( lockAnywhere && locationIdMin < Chk::LocationId::Anywhere && locationIdMax > Chk::LocationId::Anywhere )
                edit->locations.move_up(Chk::LocationId::Anywhere);

            return true;
        }
    }
    return false;
}

bool Scenario::isBlank(size_t locationId) const
{
    return locationId >= read.locations.size() || read.locations[locationId].isBlank();
}

void Scenario::downsizeOutOfBoundsLocations()
{
    auto edit = create_action();
    size_t pixelWidth = this->getPixelWidth();
    size_t pixelHeight = this->getPixelHeight();
    size_t numLocations = this->numLocations();
    for ( size_t i=1; i<=numLocations; i++ )
    {
        const Chk::Location & location = getLocation(i);

        if ( location.left >= pixelWidth )
            edit->locations[i].left = u32(pixelWidth-1);

        if ( location.right >= pixelWidth )
            edit->locations[i].right = u32(pixelWidth-1);

        if ( location.top >= pixelHeight )
            edit->locations[i].top = u32(pixelHeight-1);

        if ( location.bottom >= pixelHeight )
            edit->locations[i].bottom = u32(pixelHeight-1);
    }
}

void Scenario::markNonZeroLocations(std::bitset<Chk::TotalLocations+1> & locationIdUsed) const
{
    size_t limit = std::min(Chk::TotalLocations, read.locations.size());
    for ( size_t i=1; i<limit; i++ )
    {
        if ( !read.locations[i].isBlank() )
            locationIdUsed[i] = true;
    }
}

bool Scenario::locationsFitOriginal(bool lockAnywhere, bool autoDefragment)
{
    std::bitset<Chk::TotalLocations+1> locationIdUsed;
    this->markUsedTriggerLocations(locationIdUsed);
    markNonZeroLocations(locationIdUsed);

    size_t countUsedOrCreated = 0;
    for ( size_t i=1; i<=Chk::TotalLocations; i++ )
    {
        if ( locationIdUsed[i] || (i == Chk::LocationId::Anywhere && lockAnywhere) )
            countUsedOrCreated++;
    }

    return countUsedOrCreated <= Chk::TotalOriginalLocations;
}

bool Scenario::trimLocationsToOriginal(bool lockAnywhere, bool autoDefragment)
{
    if ( read.locations.size() > Chk::TotalOriginalLocations )
    {
        auto edit = create_action(ActionDescriptor::TrimLocationsToOriginal);
        std::bitset<Chk::TotalLocations+1> locationIdUsed;
        this->markUsedTriggerLocations(locationIdUsed);
        markNonZeroLocations(locationIdUsed);

        size_t countUsedOrCreated = 0;
        for ( size_t i=1; i<=Chk::TotalLocations; i++ )
        {
            if ( locationIdUsed[i] || (i == Chk::LocationId::Anywhere && lockAnywhere) )
                countUsedOrCreated++;
        }

        if ( countUsedOrCreated <= Chk::TotalOriginalLocations )
        {
            std::map<u32, u32> locationIdRemappings;
            for ( size_t firstUnused=1; firstUnused<=Chk::TotalLocations; firstUnused++ )
            {
                if ( !locationIdUsed[firstUnused] && (firstUnused != Chk::LocationId::Anywhere || !lockAnywhere) )
                {
                    for ( size_t i=firstUnused+1; i<=Chk::TotalLocations; i++ )
                    {
                        if ( locationIdUsed[i] && (i != Chk::LocationId::Anywhere || !lockAnywhere) )
                        {
                            edit->locations[firstUnused] = read.locations[i];
                            locationIdUsed[firstUnused] = true;
                            edit->locations[i] = Chk::Location{};
                            locationIdUsed[i] = false;
                            locationIdRemappings.insert(std::pair<u32, u32>(u32(i), u32(firstUnused)));
                            break;
                        }
                    }
                }
            }
            std::vector<std::size_t> indexesRemoved(read.locations.size()-Chk::TotalOriginalLocations-1);
            std::iota(indexesRemoved.begin(), indexesRemoved.end(), Chk::TotalOriginalLocations+1);
            edit->locations.remove(indexesRemoved);
            this->remapTriggerLocationIds(locationIdRemappings);
            return true;
        }
    }
    return read.locations.size() <= Chk::TotalOriginalLocations;
}

void Scenario::expandToScHybridOrExpansion()
{
    if ( read.locations.size() < Chk::TotalLocations )
        create_action()->locations.append(std::vector<Chk::Location>(Chk::TotalLocations+1-read.locations.size()));
}

bool Scenario::anywhereIsStandardDimensions(u16 prevWidth, u16 prevHeight) const
{
    const Chk::Location & anywhere = this->getLocation(Chk::LocationId::Anywhere);
    if ( prevWidth == 0 && prevHeight == 0 )
        return anywhere.left == 0 && anywhere.top == 0 && anywhere.right == this->getPixelWidth() && anywhere.bottom == this->getPixelHeight();
    else
        return anywhere.left == 0 && anywhere.top == 0 && anywhere.right == prevWidth*Sc::Terrain::PixelsPerTile && anywhere.bottom == prevHeight*Sc::Terrain::PixelsPerTile;
}

void Scenario::matchAnywhereToDimensions()
{
    Chk::Location anywhere = read.locations[Chk::LocationId::Anywhere];
    if ( anywhere.left != 0 || anywhere.top != 0 || anywhere.right != (u32)this->getPixelWidth() || anywhere.bottom != (u32)this->getPixelHeight() )
    {
        anywhere.left = 0;
        anywhere.top = 0;
        anywhere.right = (u32)this->getPixelWidth();
        anywhere.bottom = (u32)this->getPixelHeight();
        create_action()->locations[Chk::LocationId::Anywhere] = anywhere;
    }
}

void Scenario::appendLocationStrUsage(size_t stringId, std::vector<Chk::StringUser> & stringUsers, u32 userMask) const
{
    if ( (userMask & Chk::StringUserFlag::Location) == Chk::StringUserFlag::Location )
    {
        u16 u16StringId = (u16)stringId;
        for ( size_t i=0; i<read.locations.size(); i++ )
        {
            if ( stringId == read.locations[i].stringId )
                stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::Location, i));
        }
    }
}

bool Scenario::locationStringUsed(size_t stringId, Chk::Scope storageScope, u32 userMask) const
{
    if ( (userMask & Chk::StringUserFlag::Location) == Chk::StringUserFlag::Location )
    {
        auto usedByGame = [&](){
            for ( const auto & location : read.locations )
            {
                if ( stringId == location.stringId )
                    return true;
            }
            return false;
        };
        auto usedByEditor = [&](){
            for ( size_t i=0; i<Chk::TotalLocations; ++i )
            {
                if ( stringId == read.editorStringOverrides.locationName[i] )
                    return true;
            }
            return false;
        };
        switch ( storageScope )
        {
            case Chk::Scope::Either:
            case Chk::Scope::EditorOverGame:
            case Chk::Scope::GameOverEditor: return usedByGame() || usedByEditor();
            case Chk::Scope::Game: return usedByGame();
            case Chk::Scope::Editor: return usedByEditor();
            default: return false;
        }
    }
    return false;
}

void Scenario::markUsedLocationStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask) const
{
    return read.markUsedLocationStrings(stringIdUsed, userMask);
}

void Scenario::remapLocationStringIds(const std::map<u32, u32> & stringIdRemappings)
{
    auto edit = create_action();
    for ( size_t i=1; i<read.locations.size(); i++ )
    {
        auto found = stringIdRemappings.find(read.locations[i].stringId);
        if ( found != stringIdRemappings.end() )
            edit->locations[i].stringId = found->second;
    }
}

void Scenario::deleteLocationString(size_t stringId)
{
    auto edit = create_action(ActionDescriptor::UpdateLocationName);
    for ( size_t i=1; i<read.locations.size(); i++ )
    {
        if ( read.locations[i].stringId == stringId )
            edit->locations[i].stringId = 0;
    }
}

bool Scenario::useExpansionUnitSettings(Chk::UseExpSection useExp, Sc::Weapon::Type weaponType) const
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto: return weaponType >= Sc::Weapon::Type::NeutronFlare || this->isHybridOrAbove();
        case Chk::UseExpSection::Yes: return true;
        case Chk::UseExpSection::No: return false;
        case Chk::UseExpSection::NoIfOrigAvailable: return !this->hasSection(Chk::SectionName::UNIS);
        case Chk::UseExpSection::YesIfAvailable: 
        default: return this->hasSection(Chk::SectionName::UNIx);
    }
    return true;
}

bool Scenario::unitUsesDefaultSettings(Sc::Unit::Type unitType, Chk::UseExpSection useExp) const
{
    if ( unitType >= Sc::Unit::TotalTypes )
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS/UNIx section!");

    return useExpansionUnitSettings(useExp) ?
        read.unitSettings.useDefault[unitType] != Chk::UseDefault::No : read.origUnitSettings.useDefault[unitType] != Chk::UseDefault::No;
}

u32 Scenario::getUnitHitpoints(Sc::Unit::Type unitType, Chk::UseExpSection useExp) const
{
    if ( unitType >= Sc::Unit::TotalTypes )
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS/UNIx section!");

    return useExpansionUnitSettings(useExp) ? read.unitSettings.hitpoints[unitType] : read.origUnitSettings.hitpoints[unitType];
}

u16 Scenario::getUnitShieldPoints(Sc::Unit::Type unitType, Chk::UseExpSection useExp) const
{
    if ( unitType >= Sc::Unit::TotalTypes )
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS/UNIx section!");

    return useExpansionUnitSettings(useExp) ? read.unitSettings.shieldPoints[unitType] : read.origUnitSettings.shieldPoints[unitType];
}

u8 Scenario::getUnitArmorLevel(Sc::Unit::Type unitType, Chk::UseExpSection useExp) const
{
    if ( unitType >= Sc::Unit::TotalTypes )
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS/UNIx section!");

    return useExpansionUnitSettings(useExp) ? read.unitSettings.armorLevel[unitType] : read.origUnitSettings.armorLevel[unitType];
}

u16 Scenario::getUnitBuildTime(Sc::Unit::Type unitType, Chk::UseExpSection useExp) const
{
    if ( unitType >= Sc::Unit::TotalTypes )
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS/UNIx section!");

    return useExpansionUnitSettings(useExp) ? read.unitSettings.buildTime[unitType] : read.origUnitSettings.buildTime[unitType];
}

u16 Scenario::getUnitMineralCost(Sc::Unit::Type unitType, Chk::UseExpSection useExp) const
{
    if ( unitType >= Sc::Unit::TotalTypes )
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS/UNIx section!");

    return useExpansionUnitSettings(useExp) ? read.unitSettings.mineralCost[unitType] : read.origUnitSettings.mineralCost[unitType];
}

u16 Scenario::getUnitGasCost(Sc::Unit::Type unitType, Chk::UseExpSection useExp) const
{
    if ( unitType >= Sc::Unit::TotalTypes )
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS/UNIx section!");

    return useExpansionUnitSettings(useExp) ? read.unitSettings.gasCost[unitType] : read.origUnitSettings.gasCost[unitType];
}

u16 Scenario::getWeaponBaseDamage(Sc::Weapon::Type weaponType, Chk::UseExpSection useExp) const
{
    bool useExpansion = useExpansionUnitSettings(useExp, weaponType);
    size_t limit = useExpansion ? Sc::Weapon::Total : Sc::Weapon::TotalOriginal;
    if ( weaponType > limit )
    {
        throw std::out_of_range(std::string("WeaponType: ") + std::to_string((size_t)weaponType) +
            " is out of range for the " + (useExpansion ? "UNIx" : "UNIS") + " section!");
    }
    return useExpansion ? read.unitSettings.baseDamage[weaponType] : read.origUnitSettings.baseDamage[weaponType];
}

u16 Scenario::getWeaponUpgradeDamage(Sc::Weapon::Type weaponType, Chk::UseExpSection useExp) const
{
    bool useExpansion = useExpansionUnitSettings(useExp, weaponType);
    size_t limit = useExpansion ? Sc::Weapon::Total : Sc::Weapon::TotalOriginal;
    if ( weaponType > limit )
    {
        throw std::out_of_range(std::string("WeaponType: ") + std::to_string((size_t)weaponType) +
            " is out of range for the " + (useExpansion ? "UNIx" : "UNIS") + " section!");
    }
    return useExpansion ? read.unitSettings.upgradeDamage[weaponType] : read.origUnitSettings.upgradeDamage[weaponType];
}

void Scenario::setUnitUsesDefaultSettings(Sc::Unit::Type unitType, bool useDefault, Chk::UseExpSection useExp)
{
    if ( unitType >= Sc::Unit::TotalTypes )
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS/UNIx section!");

    auto edit = create_action(ActionDescriptor::ToggleUnitTypeUsesDefaultSettings);
    Chk::UseDefault value = useDefault ? Chk::UseDefault::Yes : Chk::UseDefault::No;
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: edit->unitSettings.useDefault[unitType] = value; edit->origUnitSettings.useDefault[unitType] = value; break;
        case Chk::UseExpSection::Yes: edit->unitSettings.useDefault[unitType] = value; break;
        case Chk::UseExpSection::No: edit->origUnitSettings.useDefault[unitType] = value; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( this->hasSection(Chk::SectionName::UNIx) )
                edit->unitSettings.useDefault[unitType] = value;
            else
                edit->origUnitSettings.useDefault[unitType] = value;
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( this->hasSection(Chk::SectionName::UNIS) )
                edit->origUnitSettings.useDefault[unitType] = value;
            else
                edit->unitSettings.useDefault[unitType] = value;
            break;
    }
}

void Scenario::setUnitHitpoints(Sc::Unit::Type unitType, u32 hitpoints, Chk::UseExpSection useExp)
{
    if ( unitType >= Sc::Unit::TotalTypes )
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS/UNIx section!");
    
    if ( getUnitHitpoints(unitType, useExp) == hitpoints )
        return;

    auto edit = create_action(ActionDescriptor::UpdateUnitTypeHitpoints);
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: edit->unitSettings.hitpoints[unitType] = hitpoints; edit->origUnitSettings.hitpoints[unitType] = hitpoints; break;
        case Chk::UseExpSection::Yes: edit->unitSettings.hitpoints[unitType] = hitpoints; break;
        case Chk::UseExpSection::No: edit->origUnitSettings.hitpoints[unitType] = hitpoints; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( this->hasSection(Chk::SectionName::UNIx) )
                edit->unitSettings.hitpoints[unitType] = hitpoints;
            else
                edit->origUnitSettings.hitpoints[unitType] = hitpoints;
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( this->hasSection(Chk::SectionName::UNIS) )
                edit->origUnitSettings.hitpoints[unitType] = hitpoints;
            else
                edit->unitSettings.hitpoints[unitType] = hitpoints;
            break;
    }
}

void Scenario::setUnitShieldPoints(Sc::Unit::Type unitType, u16 shieldPoints, Chk::UseExpSection useExp)
{
    if ( unitType >= Sc::Unit::TotalTypes )
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS/UNIx section!");
    
    if ( getUnitShieldPoints(unitType, useExp) == shieldPoints )
        return;
    
    auto edit = create_action(ActionDescriptor::UpdateUnitTypeShields);
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: edit->unitSettings.shieldPoints[unitType] = shieldPoints; edit->origUnitSettings.shieldPoints[unitType] = shieldPoints; break;
        case Chk::UseExpSection::Yes: edit->unitSettings.shieldPoints[unitType] = shieldPoints; break;
        case Chk::UseExpSection::No: edit->origUnitSettings.shieldPoints[unitType] = shieldPoints; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( this->hasSection(Chk::SectionName::UNIx) )
                edit->unitSettings.shieldPoints[unitType] = shieldPoints;
            else
                edit->origUnitSettings.shieldPoints[unitType] = shieldPoints;
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( this->hasSection(Chk::SectionName::UNIS) )
                edit->origUnitSettings.shieldPoints[unitType] = shieldPoints;
            else
                edit->unitSettings.shieldPoints[unitType] = shieldPoints;
            break;
    }
}

void Scenario::setUnitArmorLevel(Sc::Unit::Type unitType, u8 armorLevel, Chk::UseExpSection useExp)
{
    if ( unitType >= Sc::Unit::TotalTypes )
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS/UNIx section!");
    
    if ( getUnitArmorLevel(unitType, useExp) == armorLevel )
        return;
    
    auto edit = create_action(ActionDescriptor::UpdateUnitTypeArmor);
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: edit->unitSettings.armorLevel[unitType] = armorLevel; edit->origUnitSettings.armorLevel[unitType] = armorLevel; break;
        case Chk::UseExpSection::Yes: edit->unitSettings.armorLevel[unitType] = armorLevel; break;
        case Chk::UseExpSection::No: edit->origUnitSettings.armorLevel[unitType] = armorLevel; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( this->hasSection(Chk::SectionName::UNIx) )
                edit->unitSettings.armorLevel[unitType] = armorLevel;
            else
                edit->origUnitSettings.armorLevel[unitType] = armorLevel;
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( this->hasSection(Chk::SectionName::UNIS) )
                edit->origUnitSettings.armorLevel[unitType] = armorLevel;
            else
                edit->unitSettings.armorLevel[unitType] = armorLevel;
            break;
    }
}

void Scenario::setUnitBuildTime(Sc::Unit::Type unitType, u16 buildTime, Chk::UseExpSection useExp)
{
    if ( unitType >= Sc::Unit::TotalTypes )
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS/UNIx section!");
    
    if ( getUnitBuildTime(unitType, useExp) == buildTime )
        return;
    
    auto edit = create_action(ActionDescriptor::UpdateUnitTypeBuildTime);
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: edit->unitSettings.buildTime[unitType] = buildTime; edit->origUnitSettings.buildTime[unitType] = buildTime; break;
        case Chk::UseExpSection::Yes: edit->unitSettings.buildTime[unitType] = buildTime; break;
        case Chk::UseExpSection::No: edit->origUnitSettings.buildTime[unitType] = buildTime; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( this->hasSection(Chk::SectionName::UNIx) )
                edit->unitSettings.buildTime[unitType] = buildTime;
            else
                edit->origUnitSettings.buildTime[unitType] = buildTime;
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( this->hasSection(Chk::SectionName::UNIS) )
                edit->origUnitSettings.buildTime[unitType] = buildTime;
            else
                edit->unitSettings.buildTime[unitType] = buildTime;
            break;
    }
}

void Scenario::setUnitMineralCost(Sc::Unit::Type unitType, u16 mineralCost, Chk::UseExpSection useExp)
{
    if ( unitType >= Sc::Unit::TotalTypes )
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS/UNIx section!");
    
    if ( getUnitMineralCost(unitType, useExp) == mineralCost )
        return;
    
    auto edit = create_action(ActionDescriptor::UpdateUnitTypeMineralCost);
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: edit->unitSettings.mineralCost[unitType] = mineralCost; edit->origUnitSettings.mineralCost[unitType] = mineralCost; break;
        case Chk::UseExpSection::Yes: edit->unitSettings.mineralCost[unitType] = mineralCost; break;
        case Chk::UseExpSection::No: edit->origUnitSettings.mineralCost[unitType] = mineralCost; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( this->hasSection(Chk::SectionName::UNIx) )
                edit->unitSettings.mineralCost[unitType] = mineralCost;
            else
                edit->origUnitSettings.mineralCost[unitType] = mineralCost;
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( this->hasSection(Chk::SectionName::UNIS) )
                edit->origUnitSettings.mineralCost[unitType] = mineralCost;
            else
                edit->unitSettings.mineralCost[unitType] = mineralCost;
            break;
    }
}

void Scenario::setUnitGasCost(Sc::Unit::Type unitType, u16 gasCost, Chk::UseExpSection useExp)
{
    if ( unitType >= Sc::Unit::TotalTypes )
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS/UNIx section!");
    
    if ( getUnitGasCost(unitType, useExp) == gasCost )
        return;
    
    auto edit = create_action(ActionDescriptor::UpdateUnitTypeGasCost);
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: edit->unitSettings.gasCost[unitType] = gasCost; edit->origUnitSettings.gasCost[unitType] = gasCost; break;
        case Chk::UseExpSection::Yes: edit->unitSettings.gasCost[unitType] = gasCost; break;
        case Chk::UseExpSection::No: edit->origUnitSettings.gasCost[unitType] = gasCost; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( this->hasSection(Chk::SectionName::UNIx) )
                edit->unitSettings.gasCost[unitType] = gasCost;
            else
                edit->origUnitSettings.gasCost[unitType] = gasCost;
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( this->hasSection(Chk::SectionName::UNIS) )
                edit->origUnitSettings.gasCost[unitType] = gasCost;
            else
                edit->unitSettings.gasCost[unitType] = gasCost;
            break;
    }
}

void Scenario::setWeaponBaseDamage(Sc::Weapon::Type weaponType, u16 baseDamage, Chk::UseExpSection useExp)
{
    if ( getWeaponBaseDamage(weaponType, useExp) == baseDamage )
        return;

    auto edit = create_action(ActionDescriptor::UpdateWeaponBaseDamage);
    auto checkLimit = [&weaponType](bool expansion){
        if ( (expansion && weaponType >= Sc::Weapon::Total) || (!expansion && weaponType >= Sc::Weapon::TotalOriginal) )
            throw std::out_of_range(std::string("WeaponType: ") + std::to_string((size_t)weaponType) +
                " is out of range for the " + (expansion ? "UNIx" : "UNIS") + " section!");
    };

    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( weaponType >= Sc::Weapon::TotalOriginal && read.version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( weaponType < Sc::Weapon::TotalOriginal && hasSection(Chk::SectionName::UNIS) )
                edit->origUnitSettings.baseDamage[weaponType] = baseDamage;
            checkLimit(true);
            edit->unitSettings.baseDamage[weaponType] = baseDamage;
            break;
        case Chk::UseExpSection::Both: checkLimit(false); edit->origUnitSettings.baseDamage[weaponType] = baseDamage; edit->unitSettings.baseDamage[weaponType] = baseDamage; break;
        case Chk::UseExpSection::Yes: checkLimit(true); edit->unitSettings.baseDamage[weaponType] = baseDamage; break;
        case Chk::UseExpSection::No: checkLimit(false); edit->origUnitSettings.baseDamage[weaponType] = baseDamage; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::UNIx) ) {
                checkLimit(true);
                edit->unitSettings.baseDamage[weaponType] = baseDamage;
            } else {
                checkLimit(false);
                edit->origUnitSettings.baseDamage[weaponType] = baseDamage;
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::UNIS) ) {
                checkLimit(false);
                edit->origUnitSettings.baseDamage[weaponType] = baseDamage;
            } else {
                checkLimit(true);
                edit->unitSettings.baseDamage[weaponType] = baseDamage;
            }
            break;
    }
}

void Scenario::setWeaponUpgradeDamage(Sc::Weapon::Type weaponType, u16 upgradeDamage, Chk::UseExpSection useExp)
{
    if ( getWeaponUpgradeDamage(weaponType, useExp) == upgradeDamage )
        return;

    auto edit = create_action(ActionDescriptor::UpdateWeaponUpgradeDamage);
    auto checkLimit = [&weaponType](bool expansion){
        if ( (expansion && weaponType >= Sc::Weapon::Total) || (!expansion && weaponType >= Sc::Weapon::TotalOriginal) )
            throw std::out_of_range(std::string("WeaponType: ") + std::to_string((size_t)weaponType) +
                " is out of range for the " + (expansion ? "UNIx" : "UNIS") + " section!");
    };
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( weaponType >= Sc::Weapon::TotalOriginal && read.version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( weaponType < Sc::Weapon::TotalOriginal && hasSection(Chk::SectionName::UNIS) )
                edit->origUnitSettings.upgradeDamage[weaponType] = upgradeDamage;
            checkLimit(true);
            edit->unitSettings.upgradeDamage[weaponType] = upgradeDamage;
            break;
        case Chk::UseExpSection::Both: checkLimit(false); edit->origUnitSettings.upgradeDamage[weaponType] = upgradeDamage; edit->unitSettings.upgradeDamage[weaponType] = upgradeDamage; break;
        case Chk::UseExpSection::Yes: checkLimit(true); edit->unitSettings.upgradeDamage[weaponType] = upgradeDamage; break;
        case Chk::UseExpSection::No: checkLimit(false); edit->origUnitSettings.upgradeDamage[weaponType] = upgradeDamage; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::UNIx) ) {
                checkLimit(true);
                edit->unitSettings.upgradeDamage[weaponType] = upgradeDamage;
            } else {
                checkLimit(false);
                edit->origUnitSettings.upgradeDamage[weaponType] = upgradeDamage;
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::UNIS) ) {
                checkLimit(false);
                edit->origUnitSettings.upgradeDamage[weaponType] = upgradeDamage;
            } else {
                checkLimit(true);
                edit->unitSettings.upgradeDamage[weaponType] = upgradeDamage;
            }
            break;
    }
}

bool Scenario::isUnitBuildable(Sc::Unit::Type unitType, size_t playerIndex) const
{
    if ( unitType < Sc::Unit::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            return read.unitAvailability.playerUnitBuildable[playerIndex][unitType] != Chk::Available::No;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PUNI section!");
    }
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the PUNI section!");
}

bool Scenario::isUnitDefaultBuildable(Sc::Unit::Type unitType) const
{
    if ( unitType < Sc::Unit::TotalTypes )
        return read.unitAvailability.defaultUnitBuildable[unitType] != Chk::Available::No;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the PUNI section!");
}

bool Scenario::playerUsesDefaultUnitBuildability(Sc::Unit::Type unitType, size_t playerIndex) const
{
    if ( unitType < Sc::Unit::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            return read.unitAvailability.playerUnitUsesDefault[playerIndex][unitType] != Chk::UseDefault::No;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PUNI section!");
    }
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the PUNI section!");
}

void Scenario::setUnitBuildable(Sc::Unit::Type unitType, size_t playerIndex, bool buildable)
{
    Chk::Available unitBuildable = buildable ? Chk::Available::Yes : Chk::Available::No;
    if ( unitType < Sc::Unit::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            create_action(ActionDescriptor::SetUnitTypeBuildable)->unitAvailability.playerUnitBuildable[playerIndex][unitType] = unitBuildable;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PUNI section!");
    }
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the PUNI section!");
}

void Scenario::setUnitDefaultBuildable(Sc::Unit::Type unitType, bool buildable)
{
    Chk::Available unitDefaultBuildable = buildable ? Chk::Available::Yes : Chk::Available::No;
    if ( unitType < Sc::Unit::TotalTypes )
        create_action(ActionDescriptor::SetUnitTypeDefaultBuildable)->unitAvailability.defaultUnitBuildable[unitType] = unitDefaultBuildable;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the PUNI section!");
}

void Scenario::setPlayerUsesDefaultUnitBuildability(Sc::Unit::Type unitType, size_t playerIndex, bool useDefault)
{
    Chk::UseDefault playerUnitUsesDefault = useDefault ? Chk::UseDefault::Yes : Chk::UseDefault::No;
    if ( unitType < Sc::Unit::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            create_action(ActionDescriptor::SetPlayerUsesDefaultUnitBuildability)->unitAvailability.playerUnitUsesDefault[playerIndex][unitType] = playerUnitUsesDefault;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PUNI section!");
    }
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the PUNI section!");
}

void Scenario::setUnitsToDefault(Chk::UseExpSection useExp)
{
    auto edit = create_action(ActionDescriptor::ResetAllUnitProperites);
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both:
            edit->origUnitSettings = Chk::UNIS {};
            edit->unitSettings = Chk::UNIx {};
            edit->unitAvailability = Chk::PUNI {};
            break;
        case Chk::UseExpSection::Yes:
            edit->unitSettings = Chk::UNIx {};
            edit->unitAvailability = Chk::PUNI {};
            break;
        case Chk::UseExpSection::No:
            edit->origUnitSettings = Chk::UNIS {};
            edit->unitAvailability = Chk::PUNI {};
            break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::UNIx) )
                edit->unitSettings = Chk::UNIx {};
            else
                edit->origUnitSettings = Chk::UNIS {};
            
            edit->unitAvailability = Chk::PUNI {};
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::UNIS) )
                edit->origUnitSettings = Chk::UNIS {};
            else
                edit->unitSettings = Chk::UNIx {};
            
            edit->unitAvailability = Chk::PUNI {};
            break;
    }
}

bool Scenario::useExpansionUpgradeCosts(Chk::UseExpSection useExp, Sc::Upgrade::Type upgradeType) const
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto: return upgradeType >= Sc::Upgrade::Type::UnusedUpgrade_46 || this->isHybridOrAbove();
        case Chk::UseExpSection::Yes: return true;
        case Chk::UseExpSection::No: return false;
        case Chk::UseExpSection::NoIfOrigAvailable: return this->hasSection(Chk::SectionName::UPGS) ? false : true;
        case Chk::UseExpSection::YesIfAvailable:
        default: return this->hasSection(Chk::SectionName::UPGx);
    }
}

bool Scenario::upgradeUsesDefaultCosts(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp) const
{
    bool expansion = useExpansionUpgradeCosts(useExp, upgradeType);
    if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
    {
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) +
            " is out of range for the " + (expansion ? "UPGx" : "UPGS") + " section!");
    }
    return expansion ? read.upgradeCosts.useDefault[upgradeType] : read.origUpgradeCosts.useDefault[upgradeType];
}

u16 Scenario::getUpgradeBaseMineralCost(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp) const
{
    bool expansion = useExpansionUpgradeCosts(useExp, upgradeType);
    if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
    {
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) +
            " is out of range for the " + (expansion ? "UPGx" : "UPGS") + " section!");
    }
    return expansion ? read.upgradeCosts.baseMineralCost[upgradeType] : read.origUpgradeCosts.baseMineralCost[upgradeType];
}

u16 Scenario::getUpgradeMineralCostFactor(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp) const
{
    bool expansion = useExpansionUpgradeCosts(useExp, upgradeType);
    if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
    {
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) +
            " is out of range for the " + (expansion ? "UPGx" : "UPGS") + " section!");
    }
    return expansion ? read.upgradeCosts.mineralCostFactor[upgradeType] : read.origUpgradeCosts.mineralCostFactor[upgradeType];
}

u16 Scenario::getUpgradeBaseGasCost(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp) const
{
    bool expansion = useExpansionUpgradeCosts(useExp, upgradeType);
    if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
    {
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) +
            " is out of range for the " + (expansion ? "UPGx" : "UPGS") + " section!");
    }
    return expansion ? read.upgradeCosts.baseGasCost[upgradeType] : read.origUpgradeCosts.baseGasCost[upgradeType];
}

u16 Scenario::getUpgradeGasCostFactor(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp) const
{
    bool expansion = useExpansionUpgradeCosts(useExp, upgradeType);
    if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
    {
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) +
            " is out of range for the " + (expansion ? "UPGx" : "UPGS") + " section!");
    }
    return expansion ? read.upgradeCosts.gasCostFactor[upgradeType] : read.origUpgradeCosts.gasCostFactor[upgradeType];
}

u16 Scenario::getUpgradeBaseResearchTime(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp) const
{
    bool expansion = useExpansionUpgradeCosts(useExp, upgradeType);
    if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
    {
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) +
            " is out of range for the " + (expansion ? "UPGx" : "UPGS") + " section!");
    }
    return expansion ? read.upgradeCosts.baseResearchTime[upgradeType] : read.origUpgradeCosts.baseResearchTime[upgradeType];
}

u16 Scenario::getUpgradeResearchTimeFactor(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp) const
{
    bool expansion = useExpansionUpgradeCosts(useExp, upgradeType);
    if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
    {
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) +
            " is out of range for the " + (expansion ? "UPGx" : "UPGS") + " section!");
    }
    return expansion ? read.upgradeCosts.researchTimeFactor[upgradeType] : read.origUpgradeCosts.researchTimeFactor[upgradeType];
}

void Scenario::setUpgradeUsesDefaultCosts(Sc::Upgrade::Type upgradeType, bool useDefault, Chk::UseExpSection useExp)
{
    auto edit = create_action(ActionDescriptor::SetUpgradeUsesDefaultCosts);
    auto checkLimit = [&upgradeType](bool expansion){
        if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
            throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) +
                " is out of range for the " + (expansion ? "UPGx" : "UPGS") + " section!");
    };
    Chk::UseDefault value = useDefault ? Chk::UseDefault::Yes : Chk::UseDefault::No;
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( upgradeType >= Sc::Upgrade::Type::UnusedUpgrade_46 && read.version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( upgradeType < Sc::Upgrade::TotalOriginalTypes && hasSection(Chk::SectionName::UNIS) )
                edit->origUpgradeCosts.useDefault[upgradeType] = value;
            checkLimit(true);
            edit->upgradeCosts.useDefault[upgradeType] = value;
            break;
        case Chk::UseExpSection::Both: checkLimit(false); edit->origUpgradeCosts.useDefault[upgradeType] = value; edit->upgradeCosts.useDefault[upgradeType] = value; break;
        case Chk::UseExpSection::Yes: checkLimit(true); edit->upgradeCosts.useDefault[upgradeType] = value; break;
        case Chk::UseExpSection::No: checkLimit(false); edit->origUpgradeCosts.useDefault[upgradeType] = value; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::UNIx) ) {
                checkLimit(true);
                edit->upgradeCosts.useDefault[upgradeType] = value;
            } else {
                checkLimit(false);
                edit->origUpgradeCosts.useDefault[upgradeType] = value;
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::UNIS) ) {
                checkLimit(false);
                edit->origUpgradeCosts.useDefault[upgradeType] = value;
            } else {
                checkLimit(true);
                edit->upgradeCosts.useDefault[upgradeType] = value;
            }
            break;
    }
}

void Scenario::setUpgradeBaseMineralCost(Sc::Upgrade::Type upgradeType, u16 baseMineralCost, Chk::UseExpSection useExp)
{
    auto edit = create_action(ActionDescriptor::SetUpgradeBaseMineralCost);
    auto checkLimit = [&upgradeType](bool expansion){
        if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
            throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) +
                " is out of range for the " + (expansion ? "UPGx" : "UPGS") + " section!");
    };
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( upgradeType >= Sc::Upgrade::Type::UnusedUpgrade_46 && read.version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( upgradeType < Sc::Upgrade::TotalOriginalTypes && hasSection(Chk::SectionName::UNIS) )
                edit->origUpgradeCosts.baseMineralCost[upgradeType] = baseMineralCost;
            checkLimit(true);
            edit->upgradeCosts.baseMineralCost[upgradeType] = baseMineralCost;
            break;
        case Chk::UseExpSection::Both: checkLimit(false); edit->origUpgradeCosts.baseMineralCost[upgradeType] = baseMineralCost; edit->upgradeCosts.baseMineralCost[upgradeType] = baseMineralCost; break;
        case Chk::UseExpSection::Yes: checkLimit(true); edit->upgradeCosts.baseMineralCost[upgradeType] = baseMineralCost; break;
        case Chk::UseExpSection::No: checkLimit(false); edit->origUpgradeCosts.baseMineralCost[upgradeType] = baseMineralCost; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::UNIx) ) {
                checkLimit(true);
                edit->upgradeCosts.baseMineralCost[upgradeType] = baseMineralCost;
            } else {
                checkLimit(false);
                edit->origUpgradeCosts.baseMineralCost[upgradeType] = baseMineralCost;
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::UNIS) ) {
                checkLimit(false);
                edit->origUpgradeCosts.baseMineralCost[upgradeType] = baseMineralCost;
            } else {
                checkLimit(true);
                edit->upgradeCosts.baseMineralCost[upgradeType] = baseMineralCost;
            }
            break;
    }
}

void Scenario::setUpgradeMineralCostFactor(Sc::Upgrade::Type upgradeType, u16 mineralCostFactor, Chk::UseExpSection useExp)
{
    auto edit = create_action(ActionDescriptor::SetUpgradeMineralCostFactor);
    auto checkLimit = [&upgradeType](bool expansion){
        if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
            throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) +
                " is out of range for the " + (expansion ? "UPGx" : "UPGS") + " section!");
    };
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( upgradeType >= Sc::Upgrade::Type::UnusedUpgrade_46 && read.version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( upgradeType < Sc::Upgrade::TotalOriginalTypes && hasSection(Chk::SectionName::UNIS) )
                edit->origUpgradeCosts.mineralCostFactor[upgradeType] = mineralCostFactor;
            checkLimit(true);
            edit->upgradeCosts.mineralCostFactor[upgradeType] = mineralCostFactor;
            break;
        case Chk::UseExpSection::Both: checkLimit(false); edit->origUpgradeCosts.mineralCostFactor[upgradeType] = mineralCostFactor; edit->upgradeCosts.mineralCostFactor[upgradeType] = mineralCostFactor; break;
        case Chk::UseExpSection::Yes: checkLimit(true); edit->upgradeCosts.mineralCostFactor[upgradeType] = mineralCostFactor; break;
        case Chk::UseExpSection::No: checkLimit(false); edit->origUpgradeCosts.mineralCostFactor[upgradeType] = mineralCostFactor; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::UNIx) ) {
                checkLimit(true);
                edit->upgradeCosts.mineralCostFactor[upgradeType] = mineralCostFactor;
            } else {
                checkLimit(false);
                edit->origUpgradeCosts.mineralCostFactor[upgradeType] = mineralCostFactor;
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::UNIS) ) {
                checkLimit(false);
                edit->origUpgradeCosts.mineralCostFactor[upgradeType] = mineralCostFactor;
            } else {
                checkLimit(true);
                edit->upgradeCosts.mineralCostFactor[upgradeType] = mineralCostFactor;
            }
            break;
    }
}

void Scenario::setUpgradeBaseGasCost(Sc::Upgrade::Type upgradeType, u16 baseGasCost, Chk::UseExpSection useExp)
{
    auto edit = create_action(ActionDescriptor::SetUpgradeBaseGasCost);
    auto checkLimit = [&upgradeType](bool expansion){
        if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
            throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) +
                " is out of range for the " + (expansion ? "UPGx" : "UPGS") + " section!");
    };
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( upgradeType >= Sc::Upgrade::Type::UnusedUpgrade_46 && read.version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( upgradeType < Sc::Upgrade::TotalOriginalTypes && hasSection(Chk::SectionName::UNIS) )
                edit->origUpgradeCosts.baseGasCost[upgradeType] = baseGasCost;
            checkLimit(true);
            edit->upgradeCosts.baseGasCost[upgradeType] = baseGasCost;
            break;
        case Chk::UseExpSection::Both: checkLimit(false); edit->origUpgradeCosts.baseGasCost[upgradeType] = baseGasCost; edit->upgradeCosts.baseGasCost[upgradeType] = baseGasCost; break;
        case Chk::UseExpSection::Yes: checkLimit(true); edit->upgradeCosts.baseGasCost[upgradeType] = baseGasCost; break;
        case Chk::UseExpSection::No: checkLimit(false); edit->origUpgradeCosts.baseGasCost[upgradeType] = baseGasCost; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::UNIx) ) {
                checkLimit(true);
                edit->upgradeCosts.baseGasCost[upgradeType] = baseGasCost;
            } else {
                checkLimit(false);
                edit->origUpgradeCosts.baseGasCost[upgradeType] = baseGasCost;
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::UNIS) ) {
                checkLimit(false);
                edit->origUpgradeCosts.baseGasCost[upgradeType] = baseGasCost;
            } else {
                checkLimit(true);
                edit->upgradeCosts.baseGasCost[upgradeType] = baseGasCost;
            }
            break;
    }
}

void Scenario::setUpgradeGasCostFactor(Sc::Upgrade::Type upgradeType, u16 gasCostFactor, Chk::UseExpSection useExp)
{
    auto edit = create_action(ActionDescriptor::SetUpgradeGasCostFactor);
    auto checkLimit = [&upgradeType](bool expansion){
        if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
            throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) +
                " is out of range for the " + (expansion ? "UPGx" : "UPGS") + " section!");
    };
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( upgradeType >= Sc::Upgrade::Type::UnusedUpgrade_46 && read.version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( upgradeType < Sc::Upgrade::TotalOriginalTypes && hasSection(Chk::SectionName::UNIS) )
                edit->origUpgradeCosts.gasCostFactor[upgradeType] = gasCostFactor;
            checkLimit(true);
            edit->upgradeCosts.gasCostFactor[upgradeType] = gasCostFactor;
            break;
        case Chk::UseExpSection::Both: checkLimit(false); edit->origUpgradeCosts.gasCostFactor[upgradeType] = gasCostFactor; edit->upgradeCosts.gasCostFactor[upgradeType] = gasCostFactor; break;
        case Chk::UseExpSection::Yes: checkLimit(true); edit->upgradeCosts.gasCostFactor[upgradeType] = gasCostFactor; break;
        case Chk::UseExpSection::No: checkLimit(false); edit->origUpgradeCosts.gasCostFactor[upgradeType] = gasCostFactor; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::UNIx) ) {
                checkLimit(true);
                edit->upgradeCosts.gasCostFactor[upgradeType] = gasCostFactor;
            } else {
                checkLimit(false);
                edit->origUpgradeCosts.gasCostFactor[upgradeType] = gasCostFactor;
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::UNIS) ) {
                checkLimit(false);
                edit->origUpgradeCosts.gasCostFactor[upgradeType] = gasCostFactor;
            } else {
                checkLimit(true);
                edit->upgradeCosts.gasCostFactor[upgradeType] = gasCostFactor;
            }
            break;
    }
}

void Scenario::setUpgradeBaseResearchTime(Sc::Upgrade::Type upgradeType, u16 baseResearchTime, Chk::UseExpSection useExp)
{
    auto edit = create_action(ActionDescriptor::SetUpgradeBaseResearchTime);
    auto checkLimit = [&upgradeType](bool expansion){
        if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
            throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) +
                " is out of range for the " + (expansion ? "UPGx" : "UPGS") + " section!");
    };
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( upgradeType >= Sc::Upgrade::Type::UnusedUpgrade_46 && read.version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( upgradeType < Sc::Upgrade::TotalOriginalTypes && hasSection(Chk::SectionName::UNIS) )
                edit->origUpgradeCosts.baseResearchTime[upgradeType] = baseResearchTime;
            checkLimit(true);
            edit->upgradeCosts.baseResearchTime[upgradeType] = baseResearchTime;
            break;
        case Chk::UseExpSection::Both: checkLimit(false); edit->origUpgradeCosts.baseResearchTime[upgradeType] = baseResearchTime; edit->upgradeCosts.baseResearchTime[upgradeType] = baseResearchTime; break;
        case Chk::UseExpSection::Yes: checkLimit(true); edit->upgradeCosts.baseResearchTime[upgradeType] = baseResearchTime; break;
        case Chk::UseExpSection::No: checkLimit(false); edit->origUpgradeCosts.baseResearchTime[upgradeType] = baseResearchTime; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::UNIx) ) {
                checkLimit(true);
                edit->upgradeCosts.baseResearchTime[upgradeType] = baseResearchTime;
            } else {
                checkLimit(false);
                edit->origUpgradeCosts.baseResearchTime[upgradeType] = baseResearchTime;
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::UNIS) ) {
                checkLimit(false);
                edit->origUpgradeCosts.baseResearchTime[upgradeType] = baseResearchTime;
            } else {
                checkLimit(true);
                edit->upgradeCosts.baseResearchTime[upgradeType] = baseResearchTime;
            }
            break;
    }
}

void Scenario::setUpgradeResearchTimeFactor(Sc::Upgrade::Type upgradeType, u16 researchTimeFactor, Chk::UseExpSection useExp)
{
    auto edit = create_action(ActionDescriptor::SetUpgradeResearchTimeFactor);
    auto checkLimit = [&upgradeType](bool expansion){
        if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
            throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) +
                " is out of range for the " + (expansion ? "UPGx" : "UPGS") + " section!");
    };
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( upgradeType >= Sc::Upgrade::Type::UnusedUpgrade_46 && read.version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( upgradeType < Sc::Upgrade::TotalOriginalTypes && hasSection(Chk::SectionName::UNIS) )
                edit->origUpgradeCosts.researchTimeFactor[upgradeType] = researchTimeFactor;
            checkLimit(true);
            edit->upgradeCosts.researchTimeFactor[upgradeType] = researchTimeFactor;
            break;
        case Chk::UseExpSection::Both: checkLimit(false); edit->origUpgradeCosts.researchTimeFactor[upgradeType] = researchTimeFactor; edit->upgradeCosts.researchTimeFactor[upgradeType] = researchTimeFactor; break;
        case Chk::UseExpSection::Yes: checkLimit(true); edit->upgradeCosts.researchTimeFactor[upgradeType] = researchTimeFactor; break;
        case Chk::UseExpSection::No: checkLimit(false); edit->origUpgradeCosts.researchTimeFactor[upgradeType] = researchTimeFactor; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::UNIx) ) {
                checkLimit(true);
                edit->upgradeCosts.researchTimeFactor[upgradeType] = researchTimeFactor;
            } else {
                checkLimit(false);
                edit->origUpgradeCosts.researchTimeFactor[upgradeType] = researchTimeFactor;
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::UNIS) ) {
                checkLimit(false);
                edit->origUpgradeCosts.researchTimeFactor[upgradeType] = researchTimeFactor;
            } else {
                checkLimit(true);
                edit->upgradeCosts.researchTimeFactor[upgradeType] = researchTimeFactor;
            }
            break;
    }
}

bool Scenario::useExpansionUpgradeLeveling(Chk::UseExpSection useExp, Sc::Upgrade::Type upgradeType) const
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto: return upgradeType >= Sc::Upgrade::Type::UnusedUpgrade_46 || this->isHybridOrAbove();
        case Chk::UseExpSection::Yes: return true;
        case Chk::UseExpSection::No: return false;
        case Chk::UseExpSection::NoIfOrigAvailable: return !this->hasSection(Chk::SectionName::UPGR);
        case Chk::UseExpSection::YesIfAvailable:
        default: return this->hasSection(Chk::SectionName::PUPx);
    }
}

size_t Scenario::getMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, Chk::UseExpSection useExp) const
{
    bool expansion = useExpansionUpgradeLeveling(useExp, upgradeType);
    if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the " + (expansion ? "PUPx" : "UPGR") + " section!");
    else if ( playerIndex >= Sc::Player::Total )
        throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the " + (expansion ? "PUPx" : "UPGR") + " section!");

    return expansion ? read.upgradeLeveling.playerMaxUpgradeLevel[playerIndex][upgradeType] : read.origUpgradeLeveling.playerMaxUpgradeLevel[playerIndex][upgradeType];
}

size_t Scenario::getStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, Chk::UseExpSection useExp) const
{
    bool expansion = useExpansionUpgradeLeveling(useExp, upgradeType);
    if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the " + (expansion ? "PUPx" : "UPGR") + " section!");
    else if ( playerIndex >= Sc::Player::Total )
        throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the " + (expansion ? "PUPx" : "UPGR") + " section!");

    return expansion ? read.upgradeLeveling.playerStartUpgradeLevel[playerIndex][upgradeType] : read.origUpgradeLeveling.playerStartUpgradeLevel[playerIndex][upgradeType];
}

size_t Scenario::getDefaultMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp) const
{
    bool expansion = useExpansionUpgradeLeveling(useExp, upgradeType);
    if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the " + (expansion ? "PUPx" : "UPGR") + " section!");

    return expansion ? read.upgradeLeveling.defaultMaxLevel[upgradeType] : read.origUpgradeLeveling.defaultMaxLevel[upgradeType];
}

size_t Scenario::getDefaultStartUpgradeLevel(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp) const
{
    bool expansion = useExpansionUpgradeLeveling(useExp, upgradeType);
    if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the " + (expansion ? "PUPx" : "UPGR") + " section!");

    return expansion ? read.upgradeLeveling.defaultStartLevel[upgradeType] : read.origUpgradeLeveling.defaultStartLevel[upgradeType];
}

bool Scenario::playerUsesDefaultUpgradeLeveling(Sc::Upgrade::Type upgradeType, size_t playerIndex, Chk::UseExpSection useExp) const
{
    
    bool expansion = useExpansionUpgradeLeveling(useExp, upgradeType);
    if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the " + (expansion ? "PUPx" : "UPGR") + " section!");
    else if ( playerIndex >= Sc::Player::Total )
        throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the " + (expansion ? "PUPx" : "UPGR") + " section!");

    return expansion ? read.upgradeLeveling.playerUpgradeUsesDefault[playerIndex][upgradeType] != Chk::UseDefault::No :
        read.origUpgradeLeveling.playerUpgradeUsesDefault[playerIndex][upgradeType] != Chk::UseDefault::No;
}

void Scenario::setMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, size_t maxUpgradeLevel, Chk::UseExpSection useExp)
{
    auto edit = create_action(ActionDescriptor::SetMaxUpgradeLevel);
    auto checkLimit = [&upgradeType, &playerIndex](bool expansion){
        if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
            throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the " + (expansion ? "PUPx" : "UPGR") + " section!");
        else if ( playerIndex >= Sc::Player::Total )
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the " + (expansion ? "PUPx" : "UPGR") + " section!");
    };
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( upgradeType >= Sc::Upgrade::Type::UnusedUpgrade_46 && read.version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( upgradeType < Sc::Upgrade::TotalOriginalTypes && hasSection(Chk::SectionName::UPGR) )
                edit->origUpgradeLeveling.playerMaxUpgradeLevel[playerIndex][upgradeType] = u8(maxUpgradeLevel);
            checkLimit(true);
            edit->upgradeLeveling.playerMaxUpgradeLevel[playerIndex][upgradeType] = u8(maxUpgradeLevel);
            break;
        case Chk::UseExpSection::Both: checkLimit(false); edit->origUpgradeLeveling.playerMaxUpgradeLevel[playerIndex][upgradeType] = u8(maxUpgradeLevel); edit->upgradeLeveling.playerMaxUpgradeLevel[playerIndex][upgradeType] = u8(maxUpgradeLevel); break;
        case Chk::UseExpSection::Yes: checkLimit(true); edit->upgradeLeveling.playerMaxUpgradeLevel[playerIndex][upgradeType] = u8(maxUpgradeLevel); break;
        case Chk::UseExpSection::No: checkLimit(false); edit->origUpgradeLeveling.playerMaxUpgradeLevel[playerIndex][upgradeType] = u8(maxUpgradeLevel); break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::PUPx) ) {
                checkLimit(true);
                edit->upgradeLeveling.playerMaxUpgradeLevel[playerIndex][upgradeType] = u8(maxUpgradeLevel);
            } else {
                checkLimit(false);
                edit->origUpgradeLeveling.playerMaxUpgradeLevel[playerIndex][upgradeType] = u8(maxUpgradeLevel);
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::UPGR) ) {
                checkLimit(false);
                edit->origUpgradeLeveling.playerMaxUpgradeLevel[playerIndex][upgradeType] = u8(maxUpgradeLevel);
            } else {
                checkLimit(true);
                edit->upgradeLeveling.playerMaxUpgradeLevel[playerIndex][upgradeType] = u8(maxUpgradeLevel);
            }
            break;
    }
}

void Scenario::setStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, size_t startUpgradeLevel, Chk::UseExpSection useExp)
{
    auto edit = create_action(ActionDescriptor::SetStartUpgradeLevel);
    auto checkLimit = [&upgradeType, &playerIndex](bool expansion){
        if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
            throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the " + (expansion ? "PUPx" : "UPGR") + " section!");
        else if ( playerIndex >= Sc::Player::Total )
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the " + (expansion ? "PUPx" : "UPGR") + " section!");
    };
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( upgradeType >= Sc::Upgrade::Type::UnusedUpgrade_46 && read.version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( upgradeType < Sc::Upgrade::TotalOriginalTypes && hasSection(Chk::SectionName::UPGR) )
                edit->origUpgradeLeveling.playerStartUpgradeLevel[playerIndex][upgradeType] = u8(startUpgradeLevel);
            checkLimit(true);
            edit->upgradeLeveling.playerStartUpgradeLevel[playerIndex][upgradeType] = u8(startUpgradeLevel);
            break;
        case Chk::UseExpSection::Both: checkLimit(false); edit->origUpgradeLeveling.playerStartUpgradeLevel[playerIndex][upgradeType] = u8(startUpgradeLevel); edit->upgradeLeveling.playerStartUpgradeLevel[playerIndex][upgradeType] = u8(startUpgradeLevel); break;
        case Chk::UseExpSection::Yes: checkLimit(true); edit->upgradeLeveling.playerStartUpgradeLevel[playerIndex][upgradeType] = u8(startUpgradeLevel); break;
        case Chk::UseExpSection::No: checkLimit(false); edit->origUpgradeLeveling.playerStartUpgradeLevel[playerIndex][upgradeType] = u8(startUpgradeLevel); break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::PUPx) ) {
                checkLimit(true);
                edit->upgradeLeveling.playerStartUpgradeLevel[playerIndex][upgradeType] = u8(startUpgradeLevel);
            } else {
                checkLimit(false);
                edit->origUpgradeLeveling.playerStartUpgradeLevel[playerIndex][upgradeType] = u8(startUpgradeLevel);
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::UPGR) ) {
                checkLimit(false);
                edit->origUpgradeLeveling.playerStartUpgradeLevel[playerIndex][upgradeType] = u8(startUpgradeLevel);
            } else {
                checkLimit(true);
                edit->upgradeLeveling.playerStartUpgradeLevel[playerIndex][upgradeType] = u8(startUpgradeLevel);
            }
            break;
    }
}

void Scenario::setDefaultMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t maxUpgradeLevel, Chk::UseExpSection useExp)
{
    auto edit = create_action(ActionDescriptor::SetDefaultMaxUpgradeLevel);
    auto checkLimit = [&upgradeType](bool expansion){
        if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
            throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the " + (expansion ? "PUPx" : "UPGR") + " section!");
    };
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( upgradeType >= Sc::Upgrade::Type::UnusedUpgrade_46 && read.version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( upgradeType < Sc::Upgrade::TotalOriginalTypes && hasSection(Chk::SectionName::UPGR) )
                edit->origUpgradeLeveling.defaultMaxLevel[upgradeType] = u8(maxUpgradeLevel);
            checkLimit(true);
            edit->upgradeLeveling.defaultMaxLevel[upgradeType] = u8(maxUpgradeLevel);
            break;
        case Chk::UseExpSection::Both: checkLimit(false); edit->origUpgradeLeveling.defaultMaxLevel[upgradeType] = u8(maxUpgradeLevel); edit->upgradeLeveling.defaultMaxLevel[upgradeType] = u8(maxUpgradeLevel); break;
        case Chk::UseExpSection::Yes: checkLimit(true); edit->upgradeLeveling.defaultMaxLevel[upgradeType] = u8(maxUpgradeLevel); break;
        case Chk::UseExpSection::No: checkLimit(false); edit->origUpgradeLeveling.defaultMaxLevel[upgradeType] = u8(maxUpgradeLevel); break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::PUPx) ) {
                checkLimit(true);
                edit->upgradeLeveling.defaultMaxLevel[upgradeType] = u8(maxUpgradeLevel);
            } else {
                checkLimit(false);
                edit->origUpgradeLeveling.defaultMaxLevel[upgradeType] = u8(maxUpgradeLevel);
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::UPGR) ) {
                checkLimit(false);
                edit->origUpgradeLeveling.defaultMaxLevel[upgradeType] = u8(maxUpgradeLevel);
            } else {
                checkLimit(true);
                edit->upgradeLeveling.defaultMaxLevel[upgradeType] = u8(maxUpgradeLevel);
            }
            break;
    }
}

void Scenario::setDefaultStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t startUpgradeLevel, Chk::UseExpSection useExp)
{
    auto edit = create_action(ActionDescriptor::SetDefaultStartUpgradeLevel);
    auto checkLimit = [&upgradeType](bool expansion){
        if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
            throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the " + (expansion ? "PUPx" : "UPGR") + " section!");
    };
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( upgradeType >= Sc::Upgrade::Type::UnusedUpgrade_46 && read.version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( upgradeType < Sc::Upgrade::TotalOriginalTypes && hasSection(Chk::SectionName::UPGR) )
                edit->origUpgradeLeveling.defaultStartLevel[upgradeType] = u8(startUpgradeLevel);
            checkLimit(true);
            edit->upgradeLeveling.defaultStartLevel[upgradeType] = u8(startUpgradeLevel);
            break;
        case Chk::UseExpSection::Both: checkLimit(false); edit->origUpgradeLeveling.defaultStartLevel[upgradeType] = u8(startUpgradeLevel); edit->upgradeLeveling.defaultStartLevel[upgradeType] = u8(startUpgradeLevel); break;
        case Chk::UseExpSection::Yes: checkLimit(true); edit->upgradeLeveling.defaultStartLevel[upgradeType] = u8(startUpgradeLevel); break;
        case Chk::UseExpSection::No: checkLimit(false); edit->origUpgradeLeveling.defaultStartLevel[upgradeType] = u8(startUpgradeLevel); break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::PUPx) ) {
                checkLimit(true);
                edit->upgradeLeveling.defaultStartLevel[upgradeType] = u8(startUpgradeLevel);
            } else {
                checkLimit(false);
                edit->origUpgradeLeveling.defaultStartLevel[upgradeType] = u8(startUpgradeLevel);
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::UPGR) ) {
                checkLimit(false);
                edit->origUpgradeLeveling.defaultStartLevel[upgradeType] = u8(startUpgradeLevel);
            } else {
                checkLimit(true);
                edit->upgradeLeveling.defaultStartLevel[upgradeType] = u8(startUpgradeLevel);
            }
            break;
    }
}

void Scenario::setPlayerUsesDefaultUpgradeLeveling(Sc::Upgrade::Type upgradeType, size_t playerIndex, bool useDefault, Chk::UseExpSection useExp)
{
    auto edit = create_action(ActionDescriptor::SetPlayerUsesDefaultUpgradeLeveling);
    auto checkLimit = [&upgradeType, &playerIndex](bool expansion){
        if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
            throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the " + (expansion ? "PUPx" : "UPGR") + " section!");
        else if ( playerIndex >= Sc::Player::Total )
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the " + (expansion ? "PUPx" : "UPGR") + " section!");
    };
    Chk::UseDefault value = useDefault ? Chk::UseDefault::Yes : Chk::UseDefault::No;
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( upgradeType >= Sc::Upgrade::Type::UnusedUpgrade_46 && read.version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( upgradeType < Sc::Upgrade::TotalOriginalTypes && hasSection(Chk::SectionName::UPGR) )
                edit->origUpgradeLeveling.playerUpgradeUsesDefault[playerIndex][upgradeType] = value;
            checkLimit(true);
            edit->upgradeLeveling.playerUpgradeUsesDefault[playerIndex][upgradeType] = value;
            break;
        case Chk::UseExpSection::Both: checkLimit(false); edit->origUpgradeLeveling.playerUpgradeUsesDefault[playerIndex][upgradeType] = value; edit->upgradeLeveling.playerUpgradeUsesDefault[playerIndex][upgradeType] = value; break;
        case Chk::UseExpSection::Yes: checkLimit(true); edit->upgradeLeveling.playerUpgradeUsesDefault[playerIndex][upgradeType] = value; break;
        case Chk::UseExpSection::No: checkLimit(false); edit->origUpgradeLeveling.playerUpgradeUsesDefault[playerIndex][upgradeType] = value; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::PUPx) ) {
                checkLimit(true);
                edit->upgradeLeveling.playerUpgradeUsesDefault[playerIndex][upgradeType] = value;
            } else {
                checkLimit(false);
                edit->origUpgradeLeveling.playerUpgradeUsesDefault[playerIndex][upgradeType] = value;
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::UPGR) ) {
                checkLimit(false);
                edit->origUpgradeLeveling.playerUpgradeUsesDefault[playerIndex][upgradeType] = value;
            } else {
                checkLimit(true);
                edit->upgradeLeveling.playerUpgradeUsesDefault[playerIndex][upgradeType] = value;
            }
            break;
    }
}

void Scenario::setUpgradesToDefault(Chk::UseExpSection useExp)
{
    auto edit = create_action(ActionDescriptor::ResetAllUpgradeProperties);
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both:
            edit->origUpgradeCosts = Chk::UPGS {};
            edit->upgradeCosts = Chk::UPGx {};
            edit->origUpgradeLeveling = Chk::UPGR {};
            edit->upgradeLeveling = Chk::PUPx {};
            break;
        case Chk::UseExpSection::Yes:
            edit->upgradeCosts = Chk::UPGx {};
            edit->upgradeLeveling = Chk::PUPx {};
            break;
        case Chk::UseExpSection::No:
            edit->origUpgradeCosts = Chk::UPGS {};
            edit->origUpgradeLeveling = Chk::UPGR {};
            break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( this->hasSection(Chk::SectionName::UPGx) )
                edit->upgradeCosts = Chk::UPGx {};
            else
                edit->origUpgradeCosts = Chk::UPGS {};

            if ( this->hasSection(Chk::SectionName::PUPx) )
                edit->upgradeLeveling = Chk::PUPx {};
            else
                edit->origUpgradeLeveling = Chk::UPGR {};
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( this->hasSection(Chk::SectionName::UPGS) )
                edit->origUpgradeCosts = Chk::UPGS {};
            else
                edit->upgradeCosts = Chk::UPGx {};

            if ( this->hasSection(Chk::SectionName::UPGR) )
                edit->origUpgradeLeveling = Chk::UPGR {};
            else
                edit->upgradeLeveling = Chk::PUPx {};
            break;
    }
}

bool Scenario::useExpansionTechCosts(Chk::UseExpSection useExp, Sc::Tech::Type techType) const
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto: return techType >= Sc::Tech::Type::Restoration || this->isHybridOrAbove();
        case Chk::UseExpSection::Yes: return true;
        case Chk::UseExpSection::No: return false;
        case Chk::UseExpSection::NoIfOrigAvailable: return !this->hasSection(Chk::SectionName::TECS);
        case Chk::UseExpSection::YesIfAvailable:
        default: return this->hasSection(Chk::SectionName::TECx);
    }
}

bool Scenario::techUsesDefaultSettings(Sc::Tech::Type techType, Chk::UseExpSection useExp) const
{
    bool expansion = useExpansionTechCosts(useExp, techType);
    if ( (expansion && techType >= Sc::Tech::TotalTypes) || (!expansion && techType >= Sc::Tech::TotalOriginalTypes) )
    {
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) +
            " is out of range for the " + (expansion ? "TECx" : "TECS") + " section!");
    }
    return expansion ? read.techCosts.useDefault[techType] != Chk::UseDefault::No : read.techCosts.useDefault[techType] != Chk::UseDefault::No;
}

u16 Scenario::getTechMineralCost(Sc::Tech::Type techType, Chk::UseExpSection useExp) const
{
    bool expansion = useExpansionTechCosts(useExp, techType);
    if ( (expansion && techType >= Sc::Tech::TotalTypes) || (!expansion && techType >= Sc::Tech::TotalOriginalTypes) )
    {
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) +
            " is out of range for the " + (expansion ? "TECx" : "TECS") + " section!");
    }
    return expansion ? read.techCosts.mineralCost[techType] : read.techCosts.mineralCost[techType];
}

u16 Scenario::getTechGasCost(Sc::Tech::Type techType, Chk::UseExpSection useExp) const
{
    bool expansion = useExpansionTechCosts(useExp, techType);
    if ( (expansion && techType >= Sc::Tech::TotalTypes) || (!expansion && techType >= Sc::Tech::TotalOriginalTypes) )
    {
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) +
            " is out of range for the " + (expansion ? "TECx" : "TECS") + " section!");
    }
    return expansion ? read.techCosts.gasCost[techType] : read.techCosts.gasCost[techType];
}

u16 Scenario::getTechResearchTime(Sc::Tech::Type techType, Chk::UseExpSection useExp) const
{
    bool expansion = useExpansionTechCosts(useExp, techType);
    if ( (expansion && techType >= Sc::Tech::TotalTypes) || (!expansion && techType >= Sc::Tech::TotalOriginalTypes) )
    {
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) +
            " is out of range for the " + (expansion ? "TECx" : "TECS") + " section!");
    }
    return expansion ? read.techCosts.researchTime[techType] : read.techCosts.researchTime[techType];
}

u16 Scenario::getTechEnergyCost(Sc::Tech::Type techType, Chk::UseExpSection useExp) const
{
    bool expansion = useExpansionTechCosts(useExp, techType);
    if ( (expansion && techType >= Sc::Tech::TotalTypes) || (!expansion && techType >= Sc::Tech::TotalOriginalTypes) )
    {
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) +
            " is out of range for the " + (expansion ? "TECx" : "TECS") + " section!");
    }
    return expansion ? read.techCosts.energyCost[techType] : read.techCosts.energyCost[techType];
}

void Scenario::setTechUsesDefaultSettings(Sc::Tech::Type techType, bool useDefault, Chk::UseExpSection useExp)
{
    auto edit = create_action(ActionDescriptor::SetTechUsesDefaultSettings);
    auto checkLimit = [&techType](bool expansion){
        if ( (expansion && techType >= Sc::Tech::TotalTypes) || (!expansion && techType >= Sc::Tech::TotalOriginalTypes) )
            throw std::out_of_range(std::string("TechType: ") + std::to_string(techType) + " is out of range for the " + (expansion ? "TECx" : "TECS") + " section!");
    };
    Chk::UseDefault value = useDefault ? Chk::UseDefault::Yes : Chk::UseDefault::No;
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( techType >= Sc::Tech::Type::Restoration && read.version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( techType < Sc::Tech::TotalOriginalTypes && hasSection(Chk::SectionName::TECS) )
                edit->origTechnologyCosts.useDefault[techType] = value;
            checkLimit(true);
            edit->techCosts.useDefault[techType] = value;
            break;
        case Chk::UseExpSection::Both: checkLimit(false); edit->origTechnologyCosts.useDefault[techType] = value; edit->techCosts.useDefault[techType] = value; break;
        case Chk::UseExpSection::Yes: checkLimit(true); edit->techCosts.useDefault[techType] = value; break;
        case Chk::UseExpSection::No: checkLimit(false); edit->origTechnologyCosts.useDefault[techType] = value; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::TECx) ) {
                checkLimit(true);
                edit->techCosts.useDefault[techType] = value;
            } else {
                checkLimit(false);
                edit->origTechnologyCosts.useDefault[techType] = value;
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::TECS) ) {
                checkLimit(false);
                edit->origTechnologyCosts.useDefault[techType] = value;
            } else {
                checkLimit(true);
                edit->techCosts.useDefault[techType] = value;
            }
            break;
    }
}

void Scenario::setTechMineralCost(Sc::Tech::Type techType, u16 mineralCost, Chk::UseExpSection useExp)
{
    auto edit = create_action(ActionDescriptor::SetTechMineralCost);
    auto checkLimit = [&techType](bool expansion){
        if ( (expansion && techType >= Sc::Tech::TotalTypes) || (!expansion && techType >= Sc::Tech::TotalOriginalTypes) )
            throw std::out_of_range(std::string("TechType: ") + std::to_string(techType) + " is out of range for the " + (expansion ? "TECx" : "TECS") + " section!");
    };
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( techType >= Sc::Tech::Type::Restoration && read.version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( techType < Sc::Tech::TotalOriginalTypes && hasSection(Chk::SectionName::TECS) )
                edit->origTechnologyCosts.mineralCost[techType] = mineralCost;
            checkLimit(true);
            edit->techCosts.mineralCost[techType] = mineralCost;
            break;
        case Chk::UseExpSection::Both: checkLimit(false); edit->origTechnologyCosts.mineralCost[techType] = mineralCost; edit->techCosts.mineralCost[techType] = mineralCost; break;
        case Chk::UseExpSection::Yes: checkLimit(true); edit->techCosts.mineralCost[techType] = mineralCost; break;
        case Chk::UseExpSection::No: checkLimit(false); edit->origTechnologyCosts.mineralCost[techType] = mineralCost; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::TECx) ) {
                checkLimit(true);
                edit->techCosts.mineralCost[techType] = mineralCost;
            } else {
                checkLimit(false);
                edit->origTechnologyCosts.mineralCost[techType] = mineralCost;
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::TECS) ) {
                checkLimit(false);
                edit->origTechnologyCosts.mineralCost[techType] = mineralCost;
            } else {
                checkLimit(true);
                edit->techCosts.mineralCost[techType] = mineralCost;
            }
            break;
    }
}

void Scenario::setTechGasCost(Sc::Tech::Type techType, u16 gasCost, Chk::UseExpSection useExp)
{
    auto edit = create_action(ActionDescriptor::SetTechGasCost);
    auto checkLimit = [&techType](bool expansion){
        if ( (expansion && techType >= Sc::Tech::TotalTypes) || (!expansion && techType >= Sc::Tech::TotalOriginalTypes) )
            throw std::out_of_range(std::string("TechType: ") + std::to_string(techType) + " is out of range for the " + (expansion ? "TECx" : "TECS") + " section!");
    };
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( techType >= Sc::Tech::Type::Restoration && read.version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( techType < Sc::Tech::TotalOriginalTypes && hasSection(Chk::SectionName::TECS) )
                edit->origTechnologyCosts.gasCost[techType] = gasCost;
            checkLimit(true);
            edit->techCosts.gasCost[techType] = gasCost;
            break;
        case Chk::UseExpSection::Both: checkLimit(false); edit->origTechnologyCosts.gasCost[techType] = gasCost; edit->techCosts.gasCost[techType] = gasCost; break;
        case Chk::UseExpSection::Yes: checkLimit(true); edit->techCosts.gasCost[techType] = gasCost; break;
        case Chk::UseExpSection::No: checkLimit(false); edit->origTechnologyCosts.gasCost[techType] = gasCost; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::TECx) ) {
                checkLimit(true);
                edit->techCosts.gasCost[techType] = gasCost;
            } else {
                checkLimit(false);
                edit->origTechnologyCosts.gasCost[techType] = gasCost;
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::TECS) ) {
                checkLimit(false);
                edit->origTechnologyCosts.gasCost[techType] = gasCost;
            } else {
                checkLimit(true);
                edit->techCosts.gasCost[techType] = gasCost;
            }
            break;
    }
}

void Scenario::setTechResearchTime(Sc::Tech::Type techType, u16 researchTime, Chk::UseExpSection useExp)
{
    auto edit = create_action(ActionDescriptor::SetTechResearchTime);
    auto checkLimit = [&techType](bool expansion){
        if ( (expansion && techType >= Sc::Tech::TotalTypes) || (!expansion && techType >= Sc::Tech::TotalOriginalTypes) )
            throw std::out_of_range(std::string("TechType: ") + std::to_string(techType) + " is out of range for the " + (expansion ? "TECx" : "TECS") + " section!");
    };
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( techType >= Sc::Tech::Type::Restoration && read.version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( techType < Sc::Tech::TotalOriginalTypes && hasSection(Chk::SectionName::TECS) )
                edit->origTechnologyCosts.researchTime[techType] = researchTime;
            checkLimit(true);
            edit->techCosts.researchTime[techType] = researchTime;
            break;
        case Chk::UseExpSection::Both: checkLimit(false); edit->origTechnologyCosts.researchTime[techType] = researchTime; edit->techCosts.researchTime[techType] = researchTime; break;
        case Chk::UseExpSection::Yes: checkLimit(true); edit->techCosts.researchTime[techType] = researchTime; break;
        case Chk::UseExpSection::No: checkLimit(false); edit->origTechnologyCosts.researchTime[techType] = researchTime; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::TECx) ) {
                checkLimit(true);
                edit->techCosts.researchTime[techType] = researchTime;
            } else {
                checkLimit(false);
                edit->origTechnologyCosts.researchTime[techType] = researchTime;
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::TECS) ) {
                checkLimit(false);
                edit->origTechnologyCosts.researchTime[techType] = researchTime;
            } else {
                checkLimit(true);
                edit->techCosts.researchTime[techType] = researchTime;
            }
            break;
    }
}

void Scenario::setTechEnergyCost(Sc::Tech::Type techType, u16 energyCost, Chk::UseExpSection useExp)
{
    auto edit = create_action(ActionDescriptor::SetTechEnergyCost);
    auto checkLimit = [&techType](bool expansion){
        if ( (expansion && techType >= Sc::Tech::TotalTypes) || (!expansion && techType >= Sc::Tech::TotalOriginalTypes) )
            throw std::out_of_range(std::string("TechType: ") + std::to_string(techType) + " is out of range for the " + (expansion ? "TECx" : "TECS") + " section!");
    };
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( techType >= Sc::Tech::Type::Restoration && read.version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( techType < Sc::Tech::TotalOriginalTypes && hasSection(Chk::SectionName::TECS) )
                edit->origTechnologyCosts.energyCost[techType] = energyCost;
            checkLimit(true);
            edit->techCosts.energyCost[techType] = energyCost;
            break;
        case Chk::UseExpSection::Both: checkLimit(false); edit->origTechnologyCosts.energyCost[techType] = energyCost; edit->techCosts.energyCost[techType] = energyCost; break;
        case Chk::UseExpSection::Yes: checkLimit(true); edit->techCosts.energyCost[techType] = energyCost; break;
        case Chk::UseExpSection::No: checkLimit(false); edit->origTechnologyCosts.energyCost[techType] = energyCost; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::TECx) ) {
                checkLimit(true);
                edit->techCosts.energyCost[techType] = energyCost;
            } else {
                checkLimit(false);
                edit->origTechnologyCosts.energyCost[techType] = energyCost;
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::TECS) ) {
                checkLimit(false);
                edit->origTechnologyCosts.energyCost[techType] = energyCost;
            } else {
                checkLimit(true);
                edit->techCosts.energyCost[techType] = energyCost;
            }
            break;
    }
}

bool Scenario::useExpansionTechAvailability(Chk::UseExpSection useExp, Sc::Tech::Type techType) const
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto: return techType >= Sc::Tech::Type::Restoration || this->isHybridOrAbove();
        case Chk::UseExpSection::Yes: return true;
        case Chk::UseExpSection::No: return false;
        case Chk::UseExpSection::NoIfOrigAvailable: return this->hasSection(Chk::SectionName::PTEC) ? false : true;
        case Chk::UseExpSection::YesIfAvailable:
        default: return this->hasSection(Chk::SectionName::PTEx) ? true : false;
    }
}

bool Scenario::techAvailable(Sc::Tech::Type techType, size_t playerIndex, Chk::UseExpSection useExp) const
{
    bool expansion = useExpansionTechAvailability(useExp, techType);
    if ( (expansion && techType >= Sc::Tech::TotalTypes) || (!expansion && techType >= Sc::Tech::TotalOriginalTypes) )
    {
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) +
            " is out of range for the " + (expansion ? "PTEx" : "PTEC") + " section!");
    }
    else if ( playerIndex >= Sc::Player::Total )
        throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the " + (expansion ? "PTEx" : "PTEC") + " section!");

    return expansion ? read.techAvailability.techAvailableForPlayer[playerIndex][techType] : read.origTechnologyAvailability.techAvailableForPlayer[playerIndex][techType];
}

bool Scenario::techResearched(Sc::Tech::Type techType, size_t playerIndex, Chk::UseExpSection useExp) const
{
    bool expansion = useExpansionTechAvailability(useExp, techType);
    if ( (expansion && techType >= Sc::Tech::TotalTypes) || (!expansion && techType >= Sc::Tech::TotalOriginalTypes) )
    {
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) +
            " is out of range for the " + (expansion ? "PTEx" : "PTEC") + " section!");
    }
    else if ( playerIndex >= Sc::Player::Total )
        throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the " + (expansion ? "PTEx" : "PTEC") + " section!");

    return expansion ? read.techAvailability.techResearchedForPlayer[playerIndex][techType] : read.origTechnologyAvailability.techResearchedForPlayer[playerIndex][techType];
}

bool Scenario::techDefaultAvailable(Sc::Tech::Type techType, Chk::UseExpSection useExp) const
{
    bool expansion = useExpansionTechAvailability(useExp, techType);
    if ( (expansion && techType >= Sc::Tech::TotalTypes) || (!expansion && techType >= Sc::Tech::TotalOriginalTypes) )
    {
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) +
            " is out of range for the " + (expansion ? "PTEx" : "PTEC") + " section!");
    }
    return expansion ? read.techAvailability.techAvailableByDefault[techType] : read.origTechnologyAvailability.techAvailableByDefault[techType];
}

bool Scenario::techDefaultResearched(Sc::Tech::Type techType, Chk::UseExpSection useExp) const
{
    bool expansion = useExpansionTechAvailability(useExp, techType);
    if ( (expansion && techType >= Sc::Tech::TotalTypes) || (!expansion && techType >= Sc::Tech::TotalOriginalTypes) )
    {
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) +
            " is out of range for the " + (expansion ? "PTEx" : "PTEC") + " section!");
    }
    return expansion ? read.techAvailability.techResearchedByDefault[techType] : read.origTechnologyAvailability.techResearchedByDefault[techType];
}

bool Scenario::playerUsesDefaultTechSettings(Sc::Tech::Type techType, size_t playerIndex, Chk::UseExpSection useExp) const
{
    bool expansion = useExpansionTechAvailability(useExp, techType);
    if ( (expansion && techType >= Sc::Tech::TotalTypes) || (!expansion && techType >= Sc::Tech::TotalOriginalTypes) )
    {
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) +
            " is out of range for the " + (expansion ? "PTEx" : "PTEC") + " section!");
    }
    else if ( playerIndex >= Sc::Player::Total )
        throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the " + (expansion ? "PTEx" : "PTEC") + " section!");

    return expansion ? read.techAvailability.playerUsesDefaultsForTech[playerIndex][techType] : read.origTechnologyAvailability.playerUsesDefaultsForTech[playerIndex][techType];
}

void Scenario::setTechAvailable(Sc::Tech::Type techType, size_t playerIndex, bool available, Chk::UseExpSection useExp)
{
    auto edit = create_action(ActionDescriptor::SetTechAvailable);
    auto checkLimit = [&techType, &playerIndex](bool expansion){
        if ( (expansion && techType >= Sc::Tech::TotalTypes) || (!expansion && techType >= Sc::Tech::TotalOriginalTypes) )
            throw std::out_of_range(std::string("TechType: ") + std::to_string(techType) + " is out of range for the " + (expansion ? "PTEx" : "PTEC") + " section!");
        else if ( playerIndex >= Sc::Player::Total )
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the " + (expansion ? "PTEx" : "PTEC") + " section!");
    };
    Chk::Available value = available ? Chk::Available::Yes : Chk::Available::No;
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( techType >= Sc::Tech::Type::Restoration && read.version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( techType < Sc::Tech::TotalOriginalTypes && hasSection(Chk::SectionName::PTEC) )
                edit->origTechnologyAvailability.techAvailableForPlayer[playerIndex][techType] = value;
            checkLimit(true);
            edit->techAvailability.techAvailableForPlayer[playerIndex][techType] = value;
            break;
        case Chk::UseExpSection::Both: checkLimit(false); edit->origTechnologyAvailability.techAvailableForPlayer[playerIndex][techType] = value; edit->techAvailability.techAvailableForPlayer[playerIndex][techType] = value; break;
        case Chk::UseExpSection::Yes: checkLimit(true); edit->techAvailability.techAvailableForPlayer[playerIndex][techType] = value; break;
        case Chk::UseExpSection::No: checkLimit(false); edit->origTechnologyAvailability.techAvailableForPlayer[playerIndex][techType] = value; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::PTEx) ) {
                checkLimit(true);
                edit->techAvailability.techAvailableForPlayer[playerIndex][techType] = value;
            } else {
                checkLimit(false);
                edit->origTechnologyAvailability.techAvailableForPlayer[playerIndex][techType] = value;
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::PTEC) ) {
                checkLimit(false);
                edit->origTechnologyAvailability.techAvailableForPlayer[playerIndex][techType] = value;
            } else {
                checkLimit(true);
                edit->techAvailability.techAvailableForPlayer[playerIndex][techType] = value;
            }
            break;
    }
}

void Scenario::setTechResearched(Sc::Tech::Type techType, size_t playerIndex, bool researched, Chk::UseExpSection useExp)
{
    auto edit = create_action(ActionDescriptor::SetTechResearched);
    auto checkLimit = [&techType, &playerIndex](bool expansion){
        if ( (expansion && techType >= Sc::Tech::TotalTypes) || (!expansion && techType >= Sc::Tech::TotalOriginalTypes) )
            throw std::out_of_range(std::string("TechType: ") + std::to_string(techType) + " is out of range for the " + (expansion ? "PTEx" : "PTEC") + " section!");
        else if ( playerIndex >= Sc::Player::Total )
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the " + (expansion ? "PTEx" : "PTEC") + " section!");
    };
    Chk::Researched value = researched ? Chk::Researched::Yes : Chk::Researched::No;
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( techType >= Sc::Tech::Type::Restoration && read.version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( techType < Sc::Tech::TotalOriginalTypes && hasSection(Chk::SectionName::PTEC) )
                edit->origTechnologyAvailability.techResearchedForPlayer[playerIndex][techType] = value;
            checkLimit(true);
            edit->techAvailability.techResearchedForPlayer[playerIndex][techType] = value;
            break;
        case Chk::UseExpSection::Both: checkLimit(false); edit->origTechnologyAvailability.techResearchedForPlayer[playerIndex][techType] = value; edit->techAvailability.techResearchedForPlayer[playerIndex][techType] = value; break;
        case Chk::UseExpSection::Yes: checkLimit(true); edit->techAvailability.techResearchedForPlayer[playerIndex][techType] = value; break;
        case Chk::UseExpSection::No: checkLimit(false); edit->origTechnologyAvailability.techResearchedForPlayer[playerIndex][techType] = value; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::PTEx) ) {
                checkLimit(true);
                edit->techAvailability.techResearchedForPlayer[playerIndex][techType] = value;
            } else {
                checkLimit(false);
                edit->origTechnologyAvailability.techResearchedForPlayer[playerIndex][techType] = value;
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::PTEC) ) {
                checkLimit(false);
                edit->origTechnologyAvailability.techResearchedForPlayer[playerIndex][techType] = value;
            } else {
                checkLimit(true);
                edit->techAvailability.techResearchedForPlayer[playerIndex][techType] = value;
            }
            break;
    }
}

void Scenario::setDefaultTechAvailable(Sc::Tech::Type techType, bool available, Chk::UseExpSection useExp)
{
    auto edit = create_action(ActionDescriptor::SetTechDefaultAvailable);
    auto checkLimit = [&techType](bool expansion){
        if ( (expansion && techType >= Sc::Tech::TotalTypes) || (!expansion && techType >= Sc::Tech::TotalOriginalTypes) )
            throw std::out_of_range(std::string("TechType: ") + std::to_string(techType) + " is out of range for the " + (expansion ? "PTEx" : "PTEC") + " section!");
    };
    Chk::Available value = available ? Chk::Available::Yes : Chk::Available::No;
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( techType >= Sc::Tech::Type::Restoration && read.version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( techType < Sc::Tech::TotalOriginalTypes && hasSection(Chk::SectionName::PTEC) )
                edit->origTechnologyAvailability.techAvailableByDefault[techType] = value;
            checkLimit(true);
            edit->techAvailability.techAvailableByDefault[techType] = value;
            break;
        case Chk::UseExpSection::Both: checkLimit(false); edit->origTechnologyAvailability.techAvailableByDefault[techType] = value; edit->techAvailability.techAvailableByDefault[techType] = value; break;
        case Chk::UseExpSection::Yes: checkLimit(true); edit->techAvailability.techAvailableByDefault[techType] = value; break;
        case Chk::UseExpSection::No: checkLimit(false); edit->origTechnologyAvailability.techAvailableByDefault[techType] = value; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::PTEx) ) {
                checkLimit(true);
                edit->techAvailability.techAvailableByDefault[techType] = value;
            } else {
                checkLimit(false);
                edit->origTechnologyAvailability.techAvailableByDefault[techType] = value;
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::PTEC) ) {
                checkLimit(false);
                edit->origTechnologyAvailability.techAvailableByDefault[techType] = value;
            } else {
                checkLimit(true);
                edit->techAvailability.techAvailableByDefault[techType] = value;
            }
            break;
    }
}

void Scenario::setDefaultTechResearched(Sc::Tech::Type techType, bool researched, Chk::UseExpSection useExp)
{
    auto edit = create_action(ActionDescriptor::SetTechDefaultResearched);
    auto checkLimit = [&techType](bool expansion){
        if ( (expansion && techType >= Sc::Tech::TotalTypes) || (!expansion && techType >= Sc::Tech::TotalOriginalTypes) )
            throw std::out_of_range(std::string("TechType: ") + std::to_string(techType) + " is out of range for the " + (expansion ? "PTEx" : "PTEC") + " section!");
    };
    Chk::Researched value = researched ? Chk::Researched::Yes : Chk::Researched::No;
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( techType >= Sc::Tech::Type::Restoration && read.version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( techType < Sc::Tech::TotalOriginalTypes && hasSection(Chk::SectionName::PTEC) )
                edit->origTechnologyAvailability.techResearchedByDefault[techType] = value;
            checkLimit(true);
            edit->techAvailability.techResearchedByDefault[techType] = value;
            break;
        case Chk::UseExpSection::Both: checkLimit(false); edit->origTechnologyAvailability.techResearchedByDefault[techType] = value; edit->techAvailability.techResearchedByDefault[techType] = value; break;
        case Chk::UseExpSection::Yes: checkLimit(true); edit->techAvailability.techResearchedByDefault[techType] = value; break;
        case Chk::UseExpSection::No: checkLimit(false); edit->origTechnologyAvailability.techResearchedByDefault[techType] = value; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::PTEx) ) {
                checkLimit(true);
                edit->techAvailability.techResearchedByDefault[techType] = value;
            } else {
                checkLimit(false);
                edit->origTechnologyAvailability.techResearchedByDefault[techType] = value;
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::PTEC) ) {
                checkLimit(false);
                edit->origTechnologyAvailability.techResearchedByDefault[techType] = value;
            } else {
                checkLimit(true);
                edit->techAvailability.techResearchedByDefault[techType] = value;
            }
            break;
    }
}

void Scenario::setPlayerUsesDefaultTechSettings(Sc::Tech::Type techType, size_t playerIndex, bool useDefault, Chk::UseExpSection useExp)
{
    auto edit = create_action(ActionDescriptor::SetPlayerUsesDefaultTechSettings);
    auto checkLimit = [&techType, &playerIndex](bool expansion){
        if ( (expansion && techType >= Sc::Tech::TotalTypes) || (!expansion && techType >= Sc::Tech::TotalOriginalTypes) )
            throw std::out_of_range(std::string("TechType: ") + std::to_string(techType) + " is out of range for the " + (expansion ? "PTEx" : "PTEC") + " section!");
        else if ( playerIndex >= Sc::Player::Total )
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the " + (expansion ? "PTEx" : "PTEC") + " section!");
    };
    Chk::UseDefault value = useDefault ? Chk::UseDefault::Yes : Chk::UseDefault::No;
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( techType >= Sc::Tech::Type::Restoration && read.version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( techType < Sc::Tech::TotalOriginalTypes && hasSection(Chk::SectionName::PTEC) )
                edit->origTechnologyAvailability.playerUsesDefaultsForTech[playerIndex][techType] = value;
            checkLimit(true);
            edit->techAvailability.playerUsesDefaultsForTech[playerIndex][techType] = value;
            break;
        case Chk::UseExpSection::Both: checkLimit(false); edit->origTechnologyAvailability.playerUsesDefaultsForTech[playerIndex][techType] = value; edit->techAvailability.playerUsesDefaultsForTech[playerIndex][techType] = value; break;
        case Chk::UseExpSection::Yes: checkLimit(true); edit->techAvailability.playerUsesDefaultsForTech[playerIndex][techType] = value; break;
        case Chk::UseExpSection::No: checkLimit(false); edit->origTechnologyAvailability.playerUsesDefaultsForTech[playerIndex][techType] = value; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::PTEx) ) {
                checkLimit(true);
                edit->techAvailability.playerUsesDefaultsForTech[playerIndex][techType] = value;
            } else {
                checkLimit(false);
                edit->origTechnologyAvailability.playerUsesDefaultsForTech[playerIndex][techType] = value;
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::PTEC) ) {
                checkLimit(false);
                edit->origTechnologyAvailability.playerUsesDefaultsForTech[playerIndex][techType] = value;
            } else {
                checkLimit(true);
                edit->techAvailability.playerUsesDefaultsForTech[playerIndex][techType] = value;
            }
            break;
    }
}

void Scenario::setTechsToDefault(Chk::UseExpSection useExp)
{
    auto edit = create_action(ActionDescriptor::ResetAllTechProperties);
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both:
            edit->origTechnologyCosts = Chk::TECS {};
            edit->techCosts = Chk::TECx {};
            edit->origTechnologyAvailability = Chk::PTEC {};
            edit->techAvailability = Chk::PTEx {};
            break;
        case Chk::UseExpSection::Yes:
            edit->techCosts = Chk::TECx {};
            edit->techAvailability = Chk::PTEx {};
            break;
        case Chk::UseExpSection::No:
            edit->origTechnologyCosts = Chk::TECS {};
            edit->origTechnologyAvailability = Chk::PTEC {};
            break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( this->hasSection(Chk::SectionName::TECx) )
                edit->techCosts = Chk::TECx {};
            else
                edit->origTechnologyCosts = Chk::TECS {};

            if ( this->hasSection(Chk::SectionName::PTEx) )
                edit->techAvailability = Chk::PTEx {};
            else
                edit->origTechnologyAvailability = Chk::PTEC {};
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( this->hasSection(Chk::SectionName::TECS) )
                edit->origTechnologyCosts = Chk::TECS {};
            else
                edit->techCosts = Chk::TECx {};

            if ( this->hasSection(Chk::SectionName::PTEC) )
                edit->origTechnologyAvailability = Chk::PTEC {};
            else
                edit->techAvailability = Chk::PTEx {};
            break;
    }
}

void Scenario::appendUnitStrUsage(size_t stringId, std::vector<Chk::StringUser> & stringUsers, u32 userMask) const
{
    u16 u16StringId = (u16)stringId;
    if ( (userMask & Chk::StringUserFlag::OriginalUnitSettings) == Chk::StringUserFlag::OriginalUnitSettings )
    {
        for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
        {
            if ( read.origUnitSettings.nameStringId[i] == u16StringId )
                stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::OriginalUnitSettings, i));
        }
    }

    if ( (userMask & Chk::StringUserFlag::ExpansionUnitSettings) == Chk::StringUserFlag::ExpansionUnitSettings )
    {
        for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
        {
            if ( read.unitSettings.nameStringId[i] == u16StringId )
                stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::ExpansionUnitSettings, i));
        }
    }
}

bool Scenario::unitStringUsed(size_t stringId, u32 userMask) const
{
    u16 u16StringId = (u16)stringId;
    if ( (userMask & Chk::StringUserFlag::OriginalUnitSettings) == Chk::StringUserFlag::OriginalUnitSettings )
    {
        for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
        {
            if ( read.origUnitSettings.nameStringId[i] == u16StringId )
                return true;
        }
    }
    if ( (userMask & Chk::StringUserFlag::ExpansionUnitSettings) == Chk::StringUserFlag::ExpansionUnitSettings )
    {
        for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
        {
            if ( read.unitSettings.nameStringId[i] == u16StringId )
                return true;
        }
    }
    return false;
}

void Scenario::markUsedUnitStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask) const
{
    return read.markUsedUnitStrings(stringIdUsed, userMask);
}

void Scenario::remapUnitStringIds(const std::map<u32, u32> & stringIdRemappings)
{
    auto edit = create_action();
    for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
    {
        auto found = stringIdRemappings.find(read.origUnitSettings.nameStringId[i]);
        if ( found != stringIdRemappings.end() )
            edit->origUnitSettings.nameStringId[i] = found->second;
    }
    for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
    {
        auto found = stringIdRemappings.find(read.unitSettings.nameStringId[i]);
        if ( found != stringIdRemappings.end() )
            edit->unitSettings.nameStringId[i] = found->second;
    }
}

void Scenario::deleteUnitString(size_t stringId)
{
    auto edit = create_action(ActionDescriptor::UpdateUnitName);
    for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
    {
        if ( read.origUnitSettings.nameStringId[i] == stringId )
            edit->origUnitSettings.nameStringId[i] = 0;
    }
    for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
    {
        if ( read.unitSettings.nameStringId[i] == stringId )
            edit->unitSettings.nameStringId[i] = 0;
    }
}

Chk::Cuwp Scenario::getCuwp(size_t cuwpIndex) const
{
    if ( cuwpIndex >= Sc::Unit::MaxCuwps )
        throw std::out_of_range(std::string("CuwpIndex: ") + std::to_string(cuwpIndex) + " is out of range for the UPRP section!");

    return read.createUnitProperties[cuwpIndex];
}

void Scenario::setCuwp(size_t cuwpIndex, const Chk::Cuwp & cuwp)
{
    if ( cuwpIndex < Sc::Unit::MaxCuwps && read.createUnitProperties[cuwpIndex] != cuwp )
        create_action(ActionDescriptor::SetCuwp)->createUnitProperties[cuwpIndex] = cuwp;
}

size_t Scenario::addCuwp(const Chk::Cuwp & cuwp, bool fixUsageBeforeAdding, size_t excludedTriggerIndex, size_t excludedTriggerActionIndex)
{
    auto edit = create_action(ActionDescriptor::AddCuwp);
    for ( size_t i = 0; i < Sc::Unit::MaxCuwps; i++ )
    {
        if ( memcmp(&cuwp, &read.createUnitProperties[i], sizeof(Chk::Cuwp)) == 0 )
            return i; // Return existing CUWP index
    }
    if ( fixUsageBeforeAdding )
        fixCuwpUsage(excludedTriggerIndex, excludedTriggerActionIndex);

    size_t nextUnused = Sc::Unit::MaxCuwps;
    for ( size_t i = 0; i < Sc::Unit::MaxCuwps; i++ )
    {
        if ( read.createUnitPropertiesUsed[i] == Chk::CuwpUsed::No )
        {
            nextUnused = i;
            break;
        }
    }
    if ( nextUnused < Sc::Unit::MaxCuwps )
        edit->createUnitProperties[nextUnused] = cuwp;

    return nextUnused;
}

void Scenario::fixCuwpUsage(size_t excludedTriggerIndex, size_t excludedTriggerActionIndex)
{
    auto edit = create_action();
    for ( size_t i=0; i<Sc::Unit::MaxCuwps; i++ )
        edit->createUnitPropertiesUsed[i] = Chk::CuwpUsed::No;

    size_t numTriggers = this->numTriggers();
    for ( size_t i=0; i<numTriggers; ++i )
    {
        const auto & trigger = read.triggers[i];
        for ( size_t actionIndex = 0; actionIndex < Chk::Trigger::MaxActions; ++actionIndex )
        {
            const auto & action = trigger.actions[actionIndex];
            if ( action.actionType == Chk::Action::Type::CreateUnitWithProperties && action.number < Sc::Unit::MaxCuwps && !(i == excludedTriggerIndex && actionIndex == excludedTriggerActionIndex) )
                edit->createUnitPropertiesUsed[action.number] = Chk::CuwpUsed::Yes;
        }
    }
}

bool Scenario::cuwpUsed(size_t cuwpIndex) const
{
    if ( cuwpIndex < Sc::Unit::MaxCuwps )
        return read.createUnitPropertiesUsed[cuwpIndex] != Chk::CuwpUsed::No;
    else
        throw std::out_of_range(std::string("CuwpIndex: ") + std::to_string(cuwpIndex) + " is out of range for the UPUS section!");
}

void Scenario::setCuwpUsed(size_t cuwpIndex, bool cuwpUsed)
{
    if ( cuwpIndex < Sc::Unit::MaxCuwps )
        create_action(ActionDescriptor::SetCuwpUsed)->createUnitPropertiesUsed[cuwpIndex] = cuwpUsed ? Chk::CuwpUsed::Yes : Chk::CuwpUsed::No;
    else
        throw std::out_of_range(std::string("CuwpIndex: ") + std::to_string(cuwpIndex) + " is out of range for the UPUS section!");
}

size_t Scenario::numTriggers() const
{
    return read.triggers.size();
}

Scenario::EditTrigger Scenario::editTrigger(size_t triggerIndex)
{
    if ( triggerIndex < read.triggers.size() )
        return EditTrigger(this, view.triggers[triggerIndex]);
    else
        throw std::out_of_range(std::string("TriggerIndex: ") + std::to_string(triggerIndex) + " is out of range for the TRIG section!");
}

const Chk::Trigger & Scenario::getTrigger(size_t triggerIndex) const
{
    if ( triggerIndex < read.triggers.size() )
        return read.triggers[triggerIndex];
    else
        throw std::out_of_range(std::string("TriggerIndex: ") + std::to_string(triggerIndex) + " is out of range for the TRIG section!");
}

size_t Scenario::addTrigger(const Chk::Trigger & trigger)
{
    create_action(ActionDescriptor::AddTrigger)->triggers.append(trigger);
    return read.triggers.size()-1;
}

void Scenario::insertTrigger(size_t triggerIndex, const Chk::Trigger & trigger)
{
    if ( triggerIndex < read.triggers.size() )
        create_action(ActionDescriptor::AddTrigger)->triggers.insert(triggerIndex, trigger);
    else
        create_action(ActionDescriptor::AddTrigger)->triggers.append(trigger);

    fixTriggerExtensions();
}

void Scenario::deleteTrigger(size_t triggerIndex)
{
    if ( triggerIndex < read.triggers.size() )
    {
        this->removeTriggersExtension(triggerIndex);
        create_action(ActionDescriptor::DeleteTrigger)->triggers.remove(triggerIndex);
    }
    fixTriggerExtensions();
}

void Scenario::moveTrigger(size_t triggerIndexFrom, size_t triggerIndexTo)
{
    if ( triggerIndexFrom < read.triggers.size() && triggerIndexTo < read.triggers.size() )
    {
        create_action(ActionDescriptor::MoveTrigger)->triggers.move_to(triggerIndexFrom, triggerIndexTo);
        fixTriggerExtensions();
    }
}

std::vector<Chk::Trigger> Scenario::replaceTriggerRange(size_t beginIndex, size_t endIndex, std::vector<Chk::Trigger> & triggers)
{
    auto edit = create_action(ActionDescriptor::ReplaceTriggerRange);
    if ( beginIndex == 0 && endIndex == read.triggers.size() )
    {
        edit->triggers = triggers;
        fixTriggerExtensions();
        return triggers;
    }
    else if ( beginIndex < endIndex && endIndex <= read.triggers.size() )
    {
        std::vector<Chk::Trigger> replacedTriggers(read.triggers.begin()+beginIndex, read.triggers.begin()+endIndex);
        for ( std::size_t i=beginIndex; i<endIndex; ++i )
            edit->triggers[i] = triggers[i-beginIndex];

        if ( triggers.size() > endIndex-beginIndex ) // New triggers are inserted at the end of the range
        {
            std::vector<Chk::Trigger> expansion(triggers.begin()+(endIndex-beginIndex), triggers.end());
            edit->triggers.insert(endIndex, expansion);
        }

        fixTriggerExtensions();
        return replacedTriggers;
    }
    else
        throw std::out_of_range(std::string("Range [") + std::to_string(beginIndex) + ", " + std::to_string(endIndex) +
            ") is invalid for trigger list of size: " + std::to_string(read.triggers.size()));
}

bool Scenario::hasTriggerExtension(size_t triggerIndex) const
{
    if ( triggerIndex > read.triggers.size() )
        return false;

    size_t extendedTrigDataIndex = read.triggers[triggerIndex].getExtendedDataIndex();
    return extendedTrigDataIndex < read.triggerExtensions.size();
}

std::size_t Scenario::getTriggerExtension(size_t triggerIndex, bool addIfNotFound)
{
    if ( triggerIndex >= read.triggers.size() )
        throw std::out_of_range(std::string("TriggerIndex: ") + std::to_string(triggerIndex) + " is out of range for the TRIG section!");

    const auto & trigger = read.triggers[triggerIndex];
    size_t extendedTrigDataIndex = trigger.getExtendedDataIndex();
    if ( extendedTrigDataIndex < read.triggerExtensions.size() )
        return extendedTrigDataIndex;
    else if ( addIfNotFound )
    {
        auto edit = create_action(ActionDescriptor::CreateExtendedTriggerData);
        std::set<u32> usedExtensionIndexes {};
        for ( const auto & trig : read.triggers )
        {
            u32 usedIndex = u32(trig.getExtendedDataIndex());
            if ( (usedIndex & Chk::UnusedExtendedTrigDataIndexCheck) != 0 ) // Some usable index
                usedExtensionIndexes.insert(usedIndex);
        }
        for ( size_t i=0; i<read.triggerExtensions.size(); i++ )
        {
            if ( (i & Chk::UnusedExtendedTrigDataIndexCheck) != 0 && usedExtensionIndexes.count(u32(i)) == 0 ) // If index is usable and unused
            {
                edit->triggerExtensions[i] = Chk::ExtendedTrigData{};
                editTrigger(i).setExtendedDataIndex(read.triggerExtensions.size()-1);
                return i;
            }
        }

        while ( (read.triggerExtensions.size() & Chk::UnusedExtendedTrigDataIndexCheck) == 0 ) // While next index is unusable
            edit->triggerExtensions.append(Chk::ExtendedTrigData{}); // Put a dummy value in this position

        edit->triggerExtensions.append(Chk::ExtendedTrigData{}); // Put real/usable extension data in this position
        editTrigger(triggerIndex).setExtendedDataIndex(read.triggerExtensions.size()-1);
        return read.triggerExtensions.size()-1;
    }
    else
        throw std::logic_error(std::string("TriggerIndex: ") + std::to_string(triggerIndex) + " did not have an extension and addIfNotFound was not set!");
}

const Chk::ExtendedTrigData & Scenario::getTriggerExtension(size_t triggerIndex) const
{
    if ( triggerIndex >= read.triggers.size() )
        throw std::out_of_range(std::string("TriggerIndex: ") + std::to_string(triggerIndex) + " is out of range for the TRIG section!");

    auto & trigger = read.triggers[triggerIndex];
    size_t extendedTrigDataIndex = trigger.getExtendedDataIndex();
    if ( extendedTrigDataIndex < read.triggerExtensions.size() )
        return read.triggerExtensions[extendedTrigDataIndex];
    else
        throw std::logic_error(std::string("TriggerIndex: ") + std::to_string(triggerIndex) + " did not have an extension!");
}

void Scenario::removeTriggersExtension(size_t triggerIndex)
{
    if ( triggerIndex >= read.triggers.size() )
        throw std::out_of_range(std::string("TriggerIndex: ") + std::to_string(triggerIndex) + " is out of range for the TRIG section!");

    auto trigger = this->editTrigger(triggerIndex);
    size_t extendedTrigDataIndex = trigger->getExtendedDataIndex();
    trigger.clearExtendedDataIndex();
    deleteTriggerExtension(extendedTrigDataIndex);
}

void Scenario::deleteTriggerExtension(size_t triggerExtensionIndex)
{
    if ( triggerExtensionIndex < read.triggerExtensions.size() && triggerExtensionIndex != Chk::ExtendedTrigDataIndex::None )
    {
        size_t i = read.triggerExtensions.size();
        for ( ; i > 0 && (((i-1) & Chk::UnusedExtendedTrigDataIndexCheck) == 0 || i-1 >= read.triggerExtensions.size()); i-- );

        if ( i == 0 )
            create_action(ActionDescriptor::DeleteExtendedTriggerData)->triggerExtensions.reset();
        else if ( i < read.triggerExtensions.size() )
        {
            std::vector<std::size_t> indexesErased(read.triggerExtensions.size()-i);
            std::iota(indexesErased.begin(), indexesErased.end(), i);
            create_action(ActionDescriptor::DeleteExtendedTriggerData)->triggerExtensions.remove(indexesErased);
        }
    }
}

void Scenario::fixTriggerExtensions()
{
    auto edit = create_action();
    std::set<size_t> usedExtendedTrigDataIndexes;
    size_t numTriggers = read.triggers.size();
    for ( size_t i=0; i<numTriggers; i++ )
    {
        auto trigger = editTrigger(i);
        size_t extendedDataIndex = trigger->getExtendedDataIndex();
        if ( extendedDataIndex != Chk::ExtendedTrigDataIndex::None )
        {
            if ( extendedDataIndex >= read.triggerExtensions.size() )
                trigger.clearExtendedDataIndex();
            else if ( usedExtendedTrigDataIndexes.find(extendedDataIndex) == usedExtendedTrigDataIndexes.end() ) // Valid extension
            {
                edit->triggerExtensions[extendedDataIndex].trigNum = (u32)i; // Ensure the trigNum is correct
                usedExtendedTrigDataIndexes.insert(extendedDataIndex);
            }
            else // Same extension used by multiple triggers
                trigger.clearExtendedDataIndex();
        }
    }

    size_t numTriggerExtensions = read.triggerExtensions.size();
    for ( size_t i=0; i<numTriggerExtensions; i++ )
    {
        const Chk::ExtendedTrigData & extension = read.triggerExtensions[i];
        if ( usedExtendedTrigDataIndexes.find(i) == usedExtendedTrigDataIndexes.end() ) // Extension exists, but no trigger uses it
        {
            if ( extension.trigNum != Chk::ExtendedTrigData::TrigNum::None ) // Refers to a trigger
            {
                if ( extension.trigNum < read.triggers.size() && // this trigger exists without an extension
                    read.triggers[extension.trigNum].getExtendedDataIndex() == Chk::ExtendedTrigDataIndex::None )
                {
                    editTrigger(extension.trigNum).setExtendedDataIndex(i); // Link up extension to the trigger
                }
                else // Trigger does not exist
                    this->deleteTriggerExtension(i); // Delete the extension
            }
            else if ( extension.trigNum == Chk::ExtendedTrigData::TrigNum::None ) // Does not refer to a trigger
                this->deleteTriggerExtension(i); // Delete the extension
        }
    }
}

size_t Scenario::getCommentStringId(size_t triggerIndex) const
{
    if ( triggerIndex >= read.triggers.size() )
        throw std::out_of_range(std::string("TriggerIndex: ") + std::to_string(triggerIndex) + " is out of range for the TRIG section!");
    else
        return read.triggers[triggerIndex].getComment();
}

size_t Scenario::getExtendedCommentStringId(size_t triggerIndex) const
{
    if ( triggerIndex >= read.triggers.size() )
        throw std::out_of_range(std::string("TriggerIndex: ") + std::to_string(triggerIndex) + " is out of range for the TRIG section!");
    else
    {
        const auto & trigger = read.triggers[triggerIndex];
        size_t extendedDataIndex = trigger.getExtendedDataIndex();
        if ( (extendedDataIndex & Chk::UnusedExtendedTrigDataIndexCheck) != 0 && extendedDataIndex < read.triggerExtensions.size() )
            return read.triggerExtensions[extendedDataIndex].commentStringId;
    }
    return Chk::StringId::NoString;
}

void Scenario::setExtendedCommentStringId(size_t triggerIndex, size_t stringId)
{
    auto extensionIndex = getTriggerExtension(triggerIndex, stringId != Chk::StringId::NoString);
    create_action(ActionDescriptor::UpdateTriggerComment)->triggerExtensions[extensionIndex].commentStringId = (u32)stringId;
    if ( stringId == Chk::StringId::NoString && read.triggerExtensions[extensionIndex].isBlank() )
        removeTriggersExtension(triggerIndex);
}

size_t Scenario::getExtendedNotesStringId(size_t triggerIndex) const
{
    if ( this->hasTriggerExtension(triggerIndex) )
        return getTriggerExtension(triggerIndex).notesStringId;
    else
        return Chk::StringId::NoString;
}

void Scenario::setExtendedNotesStringId(size_t triggerIndex, size_t stringId)
{
    auto extensionIndex = getTriggerExtension(triggerIndex, stringId != Chk::StringId::NoString);
    create_action(ActionDescriptor::UpdateTriggerNotes)->triggerExtensions[extensionIndex].notesStringId = (u32)stringId;
    if ( stringId == Chk::StringId::NoString && read.triggerExtensions[extensionIndex].isBlank() )
        removeTriggersExtension(triggerIndex);
}

size_t Scenario::numBriefingTriggers() const
{
    return read.briefingTriggers.size();
}

Scenario::EditBriefingTrigger Scenario::editBriefingTrigger(size_t briefingTriggerIndex)
{
    if ( briefingTriggerIndex < read.briefingTriggers.size() )
        return EditBriefingTrigger(this, view.briefingTriggers[briefingTriggerIndex]);
    else
        throw std::out_of_range(std::string("BriefingTriggerIndex: ") + std::to_string(briefingTriggerIndex) + " is out of range for the MBRF section!");
}

const Chk::Trigger & Scenario::getBriefingTrigger(size_t briefingTriggerIndex) const
{
    if ( briefingTriggerIndex < read.briefingTriggers.size() )
        return read.briefingTriggers[briefingTriggerIndex];
    else
        throw std::out_of_range(std::string("BriefingTriggerIndex: ") + std::to_string(briefingTriggerIndex) + " is out of range for the MBRF section!");
}

size_t Scenario::addBriefingTrigger(const Chk::Trigger & briefingTrigger)
{
    create_action(ActionDescriptor::AddBriefingTrigger)->briefingTriggers.append(briefingTrigger);
    return read.briefingTriggers.size()-1;
}

void Scenario::insertBriefingTrigger(size_t briefingTriggerIndex, const Chk::Trigger & briefingTrigger)
{
    if ( briefingTriggerIndex < read.briefingTriggers.size() )
        create_action(ActionDescriptor::AddBriefingTrigger)->briefingTriggers.insert(briefingTriggerIndex, briefingTrigger);
    else
        create_action(ActionDescriptor::AddBriefingTrigger)->briefingTriggers.append(briefingTrigger);
}

void Scenario::deleteBriefingTrigger(size_t briefingTriggerIndex)
{
    if ( briefingTriggerIndex < read.briefingTriggers.size() )
        create_action(ActionDescriptor::DeleteBriefingTrigger)->briefingTriggers.remove(briefingTriggerIndex);
}

void Scenario::moveBriefingTrigger(size_t briefingTriggerIndexFrom, size_t briefingTriggerIndexTo)
{
    create_action(ActionDescriptor::MoveBriefingTrigger)->briefingTriggers.move_to(briefingTriggerIndexFrom, briefingTriggerIndexTo);
}

std::vector<Chk::Trigger> Scenario::replaceBriefingTriggerRange(size_t beginIndex, size_t endIndex, std::vector<Chk::Trigger> & briefingTriggers)
{
    auto edit = create_action(ActionDescriptor::ReplaceBriefingTriggerRange);
    if ( beginIndex == 0 && endIndex == read.briefingTriggers.size() )
    {
        auto replacedBriefingTriggers = read.briefingTriggers;
        edit->briefingTriggers = briefingTriggers;
        return replacedBriefingTriggers;
    }
    else if ( beginIndex < endIndex && endIndex <= read.briefingTriggers.size() )
    {
        std::vector<Chk::Trigger> replacedBriefingTriggers(read.briefingTriggers.begin()+beginIndex, read.briefingTriggers.begin()+endIndex);
        for ( std::size_t i=beginIndex; i<endIndex; ++i )
            edit->briefingTriggers[i] = briefingTriggers[i-beginIndex];

        if ( briefingTriggers.size() > endIndex-beginIndex ) // New briefing triggers are inserted at the end of the range
        {
            std::vector<Chk::Trigger> expansion(briefingTriggers.begin()+(endIndex-beginIndex), briefingTriggers.end());
            edit->briefingTriggers.insert(endIndex, expansion);
        }

        return replacedBriefingTriggers;
    }
    else
        throw std::out_of_range(std::string("Range [") + std::to_string(beginIndex) + ", " + std::to_string(endIndex) +
            ") is invalid for briefing trigger list of size: " + std::to_string(read.briefingTriggers.size()));
}

size_t Scenario::addSound(size_t stringId)
{
    for ( size_t i=0; i<Chk::TotalSounds; i++ )
    {
        if ( read.soundPaths[i] == Chk::StringId::UnusedSound )
        {
            create_action(ActionDescriptor::AddSound)->soundPaths[i] = (u32)stringId;
            return i;
        }
    }
    return Chk::TotalSounds;
}

bool Scenario::stringIsSound(size_t stringId) const
{
    u32 u32StringId = (u32)stringId;
    for ( size_t i=0; i<Chk::TotalSounds; i++ )
    {
        if ( read.soundPaths[i] == u32StringId )
            return true;
    }
    return false;
}

size_t Scenario::getSoundStringId(size_t soundIndex) const
{
    if ( soundIndex < Chk::TotalSounds )
        return read.soundPaths[soundIndex];
    else
        throw std::out_of_range(std::string("SoundIndex: ") + std::to_string((u32)soundIndex) + " is out of range for the WAV section!");
}

void Scenario::setSoundStringId(size_t soundIndex, size_t soundStringId)
{
    if ( soundIndex < Chk::TotalSounds )
        create_action(ActionDescriptor::AddSound)->soundPaths[soundIndex] = (u32)soundStringId;
}

void Scenario::deleteSoundReferences(size_t stringId)
{
    auto edit = create_action(ActionDescriptor::RemoveSound);
    for ( size_t i=0; i<Chk::TotalSounds; i++ )
    {
        if ( read.soundPaths[i] == stringId )
            edit->soundPaths[i] = 0;
    }
    for ( std::size_t i=0; i<read.triggers.size(); ++i )
        editTrigger(i).deleteSoundReferences(stringId);
    for ( std::size_t i=0; i<read.briefingTriggers.size(); ++i )
        editBriefingTrigger(i).deleteBriefingSoundReferences(stringId);
}

bool Scenario::triggerSwitchUsed(size_t switchId) const
{
    for ( const auto & trigger : read.triggers )
    {
        if ( trigger.switchUsed(switchId) )
            return true;
    }
    return false;
}

bool Scenario::triggerLocationUsed(size_t locationId) const
{
    for ( const auto & trigger : read.triggers )
    {
        if ( trigger.locationUsed(locationId) )
            return true;
    }
    return false;
}

void Scenario::appendTriggerStrUsage(size_t stringId, std::vector<Chk::StringUser> & stringUsers, Chk::Scope storageScope, u32 userMask) const
{
    if ( (storageScope & Chk::Scope::Game) == Chk::Scope::Game )
    {
        if ( (userMask & Chk::StringUserFlag::Sound) != Chk::StringUserFlag::None )
        {
            for ( size_t i=0; i<Chk::TotalSounds; i++ )
            {
                if ( read.soundPaths[i] == stringId )
                    stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::Sound, i));
            }
        }
        if ( (userMask & Chk::StringUserFlag::Switch) != Chk::StringUserFlag::None )
        {
            for ( size_t i=0; i<Chk::TotalSwitches; i++ )
            {
                if ( read.switchNames[i] == stringId )
                    stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::Switch, i));
            }
        }
        if ( (userMask & Chk::StringUserFlag::AnyTrigger) != Chk::StringUserFlag::None )
        {
            size_t numTriggers = read.triggers.size();
            for ( size_t trigIndex=0; trigIndex<numTriggers; trigIndex++ )
            {
                const auto & trigger = read.triggers[trigIndex];
                for ( size_t actionIndex=0; actionIndex<Chk::Trigger::MaxActions; actionIndex++ )
                {
                    if ( (userMask & Chk::StringUserFlag::TriggerAction) == Chk::StringUserFlag::TriggerAction &&
                        trigger.actions[actionIndex].stringUsed(stringId, Chk::StringUserFlag::TriggerAction) )
                    {
                        stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::TriggerAction, trigIndex, actionIndex));
                    }
                    if ( (userMask & Chk::StringUserFlag::TriggerActionSound) == Chk::StringUserFlag::TriggerActionSound &&
                        trigger.actions[actionIndex].stringUsed(stringId, Chk::StringUserFlag::TriggerActionSound) )
                    {
                        stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::TriggerActionSound, trigIndex, actionIndex));
                    }
                }
            }
        }
        if ( (userMask & Chk::StringUserFlag::AnyBriefingTrigger) != Chk::StringUserFlag::None )
        {
            size_t numBriefingTriggers = read.briefingTriggers.size();
            for ( size_t briefingTrigIndex=0; briefingTrigIndex<numBriefingTriggers; briefingTrigIndex++ )
            {
                const auto & briefingTrigger = read.briefingTriggers[briefingTrigIndex];
                for ( size_t actionIndex=0; actionIndex<Chk::Trigger::MaxActions; actionIndex++ )
                {
                    if ( (userMask & Chk::StringUserFlag::BriefingTriggerAction) == Chk::StringUserFlag::BriefingTriggerAction &&
                        briefingTrigger.actions[actionIndex].briefingStringUsed(stringId, Chk::StringUserFlag::BriefingTriggerAction) )
                    {
                        stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::BriefingTriggerAction, briefingTrigIndex, actionIndex));
                    }
                    if ( (userMask & Chk::StringUserFlag::BriefingTriggerActionSound) == Chk::StringUserFlag::BriefingTriggerActionSound &&
                        briefingTrigger.actions[actionIndex].briefingStringUsed(stringId, Chk::StringUserFlag::BriefingTriggerActionSound) )
                    {
                        stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::BriefingTriggerActionSound, briefingTrigIndex, actionIndex));
                    }
                }
            }
        }
    }
    if ( (storageScope & Chk::Scope::Editor) == Chk::Scope::Editor && (userMask & Chk::StringUserFlag::AnyTriggerExtension) != Chk::StringUserFlag::None )
    {
        for ( const auto & extendedTrig : read.triggerExtensions )
        {
            if ( ((userMask & Chk::StringUserFlag::ExtendedTriggerComment) == Chk::StringUserFlag::ExtendedTriggerComment &&
                extendedTrig.commentStringId == stringId) )
            {
                stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::ExtendedTriggerComment, extendedTrig.trigNum));
            }
            if ( ((userMask & Chk::StringUserFlag::ExtendedTriggerNotes) == Chk::StringUserFlag::ExtendedTriggerNotes &&
                extendedTrig.notesStringId == stringId))
            {
                stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::ExtendedTriggerNotes, extendedTrig.trigNum));
            }
        }
    }
}

bool Scenario::triggerStringUsed(size_t stringId, Chk::Scope storageScope, u32 userMask) const
{
    if ( storageScope == Chk::Scope::Game )
    {
        if ( (userMask & Chk::StringUserFlag::Sound) == Chk::StringUserFlag::Sound && this->stringIsSound(stringId) )
            return true;
        if ( (userMask & Chk::StringUserFlag::Switch) == Chk::StringUserFlag::Switch )
        {
            u32 u32StringId = (u32)stringId;
            for ( size_t i=0; i<Chk::TotalSwitches; i++ )
            {
                if ( read.switchNames[i] == u32StringId )
                    return true;
            }
        }
        if ( (userMask & Chk::StringUserFlag::AnyTrigger) > 0 )
        {
            for ( const auto & trigger : read.triggers )
            {
                if ( trigger.stringUsed(stringId, userMask) )
                    return true;
            }
        }
        if ( (userMask & Chk::StringUserFlag::AnyBriefingTrigger) > 0 )
        {
            for ( const auto & briefingTrigger : read.briefingTriggers )
            {
                if ( briefingTrigger.briefingStringUsed(stringId, userMask) )
                    return true;
            }
        }
    }
    else if ( storageScope == Chk::Scope::Editor && (userMask & Chk::StringUserFlag::AnyTriggerExtension) > 0 )
    {
        for ( const auto & extendedTrig : read.triggerExtensions )
        {
            if ( ((userMask & Chk::StringUserFlag::ExtendedTriggerComment) == Chk::StringUserFlag::ExtendedTriggerComment &&
                    extendedTrig.commentStringId == stringId) ||
                ((userMask & Chk::StringUserFlag::ExtendedTriggerNotes) == Chk::StringUserFlag::ExtendedTriggerNotes &&
                    extendedTrig.notesStringId == stringId) )
            {
                return true;
            }
        }
    }
    return false;
}

bool Scenario::triggerGameStringUsed(size_t stringId, u32 userMask) const
{
    if ( (userMask & Chk::StringUserFlag::AnyTrigger) > 0 )
    {
        for ( const auto & trigger : read.triggers )
        {
            if ( trigger.gameStringUsed(stringId, userMask) )
                return true;
        }
    }
    if ( (userMask & Chk::StringUserFlag::AnyBriefingTrigger) > 0 )
    {
        for ( auto briefingTrigger : read.briefingTriggers )
        {
            if ( briefingTrigger.briefingStringUsed(stringId, userMask) )
                return true;
        }
    }
    return false;
}

bool Scenario::triggerEditorStringUsed(size_t stringId, Chk::Scope storageScope, u32 userMask) const
{
    if ( storageScope == Chk::Scope::Game )
    {
        if ( (userMask & Chk::StringUserFlag::Sound) == Chk::StringUserFlag::Sound )
        {
            return stringIsSound(stringId);
        }
        if ( (userMask & Chk::StringUserFlag::Switch) == Chk::StringUserFlag::Switch )
        {
            u32 u32StringId = (u32)stringId;
            for ( size_t i=0; i<Chk::TotalSwitches; i++ )
            {
                if ( read.switchNames[i] == u32StringId )
                    return true;
            }
        }
        if ( (userMask & Chk::StringUserFlag::TriggerAction) == Chk::StringUserFlag::TriggerAction )
        {
            for ( const auto & trigger : read.triggers )
            {
                if ( trigger.commentStringUsed(stringId) )
                    return true;
            }
        }
        if ( (userMask & Chk::StringUserFlag::AnyTriggerExtension) > 0 )
        {
            for ( const auto & extendedTrig : read.triggerExtensions )
            {
                if ( ((userMask & Chk::StringUserFlag::ExtendedTriggerComment) == Chk::StringUserFlag::ExtendedTriggerComment &&
                        extendedTrig.commentStringId == stringId) ||
                    ((userMask & Chk::StringUserFlag::ExtendedTriggerNotes) == Chk::StringUserFlag::ExtendedTriggerNotes &&
                        extendedTrig.notesStringId == stringId) )
                {
                    return true;
                }
            }
        }
    }
    else if ( storageScope == Chk::Scope::Editor && (userMask & Chk::StringUserFlag::AnyTriggerExtension) > 0 )
    {
        for ( const auto & extendedTrig : read.triggerExtensions )
        {
            if ( ((userMask & Chk::StringUserFlag::ExtendedTriggerComment) == Chk::StringUserFlag::ExtendedTriggerComment &&
                    extendedTrig.commentStringId == stringId) ||
                ((userMask & Chk::StringUserFlag::ExtendedTriggerNotes) == Chk::StringUserFlag::ExtendedTriggerNotes &&
                    extendedTrig.notesStringId == stringId) )
            {
                return true;
            }
        }
    }
    return false;
}

void Scenario::markUsedTriggerLocations(std::bitset<Chk::TotalLocations+1> & locationIdUsed) const
{
    for ( const auto & trigger : read.triggers )
        trigger.markUsedLocations(locationIdUsed);
}

void Scenario::markUsedTriggerStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, Chk::Scope storageScope, u32 userMask) const
{
    return read.markUsedTriggerStrings(stringIdUsed, storageScope, userMask);
}

void Scenario::markUsedTriggerGameStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask) const
{
    return read.markUsedTriggerGameStrings(stringIdUsed, userMask);
}

void Scenario::markUsedTriggerEditorStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, Chk::Scope storageScope, u32 userMask) const
{
    return read.markUsedTriggerEditorStrings(stringIdUsed, storageScope, userMask);
}

void Scenario::remapTriggerLocationIds(const std::map<u32, u32> & locationIdRemappings)
{
    auto edit = create_action();
    for ( std::size_t i=0; i<read.triggers.size(); ++i )
        editTrigger(i).remapLocationIds(locationIdRemappings);
}

void Scenario::remapTriggerStringIds(const std::map<u32, u32> & stringIdRemappings, Chk::Scope storageScope)
{
    auto edit = create_action();
    if ( storageScope == Chk::Scope::Game )
    {
        for ( size_t i=0; i<Chk::TotalSounds; i++ )
        {
            auto found = stringIdRemappings.find(read.soundPaths[i]);
            if ( found != stringIdRemappings.end() )
                edit->soundPaths[i] = found->second;
        }
        for ( size_t i=0; i<Chk::TotalSwitches; i++ )
        {
            auto found = stringIdRemappings.find(read.switchNames[i]);
            if ( found != stringIdRemappings.end() )
                edit->switchNames[i] = found->second;
        }
        for ( std::size_t i=0; i<read.triggers.size(); ++i )
            editTrigger(i).remapStringIds(stringIdRemappings);
        for ( std::size_t i=0; i<read.briefingTriggers.size(); ++i )
            editBriefingTrigger(i).remapBriefingStringIds(stringIdRemappings);
    }
    else if ( storageScope == Chk::Scope::Editor )
    {
        for ( std::size_t i=0; i<read.triggerExtensions.size(); ++i )
        {
            const auto & extendedTrig = read.triggerExtensions[i];
            auto replacement = stringIdRemappings.find(extendedTrig.commentStringId);
            if ( replacement != stringIdRemappings.end() )
                edit->triggerExtensions[i].commentStringId = replacement->second;

            replacement = stringIdRemappings.find(extendedTrig.notesStringId);
            if ( replacement != stringIdRemappings.end() )
                edit->triggerExtensions[i].notesStringId = replacement->second;
        }
    }
}

void Scenario::deleteTriggerLocation(size_t locationId)
{
    for ( std::size_t i=0; i<read.triggers.size(); ++i )
        editTrigger(i).deleteLocation(locationId);
}

void Scenario::deleteTriggerString(size_t stringId, Chk::Scope storageScope)
{
    auto edit = create_action();
    if ( storageScope == Chk::Scope::Game )
    {
        for ( size_t i=0; i<Chk::TotalSounds; i++ )
        {
            if ( read.soundPaths[i] == stringId )
                edit->soundPaths[i] = 0;
        }
        for ( size_t i=0; i<Chk::TotalSwitches; i++ )
        {
            if ( read.switchNames[i] == stringId )
                edit->switchNames[i] = 0;
        }
        for ( std::size_t i=0; i<read.triggers.size(); ++i )
            editTrigger(i).deleteString(stringId);
        for ( std::size_t i=0; i<read.briefingTriggers.size(); ++i )
            editBriefingTrigger(i).deleteBriefingString(stringId);
    }
    else if ( storageScope == Chk::Scope::Editor )
    {
        for ( std::size_t i=0; i<read.triggerExtensions.size(); ++i )
        {
            if ( read.triggerExtensions[i].commentStringId == stringId )
                edit->triggerExtensions[i].commentStringId = Chk::StringId::NoString;

            if ( read.triggerExtensions[i].notesStringId == stringId )
                edit->triggerExtensions[i].notesStringId = Chk::StringId::NoString;
        }
    }
}

void Scenario::setActionDescription(ActionDescriptor actionDescriptor)
{
    tracked::replace_pending_action_user_data({actionDescriptor});
}

bool Scenario::clearTileSelChanged()
{
    if ( tileSelChanged )
    {
        tileSelChanged = false;
        return true;
    }
    return false;
}

bool Scenario::clearFogSelChanged()
{
    if ( fogSelChanged )
    {
        fogSelChanged = false;
        return true;
    }
    return false;
}

void Scenario::selections_changed(tiles_path)
{
    tileSelChanged = true;
}

void Scenario::selections_changed(editor_tiles_path)
{
    tileSelChanged = true;
}

void Scenario::selections_changed(tiles_fog_path)
{
    fogSelChanged = true;
}

template <class Edit>
void Scenario::clearIsomRectFlags(Edit & edit, Chk::IsomRect::Point point)
{
    auto isomIndex = point.y*getIsomWidth()+point.x;
    const auto & isomRect = read.isomRects[isomIndex];
            
    edit->isomRects[isomIndex].left &= Chk::IsomRect::EditorFlag::ClearAll;
    edit->isomRects[isomIndex].top &= Chk::IsomRect::EditorFlag::ClearAll;
    edit->isomRects[isomIndex].right &= Chk::IsomRect::EditorFlag::ClearAll;
    edit->isomRects[isomIndex].bottom &= Chk::IsomRect::EditorFlag::ClearAll;
}

template <class Edit>
void Scenario::setIsomRectModified(Edit & edit, Chk::IsomRect::Point point, Sc::Isom::ProjectedQuadrant quadrant)
{
    auto isomIndex = point.y*getIsomWidth()+point.x;
    const auto & isomRect = read.isomRects[isomIndex];
    switch ( quadrant.firstSide ) {
        case Chk::IsomRect::Side::Left: edit->isomRects[isomIndex].left |= Chk::IsomRect::EditorFlag::Modified; break;
        case Chk::IsomRect::Side::Top: edit->isomRects[isomIndex].top |= Chk::IsomRect::EditorFlag::Modified; break;
        case Chk::IsomRect::Side::Right: edit->isomRects[isomIndex].right |= Chk::IsomRect::EditorFlag::Modified; break;
        default: /*Side::Bottom*/ edit->isomRects[isomIndex].bottom |= Chk::IsomRect::EditorFlag::Modified; break;
    }
    switch ( quadrant.secondSide ) {
        case Chk::IsomRect::Side::Left: edit->isomRects[isomIndex].left |= Chk::IsomRect::EditorFlag::Modified; break;
        case Chk::IsomRect::Side::Top: edit->isomRects[isomIndex].top |= Chk::IsomRect::EditorFlag::Modified; break;
        case Chk::IsomRect::Side::Right: edit->isomRects[isomIndex].right |= Chk::IsomRect::EditorFlag::Modified; break;
        default: /*Side::Bottom*/ edit->isomRects[isomIndex].bottom |= Chk::IsomRect::EditorFlag::Modified; break;
    }
}

template <class Edit>
void Scenario::setIsomRectVisited(Edit & edit, Chk::IsomRect::Point point)
{
    auto isomIndex = point.y*getIsomWidth()+point.x;
    edit->isomRects[isomIndex].right |= Chk::IsomRect::EditorFlag::Visited;
}

template <class Edit>
void Scenario::setIsomRectValue(Edit & edit, Chk::IsomRect::Point point, Sc::Isom::ProjectedQuadrant quadrant, uint16_t value)
{
    auto isomIndex = point.y*getIsomWidth()+point.x;
    const auto & isomRect = read.isomRects[isomIndex];
    switch ( quadrant.firstSide ) {
        case Chk::IsomRect::Side::Left: edit->isomRects[isomIndex].left = (value << 4) | quadrant.firstEdgeFlags; break;
        case Chk::IsomRect::Side::Top: edit->isomRects[isomIndex].top = (value << 4) | quadrant.firstEdgeFlags; break;
        case Chk::IsomRect::Side::Right: edit->isomRects[isomIndex].right = (value << 4) | quadrant.firstEdgeFlags; break;
        default: /*Side::Bottom*/ edit->isomRects[isomIndex].bottom = (value << 4) | quadrant.firstEdgeFlags; break;
    }
            
    switch ( quadrant.secondSide ) {
        case Chk::IsomRect::Side::Left: edit->isomRects[isomIndex].left = (value << 4) | quadrant.secondEdgeFlags; break;
        case Chk::IsomRect::Side::Top: edit->isomRects[isomIndex].top = (value << 4) | quadrant.secondEdgeFlags; break;
        case Chk::IsomRect::Side::Right: edit->isomRects[isomIndex].right = (value << 4) | quadrant.secondEdgeFlags; break;
        default: /*Side::Bottom*/ edit->isomRects[isomIndex].bottom = (value << 4) | quadrant.secondEdgeFlags; break;
    }
}

bool Scenario::diamondNeedsUpdate(Chk::IsomDiamond isomDiamond) const
{
    return isInBounds(isomDiamond) &&
        !centralIsomValueModified(isomDiamond) &&
        getCentralIsomValue(isomDiamond) != 0;
}

void Scenario::setIsomValue(Chk::IsomRect::Point isomDiamond, Sc::Isom::Quadrant shapeQuadrant, uint16_t isomValue, Chk::IsomCache & cache)
{
    if ( isInBounds(isomDiamond) )
    {
        auto edit = create_action();
        size_t isomRectIndex = isomDiamond.y*cache.isomWidth + size_t(isomDiamond.x);

        setIsomRectValue(edit, isomDiamond, shapeQuadrant, isomValue);
        setIsomRectModified(edit, isomDiamond, shapeQuadrant);
        cache.changedArea.expandToInclude(isomDiamond.x, isomDiamond.y);
    }
}

void Scenario::setDiamondIsomValues(Chk::IsomDiamond isomDiamond, uint16_t isomValue, Chk::IsomCache & cache)
{
    setIsomValue(isomDiamond.getRectangleCoords(Sc::Isom::Quadrant::TopLeft), Sc::Isom::Quadrant::TopLeft, isomValue, cache);
    setIsomValue(isomDiamond.getRectangleCoords(Sc::Isom::Quadrant::TopRight), Sc::Isom::Quadrant::TopRight, isomValue, cache);
    setIsomValue(isomDiamond.getRectangleCoords(Sc::Isom::Quadrant::BottomRight), Sc::Isom::Quadrant::BottomRight, isomValue, cache);
    setIsomValue(isomDiamond.getRectangleCoords(Sc::Isom::Quadrant::BottomLeft), Sc::Isom::Quadrant::BottomLeft, isomValue, cache);
}

void Scenario::loadNeighborInfo(Chk::IsomDiamond isomDiamond, IsomNeighbors & neighbors, Span<Sc::Isom::ShapeLinks> isomLinks) const
{
    for ( auto i : Chk::IsomDiamond::neighbors ) // Gather info about the four neighboring isom diamonds/isom shapes
    {
        Chk::IsomDiamond neighbor = isomDiamond.getNeighbor(i);
        if ( isInBounds(neighbor) )
        {
            uint16_t isomValue = getCentralIsomValue(neighbor);
            neighbors[i].modified = centralIsomValueModified(neighbor);
            neighbors[i].isomValue = isomValue;
            if ( isomValue < isomLinks.size() )
            {
                neighbors[i].linkId = isomLinks[isomValue].getLinkId(Sc::Isom::OppositeQuadrant(i));
                if ( neighbors[i].modified && isomLinks[isomValue].terrainType > neighbors.maxModifiedOfFour )
                    neighbors.maxModifiedOfFour = isomLinks[isomValue].terrainType;
            }
        }
    }
}

uint16_t Scenario::countNeighborMatches(const Sc::Isom::ShapeLinks & shapeLinks, IsomNeighbors & neighbors, Span<Sc::Isom::ShapeLinks> isomLinks) const
{
    auto terrainType = shapeLinks.terrainType;
    uint16_t totalMatches = 0;
    for ( auto quadrant : Sc::Isom::quadrants ) // For each quadrant in the shape (and each neighbor which overlaps with said quadrant)
    {
        const auto & neighborShape = isomLinks[neighbors[quadrant].isomValue < isomLinks.size() ? neighbors[quadrant].isomValue : 0];
        auto neighborTerrainType = neighborShape.terrainType;
        auto neighborLinkId = neighbors[quadrant].linkId;
        auto quadrantLinkId = shapeLinks.getLinkId(quadrant);

        if ( neighborLinkId == quadrantLinkId && (quadrantLinkId < Sc::Isom::LinkId::OnlyMatchSameType || terrainType == neighborTerrainType) )
            ++totalMatches;
        else if ( neighbors[quadrant].modified ) // There was no match with a neighbor that was already modified, so this isomValue can't be valid
            return uint16_t(0);
    }
    return totalMatches;
}

void Scenario::searchForBestMatch(uint16_t startingTerrainType, IsomNeighbors & neighbors, Chk::IsomCache & cache) const
{
    bool searchUntilHigherTerrainType = startingTerrainType == cache.terrainTypes.size()/2+1; // The final search always searches until end or higher types
    bool searchUntilEnd = startingTerrainType == 0; // If startingTerrainType is zero, the whole table after start must be searched

    uint16_t isomValue = cache.getTerrainTypeIsomValue(startingTerrainType);
    for ( ; isomValue < cache.isomLinks.size(); ++isomValue )
    {
        auto terrainType = cache.isomLinks[isomValue].terrainType;
        if ( !searchUntilEnd && terrainType != startingTerrainType && (!searchUntilHigherTerrainType || terrainType > startingTerrainType) )
            break; // Do not search the rest of the table

        auto matchCount = countNeighborMatches(cache.isomLinks[isomValue], neighbors, cache.isomLinks);
        if ( matchCount > neighbors.bestMatch.matchCount )
            neighbors.bestMatch = {isomValue, matchCount};
    }
}

std::optional<uint16_t> Scenario::findBestMatchIsomValue(Chk::IsomDiamond isomDiamond, Chk::IsomCache & cache) const
{
    IsomNeighbors neighbors {};
    loadNeighborInfo(isomDiamond, neighbors, cache.isomLinks);

    uint16_t prevIsomValue = getCentralIsomValue(isomDiamond);
    if ( prevIsomValue < cache.isomLinks.size() )
    {
        uint8_t prevTerrainType = cache.isomLinks[prevIsomValue].terrainType; // Y = maxOfFour, x = prevTerrainType
        uint16_t mappedTerrainType = cache.terrainTypeMap[size_t(neighbors.maxModifiedOfFour)*cache.terrainTypes.size() + size_t(prevTerrainType)];
        searchForBestMatch(mappedTerrainType, neighbors, cache);
    }
    searchForBestMatch(uint16_t(neighbors.maxModifiedOfFour), neighbors, cache);
    searchForBestMatch(uint16_t(cache.terrainTypes.size()/2 + 1), neighbors, cache);

    if ( neighbors.bestMatch.isomValue == prevIsomValue ) // This ISOM diamond was already the best possible value
        return std::nullopt;
    else
        return neighbors.bestMatch.isomValue;
}

void Scenario::radiallyUpdateTerrain(std::deque<Chk::IsomDiamond> & diamondsToUpdate, Chk::IsomCache & cache)
{
    auto edit = create_action();
    while ( !diamondsToUpdate.empty() )
    {
        Chk::IsomDiamond isomDiamond = diamondsToUpdate.front();
        diamondsToUpdate.pop_front();
        if ( diamondNeedsUpdate(isomDiamond) && !getIsomRect(isomDiamond).isVisited() )
        {
            setIsomRectVisited(edit, isomDiamond);
            cache.changedArea.expandToInclude(isomDiamond.x, isomDiamond.y);
            if ( auto bestMatch = findBestMatchIsomValue(isomDiamond, cache) )
            {
                if ( *bestMatch != 0 )
                    setDiamondIsomValues(isomDiamond, *bestMatch, cache);

                for ( auto i : Chk::IsomDiamond::neighbors )
                {
                    Chk::IsomDiamond neighbor = isomDiamond.getNeighbor(i);
                    if ( diamondNeedsUpdate(neighbor) )
                        diamondsToUpdate.push_back({neighbor.x, neighbor.y});
                }
            }
        }
    }
}
