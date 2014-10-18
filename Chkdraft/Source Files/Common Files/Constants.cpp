#include "Constants.h"
#include "CommonFiles.h"

const HFONT defaultFont = CreateFont(14, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Microsoft Sans Serif");

const double zooms[] =
{
	4.0, 3.0, 2.0, 1.5, 1.0, .66, .50, .33, .25, .10
};

const u32 onOffMenuItems[] =
{
	ID_FILE_CLOSE1, ID_FILE_SAVE1, ID_FILE_SAVEAS1,
	ID_EDIT_UNDO1, ID_EDIT_REDO1, ID_EDIT_CUT1, ID_EDIT_COPY1, ID_EDIT_PASTE1, ID_EDIT_SELECTALL, ID_EDIT_DELETE, ID_EDIT_CLEARSELECTIONS,
	ID_GRID_ULTRAFINE, ID_GRID_FINE, ID_GRID_NORMAL, ID_GRID_LARGE, ID_GRID_EXTRALARGE, ID_GRID_DISABLED,
	ID_COLOR_BLACK, ID_COLOR_GREY, ID_COLOR_WHITE, ID_COLOR_GREEN, ID_COLOR_RED, ID_COLOR_BLUE,
	ID_ZOOM_400, ID_ZOOM_300, ID_ZOOM_200, ID_ZOOM_150, ID_ZOOM_100, ID_ZOOM_66, ID_ZOOM_50, ID_ZOOM_33, ID_ZOOM_25, ID_ZOOM_10,
	ID_TERRAIN_DISPLAYTILEVALUES, ID_TERRAIN_DISPLAYTILEVALUESMTXM,
	ID_UNITS_UNITSSNAPTOGRID, ID_UNITS_ALLOWSTACK,
	ID_LOCATIONS_NOSNAP, ID_LOCATIONS_SNAPTOACTIVEGRID, ID_LOCATIONS_SNAPTOTILE, ID_LOCATIONS_LOCKANYWHERE, ID_LOCATIONS_CLIPNAMES,
	ID_SCENARIO_DESCRIPTION, ID_SCENARIO_FORCES, ID_SCENARIO_UNITSETTINGS, ID_SCENARIO_UPGRADESETTINGS, ID_SCENARIO_TECHSETTINGS,
	ID_SCENARIO_STRINGS, ID_SCENARIO_SOUNDEDITOR,
	ID_WINDOWS_CASCADE, ID_WINDOWS_TILEHORIZONTALLY, ID_WINDOWS_TILEVERTICALLY, ID_WINDOW_CLOSE
};

const int numOnOffMenuItems = sizeof(onOffMenuItems)/sizeof(const u32);


const char* upgradeNames[] = {
	"Terran Infantry Armor",
	"Terran Vehicle Plating",
	"Terran Ship Plating",
	"Zerg Carapace",
	"Zerg Flyer Carapace",
	"Protoss Armor",
	"Protoss Plating",
	"Terran Infantry Weapons",
	"Terran Vehicle Weapons",
	"Terran Ship Weapons",
	"Zerg Melee Attacks",
	"Zerg Missile Attacks",
	"Zerg Flyer Attacks",
	"Protoss Ground Weapons",
	"Protoss Air Weapons",
	"Protoss Plasma Shields",
	"U-238 Shells",
	"Ion Thrusters",
	"Burst Lasers (Unused)",
	"Titan Reactor (SV +50)",
	"Ocular Implants",
	"Moebius Reactor (Ghost +50)",
	"Apollo Reactor (Wraith +50)",
	"Colossus Reactor (BC +50)",
	"Ventral Sacs",
	"Antennae",
	"Pneumatized Carapace",
	"Metabolic Boost",
	"Adrenal Glands",
	"Muscular Augments",
	"Grooved Spines",
	"Gamete Meiosis (Queen +50)",
	"Metasynaptic Node (Defiler +50)",
	"Singularity Charge",
	"Leg Enhancements",
	"Scarab Damage",
	"Reaver Capacity",
	"Gravitic Drive",
	"Sensor Array",
	"Gravitic Boosters",
	"Khaydarin Amulet (HT +50)",
	"Apial Sensors",
	"Gravitic Thrusters",
	"Carrier Capacity",
	"Khaydarin Core (Arbiter +50)",
	"Unused Upgrade (45)",
	"Unused Upgrade (46)",
	"Argus Jewel (Corsair +50)",
	"Unused Upgrade (48)",
	"Argus Talisman (DA +50)",
	"Unused Upgrade (50)",
	"Caduceus Reactor (Medic +50)",
	"Chitinous Plating",
	"Anabolic Synthesis",
	"Charon Booster",
	"Unused Upgrade (55)",
	"Unused Upgrade (56)",
	"Unused Upgrade (57)",
	"Unused Upgrade (58)",
	"Unused Upgrade (59)",
	"Special Upgrade (60)"
}; 
const int numUpgradeNames = sizeof(upgradeNames)/sizeof(const char*);


const char* techNames[] = {
	"Stim Packs",
	"Lockdown",
	"EMP Shockwave",
	"Spider Mines",
	"Scanner Sweep",
	"Tank Siege Mode",
	"Defensive Matrix",
	"Irradiate",
	"Yamato Gun",
	"Cloaking Field",
	"Personnel Cloaking",
	"Burrowing",
	"Infestation",
	"Spawn Broodlings",
	"Dark Swarm",
	"Plague",
	"Consume",
	"Ensnare",
	"Parasite",
	"Psionic Storm",
	"Hallucination",
	"Recall",
	"Stasis Field",
	"Archon Warp",
	"Restoration",
	"Disruption Web",
	"Unused Tech (26)",
	"Mind Control",
	"Dark Archon Meld",
	"Feedback",
	"Optical Flare",
	"Maelstrom",
	"Lurker Aspect",
	"Unused Tech (33)",
	"Healing",
	"Unused Tech (35)",
	"Unused Tech (36)",
	"Unused Tech (37)",
	"Unused Tech (38)",
	"Unused Tech (39)",
	"Unused Tech (40)",
	"Unused Tech (41)",
	"Unused Tech (42)",
	"Unused Tech (43)",
};
const int numTechNames = sizeof(techNames)/sizeof(const char*);


const char* playerOwners[] = { "Unused", "Rescuable", "Computer", "Human", "Neutral" };
const int numPlayerOwners = sizeof(playerOwners)/sizeof(const char*);


const char* playerRaces[] = { "Zerg", "Terran", "Protoss", "Independent", "Neutral", "User Select", "Inactive", "Human" };
const int numPlayerRaces = sizeof(playerRaces)/sizeof(const char*);


const char* playerColors[] = { "Red", "Blue", "Teal", "Purple", "Orange", "Brown", "White", "Yellow", "Green", "Pale Yellow",
							   "Tan", "Dark Aqua", "Pale Green", "Blueish Gray", "Pale Yellow", "Cyan" };
const int numPlayerColors = sizeof(playerColors)/sizeof(const char*);

const COLORREF stringColors[] = { RGB(184, 184, 232), RGB(184, 184, 232), RGB(220, 220,  60), RGB(255, 255, 255),
								  RGB(132, 116, 116), RGB(200,  24,  24), RGB( 16, 252,  24), RGB(244,   4,   4),
								  RGB( 16, 252,  24), RGB( 16, 252,  24), RGB( 12,  72, 204), RGB( 44, 180, 148),
								  RGB(132,  64, 156), RGB(248, 140,  20), RGB( 16, 252,  24), RGB( 16, 252,  24),
								  RGB( 16, 252,  24), RGB(112,  48,  20), RGB(204, 224, 208), RGB(252, 252,  56),
								  RGB(  8, 128,   8), RGB(252, 252, 124), RGB(184, 184, 232), RGB(236, 196, 176),
								  RGB( 64, 104, 212), RGB(116, 164, 124), RGB(144, 144, 184), RGB(  0, 228, 252) };
const char* stringColorStrings[] = { " - Use Default", " - Cyan", " - Yellow", " - White", " - Grey", " - Red", " - Green", " - Red (p1)",
									 " - Invisible", " - Remove beyond", " - Blue (p2)", " - Teal (p3)", " - Purple (p4)", " - Orange (p5)",
									 " - Right Align", " - Center Align", " - Invisible", " - Brown (p6)", " - White (p7)", " - Yellow (p8)",
									 " - Green (p9)", " - Bright Yellow (p10)", " - Cyan", " - Pinkish (p11)", " - Dark Cyan (p12)",
									 " - Greygreen", " - Bluegrey", " - Turquoise" };
const char* stringColorPrefixes[] = { "<01><1>", "<02><2>", "<03><3>", "<04><4>", "<05><5>", "<06><6>", "<07><7>", "<08><8>",
									  "<0B>", "<0C>", "<0E>", "<0F>", "<10>", "<11>", "<12><R>", "<13><C>", "<14>", "<15>",
									  "<16>", "<17>", "<18>", "<19>", "<1A>", "<1B>", "<1C>", "<1D>", "<1E>", "<1F>" };

const char* tilesetNames[] = { "Badlands", "Space Platform", "Installation", "Ash World", "Jungle World", "Desert World", "Ice World", "Twilight World" };

const char* badlandsInitTerrain[] = { "Dirt", "Mud", "High Dirt", "Water", "Grass", "High Grass", "Structure", "Asphalt", "Rocky Ground", "Null Terrain" };
const char* spaceInitTerrain[] = { "Space", "Low Platform", "Rusty Pit", "Platform", "Dark Platform", "Plating", "Solar Array", "High Platform", "High Plating", "Elevated Catwalk", "Null Terrain" };
const char* installInitTerrain[] = { "Substructure", "Floor", "Roof", "Substructure Plating", "Plating", "Substructure Panels", "Bottomless Pit", "Null Terrain" };
const char* ashInitTerrain[] = { "Magma", "Dirt", "Lava", "Shale", "Broken Rock", "High Dirt", "High Lava", "High Shale", "Null Terrain" };
const char* jungInitTerrain[] = { "Water", "Dirt", "Mud", "Jungle", "Rocky Ground", "Ruins", "Raised Jungle", "Temple", "High Dirt", "High Jungle", "High Ruins", "High Raised Jungle", "High Temple", "Null Terrain" };
const char* desertInitTerrain[] = { "Tar", "Dirt", "Dried Mud", "Sand Dunes", "Rocky Ground", "Crags", "Sandy Sunken Pit", "Compound", "High Dirt", "High Sand Dunes", "High Crags", "High Sandy Sunken Pit", "High Compound", "Null Terrain" };
const char* iceInitTerrain[] = { "Ice", "Snow", "Moguls", "Dirt", "Rocky Snow", "Grass", "Water", "Outpost", "High Snow", "High Dirt", "High Grass", "High Water", "High Outpost", "Null Terrain" };
const char* twilightInitTerrain[] = { "Water", "Dirt", "Mud", "Crushed Rock", "Crevices", "Flagstones", "Sunken Ground", "Basilica", "High Dirt", "High Crushed Rock", "High Flagstones", "High Sunken Ground", "High Basilica", "Null Terrain" };
const char** initTerrains[] = { badlandsInitTerrain, spaceInitTerrain, installInitTerrain, ashInitTerrain, jungInitTerrain, desertInitTerrain, iceInitTerrain, twilightInitTerrain };
const int numBadlandsInitTerrain = 10;
const int numSpaceInitTerrain = 11;
const int numInstallationInitTerrain = 8;
const int numAshInitTerrain = 9;
const int numJungInitTerrain = 14;
const int numDesertInitTerrain = 14;
const int numIceInitTerrain = 14;
const int numTwilightInitTerrain = 14;

const int numTilesetInitTerrains[] = { numBadlandsInitTerrain, numSpaceInitTerrain, numInstallationInitTerrain,
									   numAshInitTerrain, numJungInitTerrain, numDesertInitTerrain,
									   numIceInitTerrain, numTwilightInitTerrain };
