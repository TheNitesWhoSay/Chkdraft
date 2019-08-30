#ifndef SECTIONS_H
#define SECTIONS_H
#include "Buffer.h"
#include "EscapeStrings.h"
#include "StringTableNode.h"
#include <memory>
#include <string>
#include <deque>
#include <bitset>
#include <vector>

#undef PlaySound

/** 
    This file contains section data for an empty starcraft 1.16.1 hybrid map

    Oct notation is used for all characters below space on the ascii table,
    as well as for all otherwise invisible characters.

    Details about CHK sections can be found at: http://www.staredit.net/wiki/index.php/Scenario.chk
*/

namespace Sc {

    class Player {
    public:
        static constexpr size_t Total = 12;
        static constexpr size_t TotalSlots = 8;
        static constexpr size_t TotalGroups = 27;
        enum class Id { // Typeless
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
        };
        enum class SlotOwner : u8 { // u8
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
        };
    };

    class Unit {
    public:
        static constexpr size_t TotalTypes = 228; // Number of real units; excludes Id228, AnyUnit, Men, Buildings, and Factories
        static constexpr size_t TotalReferenceTypes = 233; // Number of types referenced in triggers; includes all real units as well as Id228, AnyUnit, Men, Buildings, and Factories
        enum class Type : u16 { // u16
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
            Factories = 232
        };
        static constexpr size_t MaxCuwps = 64;
    };

    class Upgrade {
    public:
        static constexpr size_t TotalOriginalTypes = 46;
        static constexpr size_t TotalTypes = 61;
        enum class Type : u32 { // u32
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
        };
    };

    class Tech {
    public:
        static constexpr size_t TotalOriginalTypes = 24;
        static constexpr size_t TotalTypes = 44;
        enum class Type : u32 { // u32
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
        };
    };

    class Ai {
    public:
        enum class ScriptId : u32 { // u32
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
    };

    class Terrain {
    public:
        static constexpr size_t PixelsPerTile = 32;

        enum class Tileset : u16 { // u16
            Badlands = 0,
            SpacePlatform = 1,
            Installation = 2,
            Ashworld = 3,
            Jungle = 4,
            Desert = 5,
            Arctic = 6,
            Twilight = 7
        };
    };

    class Weapon {
    public:
        static constexpr size_t TotalOriginal = 100;
        static constexpr size_t Total = 130;
        enum class Type : u8 { // u8
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
        };
    };

}

enum class SectionName : uint32_t { // The section name values, as they appear in the binary scenario file
    TYPE = 1162893652, VER = 542262614, IVER = 1380275785, IVE2 = 843404873,
    VCOD = 1146045270, IOWN = 1314344777, OWNR = 1380865871, ERA = 541151813,
    DIM = 541935940, SIDE = 1162103123, MTXM = 1297634381, PUNI = 1229870416,
    UPGR = 1380405333, PTEC = 1128617040, UNIT = 1414090325, ISOM = 1297044297,
    TILE = 1162627412, DD2 = 540165188, THG2 = 843532372, MASK = 1263747405,
    STR = 542266451, UPRP = 1347571797, UPUS = 1398100053, MRGN = 1313296973,
    TRIG = 1195987540, MBRF = 1179796045, SPRP = 1347571795, FORC = 1129467718,
    WAV = 542523735, UNIS = 1397313109, UPGS = 1397182549, TECS = 1396917588,
    SWNM = 1296979795, COLR = 1380732739, PUPx = 2018530640, PTEx = 2017809488,
    UNIx = 2018070101, UPGx = 2017939541, TECx = 2017674580,

    OSTR = 1381258063, KSTR = 1381258059, KTRG = 1196577867, KTGP = 1346851915,

    UNKNOWN = u32_max
};

enum class SectionIndex : uint32_t { // The index at which a section appears in the default scenario file, this is not related to section names
    TYPE = 0, VER = 1, IVER = 2, IVE2 = 3,
    VCOD = 4, IOWN = 5, OWNR = 6, ERA = 7,
    DIM = 8, SIDE = 9, MTXM = 10, PUNI = 11,
    UPGR = 12, PTEC = 13, UNIT = 14, ISOM = 15,
    TILE = 16, DD2 = 17, THG2 = 18, MASK = 19,
    STR = 20, UPRP = 21, UPUS = 22, MRGN = 23,
    TRIG = 24, MBRF = 25, SPRP = 26, FORC = 27,
    WAV = 28, UNIS = 29, UPGS = 30, TECS = 31,
    SWNM = 32, COLR = 33, PUPx = 34, PTEx = 35,
    UNIx = 36, UPGx = 37, TECx = 38,

    KSTR = 39,

    UNKNOWN = u32_max
};

enum class LoadBehavior
{
    Standard, // The last instance of a section is used
    Override, // The first instance of the section has part or all of its data overridden by subsequent instances
    Append // Subsequent instances of the section will be appended to the first instance
};

class ChkSection;
using Section = std::shared_ptr<ChkSection>;

class ChkSection
{
    public:
        static constexpr u32 TotalKnownChkSections = 40;
        static constexpr u32 MaxChkSectionSize = s32_max;
        
        ChkSection(SectionName sectionName, bool virtualizable = false, bool dataIsVirtual = false);
        template <typename StructType>
        ChkSection(SectionName sectionName, StructType & data, bool virtualizable = false, bool dataIsVirtual = false);
        virtual ~ChkSection() { }

        virtual void Validate(bool hybridOrBroodWar) { } // throws SectionValidationException
        SectionIndex getIndex() { return sectionIndex; }
        SectionName getName() { return sectionName; }
        template<typename t> t getNameValue() { return (t)sectionName; }
        std::ostream & write(std::ostream &s); // Writes the section name as a u32, then the result of getSize as a u32, then calls writeData

    protected:
        virtual u32 getSize(); // Gets the size of the data that can be written to an output stream, or throws MaxSectionSizeExceeded if size would be over u32_max
        virtual std::ostream & writeData(std::ostream &s, u32 sizeInBytes) { return s; } // Writes exactly sizeInBytes bytes to the output stream, by default this is the buffer data
        bool isVirtual() { return dataIsVirtual; }
        virtual void setVirtual(bool isVirtual) { // If the client calls code that normalizes the size (any change), flag virtual as false
            this->dataIsVirtual = virtualizable ? isVirtual : false; }

        std::shared_ptr<buffer> rawData;

    private:
        SectionIndex sectionIndex;
        SectionName sectionName;
        bool virtualizable; // Whether this section can be different from the expected structure
        bool dataIsVirtual; // Whether this section is different from the expected structure

    public: // Static methods
        static SectionName getName(u32 sectionIndex) { return sectionNames[sectionIndex]; }
        static SectionName getName(SectionIndex sectionIndex) { return sectionNames[(u32)sectionIndex]; }
        static SectionIndex getIndex(SectionName sectionName) { return sectionIndexes.at(sectionName); }
        static LoadBehavior getLoadBehavior(SectionIndex sectionIndex);
        static std::string getNameString(SectionName sectionName);

        class MaxSectionSizeExceeded : public std::exception
        {
            public:
                MaxSectionSizeExceeded(SectionName sectionName, std::string sectionSize);
                virtual ~MaxSectionSizeExceeded() { }
                virtual const char* what() const throw() { return error.c_str(); }
                const std::string error;

            private:
                MaxSectionSizeExceeded(); // Disallow ctor
        };

        class SectionSerializationSizeMismatch : public std::exception
        {
            public:
                SectionSerializationSizeMismatch(SectionName sectionName, u32 expectedSectionSize, size_t actualDataSize);
                virtual ~SectionSerializationSizeMismatch() { }
                virtual const char* what() const throw() { return error.c_str(); }
                const std::string error;

            private:
                SectionSerializationSizeMismatch(); // Disallow ctor
        };

        class SectionValidationException : public std::exception
        {
            public:
                SectionValidationException(SectionName sectionName, std::string error);
                virtual ~SectionValidationException() { }
                virtual const char* what() const throw() { return error.c_str(); }

            private:
                std::string error;
                SectionValidationException(); // Disallow ctor
        };

        ChkSection() { } // Disallow ctor

        // Static data
        static SectionName sectionNames[];
        static std::unordered_map<SectionName, std::string> sectionNameStrings;
        static std::unordered_map<SectionName, SectionIndex> sectionIndexes;
        static std::unordered_map<SectionIndex, LoadBehavior> nonStandardLoadBehaviors;
};

/** Any chk section that either must fit in an exact structure, or is
    an exact structure but is not validated by StarCraft,
    that is it's virtualizable: it may differ from the exact struct
*/
template <typename StructType, bool virtualizable>
class StructSection : public ChkSection
{
    public:
        StructSection(SectionName sectionName) : ChkSection(sectionName) { }
        StructSection(SectionName sectionName, StructType & data) : ChkSection(sectionName, data) { }
        virtual ~StructSection() { }
        StructType & asStruct() { return *data; }

    protected:
        virtual u32 getSize() {
            if ( isVirtual() ) {
                if ( rawData->size() <= u32_max ) return (u32)rawData->size();
                else throw MaxSectionSizeExceeded(getName(), std::to_string(rawData->size()));
            } else return sizeof(StructType);
        }
        virtual std::ostream & writeData(std::ostream &s, u32 sizeInBytes) {
            if ( isVirtual() ) { rawData->write(s, false); } else { s.write(reinterpret_cast<const char*>(&data), sizeof(StructType)); } return s; }

        StructType* data;
};

/** Any chk section that is of variable length or doesn't conform to
    some kind of solid structure is a DynamicSection,
    if it's not validated by StarCraft it's virtualizable: may differ from the exact struct */
template <bool virtualizable>
class DynamicSection : public ChkSection
{
    public:
        DynamicSection(SectionName sectionName) : ChkSection(sectionName) { }
        virtual ~DynamicSection() { }

    protected:
         // Gets the size of the data that can be written to an output stream, or throws MaxSectionSizeExceeded if size would be over u32_max
        virtual u32 getSize() = 0;
        // Writes exactly sizeInBytes bytes to the output stream, if virtual this is the buffer data, else this must be overridden by the individual section class
        virtual std::ostream & writeData(std::ostream &s, u32 sizeInBytes) = 0;
};


class TypeSection; class VerSection; class IverSection; class Ive2Section; class VcodSection; class IownSection;
class OwnrSection; class EraSection; class DimSection; class SideSection; class MtxmSection; class PuniSection;
class UpgrSection; class PtecSection; class UnitSection; class IsomSection; class TileSection; class Dd2Section;
class Thg2Section; class MaskSection; class StrSection; class UprpSection; class UpusSection; class MrgnSection;
class TrigSection; class MbrfSection; class SprpSection; class ForcSection; class WavSection; class UnisSection;
class UpgsSection; class TecsSection; class SwnmSection; class ColrSection; class PupxSection; class PtexSection;
class UnixSection; class UpgxSection; class TecxSection;

class OstrSection; class KstrSection;

typedef std::shared_ptr<TypeSection> TypeSectionPtr; typedef std::shared_ptr<VerSection> VerSectionPtr; typedef std::shared_ptr<IverSection> IverSectionPtr;
typedef std::shared_ptr<Ive2Section> Ive2SectionPtr; typedef std::shared_ptr<VcodSection> VcodSectionPtr; typedef std::shared_ptr<IownSection> IownSectionPtr;
typedef std::shared_ptr<OwnrSection> OwnrSectionPtr; typedef std::shared_ptr<EraSection> EraSectionPtr; typedef std::shared_ptr<DimSection> DimSectionPtr;
typedef std::shared_ptr<SideSection> SideSectionPtr; typedef std::shared_ptr<MtxmSection> MtxmSectionPtr; typedef std::shared_ptr<PuniSection> PuniSectionPtr;
typedef std::shared_ptr<UpgrSection> UpgrSectionPtr; typedef std::shared_ptr<PtecSection> PtecSectionPtr; typedef std::shared_ptr<UnitSection> UnitSectionPtr;
typedef std::shared_ptr<IsomSection> IsomSectionPtr; typedef std::shared_ptr<TileSection> TileSectionPtr; typedef std::shared_ptr<Dd2Section> Dd2SectionPtr;
typedef std::shared_ptr<Thg2Section> Thg2SectionPtr; typedef std::shared_ptr<MaskSection> MaskSectionPtr; typedef std::shared_ptr<StrSection> StrSectionPtr;
typedef std::shared_ptr<UprpSection> UprpSectionPtr; typedef std::shared_ptr<UpusSection> UpusSectionPtr; typedef std::shared_ptr<MrgnSection> MrgnSectionPtr;
typedef std::shared_ptr<TrigSection> TrigSectionPtr; typedef std::shared_ptr<MbrfSection> MbrfSectionPtr; typedef std::shared_ptr<SprpSection> SprpSectionPtr;
typedef std::shared_ptr<ForcSection> ForcSectionPtr; typedef std::shared_ptr<WavSection> WavSectionPtr; typedef std::shared_ptr<UnisSection> UnisSectionPtr;
typedef std::shared_ptr<UpgsSection> UpgsSectionPtr; typedef std::shared_ptr<TecsSection> TecsSectionPtr; typedef std::shared_ptr<SwnmSection> SwnmSectionPtr;
typedef std::shared_ptr<ColrSection> ColrSectionPtr; typedef std::shared_ptr<PupxSection> PupxSectionPtr; typedef std::shared_ptr<PtexSection> PtexSectionPtr;
typedef std::shared_ptr<UnixSection> UnixSectionPtr; typedef std::shared_ptr<UpgxSection> UpgxSectionPtr; typedef std::shared_ptr<TecxSection> TecxSectionPtr;

typedef std::shared_ptr<OstrSection> OstrSectionPtr; typedef std::shared_ptr<KstrSection> KstrSectionPtr;

namespace Chk {
#pragma pack(push, 1)
    
    class Unit; class IsomEntry; class Doodad; class Sprite; class Cuwp; class Location; class Condition; class Action; class Trigger; class StringProperties;
    using UnitPtr = std::shared_ptr<Unit>; using IsomEntryPtr = std::shared_ptr<IsomEntry>; using DoodadPtr = std::shared_ptr<Doodad>; using SpritePtr = std::shared_ptr<Sprite>;
    using CuwpPtr = std::shared_ptr<Cuwp>; using LocationPtr = std::shared_ptr<Location>; using ConditionPtr = std::shared_ptr<Condition>; using ActionPtr = std::shared_ptr<Action>;
    using TriggerPtr = std::shared_ptr<Trigger>; using StringPropertiesPtr = std::shared_ptr<StringProperties>;

    enum class Type : u32 { // u32
        RAWS = 0x53574152, // 1.04 StarCraft and above ("hybrid")
        RAWB = 0x42574152 // Brood War
    };
    
    enum class Version : u16 { // u16
        StarCraft_Original = 59,
        StarCraft_Hybrid = 63, // 1.04 StarCraft and above ("hybrid")
        StarCraft_BroodWar = 205 // Brood War
    };
    
    enum class IVersion : u16 { // u16
        Beta = 9,
        Current = 10
    };
    
    enum class I2Version : u16 { // u16
        StarCraft_1_04 = 11 // 1.04 StarCraft and above ("hybrid") or Brood War
    };
    
    enum class ValidationOpCodes : u8 { // u8
        XOR_Shifted_Sections = 0, // XOR the current hash with total values of OWNR, SIDE*256, FORC*65536
        Add_Shifted_Sections = 1, // Adds to the current hash with the total values of OWNR, SIDE*256, FORC*65536
        Sub_Shifted_Sections = 2, // Subtracts from the current hash with the total values of OWNR, SIDE*256, FORC*65536
        XOR_Sections = 3, // XOR the current hash with the VCOD table at index OWNR, SIDE, FORC, and 0
        XOR_Sections_04 = 4, // Same as XOR_Sections
        XOR_Sections_05 = 5, // Same as XOR_Sections
        ORs_And_Shifts = 6, // Complex function consisting of ORs and shifts
        ORs_And_Reverse_Shifts = 7, // Same as ORS_AND_SHIFTS with shift direction inverted
        NOOP = 8 // No operation
    };
    
