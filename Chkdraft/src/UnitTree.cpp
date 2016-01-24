#include "UnitTree.h"
#include "Chkdraft.h"

bool UnitTree::AddThis(HWND hTree, HTREEITEM hRoot)
{
	if ( TreeViewControl::SetHandle(hTree) )
	{
		InsertAllUnits(hRoot);
		return true;
	}
	else
		return false;
}

bool UnitTree::CreateThis(HWND hParent, int x, int y, int width, int height, bool hasButtons, u32 id)
{
	if ( TreeViewControl::CreateThis(hParent, x, y, width, height, hasButtons, id) )
	{
		InsertAllUnits(NULL);
		return true;
	}
	else
		return false;
}

void UnitTree::UpdateUnitNames(const char** displayNames)
{
	for ( int i=0; i<233; i++ )
		UnitDisplayName[i] = displayNames[i];
}

void UnitTree::UpdateUnitName(int UnitID)
{
	TreeViewControl::SetItemText(UnitItems[UnitID], UnitDisplayName[UnitID]);
}

void UnitTree::InsertAllUnits(HTREEITEM hParent)
{
	InsertZergItems(hParent);
	InsertTerranItems(hParent);
	InsertProtossItems(hParent);
	InsertNeutralItems(hParent);
	InsertUndefinedItems(hParent);
}

void UnitTree::InsertZergItems(HTREEITEM hParent)
{
	const int Units_Zerg_Misc[]				= { 59, 36, 35, 97 },
			  Units_Zerg_Air[]				= { 62, 44, 43, 42, 45, 47 },
			  Units_Zerg_Buildings[]		= { 130, 143, 136, 139, 149, 137, 131, 133, 135, 132, 134, 138, 142, 141, 144, 146, 140 },
			  Units_Zerg_Ground[]			= { 50, 40, 46, 41, 38, 103, 39, 37 },
			  Units_Zerg_Heroes[]			= { 54, 53, 104, 51, 56, 55, 49, 48, 52, 57 },
			  Units_Zerg_Special[]			= { 194, 197, 191 },
			  Units_Zerg_SpecialBuildings[]	= { 150, 201, 151, 152, 148, 147 },
			  Units_Zerg_Zerg[]				= { 145, 153 };

	const int* TreeZerg[] = {
		Units_Zerg_Misc, Units_Zerg_Air, Units_Zerg_Buildings,
		Units_Zerg_Ground, Units_Zerg_Heroes, Units_Zerg_Special,
		Units_Zerg_SpecialBuildings, Units_Zerg_Zerg };
	const int nTreeZerg[] = {
		sizeof(Units_Zerg_Misc)/sizeof(const int), sizeof(Units_Zerg_Air)/sizeof(const int), sizeof(Units_Zerg_Buildings)/sizeof(const int),
		sizeof(Units_Zerg_Ground)/sizeof(const int), sizeof(Units_Zerg_Heroes)/sizeof(const int), sizeof(Units_Zerg_Special)/sizeof(const int),
		sizeof(Units_Zerg_SpecialBuildings)/sizeof(const int), sizeof(Units_Zerg_Zerg)/sizeof(const int) };
	const char* sTreeZerg[] = {
		"*", "Air Units", "Buildings",
		"Ground Units", "Heroes", "Special",
		"Special Buildings", "Zerg" };

	HTREEITEM LastRace = InsertTreeItem(hParent, "Zerg", TREE_TYPE_CATEGORY|LAYER_UNITS);
	for ( int i=0; i<sizeof(TreeZerg)/sizeof(const int*); i++ )
	{
		HTREEITEM LastGroup = InsertTreeItem(LastRace, sTreeZerg[i], TREE_TYPE_CATEGORY|LAYER_UNITS);
		InsertUnits(LastGroup, TreeZerg[i], nTreeZerg[i]);
	}
}

