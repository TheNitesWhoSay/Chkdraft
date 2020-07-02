#include "Sections.h"
#include <unordered_map>
#include <algorithm>
#include <set>
#include <memory>
#include <iterator>

ChkSection::ChkSection(SectionName sectionName, bool virtualizable, bool isVirtual)
    : sectionIndex(SectionIndex::UNKNOWN), sectionName(sectionName), virtualizable(virtualizable), dataIsVirtual(isVirtual)
{
    auto foundSectionIndex = sectionIndexes.find(sectionName);
    if ( foundSectionIndex != sectionIndexes.end() )
        sectionIndex = foundSectionIndex->second;
}

ChkSection::~ChkSection()
{

}

LoadBehavior ChkSection::getLoadBehavior(SectionName sectionName)
{
    auto nonStandardLoadBehavior = nonStandardLoadBehaviors.find(sectionName);
    if ( nonStandardLoadBehavior != nonStandardLoadBehaviors.end() )
        return nonStandardLoadBehavior->second;
    else
        return LoadBehavior::Standard;
}

std::string ChkSection::getNameString(SectionName sectionName)
{
    auto foundSectionNameString = sectionNameStrings.find(sectionName);
    if ( foundSectionNameString != sectionNameStrings.end() )
        return foundSectionNameString->second;
    else
        return sectionNameStrings.find(SectionName::UNKNOWN)->second;
}

Section allocate(const SectionName & sectionName)
{
    switch ( sectionName )
    {
        case SectionName::TYPE: return std::static_pointer_cast<ChkSection>(TypeSectionPtr(new TypeSection()));
        case SectionName::VER: return std::static_pointer_cast<ChkSection>(VerSectionPtr(new VerSection()));
        case SectionName::IVER: return std::static_pointer_cast<ChkSection>(IverSectionPtr(new IverSection()));
        case SectionName::IVE2: return std::static_pointer_cast<ChkSection>(Ive2SectionPtr(new Ive2Section()));

        case SectionName::VCOD: return std::static_pointer_cast<ChkSection>(VcodSectionPtr(new VcodSection()));
        case SectionName::IOWN: return std::static_pointer_cast<ChkSection>(IownSectionPtr(new IownSection()));
        case SectionName::OWNR: return std::static_pointer_cast<ChkSection>(OwnrSectionPtr(new OwnrSection()));
        case SectionName::ERA: return std::static_pointer_cast<ChkSection>(EraSectionPtr(new EraSection()));

        case SectionName::DIM: return std::static_pointer_cast<ChkSection>(DimSectionPtr(new DimSection()));
        case SectionName::SIDE: return std::static_pointer_cast<ChkSection>(SideSectionPtr(new SideSection()));
        case SectionName::MTXM: return std::static_pointer_cast<ChkSection>(MtxmSectionPtr(new MtxmSection()));
        case SectionName::PUNI: return std::static_pointer_cast<ChkSection>(PuniSectionPtr(new PuniSection()));

        case SectionName::UPGR: return std::static_pointer_cast<ChkSection>(UpgrSectionPtr(new UpgrSection()));
        case SectionName::PTEC: return std::static_pointer_cast<ChkSection>(PtecSectionPtr(new PtecSection()));
        case SectionName::UNIT: return std::static_pointer_cast<ChkSection>(UnitSectionPtr(new UnitSection()));
        case SectionName::ISOM: return std::static_pointer_cast<ChkSection>(IsomSectionPtr(new IsomSection()));

        case SectionName::TILE: return std::static_pointer_cast<ChkSection>(TileSectionPtr(new TileSection()));
        case SectionName::DD2: return std::static_pointer_cast<ChkSection>(Dd2SectionPtr(new Dd2Section()));
        case SectionName::THG2: return std::static_pointer_cast<ChkSection>(Thg2SectionPtr(new Thg2Section()));
        case SectionName::MASK: return std::static_pointer_cast<ChkSection>(MaskSectionPtr(new MaskSection()));

        case SectionName::STR: return std::static_pointer_cast<ChkSection>(StrSectionPtr(new StrSection()));
        case SectionName::UPRP: return std::static_pointer_cast<ChkSection>(UprpSectionPtr(new UprpSection()));
        case SectionName::UPUS: return std::static_pointer_cast<ChkSection>(UpusSectionPtr(new UpusSection()));
        case SectionName::MRGN: return std::static_pointer_cast<ChkSection>(MrgnSectionPtr(new MrgnSection()));

        case SectionName::TRIG: return std::static_pointer_cast<ChkSection>(TrigSectionPtr(new TrigSection()));
        case SectionName::MBRF: return std::static_pointer_cast<ChkSection>(MbrfSectionPtr(new MbrfSection()));
        case SectionName::SPRP: return std::static_pointer_cast<ChkSection>(SprpSectionPtr(new SprpSection()));
        case SectionName::FORC: return std::static_pointer_cast<ChkSection>(ForcSectionPtr(new ForcSection()));

        case SectionName::WAV: return std::static_pointer_cast<ChkSection>(WavSectionPtr(new WavSection()));
        case SectionName::UNIS: return std::static_pointer_cast<ChkSection>(UnisSectionPtr(new UnisSection()));
        case SectionName::UPGS: return std::static_pointer_cast<ChkSection>(UpgsSectionPtr(new UpgsSection()));
        case SectionName::TECS: return std::static_pointer_cast<ChkSection>(TecsSectionPtr(new TecsSection()));

        case SectionName::SWNM: return std::static_pointer_cast<ChkSection>(SwnmSectionPtr(new SwnmSection()));
        case SectionName::COLR: return std::static_pointer_cast<ChkSection>(ColrSectionPtr(new ColrSection()));
        case SectionName::PUPx: return std::static_pointer_cast<ChkSection>(PupxSectionPtr(new PupxSection()));
        case SectionName::PTEx: return std::static_pointer_cast<ChkSection>(PtexSectionPtr(new PtexSection()));

        case SectionName::UNIx: return std::static_pointer_cast<ChkSection>(UnixSectionPtr(new UnixSection()));
        case SectionName::UPGx: return std::static_pointer_cast<ChkSection>(UpgxSectionPtr(new UpgxSection()));
        case SectionName::TECx: return std::static_pointer_cast<ChkSection>(TecxSectionPtr(new TecxSection()));

        case SectionName::OSTR: return std::static_pointer_cast<ChkSection>(OstrSectionPtr(new OstrSection()));
        case SectionName::KSTR: return std::static_pointer_cast<ChkSection>(KstrSectionPtr(new KstrSection()));
            
        case SectionName::KTRG: return std::static_pointer_cast<ChkSection>(KtrgSectionPtr(new KtrgSection()));
        case SectionName::KTGP: return std::static_pointer_cast<ChkSection>(KtgpSectionPtr(new KtgpSection()));
        
        case SectionName::UNKNOWN:
        default: return DataSectionPtr(new DataSection(sectionName));
    }
}

Section ChkSection::read(std::multimap<SectionName, Section> & parsedSections, const Chk::SectionHeader & sectionHeader, std::istream & is, output_param Chk::SectionSize & sizeRead)
{
    const SectionName & sectionName = sectionHeader.name;
    const Chk::SectionSize sectionSizeInBytes = sectionHeader.sizeInBytes;
    LoadBehavior loadBehavior = ChkSection::getLoadBehavior(sectionName);
    Section section = nullptr;
    bool overrideOrAppend = false;

    if ( sectionSizeInBytes < 0 )
        throw NegativeSectionSize(sectionName);

    if ( loadBehavior == LoadBehavior::Standard )
        section = allocate(sectionName);
    else // if ( loadBehavior == LoadBehavior::Override || loadBehavior == LoadBehavior::Append )
    {
        auto sectionInstances = parsedSections.equal_range(sectionName);
        if ( sectionInstances.first != parsedSections.end() && sectionInstances.first->first == sectionName )
        {
            section = sectionInstances.first->second;
            overrideOrAppend = true;
        }
        else
            section = allocate(sectionName);
    }

    sizeRead = (Chk::SectionSize)section->read(sectionHeader, is, overrideOrAppend);
    return section;
}

void ChkSection::writeWithHeader(std::ostream & os, ScenarioSaver & scenarioSaver)
{
    Chk::SectionSize size = getSize(scenarioSaver);
    os.write((const char*)&sectionName, sizeof(sectionName));
    os.write((const char*)&size, sizeof(size));
    write(os, scenarioSaver);
}

ChkSection::MaxSectionSizeExceeded::MaxSectionSizeExceeded(SectionName sectionName, std::string sectionSize) :
    error("The size of ChkSection " + ChkSection::getNameString(sectionName) + " was " + sectionSize + " which exceeded the max size: " + std::to_string(MaxChkSectionSize))
{

}

ChkSection::SectionSerializationSizeMismatch::SectionSerializationSizeMismatch(SectionName sectionName, u32 expectedSectionSize, size_t actualDataSize) :
    error("The expected size of ChkSection " + ChkSection::getNameString(sectionName) + " was " + std::to_string(expectedSectionSize)
        + " which exceeded the actual size: " + std::to_string(actualDataSize))
{

}

ChkSection::SectionValidationException::SectionValidationException(SectionName sectionName, std::string error) :
    error("Validation error in chk section " + ChkSection::getNameString(sectionName) + " : " + error)
{

}

ChkSection::NegativeSectionSize::NegativeSectionSize(SectionName sectionName) :
    std::out_of_range("Cannot read a section with a negative size into a " + ChkSection::getNameString(sectionName) + " section instance!")
{

}

std::vector<SectionName> ChkSection::sectionNames = {
    SectionName::TYPE, SectionName::VER, SectionName::IVER, SectionName::IVE2,
    SectionName::VCOD, SectionName::IOWN, SectionName::OWNR, SectionName::ERA,
    SectionName::DIM, SectionName::SIDE, SectionName::MTXM, SectionName::PUNI,
    SectionName::UPGR, SectionName::PTEC, SectionName::UNIT, SectionName::ISOM,
    SectionName::TILE, SectionName::DD2, SectionName::THG2, SectionName::MASK,
    SectionName::STR, SectionName::UPRP, SectionName::UPUS, SectionName::MRGN,
    SectionName::TRIG, SectionName::MBRF, SectionName::SPRP, SectionName::FORC,
    SectionName::WAV, SectionName::UNIS, SectionName::UPGS, SectionName::TECS,
    SectionName::SWNM, SectionName::COLR, SectionName::PUPx, SectionName::PTEx,
    SectionName::UNIx, SectionName::UPGx, SectionName::TECx,

    SectionName::OSTR,
    SectionName::KSTR,
    SectionName::KTRG,
    SectionName::KTGP,

    SectionName::UNKNOWN
};

std::unordered_map<SectionName, std::string> ChkSection::sectionNameStrings = {
    std::pair<SectionName, std::string>(SectionName::TYPE, "TYPE"), std::pair<SectionName, std::string>(SectionName::VER, "VER"),
    std::pair<SectionName, std::string>(SectionName::IVER, "IVER"), std::pair<SectionName, std::string>(SectionName::IVE2, "IVE2"),
    std::pair<SectionName, std::string>(SectionName::VCOD, "VCOD"), std::pair<SectionName, std::string>(SectionName::IOWN, "IOWN"),
    std::pair<SectionName, std::string>(SectionName::OWNR, "OWNR"), std::pair<SectionName, std::string>(SectionName::ERA, "ERA"),
    std::pair<SectionName, std::string>(SectionName::DIM, "DIM"), std::pair<SectionName, std::string>(SectionName::SIDE, "SIDE"),
    std::pair<SectionName, std::string>(SectionName::MTXM, "MTXM"), std::pair<SectionName, std::string>(SectionName::PUNI, "PUNI"),
    std::pair<SectionName, std::string>(SectionName::UPGR, "UPGR"), std::pair<SectionName, std::string>(SectionName::PTEC, "PTEC"),
    std::pair<SectionName, std::string>(SectionName::UNIT, "UNIT"), std::pair<SectionName, std::string>(SectionName::ISOM, "ISOM"),
    std::pair<SectionName, std::string>(SectionName::TILE, "TILE"), std::pair<SectionName, std::string>(SectionName::DD2, "DD2"),
    std::pair<SectionName, std::string>(SectionName::THG2, "THG2"), std::pair<SectionName, std::string>(SectionName::MASK, "MASK"),
    std::pair<SectionName, std::string>(SectionName::STR, "STR"), std::pair<SectionName, std::string>(SectionName::UPRP, "UPRP"),
    std::pair<SectionName, std::string>(SectionName::UPUS, "UPUS"), std::pair<SectionName, std::string>(SectionName::MRGN, "MRGN"),
    std::pair<SectionName, std::string>(SectionName::TRIG, "TRIG"), std::pair<SectionName, std::string>(SectionName::MBRF, "MBRF"),
    std::pair<SectionName, std::string>(SectionName::SPRP, "SPRP"), std::pair<SectionName, std::string>(SectionName::FORC, "FORC"),
    std::pair<SectionName, std::string>(SectionName::WAV, "WAV"), std::pair<SectionName, std::string>(SectionName::UNIS, "UNIS"),
    std::pair<SectionName, std::string>(SectionName::UPGS, "UPGS"), std::pair<SectionName, std::string>(SectionName::TECS, "TECS"),
    std::pair<SectionName, std::string>(SectionName::SWNM, "SWNM"), std::pair<SectionName, std::string>(SectionName::COLR, "COLR"),
    std::pair<SectionName, std::string>(SectionName::PUPx, "PUPx"), std::pair<SectionName, std::string>(SectionName::PTEx, "PTEx"),
    std::pair<SectionName, std::string>(SectionName::UNIx, "UNIx"), std::pair<SectionName, std::string>(SectionName::UPGx, "UPGx"),
    std::pair<SectionName, std::string>(SectionName::TECx, "TECx"),
    
    std::pair<SectionName, std::string>(SectionName::OSTR, "OSTR"),
    std::pair<SectionName, std::string>(SectionName::KSTR, "KSTR"),
    std::pair<SectionName, std::string>(SectionName::KTRG, "KTRG"),
    std::pair<SectionName, std::string>(SectionName::KTGP, "KTGP"),

    std::pair<SectionName, std::string>(SectionName::UNKNOWN, "UNKNOWN")
};

std::unordered_map<SectionName, SectionIndex> ChkSection::sectionIndexes = {
    std::pair<SectionName, SectionIndex>(SectionName::TYPE, SectionIndex::TYPE),
    std::pair<SectionName, SectionIndex>(SectionName::VER, SectionIndex::VER),
    std::pair<SectionName, SectionIndex>(SectionName::IVER, SectionIndex::IVER),
    std::pair<SectionName, SectionIndex>(SectionName::IVE2, SectionIndex::IVE2),
    std::pair<SectionName, SectionIndex>(SectionName::VCOD, SectionIndex::VCOD),
    std::pair<SectionName, SectionIndex>(SectionName::IOWN, SectionIndex::IOWN),
    std::pair<SectionName, SectionIndex>(SectionName::OWNR, SectionIndex::OWNR),
    std::pair<SectionName, SectionIndex>(SectionName::ERA, SectionIndex::ERA),
    std::pair<SectionName, SectionIndex>(SectionName::DIM, SectionIndex::DIM),
    std::pair<SectionName, SectionIndex>(SectionName::SIDE, SectionIndex::SIDE),
    std::pair<SectionName, SectionIndex>(SectionName::MTXM, SectionIndex::MTXM),
    std::pair<SectionName, SectionIndex>(SectionName::PUNI, SectionIndex::PUNI),
    std::pair<SectionName, SectionIndex>(SectionName::UPGR, SectionIndex::UPGR),
    std::pair<SectionName, SectionIndex>(SectionName::PTEC, SectionIndex::PTEC),
    std::pair<SectionName, SectionIndex>(SectionName::UNIT, SectionIndex::UNIT),
    std::pair<SectionName, SectionIndex>(SectionName::ISOM, SectionIndex::ISOM),
    std::pair<SectionName, SectionIndex>(SectionName::TILE, SectionIndex::TILE),
    std::pair<SectionName, SectionIndex>(SectionName::DD2, SectionIndex::DD2),
    std::pair<SectionName, SectionIndex>(SectionName::THG2, SectionIndex::THG2),
    std::pair<SectionName, SectionIndex>(SectionName::MASK, SectionIndex::MASK),
    std::pair<SectionName, SectionIndex>(SectionName::STR, SectionIndex::STR),
    std::pair<SectionName, SectionIndex>(SectionName::UPRP, SectionIndex::UPRP),
    std::pair<SectionName, SectionIndex>(SectionName::UPUS, SectionIndex::UPUS),
    std::pair<SectionName, SectionIndex>(SectionName::MRGN, SectionIndex::MRGN),
    std::pair<SectionName, SectionIndex>(SectionName::TRIG, SectionIndex::TRIG),
    std::pair<SectionName, SectionIndex>(SectionName::MBRF, SectionIndex::MBRF),
    std::pair<SectionName, SectionIndex>(SectionName::SPRP, SectionIndex::SPRP),
    std::pair<SectionName, SectionIndex>(SectionName::FORC, SectionIndex::FORC),
    std::pair<SectionName, SectionIndex>(SectionName::WAV, SectionIndex::WAV),
    std::pair<SectionName, SectionIndex>(SectionName::UNIS, SectionIndex::UNIS),
    std::pair<SectionName, SectionIndex>(SectionName::UPGS, SectionIndex::UPGS),
    std::pair<SectionName, SectionIndex>(SectionName::TECS, SectionIndex::TECS),
    std::pair<SectionName, SectionIndex>(SectionName::SWNM, SectionIndex::SWNM),
    std::pair<SectionName, SectionIndex>(SectionName::COLR, SectionIndex::COLR),
    std::pair<SectionName, SectionIndex>(SectionName::PUPx, SectionIndex::PUPx),
    std::pair<SectionName, SectionIndex>(SectionName::PTEx, SectionIndex::PTEx),
    std::pair<SectionName, SectionIndex>(SectionName::UNIx, SectionIndex::UNIx),
    std::pair<SectionName, SectionIndex>(SectionName::UPGx, SectionIndex::UPGx),
    std::pair<SectionName, SectionIndex>(SectionName::TECx, SectionIndex::TECx),
    
    std::pair<SectionName, SectionIndex>(SectionName::OSTR, SectionIndex::OSTR),
    std::pair<SectionName, SectionIndex>(SectionName::KSTR, SectionIndex::KSTR),
    std::pair<SectionName, SectionIndex>(SectionName::KTRG, SectionIndex::KTRG),
    std::pair<SectionName, SectionIndex>(SectionName::KTGP, SectionIndex::KTGP),

    std::pair<SectionName, SectionIndex>(SectionName::UNKNOWN, SectionIndex::UNKNOWN)
};

std::unordered_map<SectionName, LoadBehavior> ChkSection::nonStandardLoadBehaviors = {
    std::pair<SectionName, LoadBehavior>(SectionName::MTXM, LoadBehavior::Override),
    std::pair<SectionName, LoadBehavior>(SectionName::UNIT, LoadBehavior::Append),
    std::pair<SectionName, LoadBehavior>(SectionName::THG2, LoadBehavior::Append),
    std::pair<SectionName, LoadBehavior>(SectionName::STR, LoadBehavior::Override),
    std::pair<SectionName, LoadBehavior>(SectionName::TRIG, LoadBehavior::Append),
    std::pair<SectionName, LoadBehavior>(SectionName::MBRF, LoadBehavior::Append)
};

StrProp::StrProp() : red(0), green(0), blue(0), isUsed(false), hasPriority(false), isBold(false), isUnderlined(false), isItalics(false), size(Chk::baseFontSize)
{

}

StrProp::StrProp(Chk::StringProperties stringProperties) :
    red(stringProperties.red), green(stringProperties.green), blue(stringProperties.blue),
    isUsed((stringProperties.flags & Chk::StrFlags::isUsed) == Chk::StrFlags::isUsed), hasPriority((stringProperties.flags & Chk::StrFlags::hasPriority) == Chk::StrFlags::hasPriority),
    isBold((stringProperties.flags & Chk::StrFlags::bold) == Chk::StrFlags::bold), isUnderlined((stringProperties.flags & Chk::StrFlags::underlined) == Chk::StrFlags::underlined),
    isItalics((stringProperties.flags & Chk::StrFlags::italics) == Chk::StrFlags::italics),
    size(Chk::baseFontSize +
        ((stringProperties.flags & Chk::StrFlags::sizePlusFourSteps) == Chk::StrFlags::sizePlusFourSteps) ? 4*Chk::fontStepSize : 0 +
        ((stringProperties.flags & Chk::StrFlags::sizePlusTwoSteps) == Chk::StrFlags::sizePlusTwoSteps) ? 2*Chk::fontStepSize : 0 +
        ((stringProperties.flags & Chk::StrFlags::sizePlusOneStep) == Chk::StrFlags::sizePlusOneStep) ? 1*Chk::fontStepSize : 0)
{
    
}

StrProp::StrProp(u8 red, u8 green, u8 blue, u32 size, bool isUsed, bool hasPriority, bool isBold, bool isUnderlined, bool isItalics)
    : red(red), green(green), blue(blue), size(size), isUsed(isUsed), hasPriority(hasPriority), isBold(isBold), isUnderlined(isUnderlined), isItalics(isItalics)
{

}

ScStr::ScStr(const std::string & str) : strProp()
{
    for ( const char & c : str )
        allocation.push_back(c);

    if ( allocation.size() == 0 || allocation.back() != '\0' )
        allocation.push_back('\0');

    this->str = &allocation[0];
}

ScStr::ScStr(const std::string & str, const StrProp & strProp) : strProp(strProp)
{
    for ( const char & c : str )
        allocation.push_back(c);

    if ( allocation.size() == 0 || allocation.back() != '\0' )
        allocation.push_back('\0');

    this->str = &allocation[0];
}

bool ScStr::empty()
{
    return parentStr == nullptr ? allocation.size() <= 1 : parentStr->empty();
}

size_t ScStr::length()
{
    return parentStr == nullptr ? (allocation.size() > 0 ? allocation.size()-1 : 0) : strlen(str);
}

StrProp & ScStr::properties()
{
    return strProp;
}

template <typename StringType>
int ScStr::compare(const StringType & str)
{
    RawString rawStr;
    convertStr<StringType, RawString>(str, rawStr);
    return strcmp(this->str, rawStr.c_str());
}
template int ScStr::compare<RawString>(const RawString & str);
template int ScStr::compare<EscString>(const EscString & str);
template int ScStr::compare<ChkdString>(const ChkdString & str);
template int ScStr::compare<SingleLineChkdString>(const SingleLineChkdString & str);

template <typename StringType>
std::shared_ptr<StringType> ScStr::toString()
{
    std::shared_ptr<StringType> destStr = std::shared_ptr<StringType>(new StringType());
    convertStr<RawString, StringType>(str, *destStr);
    return destStr;
}
template std::shared_ptr<RawString> ScStr::toString<RawString>();
template std::shared_ptr<EscString> ScStr::toString<EscString>();
template std::shared_ptr<ChkdString> ScStr::toString<ChkdString>();
template std::shared_ptr<SingleLineChkdString> ScStr::toString<SingleLineChkdString>();

ScStrPtr ScStr::getParentStr()
{
    return parentStr;
}

ScStrPtr ScStr::getChildStr()
{
    return childStr;
}

bool ScStr::adopt(ScStrPtr lhs, ScStrPtr rhs)
{
    if ( rhs != nullptr && lhs != nullptr && rhs->parentStr == nullptr || lhs->parentStr == nullptr )
    {
        size_t lhsLength = lhs->parentStr == nullptr ? lhs->allocation.size()-1 : strlen(lhs->str);
        size_t rhsLength = rhs->parentStr == nullptr ? rhs->allocation.size()-1 : strlen(rhs->str);
        if ( rhsLength > lhsLength ) // The length of rhs is greater
        {
            const char* rhsSubString = &rhs->str[rhsLength-lhsLength];
            if ( strcmp(lhs->str, rhsSubString) == 0 ) // rhs can adopt lhs
                ScStr::adopt(rhs, lhs, rhsLength, lhsLength, rhsSubString);
        }
        else // lhsLength >= rhsLength - the length of lhs is greater or equal to rhs
        {
            const char* lhsSubString = &lhs->str[lhsLength-rhsLength];
            if ( strcmp(rhs->str, lhsSubString) == 0 ) // lhs can adopt rhs
                ScStr::adopt(lhs, rhs, lhsLength, rhsLength, lhsSubString);
        }
    }
    return false;
}

void ScStr::disown()
{
    if ( childStr != nullptr )
    {
        childStr->parentStr = parentStr; // Make child's parent its grandparent (or nullptr if none)
        childStr = nullptr;
    }
    if ( parentStr != nullptr )
    {
        parentStr->childStr = childStr; // Make parent's child its grandchild (or nullptr if none)
        parentStr = nullptr;
    }
}

void ScStr::adopt(ScStrPtr parent, ScStrPtr child, size_t parentLength, size_t childLength, const char* parentSubString)
{
    ScStrPtr nextChild = parent->childStr; // Some descendent of parent may be a more suitable parent for this child
    while ( nextChild != nullptr && nextChild->length() > childLength )
    {
        parent = nextChild;
        nextChild = parent->childStr;
    }
    if ( nextChild != nullptr ) // There are more children smaller than this one that this child should adopt
    {
        child->childStr = parent->childStr; // Make child's child the smaller child
        child->childStr->parentStr = child; // Make parent for that same smaller child this child
        child->parentStr = parent; // Make child's parent the smallest suitable parent selected above
        parent->childStr = child; // Assign the child as child to the parent
    }
    else // nextChild == nullptr, there are no more children smaller than this one
    {
        child->parentStr = parent; // Make child's parent the smallest suitable parent selected above
        parent->childStr = child; // Assign the child as child to the parent
    }
}

StrSerializationFailure::StrSerializationFailure()
    : StringException("Unknown error serializing STR section!")
{

}

MaximumStringsExceeded::MaximumStringsExceeded(std::string sectionName, size_t numStrings, size_t maxStrings)
    : StringException("Error with " + sectionName + " section: " + std::to_string(numStrings)
        + " strings given, but the " + sectionName + " section can have " + std::to_string(maxStrings) + " strings max.")
{
    
}

MaximumStringsExceeded::MaximumStringsExceeded()
    : StringException("Error placing string, limit of 65536 strings exceeded!")
{

}

InsufficientStringCapacity::InsufficientStringCapacity(std::string sectionName, size_t numStrings, size_t requestedCapacity, bool autoDefragment)
    : StringException("Error changing the " + sectionName + " section string capacity to " + std::to_string(requestedCapacity)
        + (autoDefragment || numStrings > requestedCapacity ? " there are " + std::to_string(numStrings) + " strings used in the map." : " the " + sectionName + " section would need to be defragmented."))
{

}

MaximumCharactersExceeded::MaximumCharactersExceeded(std::string sectionName, size_t numCharacters, size_t characterSpaceSize)
    : StringException("Error serializing " + sectionName + " section: " + std::to_string(numCharacters) + " characters given, which cannot fit in the "
        + std::to_string(characterSpaceSize) + " bytes of pointable character space using selected compression.")
{

}

MaximumOffsetAndCharsExceeded::MaximumOffsetAndCharsExceeded(std::string sectionName, size_t numStrings, size_t numCharacters, size_t sectionSpace)
    : StringException("Error serializing " + sectionName + " section: " + std::to_string(numStrings) + " strings worth of offsets and " + std::to_string(numCharacters)
        + " characters given, which cannot together fit in the " + std::to_string(sectionSpace) + " bytes of section space using selected compression.")
{

}

TypeSectionPtr TypeSection::GetDefault(Chk::Version version)
{
    if ( version < Chk::Version::StarCraft_BroodWar )
        return TypeSectionPtr(new (std::nothrow) TypeSection(Chk::TYPE { Chk::Type::RAWS }));
    else // BroodWar or above
        return TypeSectionPtr(new (std::nothrow) TypeSection(Chk::TYPE { Chk::Type::RAWB }));
}

TypeSection::TypeSection(const Chk::TYPE & data) : StructSection<Chk::TYPE, true>(SectionName::TYPE, data)
{

}

TypeSection::TypeSection() : StructSection<Chk::TYPE, true>(SectionName::TYPE)
{

}

TypeSection::~TypeSection()
{

}

Chk::Type TypeSection::getType()
{
    return data->scenarioType;
}

void TypeSection::setType(Chk::Type type)
{
    data->scenarioType = type;
    setVirtual(false);
}

VerSectionPtr VerSection::GetDefault()
{
    return VerSectionPtr(new (std::nothrow) VerSection(Chk::VER {
        Chk::Version::StarCraft_Hybrid
    }));
}

VerSection::VerSection(const Chk::VER & data) : StructSection<Chk::VER, false>(SectionName::VER, data)
{

}

VerSection::VerSection() : StructSection<Chk::VER, false>(SectionName::VER)
{

}

VerSection::~VerSection()
{

}

bool VerSection::isOriginal()
{
    return data->version < Chk::Version::StarCraft_Hybrid;
}

bool VerSection::isHybrid()
{
    return data->version >= Chk::Version::StarCraft_Hybrid && data->version < Chk::Version::StarCraft_BroodWar;
}

bool VerSection::isExpansion()
{
    return data->version >= Chk::Version::StarCraft_BroodWar;
}

bool VerSection::isHybridOrAbove()
{
    return data->version >= Chk::Version::StarCraft_Hybrid;
}

Chk::Version VerSection::getVersion()
{
    return data->version;
}

void VerSection::setVersion(Chk::Version version)
{
    data->version = version;
}

