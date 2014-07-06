#ifndef DATA_H
#define DATA_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"

struct TileSet
{
	buffer cv5;
	buffer vf4;
	buffer vr4;
	buffer vx4;
	buffer wpe;
};

class Tiles
{
	public:
		TileSet set[8];
		bool LoadSets(MPQHANDLE &hStarDat, MPQHANDLE &hBrooDat, MPQHANDLE &hPatchRt);

	private:
		bool LoadSet(MPQHANDLE &hStarDat, MPQHANDLE &hBrooDat, MPQHANDLE &hPatchRt, const char* name, u8 num);
};

struct UNITDAT
{
	u8 Graphics;
	u16 Subunit1;
	u16 Subunit2;
	u16 Infestation; // ID 106-201 only
	u32 ConstructionAnimation;
	u8 UnitDirection;
	u8 ShieldEnable;
	u16 ShieldAmount;
	u32 HitPoints;
	u8 ElevationLevel;
	u8 Unknown;
	u8 Sublabel;
	u8 CompAIIdle;
	u8 HumanAIIdle;
	u8 ReturntoIdle;
	u8 AttackUnit;
	u8 AttackMove;
	u8 GroundWeapon;
	u8 MaxGroundHits;
	u8 AirWeapon;
	u8 MaxAirHits;
	u8 AIInternal;
	u32 SpecialAbilityFlags;
	u8 TargetAcquisitionRange;
	u8 SightRange;
	u8 ArmorUpgrade;
	u8 UnitSize;
	u8 Armor;
	u8 RightClickAction;
	u16 ReadySound; // ID 0-105 only
	u16 WhatSoundStart;
	u16 WhatSoundEnd;
	u16 PissSoundStart; // ID 0-105 only
	u16 PissSoundEnd; // ID 0-105 only
	u16 YesSoundStart; // ID 0-105 only
	u16 YesSoundEnd; // ID 0-105 only
	u16 StarEditPlacementBoxWidth;
	u16 StarEditPlacementBoxHeight;
	u16 AddonHorizontal; // ID 106-201 only
	u16 AddonVertical; // ID 106-201 only
	u16 UnitSizeLeft;
	u16 UnitSizeUp;
	u16 UnitSizeRight;
	u16 UnitSizeDown;
	u16 Portrait;
	u16 MineralCost;
	u16 VespeneCost;
	u16 BuildTime;
	u16 Unknown1;
	u8 StarEditGroupFlags;
	u8 SupplyProvided;
	u8 SupplyRequired;
	u8 SpaceRequired;
	u8 SpaceProvided;
	u16 BuildScore;
	u16 DestroyScore;
	u16 UnitMapString;
	u8 BroodwarUnitFlag;
	u16 StarEditAvailabilityFlags;
};

struct FLINGYDAT
{
	u16 Sprite;
	u32 TopSpeed;
	u16 Acceleration;
	u32 HaltDistance;
	u8 TurnRadius;
	u8 Unused;
	u8 MoveControl;
};

struct SPRITEDAT
{
	u16 ImageFile;
	u8 HealthBar;
	u8 Unknown;
	u8 IsVisible;
	u8 SelectionCircleImage;
	u8 SelectionCircleOffset;
};

struct IMAGEDAT
{
	u32 GRPFile;
	u8 GraphicTurns;
	u8 Clickable;
	u8 UseFullIscript;
	u8 DrawIfCloaked;
	u8 DrawFunction;
	u8 Remapping;
	u32 IscriptID;
	u32 ShieldOverlay;
	u32 AttackOverlay;
	u32 DamageOverlay;
	u32 SpecialOverlay;
	u32 LandingDustOverlay;
	u32 LiftOffOverlay;
};

struct FRAME
{
	u8 xOffset;
	u8 yOffset;
	u8 frameWidth;
	u8 frameHeight;
};

class GRP
{
	private:
		buffer imageDat;

