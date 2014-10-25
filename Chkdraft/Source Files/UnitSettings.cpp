#include "UnitSettings.h"
#include "Chkdraft.h"

UnitSettingsWindow::UnitSettingsWindow() : selectedUnit(-1), possibleUnitNameUpdate(false), isDisabled(true), refreshing(false)
{

}

bool UnitSettingsWindow::CreateThis(HWND hParent)
{
	if ( getHandle() != NULL )
		return SetParent(getHandle(), hParent) != NULL;

	if ( ClassWindow::RegisterWindowClass(NULL, NULL, NULL, NULL, NULL, "UnitSettings", NULL, false) &&
		 ClassWindow::CreateClassWindow(NULL, "UnitSettings", WS_VISIBLE|WS_CHILD, 4, 22, 592, 524, hParent, (HMENU)ID_UNITSETTINGS) )
	{
		CreateSubWindows(getHandle());
		return true;
	}
	else
		return false;
}

bool UnitSettingsWindow::DestroyThis()
{
	return true;
}

void UnitSettingsWindow::RefreshWindow()
{
	refreshing = true;
	if ( selectedUnit >= 0 && selectedUnit < 228 && chkd.maps.curr != nullptr )
	{
		u16 unitId = (u16)selectedUnit;
		if ( isDisabled )
			EnableUnitEditing();

		Scenario* chk = chkd.maps.curr;

		bool useDefaultSettings = chk->unitUsesDefaultSettings(unitId);
		if ( useDefaultSettings )
		{
			checkUseUnitDefaults.SetCheck(true);
			DisableUnitProperties();
		}
		else
		{
			checkUseUnitDefaults.SetCheck(false);
			EnableUnitProperties();
		}

		UNITDAT* unitDat = chkd.scData.UnitDat(unitId);
		u32 hitpoints,
			groundWeapon = (u32)unitDat->GroundWeapon,
			airWeapon	 = (u32)unitDat->AirWeapon;
		u16 shieldPoints, buildTime, mineralCost, gasCost, baseGroundWeapon, bonusGroundWeapon,
			baseAirWeapon, bonusAirWeapon,
			subUnitId = unitDat->Subunit1;
		u8 armor, hitpointsByte;
		
		if ( subUnitId != 228 ) // If unit has a subunit
		{
			if ( groundWeapon == 130 ) // If unit might have a subunit ground attack
				groundWeapon = chkd.scData.UnitDat(subUnitId)->GroundWeapon;
			if ( airWeapon == 130 ) // If unit might have a subunit air attack
				airWeapon = chkd.scData.UnitDat(subUnitId)->AirWeapon;
		}

		if ( groundWeapon == 130 )
		{
			groupGroundWeapon.SetText("No Ground Weapon");
			groupGroundWeapon.DisableThis();
			editGroundDamage.SetText("");
			editGroundDamage.DisableThis();
			editGroundBonus.SetText("");
			editGroundBonus.DisableThis();
			textGroundDamage.DisableThis();
			textGroundBonus.DisableThis();
		}
		else
		{
			stringstream newGroundGroupText;
			newGroundGroupText << "Ground Weapon [" << weaponNames[groundWeapon] << "]";
			groupGroundWeapon.SetText(newGroundGroupText.str().c_str());
		}

		if ( airWeapon == 130 || airWeapon == groundWeapon )
		{
			if ( airWeapon != 130 && airWeapon == groundWeapon )
			{
				stringstream newAirGroupText;
				newAirGroupText << "Air Weapon [" << weaponNames[airWeapon] << "]";
				groupAirWeapon.SetText(newAirGroupText.str().c_str());
			}
			else
				groupAirWeapon.SetText("No Air Weapon");

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
			stringstream newAirGroupText;
			newAirGroupText << "Air Weapon [" << weaponNames[airWeapon] << "]";
			groupAirWeapon.SetText(newAirGroupText.str().c_str());
		}

		if ( useDefaultSettings )
		{
			editHitPoints.SetEditNum<u32>(unitDat->HitPoints/256);
			editHitPointsByte.SetEditNum<u32>(0);
			editShieldPoints.SetEditNum<u16>(unitDat->ShieldAmount);
			editArmor.SetEditNum<u8>(unitDat->Armor);
			editBuildTime.SetEditNum<u16>(unitDat->BuildTime);
			editMineralCost.SetEditNum<u16>(unitDat->MineralCost);
			editGasCost.SetEditNum<u16>(unitDat->VespeneCost);
			if ( groundWeapon != 130 )
			{
				editGroundDamage.SetEditNum<u16>(chkd.scData.WeaponDat(groundWeapon)->DamageAmount);
				editGroundBonus.SetEditNum<u16>(chkd.scData.WeaponDat(groundWeapon)->DamageBonus);
			}
			if ( airWeapon != 130 && airWeapon != groundWeapon )
			{
				editAirDamage.SetEditNum<u16>(chkd.scData.WeaponDat(airWeapon)->DamageAmount);
				editAirBonus.SetEditNum<u16>(chkd.scData.WeaponDat(airWeapon)->DamageBonus);
			}
		}
		else // Not default settings
		{
			if ( chk->getUnitSettingsHitpoints(unitId, hitpoints) )
				editHitPoints.SetEditNum<u32>(hitpoints);
			if ( chk->getUnitSettingsHitpointByte(unitId, hitpointsByte) )
				editHitPointsByte.SetEditNum<u8>(hitpointsByte);
			if ( chk->getUnitSettingsShieldPoints(unitId, shieldPoints) )
				editShieldPoints.SetEditNum<u16>(shieldPoints);
			if ( chk->getUnitSettingsArmor(unitId, armor) )
				editArmor.SetEditNum<u8>(armor);
			if ( chk->getUnitSettingsBuildTime(unitId, buildTime) )
				editBuildTime.SetEditNum<u16>(buildTime);
			if ( chk->getUnitSettingsMineralCost(unitId, mineralCost) )
				editMineralCost.SetEditNum<u16>(mineralCost);
			if ( chk->getUnitSettingsGasCost(unitId, gasCost) )
				editGasCost.SetEditNum<u16>(gasCost);
			if ( chk->getUnitSettingsBaseWeapon(groundWeapon, baseGroundWeapon) )
				editGroundDamage.SetEditNum<u16>(baseGroundWeapon);
			if ( chk->getUnitSettingsBonusWeapon(groundWeapon, bonusGroundWeapon) )
				editGroundBonus.SetEditNum<u16>(bonusGroundWeapon);
			if ( airWeapon != groundWeapon && chk->getUnitSettingsBaseWeapon(airWeapon, baseAirWeapon) )
				editAirDamage.SetEditNum<u16>(baseAirWeapon);
			if ( airWeapon != groundWeapon && chk->getUnitSettingsBonusWeapon(airWeapon, bonusAirWeapon) )
				editAirBonus.SetEditNum<u16>(bonusAirWeapon);
		}

		checkEnabledByDefault.SetCheck(chk->unitIsEnabled(unitId));

		for ( int i=0; i<12; i++ )
		{
			u8 enabledState = chk->getUnitEnabledState(unitId, (u8)i);
			if ( enabledState == UNIT_STATE_DEFAULTFORPLAYER )
				dropPlayerAvailability[i].SetSel(0);
			else if ( enabledState == UNIT_STATE_ENABLEDFORPLAYER )
				dropPlayerAvailability[i].SetSel(1);
			else if ( enabledState == UNIT_STATE_DISABLEDFORPLAYER )
				dropPlayerAvailability[i].SetSel(2);
		}

		u16 unitStringNum = 0;
		if ( chk->getUnitStringNum(unitId, unitStringNum) && unitStringNum == 0 )
		{
			editUnitName.DisableThis();
			checkUseDefaultName.SetCheck(true);
		}
		else
		{
			if ( !useDefaultSettings )
				editUnitName.EnableThis();
			checkUseDefaultName.SetCheck(false);
		}
			
		std::string unitName;
		chk->getUnitName(unitName, unitId);
		editUnitName.SetText(unitName.c_str());
	}
	else
		DisableUnitEditing();

	refreshing = false;
}

