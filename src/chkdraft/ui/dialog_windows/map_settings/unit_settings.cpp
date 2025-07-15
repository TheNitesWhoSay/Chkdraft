#include "unit_settings.h"
#include "chkdraft/chkdraft.h"
#include <sstream>
#include <string>

enum_t(Id, u32, {
    TREE_UNITSETTINGS = ID_FIRST,
    BUTTON_RESETALLUNITDEFAULTS,
    CHECK_USEUNITDEFAULTS,
    CHECK_ENABLEDBYDEFAULT,
    GROUP_UNITPROPERTIES,
    GROUP_GROUNDWEAPON,
    GROUP_AIRWEAPON,
    GROUP_UNITNAME,
    GROUP_UNITAVAILABILITY,
    TEXT_UNITHITPOINTS,
    EDIT_UNITHITPOINTS,
    EDIT_UNITHITPOINTSBYTE,
    TEXT_UNITSHIELDPOINTS,
    EDIT_UNITSHIELDPOINTS,
    TEXT_UNITARMOR,
    EDIT_UNITARMOR,
    TEXT_UNITBUILDTIME,
    EDIT_UNITBUILDTIME,
    TEXT_UNITMINERALCOST,
    EDIT_UNITMINERALCOST,
    TEXT_UNITGASCOST,
    EDIT_UNITGASCOST,
    TEXT_UNITGROUNDDAMAGE,
    EDIT_UNITGROUNDDAMAGE,
    TEXT_UNITGROUNDBONUS,
    EDIT_UNITGROUNDBONUS,
    TEXT_UNITAIRDAMAGE,
    EDIT_UNITAIRDAMAGE,
    TEXT_UNITAIRBONUS,
    EDIT_UNITAIRBONUS,
    CHECK_USEDEFAULTUNITNAME,
    EDIT_UNITNAME,
    TEXT_P1UNITAVAILABILITY,
    TEXT_P12UNITAVAILABILITY = (TEXT_P1UNITAVAILABILITY + 11),
    DROP_P1UNITAVAILABILITY,
    DROP_P12UNITAVAILABILITY = (DROP_P1UNITAVAILABILITY + 11)
});

UnitSettingsWindow::UnitSettingsWindow() : selectedUnitType(Sc::Unit::Type::NoUnit), possibleUnitNameUpdate(false), isDisabled(true), refreshing(false)
{

}

UnitSettingsWindow::~UnitSettingsWindow()
{

}

bool UnitSettingsWindow::CreateThis(HWND hParent, u64 windowId)
{
    if ( getHandle() != NULL )
        return SetParent(hParent);

    if ( ClassWindow::RegisterWindowClass(0, NULL, NULL, NULL, NULL, "UnitSettings", NULL, false) &&
         ClassWindow::CreateClassWindow(0, "UnitSettings", WS_VISIBLE|WS_CHILD, 4, 22, 592, 524, hParent, (HMENU)windowId) )
    {
        CreateSubWindows(getHandle());
        return true;
    }
    else
        return false;
}

bool UnitSettingsWindow::DestroyThis()
{
    ClassWindow::DestroyThis();
    this->selectedUnitType = Sc::Unit::Type::NoUnit;
    this->possibleUnitNameUpdate = false;
    this->isDisabled = true;
    this->refreshing = false;
    return true;
}

