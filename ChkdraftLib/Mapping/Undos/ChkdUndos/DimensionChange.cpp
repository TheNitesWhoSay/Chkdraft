#include "DimensionChange.h"
#include "../../../Chkdraft.h"
#include "../../../Windows/MainWindows/GuiMap.h"

DimensionChange::~DimensionChange()
{

}

std::shared_ptr<DimensionChange> DimensionChange::Make(void* guiMap)
{
    return std::shared_ptr<DimensionChange>(new DimensionChange(guiMap));
}

void DimensionChange::Reverse(void *guiMap)
{
    Scenario* scenario = (GuiMap*)guiMap;
    std::swap(this->dimensions, scenario->dimensions);
    std::swap(this->tileset, scenario->tileset);
    std::swap(this->tiles, scenario->tiles);
    std::swap(this->editorTiles, scenario->editorTiles);
    std::swap(this->tileFog, scenario->tileFog);
    std::swap(this->isomRects, scenario->isomRects);
    std::swap(this->doodads, scenario->doodads);
    std::swap(this->sprites, scenario->sprites);
    std::swap(this->units, scenario->units);
    std::swap(this->locations, scenario->locations);
    ((GuiMap*)guiMap)->refreshScenario();
    ((GuiMap*)guiMap)->Redraw(true);
}

int32_t DimensionChange::GetType()
{
    return UndoTypes::TerrainChange;
}

DimensionChange::DimensionChange(void* guiMap)
{
    Scenario* scenario = (GuiMap*)guiMap;
    this->dimensions = scenario->dimensions;
    this->tileset = scenario->tileset;
    this->tiles = scenario->tiles;
    this->editorTiles = scenario->editorTiles;
    this->tileFog = scenario->tileFog;
    this->isomRects = scenario->isomRects;
    this->doodads = scenario->doodads;
    this->sprites = scenario->sprites;
    this->units = scenario->units;
    this->locations = scenario->locations;
}