    constexpr u32 TotalValidationSeeds = 256;
    constexpr u32 TotalValidationOpCodes = 16;
    
    enum class Race : u8 { // u8
        Zerg = 0,
        Terran = 1,
        Protoss = 2,
        Independent = 3,
        Neutral = 4,
        UserSelectable = 5,
        Random = 6,
        Inactive = 7
    };
    
    enum class Available : u8 { // u8
        No = 0, // Unit/technology is not availble for production/research
        Yes = 1 // Unit/technology is available for production/research
    };
    
    enum class UseDefault : u8 { // u8
        No = 0, // Player does not use defaults for this unit/upgrade/technology or unit/upgrade/technlology does not use default settings
        Yes = 1 // Player uses defaults for this unit/upgrade/technology or unit/upgrade/technology does not use default settings
    };
    
    enum class Researched : u8 { // u8
        No = 0, // Technology is not researched
        Yes = 1 // Technology is researched
    };
    
    __declspec(align(1)) class Unit {
    public:
        enum class RelationFlag : u16 // u16
        {
            NydusLink = BIT_9,
            AddonLink = BIT_10
        };
        enum class State : u16 // u16
        {
            Cloak = BIT_0,
            Burrow = BIT_1,
            InTransit = BIT_2,
            Hallucinated = BIT_3,
            Invincible = BIT_4,

            xCloak = x16BIT_0,
            xBurrow = x16BIT_1,
            xInTransit = x16BIT_2,
            xHallucinate = x16BIT_3,
            xInvincible = x16BIT_4
        };
        enum class ValidField : u16 // u16
        {
            Owner = BIT_0, 
            Hitpoints = BIT_1,
            Shields = BIT_2,
            Energy = BIT_3,
            Resources = BIT_4,
            Hanger = BIT_5,

            xOwner = x16BIT_0,
            xHitpoints = x16BIT_1,
            xShields = x16BIT_2,
            xEnergy = x16BIT_3,
            xResources = x16BIT_4,
            xHanger = x16BIT_5
        };
        enum class Field { // Typeless
            ClassId, Xc, Yc, Type, RelationFlags, ValidStateFlags, ValidFieldFlags, Owner, HitpointPercent, ShieldPercent, EnergyPercent, ResourceAmount, HangerAmount, StateFlags, Unused, RelationClassId
        };

        u32 classId; // Unique number used to identify this unit for related units (may be an addon or the building that has an addon)
        u16 xc;
        u16 yc;
        Sc::Unit::Type type; // u16
        u16 relationFlags;
        u16 validStateFlags;
        u16 validFieldFlags;
        u8 owner;
        u8 hitpointPercent;
        u8 shieldPercent;
        u8 energyPercent;
        u32 resourceAmount;
        u16 hangerAmount;
        u16 stateFlags;
        u32 unused;
        u32 relationClassId; // classId of related unit (may be an addon or the building that has an addon)
    }; // 36 (0x24) bytes

    __declspec(align(1)) class IsomEntry { // 8 bytes
        u16 left;
        u16 top;
        u16 right;
        u16 bottom;
    };
    
    __declspec(align(1)) class Doodad
    {
    public:
        enum class Type : u16 {

        };
        enum class Enabled : u8 {
            Enabled = 0, // Doodad is enabled
            Disabled = 1 // Doodad is disabled
        };

        Type type;
        u16 xc;
        u16 yc;
        u8 owner;
        Enabled enabled; 
    };

    __declspec(align(1)) class Sprite
    {
    public:
        enum class Type : u16 {
            // TODO: index all sprites
        };
        enum class SpriteFlags : u16 {
            DrawAsSprite = BIT_12, // If deselected this is a SpriteUnit
            Disabled = BIT_15 // Only valid if draw as sprite is unchecked, disables the unit
        };

        bool isDrawnAsSprite();

        Type type;
        u16 xc;
        u16 yc;
        u8 owner;
        u8 unused;
        u16 flags;
    };

    enum class FogOfWarPlayers : u8 { // u8
        Visible = 0,
        Player1 = BIT_0,
        Player2 = BIT_1,
        Player3 = BIT_2,
        Player4 = BIT_3,
        Player5 = BIT_4,
        Player6 = BIT_5,
        Player7 = BIT_6,
        Player8 = BIT_7,
        Opaque = u8_max
    };

    constexpr size_t MaxStrings = u16_max + 1; // Including stringId:0
    constexpr size_t MaxKStrings = MaxStrings;
    
    enum class StringId : size_t { // size_t
        NoString = 0,
        DefaultFileName = 0,
        DefaultDescription = 0,
        UnusedSound = 0
    };

    enum class CuwpUsed : u8 { // u8
        No = 0, // CUWP slot is unused
        Yes = 1 // CUWP slot is used
    };

    __declspec(align(1)) class Cuwp
    {
    public:
        enum class State : u16 // u16
        {
            Cloak = BIT_0,
            Burrow = BIT_1,
            InTransit = BIT_2,
            Hallucinated = BIT_3,
            Invincible = BIT_4,

            xCloak = x16BIT_0,
            xBurrow = x16BIT_1,
            xInTransit = x16BIT_2,
            xHallucinated = x16BIT_3,
            xInvincible = x16BIT_4
        };
        enum class ValidField : u16 // u16
        {
            Owner = BIT_0, 
            Hitpoints = BIT_1,
            Shields = BIT_2,
            Energy = BIT_3,
            Resources = BIT_4,
            Hanger = BIT_5,

            xOwner = x16BIT_0,
            xHitpoints = x16BIT_1,
            xShields = x16BIT_2,
            xEnergy = x16BIT_3,
            xResources = x16BIT_4,
            xHanger = x16BIT_5
        };
        
        bool isCloaked();
        bool isBurrowed();
        bool isInTransit();
        bool isHallucinated();
        bool isInvincible();
        
        void setCloaked(bool cloaked);
        void setBurrowed(bool burrowed);
        void setInTransit(bool inTransit);
        void setHallucinated(bool hallucinated);
        void setInvincible(bool invincible);

        u16 validUnitStateFlags;
        u16 validUnitFieldFlags;
        u8 owner;
        u8 hitpointPercent;
        u8 shieldPercent;
        u8 energyPercent;
        u32 resourceAmount;
        u16 hangerAmount;
        u16 unitStateFlags;
    };

    constexpr u32 TotalOriginalLocations = 64;
    constexpr u32 TotalLocations = 255;

    enum class LocationId : u32 { // u32
        NoLocation = 0,
        Anywhere = 64
    };
    
    __declspec(align(1)) class Location
    {
    public:
        enum class Elevation : u16 {
            LowElevation = BIT_0,
            MediumElevation = BIT_1,
            HighElevation = BIT_2,
            LowAir = BIT_3,
            MediumAir = BIT_4,
            HighAir = BIT_5
        };
        enum class Field { // Typeless
            Left, Top, Right, Bottom, StringId, ElevationFlags
        };

        Location();
        
        u32 left;
        u32 top;
        u32 right;
        u32 bottom;
        u16 stringId;
        u16 elevationFlags;
    };

    constexpr u32 TotalForces = 4;
    
    enum class Force : u8 { // u8
        Force1 = 0,
        Force2 = 1,
        Force3 = 2,
        Force4 = 3
    };

    enum class ForceFlags : u8 { // u8
        RandomizeStartLocation = BIT_0,
        RandomAllies = BIT_1,
        AlliedVictory = BIT_2,
        SharedVision = BIT_3,
        All = (RandomizeStartLocation | RandomAllies | AlliedVictory | SharedVision)
    };
    
    constexpr u32 TotalSounds = 512;
    
    constexpr u32 TotalSwitches = 256;

    constexpr u32 MaximumTriggers = 894785; // 894784 real triggers at 2400 bytes each could fit in MaxChkSectionSize: s32_max, maybe last couple bytes would be padded adding a partial trigger

    __declspec(align(1)) class Condition
    {
    public:
        static constexpr size_t NumConditionTypes = 24;
        static constexpr size_t MaxArguments = 9;
        enum class Type : u8 { // u8
            Accumulate = 4,
            Always = 22,
            Bring = 3,
            Command = 2,
            CommandTheLeast = 16,
            CommandTheLeastAt = 17,
            CommandTheMost = 6,
            CommandTheMostAt = 7,
            CountdownTimer = 1,
            Deaths = 15,
            ElapsedTime = 12,
            HighestScore = 9,
            Kill = 5,
            LeastKills = 18,
            LeastResources = 20,
            LowestScore = 19,
            MostKills = 8,
            MostResources = 10,
            Never = 23,
            IsBriefing = 13,
            NoCondition = 0,
            Opponents = 14,
            Score = 21,
            Switch = 11
        };
        enum class VirtualType : s32 { // s32

            Accumulate = 4,
            Always = 22,
            Bring = 3,
            Command = 2,
            CommandTheLeast = 16,
            CommandTheLeastAt = 17,
            CommandTheMost = 6,
            CommandTheMostAt = 7,
            CountdownTimer = 1,
            Deaths = 15,
            ElapsedTime = 12,
            HighestScore = 9,
            Kill = 5,
            LeastKills = 18,
            LeastResources = 20,
            LowestScore = 19,
            MostKills = 8,
            MostResources = 10,
            Never = 23,
            NeverAlt = 13,
            NoCondition = 0,
            Opponents = 14,
            Score = 21,
            Switch = 11,

            Custom = -1,
            Memory = -2,

            Indeterminate = s32_min
        };
        enum class ExtendedBaseType : s32 { // s32
            Memory = (u8)Type::Deaths, // Deaths
        };
        enum class Flags : u8 { // u8
            Disabled = BIT_1, // If set, the trigger condition is disabled/ignored
            UnitTypeUsed = BIT_4, // If set the unitType field is used

            xDisabled = x8BIT_1
        };
        enum class ArgType { // Typeless
            NoType = 0,
            Unit = 1,
            Location = 2,
            Player = 3,
            Amount = 4,
            NumericComparison = 5,
            ResourceType = 6,
            ScoreType = 7,
            Switch = 8,
            SwitchState = 9,
            Comparison = 10, // NumericComparison, SwitchState
            ConditionType = 11,
            TypeIndex = 12, // ResourceType, ScoreType, Switch
            Flags = 13,
            MaskFlag = 14,
            MemoryOffset = 15
        };
        enum class ArgField : u32 { // u32
            LocationId = 0,
            Player = 4,
            Amount = 8,
            UnitType = 12,
            Comparison = 14,
            ConditionType = 15,
            TypeIndex = 16,
            Flags = 17,
            MaskFlag = 18,
            NoField = u32_max
        };
        enum class Amount : u32 { // u32
            All = 0
        };
        enum class Comparison : u8 { // u8
            AtLeast = 0,
            AtMost = 1,
            Set = 2,
            NotSet = 3,
            Exactly = 10
        };
        enum class ScoreType : u8 { // u8
            Total = 0,
            Units = 1,
            Buildings = 2,
            UnitsAndBuildings = 3,
            Kills = 4,
            Razings = 5,
            KillsAndRazings = 6,
            Custom = 7
        };
        enum class MaskFlag : u16 { // u16
            Enabled = 0x4353, // "SC" in little-endian; 'S' = 0x53, 'C' = 0x43
            Disabled = 0
        };
        struct Argument {
            ArgType type;
            ArgField field;
        };
        struct VirtualCondition {
            VirtualType baseConditionType;
            Argument arguments[MaxArguments];
        };

        u32 locationId; // 1 based, is also the bitmask for deaths
        u32 player;
        u32 amount;
        Sc::Unit::Type unitType;
        Comparison comparison;
        Type conditionType;
        u8 typeIndex; // Resource type/score type/switch num
        u8 flags;
        MaskFlag maskFlag; // Set to "SC" (0x53, 0x43) for masked deaths; leave as zero otherwise

        void ToggleDisabled();
        bool isDisabled();
        ArgType getClassicArgType(Type conditionType, size_t argNum);
        ArgType getClassicArgType(VirtualType conditionType, size_t argNum);

        static Argument classicArguments[NumConditionTypes][MaxArguments];
        static Argument textArguments[NumConditionTypes][MaxArguments];
        static u8 defaultFlags[NumConditionTypes];
        static std::unordered_map<VirtualType, VirtualCondition> virtualConditions;
    };

