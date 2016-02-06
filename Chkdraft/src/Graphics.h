#ifndef GRAPHICS_H
#define GRAPHICS_H
#include "CommonFiles/CommonFiles.h"
#include "Clipboard.h"
#include "ScData.h"
#include <string>
#include <vector>
#include <array>
#include "Defines.h" // I don't know where these go

/* Default Draw Order:
	- Final Tile Data (what MTXM will be)
	- Units & Sprites, highest YC to lowest YC
	  in the case of a tie, the highest index
	  is drawn first. */

typedef std::vector<u32> ChkdBitmap;

struct colorcycle
{
	char enabled; // This record is used
	char steps; // How many frames to pause between cycling
	char timer; // Frame counter
	short start; // WPE index to cycle from
	short stop; // WPE index to cycle to
};

class ColorCycler
{
	public:
		static bool CycleColors(const u16 tileset); // Returns true if the map should be redrawn

	private:
		static DWORD ccticks; // GetTickCount -- updated every time GetTickCount increases (~16 ms)
		static colorcycle cctable[4][8]; // Different color cycling definition tables
		static const u8 cctilesets[8]; // Table to use for each tileset
};

// So things don't get mad about references to nonexistent items
class ImageNode;
class SpriteNode;
class UnitNode;
class Graphics;

union ColoringData
	// Special parameters for certain render functions
{
	PCX* effect;
	u8 selectColor;
	u8 playerColor;
	struct
	{
		u8 state;
		u8 timer;
	} warpFlash, cloak;
	u16 raw16; // Direct access to the low word, for writing
	u32 raw32; // Direct access to the whole value

	ColoringData() {} // Doesn't do anything, except make it not be mad about the next one
	ColoringData(u32 a) : raw32(a) {} // Initialize with a value, for passing as direct a parameter value
};

// Prototypes for function lists
typedef void (Graphics::*UpdateFunction)(ImageNode* image);
typedef void (Graphics::*RenderFunction)(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP* grp, u16 frame, RECT* grpRect, ColoringData colorData);

class ImageNode
	// Single image to be drawn. References Images.dat, owned by SpriteNode.
	// Most of this copy+pasted from BWAPI
{
public:
	ImageNode() { memset(this, 0, sizeof(ImageNode)); } // Prevents breaking

	ImageNode* prev;
	ImageNode* next;
	u16 imageID;
	u8 paletteType; // DRAW_* defines
	u8 direction;
	u16 flags; /* Flag values -- should be defined somewhere
			   0x0001 - Redraw
			   0x0002 - Flipped
			   0x0004 - Don't calc mapPosition.y? Unused?
			   0x0008 - Graphics Turns
			   0x0010 - Use full iscript
			   0x0020 - Clickable
			   0x0040 - Hidden/Invisible
			   0x0080 - Update image position offset ? (LO related? Update LO? position?)
			   */
	s8 horizontalOffset;
	s8 verticalOffset;
	u16 iscriptHeader;
	u16 iscriptOffset;
	u16 iscriptReturn; // Used in call and return opcodes
	u8 anim; // ANIM_* defines
	u8 sleep;
	u16 frameSet; // Frameset set base on turning graphics
	u16 frameIndex; // FrameSet + direction
	points mapPosition;
	points screenPosition;
	rect grpBounds; // Rect showing screen clipping.
	GRP* GRPFile;
	ColoringData coloringData; // Values used for certain paletteTypes, passed to renderFunction
	RenderFunction renderFunction;
	UpdateFunction updateFunction;
	SpriteNode* spriteOwner;
};

class SpriteNode
	// Collection of images. References Sprites.dat, owned by units or raw sprited (technically thingy's, but we're not using those)
	// Most of this copy+pasted from BWAPI
{
public:
	SpriteNode() { memset(this, 0, sizeof(SpriteNode)); } // Prevents breaking

	SpriteNode* prev;
	SpriteNode* next;
	u16 spriteID;
	u8 playerID;
	u8 selectionIndex; //0 <= selectionIndex <= 11. Index in the selection area at bottom of screen.
	u8 visibilityFlags;  // Player bits indicating the visibility for a player
	u8 elevationLevel;
	u8 flags;
	/*	0x01  Draw selection circle
	0x02  unk02 - Ally selection?
	0x04  unk04 - Ally selection?
	0x08  Draw HP Bar
	0x10
	0x20  Hidden
	0x40  Burrowed
	0x80  Iscript unbreakable code section */
	u8 selectionTimer;
	u16 index;
	u8 unkflags_12;
	u8 unkflags_13;
	points position;
	ImageNode* pImagePrimary; // Main unit graphic, typically the body of the unit or building
	ImageNode* pImageHead; // Highest/topmost image
	ImageNode* pImageTail; // Lowest/bottommost image
};