void UnitSettingsWindow::RefreshWindow()
{
    refreshing = true;
    if ( selectedUnitType != Sc::Unit::Type::NoUnit && (u16)selectedUnitType < Sc::Unit::TotalTypes && CM != nullptr )
    {
        if ( isDisabled )
            EnableUnitEditing();

        bool unitUsesDefaultSettings = CM->unitUsesDefaultSettings(selectedUnitType);
        checkUseUnitDefaults.SetCheck(unitUsesDefaultSettings);
        if ( unitUsesDefaultSettings )
            DisableUnitProperties();
        else
            EnableUnitProperties();

        const Sc::Unit::DatEntry & unitDat = chkd.scData.units.getUnit(selectedUnitType);

        u32 groundWeapon = (u32)unitDat.groundWeapon,
            airWeapon    = (u32)unitDat.airWeapon;
        Sc::Unit::Type subUnitId = unitDat.subunit1;
        
        if ( subUnitId != 228 ) // If unit has a subunit
        {
            if ( groundWeapon == 130 ) // If unit might have a subunit ground attack
                groundWeapon = chkd.scData.units.getUnit(subUnitId).groundWeapon;
            if ( airWeapon == 130 ) // If unit might have a subunit air attack
                airWeapon = chkd.scData.units.getUnit(subUnitId).airWeapon;
        }

        if ( groundWeapon == 130 )
        {
            groupGroundWeapon.SetWinText("No Ground Weapon");
            groupGroundWeapon.DisableThis();
            editGroundDamage.SetWinText("");
            editGroundDamage.DisableThis();
            editGroundBonus.SetWinText("");
            editGroundBonus.DisableThis();
            textGroundDamage.DisableThis();
            textGroundBonus.DisableThis();
        }
        else
        {
            groupGroundWeapon.SetWinText("Ground Weapon [" + weaponNames.at(groundWeapon) + "]");
        }

        if ( airWeapon == 130 || airWeapon == groundWeapon )
        {
            if ( airWeapon != 130 && airWeapon == groundWeapon )
            {
                groupAirWeapon.SetWinText("Air Weapon [" + weaponNames.at(airWeapon) + "]");
            }
            else
                groupAirWeapon.SetWinText("No Air Weapon");

            groupAirWeapon.DisableThis();
            editAirDamage.SetText("");
            editAirDamage.DisableThis();
            editAirBonus.SetText("");
            editAirBonus.DisableThis();
            textAirDamage.DisableThis();
            textAirBonus.DisableThis();
        }
        else
        {
            groupAirWeapon.SetWinText("Air Weapon [" + weaponNames.at(airWeapon) + "]");
        }

        if ( unitUsesDefaultSettings )
        {
            editHitPoints.SetEditNum<u32>(unitDat.hitPoints/256);
            editHitPointsByte.SetEditNum<u32>(unitDat.hitPoints%256);
            editShieldPoints.SetEditNum<u16>(unitDat.shieldAmount);
            editArmor.SetEditNum<u8>(unitDat.armor);
            editBuildTime.SetEditNum<u16>(unitDat.buildTime);
            editMineralCost.SetEditNum<u16>(unitDat.mineralCost);
            editGasCost.SetEditNum<u16>(unitDat.vespeneCost);
            if ( groundWeapon != 130 )
            {
                editGroundDamage.SetEditNum<u16>(chkd.scData.weapons.get((Sc::Weapon::Type)groundWeapon).damageAmount);
                editGroundBonus.SetEditNum<u16>(chkd.scData.weapons.get((Sc::Weapon::Type)groundWeapon).damageBonus);
            }
            if ( airWeapon != 130 && airWeapon != groundWeapon )
            {
                editAirDamage.SetEditNum<u16>(chkd.scData.weapons.get((Sc::Weapon::Type)airWeapon).damageAmount);
                editAirBonus.SetEditNum<u16>(chkd.scData.weapons.get((Sc::Weapon::Type)airWeapon).damageBonus);
            }
        }
        else // Not default settings
        {
            u32 unitHitpoints = CM->getUnitHitpoints(selectedUnitType);
            editHitPoints.SetEditNum<u32>(unitHitpoints/256);
            editHitPointsByte.SetEditNum<u8>(unitHitpoints%256);
            editShieldPoints.SetEditNum<u16>(CM->getUnitShieldPoints(selectedUnitType));
            editArmor.SetEditNum<u8>(CM->getUnitArmorLevel(selectedUnitType));
            editBuildTime.SetEditNum<u16>(CM->getUnitBuildTime(selectedUnitType));
            editMineralCost.SetEditNum<u16>(CM->getUnitMineralCost(selectedUnitType));
            editGasCost.SetEditNum<u16>(CM->getUnitGasCost(selectedUnitType));
            if ( groundWeapon != 130 )
            {
                editGroundDamage.SetEditNum<u16>(CM->getWeaponBaseDamage((Sc::Weapon::Type)groundWeapon));
                editGroundBonus.SetEditNum<u16>(CM->getWeaponUpgradeDamage((Sc::Weapon::Type)groundWeapon));
            }
            if ( airWeapon != 130 && airWeapon != groundWeapon )
            {
                editAirDamage.SetEditNum<u16>(CM->getWeaponBaseDamage((Sc::Weapon::Type)airWeapon));
                editAirBonus.SetEditNum<u16>(CM->getWeaponUpgradeDamage((Sc::Weapon::Type)airWeapon));
            }
        }

        checkEnabledByDefault.SetCheck(CM->isUnitDefaultBuildable(selectedUnitType));

        for ( size_t i=0; i<12; i++ )
        {
            if ( CM->playerUsesDefaultUnitBuildability(selectedUnitType, i) )
                dropPlayerAvailability[i].SetSel(0);
            else if ( CM->isUnitBuildable(selectedUnitType, i) )
                dropPlayerAvailability[i].SetSel(1);
            else // playerUsesDefault == false && isUnitBuildable == false
                dropPlayerAvailability[i].SetSel(2);
        }

        if ( CM->getUnitNameStringId(selectedUnitType) == Chk::StringId::NoString )
        {
            editUnitName.DisableThis();
            checkUseDefaultName.SetCheck(true);
        }
        else
        {
            if ( !unitUsesDefaultSettings )
                editUnitName.EnableThis();

            checkUseDefaultName.SetCheck(false);
        }
            
        auto unitName = CM->getUnitName<ChkdString>(selectedUnitType, true);
        editUnitName.SetText(*unitName);
        chkd.mapSettingsWindow.SetWinText("Map Settings - [" + Sc::Unit::defaultDisplayNames[(u16)selectedUnitType] + ']');
    }
    else
        DisableUnitEditing();

    refreshing = false;
}