IverSectionPtr IverSection::GetDefault()
{
    return IverSectionPtr(new (std::nothrow) IverSection(Chk::IVER {
        Chk::IVersion::Current
    }));
}

IverSection::IverSection(const Chk::IVER & data) : StructSection<Chk::IVER, true>(SectionName::IVER, data)
{

}

IverSection::IverSection() : StructSection<Chk::IVER, true>(SectionName::IVER)
{

}

IverSection::~IverSection()
{

}

Chk::IVersion IverSection::getVersion()
{
    return data->version;
}

void IverSection::setVersion(Chk::IVersion version)
{
    data->version = version;
}

Ive2SectionPtr Ive2Section::GetDefault()
{
    return Ive2SectionPtr(new (std::nothrow) Ive2Section(Chk::IVE2 {
        Chk::I2Version::StarCraft_1_04
    }));
}

Ive2Section::Ive2Section(const Chk::IVE2 & data) : StructSection<Chk::IVE2, true>(SectionName::IVE2, data)
{

}

Ive2Section::Ive2Section() : StructSection<Chk::IVE2, true>(SectionName::IVE2)
{

}

Ive2Section::~Ive2Section()
{

}

Chk::I2Version Ive2Section::getVersion()
{
    return data->version;
}

void Ive2Section::setVersion(Chk::I2Version version)
{
    data->version = version;
}

VcodSectionPtr VcodSection::GetDefault()
{
    return VcodSectionPtr(new (std::nothrow) VcodSection(Chk::VCOD {
        {
            0x77CA1934, 0x7168DC99, 0xC3BF600A, 0xA775E7A7, 0xA67D291F, 0xBB3AB0D7, 0xED2431CC, 0x0B134C17, 0xB7A22065, 0x6B18BD91, 0xDD5DC38D, 0x37D57AE2,
            0xD46459F6, 0x0F129A63, 0x462E5C43, 0x2AF874E3, 0x06376A08, 0x3BD6F637, 0x1663940E, 0xEC5C6745, 0xB7F77BD7, 0x9ED4FC1A, 0x8C3FFA73, 0x0FE1C02E,
            0x070974D1, 0xD764E395, 0x74681675, 0xDA4FA799, 0x1F1820D5, 0xBEA0E6E7, 0x1FE3B6A6, 0x70EF0CCA, 0x311AD531, 0x3524B84D, 0x7DC7F8E3, 0xDE581AE1,
            0x432705F4, 0x07DBACBA, 0x0ABE69DC, 0x49EC8FA8, 0x3F1658D7, 0x8AC1DBE5, 0x05C0CF41, 0x721CCA9D, 0xA55FB1A2, 0x9B7023C4, 0x14E10484, 0xDA907B80,
            0x0669DBFA, 0x400FF3A3, 0xD4CEF3BE, 0xD7CBC9E3, 0x3401405A, 0xF81468F2, 0x1AC58E38, 0x4B3DD6FE, 0xFA050553, 0x8E451034, 0xFE6991DD, 0xF0EEE0AF,
            0xDD7E48F3, 0x75DCAD9F, 0xE5AC7A62, 0x67621B31, 0x4D36CD20, 0x742198E0, 0x717909FB, 0x7FCD6736, 0x3CD65F77, 0xC6A6A2A2, 0x6ACEE31A, 0x6CA9CD4E,
            0x3B9DBA86, 0xFD76F4B5, 0xBCF044F8, 0x296EE92E, 0x6B2F2523, 0x4427AB08, 0x99CC127A, 0x75F2DCED, 0x7E383CC5, 0xC51B1CF7, 0x65942DD1, 0xDD48C906,
            0xAC2D32BE, 0x8132C9B5, 0x34D84A66, 0xDF153F35, 0xB6EBEEB2, 0x964DF604, 0x9C944235, 0x61D38A62, 0x6F7BA852, 0xF4FC61DC, 0xFE2D146C, 0x0AA4EA99,
            0x13FED9E8, 0x594448D0, 0xE3F36680, 0x198DD934, 0xFE63D716, 0x3A7E1830, 0xB10F8D9B, 0x8CF5F012, 0xDB58780A, 0x8CB8633E, 0x8EF3AA3A, 0x2E1A8A37,
            0xEFF9315C, 0x7EE36DE3, 0x133EBD9B, 0xB9C044C6, 0x90DA3ABC, 0x74B0ADA4, 0x892757F8, 0x373FE647, 0x5A7942E4, 0xEE8D43DF, 0xE8490AB4, 0x1A88C33C,
            0x766B0188, 0xA3FDC38A, 0x564E7A16, 0xBACB7FA7, 0xEC1C5E02, 0x76C9B9B0, 0x39B1821E, 0xC557C93E, 0x4C382419, 0xB8542F5D, 0x8E575D6F, 0x520AA130,
            0x5E71186D, 0x59C30613, 0x623EDC1F, 0xEBB5DADC, 0xF995911B, 0xDAD591A7, 0x6BCE5333, 0x017000F5, 0xE8EED87F, 0xCEF10AC0, 0xD3B6EB63, 0xA5CCEF78,
            0xA4BC5DAA, 0xD2F2AB96, 0x9AEAFF61, 0xA2ED6AA8, 0x61ED3EBD, 0x9282C139, 0xB1233616, 0xE524A0B0, 0xAAA79B05, 0x339B120D, 0xDA209283, 0xFCECB025,
            0x2338D024, 0x74F295FC, 0x19E57380, 0x447D5097, 0xDB449345, 0x691DADA2, 0xE7EE1444, 0xFF877F2C, 0xF1329E38, 0xDA29BC4D, 0xFE262742, 0xA92BD2C1,
            0x0E7A42F6, 0xD17CE8CB, 0x56EC5B0F, 0x3161B769, 0x25F96DB4, 0x6D793440, 0x0BA753FA, 0xCE82A4FA, 0x614945C3, 0x8F2C450D, 0xF7604928, 0x1EC97DF3,
            0xC189D00F, 0xD3F85226, 0x14358F4D, 0x0B5F9DBA, 0x004AA907, 0x2F2622F7, 0x1FFB673E, 0xC6119CA1, 0x665D4F69, 0x90153458, 0x4654E56C, 0xD6635FAF,
            0xDF950C8A, 0xAFE40DBD, 0x4C4040BF, 0x7151F6A3, 0xF826ED29, 0xD5222885, 0xFACFBEBF, 0x517FC528, 0x076306B8, 0x298FBDEC, 0x717E55FA, 0x6632401A,
            0x9DDED4E8, 0x93FC5ED4, 0x3BD53D7A, 0x802E75CD, 0x87744F0A, 0xEA8FCC1B, 0x7CDBA99A, 0xEFE55316, 0x6EC178AB, 0x5A8972A4, 0x50702C98, 0x1FDFA1FB,
            0x44D9B76B, 0x56828007, 0x83C0BFFD, 0x5BD0490E, 0x0E6A681E, 0x2F0BC29A, 0xE1A0438E, 0xB2F60C99, 0x5E1C7AE0, 0x45A0C82C, 0x88E90B3C, 0xC696B9AC,
            0x2A83AE74, 0x65FA13BB, 0xA61F4FEB, 0xE18A8AB0, 0xB9B8E981, 0x4E1555D5, 0x9BADF245, 0x7E35C23E, 0x722E925F, 0x23685BB6, 0x0E45C66E, 0xD4873BE9,
            0xE3C041F4, 0xBE4405A8, 0x138A0FE4, 0xF437C41A, 0xEF55405A, 0x4B1D799D, 0x9C3A794A, 0xCC378576, 0xB60F3D82, 0x7E93A660, 0xC4C25CBD, 0x907FC772,
            0x10961B4D, 0x68680513, 0xFF7BC035, 0x2A438546
        },
        {
            Chk::ValidationOpCodes::Add_Shifted_Sections, Chk::ValidationOpCodes::XOR_Sections_04       , Chk::ValidationOpCodes::XOR_Sections_05,
            Chk::ValidationOpCodes::ORs_And_Shifts      , Chk::ValidationOpCodes::Sub_Shifted_Sections  , Chk::ValidationOpCodes::Add_Shifted_Sections,
            Chk::ValidationOpCodes::XOR_Sections_05     , Chk::ValidationOpCodes::Sub_Shifted_Sections  , Chk::ValidationOpCodes::XOR_Shifted_Sections,
            Chk::ValidationOpCodes::XOR_Sections        , Chk::ValidationOpCodes::ORs_And_Reverse_Shifts, Chk::ValidationOpCodes::ORs_And_Reverse_Shifts,
            Chk::ValidationOpCodes::XOR_Sections_05     , Chk::ValidationOpCodes::XOR_Sections_04       , Chk::ValidationOpCodes::ORs_And_Shifts,
            Chk::ValidationOpCodes::XOR_Sections
        }
    }));
}

VcodSection::VcodSection(const Chk::VCOD & data) : StructSection<Chk::VCOD, false>(SectionName::VCOD, data)
{

}

VcodSection::VcodSection() : StructSection<Chk::VCOD, false>(SectionName::VCOD)
{

}

VcodSection::~VcodSection()
{

}

bool VcodSection::isDefault()
{
    VcodSectionPtr defaultVcod = VcodSection::GetDefault();
    return memcmp(data, defaultVcod->data, sizeof(Chk::VCOD)) == 0;
}

void VcodSection::setToDefault()
{
    VcodSectionPtr defaultVcod = VcodSection::GetDefault();
    memcpy(data, defaultVcod->data, sizeof(Chk::VCOD));
}

IownSectionPtr IownSection::GetDefault()
{
    return IownSectionPtr(new (std::nothrow) IownSection(Chk::IOWN {
        Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen,
        Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen,
        Sc::Player::SlotType::Inactive, Sc::Player::SlotType::Inactive, Sc::Player::SlotType::Inactive, Sc::Player::SlotType::Inactive
    }));
}

IownSection::IownSection(const Chk::IOWN & data) : StructSection<Chk::IOWN, true>(SectionName::IOWN, data)
{

}

IownSection::IownSection() : StructSection<Chk::IOWN, true>(SectionName::IOWN)
{

}

IownSection::~IownSection()
{

}

Sc::Player::SlotType IownSection::getSlotType(size_t slotIndex)
{
    if ( slotIndex < Sc::Player::Total )
        return data->slotType[slotIndex];
    else
        throw std::out_of_range(std::string("SlotIndex: ") + std::to_string(slotIndex) + " is out of range for the IOWN section!");
}

void IownSection::setSlotType(size_t slotIndex, Sc::Player::SlotType slotType)
{
    if ( slotIndex < Sc::Player::Total )
        data->slotType[slotIndex] = slotType;
    else
        throw std::out_of_range(std::string("SlotIndex: ") + std::to_string(slotIndex) + " is out of range for the IOWN section!");
}

OwnrSectionPtr OwnrSection::GetDefault()
{
    return OwnrSectionPtr(new (std::nothrow) OwnrSection(Chk::OWNR {
        Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen,
        Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen,
        Sc::Player::SlotType::Inactive, Sc::Player::SlotType::Inactive, Sc::Player::SlotType::Inactive, Sc::Player::SlotType::Inactive
    }));
}

OwnrSection::OwnrSection(const Chk::OWNR & data) : StructSection<Chk::OWNR, false>(SectionName::OWNR, data)
{

}

OwnrSection::OwnrSection() : StructSection<Chk::OWNR, false>(SectionName::OWNR)
{

}

OwnrSection::~OwnrSection()
{

}

Sc::Player::SlotType OwnrSection::getSlotType(size_t slotIndex)
{
    if ( slotIndex < Sc::Player::Total )
        return data->slotType[slotIndex];
    else
        throw std::out_of_range(std::string("SlotIndex: ") + std::to_string(slotIndex) + " is out of range for the OWNR section!");
}

void OwnrSection::setSlotType(size_t slotIndex, Sc::Player::SlotType slotType)
{
    if ( slotIndex < Sc::Player::Total )
        data->slotType[slotIndex] = slotType;
    else
        throw std::out_of_range(std::string("SlotIndex: ") + std::to_string(slotIndex) + " is out of range for the OWNR section!");
}

EraSectionPtr EraSection::GetDefault(Sc::Terrain::Tileset tileset)
{
    return EraSectionPtr(new (std::nothrow) EraSection(Chk::ERA { tileset }));
}

EraSection::EraSection(const Chk::ERA & data) : StructSection<Chk::ERA, false>(SectionName::ERA, data)
{

}

EraSection::EraSection() : StructSection<Chk::ERA, false>(SectionName::ERA)
{

}

EraSection::~EraSection()
{

}

Sc::Terrain::Tileset EraSection::getTileset()
{
    return data->tileset;
}

void EraSection::setTileset(Sc::Terrain::Tileset tileset)
{
    data->tileset = tileset;
}

DimSectionPtr DimSection::GetDefault(u16 tileWidth, u16 tileHeight)
{
    return DimSectionPtr(new (std::nothrow) DimSection(Chk::DIM { tileWidth, tileHeight }));
}

DimSection::DimSection(const Chk::DIM & data) : StructSection<Chk::DIM, false>(SectionName::DIM, data)
{

}

DimSection::DimSection() : StructSection<Chk::DIM, false>(SectionName::DIM)
{

}

DimSection::~DimSection()
{

}

size_t DimSection::getTileWidth()
{
    return data->tileWidth;
}

size_t DimSection::getTileHeight()
{
    return data->tileHeight;
}

size_t DimSection::getPixelWidth()
{
    return data->tileWidth * Sc::Terrain::PixelsPerTile;
}

size_t DimSection::getPixelHeight()
{
    return data->tileHeight * Sc::Terrain::PixelsPerTile;
}

void DimSection::setTileWidth(u16 tileWidth)
{
    data->tileWidth = tileWidth;
}

void DimSection::setTileHeight(u16 tileHeight)
{
    data->tileHeight = tileHeight;
}

void DimSection::setDimensions(u16 tileWidth, u16 tileHeight)
{
    data->tileWidth = tileWidth;
    data->tileHeight = tileHeight;
}


SideSectionPtr SideSection::GetDefault()
{
    return SideSectionPtr(new (std::nothrow) SideSection(Chk::SIDE {
        Chk::Race::Terran  , Chk::Race::Zerg    , Chk::Race::Protoss , Chk::Race::Terran,
        Chk::Race::Zerg    , Chk::Race::Protoss , Chk::Race::Terran  , Chk::Race::Zerg,
        Chk::Race::Inactive, Chk::Race::Inactive, Chk::Race::Inactive, Chk::Race::Neutral
    }));
}

SideSection::SideSection(const Chk::SIDE & data) : StructSection<Chk::SIDE, false>(SectionName::SIDE, data)
{

}

SideSection::SideSection() : StructSection<Chk::SIDE, false>(SectionName::SIDE)
{

}

SideSection::~SideSection()
{

}

Chk::Race SideSection::getPlayerRace(size_t playerIndex)
{
    if ( playerIndex < Sc::Player::Total )
        return data->playerRaces[playerIndex];
    else
        throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the SIDE section!");
}

void SideSection::setPlayerRace(size_t playerIndex, Chk::Race race)
{
    if ( playerIndex < Sc::Player::Total )
        data->playerRaces[playerIndex] = race;
    else
        throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the SIDE section!");
}


MtxmSectionPtr MtxmSection::GetDefault(u16 tileWidth, u16 tileHeight)
{
    MtxmSectionPtr newSection(new (std::nothrow) MtxmSection());
    newSection->tiles.assign(size_t(tileWidth)*size_t(tileHeight), u16(0));
    return newSection;
}

MtxmSection::MtxmSection() : DynamicSection<false>(SectionName::MTXM)
{

}

MtxmSection::~MtxmSection()
{

}

u16 MtxmSection::getTile(size_t tileIndex)
{
    if ( tileIndex < tiles.size() )
        return tiles[tileIndex];
    else
        throw std::out_of_range(std::string("TileIndex: ") + std::to_string(tileIndex) + " is past the end of the MTXM section!");
}

void MtxmSection::setTile(size_t tileIndex, u16 tileValue)
{
    if ( tileIndex < tiles.size() )
        tiles[tileIndex] = tileValue;
    else
        throw std::out_of_range(std::string("TileIndex: ") + std::to_string(tileIndex) + " is past the end of the MTXM section!");
}

void setMtxmOrTileDimensions(std::vector<u16> tiles, u16 newTileWidth, u16 newTileHeight, u16 oldTileWidth, u16 oldTileHeight, s32 leftEdge, s32 topEdge)
{
    s64 oldLeft = 0, oldTop = 0, oldRight = oldTileWidth, oldBottom = oldTileHeight,
        newLeft = leftEdge, newTop = topEdge, newRight = (s64)leftEdge+(s64)newTileWidth, newBottom = (s64)topEdge+(s64)newTileHeight,
        currTileWidth = oldTileWidth, currTileHeight = oldTileHeight;

    // Remove tiles as neccessary in order: bottom, top, right, left
    if ( newBottom < oldBottom && currTileWidth > 0 && currTileHeight > 0 ) // Rows need to be removed from the bottom
    {
        s64 numRowsRemoved = oldBottom - newBottom;
        s64 numTilesRemoved = numRowsRemoved*currTileWidth;
        if ( numTilesRemoved >= (s64)tiles.size() )
        {
            tiles.clear();
            currTileWidth = 0;
            currTileHeight = 0;
        }
        else
        {
            tiles.erase(tiles.begin()+(tiles.size()-numTilesRemoved), tiles.end());
            currTileHeight -= numRowsRemoved;
        }
    }

    if ( newTop > oldTop && currTileWidth > 0 && currTileHeight > 0 ) // Rows need to be removed from the top
    {
        s64 numRowsRemoved = newTop - oldTop;
        s64 numTilesRemoved = numRowsRemoved*currTileWidth;
        if ( numTilesRemoved >= (s64)tiles.size() )
        {
            tiles.clear();
            currTileWidth = 0;
            currTileHeight = 0;
        }
        else
        {
            tiles.erase(tiles.begin(), tiles.begin()+numTilesRemoved);
            currTileHeight -= numRowsRemoved;
        }
    }

    if ( newRight < oldRight && currTileWidth > 0 && currTileHeight > 0 ) // Columns need to be removed from the right
    {
        s64 numColumnsRemoved = oldRight - newRight;
        if ( numColumnsRemoved >= currTileWidth )
        {
            tiles.clear();
            currTileWidth = 0;
            currTileHeight = 0;
        }
        else
        {
            size_t firstRemovedColumn = currTileWidth - numColumnsRemoved;
            for ( size_t row = currTileHeight-1; row < (size_t)currTileHeight; row-- )
            {
                size_t rowOffset = row*currTileWidth;
                size_t removedColumnsOffset = rowOffset+firstRemovedColumn;
                auto start = tiles.begin()+removedColumnsOffset;
                tiles.erase(start, start+numColumnsRemoved);
            }
            currTileWidth -= numColumnsRemoved;
        }
    }

    if ( newLeft > oldLeft && currTileWidth > 0 && currTileHeight > 0 ) // Columns need to be removed from the left
    {
        s64 numColumnsRemoved = newLeft - oldLeft;
        if ( numColumnsRemoved >= currTileHeight )
        {
            tiles.clear();
            currTileWidth = 0;
            currTileHeight = 0;
        }
        else
        {
            for ( size_t row = currTileHeight-1; row < (size_t)currTileHeight; row-- )
            {
                size_t rowOffset = row*currTileWidth;
                auto start = tiles.begin()+rowOffset;
                tiles.erase(start, start+numColumnsRemoved);
            }
            currTileWidth -= numColumnsRemoved;
        }
    }

    if ( currTileWidth == 0 || currTileHeight == 0 )
        tiles.clear();
    else // currTileWidth > 0 && currTileHeight > 0
    {
        // Add tiles as necessary in order: right, left, top, bottom

        if ( newRight > oldRight ) // Columns need to be added to the right
        {
            s64 numColumnsAdded = newRight - oldRight;
            for ( s64 row = currTileHeight-1; row >= 0; row-- )
            {
                s64 rowOffset = row*currTileWidth;
                s64 start = rowOffset + numColumnsAdded;
                tiles.insert(tiles.begin() + start, numColumnsAdded, u16(0));
            }
            currTileWidth += numColumnsAdded;
        }

        if ( newLeft < oldLeft ) // Columns need to be added to the left
        {
            s64 numColumnsAdded = oldLeft - newLeft;
            for ( s64 row = currTileHeight-1; row >= 0; row-- )
            {
                s64 rowOffset = row*numColumnsAdded;
                tiles.insert(tiles.begin() + rowOffset, numColumnsAdded, u16(0));
            }
            currTileWidth += numColumnsAdded;
        }

        if ( newTop < oldTop ) // Rows need to be added to the top
        {
            s64 numRowsAdded = oldTop - newTop;
            s64 numTilesAdded = currTileWidth*numRowsAdded;
            tiles.insert(tiles.begin(), numTilesAdded, u16(0));
            currTileHeight += numRowsAdded;
        }

        if ( newBottom > oldBottom ) // Rows need to be added to the bottom
        {
            s64 numRowsAdded = newBottom - oldBottom;
            s64 numTilesAdded = currTileWidth*numRowsAdded;
            tiles.insert(tiles.end(), numTilesAdded, u16(0));
        }
    }
}

void MtxmSection::setDimensions(u16 newTileWidth, u16 newTileHeight, u16 oldTileWidth, u16 oldTileHeight, s32 leftEdge, s32 topEdge)
{
    setMtxmOrTileDimensions(tiles, newTileWidth, newTileHeight, oldTileWidth, oldTileHeight, leftEdge, topEdge);
}

Chk::SectionSize MtxmSection::getSize(ScenarioSaver &)
{
    return Chk::SectionSize(sizeof(u16) * tiles.size());
}

std::streamsize MtxmSection::read(const Chk::SectionHeader & sectionHeader, std::istream & is, bool overriding)
{
    if ( sectionHeader.sizeInBytes < 0 )
        throw NegativeSectionSize(sectionHeader.name);

    size_t readSize = size_t(sectionHeader.sizeInBytes);
    if ( readSize > 0 )
    {
        size_t readNumTiles = readSize/sizeof(u16) + readSize%sizeof(u16);
        if ( readNumTiles > tiles.size() ) // Need to expand section
            tiles.insert(tiles.end(), readNumTiles-tiles.size(), u16(0));
        else if ( !overriding && readNumTiles < tiles.size() ) // Need to shrink section
            tiles.erase(tiles.begin()+readNumTiles, tiles.end());
    
        std::memset(&tiles[0], 0, readSize); // Zero out the bytes about to be read
        is.read((char*)&tiles[0], std::streamsize(sectionHeader.sizeInBytes));
        return is.gcount();
    }
    else if ( !overriding )
        tiles.clear();

    return 0;
}

void MtxmSection::write(std::ostream & os, ScenarioSaver &)
{
    os.write((const char*)&tiles[0], std::streamsize(sizeof(u16)*tiles.size()));
}


PuniSectionPtr PuniSection::GetDefault()
{
    Chk::PUNI data = {
        {}, // playerUnitBuildable (all Yes/1, set below)
        {}, // defaultUnitBuildable (all Yes/1, set below)
        {} // playerUnitUsesDefault (all Yes/1, set below)
    };
    memset(&data.playerUnitBuildable, (int)Chk::Available::Yes, Sc::Unit::TotalTypes*Sc::Player::Total);
    memset(&data.defaultUnitBuildable, (int)Chk::Available::Yes, Sc::Unit::TotalTypes);
    memset(&data.playerUnitUsesDefault, (int)Chk::Available::Yes, Sc::Unit::TotalTypes*Sc::Player::Total);
    return PuniSectionPtr(new (std::nothrow) PuniSection(data));
}

PuniSection::PuniSection(const Chk::PUNI & data) : StructSection<Chk::PUNI, false>(SectionName::PUNI, data)
{

}

PuniSection::PuniSection() : StructSection<Chk::PUNI, false>(SectionName::PUNI)
{

}

PuniSection::~PuniSection()
{

}

bool PuniSection::isUnitBuildable(Sc::Unit::Type unitType, size_t playerIndex)
{
    if ( unitType < Sc::Unit::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            return data->playerUnitBuildable[unitType][playerIndex] != Chk::Available::No;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PUNI section!");
    }
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the PUNI section!");
}

bool PuniSection::isUnitDefaultBuildable(Sc::Unit::Type unitType)
{
    if ( unitType < Sc::Unit::TotalTypes )
        return data->defaultUnitBuildable[unitType] != Chk::Available::No;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the PUNI section!");
}

bool PuniSection::playerUsesDefault(Sc::Unit::Type unitType, size_t playerIndex)
{
    if ( unitType < Sc::Unit::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            return data->playerUnitUsesDefault[unitType][playerIndex] != Chk::UseDefault::No;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PUNI section!");
    }
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the PUNI section!");
}

void PuniSection::setUnitBuildable(Sc::Unit::Type unitType, size_t playerIndex, bool buildable)
{
    Chk::Available unitBuildable = buildable ? Chk::Available::Yes : Chk::Available::No;
    if ( unitType < Sc::Unit::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            data->playerUnitBuildable[unitType][playerIndex] = unitBuildable;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PUNI section!");
    }
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the PUNI section!");
}

void PuniSection::setUnitDefaultBuildable(Sc::Unit::Type unitType, bool buildable)
{
    Chk::Available unitDefaultBuildable = buildable ? Chk::Available::Yes : Chk::Available::No;
    if ( unitType < Sc::Unit::TotalTypes )
        data->defaultUnitBuildable[unitType] = unitDefaultBuildable;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the PUNI section!");
}

void PuniSection::setPlayerUsesDefault(Sc::Unit::Type unitType, size_t playerIndex, bool useDefault)
{
    Chk::UseDefault playerUnitUsesDefault = useDefault ? Chk::UseDefault::Yes : Chk::UseDefault::No;
    if ( unitType < Sc::Unit::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            data->playerUnitUsesDefault[unitType][playerIndex] = playerUnitUsesDefault;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PUNI section!");
    }
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the PUNI section!");
}


UpgrSectionPtr UpgrSection::GetDefault()
{
    Chk::UPGR data = {
        {}, // playerMaxUpgradeLevel (all 0)
        {}, // playerStartUpgradeLevel (all 0)
        { // defaultMaxLevel
            3, 3, 3, 3,    3, 3, 3, 3,    3, 3, 3, 3,    3, 3, 3, 3,
            1, 1, 0, 1,    1, 1, 1, 1,    1, 1, 1, 1,    1, 1, 1, 1,
            1, 1, 1, 1,    1, 1, 1, 1,    1, 1, 1, 1,    1, 0
        },
        {}, // defaultStartLevel (all 0)
        {}, // playerUpgradeUsesDefault (all Yes/1, set below)
    };
    memset(&data.playerUpgradeUsesDefault, (int)Chk::UseDefault::Yes, Sc::Upgrade::TotalOriginalTypes*Sc::Player::Total);
    return UpgrSectionPtr(new (std::nothrow) UpgrSection(data));
}

UpgrSection::UpgrSection(const Chk::UPGR & data) : StructSection<Chk::UPGR, false>(SectionName::UPGR, data)
{

}

UpgrSection::UpgrSection() : StructSection<Chk::UPGR, false>(SectionName::UPGR)
{

}

UpgrSection::~UpgrSection()
{

}

size_t UpgrSection::getMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex)
{
    if ( upgradeType < Sc::Upgrade::TotalOriginalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            return data->playerMaxUpgradeLevel[upgradeType][playerIndex];
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the UPGR section!");
    }
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the UPGR section!");
}

size_t UpgrSection::getStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex)
{
    if ( upgradeType < Sc::Upgrade::TotalOriginalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            return (size_t)data->playerStartUpgradeLevel[upgradeType][playerIndex];
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the UPGR section!");
    }
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the UPGR section!");
}

size_t UpgrSection::getDefaultMaxUpgradeLevel(Sc::Upgrade::Type upgradeType)
{
    if ( upgradeType < Sc::Upgrade::TotalOriginalTypes )
        return data->defaultMaxLevel[upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the UPGR section!");
}

size_t UpgrSection::getDefaultStartUpgradeLevel(Sc::Upgrade::Type upgradeType)
{
    if ( upgradeType < Sc::Upgrade::TotalOriginalTypes )
        return data->defaultStartLevel[upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the UPGR section!");
}

bool UpgrSection::playerUsesDefault(Sc::Upgrade::Type upgradeType, size_t playerIndex)
{
    if ( upgradeType < Sc::Upgrade::TotalOriginalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            return data->playerUpgradeUsesDefault[upgradeType][playerIndex] != Chk::UseDefault::No;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the UPGR section!");
    }
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the UPGR section!");
}

void UpgrSection::setMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, size_t maxUpgradeLevel)
{
    if ( upgradeType < Sc::Upgrade::TotalOriginalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            data->playerMaxUpgradeLevel[upgradeType][playerIndex] = (u8)maxUpgradeLevel;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the UPGR section!");
    }
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the UPGR section!");
}

void UpgrSection::setStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, size_t startUpgradeLevel)
{
    if ( upgradeType < Sc::Upgrade::TotalOriginalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            data->playerStartUpgradeLevel[upgradeType][playerIndex] = (u8)startUpgradeLevel;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the UPGR section!");
    }
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the UPGR section!");
}

void UpgrSection::setDefaultMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t maxUpgradeLevel)
{
    if ( upgradeType < Sc::Upgrade::TotalOriginalTypes )
        data->defaultMaxLevel[upgradeType] = (u8)maxUpgradeLevel;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the UPGR section!");
}

void UpgrSection::setDefaultStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t startUpgradeLevel)
{
    if ( upgradeType < Sc::Upgrade::TotalOriginalTypes )
        data->defaultStartLevel[upgradeType] = (u8)startUpgradeLevel;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the UPGR section!");
}