class UnitNode
	// All the unit information you will never need! References Flingy.dat and Units.dat
	// Copy+pasted from BWAPI with slight modifications, and 99% absolutely unnecessary (and will be cut)
{
public:
	UnitNode() { memset(this, 0, sizeof(UnitNode)); }

	/*0x000*/ UnitNode*    prev;
	/*0x004*/ UnitNode*    next;      /**< Pointer to next unit in the unit linked list, we use
									  *   it to iterate units.
									  *   @see BW#BWXFN_UnitNodeTable_FirstElement
									  **/
	/*0x008*/ s32       hitPoints;  // Hit points of unit, note that the displayed value in broodwar is ceil(healthPoints/256)
									// Officially called "xLife"
	/*0x00C*/ SpriteNode*  sprite;
	// One of these Position structs are just called "swLocX" and "swLocY"
	/*0x010*/ points    moveTarget;             // The position or unit to move to. It is NOT an order target.
	/*0x014*/ UnitNode* moveTargetUnit;
	/*0x018*/ points  nextMovementWaypoint;   /**< The next way point in the path the unit is following to get to its destination.
											  * Equal to moveToPos for air units since they don't need to navigate around buildings
											  * or other units.
											  */
	/*0x01C*/ points      nextTargetWaypoint;   // The desired position
	/*0x020*/ u8            movementFlags;        // Flags specifying movement type - defined in BW#MovementFlags.
	/*0x021*/ u8            currentDirection1;    // The current direction the unit is facing
	/*0x022*/ u8            flingyTurnRadius;
	/*0x023*/ u8            velocityDirection1;  /**< This usually only differs from the currentDirection field for units that can accelerate
												 *   and travel in a different direction than they are facing. For example Mutalisks can change
												 *   the direction they are facing faster than then can change the direction they are moving.
												 */
	/*0x024*/ u16           flingyID;
	/*0x026*/ u8            _unknown_0x026;
	/*0x027*/ u8            flingyMovementType;
	/*0x028*/ points      position;         // Current position of the unit
	/*0x02C*/ POINT         halt;             // @todo Unknown    // Either this or current_speed is officially called "xDX, xDY" (no POINT struct)
	/*0x034*/ u32           flingyTopSpeed;
	/*0x038*/ s32           current_speed1;
	/*0x03C*/ s32           current_speed2;
	/*0x040*/ POINT         current_speed;

	/*0x048*/ u16           flingyAcceleration;
	/*0x04A*/ u8            currentDirection2;
	/*0x04B*/ u8            velocityDirection2;   // pathing related, gets this value from Path::unk_1A?
	/*0x04C*/ u8            playerID;             // Specification of owner of this unit.
	/*0x04D*/ u8            orderID;              // Specification of type of order currently given.
	/*0x04E*/ u8            orderState;  //< Additional order info (mostly unknown, wander property investigated so far)  // officially "ubActionState"
										 /*  0x01  Moving/Following Order
										 0x02  No collide (Larva)?
										 0x04  Harvesting? Working?
										 0x08  Constructing Stationary
										 Note: I don't actually think these are flags
										 */
	/*0x04F*/ u8            orderSignal;  /*  0x01  Update building graphic/state
										  0x02  Casting spell
										  0x04  Reset collision? Always enabled for hallucination...
										  0x10  Lift/Land state
										  */
	/*0x050*/ u16           orderUnitType;      // officially "uwFoggedTarget"

	u16 __0x52;  // 2-byte padding

	/*0x054*/ u8            mainOrderTimer;       // A timer for orders, example: time left before minerals are harvested
	/*0x055*/ u8            groundWeaponCooldown;
	/*0x056*/ u8            airWeaponCooldown;
	/*0x057*/ u8            spellCooldown;
	/*0x058*/ points        orderTarget;        // officially called ActionFocus
	/*0x05C*/ UnitNode*     orderTargetUnit;
	// ^-- POSSIBLE SHARED BULLET/UNIT STRUCTURE END

	/*0x060*/ u32         shieldPoints;      // BW shows this value/256, possibly not u32?
	/*0x064*/ u16         unitType;        // Specifies the type of unit.

	u16 __0x66;  // 2-byte padding

				 // CLink<CUnit> player_link;
	/*0x068*/ UnitNode*  previousPlayerUnit;
	/*0x06C*/ UnitNode*  nextPlayerUnit;

	/*0x070*/ UnitNode*  subUnit;

	// CLink<COrder> orderQueue_link;
	/*0x074*/ int*   orderQueueHead; // COrder
	/*0x078*/ int*   orderQueueTail; // COrder

	/*0x07C*/ UnitNode*  autoTargetUnit;      // The auto-acquired target (Note: This field is actually used for different targets internally, especially by the AI)
	/*0x080*/ UnitNode*  connectedUnit;       // Addon is connected to building (addon has conntected building, but not in other direction  (officially "pAttached")
	/*0x084*/ u8          orderQueueCount;      // @todo Verify   // officially "ubQueuedOrders"
	/*0x085*/ u8          orderQueueTimer;      // counts/cycles down from from 8 to 0 (inclusive). See also 0x122.
	/*0x086*/ u8          _unknown_0x086;       // pathing related?
	/*0x087*/ u8          attackNotifyTimer;    // Prevent "Your forces are under attack." on every attack
	/*0x088*/ u16         previousUnitType;     // Stores the type of the unit prior to being morphed/constructed
	/*0x08A*/ u8          lastEventTimer;       // countdown that stops being recent when it hits 0 
	/*0x08B*/ u8          lastEventColor;       // 17 = was completed (train, morph), 174 = was attacked
	/*0x08C*/ u16         _unused_0x08C;        // might have originally been RGB from lastEventColor
	/*0x08E*/ u8          rankIncrease;         // Adds this value to the unit's base rank
	/*0x08F*/ u8          killCount;            // Killcount
	/*0x090*/ u8          lastAttackingPlayer;  // the player that last attacked this unit
	/*0x091*/ u8          secondaryOrderTimer;
	/*0x092*/ u8          AIActionFlag;           // Internal use by AI only
	/*0x093*/ u8          userActionFlags;        // some flags that change when the user interacts with the unit
												  // 2 = issued an order, 3 = interrupted an order, 4 = self destructing

	/*0x094*/ u16         currentButtonSet;       // The u16 is a guess, used to be u8
	/*0x096*/ u8          isCloaked;
	/*0x097*/ u8          movementState;          // A value based on conditions related to pathing, see Path.h for info
	/*0x098*/ u16         buildQueue[5];          //< Queue of units to build. Note that it doesn't begin with index 0, but with #buildQueueSlot index. 
	/*0x0A2*/ u16         energy;                 //< Energy Points   // officially "xwMagic"
	/*0x0A4*/ u8          buildQueueSlot;         //< Index of active unit in #buildQueue. 
	/*0x0A5*/ u8          uniquenessIdentifier;   //< A byte used to determine uniqueness of the unit
	/*0x0A6*/ u8          secondaryOrderID;       //< (Build addon verified) @todo verify (Cloak, Build, ExpandCreep suggested by EUDDB) 
	/*0x0A7*/ u8          buildingOverlayState;   // 0 means the building has the largest amount of fire/blood
	/*0x0A8*/ u16         hpGain;                 //< hp gained on construction or repair
	/*0x0AA*/ u16         shieldGain;             //< Shield gain on construction
	/*0x0AC*/ u16         remainingBuildTime;     //< Remaining bulding time; This is also the timer for powerups (flags) to return to their original location.
	/*0x0AE*/ u16         previousHP;             // The HP of the unit before it changed (example Drone->Hatchery, the Drone's HP will be stored here)
	/*0x0B0*/ u16         loadedUnitIndex[8];     // officially called "uwTransport[8]"
												  ///////////////
	/*0x0C0*/ union
	{
		struct
		{
			u8 spiderMineCount; // 0
		} vulture;

		struct
		{
			UnitNode*  pInHanger;      // 0   first child inside the hanger    // official
			UnitNode*  pOutHanger;     // 4   first child outside the hanger
			u8      inHangerCount;  // 8   number inside the hanger
			u8      outHangerCount; // 9   number outside the hanger
		} carrier; // also applies to reaver (Carrier is official)

		struct
		{
			UnitNode*  parent;     // 0
								   // CLink<CUnit> hanger_link;
			UnitNode*  prev;       // 4
			UnitNode*  next;       // 8
			u8      inHanger;   // C
		} fighter;  // also applies to scarab

		struct
		{
			u32 _unknown_00;
			u32 _unknown_04;
			u32 flagSpawnFrame; // flag beacons, the frame that the flag will spawn
		} beacon;

		struct
		{
			UnitNode*  addon;                // 0
			u16     addonBuildType;       // 4
			u16     upgradeResearchTime;  // 6
			u8      techType;             // 8
			u8      upgradeType;          // 9
			u8      larvaTimer;           // A
			u8      landingTimer;         // B
			u8      creepTimer;           // C
			u8      upgradeLevel;         // D
			u16 __E;  // 2-byte padding
		} building;

		struct
		{
			UnitNode*  pPowerup;                 // 0    // official
			POINTS  targetResource;           // 4
			UnitNode*  targetResourceUnit;       // 8
			u16     repairResourceLossTimer;  // C
			u8      isCarryingSomething;      // E    // There is a "ubIsHarvesting" somewhere
			u8      resourceCarryCount;       // F
		} worker;   // Official name, but there is also a "CUnit::WorkerList::pHarvestBldg" somewhere
	};
	///////////////
	/*0x0D0*/ union
	{
		struct
		{
			u16     resourceCount;       // 0 // amount of resources
			u8      resourceIscript;     // 2
			u8      gatherQueueCount;    // 3
			UnitNode*  nextGatherer;        // 4  // pointer to the next workerunit waiting in line to gather
			u8      resourceGroup;       // 8
			u8      resourceBelongsToAI; // 9
		} resource;  // When the unit is resource container
		struct { UnitNode*   exit; } nydus; // connected nydius canal
		struct { SpriteNode* nukeDot; } ghost;
		struct { SpriteNode* pPowerTemplate; } pylon;
		struct
		{
			UnitNode* pNuke;     // attached nuke    // official name
			bool bReady;      // 4   // official name
		} silo;   // Should be "CUnit::Silo::"
		struct
		{
			rect harvestValue;
		} hatchery; // wtf???
		struct { POINTS origin; } powerup;
		struct
		{
			UnitNode* harvestTarget;           // 0
											   // CLINK<CUnit> harvest_link;
			UnitNode* prevHarvestUnit;         // 4   // When there is a gather conflict
			UnitNode* nextHarvestUnit;         // 8
		} gatherer; //there is also a "CUnit::WorkerList::pHarvestBldg" somewhere
	};
	/*0x0DC*/ u32       statusFlags;
	/*0x0E0*/ u8        resourceType;         // Resource being held by worker: 1 = gas, 2 = ore
	/*0x0E1*/ u8        wireframeRandomizer;
	/*0x0E2*/ u8        secondaryOrderState;
	/*0x0E3*/ u8        recentOrderTimer;     // Counts down from 15 to 0 when most orders are given,
											  // or when the unit moves after reaching a patrol location
	/*0x0E4*/ s32       visibilityStatus;     // Flags specifying which players can detect this unit (cloaked/burrowed)
	/*0x0E8*/ points  secondaryOrderPosition;       // unused
	/*0x0EC*/ UnitNode*    currentBuildUnit;    // tied to secondary order
												// CLink<CUnit> burrow_link;
	/*0x0F0*/ UnitNode*    previousBurrowedUnit;
	/*0x0F4*/ UnitNode*    nextBurrowedUnit;
	/*0x0F8*/ union
	{
		struct
		{
			POINTS  position;
			UnitNode*  unit;
		} rally;  // If the unit is rally type

		struct
		{ // CLink<CUnit> power_link;
			UnitNode*  prevPsiProvider;
			UnitNode*  nextPsiProvider;
		} PsiProvider;  // If the unit is psi provider
	};
	/*0x100*/ void* path;    // officially "unitPath"
	/*0x104*/ u8    pathingCollisionInterval; // unknown
	/*0x105*/ u8    pathingFlags;             // 0x01 = uses pathing; 0x02 = ?; 0x04 = ?
	/*0x106*/ u8    _unused_0x106;
	/*0x107*/ u8    isBeingHealed;    // 1 if a medic is currently healing this unit
	/*0x108*/ rect  contourBounds;    // a rect that specifies the closest contour (collision) points
	struct
	{
		/*0x110*/ u16         removeTimer;      //  does not apply to scanner sweep
		/*0x112*/ u16         defenseMatrixDamage;
		/*0x114*/ u8          defenseMatrixTimer;
		/*0x115*/ u8          stimTimer;
		/*0x116*/ u8          ensnareTimer;
		/*0x117*/ u8          lockdownTimer;
		/*0x118*/ u8          irradiateTimer;
		/*0x119*/ u8          stasisTimer;
		/*0x11A*/ u8          plagueTimer;
		/*0x11B*/ u8          stormTimer;
		/*0x11C*/ UnitNode*      irradiatedBy;
		/*0x120*/ u8          irradiatePlayerID;
		/*0x121*/ u8          parasiteFlags;    // bitmask identifying which players have parasited this unit
		/*0x122*/ u8          cycleCounter;     // counts/cycles up from 0 to 7 (inclusive). See also 0x85.
		/*0x123*/ u8          isBlind;
		/*0x124*/ u8          maelstromTimer;
		/*0x125*/ u8          _unused_0x125;    // ?? Might be afterburner timer or ultralisk roar timer
		/*0x126*/ u8          acidSporeCount;
		/*0x127*/ u8          acidSporeTime[9];
	} status;
	/*0x130*/ u16   bulletBehaviour3by3AttackSequence;  // Counts up for the number of bullets shot by a unit using
														// this weapon behaviour and resets after it reaches 12

	u16   _padding_0x132;   // 2-byte padding

	/*0x134*/ void* pAI;            // pointer to AI class, we're not using this though  // official name
	/*0x138*/ u16   airStrength;
	/*0x13A*/ u16   groundStrength;
	struct
	{             // Official names are "posSortXL, posSortXR, posSortYT, posSortYB"
		u32 left, right, top, bottom; // Ordering for unit boundries in unit finder for binary search
	} finder;
	/*0x14C*/ u8    _repulseUnknown;        // @todo Unknown
	/*0x14D*/ u8    repulseAngle;           // updated only when air unit is being pushed
	/*0x14E*/ u8    bRepMtxX;              //  (mapsizex/1.5 max)   // repulse matrix X/Y
	/*0x14F*/ u8    bRepMtxY;              //  (mapsizex/1.5 max)
};


