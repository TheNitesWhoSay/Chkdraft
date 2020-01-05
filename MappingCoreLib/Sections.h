#ifndef SECTIONS_H
#define SECTIONS_H
#include "Buffer.h"
#include "EscapeStrings.h"
#include "Chk.h"
#include <memory>
#include <string>
#include <deque>
#include <bitset>
#include <vector>

/*
    This file defines sections which encapsulate the storage structures defined in the Chk (see Chk.h)

    The individual sections must be split into two sets of structures/classes rather than using one combined class due to a couple factors...
        - To use the binary form of a structure/class and the sizeof operator for reading/writing you cannot have it extend a base class,
          extending a class adds a hidden base class pointer (and possibly more), increasing the size by an indeterminate amount
        - Sections need to extend a common base class "ChkSection" to be efficiently stored in a container and share common behavior
        - Not all sections have a fixed structure, and many that have a fixed structure are not validated so the editor may need to correct for that,
          such structures need specialized behavior (see "StructSection" and "DynamicSection", and the virtualizable property)
    
    Data notes:
        - All sections are backed by a buffer "rawData", though most dynamic sections will create their own storage and only sync from/to the buffer upon request
        - All struct sections have a structure pointer "data" for their respective Chk structure, this points to the same data in "rawData" which is guarenteed to be at least the size of the default Chk structure
        - APIs are added at this level to read and manipulate each sections individual data (though the structure pointers and buffers are also usable), as well as to generate the default version of a section

    Any unpacked versions containing the same data as the storage structures is also defined here
*/

class ChkSection;
template <typename StructType, bool virtualizable> class StructSection;
template <bool virtualizable> class DynamicSection;
using Section = std::shared_ptr<ChkSection>;



class TypeSection; class VerSection; class IverSection; class Ive2Section; class VcodSection; class IownSection;
class OwnrSection; class EraSection; class DimSection; class SideSection; class MtxmSection; class PuniSection;
class UpgrSection; class PtecSection; class UnitSection; class IsomSection; class TileSection; class Dd2Section;
class Thg2Section; class MaskSection; class StrSection; class UprpSection; class UpusSection; class MrgnSection;
class TrigSection; class MbrfSection; class SprpSection; class ForcSection; class WavSection; class UnisSection;
class UpgsSection; class TecsSection; class SwnmSection; class ColrSection; class PupxSection; class PtexSection;
class UnixSection; class UpgxSection; class TecxSection; class OstrSection; class KstrSection;
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



class StrProp;
class ScStr;
struct zzStringTableNode;
class StrCompressionElevator;
class StringException;
class MaximumStringsExceeded;
class InsufficientStringCapacity;
class MaximumCharactersExceeded;
class MaximumOffsetAndCharsExceeded;
class StrSynchronizer;
using StrCompressionElevatorPtr = std::shared_ptr<StrCompressionElevator>;
using StrSynchronizerPtr = std::shared_ptr<StrSynchronizer>;
using ScStrPtr = std::shared_ptr<ScStr>;




enum_t(SectionName, u32, { // The section name values, as they appear in the binary scenario file
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
});

enum_t(SectionIndex, u32, { // The index at which a section appears in the default scenario file (plus indexes for extended sections), this is not related to section names
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

    OSTR = 39,
    KSTR = 40,

    UNKNOWN = u32_max
});

enum class LoadBehavior
{
    Standard, // The last instance of a section is used
    Override, // The first instance of the section has part or all of its data overridden by subsequent instances
    Append // Subsequent instances of the section will be appended to the first instance
};

class ChkSection
{
    public:
        static constexpr u32 TotalKnownChkSections = 40;
        static constexpr u32 MaxChkSectionSize = s32_max;
        
        ChkSection(SectionName sectionName, bool virtualizable = false, bool dataIsVirtual = false);
        template <typename StructType>
        ChkSection(SectionName sectionName, const StructType & data, bool virtualizable = false, bool dataIsVirtual = false);
        virtual ~ChkSection() { }

        virtual void Validate(bool hybridOrBroodWar) { } // throws SectionValidationException
        SectionIndex getIndex() { return sectionIndex; }
        SectionName getName() { return sectionName; }
        template<typename t> t getNameValue() { return (t)sectionName; }
        std::ostream & write(std::ostream &s); // Writes the section name as a u32, then the result of getSize as a u32, then calls writeData
        virtual u32 getSize(); // Gets the size of the data that can be written to an output stream, or throws MaxSectionSizeExceeded if size would be over u32_max

