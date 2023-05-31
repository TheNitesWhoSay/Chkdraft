#ifndef SC_H
#define SC_H
#include "Basics.h"
#include "MpqFile.h"
#include "SystemIO.h"
#include "FileBrowser.h"
#include <array>

/**
    The Sc files defines static structures, constants, and enumerations general to StarCraft, there may be some limited overlap with Chk

    This file also provides resources to find the StarCraft directory and load assets from the StarCraft data files
*/

namespace Sc {

    /**
        A data file typically from the StarCraft directory
        e.g. StarDat.mpq, BrooDat.mpq, patch_rt.mpq
    */
    class DataFile {
    public:
        enum class Priority : u32 {
            MaximumPriority = 0,
            PatchRt = 100,
            BrooDat = 200,
            StarDat = 300,
            MinimumPriority = u32_max
        };
        
        static const std::string starCraftFileName;
        static const std::string starDatFileName;
        static const std::string brooDatFileName;
        static const std::string patchRtFileName;
        
        static std::vector<FilterEntry<u32>> getStarDatFilter();
        static std::vector<FilterEntry<u32>> getBrooDatFilter();
        static std::vector<FilterEntry<u32>> getPatchRtFilter();
        static std::vector<FilterEntry<u32>> getStarCraftExeFilter();
        
        class Descriptor // Describes a data files priority in relation to other data files, the file name and path, whether it's expected in StarCraft's directory, and what file browser to use
        {
        public:
            Descriptor(Priority priority, const std::string & fileName, const std::string & expectedFilePath = "", FileBrowserPtr<u32> browser = nullptr, bool expectedInScDirectory = true);

            Priority getPriority() const;
            const std::string & getFileName() const;
            const std::string & getExpectedFilePath() const;
            FileBrowserPtr<u32> getBrowser() const;
            bool isExpectedInScDirectory() const;

            void setExpectedFilePath(const std::string & expectedFilePath);

        private:
            Priority priority;
            std::string fileName;
            std::string expectedFilePath;
            FileBrowserPtr<u32> browser;
            bool expectedInScDirectory;
        };
        
        static std::unordered_map<Priority, Descriptor> getDefaultDataFiles();
        
        class Browser // An extensible, system independent browser for retrieving a set of StarCraft data files given their descriptors and possibly a specialized browser for the StarCraft directory
        {
        public:
            virtual std::vector<MpqFilePtr> openScDataFiles(
                const std::unordered_map<Priority, Descriptor> & dataFiles = getDefaultDataFiles(),
                const std::string & expectedStarCraftDirectory = ::getDefaultScPath(),
                FileBrowserPtr<u32> starCraftBrowser = getDefaultStarCraftBrowser());

            virtual bool findStarCraftDirectory(std::string & starCraftDirectory, bool & declinedBrowse, const std::string & expectedStarCraftDirectory = "", FileBrowserPtr<u32> starCraftBrowser = nullptr);

            virtual MpqFilePtr openDataFile(const std::string & dataFilePath, const Descriptor & dataFileDescriptor);

            static FileBrowserPtr<u32> getDefaultStarCraftBrowser();
        };
        using BrowserPtr = std::shared_ptr<Browser>;
    };

    class Player {
    public:
        static constexpr size_t Total = 12;
        static constexpr size_t TotalSlots = 8;
        static constexpr size_t TotalOwners = 27;
        enum_t(Id, size_t, { // size_t
            Player1 = 0,
            Player2 = 1,
            Player3 = 2,
            Player4 = 3,
            Player5 = 4,
            Player6 = 5,
            Player7 = 6,
            Player8 = 7,
            Player9 = 8,
            Player10 = 9,
            Player11 = 10,
            Player12_Neutral = 11,
            None = 12,
            CurrentPlayer = 13,
            Foes = 14,
            Allies = 15,
            NeutralPlayers = 16,
            AllPlayers = 17,
            Force1 = 18,
            Force2 = 19,
            Force3 = 20,
            Force4 = 21,
            Unused1 = 22,
            Unused2 = 23,
            Unused3 = 24,
            Unused4 = 25,
            NonAlliedVictoryPlayers = 26
        });
        enum_t(SlotType, u8, { // u8
            Inactive = 0,
            GameComputer = 1,
            GameHuman = 2,
            RescuePassive = 3,
            Unused = 4,
            Computer = 5,
            Human = 6,
            GameOpen = 6,
            Neutral = 7,
            GameClosed = 8
        });
    };

    class Unit {
    public:
        static constexpr size_t TotalTypes = 228; // Number of real units; excludes Id228, AnyUnit, Men, Buildings, and Factories
        static constexpr size_t TotalReferenceTypes = 233; // Number of types referenced in triggers; includes all real units as well as Id228, AnyUnit, Men, Buildings, and Factories
        static constexpr size_t TotalFlingies = 209; // Number of flingy entries
        enum_t(Type, u16, {
            TerranMarine = 0,
            TerranGhost = 1,
            TerranVulture = 2,
            TerranGoliath = 3,
            GoliathTurret = 4,
            TerranSiegeTank_TankMode = 5,
            SiegeTankTurret_TankMode = 6,
            TerranScv = 7,
            TerranWraith = 8,
            TerranScienceVessel = 9,
            GuiMontag_Firebat = 10,
            TerranDropship = 11,
            TerranBattlecruiser = 12,
            SpiderMine = 13,
            NuclearMissile = 14,
            TerranCivilian = 15,
            SarahKerrigan_Ghost = 16,
            AlanSchezar_Goliath = 17,
            AlanTurret = 18,
            JimRaynor_Vulture = 19,
            JimRaynor_Marine = 20,
            TomKazansky_Wraith = 21,
            Magellan_ScienceVessel = 22,
            EdmundDuke_SiegeTank = 23,
            DukeTurretType1 = 24,
            EdmundDuke_SiegeMode = 25,
            DukeTurretType2 = 26,
            ArcturusMengsk_Battlecruiser = 27,
            Hyperion_Battlecruiser = 28,
            NoradII_Battlecruiser = 29,
            TerranSiegeTank_SiegeMode = 30,
            SiegeTankTurret_SiegeMode = 31,
            TerranFirebat = 32,
            ScannerSweep = 33,
            TerranMedic = 34,
            ZergLarva = 35,
            ZergEgg = 36,
            ZergZergling = 37,
            ZergHydralisk = 38,
            ZergUltralisk = 39,
            ZergBroodling = 40,
            ZergDrone = 41,
            ZergOverlord = 42,
            ZergMutalisk = 43,
            ZergGuardian = 44,
            ZergQueen = 45,
            ZergDefiler = 46,
            ZergScourge = 47,
            Torrasque_Ultralisk = 48,
            Matriarch_Queen = 49,
            InfestedTerran = 50,
            InfestedKerrigan_InfestedTerran = 51,
            UncleanOne_Defiler = 52,
            HunterKiller_Hydralisk = 53,
            DevouringOne_Zergling = 54,
            Kukulza_Mutalisk = 55,
            Kukulza_Guardian = 56,
            Yggdrasill_Overlord = 57,
            TerranValkrie = 58,
            Cocoon = 59,
            ProtossCorsair = 60,
            ProtossDarkTemplar = 61,
            ZergDevourer = 62,
            ProtossDarkArchon = 63,
            ProtossProbe = 64,
            ProtossZealot = 65,
            ProtossDragoon = 66,
            ProtossHighTemplar = 67,
            ProtossArchon = 68,
            ProtossShuttle = 69,
            ProtossScout = 70,
            ProtossArbiter = 71,
            ProtossCarrier = 72,
            ProtossInterceptor = 73,
            DarkTemplar_Hero = 74,
            Zeratul_DarkTemplar = 75,
            TassadarZeratul_Archon = 76,
            Fenix_Zealot = 77,
            Fenix_Dragoon = 78,
            Tassadar_Templar = 79,
            Mojo_Scout = 80,
            Warbringer_Reaver = 81,
            Gantrithor_Carrier = 82,
            ProtossReaver = 83,
            ProtossObserver = 84,
            ProtossScarab = 85,
            Danimoth_Arbiter = 86,
            Aldaris_Templar = 87,
            Artanis_Scout = 88,
            Rhynadon_BadlandsCritter = 89,
            Bengalaas_Jungle = 90,
            CargoShip_Unused = 91,
            MercenaryGunship_Unused = 92,
            Scantid_DesertCritter = 93,
            Kakaru_TwilightCritter = 94,
            Ragnasaur_AshworldCritter = 95,
            Ursadon_IceWorldCritter = 96,
            LurkerEgg = 97,
            Raszagal_Corsair = 98,
            SamirDuran_Ghost = 99,
            AlexeiStukov_Ghost = 100,
            MapRevealer = 101,
            GerardDuGalle_BattleCruiser = 102,
            ZergLurker = 103,
            InfestedDuran = 104,
            DisruptionField = 105,
            TerranCommandCenter = 106,
            TerranComsatStation = 107,
            TerranNuclearSilo = 108,
            TerranSupplyDepot = 109,
            TerranRefinery = 110,
            TerranBarracks = 111,
            TerranAcademy = 112,
            TerranFactory = 113,
            TerranStarport = 114,
            TerranControlTower = 115,
            TerranScienceFacility = 116,
            TerranCovertOps = 117,
            TerranPhysicsLab = 118,
            Starbase_Unused = 119,
            TerranMachineShop = 120,
            RepairBay_Unused = 121,
            TerranEngineeringBay = 122,
            TerranArmory = 123,
            TerranMissileTurret = 124,
            TerranBunker = 125,
            NoradII_Crashed = 126,
            IonCannon = 127,
            UrajCrystal = 128,
            KhalisCrystal = 129,
            InfestedCommandCenter = 130,
            ZergHatchery = 131,
            ZergLair = 132,
            ZergHive = 133,
            ZergNydusCanal = 134,
            ZergHydraliskDen = 135,
            ZergDefilerMound = 136,
            ZergGreaterSpire = 137,
            ZergQueensNest = 138,
            ZergEvolutionChamber = 139,
            ZergUltraliskCavern = 140,
            ZergSpire = 141,
            ZergSpawningPool = 142,
            ZergCreepColony = 143,
            ZergSporeColony = 144,
            UnusedZergBuilding1 = 145,
            ZergSunkenColony = 146,
            ZergOvermind_WithShell = 147,
            ZergOvermind = 148,
            ZergExtractor = 149,
            MatureCrysalis = 150,
            ZergCerebrate = 151,
            ZergCerebrateDaggoth = 152,
            UnusedZergBuilding2 = 153,
            ProtossNexus = 154,
            ProtossRoboticsFacility = 155,
            ProtossPylon = 156,
            ProtossAssimilator = 157,
            UnusedProtossBuilding1 = 158,
            ProtossObservatory = 159,
            ProtossGateway = 160,
            UnusedProtossBuilding2 = 161,
            ProtossPhotonCannon = 162,
            ProtossCitadelOfAdum = 163,
            ProtossCyberneticsCore = 164,
            ProtossTemplarArchives = 165,
            ProtossForge = 166,
            ProtossStargate = 167,
            StasisCellPrison = 168,
            ProtossFleetBeacon = 169,
            ProtossArbiterTribunal = 170,
            ProtossRoboticsSupportBay = 171,
            ProtossShieldBattery = 172,
            KhaydarinCrystalFormation = 173,
            ProtossTemple = 174,
            XelNagaTemple = 175,
            MineralFieldType1 = 176,
            MineralFieldType2 = 177,
            MineralFieldType3 = 178,
            Cave = 179,
            CaveIn = 180,
            Cantina = 181,
            MiningPlatform_Unused = 182,
            IndependentCommandCenter_Unused = 183,
            IndependentStarport_Unused = 184,
            IndependentJumpGate_Unused = 185,
            Ruins_Unused = 186,
            KhadarinCrystalFormation_Unused = 187,
            VespeneGeyser = 188,
            WarpGate = 189,
            PsiDistrupter = 190,
            ZergMarker = 191,
            TerranMarker = 192,
            ProtossMarker = 193,
            ZergBeacon = 194,
            TerranBeacon = 195,
            ProtossBeacon = 196,
            ZergFlagBeacon = 197,
            TerranFlagBeacon = 198,
            ProtossFlagBeacon = 199,
            PowerGenerator = 200,
            OvermindCocoon = 201,
            DarkSwarm = 202,
            FloorMissileTrap = 203,
            FloorHatch_Unused = 204,
            LeftUpperLevelDoor = 205,
            RightUpperLevelDoor = 206,
            LeftPitDoor = 207,
            RightPitDoor = 208,
            FloorGunTrap = 209,
            LeftWallMissileTrap = 210,
            LeftWallFlameTrap = 211,
            RightWallMissileTrap = 212,
            RightWallFlameTrap = 213,
            StartLocation = 214,
            Flag = 215,
            YoungChrysalis = 216,
            PsiEmitter = 217,
            DataDisc = 218,
            KhaydarinCrystal = 219,
            MineralClusterType1 = 220,
            MineralClusterType2 = 221,
            ProtossVespeneGasOrbType1 = 222,
            ProtossVespeneGasOrbType2 = 223,
            ZergVespeneGasSacType1 = 224,
            ZergVespeneGasSacType2 = 225,
            TerranVespeneGasTankType1 = 226,
            TerranVespeneGasTankType2 = 227,
            Id228 = 228,
            AnyUnit = 229,
            Men = 230,
            Buildings = 231,
            Factories = 232,

            NoSubUnit = 228,
            NoUnit = u16_max
        });
        static constexpr size_t MaxCuwps = 64;