class Graphics
{
	public:

		Graphics(Scenario &chk);

		void DrawMap( u16 bitWidth, u16 bitHeight,
					  s32 screenLeft, s32 screenTop,
					  ChkdBitmap& bitmap, SELECTIONS& selections,
					  u32 layer, HDC hDC, bool showAnywhere );

		void DrawTerrain(ChkdBitmap& bitmap);
		void DrawTileElevations(ChkdBitmap& bitmap);
		void DrawGrid(ChkdBitmap& bitmap);
		void DrawLocations(ChkdBitmap& bitmap, SELECTIONS& selections, bool showAnywhere);
		void DrawUnits(ChkdBitmap& bitmap, SELECTIONS& selections);
		void DrawSprites(ChkdBitmap& bitmap);
		void DrawLocationNames(HDC hDC);
		void DrawTileNumbers(HDC hDC);

		void AdjustSize(u32 newWidth, u32 newHeight); // Updates pane size and first and last sprite nodes
		void AdjustPosition(u32 newX, u32 newY); // Updates first and last sprite nodes

		void ToggleTileNumSource(bool MTXMoverTILE);
		bool mtxmOverTile();
		bool DisplayingTileNums();
		void ToggleLocationNameClip();
		bool ClippingLocationNames();

		void ToggleDisplayElevations();
		bool DisplayingElevations();