void UpgrSection::setPlayerUsesDefault(Sc::Upgrade::Type upgradeType, size_t playerIndex, bool useDefault)
{
    Chk::UseDefault playerUpgradeUsesDefault = useDefault ? Chk::UseDefault::Yes : Chk::UseDefault::No;
    if ( upgradeType < Sc::Upgrade::TotalOriginalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            data->playerUpgradeUsesDefault[upgradeType][playerIndex] = playerUpgradeUsesDefault;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the UPGR section!");
    }
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the UPGR section!");
}


PtecSectionPtr PtecSection::GetDefault()
{
    Chk::PTEC data = {
        {}, // techAvailableForPlayer (all No/0)
        {}, // techResearchedForPlayer (all No/0)
        {}, // techAvailableByDefault (all Yes/1, set below)
        { // techResearchedByDefault
            Chk::Researched::No , Chk::Researched::No , Chk::Researched::No , Chk::Researched::No ,
            Chk::Researched::Yes, Chk::Researched::No , Chk::Researched::Yes, Chk::Researched::No ,
            Chk::Researched::No , Chk::Researched::No , Chk::Researched::No , Chk::Researched::No ,
            Chk::Researched::Yes, Chk::Researched::No , Chk::Researched::Yes, Chk::Researched::No ,
            Chk::Researched::No , Chk::Researched::No , Chk::Researched::Yes, Chk::Researched::No ,
            Chk::Researched::No , Chk::Researched::No , Chk::Researched::No , Chk::Researched::Yes
        },
        {} // playerUsesDefaultsForTech (all Yes/1, set below)
    };
    memset(&data.techAvailableByDefault, (int)Chk::Available::Yes, Sc::Tech::TotalOriginalTypes);
    memset(&data.playerUsesDefaultsForTech, (int)Chk::UseDefault::Yes, Sc::Tech::TotalOriginalTypes*Sc::Player::Total);
    return PtecSectionPtr(new (std::nothrow) PtecSection(data));
}

PtecSection::PtecSection(const Chk::PTEC & data) : StructSection<Chk::PTEC, false>(SectionName::PTEC, data)
{

}

PtecSection::PtecSection() : StructSection<Chk::PTEC, false>(SectionName::PTEC)
{

}

PtecSection::~PtecSection()
{

}

bool PtecSection::techAvailable(Sc::Tech::Type techType, size_t playerIndex)
{
    if ( (size_t)techType < Sc::Tech::TotalOriginalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            return data->techAvailableForPlayer[(size_t)techType][playerIndex] != Chk::Available::No;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PTEC section!");
    }
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) + " is out of range for the PTEC section!");
}

bool PtecSection::techResearched(Sc::Tech::Type techType, size_t playerIndex)
{
    if ( (size_t)techType < Sc::Tech::TotalOriginalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            return data->techResearchedForPlayer[(size_t)techType][playerIndex] != Chk::Researched::No;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PTEC section!");
    }
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) + " is out of range for the PTEC section!");
}

bool PtecSection::techDefaultAvailable(Sc::Tech::Type techType)
{
    if ( (size_t)techType < Sc::Tech::TotalOriginalTypes )
        return data->techAvailableByDefault[(size_t)techType] != Chk::Available::No;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) + " is out of range for the PTEC section!");
}

bool PtecSection::techDefaultResearched(Sc::Tech::Type techType)
{
    if ( (size_t)techType < Sc::Tech::TotalOriginalTypes )
        return data->techResearchedByDefault[(size_t)techType] != Chk::Researched::No;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) + " is out of range for the PTEC section!");
}

bool PtecSection::playerUsesDefault(Sc::Tech::Type techType, size_t playerIndex)
{
    if ( (size_t)techType < Sc::Tech::TotalOriginalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            return data->playerUsesDefaultsForTech[(size_t)techType][playerIndex] != Chk::UseDefault::No;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PTEC section!");
    }
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) + " is out of range for the PTEC section!");
}

void PtecSection::setTechAvailable(Sc::Tech::Type techType, size_t playerIndex, bool available)
{
    Chk::Available techAvailable = available ? Chk::Available::Yes : Chk::Available::No;
    if ( (size_t)techType < Sc::Tech::TotalOriginalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            data->techAvailableForPlayer[(size_t)techType][playerIndex] = techAvailable;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PTEC section!");
    }
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) + " is out of range for the PTEC section!");
}

void PtecSection::setTechResearched(Sc::Tech::Type techType, size_t playerIndex, bool researched)
{
    Chk::Researched techResearched = researched ? Chk::Researched::Yes : Chk::Researched::No;
    if ( (size_t)techType < Sc::Tech::TotalOriginalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            data->techResearchedForPlayer[(size_t)techType][playerIndex] = techResearched;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PTEC section!");
    }
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) + " is out of range for the PTEC section!");
}

void PtecSection::setDefaultTechAvailable(Sc::Tech::Type techType, bool available)
{
    Chk::Available techAvailable = available ? Chk::Available::Yes : Chk::Available::No;
    if ( (size_t)techType < Sc::Tech::TotalOriginalTypes )
        data->techAvailableByDefault[(size_t)techType] = techAvailable;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) + " is out of range for the PTEC section!");
}

void PtecSection::setDefaultTechResearched(Sc::Tech::Type techType, bool researched)
{
    Chk::Researched techResearched = researched ? Chk::Researched::Yes : Chk::Researched::No;
    if ( (size_t)techType < Sc::Tech::TotalOriginalTypes )
        data->techResearchedByDefault[(size_t)techType] = techResearched;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) + " is out of range for the PTEC section!");
}

void PtecSection::setPlayerUsesDefault(Sc::Tech::Type techType, size_t playerIndex, bool useDefault)
{
    Chk::UseDefault playerUsesDefault = useDefault ? Chk::UseDefault::Yes : Chk::UseDefault::No;
    if ( (size_t)techType < Sc::Tech::TotalOriginalTypes )
        data->playerUsesDefaultsForTech[(size_t)techType][playerIndex] = playerUsesDefault;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) + " is out of range for the PTEC section!");
}


UnitSectionPtr UnitSection::GetDefault()
{
    return UnitSectionPtr(new (std::nothrow) UnitSection());
}

UnitSection::UnitSection() : DynamicSection<false>(SectionName::UNIT)
{

}

UnitSection::~UnitSection()
{

}

size_t UnitSection::numUnits()
{
    return units.size();
}

std::shared_ptr<Chk::Unit> UnitSection::getUnit(size_t unitIndex)
{
    return units[unitIndex];
}

size_t UnitSection::addUnit(std::shared_ptr<Chk::Unit> unit)
{
    units.push_back(unit);
    return units.size()-1;
}

void UnitSection::insertUnit(size_t unitIndex, std::shared_ptr<Chk::Unit> unit)
{
    if ( unitIndex < units.size() )
    {
        auto position = std::next(units.begin(), unitIndex);
        units.insert(position, unit);
    }
    else if ( unitIndex == units.size() )
        units.push_back(unit);
}

void UnitSection::deleteUnit(size_t unitIndex)
{
    if ( unitIndex < units.size() )
    {
        auto unit = std::next(units.begin(), unitIndex);
        units.erase(unit);
    }
}

void UnitSection::moveUnit(size_t unitIndexFrom, size_t unitIndexTo)
{
    size_t unitIndexMin = std::min(unitIndexFrom, unitIndexTo);
    size_t unitIndexMax = std::max(unitIndexFrom, unitIndexTo);
    if ( unitIndexMax < units.size() && unitIndexFrom != unitIndexTo )
    {
        if ( unitIndexMax-unitIndexMin == 1 && unitIndexMax < units.size() ) // Move up or down by 1 using swap
            units[unitIndexMin].swap(units[unitIndexMax]);
        else // Move up or down by more than one, remove from present location, insert in the list at destination
        {
            auto unit = units[unitIndexFrom];
            auto toErase = std::next(units.begin(), unitIndexFrom);
            units.erase(toErase);
            auto insertPosition = std::next(units.begin(), unitIndexTo-1);
            units.insert(insertPosition, unit);
        }
    }
}

Chk::SectionSize UnitSection::getSize(ScenarioSaver &)
{
    return Chk::SectionSize(sizeof(Chk::Unit) * units.size());
}

std::streamsize UnitSection::read(const Chk::SectionHeader & sectionHeader, std::istream & is, bool append)
{
    if ( sectionHeader.sizeInBytes < 0 )
        throw NegativeSectionSize(sectionHeader.name);

    size_t readSize = size_t(sectionHeader.sizeInBytes);
    if ( readSize > 0 )
    {
        size_t readNumUnits = readSize/sizeof(Chk::Unit) + readSize%sizeof(Chk::Unit);
        std::vector<Chk::Unit> unitBuffer(readNumUnits);
        is.read((char*)&unitBuffer[0], std::streamsize(sectionHeader.sizeInBytes));

        if ( append )
        {
            for ( auto & unit : unitBuffer )
                units.push_back(Chk::UnitPtr(new Chk::Unit(unit)));
        }
        else
        {
            units.clear();
            for ( auto & unit : unitBuffer )
                units.push_back(Chk::UnitPtr(new Chk::Unit(unit)));
        }
        return is.gcount();
    }
    else if ( !append )
        units.clear();

    return 0;
}

void UnitSection::write(std::ostream & os, ScenarioSaver &)
{
    for ( auto unit : units )
        os.write((const char*)unit.get(), std::streamsize(sizeof(Chk::Unit)));
}


IsomSectionPtr IsomSection::GetDefault(u16 tileWidth, u16 tileHeight)
{
    IsomSectionPtr newSection(new (std::nothrow) IsomSection());
    size_t numIsomEntries = (size_t(tileWidth) / size_t(2) + size_t(1)) * (size_t(tileHeight) + size_t(1));
    Chk::IsomEntry blank = {};
    newSection->isomEntries.assign(numIsomEntries, blank);
    return newSection;
}

IsomSection::IsomSection() : DynamicSection<true>(SectionName::ISOM)
{

}

IsomSection::~IsomSection()
{

}

Chk::IsomEntry & IsomSection::getIsomEntry(size_t isomIndex)
{
    if ( isomIndex < isomEntries.size() )
        return isomEntries[isomIndex];
    else
        throw std::out_of_range(std::string("IsomIndex: ") + std::to_string(isomIndex) + " is past the end of the ISOM section!");
}

void IsomSection::setDimensions(u16 newTileWidth, u16 newTileHeight, u16 oldTileWidth, u16 oldTileHeight, s32 leftEdge, s32 topEdge)
{
    size_t oldNumIndices = isomEntries.size();
    size_t newNumIndices = (size_t)newTileWidth * (size_t)newTileHeight;
    if ( oldNumIndices < newNumIndices )
    {
        for ( size_t i=oldNumIndices; i<newNumIndices; i++ )
            isomEntries.push_back(Chk::IsomEntry());
    }
    else if ( oldNumIndices > newNumIndices )
    {
        auto eraseStart = isomEntries.begin();
        std::advance(eraseStart, newNumIndices);
        isomEntries.erase(eraseStart, isomEntries.end());
    }
}

Chk::SectionSize IsomSection::getSize(ScenarioSaver &)
{
    return Chk::SectionSize(sizeof(Chk::IsomEntry)*isomEntries.size());
}

std::streamsize IsomSection::read(const Chk::SectionHeader & sectionHeader, std::istream & is, bool)
{
    if ( sectionHeader.sizeInBytes < 0 )
        throw NegativeSectionSize(sectionHeader.name);

    size_t readSize = size_t(sectionHeader.sizeInBytes);
    if ( readSize > 0 )
    {
        size_t readNumIsomEntries = readSize/sizeof(Chk::IsomEntry) + readSize%sizeof(Chk::IsomEntry);
        Chk::IsomEntry blank = {};
        isomEntries.assign(readNumIsomEntries, blank);
        is.read((char*)&isomEntries[0], std::streamsize(sectionHeader.sizeInBytes));
        return is.gcount();
    }
    else
        isomEntries.clear();

    return 0;
}

void IsomSection::write(std::ostream & os, ScenarioSaver &)
{
    os.write((const char*)&isomEntries[0], std::streamsize(sizeof(Chk::IsomEntry)*isomEntries.size()));
}

TileSectionPtr TileSection::GetDefault(u16 tileWidth, u16 tileHeight)
{
    TileSectionPtr newSection(new (std::nothrow) TileSection());
    newSection->tiles.assign(size_t(tileWidth)*size_t(tileHeight), u16(0));
    return newSection;
}

TileSection::TileSection() : DynamicSection<true>(SectionName::TILE)
{

}

TileSection::~TileSection()
{

}

u16 TileSection::getTile(size_t tileIndex)
{
    if ( tileIndex < tiles.size() )
        return tiles[tileIndex];
    else
        throw std::out_of_range(std::string("TileIndex: ") + std::to_string(tileIndex) + " is past the end of the TILE section!");
}

void TileSection::setTile(size_t tileIndex, u16 tileValue)
{
    if ( tileIndex < tiles.size() )
        tiles[tileIndex] = tileValue;
    else
        throw std::out_of_range(std::string("TileIndex: ") + std::to_string(tileIndex) + " is past the end of the TILE section!");
}

void TileSection::setDimensions(u16 newTileWidth, u16 newTileHeight, u16 oldTileWidth, u16 oldTileHeight, s32 leftEdge, s32 topEdge)
{
    setMtxmOrTileDimensions(tiles, newTileWidth, newTileHeight, oldTileWidth, oldTileHeight, leftEdge, topEdge);
}

Chk::SectionSize TileSection::getSize(ScenarioSaver &)
{
    return Chk::SectionSize(sizeof(u16) * tiles.size());
}

std::streamsize TileSection::read(const Chk::SectionHeader & sectionHeader, std::istream & is, bool overriding)
{
    if ( sectionHeader.sizeInBytes < 0 )
        throw NegativeSectionSize(sectionHeader.name);

    size_t readSize = size_t(sectionHeader.sizeInBytes);
    if ( readSize > 0 )
    {
        size_t readNumTiles = readSize/sizeof(u16) + readSize%sizeof(u16);
        if ( readNumTiles > tiles.size() ) // Need to expand section
            tiles.insert(tiles.end(), readNumTiles-tiles.size(), u16(0));
        else if ( !overriding && readNumTiles < tiles.size()  ) // Need to shrink section
            tiles.erase(tiles.begin()+readNumTiles, tiles.end());
    
        std::memset(&tiles[0], 0, readSize); // Zero out the bytes about to be read
        is.read((char*)&tiles[0], std::streamsize(sectionHeader.sizeInBytes));
        return is.gcount();
    }
    else
        tiles.clear();

    return 0;
}

void TileSection::write(std::ostream & os, ScenarioSaver &)
{
    os.write((const char*)&tiles[0], std::streamsize(sizeof(u16)*tiles.size()));
}

Dd2SectionPtr Dd2Section::GetDefault()
{
    return Dd2SectionPtr(new (std::nothrow) Dd2Section());
}

Dd2Section::Dd2Section() : DynamicSection<true>(SectionName::DD2)
{

}

Dd2Section::~Dd2Section()
{

}

size_t Dd2Section::numDoodads()
{
    return doodads.size();
}

std::shared_ptr<Chk::Doodad> Dd2Section::getDoodad(size_t doodadIndex)
{
    return doodads[doodadIndex];
}

size_t Dd2Section::addDoodad(std::shared_ptr<Chk::Doodad> doodad)
{
    doodads.push_back(doodad);
    return doodads.size()-1;
}

void Dd2Section::insertDoodad(size_t doodadIndex, std::shared_ptr<Chk::Doodad> doodad)
{
    if ( doodadIndex < doodads.size() )
    {
        auto position = std::next(doodads.begin(), doodadIndex);
        doodads.insert(position, doodad);
    }
    else if ( doodadIndex == doodads.size() )
        doodads.push_back(doodad);
}

void Dd2Section::deleteDoodad(size_t doodadIndex)
{
    if ( doodadIndex < doodads.size() )
    {
        auto doodad = std::next(doodads.begin(), doodadIndex);
        doodads.erase(doodad);
    }
}

void Dd2Section::moveDoodad(size_t doodadIndexFrom, size_t doodadIndexTo)
{
    size_t doodadIndexMin = std::min(doodadIndexFrom, doodadIndexTo);
    size_t doodadIndexMax = std::max(doodadIndexFrom, doodadIndexTo);
    if ( doodadIndexMax < doodads.size() && doodadIndexFrom != doodadIndexTo )
    {
        if ( doodadIndexMax-doodadIndexMin == 1 && doodadIndexMax < doodads.size() ) // Move up or down by 1 using swap
            doodads[doodadIndexMin].swap(doodads[doodadIndexMax]);
        else // Move up or down by more than one, remove from present location, insert in the list at destination
        {
            auto doodad = doodads[doodadIndexFrom];
            auto toErase = std::next(doodads.begin(), doodadIndexFrom);
            doodads.erase(toErase);
            auto insertPosition = std::next(doodads.begin(), doodadIndexTo-1);
            doodads.insert(insertPosition, doodad);
        }
    }
}

Chk::SectionSize Dd2Section::getSize(ScenarioSaver &)
{
    return Chk::SectionSize(sizeof(Chk::Doodad) * doodads.size());
}

std::streamsize Dd2Section::read(const Chk::SectionHeader & sectionHeader, std::istream & is, bool append)
{
    if ( sectionHeader.sizeInBytes < 0 )
        throw NegativeSectionSize(sectionHeader.name);

    size_t readSize = size_t(sectionHeader.sizeInBytes);
    if ( readSize > 0 )
    {
        size_t numReadDoodads = readSize/sizeof(Chk::Doodad) + readSize%sizeof(Chk::Doodad);
        std::vector<Chk::Doodad> doodadBuffer(numReadDoodads);
        is.read((char*)&doodadBuffer[0], std::streamsize(sectionHeader.sizeInBytes));

        doodads.clear();
        for ( auto & doodad : doodadBuffer )
            doodads.push_back(Chk::DoodadPtr(new Chk::Doodad(doodad)));
    
        return is.gcount();
    }
    else
        doodads.clear();

    return 0;
}

void Dd2Section::write(std::ostream & os, ScenarioSaver &)
{
    for ( auto doodad : doodads )
        os.write((const char*)doodad.get(), std::streamsize(sizeof(Chk::Doodad)));
}

Thg2SectionPtr Thg2Section::GetDefault()
{
    return Thg2SectionPtr(new (std::nothrow) Thg2Section());
}

Thg2Section::Thg2Section() : DynamicSection<false>(SectionName::THG2)
{

}

Thg2Section::~Thg2Section()
{

}

size_t Thg2Section::numSprites()
{
    return sprites.size();
}

std::shared_ptr<Chk::Sprite> Thg2Section::getSprite(size_t spriteIndex)
{
    return sprites[spriteIndex];
}

size_t Thg2Section::addSprite(std::shared_ptr<Chk::Sprite> sprite)
{
    sprites.push_back(sprite);
    return sprites.size()-1;
}

void Thg2Section::insertSprite(size_t spriteIndex, std::shared_ptr<Chk::Sprite> sprite)
{
    if ( spriteIndex < sprites.size() )
    {
        auto position = std::next(sprites.begin(), spriteIndex);
        sprites.insert(position, sprite);
    }
    else if ( spriteIndex == sprites.size() )
        sprites.push_back(sprite);
}

void Thg2Section::deleteSprite(size_t spriteIndex)
{
    if ( spriteIndex < sprites.size() )
    {
        auto sprite = std::next(sprites.begin(), spriteIndex);
        sprites.erase(sprite);
    }
}

void Thg2Section::moveSprite(size_t spriteIndexFrom, size_t spriteIndexTo)
{
    size_t spriteIndexMin = std::min(spriteIndexFrom, spriteIndexTo);
    size_t spriteIndexMax = std::max(spriteIndexFrom, spriteIndexTo);
    if ( spriteIndexMax < sprites.size() && spriteIndexFrom != spriteIndexTo )
    {
        if ( spriteIndexMax-spriteIndexMin == 1 && spriteIndexMax < sprites.size() ) // Move up or down by 1 using swap
            sprites[spriteIndexMin].swap(sprites[spriteIndexMax]);
        else // Move up or down by more than one, remove from present location, insert in the list at destination
        {
            auto sprite = sprites[spriteIndexFrom];
            auto toErase = std::next(sprites.begin(), spriteIndexFrom);
            sprites.erase(toErase);
            auto insertPosition = std::next(sprites.begin(), spriteIndexTo-1);
            sprites.insert(insertPosition, sprite);
        }
    }
}

Chk::SectionSize Thg2Section::getSize(ScenarioSaver &)
{
    return Chk::SectionSize(sizeof(Chk::Sprite) * sprites.size());
}

std::streamsize Thg2Section::read(const Chk::SectionHeader & sectionHeader, std::istream & is, bool append)
{
    if ( sectionHeader.sizeInBytes < 0 )
        throw NegativeSectionSize(sectionHeader.name);

    size_t readSize = size_t(sectionHeader.sizeInBytes);
    if ( readSize > 0 )
    {
        size_t numReadSprites = readSize/sizeof(Chk::Sprite) + readSize%sizeof(Chk::Sprite);
        std::vector<Chk::Sprite> spriteBuffer(numReadSprites);
        is.read((char*)&spriteBuffer[0], std::streamsize(sectionHeader.sizeInBytes));

        if ( append )
        {
            for ( Chk::Sprite & sprite : spriteBuffer )
                sprites.push_back(Chk::SpritePtr(new Chk::Sprite(sprite)));
        }
        else
        {
            sprites.clear();
            for ( Chk::Sprite & sprite : spriteBuffer )
                sprites.push_back(Chk::SpritePtr(new Chk::Sprite(sprite)));
        }
        return is.gcount();
    }
    else if ( !append )
        sprites.clear();

    return 0;
}

void Thg2Section::write(std::ostream & os, ScenarioSaver &)
{
    for ( auto sprite : sprites )
        os.write((const char*)sprite.get(), std::streamsize(sizeof(Chk::Sprite)));
}

MaskSectionPtr MaskSection::GetDefault(u16 tileWidth, u16 tileHeight)
{
    MaskSectionPtr newSection(new (std::nothrow) MaskSection());
    newSection->fogTiles.assign(size_t(tileWidth)*size_t(tileHeight), u8(0));
    return newSection;
}

MaskSection::MaskSection() : DynamicSection<true>(SectionName::MASK)
{

}

MaskSection::~MaskSection()
{

}

u8 MaskSection::getFog(size_t tileIndex)
{
    if ( tileIndex < fogTiles.size() )
        return fogTiles[tileIndex];
    else
        throw std::out_of_range(std::string("TileIndex: ") + std::to_string(tileIndex) + " is past the end of the MASK section!");
}

void MaskSection::setFog(size_t tileIndex, u8 fogOfWarPlayers)
{
    if ( tileIndex < fogTiles.size() )
        fogTiles[tileIndex] = fogOfWarPlayers;
    else
        throw std::out_of_range(std::string("TileIndex: ") + std::to_string(tileIndex) + " is past the end of the MASK section!");
}

void MaskSection::setDimensions(u16 newTileWidth, u16 newTileHeight, u16 oldTileWidth, u16 oldTileHeight, s32 leftEdge, s32 topEdge)
{
    s64 oldLeft = 0, oldTop = 0, oldRight = oldTileWidth, oldBottom = oldTileHeight,
        newLeft = leftEdge, newTop = topEdge, newRight = (s64)leftEdge+(s64)newTileWidth, newBottom = (s64)topEdge+(s64)newTileHeight,
        currTileWidth = oldTileWidth, currTileHeight = oldTileHeight;

    // Remove tiles as neccessary in order: bottom, top, right, left
    if ( newBottom < oldBottom && currTileWidth > 0 && currTileHeight > 0 ) // Rows need to be removed from the bottom
    {
        s64 numRowsRemoved = oldBottom - newBottom;
        s64 numTilesRemoved = numRowsRemoved*currTileWidth;
        if ( numTilesRemoved >= (s64)fogTiles.size() )
        {
            fogTiles.clear();
            currTileWidth = 0;
            currTileHeight = 0;
        }
        else
        {
            fogTiles.erase(fogTiles.begin()+(fogTiles.size()-numTilesRemoved), fogTiles.end());
            currTileHeight -= numRowsRemoved;
        }
    }

    if ( newTop > oldTop && currTileWidth > 0 && currTileHeight > 0 ) // Rows need to be removed from the top
    {
        s64 numRowsRemoved = newTop - oldTop;
        s64 numTilesRemoved = numRowsRemoved*currTileWidth;
        if ( numTilesRemoved >= (s64)fogTiles.size() )
        {
            fogTiles.clear();
            currTileWidth = 0;
            currTileHeight = 0;
        }
        else
        {
            fogTiles.erase(fogTiles.begin(), fogTiles.begin()+numTilesRemoved);
            currTileHeight -= numRowsRemoved;
        }
    }

    if ( newRight < oldRight && currTileWidth > 0 && currTileHeight > 0 ) // Columns need to be removed from the right
    {
        s64 numColumnsRemoved = oldRight - newRight;
        if ( numColumnsRemoved >= currTileWidth )
        {
            fogTiles.clear();
            currTileWidth = 0;
            currTileHeight = 0;
        }
        else
        {
            size_t firstRemovedColumn = currTileWidth - numColumnsRemoved;
            for ( size_t row = currTileHeight-1; row < (size_t)currTileHeight; row-- )
            {
                size_t rowOffset = row*currTileWidth;
                size_t removedColumnsOffset = rowOffset+firstRemovedColumn;
                auto start = fogTiles.begin()+removedColumnsOffset;
                fogTiles.erase(start, start+numColumnsRemoved);
            }
            currTileWidth -= numColumnsRemoved;
        }
    }

    if ( newLeft > oldLeft && currTileWidth > 0 && currTileHeight > 0 ) // Columns need to be removed from the left
    {
        s64 numColumnsRemoved = newLeft - oldLeft;
        if ( numColumnsRemoved >= currTileHeight )
        {
            fogTiles.clear();
            currTileWidth = 0;
            currTileHeight = 0;
        }
        else
        {
            for ( size_t row = currTileHeight-1; row < (size_t)currTileHeight; row-- )
            {
                size_t rowOffset = row*currTileWidth;
                auto start = fogTiles.begin()+rowOffset;
                fogTiles.erase(start, start+numColumnsRemoved);
            }
            currTileWidth -= numColumnsRemoved;
        }
    }

    if ( currTileWidth == 0 || currTileHeight == 0 )
        fogTiles.clear();
    else // currTileWidth > 0 && currTileHeight > 0
    {
        // Add tiles as necessary in order: right, left, top, bottom

        if ( newRight > oldRight ) // Columns need to be added to the right
        {
            s64 numColumnsAdded = newRight - oldRight;
            for ( s64 row = currTileHeight-1; row >= 0; row-- )
            {
                s64 rowOffset = row*currTileWidth;
                s64 start = rowOffset + numColumnsAdded;
                fogTiles.insert(fogTiles.begin() + start, numColumnsAdded, u16(0));
            }
            currTileWidth += numColumnsAdded;
        }

        if ( newLeft < oldLeft ) // Columns need to be added to the left
        {
            s64 numColumnsAdded = oldLeft - newLeft;
            for ( s64 row = currTileHeight-1; row >= 0; row-- )
            {
                s64 rowOffset = row*numColumnsAdded;
                fogTiles.insert(fogTiles.begin() + rowOffset, numColumnsAdded, u16(0));
            }
            currTileWidth += numColumnsAdded;
        }

        if ( newTop < oldTop ) // Rows need to be added to the top
        {
            s64 numRowsAdded = oldTop - newTop;
            s64 numTilesAdded = currTileWidth*numRowsAdded;
            fogTiles.insert(fogTiles.begin(), numTilesAdded, u16(0));
            currTileHeight += numRowsAdded;
        }

        if ( newBottom > oldBottom ) // Rows need to be added to the bottom
        {
            s64 numRowsAdded = newBottom - oldBottom;
            s64 numTilesAdded = currTileWidth*numRowsAdded;
            fogTiles.insert(fogTiles.end(), numTilesAdded, u16(0));
        }
    }
}

Chk::SectionSize MaskSection::getSize(ScenarioSaver &)
{
    return Chk::SectionSize(sizeof(u8) * fogTiles.size());
}

std::streamsize MaskSection::read(const Chk::SectionHeader & sectionHeader, std::istream & is, bool overriding)
{
    if ( sectionHeader.sizeInBytes < 0 )
        throw NegativeSectionSize(sectionHeader.name);

    size_t readSize = size_t(sectionHeader.sizeInBytes);
    if ( readSize > 0 )
    {
        size_t readNumTiles = readSize/sizeof(u8) + readSize%sizeof(u8);
        if ( readNumTiles > fogTiles.size() ) // Need to expand section
            fogTiles.insert(fogTiles.end(), readNumTiles-fogTiles.size(), u8(0));
        else if ( !overriding && readNumTiles < fogTiles.size()  ) // Need to shrink section
            fogTiles.erase(fogTiles.begin()+readNumTiles, fogTiles.end());
    
        std::memset(&fogTiles[0], 0, readSize); // Zero out the bytes about to be read
        is.read((char*)&fogTiles[0], std::streamsize(sectionHeader.sizeInBytes));
        return is.gcount();
    }
    else
        fogTiles.clear();

    return 0;
}

void MaskSection::write(std::ostream & os, ScenarioSaver &)
{
    os.write((const char*)&fogTiles[0], std::streamsize(fogTiles.size()));
}