        static const std::vector<std::string> defaultDisplayNames;
        static const std::vector<std::string> legacyTextTrigDisplayNames;

        struct DatEntry
        {
            u8 graphics;
            Sc::Unit::Type subunit1;
            Sc::Unit::Type subunit2;
            u16 infestation; // Id 106-201 only
            u32 constructionAnimation;
            u8 unitDirection;
            u8 shieldEnable;
            u16 shieldAmount;
            u32 hitPoints;
            u8 elevationLevel;
            u8 unknown;
            u8 sublabel;
            u8 compAIIdle;
            u8 humanAIIdle;
            u8 returntoIdle;
            u8 attackUnit;
            u8 attackMove;
            u8 groundWeapon;
            u8 maxGroundHits;
            u8 airWeapon;
            u8 maxAirHits;
            u8 aiInternal;
            u32 specialAbilityFlags;
            u8 targetAcquisitionRange;
            u8 sightRange;
            u8 armorUpgrade;
            u8 unitSize;
            u8 armor;
            u8 rightClickAction;
            u16 readySound; // Id 0-105 only
            u16 whatSoundStart;
            u16 WhatSoundEnd;
            u16 pissSoundStart; // Id 0-105 only
            u16 pissSoundEnd; // Id 0-105 only
            u16 yesSoundStart; // Id 0-105 only
            u16 yesSoundEnd; // Id 0-105 only
            u16 starEditPlacementBoxWidth;
            u16 starEditPlacementBoxHeight;
            u16 addonHorizontal; // Id 106-201 only
            u16 addonVertical; // Id 106-201 only
            u16 unitSizeLeft;
            u16 unitSizeUp;
            u16 unitSizeRight;
            u16 unitSizeDown;
            u16 portrait;
            u16 mineralCost;
            u16 vespeneCost;
            u16 buildTime;
            u16 unknown1;
            u8 starEditGroupFlags;
            u8 supplyProvided;
            u8 supplyRequired;
            u8 spaceRequired;
            u8 spaceProvided;
            u16 buildScore;
            u16 destroyScore;
            u16 unitMapString;
            u8 broodWarUnitFlag;
            u16 starEditAvailabilityFlags;
        };
        struct FlingyDatEntry
        {
            u16 sprite;
            u32 topSpeed;
            u16 acceleration;
            u32 haltDistance;
            u8 turnRadius;
            u8 unused;
            u8 moveControl;
        };
        
#pragma pack(push, 1)
        __declspec(align(1)) struct DatFile
        {
            enum_t(IdRange, size_t, {
                From0To105 = 106,
                From106To201 = 96
            });

            u8 graphics[TotalTypes];
            Sc::Unit::Type subunit1[TotalTypes];
            Sc::Unit::Type subunit2[TotalTypes];
            u16 infestation[IdRange::From106To201]; // Id 106-201 only
            u32 constructionAnimation[TotalTypes];
            u8 unitDirection[TotalTypes];
            u8 shieldEnable[TotalTypes];
            u16 shieldAmount[TotalTypes];
            u32 hitPoints[TotalTypes];
            u8 elevationLevel[TotalTypes];
            u8 unknown[TotalTypes];
            u8 sublabel[TotalTypes];
            u8 compAIIdle[TotalTypes];
            u8 humanAIIdle[TotalTypes];
            u8 returntoIdle[TotalTypes];
            u8 attackUnit[TotalTypes];
            u8 attackMove[TotalTypes];
            u8 groundWeapon[TotalTypes];
            u8 maxGroundHits[TotalTypes];
            u8 airWeapon[TotalTypes];
            u8 maxAirHits[TotalTypes];
            u8 aiInternal[TotalTypes];
            u32 specialAbilityFlags[TotalTypes];
            u8 targetAcquisitionRange[TotalTypes];
            u8 sightRange[TotalTypes];
            u8 armorUpgrade[TotalTypes];
            u8 unitSize[TotalTypes];
            u8 armor[TotalTypes];
            u8 rightClickAction[TotalTypes];
            u16 readySound[IdRange::From0To105]; // Id 0-105 only
            u16 whatSoundStart[TotalTypes];
            u16 whatSoundEnd[TotalTypes];
            u16 pissSoundStart[IdRange::From0To105]; // Id 0-105 only
            u16 pissSoundEnd[IdRange::From0To105]; // Id 0-105 only
            u16 yesSoundStart[IdRange::From0To105]; // Id 0-105 only
            u16 yesSoundEnd[IdRange::From0To105]; // Id 0-105 only
            u16 starEditPlacementBoxWidth[TotalTypes];
            u16 starEditPlacementBoxHeight[TotalTypes];
            u16 addonHorizontal[IdRange::From106To201]; // Id 106-201 only
            u16 addonVertical[IdRange::From106To201]; // Id 106-201 only
            u16 unitSizeLeft[TotalTypes];
            u16 unitSizeUp[TotalTypes];
            u16 unitSizeRight[TotalTypes];
            u16 unitSizeDown[TotalTypes];
            u16 portrait[TotalTypes];
            u16 mineralCost[TotalTypes];
            u16 vespeneCost[TotalTypes];
            u16 buildTime[TotalTypes];
            u16 unknown1[TotalTypes];
            u8 starEditGroupFlags[TotalTypes];
            u8 supplyProvided[TotalTypes];
            u8 supplyRequired[TotalTypes];
            u8 spaceRequired[TotalTypes];
            u8 spaceProvided[TotalTypes];
            u16 buildScore[TotalTypes];
            u16 destroyScore[TotalTypes];
            u16 unitMapString[TotalTypes];
            u8 broodWarUnitFlag[TotalTypes];
            u16 starEditAvailabilityFlags[TotalTypes];
        };
        __declspec(align(1)) struct FlingyDatFile
        {
            u16 sprite[TotalFlingies];
            u32 topSpeed[TotalFlingies];
            u16 acceleration[TotalFlingies];
            u32 haltDistance[TotalFlingies];
            u8 turnRadius[TotalFlingies];
            u8 unused[TotalFlingies];
            u8 moveControl[TotalFlingies];
        };
#pragma pack(pop)

        bool load(const std::vector<MpqFilePtr> & orderedSourceFiles);
        const DatEntry & getUnit(Type unitType) const;
        const FlingyDatEntry & getFlingy(size_t flingyIndex) const;

    private:
        std::vector<DatEntry> units;
        std::vector<FlingyDatEntry> flingies;
    };

    class Sprite
    {
    public:
        enum_t(Type, u16, {
            // TODO: Aftering loading code is working, index all sprites using imageFile + grpFile + 
        });
        static constexpr size_t TotalSprites = 517;
        static constexpr size_t TotalImages = 999;
        struct DatEntry
        {
            u16 imageFile;
            u8 healthBar; // Id 130-517 only
            u8 unknown;
            u8 isVisible;
            u8 selectionCircleImage; // Id 130-517 only
            u8 selectionCircleOffset; // Id 130-517 only
        };
        struct ImageDatEntry
        {
            u32 grpFile;
            u8 graphicTurns;
            u8 clickable;
            u8 useFullIScript;
            u8 drawIfCloaked;
            u8 drawFunction;
            u8 remapping;
            u32 iScriptId;
            u32 shieldOverlay;
            u32 attackOverlay;
            u32 damageOverlay;
            u32 specialOverlay;
            u32 landingDustOverlay;
            u32 liftOffOverlay;
        };
        
#pragma pack(push, 1)
        __declspec(align(1)) struct DatFile
        {
            enum_t(IdRange, size_t, {
                From0To129 = 130,
                From130To516 = 387
            });
            
            u16 imageFile[TotalSprites]; // images.dat index
            u8 healthBar[IdRange::From130To516]; // Id 130-516 only
            u8 unknown[TotalSprites];
            u8 isVisible[TotalSprites];
            u8 selectionCircleImage[IdRange::From130To516]; // Id 130-516 only
            u8 selectionCircleOffset[IdRange::From130To516]; // Id 130-516 only
        };
        __declspec(align(1)) struct ImageDatFile
        {
            u32 grpFile[TotalImages]; // image.tbl index
            u8 graphicTurns[TotalImages];
            u8 clickable[TotalImages];
            u8 useFullIscript[TotalImages];
            u8 drawIfCloaked[TotalImages];
            u8 drawFunction[TotalImages];
            u8 remapping[TotalImages];
            u32 iScriptId[TotalImages];
            u32 shieldOverlay[TotalImages];
            u32 attackOverlay[TotalImages];
            u32 damageOverlay[TotalImages];
            u32 specialOverlay[TotalImages];
            u32 landingDustOverlay[TotalImages];
            u32 liftOffOverlay[TotalImages];
        };
        