    __declspec(align(1)) class Action
    {
    public:
        static constexpr size_t NumActionTypes = 60;
        static constexpr size_t NumBriefingActionTypes = 10;
        static constexpr size_t InternalDataBytes = 3;
        static constexpr size_t MaxArguments = 11;
        enum class Type : u8 { // u8
            CenterView = 10,
            Comment = 47,
            CreateUnit = 44,
            CreateUnitWithProperties = 11,
            Defeat = 2,
            DisplayTextMessage = 9,
            Draw = 56,
            GiveUnitsToPlayer = 48,
            KillUnit = 22,
            KillUnitAtLocation = 23,
            LeaderboardCtrlAtLoc = 18,
            LeaderboardCtrl = 17,
            LeaderboardGreed = 40,
            LeaderboardKills = 20,
            LeaderboardPoints = 21,
            LeaderboardResources = 19,
            LeaderboardCompPlayers = 32,
            LeaderboardGoalCtrlAtLoc = 34,
            LeaderboardGoalCtrl = 33,
            LeaderboardGoalKills = 36,
            LeaderboardGoalPoints = 37,
            LeaderboardGoalResources = 35,
            MinimapPing = 28,
            ModifyUnitEnergy = 50,
            ModifyUnitHangerCount = 53,
            ModifyUnitHitpoints = 49,
            ModifyUnitResourceAmount = 52,
            ModifyUnitShieldPoints = 51,
            MoveLocation = 38,
            MoveUnit = 39,
            MuteUnitSpeech = 30,
            NoAction = 0,
            Order = 46,
            PauseGame = 5,
            PauseTimer = 54,
            PlaySound = 8,
            PreserveTrigger = 3,
            RemoveUnit = 24,
            RemoveUnitAtLocation = 25,
            RunAiScript = 15,
            RunAiScriptAtLocation = 16,
            SetAllianceStatus = 57,
            SetCountdownTimer = 14,
            SetDeaths = 45,
            SetDoodadState = 42,
            SetInvincibility = 43,
            SetMissionObjectives = 12,
            SetNextScenario = 41,
            SetResources = 26,
            SetScore = 27,
            SetSwitch = 13,
            TalkingPortrait = 29,
            Transmission = 7,
            UnmuteUnitSpeech = 31,
            UnpauseGame = 6,
            UnpauseTimer = 55,
            Victory = 1,
            Wait = 4,
            LastAction = 59,

            BriefingNoAction = 0,
            BriefingWait = 1,
            BriefingPlaySound = 2,
            BriefingTextMessage = 3,
            BriefingMissionObjectives = 4,
            BriefingShowPortrait = 5,
            BriefingHidePortrait = 6,
            BriefingDisplayTalkingPortrait = 7,
            BriefingTransmission = 8,
            BriefingEnableSkipTutorial = 9
        };
        enum class VirtualType : s32 { // s32
            CenterView = 10,
            Comment = 47,
            CreateUnit = 44,
            CreateUnitWithProperties = 11,
            Defeat = 2,
            DisplayTextMessage = 9,
            Draw = 56,
            GiveUnitsToPlayer = 48,
            KillUnit = 22,
            KillUnitAtLocation = 23,
            LeaderboardCtrlAtLoc = 18,
            LeaderboardCtrl = 17,
            LeaderboardGreed = 40,
            LeaderboardKills = 20,
            LeaderboardPoints = 21,
            LeaderboardResources = 19,
            LeaderboardCompPlayers = 32,
            LeaderboardGoalCtrlAtLoc = 34,
            LeaderboardGoalCtrl = 33,
            LeaderboardGoalKills = 36,
            LeaderboardGoalPoints = 37,
            LeaderboardGoalResources = 35,
            MinimapPing = 28,
            ModifyUnitEnergy = 50,
            ModifyUnitHangerCount = 53,
            ModifyUnitHitpoints = 49,
            ModifyUnitResourceAmount = 52,
            ModifyUnitShieldPoints = 51,
            MoveLocation = 38,
            MoveUnit = 39,
            MuteUnitSpeech = 30,
            NoAction = 0,
            Order = 46,
            PauseGame = 5,
            PauseTimer = 54,
            PlaySound = 8,
            PreserveTrigger = 3,
            RemoveUnit = 24,
            RemoveUnitAtLocation = 25,
            RunAiScript = 15,
            RunAiScriptAtLocation = 16,
            SetAllianceStatus = 57,
            SetCountdownTimer = 14,
            SetDeaths = 45,
            SetDoodadState = 42,
            SetInvincibility = 43,
            SetMissionObjectives = 12,
            SetNextScenario = 41,
            SetResources = 26,
            SetScore = 27,
            SetSwitch = 13,
            TalkingPortrait = 29,
            Transmission = 7,
            UnmuteUnitSpeech = 31,
            UnpauseGame = 6,
            UnpauseTimer = 55,
            Victory = 1,
            Wait = 4,

            Custom = -1,
            SetMemory = -2,

            Indeterminate = s32_min,

            BriefingNoAction = 0,
            BriefingWait = 1,
            BriefingPlaySound = 2,
            BriefingTextMessage = 3,
            BriefingMissionObjectives = 4,
            BriefingShowPortrait = 5,
            BriefingHidePortrait = 6,
            BriefingDisplayTalkingPortrait = 7,
            BriefingTransmission = 8,
            BriefingEnableSkipTutorial = 9
        };
        enum class ExtendedBaseType : s32 { // s32
            SetMemory = (u8)Type::SetDeaths, // SetDeaths
        };
        enum class Flags : u8 { // u8
            Disabled = BIT_1, // If set, the trigger action is disabled/ignored
            AlwaysDisplay = BIT_2, // If set, the text message will display regardless of the users subtitles setting
            UnitPropertiesUsed = BIT_3, // If set the unitProperties field is used
            UnitTypeUsed = BIT_4, // If set the unitType field is used

            xDisabled = x8BIT_1,
            xAlwaysDisplay = x8BIT_2
        };
        enum class ArgType : u32 { // u32
            NoType = 0,
            Location = 1,
            String = 2,
            Player = 3,
            Unit = 4,
            NumUnits = 5,
            CUWP = 6,
            TextFlags = 7,
            Amount = 8,
            ScoreType = 9,
            ResourceType = 10,
            StateMod = 11,
            Percent = 12,
            Order = 13,
            Sound = 14,
            Duration = 15,
            Script = 16,
            AllyState = 17,
            NumericMod = 18,
            Switch = 19,
            SwitchMod = 20,
            Type = 21,
            ActionType = 22,
            SecondaryType = 23,
            Flags = 24,
            Number = 25, // Amount, Group2, LocDest, UnitPropNum, ScriptNum
            TypeIndex = 26, // Unit, ScoreType, ResourceType, AllianceStatus
            SecondaryTypeIndex = 27, // NumUnits (0=all), SwitchAction, UnitOrder, ModifyType
            Padding = 28,
            MaskFlag = 29,
            MemoryOffset = 30
        };
        enum class ArgField : u32 { // u32
            LocationId = 0,
            StringId = 4,
            SoundStringId = 8,
            Time = 12,
            Group = 16,
            Number = 20,
            Type = 24,
            ActionType = 26,
            Type2 = 27,
            Flags = 28,
            Padding = 29,
            MaskFlag = 30,
            NoField = u32_max
        };
        enum class Amount : u32 { // u32
            All = 0
        };
        enum class NumUnits : u8 { // u8
            All = 0
        };
        enum class ResourceType : u8 { // u8
            Ore = 0,
            Gas = 1,
            OreAndGas = 2
        };
        enum class AllianceStatus : u16 { // u16
            Enemy = 0,
            Ally = 1,
            AlliedVictory = 2
        };
        enum class ValueModifier : u8 { // u8
            Enable = 4,
            Enabled = 4,
            Set = 4,
            Disable = 5,
            Diabled = 5,
            Clear = 5,
            Toggle = 6,
            SetTo = 7,
            Add = 8,
            Subtract = 9,
            Randomize = 11
        };
        enum class UnitOrders : u8 { // u8
            Move = 0,
            Patrol = 1,
            Attack = 2
        };
        enum class MaskFlag : u16 { // u16
            Enabled = 0x4353, // "SC" in little-endian; 'S' = 0x53, 'C' = 0x43
            Disabled = 0
        };
        struct Argument {
            ArgType type;
            ArgField field;
        };
        struct VirtualAction {
            VirtualType baseActionType;
            Argument arguments[MaxArguments];
        };

        u32 locationId; // 1 based, is also the bitmask for deaths
        u32 stringId;
        u32 soundStringId;
        u32 time;
        u32 group; // Group/ZeroBasedBriefingSlot
        u32 number; // Amount/Group2/LocDest/UnitPropNum/ScriptNum
        u16 type; // Unit/score/resource type/alliance status
        Type actionType; // u8
        u8 type2; // Num units/switch action/unit order/modify type
        u8 flags;
        u8 padding;
        MaskFlag maskFlag; // u16, set to "SC" (0x53, 0x43) for masked deaths

        bool isDisabled() { return (flags & (u8)Flags::Disabled) == (u8)Flags::Disabled; }
        void ToggleDisabled();
        ArgType getClassicArgType(Type actionType, size_t argNum);
        ArgType getClassicArgType(VirtualType actionType, size_t argNum);
        inline bool stringUsed(size_t stringId);
        inline bool gameStringUsed(size_t stringId);
        inline bool commentStringUsed(size_t stringId);
        inline bool briefingStringUsed(size_t stringId);
        inline void markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed);
        inline void markUsedGameStrings(std::bitset<Chk::MaxStrings> &stringIdUsed);
        inline void markUsedCommentStrings(std::bitset<Chk::MaxStrings> &stringIdUsed);
        inline void markUsedBriefingStrings(std::bitset<Chk::MaxStrings> &stringIdUsed);
        void remapStringIds(std::map<u32, u32> stringIdRemappings);
        void remapBriefingStringIds(std::map<u32, u32> stringIdRemappings);
        void deleteString(size_t stringId);
        void deleteBriefingString(size_t stringId);

        static Argument classicArguments[NumActionTypes][MaxArguments];
        static Argument textArguments[NumActionTypes][MaxArguments];
        static u8 defaultFlags[NumActionTypes];
        static std::unordered_map<VirtualType, VirtualAction> virtualActions;
        static bool actionUsesStringArg[NumActionTypes];
        static bool actionUsesGameStringArg[NumActionTypes]; // The only editor-specific string in trigger actions is "comment"
        static bool actionUsesSoundArg[NumActionTypes];
        static Argument classicBriefingArguments[NumBriefingActionTypes][MaxArguments];
        static Argument briefingTextArguments[NumBriefingActionTypes][MaxArguments];
        static u8 briefingDefaultFlags[NumBriefingActionTypes];
        static bool briefingActionUsesStringArg[NumBriefingActionTypes];
        static bool briefingActionUsesSoundArg[NumBriefingActionTypes];
    }; // 32 (0x20) bytes

    __declspec(align(1)) class Trigger
    {
    public:
        static constexpr size_t MaxConditions = 16;
        static constexpr size_t MaxActions = 64;
        static constexpr size_t MaxOwners = 27;
        enum class Flags : u32 {
            IgnoreConditionsOnce = BIT_0,
            IgnoreDefeatDraw = BIT_1,
            PreserveTrigger = BIT_2,
            Disabled = BIT_3,
            IgnoreMiscActions = BIT_4,
            Paused = BIT_5,
            IgnoreWaitSkipOnce = BIT_6
        };
        enum class Owned : u8 { // u8
            Yes = 1,
            No = 0
        };
        Condition & condition(size_t conditionIndex);
        Action & action(size_t actionIndex);
        Owned & owned(size_t ownerIndex);
        Trigger& operator= (const Trigger &trigger);
        void deleteAction(size_t actionIndex, bool alignTop = true);
        void deleteCondition(size_t conditionIndex, bool alignTop = true);
        
        bool preserveTriggerFlagged();
        bool disabled();
        bool ignoreConditionsOnce();
        bool ignoreWaitSkipOnce();
        bool ignoreMiscActionsOnce();
        bool ignoreDefeatDraw();
        bool pauseFlagged();

        void setPreserveTriggerFlagged(bool preserved);
        void setDisabled(bool disabled);
        void setIgnoreConditionsOnce(bool ignoreConditionsOnce);
        void setIgnoreWaitSkipOnce(bool ignoreWaitSkipOnce);
        void setIgnoreMiscActionsOnce(bool ignoreMiscActionsOnce);
        void setIgnoreDefeatDraw(bool ignoreDefeatDraw);
        void setPauseFlagged(bool pauseFlagged);

        size_t numUsedConditions();
        size_t numUsedActions();
        inline bool stringUsed(size_t stringId);
        inline bool gameStringUsed(size_t stringId);
        inline bool commentStringUsed(size_t stringId);
        inline bool briefingStringUsed(size_t stringId);
        inline void markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed);
        inline void markUsedGameStrings(std::bitset<Chk::MaxStrings> &stringIdUsed);
        inline void markUsedCommentStrings(std::bitset<Chk::MaxStrings> &stringIdUsed);
        inline void markUsedBriefingStrings(std::bitset<Chk::MaxStrings> &stringIdUsed);
        void remapStringIds(std::map<u32, u32> stringIdRemappings);
        void remapBriefingStringIds(std::map<u32, u32> stringIdRemappings);
        void deleteString(size_t stringId);
        void deleteBriefingString(size_t stringId);

        void alignConditionsTop();
        void alignActionsTop();