		GRID& grid(u32 gridNum);

		/** Forces all SpriteNodes to be updated upon next draw
			Should be called whenever changes are made to
			unit or sprite data */
		void InvalidateSprites();


		// Basic unit/sprite management functions -- Mostly just ideas and sample functionality
		bool addUnit(ChkUnit* unit); // Adds unit node and unit to graphic list
		bool insertUnit(u16 index, ChkUnit* unit); // Adds unit node and inserts in to graphic list
		void removeUnit(int index); // Deletes unit node and removes from graphic list
		bool recreateUnit(int index, ChkUnit* unit); // Recreates the unit at index in the graphic list, such as for when entirely changing a unit's type
		void updateUnit(int index, ChkUnit* unit); // Used for stat changes, etc.

		bool addSprite(ChkSprite* thg2);
		void removeSprite(int index);
		bool recreateSprite(int index, ChkSprite* thg2);
		void updateSprite(int index, ChkSprite* thg2); // Updates stat changes, etc.


		// Update and Render functions -- Public for the sole purpose of being able to populate the array that I couldn't figure out how to get to work within the class definition
		void updateNull(ImageNode* image) {} // Does nothing
		void updateCloaking(ImageNode* image);
		void updateDecloaking(ImageNode* image);
		void updateWarpFlash(ImageNode* image);