    protected:
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
        static SectionName getName(SectionIndex sectionIndex) { return sectionNames[sectionIndex]; }
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

    private:
        ChkSection(); // Disallow ctor

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
template <typename StructType, bool Virtualizable>
class StructSection : public ChkSection
{
    public:
        StructSection(SectionName sectionName) : ChkSection(sectionName, Virtualizable), data(nullptr) { }
        StructSection(SectionName sectionName, const StructType & data) : ChkSection(sectionName, data, Virtualizable),
            data(ChkSection::rawData->getPtr<StructType>()) { }
        virtual ~StructSection() { }
        StructType & asStruct() { return *data; }

    protected:
        virtual u32 getSize() {
            if ( isVirtual() ) {
                if ( rawData->size() <= MaxChkSectionSize ) return (u32)rawData->size();
                else throw MaxSectionSizeExceeded(getName(), std::to_string(rawData->size()));
            } else return sizeof(StructType);
        }
        virtual std::ostream & writeData(std::ostream &s, u32 sizeInBytes) {
            if ( isVirtual() ) { rawData->write(s, false); } else { s.write(reinterpret_cast<const char*>(data), sizeof(StructType)); } return s; }

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



class TypeSection : public StructSection<Chk::TYPE, true>
{
    public:
        static TypeSectionPtr GetDefault(Chk::Version version = Chk::Version::StarCraft_Hybrid);
        TypeSection(const Chk::TYPE & data);

        Chk::Type getType();
        void setType(Chk::Type type);
};

class VerSection : public StructSection<Chk::VER, false>
{
    public:
        static VerSectionPtr GetDefault();
        VerSection(const Chk::VER & data);

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
        static IverSectionPtr GetDefault();
        IverSection(const Chk::IVER & data);

        Chk::IVersion getVersion();
        void setVersion(Chk::IVersion version);
};

class Ive2Section : public StructSection<Chk::IVE2, true>
{
    public:
        static Ive2SectionPtr GetDefault();
        Ive2Section(const Chk::IVE2 & data);

        Chk::I2Version getVersion();
        void setVersion(Chk::I2Version version);
};

class VcodSection : public StructSection<Chk::VCOD, false>
{
    public:
        static VcodSectionPtr GetDefault();
        VcodSection(const Chk::VCOD & data);

        bool isDefault();
        void setToDefault();
};

class IownSection : public StructSection<Chk::IOWN, true>
{
    public:
        static IownSectionPtr GetDefault();
        IownSection(const Chk::IOWN & data);

        Sc::Player::SlotType getSlotType(size_t slotIndex);
        void setSlotType(size_t slotIndex, Sc::Player::SlotType slotType);
};

class OwnrSection : public StructSection<Chk::OWNR, false>
{
    public:
        static OwnrSectionPtr GetDefault();
        OwnrSection(const Chk::OWNR & data);
        
        Sc::Player::SlotType getSlotType(size_t slotIndex);
        void setSlotType(size_t slotIndex, Sc::Player::SlotType slotType);
};

class EraSection : public StructSection<Chk::ERA, false>
{
    public:
        static EraSectionPtr GetDefault(Sc::Terrain::Tileset tileset);
        EraSection(const Chk::ERA & data);

        Sc::Terrain::Tileset getTileset();
        void setTileset(Sc::Terrain::Tileset tileset);
};

class DimSection : public StructSection<Chk::DIM, false>
{
    public:
        static DimSectionPtr GetDefault(u16 tileWidth, u16 tileHeigh);
        DimSection(const Chk::DIM & data);

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
        static SideSectionPtr GetDefault();
        SideSection(const Chk::SIDE & data);

        Chk::Race getPlayerRace(size_t playerIndex);
        void setPlayerRace(size_t playerIndex, Chk::Race race);
};

class MtxmSection : public DynamicSection<false>
{
    public:
        static MtxmSectionPtr GetDefault(u16 tileWidth, u16 tileHeigh);
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
        static PuniSectionPtr GetDefault();
        PuniSection(const Chk::PUNI & data);

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
        static UpgrSectionPtr GetDefault();
        UpgrSection(const Chk::UPGR & data);

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
        static PtecSectionPtr GetDefault();
        PtecSection(const Chk::PTEC & data);

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
        static UnitSectionPtr GetDefault();
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
        static IsomSectionPtr GetDefault(u16 tileWidth, u16 tileHeigh);
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
        static TileSectionPtr GetDefault(u16 tileWidth, u16 tileHeigh);
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
        static Dd2SectionPtr GetDefault();
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
        static Thg2SectionPtr GetDefault();
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
        static MaskSectionPtr GetDefault(u16 tileWidth, u16 tileHeigh);
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
        static StrSectionPtr GetDefault();
        StrSection();
        
