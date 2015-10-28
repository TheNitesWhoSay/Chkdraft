#ifndef MAPS_H
#define MAPS_H
#include "Common Files/CommonFiles.h"
#include "GuiMap.h"

struct MapNode
{
	GuiMap map;
	u16 mapID;
	MapNode* next;
};

class MAPS : public MdiClient
{
	public:
		GuiMap* curr;

		CLIPBOARD clipboard;

		MAPS();
		~MAPS();

		void FocusActive();
		void Focus(HWND hFocus);
		GuiMap* GetMap(HWND hMap);
		GuiMap* GetMap(u16 mapID);
		u16 GetMapID(GuiMap* map);

		bool NewMap(u16 width, u16 height, u16 tileset, u32 terrain, u32 triggers);
		bool OpenMap(const char* fileName);
		bool OpenMap(); // Accelerator for OpenMap
		bool SaveCurr(bool saveAs);
		void CloseMap(HWND hMap);
		void CloseActive();

		void UpdateTreeView();
		void SetGrid(s16 xSize, s16 ySize);
		void SetGridColor(u8 red, u8 green, u8 blue);
		void ChangeLayer(u8 newLayer);
		void ChangeZoom(bool increment);
		void ChangePlayer(u8 newPlayer);
		void cut();
		void copy();
		void startPaste(bool isQuickPaste);
		void endPaste();
		void properties();

		void stickCursor(); // Ensures that the cursor does revert during click & drags
		void updateCursor(s32 xc, s32 yc);

	protected:
		void PushNode(MapNode* map);

	private:
		MapNode* firstMap;
		int UntitledNumber;
		u16 lastUsedMapID;

		bool nonStandardCursor;
		HCURSOR* currCursor;
		HCURSOR standardCursor;
		HCURSOR sizeAllCursor;
		HCURSOR neswCursor;
		HCURSOR nwseCursor;
		HCURSOR nsCursor;
		HCURSOR weCursor;
};

#endif