		// Render functions should be renamed to be more descriptive -- Referencing the imgRenderFxns table helps
		void imageRenderFxn0_0(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP *grp, u16 frame, RECT *grpRect, ColoringData colorData);
		void imageRenderFxn0_1(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP *grp, u16 frame, RECT *grpRect, ColoringData colorData);
		void imageRenderFxn2_0(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP *grp, u16 frame, RECT *grpRect, ColoringData colorData);
		void imageRenderFxn2_1(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP *grp, u16 frame, RECT *grpRect, ColoringData colorData);
		void imageRenderFxn3_0(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP *grp, u16 frame, RECT *grpRect, ColoringData colorData);
		void imageRenderFxn3_1(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP *grp, u16 frame, RECT *grpRect, ColoringData colorData);
		void imageRenderFxn5_0(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP *grp, u16 frame, RECT *grpRect, ColoringData colorData);
		void imageRenderFxn5_1(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP *grp, u16 frame, RECT *grpRect, ColoringData colorData);
		void imageRenderFxn6_0(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP *grp, u16 frame, RECT *grpRect, ColoringData colorData);
		void imageRenderFxn6_1(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP *grp, u16 frame, RECT *grpRect, ColoringData colorData);
		void imageRenderFxn8_0(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP *grp, u16 frame, RECT *grpRect, ColoringData colorData);
		void imageRenderFxn8_1(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP *grp, u16 frame, RECT *grpRect, ColoringData colorData);
		void imageRenderFxn9_0(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP *grp, u16 frame, RECT *grpRect, ColoringData colorData);
		void imageRenderFxn9_1(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP *grp, u16 frame, RECT *grpRect, ColoringData colorData);
		void imageRenderFxn10_0(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP *grp, u16 frame, RECT *grpRect, ColoringData colorData);
		void imageRenderFxn10_1(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP *grp, u16 frame, RECT *grpRect, ColoringData colorData);
		void imageRenderFxn11_0(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP *grp, u16 frame, RECT *grpRect, ColoringData colorData);
		void imageRenderFxn12_0(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP *grp, u16 frame, RECT *grpRect, ColoringData colorData);
		void imageRenderFxn12_1(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP *grp, u16 frame, RECT *grpRect, ColoringData colorData);
		void imageRenderFxn13_0(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP *grp, u16 frame, RECT *grpRect, ColoringData colorData);
		void imageRenderFxn14_0(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP *grp, u16 frame, RECT *grpRect, ColoringData colorData);
		void imageRenderFxn14_1(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP *grp, u16 frame, RECT *grpRect, ColoringData colorData);
		void imageRenderFxn15_0(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP *grp, u16 frame, RECT *grpRect, ColoringData colorData);
		void imageRenderFxn16_0(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP *grp, u16 frame, RECT *grpRect, ColoringData colorData);
		void imageRenderFxn16_1(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP *grp, u16 frame, RECT *grpRect, ColoringData colorData);
		void imageRenderFxn17_0(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP *grp, u16 frame, RECT *grpRect, ColoringData colorData);
		void imageRenderFxn17_1(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP *grp, u16 frame, RECT *grpRect, ColoringData colorData);



	private:

		Scenario &chk; // Pointer to the scenario file this instance of graphics renders

		bool isValid; // When false, requires complete sprite refresh
		bool isSorted; // When false, requires sorting before next use
		
		struct THG2Ref // Specifies whether map sprite is a unit or sprite
		{
			bool isUnit;
			union
			{
				SpriteNode* sprite; // Pure sprite
				UnitNode* unit; // Unit sprite
			};
		};

		bool initSprite(THG2Ref& thing, ChkSprite* thg2);
		std::vector<UnitNode*> UNITGraphics; // List of unit nodes, corresponding 1:1 (hopefully) to each UNIT entry
		std::vector<THG2Ref> THG2Graphics; // List of sprite or unit nodes, corresponding 1:! (hopefully) to each THG2 entry

		std::vector<std::shared_ptr<ImageNode>> imageNodes;
		std::vector<std::shared_ptr<SpriteNode>> spriteNodes;
		std::vector<std::shared_ptr<UnitNode>> unitNodes;
		ImageNode* imageEmptyNode; // First empty slot index -- New entities use this slot (Though keeping empty slots may not be totally necessary)
		SpriteNode* spriteEmptyNode;
		UnitNode* unitEmptyNode;
		ImageNode* imageLastCreated; // Index of the last created entity -- New entities added after this
		SpriteNode* spriteLastCreated;
		UnitNode* unitLastCreated;

		HWND hPane; // Handle to destination window
		s32 screenLeft; // X-Position of the screens left edge in the map
		s32 screenTop; // Y-Position of the screens top edge in the map
		u32 screenWidth; // Width of screen
		u32 screenHeight; // Height of screen
		u16 mapWidth; // In tiles
		u16 mapHeight; // In tiles

		GRID grids[2]; // The grids drawn over the map
		bool tileNumsFromMTXM; // When false, tile nums are from TILE
		bool displayingTileNums; // Determines whether tile numbers are showing
		bool displayingElevations; // Determines whether tile elevations are colored in
		bool clipLocationNames; // Determines whether the locationName can be drawn partly outside locations

		// Utility Methods...

		// Units/Sprites/Iscripts

