#include "Sections.h"
#include <unordered_map>
#include <algorithm>
#include <set>

ChkSection::ChkSection(SectionName sectionName, bool virtualizable, bool isVirtual)
    : rawData(new buffer()), sectionIndex(SectionIndex::UNKNOWN), sectionName(sectionName), virtualizable(virtualizable), dataIsVirtual(isVirtual)
{
    auto foundSectionIndex = sectionIndexes.find(sectionName);
    if ( foundSectionIndex != sectionIndexes.end() )
        sectionIndex = foundSectionIndex->second;
}

template <typename StructType>
ChkSection::ChkSection(SectionName sectionName, const StructType & data, bool virtualizable, bool isVirtual)
    : sectionIndex(SectionIndex::UNKNOWN), sectionName(sectionName), virtualizable(virtualizable), dataIsVirtual(isVirtual), rawData(buffer::make(data))
{
    auto foundSectionIndex = sectionIndexes.find(sectionName);
    if ( foundSectionIndex != sectionIndexes.end() )
        sectionIndex = foundSectionIndex->second;
}

std::ostream & ChkSection::write(std::ostream &s)
{
    u32 size = getSize();
    s << (u32)sectionName << size;
    return writeData(s, size);
}

u32 ChkSection::getSize()
{
    s64 bufferSize = rawData->size();
    if ( bufferSize > (s64)ChkSection::MaxChkSectionSize )
        throw MaxSectionSizeExceeded(sectionName, std::to_string(bufferSize));
    else
        return (u32)rawData->size();
}

LoadBehavior ChkSection::getLoadBehavior(SectionIndex sectionIndex)
{
    auto nonStandardLoadBehavior = nonStandardLoadBehaviors.find(sectionIndex);
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

SectionName ChkSection::sectionNames[] = {
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

    SectionName::KSTR,

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

    std::pair<SectionName, std::string>(SectionName::KSTR, "KSTR"),

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

    std::pair<SectionName, SectionIndex>(SectionName::KSTR, SectionIndex::KSTR),

    std::pair<SectionName, SectionIndex>(SectionName::UNKNOWN, SectionIndex::UNKNOWN)
};

std::unordered_map<SectionIndex, LoadBehavior> ChkSection::nonStandardLoadBehaviors = {
    std::pair<SectionIndex, LoadBehavior>(SectionIndex::MTXM, LoadBehavior::Override),
    std::pair<SectionIndex, LoadBehavior>(SectionIndex::UNIT, LoadBehavior::Append),
    std::pair<SectionIndex, LoadBehavior>(SectionIndex::THG2, LoadBehavior::Append),
    std::pair<SectionIndex, LoadBehavior>(SectionIndex::STR, LoadBehavior::Override),
    std::pair<SectionIndex, LoadBehavior>(SectionIndex::TRIG, LoadBehavior::Append),
    std::pair<SectionIndex, LoadBehavior>(SectionIndex::MBRF, LoadBehavior::Append)
};

StrProp::StrProp() : red(0), green(0), blue(0), isUsed(false), hasPriority(false), isBold(false), isUnderlined(false), isItalics(false), size(Chk::baseFontSize)
{

}

StrProp::StrProp(Chk::StringProperties stringProperties) :
    red(stringProperties.red), green(stringProperties.green), blue(stringProperties.blue),
    isUsed((stringProperties.flags & (u8)Chk::StrFlags::isUsed) == (u8)Chk::StrFlags::isUsed), hasPriority((stringProperties.flags & (u8)Chk::StrFlags::hasPriority) == (u8)Chk::StrFlags::hasPriority),
    isBold((stringProperties.flags & (u8)Chk::StrFlags::bold) == (u8)Chk::StrFlags::bold), isUnderlined((stringProperties.flags & (u8)Chk::StrFlags::underlined) == (u8)Chk::StrFlags::underlined),
    isItalics((stringProperties.flags & (u8)Chk::StrFlags::italics) == (u8)Chk::StrFlags::italics),
    size(Chk::baseFontSize +
        ((stringProperties.flags & (u8)Chk::StrFlags::sizePlusFourSteps) == (u8)Chk::StrFlags::sizePlusFourSteps) ? 4*Chk::fontStepSize : 0 +
        ((stringProperties.flags & (u8)Chk::StrFlags::sizePlusTwoSteps) == (u8)Chk::StrFlags::sizePlusTwoSteps) ? 2*Chk::fontStepSize : 0 +
        ((stringProperties.flags & (u8)Chk::StrFlags::sizePlusOneStep) == (u8)Chk::StrFlags::sizePlusOneStep) ? 1*Chk::fontStepSize : 0)
{
    
}

ScStr::ScStr(const std::string &str) : allocation(str)
{

}

ScStr::ScStr(const std::string &str, const StrProp &strProp) : allocation(str), strProp(strProp)
{

}

bool ScStr::empty()
{
    return parentStr == nullptr ? allocation.empty() : parentStr->empty();
}

size_t ScStr::length()
{
    return parentStr == nullptr ? allocation.length() : strlen(str);
}

template <typename StringType>
int ScStr::compare(const StringType &str)
{
    RawString rawStr;
    ConvertStr<StringType, RawString>(str, rawStr);
    return strcmp(this->str, rawStr.c_str());
}
template int ScStr::compare<RawString>(const RawString &str);
template int ScStr::compare<EscString>(const EscString &str);
template int ScStr::compare<ChkdString>(const ChkdString &str);
template int ScStr::compare<SingleLineChkdString>(const SingleLineChkdString &str);

template <typename StringType>
StringType ScStr::toString()
{
    StringType destStr;
    ConvertStr<RawString, StringType>(str, destStr);
    return destStr;
}
template RawString ScStr::toString<RawString>();
template EscString ScStr::toString<EscString>();
template ChkdString ScStr::toString<ChkdString>();
template SingleLineChkdString ScStr::toString<SingleLineChkdString>();

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
        size_t lhsLength = lhs->parentStr == nullptr ? lhs->allocation.length() : strlen(lhs->str);
        size_t rhsLength = rhs->parentStr == nullptr ? rhs->allocation.length() : strlen(rhs->str);
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
    s64 numTiles = (s64)tileWidth*(s64)tileHeight;
    newSection->rawData->add<u16>(0, numTiles);
    return newSection;
}

MtxmSection::MtxmSection() : DynamicSection<false>(SectionName::MTXM)
{

}

u16 MtxmSection::getTile(size_t tileIndex)
{
    if ( (s64)tileIndex < rawData->size() / 2 )
        return rawData->get<u16>(tileIndex * 2);
    else
        throw std::out_of_range(std::string("TileIndex: ") + std::to_string(tileIndex) + " is past the end of the MTXM section!");
}

void MtxmSection::setTile(size_t tileIndex, u16 tileValue)
{
    if ( (s64)tileIndex < rawData->size() / 2 )
        rawData->replace(tileIndex * 2, tileValue);
    else
        throw std::out_of_range(std::string("TileIndex: ") + std::to_string(tileIndex) + " is past the end of the MTXM section!");
}

