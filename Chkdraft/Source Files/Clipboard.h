#ifndef CLIPBOARD_H
#define CLIPBOARD_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"
#include "Selections.h"
#include "Undo.h"
#include "Data.h"

class PasteTileNode
{
	public:
		u16 value;
		s32 xc;
		s32 yc;
		u8 neighbors; // Same as in TileNode
		PasteTileNode* next;

		PasteTileNode(u16 value, s32 xc, s32 yc, u8 neighbors, PasteTileNode* next) :
			value(value), xc(xc), yc(yc), neighbors(neighbors), next(next) { }
};

class PasteUnitNode
{
	public:
		ChkUnit unit;
		s32 xc;
		s32 yc;
		PasteUnitNode* next;

		PasteUnitNode(ChkUnit* unitRef) : next(nullptr) { unit = (*unitRef); xc = unit.xc; yc = unit.yc; }
		PasteUnitNode(ChkUnit* unitRef, PasteUnitNode* next) : next(next) { unit = (*unitRef); xc = unit.xc; yc = unit.yc; }

	private:
		PasteUnitNode(); // Disallow ctor
};

class CLIPBOARD
{
	public:

		CLIPBOARD();
		~CLIPBOARD();

		bool hasTiles();
		bool hasUnits() { return headCopyUnit != nullptr; }
		void copy(SELECTIONS* selection, Scenario* chk, u8 layer);
		
		void addQuickTile(u16 index, s32 xc, s32 yc);
		bool hasQuickTiles() { return headQuickTile != nullptr; }

		void addQuickUnit(ChkUnit* unitRef);
		bool hasQuickUnits() { return headQuickUnit != nullptr; }

		void beginPasting(bool isQuickPaste);
		void endPasting();

		void doPaste(u8 layer, s32 mapClickX, s32 mapClickY, Scenario* chk, UNDOS& undos, bool allowStack);

		PasteTileNode* getFirstTile();
		PasteUnitNode* getFirstUnit();
		bool isPasting() { return pasting; }
		bool isQuickPasting() { return pasting && quickPaste; }
		bool isPreviousPasteLoc(u16 x, u16 y) { return x == prevPaste.x && y == prevPaste.y; }

	private:

		bool pasting;
		bool quickPaste; /* Controls whether you are using quick placement,
							as opposed to something saved on clipboard.		*/
		RECT edges;
		PasteTileNode* headCopyTile;
		PasteTileNode* headQuickTile;
		PasteUnitNode* headCopyUnit;
		PasteUnitNode* headQuickUnit;

		POINT prevPaste;

		void ClearCopyTiles();
		void ClearCopyUnits();
		void ClearQuickItems();
};

#endif