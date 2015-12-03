#ifndef CLIPBOARD_H
#define CLIPBOARD_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"
#include "Selections.h"
#include "Undo.h"
#include "Data.h"

void StringToWindowsClipboard(std::string &str);

class PasteTileNode
{
	public:
		u16 value;
		s32 xc;
		s32 yc;
		u8 neighbors; // Same as in TileNode

		PasteTileNode(u16 value, s32 xc, s32 yc, u8 neighbors) :
			value(value), xc(xc), yc(yc), neighbors(neighbors) { }
};

class PasteUnitNode
{
	public:
		ChkUnit unit;
		s32 xc;
		s32 yc;

		PasteUnitNode(ChkUnit* unitRef) { std::memcpy(&unit, unitRef, UNIT_STRUCT_SIZE); xc = unit.xc; yc = unit.yc; }

	private:
		PasteUnitNode(); // Disallow ctor
};

class CLIPBOARD
{
	public:

		CLIPBOARD();
		~CLIPBOARD();

		bool hasTiles();
		bool hasUnits() { return copyUnits.size() > 0; }
		void copy(SELECTIONS* selection, Scenario* chk, u8 layer);
		
		void addQuickTile(u16 index, s32 xc, s32 yc);
		bool hasQuickTiles() { return quickTiles.size() > 0; }

		void addQuickUnit(ChkUnit* unitRef);
		bool hasQuickUnits() { return quickUnits.size() > 0; }

		void beginPasting(bool isQuickPaste);
		void endPasting();

		void doPaste(u8 layer, s32 mapClickX, s32 mapClickY, Scenario* chk, UNDOS& undos, bool allowStack);

		std::vector<PasteTileNode> &getTiles();
		std::vector<PasteUnitNode> &getUnits();
		bool isPasting() { return pasting; }
		bool isQuickPasting() { return pasting && quickPaste; }
		bool isPreviousPasteLoc(u16 x, u16 y) { return x == prevPaste.x && y == prevPaste.y; }

	private:

		bool pasting;
		bool quickPaste; /* Controls whether you are using quick placement,
							as opposed to something saved on clipboard.		*/
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