void setMtxmOrTileDimensions(BufferPtr rawData, u16 newTileWidth, u16 newTileHeight, u16 oldTileWidth, u16 oldTileHeight, s32 leftEdge, s32 topEdge)
{
    s64 oldLeft = 0, oldTop = 0, oldRight = oldTileWidth, oldBottom = oldTileHeight,
        newLeft = leftEdge, newTop = topEdge, newRight = (s64)leftEdge+(s64)newTileWidth, newBottom = (s64)topEdge+(s64)newTileHeight,
        currTileWidth = oldTileWidth, currTileHeight = oldTileHeight;

    // Remove tiles as neccessary in order: bottom, top, right, left
    if ( newBottom < oldBottom && currTileWidth > 0 && currTileHeight > 0 ) // Rows need to be removed from the bottom
    {
        s64 numRowsRemoved = oldBottom - newBottom;
        s64 numTilesRemoved = numRowsRemoved*currTileWidth;
        s64 numBytesRemoved = 2*numTilesRemoved;
        if ( numBytesRemoved >= rawData->size() )
        {
            rawData->trimTo(0);
            currTileWidth = 0;
            currTileHeight = 0;
        }
        else
        {
            rawData->del(rawData->size() - numBytesRemoved, numBytesRemoved);
            currTileHeight -= numRowsRemoved;
        }
    }

    if ( newTop > oldTop && currTileWidth > 0 && currTileHeight > 0 ) // Rows need to be removed from the top
    {
        s64 numRowsRemoved = newTop - oldTop;
        s64 numTilesRemoved = numRowsRemoved*currTileWidth;
        s64 numBytesRemoved = 2*numTilesRemoved;
        if ( numBytesRemoved >= rawData->size() )
        {
            rawData->trimTo(0);
            currTileWidth = 0;
            currTileHeight = 0;
        }
        else
        {
            rawData->del(0, numBytesRemoved);
            currTileHeight -= numRowsRemoved;
        }
    }

    if ( newRight < oldRight && currTileWidth > 0 && currTileHeight > 0 ) // Columns need to be removed from the right
    {
        s64 numColumnsRemoved = oldRight - newRight;
        if ( numColumnsRemoved >= currTileWidth )
        {
            rawData->trimTo(0);
            currTileWidth = 0;
            currTileHeight = 0;
        }
        else
        {
            s64 numBytesPerRow = 2*currTileWidth;
            s64 numBytesRemovedPerRow = 2*numColumnsRemoved;
            s64 rowDataRemovalOffset = numBytesPerRow - numBytesRemovedPerRow;
            for ( s64 row = currTileHeight-1; row >= 0; row-- )
            {
                s64 rowOffset = row*numBytesPerRow;
                s64 dataRemovalOffset = rowOffset+rowDataRemovalOffset;
                rawData->del(dataRemovalOffset, numBytesRemovedPerRow);
            }
            currTileWidth -= numColumnsRemoved;
        }
    }

    if ( newLeft > oldLeft && currTileWidth > 0 && currTileHeight > 0 ) // Columns need to be removed from the left
    {
        s64 numColumnsRemoved = newLeft - oldLeft;
        if ( numColumnsRemoved >= currTileHeight )
        {
            rawData->trimTo(0);
            currTileWidth = 0;
            currTileHeight = 0;
        }
        else
        {
            s64 numBytesPerRow = 2*currTileWidth;
            s64 numBytesRemovedPerRow = 2*numColumnsRemoved;
            for ( s64 row = currTileHeight-1; row >= 0; row-- )
            {
                s64 rowOffset = row*numBytesPerRow;
                rawData->del(rowOffset, numBytesRemovedPerRow);
            }
            currTileWidth -= numColumnsRemoved;
        }
    }

    if ( currTileWidth == 0 || currTileHeight == 0 )
    {
        s64 numTilesToAdd = currTileWidth*currTileHeight;
        s64 bytesToAdd = 2*numTilesToAdd;
        rawData->trimTo(bytesToAdd);
    }
    else // currTileWidth > 0 && currTileHeight > 0
    {
        // Add tiles as necessary in order: right, left, top, bottom

        if ( newRight > oldRight ) // Columns need to be added to the right
        {
            s64 numBytesPerRow = 2*currTileWidth;
            s64 numColumnsAdded = newRight - oldRight;
            s64 numBytesAddedPerRow = 2*numColumnsAdded;
            for ( s64 row = currTileHeight-1; row >= 0; row-- )
            {
                s64 rowOffset = row*numBytesPerRow;
                s64 dataAdditionOffset = rowOffset + numBytesPerRow;
                rawData->insert<u8>(dataAdditionOffset, 0, numBytesAddedPerRow);
            }
            currTileWidth += numColumnsAdded;
        }

        if ( newLeft < oldLeft ) // Columns need to be added to the left
        {
            s64 numColumnsAdded = oldLeft - newLeft;
            s64 numBytesAddedPerRow = 2*numColumnsAdded;
            for ( s64 row = currTileHeight-1; row >= 0; row-- )
            {
                s64 rowOffset = row*numBytesAddedPerRow;
                rawData->insert<u8>(rowOffset, 0, numBytesAddedPerRow);
            }
            currTileWidth += numColumnsAdded;
        }

        if ( newTop < oldTop ) // Rows need to be added to the top
        {
            s64 numRowsAdded = oldTop - newTop;
            s64 numBytesAdded = 2*numRowsAdded;
            rawData->insert<u8>(0, 0, numBytesAdded);
            currTileHeight += numRowsAdded;
        }

        if ( newBottom > oldBottom ) // Rows need to be added to the bottom
        {
            s64 numRowsAdded = newBottom - oldBottom;
            s64 numBytesAdded = 2*numRowsAdded;
            rawData->add<u8>(0, numBytesAdded);
            currTileHeight += numRowsAdded;
        }
    }
}

void MtxmSection::setDimensions(u16 newTileWidth, u16 newTileHeight, u16 oldTileWidth, u16 oldTileHeight, s32 leftEdge, s32 topEdge)
{
    setMtxmOrTileDimensions(rawData, newTileWidth, newTileHeight, oldTileWidth, oldTileHeight, leftEdge, topEdge);
}

u32 MtxmSection::getSize()
{
    return ChkSection::getSize();
}

std::ostream & MtxmSection::writeData(std::ostream &s, u32 sizeInBytes)
{
    rawData->write(s, false);
    return s;
}


PuniSectionPtr PuniSection::GetDefault()
{
    PuniSectionPtr newSection(new (std::nothrow) PuniSection());
    newSection->rawData->add<u8>(1, sizeof(Chk::PUNI));
    return newSection;
}

PuniSection::PuniSection() : StructSection<Chk::PUNI, false>(SectionName::PUNI)
{

}

bool PuniSection::isUnitBuildable(Sc::Unit::Type unitType, size_t playerIndex)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            return data->playerUnitBuildable[(size_t)unitType][playerIndex] != Chk::Available::No;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PUNI section!");
    }
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((size_t)unitType) + " is out of range for the PUNI section!");
}

bool PuniSection::isUnitDefaultBuildable(Sc::Unit::Type unitType)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        return data->defaultUnitBuildable[(size_t)unitType] != Chk::Available::No;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((size_t)unitType) + " is out of range for the PUNI section!");
}

bool PuniSection::playerUsesDefault(Sc::Unit::Type unitType, size_t playerIndex)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            return data->playerUnitUsesDefault[(size_t)unitType][playerIndex] != Chk::UseDefault::No;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PUNI section!");
    }
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((size_t)unitType) + " is out of range for the PUNI section!");
}

void PuniSection::setUnitBuildable(Sc::Unit::Type unitType, size_t playerIndex, bool buildable)
{
    Chk::Available unitBuildable = buildable ? Chk::Available::Yes : Chk::Available::No;
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            data->playerUnitBuildable[(size_t)unitType][playerIndex] = unitBuildable;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PUNI section!");
    }
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((size_t)unitType) + " is out of range for the PUNI section!");
}

void PuniSection::setUnitDefaultBuildable(Sc::Unit::Type unitType, bool buildable)
{
    Chk::Available unitDefaultBuildable = buildable ? Chk::Available::Yes : Chk::Available::No;
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        data->defaultUnitBuildable[(size_t)unitType] = unitDefaultBuildable;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((size_t)unitType) + " is out of range for the PUNI section!");
}

void PuniSection::setPlayerUsesDefault(Sc::Unit::Type unitType, size_t playerIndex, bool useDefault)
{
    Chk::UseDefault playerUnitUsesDefault = useDefault ? Chk::UseDefault::Yes : Chk::UseDefault::No;
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            data->playerUnitUsesDefault[(size_t)unitType][playerIndex] = playerUnitUsesDefault;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PUNI section!");
    }
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((size_t)unitType) + " is out of range for the PUNI section!");
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

size_t UpgrSection::getMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            return data->playerMaxUpgradeLevel[(size_t)upgradeType][playerIndex];
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the UPGR section!");
    }
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the UPGR section!");
}

size_t UpgrSection::getStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            return (size_t)data->playerStartUpgradeLevel[(size_t)upgradeType][playerIndex];
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the UPGR section!");
    }
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the UPGR section!");
}

size_t UpgrSection::getDefaultMaxUpgradeLevel(Sc::Upgrade::Type upgradeType)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
        return data->defaultMaxLevel[(size_t)upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the UPGR section!");
}

size_t UpgrSection::getDefaultStartUpgradeLevel(Sc::Upgrade::Type upgradeType)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
        return data->defaultStartLevel[(size_t)upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the UPGR section!");
}