void UnitSettingsWindow::CreateSubWindows(HWND hParent)
{
    unitTree.UpdateUnitNames(Sc::Unit::defaultDisplayNames);
    unitTree.CreateThis(hParent, 5, 5, 200, 489, false, Id::TREE_UNITSETTINGS);
    unitTree.UpdateUnitTree();
    buttonResetUnitDefaults.CreateThis(hParent, 5, 494, 200, 25, "Reset All Units To Default", Id::BUTTON_RESETALLUNITDEFAULTS);
    checkUseUnitDefaults.CreateThis(hParent, 210, 5, 100, 20, false, "Use Unit Defaults", Id::CHECK_USEUNITDEFAULTS);
    checkEnabledByDefault.CreateThis(hParent, 403, 5, 120, 20, false, "Enabled by Default", Id::CHECK_ENABLEDBYDEFAULT);

    groupUnitProperties.CreateThis(hParent, 210, 25, 377, 292, "Unit Properties", Id::GROUP_UNITPROPERTIES);
    textHitPoints.CreateThis(hParent, 215, 40, 75, 20, "Hit Points", Id::TEXT_UNITHITPOINTS);
    editHitPoints.CreateThis(hParent, 303, 40, 63, 20, false, Id::EDIT_UNITHITPOINTS);
    editHitPointsByte.CreateThis(hParent, 371, 40, 15, 20, false, Id::EDIT_UNITHITPOINTSBYTE);
    textShieldPoints.CreateThis(hParent, 215, 65, 83, 20, "Shield Points", Id::TEXT_UNITSHIELDPOINTS);
    editShieldPoints.CreateThis(hParent, 303, 65, 83, 20, false, Id::EDIT_UNITSHIELDPOINTS);
    textArmor.CreateThis(hParent, 215, 90, 75, 20, "Armor", Id::TEXT_UNITARMOR);
    editArmor.CreateThis(hParent, 303, 90, 83, 20, false, Id::EDIT_UNITARMOR);
    textBuildTime.CreateThis(hParent, 411, 40, 75, 20, "Build Time", Id::TEXT_UNITBUILDTIME);
    editBuildTime.CreateThis(hParent, 499, 40, 83, 20, false, Id::EDIT_UNITBUILDTIME);
    textMineralCost.CreateThis(hParent, 411, 65, 75, 20, "Mineral Cost", Id::TEXT_UNITMINERALCOST);
    editMineralCost.CreateThis(hParent, 499, 65, 83, 20, false, Id::EDIT_UNITMINERALCOST);
    textGasCost.CreateThis(hParent, 411, 90, 75, 20, "Gas Cost", Id::TEXT_UNITGASCOST);
    editGasCost.CreateThis(hParent, 499, 90, 83, 20, false, Id::EDIT_UNITGASCOST);

    groupGroundWeapon.CreateThis(hParent, 215, 115, 367, 62, "Ground Weapon [NAME]", Id::GROUP_GROUNDWEAPON);
    textGroundDamage.CreateThis(hParent, 220, 135, 75, 20, "Damage", Id::TEXT_UNITGROUNDDAMAGE);
    editGroundDamage.CreateThis(hParent, 308, 135, 83, 20, false, Id::EDIT_UNITGROUNDDAMAGE);
    textGroundBonus.CreateThis(hParent, 401, 135, 75, 20, "Bonus", Id::TEXT_UNITGROUNDBONUS);
    editGroundBonus.CreateThis(hParent, 489, 135, 83, 20, false, Id::EDIT_UNITGROUNDBONUS);

    groupAirWeapon.CreateThis(hParent, 215, 182, 367, 62, "Air Weapon [NAME]", Id::GROUP_AIRWEAPON);
    textAirDamage.CreateThis(hParent, 220, 202, 75, 20, "Damage", Id::TEXT_UNITAIRDAMAGE);
    editAirDamage.CreateThis(hParent, 308, 202, 83, 20, false, Id::EDIT_UNITAIRDAMAGE);
    textAirBonus.CreateThis(hParent, 401, 202, 75, 20, "Bonus", Id::TEXT_UNITAIRBONUS);
    editAirBonus.CreateThis(hParent, 489, 202, 83, 20, false, Id::EDIT_UNITAIRBONUS);

    groupUnitName.CreateThis(hParent, 215, 249, 367, 62, "Unit Name", Id::GROUP_UNITNAME);
    checkUseDefaultName.CreateThis(hParent, 220, 269, 75, 20, false, "Use Default", Id::CHECK_USEDEFAULTUNITNAME);
    editUnitName.CreateThis(hParent, 342, 269, 234, 20, false, Id::EDIT_UNITNAME);

    for ( int y=0; y<6; y++ )
    {
        for ( int x=0; x<2; x++ )
        {
            int player = y*2+x;
            std::stringstream ssPlayer;
            if ( player < 9 )
                ssPlayer << "Player 0" << player+1;
            else
                ssPlayer << "Player " << player+1;

            textPlayerAvailability[player].CreateThis(hParent, 215+188*x, 336+27*y, 75, 20, ssPlayer.str(), Id::TEXT_P1UNITAVAILABILITY+player);
        }
    }

    const std::vector<std::string> items = { "Default", "Yes", "No" };
    size_t numItems = items.size();

    for ( int y=0; y<6; y++ )
    {
        for ( int x=0; x<2; x++ )
        {
            int player = y*2+x;
            dropPlayerAvailability[player].CreateThis(hParent, 304+188*x, 336+27*y, 84, 100, false, false, Id::DROP_P1UNITAVAILABILITY+player, items);
        }
    }

    groupUnitAvailability.CreateThis(hParent, 210, 321, 372, 198, "Unit Availability", Id::GROUP_UNITAVAILABILITY);

    DisableUnitEditing();
}