        __declspec(align(1)) struct PixelLine {
            enum_t(Header, u8, {
                // If (header & IsTransparentLine): place horizontal transparent line of (header & TransparentLineLength) pixels in length
                IsTransparentLine = BIT_7,
                TransparentLineLength = x8BIT_7,

                // Else if (header & IsSolidLine): place horizontal line of one solid color of (header & SolidLineLength) pixels in length
                IsSolidLine = BIT_6,
                SolidLineLength = x8BIT_7 & x8BIT_6,

                // Else: place horizontal line of speckled colors of (header) pixels in length
                SpeckledLineLength = x8BIT_7 & x8BIT_6,
                
                IsTransparentOrSolidLine = IsTransparentLine | IsSolidLine
            });
                
            /** Units are drawn using three sets of colors:
                1.) Color at indexes 0-7 on the palette, sometimes remapped to tselect for selection circles, or using shift for hallucintations
                2.) Color at indexes 8-15 on the palette, usually remapped to tunit to apply player-based colors, or using shift for hallucinations
                3.) Color at indexes 16-255 on the palette, sometimes remapped using shift for hallucinations
                        
                Note that for palette (which is initially loaded from tileset\tilesetName.wpe)...
                color values 248-254 will rotate rightwards every eight ticks (every change in GetTickCount(), ~16ms) on badlands, jungle, desert, iceworld, and twilight */

            u8 header; // See the Header enum
            u8 paletteIndex[1]; // For transparent lines this array is unused, for solid lines this is paletteIndex[1], for speckled lines this is paletteIndex[header]
            
            inline bool isTransparentLine() const { return (header & Header::IsTransparentLine) == Header::IsTransparentLine; }
            inline bool isSolidLine() const { return (header & Header::IsTransparentOrSolidLine) == Header::IsSolidLine; }
            inline bool isSpeckled() const { return (header & Header::IsTransparentOrSolidLine) == 0; }
            inline u8 transparentLineLength() const { return header & Header::TransparentLineLength; }
            inline u8 solidLineLength() const { return header & Header::SolidLineLength; }
            inline u8 speckledLineLength() const { return header; }
            inline u8 lineLength() const { return isTransparentLine() ? transparentLineLength() : (isSolidLine() ? solidLineLength() : speckledLineLength()); }
            inline u8 sizeInBytes() const { return isTransparentLine() ? 1 : (isSolidLine() ? 2 : 1+speckledLineLength()); }

            constexpr static size_t MinimumSize = 1;
        }; // 1 <= Size <= 64
        __declspec(align(1)) struct PixelRow {
            PixelLine adjacentHorizontalLines[1]; // One pixel row is made up of an indeterminate number of adjacent horizontal lines, these lines continue until you've placed the pixel at frameWidth-1

            inline u8 totalLines(u8 frameWidth, u16 maxRowLength) const {
                u8 totalLines = 0;
                u32 offsetWithinRow = 0;
                const PixelLine* pixelLine = &adjacentHorizontalLines[0];
                for ( u16 x=0; x<frameWidth && offsetWithinRow < (u32)maxRowLength; totalLines++ )
                {
                    offsetWithinRow += pixelLine->sizeInBytes();
                    pixelLine = (const PixelLine*)((u8*)adjacentHorizontalLines)[offsetWithinRow];
                }
                return totalLines;
            }

            constexpr static size_t MinimumSize = 1;
        }; // Size >= 1
        __declspec(align(1)) struct GrpFrame {
            u16 rowOffsets[1]; // u16 rowOffsets[frameHeight] // One frame is made up of frameHeight pixel rows
            // PixelRow rows[frameHeight] // One frame is made up of frameHeight pixel rows, the size of a PixelRow may vary
        }; // Size >= 3*frameHeight
        __declspec(align(1)) struct GrpFrameHeader {
            u8 xOffset;
            u8 yOffset;
            u8 frameWidth;
            u8 frameHeight;
            u32 frameOffset;
        };
        __declspec(align(1)) struct GrpFile {
            u16 numFrames;
            u16 grpWidth;
            u16 grpHeight;
            GrpFrameHeader frameHeaders[1]; // GrpFrameHeader frameHeaders[numFrames]
            // GrpFrame frames[numFrames]; // The size of a GrpFrame may vary

            static constexpr size_t FileHeaderSize = sizeof(numFrames)+sizeof(grpWidth)+sizeof(grpHeight);
        };
#pragma pack(pop)

        class Grp
        {
        public:
            virtual ~Grp() {}
            bool load(const std::vector<MpqFilePtr> & orderedSourceFiles, const std::string & mpqFileName);
            void makeBlank();
            const GrpFile & get() const;

        private:
            std::vector<u8> grpData;
            
            inline bool isValid(const std::string & mpqFileName) const;
            inline bool fileHeaderIsValid(const std::string & mpqFileName) const;
            inline bool frameHeadersAreValid(const std::string & mpqFileName) const;
            inline bool framesAreValid(const std::string & mpqFileName) const;
        };

        bool load(const std::vector<MpqFilePtr> & orderedSourceFiles);
        const Grp & getGrp(size_t grpIndex);
        const ImageDatEntry & getImage(size_t imageIndex) const;
        const DatEntry & getSprite(size_t spriteIndex) const;
        size_t numGrps() const;
        size_t numImages() const;
        size_t numSprites() const;

    private:
        std::vector<Grp> grps;
        std::vector<ImageDatEntry> images;
        std::vector<DatEntry> sprites;
    };

    class Upgrade {
    public:
        static constexpr size_t TotalOriginalTypes = 46;
        static constexpr size_t TotalTypes = 61;
        enum_t(Type, u32, { // u32
            TerranInfantryArmor = 0,
            TerranVehiclePlating = 1,
            TerranShipPlating = 2,
            ZergCarapace = 3,
            ZergFlyerCarapace = 4,
            ProtossArmor = 5,
            ProtossPlating = 6,
            TerranInfantryWeapons = 7,
            TerranVehicleWeapons = 8,
            TerranShipWeapons = 9,
            ZergMeleeAttacks = 10,
            ZergMissileAttacks = 11,
            ZergFlyerAttacks = 12,
            ProtossGroundWeapons = 13,
            ProtossAirWeapons = 14,
            ProtossPlasmaShields = 15,
            U238Shells = 16,
            IonThrusters = 17,
            BurstLasers_Unused = 18,
            TitanReactor = 19,
            OcularImplants = 20,
            MoebiusReactor = 21,
            ApolloReactor = 22,
            ColossusReactor = 23,
            VentralSacs = 24,
            Antennae = 25,
            PneumatizedCarapace = 26,
            MetabolicBoost = 27,
            AdrenalGlands = 28,
            MuscularAugments = 29,
            GroovedSpines = 30,
            GameteMeiosis = 31,
            MetasynapticNode = 32,
            SingularityCharge = 33,
            LegEnhancements = 34,
            ScarabDamage = 35,
            ReaverCapacity = 36,
            GraviticDrive = 37,
            SensorArray = 38,
            GraviticBoosters = 39,
            KhaydarinAmulet = 40,
            ApialSensors = 41,
            GraviticThrusters = 42,
            CarrierCapacity = 43,
            KhaydarinCore = 44,
            UnusedUpgrade_45 = 45,
            UnusedUpgrade_46 = 46,
            ArgusJewel = 47,
            UnusedUpgrade_48 = 48,
            ArgusTalisman = 49,
            UnusedUpgrade_50 = 50,
            CaduceusReactor = 51,
            ChitinousPlating = 52,
            AnabolicSynthesis = 53,
            CharonBooster = 54,
            UnusedUpgrade_55 = 55,
            UnusedUpgrade_56 = 56,
            UnusedUpgrade_57 = 57,
            UnusedUpgrade_58 = 58,
            UnusedUpgrade_59 = 59,
            SpecialUpgrade_60 = 60
        });

        static const std::vector<std::string> names;

        struct DatEntry
        {
            u16 mineralCost;
            u16 mineralFactor;
            u16 vespeneCost;
            u16 vespeneFactor;
            u16 timeCost;
            u16 timeFactor;
            u16 unknown;
            u16 icon;
            u16 label;
            u8 race;
            u8 maxRepeats;
            u8 broodWarSpecific;
        };
        
#pragma pack(push, 1)
        __declspec(align(1)) struct OriginalDatFile
        {
            u16 mineralCost[TotalOriginalTypes];
            u16 mineralFactor[TotalOriginalTypes];
            u16 vespeneCost[TotalOriginalTypes];
            u16 vespeneFactor[TotalOriginalTypes];
            u16 timeCost[TotalOriginalTypes];
            u16 timeFactor[TotalOriginalTypes];
            u16 unknown[TotalOriginalTypes];
            u16 icon[TotalOriginalTypes];
            u16 label[TotalOriginalTypes];
            u8 race[TotalOriginalTypes];
            u8 maxRepeats[TotalOriginalTypes];
        };
        __declspec(align(1)) struct DatFile
        {
            u16 mineralCost[TotalTypes];
            u16 mineralFactor[TotalTypes];
            u16 vespeneCost[TotalTypes];
            u16 vespeneFactor[TotalTypes];
            u16 timeCost[TotalTypes];
            u16 timeFactor[TotalTypes];
            u16 unknown[TotalTypes];
            u16 icon[TotalTypes];
            u16 label[TotalTypes];
            u8 race[TotalTypes];
            u8 maxRepeats[TotalTypes];
            u8 broodWarSpecific[TotalTypes];
        };
#pragma pack(pop)

        bool load(const std::vector<MpqFilePtr> & orderedSourceFiles);
        const DatEntry & getUpgrade(Type upgradeType) const;

    private:
        std::vector<DatEntry> upgrades;
    };

    class Tech {
    public:
        static constexpr size_t TotalOriginalTypes = 24;
        static constexpr size_t TotalTypes = 44;
        enum_t(Type, u32, { // u32
            StimPacks = 0,
            Lockdown = 1,
            EMPShockwave = 2,
            SpiderMines = 3,
            ScannerSweep = 4,
            TankSiegeMode = 5,
            DefensiveMatrix = 6,
            Irradiate = 7,
            YamatoGun = 8,
            CloakingField = 9,
            PersonnelCloaking = 10,
            Burrowing = 11,
            Infestation = 12,
            SpawnBroodlings = 13,
            DarkSwarm = 14,
            Plague = 15,
            Consume = 16,
            Ensnare = 17,
            Parasite = 18,
            PsionicStorm = 19,
            Hallucination = 20,
            Recall = 21,
            StasisField = 22,
            ArchonWarp = 23,
            Restoration = 24,
            DisruptionWeb = 25,
            UnusedTech_26 = 26,
            MindControl = 27,
            DarkArchonMeld = 28,
            Feedback = 29,
            OpticalFlare = 30,
            Maelstrom = 31,
            LurkerAspect = 32,
            UnusedTech_33 = 33,
            Healing = 34,
            UnusedTech_35 = 35,
            UnusedTech_36 = 36,
            UnusedTech_37 = 37,
            UnusedTech_38 = 38,
            UnusedTech_39 = 39,
            UnusedTech_40 = 40,
            UnusedTech_41 = 41,
            UnusedTech_42 = 42,
            UnusedTech_43 = 43
        });