bool UpgrSection::playerUsesDefault(Sc::Upgrade::Type upgradeType, size_t playerIndex)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            return data->playerUpgradeUsesDefault[(size_t)upgradeType][playerIndex] != Chk::UseDefault::No;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the UPGR section!");
    }
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the UPGR section!");
}

void UpgrSection::setMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, u8 maxUpgradeLevel)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            data->playerMaxUpgradeLevel[(size_t)upgradeType][playerIndex] = maxUpgradeLevel;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the UPGR section!");
    }
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the UPGR section!");
}

void UpgrSection::setStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, u8 startUpgradeLevel)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            data->playerStartUpgradeLevel[(size_t)upgradeType][playerIndex] = startUpgradeLevel;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the UPGR section!");
    }
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the UPGR section!");
}

void UpgrSection::setDefaultMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, u8 maxUpgradeLevel)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
        data->defaultMaxLevel[(size_t)upgradeType] = maxUpgradeLevel;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the UPGR section!");
}

void UpgrSection::setDefaultStartUpgradeLevel(Sc::Upgrade::Type upgradeType, u8 startUpgradeLevel)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
        data->defaultStartLevel[(size_t)upgradeType] = startUpgradeLevel;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the UPGR section!");
}

void UpgrSection::setPlayerUsesDefault(Sc::Upgrade::Type upgradeType, size_t playerIndex, bool useDefault)
{
    Chk::UseDefault playerUpgradeUsesDefault = useDefault ? Chk::UseDefault::Yes : Chk::UseDefault::No;
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            data->playerUpgradeUsesDefault[(size_t)upgradeType][playerIndex] = playerUpgradeUsesDefault;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the UPGR section!");
    }
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the UPGR section!");
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

u32 UnitSection::getSize()
{
    return u32(units.size() * sizeof(Chk::Unit));
}

std::ostream & UnitSection::writeData(std::ostream &s, u32 sizeInBytes)
{
    for ( auto unit : units )
        s << (*unit);

    return s;
}

IsomSectionPtr IsomSection::GetDefault(u16 tileWidth, u16 tileHeight)
{
    IsomSectionPtr newSection(new (std::nothrow) IsomSection());
    s64 numIsomEntries = (s64(tileWidth) / s64(2) + s64(1)) * (s64(tileHeight) + s64(1)) * s64(4);
    newSection->rawData->add<u16>(0, numIsomEntries);
    return newSection;
}

IsomSection::IsomSection() : DynamicSection<true>(SectionName::ISOM)
{

}

std::shared_ptr<Chk::IsomEntry> IsomSection::getIsomEntry(size_t isomIndex)
{
    size_t dataLocation = isomIndex * sizeof(Chk::IsomEntry);
    if ( s64(dataLocation + sizeof(Chk::IsomEntry)) <= rawData->size() )
        return isomEntries[isomIndex];
    else
        return nullptr;
}

void IsomSection::setDimensions(u16 newTileWidth, u16 newTileHeight, u16 oldTileWidth, u16 oldTileHeight, s32 leftEdge, s32 topEdge)
{
    size_t oldNumIndices = isomEntries.size();
    size_t newNumIndices = (size_t)newTileWidth * (size_t)newTileHeight;
    if ( oldNumIndices < newNumIndices )
    {
        for ( size_t i=oldNumIndices; i<newNumIndices; i++ )
            isomEntries.push_back(std::shared_ptr<Chk::IsomEntry>(new Chk::IsomEntry()));
    }
    else if ( oldNumIndices > newNumIndices )
    {
        auto eraseStart = isomEntries.begin();
        std::advance(eraseStart, newNumIndices);
        isomEntries.erase(eraseStart, isomEntries.end());
    }
}

u32 IsomSection::getSize()
{
    return isVirtual() ? ChkSection::getSize() : u32(isomEntries.size() * sizeof(Chk::IsomEntry));
}

std::ostream & IsomSection::writeData(std::ostream &s, u32 sizeInBytes)
{
    if ( isVirtual() )
        rawData->write(s, false);
    else
    {
        for ( auto isomEntry : isomEntries )
            s << (*isomEntry);
    }
    return s;
}

TileSectionPtr TileSection::GetDefault(u16 tileWidth, u16 tileHeight)
{
    TileSectionPtr newSection(new (std::nothrow) TileSection());
    s64 numTiles = (s64)tileWidth*(s64)tileHeight;
    newSection->rawData->add<u16>(0, numTiles);
    return newSection;
}

TileSection::TileSection() : DynamicSection<true>(SectionName::TILE)
{

}

u16 TileSection::getTile(size_t tileIndex)
{
    return rawData->get<u16>((s64)tileIndex*(s64)2);
}

void TileSection::setTile(size_t tileIndex, u16 tileValue)
{
    rawData->replace<u16>((s64)tileIndex*(s64)2, tileValue);
}

void TileSection::setDimensions(u16 newTileWidth, u16 newTileHeight, u16 oldTileWidth, u16 oldTileHeight, s32 leftEdge, s32 topEdge)
{
    setMtxmOrTileDimensions(rawData, newTileWidth, newTileHeight, oldTileWidth, oldTileHeight, leftEdge, topEdge);
}

u32 TileSection::getSize()
{
    return ChkSection::getSize();
}

std::ostream & TileSection::writeData(std::ostream &s, u32 sizeInBytes)
{
    return rawData->write(s, false);
}

Dd2SectionPtr Dd2Section::GetDefault()
{
    return Dd2SectionPtr(new (std::nothrow) Dd2Section());
}

Dd2Section::Dd2Section() : DynamicSection<true>(SectionName::DD2)
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

u32 Dd2Section::getSize()
{
    return u32(doodads.size() * sizeof(Chk::Doodad));
}

std::ostream & Dd2Section::writeData(std::ostream &s, u32 sizeInBytes)
{
    if ( isVirtual() )
        rawData->write(s, false);
    else
    {
        for ( auto doodad : doodads )
            s << (*doodad);
    }
    return s;
}

Thg2SectionPtr Thg2Section::GetDefault()
{
    return Thg2SectionPtr(new (std::nothrow) Thg2Section());
}

Thg2Section::Thg2Section() : DynamicSection<false>(SectionName::THG2)
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

u32 Thg2Section::getSize()
{
    return u32(sprites.size() * sizeof(Chk::Sprite));
}

std::ostream & Thg2Section::writeData(std::ostream &s, u32 sizeInBytes)
{
    for ( auto sprite : sprites )
        s << (*sprite);

    return s;
}

MaskSectionPtr MaskSection::GetDefault(u16 tileWidth, u16 tileHeight)
{
    MaskSectionPtr newSection(new (std::nothrow) MaskSection());
    s64 numTiles = (s64)tileWidth*(s64)tileHeight;
    newSection->rawData->add<u8>(255, numTiles);
    return newSection;
}

MaskSection::MaskSection() : DynamicSection<true>(SectionName::MASK)
{

}

u8 MaskSection::getFog(size_t tileIndex)
{
    return rawData->get<u8>((s64)tileIndex);
}