void UnitTree::InsertTerranItems(HTREEITEM hParent)
{
	const int Units_Terran_Misc[]			  = { 18, 24, 26, 4, 33, 6, 31, 91, 92, 119 },
			  Units_Terran_Addons[]			  = { 107, 115, 117, 120, 108, 118 },
			  Units_Terran_Air[]			  = { 12, 11, 9, 58, 8 },
			  Units_Terran_Buildings[]		  = { 112, 123, 111, 125, 106, 122, 113, 124, 110, 116, 114, 109 },
			  Units_Terran_Ground[]			  = { 32, 1, 3, 0, 34, 7, 30, 5, 2 },
			  Units_Terran_Heroes[]			  = { 17, 100, 27, 25, 23, 102, 10, 28, 20, 19, 22, 29, 99, 16, 15, 21 },
			  Units_Terran_Special[]		  = { 14, 195, 198, 192, 13 },
			  Units_Terran_SpecialBuildings[] = { 127, 126, 200, 190 },
			  Units_Terran_Terran[]			  = { 121 };

	const int* TreeTerran[] = {
		Units_Terran_Misc, Units_Terran_Addons, Units_Terran_Air,
		Units_Terran_Buildings, Units_Terran_Ground, Units_Terran_Heroes,
		Units_Terran_Special, Units_Terran_SpecialBuildings, Units_Terran_Terran };
	const int nTreeTerran[] = {
		sizeof(Units_Terran_Misc)/sizeof(const int), sizeof(Units_Terran_Addons)/sizeof(const int), sizeof(Units_Terran_Air)/sizeof(const int),
		sizeof(Units_Terran_Buildings)/sizeof(const int), sizeof(Units_Terran_Ground)/sizeof(const int), sizeof(Units_Terran_Heroes)/sizeof(const int),
		sizeof(Units_Terran_Special)/sizeof(const int), sizeof(Units_Terran_SpecialBuildings)/sizeof(const int), sizeof(Units_Terran_Terran)/sizeof(const int) };
	const char* sTreeTerran[] = {
		"*", "Addons", "Air Units",
		"Buildings", "Ground Units", "Heroes",
		"Special", "Special Buildings", "Terran" };

	HTREEITEM LastRace = InsertTreeItem(hParent, "Terran", TREE_TYPE_CATEGORY|LAYER_UNITS);
	for ( int i=0; i<sizeof(TreeTerran)/sizeof(const int*); i++ )
	{
		HTREEITEM LastGroup = InsertTreeItem(LastRace, sTreeTerran[i], TREE_TYPE_CATEGORY|LAYER_UNITS);
		InsertUnits(LastGroup, TreeTerran[i], nTreeTerran[i]);
	}
}

void UnitTree::InsertProtossItems(HTREEITEM hParent)
{
	const int Units_Protoss_Air[]			   = { 71, 72, 60, 73, 84, 70, 69 },
			  Units_Protoss_Buildings[]		   = { 170, 157, 163, 164, 169, 166, 160, 154, 159, 162, 156, 155, 171, 172, 167, 165 },
			  Units_Protoss_Ground[]		   = { 68, 63, 61, 66, 67, 64, 83, 85, 65 },
			  Units_Protoss_Heroes[]		   = { 87, 88, 86, 74, 78, 77, 82, 80, 98, 79, 76, 81, 75 },
			  Units_Protoss_Protoss[]		   = { 158, 161 },
			  Units_Protoss_Special[]		   = { 196, 199, 193 },
			  Units_Protoss_SpecialBuildings[] = { 173, 174, 168, 189, 175 };

	const int* TreeProtoss[] = {
		Units_Protoss_Air, Units_Protoss_Buildings, Units_Protoss_Ground,
		Units_Protoss_Heroes, Units_Protoss_Protoss, Units_Protoss_Special,
		Units_Protoss_SpecialBuildings };
	const int nTreeProtoss[] = {
		sizeof(Units_Protoss_Air)/sizeof(const int), sizeof(Units_Protoss_Buildings)/sizeof(const int), sizeof(Units_Protoss_Ground)/sizeof(const int),
		sizeof(Units_Protoss_Heroes)/sizeof(const int), sizeof(Units_Protoss_Protoss)/sizeof(const int), sizeof(Units_Protoss_Special)/sizeof(const int),
		sizeof(Units_Protoss_SpecialBuildings)/sizeof(const int) };
	const char* sTreeProtoss[] = {
		"Air Units", "Buildings", "Ground Units",
		"Heroes", "Protoss", "Special",
		"Special Buildings" };

	HTREEITEM LastRace = InsertTreeItem(hParent, "Protoss", TREE_TYPE_CATEGORY|LAYER_UNITS);
	for ( int i=0; i<sizeof(TreeProtoss)/sizeof(const int*); i++ )
	{
		HTREEITEM LastGroup = InsertTreeItem(LastRace, sTreeProtoss[i], TREE_TYPE_CATEGORY|LAYER_UNITS);
		InsertUnits(LastGroup, TreeProtoss[i], nTreeProtoss[i]);
	}
}