        static const std::vector<std::string> names;

        struct DatEntry
        {
            u16 mineralCost;
            u16 vespeneCost;
            u16 researchTime;
            u16 energyCost;
            u32 unknown;
            u16 icon;
            u16 label;
            u8 race;
            u8 unused;
            u8 broodWar;
        };
        
#pragma pack(push, 1)
        __declspec(align(1)) struct OriginalDatFile
        {
            u16 mineralCost[TotalOriginalTypes];
            u16 vespeneCost[TotalOriginalTypes];
            u16 researchTime[TotalOriginalTypes];
            u16 energyCost[TotalOriginalTypes];
            u32 unknown[TotalOriginalTypes];
            u16 icon[TotalOriginalTypes];
            u16 label[TotalOriginalTypes];
            u8 race[TotalOriginalTypes];
            u8 unused[TotalOriginalTypes];
        };
        __declspec(align(1)) struct DatFile
        {
            u16 mineralCost[TotalTypes];
            u16 vespeneCost[TotalTypes];
            u16 researchTime[TotalTypes];
            u16 energyCost[TotalTypes];
            u32 unknown[TotalTypes];
            u16 icon[TotalTypes];
            u16 label[TotalTypes];
            u8 race[TotalTypes];
            u8 unused[TotalTypes];
            u8 broodWar[TotalTypes];
        };
#pragma pack(pop)

        bool load(const std::vector<MpqFilePtr> & orderedSourceFiles);
        const DatEntry & getTech(Type techType) const;

    private:
        std::vector<DatEntry> techs;
    };

    class TblFile
    {
    public:
        bool load(const std::vector<MpqFilePtr> & orderedSourceFiles, const std::string & mpqFileName);
        size_t numStrings() const;
        const std::string & getString(size_t stringIndex) const;
        bool getString(size_t stringIndex, std::string & outString) const;

    private:
        std::vector<std::string> strings;
    };
    using TblFilePtr = std::shared_ptr<TblFile>;

    class Ai {
    public:
        const static std::string aiScriptBinPath;

        enum class ScriptId : u32 { // u32
            NoScript = 0,
            Ter3 = 863135060,
            Ter5 = 896689492,
            Te5H = 1211458900,
            Ter6 = 913466708,
            Te6b = 1647732052,
            Te6c = 1664509268,
            Ter7 = 930243924,
            Te7s = 1933010260,
            Te7m = 1832346964,
            Ter8 = 947021140,
            Tr9L = 1278833236,
            Tr9H = 1211724372,
            Te10 = 808543572,
            T11z = 2050044244,
            T11a = 1630613844,
            T11b = 1647391060,
            T12N = 1311912276,
            T12P = 1345466708,
            T12T = 1412575572,
            TED1 = 826557780,
            TED2 = 843334996,
            TED3 = 860112212,
            TSW1 = 827806548,
            TSW2 = 844583764,
            TSW3 = 861360980,
            TSW4 = 878138196,
            TSW5 = 894915412,
            Zer1 = 829580634,
            Zer2 = 846357850,
            Zer3 = 863135066,
            Zer4 = 879912282,
            Ze4S = 1395942746,
            Zer6 = 913466714,
            Zr7a = 1631023706,
            Zr7g = 1731687002,
            Zr7s = 1933013594,
            Zer8 = 947021146,
            Ze8T = 1412982106,
            Zer9 = 963798362,
            Z9ly = 2037135706,
            Z9ry = 2037528922,
            Z9lo = 1869363546,
            Z9ro = 1869756762,
            Z10a = 1630548314,
            Z10b = 1647325530,
            Z10c = 1664102746,
            Z10d = 1680879962,
            Z10e = 1697657178,
            Pro1 = 829387344,
            Pro2 = 846164560,
            Pr3R = 1379103312,
            Pr3G = 1194553936,
            Pro4 = 879718992,
            Pr5I = 1228239440,
            Pr5B = 1110798928,
            Pro7 = 930050640,
            Pr7B = 1110930000,
            Pr7S = 1396142672,
            Pro8 = 946827856,
            Pr8B = 1110995536,
            Pr8D = 1144549968,
            Pro9 = 963605072,
            Pr9W = 1463382608,
            Pr9Y = 1496937040,
            Pr10 = 808546896,
            P10C = 1127231824,
            P10o = 1865429328,
            PB1A = 1093747280,
            PB1B = 1110524496,
            PB1C = 1127301712,
            PB1D = 1144078928,
            PB1E = 1160856144,
            PB1F = 1177633360,
            PB2A = 1093812816,
            PB2B = 1110590032,
            PB2C = 1127367248,
            PB2D = 1144144464,
            PB2E = 1160921680,
            PB2F = 1177698896,
            PB3A = 1093878352,
            PB3B = 1110655568,
            PB3C = 1127432784,
            PB3D = 1144210000,
            PB3E = 1160987216,
            PB3F = 1177764432,
            PB4A = 1093943888,
            PB4B = 1110721104,
            PB4C = 1127498320,
            PB4D = 1144275536,
            PB4E = 1161052752,
            PB4F = 1177829968,
            PB5A = 1094009424,
            PB5B = 1110786640,
            PB5C = 1127563856,
            PB5D = 1144341072,
            PB5E = 1161118288,
            PB5F = 1177895504,
            PB6A = 1094074960,
            PB6B = 1110852176,
            PB6C = 1127629392,
            PB6D = 1144406608,
            PB6E = 1161183824,
            PB6F = 1177961040,
            PB7A = 1094140496,
            PB7B = 1110917712,
            PB7C = 1127694928,
            PB7D = 1144472144,
            PB7E = 1161249360,
            PB7F = 1178026576,
            PB8A = 1094206032,
            PB8B = 1110983248,
            PB8C = 1127760464,
            PB8D = 1144537680,
            PB8E = 1161314896,
            PB8F = 1178092112,
            TB1A = 1093747284,
            TB1B = 1110524500,
            TB1C = 1127301716,
            TB1D = 1144078932,
            TB1E = 1160856148,
            TB1F = 1177633364,
            TB2A = 1093812820,
            TB2B = 1110590036,
            TB2C = 1127367252,
            TB2D = 1144144468,
            TB2E = 1160921684,
            TB2F = 1177698900,
            TB3A = 1093878356,
            TB3B = 1110655572,
            TB3C = 1127432788,
            TB3D = 1144210004,
            TB3E = 1160987220,
            TB3F = 1177764436,
            TB4A = 1093943892,
            TB4B = 1110721108,
            TB4C = 1127498324,
            TB4D = 1144275540,
            TB4E = 1161052756,
            TB4F = 1177829972,
            TB5A = 1094009428,
            TB5B = 1110786644,
            TB5C = 1127563860,
            TB5D = 1144341076,
            TB5E = 1161118292,
            TB5F = 1177895508,
            TB6A = 1094074964,
            TB6B = 1110852180,
            TB6C = 1127629396,
            TB6D = 1144406612,
            TB6E = 1161183828,
            TB6F = 1177961044,
            TB7A = 1094140500,
            TB7B = 1110917716,
            TB7C = 1127694932,
            TB7D = 1144472148,
            TB7E = 1161249364,
            TB7F = 1178026580,
            TB8A = 1094206036,
            TB8B = 1110983252,
            TB8C = 1127760468,
            TB8D = 1144537684,
            TB8E = 1161314900,
            TB8F = 1178092116,
            ZB1A = 1093747290,
            ZB1B = 1110524506,
            ZB1C = 1127301722,
            ZB1D = 1144078938,
            ZB1E = 1160856154,
            ZB1F = 1177633370,
            ZB2A = 1093812826,
            ZB2B = 1110590042,
            ZB2C = 1127367258,
            ZB2D = 1144144474,
            ZB2E = 1160921690,
            ZB2F = 1177698906,
            ZB3A = 1093878362,
            ZB3B = 1110655578,
            ZB3C = 1127432794,
            ZB3D = 1144210010,
            ZB3E = 1160987226,
            ZB3F = 1177764442,
            ZB4A = 1093943898,
            ZB4B = 1110721114,
            ZB4C = 1127498330,
            ZB4D = 1144275546,
            ZB4E = 1161052762,
            ZB4F = 1177829978,
            ZB5A = 1094009434,
            ZB5B = 1110786650,
            ZB5C = 1127563866,
            ZB5D = 1144341082,
            ZB5E = 1161118298,
            ZB5F = 1177895514,
            ZB6A = 1094074970,
            ZB6B = 1110852186,
            ZB6C = 1127629402,
            ZB6D = 1144406618,
            ZB6E = 1161183834,
            ZB6F = 1177961050,
            ZB7A = 1094140506,
            ZB7B = 1110917722,
            ZB7C = 1127694938,
            ZB7D = 1144472154,
            ZB7E = 1161249370,
            ZB7F = 1178026586,
            ZB8A = 1094206042,
            ZB8B = 1110983258,
            ZB8C = 1127760474,
            ZB8D = 1144537690,
            ZB8E = 1161314906,
            ZB8F = 1178092122,
            ZB9A = 1094271578,
            ZB9B = 1111048794,
            ZB9C = 1127826010,
            ZB9D = 1144603226,
            ZB9E = 1161380442,
            ZB9F = 1178157658,
            ZB0A = 1093681754,
            ZB0B = 1110458970,
            ZB0C = 1127236186,
            ZB0D = 1144013402,
            ZB0E = 1160790618,
            ZB0F = 1177567834,
            TMCu = 1967344980,
            ZMCu = 1967344986,
            PMCu = 1967344976,
            TMCx = 2017676628,
            ZMCx = 2017676634,
            PMCx = 2017676624,
            TLOf = 1716472916,
            TMED = 1145392468,
            THIf = 1716078676,
            TSUP = 1347769172,
            TARE = 1163018580,
            ZLOf = 1716472922,
            ZMED = 1145392474,
            ZHIf = 1716078682,
            ZSUP = 1347769178,
            ZARE = 1163018586,
            PLOf = 1716472912,
            PMED = 1145392464,
            PHIf = 1716078672,
            PSUP = 1347769168,
            PARE = 1163018576,
            TLOx = 2018462804,
            TMEx = 2017807700,
            THIx = 2018068564,
            TSUx = 2018857812,
            TARx = 2018656596,
            ZLOx = 2018462810,
            ZMEx = 2017807706,
            ZHIx = 2018068570,
            ZSUx = 2018857818,
            ZARx = 2018656602,
            PLOx = 2018462800,
            PMEx = 2017807696,
            PHIx = 2018068560,
            PSUx = 2018857808,
            PARx = 2018656592,
            Suic = 1667855699,
            SuiR = 1382643027,
            Rscu = 1969451858,
            PlusVi0 = 812209707, /* +Vi0 */
            PlusVi1 = 828986923, /* +Vi1 */
            PlusVi2 = 845764139, /* +Vi2 */
            PlusVi3 = 862541355, /* +Vi3 */
            PlusVi4 = 879318571, /* +Vi4 */
            PlusVi5 = 896095787, /* +Vi5 */
            PlusVi6 = 912873003, /* +Vi6 */
            PlusVi7 = 929650219, /* +Vi7 */
            MinusVi0 = 812209709, /* -Vi0 */
            MinusVi1 = 828986925, /* -Vi1 */
            MinusVi2 = 845764141, /* -Vi2 */
            MinusVi3 = 862541357, /* -Vi3 */
            MinusVi4 = 879318573, /* -Vi4 */
            MinusVi5 = 896095789, /* -Vi5 */
            MinusVi6 = 912873005, /* -Vi6 */
            MinusVi7 = 929650221, /* -Vi7 */
            MvTe = 1700034125,
            ClrC = 1131572291,
            Enmy = 2037214789,
            Ally = 2037148737,
            VluA = 1098214486,
            EnBk = 1799515717,
            StTg = 1733588051,
            StPt = 1951429715,
            EnTr = 1918135877,
            ExTr = 1918138437,
            NuHe = 1699247438,
            HaHe = 1699242312,
            JYDg = 1732532554,
            DWHe = 1699239748,
            ReHe = 1699243346
        };

