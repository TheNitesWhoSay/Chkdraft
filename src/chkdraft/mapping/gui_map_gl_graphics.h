#ifndef GUIMAPGLGRAPHICS_H
#define GUIMAPGLGRAPHICS_H
#include <mapping_core/render/sc_gl_graphics.h>

class GuiMap;

class GuiMapGraphics : public MapGraphics
{
    GuiMap & guiMap; // Reference to the map this instance of graphics renders

public:
    GuiMapGraphics(Sc::Data & data, GuiMap & guiMap);

    void drawTileSelection();
    void drawClipboardActors(bool hasCrgb);
    void drawTemporaryLocations();
    void drawFogTileSelection();
    void drawDoodadSelection();
    void drawPastes();

    void render();
};


#endif