        bool isSynced();
        void flagUnsynced();
        void syncFromBuffer(StrSynchronizer & strSynchronizer);
        void syncToBuffer(StrSynchronizer & strSynchronizer);

    protected:
        virtual u32 getSize();
        virtual std::ostream & writeData(std::ostream &s, u32 sizeInBytes);

    private:
        bool synced; // If false, then the list of strings would need to be written to the buffer (using syncToBuffer) for the two to be in sync
};

class UprpSection : public StructSection<Chk::UPRP, false>
{
    public:
        static UprpSectionPtr GetDefault();
        UprpSection(const Chk::UPRP & data);

        Chk::Cuwp getCuwp(size_t cuwpIndex);
        void setCuwp(size_t cuwpIndex, const Chk::Cuwp &cuwp);
        size_t findCuwp(const Chk::Cuwp &cuwp);
};

class UpusSection : public StructSection<Chk::UPUS, true>
{
    public:
        static UpusSectionPtr GetDefault();
        UpusSection(const Chk::UPUS & data);

        bool cuwpUsed(size_t cuwpIndex);
        void setCuwpUsed(size_t cuwpIndex, bool cuwpUsed);
        size_t getNextUnusedCuwpIndex();
};

class MrgnSection : public DynamicSection<false>
{
    public:
        static MrgnSectionPtr GetDefault(u16 tileWidth, u16 tileHeigh);
        MrgnSection();

        size_t numLocations();
        void sizeToScOriginal();
        void sizeToScHybridOrExpansion();
        std::shared_ptr<Chk::Location> getLocation(size_t locationIndex);
        size_t addLocation(std::shared_ptr<Chk::Location> location);
        void insertLocation(size_t locationIndex, std::shared_ptr<Chk::Location> location);
        void deleteLocation(size_t locationIndex);
        void moveLocation(size_t locationIndexFrom, size_t locationIndexTo);
        bool isBlank(size_t locationIndex);

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
        static TrigSectionPtr GetDefault();
        TrigSection();

        size_t numTriggers();
        std::shared_ptr<Chk::Trigger> getTrigger(size_t triggerIndex);
        size_t addTrigger(std::shared_ptr<Chk::Trigger> trigger);
        void insertTrigger(size_t triggerIndex, std::shared_ptr<Chk::Trigger> trigger);
        void deleteTrigger(size_t triggerIndex);
        void moveTrigger(size_t triggerIndexFrom, size_t triggerIndexTo);
        void swap(std::deque<std::shared_ptr<Chk::Trigger>> & triggers);

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
        static MbrfSectionPtr GetDefault();
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
        static SprpSectionPtr GetDefault(u16 scenarioNameStringId = 1, u16 scenarioDescriptionStringId = 2);
        SprpSection(const Chk::SPRP & data);
        
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
        static ForcSectionPtr GetDefault();
        ForcSection(const Chk::FORC & data);

        Chk::Force getPlayerForce(size_t slotIndex);
        size_t getForceStringId(Chk::Force force);
        u8 getForceFlags(Chk::Force force);
        void setPlayerForce(size_t slotIndex, Chk::Force force);
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
        static WavSectionPtr GetDefault();
        WavSection(const Chk::WAV & data);

        size_t addSound(size_t stringId);
        bool stringIsSound(size_t stringId);
        size_t getSoundStringId(size_t soundIndex);
        void setSoundStringId(size_t soundIndex, size_t soundStringId);

        bool stringUsed(size_t stringId);
        void markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed);
        void remapStringIds(std::map<u32, u32> stringIdRemappings);
        void deleteString(size_t stringId);
};

class UnisSection : public StructSection<Chk::UNIS, false>
{
    public:
        static UnisSectionPtr GetDefault();
        UnisSection(const Chk::UNIS & data);

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
        static UpgsSectionPtr GetDefault();
        UpgsSection(const Chk::UPGS & data);

        bool upgradeUsesDefaultCosts(Sc::Upgrade::Type upgradeType);
        u16 getBaseMineralCost(Sc::Upgrade::Type upgradeType);
        u16 getMineralCostFactor(Sc::Upgrade::Type upgradeType);
        u16 getBaseGasCost(Sc::Upgrade::Type upgradeType);
        u16 getGasCostFactor(Sc::Upgrade::Type upgradeType);
        u16 getBaseResearchTime(Sc::Upgrade::Type upgradeType);
        u16 getResearchTimeFactor(Sc::Upgrade::Type upgradeType);