void MaskSection::setFog(size_t tileIndex, u8 fogOfWarPlayers)
{
    rawData->replace<u16>((s64)tileIndex, fogOfWarPlayers);
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
        if ( numTilesRemoved >= rawData->size() )
        {
            rawData->trimTo(0);
            currTileWidth = 0;
            currTileHeight = 0;
        }
        else
        {
            rawData->del(rawData->size() - numTilesRemoved, numTilesRemoved);
            currTileHeight -= numRowsRemoved;
        }
    }

    if ( newTop > oldTop && currTileWidth > 0 && currTileHeight > 0 ) // Rows need to be removed from the top
    {
        s64 numRowsRemoved = newTop - oldTop;
        s64 numTilesRemoved = numRowsRemoved*currTileWidth;
        if ( numTilesRemoved >= rawData->size() )
        {
            rawData->trimTo(0);
            currTileWidth = 0;
            currTileHeight = 0;
        }
        else
        {
            rawData->del(0, numTilesRemoved);
            currTileHeight -= numRowsRemoved;
        }
    }

    if ( newRight < oldRight && currTileWidth > 0 && currTileHeight > 0 ) // Columns need to be removed from the right
    {
        s64 numColumnsRemoved = oldRight - newRight;
        if ( numColumnsRemoved >= currTileWidth )
        {
            rawData->trimTo(0);
            currTileWidth = 0;
            currTileHeight = 0;
        }
        else
        {
            s64 rowDataRemovalOffset = currTileWidth - numColumnsRemoved;
            for ( s64 row = currTileHeight-1; row >= 0; row-- )
            {
                s64 rowOffset = row*currTileWidth;
                s64 dataRemovalOffset = rowOffset+rowDataRemovalOffset;
                rawData->del(dataRemovalOffset, numColumnsRemoved);
            }
            currTileWidth -= numColumnsRemoved;
        }
    }

    if ( newLeft > oldLeft && currTileWidth > 0 && currTileHeight > 0 ) // Columns need to be removed from the left
    {
        s64 numColumnsRemoved = newLeft - oldLeft;
        if ( numColumnsRemoved >= currTileHeight )
        {
            rawData->trimTo(0);
            currTileWidth = 0;
            currTileHeight = 0;
        }
        else
        {
            for ( s64 row = currTileHeight-1; row >= 0; row-- )
            {
                s64 rowOffset = row*currTileWidth;
                rawData->del(rowOffset, numColumnsRemoved);
            }
            currTileWidth -= numColumnsRemoved;
        }
    }

    if ( currTileWidth == 0 || currTileHeight == 0 )
    {
        s64 numTilesToAdd = currTileWidth*currTileHeight;
        rawData->trimTo(numTilesToAdd);
    }
    else // currTileWidth > 0 && currTileHeight > 0
    {
        // Add tiles as necessary in order: right, left, top, bottom

        if ( newRight > oldRight ) // Columns need to be added to the right
        {
            s64 numColumnsAdded = newRight - oldRight;
            for ( s64 row = currTileHeight-1; row >= 0; row-- )
            {
                s64 rowOffset = row*currTileWidth;
                s64 dataAdditionOffset = rowOffset + currTileWidth;
                rawData->insert<u8>(dataAdditionOffset, 0, numColumnsAdded);
            }
            currTileWidth += numColumnsAdded;
        }

        if ( newLeft < oldLeft ) // Columns need to be added to the left
        {
            s64 numColumnsAdded = oldLeft - newLeft;
            for ( s64 row = currTileHeight-1; row >= 0; row-- )
            {
                s64 rowOffset = row*numColumnsAdded;
                rawData->insert<u8>(rowOffset, 0, numColumnsAdded);
            }
            currTileWidth += numColumnsAdded;
        }

        if ( newTop < oldTop ) // Rows need to be added to the top
        {
            s64 numRowsAdded = oldTop - newTop;
            rawData->insert<u8>(0, 0, numRowsAdded);
            currTileHeight += numRowsAdded;
        }

        if ( newBottom > oldBottom ) // Rows need to be added to the bottom
        {
            s64 numRowsAdded = newBottom - oldBottom;
            rawData->add<u8>(0, numRowsAdded);
            currTileHeight += numRowsAdded;
        }
    }
}

u32 MaskSection::getSize()
{
    return ChkSection::getSize();
}

std::ostream & MaskSection::writeData(std::ostream &s, u32 sizeInBytes)
{
    return rawData->write(s, false);
}

StrSectionPtr StrSection::GetDefault()
{
    StrSectionPtr newSection(new (std::nothrow) StrSection());
    BufferPtr rawData = newSection->rawData;

    const std::vector<std::string> defaultStrings = {
        /* StringId: 1 */ "Untitled Scenario",
        /* StringId: 2 */ "Destroy all enemy buildings.",
        /* StringId: 3 */ "Anywhere",
        /* StringId: 4 */ "Force 1",
        /* StringId: 5 */ "Force 2",
        /* StringId: 6 */ "Force 3",
        /* StringId: 7 */ "Force 4"
    };

    u16 stringCapacity = 1024;
    u16 characterDataStart = 2 + 2*stringCapacity; // Calculate where character data begins (2 bytes for stringCapacity, then 2*stringCapacity for offsets)
    u16 stringStart = characterDataStart + 1; // Move past initial NUL terminator

    for ( const std::string &string : defaultStrings )
    {
        rawData->add<u16>(stringStart); // Add offset for this string
        stringStart += u16(string.size() + 1); // Advance the length of the string plus 1 for the NUL terminator
    }
    u16 numBlankStrings = stringCapacity - (u16)defaultStrings.size(); // Find the amount of strings that will have unused offsets
    rawData->add<u16>(characterDataStart, numBlankStrings); // Add all the offsets for the unused strings, pointing to the initial NUL terminator

    rawData->add<char>('\0'); // Add the initial NUL terminator
    for ( const std::string &string : defaultStrings )
        rawData->addStr(string.c_str(), string.size()+1); // Add the string plus the NUL terminator

    return newSection;
}

StrSection::StrSection() : DynamicSection<false>(SectionName::STR)
{

}

bool StrSection::isSynced()
{
    return synced;
}

void StrSection::flagUnsynced()
{
    synced = false;
}

void StrSection::syncFromBuffer(StrSynchronizerPtr strSynchronizer)
{
    synced = false;
    strSynchronizer->synchronzieFromStrBuffer(*rawData);
}

void StrSection::syncToBuffer(StrSynchronizerPtr strSynchronizer)
{
    strSynchronizer->synchronizeToStrBuffer(*rawData);
    synced = true;
}

u32 StrSection::getSize()
{
    return synced ? (u32)this->rawData->size() : 0;
}

std::ostream & StrSection::writeData(std::ostream &s, u32 sizeInBytes)
{
    return synced ? rawData->write(s, false) : s;
}

UprpSectionPtr UprpSection::GetDefault()
{
    return UprpSectionPtr(new (std::nothrow) UprpSection(Chk::UPRP {}));
}

UprpSection::UprpSection(const Chk::UPRP & data) : StructSection<Chk::UPRP, false>(SectionName::UPRP, data)
{

}

Chk::Cuwp UprpSection::getCuwp(size_t cuwpIndex)
{
    return data->createUnitProperties[cuwpIndex];
}

void UprpSection::setCuwp(size_t cuwpIndex, const Chk::Cuwp &cuwp)
{
    if ( cuwpIndex < Sc::Unit::MaxCuwps )
        memcpy(&data->createUnitProperties[cuwpIndex], &cuwp, sizeof(Chk::Cuwp));
}

size_t UprpSection::findCuwp(const Chk::Cuwp &cuwp)
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
    Chk::Location unusedLocation;
    Chk::Location anywhereLocation;
    anywhereLocation.right = (s64)tileWidth*(s64)32;
    anywhereLocation.bottom = (s64)tileHeight*(s64)32;
    anywhereLocation.stringId = 3;
    newSection->rawData->add(unusedLocation, 63);
    newSection->rawData->add(anywhereLocation, 1);
    return newSection;
}

MrgnSection::MrgnSection() : DynamicSection<false>(SectionName::MRGN)
{

}

size_t MrgnSection::numLocations()
{
    return locations.size();
}

void MrgnSection::sizeToScOriginal()
{
    size_t numLocations = locations.size();
    if ( numLocations > Chk::TotalOriginalLocations )
    {
        auto firstErased = locations.begin();
        std::advance(firstErased, Chk::TotalOriginalLocations);
        locations.erase(firstErased, locations.end());
    }
    else if ( numLocations < Chk::TotalOriginalLocations )
    {
        for ( size_t i=numLocations; i<Chk::TotalOriginalLocations; i++ )
            locations.push_back(std::shared_ptr<Chk::Location>(new Chk::Location()));
    }
}

void MrgnSection::sizeToScHybridOrExpansion()
{
    size_t numLocations = locations.size();
    if ( numLocations > Chk::TotalLocations )
    {
        auto firstErased = locations.begin();
        std::advance(firstErased, Chk::TotalLocations);
        locations.erase(firstErased, locations.end());
    }
    else if ( numLocations < Chk::TotalLocations )
    {
        for ( size_t i=numLocations; i<Chk::TotalLocations; i++ )
            locations.push_back(std::shared_ptr<Chk::Location>(new Chk::Location()));
    }
}

std::shared_ptr<Chk::Location> MrgnSection::getLocation(size_t locationIndex)
{
    return locations[locationIndex];
}