        Condition conditions[MaxConditions]; // 20 bytes * 16 conditions = 320 bytes, start at 0x0
        Action actions[MaxActions]; // 32 bytes (0x20) * 64 actions = 2048 bytes, start at 0x140
        u32 flags; // 4 bytes
        Owned owners[MaxOwners]; // 1 byte * 27 owners = 27 bytes
        u8 currentAction; // Used internally by StarCraft, incremented after each action is executed, trigger execution ends when currentAction equals MaxActions or Action::Type::NoAction is hit
    }; // 2400 (0x960) bytes total

    enum class PlayerColor : u8 { // u8
        Red = 0,
        Blue = 1,
        Teal = 2,
        Purple = 3,
        Orange = 4,
        Brown = 5,
        White = 6,
        Yellow = 7,
        Green = 8,
        PaleYellow = 9,
        Tan = 10,
        Azure_NeutralColor = 11
    };

    constexpr u32 baseFontSize = 8;
    constexpr u32 fontStepSize = 2;

    enum class StrFlags : u8 { // u8
        isUsed = BIT_0,
        hasPriority = BIT_1,
        bold = BIT_2,
        underlined = BIT_3,
        italics = BIT_4,
        sizePlusFourSteps = BIT_5,
        sizePlusTwoSteps = BIT_6,
        sizePlusOneStep = BIT_7
    };
    
    enum class Scope : u32 { // u32
        None = 0,
        Game = BIT_1,
        Editor = BIT_2,
        GameOverEditor = BIT_3,
        EditorOverGame = BIT_4,
        Both = (Game | Editor),
        Either = Both
    };

    enum class UseExpSection
    {
        Auto = 0, // Auto-read based on current version, if updating update both
        Yes = 1, // Always use expansion section
        No = 2, // Always use original section
        Both = 3, // Auto-read based on current version, if updating update both
        YesIfAvailable = 4, // Default to expansion, use original if expansion is unavailable
        NoIfOrigAvailable = 5 // Default to original, use expansion if original is unavailable
    };

    enum class KstrVersion : u32 { // u32
        Deprecated = 1,
        Current = 2
    };

    __declspec(align(1)) class StringProperties {
    public:
        u8 red;
        u8 green;
        u8 blue;
        u8 flags;
        
        StringProperties();
        StringProperties(u8 red, u8 green, u8 blue, bool isUsed, bool hasPriority, bool isBold, bool isUnderlined, bool isItalics, u32 size);
        
        class InvalidSize : std::exception
        {
            public:
                InvalidSize(u32 size, u32 baseFontSize, u32 fontStepSize) : error(error) { }
                virtual const char* what() const throw() { return error.c_str(); }
                const std::string error;
            private:
                InvalidSize(); // Disallow ctor
        };
    };
    
    __declspec(align(1)) struct TYPE {
        Type scenarioType;
    }; // Size: 4 (not validated)

    __declspec(align(1)) struct VER {
        Version version;
    }; // Size: 2 (validated)
    
    __declspec(align(1)) struct IVER {
        IVersion version;
    }; // Size: 2 (not validated)
    
    __declspec(align(1)) struct IVE2 {
        I2Version version;
    }; // Size: 2 (not validated)
    
    __declspec(align(1)) struct VCOD {
        u32 seedValues[TotalValidationSeeds];
        ValidationOpCodes opCodes[TotalValidationOpCodes];
    }; // Size: 1040 (validated)
    
    __declspec(align(1)) struct IOWN {
        Sc::Player::SlotOwner owner[Sc::Player::Total];
    }; // Size: 12 (not validated)
    
    __declspec(align(1)) struct OWNR {
        Sc::Player::SlotOwner owner[Sc::Player::Total];
    }; // Size: 12 (validated)
    
    __declspec(align(1)) struct ERA {
        Sc::Terrain::Tileset tileset;
    }; // Size: 2 (validated)
    
    __declspec(align(1)) struct DIM {
        u16 tileWidth;
        u16 tileHeight;
    }; // Size: 4 (validated)
    
    __declspec(align(1)) struct SIDE {
        Race playerRaces[Sc::Player::Total];
    }; // Size: 12 (validated)
    
    __declspec(align(1)) struct MTXM {
        //u16 tiles[0]; // u16 tiles[tileWidth][tileHeight];
    }; // Size <= 256*256*2 (validated)
    
    __declspec(align(1)) struct PUNI {
        Available playerUnitBuildable[Sc::Unit::TotalTypes][Sc::Player::Total];
        Available defaultUnitBuildable[Sc::Unit::TotalTypes];
        UseDefault playerUnitUsesDefault[Sc::Unit::TotalTypes][Sc::Player::Total];
    }; // Size: 5700 (validated)
    
    __declspec(align(1)) struct UPGR {
        u8 playerMaxUpgradeLevel[Sc::Upgrade::TotalOriginalTypes][Sc::Player::Total];
        u8 playerStartUpgradeLevel[Sc::Upgrade::TotalOriginalTypes][Sc::Player::Total];
        u8 defaultMaxLevel[Sc::Upgrade::TotalOriginalTypes];
        u8 defaultStartLevel[Sc::Upgrade::TotalOriginalTypes];
        UseDefault playerUpgradeUsesDefault[Sc::Upgrade::TotalOriginalTypes][Sc::Player::Total];
    }; // Size: 1748 (validated)
    
    __declspec(align(1)) struct PTEC {
        Available techAvailableForPlayer[Sc::Tech::TotalOriginalTypes][Sc::Player::Total];
        Researched techResearchedForPlayer[Sc::Tech::TotalOriginalTypes][Sc::Player::Total];
        Available techAvailableByDefault[Sc::Tech::TotalOriginalTypes];
        Researched techResearchedByDefault[Sc::Tech::TotalOriginalTypes];
        UseDefault playerUsesDefaultsForTech[Sc::Tech::TotalOriginalTypes][Sc::Player::Total];
    }; // Size: 912 (validated)
    
    __declspec(align(1)) struct UNIT {
        //Unit units[0]; // Unit units[numUnits];
    }; // Size: Multiple of 36 (validated)
    
    __declspec(align(1)) struct ISOM {
        //IsomEntry isomData[0]; // IsomEntry isomData[tileWidth/2 + 1][tileHeight + 1]; // Access x*2, width*y
    }; // Size: (tileWidth / 2 + 1) * (tileHeight + 1) * 8 (not validated)
    
    __declspec(align(1)) struct TILE {
        //u16 tiles[0]; // u16 tiles[tileWidth][tileHeight];
    }; // Size: tileWidth*tileHeight*2 (not validated)
    
    __declspec(align(1)) struct DD2 {
        //Doodad doodads[0]; // Doodad doodads[numDoodads];
    }; // Size: Multiple of 8 (not validated)
    
    __declspec(align(1)) struct THG2 {
        //Sprite sprites[0]; // Sprite sprites[numSprites];
    }; // Size: Multiple of 10 (not validated)
    
    __declspec(align(1)) struct MASK {
        //u8 tileFog[0]; // u8 tileFog[tileWidth][tileHeight];
    }; // Size: tileWidth*tileHeight (not validated)
    
    __declspec(align(1)) struct STR {
        u16 numStrings; // Number of strings in the section (default 1024)
        // u16 stringOffsets[0]; // u16 stringOffsets[numStrings]; // stringId: 1 = stringOffsets[0];
                                 // Offsets to each string within the string section (not within stringData, but within the whole section)
        // void[] stringData; // Character data, usually starting wiht a NUL character, unless recycling sub-strings each string is null terminated
        // Possible non-string data appended after stringData
    }; // Size >= 1 (validated)
    
    __declspec(align(1)) struct UPRP {
        Cuwp createUnitProperties[Sc::Unit::MaxCuwps];
    }; // Size: 1280 (validated)
    
    __declspec(align(1)) struct UPUS {
        CuwpUsed cuwpUsed[Sc::Unit::MaxCuwps];
    }; // Size: 64 (validated
    
    __declspec(align(1)) struct MRGN {
        Location locations[TotalLocations]; // If this is a vanilla map, only the first 64 locations are usable
    }; // Size: 1280 in Original, 5100 in Hybrid or Broodwar (validated)
    
    __declspec(align(1)) struct TRIG {
        // Trigger triggers[0]; // Trigger[numTriggers] triggers;
    }; // Size: Multiple of 2400 (validated)

    __declspec(align(1)) struct MBRF {
        // Trigger triggers[0]; // Chk::Trigger[numTriggers];
    }; // Size: Multiple of 2400 (validated)
    
    __declspec(align(1)) struct SPRP {
        u16 scenarioNameStringId;
        u16 scenarioDescriptionStringId;
    }; // Size: 4 (validated)
    
    __declspec(align(1)) struct FORC {
        Force playerForce[Sc::Player::TotalSlots];
        u16 forceString[TotalForces];
        u8 flags[TotalForces];
    }; // Size <= 20 (validated)
    
    __declspec(align(1)) struct WAV {
        u32 soundPathStringId[TotalSounds];
    }; // Size: 2048 (not validated)
    
    __declspec(align(1)) struct UNIS {
        UseDefault useDefault[Sc::Unit::TotalTypes];
        u32 hitpoints[Sc::Unit::TotalTypes]; // Displayed value is hitpoints / 256
        u16 shieldPoints[Sc::Unit::TotalTypes];
        u8 armorLevel[Sc::Unit::TotalTypes]; // Note: 255 armor + ultra armor upgrade = 0 armor
        u16 buildTime[Sc::Unit::TotalTypes];
        u16 mineralCost[Sc::Unit::TotalTypes];
        u16 gasCost[Sc::Unit::TotalTypes];
        u16 nameStringId[Sc::Unit::TotalTypes];
        u16 baseDamage[Sc::Weapon::TotalOriginal];
        u16 upgradeDamage[Sc::Weapon::TotalOriginal];
    }; // Size: 4048 (validated)

    __declspec(align(1)) struct UPGS {
        UseDefault useDefault[Sc::Upgrade::TotalOriginalTypes];
        u16 baseMineralCost[Sc::Upgrade::TotalOriginalTypes];
        u16 mineralCostFactor[Sc::Upgrade::TotalOriginalTypes];
        u16 baseGasCost[Sc::Upgrade::TotalOriginalTypes];
        u16 gasCostFactor[Sc::Upgrade::TotalOriginalTypes];
        u16 baseResearchTime[Sc::Upgrade::TotalOriginalTypes];
        u16 researchTimeFactor[Sc::Upgrade::TotalOriginalTypes];
    }; // Size: 598 (validated)
    
    __declspec(align(1)) struct TECS {
        UseDefault useDefault[Sc::Tech::TotalOriginalTypes];
        u16 mineralCost[Sc::Tech::TotalOriginalTypes];
        u16 gasCost[Sc::Tech::TotalOriginalTypes];
        u16 researchTime[Sc::Tech::TotalOriginalTypes];
        u16 energyCost[Sc::Tech::TotalOriginalTypes];
    }; // Size: 216 (validated)
    
    __declspec(align(1)) struct SWNM {
        u32 switchName[TotalSwitches];
    }; // Size: 1024 (not validated)
    
    __declspec(align(1)) struct COLR {
        PlayerColor playerColor[Sc::Player::TotalSlots];
    }; // Size: 8 (validated)
    
    __declspec(align(1)) struct PUPx {
        u8 playerMaxUpgradeLevel[Sc::Upgrade::TotalTypes][Sc::Player::Total];
        u8 playerStartUpgradeLevel[Sc::Upgrade::TotalTypes][Sc::Player::Total];
        u8 defaultMaxLevel[Sc::Upgrade::TotalTypes];
        u8 defaultStartLevel[Sc::Upgrade::TotalTypes];
        UseDefault playerUpgradeUsesDefault[Sc::Upgrade::TotalTypes][Sc::Player::Total];
    }; // Size: 2318 (validated)
    
    __declspec(align(1)) struct PTEx {
        Available techAvailableForPlayer[Sc::Tech::TotalTypes][Sc::Player::Total];
        Researched techResearchedForPlayer[Sc::Tech::TotalTypes][Sc::Player::Total];
        Available techAvailableByDefault[Sc::Tech::TotalTypes];
        Researched techResearchedByDefault[Sc::Tech::TotalTypes];
        UseDefault playerUsesDefaultsForTech[Sc::Tech::TotalTypes][Sc::Player::Total];
    }; // Size: 1672 (validated)
    
    __declspec(align(1)) struct UNIx {
        UseDefault useDefault[Sc::Unit::TotalTypes];
        u32 hitpoints[Sc::Unit::TotalTypes]; // Displayed value is hitpoints / 256
        u16 shieldPoints[Sc::Unit::TotalTypes];
        u8 armorLevel[Sc::Unit::TotalTypes]; // Note: 255 armor + ultra armor upgrade = 0 armor
        u16 buildTime[Sc::Unit::TotalTypes];
        u16 mineralCost[Sc::Unit::TotalTypes];
        u16 gasCost[Sc::Unit::TotalTypes];
        u16 nameStringId[Sc::Unit::TotalTypes];
        u16 baseDamage[Sc::Weapon::Total];
        u16 upgradeDamage[Sc::Weapon::Total];
    }; // Size: 4168 (validated)
    
    __declspec(align(1)) struct UPGx {
        UseDefault useDefault[Sc::Upgrade::TotalTypes];
        u16 baseMineralCost[Sc::Upgrade::TotalTypes];
        u16 mineralCostFactor[Sc::Upgrade::TotalTypes];
        u16 baseGasCost[Sc::Upgrade::TotalTypes];
        u16 gasCostFactor[Sc::Upgrade::TotalTypes];
        u16 baseResearchTime[Sc::Upgrade::TotalTypes];
        u16 researchTimeFactor[Sc::Upgrade::TotalTypes];
    }; // Size: 794 (validated)
    
    __declspec(align(1)) struct TECx {
        UseDefault useDefault[Sc::Tech::TotalTypes];
        u16 mineralCost[Sc::Tech::TotalTypes];
        u16 gasCost[Sc::Tech::TotalTypes];
        u16 researchTime[Sc::Tech::TotalTypes];
        u16 energyCost[Sc::Tech::TotalTypes];
    }; // Size: 396 (validated)
    
    __declspec(align(1)) struct OSTR {
        u32 version; // Current version: 1
        u32 scenarioName;
        u32 scenarioDescription;
        u32 forceName[Chk::TotalForces];
        u32 unitName[Sc::Unit::TotalTypes];
        u32 expUnitName[Sc::Unit::TotalTypes];
        u32 soundPath[Chk::TotalSounds];
        u32 switchName[Chk::TotalSwitches];
        u32 locationName[Chk::TotalLocations];
    };

    __declspec(align(1)) struct KSTR {
        u32 version; // Current version: 2
        u32 numStrings; // Number of strings in the section
        //u32 stringOffsets[0]; // u32 stringOffsets[numStrings]; // Offsets to each string within the string section (not within stringData, but within the whole section)
        // StringProperties[numStrings] stringProperties; // String properties
        // void[] stringData; // List of strings, each null terminated, starting with one NUL character
    }; // Size: 8+8*numStrings+stringDataSize (not validated)

    
    std::ostream & operator<< (std::ostream & out, const Unit & unit);
    std::ostream & operator<< (std::ostream & out, const Doodad & doodad);
    std::ostream & operator<< (std::ostream & out, const Sprite & sprite);
    std::ostream & operator<< (std::ostream & out, const Location & location);
    std::ostream & operator<< (std::ostream & out, const Condition & condition);
    std::ostream & operator<< (std::ostream & out, const Action & action);
    std::ostream & operator<< (std::ostream & out, const Trigger & trigger);

#pragma pack(pop)
}

class StrProp {
    public:
        u8 red;
        u8 green;
        u8 blue;
        u32 size;
        bool isUsed;
        bool hasPriority;
        bool isBold;
        bool isUnderlined;
        bool isItalics;
    
        StrProp();
        StrProp(Chk::StringProperties stringProperties);
};

class ScStr;
using ScStrPtr = std::shared_ptr<ScStr>;
class ScStr
{
    public:
        const char* str; // The content of this string (RawString/no formatting)
        
        ScStr(const std::string &str);
        ScStr(const std::string &str, const StrProp &strProp);

        bool empty();
        size_t length();

        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkdString (Editor <01>Style)
        int compare(const StringType &str);

        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkdString (Editor <01>Style)
        StringType toString();

        ScStrPtr getParentStr();
        ScStrPtr getChildStr();
        static bool adopt(ScStrPtr lhs, ScStrPtr rhs); // Make lhs the parent of rhs if possible, or vice-versa
        void disown(); // Break any connections with parent and child strings

    private:
        ScStrPtr parentStr; // The larger string inside which this string fits, if null then this is a root string
        ScStrPtr childStr; // The next largest string that fits inside this string
        std::string allocation; // If parentStr is null, then this is the actual string data and str points to the first character
                                // else str points to first character of this string within the allocation of the highest-order parent
        StrProp strProp; // Additional color and font details, if this string is extended and gets stored

        static void adopt(ScStrPtr parent, ScStrPtr child, size_t parentLength, size_t childLength, const char* parentSubString);
};

class ScStrPair
{
    public:
        ScStrPtr gameStr;
        ScStrPtr editorStr;
        
        ScStrPair(const std::string &str, Chk::Scope storageScope = Chk::Scope::Game);
        ScStrPair(const std::string &str, Chk::Scope storageScope = Chk::Scope::Game, StrProp strProp = StrProp());
        ScStrPair(const std::string &gameStr, const std::string &editorStr);
        ScStrPair(const std::string &gameStr, const std::string &editorStr, StrProp strProp = StrProp());
};

enum class StrCompressFlag : u32 {
    DuplicateStringRecycling = BIT_0,
    LastStringTrick = BIT_1,
    ReverseStacking = BIT_2,
    SizeBytesRecycling = BIT_3,
    SubStringRecycling = BIT_4 | DuplicateStringRecycling,
    OffsetInterlacing = BIT_5 | SubStringRecycling, // When active, ignores ReverseStacking
    OrderShuffledInterlacing = BIT_6 | OffsetInterlacing, // When active, ignores LastStringTrick
    SpareShuffledInterlacing = BIT_7 | OffsetInterlacing,
    SubShuffledInterlacing = BIT_8 | OffsetInterlacing & (~SubStringRecycling) | DuplicateStringRecycling, // When active, ignores 

    SubLastStringTrick = SubStringRecycling | LastStringTrick, // Causes different LastStringTrick behavior
    SizedLastStringTrick = SizeBytesRecycling | LastStringTrick, // Causes different LastStringTrick behavior
    SubSizedLastStringTrick = SubStringRecycling | SizeBytesRecycling | LastStringTrick, // Causes different LastStringTrick behavior
    None = 0,
    AllNonInterlacing = DuplicateStringRecycling | LastStringTrick | ReverseStacking | SizeBytesRecycling | SubStringRecycling,
    AllInterlacing = OffsetInterlacing | OrderShuffledInterlacing | SpareShuffledInterlacing | SubShuffledInterlacing,
    All = AllNonInterlacing | AllInterlacing,
    Unchanged = u32_max,
    Default = DuplicateStringRecycling
};
/** None - No compression methods applied
    DuplicateStringRecycling - All duplicate strings share one stringId
    LastStringTrick - The largest has the highest offset value
    ReverseStacking - All offsets use the highest necessary string ids such that character data preceeds all offsets (the NUL/zero-length string loses its sticky position)
    SizeBytesRecycling - If possible to do so, the bytes denoting the number of strings in the section are also used as characters
    SubStringRecycling - All eligible sub-strings (strings that fit in the end of some other string) are recycled
    OffsetInterlacing - Offsets are combined with character data where possible
    OrderShuffledInterlacing - OffsetInterlacing is attempted with strings arranged in every possible order
    SpareShuffledInterlacing - OffsetInterlacing is attempted with every possible distribution of one to total spare codepoints
    SubShuffledInterlacing - OffsetInterlacing is attempted with every possible permutation of potentially advantagous sub-strings unrecycled or recycled
            
    Fighting three limits...
    1.) Max Strings     :   numStrings <= 65535
    2.) Max Characters  :   SUM(LENGTH(rootStrings)) + numRootStrings - (LastStringTrick ? LENGTH(lastString) : 0) - (SizeBytesRecycling ? 0 : 2) < 65536 &&
                            SUM(LENGTH(rootStrings)) + numRootStrings <= 2147483647
    3.) Chars & Offsets :   IF ( LastStringTrick && MAX(LENGTH(string)) > 65538 && MAX(stringOffset) + LENGTH(lastString) >= 131074 ) {
                                2147483647 >= SUM(LENGTH(rootStrings)) + numRootStrings + 2*numStrings +
                                    (SizeBytesRecycling ? 0 : 2) +
                                    (OffsetInterlacing ? offsetBytesBlocked - offsetBytesRecycled : 0) &&
                                131074 >= SUM(LENGTH(rootStrings)) + numRootStrings + 2*numStrings - (LENGTH(lastString) + MAX(stringOffset) - 131074)
                                    (SizeBytesRecycling ? 0 : 2) +
                                    (OffsetInterlacing ? offsetBytesRecycled - offsetBytesBlocked : 0)
                            } else { // No excessively large last string
                                131074 >= SUM(LENGTH(rootStrings)) + numRootStrings + 2*numStrings +  +
                                    (SizeByteRecycling ? 0 : 2) +
                                    (OffsetInterlacing ? offsetBytesBlocked - offsetBytesRecycled : 0)
                            }

    1.) This hard limit is given by numStrings being a u16, you only have usuable offsets at indexes 0-65535/stringIds 1-65536 .
    2.) The first part of this limit is given by string offsets being u16, such that no string points past byte 65535 .
        Each root string (potentially containing substrings) occupies its length plus one NUL character.
        The last string/substrings may have an ending that flows onto the 65536th byte and beyond, if it does, then that NUL character can be ignored.
        The first two bytes of the section which denote numStrings may also be used as characters.
        Even with the last string flowing over the regular limit, you'll still hit the section max (s32_max: 2147483647) eventually.
    3.) Offsets being an array of u16s with a max index of numStrings (65535) can only occupy space before the 131074th byte
        If the last string flows beyond that point then we're given two limits: first the limit of the section size
        and second all characters and offsets minus any overhang from the last string must fit in the first 131074 bytes.
        Else we're given one limit: all characters and offsets must fit in a space no larger than 131074 bytes.

    (1) is addressed only by "DuplicateStringRecycling", no other compression methods reduce the string count (sub-strings still have their own offset/are their own string)
    (2) is addressed by "DuplicateStringRecycling", "LastStringTrick", "ReverseStacking", "SizeBytesRecycling", and "SubStringRecycling"
    (3) is addressed by everything that addressed (2) (except "LastStringTrick"), plus all types of offset interlacing

    - Automatically fail an operation if none of the compression methods affecting that limit, combined in every possible way can make the above equations satisfy.
    - Calculate section configuration viability before physically laying them out in memory
*/