        void setUpgradeUsesDefaultCosts(Sc::Upgrade::Type upgradeType, bool useDefault);
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
        static TecsSectionPtr GetDefault();
        TecsSection(const Chk::TECS & data);

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
        static SwnmSectionPtr GetDefault();
        SwnmSection(const Chk::SWNM & data);

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
        static ColrSectionPtr GetDefault();
        ColrSection(const Chk::COLR & data);

        Chk::PlayerColor getPlayerColor(size_t slotIndex);
        void setPlayerColor(size_t slotIndex, Chk::PlayerColor color);
};

class PupxSection : public StructSection<Chk::PUPx, false>
{
    public:
        static PupxSectionPtr GetDefault();
        PupxSection(const Chk::PUPx & data);

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
        static PtexSectionPtr GetDefault();
        PtexSection(const Chk::PTEx & data);

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
        static UnixSectionPtr GetDefault();
        UnixSection(const Chk::UNIx & data);

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
        static UpgxSectionPtr GetDefault();
        UpgxSection(const Chk::UPGx & data);

        bool upgradeUsesDefaultCosts(Sc::Upgrade::Type upgradeType);
        u16 getBaseMineralCost(Sc::Upgrade::Type upgradeType);
        u16 getMineralCostFactor(Sc::Upgrade::Type upgradeType);
        u16 getBaseGasCost(Sc::Upgrade::Type upgradeType);
        u16 getGasCostFactor(Sc::Upgrade::Type upgradeType);
        u16 getBaseResearchTime(Sc::Upgrade::Type upgradeType);
        u16 getResearchTimeFactor(Sc::Upgrade::Type upgradeType);

        void setUpgradeUsesDefaultCosts(Sc::Upgrade::Type upgradeType, bool useDefault);
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
        static TecxSectionPtr GetDefault();
        TecxSection(const Chk::TECx & data);

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
        static OstrSectionPtr GetDefault();
        OstrSection(const Chk::OSTR & data);

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
        static KstrSectionPtr GetDefault();
        KstrSection();
        
        bool isSynced();
        void flagUnsynced();
        void syncFromBuffer(StrSynchronizer & strSynchronizer);
        void syncToBuffer(StrSynchronizer & strSynchronizer);

    protected:
        virtual u32 getSize();
        virtual std::ostream & writeData(std::ostream &s, u32 sizeInBytes);

    private:
        bool synced; // If false, then the list of strings would need to be written to the buffer (using syncToBuffer) for the two to be in sync
};



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
        std::vector<char> allocation; // If parentStr is null, then this is the actual string data and str points to the first character
                                      // else str points to first character of this string within the allocation of the highest-order parent
        StrProp strProp; // Additional color and font details, if this string is extended and gets stored

        static void adopt(ScStrPtr parent, ScStrPtr child, size_t parentLength, size_t childLength, const char* parentSubString);
};

struct zzStringTableNode {
    ScStrPtr scStr;
    u32 stringId;
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
enum_t(StrCompressFlag, u32, {
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
});

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
            u32 requestedCompressionFlags = StrCompressFlag::Unchanged, u32 allowedCompressionFlags = StrCompressFlag::Unchanged) = 0;
        virtual void synchronizeFromStrBuffer(const buffer &rawData) = 0;

        virtual void synchronizeToKstrBuffer(buffer &rawData, StrCompressionElevatorPtr compressionElevator = StrCompressionElevator::NeverElevate(),
            u32 requestedCompressionFlags = StrCompressFlag::Unchanged, u32 allowedCompressionFlags = StrCompressFlag::Unchanged) = 0;
        virtual void synchronizeFromKstrBuffer(const buffer &rawData) = 0;

        u32 getRequestedCompressionFlags() const { return requestedCompressionFlags; }
        u32 getAllowedCompressionFlags() const { return allowedCompressionFlags; }
        void setRequestedCompressionFlags(u32 requestedCompressionFlags) { this->requestedCompressionFlags = requestedCompressionFlags; }
        void setAllowedCompressionFlags(u32 allowedCompressionFlags) { this->allowedCompressionFlags = allowedCompressionFlags; }

    private:
        u32 requestedCompressionFlags;
        u32 allowedCompressionFlags;

        StrSynchronizer(); // Disallow ctor
};

#endif