size_t MrgnSection::addLocation(std::shared_ptr<Chk::Location> location)
{
    locations.push_back(location);
    return locations.size()-1;
}

void MrgnSection::insertLocation(size_t locationIndex, std::shared_ptr<Chk::Location> location)
{
    if ( locationIndex < locations.size() )
    {
        auto position = std::next(locations.begin(), locationIndex);
        locations.insert(position, location);
    }
    else if ( locationIndex == locations.size() )
        locations.push_back(location);
}

void MrgnSection::deleteLocation(size_t locationIndex)
{
    if ( locationIndex < locations.size() )
    {
        auto location = std::next(locations.begin(), locationIndex);
        locations.erase(location);
    }
}

void MrgnSection::moveLocation(size_t locationIndexFrom, size_t locationIndexTo)
{
    size_t locationIndexMin = std::min(locationIndexFrom, locationIndexTo);
    size_t locationIndexMax = std::max(locationIndexFrom, locationIndexTo);
    if ( locationIndexMax < locations.size() && locationIndexFrom != locationIndexTo )
    {
        if ( locationIndexMax-locationIndexMin == 1 && locationIndexMax < locations.size() ) // Move up or down by 1 using swap
            locations[locationIndexMin].swap(locations[locationIndexMax]);
        else // Move up or down by more than one, remove from present location, insert in the list at destination
        {
            auto location = locations[locationIndexFrom];
            auto toErase = std::next(locations.begin(), locationIndexFrom);
            locations.erase(toErase);
            auto insertPosition = std::next(locations.begin(), locationIndexTo-1);
            locations.insert(insertPosition, location);
        }
    }
}

bool MrgnSection::stringUsed(size_t stringId)
{
    u16 u16StringId = (u16)stringId;
    for ( auto location : locations )
    {
        if ( location->stringId == u16StringId )
            return true;
    }
    return false;
}

void MrgnSection::markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    for ( auto location : locations )
    {
        if ( location->stringId > 0 )
            stringIdUsed[location->stringId] = true;
    }
}

void MrgnSection::remapStringIds(std::map<u32, u32> stringIdRemappings)
{
    for ( auto location : locations )
    {
        auto found = stringIdRemappings.find(location->stringId);
        if ( found != stringIdRemappings.end() )
            location->stringId = found->second;
    }
}

void MrgnSection::deleteString(size_t stringId)
{
    for ( auto location : locations )
    {
        if ( location->stringId == stringId )
            location->stringId = 0;
    }
}

u32 MrgnSection::getSize()
{
    return u32(sizeof(Chk::Location) * locations.size());
}

std::ostream & MrgnSection::writeData(std::ostream &s, u32 sizeInBytes)
{
    size_t actualDataSize = sizeof(Chk::Location) * locations.size();
    if ( actualDataSize != (size_t)sizeInBytes )
        throw new SectionSerializationSizeMismatch(SectionName::MRGN, sizeInBytes, actualDataSize);

    for ( auto location : locations )
        s << (*location);

    return s;
}

TrigSectionPtr TrigSection::GetDefault()
{
    return TrigSectionPtr(new (std::nothrow) TrigSection());
}

TrigSection::TrigSection() : DynamicSection<false>(SectionName::TRIG)
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

bool TrigSection::stringUsed(size_t stringId)
{
    for ( auto trigger : triggers )
    {
        if ( trigger->stringUsed(stringId) )
            return true;
    }
    return false;
}

bool TrigSection::gameStringUsed(size_t stringId)
{
    for ( auto trigger : triggers )
    {
        if ( trigger->gameStringUsed(stringId) )
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

void TrigSection::markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    for ( auto trigger : triggers )
        trigger->markUsedStrings(stringIdUsed);
}

void TrigSection::markUsedGameStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    for ( auto trigger : triggers )
        trigger->markUsedGameStrings(stringIdUsed);
}

void TrigSection::markUsedCommentStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    for ( auto trigger : triggers )
        trigger->markUsedCommentStrings(stringIdUsed);
}

void TrigSection::remapStringIds(std::map<u32, u32> stringIdRemappings)
{
    for ( auto trigger : triggers )
        trigger->remapStringIds(stringIdRemappings);
}

void TrigSection::deleteString(size_t stringId)
{
    for ( auto trigger : triggers )
        trigger->deleteString(stringId);
}

u32 TrigSection::getSize()
{
    return u32(sizeof(Chk::Trigger) * triggers.size());
}

std::ostream & TrigSection::writeData(std::ostream &s, u32 sizeInBytes)
{
    size_t actualDataSize = sizeof(Chk::Location) * triggers.size();
    if ( actualDataSize != (size_t)sizeInBytes )
        throw new SectionSerializationSizeMismatch(SectionName::TRIG, sizeInBytes, actualDataSize);

    for ( auto trigger : triggers )
        s << (*trigger);

    return s;
}

MbrfSectionPtr MbrfSection::GetDefault()
{
    return MbrfSectionPtr(new (std::nothrow) MbrfSection());
}

MbrfSection::MbrfSection() : DynamicSection<false>(SectionName::MBRF)
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

bool MbrfSection::stringUsed(size_t stringId)
{
    for ( auto briefingTrigger : briefingTriggers )
    {
        if ( briefingTrigger->briefingStringUsed(stringId) )
            return true;
    }
    return false;
}

void MbrfSection::markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    for ( auto briefingTrigger : briefingTriggers )
        briefingTrigger->markUsedStrings(stringIdUsed);
}

void MbrfSection::remapStringIds(std::map<u32, u32> stringIdRemappings)
{
    for ( auto briefingTrigger : briefingTriggers )
        briefingTrigger->remapBriefingStringIds(stringIdRemappings);
}

void MbrfSection::deleteString(size_t stringId)
{
    for ( auto briefingTrigger : briefingTriggers )
        briefingTrigger->deleteString(stringId);
}

u32 MbrfSection::getSize()
{
    return u32(sizeof(Chk::Trigger) * briefingTriggers.size());
}

std::ostream & MbrfSection::writeData(std::ostream &s, u32 sizeInBytes)
{
    for ( auto briefingTrigger : briefingTriggers )
        s << (*briefingTrigger);

    return s;
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

bool SprpSection::stringUsed(size_t stringId)
{
    return data->scenarioNameStringId == (u16)stringId || data->scenarioDescriptionStringId == (u16)stringId;
}

void SprpSection::deleteString(size_t stringId)
{
    if ( data->scenarioNameStringId == stringId )
        data->scenarioNameStringId = 0;

    if ( data->scenarioDescriptionStringId == stringId )
        data->scenarioDescriptionStringId = 0;
}

void SprpSection::remapStringIds(std::map<u32, u32> stringIdRemappings)
{
    auto scenarioNameRemapping = stringIdRemappings.find(data->scenarioNameStringId);
    auto scenarioDescriptionRemapping = stringIdRemappings.find(data->scenarioDescriptionStringId);

    if ( scenarioNameRemapping != stringIdRemappings.end() )
        data->scenarioNameStringId = scenarioNameRemapping->second;

    if ( scenarioDescriptionRemapping != stringIdRemappings.end() )
        data->scenarioDescriptionStringId = scenarioDescriptionRemapping->second;
}

void SprpSection::markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    if ( data->scenarioNameStringId > 0 )
        stringIdUsed[data->scenarioNameStringId] = true;

    if ( data->scenarioDescriptionStringId > 0 )
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
        { (u8)Chk::ForceFlags::All, (u8)Chk::ForceFlags::All, (u8)Chk::ForceFlags::All, (u8)Chk::ForceFlags::All } // flags
    }));
}

ForcSection::ForcSection(const Chk::FORC & data) : StructSection<Chk::FORC, false>(SectionName::FORC, data)
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
    if ( (size_t)force < Chk::TotalForces )
        return data->forceString[(size_t)force];
    else
        throw std::out_of_range(std::string("Force: ") + std::to_string((u32)force) + " is out of range for the FORC section!");
}

u8 ForcSection::getForceFlags(Chk::Force force)
{
    if ( (size_t)force < Chk::TotalForces )
        return data->flags[(size_t)force];
    else
        throw std::out_of_range(std::string("Force: ") + std::to_string((u32)force) + " is out of range for the FORC section!");
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
    if ( (size_t)force < Chk::TotalForces )
        data->forceString[(size_t)force] = forceStringId;
}