class StrCompressionElevator;
using StrCompressionElevatorPtr = std::shared_ptr<StrCompressionElevator>;
class StrCompressionElevator
{
    public:
        bool elevate(u32 currentlyAllowedCompressionFlags, u32 nextAllowableCompression) { return false; }

        static StrCompressionElevatorPtr NeverElevate() { return StrCompressionElevatorPtr(new StrCompressionElevator()); }
};

class StringException : std::exception
{
    public:
        StringException(const std::string &error) : error(error) { }
        virtual const char* what() const throw() { return error.c_str(); }
        const std::string error;
    private:
        StringException(); // Disallow ctor
};

class MaximumStringsExceeded : public StringException
{
    public:
        MaximumStringsExceeded(std::string sectionName, size_t numStrings, size_t maxStrings);
        MaximumStringsExceeded();
        virtual const char* what() const throw() { return error.c_str(); }
};

class InsufficientStringCapacity : public StringException
{
    public:
        InsufficientStringCapacity(std::string sectionName, size_t numStrings, size_t requestedCapacity, bool autoDefragment);
        virtual const char* what() const throw() { return error.c_str(); }
    private:
        InsufficientStringCapacity(); // Disallow ctor
};

class MaximumCharactersExceeded : StringException
{
    public:
        MaximumCharactersExceeded(std::string sectionName, size_t numCharacters, size_t characterSpaceSize);
        virtual const char* what() const throw() { return error.c_str(); }
    private:
        MaximumCharactersExceeded(); // Disallow ctor
};

class MaximumOffsetAndCharsExceeded : StringException
{
    public:
        MaximumOffsetAndCharsExceeded(std::string sectionName, size_t numStrings, size_t numCharacters, size_t sectionSpace);
        virtual const char* what() const throw() { return error.c_str(); }
    private:
        MaximumOffsetAndCharsExceeded(); // Disallow ctor
};

class StrSynchronizer
{
    public:
        class SyncException;

        StrSynchronizer(u32 requestedCompressionFlags, u32 allowedCompressionFlags)
            : requestedCompressionFlags(requestedCompressionFlags), allowedCompressionFlags(allowedCompressionFlags) { }

        virtual void synchronizeToStrBuffer(buffer &rawData, StrCompressionElevatorPtr compressionElevator = StrCompressionElevator::NeverElevate(),
            u32 requestedCompressionFlags = (u32)StrCompressFlag::Unchanged, u32 allowedCompressionFlags = (u32)StrCompressFlag::Unchanged) = 0;
        virtual void synchronzieFromStrBuffer(const buffer &rawData) = 0;

        virtual void synchronizeToKstrBuffer(buffer &rawData, StrCompressionElevatorPtr compressionElevator = StrCompressionElevator::NeverElevate(),
            u32 requestedCompressionFlags = (u32)StrCompressFlag::Unchanged, u32 allowedCompressionFlags = (u32)StrCompressFlag::Unchanged) = 0;
        virtual void synchronzieFromKstrBuffer(const buffer &rawData) = 0;

        u32 getRequestedCompressionFlags() const { return requestedCompressionFlags; }
        u32 getAllowedCompressionFlags() const { return allowedCompressionFlags; }
        void setRequestedCompressionFlags(u32 requestedCompressionFlags) { this->requestedCompressionFlags = requestedCompressionFlags; }
        void setAllowedCompressionFlags(u32 allowedCompressionFlags) { this->allowedCompressionFlags = allowedCompressionFlags; }

    private:
        u32 requestedCompressionFlags;
        u32 allowedCompressionFlags;

        StrSynchronizer(); // Disallow ctor
};
using StrSynchronizerPtr = std::shared_ptr<StrSynchronizer>;


class TypeSection : public StructSection<Chk::TYPE, true>
{
    public:
        static Section GetDefault();
        TypeSection(Chk::TYPE & data);

        void setType(Chk::Type type);
};

class VerSection : public StructSection<Chk::VER, false>
{
    public:
        static Section GetDefault();
        VerSection(Chk::VER & data);

        bool isOriginal();
        bool isHybrid();
        bool isExpansion();
        bool isHybridOrAbove();
        Chk::Version getVersion();
        void setVersion(Chk::Version version);
};

class IverSection : public StructSection<Chk::IVER, true>
{
    public:
        static Section GetDefault();
        IverSection(Chk::IVER & data);

        Chk::IVersion getVersion();
        void setVersion(Chk::IVersion version);
};

class Ive2Section : public StructSection<Chk::IVE2, true>
{
    public:
        static Section GetDefault();
        Ive2Section(Chk::IVE2 & data);

        Chk::I2Version getVersion();
        void setVersion(Chk::I2Version version);
};

class VcodSection : public StructSection<Chk::VCOD, false>
{
    public:
        static Section GetDefault();
        VcodSection(Chk::VCOD & data);

        bool isDefault();
        void setToDefault();
};

class IownSection : public StructSection<Chk::IOWN, true>
{
    public:
        static Section GetDefault();
        IownSection(Chk::IOWN & data);

        Sc::Player::SlotOwner getSlotOwner(size_t slotIndex);
        void setSlotOwner(size_t slotIndex, Sc::Player::SlotOwner owner);
};

class OwnrSection : public StructSection<Chk::OWNR, false>
{
    public:
        static Section GetDefault();
        OwnrSection(Chk::OWNR & data);
        
        Sc::Player::SlotOwner getSlotOwner(size_t slotIndex);
        void setSlotOwner(size_t slotIndex, Sc::Player::SlotOwner owner);
};

class EraSection : public StructSection<Chk::ERA, false>
{
    public:
        static Section GetDefault(Sc::Terrain::Tileset tileset);
        EraSection(Chk::ERA & data);

        Sc::Terrain::Tileset getTileset();
        void setTileset(Sc::Terrain::Tileset tileset);
};

class DimSection : public StructSection<Chk::DIM, false>
{
    public:
        static Section GetDefault(u16 tileWidth, u16 tileHeigh);
        DimSection(Chk::DIM & data);

        size_t getTileWidth();
        size_t getTileHeight();
        size_t getPixelWidth();
        size_t getPixelHeight();
        void setTileWidth(u16 tileWidth);
        void setTileHeight(u16 tileHeight);
        void setDimensions(u16 tileWidth, u16 tileHeight);
};

class SideSection : public StructSection<Chk::SIDE, false>
{
    public:
        static Section GetDefault();
        SideSection(Chk::SIDE & data);

        Chk::Race getPlayerRace(size_t playerIndex);
        void setPlayerRace(size_t playerIndex, Chk::Race race);
};

class MtxmSection : public DynamicSection<false>
{
    public:
        static Section GetDefault(u16 tileWidth, u16 tileHeigh);
        MtxmSection();

        u16 getTile(size_t tileIndex);
        void setTile(size_t tileIndex, u16 tileValue);
        void setDimensions(u16 newTileWidth, u16 newTileHeight, u16 oldTileWidth, u16 oldTileHeight, s32 leftEdge = 0, s32 topEdge = 0);

    protected:
        virtual u32 getSize();
        virtual std::ostream & writeData(std::ostream &s, u32 sizeInBytes);
};

class PuniSection : public StructSection<Chk::PUNI, false>
{
    public:
        static Section GetDefault();
        PuniSection();

        bool isUnitBuildable(Sc::Unit::Type unitType, size_t playerIndex);
        bool isUnitDefaultBuildable(Sc::Unit::Type unitType);
        bool playerUsesDefault(Sc::Unit::Type unitType, size_t playerIndex);
        void setUnitBuildable(Sc::Unit::Type unitType, size_t playerIndex, bool buildable);
        void setUnitDefaultBuildable(Sc::Unit::Type unitType, bool buildable);
        void setPlayerUsesDefault(Sc::Unit::Type unitType, size_t playerIndex, bool useDefault);
};

class UpgrSection : public StructSection<Chk::UPGR, false>
{
    public:
        static Section GetDefault();
        UpgrSection(Chk::UPGR & data);

        size_t getMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex);
        size_t getStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex);
        size_t getDefaultMaxUpgradeLevel(Sc::Upgrade::Type upgradeType);
        size_t getDefaultStartUpgradeLevel(Sc::Upgrade::Type upgradeType);
        bool playerUsesDefault(Sc::Upgrade::Type upgradeType, size_t playerIndex);
        void setMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, u8 maxUpgradeLevel);
        void setStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, u8 startUpgradeLevel);
        void setDefaultMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, u8 maxUpgradeLevel);
        void setDefaultStartUpgradeLevel(Sc::Upgrade::Type upgradeType, u8 startUpgradeLevel);
        void setPlayerUsesDefault(Sc::Upgrade::Type upgradeType, size_t playerIndex, bool useDefault);
};

class PtecSection : public StructSection<Chk::PTEC, false>
{
    public:
        static Section GetDefault();
        PtecSection(Chk::PTEC & data);

        bool techAvailable(Sc::Tech::Type techType, size_t playerIndex);
        bool techResearched(Sc::Tech::Type techType, size_t playerIndex);
        bool techDefaultAvailable(Sc::Tech::Type techType);
        bool techDefaultResearched(Sc::Tech::Type techType);
        bool playerUsesDefault(Sc::Tech::Type techType, size_t playerIndex);
        void setTechAvailable(Sc::Tech::Type techType, size_t playerIndex, bool available);
        void setTechResearched(Sc::Tech::Type techType, size_t playerIndex, bool researched);
        void setDefaultTechAvailable(Sc::Tech::Type techType, bool available);
        void setDefaultTechResearched(Sc::Tech::Type techType, bool researched);
        void setPlayerUsesDefault(Sc::Tech::Type techType, size_t playerIndex, bool useDefault);
};

class UnitSection : public DynamicSection<false>
{
    public:
        static Section GetDefault();
        UnitSection();

        size_t numUnits();
        std::shared_ptr<Chk::Unit> getUnit(size_t unitIndex);
        size_t addUnit(std::shared_ptr<Chk::Unit> unit);
        void insertUnit(size_t unitIndex, std::shared_ptr<Chk::Unit> unit);
        void deleteUnit(size_t unitIndex);
        void moveUnit(size_t unitIndexFrom, size_t unitIndexTo);

    protected:
        virtual u32 getSize();
        virtual std::ostream & writeData(std::ostream &s, u32 sizeInBytes);

    private:
        std::deque<std::shared_ptr<Chk::Unit>> units;
};

class IsomSection : public DynamicSection<true>
{
    public:
        static Section GetDefault(u16 tileWidth, u16 tileHeigh);
        IsomSection();

        std::shared_ptr<Chk::IsomEntry> getIsomEntry(size_t isomIndex);
        void setDimensions(u16 newTileWidth, u16 newTileHeight, u16 oldTileWidth, u16 oldTileHeight, s32 leftEdge = 0, s32 topEdge = 0);

    protected:
        virtual u32 getSize();
        virtual std::ostream & writeData(std::ostream &s, u32 sizeInBytes);

    private:
        std::vector<std::shared_ptr<Chk::IsomEntry>> isomEntries;
};

class TileSection : public DynamicSection<true>
{
    public:
        static Section GetDefault(u16 tileWidth, u16 tileHeigh);
        TileSection();

        u16 getTile(size_t tileIndex);
        void setTile(size_t tileIndex, u16 tileValue);
        void setDimensions(u16 newTileWidth, u16 newTileHeight, u16 oldTileWidth, u16 oldTileHeight, s32 leftEdge = 0, s32 topEdge = 0);

    protected:
        virtual u32 getSize();
        virtual std::ostream & writeData(std::ostream &s, u32 sizeInBytes);
};

class Dd2Section : public DynamicSection<true>
{
    public:
        static Section GetDefault();
        Dd2Section();

        size_t numDoodads();
        std::shared_ptr<Chk::Doodad> getDoodad(size_t doodadIndex);
        size_t addDoodad(std::shared_ptr<Chk::Doodad> doodad);
        void insertDoodad(size_t doodadIndex, std::shared_ptr<Chk::Doodad> doodad);
        void deleteDoodad(size_t doodadIndex);
        void moveDoodad(size_t doodadIndexFrom, size_t doodadIndexTo);

    protected:
        virtual u32 getSize();
        virtual std::ostream & writeData(std::ostream &s, u32 sizeInBytes);

    private:
        std::deque<std::shared_ptr<Chk::Doodad>> doodads;
};

class Thg2Section : public DynamicSection<false>
{
    public:
        static Section GetDefault();
        Thg2Section();

        size_t numSprites();
        std::shared_ptr<Chk::Sprite> getSprite(size_t spriteIndex);
        size_t addSprite(std::shared_ptr<Chk::Sprite> sprite);
        void insertSprite(size_t spriteIndex, std::shared_ptr<Chk::Sprite> sprite);
        void deleteSprite(size_t spriteIndex);
        void moveSprite(size_t spriteIndexFrom, size_t spriteIndexTo);

    protected:
        virtual u32 getSize();
        virtual std::ostream & writeData(std::ostream &s, u32 sizeInBytes);

    private:
        std::deque<std::shared_ptr<Chk::Sprite>> sprites;
};

class MaskSection : public DynamicSection<true>
{
    public:
        static Section GetDefault(u16 tileWidth, u16 tileHeigh);
        MaskSection();

        u8 getFog(size_t tileIndex);
        void setFog(size_t tileIndex, u8 fogOfWarPlayers);
        void setDimensions(u16 newTileWidth, u16 newTileHeight, u16 oldTileWidth, u16 oldTileHeight, s32 leftEdge = 0, s32 topEdge = 0);

    protected:
        virtual u32 getSize();
        virtual std::ostream & writeData(std::ostream &s, u32 sizeInBytes);
};