StrSectionPtr StrSection::GetDefault(bool blank)
{
    StrSectionPtr newSection(new (std::nothrow) StrSection());

    newSection->strings.clear();
    newSection->strings.push_back(nullptr); // Fill the non-existant 0th stringId
    if ( !blank )
    {
        const std::vector<std::string> defaultStrings = {
            /* StringId: 1 */ "Untitled Scenario",
            /* StringId: 2 */ "Destroy all enemy buildings.",
            /* StringId: 3 */ "Anywhere",
            /* StringId: 4 */ "Force 1",
            /* StringId: 5 */ "Force 2",
            /* StringId: 6 */ "Force 3",
            /* StringId: 7 */ "Force 4"
        };

        for ( const std::string & defaultString : defaultStrings )
            newSection->strings.push_back(ScStrPtr(new ScStr(defaultString)));
    }

    return newSection;
}

StrSection::StrSection() : DynamicSection<false>(SectionName::STR), bytePaddedTo(4), initialTailDataOffset(0)
{
    
}


StrSection::StrSection(const StrSection & other) : DynamicSection<false>(SectionName::STR), strings(other.strings), stringBytes(other.stringBytes),
    bytePaddedTo(other.bytePaddedTo), initialTailDataOffset(other.initialTailDataOffset), tailData(other.tailData)
{

}

StrSection::~StrSection()
{

}

size_t StrSection::getCapacity()
{
    return strings.size();
}

bool StrSection::stringStored(size_t stringId)
{
    return stringId < strings.size() && strings[stringId] != nullptr;
}

void StrSection::unmarkUnstoredStrings(std::bitset<Chk::MaxStrings> & stringIdUsed)
{
    size_t limit = std::min((size_t)Chk::MaxStrings, strings.size());
    size_t stringId = 1;
    for ( ; stringId < limit; ++stringId )
    {
        if ( stringIdUsed[stringId] && strings[stringId] == nullptr )
            stringIdUsed[stringId] = false;
    }
    for ( ; stringId < Chk::MaxStrings; stringId++ )
    {
        if ( stringIdUsed[stringId] )
            stringIdUsed[stringId] = false;
    }
}

template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkdString (Editor <01>Style)
std::shared_ptr<StringType> StrSection::getString(size_t stringId)
{
    return stringId < strings.size() && strings[stringId] != nullptr ? strings[stringId]->toString<StringType>() : nullptr;
}
template std::shared_ptr<RawString> StrSection::getString<RawString>(size_t stringId);
template std::shared_ptr<EscString> StrSection::getString<EscString>(size_t stringId);
template std::shared_ptr<ChkdString> StrSection::getString<ChkdString>(size_t stringId);
template std::shared_ptr<SingleLineChkdString> StrSection::getString<SingleLineChkdString>(size_t stringId);

template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
size_t StrSection::findString(const StringType & str)
{
    for ( size_t stringId=1; stringId<strings.size(); stringId++ )
    {
        if ( strings[stringId] != nullptr && strings[stringId]->compare<StringType>(str) == 0 )
            return stringId;
    }
    return Chk::StringId::NoString;
}
template size_t StrSection::findString<RawString>(const RawString & str);
template size_t StrSection::findString<EscString>(const EscString & str);
template size_t StrSection::findString<ChkdString>(const ChkdString & str);
template size_t StrSection::findString<SingleLineChkdString>(const SingleLineChkdString & str);

bool StrSection::setCapacity(size_t stringCapacity, StrSynchronizer & strSynchronizer, bool autoDefragment)
{
    if ( stringCapacity > Chk::MaxStrings )
        throw MaximumStringsExceeded();

    std::bitset<Chk::MaxStrings> stringIdUsed;
    strSynchronizer.markValidUsedStrings(stringIdUsed, Chk::Scope::Either, Chk::Scope::Game);
    size_t numValidUsedStrings = 0;
    size_t highestValidUsedStringId = 0;
    for ( size_t stringId = 1; stringId<Chk::MaxStrings; stringId++ )
    {
        if ( stringIdUsed[stringId] )
        {
            numValidUsedStrings ++;
            highestValidUsedStringId = stringId;
        }
    }

    if ( numValidUsedStrings > stringCapacity )
        throw InsufficientStringCapacity(ChkSection::getNameString(SectionName::STR), numValidUsedStrings, stringCapacity, autoDefragment);
    else if ( highestValidUsedStringId > stringCapacity )
    {
        if ( autoDefragment && numValidUsedStrings <= stringCapacity )
            defragment(strSynchronizer, false);
        else
            throw InsufficientStringCapacity(ChkSection::getNameString(SectionName::STR), numValidUsedStrings, stringCapacity, autoDefragment);
    }
        
    while ( strings.size() <= stringCapacity )
        strings.push_back(nullptr);

    while ( strings.size() > stringCapacity )
        strings.pop_back();

    return true;
}

template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
size_t StrSection::addString(const StringType & str, StrSynchronizer & strSynchronizer, bool autoDefragment)
{
    RawString rawString;
    convertStr<StringType, RawString>(str, rawString);

    size_t stringId = findString<StringType>(str);
    if ( stringId != (size_t)Chk::StringId::NoString )
        return stringId; // String already exists, return the id

    std::bitset<Chk::MaxStrings> stringIdUsed;
    strSynchronizer.markUsedStrings(stringIdUsed, Chk::Scope::Either, Chk::Scope::Game);
    size_t nextUnusedStringId = getNextUnusedStringId(stringIdUsed, true);
        
    if ( nextUnusedStringId >= strings.size() )
        setCapacity(nextUnusedStringId+1, strSynchronizer, autoDefragment);
    else if ( nextUnusedStringId == 0 )
        throw MaximumStringsExceeded();

    strings[nextUnusedStringId] = ScStrPtr(new ScStr(rawString));
    return nextUnusedStringId;
}
template size_t StrSection::addString<RawString>(const RawString & str, StrSynchronizer & strSynchronizer, bool autoDefragment);
template size_t StrSection::addString<EscString>(const EscString & str, StrSynchronizer & strSynchronizer, bool autoDefragment);
template size_t StrSection::addString<ChkdString>(const ChkdString & str, StrSynchronizer & strSynchronizer, bool autoDefragment);
template size_t StrSection::addString<SingleLineChkdString>(const SingleLineChkdString & str, StrSynchronizer & strSynchronizer, bool autoDefragment);

template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
void StrSection::replaceString(size_t stringId, const StringType & str)
{
    RawString rawString;
    convertStr<StringType, RawString>(str, rawString);

    if ( stringId < strings.size() )
        strings[stringId] = ScStrPtr(new ScStr(rawString, StrProp()));
}
template void StrSection::replaceString<RawString>(size_t stringId, const RawString & str);
template void StrSection::replaceString<EscString>(size_t stringId, const EscString & str);
template void StrSection::replaceString<ChkdString>(size_t stringId, const ChkdString & str);
template void StrSection::replaceString<SingleLineChkdString>(size_t stringId, const SingleLineChkdString & str);

void StrSection::deleteUnusedStrings(StrSynchronizer & strSynchronizer)
{
    std::bitset<65536> stringIdUsed;
    strSynchronizer.markUsedStrings(stringIdUsed, Chk::Scope::Either, Chk::Scope::Game);
    for ( size_t i=0; i<strings.size(); i++ )
    {
        if ( !stringIdUsed[i] && strings[i] != nullptr )
            strings[i] = nullptr;
    }
}

bool StrSection::deleteString(size_t stringId, bool deleteOnlyIfUnused, StrSynchronizerPtr strSynchronizer)
{
    if ( !deleteOnlyIfUnused || (strSynchronizer != nullptr && !strSynchronizer->stringUsed(stringId, Chk::Scope::Either, Chk::Scope::Game, false)) )
    {
        if ( stringId < strings.size() )
        {
            strings[stringId] = nullptr;
            return true;
        }
    }
    return false;
}

void StrSection::moveString(size_t stringIdFrom, size_t stringIdTo, StrSynchronizer & strSynchronizer)
{
    size_t stringIdMin = std::min(stringIdFrom, stringIdTo);
    size_t stringIdMax = std::max(stringIdFrom, stringIdTo);
    if ( stringIdMin > 0 && stringIdMax <= strings.size() && stringIdFrom != stringIdTo )
    {
        std::bitset<Chk::MaxStrings> stringIdUsed;
        strSynchronizer.markUsedStrings(stringIdUsed, Chk::Scope::Game);
        ScStrPtr selected = strings[stringIdFrom];
        stringIdUsed[stringIdFrom] = false;
        std::map<u32, u32> stringIdRemappings;
        if ( stringIdTo < stringIdFrom ) // Move to a lower stringId, if there are strings in the way, cascade towards stringIdFrom
        {
            while ( stringIdUsed[stringIdTo] ) // There is a block of one or more strings where the selected string needs to go
            {
                for ( size_t stringId = stringIdTo+1; stringId <= stringIdFrom; stringId ++ ) // Find the lowest available stringId past the block
                {
                    if ( !stringIdUsed[stringId] ) // Move the highest stringId remaining in the block to the next available stringId
                    {
                        ScStrPtr highestString = strings[stringId-1];
                        strings[stringId-1] = nullptr;
                        stringIdUsed[stringId-1] = false;
                        strings[stringId] = highestString;
                        stringIdUsed[stringId] = true;
                        stringIdRemappings.insert(std::pair<u32, u32>(u32(stringId-1), (u32)stringId));
                        break;
                    }
                }
            }
            stringIdRemappings.insert(std::pair<u32, u32>((u32)stringIdFrom, (u32)stringIdTo));
        }
        else if ( stringIdTo > stringIdFrom ) // Move to a higher stringId, if there are strings in the way, cascade towards stringIdTo
        {
            while ( stringIdUsed[stringIdTo] ) // There is a block of one or more strings where the selected string needs to go
            {
                for ( size_t stringId = stringIdTo-1; stringId >= stringIdFrom; stringId -- ) // Find the highest available stringId past the block
                {
                    if ( !stringIdUsed[stringId] ) // Move the lowest stringId in the block to the available stringId
                    {
                        ScStrPtr lowestString = strings[stringId+1];
                        strings[stringId+1] = nullptr;
                        stringIdUsed[stringId+1] = false;
                        strings[stringId] = lowestString;
                        stringIdUsed[stringId] = true;
                        stringIdRemappings.insert(std::pair<u32, u32>(u32(stringId+1), (u32)stringId));
                        break;
                    }
                }
            }
        }
        strings[stringIdTo] = selected;
        stringIdRemappings.insert(std::pair<u32, u32>((u32)stringIdFrom, (u32)stringIdTo));
        strSynchronizer.remapStringIds(stringIdRemappings, Chk::Scope::Game);
    }
}

bool StrSection::checkFit(StrSynchronizer & strSynchronizer, StrCompressionElevatorPtr compressionElevator)
{
    if ( strings.size() > Chk::MaxStrings )
        return false;

    try {
        strSynchronizer.syncStringsToBytes(strings, stringBytes, compressionElevator);
        return true;
    } catch ( std::exception & ) {
        return false;
    }
}

bool StrSection::defragment(StrSynchronizer & strSynchronizer, bool matchCapacityToUsage)
{
    size_t nextCandidateStringId = 0;
    size_t numStrings = strings.size();
    std::map<u32, u32> stringIdRemappings;
    for ( size_t i=0; i<numStrings; i++ )
    {
        if ( strings[i] == nullptr )
        {
            for ( size_t j = std::max(i+1, nextCandidateStringId); j < numStrings; j++ )
            {
                if ( strings[j] != nullptr )
                {
                    strings[i] = strings[j];
                    stringIdRemappings.insert(std::pair<u32, u32>((u32)j, (u32)i));
                    break;
                }
            }
        }
    }

    if ( !stringIdRemappings.empty() )
    {
        strSynchronizer.remapStringIds(stringIdRemappings, Chk::Scope::Game);
        return true;
    }
    return false;
}

std::vector<u8> & StrSection::getTailData()
{
    return tailData;
}

size_t StrSection::getTailDataOffset(StrSynchronizer & strSynchronizer)
{
    strSynchronizer.syncStringsToBytes(strings, stringBytes);
    return stringBytes.size();
}

size_t StrSection::getInitialTailDataOffset()
{
    return initialTailDataOffset;
}

size_t StrSection::getBytePaddedTo()
{
    return bytePaddedTo;
}
        
void StrSection::setBytePaddedTo(size_t bytePaddedTo)
{
    this->bytePaddedTo = bytePaddedTo;
}

StrSectionPtr StrSection::backup()
{
    StrSectionPtr backup = StrSectionPtr(new StrSection(*this));
    return backup;
}

void StrSection::restore(StrSectionPtr backup)
{
    if ( backup != nullptr )
    {
        strings.swap(backup->strings);
        stringBytes.swap(backup->stringBytes);
        bytePaddedTo = backup->bytePaddedTo;
        initialTailDataOffset = backup->initialTailDataOffset;
        tailData.swap(backup->tailData);
    }
}

Chk::SectionSize StrSection::getSize(ScenarioSaver & scenarioSaver)
{
    if ( syncStringsToBytes(scenarioSaver) )
    {
        size_t totalSize = stringBytes.size();
        if ( tailData.size() > 0 )
        {
            if ( (bytePaddedTo == 2 || bytePaddedTo == 4) && totalSize%bytePaddedTo != 0 )
                totalSize += bytePaddedTo - totalSize%bytePaddedTo;

            totalSize += tailData.size();
        }

        if ( totalSize <= ChkSection::MaxChkSectionSize )
            return (Chk::SectionSize)totalSize;
        else
            throw MaxSectionSizeExceeded(SectionName::STR, std::to_string(stringBytes.size()));
    }
    else
        throw StrSerializationFailure();
}

std::streamsize StrSection::read(const Chk::SectionHeader & sectionHeader, std::istream & is, bool overriding)
{
    if ( sectionHeader.sizeInBytes < 0 )
        throw NegativeSectionSize(sectionHeader.name);

    size_t readSize = size_t(sectionHeader.sizeInBytes);
    if ( readSize > 0 )
    {
        if ( overriding && tailData.size() > 0 ) // Need to dump tail data back into stringBytes before reading the overriding data
        {
            // After reading the override there may be different tail data/offsets or no tail data at all
            stringBytes.insert(stringBytes.end(), tailData.begin(), tailData.end());
            tailData.clear();
            initialTailDataOffset = 0;
            bytePaddedTo = 4;
        }

        if ( readSize > stringBytes.size() ) // Need to expand section
            stringBytes.insert(stringBytes.end(), readSize-stringBytes.size(), u8(0));
        else if ( !overriding && readSize < stringBytes.size()  ) // Need to shrink section
            stringBytes.erase(stringBytes.begin()+readSize, stringBytes.end());
    
        std::memset(&stringBytes[0], 0, readSize); // Zero out the bytes about to be read
        is.read((char*)&stringBytes[0], std::streamsize(sectionHeader.sizeInBytes));
        syncBytesToStrings();
        return is.gcount();
    }
    else if ( !overriding )
    {
        stringBytes.clear();
        strings.clear();
        tailData.clear();
        initialTailDataOffset = 0;
        bytePaddedTo = 4;
    }
    return 0;
}

void StrSection::write(std::ostream & os, ScenarioSaver & scenarioSaver)
{
    if ( syncStringsToBytes(scenarioSaver) )
    {
        size_t stringBytesSize = stringBytes.size();
        os.write((const char*)&stringBytes[0], (std::streamsize)stringBytesSize);
        if ( tailData.size() > 0 )
        {
            if ( bytePaddedTo == 2 || bytePaddedTo == 4 )
            {
                const char padding[] = { '\0', '\0', '\0', '\0' };
                if ( stringBytesSize%bytePaddedTo != 0 )
                {
                    logger.info() << "Writing " << (bytePaddedTo-stringBytesSize%bytePaddedTo) << " bytes of padding after the regular STR section data" << std::endl;
                    os.write(padding, std::streamsize(bytePaddedTo-stringBytesSize%bytePaddedTo));
                }
            }
            logger.info() << "Writing " << tailData.size() << " bytes of data after the regular STR section data" << std::endl;
            os.write((const char*)&tailData[0], (std::streamsize)tailData.size());
        }
    }
}

size_t StrSection::getNextUnusedStringId(std::bitset<Chk::MaxStrings> & stringIdUsed, bool checkBeyondCapacity, size_t firstChecked)
{
    size_t limit = checkBeyondCapacity ? Chk::MaxStrings : strings.size();
    for ( size_t i=firstChecked; i<limit; i++ )
    {
        if ( !stringIdUsed[i] )
            return i;
    }
    return 0;
}

void StrSection::resolveParantage()
{
    for ( auto string = strings.begin(); string != strings.end(); ++string )
    {
        auto otherString = string;
        ++otherString;
        for ( ; otherString != strings.end(); ++otherString )
            ScStr::adopt(*string, *otherString);
    }
}

void StrSection::resolveParantage(ScStrPtr string)
{
    if ( string != nullptr )
    {
        string->disown();
        for ( auto it = strings.begin(); it != strings.end(); ++it )
        {
            ScStrPtr otherString = *it;
            if ( otherString != nullptr && otherString != string )
                ScStr::adopt(string, otherString);
        }
    }
}

bool StrSection::stringsMatchBytes()
{
    if ( stringBytes.size() == 0 )
        return false;

    size_t numBytes = stringBytes.size();
    u16 rawNumStrings = numBytes > 2 ? (u16 &)stringBytes[0] : numBytes == 1 ? (u16)stringBytes[0] : 0;
    if ( size_t(rawNumStrings) != strings.size()-1 )
        return false; // Size mismatch

    for ( size_t stringId = 1; stringId<strings.size(); ++stringId )
    {
        ScStrPtr scStr = strings[stringId];
        if ( scStr != nullptr )
        {
            size_t offsetPos = sizeof(u16)*stringId;
            if ( offsetPos+1 < numBytes )
            {
                u16 stringOffset = (u16 &)stringBytes[offsetPos];
                if ( size_t(stringOffset)+scStr->length() > numBytes || std::memcmp(scStr->str, &stringBytes[stringOffset], scStr->length()) != 0 )
                    return false; // String out of bounds or not equal
            }
            else // String offset out of bounds
                return false;
        }
    }
    return true;
}

bool StrSection::syncStringsToBytes(ScenarioSaver & scenarioSaver)
{
    auto strSynchronizer = scenarioSaver.getStrSynchronizer();
    if ( strSynchronizer != nullptr )
        strSynchronizer->syncStringsToBytes(strings, stringBytes);
    else
    {
        constexpr size_t maxStrings = (size_t(u16_max) - sizeof(u16))/sizeof(u16);
        size_t numStrings = strings.size() > 0 ? strings.size()-1 : 0; // Exclude string at index 0
        if ( numStrings > maxStrings )
            throw MaximumStringsExceeded(ChkSection::getNameString(SectionName::STR), numStrings, maxStrings);
        
        size_t sizeAndOffsetSpaceAndNulSpace = sizeof(u16) + sizeof(u16)*numStrings + 1;
        size_t sectionSize = sizeAndOffsetSpaceAndNulSpace;
        for ( size_t i=1; i<=numStrings; i++ )
        {
            if ( strings[i] != nullptr )
                sectionSize += strings[i]->length();
        }

        constexpr size_t maxStandardSize = u16_max;
        if ( sectionSize > maxStandardSize )
            throw MaximumCharactersExceeded(ChkSection::getNameString(SectionName::STR), sectionSize-sizeAndOffsetSpaceAndNulSpace, maxStandardSize);

        stringBytes.assign(sizeof(u16)+sizeof(u16)*numStrings, u8(0));
        (u16 &)stringBytes[0] = (u16)numStrings;
        u16 initialNulOffset = u16(stringBytes.size());
        stringBytes.push_back(u8('\0')); // Add initial NUL character
        for ( size_t i=1; i<=numStrings; i++ )
        {
            if ( strings[i] == nullptr )
                (u16 &)stringBytes[sizeof(u16)*i] = initialNulOffset;
            else
            {
                (u16 &)stringBytes[sizeof(u16)*i] = u16(stringBytes.size());
                stringBytes.insert(stringBytes.end(), strings[i]->str, strings[i]->str+(strings[i]->length()+1));
            }
        }
    }
    return true;
}

void StrSection::syncBytesToStrings()
{
    size_t numBytes = stringBytes.size();
    u16 rawNumStrings = numBytes >= 2 ? (u16 &)stringBytes[0] : numBytes == 1 ? (u16)stringBytes[0] : 0;
    size_t highestStringWithValidOffset = std::min(size_t(rawNumStrings), numBytes < 4 ? 0 : numBytes/2-1);
    strings.clear();
    strings.push_back(nullptr); // Fill the non-existant 0th stringId

    size_t stringId = 1;
    size_t sectionLastCharacter = 0;
    for ( ; stringId <= highestStringWithValidOffset; ++stringId )
    {
        size_t offsetPos = sizeof(u16)*stringId;
        size_t stringOffset = size_t((u16 &)stringBytes[offsetPos]);
        size_t lastCharacter = loadString(stringOffset, numBytes);

        if ( lastCharacter > sectionLastCharacter )
            sectionLastCharacter = lastCharacter;
    }
    if ( highestStringWithValidOffset < size_t(rawNumStrings) ) // Some offsets aren't within bounds
    {
        if ( numBytes % 2 > 0 ) // Can read one byte of an offset
        {
            stringId ++;
            size_t stringOffset = size_t((u16)stringBytes[numBytes-1]);
            loadString(stringOffset, numBytes);
        }
        for ( ; stringId <= size_t(rawNumStrings); ++stringId ) // Any remaining strings are fully out of bounds
            strings.push_back(nullptr);
    }

    size_t offsetsEnd = sizeof(u16) + sizeof(u16)*rawNumStrings;
    size_t charactersEnd = sectionLastCharacter+1;
    size_t regularStrSectionEnd = std::max(offsetsEnd, charactersEnd);
    if ( regularStrSectionEnd < numBytes ) // Tail data exists starting at regularStrSectionEnd
    {
        bytePaddedTo = 0;
        initialTailDataOffset = regularStrSectionEnd;
        auto tailStart = std::next(stringBytes.begin(), regularStrSectionEnd);
        auto tailEnd = stringBytes.end();
        tailData.assign(tailStart, tailEnd);
        stringBytes.erase(tailStart, tailEnd);
        logger.info() << "Read " << tailData.size() << " bytes of tailData after the STR section" << std::endl;
    }
    else // No tail data exists
    {
        bytePaddedTo = 4;
        initialTailDataOffset = 0;
        tailData.clear();
    }
}

size_t StrSection::loadString(const size_t & stringOffset, const size_t & sectionSize)
{
    if ( stringOffset < sectionSize )
    {
        auto nextNull = std::find(stringBytes.begin()+stringOffset, stringBytes.end(), u8('\0'));
        if ( nextNull != stringBytes.end() )
        {
            auto nullIndex = std::distance(stringBytes.begin(), nextNull);
            if ( size_t(nullIndex) >= stringOffset ) // Regular string
            {
                strings.push_back(ScStrPtr(new ScStr(std::string((const char*)&stringBytes[stringOffset]))));
                return nullIndex;
            }
            else // String ends where section ends
            {
                strings.push_back(ScStrPtr(new ScStr(std::string((const char*)&stringBytes[stringOffset], sectionSize-stringOffset))));
                return sectionSize-1;
            }
        }
        else if ( sectionSize > stringOffset ) // String ends where section ends
        {
            strings.push_back(ScStrPtr(new ScStr(std::string((const char*)&stringBytes[stringOffset], sectionSize-stringOffset))));
            return sectionSize-1;
        }
        else // Character data would be out of bounds
            strings.push_back(nullptr);
    }
    else // Offset is out of bounds
        strings.push_back(nullptr);

    return 0;
}

UprpSectionPtr UprpSection::GetDefault()
{
    return UprpSectionPtr(new (std::nothrow) UprpSection(Chk::UPRP {}));
}

UprpSection::UprpSection(const Chk::UPRP & data) : StructSection<Chk::UPRP, false>(SectionName::UPRP, data)
{

}

UprpSection::UprpSection() : StructSection<Chk::UPRP, false>(SectionName::UPRP)
{

}

UprpSection::~UprpSection()
{

}

Chk::Cuwp UprpSection::getCuwp(size_t cuwpIndex)
{
    return data->createUnitProperties[cuwpIndex];
}

void UprpSection::setCuwp(size_t cuwpIndex, const Chk::Cuwp & cuwp)
{
    if ( cuwpIndex < Sc::Unit::MaxCuwps )
        memcpy(&data->createUnitProperties[cuwpIndex], &cuwp, sizeof(Chk::Cuwp));
}

size_t UprpSection::findCuwp(const Chk::Cuwp & cuwp)
{
    for ( size_t i = 0; i < Sc::Unit::MaxCuwps; i++ )
    {
        if ( memcmp(&cuwp, &data->createUnitProperties[i], sizeof(Chk::Cuwp)) == 0 )
            return i;
    }
    return Sc::Unit::MaxCuwps;
}

UpusSectionPtr UpusSection::GetDefault()
{
    return UpusSectionPtr(new (std::nothrow) UpusSection(Chk::UPUS {}));
}

UpusSection::UpusSection(const Chk::UPUS & data) : StructSection<Chk::UPUS, true>(SectionName::UPUS, data)
{

}

UpusSection::UpusSection() : StructSection<Chk::UPUS, true>(SectionName::UPUS)
{

}

UpusSection::~UpusSection()
{

}

bool UpusSection::cuwpUsed(size_t cuwpIndex)
{
    if ( cuwpIndex < Sc::Unit::MaxCuwps )
        return data->cuwpUsed[cuwpIndex] == Chk::CuwpUsed::No ? false : true;
    else
        return false;
}

void UpusSection::setCuwpUsed(size_t cuwpIndex, bool cuwpUsed)
{
    if ( cuwpIndex < Sc::Unit::MaxCuwps )
        data->cuwpUsed[cuwpIndex] = cuwpUsed ? Chk::CuwpUsed::Yes : Chk::CuwpUsed::No;
}

size_t UpusSection::getNextUnusedCuwpIndex()
{
    for ( size_t i = 0; i < Sc::Unit::MaxCuwps; i++ )
    {
        if ( data->cuwpUsed[i] == Chk::CuwpUsed::No )
            return i;
    }
    return Sc::Unit::MaxCuwps;
}

MrgnSectionPtr MrgnSection::GetDefault(u16 tileWidth, u16 tileHeight)
{
    MrgnSectionPtr newSection(new (std::nothrow) MrgnSection());

    newSection->locations.push_back(nullptr); // Index 0 is unused

    for ( size_t i=1; i<Chk::LocationId::Anywhere; i++ )
        newSection->locations.push_back(Chk::LocationPtr(new Chk::Location()));
    
    Chk::LocationPtr anywhere = Chk::LocationPtr(new Chk::Location());
    anywhere->right = (s32)tileWidth*32;
    anywhere->bottom = (s32)tileHeight*32;
    anywhere->stringId = 3;
    newSection->locations.push_back(Chk::LocationPtr(anywhere));

    for ( size_t i=Chk::LocationId::Anywhere+1; i<=Chk::TotalLocations; i++ )
        newSection->locations.push_back(Chk::LocationPtr(new Chk::Location()));

    return newSection;
}

MrgnSection::MrgnSection() : DynamicSection<false>(SectionName::MRGN)
{

}

MrgnSection::~MrgnSection()
{

}

size_t MrgnSection::numLocations()
{
    return locations.size() > 0 ? locations.size()-1 : 0;
}

std::shared_ptr<Chk::Location> MrgnSection::getLocation(size_t locationId)
{
    return locations[locationId];
}

size_t MrgnSection::addLocation(std::shared_ptr<Chk::Location> location)
{
    for ( size_t i=1; i<locations.size(); i++ )
    {
        if ( isBlank(i) )
        {
            locations[i] = location;
            return i;
        }
    }
    return Chk::LocationId::NoLocation;
}

void MrgnSection::replaceLocation(size_t locationId, std::shared_ptr<Chk::Location> location)
{
    if ( locationId > 0 && locationId < locations.size() )
    {
        if ( isBlank(locationId) )
            locations[locationId] = location;
    }
    else
        throw std::out_of_range(std::string("LocationId: ") + std::to_string((u32)locationId) + " is out of range for the MRGN section!");
}

void MrgnSection::deleteLocation(size_t locationId)
{
    if ( locationId > 0 && locationId < locations.size() )
    {
        auto location = std::next(locations.begin(), locationId);
        (*location)->stringId = 0;
        (*location)->left = 0;
        (*location)->right = 0;
        (*location)->top = 0;
        (*location)->bottom = 0;
        (*location)->elevationFlags = 0;
    }
}

bool MrgnSection::moveLocation(size_t locationIdFrom, size_t locationIdTo, bool lockAnywhere)
{
    size_t locationIdMin = std::min(locationIdFrom, locationIdTo);
    size_t locationIdMax = std::max(locationIdFrom, locationIdTo);
    if ( locationIdFrom > 0 && locationIdTo > 0 && locationIdMax < locations.size() && locationIdFrom != locationIdTo &&
         (!lockAnywhere || (locationIdMin != Chk::LocationId::Anywhere && locationIdMax != Chk::LocationId::Anywhere)) )
    {
        if ( locationIdMax-locationIdMin == 1 && locationIdMax < locations.size() ) // Move up or down by 1 using swap
        {
            locations[locationIdMin].swap(locations[locationIdMax]);
            return true;
        }
        else // Move up or down by more than one, remove from present location, insert in the list at destination
        {
            auto location = locations[locationIdFrom];
            auto toErase = std::next(locations.begin(), locationIdFrom);
            locations.erase(toErase);
            auto insertPosition = std::next(locations.begin(), locationIdTo-1);
            locations.insert(insertPosition, location);

            if ( lockAnywhere && locationIdMin < Chk::LocationId::Anywhere && locationIdMax > Chk::LocationId::Anywhere )
            {
                std::swap(*std::next(locations.begin(), Chk::LocationId::Anywhere-1), *std::next(locations.begin(), Chk::LocationId::Anywhere));
                return true;
            }
        }
    }
    return false;
}