void ForcSection::setForceFlags(Chk::Force force, u8 forceFlags)
{
    if ( (size_t)force < Chk::TotalForces )
        data->flags[(size_t)force] = forceFlags;
}

bool ForcSection::stringUsed(size_t stringId)
{
    return data->forceString[0] == (u16)stringId || data->forceString[1] == (u16)stringId || data->forceString[2] == (u16)stringId || data->forceString[3] == (u16)stringId;
}

void ForcSection::markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    for ( size_t i=0; i<Chk::TotalForces; i++ )
    {
        if ( data->forceString[i] > 0 )
            stringIdUsed[data->forceString[i]] = true;
    }
}

void ForcSection::remapStringIds(std::map<u32, u32> stringIdRemappings)
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

void WavSection::markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    for ( size_t i=0; i<Chk::TotalSounds; i++ )
    {
        if ( data->soundPathStringId[i] > 0 && data->soundPathStringId[i] < Chk::MaxStrings )
            stringIdUsed[data->soundPathStringId[i]] = true;
    }
}

void WavSection::remapStringIds(std::map<u32, u32> stringIdRemappings)
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

bool UnisSection::unitUsesDefaultSettings(Sc::Unit::Type unitType)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        return data->useDefault[(size_t)unitType] != Chk::UseDefault::No;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((size_t)unitType) + " is out of range for the UNIS section!");
}

u32 UnisSection::getUnitHitpoints(Sc::Unit::Type unitType)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        return data->hitpoints[(size_t)unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((size_t)unitType) + " is out of range for the UNIS section!");
}

u16 UnisSection::getUnitShieldPoints(Sc::Unit::Type unitType)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        return data->shieldPoints[(size_t)unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((size_t)unitType) + " is out of range for the UNIS section!");
}

u8 UnisSection::getUnitArmorLevel(Sc::Unit::Type unitType)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        return data->armorLevel[(size_t)unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((size_t)unitType) + " is out of range for the UNIS section!");
}

u16 UnisSection::getUnitBuildTime(Sc::Unit::Type unitType)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        return data->buildTime[(size_t)unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((size_t)unitType) + " is out of range for the UNIS section!");
}

u16 UnisSection::getUnitMineralCost(Sc::Unit::Type unitType)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        return data->mineralCost[(size_t)unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((size_t)unitType) + " is out of range for the UNIS section!");
}

u16 UnisSection::getUnitGasCost(Sc::Unit::Type unitType)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        return data->gasCost[(size_t)unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((size_t)unitType) + " is out of range for the UNIS section!");
}

size_t UnisSection::getUnitNameStringId(Sc::Unit::Type unitType)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        return data->nameStringId[(size_t)unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((size_t)unitType) + " is out of range for the UNIS section!");
}

u16 UnisSection::getWeaponBaseDamage(Sc::Weapon::Type weaponType)
{
    if ( (size_t)weaponType < Sc::Weapon::TotalOriginal )
        return data->baseDamage[(size_t)weaponType];
    else
        throw std::out_of_range(std::string("WeaponType: ") + std::to_string((u32)weaponType) + " is out of range for the UNIS section!");
}

u16 UnisSection::getWeaponUpgradeDamage(Sc::Weapon::Type weaponType)
{
    if ( (size_t)weaponType < Sc::Weapon::TotalOriginal )
        return data->upgradeDamage[(size_t)weaponType];
    else
        throw std::out_of_range(std::string("WeaponType: ") + std::to_string((u32)weaponType) + " is out of range for the UNIS section!");
}

void UnisSection::setUnitUsesDefaultSettings(Sc::Unit::Type unitType, bool useDefault)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        data->useDefault[(size_t)unitType] = useDefault ? Chk::UseDefault::Yes : Chk::UseDefault::No;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((size_t)unitType) + " is out of range for the UNIS section!");
}

void UnisSection::setUnitHitpoints(Sc::Unit::Type unitType, u32 hitpoints)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        data->hitpoints[(size_t)unitType] = hitpoints;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((size_t)unitType) + " is out of range for the UNIS section!");
}

void UnisSection::setUnitShieldPoints(Sc::Unit::Type unitType, u16 shieldPoints)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        data->shieldPoints[(size_t)unitType] = shieldPoints;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((size_t)unitType) + " is out of range for the UNIS section!");
}

void UnisSection::setUnitArmorLevel(Sc::Unit::Type unitType, u8 armorLevel)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        data->armorLevel[(size_t)unitType] = armorLevel;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((size_t)unitType) + " is out of range for the UNIS section!");
}

void UnisSection::setUnitBuildTime(Sc::Unit::Type unitType, u16 buildTime)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        data->buildTime[(size_t)unitType] = buildTime;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((size_t)unitType) + " is out of range for the UNIS section!");
}

void UnisSection::setUnitMineralCost(Sc::Unit::Type unitType, u16 mineralCost)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        data->mineralCost[(size_t)unitType] = mineralCost;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((size_t)unitType) + " is out of range for the UNIS section!");
}

void UnisSection::setUnitGasCost(Sc::Unit::Type unitType, u16 gasCost)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        data->gasCost[(size_t)unitType] = gasCost;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((size_t)unitType) + " is out of range for the UNIS section!");
}

void UnisSection::setUnitNameStringId(Sc::Unit::Type unitType, u16 nameStringId)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        data->nameStringId[(size_t)unitType] = nameStringId;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((size_t)unitType) + " is out of range for the UNIS section!");
}

void UnisSection::setWeaponBaseDamage(Sc::Weapon::Type weaponType, u16 baseDamage)
{
    if ( (size_t)weaponType < Sc::Weapon::TotalOriginal )
        data->baseDamage[(size_t)weaponType] = baseDamage;
    else
        throw std::out_of_range(std::string("WeaponType: ") + std::to_string((u32)weaponType) + " is out of range for the UNIS section!");
}

void UnisSection::setWeaponUpgradeDamage(Sc::Weapon::Type weaponType, u16 upgradeDamage)
{
    if ( (size_t)weaponType < Sc::Weapon::TotalOriginal )
        data->upgradeDamage[(size_t)weaponType] = upgradeDamage;
    else
        throw std::out_of_range(std::string("WeaponType: ") + std::to_string((u32)weaponType) + " is out of range for the UNIS section!");
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

void UnisSection::markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
    {
        if ( data->nameStringId[i] > 0 )
            stringIdUsed[data->nameStringId[i]] = true;
    }
}

void UnisSection::remapStringIds(std::map<u32, u32> stringIdRemappings)
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

bool UpgsSection::upgradeUsesDefaultCosts(Sc::Upgrade::Type upgradeType)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
        return data->useDefault[(size_t)upgradeType] != Chk::UseDefault::No;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the UPGS section!");
}

u16 UpgsSection::getBaseMineralCost(Sc::Upgrade::Type upgradeType)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
        return data->baseMineralCost[(size_t)upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the UPGS section!");
}

u16 UpgsSection::getMineralCostFactor(Sc::Upgrade::Type upgradeType)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
        return data->mineralCostFactor[(size_t)upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the UPGS section!");
}

u16 UpgsSection::getBaseGasCost(Sc::Upgrade::Type upgradeType)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
        return data->baseGasCost[(size_t)upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the UPGS section!");
}

u16 UpgsSection::getGasCostFactor(Sc::Upgrade::Type upgradeType)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
        return data->gasCostFactor[(size_t)upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the UPGS section!");
}

u16 UpgsSection::getBaseResearchTime(Sc::Upgrade::Type upgradeType)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
        return data->baseResearchTime[(size_t)upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the UPGS section!");
}

u16 UpgsSection::getResearchTimeFactor(Sc::Upgrade::Type upgradeType)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
        return data->researchTimeFactor[(size_t)upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the UPGS section!");
}


void UpgsSection::setUpgradeUsesDefaultCosts(Sc::Upgrade::Type upgradeType, bool useDefault)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
        data->useDefault[(size_t)upgradeType] = useDefault ? Chk::UseDefault::Yes : Chk::UseDefault::No;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the UPGS section!");
}

void UpgsSection::setBaseMineralCost(Sc::Upgrade::Type upgradeType, u16 baseMineralCost)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
        data->baseMineralCost[(size_t)upgradeType] = baseMineralCost;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the UPGS section!");
}