void UnitSettingsWindow::DisableUnitEditing()
{
    isDisabled = true;
    chkd.mapSettingsWindow.SetWinText("Map Settings");
    DisableUnitProperties();
    checkUseUnitDefaults.DisableThis();
    checkEnabledByDefault.DisableThis();

    groupUnitAvailability.DisableThis();
    for ( int i=0; i<12; i++ )
    {
        textPlayerAvailability[i].DisableThis();
        dropPlayerAvailability[i].DisableThis();
    }
}

void UnitSettingsWindow::EnableUnitEditing()
{
    isDisabled = false;
    if ( selectedUnitType != Sc::Unit::Type::NoUnit && CM->unitUsesDefaultSettings(selectedUnitType) == false )
        EnableUnitProperties();

    checkUseUnitDefaults.EnableThis();
    checkEnabledByDefault.EnableThis();

    groupUnitAvailability.EnableThis();
    for ( int i=0; i<12; i++ )
    {
        textPlayerAvailability[i].EnableThis();
        dropPlayerAvailability[i].EnableThis();
    }
}

void UnitSettingsWindow::DisableUnitProperties()
{
    groupUnitProperties.DisableThis();
    textHitPoints.DisableThis();
    editHitPoints.DisableThis();
    editHitPointsByte.DisableThis();
    textShieldPoints.DisableThis();
    editShieldPoints.DisableThis();
    textArmor.DisableThis();
    editArmor.DisableThis();
    textBuildTime.DisableThis();
    editBuildTime.DisableThis();
    textMineralCost.DisableThis();
    editMineralCost.DisableThis();
    textGasCost.DisableThis();
    editGasCost.DisableThis();

    groupGroundWeapon.DisableThis();
    textGroundDamage.DisableThis();
    editGroundDamage.DisableThis();
    textGroundBonus.DisableThis();
    editGroundBonus.DisableThis();

    groupAirWeapon.DisableThis();
    textAirDamage.DisableThis();
    editAirDamage.DisableThis();
    textAirBonus.DisableThis();
    editAirBonus.DisableThis();

    groupUnitName.DisableThis();
    checkUseDefaultName.DisableThis();
    editUnitName.DisableThis();
}

void UnitSettingsWindow::EnableUnitProperties()
{
    groupUnitProperties.EnableThis();
    textHitPoints.EnableThis();
    editHitPoints.EnableThis();
    editHitPointsByte.EnableThis();
    textShieldPoints.EnableThis();
    editShieldPoints.EnableThis();
    textArmor.EnableThis();
    editArmor.EnableThis();
    textBuildTime.EnableThis();
    editBuildTime.EnableThis();
    textMineralCost.EnableThis();
    editMineralCost.EnableThis();
    textGasCost.EnableThis();
    editGasCost.EnableThis();

    groupGroundWeapon.EnableThis();
    textGroundDamage.EnableThis();
    editGroundDamage.EnableThis();
    textGroundBonus.EnableThis();
    editGroundBonus.EnableThis();

    groupAirWeapon.EnableThis();
    textAirDamage.EnableThis();
    editAirDamage.EnableThis();
    textAirBonus.EnableThis();
    editAirBonus.EnableThis();

    groupUnitName.EnableThis();
    checkUseDefaultName.EnableThis();

    if ( SendMessage((HWND)checkUseDefaultName.getHandle(), BM_GETCHECK, 0, 0) == BST_UNCHECKED )
        editUnitName.EnableThis();
}