bool MrgnSection::isBlank(size_t locationId)
{
    if ( locationId > 0 && locationId < locations.size() )
    {
        auto & location = locations[locationId];
        return location == nullptr || location->isBlank();
    }
    return false;
}

bool MrgnSection::stringUsed(size_t stringId)
{
    u16 u16StringId = (u16)stringId;
    for ( size_t i=1; i<locations.size(); i++ )
    {
        auto location = locations[i];
        if ( location != nullptr && location->stringId == u16StringId )
            return true;
    }
    return false;
}

void MrgnSection::markNonZeroLocations(std::bitset<Chk::TotalLocations+1> & locationIdUsed)
{
    size_t limit = std::min(Chk::TotalLocations, locations.size());
    for ( size_t i=1; i<limit; i++ )
    {
        auto & location = locations[i];
        if ( location != nullptr && !location->isBlank() )
            locationIdUsed[i] = true;
    }
}

void MrgnSection::markUsedStrings(std::bitset<Chk::MaxStrings> & stringIdUsed)
{
    for ( size_t i=1; i<locations.size(); i++ )
    {
        auto & location = locations[i];
        if ( location->stringId > 0 )
            stringIdUsed[location->stringId] = true;
    }
}

void MrgnSection::remapStringIds(const std::map<u32, u32> & stringIdRemappings)
{
    for ( size_t i=1; i<locations.size(); i++ )
    {
        auto & location = locations[i];
        auto found = stringIdRemappings.find(location->stringId);
        if ( found != stringIdRemappings.end() )
            location->stringId = found->second;
    }
}

void MrgnSection::deleteString(size_t stringId)
{
    for ( size_t i=1; i<locations.size(); i++ )
    {
        auto & location = locations[i];
        if ( location->stringId == stringId )
            location->stringId = 0;
    }
}

bool MrgnSection::locationsFitOriginal(LocationSynchronizer & locationSynchronizer, bool lockAnywhere, bool autoDefragment)
{
    std::bitset<Chk::TotalLocations+1> locationIdUsed;
    locationSynchronizer.markUsedLocations(locationIdUsed);
    markNonZeroLocations(locationIdUsed);

    size_t countUsedOrCreated = 0;
    for ( size_t i=1; i<=Chk::TotalLocations; i++ )
    {
        if ( locationIdUsed[i] || (i == Chk::LocationId::Anywhere && lockAnywhere) )
            countUsedOrCreated++;
    }

    return countUsedOrCreated <= Chk::TotalOriginalLocations;
}

bool MrgnSection::trimToOriginal(LocationSynchronizer & locationSynchronizer, bool lockAnywhere, bool autoDefragment)
{
    if ( locations.size() > Chk::TotalOriginalLocations )
    {
        std::bitset<Chk::TotalLocations+1> locationIdUsed;
        locationSynchronizer.markUsedLocations(locationIdUsed);
        markNonZeroLocations(locationIdUsed);

        size_t countUsedOrCreated = 0;
        for ( size_t i=1; i<=Chk::TotalLocations; i++ )
        {
            if ( locationIdUsed[i] || (i == Chk::LocationId::Anywhere && lockAnywhere) )
                countUsedOrCreated++;
        }

        if ( countUsedOrCreated <= Chk::TotalOriginalLocations )
        {
            std::map<u32, u32> locationIdRemappings;
            for ( size_t firstUnused=1; firstUnused<=Chk::TotalLocations; firstUnused++ )
            {
                if ( !locationIdUsed[firstUnused] && (firstUnused != Chk::LocationId::Anywhere || !lockAnywhere) )
                {
                    for ( size_t i=firstUnused+1; i<=Chk::TotalLocations; i++ )
                    {
                        if ( locationIdUsed[i] && (i != Chk::LocationId::Anywhere || !lockAnywhere) )
                        {
                            locations[firstUnused] = locations[i];
                            locationIdUsed[firstUnused] = true;
                            locations[i] = Chk::LocationPtr(new Chk::Location());
                            locationIdUsed[i] = false;
                            locationIdRemappings.insert(std::pair<u32, u32>(u32(i), u32(firstUnused)));
                            break;
                        }
                    }
                }
            }
            locations.erase(locations.begin()+Chk::TotalOriginalLocations+1, locations.end());
            locationSynchronizer.remapLocationIds(locationIdRemappings);
            return true;
        }
    }
    return false;
}

void MrgnSection::expandToScHybridOrExpansion()
{
    size_t numLocations = locations.size();
    for ( size_t i=numLocations; i<=Chk::TotalLocations; i++ )
        locations.push_back(std::shared_ptr<Chk::Location>(new Chk::Location()));
}

Chk::SectionSize MrgnSection::getSize(ScenarioSaver & scenarioSaver)
{
    return Chk::SectionSize(sizeof(Chk::Location) * (locations.size()-1));
}

std::streamsize MrgnSection::read(const Chk::SectionHeader & sectionHeader, std::istream & is, bool append)
{
    if ( sectionHeader.sizeInBytes < 0 )
        throw NegativeSectionSize(sectionHeader.name);

    size_t readSize = size_t(sectionHeader.sizeInBytes);
    if ( readSize > 0 )
    {
        size_t readNumLocations = readSize/sizeof(Chk::Location) + readSize%sizeof(Chk::Location);
        std::vector<Chk::Location> locationBuffer(readNumLocations);
        is.read((char*)&locationBuffer[0], std::streamsize(sectionHeader.sizeInBytes));

        if ( append )
        {
            for ( auto & location : locationBuffer )
                locations.push_back(Chk::LocationPtr(new Chk::Location(location)));
        }
        else
        {
            locations.assign(1, nullptr);
            for ( auto & location : locationBuffer )
                locations.push_back(Chk::LocationPtr(new Chk::Location(location)));
        }
        return is.gcount();
    }
    else if ( !append )
        locations.assign(1, nullptr);

    return 0;
}

void MrgnSection::write(std::ostream & os, ScenarioSaver & scenarioSaver)
{
    for ( size_t i=1; i<locations.size(); i++ )
        os.write((const char*)locations[i].get(), std::streamsize(sizeof(Chk::Location)));
}

TrigSectionPtr TrigSection::GetDefault()
{
    return TrigSectionPtr(new (std::nothrow) TrigSection());
}

TrigSection::TrigSection() : DynamicSection<false>(SectionName::TRIG)
{

}

TrigSection::~TrigSection()
{

}

size_t TrigSection::numTriggers()
{
    return triggers.size();
}

std::shared_ptr<Chk::Trigger> TrigSection::getTrigger(size_t triggerIndex)
{
    return triggers[triggerIndex];
}

size_t TrigSection::addTrigger(std::shared_ptr<Chk::Trigger> trigger)
{
    triggers.push_back(trigger);
    return triggers.size()-1;
}

void TrigSection::insertTrigger(size_t triggerIndex, std::shared_ptr<Chk::Trigger> trigger)
{
    if ( triggerIndex < triggers.size() )
    {
        auto position = std::next(triggers.begin(), triggerIndex);
        triggers.insert(position, trigger);
    }
    else if ( triggerIndex == triggers.size() )
        triggers.push_back(trigger);
}

void TrigSection::deleteTrigger(size_t triggerIndex)
{
    if ( triggerIndex < triggers.size() )
    {
        auto trigger = std::next(triggers.begin(), triggerIndex);
        triggers.erase(trigger);
    }
}

void TrigSection::moveTrigger(size_t triggerIndexFrom, size_t triggerIndexTo)
{
    size_t triggerIndexMin = std::min(triggerIndexFrom, triggerIndexTo);
    size_t triggerIndexMax = std::max(triggerIndexFrom, triggerIndexTo);
    if ( triggerIndexMax < triggers.size() && triggerIndexFrom != triggerIndexTo )
    {
        if ( triggerIndexMax-triggerIndexMin == 1 && triggerIndexMax < triggers.size() ) // Move up or down by 1 using swap
            triggers[triggerIndexMin].swap(triggers[triggerIndexMax]);
        else // Move up or down by more than one, remove from present location, insert in the list at destination
        {
            auto trigger = triggers[triggerIndexFrom];
            auto toErase = std::next(triggers.begin(), triggerIndexFrom);
            triggers.erase(toErase);
            auto insertPosition = std::next(triggers.begin(), triggerIndexTo-1);
            triggers.insert(insertPosition, trigger);
        }
    }
}

void TrigSection::swap(std::deque<std::shared_ptr<Chk::Trigger>> & triggers)
{
    this->triggers.swap(triggers);
}

std::deque<Chk::TriggerPtr> TrigSection::replaceRange(size_t beginIndex, size_t endIndex, std::deque<Chk::TriggerPtr> & triggers)
{
    if ( beginIndex == 0 && endIndex == triggers.size() )
    {
        this->triggers.swap(triggers);
        return triggers;
    }
    else if ( beginIndex < endIndex && endIndex <= this->triggers.size() )
    {
        auto begin = this->triggers.begin()+beginIndex;
        auto end = this->triggers.begin()+endIndex;
        std::deque<Chk::TriggerPtr> replacedTriggers(this->triggers.begin()+beginIndex, this->triggers.begin()+endIndex);
        this->triggers.erase(begin, end);
        this->triggers.insert(this->triggers.begin()+beginIndex, triggers.begin(), triggers.end());
        return replacedTriggers;
    }
    else
        throw std::out_of_range(std::string("Range [") + std::to_string(beginIndex) + ", " + std::to_string(endIndex) +
            ") is invalid for trigger list of size: " + std::to_string(triggers.size()));
}

bool TrigSection::locationUsed(size_t locationId)
{
    for ( auto trigger : triggers )
    {
        if ( trigger->locationUsed(locationId) )
            return true;
    }
    return false;
}

bool TrigSection::stringUsed(size_t stringId, u32 userMask)
{
    for ( auto trigger : triggers )
    {
        if ( trigger->stringUsed(stringId, userMask) )
            return true;
    }
    return false;
}

bool TrigSection::gameStringUsed(size_t stringId, u32 userMask)
{
    for ( auto trigger : triggers )
    {
        if ( trigger->gameStringUsed(stringId, userMask) )
            return true;
    }
    return false;
}

bool TrigSection::commentStringUsed(size_t stringId)
{
    for ( auto trigger : triggers )
    {
        if ( trigger->commentStringUsed(stringId) )
            return true;
    }
    return false;
}

void TrigSection::markUsedLocations(std::bitset<Chk::TotalLocations+1> & locationIdUsed)
{
    for ( auto trigger : triggers )
        trigger->markUsedLocations(locationIdUsed);
}

void TrigSection::markUsedStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask)
{
    for ( auto trigger : triggers )
        trigger->markUsedStrings(stringIdUsed, userMask);
}

void TrigSection::markUsedGameStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask)
{
    for ( auto trigger : triggers )
        trigger->markUsedGameStrings(stringIdUsed, userMask);
}

void TrigSection::markUsedCommentStrings(std::bitset<Chk::MaxStrings> & stringIdUsed)
{
    for ( auto trigger : triggers )
        trigger->markUsedCommentStrings(stringIdUsed);
}

void TrigSection::remapLocationIds(const std::map<u32, u32> & locationIdRemappings)
{
    for ( auto trigger : triggers )
        trigger->remapLocationIds(locationIdRemappings);
}

void TrigSection::remapStringIds(const std::map<u32, u32> & stringIdRemappings)
{
    for ( auto trigger : triggers )
        trigger->remapStringIds(stringIdRemappings);
}

void TrigSection::deleteLocation(size_t locationId)
{
    for ( auto trigger : triggers )
        trigger->deleteLocation(locationId);
}

void TrigSection::deleteString(size_t stringId)
{
    for ( auto trigger : triggers )
        trigger->deleteString(stringId);
}

Chk::SectionSize TrigSection::getSize(ScenarioSaver &)
{
    return Chk::SectionSize(sizeof(Chk::Trigger) * triggers.size());
}

std::streamsize TrigSection::read(const Chk::SectionHeader & sectionHeader, std::istream & is, bool append)
{
    if ( sectionHeader.sizeInBytes < 0 )
        throw NegativeSectionSize(sectionHeader.name);

    size_t readSize = size_t(sectionHeader.sizeInBytes);
    if ( readSize > 0 )
    {
        size_t readNumTriggers = readSize/sizeof(Chk::Trigger) + readSize%sizeof(Chk::Trigger);
        std::vector<Chk::Trigger> triggerBuffer(readNumTriggers);
        is.read((char*)&triggerBuffer[0], std::streamsize(sectionHeader.sizeInBytes));

        if ( append )
        {
            for ( Chk::Trigger & trigger : triggerBuffer )
                triggers.push_back(Chk::TriggerPtr(new Chk::Trigger(trigger)));
        }
        else
        {
            triggers.clear();
            for ( Chk::Trigger & trigger : triggerBuffer )
                triggers.push_back(Chk::TriggerPtr(new Chk::Trigger(trigger)));
        }
        return is.gcount();
    }
    else if ( !append )
        triggers.clear();

    return 0;
}

void TrigSection::write(std::ostream & os, ScenarioSaver &)
{
    for ( auto trigger : triggers )
        os.write((const char*)trigger.get(), std::streamsize(sizeof(Chk::Trigger)));
}

MbrfSectionPtr MbrfSection::GetDefault()
{
    return MbrfSectionPtr(new (std::nothrow) MbrfSection());
}

MbrfSection::MbrfSection() : DynamicSection<false>(SectionName::MBRF)
{

}

MbrfSection::~MbrfSection()
{

}

size_t MbrfSection::numBriefingTriggers()
{
    return briefingTriggers.size();
}

std::shared_ptr<Chk::Trigger> MbrfSection::getBriefingTrigger(size_t briefingTriggerIndex)
{
    return briefingTriggers[briefingTriggerIndex];
}

size_t MbrfSection::addBriefingTrigger(std::shared_ptr<Chk::Trigger> briefingTrigger)
{
    briefingTriggers.push_back(briefingTrigger);
    return briefingTriggers.size()-1;
}

void MbrfSection::insertBriefingTrigger(size_t briefingTriggerIndex, std::shared_ptr<Chk::Trigger> briefingTrigger)
{
    if ( briefingTriggerIndex < briefingTriggers.size() )
    {
        auto position = std::next(briefingTriggers.begin(), briefingTriggerIndex);
        briefingTriggers.insert(position, briefingTrigger);
    }
    else if ( briefingTriggerIndex == briefingTriggers.size() )
        briefingTriggers.push_back(briefingTrigger);
}

void MbrfSection::deleteBriefingTrigger(size_t briefingTriggerIndex)
{
    if ( briefingTriggerIndex < briefingTriggers.size() )
    {
        auto briefingTrigger = std::next(briefingTriggers.begin(), briefingTriggerIndex);
        briefingTriggers.erase(briefingTrigger);
    }
}

void MbrfSection::moveBriefingTrigger(size_t briefingTriggerIndexFrom, size_t briefingTriggerIndexTo)
{
    size_t briefingTriggerIndexMin = std::min(briefingTriggerIndexFrom, briefingTriggerIndexTo);
    size_t briefingTriggerIndexMax = std::max(briefingTriggerIndexFrom, briefingTriggerIndexTo);
    if ( briefingTriggerIndexMax < briefingTriggers.size() && briefingTriggerIndexFrom != briefingTriggerIndexTo )
    {
        if ( briefingTriggerIndexMax-briefingTriggerIndexMin == 1 && briefingTriggerIndexMax < briefingTriggers.size() ) // Move up or down by 1 using swap
            briefingTriggers[briefingTriggerIndexMin].swap(briefingTriggers[briefingTriggerIndexMax]);
        else // Move up or down by more than one, remove from present location, insert in the list at destination
        {
            auto briefingTrigger = briefingTriggers[briefingTriggerIndexFrom];
            auto toErase = std::next(briefingTriggers.begin(), briefingTriggerIndexFrom);
            briefingTriggers.erase(toErase);
            auto insertPosition = std::next(briefingTriggers.begin(), briefingTriggerIndexTo-1);
            briefingTriggers.insert(insertPosition, briefingTrigger);
        }
    }
}

bool MbrfSection::stringUsed(size_t stringId, u32 userMask)
{
    for ( auto briefingTrigger : briefingTriggers )
    {
        if ( briefingTrigger->briefingStringUsed(stringId, userMask) )
            return true;
    }
    return false;
}

void MbrfSection::markUsedStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask)
{
    for ( auto briefingTrigger : briefingTriggers )
        briefingTrigger->markUsedBriefingStrings(stringIdUsed, userMask);
}

void MbrfSection::remapStringIds(const std::map<u32, u32> & stringIdRemappings)
{
    for ( auto briefingTrigger : briefingTriggers )
        briefingTrigger->remapBriefingStringIds(stringIdRemappings);
}

void MbrfSection::deleteString(size_t stringId)
{
    for ( auto briefingTrigger : briefingTriggers )
        briefingTrigger->deleteString(stringId);
}

Chk::SectionSize MbrfSection::getSize(ScenarioSaver &)
{
    return Chk::SectionSize(sizeof(Chk::Trigger) * briefingTriggers.size());
}

std::streamsize MbrfSection::read(const Chk::SectionHeader & sectionHeader, std::istream & is, bool append)
{
    if ( sectionHeader.sizeInBytes < 0 )
        throw NegativeSectionSize(sectionHeader.name);

    size_t readSize = size_t(sectionHeader.sizeInBytes);
    if ( readSize > 0 )
    {
        size_t readNumTriggers = readSize/sizeof(Chk::Trigger) + readSize%sizeof(Chk::Trigger);
        std::vector<Chk::Trigger> triggerBuffer(readNumTriggers);
        is.read((char*)&triggerBuffer[0], std::streamsize(sectionHeader.sizeInBytes));

        if ( append )
        {
            for ( Chk::Trigger & briefingTrigger : triggerBuffer )
                briefingTriggers.push_back(Chk::TriggerPtr(new Chk::Trigger(briefingTrigger)));
        }
        else
        {
            briefingTriggers.clear();
            for ( Chk::Trigger & briefingTrigger : triggerBuffer )
                briefingTriggers.push_back(Chk::TriggerPtr(new Chk::Trigger(briefingTrigger)));
        }
        return is.gcount();
    }
    else if ( !append )
        briefingTriggers.clear();

    return 0;
}

void MbrfSection::write(std::ostream & os, ScenarioSaver &)
{
    for ( auto trigger : briefingTriggers )
        os.write((const char*)trigger.get(), std::streamsize(sizeof(Chk::Trigger)));
}

SprpSectionPtr SprpSection::GetDefault(u16 scenarioNameStringId, u16 scenarioDescriptionStringId)
{
    return SprpSectionPtr(new (std::nothrow) SprpSection(Chk::SPRP {
        scenarioNameStringId, scenarioDescriptionStringId
    }));
}

SprpSection::SprpSection(const Chk::SPRP & data) : StructSection<Chk::SPRP, false>(SectionName::SPRP, data)
{

}

SprpSection::SprpSection() : StructSection<Chk::SPRP, false>(SectionName::SPRP)
{

}

SprpSection::~SprpSection()
{

}

size_t SprpSection::getScenarioNameStringId()
{
    return data->scenarioNameStringId;
}

size_t SprpSection::getScenarioDescriptionStringId()
{
    return data->scenarioDescriptionStringId;
}

void SprpSection::setScenarioNameStringId(u16 scenarioNameStringId)
{
    data->scenarioNameStringId = scenarioNameStringId;
}

void SprpSection::setScenarioDescriptionStringId(u16 scenarioDescriptionStringId)
{
    data->scenarioDescriptionStringId = scenarioDescriptionStringId;
}

bool SprpSection::stringUsed(size_t stringId, u32 userMask)
{
    return ((userMask & Chk::StringUserFlag::ScenarioName) == Chk::StringUserFlag::ScenarioName && data->scenarioNameStringId == (u16)stringId )
        || ((userMask & Chk::StringUserFlag::ScenarioDescription) == Chk::StringUserFlag::ScenarioDescription && data->scenarioDescriptionStringId == (u16)stringId );
}

void SprpSection::deleteString(size_t stringId)
{
    if ( data->scenarioNameStringId == stringId )
        data->scenarioNameStringId = 0;

    if ( data->scenarioDescriptionStringId == stringId )
        data->scenarioDescriptionStringId = 0;
}

void SprpSection::remapStringIds(const std::map<u32, u32> & stringIdRemappings)
{
    auto scenarioNameRemapping = stringIdRemappings.find(data->scenarioNameStringId);
    auto scenarioDescriptionRemapping = stringIdRemappings.find(data->scenarioDescriptionStringId);

    if ( scenarioNameRemapping != stringIdRemappings.end() )
        data->scenarioNameStringId = scenarioNameRemapping->second;

    if ( scenarioDescriptionRemapping != stringIdRemappings.end() )
        data->scenarioDescriptionStringId = scenarioDescriptionRemapping->second;
}

void SprpSection::markUsedStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask)
{
    if ( (userMask & Chk::StringUserFlag::ScenarioName) == Chk::StringUserFlag::ScenarioName && data->scenarioNameStringId > 0 )
        stringIdUsed[data->scenarioNameStringId] = true;

    if ( (userMask & Chk::StringUserFlag::ScenarioDescription) == Chk::StringUserFlag::ScenarioDescription && data->scenarioDescriptionStringId > 0 )
        stringIdUsed[data->scenarioDescriptionStringId] = true;
}


ForcSectionPtr ForcSection::GetDefault()
{
    constexpr u16 Force1String = 4;
    constexpr u16 Force2String = 5;
    constexpr u16 Force3String = 6;
    constexpr u16 Force4String = 7;

    return ForcSectionPtr(new (std::nothrow) ForcSection(Chk::FORC {
        { // playerForce
            Chk::Force::Force1, Chk::Force::Force1, Chk::Force::Force1, Chk::Force::Force1,
            Chk::Force::Force1, Chk::Force::Force1, Chk::Force::Force1, Chk::Force::Force1
        },
        { Force1String, Force2String, Force3String, Force4String }, // forceString
        { Chk::ForceFlags::All, Chk::ForceFlags::All, Chk::ForceFlags::All, Chk::ForceFlags::All } // flags
    }));
}

ForcSection::ForcSection(const Chk::FORC & data) : StructSection<Chk::FORC, false>(SectionName::FORC, data)
{

}

ForcSection::ForcSection() : StructSection<Chk::FORC, false>(SectionName::FORC)
{

}

ForcSection::~ForcSection()
{

}

Chk::Force ForcSection::getPlayerForce(size_t slotIndex)
{
    if ( slotIndex < Sc::Player::TotalSlots )
        return data->playerForce[slotIndex];
    else
        throw std::out_of_range(std::string("SlotIndex: ") + std::to_string((u32)slotIndex) + " is out of range for the FORC section!");
}

size_t ForcSection::getForceStringId(Chk::Force force)
{
    if ( force < Chk::TotalForces )
        return data->forceString[(size_t)force];
    else
        throw std::out_of_range(std::string("Force: ") + std::to_string(force) + " is out of range for the FORC section!");
}

u8 ForcSection::getForceFlags(Chk::Force force)
{
    if ( force < Chk::TotalForces )
        return data->flags[force];
    else
        throw std::out_of_range(std::string("Force: ") + std::to_string(force) + " is out of range for the FORC section!");
}

void ForcSection::setPlayerForce(size_t slotIndex, Chk::Force force)
{
    if ( slotIndex < Sc::Player::TotalSlots )
        data->playerForce[slotIndex] = force;
    else
        throw std::out_of_range(std::string("SlotIndex: ") + std::to_string((u32)slotIndex) + " is out of range for the FORC section!");
}

void ForcSection::setForceStringId(Chk::Force force, u16 forceStringId)
{
    if ( force < Chk::TotalForces )
        data->forceString[force] = forceStringId;
}

void ForcSection::setForceFlags(Chk::Force force, u8 forceFlags)
{
    if ( force < Chk::TotalForces )
        data->flags[force] = forceFlags;
}

bool ForcSection::stringUsed(size_t stringId)
{
    return data->forceString[0] == (u16)stringId || data->forceString[1] == (u16)stringId || data->forceString[2] == (u16)stringId || data->forceString[3] == (u16)stringId;
}

void ForcSection::markUsedStrings(std::bitset<Chk::MaxStrings> & stringIdUsed)
{
    for ( size_t i=0; i<Chk::TotalForces; i++ )
    {
        if ( data->forceString[i] > 0 )
            stringIdUsed[data->forceString[i]] = true;
    }
}

void ForcSection::remapStringIds(const std::map<u32, u32> & stringIdRemappings)
{
    auto forceOneRemapping = stringIdRemappings.find(data->forceString[0]);
    auto forceTwoRemapping = stringIdRemappings.find(data->forceString[1]);
    auto forceThreeRemapping = stringIdRemappings.find(data->forceString[2]);
    auto forceFourRemapping = stringIdRemappings.find(data->forceString[3]);

    if ( forceOneRemapping != stringIdRemappings.end() )
        data->forceString[0] = forceOneRemapping->second;

    if ( forceTwoRemapping != stringIdRemappings.end() )
        data->forceString[1] = forceTwoRemapping->second;

    if ( forceThreeRemapping != stringIdRemappings.end() )
        data->forceString[2] = forceThreeRemapping->second;

    if ( forceFourRemapping != stringIdRemappings.end() )
        data->forceString[3] = forceFourRemapping->second;
}

void ForcSection::deleteString(size_t stringId)
{
    for ( size_t i=0; i<Chk::TotalForces; i++ )
    {
        if ( data->forceString[i] == stringId )
            data->forceString[i] = 0;
    }
}

WavSectionPtr WavSection::GetDefault()
{
    return WavSectionPtr(new (std::nothrow) WavSection(Chk::WAV {}));
}

WavSection::WavSection(const Chk::WAV & data) : StructSection<Chk::WAV, true>(SectionName::WAV, data)
{

}

WavSection::WavSection() : StructSection<Chk::WAV, true>(SectionName::WAV)
{

}

WavSection::~WavSection()
{

}

size_t WavSection::addSound(size_t stringId)
{
    for ( size_t i=0; i<Chk::TotalSounds; i++ )
    {
        if ( data->soundPathStringId[i] == Chk::StringId::UnusedSound )
        {
            data->soundPathStringId[i] = (u32)stringId;
            return i;
        }
    }
    return Chk::TotalSounds;
}

bool WavSection::stringIsSound(size_t stringId)
{
    u32 u32StringId = (u32)stringId;
    for ( size_t i=0; i<Chk::TotalSounds; i++ )
    {
        if ( data->soundPathStringId[i] == u32StringId )
            return true;
    }
    return false;
}

size_t WavSection::getSoundStringId(size_t soundIndex)
{
    if ( soundIndex < Chk::TotalSounds )
        return data->soundPathStringId[soundIndex];
    else
        throw std::out_of_range(std::string("SoundIndex: ") + std::to_string((u32)soundIndex) + " is out of range for the WAV section!");
}

void WavSection::setSoundStringId(size_t soundIndex, size_t soundStringId)
{
    if ( soundIndex < Chk::TotalSounds )
        data->soundPathStringId[soundIndex] = (u32)soundStringId;
}

bool WavSection::stringUsed(size_t stringId)
{
    return stringIsSound(stringId);
}

void WavSection::markUsedStrings(std::bitset<Chk::MaxStrings> & stringIdUsed)
{
    for ( size_t i=0; i<Chk::TotalSounds; i++ )
    {
        if ( data->soundPathStringId[i] > 0 && data->soundPathStringId[i] < Chk::MaxStrings )
            stringIdUsed[data->soundPathStringId[i]] = true;
    }
}

void WavSection::remapStringIds(const std::map<u32, u32> & stringIdRemappings)
{
    for ( size_t i=0; i<Chk::TotalSounds; i++ )
    {
        auto found = stringIdRemappings.find(data->soundPathStringId[i]);
        if ( found != stringIdRemappings.end() )
            data->soundPathStringId[i] = found->second;
    }
}

void WavSection::deleteString(size_t stringId)
{
    for ( size_t i=0; i<Chk::TotalSounds; i++ )
    {
        if ( data->soundPathStringId[i] == stringId )
            data->soundPathStringId[i] = 0;
    }
}


UnisSectionPtr UnisSection::GetDefault()
{
    Chk::UNIS data = {
        {}, // useDefault (all Yes/1, set below)
        {}, // hitpoints (all 0)
        {}, // shieldPoints (all 0)
        {}, // armorPoints (all 0)
        {}, // buildTime (all 0)
        {}, // mineralCost (all 0)
        {}, // gasCost (all 0)
        {}, // nameString (all 0)
        {}, // baseDamage (all 0)
        {}, // upgradeDamage (all 0)
    };
    memset(&data.useDefault, (int)Chk::UseDefault::Yes, Sc::Unit::TotalTypes);
    return UnisSectionPtr(new (std::nothrow) UnisSection(data));
}

UnisSection::UnisSection(const Chk::UNIS & data) : StructSection<Chk::UNIS, false>(SectionName::UNIS, data)
{

}

UnisSection::UnisSection() : StructSection<Chk::UNIS, false>(SectionName::UNIS)
{

}

UnisSection::~UnisSection()
{

}

bool UnisSection::unitUsesDefaultSettings(Sc::Unit::Type unitType)
{
    if ( unitType < Sc::Unit::TotalTypes )
        return data->useDefault[unitType] != Chk::UseDefault::No;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS section!");
}