void UnitSettingsWindow::CreateSubWindows(HWND hParent)
{
	unitTree.UpdateUnitNames(DefaultUnitDisplayName);
	unitTree.CreateThis(hParent, 5, 5, 200, 489, false, ID_UNITSETTINGSTREE);
	buttonResetUnitDefaults.CreateThis(hParent, 5, 494, 200, 25, "Reset All Units To Default", ID_BUTTON_RESETALLUNITDEFAULTS);
	checkUseUnitDefaults.CreateThis(hParent, 210, 5, 100, 20, false, "Use Unit Defaults", ID_CHECK_USEUNITDEFAULTS);
	checkEnabledByDefault.CreateThis(hParent, 403, 5, 120, 20, false, "Enabled by Default", ID_CHECK_ENABLEDBYDEFAULT);

	groupUnitProperties.CreateThis(hParent, 210, 25, 377, 292, "Unit Properties", ID_GROUP_UNITPROPERTIES);
	textHitPoints.CreateThis(hParent, 215, 40, 75, 20, "Hit Points", ID_TEXT_UNITHITPOINTS);
	editHitPoints.CreateThis(hParent, 303, 40, 63, 20, false, ID_EDIT_UNITHITPOINTS);
	editHitPointsByte.CreateThis(hParent, 371, 40, 15, 20, false, ID_EDIT_UNITHITPOINTSBYTE);
	textShieldPoints.CreateThis(hParent, 215, 65, 83, 20, "Shield Points", ID_TEXT_UNITSHIELDPOINTS);
	editShieldPoints.CreateThis(hParent, 303, 65, 83, 20, false, ID_EDIT_UNITSHIELDPOINTS);
	textArmor.CreateThis(hParent, 215, 90, 75, 20, "Armor", ID_TEXT_UNITARMOR);
	editArmor.CreateThis(hParent, 303, 90, 83, 20, false, ID_EDIT_UNITARMOR);
	textBuildTime.CreateThis(hParent, 411, 40, 75, 20, "Build Time", ID_TEXT_UNITBUILDTIME);
	editBuildTime.CreateThis(hParent, 499, 40, 83, 20, false, ID_EDIT_UNITBUILDTIME);
	textMineralCost.CreateThis(hParent, 411, 65, 75, 20, "Mineral Cost", ID_TEXT_UNITMINERALCOST);
	editMineralCost.CreateThis(hParent, 499, 65, 83, 20, false, ID_EDIT_UNITMINERALCOST);
	textGasCost.CreateThis(hParent, 411, 90, 75, 20, "Gas Cost", ID_TEXT_UNITGASCOST);
	editGasCost.CreateThis(hParent, 499, 90, 83, 20, false, ID_EDIT_UNITGASCOST);

	groupGroundWeapon.CreateThis(hParent, 215, 115, 367, 62, "Ground Weapon [NAME]", ID_GROUP_GROUNDWEAPON);
	textGroundDamage.CreateThis(hParent, 220, 135, 75, 20, "Damage", ID_TEXT_UNITGROUNDDAMAGE);
	editGroundDamage.CreateThis(hParent, 308, 135, 83, 20, false, ID_EDIT_UNITGROUNDDAMAGE);
	textGroundBonus.CreateThis(hParent, 401, 135, 75, 20, "Bonus", ID_TEXT_UNITGROUNDBONUS);
	editGroundBonus.CreateThis(hParent, 489, 135, 83, 20, false, ID_EDIT_UNITGROUNDBONUS);

	groupAirWeapon.CreateThis(hParent, 215, 182, 367, 62, "Air Weapon [NAME]", ID_GROUP_AIRWEAPON);
	textAirDamage.CreateThis(hParent, 220, 202, 75, 20, "Damage", ID_TEXT_UNITAIRDAMAGE);
	editAirDamage.CreateThis(hParent, 308, 202, 83, 20, false, ID_EDIT_UNITAIRDAMAGE);
	textAirBonus.CreateThis(hParent, 401, 202, 75, 20, "Bonus", ID_TEXT_UNITAIRBONUS);
	editAirBonus.CreateThis(hParent, 489, 202, 83, 20, false, ID_EDIT_UNITAIRBONUS);

	groupUnitName.CreateThis(hParent, 215, 249, 367, 62, "Unit Name", ID_GROUP_UNITNAME);
	checkUseDefaultName.CreateThis(hParent, 220, 269, 75, 20, false, "Use Default", ID_CHECK_USEDEFAULTUNITNAME);
	editUnitName.CreateThis(hParent, 342, 269, 234, 20, false, ID_EDIT_UNITNAME);

	for ( int y=0; y<6; y++ )
	{
		for ( int x=0; x<2; x++ )
		{
			int player = y*2+x;
			stringstream ssPlayer;
			if ( player < 9 )
				ssPlayer << "Player 0" << player+1;
			else
				ssPlayer << "Player " << player+1;

			textPlayerAvailability[player].CreateThis(hParent, 215+188*x, 336+27*y, 75, 20, ssPlayer.str().c_str(), ID_TEXT_P1UNITAVAILABILITY+player);
		}
	}

	const char* items[] = { "Default", "Yes", "No" };
	int numItems = sizeof(items)/sizeof(const char*);

	for ( int y=0; y<6; y++ )
	{
		for ( int x=0; x<2; x++ )
		{
			int player = y*2+x;
			dropPlayerAvailability[player].CreateThis( hParent, 304+188*x, 336+27*y, 84, 100, false, ID_DROP_P1UNITAVAILABILITY+player,
													   numItems, items, defaultFont );
		}
	}

	groupUnitAvailability.CreateThis(hParent, 210, 321, 372, 198, "Unit Availability", ID_GROUP_UNITAVAILABILITY);

	DisableUnitEditing();
}