void UnitSettingsWindow::CheckReplaceUnitName()
{
    if ( possibleUnitNameUpdate && checkUseDefaultName.isChecked() )
        possibleUnitNameUpdate = false;

    RawString rawUnitName;
    if ( possibleUnitNameUpdate && selectedUnitType != Sc::Unit::Type::NoUnit && (u16)selectedUnitType < Sc::Unit::TotalTypes )
    {
        if ( auto newUnitName = editUnitName.GetWinText() )
        {
            auto edit = CM->operator()(ActionDescriptor::UpdateUnitName);
            CM->setUnitName<ChkdString>((Sc::Unit::Type)selectedUnitType, *newUnitName);
            CM->deleteUnusedStrings(Chk::Scope::Both);
            chkd.unitWindow.RepopulateList();
            RedrawWindow(chkd.unitWindow.getHandle(), NULL, NULL, RDW_INVALIDATE);

            possibleUnitNameUpdate = false;
        }
    }
}

void UnitSettingsWindow::SetDefaultUnitProperties()
{
    refreshing = true;
    if ( selectedUnitType != Sc::Unit::Type::NoUnit )
    {
        // Remove Custom Unit Name
        auto edit = CM->operator()(ActionDescriptor::UpdateUnitName);
        u16 origName = (u16)CM->getUnitNameStringId(selectedUnitType, Chk::UseExpSection::No);
        u16 expName = (u16)CM->getUnitNameStringId(selectedUnitType, Chk::UseExpSection::Yes);
        CM->setUnitNameStringId(selectedUnitType, 0, Chk::UseExpSection::No);
        CM->setUnitNameStringId(selectedUnitType, 0, Chk::UseExpSection::Yes);
        CM->deleteUnusedStrings(Chk::Scope::Both);
        
        auto unitName = CM->getUnitName<ChkdString>(selectedUnitType, true);
        editUnitName.SetText(*unitName);
        checkUseDefaultName.DisableThis();
        editUnitName.DisableThis();
        CM->deleteString(origName);
        CM->deleteString(expName);
        chkd.unitWindow.RepopulateList();
        RedrawWindow(chkd.unitWindow.getHandle(), NULL, NULL, RDW_INVALIDATE);

        u32 groundWeapon = (u32)chkd.scData.units.getUnit(selectedUnitType).groundWeapon,
            airWeapon    = (u32)chkd.scData.units.getUnit(selectedUnitType).airWeapon;

        Sc::Unit::Type subUnitId = chkd.scData.units.getUnit(selectedUnitType).subunit1;
        if ( subUnitId != 228 ) // If unit has a subunit
        {
            if ( groundWeapon == 130 ) // If unit might have a subunit ground attack
                groundWeapon = chkd.scData.units.getUnit(subUnitId).groundWeapon;
            if ( airWeapon == 130 ) // If unit might have a subunit air attack
                airWeapon = chkd.scData.units.getUnit(subUnitId).airWeapon;
        }
        
        CM->setUnitHitpoints(selectedUnitType, chkd.scData.units.getUnit(selectedUnitType).hitPoints);
        CM->setUnitShieldPoints(selectedUnitType, chkd.scData.units.getUnit(selectedUnitType).shieldAmount);
        CM->setUnitArmorLevel(selectedUnitType, chkd.scData.units.getUnit(selectedUnitType).armor);
        CM->setUnitBuildTime(selectedUnitType, chkd.scData.units.getUnit(selectedUnitType).buildTime);
        CM->setUnitMineralCost(selectedUnitType, chkd.scData.units.getUnit(selectedUnitType).mineralCost);
        CM->setUnitGasCost(selectedUnitType, chkd.scData.units.getUnit(selectedUnitType).vespeneCost);

        if ( groundWeapon != 130 )
        {
            u16 defaultBaseDamage = chkd.scData.weapons.get((Sc::Weapon::Type)groundWeapon).damageAmount,
                defaultBonusDamage = chkd.scData.weapons.get((Sc::Weapon::Type)groundWeapon).damageBonus;
            
            CM->setWeaponBaseDamage((Sc::Weapon::Type)groundWeapon, defaultBaseDamage);
            CM->setWeaponUpgradeDamage((Sc::Weapon::Type)groundWeapon, defaultBonusDamage);
        }

        if ( airWeapon != 130 )
        {
            u16 defaultBaseDamage = chkd.scData.weapons.get((Sc::Weapon::Type)airWeapon).damageAmount,
                defaultBonusDamage = chkd.scData.weapons.get((Sc::Weapon::Type)airWeapon).damageBonus;
            
            CM->setWeaponBaseDamage((Sc::Weapon::Type)airWeapon, defaultBaseDamage);
            CM->setWeaponUpgradeDamage((Sc::Weapon::Type)airWeapon, defaultBonusDamage);
        }
    }
    refreshing = false;
}