	public:
		bool LoadGrp(const char* fileName, MPQHANDLE &hStarDat, MPQHANDLE &hBrooDat, MPQHANDLE &hPatchRt);
		u16 numFrames() { return imageDat.get<u16>(0); }
		u16 GrpWidth() { return imageDat.get<u16>(2); }
		u16 GrpHeight() { return imageDat.get<u16>(4); }
		u8 xOffset(u32 frame) { return imageDat.get<u8>(0x6+frame*8); }
		u8 yOffset(u32 frame) { return imageDat.get<u8>(0x7+frame*8); }
		u8 frameWidth(u32 frame) { return imageDat.get<u8>(0x8+frame*8); }
		u8 frameHeight(u32 frame) { return imageDat.get<u8>(0x9+frame*8); }
		u8* data(u32 frame, u32 line);
};

class Units
{
	public:
		Units();
		bool LoadUnits(MPQHANDLE hStarDat, MPQHANDLE hBrooDat, MPQHANDLE hPatchRt);
		UNITDAT* UnitDat(u16 id);

	private:
		UNITDAT unit[228];
};

class Sprites
{
	public:
		Sprites();
		bool LoadSprites(MPQHANDLE hStarDat, MPQHANDLE hBrooDat, MPQHANDLE hPatchRt);
		FLINGYDAT* FlingyDat(u32 id);
		SPRITEDAT* SpriteDat(u32 id);
		IMAGEDAT* ImageDat(u32 id);

	private:
		FLINGYDAT flingy[209];
		SPRITEDAT sprite[517];
		IMAGEDAT image[999];
		buffer iscriptBin;
};

#define PCX_MANUFACTURER 0x00
#define PCX_VER_INFO 0x01
#define PCX_ENCODING 0x02
#define PCX_BIT_COUNT 0x03
#define PCX_LEFT_MARGIN 0x04
#define PCX_UPPER_MARGIN 0x06
#define PCX_RIGHT_MARGIN 0x08
#define PCX_LOWER_MARGIN 0x0A
#define PCX_HOZ_DPI 0x0C
#define PCX_VERT_DPI 0x0E
#define PCX_PALETTE 0x10
#define PCX_RESERVED 0x40
#define PCX_NCP 0x41
#define PCX_NBS 0x42
#define PCX_PAL_INFO 0x44
#define PCX_HOZ_SCREEN_SIZE 0x46
#define PCX_VERT_SCREEN_SIZE 0x48
#define PCX_RESERVED2 0x4A
#define PCX_DATA 0x80

class PCX
{
	public:
		buffer pcxDat;
		bool load(MPQHANDLE &hStarDat, MPQHANDLE &hBrooDat, MPQHANDLE &hPatchRt, const char* filename);
};

class DATA
{
	public:
		Tiles tilesets;
		Units units;
		Sprites sprites;
		PCX tminimap;
		PCX tunit;
		PCX tselect;
		GRP* grps;
		u16 numGrps;
		const char* UnitDisplayName[233];
		void UpdateDisplayNames(const char** nameList);

		UNITDAT*   UnitDat	(u16 id) { return units.UnitDat	   (id); }
		FLINGYDAT* FlingyDat(u32 id) { return sprites.FlingyDat(id); }
		SPRITEDAT* SpriteDat(u32 id) { return sprites.SpriteDat(id); }
		IMAGEDAT*  ImageDat	(u32 id) { return sprites.ImageDat (id); }

		void Load();

	private:
		bool LoadGrps(MPQHANDLE &hStarDat, MPQHANDLE &hBrooDat, MPQHANDLE &hPatchRt);
		bool tilesLoaded;
		bool unitsLoaded;
};


bool GetCV5sReferences(TileSet* tiles, u32 &cv5Reference, u16 &TileValue);

void GetMegaTileRef(TileSet* tiles, u32 &MegaTileReference, u32 &cv5Reference);

void GetMiniTileRef(TileSet* tiles, u32 &MiniTileReference, u32 &MegaTileReference, u8 &xMiniTile, u8 &yMiniTile);

#endif