		// SC Variables used -- we don't need all of them, and we have our own equivalents for some.
		u8 grpReindexing[256]; // Remapping table; tunit and tselect data are copied in to here, otherwise grpReindexing[i] == i
		// int unk_51C610[256];             // 51C610 - random counter by source? (size = 256?)
		// int unk_51CA10;                  // 51CA10 - Another rand seed?
		u32 randSeed = 0;                   // 51CA14 - rand seed
		// int unk_51CA18 = 0;              // 51CA18 - random counter?
		// grphead* grpArray[999];          // 51CED0 - grp_arr_51CED0 == chkd.scData.grps
		//  loHead* overlayArrays[5][999];   // 51F2A8 == chkd.scData.grps[chkd.scData.ImageDat(id)->_____Overlay]
		//  u8 unk_5240D0[999];              // 5240D0 - buildingOverlayState initial values?
		//  struct {
		//   u32 HumanID;
		//   u32 StormID;
		//   u8  Type;                    // chkd.maps.curr->getPlayerOwner(player, &owner)
		//   u8  Race;                    // chkd.maps.curr->getPlayerRace(player, &race)
		//   u8  Team;
		//   u8  Name[25];
		//  } ActivePlayers[12];             // 57EEE0
		//  CImage *unk_img_57EB68;          // 57EB68 - First Empty Image? == imageEmptyNode
		//  CImage *unk_img_57EB70;          // 57EB70 - Last Empty Image?
		u8 visionFilter;                 // 57F0B0
		//  u16 mapWidth;                    // 57F1D4 == mapWidth
		//  u16 mapHeight;                   // 57F1D6 == mapHeight
		//	int tileset;                     // 57F1DC == chkd.maps.curr->getTileset()
		//	char bwUpgradesR[12][15];        // 58F334
		//	int isExpansion;                 // 58F440 == chkd.maps.curr->isExpansion()
		//	char unk_59686D;                 // 59686D - ???
		//	char unk_596871;                 // 596871 - ???
		//	char unk_596877;                 // 596877 - ???
		//	u16 unk_59CCA0;                  // 59CCA0 - Error Code?
		//	int activeUnitCount;             // 6283F0
		//	CUnit* playerUnitList[? ];        // 6283F8
		//	UnitNode* firstUnit;             // 628430
		//	CUnit* unk_unit_628438;          // 628438 - Next Unused Unit? == unitEmptyNode
		//	int screenPosX;                  // 62848C == screenLeft
		//	int screenPosY;                  // 6284A8 == screenRight
		//	CSprite* unk_sprite_629288[256]; // 629288 - Some sprite ptr array? - Y-coord Sprite Group First
		//	CSprite* unk_sprite_629688[256]; // 629688 - Some sprite ptr array? - Y-coord Sprite Group Last
		//	CSprite* unk_sprite_63FE30;      // 63FE30 - First Empty Sprite? == spriteEmptyNode
		//	CSprite* unk_sprite_63FE34;      // 63FE34 - Last Empty Sprite?
		//	CFlingy* unk_63FEC8;             // 63FEC8 - First Used Flingy? (Bullet?)
		//	CFlingy* unk_63FF34;             // 63FF34 - Last Used Flingy? (Bullet?)
		//	CFlingy* unk_63FF38;             // 63FF38 - Next Unused Flingy (Bullet?)
		//	CFlingy* unk_63FF3C;             // 63FF3C - Last Unused Flingy (Bullet?)
		//	int unk_640880;                  // 640880 - ???
		//	CThingy* unk_thingy_65291C;      // 65291C - Last Created Thingy?
		//	char unk_652920;                 // 652920 - display target marker
		//	CThingy* unk_thingy_654874;      // 654874 - Last Used Thingy?
		//	CThingy* unk_thingy_654878;      // 654878 - First Empty Thingy?
		//	CThingy* unk_thingy_65487C;      // 65487C - Last Empty Thingy?
		char unk_6CEFB5;                 // 6CEFB5 - ??? -- 1: refresh images, 2: refreshed ?
		//	int unk_6D0F14;                  // 6D0F14 - ???
		//	int unk_6D11C8;                  // 6D11C8 - Turn off RNG?
		UnitNode *unk_unit_6D11F4;       // 6D11F4 - iscript active thingy/entity?
		//	CBullet *unk_unit_6D11F8;        // 6D11F8 - iscript active bullet
		UnitNode *activeIscriptUnit;     // 6D11FC - iscript active unit
		//	u8* isbuf;                       // 6D1200 - iscript buffer ~~ chkd.scData.iScripts.iscriptBin
		//	int* unk_arr_6D1260;             // 6D1260
		u8 activePlayerColor;            // 6D5BD4 - tunit player color loaded
		//	char unk_6DEFF8;                 // 6DEFF8 - ???