class StrSection : public DynamicSection<false>
{
    public:
        static Section GetDefault();
        StrSection();
        
        bool isSynced();
        void flagUnsynced();
        void syncFromBuffer(StrSynchronizerPtr strSynchronizer);
        void syncToBuffer(StrSynchronizerPtr strSynchronizer);

    protected:
        virtual u32 getSize();
        virtual std::ostream & writeData(std::ostream &s, u32 sizeInBytes);

    private:
        bool synced; // If false, then the list of strings would need to be written to the buffer (using syncToBuffer) for the two to be in sync
};

class UprpSection : public StructSection<Chk::UPRP, false>
{
    public:
        static Section GetDefault();
        UprpSection(Chk::UPRP & data);

        Chk::Cuwp getCuwp(size_t cuwpIndex);
        void setCuwp(size_t cuwpIndex, const Chk::Cuwp &cuwp);
        size_t findCuwp(const Chk::Cuwp &cuwp);
};

class UpusSection : public StructSection<Chk::UPUS, true>
{
    public:
        static Section GetDefault();
        UpusSection(Chk::UPUS & data);

        bool cuwpUsed(size_t cuwpIndex);
        void setCuwpUsed(size_t cuwpIndex, bool cuwpUsed);
        size_t getNextUnusedCuwpIndex();
};

class MrgnSection : public DynamicSection<false>
{
    public:
        static Section GetDefault(u16 tileWidth, u16 tileHeigh);
        MrgnSection();

        size_t numLocations();
        void sizeToScOriginal();
        void sizeToScHybridOrExpansion();
        std::shared_ptr<Chk::Location> getLocation(size_t locationIndex);
        size_t addLocation(std::shared_ptr<Chk::Location> location);
        void insertLocation(size_t locationIndex, std::shared_ptr<Chk::Location> location);
        void deleteLocation(size_t locationIndex);
        void moveLocation(size_t locationIndexFrom, size_t locationIndexTo);
        bool stringUsed(size_t stringId);
        void markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed);
        void remapStringIds(std::map<u32, u32> stringIdRemappings);
        void deleteString(size_t stringId);

    protected:
        virtual u32 getSize();
        virtual std::ostream & writeData(std::ostream &s, u32 sizeInBytes);

    private:
        std::vector<std::shared_ptr<Chk::Location>> locations;
};

class TrigSection : public DynamicSection<false>
{
    public:
        static Section GetDefault();
        TrigSection();

        void ReplaceData(buffer & data); // TODO: Remove me
        bool AddTrigger(Chk::Trigger* trigger); // TODO: Remove me
        std::shared_ptr<Chk::Trigger> getTrigger(u32 trigNum); // TODO: Remove me

        size_t numTriggers();
        std::shared_ptr<Chk::Trigger> getTrigger(size_t triggerIndex);
        size_t addTrigger(std::shared_ptr<Chk::Trigger> trigger);
        void insertTrigger(size_t triggerIndex, std::shared_ptr<Chk::Trigger> trigger);
        void deleteTrigger(size_t triggerIndex);
        void moveTrigger(size_t triggerIndexFrom, size_t triggerIndexTo);
        bool stringUsed(size_t stringId);
        bool gameStringUsed(size_t stringId);
        bool commentStringUsed(size_t stringId);
        void markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed);
        void markUsedGameStrings(std::bitset<Chk::MaxStrings> &stringIdUsed);
        void markUsedCommentStrings(std::bitset<Chk::MaxStrings> &stringIdUsed);
        void remapStringIds(std::map<u32, u32> stringIdRemappings);
        void deleteString(size_t stringId);

    protected:
        virtual u32 getSize();
        virtual std::ostream & writeData(std::ostream &s, u32 sizeInBytes);

    private:
        std::deque<std::shared_ptr<Chk::Trigger>> triggers;
};

class MbrfSection : public DynamicSection<false>
{
    public:
        static Section GetDefault();
        MbrfSection();

        size_t numBriefingTriggers();
        std::shared_ptr<Chk::Trigger> getBriefingTrigger(size_t briefingTriggerIndex);
        size_t addBriefingTrigger(std::shared_ptr<Chk::Trigger> briefingTrigger);
        void insertBriefingTrigger(size_t briefingTriggerIndex, std::shared_ptr<Chk::Trigger> briefingTrigger);
        void deleteBriefingTrigger(size_t briefingTriggerIndex);
        void moveBriefingTrigger(size_t briefingTriggerIndexFrom, size_t briefingTriggerIndexTo);
        bool stringUsed(size_t stringId);
        void markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed);
        void remapStringIds(std::map<u32, u32> stringIdRemappings);
        void deleteString(size_t stringId);

    protected:
        virtual u32 getSize();
        virtual std::ostream & writeData(std::ostream &s, u32 sizeInBytes);

    private:
        std::deque<std::shared_ptr<Chk::Trigger>> briefingTriggers;
};

class SprpSection : public StructSection<Chk::SPRP, false>
{
    public:
        static Section GetDefault();
        SprpSection(Chk::SPRP & data);
        
        size_t getScenarioNameStringId();
        size_t getScenarioDescriptionStringId();
        void setScenarioNameStringId(u16 scenarioNameStringId);
        void setScenarioDescriptionStringId(u16 scenarioDescriptionStringId);
        bool stringUsed(size_t stringId);
        void markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed);
        void remapStringIds(std::map<u32, u32> stringIdRemappings);
        void deleteString(size_t stringId);
};

class ForcSection : public StructSection<Chk::FORC, false>
{
    public:
        static Section GetDefault();
        ForcSection(Chk::FORC & data);

        Chk::Force getPlayerForce(Sc::Player::SlotOwner slotOwner);
        size_t getForceStringId(Chk::Force force);
        u8 getForceFlags(Chk::Force force);
        void setPlayerForce(Sc::Player::SlotOwner slotOwner, Chk::Force force);
        void setForceStringId(Chk::Force force, u16 forceStringId);
        void setForceFlags(Chk::Force force, u8 forceFlags);
        bool stringUsed(size_t stringId);
        void markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed);
        void remapStringIds(std::map<u32, u32> stringIdRemappings);
        void deleteString(size_t stringId);
};

class WavSection : public StructSection<Chk::WAV, true>
{
    public:
        static Section GetDefault();
        WavSection(Chk::WAV & data);

        bool stringIsSound(size_t stringId);
        size_t getSoundStringId(size_t soundIndex);
        void setSoundStringId(size_t soundIndex, size_t soundStringId);
        inline bool stringUsed(size_t stringId);
        void markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed);
        void remapStringIds(std::map<u32, u32> stringIdRemappings);
        void deleteString(size_t stringId);
};

class UnisSection : public StructSection<Chk::UNIS, false>
{
    public:
        static Section GetDefault();
        UnisSection(Chk::UNIS & data);

        bool unitUsesDefaultSettings(Sc::Unit::Type unitType);
        u32 getUnitHitpoints(Sc::Unit::Type unitType);
        u16 getUnitShieldPoints(Sc::Unit::Type unitType);
        u8 getUnitArmorLevel(Sc::Unit::Type unitType);
        u16 getUnitBuildTime(Sc::Unit::Type unitType);
        u16 getUnitMineralCost(Sc::Unit::Type unitType);
        u16 getUnitGasCost(Sc::Unit::Type unitType);
        size_t getUnitNameStringId(Sc::Unit::Type unitType);
        u16 getWeaponBaseDamage(Sc::Weapon::Type weaponType);
        u16 getWeaponUpgradeDamage(Sc::Weapon::Type weaponType);

        void setUnitUsesDefaultSettings(Sc::Unit::Type unitType, bool useDefault);
        void setUnitHitpoints(Sc::Unit::Type unitType, u32 hitpoints);
        void setUnitShieldPoints(Sc::Unit::Type unitType, u16 shieldPoints);
        void setUnitArmorLevel(Sc::Unit::Type unitType, u8 armorLevel);
        void setUnitBuildTime(Sc::Unit::Type unitType, u16 buildTime);
        void setUnitMineralCost(Sc::Unit::Type unitType, u16 mineralCost);
        void setUnitGasCost(Sc::Unit::Type unitType, u16 gasCost);
        void setUnitNameStringId(Sc::Unit::Type unitType, u16 nameStringId);
        void setWeaponBaseDamage(Sc::Weapon::Type weaponType, u16 baseDamage);
        void setWeaponUpgradeDamage(Sc::Weapon::Type weaponType, u16 upgradeDamage);

        bool stringUsed(size_t stringId);
        void markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed);
        void remapStringIds(std::map<u32, u32> stringIdRemappings);
        void deleteString(size_t stringId);
};

class UpgsSection : public StructSection<Chk::UPGS, false>
{
    public:
        static Section GetDefault();
        UpgsSection(Chk::UPGS & data);

        bool upgradeUsesDefaultSettings(Sc::Upgrade::Type upgradeType);
        u16 getBaseMineralCost(Sc::Upgrade::Type upgradeType);
        u16 getMineralCostFactor(Sc::Upgrade::Type upgradeType);
        u16 getBaseGasCost(Sc::Upgrade::Type upgradeType);
        u16 getGasCostFactor(Sc::Upgrade::Type upgradeType);
        u16 getBaseResearchTime(Sc::Upgrade::Type upgradeType);
        u16 getResearchTimeFactor(Sc::Upgrade::Type upgradeType);

        void setUpgradeUsesDefaultSettings(Sc::Upgrade::Type upgradeType, bool useDefault);
        void setBaseMineralCost(Sc::Upgrade::Type upgradeType, u16 baseMineralCost);
        void setMineralCostFactor(Sc::Upgrade::Type upgradeType, u16 mineralCostFactor);
        void setBaseGasCost(Sc::Upgrade::Type upgradeType, u16 baseGasCost);
        void setGasCostFactor(Sc::Upgrade::Type upgradeType, u16 gasCostFactor);
        void setBaseResearchTime(Sc::Upgrade::Type upgradeType, u16 baseResearchTime);
        void setResearchTimeFactor(Sc::Upgrade::Type upgradeType, u16 researchTimeFactor);
};

class TecsSection : public StructSection<Chk::TECS, false>
{
    public:
        static Section GetDefault();
        TecsSection(Chk::TECS & data);

        bool techUsesDefaultSettings(Sc::Tech::Type techType);
        u16 getTechMineralCost(Sc::Tech::Type techType);
        u16 getTechGasCost(Sc::Tech::Type techType);
        u16 getTechResearchTime(Sc::Tech::Type techType);
        u16 getTechEnergyCost(Sc::Tech::Type techType);

        void setTechUsesDefaultSettings(Sc::Tech::Type techType, bool useDefault);
        void setTechMineralCost(Sc::Tech::Type techType, u16 mineralCost);
        void setTechGasCost(Sc::Tech::Type techType, u16 gasCost);
        void setTechResearchTime(Sc::Tech::Type techType, u16 researchTime);
        void setTechEnergyCost(Sc::Tech::Type techType, u16 energyCost);
};

class SwnmSection : public StructSection<Chk::SWNM, true>
{
    public:
        static Section GetDefault();
        SwnmSection(Chk::SWNM & data);

        size_t getSwitchNameStringId(size_t switchIndex);
        void setSwitchNameStringId(size_t switchIndex, size_t stringId);
        bool stringUsed(size_t stringId);
        void markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed);
        void remapStringIds(std::map<u32, u32> stringIdRemappings);
        void deleteString(size_t stringId);
};

class ColrSection : public StructSection<Chk::COLR, false>
{
    public:
        static Section GetDefault();
        ColrSection(Chk::COLR & data);

        Chk::PlayerColor getPlayerColor(Sc::Player::SlotOwner slotOwner);
        void setPlayerColor(Sc::Player::SlotOwner slotOwner, Chk::PlayerColor color);
};

class PupxSection : public StructSection<Chk::PUPx, false>
{
    public:
        static Section GetDefault();
        PupxSection(Chk::PUPx & data);

        size_t getMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex);
        size_t getStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex);
        size_t getDefaultMaxUpgradeLevel(Sc::Upgrade::Type upgradeType);
        size_t getDefaultStartUpgradeLevel(Sc::Upgrade::Type upgradeType);
        bool playerUsesDefault(Sc::Upgrade::Type upgradeType, size_t playerIndex);
        void setMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, size_t maxUpgradeLevel);
        void setStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, size_t startUpgradeLevel);
        void setDefaultMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t maxUpgradeLevel);
        void setDefaultStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t startUpgradeLevel);
        void setPlayerUsesDefault(Sc::Upgrade::Type upgradeType, size_t playerIndex, bool useDefault);
};

class PtexSection : public StructSection<Chk::PTEx, false>
{
    public:
        static Section GetDefault();
        PtexSection(Chk::PTEx & data);

        bool techAvailable(Sc::Tech::Type techType, size_t playerIndex);
        bool techResearched(Sc::Tech::Type techType, size_t playerIndex);
        bool techDefaultAvailable(Sc::Tech::Type techType);
        bool techDefaultResearched(Sc::Tech::Type techType);
        bool playerUsesDefault(Sc::Tech::Type techType, size_t playerIndex);
        void setTechAvailable(Sc::Tech::Type techType, size_t playerIndex, bool available);
        void setTechResearched(Sc::Tech::Type techType, size_t playerIndex, bool researched);
        void setDefaultTechAvailable(Sc::Tech::Type techType, bool available);
        void setDefaultTechResearched(Sc::Tech::Type techType, bool researched);
        void setPlayerUsesDefault(Sc::Tech::Type techType, size_t playerIndex, bool useDefault);
};

class UnixSection : public StructSection<Chk::UNIx, false>
{
    public:
        static Section GetDefault();
        UnixSection(Chk::UNIx & data);

        bool unitUsesDefaultSettings(Sc::Unit::Type unitType);
        u32 getUnitHitpoints(Sc::Unit::Type unitType);
        u16 getUnitShieldPoints(Sc::Unit::Type unitType);
        u8 getUnitArmorLevel(Sc::Unit::Type unitType);
        u16 getUnitBuildTime(Sc::Unit::Type unitType);
        u16 getUnitMineralCost(Sc::Unit::Type unitType);
        u16 getUnitGasCost(Sc::Unit::Type unitType);
        size_t getUnitNameStringId(Sc::Unit::Type unitType);
        u16 getWeaponBaseDamage(Sc::Weapon::Type weaponType);
        u16 getWeaponUpgradeDamage(Sc::Weapon::Type weaponType);

        void setUnitUsesDefaultSettings(Sc::Unit::Type unitType, bool useDefault);
        void setUnitHitpoints(Sc::Unit::Type unitType, u32 hitpoints);
        void setUnitShieldPoints(Sc::Unit::Type unitType, u16 shieldPoints);
        void setUnitArmorLevel(Sc::Unit::Type unitType, u8 armorLevel);
        void setUnitBuildTime(Sc::Unit::Type unitType, u16 buildTime);
        void setUnitMineralCost(Sc::Unit::Type unitType, u16 mineralCost);
        void setUnitGasCost(Sc::Unit::Type unitType, u16 gasCost);
        void setUnitNameStringId(Sc::Unit::Type unitType, u16 nameStringId);
        void setWeaponBaseDamage(Sc::Weapon::Type weaponType, u16 baseDamage);
        void setWeaponUpgradeDamage(Sc::Weapon::Type weaponType, u16 upgradeDamage);

        bool stringUsed(size_t stringId);
        void markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed);
        void remapStringIds(std::map<u32, u32> stringIdRemappings);
        void deleteString(size_t stringId);
};

class UpgxSection : public StructSection<Chk::UPGx, false>
{
    public:
        static Section GetDefault();
        UpgxSection(Chk::UPGx & data);