void UnitSettingsWindow::ClearDefaultUnitProperties()
{
    if ( selectedUnitType != Sc::Unit::Type::NoUnit )
    {
        u32 groundWeapon = (u32)chkd.scData.units.getUnit(selectedUnitType).groundWeapon,
            airWeapon    = (u32)chkd.scData.units.getUnit(selectedUnitType).airWeapon;

        u16 origName = (u16)CM->getUnitNameStringId(selectedUnitType, Chk::UseExpSection::No),
            expName = (u16)CM->getUnitNameStringId(selectedUnitType, Chk::UseExpSection::Yes);
        CM->setUnitNameStringId(selectedUnitType, 0, Chk::UseExpSection::Both);
        CM->deleteString(origName);
        CM->deleteString(expName);
        
        CM->setUnitHitpoints(selectedUnitType, 0, Chk::UseExpSection::Both);
        CM->setUnitShieldPoints(selectedUnitType, 0, Chk::UseExpSection::Both);
        CM->setUnitArmorLevel(selectedUnitType, 0, Chk::UseExpSection::Both);
        CM->setUnitBuildTime(selectedUnitType, 0, Chk::UseExpSection::Both);
        CM->setUnitMineralCost(selectedUnitType, 0, Chk::UseExpSection::Both);
        CM->setUnitGasCost(selectedUnitType, 0, Chk::UseExpSection::Both);
    }
}

LRESULT UnitSettingsWindow::Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr)
{
    if ( nmhdr->code == TVN_SELCHANGED && ((NMTREEVIEW*)nmhdr)->action != TVC_UNKNOWN )
    {
        LPARAM itemType = (((NMTREEVIEW*)nmhdr)->itemNew.lParam)&TreeTypePortion,
            itemData = (((NMTREEVIEW*)nmhdr)->itemNew.lParam)&TreeDataPortion;

        Sc::Unit::Type unitType = (Sc::Unit::Type)itemData;
        if ( itemType == TreeTypeUnit && unitType < Sc::Unit::TotalTypes )
        {
            CheckReplaceUnitName();
            selectedUnitType = unitType;
            RefreshWindow();
        }
        else
        {
            CheckReplaceUnitName();
            selectedUnitType = Sc::Unit::Type::NoUnit;
            DisableUnitEditing();
        }
    }
    return ClassWindow::Notify(hWnd, idFrom, nmhdr);
}