		// These functions should be pruned and sorted
		// Some unused functions and commented out references exist for potential usefulness -- like creep, fog, etc. previewing
		UnitNode* CreateUnitXY(int player, u16 id, int x, int y);
		UnitNode* CreateUnit(int unit, int x, int y, int player);
		UnitNode* UnitConstructor(int unitid, int x, int y, int player, int randseed);
		bool spreadsCreep(u16 unitType, bool unk);
		void ApplyCreepAtLocationFromUnitType(int unitid, int x, int y);
		void updateUnitStatsFinishBuilding(UnitNode* unit);
		bool unknown_0049EC30(UnitNode* unit);
		void updateUnitStrengthAndApplyDefaultOrders(UnitNode* unit);
		void unknown_00462960(UnitNode* unit);
		void UnitDestructor(UnitNode* unit);
		void _UnitDestructor(UnitNode* unit);
		void UnitDestructor_SpecialCases(UnitNode* unit);
		bool unknown_004A0080(UnitNode* unit);
		bool IsUnitCloaking(UnitNode* unit);
		void DoCloak(UnitNode* unit, int sound);
		void updateSpriteFlagOnPalette(SpriteNode* sprite);
		void unknown_00498170(SpriteNode* sprite);
		void UpdateSpriteImagesForCloak(SpriteNode* sprite);
		void unknown_0049FD00(UnitNode* unit);
		void unknown_0049A7F0(UnitNode* unit);
		void unknown_0049E3F0(UnitNode* unit);
		void bldgLiftoff(UnitNode* unit);
		void GiveUnitToNeutral(UnitNode* unit);
		void GiveAllUnitPropertiesToPlayerAndMakeIdle(UnitNode* unit, u8 player);
		void getImageAttackFrame(point* out, SpriteNode* sprite, int goffs, u8 losrc);
		void setSpriteMainImgOffset(SpriteNode* sprite, int unk1, int unk2);
		bool unknown_004A0320(UnitNode* unit, int unitid, int x, int y, int player);
		bool unknown_0049ECF0(UnitNode* unit, int y, u16 unitid, int x, u8 player);
		void SetConstructionGraphic(UnitNode* unit, int unk1);
		void playImageIscript(ImageNode* image, char anim);
		void turnUnit(UnitNode* unit, int direction);
		void initUnitTrapDoodad(UnitNode* unit);
		void updateBuildingLandFog(UnitNode* unit, int x, int y);
		void refreshAllVisibleImagesAtScreenPosition(SpriteNode* sprite, u8 newFlags);
		void refreshUnitVision(UnitNode* unit);
		bool unknown_00496360(int flingyid, s16 x, s16 y, UnitNode* flingy, int player, u8 direction);
		void spriteToIscriptLoop(SpriteNode* sprite);
		void ReplaceSpriteOverlayImage(SpriteNode* sprite, int imageid, int direction);
		void unknown_00497FD0(SpriteNode* sprite, u8 unk1, u8 cloaked, int EBP5);
		void refreshAllVisibleSprites(SpriteNode* sprite);
		void iscript_OpcodeCases(ImageNode *image, u16 hdr, int unk_arg2, int unk_arg3);
		void setImageDirection(ImageNode* image, int direction);
		void refreshImageData(rect* rect);
		SpriteNode* createSprite(int id, int x, int y, char player);
		void ImageDestructor(ImageNode* image);
		ImageNode* CreateImageOverlay(SpriteNode* sprite, int imageid, int x, int y, int direction);
		void ISCRIPT_PlayFrame(ImageNode* image, int frame);
		void ISCRIPT_setPosition(ImageNode* image, s8 x, s8 y);
		ImageNode* ISCRIPT_CreateImage(ImageNode* parent, int id, int horizontalOffset, int verticalOffset, int isOverlay);
		void updateImagePositionOffset(ImageNode* image);
		void ISCRIPT_UseLOFile(ImageNode* image, u32 losrc, u32 goffs, point* out);
		bool canUnitTypeFitAt(u16 unitID, s16 x, s16 y);
		bool unitIsGoliath(UnitNode* unit);
		SpriteNode* ISCRIPT_CreateSprite(ImageNode* image, int id, int x, int y, int elevation);
		void flipImage(ImageNode* image, u8 flip);
		void ISCRIPT_PlaySnd(ImageNode* image, u16 sound);
		void ISCRIPT_AttackMelee(UnitNode* unit);
		void setImagePaletteType(ImageNode* image, char paletteType);
		void updateImageFrameIndex(ImageNode* image);
		void showImage(ImageNode* image);
		void hideImage(ImageNode* image);
		void CBullet_Damage(UnitNode* unit);
		void _turnGraphic(UnitNode* unit, int direction);
		void unitSetRetreatPoint(UnitNode* unit, u8 direction);
		void unknown_004762C0(UnitNode* unit, int* unk_x, int* unk_y);
		bool unknown_0042A5C0(points* tgt_center, points* pos, rect* tgt_rect);
		int isDistanceGreaterThanHaltDistance(UnitNode* unit, int distance, int x, int y);
		int getDistanceFast(int x1, int x2, int y1, int y2);
		int unknown_00494BD0(int a, int b); // angleDistance
		int getDirectionFromPoints(int x1, int y1, int x2, int y2);
		int GetFastestDistance(int x);
		void ISCRIPT_AttackWith(UnitNode* unit, u8 arg);
		bool canCastSpell_0(UnitNode* unit);
		void ISCRIPT_UseWeapon(UnitNode* unit, u8 weapon);
		void IgnoreAllScriptAndGotoIdle(UnitNode* unit);
		void ISCRIPT_NoBrkCodeEnd(UnitNode* unit);
		bool initSpriteData(SpriteNode* sprite, short id, short x, short y, u8 player);
		void refreshImage(ImageNode* image);
		ImageNode* unknown_004D4E30(); // GetNextUnusedImage
		void InitializeImageData(ImageNode* image, SpriteNode* sprite, int imageID, int x, int y);
		void unknown_004D66B0(ImageNode* image, SpriteNode* sprite, int imageid);
		void updateImageDirection(ImageNode* image, int direction);
		bool unitIsOwnerByCurrentPlayer(UnitNode* unit);
		void CopyImagePaletteType(ImageNode* dst, ImageNode* src);
		SpriteNode* CreateThingy(int id, int x, int y, int unk); // CThingy
		bool unknown_004878F0(SpriteNode* thingy); // CFlingy / Thingy
		void playIdleIscript(SpriteNode* sprite);
		void orders_Nothing2(UnitNode* unit);
		void makeSpriteVisible(SpriteNode* sprite);
		void ApplyUnitEffects(UnitNode* node);
		void UpdateUnitSpriteInfo(UnitNode* unit);
		void updateUnitOrderFinder(/* ? */);
		void toggleUnitPath(UnitNode* unit);
		void removeSelectionCircleAndHPBar(SpriteNode* sprite);
		void removeSelectionCircle(SpriteNode* sprite);
		u8 removeSelectionCircleFromSprite(SpriteNode* sprite);
		void ISCRIPT_CastSpell(UnitNode* unit, u8 weapon);
		void CImage__updateGraphicData(ImageNode* image);
		void setAllOverlayDirectionsGeneric(SpriteNode* thingy, int direction); // CThingy
		void image_Insert(ImageNode* image, ImageNode* newImage, ImageNode** pImageHead);
		void somePlayImageCrapThatCrashes(ImageNode* image, SpriteNode* sprite, int x, int y, int imageid);
		void editUnitFlags(UnitNode* unit, ChkUnit* chkUnit);
		void setResourceCount(UnitNode* unit);
		void CHK_UNIT_ApplyBurrowFlag(UnitNode* unit, int validFlags, int applyFlags);
		void AI_CloakUnit(UnitNode* unit);
		void CHK_UNIT_ApplyOtherFlags(UnitNode* unit, int validFlags, int applyFlags);
		void CHK_UNIT_FinalCreateStep(UnitNode* unit);
		void thg2SpecialDIsableUnit(UnitNode* unit);
		void SpriteDestructor(SpriteNode* image);
		void setSpriteColoringData(SpriteNode* sprite, u8 colordata, bool hallucination);
		void updateAndDrawThingy(ChkdBitmap& bitmap, SpriteNode* sprite);
		void drawSprite(ChkdBitmap& bitmap, SpriteNode* sprite);
		void updateTrans50PlayerColors(u8 paletteType, u8 playerID);
		void drawImage(ChkdBitmap& bitmap, ImageNode* image);
		u16 getRand();
};