void UnitSettingsWindow::DisableUnitEditing()
{
	isDisabled = true;
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
	if ( selectedUnit >= 0 && chkd.maps.curr->unitUsesDefaultSettings((u16)selectedUnit) == false )
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

	if ( SendMessage((HWND)checkUseDefaultName.getHandle(), BM_GETCHECK, NULL, NULL) == BST_UNCHECKED )
		editUnitName.EnableThis();
}

void UnitSettingsWindow::CheckReplaceUnitName()
{
	if ( possibleUnitNameUpdate && checkUseDefaultName.isChecked() )
		possibleUnitNameUpdate = false;

	Scenario* chk = chkd.maps.curr;
	string newUnitName;
	if ( possibleUnitNameUpdate && selectedUnit >= 0 && selectedUnit < 228 && editUnitName.GetEditText(newUnitName) )
	{
		u16* originalNameString = nullptr;
		u16* expansionNameString = nullptr;
		bool gotOrig = chk->UNIx().getPtr<u16>(originalNameString, 2*selectedUnit+UNIT_SETTINGS_STRING_IDS, 2);
		bool gotExp = chk->UNIS().getPtr<u16>(expansionNameString, 2*selectedUnit+UNIT_SETTINGS_STRING_IDS, 2);
		if ( ( (chk->isExpansion() && gotExp) || (!chk->isExpansion() && gotOrig) ) &&
			 parseEscapedString(newUnitName) )
		{
			bool replacedOrig = false, replacedExp = false;
			if ( gotOrig )
				replacedOrig = chk->replaceString(newUnitName, *originalNameString, false, true);
			if ( gotExp )
				replacedExp = chk->replaceString(newUnitName, *expansionNameString, false, true);

			if ( replacedOrig || replacedExp )
			{
				chkd.maps.curr->notifyChange(false);
				chkd.unitWindow.RepopulateList();
				RedrawWindow(chkd.unitWindow.getHandle(), NULL, NULL, RDW_INVALIDATE);
			}
			else
				RefreshWindow();
		}
		possibleUnitNameUpdate = false;
	}
}