void UpgsSection::setMineralCostFactor(Sc::Upgrade::Type upgradeType, u16 mineralCostFactor)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
        data->mineralCostFactor[(size_t)upgradeType] = mineralCostFactor;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the UPGS section!");
}

void UpgsSection::setBaseGasCost(Sc::Upgrade::Type upgradeType, u16 baseGasCost)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
        data->baseGasCost[(size_t)upgradeType] = baseGasCost;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the UPGS section!");
}

void UpgsSection::setGasCostFactor(Sc::Upgrade::Type upgradeType, u16 gasCostFactor)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
        data->gasCostFactor[(size_t)upgradeType] = gasCostFactor;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the UPGS section!");
}

void UpgsSection::setBaseResearchTime(Sc::Upgrade::Type upgradeType, u16 baseResearchTime)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
        data->baseResearchTime[(size_t)upgradeType] = baseResearchTime;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the UPGS section!");
}

void UpgsSection::setResearchTimeFactor(Sc::Upgrade::Type upgradeType, u16 researchTimeFactor)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
        data->researchTimeFactor[(size_t)upgradeType] = researchTimeFactor;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the UPGS section!");
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

void SwnmSection::markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    for ( size_t i=0; i<Chk::TotalSwitches; i++ )
    {
        if ( data->switchName[i] > 0 && data->switchName[i] < Chk::MaxStrings )
            stringIdUsed[data->switchName[i]] = true;
    }
}

void SwnmSection::remapStringIds(std::map<u32, u32> stringIdRemappings)
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

size_t PupxSection::getMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            return data->playerMaxUpgradeLevel[(size_t)upgradeType][playerIndex];
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PUPx section!");
    }
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the PUPx section!");
}

size_t PupxSection::getStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            return data->playerStartUpgradeLevel[(size_t)upgradeType][playerIndex];
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PUPx section!");
    }
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the PUPx section!");
}

size_t PupxSection::getDefaultMaxUpgradeLevel(Sc::Upgrade::Type upgradeType)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
        return data->defaultMaxLevel[(size_t)upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the PUPx section!");
}

size_t PupxSection::getDefaultStartUpgradeLevel(Sc::Upgrade::Type upgradeType)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
        return data->defaultStartLevel[(size_t)upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the PUPx section!");
}

bool PupxSection::playerUsesDefault(Sc::Upgrade::Type upgradeType, size_t playerIndex)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            return data->playerUpgradeUsesDefault[(size_t)upgradeType][playerIndex] != Chk::UseDefault::No;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PUPx section!");
    }
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the PUPx section!");
}

void PupxSection::setMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, size_t maxUpgradeLevel)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            data->playerMaxUpgradeLevel[(size_t)upgradeType][playerIndex] = (u8)maxUpgradeLevel;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PUPx section!");
    }
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the PUPx section!");
}

void PupxSection::setStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, size_t startUpgradeLevel)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            data->playerStartUpgradeLevel[(size_t)upgradeType][playerIndex] = (u8)startUpgradeLevel;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PUPx section!");
    }
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the PUPx section!");
}

void PupxSection::setDefaultMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t maxUpgradeLevel)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
        data->defaultMaxLevel[(size_t)upgradeType] = (u8)maxUpgradeLevel;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the PUPx section!");
}

void PupxSection::setDefaultStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t startUpgradeLevel)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
        data->defaultStartLevel[(size_t)upgradeType] = (u8)startUpgradeLevel;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the PUPx section!");
}

void PupxSection::setPlayerUsesDefault(Sc::Upgrade::Type upgradeType, size_t playerIndex, bool useDefault)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            data->playerUpgradeUsesDefault[(size_t)upgradeType][playerIndex] = useDefault ? Chk::UseDefault::Yes : Chk::UseDefault::No;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PUPx section!");
    }
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the PUPx section!");
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

bool UnixSection::unitUsesDefaultSettings(Sc::Unit::Type unitType)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        return data->useDefault[(size_t)unitType] != Chk::UseDefault::No;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((size_t)unitType) + " is out of range for the UNIS section!");
}

u32 UnixSection::getUnitHitpoints(Sc::Unit::Type unitType)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        return data->hitpoints[(size_t)unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((size_t)unitType) + " is out of range for the UNIx section!");
}

u16 UnixSection::getUnitShieldPoints(Sc::Unit::Type unitType)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        return data->shieldPoints[(size_t)unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((size_t)unitType) + " is out of range for the UNIx section!");
}

u8 UnixSection::getUnitArmorLevel(Sc::Unit::Type unitType)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        return data->armorLevel[(size_t)unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((size_t)unitType) + " is out of range for the UNIx section!");
}

u16 UnixSection::getUnitBuildTime(Sc::Unit::Type unitType)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        return data->buildTime[(size_t)unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((size_t)unitType) + " is out of range for the UNIx section!");
}

u16 UnixSection::getUnitMineralCost(Sc::Unit::Type unitType)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        return data->mineralCost[(size_t)unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((size_t)unitType) + " is out of range for the UNIx section!");
}

u16 UnixSection::getUnitGasCost(Sc::Unit::Type unitType)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        return data->gasCost[(size_t)unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((size_t)unitType) + " is out of range for the UNIx section!");
}

size_t UnixSection::getUnitNameStringId(Sc::Unit::Type unitType)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        return data->nameStringId[(size_t)unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((size_t)unitType) + " is out of range for the UNIx section!");
}

u16 UnixSection::getWeaponBaseDamage(Sc::Weapon::Type weaponType)
{
    if ( (size_t)weaponType < Sc::Weapon::Total )
        return data->baseDamage[(size_t)weaponType];
    else
        throw std::out_of_range(std::string("WeaponType: ") + std::to_string((u32)weaponType) + " is out of range for the UNIx section!");
}

u16 UnixSection::getWeaponUpgradeDamage(Sc::Weapon::Type weaponType)
{
    if ( (size_t)weaponType < Sc::Weapon::Total )
        return data->upgradeDamage[(size_t)weaponType];
    else
        throw std::out_of_range(std::string("WeaponType: ") + std::to_string((u32)weaponType) + " is out of range for the UNIx section!");
}

void UnixSection::setUnitUsesDefaultSettings(Sc::Unit::Type unitType, bool useDefault)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        data->useDefault[(size_t)unitType] = useDefault ? Chk::UseDefault::Yes : Chk::UseDefault::No;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((size_t)unitType) + " is out of range for the UNIx section!");
}

void UnixSection::setUnitHitpoints(Sc::Unit::Type unitType, u32 hitpoints)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        data->hitpoints[(size_t)unitType] = hitpoints;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((size_t)unitType) + " is out of range for the UNIx section!");
}

void UnixSection::setUnitShieldPoints(Sc::Unit::Type unitType, u16 shieldPoints)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        data->shieldPoints[(size_t)unitType] = shieldPoints;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((size_t)unitType) + " is out of range for the UNIx section!");
}

void UnixSection::setUnitArmorLevel(Sc::Unit::Type unitType, u8 armorLevel)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        data->armorLevel[(size_t)unitType] = armorLevel;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((size_t)unitType) + " is out of range for the UNIx section!");
}

void UnixSection::setUnitBuildTime(Sc::Unit::Type unitType, u16 buildTime)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        data->buildTime[(size_t)unitType] = buildTime;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((size_t)unitType) + " is out of range for the UNIx section!");
}

void UnixSection::setUnitMineralCost(Sc::Unit::Type unitType, u16 mineralCost)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        data->mineralCost[(size_t)unitType] = mineralCost;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((size_t)unitType) + " is out of range for the UNIx section!");
}

void UnixSection::setUnitGasCost(Sc::Unit::Type unitType, u16 gasCost)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        data->gasCost[(size_t)unitType] = gasCost;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((size_t)unitType) + " is out of range for the UNIx section!");
}

void UnixSection::setUnitNameStringId(Sc::Unit::Type unitType, u16 nameStringId)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        data->nameStringId[(size_t)unitType] = nameStringId;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((size_t)unitType) + " is out of range for the UNIx section!");
}