u32 UnisSection::getUnitHitpoints(Sc::Unit::Type unitType)
{
    if ( unitType < Sc::Unit::TotalTypes )
        return data->hitpoints[unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS section!");
}

u16 UnisSection::getUnitShieldPoints(Sc::Unit::Type unitType)
{
    if ( unitType < Sc::Unit::TotalTypes )
        return data->shieldPoints[unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS section!");
}

u8 UnisSection::getUnitArmorLevel(Sc::Unit::Type unitType)
{
    if ( unitType < Sc::Unit::TotalTypes )
        return data->armorLevel[unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS section!");
}

u16 UnisSection::getUnitBuildTime(Sc::Unit::Type unitType)
{
    if ( unitType < Sc::Unit::TotalTypes )
        return data->buildTime[unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS section!");
}

u16 UnisSection::getUnitMineralCost(Sc::Unit::Type unitType)
{
    if ( unitType < Sc::Unit::TotalTypes )
        return data->mineralCost[unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS section!");
}

u16 UnisSection::getUnitGasCost(Sc::Unit::Type unitType)
{
    if ( unitType < Sc::Unit::TotalTypes )
        return data->gasCost[unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS section!");
}

size_t UnisSection::getUnitNameStringId(Sc::Unit::Type unitType)
{
    if ( unitType < Sc::Unit::TotalTypes )
        return data->nameStringId[unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS section!");
}

u16 UnisSection::getWeaponBaseDamage(Sc::Weapon::Type weaponType)
{
    if ( (size_t)weaponType < Sc::Weapon::TotalOriginal )
        return data->baseDamage[weaponType];
    else
        throw std::out_of_range(std::string("WeaponType: ") + std::to_string((size_t)weaponType) + " is out of range for the UNIS section!");
}

u16 UnisSection::getWeaponUpgradeDamage(Sc::Weapon::Type weaponType)
{
    if ( (size_t)weaponType < Sc::Weapon::TotalOriginal )
        return data->upgradeDamage[weaponType];
    else
        throw std::out_of_range(std::string("WeaponType: ") + std::to_string((size_t)weaponType) + " is out of range for the UNIS section!");
}

void UnisSection::setUnitUsesDefaultSettings(Sc::Unit::Type unitType, bool useDefault)
{
    if ( unitType < Sc::Unit::TotalTypes )
        data->useDefault[unitType] = useDefault ? Chk::UseDefault::Yes : Chk::UseDefault::No;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS section!");
}

void UnisSection::setUnitHitpoints(Sc::Unit::Type unitType, u32 hitpoints)
{
    if ( unitType < Sc::Unit::TotalTypes )
        data->hitpoints[unitType] = hitpoints;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS section!");
}

void UnisSection::setUnitShieldPoints(Sc::Unit::Type unitType, u16 shieldPoints)
{
    if ( unitType < Sc::Unit::TotalTypes )
        data->shieldPoints[unitType] = shieldPoints;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS section!");
}

void UnisSection::setUnitArmorLevel(Sc::Unit::Type unitType, u8 armorLevel)
{
    if ( unitType < Sc::Unit::TotalTypes )
        data->armorLevel[unitType] = armorLevel;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS section!");
}

void UnisSection::setUnitBuildTime(Sc::Unit::Type unitType, u16 buildTime)
{
    if ( unitType < Sc::Unit::TotalTypes )
        data->buildTime[unitType] = buildTime;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS section!");
}

void UnisSection::setUnitMineralCost(Sc::Unit::Type unitType, u16 mineralCost)
{
    if ( unitType < Sc::Unit::TotalTypes )
        data->mineralCost[unitType] = mineralCost;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS section!");
}

void UnisSection::setUnitGasCost(Sc::Unit::Type unitType, u16 gasCost)
{
    if ( unitType < Sc::Unit::TotalTypes )
        data->gasCost[unitType] = gasCost;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS section!");
}

void UnisSection::setUnitNameStringId(Sc::Unit::Type unitType, u16 nameStringId)
{
    if ( unitType < Sc::Unit::TotalTypes )
        data->nameStringId[unitType] = nameStringId;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS section!");
}

void UnisSection::setWeaponBaseDamage(Sc::Weapon::Type weaponType, u16 baseDamage)
{
    if ( (size_t)weaponType < Sc::Weapon::TotalOriginal )
        data->baseDamage[weaponType] = baseDamage;
    else
        throw std::out_of_range(std::string("WeaponType: ") + std::to_string((size_t)weaponType) + " is out of range for the UNIS section!");
}

void UnisSection::setWeaponUpgradeDamage(Sc::Weapon::Type weaponType, u16 upgradeDamage)
{
    if ( (size_t)weaponType < Sc::Weapon::TotalOriginal )
        data->upgradeDamage[weaponType] = upgradeDamage;
    else
        throw std::out_of_range(std::string("WeaponType: ") + std::to_string((size_t)weaponType) + " is out of range for the UNIS section!");
}

bool UnisSection::stringUsed(size_t stringId)
{
    u16 u16StringId = (u16)stringId;
    for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
    {
        if ( data->nameStringId[i] == u16StringId )
            return true;
    }
    return false;
}

void UnisSection::markUsedStrings(std::bitset<Chk::MaxStrings> & stringIdUsed)
{
    for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
    {
        if ( data->nameStringId[i] > 0 )
            stringIdUsed[data->nameStringId[i]] = true;
    }
}

void UnisSection::remapStringIds(const std::map<u32, u32> & stringIdRemappings)
{
    for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
    {
        auto found = stringIdRemappings.find(data->nameStringId[i]);
        if ( found != stringIdRemappings.end() )
            data->nameStringId[i] = found->second;
    }
}

void UnisSection::deleteString(size_t stringId)
{
    for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
    {
        if ( data->nameStringId[i] == stringId )
            data->nameStringId[i] = 0;
    }
}


UpgsSectionPtr UpgsSection::GetDefault()
{
    Chk::UPGS data = {
        {}, // useDefault (all Yes/1, set below)
        {}, // baseMineralCost (all 0)
        {}, // mineralCostFactor (all 0)
        {}, // baseGasCost (all 0)
        {}, // gasCostFactor (all 0)
        {}, // baseResearchTime (all 0)
        {} // researchTimeFactor (all 0)
    };
    memset(&data.useDefault, (int)Chk::UseDefault::Yes, Sc::Upgrade::TotalOriginalTypes);
    return UpgsSectionPtr(new (std::nothrow) UpgsSection(data));
}

UpgsSection::UpgsSection(const Chk::UPGS & data) : StructSection<Chk::UPGS, false>(SectionName::UPGS, data)
{

}

UpgsSection::UpgsSection() : StructSection<Chk::UPGS, false>(SectionName::UPGS)
{

}

UpgsSection::~UpgsSection()
{

}

bool UpgsSection::upgradeUsesDefaultCosts(Sc::Upgrade::Type upgradeType)
{
    if ( upgradeType < Sc::Upgrade::TotalOriginalTypes )
        return data->useDefault[upgradeType] != Chk::UseDefault::No;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the UPGS section!");
}

u16 UpgsSection::getBaseMineralCost(Sc::Upgrade::Type upgradeType)
{
    if ( upgradeType < Sc::Upgrade::TotalOriginalTypes )
        return data->baseMineralCost[upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the UPGS section!");
}

u16 UpgsSection::getMineralCostFactor(Sc::Upgrade::Type upgradeType)
{
    if ( upgradeType < Sc::Upgrade::TotalOriginalTypes )
        return data->mineralCostFactor[upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the UPGS section!");
}

u16 UpgsSection::getBaseGasCost(Sc::Upgrade::Type upgradeType)
{
    if ( upgradeType < Sc::Upgrade::TotalOriginalTypes )
        return data->baseGasCost[upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the UPGS section!");
}

u16 UpgsSection::getGasCostFactor(Sc::Upgrade::Type upgradeType)
{
    if ( upgradeType < Sc::Upgrade::TotalOriginalTypes )
        return data->gasCostFactor[upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the UPGS section!");
}

u16 UpgsSection::getBaseResearchTime(Sc::Upgrade::Type upgradeType)
{
    if ( upgradeType < Sc::Upgrade::TotalOriginalTypes )
        return data->baseResearchTime[upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the UPGS section!");
}

u16 UpgsSection::getResearchTimeFactor(Sc::Upgrade::Type upgradeType)
{
    if ( upgradeType < Sc::Upgrade::TotalOriginalTypes )
        return data->researchTimeFactor[upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the UPGS section!");
}


void UpgsSection::setUpgradeUsesDefaultCosts(Sc::Upgrade::Type upgradeType, bool useDefault)
{
    if ( upgradeType < Sc::Upgrade::TotalOriginalTypes )
        data->useDefault[upgradeType] = useDefault ? Chk::UseDefault::Yes : Chk::UseDefault::No;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the UPGS section!");
}

void UpgsSection::setBaseMineralCost(Sc::Upgrade::Type upgradeType, u16 baseMineralCost)
{
    if ( upgradeType < Sc::Upgrade::TotalOriginalTypes )
        data->baseMineralCost[upgradeType] = baseMineralCost;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the UPGS section!");
}

void UpgsSection::setMineralCostFactor(Sc::Upgrade::Type upgradeType, u16 mineralCostFactor)
{
    if ( upgradeType < Sc::Upgrade::TotalOriginalTypes )
        data->mineralCostFactor[upgradeType] = mineralCostFactor;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the UPGS section!");
}

void UpgsSection::setBaseGasCost(Sc::Upgrade::Type upgradeType, u16 baseGasCost)
{
    if ( upgradeType < Sc::Upgrade::TotalOriginalTypes )
        data->baseGasCost[upgradeType] = baseGasCost;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the UPGS section!");
}

void UpgsSection::setGasCostFactor(Sc::Upgrade::Type upgradeType, u16 gasCostFactor)
{
    if ( upgradeType < Sc::Upgrade::TotalOriginalTypes )
        data->gasCostFactor[upgradeType] = gasCostFactor;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the UPGS section!");
}

void UpgsSection::setBaseResearchTime(Sc::Upgrade::Type upgradeType, u16 baseResearchTime)
{
    if ( upgradeType < Sc::Upgrade::TotalOriginalTypes )
        data->baseResearchTime[upgradeType] = baseResearchTime;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the UPGS section!");
}

void UpgsSection::setResearchTimeFactor(Sc::Upgrade::Type upgradeType, u16 researchTimeFactor)
{
    if ( upgradeType < Sc::Upgrade::TotalOriginalTypes )
        data->researchTimeFactor[upgradeType] = researchTimeFactor;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the UPGS section!");
}


TecsSectionPtr TecsSection::GetDefault()
{
    Chk::TECS data = {
        {}, // useDefault (all Yes/1, set below)
        {}, // mineralCost (all 0)
        {}, // gasCost (all 0)
        {}, // researchTime (all 0)
        {} // energyCost (all 0)
    };
    memset(&data.useDefault, (int)Chk::UseDefault::Yes, Sc::Tech::TotalOriginalTypes);
    return TecsSectionPtr(new (std::nothrow) TecsSection(data));
}

TecsSection::TecsSection(const Chk::TECS & data) : StructSection<Chk::TECS, false>(SectionName::TECS, data)
{

}

TecsSection::TecsSection() : StructSection<Chk::TECS, false>(SectionName::TECS)
{

}

TecsSection::~TecsSection()
{

}

bool TecsSection::techUsesDefaultSettings(Sc::Tech::Type techType)
{
    if ( (size_t)techType < Sc::Tech::TotalOriginalTypes )
        return data->useDefault[(size_t)techType] != Chk::UseDefault::No;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) + " is out of range for the TECS section!");
}

u16 TecsSection::getTechMineralCost(Sc::Tech::Type techType)
{
    if ( (size_t)techType < Sc::Tech::TotalOriginalTypes )
        return data->mineralCost[(size_t)techType];
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) + " is out of range for the TECS section!");
}

u16 TecsSection::getTechGasCost(Sc::Tech::Type techType)
{
    if ( (size_t)techType < Sc::Tech::TotalOriginalTypes )
        return data->gasCost[(size_t)techType];
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) + " is out of range for the TECS section!");
}

u16 TecsSection::getTechResearchTime(Sc::Tech::Type techType)
{
    if ( (size_t)techType < Sc::Tech::TotalOriginalTypes )
        return data->researchTime[(size_t)techType];
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) + " is out of range for the TECS section!");
}

u16 TecsSection::getTechEnergyCost(Sc::Tech::Type techType)
{
    if ( (size_t)techType < Sc::Tech::TotalOriginalTypes )
        return data->energyCost[(size_t)techType];
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) + " is out of range for the TECS section!");
}

void TecsSection::setTechUsesDefaultSettings(Sc::Tech::Type techType, bool useDefault)
{
    if ( (size_t)techType < Sc::Tech::TotalOriginalTypes )
        data->useDefault[(size_t)techType] = useDefault ? Chk::UseDefault::Yes : Chk::UseDefault::No;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) + " is out of range for the TECS section!");
}

void TecsSection::setTechMineralCost(Sc::Tech::Type techType, u16 mineralCost)
{
    if ( (size_t)techType < Sc::Tech::TotalOriginalTypes )
        data->mineralCost[(size_t)techType] = mineralCost;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) + " is out of range for the TECS section!");
}

void TecsSection::setTechGasCost(Sc::Tech::Type techType, u16 gasCost)
{
    if ( (size_t)techType < Sc::Tech::TotalOriginalTypes )
        data->gasCost[(size_t)techType] = gasCost;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) + " is out of range for the TECS section!");
}

void TecsSection::setTechResearchTime(Sc::Tech::Type techType, u16 researchTime)
{
    if ( (size_t)techType < Sc::Tech::TotalOriginalTypes )
        data->researchTime[(size_t)techType] = researchTime;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) + " is out of range for the TECS section!");
}

void TecsSection::setTechEnergyCost(Sc::Tech::Type techType, u16 energyCost)
{
    if ( (size_t)techType < Sc::Tech::TotalOriginalTypes )
        data->energyCost[(size_t)techType] = energyCost;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) + " is out of range for the TECS section!");
}


SwnmSectionPtr SwnmSection::GetDefault()
{
    return SwnmSectionPtr(new (std::nothrow) SwnmSection(Chk::SWNM {}));
}

SwnmSection::SwnmSection(const Chk::SWNM & data) : StructSection<Chk::SWNM, true>(SectionName::SWNM, data)
{

}

SwnmSection::SwnmSection() : StructSection<Chk::SWNM, true>(SectionName::SWNM)
{

}

SwnmSection::~SwnmSection()
{

}

size_t SwnmSection::getSwitchNameStringId(size_t switchIndex)
{
    if ( switchIndex < Chk::TotalSwitches )
        return data->switchName[switchIndex];
    else
        throw std::out_of_range(std::string("switchIndex: ") + std::to_string((u32)switchIndex) + " is out of range for the SWNM section!");
}

void SwnmSection::setSwitchNameStringId(size_t switchIndex, size_t stringId)
{
    if ( switchIndex < Chk::TotalSwitches )
        data->switchName[switchIndex] = (u32)stringId;
}

bool SwnmSection::stringUsed(size_t stringId)
{
    u32 u32StringId = (u32)stringId;
    for ( size_t i=0; i<Chk::TotalSwitches; i++ )
    {
        if ( data->switchName[i] == u32StringId )
            return true;
    }
    return false;
}

void SwnmSection::markUsedStrings(std::bitset<Chk::MaxStrings> & stringIdUsed)
{
    for ( size_t i=0; i<Chk::TotalSwitches; i++ )
    {
        if ( data->switchName[i] > 0 && data->switchName[i] < Chk::MaxStrings )
            stringIdUsed[data->switchName[i]] = true;
    }
}

void SwnmSection::remapStringIds(const std::map<u32, u32> & stringIdRemappings)
{
    for ( size_t i=0; i<Chk::TotalSwitches; i++ )
    {
        auto found = stringIdRemappings.find(data->switchName[i]);
        if ( found != stringIdRemappings.end() )
            data->switchName[i] = found->second;
    }
}

void SwnmSection::deleteString(size_t stringId)
{
    for ( size_t i=0; i<Chk::TotalSwitches; i++ )
    {
        if ( data->switchName[i] == stringId )
            data->switchName[i] = 0;
    }
}


ColrSectionPtr ColrSection::GetDefault()
{
    return ColrSectionPtr(new (std::nothrow) ColrSection(Chk::COLR {
        Chk::PlayerColor::Red   , Chk::PlayerColor::Blue , Chk::PlayerColor::Teal , Chk::PlayerColor::Purple,
        Chk::PlayerColor::Orange, Chk::PlayerColor::Brown, Chk::PlayerColor::White, Chk::PlayerColor::Yellow
    }));
}

ColrSection::ColrSection(const Chk::COLR & data) : StructSection<Chk::COLR, false>(SectionName::COLR, data)
{

}

ColrSection::ColrSection() : StructSection<Chk::COLR, false>(SectionName::COLR)
{

}

ColrSection::~ColrSection()
{

}

Chk::PlayerColor ColrSection::getPlayerColor(size_t slotIndex)
{
    if ( slotIndex < Sc::Player::TotalSlots )
        return data->playerColor[slotIndex];
    else
        throw std::out_of_range(std::string("SlotIndex: ") + std::to_string((u32)slotIndex) + " is out of range for the COLR section!");
}

void ColrSection::setPlayerColor(size_t slotIndex, Chk::PlayerColor color)
{
    if ( slotIndex < Sc::Player::TotalSlots )
        data->playerColor[slotIndex] = color;
    else
        throw std::out_of_range(std::string("SlotIndex: ") + std::to_string((u32)slotIndex) + " is out of range for the COLR section!");
}

PupxSectionPtr PupxSection::GetDefault()
{
    Chk::PUPx data = {
        {}, // playerMaxUpgradeLevel (all 0)
        {}, // playerStartUpgradeLevel (all 0)
        {
            3, 3, 3, 3,    3, 3, 3, 3,    3, 3, 3, 3,    3, 3, 3, 3,
            1, 1, 0, 1,    1, 1, 1, 1,    1, 1, 1, 1,    1, 1, 1, 1,
            1, 1, 1, 1,    1, 1, 1, 1,    1, 1, 1, 1,    1, 0, 0, 1,
            0, 1, 0, 1,    1, 1, 1, 0,    0, 0, 0, 0,    0
        }, // defaultMaxLevel
        {}, // defaultStartLevel (all 0)
        {} // playerUpgradeUsesDefault (all Yes/1, set below)
    };
    memset(&data.playerUpgradeUsesDefault, (int)Chk::UseDefault::Yes, Sc::Upgrade::TotalTypes*Sc::Player::Total);
    return PupxSectionPtr(new (std::nothrow) PupxSection(data));
}

PupxSection::PupxSection(const Chk::PUPx & data) : StructSection<Chk::PUPx, false>(SectionName::PUPx, data)
{

}

PupxSection::PupxSection() : StructSection<Chk::PUPx, false>(SectionName::PUPx)
{

}

PupxSection::~PupxSection()
{

}

size_t PupxSection::getMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex)
{
    if ( upgradeType < Sc::Upgrade::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            return data->playerMaxUpgradeLevel[upgradeType][playerIndex];
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PUPx section!");
    }
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the PUPx section!");
}

size_t PupxSection::getStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex)
{
    if ( upgradeType < Sc::Upgrade::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            return data->playerStartUpgradeLevel[upgradeType][playerIndex];
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PUPx section!");
    }
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the PUPx section!");
}

size_t PupxSection::getDefaultMaxUpgradeLevel(Sc::Upgrade::Type upgradeType)
{
    if ( upgradeType < Sc::Upgrade::TotalTypes )
        return data->defaultMaxLevel[upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the PUPx section!");
}

size_t PupxSection::getDefaultStartUpgradeLevel(Sc::Upgrade::Type upgradeType)
{
    if ( upgradeType < Sc::Upgrade::TotalTypes )
        return data->defaultStartLevel[upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the PUPx section!");
}

bool PupxSection::playerUsesDefault(Sc::Upgrade::Type upgradeType, size_t playerIndex)
{
    if ( upgradeType < Sc::Upgrade::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            return data->playerUpgradeUsesDefault[upgradeType][playerIndex] != Chk::UseDefault::No;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PUPx section!");
    }
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the PUPx section!");
}

void PupxSection::setMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, size_t maxUpgradeLevel)
{
    if ( upgradeType < Sc::Upgrade::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            data->playerMaxUpgradeLevel[upgradeType][playerIndex] = (u8)maxUpgradeLevel;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PUPx section!");
    }
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the PUPx section!");
}

void PupxSection::setStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, size_t startUpgradeLevel)
{
    if ( upgradeType < Sc::Upgrade::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            data->playerStartUpgradeLevel[upgradeType][playerIndex] = (u8)startUpgradeLevel;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PUPx section!");
    }
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the PUPx section!");
}

void PupxSection::setDefaultMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t maxUpgradeLevel)
{
    if ( upgradeType < Sc::Upgrade::TotalTypes )
        data->defaultMaxLevel[upgradeType] = (u8)maxUpgradeLevel;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the PUPx section!");
}

void PupxSection::setDefaultStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t startUpgradeLevel)
{
    if ( upgradeType < Sc::Upgrade::TotalTypes )
        data->defaultStartLevel[upgradeType] = (u8)startUpgradeLevel;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the PUPx section!");
}

void PupxSection::setPlayerUsesDefault(Sc::Upgrade::Type upgradeType, size_t playerIndex, bool useDefault)
{
    if ( upgradeType < Sc::Upgrade::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            data->playerUpgradeUsesDefault[upgradeType][playerIndex] = useDefault ? Chk::UseDefault::Yes : Chk::UseDefault::No;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PUPx section!");
    }
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the PUPx section!");
}


PtexSectionPtr PtexSection::GetDefault()
{
    Chk::PTEx data = {
        {}, // techAvailableForPlayer (all No/0)
        {}, // techResearchedForPlayer (all No/0)
        {}, // techAvailableByDefault (all Yes/1, set below)
        { // techResearchedByDefault
            Chk::Researched::No , Chk::Researched::No , Chk::Researched::No , Chk::Researched::No ,
            Chk::Researched::Yes, Chk::Researched::No , Chk::Researched::Yes, Chk::Researched::No ,
            Chk::Researched::No , Chk::Researched::No , Chk::Researched::No , Chk::Researched::No,
            Chk::Researched::Yes, Chk::Researched::No , Chk::Researched::Yes, Chk::Researched::No,
            Chk::Researched::No , Chk::Researched::No , Chk::Researched::Yes, Chk::Researched::No,
            Chk::Researched::No , Chk::Researched::No , Chk::Researched::No , Chk::Researched::Yes,
            Chk::Researched::No , Chk::Researched::No , Chk::Researched::No , Chk::Researched::No,
            Chk::Researched::Yes, Chk::Researched::Yes, Chk::Researched::No , Chk::Researched::No,
            Chk::Researched::No , Chk::Researched::Yes, Chk::Researched::Yes, Chk::Researched::No,
            Chk::Researched::No , Chk::Researched::No , Chk::Researched::No , Chk::Researched::No,
            Chk::Researched::No , Chk::Researched::No , Chk::Researched::No , Chk::Researched::No
        },
        {} // playerUsesDefaultsForTech (all Yes/1, set below)
    };
    memset(&data.techAvailableByDefault, (int)Chk::Available::Yes, Sc::Tech::TotalTypes);
    memset(&data.playerUsesDefaultsForTech, (int)Chk::UseDefault::Yes, Sc::Tech::TotalTypes*Sc::Player::Total);
    return PtexSectionPtr(new (std::nothrow) PtexSection(data));
}

PtexSection::PtexSection(const Chk::PTEx & data) : StructSection<Chk::PTEx, false>(SectionName::PTEx, data)
{

}

PtexSection::PtexSection() : StructSection<Chk::PTEx, false>(SectionName::PTEx)
{

}

PtexSection::~PtexSection()
{

}

bool PtexSection::techAvailable(Sc::Tech::Type techType, size_t playerIndex)
{
    if ( (size_t)techType < Sc::Tech::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            return data->techAvailableForPlayer[(size_t)techType][playerIndex] != Chk::Available::No;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PTEx section!");
    }
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) + " is out of range for the PTEx section!");
}

bool PtexSection::techResearched(Sc::Tech::Type techType, size_t playerIndex)
{
    if ( (size_t)techType < Sc::Tech::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            return data->techResearchedForPlayer[(size_t)techType][playerIndex] != Chk::Researched::No;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PTEx section!");
    }
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) + " is out of range for the PTEx section!");
}

bool PtexSection::techDefaultAvailable(Sc::Tech::Type techType)
{
    if ( (size_t)techType < Sc::Tech::TotalTypes )
        return data->techAvailableByDefault[(size_t)techType] != Chk::Available::No;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) + " is out of range for the PTEx section!");
}

bool PtexSection::techDefaultResearched(Sc::Tech::Type techType)
{
    if ( (size_t)techType < Sc::Tech::TotalTypes )
        return data->techResearchedByDefault[(size_t)techType] != Chk::Researched::No;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) + " is out of range for the PTEx section!");
}

bool PtexSection::playerUsesDefault(Sc::Tech::Type techType, size_t playerIndex)
{
    if ( (size_t)techType < Sc::Tech::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            return data->playerUsesDefaultsForTech[(size_t)techType][playerIndex] != Chk::UseDefault::No;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PTEx section!");
    }
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) + " is out of range for the PTEx section!");
}

void PtexSection::setTechAvailable(Sc::Tech::Type techType, size_t playerIndex, bool available)
{
    Chk::Available techAvailable = available ? Chk::Available::Yes : Chk::Available::No;
    if ( (size_t)techType < Sc::Tech::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            data->techAvailableForPlayer[(size_t)techType][playerIndex] = techAvailable;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PTEx section!");
    }
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) + " is out of range for the PTEx section!");
}

void PtexSection::setTechResearched(Sc::Tech::Type techType, size_t playerIndex, bool researched)
{
    Chk::Researched techResearched = researched ? Chk::Researched::Yes : Chk::Researched::No;
    if ( (size_t)techType < Sc::Tech::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            data->techResearchedForPlayer[(size_t)techType][playerIndex] = techResearched;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PTEx section!");
    }
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) + " is out of range for the PTEx section!");
}

void PtexSection::setDefaultTechAvailable(Sc::Tech::Type techType, bool available)
{
    Chk::Available techAvailable = available ? Chk::Available::Yes : Chk::Available::No;
    if ( (size_t)techType < Sc::Tech::TotalTypes )
        data->techAvailableByDefault[(size_t)techType] = techAvailable;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) + " is out of range for the PTEx section!");
}

void PtexSection::setDefaultTechResearched(Sc::Tech::Type techType, bool researched)
{
    Chk::Researched techResearched = researched ? Chk::Researched::Yes : Chk::Researched::No;
    if ( (size_t)techType < Sc::Tech::TotalTypes )
        data->techResearchedByDefault[(size_t)techType] = techResearched;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) + " is out of range for the PTEx section!");
}

void PtexSection::setPlayerUsesDefault(Sc::Tech::Type techType, size_t playerIndex, bool useDefault)
{
    Chk::UseDefault playerUsesDefault = useDefault ? Chk::UseDefault::Yes : Chk::UseDefault::No;
    if ( (size_t)techType < Sc::Tech::TotalTypes )
        data->playerUsesDefaultsForTech[(size_t)techType][playerIndex] = playerUsesDefault;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) + " is out of range for the PTEx section!");
}

UnixSectionPtr UnixSection::GetDefault()
{
    Chk::UNIx data = {
        {}, // useDefault (all Yes/1, set below)
        {}, // hitpoints (all 0)
        {}, // shieldPoints (all 0)
        {}, // armorPoints (all 0)
        {}, // buildTime (all 0)
        {}, // mineralCost (all 0)
        {}, // gasCost (all 0)
        {}, // nameString (all 0)
        {}, // baseDamage (all 0)
        {}, // upgradeDamage (all 0)
    };
    memset(&data.useDefault, (int)Chk::UseDefault::Yes, Sc::Unit::TotalTypes);
    return UnixSectionPtr(new (std::nothrow) UnixSection(data));
}

UnixSection::UnixSection(const Chk::UNIx & data) : StructSection<Chk::UNIx, false>(SectionName::UNIx, data)
{

}

UnixSection::UnixSection() : StructSection<Chk::UNIx, false>(SectionName::UNIx)
{

}

UnixSection::~UnixSection()
{

}

bool UnixSection::unitUsesDefaultSettings(Sc::Unit::Type unitType)
{
    if ( unitType < Sc::Unit::TotalTypes )
        return data->useDefault[unitType] != Chk::UseDefault::No;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS section!");
}