void UnitTree::InsertNeutralItems(HTREEITEM hParent)
{
	const int Units_Neutral_Critters[]		= { 90, 94, 95, 89, 93, 96 },
			  Units_Neutral_Doodads[]		= { 209, 204, 203, 207, 205, 211, 210, 208, 206, 213, 212 },
			  Units_Neutral_Neutral[]		= { 181, 179, 180, 185, 187, 182, 186 },
			  Units_Neutral_Powerups[]		= { 218, 129, 219, 217, 128, 216 },
			  Units_Neutral_Protoss[]		= { 105 },
			  Units_Neutral_Resources[]		= { 220, 221, 176, 177, 178, 188, 222, 223, 224, 225, 226, 227 },
			  Units_Neutral_Special[]		= { 215, 101 },
			  Units_Neutral_StartLocation[]	= { 214 },
			  Units_Neutral_Zerg[]			= { 202 };

	const int* TreeNeutral[] = {
		Units_Neutral_Critters, Units_Neutral_Doodads, Units_Neutral_Neutral,
		Units_Neutral_Powerups, Units_Neutral_Protoss, Units_Neutral_Resources,
		Units_Neutral_Special, Units_Neutral_StartLocation, Units_Neutral_Zerg };
	const int nTreeNeutral[] = {
		sizeof(Units_Neutral_Critters)/sizeof(const int), sizeof(Units_Neutral_Doodads)/sizeof(const int), sizeof(Units_Neutral_Neutral)/sizeof(const int),
		sizeof(Units_Neutral_Powerups)/sizeof(const int), sizeof(Units_Neutral_Protoss)/sizeof(const int), sizeof(Units_Neutral_Resources)/sizeof(const int),
		sizeof(Units_Neutral_Special)/sizeof(const int), sizeof(Units_Neutral_StartLocation)/sizeof(const int), sizeof(Units_Neutral_Zerg)/sizeof(const int) };
	const char* sTreeNeutral[] = {
		"Critters", "Doodads", "Neutral",
		"Powerups", "Protoss", "Resources",
		"Special", "Start Location", "Zerg" };

	HTREEITEM LastRace = InsertTreeItem(hParent, "Neutral", TREE_TYPE_CATEGORY|LAYER_UNITS);
	for ( int i=0; i<sizeof(TreeNeutral)/sizeof(const int*); i++ )
	{
		HTREEITEM LastGroup = InsertTreeItem(LastRace, sTreeNeutral[i], TREE_TYPE_CATEGORY|LAYER_UNITS);
		InsertUnits(LastGroup, TreeNeutral[i], nTreeNeutral[i]);
	}
}

void UnitTree::InsertUndefinedItems(HTREEITEM hParent)
{
	const int Units_Independent[] = { 183, 184 };

	const int* TreeUndefined[] = { Units_Independent };
	const int nTreeUndefined[] = { sizeof(Units_Independent)/sizeof(const int) };
	const char* sTreeUndefined[] = { "Independent" };

	HTREEITEM LastRace = InsertTreeItem(hParent, "Undefined", TREE_TYPE_CATEGORY|LAYER_UNITS);
	for ( int i=0; i<sizeof(TreeUndefined)/sizeof(const int*); i++ )
	{
		HTREEITEM LastGroup = InsertTreeItem(LastRace, sTreeUndefined[i], TREE_TYPE_CATEGORY|LAYER_UNITS);
		InsertUnits(LastGroup, TreeUndefined[i], nTreeUndefined[i]);
	}
}

void UnitTree::InsertUnits(HTREEITEM hParent, const int* items, int amount)
{
	for ( int i=0; i<amount; i++ )
		UnitItems[items[i]] = InsertTreeItem(hParent, UnitDisplayName[items[i]], TREE_TYPE_UNIT|items[i]);
}