        enum class Script : u32 { // u32
            NoScript = (u32)ScriptId::NoScript,
            Terran3ZergTown = (u32)ScriptId::Ter3,
            Terran5TerranMainTown = (u32)ScriptId::Ter5,
            Terran5TerranHarvestTown = (u32)ScriptId::Te5H,
            Terran6AirAttackZerg = (u32)ScriptId::Ter6,
            Terran6GroundAttackZerg = (u32)ScriptId::Te6b,
            Terran6ZergSupportTown = (u32)ScriptId::Te6c,
            Terran7BottomZergTown = (u32)ScriptId::Ter7,
            Terran7RightZergTown = (u32)ScriptId::Te7s,
            Terran7MiddleZergTown = (u32)ScriptId::Te7m,
            Terran8ConfederateTown = (u32)ScriptId::Ter8,
            Terran9LightAttack = (u32)ScriptId::Tr9L,
            Terran9HeavyAttack = (u32)ScriptId::Tr9H,
            Terran10ConfederateTowns = (u32)ScriptId::Te10,
            Terran11ZergTown = (u32)ScriptId::T11z,
            Terran11LowerProtossTown = (u32)ScriptId::T11a,
            Terran11UpperProtossTown = (u32)ScriptId::T11b,
            Terran12NukeTown = (u32)ScriptId::T12N,
            Terran12PhoenixTown = (u32)ScriptId::T12P,
            Terran12TankTown = (u32)ScriptId::T12T,
            Terran1ElectronicDistribution = (u32)ScriptId::TED1,
            Terran2ElectronicDistribution = (u32)ScriptId::TED2,
            Terran3ElectronicDistribution = (u32)ScriptId::TED3,
            Terran1Shareware = (u32)ScriptId::TSW1,
            Terran2Shareware = (u32)ScriptId::TSW2,
            Terran3Shareware = (u32)ScriptId::TSW3,
            Terran4Shareware = (u32)ScriptId::TSW4,
            Terran5Shareware = (u32)ScriptId::TSW5,
            Zerg1TerranTown = (u32)ScriptId::Zer1,
            Zerg2ProtossTown = (u32)ScriptId::Zer2,
            Zerg3TerranTown = (u32)ScriptId::Zer3,
            Zerg4RightTerranTown = (u32)ScriptId::Zer4,
            Zerg4LowerTerranTown = (u32)ScriptId::Ze4S,
            Zerg6ProtossTown = (u32)ScriptId::Zer6,
            Zerg7AirTown = (u32)ScriptId::Zr7a,
            Zerg7GroundTown = (u32)ScriptId::Zr7g,
            Zerg7SupportTown = (u32)ScriptId::Zr7s,
            Zerg8ScoutTown = (u32)ScriptId::Zer8,
            Zerg8TemplarTown = (u32)ScriptId::Ze8T,
            Zerg9TealProtoss = (u32)ScriptId::Zer9,
            Zerg9LeftYellowProtoss = (u32)ScriptId::Z9ly,
            Zerg9RightYellowProtoss = (u32)ScriptId::Z9ry,
            Zerg9LeftOrangeProtoss = (u32)ScriptId::Z9lo,
            Zerg9RightOrangeProtoss = (u32)ScriptId::Z9ro,
            Zerg10LeftTeal_Attack = (u32)ScriptId::Z10a,
            Zerg10RightTeal_Support = (u32)ScriptId::Z10b,
            Zerg10LeftYellow_Support = (u32)ScriptId::Z10c,
            Zerg10RightYellow_Attack = (u32)ScriptId::Z10d,
            Zerg10RedProtoss = (u32)ScriptId::Z10e,
            Protoss1ZergTown = (u32)ScriptId::Pro1,
            Protoss2ZergTown = (u32)ScriptId::Pro2,
            Protoss3AirZergTown = (u32)ScriptId::Pr3R,
            Protoss3GroundZergTown = (u32)ScriptId::Pr3G,
            Protoss4ZergTown = (u32)ScriptId::Pro4,
            Protoss5ZergTownIsland = (u32)ScriptId::Pr5I,
            Protoss5ZergTownBase = (u32)ScriptId::Pr5B,
            Protoss7LeftProtossTown = (u32)ScriptId::Pro7,
            Protoss7RightProtossTown = (u32)ScriptId::Pr7B,
            Protoss7ShrineProtoss = (u32)ScriptId::Pr7S,
            Protoss8LeftProtossTown = (u32)ScriptId::Pro8,
            Protoss8RightProtossTown = (u32)ScriptId::Pr8B,
            Protoss8ProtossDefenders = (u32)ScriptId::Pr8D,
            Protoss9GroundZerg = (u32)ScriptId::Pro9,
            Protoss9AirZerg = (u32)ScriptId::Pr9W,
            Protoss9SpellZerg = (u32)ScriptId::Pr9Y,
            Protoss10MiniTowns = (u32)ScriptId::Pr10,
            Protoss10MiniTownMaster = (u32)ScriptId::P10C,
            Protoss10OvermindDefenders = (u32)ScriptId::P10o,
            BroodWarsProtoss1TownA = (u32)ScriptId::PB1A,
            BroodWarsProtoss1TownB = (u32)ScriptId::PB1B,
            BroodWarsProtoss1TownC = (u32)ScriptId::PB1C,
            BroodWarsProtoss1TownD = (u32)ScriptId::PB1D,
            BroodWarsProtoss1TownE = (u32)ScriptId::PB1E,
            BroodWarsProtoss1TownF = (u32)ScriptId::PB1F,
            BroodWarsProtoss2TownA = (u32)ScriptId::PB2A,
            BroodWarsProtoss2TownB = (u32)ScriptId::PB2B,
            BroodWarsProtoss2TownC = (u32)ScriptId::PB2C,
            BroodWarsProtoss2TownD = (u32)ScriptId::PB2D,
            BroodWarsProtoss2TownE = (u32)ScriptId::PB2E,
            BroodWarsProtoss2TownF = (u32)ScriptId::PB2F,
            BroodWarsProtoss3TownA = (u32)ScriptId::PB3A,
            BroodWarsProtoss3TownB = (u32)ScriptId::PB3B,
            BroodWarsProtoss3TownC = (u32)ScriptId::PB3C,
            BroodWarsProtoss3TownD = (u32)ScriptId::PB3D,
            BroodWarsProtoss3TownE = (u32)ScriptId::PB3E,
            BroodWarsProtoss3TownF = (u32)ScriptId::PB3F,
            BroodWarsProtoss4TownA = (u32)ScriptId::PB4A,
            BroodWarsProtoss4TownB = (u32)ScriptId::PB4B,
            BroodWarsProtoss4TownC = (u32)ScriptId::PB4C,
            BroodWarsProtoss4TownD = (u32)ScriptId::PB4D,
            BroodWarsProtoss4TownE = (u32)ScriptId::PB4E,
            BroodWarsProtoss4TownF = (u32)ScriptId::PB4F,
            BroodWarsProtoss5TownA = (u32)ScriptId::PB5A,
            BroodWarsProtoss5TownB = (u32)ScriptId::PB5B,
            BroodWarsProtoss5TownC = (u32)ScriptId::PB5C,
            BroodWarsProtoss5TownD = (u32)ScriptId::PB5D,
            BroodWarsProtoss5TownE = (u32)ScriptId::PB5E,
            BroodWarsProtoss5TownF = (u32)ScriptId::PB5F,
            BroodWarsProtoss6TownA = (u32)ScriptId::PB6A,
            BroodWarsProtoss6TownB = (u32)ScriptId::PB6B,
            BroodWarsProtoss6TownC = (u32)ScriptId::PB6C,
            BroodWarsProtoss6TownD = (u32)ScriptId::PB6D,
            BroodWarsProtoss6TownE = (u32)ScriptId::PB6E,
            BroodWarsProtoss6TownF = (u32)ScriptId::PB6F,
            BroodWarsProtoss7TownA = (u32)ScriptId::PB7A,
            BroodWarsProtoss7TownB = (u32)ScriptId::PB7B,
            BroodWarsProtoss7TownC = (u32)ScriptId::PB7C,
            BroodWarsProtoss7TownD = (u32)ScriptId::PB7D,
            BroodWarsProtoss7TownE = (u32)ScriptId::PB7E,
            BroodWarsProtoss7TownF = (u32)ScriptId::PB7F,
            BroodWarsProtoss8TownA = (u32)ScriptId::PB8A,
            BroodWarsProtoss8TownB = (u32)ScriptId::PB8B,
            BroodWarsProtoss8TownC = (u32)ScriptId::PB8C,
            BroodWarsProtoss8TownD = (u32)ScriptId::PB8D,
            BroodWarsProtoss8TownE = (u32)ScriptId::PB8E,
            BroodWarsProtoss8TownF = (u32)ScriptId::PB8F,
            BroodWarsTerran1TownA = (u32)ScriptId::TB1A,
            BroodWarsTerran1TownB = (u32)ScriptId::TB1B,
            BroodWarsTerran1TownC = (u32)ScriptId::TB1C,
            BroodWarsTerran1TownD = (u32)ScriptId::TB1D,
            BroodWarsTerran1TownE = (u32)ScriptId::TB1E,
            BroodWarsTerran1TownF = (u32)ScriptId::TB1F,
            BroodWarsTerran2TownA = (u32)ScriptId::TB2A,
            BroodWarsTerran2TownB = (u32)ScriptId::TB2B,
            BroodWarsTerran2TownC = (u32)ScriptId::TB2C,
            BroodWarsTerran2TownD = (u32)ScriptId::TB2D,
            BroodWarsTerran2TownE = (u32)ScriptId::TB2E,
            BroodWarsTerran2TownF = (u32)ScriptId::TB2F,
            BroodWarsTerran3TownA = (u32)ScriptId::TB3A,
            BroodWarsTerran3TownB = (u32)ScriptId::TB3B,
            BroodWarsTerran3TownC = (u32)ScriptId::TB3C,
            BroodWarsTerran3TownD = (u32)ScriptId::TB3D,
            BroodWarsTerran3TownE = (u32)ScriptId::TB3E,
            BroodWarsTerran3TownF = (u32)ScriptId::TB3F,
            BroodWarsTerran4TownA = (u32)ScriptId::TB4A,
            BroodWarsTerran4TownB = (u32)ScriptId::TB4B,
            BroodWarsTerran4TownC = (u32)ScriptId::TB4C,
            BroodWarsTerran4TownD = (u32)ScriptId::TB4D,
            BroodWarsTerran4TownE = (u32)ScriptId::TB4E,
            BroodWarsTerran4TownF = (u32)ScriptId::TB4F,
            BroodWarsTerran5TownA = (u32)ScriptId::TB5A,
            BroodWarsTerran5TownB = (u32)ScriptId::TB5B,
            BroodWarsTerran5TownC = (u32)ScriptId::TB5C,
            BroodWarsTerran5TownD = (u32)ScriptId::TB5D,
            BroodWarsTerran5TownE = (u32)ScriptId::TB5E,
            BroodWarsTerran5TownF = (u32)ScriptId::TB5F,
            BroodWarsTerran6TownA = (u32)ScriptId::TB6A,
            BroodWarsTerran6TownB = (u32)ScriptId::TB6B,
            BroodWarsTerran6TownC = (u32)ScriptId::TB6C,
            BroodWarsTerran6TownD = (u32)ScriptId::TB6D,
            BroodWarsTerran6TownE = (u32)ScriptId::TB6E,
            BroodWarsTerran6TownF = (u32)ScriptId::TB6F,
            BroodWarsTerran7TownA = (u32)ScriptId::TB7A,
            BroodWarsTerran7TownB = (u32)ScriptId::TB7B,
            BroodWarsTerran7TownC = (u32)ScriptId::TB7C,
            BroodWarsTerran7TownD = (u32)ScriptId::TB7D,
            BroodWarsTerran7TownE = (u32)ScriptId::TB7E,
            BroodWarsTerran7TownF = (u32)ScriptId::TB7F,
            BroodWarsTerran8TownA = (u32)ScriptId::TB8A,
            BroodWarsTerran8TownB = (u32)ScriptId::TB8B,
            BroodWarsTerran8TownC = (u32)ScriptId::TB8C,
            BroodWarsTerran8TownD = (u32)ScriptId::TB8D,
            BroodWarsTerran8TownE = (u32)ScriptId::TB8E,
            BroodWarsTerran8TownF = (u32)ScriptId::TB8F,
            BroodWarsZerg1TownA = (u32)ScriptId::ZB1A,
            BroodWarsZerg1TownB = (u32)ScriptId::ZB1B,
            BroodWarsZerg1TownC = (u32)ScriptId::ZB1C,
            BroodWarsZerg1TownD = (u32)ScriptId::ZB1D,
            BroodWarsZerg1TownE = (u32)ScriptId::ZB1E,
            BroodWarsZerg1TownF = (u32)ScriptId::ZB1F,
            BroodWarsZerg2TownA = (u32)ScriptId::ZB2A,
            BroodWarsZerg2TownB = (u32)ScriptId::ZB2B,
            BroodWarsZerg2TownC = (u32)ScriptId::ZB2C,
            BroodWarsZerg2TownD = (u32)ScriptId::ZB2D,
            BroodWarsZerg2TownE = (u32)ScriptId::ZB2E,
            BroodWarsZerg2TownF = (u32)ScriptId::ZB2F,
            BroodWarsZerg3TownA = (u32)ScriptId::ZB3A,
            BroodWarsZerg3TownB = (u32)ScriptId::ZB3B,
            BroodWarsZerg3TownC = (u32)ScriptId::ZB3C,
            BroodWarsZerg3TownD = (u32)ScriptId::ZB3D,
            BroodWarsZerg3TownE = (u32)ScriptId::ZB3E,
            BroodWarsZerg3TownF = (u32)ScriptId::ZB3F,
            BroodWarsZerg4TownA = (u32)ScriptId::ZB4A,
            BroodWarsZerg4TownB = (u32)ScriptId::ZB4B,
            BroodWarsZerg4TownC = (u32)ScriptId::ZB4C,
            BroodWarsZerg4TownD = (u32)ScriptId::ZB4D,
            BroodWarsZerg4TownE = (u32)ScriptId::ZB4E,
            BroodWarsZerg4TownF = (u32)ScriptId::ZB4F,
            BroodWarsZerg5TownA = (u32)ScriptId::ZB5A,
            BroodWarsZerg5TownB = (u32)ScriptId::ZB5B,
            BroodWarsZerg5TownC = (u32)ScriptId::ZB5C,
            BroodWarsZerg5TownD = (u32)ScriptId::ZB5D,
            BroodWarsZerg5TownE = (u32)ScriptId::ZB5E,
            BroodWarsZerg5TownF = (u32)ScriptId::ZB5F,
            BroodWarsZerg6TownA = (u32)ScriptId::ZB6A,
            BroodWarsZerg6TownB = (u32)ScriptId::ZB6B,
            BroodWarsZerg6TownC = (u32)ScriptId::ZB6C,
            BroodWarsZerg6TownD = (u32)ScriptId::ZB6D,
            BroodWarsZerg6TownE = (u32)ScriptId::ZB6E,
            BroodWarsZerg6TownF = (u32)ScriptId::ZB6F,
            BroodWarsZerg7TownA = (u32)ScriptId::ZB7A,
            BroodWarsZerg7TownB = (u32)ScriptId::ZB7B,
            BroodWarsZerg7TownC = (u32)ScriptId::ZB7C,
            BroodWarsZerg7TownD = (u32)ScriptId::ZB7D,
            BroodWarsZerg7TownE = (u32)ScriptId::ZB7E,
            BroodWarsZerg7TownF = (u32)ScriptId::ZB7F,
            BroodWarsZerg8TownA = (u32)ScriptId::ZB8A,
            BroodWarsZerg8TownB = (u32)ScriptId::ZB8B,
            BroodWarsZerg8TownC = (u32)ScriptId::ZB8C,
            BroodWarsZerg8TownD = (u32)ScriptId::ZB8D,
            BroodWarsZerg8TownE = (u32)ScriptId::ZB8E,
            BroodWarsZerg8TownF = (u32)ScriptId::ZB8F,
            BroodWarsZerg9TownA = (u32)ScriptId::ZB9A,
            BroodWarsZerg9TownB = (u32)ScriptId::ZB9B,
            BroodWarsZerg9TownC = (u32)ScriptId::ZB9C,
            BroodWarsZerg9TownD = (u32)ScriptId::ZB9D,
            BroodWarsZerg9TownE = (u32)ScriptId::ZB9E,
            BroodWarsZerg9TownF = (u32)ScriptId::ZB9F,
            BroodWarsZerg10TownA = (u32)ScriptId::ZB0A,
            BroodWarsZerg10TownB = (u32)ScriptId::ZB0B,
            BroodWarsZerg10TownC = (u32)ScriptId::ZB0C,
            BroodWarsZerg10TownD = (u32)ScriptId::ZB0D,
            BroodWarsZerg10TownE = (u32)ScriptId::ZB0E,
            BroodWarsZerg10TownF = (u32)ScriptId::ZB0F,
            TerranCustomLevel = (u32)ScriptId::TMCu,
            ZergCustomLevel = (u32)ScriptId::ZMCu,
            ProtossCustomLevel = (u32)ScriptId::PMCu,
            TerranExpansionCustomLevel = (u32)ScriptId::TMCx,
            ZergExpansionCustomLevel = (u32)ScriptId::ZMCx,
            ProtossExpansionCustomLevel = (u32)ScriptId::PMCx,
            TerranCampaignEasy = (u32)ScriptId::TLOf,
            TerranCampaignMedium = (u32)ScriptId::TMED,
            TerranCampaignDifficult = (u32)ScriptId::THIf,
            TerranCampaignInsane = (u32)ScriptId::TSUP,
            TerranCampaignAreaTown = (u32)ScriptId::TARE,
            ZergCampaignEasy = (u32)ScriptId::ZLOf,
            ZergCampaignMedium = (u32)ScriptId::ZMED,
            ZergCampaignDifficult = (u32)ScriptId::ZHIf,
            ZergCampaignInsane = (u32)ScriptId::ZSUP,
            ZergCampaignAreaTown = (u32)ScriptId::ZARE,
            ProtossCampaignEasy = (u32)ScriptId::PLOf,
            ProtossCampaignMedium = (u32)ScriptId::PMED,
            ProtossCampaignDifficult = (u32)ScriptId::PHIf,
            ProtossCampaignInsane = (u32)ScriptId::PSUP,
            ProtossCampaignAreaTown = (u32)ScriptId::PARE,
            ExpansionTerranCampaignEasy = (u32)ScriptId::TLOx,
            ExpansionTerranCampaignMedium = (u32)ScriptId::TMEx,
            ExpansionTerranCampaignDifficult = (u32)ScriptId::THIx,
            ExpansionTerranCampaignInsane = (u32)ScriptId::TSUx,
            ExpansionTerranCampaignAreaTown = (u32)ScriptId::TARx,
            ExpansionZergCampaignEasy = (u32)ScriptId::ZLOx,
            ExpansionZergCampaignMedium = (u32)ScriptId::ZMEx,
            ExpansionZergCampaignDifficult = (u32)ScriptId::ZHIx,
            ExpansionZergCampaignInsane = (u32)ScriptId::ZSUx,
            ExpansionZergCampaignAreaTown = (u32)ScriptId::ZARx,
            ExpansionProtossCampaignEasy = (u32)ScriptId::PLOx,
            ExpansionProtossCampaignMedium = (u32)ScriptId::PMEx,
            ExpansionProtossCampaignDifficult = (u32)ScriptId::PHIx,
            ExpansionProtossCampaignInsane = (u32)ScriptId::PSUx,
            ExpansionProtossCampaignAreaTown = (u32)ScriptId::PARx,
            SendAllUnitsonStrategicSuicideMissions = (u32)ScriptId::Suic,
            SendAllUnitsonRandomSuicideMissions = (u32)ScriptId::SuiR,
            SwitchComputerPlayertoRescuePassive = (u32)ScriptId::Rscu,
            TurnONSharedVisionforPlayer1 = (u32)ScriptId::PlusVi0,
            TurnONSharedVisionforPlayer2 = (u32)ScriptId::PlusVi1,
            TurnONSharedVisionforPlayer3 = (u32)ScriptId::PlusVi2,
            TurnONSharedVisionforPlayer4 = (u32)ScriptId::PlusVi3,
            TurnONSharedVisionforPlayer5 = (u32)ScriptId::PlusVi4,
            TurnONSharedVisionforPlayer6 = (u32)ScriptId::PlusVi5,
            TurnONSharedVisionforPlayer7 = (u32)ScriptId::PlusVi6,
            TurnONSharedVisionforPlayer8 = (u32)ScriptId::PlusVi7,
            TurnOFFSharedVisionforPlayer1 = (u32)ScriptId::MinusVi0,
            TurnOFFSharedVisionforPlayer2 = (u32)ScriptId::MinusVi1,
            TurnOFFSharedVisionforPlayer3 = (u32)ScriptId::MinusVi2,
            TurnOFFSharedVisionforPlayer4 = (u32)ScriptId::MinusVi3,
            TurnOFFSharedVisionforPlayer5 = (u32)ScriptId::MinusVi4,
            TurnOFFSharedVisionforPlayer6 = (u32)ScriptId::MinusVi5,
            TurnOFFSharedVisionforPlayer7 = (u32)ScriptId::MinusVi6,
            TurnOFFSharedVisionforPlayer8 = (u32)ScriptId::MinusVi7,
            MoveDarkTemplarstoRegion = (u32)ScriptId::MvTe,
            ClearPreviousCombatData = (u32)ScriptId::ClrC,
            SetPlayertoEnemy = (u32)ScriptId::Enmy,
            SetPlayertoAlly = (u32)ScriptId::Ally,
            ValueThisAreaHigher = (u32)ScriptId::VluA,
            EnterClosestBunker = (u32)ScriptId::EnBk,
            SetGenericCommandTarget = (u32)ScriptId::StTg,
            MakeTheseUnitsPatrol = (u32)ScriptId::StPt,
            EnterTransport = (u32)ScriptId::EnTr,
            ExitTransport = (u32)ScriptId::ExTr,
            AINukeHere_NuHe = (u32)ScriptId::NuHe,
            AIHarassHere = (u32)ScriptId::HaHe,
            SetUnitOrderToJunkYardDog = (u32)ScriptId::JYDg,
            AINukeHere_DWHe = (u32)ScriptId::DWHe,
            AINukeHere_ReHe = (u32)ScriptId::ReHe
        };