u32 UnixSection::getUnitHitpoints(Sc::Unit::Type unitType)
{
    if ( unitType < Sc::Unit::TotalTypes )
        return data->hitpoints[unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIx section!");
}

u16 UnixSection::getUnitShieldPoints(Sc::Unit::Type unitType)
{
    if ( unitType < Sc::Unit::TotalTypes )
        return data->shieldPoints[unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIx section!");
}

u8 UnixSection::getUnitArmorLevel(Sc::Unit::Type unitType)
{
    if ( unitType < Sc::Unit::TotalTypes )
        return data->armorLevel[unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIx section!");
}

u16 UnixSection::getUnitBuildTime(Sc::Unit::Type unitType)
{
    if ( unitType < Sc::Unit::TotalTypes )
        return data->buildTime[unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIx section!");
}

u16 UnixSection::getUnitMineralCost(Sc::Unit::Type unitType)
{
    if ( unitType < Sc::Unit::TotalTypes )
        return data->mineralCost[unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIx section!");
}

u16 UnixSection::getUnitGasCost(Sc::Unit::Type unitType)
{
    if ( unitType < Sc::Unit::TotalTypes )
        return data->gasCost[unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIx section!");
}

size_t UnixSection::getUnitNameStringId(Sc::Unit::Type unitType)
{
    if ( unitType < Sc::Unit::TotalTypes )
        return data->nameStringId[unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIx section!");
}

u16 UnixSection::getWeaponBaseDamage(Sc::Weapon::Type weaponType)
{
    if ( (size_t)weaponType < Sc::Weapon::Total )
        return data->baseDamage[weaponType];
    else
        throw std::out_of_range(std::string("WeaponType: ") + std::to_string((size_t)weaponType) + " is out of range for the UNIx section!");
}

u16 UnixSection::getWeaponUpgradeDamage(Sc::Weapon::Type weaponType)
{
    if ( (size_t)weaponType < Sc::Weapon::Total )
        return data->upgradeDamage[weaponType];
    else
        throw std::out_of_range(std::string("WeaponType: ") + std::to_string((size_t)weaponType) + " is out of range for the UNIx section!");
}

void UnixSection::setUnitUsesDefaultSettings(Sc::Unit::Type unitType, bool useDefault)
{
    if ( unitType < Sc::Unit::TotalTypes )
        data->useDefault[unitType] = useDefault ? Chk::UseDefault::Yes : Chk::UseDefault::No;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIx section!");
}

void UnixSection::setUnitHitpoints(Sc::Unit::Type unitType, u32 hitpoints)
{
    if ( unitType < Sc::Unit::TotalTypes )
        data->hitpoints[unitType] = hitpoints;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIx section!");
}

void UnixSection::setUnitShieldPoints(Sc::Unit::Type unitType, u16 shieldPoints)
{
    if ( unitType < Sc::Unit::TotalTypes )
        data->shieldPoints[unitType] = shieldPoints;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIx section!");
}

void UnixSection::setUnitArmorLevel(Sc::Unit::Type unitType, u8 armorLevel)
{
    if ( unitType < Sc::Unit::TotalTypes )
        data->armorLevel[unitType] = armorLevel;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIx section!");
}

void UnixSection::setUnitBuildTime(Sc::Unit::Type unitType, u16 buildTime)
{
    if ( unitType < Sc::Unit::TotalTypes )
        data->buildTime[unitType] = buildTime;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIx section!");
}

void UnixSection::setUnitMineralCost(Sc::Unit::Type unitType, u16 mineralCost)
{
    if ( unitType < Sc::Unit::TotalTypes )
        data->mineralCost[unitType] = mineralCost;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIx section!");
}

void UnixSection::setUnitGasCost(Sc::Unit::Type unitType, u16 gasCost)
{
    if ( unitType < Sc::Unit::TotalTypes )
        data->gasCost[unitType] = gasCost;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIx section!");
}

void UnixSection::setUnitNameStringId(Sc::Unit::Type unitType, u16 nameStringId)
{
    if ( unitType < Sc::Unit::TotalTypes )
        data->nameStringId[unitType] = nameStringId;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIx section!");
}

void UnixSection::setWeaponBaseDamage(Sc::Weapon::Type weaponType, u16 baseDamage)
{
    if ( (size_t)weaponType < Sc::Weapon::Total )
        data->baseDamage[weaponType] = baseDamage;
    else
        throw std::out_of_range(std::string("WeaponType: ") + std::to_string((size_t)weaponType) + " is out of range for the UNIx section!");
}

void UnixSection::setWeaponUpgradeDamage(Sc::Weapon::Type weaponType, u16 upgradeDamage)
{
    if ( (size_t)weaponType < Sc::Weapon::Total )
        data->upgradeDamage[weaponType] = upgradeDamage;
    else
        throw std::out_of_range(std::string("WeaponType: ") + std::to_string((size_t)weaponType) + " is out of range for the UNIx section!");
}

bool UnixSection::stringUsed(size_t stringId)
{
    u16 u16StringId = (u16)stringId;
    for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
    {
        if ( data->nameStringId[i] == u16StringId )
            return true;
    }
    return false;
}

void UnixSection::markUsedStrings(std::bitset<Chk::MaxStrings> & stringIdUsed)
{
    for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
    {
        if ( data->nameStringId[i] > 0 )
            stringIdUsed[data->nameStringId[i]] = true;
    }
}

void UnixSection::remapStringIds(const std::map<u32, u32> & stringIdRemappings)
{
    for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
    {
        auto found = stringIdRemappings.find(data->nameStringId[i]);
        if ( found != stringIdRemappings.end() )
            data->nameStringId[i] = found->second;
    }
}

void UnixSection::deleteString(size_t stringId)
{
    for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
    {
        if ( data->nameStringId[i] == stringId )
            data->nameStringId[i] = 0;
    }
}


UpgxSectionPtr UpgxSection::GetDefault() // 794
{
    Chk::UPGx data = {
        {}, // useDefault (all Yes/1, set below)
        {}, // baseMineralCost (all 0)
        {}, // mineralCostFactor (all 0)
        {}, // baseGasCost (all 0)
        {}, // gasCostFactor (all 0)
        {}, // baseResearchTime (all 0)
        {} // researchTimeFactor (all 0)
    };
    memset(&data.useDefault, (int)Chk::UseDefault::Yes, Sc::Upgrade::TotalTypes);
    return UpgxSectionPtr(new (std::nothrow) UpgxSection(data));
}

UpgxSection::UpgxSection(const Chk::UPGx & data) : StructSection<Chk::UPGx, false>(SectionName::UPGx, data)
{

}

UpgxSection::UpgxSection() : StructSection<Chk::UPGx, false>(SectionName::UPGx)
{

}

UpgxSection::~UpgxSection()
{

}

bool UpgxSection::upgradeUsesDefaultCosts(Sc::Upgrade::Type upgradeType)
{
    if ( upgradeType < Sc::Upgrade::TotalTypes )
        return data->useDefault[upgradeType] != Chk::UseDefault::No;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the UPGx section!");
}

u16 UpgxSection::getBaseMineralCost(Sc::Upgrade::Type upgradeType)
{
    if ( upgradeType < Sc::Upgrade::TotalTypes )
        return data->baseMineralCost[upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the UPGx section!");
}

u16 UpgxSection::getMineralCostFactor(Sc::Upgrade::Type upgradeType)
{
    if ( upgradeType < Sc::Upgrade::TotalTypes )
        return data->mineralCostFactor[upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the UPGx section!");
}

u16 UpgxSection::getBaseGasCost(Sc::Upgrade::Type upgradeType)
{
    if ( upgradeType < Sc::Upgrade::TotalTypes )
        return data->baseGasCost[upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the UPGx section!");
}

u16 UpgxSection::getGasCostFactor(Sc::Upgrade::Type upgradeType)
{
    if ( upgradeType < Sc::Upgrade::TotalTypes )
        return data->gasCostFactor[upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the UPGx section!");
}

u16 UpgxSection::getBaseResearchTime(Sc::Upgrade::Type upgradeType)
{
    if ( upgradeType < Sc::Upgrade::TotalTypes )
        return data->baseResearchTime[upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the UPGx section!");
}

u16 UpgxSection::getResearchTimeFactor(Sc::Upgrade::Type upgradeType)
{
    if ( upgradeType < Sc::Upgrade::TotalTypes )
        return data->researchTimeFactor[upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the UPGx section!");
}


void UpgxSection::setUpgradeUsesDefaultCosts(Sc::Upgrade::Type upgradeType, bool useDefault)
{
    if ( upgradeType < Sc::Upgrade::TotalTypes )
        data->useDefault[upgradeType] = useDefault ? Chk::UseDefault::Yes : Chk::UseDefault::No;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the UPGx section!");
}

void UpgxSection::setBaseMineralCost(Sc::Upgrade::Type upgradeType, u16 baseMineralCost)
{
    if ( upgradeType < Sc::Upgrade::TotalTypes )
        data->baseMineralCost[upgradeType] = baseMineralCost;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the UPGx section!");
}

void UpgxSection::setMineralCostFactor(Sc::Upgrade::Type upgradeType, u16 mineralCostFactor)
{
    if ( upgradeType < Sc::Upgrade::TotalTypes )
        data->mineralCostFactor[upgradeType] = mineralCostFactor;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the UPGx section!");
}

void UpgxSection::setBaseGasCost(Sc::Upgrade::Type upgradeType, u16 baseGasCost)
{
    if ( upgradeType < Sc::Upgrade::TotalTypes )
        data->baseGasCost[upgradeType] = baseGasCost;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the UPGx section!");
}

void UpgxSection::setGasCostFactor(Sc::Upgrade::Type upgradeType, u16 gasCostFactor)
{
    if ( upgradeType < Sc::Upgrade::TotalTypes )
        data->gasCostFactor[upgradeType] = gasCostFactor;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the UPGx section!");
}

void UpgxSection::setBaseResearchTime(Sc::Upgrade::Type upgradeType, u16 baseResearchTime)
{
    if ( upgradeType < Sc::Upgrade::TotalTypes )
        data->baseResearchTime[upgradeType] = baseResearchTime;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the UPGx section!");
}

void UpgxSection::setResearchTimeFactor(Sc::Upgrade::Type upgradeType, u16 researchTimeFactor)
{
    if ( upgradeType < Sc::Upgrade::TotalTypes )
        data->researchTimeFactor[upgradeType] = researchTimeFactor;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the UPGx section!");
}

TecxSectionPtr TecxSection::GetDefault()
{
    Chk::TECx data = {
        {}, // useDefault (all Yes/1, set below)
        {}, // mineralCost
        {}, // gasCost
        {}, // researchTime
        {} // energyCost
    };
    memset(&data.useDefault, (int)Chk::UseDefault::Yes, Sc::Tech::TotalTypes);
    return TecxSectionPtr(new (std::nothrow) TecxSection(data));
}

TecxSection::TecxSection(const Chk::TECx & data) : StructSection<Chk::TECx, false>(SectionName::TECx, data)
{

}

TecxSection::TecxSection() : StructSection<Chk::TECx, false>(SectionName::TECx)
{

}

TecxSection::~TecxSection()
{

}

bool TecxSection::techUsesDefaultSettings(Sc::Tech::Type techType)
{
    if ( (size_t)techType < Sc::Tech::TotalTypes )
        return data->useDefault[(size_t)techType] != Chk::UseDefault::No;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) + " is out of range for the TECx section!");
}

u16 TecxSection::getTechMineralCost(Sc::Tech::Type techType)
{
    if ( (size_t)techType < Sc::Tech::TotalTypes )
        return data->mineralCost[(size_t)techType];
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) + " is out of range for the TECx section!");
}

u16 TecxSection::getTechGasCost(Sc::Tech::Type techType)
{
    if ( (size_t)techType < Sc::Tech::TotalTypes )
        return data->gasCost[(size_t)techType];
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) + " is out of range for the TECx section!");
}

u16 TecxSection::getTechResearchTime(Sc::Tech::Type techType)
{
    if ( (size_t)techType < Sc::Tech::TotalTypes )
        return data->researchTime[(size_t)techType];
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) + " is out of range for the TECx section!");
}

u16 TecxSection::getTechEnergyCost(Sc::Tech::Type techType)
{
    if ( (size_t)techType < Sc::Tech::TotalTypes )
        return data->energyCost[(size_t)techType];
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) + " is out of range for the TECx section!");
}


void TecxSection::setTechUsesDefaultSettings(Sc::Tech::Type techType, bool useDefault)
{
    if ( (size_t)techType < Sc::Tech::TotalTypes )
        data->useDefault[(size_t)techType] = useDefault ? Chk::UseDefault::Yes : Chk::UseDefault::No;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) + " is out of range for the TECx section!");
}

void TecxSection::setTechMineralCost(Sc::Tech::Type techType, u16 mineralCost)
{
    if ( (size_t)techType < Sc::Tech::TotalTypes )
        data->mineralCost[(size_t)techType] = mineralCost;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) + " is out of range for the TECx section!");
}

void TecxSection::setTechGasCost(Sc::Tech::Type techType, u16 gasCost)
{
    if ( (size_t)techType < Sc::Tech::TotalTypes )
        data->gasCost[(size_t)techType] = gasCost;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) + " is out of range for the TECx section!");
}

void TecxSection::setTechResearchTime(Sc::Tech::Type techType, u16 researchTime)
{
    if ( techType < Sc::Tech::TotalTypes )
        data->researchTime[techType] = researchTime;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) + " is out of range for the TECx section!");
}

void TecxSection::setTechEnergyCost(Sc::Tech::Type techType, u16 energyCost)
{
    if ( (size_t)techType < Sc::Tech::TotalTypes )
        data->energyCost[(size_t)techType] = energyCost;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) + " is out of range for the TECx section!");
}

OstrSectionPtr OstrSection::GetDefault()
{
    return OstrSectionPtr(new (std::nothrow) OstrSection(Chk::OSTR {
        1, // Version
        0, // scenarioName
        0, // scenarioDescription
        {}, // forceName (all 0)
        {}, // unitName (all 0)
        {}, // expUnitName (all 0)
        {}, // soundPath (all 0)
        {}, // switchName (all 0)
        {} // locationName (all 0)
    }));
}

OstrSection::OstrSection(const Chk::OSTR & data) : StructSection<Chk::OSTR, true>(SectionName::OSTR, data)
{

}

OstrSection::OstrSection() : StructSection<Chk::OSTR, true>(SectionName::OSTR)
{

}

OstrSection::~OstrSection()
{

}

u32 OstrSection::getVersion()
{
    return data->version;
}

u32 OstrSection::getScenarioNameStringId()
{
    return data->scenarioName;
}

u32 OstrSection::getScenarioDescriptionStringId()
{
    return data->scenarioDescription;
}

u32 OstrSection::getForceNameStringId(Chk::Force force)
{
    if ( force < Chk::TotalForces )
        return data->forceName[force];
    else
        throw std::out_of_range(std::string("ForceIndex: ") + std::to_string(force) + " is out of range for the OSTR section!");
}