        bool upgradeUsesDefaultSettings(Sc::Upgrade::Type upgradeType);
        u16 getBaseMineralCost(Sc::Upgrade::Type upgradeType);
        u16 getMineralCostFactor(Sc::Upgrade::Type upgradeType);
        u16 getBaseGasCost(Sc::Upgrade::Type upgradeType);
        u16 getGasCostFactor(Sc::Upgrade::Type upgradeType);
        u16 getBaseResearchTime(Sc::Upgrade::Type upgradeType);
        u16 getResearchTimeFactor(Sc::Upgrade::Type upgradeType);

        void setUpgradeUsesDefaultSettings(Sc::Upgrade::Type upgradeType, bool useDefault);
        void setBaseMineralCost(Sc::Upgrade::Type upgradeType, u16 baseMineralCost);
        void setMineralCostFactor(Sc::Upgrade::Type upgradeType, u16 mineralCostFactor);
        void setBaseGasCost(Sc::Upgrade::Type upgradeType, u16 baseGasCost);
        void setGasCostFactor(Sc::Upgrade::Type upgradeType, u16 gasCostFactor);
        void setBaseResearchTime(Sc::Upgrade::Type upgradeType, u16 baseResearchTime);
        void setResearchTimeFactor(Sc::Upgrade::Type upgradeType, u16 researchTimeFactor);
};

class TecxSection : public StructSection<Chk::TECx, false>
{
    public:
        static Section GetDefault();
        TecxSection(Chk::TECx & data);

        bool techUsesDefaultSettings(Sc::Tech::Type techType);
        u16 getTechMineralCost(Sc::Tech::Type techType);
        u16 getTechGasCost(Sc::Tech::Type techType);
        u16 getTechResearchTime(Sc::Tech::Type techType);
        u16 getTechEnergyCost(Sc::Tech::Type techType);

        void setTechUsesDefaultSettings(Sc::Tech::Type techType, bool useDefault);
        void setTechMineralCost(Sc::Tech::Type techType, u16 mineralCost);
        void setTechGasCost(Sc::Tech::Type techType, u16 gasCost);
        void setTechResearchTime(Sc::Tech::Type techType, u16 researchTime);
        void setTechEnergyCost(Sc::Tech::Type techType, u16 energyCost);
};

class OstrSection : public StructSection<Chk::OSTR, true>
{
    public:
        static Section GetDefault();
        OstrSection(Chk::OSTR & data);

        u32 getVersion();

        u32 getScenarioNameStringId();
        u32 getScenarioDescriptionStringId();
        u32 getForceNameStringId(Chk::Force force);
        u32 getUnitNameStringId(Sc::Unit::Type unitType);
        u32 getExpUnitNameStringId(Sc::Unit::Type unitType);
        u32 getSoundPathStringId(size_t soundIndex);
        u32 getSwitchNameStringId(size_t switchIndex);
        u32 getLocationNameStringId(size_t locationIndex);

        void setScenarioNameStringId(u32 scenarioNameStringId);
        void setScenarioDescriptionStringId(u32 scenarioDescriptionStringId);
        void setForceNameStringId(Chk::Force force, u32 forceNameStringId);
        void setUnitNameStringId(Sc::Unit::Type unitType, u32 unitNameStringId);
        void setExpUnitNameStringId(Sc::Unit::Type unitType, u32 expUnitNameStringId);
        void setSoundPathStringId(size_t soundIndex, u32 soundPathStringId);
        void setSwitchNameStringId(size_t switchIndex, u32 switchNameStringId);
        void setLocationNameStringId(size_t locationIndex, u32 locationNameStringId);

        bool stringUsed(size_t stringId);
        void markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed);
        void remapStringIds(std::map<u32, u32> stringIdRemappings);
        void deleteString(size_t stringId);
};

class KstrSection : public DynamicSection<true>
{
    public:
        static Section GetDefault();
        KstrSection();
        
        bool isSynced();
        void flagUnsynced();
        void syncFromBuffer(StrSynchronizerPtr strSynchronizer);
        void syncToBuffer(StrSynchronizerPtr strSynchronizer);

    protected:
        virtual u32 getSize();
        virtual std::ostream & writeData(std::ostream &s, u32 sizeInBytes);

    private:
        bool synced; // If false, then the list of strings would need to be written to the buffer (using syncToBuffer) for the two to be in sync
};


class Versions;
class Strings;
class Players;
class Terrain;
class Layers;
class Properties;
class Triggers;
typedef std::shared_ptr<Versions> VersionsPtr;
typedef std::shared_ptr<Strings> StringsPtr;
typedef std::shared_ptr<Players> PlayersPtr;
typedef std::shared_ptr<Terrain> TerrainPtr;
typedef std::shared_ptr<Layers> LayersPtr;
typedef std::shared_ptr<Properties> PropertiesPtr;
typedef std::shared_ptr<Triggers> TriggersPtr;


class Versions
{
    public:
        VerSectionPtr ver; // StarCraft version information
        TypeSectionPtr type; // Redundant versioning
        IverSectionPtr iver; // Redundant versioning
        Ive2SectionPtr ive2; // Redundant versioning
        VcodSectionPtr vcod; // Validation
        
        bool is(Chk::Version version);
        bool isOriginal();
        bool isHybrid();
        bool isExpansion();
        bool isHybridOrAbove();
        void changeTo(Chk::Version version);

        bool hasDefaultValidation();
        void setToDefaultValidation();

    private:
        LayersPtr layers; // For updating location capacity as necessary
};

class Strings : StrSynchronizer
{
    public:

        StrSectionPtr str; // StarCraft string data
        KstrSectionPtr kstr; // Editor only string data
        SprpSectionPtr sprp; // Scenario name and description
        OstrSectionPtr ostr; // Overrides for all but trigger and briefing strings

        enum class RescopeFlag : u32 {
            RescopeSwitchNames = BIT_0,
            RescopeComments = BIT_1,
            Rescope
        };

        enum class StringUserType : u8 {
            None = 0,
            ScenarioName,
            ScenarioDescription = ScenarioName+1,
            Force,
            Location,
            OriginalUnitSettings,
            ExpansionUnitSettings,
            Sound,
            Switch,
            Trigger,
            BriefingTrigger
        };

        Strings();

        size_t getCapacity(Chk::Scope storageScope = Chk::Scope::Game);

        bool stringUsed(size_t stringId, Chk::Scope usageScope = Chk::Scope::Either, Chk::Scope storageScope = Chk::Scope::Game, bool ensureStored = false);
        bool stringStored(size_t stringId, Chk::Scope storageScope = Chk::Scope::Either);
        void markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed, Chk::Scope usageScope = Chk::Scope::Either, Chk::Scope storageScope = Chk::Scope::Either);
        void markValidUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed, Chk::Scope usageScope = Chk::Scope::Either, Chk::Scope storageScope = Chk::Scope::Either);
        
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkdString (Editor <01>Style)
        std::shared_ptr<StringType> getString(size_t stringId, Chk::Scope storageScope = Chk::Scope::EditorOverGame); // Gets the string at stringId with formatting based on StringType

        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        size_t findString(const StringType &str, Chk::Scope storageScope = Chk::Scope::Game);

        void setCapacity(size_t stringCapacity, Chk::Scope storageScope = Chk::Scope::Game, bool autoDefragment = true);
        
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        size_t addString(const StringType &str, Chk::Scope storageScope = Chk::Scope::Game, bool autoDefragment = true);
        
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        void replaceString(size_t stringId, const StringType &str, Chk::Scope storageScope = Chk::Scope::Game);

        void deleteString(size_t stringId, Chk::Scope storageScope = Chk::Scope::Both, bool deleteOnlyIfUnused = true);
        void moveString(size_t stringIdFrom, size_t stringIdTo, Chk::Scope storageScope = Chk::Scope::Game);
        size_t rescopeString(size_t stringId, Chk::Scope changeStorageScopeTo = Chk::Scope::Editor, bool autoDefragment = true);

        size_t getScenarioNameStringId(Chk::Scope storageScope = Chk::Scope::Game);
        size_t getScenarioDescriptionStringId(Chk::Scope storageScope = Chk::Scope::Game);
        size_t getForceNameStringId(Chk::Force force, Chk::Scope storageScope = Chk::Scope::Game);
        size_t getUnitNameStringId(Sc::Unit::Type unitType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto, Chk::Scope storageScope = Chk::Scope::Game);
        size_t getSoundPathStringId(size_t soundIndex, Chk::Scope storageScope = Chk::Scope::Game);
        size_t getSwitchNameStringId(size_t switchIndex, Chk::Scope storageScope = Chk::Scope::Game);
        size_t getLocationNameStringId(size_t locationIndex, Chk::Scope storageScope = Chk::Scope::Game);

        void setScenarioNameStringId(size_t scenarioNameStringId, Chk::Scope storageScope = Chk::Scope::Game);
        void setScenarioDescriptionStringId(size_t scenarioDescriptionStringId, Chk::Scope storageScope = Chk::Scope::Game);
        void setForceNameStringId(Chk::Force force, size_t forceNameStringId, Chk::Scope storageScope = Chk::Scope::Game);
        void setUnitNameStringId(Sc::Unit::Type unitType, size_t unitNameStringId, Chk::UseExpSection useExp = Chk::UseExpSection::Auto, Chk::Scope storageScope = Chk::Scope::Game);
        void setSoundPathStringId(size_t soundIndex, size_t soundPathStringId, Chk::Scope storageScope = Chk::Scope::Game);
        void setSwitchNameStringId(size_t switchIndex, size_t switchNameStringId, Chk::Scope storageScope = Chk::Scope::Game);
        void setLocationNameStringId(size_t locationIndex, size_t locationNameStringId, Chk::Scope storageScope = Chk::Scope::Game);
        
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        std::shared_ptr<StringType> getString(size_t gameStringId, size_t editorStringId, Chk::Scope storageScope);
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        std::shared_ptr<StringType> getScenarioName(Chk::Scope storageScope = Chk::Scope::EditorOverGame);
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        std::shared_ptr<StringType> getScenarioDescription(Chk::Scope storageScope = Chk::Scope::EditorOverGame);
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        std::shared_ptr<StringType> getForceName(Chk::Force force, Chk::Scope storageScope = Chk::Scope::EditorOverGame);
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        std::shared_ptr<StringType> getUnitName(Sc::Unit::Type unitType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto, Chk::Scope storageScope = Chk::Scope::EditorOverGame);
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        std::shared_ptr<StringType> getSoundPath(size_t soundIndex, Chk::Scope storageScope = Chk::Scope::EditorOverGame);
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        std::shared_ptr<StringType> getSwitchName(size_t switchIndex, Chk::Scope storageScope = Chk::Scope::EditorOverGame);
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        std::shared_ptr<StringType> getLocationName(size_t locationIndex, Chk::Scope storageScope = Chk::Scope::EditorOverGame);

        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        void setScenarioName(const StringType &scenarioNameString, Chk::Scope storageScope = Chk::Scope::Game, bool autoDefragment = true);
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        void setScenarioDescription(const StringType &scenarioDescription, Chk::Scope storageScope = Chk::Scope::Game, bool autoDefragment = true);
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        void setForceName(Chk::Force force, const StringType &forceName, Chk::Scope storageScope = Chk::Scope::Game, bool autoDefragment = true);
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        void setUnitName(Sc::Unit::Type unitType, const StringType &unitName, Chk::UseExpSection useExp = Chk::UseExpSection::Auto, Chk::Scope storageScope = Chk::Scope::Game, bool autoDefragment = true);
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        void setSoundPath(size_t soundIndex, const StringType &soundPath, Chk::Scope storageScope = Chk::Scope::Game, bool autoDefragment = true);
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        void setSwitchName(size_t switchIndex, const StringType &switchName, Chk::Scope storageScope = Chk::Scope::Game, bool autoDefragment = true);
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        void setLocationName(size_t locationIndex, const StringType &locationName, Chk::Scope storageScope = Chk::Scope::Game, bool autoDefragment = true);

        // Checks whether there's any viable configuration for the STR section given the current allowedCompressionFlags
        // allowedCompressionFlags may be increased as necessary if elevator.elevate() returns true
        bool checkFit(StrCompressionElevatorPtr compressionElevator = StrCompressionElevatorPtr());

        // Creates a viable internal data buffer for the string section using the methods in requestedCompressionFlags
        // If no configuration among requestedCompressionFlags is viable, additional methods through allowedCompressionFlags are added as neccessary
        // allowedCompressionFlags may be increased as neccessary if elevator.elevate() returns true
        virtual void synchronizeToStrBuffer(buffer &rawData, StrCompressionElevatorPtr compressionElevator = StrCompressionElevator::NeverElevate(),
            u32 requestedCompressionFlags = (u32)StrCompressFlag::Unchanged, u32 allowedCompressionFlags = (u32)StrCompressFlag::Unchanged);
        virtual void synchronzieFromStrBuffer(const buffer &rawData);

        virtual void synchronizeToKstrBuffer(buffer &rawData, StrCompressionElevatorPtr compressionElevator = StrCompressionElevator::NeverElevate(),
            u32 requestedCompressionFlags = (u32)StrCompressFlag::Unchanged, u32 allowedCompressionFlags = (u32)StrCompressFlag::Unchanged);
        virtual void synchronzieFromKstrBuffer(const buffer &rawData);

    private:
        Versions versions; // For auto-determining the section for regular or expansion units
        PlayersPtr players; // For finding force string usage
        LayersPtr layers; // For finding location string usage
        PropertiesPtr properties; // For finding unit name string usage
        TriggersPtr triggers; // For finding trigger and briefing string usage

        static const std::vector<u32> compressionFlagsProgression;

        std::deque<ScStrPtr> strings;
        std::deque<ScStrPtr> kStrings;
        
        inline void loadString(std::deque<ScStrPtr> &stringContainer, const buffer &rawData, const u16 &stringOffset, const size_t &sectionSize);
        size_t getNextUnusedStringId(std::bitset<Chk::MaxStrings> &stringIdUsed, Chk::Scope storageScope = Chk::Scope::Game, bool checkBeyondScopedCapacity = true);
        void resolveParantage();
        void resolveParantage(ScStrPtr string);
        bool defragmentStr(bool matchCapacityToUsage = true); // Returns true if any fragmented strings are packed
        bool defragmentKstr(bool matchCapacityToUsage = true); // Returns true if any fragmented strings are packed
        void replaceStringId(size_t oldStringId, size_t newStringId);
        void remapStringIds(std::map<u32, u32> stringIdRemappings, Chk::Scope storageScope);
        
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        std::shared_ptr<StringType> convert(ScStrPtr string);
};

class Players
{
    public:
        OwnrSectionPtr ownr; // Slot owners
        IownSectionPtr iown; // Redundant slot owners
        SideSectionPtr side; // Races
        ColrSectionPtr colr; // Player colors
        ForcSectionPtr forc; // Forces

        Sc::Player::SlotOwner getSlotOwner(size_t slotIndex, Chk::Scope scope = Chk::Scope::Game);
        void setSlotOwner(size_t slotIndex, Sc::Player::SlotOwner owner, Chk::Scope scope = Chk::Scope::Both);

        Chk::Race getPlayerRace(size_t playerIndex);
        void setPlayerRace(size_t playerIndex, Chk::Race race);

        Chk::PlayerColor getPlayerColor(Sc::Player::SlotOwner slotOwner);
        void setPlayerColor(Sc::Player::SlotOwner slotOwner, Chk::PlayerColor color);