        static const std::unordered_map<Sc::Ai::ScriptId, std::string> scriptNames;
        static const std::unordered_map<Sc::Ai::ScriptId, std::string> scriptIdStr;
        
#pragma pack(push, 1)
        __declspec(align(1)) struct Entry
        {
            enum_t(Flags, u32, { // u32
                UseWithRunAiScriptAtLocation = 0x1,
                StarEditInvisible = 0x2,
                BroodWarOnly = 0x4
            });
            u32 identifier; // 4-byte text, stored in TRIG section
            u32 filePtr; // File pointer in this file, 0 means it's in BWScript.bin
            u32 statStrIndex; // stat_txt.tbl string index for name
            u32 flags;
            
            const std::string & getName(const TblFile & statTxt) const;
            bool getName(const TblFile & statTxt, std::string & outAiName) const;
        };
#pragma pack(pop)

        virtual ~Ai();
        bool load(const std::vector<MpqFilePtr> & orderedSourceFiles, TblFilePtr statTxt = nullptr);
        const Entry & getEntry(size_t aiIndex) const;
        const std::string & getName(size_t aiIndex) const;
        bool getName(size_t aiIndex, std::string & outAiName) const;
        bool getNameById(u32 aiId, std::string & outAiName) const;
        size_t numEntries() const;