u32 OstrSection::getUnitNameStringId(Sc::Unit::Type unitType)
{
    if ( unitType < Sc::Unit::TotalTypes )
        return data->unitName[unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the OSTR section!");
}

u32 OstrSection::getExpUnitNameStringId(Sc::Unit::Type unitType)
{
    if ( unitType < Sc::Unit::TotalTypes )
        return data->expUnitName[unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the OSTR section!");
}

u32 OstrSection::getSoundPathStringId(size_t soundIndex)
{
    if ( soundIndex < Chk::TotalSounds )
        return data->soundPath[soundIndex];
    else
        throw std::out_of_range(std::string("SoundIndex: ") + std::to_string((u32)soundIndex) + " is out of range for the OSTR section!");
}

u32 OstrSection::getSwitchNameStringId(size_t switchIndex)
{
    if ( switchIndex < Chk::TotalSwitches )
        return data->switchName[switchIndex];
    else
        throw std::out_of_range(std::string("switchIndex: ") + std::to_string((u32)switchIndex) + " is out of range for the OSTR section!");
}

u32 OstrSection::getLocationNameStringId(size_t locationId)
{
    if ( locationId > 0 && locationId <= Chk::TotalLocations )
        return data->locationName[locationId-1];
    else
        throw std::out_of_range(std::string("locationId: ") + std::to_string((u32)locationId) + " is out of range for the OSTR section!");
}

void OstrSection::setScenarioNameStringId(u32 scenarioNameStringId)
{
    data->scenarioName = scenarioNameStringId;
}

void OstrSection::setScenarioDescriptionStringId(u32 scenarioDescriptionStringId)
{
    data->scenarioDescription = scenarioDescriptionStringId;
}

void OstrSection::setForceNameStringId(Chk::Force force, u32 forceNameStringId)
{
    if ( force < Chk::TotalForces )
        data->forceName[force] = forceNameStringId;
    else
        throw std::out_of_range(std::string("Force: ") + std::to_string(force) + " is out of range for the OSTR section!");
}

void OstrSection::setUnitNameStringId(Sc::Unit::Type unitType, u32 unitNameStringId)
{
    if ( unitType < Sc::Unit::TotalTypes )
        data->unitName[unitType] = unitNameStringId;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the OSTR section!");
}

void OstrSection::setExpUnitNameStringId(Sc::Unit::Type unitType, u32 expUnitNameStringId)
{
    if ( unitType < Sc::Unit::TotalTypes )
        data->expUnitName[unitType] = expUnitNameStringId;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the OSTR section!");
}

void OstrSection::setSoundPathStringId(size_t soundIndex, u32 soundPathStringId)
{
    if ( soundIndex < Chk::TotalSounds )
        data->soundPath[soundIndex] = soundPathStringId;
    else
        throw std::out_of_range(std::string("SoundIndex: ") + std::to_string((u32)soundIndex) + " is out of range for the OSTR section!");
}

void OstrSection::setSwitchNameStringId(size_t switchIndex, u32 switchNameStringId)
{
    if ( switchIndex < Chk::TotalSwitches )
        data->switchName[switchIndex] = switchNameStringId;
    else
        throw std::out_of_range(std::string("switchIndex: ") + std::to_string((u32)switchIndex) + " is out of range for the OSTR section!");
}

void OstrSection::setLocationNameStringId(size_t locationId, u32 locationNameStringId)
{
    if ( locationId > 0 && locationId <= Chk::TotalLocations )
        data->locationName[locationId-1] = locationNameStringId;
    else
        throw std::out_of_range(std::string("locationId: ") + std::to_string((u32)locationId) + " is out of range for the OSTR section!");
}

bool OstrSection::stringUsed(size_t stringId, u32 userMask)
{
    if ( data->scenarioName == stringId || data->scenarioDescription == stringId )
        return true;

    for ( size_t i=0; i<Chk::TotalForces; i++ )
    {
        if ( data->forceName[i] == stringId )
            return true;
    }

    for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
    {
        if ( data->unitName[i] == stringId )
            return true;
    }

    for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
    {
        if ( data->expUnitName[i] == stringId )
            return true;
    }

    for ( size_t i=0; i<Chk::TotalSounds; i++ )
    {
        if ( data->soundPath[i] == stringId )
            return true;
    }

    for ( size_t i=0; i<Chk::TotalSwitches; i++ )
    {
        if ( data->switchName[i] == stringId )
            return true;
    }

    for ( size_t i=0; i<Chk::TotalLocations; i++ )
    {
        if ( data->locationName[i] == stringId )
            return true;
    }

    return false;
}

void OstrSection::markUsedStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask)
{
    if ( (userMask & Chk::StringUserFlag::ScenarioName) == Chk::StringUserFlag::ScenarioName && data->scenarioName != 0 )
        stringIdUsed[data->scenarioName] = true;

    if ( (userMask & Chk::StringUserFlag::ScenarioDescription) == Chk::StringUserFlag::ScenarioDescription && data->scenarioDescription != 0 )
        stringIdUsed[data->scenarioDescription] = true;

    if ( (userMask & Chk::StringUserFlag::Force) == Chk::StringUserFlag::Force )
    {
        for ( size_t i=0; i<Chk::TotalForces; i++ )
        {
            if ( data->forceName[i] != 0 )
                stringIdUsed[data->forceName[i]] = true;
        }
    }

    if ( (userMask & Chk::StringUserFlag::OriginalUnitSettings) == Chk::StringUserFlag::OriginalUnitSettings )
    {
        for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
        {
            if ( data->unitName[i] != 0 )
                stringIdUsed[data->unitName[i]] = true;
        }
    }

    if ( (userMask & Chk::StringUserFlag::ExpansionUnitSettings) == Chk::StringUserFlag::ExpansionUnitSettings )
    {
        for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
        {
            if ( data->expUnitName[i] != 0 )
                stringIdUsed[data->expUnitName[i]] = true;
        }
    }

    if ( (userMask & Chk::StringUserFlag::Sound) == Chk::StringUserFlag::Sound )
    {
        for ( size_t i=0; i<Chk::TotalSounds; i++ )
        {
            if ( data->soundPath[i] != 0 )
                stringIdUsed[data->soundPath[i]] = true;
        }
    }

    if ( (userMask & Chk::StringUserFlag::Switch) == Chk::StringUserFlag::Switch )
    {
        for ( size_t i=0; i<Chk::TotalSwitches; i++ )
        {
            if ( data->switchName[i] != 0 )
                stringIdUsed[data->switchName[i]] = true;
        }
    }

    if ( (userMask & Chk::StringUserFlag::Location) == Chk::StringUserFlag::Location )
    {
        for ( size_t i=0; i<Chk::TotalLocations; i++ )
        {
            if ( data->locationName[i] != 0 )
                stringIdUsed[data->locationName[i]] = true;
        }
    }
}

void OstrSection::remapStringIds(const std::map<u32, u32> & stringIdRemappings)
{
    auto scenarioNameRemapping = stringIdRemappings.find(data->scenarioName);
    auto scenarioDescriptionRemapping= stringIdRemappings.find(data->scenarioDescription);
    auto forceOneRemapping = stringIdRemappings.find(data->forceName[0]);
    auto forceTwoRemapping = stringIdRemappings.find(data->forceName[1]);
    auto forceThreeRemapping = stringIdRemappings.find(data->forceName[2]);
    auto forceFourRemapping = stringIdRemappings.find(data->forceName[3]);

    if ( scenarioNameRemapping != stringIdRemappings.end() )
        data->scenarioName = scenarioNameRemapping->second;

    if ( scenarioDescriptionRemapping != stringIdRemappings.end() )
        data->scenarioDescription = scenarioDescriptionRemapping->second;
    
    if ( forceOneRemapping != stringIdRemappings.end() )
        data->forceName[0] = forceOneRemapping->second;

    if ( forceTwoRemapping != stringIdRemappings.end() )
        data->forceName[1] = forceTwoRemapping->second;

    if ( forceThreeRemapping != stringIdRemappings.end() )
        data->forceName[2] = forceThreeRemapping->second;

    if ( forceFourRemapping != stringIdRemappings.end() )
        data->forceName[3] = forceFourRemapping->second;
    
    for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
    {
        auto found = stringIdRemappings.find(data->unitName[i]);
        if ( found != stringIdRemappings.end() )
            data->unitName[i] = found->second;
    }

    for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
    {
        auto found = stringIdRemappings.find(data->expUnitName[i]);
        if ( found != stringIdRemappings.end() )
            data->expUnitName[i] = found->second;
    }
    
    for ( size_t i=0; i<Chk::TotalSounds; i++ )
    {
        auto found = stringIdRemappings.find(data->soundPath[i]);
        if ( found != stringIdRemappings.end() )
            data->soundPath[i] = found->second;
    }

    for ( size_t i=0; i<Chk::TotalSwitches; i++ )
    {
        auto found = stringIdRemappings.find(data->switchName[i]);
        if ( found != stringIdRemappings.end() )
            data->switchName[i] = found->second;
    }

    for ( size_t i=0; i<Chk::TotalLocations; i++ )
    {
        auto found = stringIdRemappings.find(data->locationName[i]);
        if ( found != stringIdRemappings.end() )
            data->locationName[i] = found->second;
    }
}

void OstrSection::deleteString(size_t stringId)
{
    if ( data->scenarioName == stringId )
        data->scenarioName = 0;
    
    if ( data->scenarioDescription == stringId )
        data->scenarioDescription = 0;

    for ( size_t i=0; i<Chk::TotalForces; i++ )
    {
        if ( data->forceName[i] == stringId )
            data->forceName[i] = 0;
    }

    for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
    {
        if ( data->unitName[i] == stringId )
            data->unitName[i] = 0;
    }

    for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
    {
        if ( data->expUnitName[i] == stringId )
            data->expUnitName[i] = 0;
    }

    for ( size_t i=0; i<Chk::TotalSounds; i++ )
    {
        if ( data->soundPath[i] == stringId )
            data->soundPath[i] = 0;
    }

    for ( size_t i=0; i<Chk::TotalSwitches; i++ )
    {
        if ( data->switchName[i] == stringId )
            data->switchName[i] = 0;
    }

    for ( size_t i=0; i<Chk::TotalLocations; i++ )
    {
        if ( data->locationName[i] == stringId )
            data->locationName[i] = 0;
    }
}

KstrSectionPtr KstrSection::GetDefault()
{
    KstrSectionPtr newSection(new (std::nothrow) KstrSection());
    return newSection;
}

KstrSection::KstrSection() : DynamicSection<true>(SectionName::KSTR), version(Chk::KSTR::CurrentVersion)
{

}

KstrSection::~KstrSection()
{

}

bool KstrSection::empty()
{
    for ( auto & string : strings )
    {
        if ( string != nullptr )
            return false;
    }
    return true;
}

size_t KstrSection::getCapacity()
{
    return strings.size();
}

bool KstrSection::stringStored(size_t stringId)
{
    return stringId < strings.size() && strings[stringId] != nullptr;
}

void KstrSection::unmarkUnstoredStrings(std::bitset<Chk::MaxStrings> & stringIdUsed)
{
    size_t limit = std::min((size_t)Chk::MaxKStrings, strings.size());
    size_t stringId = 1;
    for ( ; stringId < limit; ++stringId )
    {
        if ( stringIdUsed[stringId] && strings[stringId] == nullptr )
            stringIdUsed[stringId] = false;
    }
    for ( ; stringId < Chk::MaxStrings; stringId++ )
    {
        if ( stringIdUsed[stringId] )
            stringIdUsed[stringId] = false;
    }
}

StrProp KstrSection::getProperties(size_t stringId)
{
    return stringId < strings.size() && strings[stringId] != nullptr ? strings[stringId]->properties() : StrProp();
}

void KstrSection::setProperties(size_t stringId, const StrProp & strProp)
{
    if ( stringId < strings.size() && strings[stringId] != nullptr )
        strings[stringId]->properties() = strProp;
}

template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkdString (Editor <01>Style)
std::shared_ptr<StringType> KstrSection::getString(size_t stringId)
{
    return stringId < strings.size() && strings[stringId] != nullptr ? strings[stringId]->toString<StringType>() : nullptr;
}
template std::shared_ptr<RawString> KstrSection::getString<RawString>(size_t stringId);
template std::shared_ptr<EscString> KstrSection::getString<EscString>(size_t stringId);
template std::shared_ptr<ChkdString> KstrSection::getString<ChkdString>(size_t stringId);
template std::shared_ptr<SingleLineChkdString> KstrSection::getString<SingleLineChkdString>(size_t stringId);

template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
size_t KstrSection::findString(const StringType & str)
{
    for ( size_t stringId=1; stringId<strings.size(); stringId++ )
    {
        if ( strings[stringId] != nullptr && strings[stringId]->compare<StringType>(str) == 0 )
            return stringId;
    }
    return Chk::StringId::NoString;
}
template size_t KstrSection::findString<RawString>(const RawString & str);
template size_t KstrSection::findString<EscString>(const EscString & str);
template size_t KstrSection::findString<ChkdString>(const ChkdString & str);
template size_t KstrSection::findString<SingleLineChkdString>(const SingleLineChkdString & str);

bool KstrSection::setCapacity(size_t stringCapacity, StrSynchronizer & strSynchronizer, bool autoDefragment)
{
    if ( stringCapacity > Chk::MaxKStrings )
        throw MaximumStringsExceeded();

    std::bitset<Chk::MaxStrings> stringIdUsed;
    strSynchronizer.markValidUsedStrings(stringIdUsed, Chk::Scope::Either, Chk::Scope::Editor);
    size_t numValidUsedStrings = 0;
    size_t highestValidUsedStringId = 0;
    for ( size_t stringId = 1; stringId<Chk::MaxStrings; stringId++ )
    {
        if ( stringIdUsed[stringId] )
        {
            numValidUsedStrings ++;
            highestValidUsedStringId = stringId;
        }
    }

    if ( numValidUsedStrings > stringCapacity )
        throw InsufficientStringCapacity(ChkSection::getNameString(SectionName::STR), numValidUsedStrings, stringCapacity, autoDefragment);
    else if ( highestValidUsedStringId > stringCapacity )
    {
        if ( autoDefragment && numValidUsedStrings <= stringCapacity )
            defragment(strSynchronizer, false);
        else
            throw InsufficientStringCapacity(ChkSection::getNameString(SectionName::STR), numValidUsedStrings, stringCapacity, autoDefragment);
    }
        
    while ( strings.size() < stringCapacity )
        strings.push_back(nullptr);

    while ( strings.size() > stringCapacity )
        strings.pop_back();

    return true;
}
        
template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
size_t KstrSection::addString(const StringType & str, StrSynchronizer & strSynchronizer, bool autoDefragment)
{
    RawString rawString;
    convertStr<StringType, RawString>(str, rawString);

    size_t stringId = findString<StringType>(str);
    if ( stringId != (size_t)Chk::StringId::NoString )
        return stringId; // String already exists, return the id

    std::bitset<Chk::MaxStrings> stringIdUsed;
    strSynchronizer.markUsedStrings(stringIdUsed, Chk::Scope::Either, Chk::Scope::Editor);
    size_t nextUnusedStringId = getNextUnusedStringId(stringIdUsed, true);
        
    if ( nextUnusedStringId >= strings.size() )
        setCapacity(nextUnusedStringId+1, strSynchronizer, autoDefragment);
    else if ( nextUnusedStringId == 0 )
        throw MaximumStringsExceeded();

    strings[nextUnusedStringId] = ScStrPtr(new ScStr(rawString));
    return nextUnusedStringId;
}
template size_t KstrSection::addString<RawString>(const RawString & str, StrSynchronizer & strSynchronizer, bool autoDefragment);
template size_t KstrSection::addString<EscString>(const EscString & str, StrSynchronizer & strSynchronizer, bool autoDefragment);
template size_t KstrSection::addString<ChkdString>(const ChkdString & str, StrSynchronizer & strSynchronizer, bool autoDefragment);
template size_t KstrSection::addString<SingleLineChkdString>(const SingleLineChkdString & str, StrSynchronizer & strSynchronizer, bool autoDefragment);

template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
void KstrSection::replaceString(size_t stringId, const StringType & str)
{
    RawString rawString;
    convertStr<StringType, RawString>(str, rawString);

    if ( stringId < strings.size() )
        strings[stringId] = ScStrPtr(new ScStr(rawString, StrProp()));
}
template void KstrSection::replaceString<RawString>(size_t stringId, const RawString & str);
template void KstrSection::replaceString<EscString>(size_t stringId, const EscString & str);
template void KstrSection::replaceString<ChkdString>(size_t stringId, const ChkdString & str);
template void KstrSection::replaceString<SingleLineChkdString>(size_t stringId, const SingleLineChkdString & str);

void KstrSection::deleteUnusedStrings(StrSynchronizer & strSynchronizer)
{
    std::bitset<65536> stringIdUsed;
    strSynchronizer.markUsedStrings(stringIdUsed, Chk::Scope::Either, Chk::Scope::Editor);
    for ( size_t i=0; i<strings.size(); i++ )
    {
        if ( !stringIdUsed[i] && strings[i] != nullptr )
            strings[i] = nullptr;
    }
}

bool KstrSection::deleteString(size_t stringId, bool deleteOnlyIfUnused, StrSynchronizerPtr strSynchronizer)
{
    if ( !deleteOnlyIfUnused || (strSynchronizer != nullptr && !strSynchronizer->stringUsed(stringId, Chk::Scope::Either, Chk::Scope::Editor, Chk::StringUserFlag::All, false)) )
    {
        if ( stringId < strings.size() )
        {
            strings[stringId] = nullptr;
            return true;
        }
    }
    return false;
}

void KstrSection::moveString(size_t stringIdFrom, size_t stringIdTo, StrSynchronizer & strSynchronizer)
{
    size_t stringIdMin = std::min(stringIdFrom, stringIdTo);
    size_t stringIdMax = std::max(stringIdFrom, stringIdTo);
    if ( stringIdMin > 0 && stringIdMax <= strings.size() && stringIdFrom != stringIdTo )
    {
        std::bitset<Chk::MaxStrings> stringIdUsed;
        strSynchronizer.markUsedStrings(stringIdUsed, Chk::Scope::Editor);
        ScStrPtr selected = strings[stringIdFrom];
        stringIdUsed[stringIdFrom] = false;
        std::map<u32, u32> stringIdRemappings;
        if ( stringIdTo < stringIdFrom ) // Move to a lower stringId, if there are strings in the way, cascade towards stringIdFrom
        {
            while ( stringIdUsed[stringIdTo] ) // There is a block of one or more strings where the selected string needs to go
            {
                for ( size_t stringId = stringIdTo+1; stringId <= stringIdFrom; stringId ++ ) // Find the lowest available stringId past the block
                {
                    if ( !stringIdUsed[stringId] ) // Move the highest stringId remaining in the block to the next available stringId
                    {
                        ScStrPtr highestString = strings[stringId-1];
                        strings[stringId-1] = nullptr;
                        stringIdUsed[stringId-1] = false;
                        strings[stringId] = highestString;
                        stringIdUsed[stringId] = true;
                        stringIdRemappings.insert(std::pair<u32, u32>(u32(stringId-1), (u32)stringId));
                        break;
                    }
                }
            }
            stringIdRemappings.insert(std::pair<u32, u32>((u32)stringIdFrom, (u32)stringIdTo));
        }
        else if ( stringIdTo > stringIdFrom ) // Move to a higher stringId, if there are strings in the way, cascade towards stringIdTo
        {
            while ( stringIdUsed[stringIdTo] ) // There is a block of one or more strings where the selected string needs to go
            {
                for ( size_t stringId = stringIdTo-1; stringId >= stringIdFrom; stringId -- ) // Find the highest available stringId past the block
                {
                    if ( !stringIdUsed[stringId] ) // Move the lowest stringId in the block to the available stringId
                    {
                        ScStrPtr lowestString = strings[stringId+1];
                        strings[stringId+1] = nullptr;
                        stringIdUsed[stringId+1] = false;
                        strings[stringId] = lowestString;
                        stringIdUsed[stringId] = true;
                        stringIdRemappings.insert(std::pair<u32, u32>(u32(stringId+1), (u32)stringId));
                        break;
                    }
                }
            }
        }
        strings[stringIdTo] = selected;
        stringIdRemappings.insert(std::pair<u32, u32>((u32)stringIdFrom, (u32)stringIdTo));
        strSynchronizer.remapStringIds(stringIdRemappings, Chk::Scope::Editor);
    }
}

bool KstrSection::checkFit(StrSynchronizer & strSynchronizer, StrCompressionElevatorPtr compressionElevator)
{
    if ( strings.size() > Chk::MaxStrings )
        return false;

    try {
        strSynchronizer.syncStringsToBytes(strings, stringBytes, compressionElevator);
        return true;
    } catch ( std::exception & ) {
        return false;
    }
}

bool KstrSection::defragment(StrSynchronizer & strSynchronizer, bool matchCapacityToUsage)
{
    size_t nextCandidateStringId = 0;
    size_t numStrings = strings.size();
    std::map<u32, u32> stringIdRemappings;
    for ( size_t i=0; i<numStrings; i++ )
    {
        if ( strings[i] == nullptr )
        {
            for ( size_t j = std::max(i+1, nextCandidateStringId); j < numStrings; j++ )
            {
                if ( strings[j] != nullptr )
                {
                    strings[i] = strings[j];
                    stringIdRemappings.insert(std::pair<u32, u32>((u32)j, (u32)i));
                    break;
                }
            }
        }
    }

    if ( !stringIdRemappings.empty() )
    {
        strSynchronizer.remapStringIds(stringIdRemappings, Chk::Scope::Editor);
        return true;
    }
    return false;
}

u32 KstrSection::getVersion()
{
    return version;
}

void KstrSection::setVersion(u32 version)
{
    if ( version < 2 || version > Chk::KSTR::CurrentVersion )
        throw std::invalid_argument("KSTR Version " + std::to_string(version) + " is invalid!");

    this->version = version;
}

Chk::SectionSize KstrSection::getSize(ScenarioSaver & scenarioSaver)
{
    if ( syncStringsToBytes(scenarioSaver) )
    {
        if ( stringBytes.size() <= ChkSection::MaxChkSectionSize )
            return Chk::SectionSize(stringBytes.size());
        else
            throw MaxSectionSizeExceeded(SectionName::KSTR, std::to_string(stringBytes.size()));
    }
    else
        throw StrSerializationFailure();
}

std::streamsize KstrSection::read(const Chk::SectionHeader & sectionHeader, std::istream & is, bool)
{   
    if ( sectionHeader.sizeInBytes < 0 )
        throw NegativeSectionSize(sectionHeader.name);

    size_t readSize = size_t(sectionHeader.sizeInBytes);
    if ( readSize > 0 )
    {
        stringBytes.assign(readSize, u8(0));
        is.read((char*)&stringBytes[0], std::streamsize(sectionHeader.sizeInBytes));
        syncBytesToStrings();
        return is.gcount();
    }
    else
    {
        stringBytes.clear();
        strings.clear();
    }
    return 0;
}

void KstrSection::write(std::ostream & os, ScenarioSaver & scenarioSaver)
{
    if ( syncStringsToBytes(scenarioSaver) )
        os.write((const char*)&stringBytes[0], (std::streamsize)stringBytes.size());
}

size_t KstrSection::getNextUnusedStringId(std::bitset<Chk::MaxStrings> & stringIdUsed, bool checkBeyondCapacity, size_t firstChecked)
{
    size_t limit = checkBeyondCapacity ? Chk::MaxStrings : strings.size();
    for ( size_t i=firstChecked; i<limit; i++ )
    {
        if ( !stringIdUsed[i] )
            return i;
    }
    return 0;
}

bool KstrSection::stringsMatchBytes()
{
    if ( stringBytes.size() == 0 )
        return false;

    size_t numBytes = stringBytes.size();
    u32 rawNumStrings = 0;
    if ( numBytes >= 4 )
        rawNumStrings = (u32 &)stringBytes[0];
    else if ( numBytes == 3 )
    {
        u8 paddedTriplet[4] = { stringBytes[0], stringBytes[1], stringBytes[2], u8(0) };
        rawNumStrings = (u32 &)paddedTriplet[0];
    }
    else if ( numBytes == 2 )
        rawNumStrings = u32((u16 &)stringBytes[0]);
    else if ( numBytes == 1 )
        rawNumStrings = u32(stringBytes[0]);

    if ( size_t(rawNumStrings) != strings.size()-1 )
        return false; // Size mismatch

    for ( size_t stringId = 1; stringId<strings.size(); ++stringId )
    {
        ScStrPtr scStr = strings[stringId];
        if ( scStr != nullptr )
        {
            size_t offsetPos = sizeof(u32)*stringId;
            if ( offsetPos+1 < numBytes )
            {
                u32 stringOffset = (u32 &)stringBytes[offsetPos];
                if ( size_t(stringOffset)+scStr->length() > numBytes || std::memcmp(scStr->str, &stringBytes[stringOffset], scStr->length()) != 0 )
                    return false; // String out of bounds or not equal
            }
            else // String offset out of bounds
                return false;
        }
    }
    return true;
}

bool KstrSection::syncStringsToBytes(ScenarioSaver & scenarioSaver)
{
    auto strSynchronizer = scenarioSaver.getStrSynchronizer();
    if ( strSynchronizer != nullptr )
        strSynchronizer->syncKstringsToBytes(strings, stringBytes);
    else
    {
        constexpr size_t maxStrings = (size_t(s32_max) - 2*sizeof(u32))/sizeof(u32);
        size_t numStrings = strings.size() > 0 ? strings.size()-1 : 0; // Exclude string at index 0
        if ( numStrings > maxStrings )
            throw MaximumStringsExceeded(ChkSection::getNameString(SectionName::KSTR), numStrings, maxStrings);

        size_t stringPropertiesStart = 2*sizeof(u32)+2*numStrings;
        size_t versionAndSizeAndOffsetAndStringPropertiesAndNulSpace = 2*sizeof(u32) + 2*sizeof(u32)*numStrings + 1;
        size_t sectionSize = versionAndSizeAndOffsetAndStringPropertiesAndNulSpace;
        for ( size_t i=1; i<=numStrings; i++ )
        {
            if ( strings[i] != nullptr )
                sectionSize += strings[i]->length();
        }

        constexpr size_t maxStandardSize = s32_max;
        if ( sectionSize > maxStandardSize )
            throw MaximumCharactersExceeded(ChkSection::getNameString(SectionName::KSTR), sectionSize-versionAndSizeAndOffsetAndStringPropertiesAndNulSpace, maxStandardSize);

        stringBytes.assign(2*sizeof(u32)+2*sizeof(u32)*numStrings, u8(0));
        (u32 &)stringBytes[0] = Chk::KSTR::CurrentVersion;
        (u32 &)stringBytes[sizeof(u32)] = (u32)numStrings;
        u32 initialNulOffset = u32(stringBytes.size());
        stringBytes.push_back(u8('\0')); // Add initial NUL character
        for ( size_t i=1; i<=numStrings; i++ )
        {
            if ( strings[i] == nullptr )
                (u32 &)stringBytes[sizeof(u32)*i] = initialNulOffset;
            else
            {
                auto prop = strings[i]->properties();
                (u32 &)stringBytes[stringPropertiesStart+sizeof(u32)*i] = (u32 &)Chk::StringProperties(prop.red, prop.green, prop.blue, prop.isUsed, prop.hasPriority, prop.isBold, prop.isUnderlined, prop.isItalics, prop.size);
                (u32 &)stringBytes[sizeof(u32)+sizeof(u32)*i] = u32(stringBytes.size());
                stringBytes.insert(stringBytes.end(), strings[i]->str, strings[i]->str+strings[i]->length()+1);
            }
        }
    }
    return true;
}

void KstrSection::syncBytesToStrings()
{
    size_t numBytes = stringBytes.size();
    u32 version = 0;
    if ( numBytes >= 4 )
        this->version = (u32 &)stringBytes[0];
    else
        this->version = 0;

    if ( version > Chk::KSTR::CurrentVersion )
        throw SectionValidationException(Chk::SectionName::KSTR, "Unrecognized KSTR Version: " + std::to_string(version));

    u32 rawNumStrings = 0;
    if ( numBytes >= 8 )
        rawNumStrings = (u32 &)stringBytes[4];
    else if ( numBytes == 7 )
    {
        u8 paddedTriplet[4] = { stringBytes[4], stringBytes[5], stringBytes[6], u8(0) };
        rawNumStrings = (u32 &)paddedTriplet[0];
    }
    else if ( numBytes == 6 )
        rawNumStrings = u32((u16 &)stringBytes[4]);
    else if ( numBytes == 5 )
        rawNumStrings = u32(stringBytes[4]);
    
    size_t highestStringWithValidOffset = std::min(size_t(rawNumStrings), numBytes < 12 ? 0 : (numBytes-8)/4);
    size_t highestStringWithValidProperties = std::min(size_t(rawNumStrings), numBytes < 12 ? 0 : (numBytes-8)/8);
    size_t propertiesStartMinusFour = sizeof(u32)+sizeof(u32)*rawNumStrings;
    strings.clear();
    strings.push_back(nullptr); // Fill the non-existant 0th stringId

    size_t stringId = 1;
    for ( ; stringId <= highestStringWithValidOffset; ++stringId )
    {
        size_t offsetPos = sizeof(u32)+sizeof(u32)*stringId;
        size_t stringOffset = size_t((u32 &)stringBytes[offsetPos]);
        loadString(stringOffset, numBytes);
        if ( stringId <= highestStringWithValidProperties && strings[stringId] != nullptr )
        {
            size_t propertiesPos = propertiesStartMinusFour + sizeof(u32)*stringId;
            Chk::StringProperties properties = (Chk::StringProperties &)stringBytes[propertiesPos];
            strings[stringId]->properties() = StrProp(properties);
        }
    }
    if ( highestStringWithValidOffset < size_t(rawNumStrings) ) // Some offsets aren't within bounds
    {
        if ( numBytes % 4 == 3 ) // Can read three bytes of an offset
        {
            stringId ++;
            u8 paddedTriplet[4] = { stringBytes[numBytes-3], stringBytes[numBytes-2], stringBytes[numBytes-1], u8(0) };
            size_t stringOffset = size_t((u32 &)paddedTriplet[0]);
            loadString(stringOffset, numBytes);
        }
        else if ( numBytes % 4 == 2 ) // Can read two bytes of an offset
        {
            stringId ++;
            size_t stringOffset = size_t((u16 &)stringBytes[numBytes]);
            loadString(stringOffset, numBytes);
        }
        else if ( numBytes % 4 == 1 ) // Can read one byte of an offset
        {
            stringId ++;
            size_t stringOffset = size_t(stringBytes[sizeof(u32)*highestStringWithValidOffset]);
            loadString(stringOffset, numBytes);
        }
        for ( ; stringId <= size_t(rawNumStrings); ++stringId ) // Any remaining strings are fully out of bounds
            strings.push_back(nullptr);
    }
}

void KstrSection::loadString(const size_t & stringOffset, const size_t & sectionSize)
{
    if ( stringOffset < sectionSize )
    {
        auto nextNull = std::find(stringBytes.begin()+stringOffset, stringBytes.end(), u8('\0'));
        if ( nextNull != stringBytes.end() )
        {
            auto nullIndex = std::distance(stringBytes.begin(), nextNull);
            if ( size_t(nullIndex) >= stringOffset ) // Regular string
                strings.push_back(ScStrPtr(new ScStr(std::string((const char*)&stringBytes[stringOffset]))));
            else // String ends where section ends
                strings.push_back(ScStrPtr(new ScStr(std::string((const char*)&stringBytes[stringOffset], sectionSize-stringOffset))));
        }
        else if ( sectionSize > stringOffset ) // String ends where section ends
            strings.push_back(ScStrPtr(new ScStr(std::string((const char*)&stringBytes[stringOffset], sectionSize-stringOffset))));
        else // Character data would be out of bounds
            strings.push_back(nullptr);
    }
    else // Offset is out of bounds
        strings.push_back(nullptr);
}

KtrgSectionPtr KtrgSection::GetDefault()
{
    KtrgSectionPtr newSection(new (std::nothrow) KtrgSection());
    return newSection;
}

KtrgSection::KtrgSection() : DynamicSection<true>(SectionName::KTRG)
{

}

KtrgSection::~KtrgSection()
{

}

bool KtrgSection::empty()
{
    return extendedTrigData.size() < 3; // Indexes 0 and 1 are unused, so section is empty if size is less than 3
}

size_t KtrgSection::numExtendedTriggers()
{
    return extendedTrigData.size();
}

std::shared_ptr<Chk::ExtendedTrigData> KtrgSection::getExtendedTrigger(size_t extendedTriggerIndex)
{
    if ( extendedTriggerIndex < extendedTrigData.size() )
        return extendedTrigData[extendedTriggerIndex];
    else
        return nullptr;
}

size_t KtrgSection::addExtendedTrigger(std::shared_ptr<Chk::ExtendedTrigData> extendedTrigger)
{
    if ( extendedTrigger != nullptr )
    {
        for ( size_t i=0; i<extendedTrigData.size(); i++ )
        {
            if ( extendedTrigData[i] == nullptr && (i & Chk::UnusedExtendedTrigDataIndexCheck) != 0 ) // If index is unused and usable
            {
                extendedTrigData[i] = extendedTrigger;
                return i;
            }
        }

        while ( (extendedTrigData.size() & Chk::UnusedExtendedTrigDataIndexCheck) == 0 ) // While next index is unusable
            extendedTrigData.push_back(nullptr); // Put a nullptr in that position

        extendedTrigData.push_back(extendedTrigger);
        return extendedTrigData.size()-1;
    }
    return 0;
}

void KtrgSection::deleteExtendedTrigger(size_t extendedTriggerIndex)
{
    if ( extendedTriggerIndex < extendedTrigData.size() )
    {
        extendedTrigData[extendedTriggerIndex] = nullptr;
        cleanTail();
    }
}

bool KtrgSection::editorStringUsed(size_t stringId, u32 userMask)
{
    for ( const auto & extendedTrig : extendedTrigData )
    {
        if ( extendedTrig != nullptr && (
            ((userMask & Chk::StringUserFlag::ExtendedTriggerComment) == Chk::StringUserFlag::ExtendedTriggerComment &&
                extendedTrig->commentStringId == stringId) ||
            ((userMask & Chk::StringUserFlag::ExtendedTriggerNotes) == Chk::StringUserFlag::ExtendedTriggerNotes &&
                extendedTrig->notesStringId == stringId) ) )
        {
            return true;
        }
    }
    return false;
}

void KtrgSection::markUsedEditorStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask)
{
    for ( const auto & extendedTrig : extendedTrigData )
    {
        if ( extendedTrig != nullptr )
        {
            if ( (userMask & Chk::StringUserFlag::ExtendedTriggerComment) == Chk::StringUserFlag::ExtendedTriggerComment && extendedTrig->commentStringId != Chk::StringId::NoString )
                stringIdUsed[extendedTrig->commentStringId] = true;

            if ( (userMask & Chk::StringUserFlag::ExtendedTriggerNotes) == Chk::StringUserFlag::ExtendedTriggerNotes && extendedTrig->notesStringId != Chk::StringId::NoString )
                stringIdUsed[extendedTrig->notesStringId] = true;
        }
    }
}

void KtrgSection::remapEditorStringIds(const std::map<u32, u32> & stringIdRemappings)
{
    for ( const auto & extendedTrig : extendedTrigData )
    {
        if ( extendedTrig != nullptr )
        {
            auto replacement = stringIdRemappings.find(extendedTrig->commentStringId);
            if ( replacement != stringIdRemappings.end() )
                extendedTrig->commentStringId = replacement->second;

            replacement = stringIdRemappings.find(extendedTrig->notesStringId);
            if ( replacement != stringIdRemappings.end() )
                extendedTrig->notesStringId = replacement->second;
        }
    }
}

void KtrgSection::deleteEditorString(size_t stringId)
{
    for ( const auto & extendedTrig : extendedTrigData )
    {
        if ( extendedTrig != nullptr )
        {
            if ( extendedTrig->commentStringId == stringId )
                extendedTrig->commentStringId = Chk::StringId::NoString;

            if ( extendedTrig->notesStringId == stringId )
                extendedTrig->notesStringId = Chk::StringId::NoString;
        }
    }
}

Chk::SectionSize KtrgSection::getSize(ScenarioSaver & scenarioSaver)
{
    return Chk::SectionSize(4+extendedTrigData.size()*sizeof(Chk::ExtendedTrigData));
}

std::streamsize KtrgSection::read(const Chk::SectionHeader & sectionHeader, std::istream & is, bool append)
{
    if ( sectionHeader.sizeInBytes < 0 )
        throw NegativeSectionSize(sectionHeader.name);

    size_t readSize = size_t(sectionHeader.sizeInBytes);
    if ( readSize > 0 )
    {
        u32 version = 0;
        is.read((char*)&version, sizeof(u32));
        if ( sizeof(u32) == is.gcount() )
        {
            size_t readNumExtendedTrigData = (readSize-sizeof(u32))/sizeof(Chk::ExtendedTrigData);
            std::vector<Chk::ExtendedTrigData> extendedTrigDataBuffer(readNumExtendedTrigData, Chk::ExtendedTrigData());
            is.read((char*)&extendedTrigDataBuffer[0], std::streamsize(sectionHeader.sizeInBytes));
            for ( size_t i=0; i<readNumExtendedTrigData; i++ )
            {
                if ( (i & Chk::UnusedExtendedTrigDataIndexCheck) == 0 )
                    this->extendedTrigData.push_back(nullptr);
                else
                    this->extendedTrigData.push_back(Chk::ExtendedTrigDataPtr(new Chk::ExtendedTrigData(extendedTrigDataBuffer[i])));
            }
            return sizeof(u32) + is.gcount();
        }
        else
            return is.gcount();
    }
    return 0;
}

void KtrgSection::write(std::ostream & os, ScenarioSaver & scenarioSaver)
{
    u32 version = 2;
    os.write((const char*)&version, sizeof(u32));
    Chk::ExtendedTrigData blank;
    for ( const Chk::ExtendedTrigDataPtr & extendedTrigData : this->extendedTrigData )
    {
        if ( extendedTrigData != nullptr )
            os.write((const char*)extendedTrigData.get(), std::streamsize(sizeof(Chk::ExtendedTrigData)));
        else
            os.write((const char*)&blank, std::streamsize(sizeof(Chk::ExtendedTrigData)));
    }
}

void KtrgSection::cleanTail()
{
    size_t i = extendedTrigData.size();
    for ( ; i > 0 && (((i-1) & Chk::UnusedExtendedTrigDataIndexCheck) == 0 || extendedTrigData[i-1] == nullptr); i-- );

    if ( i == 0 )
        extendedTrigData.clear();
    else if ( i < extendedTrigData.size() )
    {
        auto firstErased = std::next(extendedTrigData.begin(), i);
        extendedTrigData.erase(firstErased, extendedTrigData.end());
    }
}

KtgpSectionPtr KtgpSection::GetDefault()
{
    KtgpSectionPtr newSection(new (std::nothrow) KtgpSection());
    return newSection;
}

KtgpSection::KtgpSection() : DynamicSection<true>(SectionName::KTGP)
{

}

KtgpSection::~KtgpSection()
{

}

bool KtgpSection::empty()
{
    return triggerGroups.empty();
}

Chk::SectionSize KtgpSection::getSize(ScenarioSaver & scenarioSaver)
{
    Chk::SectionSize totalSize = Chk::SectionSize(triggerGroups.size()*sizeof(Chk::TriggerGroupHeader));
    for ( Chk::TriggerGroupPtr & triggerGroup : triggerGroups )
        totalSize += Chk::SectionSize(4*triggerGroup->extendedTrigDataIndexes.size() + 4*triggerGroup->groupIndexes.size());

    return totalSize;
}

std::streamsize KtgpSection::read(const Chk::SectionHeader & sectionHeader, std::istream & is, bool append)
{
    if ( sectionHeader.sizeInBytes < 0 )
        throw NegativeSectionSize(sectionHeader.name);

    size_t readSize = size_t(sectionHeader.sizeInBytes);
    if ( readSize > 0 )
    {
        std::vector<u8> groupBytes(readSize);
        is.read((char*)&groupBytes[0], std::streamsize(sectionHeader.sizeInBytes));
        while ( groupBytes.size() < 4 )
            groupBytes.push_back(u8(0));

        u32 & version = (u32 &)groupBytes[0];
        if ( version != 1 )
            throw SectionValidationException(Chk::SectionName::KTGP, "Unrecognized version: " + std::to_string(version));

        while ( groupBytes.size() < 8 )
            groupBytes.push_back(u8(0));
        
        size_t numGroups = size_t((u32 &)groupBytes[4]);

        for ( size_t i=0; i<numGroups; i++ )
        {
            size_t headerStart = 8 + i*sizeof(Chk::TriggerGroupHeader);
            size_t sizeRequired = headerStart + sizeof(Chk::TriggerGroupHeader);
            if ( groupBytes.size() < sizeRequired )
                groupBytes.insert(groupBytes.end(), sizeRequired-groupBytes.size(), u8(0));

            Chk::TriggerGroupHeader & header = (Chk::TriggerGroupHeader &)groupBytes[headerStart];
            Chk::TriggerGroupPtr triggerGroup = Chk::TriggerGroupPtr(new Chk::TriggerGroup());
            triggerGroup->groupExpanded = (header.flags & Chk::TriggerGroupHeader::Flags::groupExpanded) == Chk::TriggerGroupHeader::Flags::groupExpanded;
            triggerGroup->groupHidden = (header.flags & Chk::TriggerGroupHeader::Flags::groupHidden) == Chk::TriggerGroupHeader::Flags::groupHidden;
            triggerGroup->templateInstanceId = header.templateInstanceId;
            triggerGroup->commentStringId = header.commentStringId;
            triggerGroup->notesStringId = header.notesStringId;
            triggerGroup->parentGroupId = header.parentGroupId;

            if ( header.numTriggers > 0 && header.numGroups > 0 && header.bodyOffset > 0 )
            {
                sizeRequired = size_t(header.bodyOffset) + size_t(4*header.numTriggers) + size_t(4*header.numGroups);
                if ( groupBytes.size() < sizeRequired )
                    groupBytes.insert(groupBytes.end(), sizeRequired-groupBytes.size(), u8(0));

                u32* body = (u32*)&groupBytes[header.bodyOffset];
                size_t bodyIndex = 0;
                for ( ; bodyIndex < header.numTriggers; bodyIndex++ )
                    triggerGroup->extendedTrigDataIndexes.push_back(body[bodyIndex]);
                for ( ; bodyIndex < size_t(header.numTriggers) + size_t(header.numGroups); bodyIndex++ )
                    triggerGroup->groupIndexes.push_back(body[bodyIndex]);
            }

            this->triggerGroups.push_back(triggerGroup);
        }

        return is.gcount();
    }
    return 0;
}

void KtgpSection::write(std::ostream & os, ScenarioSaver & scenarioSaver)
{
    u32 version = 1;
    u32 numGroups = (u32)triggerGroups.size();
    std::vector<Chk::TriggerGroupHeader> headers;
    std::vector<u32> bodyData;

    for ( size_t i=0; i<(size_t)numGroups; i++ )
    {
        const Chk::TriggerGroupPtr & triggerGroup = triggerGroups[i];
        Chk::TriggerGroupHeader header = {};
        if ( triggerGroup->groupExpanded )
            header.flags |= Chk::TriggerGroupHeader::Flags::groupExpanded;
        if ( triggerGroup->groupHidden )
            header.flags |= Chk::TriggerGroupHeader::Flags::groupHidden;

        header.templateInstanceId = triggerGroup->templateInstanceId;
        header.numTriggers = (u32)triggerGroup->extendedTrigDataIndexes.size();
        header.numGroups = (u32)triggerGroup->groupIndexes.size();
        header.commentStringId = triggerGroup->commentStringId;
        header.notesStringId = triggerGroup->notesStringId;
        header.parentGroupId = triggerGroup->parentGroupId;
        header.bodyOffset = u32(numGroups*sizeof(Chk::TriggerGroupHeader)+bodyData.size());

        for ( const u32 & extendedTrigDataIndex : triggerGroup->extendedTrigDataIndexes )
            bodyData.push_back(extendedTrigDataIndex);

        for ( const u32 & groupIndex : triggerGroup->groupIndexes )
            bodyData.push_back(groupIndex);
    }

    os.write((const char*)&version, sizeof(u32));
    os.write((const char*)&numGroups, sizeof(u32));
    os.write((const char*)&headers[0], headers.size()*sizeof(Chk::TriggerGroupHeader));
    os.write((const char*)&bodyData[0], bodyData.size()*sizeof(u32));
}

ScenarioSaver & ScenarioSaver::GetDefault()
{
    return defaultScenarioSaver;
}

bool ScenarioSaver::removeExtendedLocations()
{
    return false; // Override to customize behavior
}

StrSynchronizerPtr ScenarioSaver::getStrSynchronizer()
{
    return nullptr; // Override to provide a custom StrSynchronizer
}

ScenarioSaver ScenarioSaver::defaultScenarioSaver;