void UnitSettingsWindow::SetDefaultUnitProperties()
{
	refreshing = true;
	if ( selectedUnit >= 0 )
	{
		Scenario* chk = chkd.maps.curr;
		u16 unitId = (u16)selectedUnit;

		// Remove Custom Unit Name
		u16 origName = chk->UNIS().get<u16>(2*selectedUnit+UNIT_SETTINGS_STRING_IDS),
			expName  = chk->UNIx().get<u16>(2*selectedUnit+UNIT_SETTINGS_STRING_IDS);
		chk->UNIS().replace<u16>(2*selectedUnit+UNIT_SETTINGS_STRING_IDS, 0);
		chk->UNIx().replace<u16>(2*selectedUnit+UNIT_SETTINGS_STRING_IDS, 0);
		std::string unitName;
		chk->getUnitName(unitName, (u16)selectedUnit);
		editUnitName.SetText(unitName.c_str());
		checkUseDefaultName.DisableThis();
		editUnitName.DisableThis();
		chk->removeUnusedString(origName);
		chk->removeUnusedString(expName);
		chkd.unitWindow.RepopulateList();
		RedrawWindow(chkd.unitWindow.getHandle(), NULL, NULL, RDW_INVALIDATE);

		u32 groundWeapon = (u32)chkd.scData.UnitDat(unitId)->GroundWeapon,
			airWeapon	 = (u32)chkd.scData.UnitDat(unitId)->AirWeapon;

		u16 subUnitId = chkd.scData.UnitDat(unitId)->Subunit1;
		if ( subUnitId != 228 ) // If unit has a subunit
		{
			if ( groundWeapon == 130 ) // If unit might have a subunit ground attack
				groundWeapon = chkd.scData.UnitDat(subUnitId)->GroundWeapon;
			if ( airWeapon == 130 ) // If unit might have a subunit air attack
				airWeapon = chkd.scData.UnitDat(subUnitId)->AirWeapon;
		}
		
		chk->setUnitSettingsCompleteHitpoints(unitId, chkd.scData.UnitDat(unitId)->HitPoints);
		chk->setUnitSettingsShieldPoints(unitId, chkd.scData.UnitDat(unitId)->ShieldAmount);
		chk->setUnitSettingsArmor(unitId, chkd.scData.UnitDat(unitId)->Armor);
		chk->setUnitSettingsBuildTime(unitId, chkd.scData.UnitDat(unitId)->BuildTime);
		chk->setUnitSettingsMineralCost(unitId, chkd.scData.UnitDat(unitId)->MineralCost);
		chk->setUnitSettingsGasCost(unitId, chkd.scData.UnitDat(unitId)->VespeneCost);

		if ( groundWeapon != 130 )
		{
			u16 defaultBaseDamage = chkd.scData.WeaponDat(groundWeapon)->DamageAmount,
				defaultBonusDamage = chkd.scData.WeaponDat(groundWeapon)->DamageBonus;

			chk->setUnitSettingsBaseWeapon(groundWeapon, defaultBaseDamage);
			chk->setUnitSettingsBonusWeapon(groundWeapon, defaultBonusDamage);
		}

		if ( airWeapon != 130 )
		{
			u16 defaultBaseDamage = chkd.scData.WeaponDat(airWeapon)->DamageAmount,
				defaultBonusDamage = chkd.scData.WeaponDat(airWeapon)->DamageBonus;

			chk->setUnitSettingsBaseWeapon(airWeapon, defaultBaseDamage);
			chk->setUnitSettingsBonusWeapon(airWeapon, defaultBonusDamage);
		}
	}
	refreshing = false;
}