    private:
        std::vector<Entry> entries;
        TblFilePtr statTxt;
    };

    static constexpr size_t NumColors = 256;
#pragma pack(push, 1)
    __declspec(align(1)) struct SystemColor {
#ifdef _WIN32
        // RGBQUAD format
        u8 blue;
        u8 green;
        u8 red;
        u8 null;
#else
        u8 red;
        u8 green;
        u8 blue;
        u8 null;
#endif
        SystemColor() : red(0), green(0), blue(0), null(0) {}
        SystemColor(u8 red, u8 green, u8 blue) : red(red), green(green), blue(blue), null(0) {}
        SystemColor(const SystemColor & other, u8 redOffset, u8 greenOffset, u8 blueOffset) : red(other.red+redOffset), green(other.green+greenOffset), blue(other.blue+blueOffset), null(0) {}
    };
#pragma pack(pop)

    class Terrain {
    public:
        static constexpr size_t NumTilesets = 8;
        static constexpr size_t PixelsPerTile = 32;
        static const std::vector<std::string> TilesetNames;

        enum_t(Tileset, u16, { // u16
            Badlands = 0,
            SpacePlatform = 1,
            Installation = 2,
            Ashworld = 3,
            Jungle = 4,
            Desert = 5,
            Arctic = 6,
            Twilight = 7
        });
        
        enum class TileElevation {
            Low,
            Mid,
            High
        };
        
#pragma pack(push, 1)
        __declspec(align(1)) struct TileGroup {
            u16 index;
            u8 buildability;
            u8 groundHeight;
            u16 leftEdge;
            u16 topEdge;
            u16 rightEdge;
            u16 bottomEdge;
            u16 unknown1;
            u16 unknown2; // Unknown: edge piece has rows above it (1 = Basic edge piece, 2 = Right edge piece, 3 = Left edge piece)
            u16 unknown3;
            u16 unknown4; // Unknown: edge piece has rows below it (1 = Basic edge piece, 2 = Right edge piece, 3 = Left edge piece)
            u16 megaTileIndex[16]; // To VF4/VX4
        };
        __declspec(align(1)) struct Doodad {
            enum_t(Type, u16, {
                // TODO: After loading code is working, index all doodads by fetching tileset+index+name from CV5 doodad/stat.txt entries
            });

            u16 index; // Always 1 for doodads?
            u8 buildability;
            u8 flags; // 0x10 = sprite overlay, 0x20 = unit overlay, 0x40 = overlay flipped
            u16 overlayIndex; // Sprites.dat or Units.dat index, depending on overlay flags
            u16 unknown1;
            u16 doodadName; // stat_txt.tbl id
            u16 unknown2;
            u16 ddDataIndex; // dddata.bin index
            u16 doodadWidth; // In tiles
            u16 doodadHeight; // In tiles
            u16 unknown3;
            u16 megaTileRef[16]; // To VF4/VX4
        };
        __declspec(align(1)) struct TileFlags {
            __declspec(align(1))struct MiniTileFlags {
                enum_t(Flags, u16, {
                    Walkable = BIT_0,
                    MidElevation = BIT_1,
                    HighElevation = BIT_2,
                    BlocksView = BIT_3,
                    Ramp = BIT_4
                });

                Flags flags;
                
                inline bool isWalkable() const { return (flags & Flags::Walkable) == Flags::Walkable; }
                inline bool blocksView() const { return (flags & Flags::BlocksView) == Flags::BlocksView; }
                inline bool isRamp() const { return (flags & Flags::Ramp) == Flags::Ramp; }
                inline TileElevation getElevation() const {
                    if ( (flags & Flags::HighElevation) == Flags::HighElevation )
                        return TileElevation::High;
                    else if ( (flags & Flags::MidElevation) == Flags::MidElevation )
                        return TileElevation::Mid;
                    else
                        return TileElevation::Low;
                }
            };

            MiniTileFlags miniTileFlags[4][4];
        };
        __declspec(align(1)) struct TileGraphics {
            __declspec(align(1))struct MiniTileGraphics {
                enum_t(Graphics, u16, {
                    Flipped = BIT_0,
                    Vr4Index = u16_max & x16BIT_0
                });

                Graphics graphics;

                inline bool isFlipped() const { return (graphics & Graphics::Flipped) == Graphics::Flipped; }
                inline u16 vr4Index() const { return (graphics & Graphics::Vr4Index) >> 1; }
            };

            MiniTileGraphics miniTileGraphics[4][4];
        };
        __declspec(align(1)) struct MiniTilePixels {
            u8 wpeIndex[8][8];
        };
        __declspec(align(1)) struct WpeColor {
            u8 red;
            u8 green;
            u8 blue;
            u8 null;
        };
        
        __declspec(align(1)) struct Cv5Dat {
            static constexpr size_t MaxTileGroups = 1024;

            TileGroup tileGroups[MaxTileGroups];
            Doodad doodads[1];

            static inline size_t tileGroupsSize(size_t cv5FileSize) { return cv5FileSize/sizeof(TileGroup); }
            static inline size_t doodadsSize(size_t cv5FileSize) { return cv5FileSize > MaxTileGroups*sizeof(TileGroup) ? cv5FileSize/sizeof(Doodad)-MaxTileGroups : 0; }
        };
        __declspec(align(1)) struct Vf4Dat {
            TileFlags tileFlags[1];

            static inline size_t size(size_t fileSize) { return fileSize/sizeof(TileFlags); }
        };
        __declspec(align(1)) struct Vx4Dat {
            TileGraphics tileGraphics[1];

            static inline size_t size(size_t fileSize) { return fileSize/sizeof(TileGraphics); }
        };
        __declspec(align(1)) struct Vr4Dat {
            MiniTilePixels miniTilePixels[1];

            static inline size_t size(size_t fileSize) { return fileSize/sizeof(MiniTilePixels); }
        };
        __declspec(align(1)) struct WpeDat {
            WpeColor color[NumColors];
        };
        
#pragma pack(pop)
        
        struct Tiles
        {
            std::vector<TileGroup> tileGroups;
            std::vector<Doodad> doodads;
            std::vector<TileFlags> tileFlags;
            std::vector<TileGraphics> tileGraphics;
            std::vector<MiniTilePixels> miniTilePixels;
            std::array<SystemColor, NumColors> systemColorPalette;
            
            static inline size_t getGroupIndex(const u16 & tileIndex) { return size_t(tileIndex / 16); }
            static inline size_t getGroupMemberIndex(const u16 & tileIndex) { return size_t(tileIndex & 0xF); }
            bool load(const std::vector<MpqFilePtr> & orderedSourceFiles, const std::string & tilesetName);
        };

        const Tiles & get(const Tileset & tileset) const;
        bool load(const std::vector<MpqFilePtr> & orderedSourceFiles);
        const std::array<SystemColor, NumColors> & getColorPalette(Tileset tileset) const;

    private:
        Tiles tilesets[NumTilesets];
    };