        Chk::Force getPlayerForce(Sc::Player::SlotOwner slotOwner);
        size_t getForceStringId(Chk::Force force);
        u8 getForceFlags(Chk::Force force);
        void setPlayerForce(Sc::Player::SlotOwner slotOwner, Chk::Force force);
        void setForceStringId(Chk::Force force, u16 forceStringId);
        void setForceFlags(Chk::Force force, u8 forceFlags);
        bool stringUsed(size_t stringId);
        void markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed);
        void remapStringIds(std::map<u32, u32> stringIdRemappings);
        void deleteString(size_t stringId);

    private:
        StringsPtr strings; // For reading and updating force strings
};

class Terrain
{
    public:
        EraSectionPtr era; // Tileset
        DimSectionPtr dim; // Dimensions
        MtxmSectionPtr mtxm; // Real terrain data
        TileSectionPtr tile; // Intermediate terrain data
        IsomSectionPtr isom; // Isometric terrain data

        Sc::Terrain::Tileset getTileset();
        void setTileset(Sc::Terrain::Tileset tileset);

        size_t getTileWidth();
        size_t getTileHeight();
        size_t getPixelWidth();
        size_t getPixelHeight();
        virtual void setTileWidth(u16 newTileWidth, s32 leftEdge = 0);
        virtual void setTileHeight(u16 newTileHeight, s32 topEdge = 0);
        virtual void setDimensions(u16 newTileWidth, u16 newTileHeight, s32 leftEdge = 0, s32 topEdge = 0);

        u16 getTile(size_t tileXc, size_t tileYc, Chk::Scope scope = Chk::Scope::Game);
		inline u16 getTilePx(size_t pixelXc, size_t pixelYc, Chk::Scope scope = Chk::Scope::Game);
		void setTile(size_t tileXc, size_t tileYc, u16 tileValue, Chk::Scope scope = Chk::Scope::Both);
		inline void setTilePx(size_t pixelXc, size_t pixelYc, u16 tileValue, Chk::Scope scope = Chk::Scope::Both);
        
        std::shared_ptr<Chk::IsomEntry> getIsomEntry(size_t isomIndex);
};

class Layers : public Terrain
{
    public:
        MaskSectionPtr mask; // Fog of war
        Thg2SectionPtr thg2; // Sprites
        Dd2SectionPtr dd2; // Doodads
        UnitSectionPtr unit; // Units
        MrgnSectionPtr mrgn; // Locations

        enum class SizeValidationFlag : u16
        {
            UpdateAnywhere = BIT_0,
            UpdateAnywhereIfAlreadyStandard = BIT_1,
            UpdateOutOfBoundsLocations = BIT_2, // Not standard, locations larger than the map are useful
            RemoveOutOfBoundsDoodads = BIT_3,
            UpdateOutOfBoundsUnits = BIT_4,
            RemoveOutOfBoundsUnits = BIT_5,
            UpdateOutOfBoundsSprites = BIT_6,
            RemoveOutOfBoundsSprites = BIT_7,
            Default = UpdateAnywhereIfAlreadyStandard | RemoveOutOfBoundsDoodads | UpdateOutOfBoundsUnits | UpdateOutOfBoundsSprites
        };

        virtual void setTileWidth(u16 tileWidth, u16 sizeValidationFlags = (u16)SizeValidationFlag::Default, s32 leftEdge = 0);
        virtual void setTileHeight(u16 tileHeight, u16 sizeValidationFlags = (u16)SizeValidationFlag::Default, s32 topEdge = 0);
        virtual void setDimensions(u16 tileWidth, u16 tileHeight, u16 sizeValidationFlags = (u16)SizeValidationFlag::Default, s32 leftEdge = 0, s32 topEdge = 0);
        void validateSizes(u16 sizeValidationFlags);

        u8 getFog(size_t tileXc, size_t tileYc);
        inline u8 getFogPx(size_t pixelXc, size_t pixelYc);
        void setFog(size_t tileXc, size_t tileYc, u8 fogOfWarPlayers);
        inline void setFogPx(size_t pixelXc, size_t pixelYc, u8 fogOfWarPlayers);

        size_t numSprites();
        std::shared_ptr<Chk::Sprite> getSprite(size_t spriteIndex);
        size_t addSprite(std::shared_ptr<Chk::Sprite> sprite);
        void insertSprite(size_t spriteIndex, std::shared_ptr<Chk::Sprite> sprite);
        void deleteSprite(size_t spriteIndex);
        void moveSprite(size_t spriteIndexFrom, size_t spriteIndexTo);
        void updateOutOfBoundsSprites();
        void removeOutOfBoundsSprites();

        size_t numDoodads();
        std::shared_ptr<Chk::Doodad> getDoodad(size_t doodadIndex);
        size_t addDoodad(std::shared_ptr<Chk::Doodad> doodad);
        void insertDoodad(size_t doodadIndex, std::shared_ptr<Chk::Doodad> doodad);
        void deleteDoodad(size_t doodadIndex);
        void moveDoodad(size_t doodadIndexFrom, size_t doodadIndexTo);
        void removeOutOfBoundsDoodads();

        size_t numUnits();
        std::shared_ptr<Chk::Unit> getUnit(size_t unitIndex);
        size_t addUnit(std::shared_ptr<Chk::Unit> unit);
        void insertUnit(size_t unitIndex, std::shared_ptr<Chk::Unit> unit);
        void deleteUnit(size_t unitIndex);
        void moveUnit(size_t unitIndexFrom, size_t unitIndexTo);
        void updateOutOfBoundsUnits();
        void removeOutOfBoundsUnits();
        
        size_t numLocations();
        void sizeLocationsToScOriginal();
        void sizeLocationsToScHybridOrExpansion();
        std::shared_ptr<Chk::Location> getLocation(size_t locationIndex);
        size_t addLocation(std::shared_ptr<Chk::Location> location);
        void insertLocation(size_t locationIndex, std::shared_ptr<Chk::Location> location);
        void deleteLocation(size_t locationIndex);
        void moveLocation(size_t locationIndexFrom, size_t locationIndexTo);
        void downsizeOutOfBoundsLocations();
        
        bool anywhereIsStandardDimensions();
        void matchAnywhereToDimensions();

        bool stringUsed(size_t stringId);
        void markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed);
        void remapStringIds(std::map<u32, u32> stringIdRemappings);
        void deleteString(size_t stringId);

    private:
        StringsPtr strings; // For reading and updating location names
};

class Properties
{
    public:
        UnisSectionPtr unis; // Unit settings
        UnixSectionPtr unix; // Expansion Unit Settings
        PuniSectionPtr puni; // Unit availability
        UpgsSectionPtr upgs; // Upgrade costs
        UpgxSectionPtr upgx; // Expansion upgrade costs
        UpgrSectionPtr upgr; // Upgrade leveling
        PupxSectionPtr pupx; // Expansion upgrade leveling
        TecsSectionPtr tecs; // Technology costs
        TecxSectionPtr tecx; // Expansion technology costs
        PtecSectionPtr ptec; // Technology availability
        PtexSectionPtr ptex; // Expansion technology availability

        bool useExpansionUnitSettings(Chk::UseExpSection useExp);
        bool unitUsesDefaultSettings(Sc::Unit::Type unitType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        u32 getUnitHitpoints(Sc::Unit::Type unitType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        u16 getUnitShieldPoints(Sc::Unit::Type unitType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        u8 getUnitArmorLevel(Sc::Unit::Type unitType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        u16 getUnitBuildTime(Sc::Unit::Type unitType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        u16 getUnitMineralCost(Sc::Unit::Type unitType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        u16 getUnitGasCost(Sc::Unit::Type unitType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        size_t getUnitNameStringId(Sc::Unit::Type unitType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        u16 getWeaponBaseDamage(Sc::Weapon::Type weaponType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        u16 getWeaponUpgradeDamage(Sc::Weapon::Type weaponType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);

        void setUnitUsesDefaultSettings(Sc::Unit::Type unitType, bool useDefault, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setUnitHitpoints(Sc::Unit::Type unitType, u32 hitpoints, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setUnitShieldPoints(Sc::Unit::Type unitType, u16 shieldPoints, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setUnitArmorLevel(Sc::Unit::Type unitType, u8 armorLevel, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setUnitBuildTime(Sc::Unit::Type unitType, u16 buildTime, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setUnitMineralCost(Sc::Unit::Type unitType, u16 mineralCost, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setUnitGasCost(Sc::Unit::Type unitType, u16 gasCost, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setUnitNameStringId(Sc::Unit::Type unitType, size_t nameStringId, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setWeaponBaseDamage(Sc::Weapon::Type weaponType, u16 baseDamage, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setWeaponUpgradeDamage(Sc::Weapon::Type weaponType, u16 upgradeDamage, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);

        bool isUnitBuildable(Sc::Unit::Type unitType, size_t playerIndex);
        bool isUnitDefaultBuildable(Sc::Unit::Type unitType);
        bool playerUsesDefault(Sc::Unit::Type unitType, size_t playerIndex);
        void setUnitBuildable(Sc::Unit::Type unitType, size_t playerIndex, bool buildable);
        void setUnitDefaultBuildable(Sc::Unit::Type unitType, bool buildable);
        void setPlayerUsesDefault(Sc::Unit::Type unitType, size_t playerIndex, bool useDefault);
        
        bool useExpansionUpgradeCosts(Chk::UseExpSection useExp);
        bool upgradeUsesDefaultSettings(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        u16 getBaseMineralCost(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        u16 getMineralCostFactor(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        u16 getBaseGasCost(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        u16 getGasCostFactor(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        u16 getBaseResearchTime(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        u16 getResearchTimeFactor(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);

        void setUpgradeUsesDefaultSettings(Sc::Upgrade::Type upgradeType, bool useDefault, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setBaseMineralCost(Sc::Upgrade::Type upgradeType, u16 baseMineralCost, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setMineralCostFactor(Sc::Upgrade::Type upgradeType, u16 mineralCostFactor, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setBaseGasCost(Sc::Upgrade::Type upgradeType, u16 baseGasCost, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setGasCostFactor(Sc::Upgrade::Type upgradeType, u16 gasCostFactor, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setBaseResearchTime(Sc::Upgrade::Type upgradeType, u16 baseResearchTime, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setResearchTimeFactor(Sc::Upgrade::Type upgradeType, u16 researchTimeFactor, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);

        bool useExpansionUpgradeLeveling(Chk::UseExpSection useExp);
        size_t getMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        size_t getStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        size_t getDefaultMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        size_t getDefaultStartUpgradeLevel(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        bool playerUsesDefault(Sc::Upgrade::Type upgradeType, size_t playerIndex, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, size_t maxUpgradeLevel, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, size_t startUpgradeLevel, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setDefaultMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t maxUpgradeLevel, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setDefaultStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t startUpgradeLevel, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setPlayerUsesDefault(Sc::Upgrade::Type upgradeType, size_t playerIndex, bool useDefault, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        
        bool useExpansionTechCosts(Chk::UseExpSection useExp);
        bool techUsesDefaultSettings(Sc::Tech::Type techType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        u16 getTechMineralCost(Sc::Tech::Type techType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        u16 getTechGasCost(Sc::Tech::Type techType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        u16 getTechResearchTime(Sc::Tech::Type techType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        u16 getTechEnergyCost(Sc::Tech::Type techType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);

        void setTechUsesDefaultSettings(Sc::Tech::Type techType, bool useDefault, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setTechMineralCost(Sc::Tech::Type techType, u16 mineralCost, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setTechGasCost(Sc::Tech::Type techType, u16 gasCost, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setTechResearchTime(Sc::Tech::Type techType, u16 researchTime, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setTechEnergyCost(Sc::Tech::Type techType, u16 energyCost, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        
        bool useExpansionTechAvailability(Chk::UseExpSection useExp);
        bool techAvailable(Sc::Tech::Type techType, size_t playerIndex, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        bool techResearched(Sc::Tech::Type techType, size_t playerIndex, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        bool techDefaultAvailable(Sc::Tech::Type techType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        bool techDefaultResearched(Sc::Tech::Type techType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        bool playerUsesDefault(Sc::Tech::Type techType, size_t playerIndex, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setTechAvailable(Sc::Tech::Type techType, size_t playerIndex, bool available, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setTechResearched(Sc::Tech::Type techType, size_t playerIndex, bool researched, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setDefaultTechAvailable(Sc::Tech::Type techType, bool available, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setDefaultTechResearched(Sc::Tech::Type techType, bool researched, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setPlayerUsesDefault(Sc::Tech::Type techType, size_t playerIndex, bool useDefault, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);

        bool stringUsed(size_t stringId);
        void markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed);
        void remapStringIds(std::map<u32, u32> stringIdRemappings);
        void deleteString(size_t stringId);

    private:
        Versions versions; // For auto-determining the section for regular or expansion units
        StringsPtr strings; // For reading and updating unit names
};

class Triggers
{
    public:
        UprpSectionPtr uprp; // CUWP - Create unit with properties properties
        UpusSectionPtr upus; // CUWP usage
        TrigSectionPtr trig; // Triggers
        MbrfSectionPtr mbrf; // Mission briefing triggers
        SwnmSectionPtr swnm; // Switch names
        WavSectionPtr wav; // Sound names

        Chk::Cuwp getCuwp(size_t cuwpIndex);
        void setCuwp(size_t cuwpIndex, const Chk::Cuwp &cuwp);
        size_t addCuwp(const Chk::Cuwp &cuwp, bool fixUsageBeforeAdding = true);
        
        void fixCuwpUsage();
        bool cuwpUsed(size_t cuwpIndex);
        void setCuwpUsed(size_t cuwpIndex, bool cuwpUsed);

        void ReplaceData(buffer & data); // TODO: Remove me
        bool AddTrigger(Chk::Trigger* trigger); // TODO: Remove me
        std::shared_ptr<Chk::Trigger> getTrigger(u32 trigNum); // TODO: Remove me

        size_t numTriggers();
        std::shared_ptr<Chk::Trigger> getTrigger(size_t triggerIndex);
        size_t addTrigger(std::shared_ptr<Chk::Trigger> trigger);
        void insertTrigger(size_t triggerIndex, std::shared_ptr<Chk::Trigger> trigger);
        void deleteTrigger(size_t triggerIndex);
        void moveTrigger(size_t triggerIndexFrom, size_t triggerIndexTo);

        size_t numBriefingTriggers();
        std::shared_ptr<Chk::Trigger> getBriefingTrigger(size_t briefingTriggerIndex);
        size_t addBriefingTrigger(std::shared_ptr<Chk::Trigger> briefingTrigger);
        void insertBriefingTrigger(size_t briefingTriggerIndex, std::shared_ptr<Chk::Trigger> briefingTrigger);
        void deleteBriefingTrigger(size_t briefingTriggerIndex);
        void moveBriefingTrigger(size_t briefingTriggerIndexFrom, size_t briefingTriggerIndexTo);

        size_t getSwitchNameStringId(size_t switchIndex);
        void setSwitchNameStringId(size_t switchIndex, size_t stringId);

        bool stringIsSound(size_t stringId);
        size_t getSoundStringId(size_t soundIndex);
        void setSoundStringId(size_t soundIndex, size_t soundStringId);
        bool stringUsed(size_t stringId);
        bool gameStringUsed(size_t stringId);
        bool editorStringUsed(size_t stringId);
        void markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed);
        void markUsedGameStrings(std::bitset<Chk::MaxStrings> &stringIdUsed);
        void markUsedEditorStrings(std::bitset<Chk::MaxStrings> &stringIdUsed);
        void remapStringIds(std::map<u32, u32> stringIdRemappings);
        void deleteString(size_t stringId);

    private:
        StringsPtr strings; // For reading and updating sound paths, next scenario paths, text messages, leader board text, comments, and switch names
        LayersPtr layers; // For reading locations
};

#endif