void UnitSettingsWindow::ClearDefaultUnitProperties()
{
	if ( selectedUnit >= 0 )
	{
		Scenario* chk = chkd.maps.curr;
		u16 unitId = (u16)selectedUnit;
		u32 groundWeapon = (u32)chkd.scData.UnitDat(unitId)->GroundWeapon,
			airWeapon	 = (u32)chkd.scData.UnitDat(unitId)->AirWeapon;

		u16 origName = chk->UNIS().get<u16>(2*selectedUnit+UNIT_SETTINGS_STRING_IDS),
			expName  = chk->UNIx().get<u16>(2*selectedUnit+UNIT_SETTINGS_STRING_IDS);
		chk->UNIS().replace<u16>(2*selectedUnit+UNIT_SETTINGS_STRING_IDS, 0);
		chk->UNIx().replace<u16>(2*selectedUnit+UNIT_SETTINGS_STRING_IDS, 0);
		chk->removeUnusedString(origName);
		chk->removeUnusedString(expName);

		chk->setUnitSettingsCompleteHitpoints(unitId, 0);
		chk->setUnitSettingsShieldPoints(unitId, 0);
		chk->setUnitSettingsArmor(unitId, 0);
		chk->setUnitSettingsBuildTime(unitId, 0);
		chk->setUnitSettingsMineralCost(unitId, 0);
		chk->setUnitSettingsGasCost(unitId, 0);
	}
}