    class Weapon {
    public:
        static constexpr size_t TotalOriginal = 100;
        static constexpr size_t Total = 130;
        enum_t(Type, u8, { // u8
            GaussRifle_Normal = 0,
            GaussRifle_JimRaynorMarine = 1,
            C10ConcussionRifle_Normal = 2,
            C10ConcussionRifle_SarahKerrigan = 3,
            FragmentationGrenade_Normal = 4,
            FragmentationGrenade_JimRaynorVulture = 5,
            SpiderMines = 6,
            TwinAutocannons_Normal = 7,
            HellfireMissilePack_Normal = 8,
            TwinAutocannons_AlanSchezar = 9,
            HellfireMissilePack_AlanSchezar = 10,
            ArcliteCannon_Normal = 11,
            ArcliteCannon_EdmundDuke = 12,
            FusionCutter = 13,
            FusionCutter_Harvest = 14,
            GeminiMissiles_Normal = 15,
            BurstLasers_Normal = 16,
            GeminiMissiles_TomKazansky = 17,
            BurstLasers_TomKazansky = 18,
            ATSLaserBattery_Normal = 19,
            ATALaserBattery_Normal = 20,
            ATSLaserBattery_NoradIIMengskDuGalle = 21,
            ATALaserBattery_NoradIIMengskDuGalle = 22,
            ATSLaserBattery_Hyperion = 23,
            ATALaserBattery_Hyperion = 24,
            FlameThrower_Normal = 25,
            FlameThrower_GuiMontag = 26,
            ArcliteShockCannon_Normal = 27,
            ArcliteShockCannon_EdmundDuke = 28,
            LongboltMissiles = 29,
            YamatoGun = 30,
            NuclearMissile = 31,
            Lockdown = 32,
            EMPShockwave = 33,
            Irradiate = 34,
            Claws_Normal = 35,
            Claws_DevouringOne = 36,
            Claws_InfestedKerrigan = 37,
            NeedleSpines_Normal = 38,
            NeedleSpines_HunterKiller = 39,
            KaiserBlades_Normal = 40,
            KaiserBlades_Torrasque = 41,
            ToxicSpores_Broodling = 42,
            Spines = 43,
            Spines_Harvest = 44,
            AcidSpray_Unused = 45,
            AcidSpore_Normal = 46,
            AcidSpore_KukulzaGuardian = 47,
            GlaveWurm_Normal = 48,
            GlaveWurm_KukulzaMutalisk = 49,
            Venom_UnusedDefiler = 50,
            Venom_UnusedDefilerHero = 51,
            SeekerSpores = 52,
            SubterraneanTentacle = 53,
            Suicide_InfestedTerran = 54,
            Suicide_Scourge = 55,
            Parasite = 56,
            SpawnBroodlings = 57,
            Ensnare = 58,
            DarkSwarm = 59,
            Plague = 60,
            Consume = 61,
            ParticleBeam = 62,
            ParticleBeam_Harvest = 63,
            PsiBlades_Normal = 64,
            PsiBlades_FenixZealot = 65,
            PhaseDisruptor_Normal = 66,
            PhaseDisruptor_FenixDragoon = 67,
            PsiAssault_NormalUnused = 68,
            PsiAssault_TassadarAldaris = 69,
            PsionicShockwave_Normal = 70,
            PsionicShockwave_TassadarZeratulArchon = 71,
            Id72 = 72,
            DualPhotonBlasters_Normal = 73,
            AntiMatterMissiles_Normal = 74,
            DualPhotonBlasters_Mojo = 75,
            AntiMatterMissiles_Mojo = 76,
            PhaseDisruptorCannon_Normal = 77,
            PhaseDisruptorCannon_Danimoth = 78,
            PulseCannon = 79,
            STSPhotonCannon = 80,
            STAPhotonCannon = 81,
            Scarab = 82,
            StasisField = 83,
            PsiStorm = 84,
            WarpBlades_Zeratul = 85,
            WarpBlades_DarkTemplarHero = 86,
            Missiles_Unused = 87,
            LaserBattery1_Unused = 88,
            TormentorMissiles_Unused = 89,
            Bombs_Unused = 90,
            RaiderGun_Unused = 91,
            LaserBattery2_Unused = 92,
            LaserBattery3_Unused = 93,
            DualPhotonBlasters_Unused = 94,
            FlechetteGrenade_Unused = 95,
            TwinAutocannons_FloorTrap = 96,
            HellfireMissilePack_WallTrap = 97,
            FlameThrower_WallTrap = 98,
            HellfireMissilePack_FloorTrap = 99,
            NeutronFlare = 100,
            DisruptionWeb = 101,
            Restoration = 102,
            HaloRockets = 103,
            CorrosiveAcid = 104,
            MindControl = 105,
            Feedback = 106,
            OpticalFlare = 107,
            Maelstrom = 108,
            SubterraneanSpines = 109,
            GaussRifle0_Unused = 110,
            WarpBlades_Normal = 111,
            C10ConcussionRifle_SamirDuran = 112,
            C10ConcussionRifle_InfestedDuran = 113,
            DualPhotonBlasters_Artanis = 114,
            AntiMatterMissiles_Artanis = 115,
            C10ConcussionRifle_AlexeiStukov = 116,
            Id117 = 117,
            Id118 = 118,
            Id119 = 119,
            Id120 = 120,
            Id121 = 121,
            Id122 = 122,
            Id123 = 123,
            Id124 = 124,
            Id125 = 125,
            Id126 = 126,
            Id127 = 127,
            Id128 = 128,
            Id129 = 129
        });
        
#pragma pack(push, 1)
        __declspec(align(1)) struct DatEntry
        {
            u16 label;
            u32 graphics;
            u8 unused;
            u16 targetFlags;
            u32 minimumRange;
            u32 maximumRange;
            u8 damageUpgrade;
            u8 weaponType;
            u8 weaponBehavior;
            u8 removeAfter;
            u8 weaponEffect;
            u16 innerSplashRadius;
            u16 mediumSplashRadius;
            u16 outerSplashRadius;
            u16 damageAmount;
            u16 damageBonus;
            u8 weaponCooldown;
            u8 damageFactor;
            u8 attackAngle;
            u8 launchSpin;
            u8 forwardOffset;
            u8 upwardOffset;
            u16 targetErrorMessage;
            u16 icon;
        };

        template <bool expansion>
        struct alignas(1) DatFile
        {
            u16 label[expansion ? Total : TotalOriginal];
            u32 graphics[expansion ? Total : TotalOriginal];
            u8 unused[expansion ? Total : TotalOriginal];
            u16 targetFlags[expansion ? Total : TotalOriginal];
            u32 minimumRange[expansion ? Total : TotalOriginal];
            u32 maximumRange[expansion ? Total : TotalOriginal];
            u8 damageUpgrade[expansion ? Total : TotalOriginal];
            u8 weaponType[expansion ? Total : TotalOriginal];
            u8 weaponBehavior[expansion ? Total : TotalOriginal];
            u8 removeAfter[expansion ? Total : TotalOriginal];
            u8 weaponEffect[expansion ? Total : TotalOriginal];
            u16 innerSplashRadius[expansion ? Total : TotalOriginal];
            u16 mediumSplashRadius[expansion ? Total : TotalOriginal];
            u16 outerSplashRadius[expansion ? Total : TotalOriginal];
            u16 damageAmount[expansion ? Total : TotalOriginal];
            u16 damageBonus[expansion ? Total : TotalOriginal];
            u8 weaponCooldown[expansion ? Total : TotalOriginal];
            u8 damageFactor[expansion ? Total : TotalOriginal];
            u8 attackAngle[expansion ? Total : TotalOriginal];
            u8 launchSpin[expansion ? Total : TotalOriginal];
            u8 forwardOffset[expansion ? Total : TotalOriginal];
            u8 upwardOffset[expansion ? Total : TotalOriginal];
            u16 targetErrorMessage[expansion ? Total : TotalOriginal];
            u16 icon[expansion ? Total : TotalOriginal];
        };
#pragma pack(pop)

        bool load(const std::vector<MpqFilePtr> & orderedSourceFiles);
        const DatEntry & get(Type weaponType) const;

    private:
        std::vector<DatEntry> weapons;
    };

    class Sound {
    public:
        static const std::vector<std::string> virtualSoundPaths;
    };

    class Address {
    public:
        enum_t(Patch_1_16_1, u32, {
            DeathTable = 0x0058A364
        });
    };

    class Pcx
    {
    public:
#pragma pack(push, 1)
        __declspec(align(1)) struct PcxFile
        {
            u8 manufacturer;
            u8 verInfo;
            u8 encoding;
            u8 bitCount;
            u16 leftMargin;
            u16 upperMargin;
            u16 rightMargin;
            u16 lowerMargin;
            u16 hozDpi;
            u16 vertDpi;
            u8 palette[48];
            u8 reserved;
            u8 ncp;
            u16 nbs;
            u16 palInfo;
            u16 hozScreenSize;
            u16 vertScreenSize;
            u8 reserved2[54];
            u8 data[1];

            static constexpr size_t PcxHeaderSize = 128;
            static constexpr size_t PaletteSize = 3*256;
            static constexpr u8 MaxOffset = 192;
        };
#pragma pack(pop)

        bool load(const std::vector<MpqFilePtr> & orderedSourceFiles, const std::string & mpqFileName);
        
        std::vector<Sc::SystemColor> palette;
    };

    /**
        The Sc::Data class provides access to StarCraft data that is not statically defined in MappingCore,
        e.g. StarCraft asset files like "arr\\units.dat" or "tileset\badlands.cv5"
    */
    class Data {
    public:
        Terrain terrain;
        Unit units;
        Weapon weapons;
        Sprite sprites;
        Upgrade upgrades;
        Tech techs;
        Ai ai;
        Pcx tunit;
        Pcx tselect;
        Pcx tminimap;

        bool load(Sc::DataFile::BrowserPtr dataFileBrowser = Sc::DataFile::BrowserPtr(new Sc::DataFile::Browser()),
            const std::unordered_map<Sc::DataFile::Priority, Sc::DataFile::Descriptor> & dataFiles = Sc::DataFile::getDefaultDataFiles(),
            const std::string & expectedStarCraftDirectory = getDefaultScPath(),
            FileBrowserPtr<u32> starCraftBrowser = Sc::DataFile::Browser::getDefaultStarCraftBrowser());
        
        static bool GetAsset(const std::vector<MpqFilePtr> & orderedSourceFiles, const std::string & assetMpqPath, std::vector<u8> & outAssetContents);
        static bool GetAsset(const std::string & assetMpqPath, std::vector<u8> & outAssetContents,
            Sc::DataFile::BrowserPtr dataFileBrowser = Sc::DataFile::BrowserPtr(new Sc::DataFile::Browser()),
            const std::unordered_map<Sc::DataFile::Priority, Sc::DataFile::Descriptor> & dataFiles = Sc::DataFile::getDefaultDataFiles(),
            const std::string & expectedStarCraftDirectory = getDefaultScPath(),
            FileBrowserPtr<u32> starCraftBrowser = Sc::DataFile::Browser::getDefaultStarCraftBrowser());

        static bool ExtractAsset(const std::vector<MpqFilePtr> & orderedSourceFiles, const std::string & assetMpqPath, const std::string & systemFilePath);
        static bool ExtractAsset(const std::string & assetMpqPath, const std::string & systemFilePath,
            Sc::DataFile::BrowserPtr dataFileBrowser = Sc::DataFile::BrowserPtr(new Sc::DataFile::Browser()),
            const std::unordered_map<Sc::DataFile::Priority, Sc::DataFile::Descriptor> & dataFiles = Sc::DataFile::getDefaultDataFiles(),
            const std::string & expectedStarCraftDirectory = getDefaultScPath(),
            FileBrowserPtr<u32> starCraftBrowser = Sc::DataFile::Browser::getDefaultStarCraftBrowser());
    };
};

#endif