void UnixSection::setWeaponBaseDamage(Sc::Weapon::Type weaponType, u16 baseDamage)
{
    if ( (size_t)weaponType < Sc::Weapon::Total )
        data->baseDamage[(size_t)weaponType] = baseDamage;
    else
        throw std::out_of_range(std::string("WeaponType: ") + std::to_string((u32)weaponType) + " is out of range for the UNIx section!");
}

void UnixSection::setWeaponUpgradeDamage(Sc::Weapon::Type weaponType, u16 upgradeDamage)
{
    if ( (size_t)weaponType < Sc::Weapon::Total )
        data->upgradeDamage[(size_t)weaponType] = upgradeDamage;
    else
        throw std::out_of_range(std::string("WeaponType: ") + std::to_string((u32)weaponType) + " is out of range for the UNIx section!");
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

void UnixSection::markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
    {
        if ( data->nameStringId[i] > 0 )
            stringIdUsed[data->nameStringId[i]] = true;
    }
}

void UnixSection::remapStringIds(std::map<u32, u32> stringIdRemappings)
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

bool UpgxSection::upgradeUsesDefaultCosts(Sc::Upgrade::Type upgradeType)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
        return data->useDefault[(size_t)upgradeType] != Chk::UseDefault::No;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the UPGx section!");
}

u16 UpgxSection::getBaseMineralCost(Sc::Upgrade::Type upgradeType)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
        return data->baseMineralCost[(size_t)upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the UPGx section!");
}

u16 UpgxSection::getMineralCostFactor(Sc::Upgrade::Type upgradeType)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
        return data->mineralCostFactor[(size_t)upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the UPGx section!");
}

u16 UpgxSection::getBaseGasCost(Sc::Upgrade::Type upgradeType)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
        return data->baseGasCost[(size_t)upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the UPGx section!");
}

u16 UpgxSection::getGasCostFactor(Sc::Upgrade::Type upgradeType)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
        return data->gasCostFactor[(size_t)upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the UPGx section!");
}

u16 UpgxSection::getBaseResearchTime(Sc::Upgrade::Type upgradeType)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
        return data->baseResearchTime[(size_t)upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the UPGx section!");
}

u16 UpgxSection::getResearchTimeFactor(Sc::Upgrade::Type upgradeType)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
        return data->researchTimeFactor[(size_t)upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the UPGx section!");
}


void UpgxSection::setUpgradeUsesDefaultCosts(Sc::Upgrade::Type upgradeType, bool useDefault)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
        data->useDefault[(size_t)upgradeType] = useDefault ? Chk::UseDefault::Yes : Chk::UseDefault::No;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the UPGx section!");
}

void UpgxSection::setBaseMineralCost(Sc::Upgrade::Type upgradeType, u16 baseMineralCost)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
        data->baseMineralCost[(size_t)upgradeType] = baseMineralCost;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the UPGx section!");
}

void UpgxSection::setMineralCostFactor(Sc::Upgrade::Type upgradeType, u16 mineralCostFactor)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
        data->mineralCostFactor[(size_t)upgradeType] = mineralCostFactor;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the UPGx section!");
}

void UpgxSection::setBaseGasCost(Sc::Upgrade::Type upgradeType, u16 baseGasCost)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
        data->baseGasCost[(size_t)upgradeType] = baseGasCost;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the UPGx section!");
}

void UpgxSection::setGasCostFactor(Sc::Upgrade::Type upgradeType, u16 gasCostFactor)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
        data->gasCostFactor[(size_t)upgradeType] = gasCostFactor;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the UPGx section!");
}

void UpgxSection::setBaseResearchTime(Sc::Upgrade::Type upgradeType, u16 baseResearchTime)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
        data->baseResearchTime[(size_t)upgradeType] = baseResearchTime;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the UPGx section!");
}

void UpgxSection::setResearchTimeFactor(Sc::Upgrade::Type upgradeType, u16 researchTimeFactor)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
        data->researchTimeFactor[(size_t)upgradeType] = researchTimeFactor;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) + " is out of range for the UPGx section!");
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
    if ( (size_t)techType < Sc::Tech::TotalTypes )
        data->researchTime[(size_t)techType] = researchTime;
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
    if ( (size_t)force < Chk::TotalForces )
        return data->forceName[(size_t)force];
    else
        throw std::out_of_range(std::string("ForceIndex: ") + std::to_string((u32)force) + " is out of range for the OSTR section!");
}

u32 OstrSection::getUnitNameStringId(Sc::Unit::Type unitType)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        return data->unitName[(size_t)unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((size_t)unitType) + " is out of range for the OSTR section!");
}

u32 OstrSection::getExpUnitNameStringId(Sc::Unit::Type unitType)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        return data->expUnitName[(size_t)unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((size_t)unitType) + " is out of range for the OSTR section!");
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

u32 OstrSection::getLocationNameStringId(size_t locationIndex)
{
    if ( locationIndex < Chk::TotalLocations )
        return data->locationName[locationIndex];
    else
        throw std::out_of_range(std::string("locationIndex: ") + std::to_string((u32)locationIndex) + " is out of range for the OSTR section!");
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
    if ( (size_t)force < Chk::TotalForces )
        data->forceName[(size_t)force] = forceNameStringId;
    else
        throw std::out_of_range(std::string("Force: ") + std::to_string((u32)force) + " is out of range for the OSTR section!");
}

void OstrSection::setUnitNameStringId(Sc::Unit::Type unitType, u32 unitNameStringId)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        data->unitName[(size_t)unitType] = unitNameStringId;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((size_t)unitType) + " is out of range for the OSTR section!");
}

void OstrSection::setExpUnitNameStringId(Sc::Unit::Type unitType, u32 expUnitNameStringId)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        data->expUnitName[(size_t)unitType] = expUnitNameStringId;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((size_t)unitType) + " is out of range for the OSTR section!");
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

void OstrSection::setLocationNameStringId(size_t locationIndex, u32 locationNameStringId)
{
    if ( locationIndex < Chk::TotalLocations )
        data->locationName[locationIndex] = locationNameStringId;
    else
        throw std::out_of_range(std::string("locationIndex: ") + std::to_string((u32)locationIndex) + " is out of range for the OSTR section!");
}

bool OstrSection::stringUsed(size_t stringId)
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

void OstrSection::markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    if ( data->scenarioName != 0 )
        stringIdUsed[data->scenarioName] = true;

    if ( data->scenarioDescription != 0 )
        stringIdUsed[data->scenarioDescription] = true;

    for ( size_t i=0; i<Chk::TotalForces; i++ )
    {
        if ( data->forceName[i] != 0 )
            stringIdUsed[data->forceName[i]] = true;
    }

    for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
    {
        if ( data->unitName[i] != 0 )
            stringIdUsed[data->unitName[i]] = true;
    }

    for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
    {
        if ( data->expUnitName[i] != 0 )
            stringIdUsed[data->expUnitName[i]] = true;
    }

    for ( size_t i=0; i<Chk::TotalSounds; i++ )
    {
        if ( data->soundPath[i] != 0 )
            stringIdUsed[data->soundPath[i]] = true;
    }

    for ( size_t i=0; i<Chk::TotalSwitches; i++ )
    {
        if ( data->switchName[i] != 0 )
            stringIdUsed[data->switchName[i]] = true;
    }

    for ( size_t i=0; i<Chk::TotalLocations; i++ )
    {
        if ( data->locationName[i] != 0 )
            stringIdUsed[data->locationName[i]] = true;
    }
}

void OstrSection::remapStringIds(std::map<u32, u32> stringIdRemappings)
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
    newSection->synced = false;
    return newSection;
}

KstrSection::KstrSection() : DynamicSection<true>(SectionName::KSTR)
{

}

bool KstrSection::isSynced()
{
    return synced;
}

void KstrSection::flagUnsynced()
{
    synced = false;
}

void KstrSection::syncFromBuffer(StrSynchronizerPtr strSynchronizer)
{
    synced = false;
    strSynchronizer->synchronzieFromStrBuffer(*rawData);
}

void KstrSection::syncToBuffer(StrSynchronizerPtr strSynchronizer)
{
    strSynchronizer->synchronizeToStrBuffer(*rawData);
    synced = true;
}

u32 KstrSection::getSize()
{
    return synced ? (u32)this->rawData->size() : 0;
}

std::ostream & KstrSection::writeData(std::ostream &s, u32 sizeInBytes)
{
    return synced ? rawData->write(s, false) : s;
}