LRESULT UnitSettingsWindow::Command(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if ( refreshing )
        return ClassWindow::Command(hWnd, wParam, lParam);

    switch ( LOWORD(wParam) )
    {
    case Id::BUTTON_RESETALLUNITDEFAULTS:
        if ( HIWORD(wParam) == BN_CLICKED )
        {
            if ( WinLib::GetYesNo("Are you sure you want to reset all unit settings?", "Confirm") == WinLib::PromptResult::Yes )
            {
                CM->setUnitsToDefault();
                CM->deleteUnusedStrings();
                DisableUnitEditing();
                RefreshWindow();
            }
        }
        break;

    case Id::CHECK_USEUNITDEFAULTS:
        if ( HIWORD(wParam) == BN_CLICKED )
        {
            LRESULT state = SendMessage((HWND)lParam, BM_GETCHECK, 0, 0);
            if ( selectedUnitType != Sc::Unit::Type::NoUnit )
            {
                if ( state == BST_CHECKED )
                {
                    CM->setUnitUsesDefaultSettings(selectedUnitType, true);
                    ClearDefaultUnitProperties();
                    DisableUnitProperties();
                }
                else
                {
                    CM->setUnitUsesDefaultSettings(selectedUnitType, false);
                    SetDefaultUnitProperties();
                    EnableUnitProperties();
                }

                RefreshWindow();
                chkd.unitWindow.RepopulateList();
                RedrawWindow(chkd.unitWindow.getHandle(), NULL, NULL, RDW_INVALIDATE);
            }
        }
        break;
    case Id::CHECK_USEDEFAULTUNITNAME:
        if ( HIWORD(wParam) == BN_CLICKED )
        {
            LRESULT state = SendMessage((HWND)lParam, BM_GETCHECK, 0, 0);
            if ( selectedUnitType != Sc::Unit::Type::NoUnit )
            {
                if ( state == BST_CHECKED )
                {
                    editUnitName.DisableThis();
                    auto edit = CM->operator()(ActionDescriptor::UpdateUnitName);
                    CM->setUnitNameStringId(selectedUnitType, 0, Chk::UseExpSection::Both);
                    CM->deleteUnusedStrings(Chk::Scope::Both);
                    auto unitName = CM->getUnitName<ChkdString>(selectedUnitType, true);
                    editUnitName.SetText(*unitName);
                    chkd.unitWindow.RepopulateList();
                    RedrawWindow(chkd.unitWindow.getHandle(), NULL, NULL, RDW_INVALIDATE);
                }
                else
                    editUnitName.EnableThis();
            }
        }
        break;
    case Id::CHECK_ENABLEDBYDEFAULT:
        if ( HIWORD(wParam) == BN_CLICKED )
        {
            LRESULT state = SendMessage((HWND)lParam, BM_GETCHECK, 0, 0);
            if ( selectedUnitType != Sc::Unit::Type::NoUnit )
                CM->setUnitDefaultBuildable(selectedUnitType, state == BST_CHECKED);
        }
        break;
    case Id::EDIT_UNITNAME:
        if ( HIWORD(wParam) == EN_CHANGE )
            possibleUnitNameUpdate = true;
        else if ( HIWORD(wParam) == EN_KILLFOCUS )
            CheckReplaceUnitName();
        break;
    case Id::EDIT_UNITHITPOINTS:
        if ( HIWORD(wParam) == EN_CHANGE )
        {
            u32 newHitPoints;
            if ( editHitPoints.GetEditNum<u32>(newHitPoints) )
            {
                u32 oldHitpoints = CM->getUnitHitpoints(selectedUnitType);
                CM->setUnitHitpoints(selectedUnitType, newHitPoints*256+oldHitpoints%256);
            }
        }
        break;
    case Id::EDIT_UNITHITPOINTSBYTE:
        if ( HIWORD(wParam) == EN_CHANGE )
        {
            u8 newHitPointByte;
            if ( editHitPointsByte.GetEditNum<u8>(newHitPointByte) )
            {
                u32 oldHitpoints = CM->getUnitHitpoints(selectedUnitType);
                CM->setUnitHitpoints(selectedUnitType, oldHitpoints-oldHitpoints%256+newHitPointByte);
            }
        }
        break;
    case Id::EDIT_UNITSHIELDPOINTS:
        if ( HIWORD(wParam) == EN_CHANGE )
        {
            u16 newShieldPoints;
            if ( editShieldPoints.GetEditNum<u16>(newShieldPoints) )
                CM->setUnitShieldPoints(selectedUnitType, newShieldPoints);
        }
        break;
    case Id::EDIT_UNITARMOR:
        if ( HIWORD(wParam) == EN_CHANGE )
        {
            u8 newArmorByte;
            if ( editArmor.GetEditNum<u8>(newArmorByte) )
                CM->setUnitArmorLevel(selectedUnitType, newArmorByte);
        }
        break;
    case Id::EDIT_UNITBUILDTIME:
        if ( HIWORD(wParam) == EN_CHANGE )
        {
            u16 newBuildTime;
            if ( editBuildTime.GetEditNum<u16>(newBuildTime) )
                CM->setUnitBuildTime(selectedUnitType, newBuildTime);
        }
        break;
    case Id::EDIT_UNITMINERALCOST:
        if ( HIWORD(wParam) == EN_CHANGE )
        {
            u16 newMineralCost;
            if ( editMineralCost.GetEditNum<u16>(newMineralCost) )
                CM->setUnitMineralCost(selectedUnitType, newMineralCost);
        }
        break;
    case Id::EDIT_UNITGASCOST:
        if ( HIWORD(wParam) == EN_CHANGE )
        {
            u16 newGasCost;
            if ( editGasCost.GetEditNum<u16>(newGasCost) )
                CM->setUnitGasCost(selectedUnitType, newGasCost);
        }
        break;
    case Id::EDIT_UNITGROUNDDAMAGE:
        if ( HIWORD(wParam) == EN_CHANGE )
        {
            u16 newGroundDamage;
            if ( editGroundDamage.GetEditNum<u16>(newGroundDamage) )
            {
                u32 groundWeapon = (u32)chkd.scData.units.getUnit(selectedUnitType).groundWeapon;
                Sc::Unit::Type subUnitId = chkd.scData.units.getUnit(selectedUnitType).subunit1;

                if ( subUnitId != 228 && groundWeapon == 130 ) // If unit has a subunit
                    groundWeapon = chkd.scData.units.getUnit(subUnitId).groundWeapon; // If unit might have a subunit ground attack
                if ( groundWeapon < 130 )
                    CM->setWeaponBaseDamage((Sc::Weapon::Type)groundWeapon, newGroundDamage);
            }
        }
        break;
    case Id::EDIT_UNITGROUNDBONUS:
        if ( HIWORD(wParam) == EN_CHANGE )
        {
            u16 newGroundBonus;
            if ( editGroundBonus.GetEditNum<u16>(newGroundBonus) )
            {
                u32 groundWeapon = (u32)chkd.scData.units.getUnit(selectedUnitType).groundWeapon;
                Sc::Unit::Type subUnitId = chkd.scData.units.getUnit(selectedUnitType).subunit1;

                if ( subUnitId != (u16)Sc::Unit::Type::NoSubUnit && groundWeapon == 130 ) // If unit has a subunit
                    groundWeapon = chkd.scData.units.getUnit(subUnitId).groundWeapon; // If unit might have a subunit ground attack
                if ( groundWeapon < 130 )
                    CM->setWeaponUpgradeDamage((Sc::Weapon::Type)groundWeapon, newGroundBonus);
            }
        }
        break;
    case Id::EDIT_UNITAIRDAMAGE:
        if ( HIWORD(wParam) == EN_CHANGE )
        {
            u16 newAirDamage;
            if ( editAirDamage.GetEditNum<u16>(newAirDamage) )
            {
                u32 airWeapon = (u32)chkd.scData.units.getUnit(selectedUnitType).airWeapon;
                Sc::Unit::Type subUnitId = chkd.scData.units.getUnit(selectedUnitType).subunit1;

                if ( subUnitId != (u16)Sc::Unit::Type::NoSubUnit && airWeapon == 130 ) // If unit has a subunit
                    airWeapon = chkd.scData.units.getUnit(subUnitId).airWeapon; // If unit might have a subunit ground attack
                if ( airWeapon < 130 )
                    CM->setWeaponBaseDamage((Sc::Weapon::Type)airWeapon, newAirDamage);
            }
        }
        break;
    case Id::EDIT_UNITAIRBONUS:
        if ( HIWORD(wParam) == EN_CHANGE )
        {
            u16 newAirBonus;
            if ( editAirBonus.GetEditNum<u16>(newAirBonus) )
            {
                u32 airWeapon = (u32)chkd.scData.units.getUnit(selectedUnitType).airWeapon;
                Sc::Unit::Type subUnitId = chkd.scData.units.getUnit(selectedUnitType).subunit1;

                if ( subUnitId != 228 && airWeapon == 130 ) // If unit has a subunit
                    airWeapon = chkd.scData.units.getUnit(subUnitId).airWeapon; // If unit might have a subunit ground attack

                CM->setWeaponUpgradeDamage((Sc::Weapon::Type)airWeapon, newAirBonus);
            }
        }
        break;
    default:
        if ( LOWORD(wParam) >= Id::DROP_P1UNITAVAILABILITY && LOWORD(wParam) <= Id::DROP_P12UNITAVAILABILITY &&
            HIWORD(wParam) == CBN_SELCHANGE )
        {
            u32 player = LOWORD(wParam) - Id::DROP_P1UNITAVAILABILITY;
            int sel = dropPlayerAvailability[player].GetSel();

            if ( sel == 0 ) // Default
                CM->setPlayerUsesDefaultUnitBuildability(selectedUnitType, player, true);
            else if ( sel == 1 ) // Yes
            {
                CM->setPlayerUsesDefaultUnitBuildability(selectedUnitType, player, false);
                CM->setUnitBuildable(selectedUnitType, player, true);
            }
            else if ( sel == 2 ) // No
            {
                CM->setPlayerUsesDefaultUnitBuildability(selectedUnitType, player, false);
                CM->setUnitBuildable(selectedUnitType, player, false);
            }

            RefreshWindow();
        }
        break;
    }
    return ClassWindow::Command(hWnd, wParam, lParam);
}

LRESULT UnitSettingsWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        case WM_SHOWWINDOW:
            if ( wParam == TRUE )
            {
                RefreshWindow();
                if ( selectedUnitType != Sc::Unit::Type::NoUnit )
                    chkd.mapSettingsWindow.SetWinText(std::string("Map Settings - [") + Sc::Unit::defaultDisplayNames[(u16)selectedUnitType] + ']');
                else
                    chkd.mapSettingsWindow.SetWinText("Map Settings");
            }
            else
                CheckReplaceUnitName();
            break;

        default:
            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;
    }
    return 0;
}