LRESULT UnitSettingsWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_SHOWWINDOW:
			if ( wParam == TRUE )
				RefreshWindow();
			else
				CheckReplaceUnitName();
			break;

		case WM_COMMAND:
			if ( refreshing )
				return DefWindowProc(hWnd, msg, wParam, lParam);
			
			switch ( LOWORD(wParam) )
			{
				case ID_BUTTON_RESETALLUNITDEFAULTS:
					if ( HIWORD(wParam) == BN_CLICKED )
					{
						if ( MessageBox(hWnd, "Are you sure you want to reset all unit settings?", "Confirm", MB_YESNO) == IDYES )
						{
							buffer newUNIS("UNIS"), newUNIx("UNIx"), newPUNI("PUNI");
							if ( Get_UNIS(newUNIS) )
							{
								newUNIS.del(0, 8);
								chkd.maps.curr->UNIS().takeAllData(newUNIS);
							}
							if ( Get_UNIx(newUNIx) )
							{
								newUNIx.del(0, 8);
								chkd.maps.curr->UNIx().takeAllData(newUNIx);
							}
							if ( Get_PUNI(newPUNI) )
							{
								newPUNI.del(0, 8);
								chkd.maps.curr->PUNI().takeAllData(newPUNI);
							}

							chkd.maps.curr->cleanStringTable(false);
							chkd.maps.curr->cleanStringTable(true);
							DisableUnitEditing();
							RefreshWindow();
						}
					}
					break;

				case ID_CHECK_USEUNITDEFAULTS:
					if ( HIWORD(wParam) == BN_CLICKED )
					{
						LRESULT state = SendMessage((HWND)lParam, BM_GETCHECK, NULL, NULL);
						if ( selectedUnit != -1 )
						{
							if ( state == BST_CHECKED )
							{
								ClearDefaultUnitProperties();
								DisableUnitProperties();
								chkd.maps.curr->setUnitUseDefaults((u8)selectedUnit, true);
							}
							else
							{
								SetDefaultUnitProperties();
								EnableUnitProperties();
								chkd.maps.curr->setUnitUseDefaults((u8)selectedUnit, false);
							}

							RefreshWindow();
							chkd.unitWindow.RepopulateList();
							RedrawWindow(chkd.unitWindow.getHandle(), NULL, NULL, RDW_INVALIDATE);
							chkd.maps.curr->notifyChange(false);
						}
					}
					break;
				case ID_CHECK_USEDEFAULTUNITNAME:
					if ( HIWORD(wParam) == BN_CLICKED )
					{
						LRESULT state = SendMessage((HWND)lParam, BM_GETCHECK, NULL, NULL);
						if ( selectedUnit != -1 )
						{
							if ( state == BST_CHECKED )
							{
								editUnitName.DisableThis();
								chkd.maps.curr->UNIS().replace<u16>(2*selectedUnit+UNIT_SETTINGS_STRING_IDS, 0);
								chkd.maps.curr->UNIx().replace<u16>(2*selectedUnit+UNIT_SETTINGS_STRING_IDS, 0);
								std::string unitName;
								chkd.maps.curr->getUnitName(unitName, (u16)selectedUnit);
								editUnitName.SetText(unitName.c_str());
								chkd.unitWindow.RepopulateList();
								RedrawWindow(chkd.unitWindow.getHandle(), NULL, NULL, RDW_INVALIDATE);
							}
							else
								editUnitName.EnableThis();

							chkd.maps.curr->notifyChange(false);
						}
					}
					break;
				case ID_CHECK_ENABLEDBYDEFAULT:
					if ( HIWORD(wParam) == BN_CLICKED )
					{
						LRESULT state = SendMessage((HWND)lParam, BM_GETCHECK, NULL, NULL);
						if ( selectedUnit != -1 )
							chkd.maps.curr->setUnitEnabled((u16)selectedUnit, state == BST_CHECKED );
					}
					break;
				case ID_EDIT_UNITNAME:
					if ( HIWORD(wParam) == EN_CHANGE )
						possibleUnitNameUpdate = true;
					else if ( HIWORD(wParam) == EN_KILLFOCUS )
						CheckReplaceUnitName();
					break;
				case ID_EDIT_UNITHITPOINTS:
					if ( HIWORD(wParam) == EN_CHANGE )
					{
						u32 newHitPoints;
						if ( editHitPoints.GetEditNum<u32>(newHitPoints) )
							chkd.maps.curr->setUnitSettingsHitpoints((u16)selectedUnit, newHitPoints);
					}
					break;
				case ID_EDIT_UNITHITPOINTSBYTE:
					if ( HIWORD(wParam) == EN_CHANGE )
					{
						u8 newHitPointByte;
						if ( editHitPointsByte.GetEditNum<u8>(newHitPointByte) )
							chkd.maps.curr->setUnitSettingsHitpointByte((u16)selectedUnit, newHitPointByte);
					}
					break;
				case ID_EDIT_UNITSHIELDPOINTS:
					if ( HIWORD(wParam) == EN_CHANGE )
					{
						u16 newShieldPoints;
						if ( editShieldPoints.GetEditNum<u16>(newShieldPoints) )
							chkd.maps.curr->setUnitSettingsShieldPoints((u16)selectedUnit, newShieldPoints);
					}
					break;
				case ID_EDIT_UNITARMOR:
					if ( HIWORD(wParam) == EN_CHANGE )
					{
						u8 newArmorByte;
						if ( editArmor.GetEditNum<u8>(newArmorByte) )
							chkd.maps.curr->setUnitSettingsArmor((u16)selectedUnit, newArmorByte);
					}
					break;
				case ID_EDIT_UNITBUILDTIME:
					if ( HIWORD(wParam) == EN_CHANGE )
					{
						u16 newBuildTime;
						if ( editBuildTime.GetEditNum<u16>(newBuildTime) )
							chkd.maps.curr->setUnitSettingsBuildTime((u16)selectedUnit, newBuildTime);
					}
					break;
				case ID_EDIT_UNITMINERALCOST:
					if ( HIWORD(wParam) == EN_CHANGE )
					{
						u16 newMineralCost;
						if ( editMineralCost.GetEditNum<u16>(newMineralCost) )
							chkd.maps.curr->setUnitSettingsMineralCost((u16)selectedUnit, newMineralCost);
					}
					break;
				case ID_EDIT_UNITGASCOST:
					if ( HIWORD(wParam) == EN_CHANGE )
					{
						u16 newGasCost;
						if ( editGasCost.GetEditNum<u16>(newGasCost) )
							chkd.maps.curr->setUnitSettingsGasCost((u16)selectedUnit, newGasCost);
					}
					break;
				case ID_EDIT_UNITGROUNDDAMAGE:
					if ( HIWORD(wParam) == EN_CHANGE )
					{
						u16 newGroundDamage;
						if ( editGroundDamage.GetEditNum<u16>(newGroundDamage) )
						{
							u32 groundWeapon = (u32)chkd.scData.UnitDat((u16)selectedUnit)->GroundWeapon;
							u16 subUnitId = chkd.scData.UnitDat((u16)selectedUnit)->Subunit1;
		
							if ( subUnitId != 228 && groundWeapon == 130 ) // If unit has a subunit
								groundWeapon = chkd.scData.UnitDat(subUnitId)->GroundWeapon; // If unit might have a subunit ground attack
							if ( groundWeapon < 130 )
								chkd.maps.curr->setUnitSettingsBaseWeapon(groundWeapon, newGroundDamage);
						}
					}
					break;
				case ID_EDIT_UNITGROUNDBONUS:
					if ( HIWORD(wParam) == EN_CHANGE )
					{
						u16 newGroundBonus;
						if ( editGroundBonus.GetEditNum<u16>(newGroundBonus) )
						{
							u32 groundWeapon = (u32)chkd.scData.UnitDat((u16)selectedUnit)->GroundWeapon;
							u16 subUnitId = chkd.scData.UnitDat((u16)selectedUnit)->Subunit1;
		
							if ( subUnitId != 228 && groundWeapon == 130 ) // If unit has a subunit
								groundWeapon = chkd.scData.UnitDat(subUnitId)->GroundWeapon; // If unit might have a subunit ground attack
							if ( groundWeapon < 130 )
								chkd.maps.curr->setUnitSettingsBonusWeapon(groundWeapon, newGroundBonus);
						}
					}
					break;
				case ID_EDIT_UNITAIRDAMAGE:
					if ( HIWORD(wParam) == EN_CHANGE )
					{
						u16 newAirDamage;
						if ( editAirDamage.GetEditNum<u16>(newAirDamage) )
						{
							u32 airWeapon = (u32)chkd.scData.UnitDat((u16)selectedUnit)->AirWeapon;
							u16 subUnitId = chkd.scData.UnitDat((u16)selectedUnit)->Subunit1;
		
							if ( subUnitId != 228 && airWeapon == 130 ) // If unit has a subunit
								airWeapon = chkd.scData.UnitDat(subUnitId)->AirWeapon; // If unit might have a subunit ground attack
							if ( airWeapon < 130 )
								chkd.maps.curr->setUnitSettingsBaseWeapon(airWeapon, newAirDamage);
						}
					}
					break;
				case ID_EDIT_UNITAIRBONUS:
					if ( HIWORD(wParam) == EN_CHANGE )
					{
						u16 newAirBonus;
						if ( editAirBonus.GetEditNum<u16>(newAirBonus) )
						{
							u32 airWeapon = (u32)chkd.scData.UnitDat((u16)selectedUnit)->AirWeapon;
							u16 subUnitId = chkd.scData.UnitDat((u16)selectedUnit)->Subunit1;
		
							if ( subUnitId != 228 && airWeapon == 130 ) // If unit has a subunit
								airWeapon = chkd.scData.UnitDat(subUnitId)->AirWeapon; // If unit might have a subunit ground attack

							chkd.maps.curr->setUnitSettingsBonusWeapon(airWeapon, newAirBonus);
						}
					}
					break;
				case ID_DROP_P1UNITAVAILABILITY: case ID_DROP_P2UNITAVAILABILITY: case ID_DROP_P3UNITAVAILABILITY:
				case ID_DROP_P4UNITAVAILABILITY: case ID_DROP_P5UNITAVAILABILITY: case ID_DROP_P6UNITAVAILABILITY:
				case ID_DROP_P7UNITAVAILABILITY: case ID_DROP_P8UNITAVAILABILITY: case ID_DROP_P9UNITAVAILABILITY:
				case ID_DROP_P10UNITAVAILABILITY: case ID_DROP_P11UNITAVAILABILITY: case ID_DROP_P12UNITAVAILABILITY:
					if ( HIWORD(wParam) == CBN_SELCHANGE )
					{
						u32 player = LOWORD(wParam)-ID_DROP_P1UNITAVAILABILITY;
						if ( chkd.maps.curr->setUnitEnabledState((u16)selectedUnit, (u8)player, dropPlayerAvailability[player].GetSel()) )
							chkd.maps.curr->notifyChange(false);
						else
							RefreshWindow();
					}
															 
					break;
			}
			break;

		case WM_NOTIFY:
			if ( ((NMHDR*)lParam)->code == TVN_SELCHANGED && ((LPNMTREEVIEW)lParam)->action != TVC_UNKNOWN )
			{
				LPARAM itemType = (((NMTREEVIEW*)lParam)->itemNew.lParam)&TREE_ITEM_TYPE,
					   itemData = (((NMTREEVIEW*)lParam)->itemNew.lParam)&TREE_ITEM_DATA;

				u16 unitId = (u16)itemData;
				if ( itemType == TREE_TYPE_UNIT && unitId < 228 )
				{
					CheckReplaceUnitName();
					selectedUnit = unitId;
					RefreshWindow();
				}
			}

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
			break;
	}
	return 0;
}