BITMAPINFO GetBMI(s32 width, s32 height);

void GrpToBits(ChkdBitmap& bitmap, u16 &bitWidth, u16 &bitHeight, s32 &xStart, s32 &yStart, GRP* grp,
				u16 grpXC, u16 grpYC, u16 frame, buffer* palette, u8 color, bool flipped );

void UnitToBits(ChkdBitmap& bitmap, buffer* palette, u8 color, u16 bitWidth, u16 bitHeight,
				 s32 &xStart, s32 &yStart, u16 unitID, u16 unitXC, u16 unitYC, u16 frame, bool selected );

void SpriteToBits(ChkdBitmap& bitmap, buffer* palette, u8 color, u16 bitWidth, u16 bitHeight,
				   s32 &xStart, s32 &yStart, u16 spriteID, u16 spriteXC, u16 spriteYC );

void TileToBits(ChkdBitmap& bitmap, TileSet* tiles, s32 xStart, s32 yStart, u16 width, u16 height, u16 &TileValue);

void DrawMiniTileElevation(HDC hDC, TileSet* tiles, s16 xOffset, s16 yOffset, u16 tileValue, u8 miniTileX, u8 miniTileY, BITMAPINFO &bmi);

void DrawTileElevation(HDC hDC, TileSet* tiles, s16 xOffset, s16 yOffset, u16 tileValue, BITMAPINFO &bmi);

void TileElevationsToBits(ChkdBitmap& bitmap, u32 &bitWidth, u32 &bitHeight, TileSet* tiles,
						   s16 xOffset, s16 yOffset, u16 &TileValue, BITMAPINFO &bmi, u8 miniTileSeparation );

void DrawTile( HDC hDC, TileSet* tiles, s16 xOffset, s16 yOffset, u16 &TileValue,
			   BITMAPINFO &bmi, u8 blueOffset, u8 greenOffset, u8 redOffset );

void DrawTileNumbers( HDC hDC, bool tileNumsFromMTXM, u32 screenLeft, u32 screenTop,
					  u16 xSize, u16 ySize, u16 bitHeight, u16 bitWidth, Scenario &chk );

void DrawTools( HDC hDC, HBITMAP tempBitmap, u16 width, u16 height, u32 screenLeft, u32 screenTop,
				SELECTIONS& selections, bool pasting, CLIPBOARD& clipboard, Scenario &chk, u8 layer );

void DrawTileSel(HDC hDC, u16 width, u16 height, u32 screenLeft, u32 screenTop, SELECTIONS& selections, Scenario &chk);

void DrawPasteGraphics( HDC hDC, HBITMAP tempBitmap, u16 width, u16 height, u32 screenLeft, u32 screenTop, SELECTIONS& selections,
					    CLIPBOARD& clipboard, Scenario &chk, u8 layer);

void DrawTempLocs(HDC hDC, u32 screenLeft, u32 screenTop, SELECTIONS& selections, Scenario &chk);

void DrawSelectingFrame(HDC hDC, SELECTIONS& selections, u32 screenLeft, u32 screenTop, s32 width, s32 height, double scale);

void DrawLocationFrame(HDC hDC, s32 left, s32 top, s32 right, s32 bottom);

void DrawMiniMap(HDC hDC, HWND hWnd, u16 xSize, u16 ySize, float scale, Scenario &chk);

void DrawMiniMapBox(HDC hDC, u32 screenLeft, u32 screenTop, u16 screenWidth, u16 screenHeight, u16 xSize, u16 ySize, float scale);

bool GetLineDrawSize(HDC hDC, SIZE* strSize, std::string str);

void DrawStringChunk(HDC hDC, UINT xPos, UINT yPos, std::string str);

void DrawStringLine(HDC hDC, UINT xPos, UINT yPos, LONG width, COLORREF defaultColor, std::string str);

bool GetStringDrawSize(HDC hDC, UINT &width, UINT &height, std::string str);

void DrawString(HDC hDC, UINT xPos, UINT yPos, LONG width, COLORREF defaultColor, std::string str);

#define MAX_UNIT_LEFT 128
#define MAX_UNIT_RIGHT 127
#define MAX_UNIT_UP 80
#define MAX_UNIT_DOWN 79

#endif