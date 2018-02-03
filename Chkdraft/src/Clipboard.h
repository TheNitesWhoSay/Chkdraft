#ifndef CLIPBOARD_H
#define CLIPBOARD_H
#include "CommonFiles/CommonFiles.h"
#include "MappingCore/MappingCore.h"
#include "Selections.h"
#include "Undos.h"
#include "ScData.h"
class GuiMap;

void StringToWindowsClipboard(std::string &str);

bool WindowsClipboardToString(std::string &str);

class PasteTileNode
{
    public:
        u16 value;
        s32 xc;
        s32 yc;
        TileNeighbor neighbors; // Same as in TileNode

        PasteTileNode(u16 value, s32 xc, s32 yc, TileNeighbor neighbors) :
            value(value), xc(xc), yc(yc), neighbors(neighbors) { }
};

class PasteUnitNode
{
    public:
        ChkUnit unit;
        s32 xc;
        s32 yc;

        PasteUnitNode(ChkUnit &unitRef) { std::memcpy(&unit, &unitRef, UNIT_STRUCT_SIZE); xc = unit.xc; yc = unit.yc; }

    private:
        PasteUnitNode(); // Disallow ctor
};

class Clipboard
{
    public:

        Clipboard();
        ~Clipboard();

        bool hasTiles();
        bool hasUnits() { return copyUnits.size() > 0; }
        void copy(GuiMap &map, Layer layer);
        
        void addQuickTile(u16 index, s32 xc, s32 yc);
        bool hasQuickTiles() { return quickTiles.size() > 0; }

        void addQuickUnit(ChkUnit &unitRef);
        bool hasQuickUnits() { return quickUnits.size() > 0; }

        void beginPasting(bool isQuickPaste);
        void endPasting();

        void doPaste(Layer layer, s32 mapClickX, s32 mapClickY, GuiMap &map, Undos &undos, bool allowStack);

        void toggleFillSimilarTiles();

        std::vector<PasteTileNode> &getTiles();
        std::vector<PasteUnitNode> &getUnits();
        bool isPasting() { return pasting; }
        bool isQuickPasting() { return pasting && quickPaste; }
        bool isPreviousPasteLoc(u16 x, u16 y) { return x == prevPaste.x && y == prevPaste.y; }


    protected:

        void pasteTerrain(s32 mapClickX, s32 mapClickY, GuiMap &map, Undos &undos);
        void fillPasteTerrain(s32 mapClickX, s32 mapClickY, GuiMap &map, Undos &undos);
        void pasteUnits(s32 mapClickX, s32 mapClickY, GuiMap &map, Undos &undos, bool allowStack);


    private:

        bool pasting;
        bool quickPaste; /* Controls whether you are using quick placement,
                            as opposed to something saved on clipboard.     */
        bool fillSimilarTiles;
        RECT edges;
        std::vector<PasteTileNode> copyTiles;
        std::vector<PasteTileNode> quickTiles;
        std::vector<PasteUnitNode> copyUnits;
        std::vector<PasteUnitNode> quickUnits;

        POINT prevPaste;

        void ClearCopyTiles();
        void ClearCopyUnits();
        void ClearQuickItems();
};

#endif