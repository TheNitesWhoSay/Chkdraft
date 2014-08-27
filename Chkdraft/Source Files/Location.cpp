#include "Location.h"
#include "Chkdraft.h"

LocationWindow::LocationWindow() : initializing(false), preservedStat(0), locProcLocIndex(0)
{

}

bool LocationWindow::CreateThis(HWND hParent)
{
	return ClassWindow::CreateModelessDialog(MAKEINTRESOURCE(IDD_LOCPROP), hParent);
}

bool LocationWindow::DestroyThis()
{
	if ( ClassWindow::DestroyDialog() )
	{
		locProcLocIndex = NO_LOCATION;
		return true;
	}
	else
		return false;
}

void LocationWindow::RefreshLocationElevationFlags(ChkLocation* locRef, HWND hWnd)
{
	if ( (locRef->elevation&LOC_ELEVATION_LOWGROUND) == 0)
		SendMessage(GetDlgItem(hWnd, IDC_LOWGROUND), BM_SETCHECK, BST_CHECKED, NULL);
	else
		SendMessage(GetDlgItem(hWnd, IDC_LOWGROUND), BM_SETCHECK, BST_UNCHECKED, NULL);
	if ( (locRef->elevation&LOC_ELEVATION_MEDGROUND) == 0 )
		SendMessage(GetDlgItem(hWnd, IDC_MEDGROUND), BM_SETCHECK, BST_CHECKED, NULL);
	else
		SendMessage(GetDlgItem(hWnd, IDC_MEDGROUND), BM_SETCHECK, BST_UNCHECKED, NULL);
	if ( (locRef->elevation&LOC_ELEVATION_HIGHGROUND) == 0 )
		SendMessage(GetDlgItem(hWnd, IDC_HIGHGROUND), BM_SETCHECK, BST_CHECKED, NULL);
	else
		SendMessage(GetDlgItem(hWnd, IDC_HIGHGROUND), BM_SETCHECK, BST_UNCHECKED, NULL);
	if ( (locRef->elevation&LOC_ELEVATION_LOWAIR) == 0 )
		SendMessage(GetDlgItem(hWnd, IDC_LOWAIR), BM_SETCHECK, BST_CHECKED, NULL);
	else
		SendMessage(GetDlgItem(hWnd, IDC_LOWAIR), BM_SETCHECK, BST_UNCHECKED, NULL);
	if ( (locRef->elevation&LOC_ELEVATION_MEDAIR) == 0 )
		SendMessage(GetDlgItem(hWnd, IDC_MEDAIR), BM_SETCHECK, BST_CHECKED, NULL);
	else
		SendMessage(GetDlgItem(hWnd, IDC_MEDAIR), BM_SETCHECK, BST_UNCHECKED, NULL);
	if ( (locRef->elevation&LOC_ELEVATION_HIGHAIR) == 0 )
		SendMessage(GetDlgItem(hWnd, IDC_HIGHAIR), BM_SETCHECK, BST_CHECKED, NULL);
	else
		SendMessage(GetDlgItem(hWnd, IDC_HIGHAIR), BM_SETCHECK, BST_UNCHECKED, NULL);
}

LRESULT LocationWindow::RefreshLocationInfo(HWND hWnd)
{
	initializing = true;

	if ( !chkd.maps.curr )
	{
		locProcLocIndex = NO_LOCATION;
		initializing = false;
		EndDialog(hWnd, IDCLOSE);
		return TRUE;
	}

	locProcLocIndex = chkd.maps.curr->selections().getSelectedLocation();
	ChkLocation* locRef;
	if ( locProcLocIndex != NO_LOCATION && chkd.maps.curr->getLocation(locRef, locProcLocIndex) )
	{
		char text[20];
		_itoa_s(locRef->xc1, text, 10);
		SetWindowText(GetDlgItem(hWnd, IDC_LOCLEFT), text);
		_itoa_s(locRef->yc1, text, 10);
		SetWindowText(GetDlgItem(hWnd, IDC_LOCTOP), text);
		_itoa_s(locRef->xc2, text, 10);
		SetWindowText(GetDlgItem(hWnd, IDC_LOCRIGHT), text);
		_itoa_s(locRef->yc2, text, 10);
		SetWindowText(GetDlgItem(hWnd, IDC_LOCBOTTOM), text);

		_itoa_s(locRef->elevation, text, 2);
		size_t len = strlen(text);
		if ( len < 16 )
		{
			memmove(&text[16-len], text, len);
			memset(text, '0', 16-len);
			text[16] = '\0';
		}
		SetWindowText(GetDlgItem(hWnd, IDC_RAWFLAGS), text);

		RefreshLocationElevationFlags(locRef, hWnd);

		if ( chkd.maps.curr->isExtendedString(locRef->stringNum) )
			SendMessage(GetDlgItem(hWnd, IDC_EXTLOCNAMESTR), BM_SETCHECK, BST_CHECKED, NULL);
		else
			SendMessage(GetDlgItem(hWnd, IDC_EXTLOCNAMESTR), BM_SETCHECK, BST_UNCHECKED, NULL);

		std::string locName;
		if ( chkd.maps.curr->getString(locName, locRef->stringNum) )
			SetWindowText(GetDlgItem(hWnd, IDC_LOCATION_NAME), locName.c_str());
		else
			SetWindowText(GetDlgItem(hWnd, IDC_LOCATION_NAME), "ERROR");
	}
	else
	{
		locProcLocIndex = NO_LOCATION;
		initializing = false;
		EndDialog(hWnd, IDCLOSE);
	}
	initializing = false;
	return TRUE;
}

BOOL LocationWindow::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_INITDIALOG:
			return RefreshLocationInfo(hWnd);
			break;

		case REFRESH_LOCATION:
			return RefreshLocationInfo(hWnd);
			break;

		case WM_COMMAND:
			{
				switch ( LOWORD(wParam) )
				{
					case IDOK:
						locProcLocIndex = NO_LOCATION;
						EndDialog(hWnd, IDOK);
						break;
					case IDCANCEL:
						locProcLocIndex = NO_LOCATION;
						EndDialog(hWnd, IDCANCEL);
						break;
					case IDC_INVERTX:
						if ( HIWORD(wParam) == BN_CLICKED )
						{
							ChkLocation* locRef;
							if ( chkd.maps.curr != nullptr && chkd.maps.curr->getLocation(locRef, locProcLocIndex) )
							{
								chkd.maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_XC1, locRef->xc1);
								chkd.maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_XC2, locRef->xc2);
								chkd.maps.curr->undos().startNext(0);
								std::swap(locRef->xc1, locRef->xc2);
								RefreshLocationInfo(hWnd);
								chkd.maps.curr->Redraw(false);
							}
						}
						break;
					case IDC_INVERTY:
						if ( HIWORD(wParam) == BN_CLICKED )
						{
							ChkLocation* locRef;
							if ( chkd.maps.curr != nullptr && chkd.maps.curr->getLocation(locRef, locProcLocIndex) )
							{
								chkd.maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_YC1, locRef->yc1);
								chkd.maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_YC2, locRef->yc2);
								chkd.maps.curr->undos().startNext(0);
								std::swap(locRef->yc1, locRef->yc2);
								RefreshLocationInfo(hWnd);
								chkd.maps.curr->Redraw(false);
							}
						}
						break;
					case IDC_INVERTXY:
						if ( HIWORD(wParam) == BN_CLICKED )
						{
							ChkLocation* locRef;
							if ( chkd.maps.curr != nullptr && chkd.maps.curr->getLocation(locRef, locProcLocIndex) )
							{
								chkd.maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_XC1, locRef->xc1);
								chkd.maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_XC2, locRef->xc2);
								chkd.maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_YC1, locRef->yc1);
								chkd.maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_YC2, locRef->yc2);
								chkd.maps.curr->undos().startNext(0);
								std::swap(locRef->xc1, locRef->xc2);
								std::swap(locRef->yc1, locRef->yc2);
								RefreshLocationInfo(hWnd);
								chkd.maps.curr->Redraw(false);
							}
						}
						break;
					default:
						{
							if ( !initializing )
							{
								switch ( HIWORD(wParam) )
								{
									case BN_CLICKED:
										{
											ChkLocation* locRef;
											if ( chkd.maps.curr != nullptr && chkd.maps.curr->getLocation(locRef, locProcLocIndex) )
											{
												LRESULT result = SendMessage((HWND)lParam, BM_GETCHECK,  NULL, NULL);
												if ( result == BST_CHECKED || result == BST_UNCHECKED )
												{
													bool isChecked = (result == BST_CHECKED);
													if ( isChecked )
													{
														switch ( LOWORD(wParam) )
														{
															case IDC_LOWGROUND:
																chkd.maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_ELEVATION, locRef->elevation);
																chkd.maps.curr->undos().startNext(0);
																locRef->elevation &= (~LOC_ELEVATION_LOWGROUND);
																break;
															case IDC_MEDGROUND:
																chkd.maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_ELEVATION, locRef->elevation);
																chkd.maps.curr->undos().startNext(0);
																locRef->elevation &= (~LOC_ELEVATION_MEDGROUND);
																break;
															case IDC_HIGHGROUND:
																chkd.maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_ELEVATION, locRef->elevation);
																chkd.maps.curr->undos().startNext(0);
																locRef->elevation &= (~LOC_ELEVATION_HIGHGROUND);
																break;
															case IDC_LOWAIR:
																chkd.maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_ELEVATION, locRef->elevation);
																chkd.maps.curr->undos().startNext(0);
																locRef->elevation &= (~LOC_ELEVATION_LOWAIR);
																break;
															case IDC_MEDAIR:
																chkd.maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_ELEVATION, locRef->elevation);
																chkd.maps.curr->undos().startNext(0);
																locRef->elevation &= (~LOC_ELEVATION_MEDAIR);
																break;
															case IDC_HIGHAIR:
																chkd.maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_ELEVATION, locRef->elevation);
																chkd.maps.curr->undos().startNext(0);
																locRef->elevation &= (~LOC_ELEVATION_HIGHAIR);
																break;
															case IDC_EXTLOCNAMESTR:
																{
																	string str;
																	if ( chkd.maps.curr->getString(str, locRef->stringNum) )
																	{
																		u32 newStrNum;
																		if ( chkd.maps.curr->addString(str, newStrNum, true) )
																		{
																			u32 oldStrNum = locRef->stringNum;
																			locRef->stringNum = u16(newStrNum);
																			chkd.maps.curr->removeUnusedString(oldStrNum);
																			chkd.maps.curr->refreshScenario();
																		}
																	}
																	chkd.maps.curr->notifyChange(false);
																}
																break;
														}
														RefreshLocationInfo(hWnd);
													}
													else // notChecked
													{
														switch ( LOWORD(wParam) )
														{
															case IDC_LOWGROUND:
																chkd.maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_ELEVATION, locRef->elevation);
																chkd.maps.curr->undos().startNext(0);
																locRef->elevation |= LOC_ELEVATION_LOWGROUND;
																break;
															case IDC_MEDGROUND:
																chkd.maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_ELEVATION, locRef->elevation);
																chkd.maps.curr->undos().startNext(0);
																locRef->elevation |= LOC_ELEVATION_MEDGROUND;
																break;
															case IDC_HIGHGROUND:
																chkd.maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_ELEVATION, locRef->elevation);
																chkd.maps.curr->undos().startNext(0);
																locRef->elevation |= LOC_ELEVATION_HIGHGROUND;
																break;
															case IDC_LOWAIR:
																chkd.maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_ELEVATION, locRef->elevation);
																chkd.maps.curr->undos().startNext(0);
																locRef->elevation |= LOC_ELEVATION_LOWAIR;
																break;
															case IDC_MEDAIR:
																chkd.maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_ELEVATION, locRef->elevation);
																chkd.maps.curr->undos().startNext(0);
																locRef->elevation |= LOC_ELEVATION_MEDAIR;
																break;
															case IDC_HIGHAIR:
																chkd.maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_ELEVATION, locRef->elevation);
																chkd.maps.curr->undos().startNext(0);
																locRef->elevation |= LOC_ELEVATION_HIGHAIR;
																break;
															case IDC_EXTLOCNAMESTR:
																{
																	string str;
																	if ( chkd.maps.curr->getString(str, locRef->stringNum) )
																	{
																		u32 newStrNum;
																		if ( chkd.maps.curr->addString(str, newStrNum, false) )
																		{
																			u32 oldStrNum = locRef->stringNum;
																			locRef->stringNum = u16(newStrNum);
																			chkd.maps.curr->removeUnusedString(oldStrNum);
																			chkd.maps.curr->refreshScenario();
																		}
																	}
																	chkd.maps.curr->notifyChange(false);
																}
																break;
														}
														RefreshLocationInfo(hWnd);
													}
												}
											}
										}
										break;
									case EN_SETFOCUS:
										{
											ChkLocation* locRef;
											if ( chkd.maps.curr != nullptr && chkd.maps.curr->getLocation(locRef, locProcLocIndex) )
											{
												switch ( LOWORD(wParam) )
												{
													case IDC_LOCATION_NAME: preservedStat = locRef->stringNum; break;
													case IDC_RAWFLAGS: preservedStat = locRef->elevation; break;
													case IDC_LOCLEFT: preservedStat = locRef->xc1; break;
													case IDC_LOCTOP: preservedStat = locRef->yc1; break;
													case IDC_LOCRIGHT:  preservedStat = locRef->xc2; break;
													case IDC_LOCBOTTOM:  preservedStat = locRef->yc2; break;
												}
											}
										}
										break;
									case EN_KILLFOCUS:
										{
											ChkLocation* locRef;
											if ( chkd.maps.curr != nullptr && chkd.maps.curr->getLocation(locRef, locProcLocIndex) )
											{
												switch ( LOWORD(wParam) )
												{
													case IDC_RAWFLAGS:
														{
															u16 newVal;
															if ( GetEditBinaryNum(hWnd, IDC_RAWFLAGS, newVal) && preservedStat != newVal )
															{
																locRef->elevation = newVal;
																chkd.maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_ELEVATION, preservedStat);
																chkd.maps.curr->undos().startNext(0);
																RefreshLocationInfo(hWnd);
															}
														}
														break;
													case IDC_LOCLEFT:
														{
															int newVal;
															if ( GetEditNum<int>(hWnd, IDC_LOCLEFT, newVal) )
															{
																locRef->xc1 = newVal;
																if ( newVal != preservedStat )
																{
																	chkd.maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_XC1, preservedStat);
																	chkd.maps.curr->undos().startNext(0);
																}
																chkd.maps.curr->Redraw(false);
															}
														}
														break;
													case IDC_LOCTOP:
														{
															int newVal;
															if ( GetEditNum<int>(hWnd, IDC_LOCTOP, newVal) )
															{
																locRef->yc1 = newVal;
																if ( newVal != preservedStat )
																{
																	chkd.maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_YC1, preservedStat);
																	chkd.maps.curr->undos().startNext(0);
																}
																chkd.maps.curr->Redraw(false);
															}
														}
														break;
													case IDC_LOCRIGHT:
														{
															int newVal;
															if ( GetEditNum<int>(hWnd, IDC_LOCRIGHT, newVal) )
															{
																locRef->xc2 = newVal;
																if ( newVal != preservedStat )
																{
																	chkd.maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_XC2, preservedStat);
																	chkd.maps.curr->undos().startNext(0);
																}
																chkd.maps.curr->Redraw(false);
															}
														}
														break;
													case IDC_LOCBOTTOM:
														{
															int newVal;
															if ( GetEditNum<int>(hWnd, IDC_LOCBOTTOM, newVal) )
															{
																locRef->yc2 = newVal;
																if ( newVal != preservedStat )
																{
																	chkd.maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_YC2, preservedStat);
																	chkd.maps.curr->undos().startNext(0);
																}
																chkd.maps.curr->Redraw(false);
															}
														}
														break;
													case IDC_LOCATION_NAME:
														{
															std::string oldString;
															if ( chkd.maps.curr->getString(oldString, preservedStat) )
															{
																char* newString;
																int textLength = GetWindowTextLength(GetDlgItem(hWnd, IDC_LOCATION_NAME))+1;
																if ( textLength != 0 )
																{
																	try {
																		newString = new char[textLength];
																	} catch ( std::bad_alloc ) { return TRUE; }

																	if ( GetWindowText(GetDlgItem(hWnd, IDC_LOCATION_NAME), newString, textLength) == 0 )
																	{
																		delete[] newString;
																		return TRUE;
																	}

																	if ( oldString.compare(newString) != 0 ) // Attempt string replace
																	{
																		std::string newStr;
																		try {
																			newStr = newString;
																		} catch ( std::bad_alloc ) { delete[] newString; return TRUE; }
																		delete[] newString;

																	
																		u32 newStringNum;
																		bool isExtended = (SendMessage(GetDlgItem(hWnd, IDC_EXTLOCNAMESTR), BM_GETCHECK, NULL, NULL) == BST_CHECKED);
																		if ( parseEscapedString(newStr) && chkd.maps.curr->addString(newStr, newStringNum, isExtended) )
																		{
																			locRef->stringNum = u16(newStringNum);
																			chkd.mainPlot.leftBar.mainTree.BuildLocationTree();
																			chkd.maps.curr->removeUnusedString(preservedStat);
																			chkd.maps.curr->notifyChange(false);
																			chkd.maps.curr->refreshScenario();
																		}
																	}
																}
															}
														}
														break;
												}
											}
										}
										break;
									case EN_UPDATE:
										switch ( LOWORD(wParam) )
										{
											case IDC_RAWFLAGS:
												{
													ChkLocation* locRef;
													u16 newVal;
													if ( chkd.maps.curr && chkd.maps.curr->getLocation(locRef, locProcLocIndex) &&
														 GetEditBinaryNum(hWnd, IDC_RAWFLAGS, newVal) && preservedStat != newVal )
													{
														locRef->elevation = newVal;
														initializing = true;
														RefreshLocationElevationFlags(locRef, hWnd);
														initializing = false;
													}
												}
												break;
											case IDC_LOCLEFT:
												{
													ChkLocation* locRef;
													int newVal;
													if ( chkd.maps.curr && chkd.maps.curr->getLocation(locRef, locProcLocIndex) && GetEditNum<int>(hWnd, IDC_LOCLEFT, newVal) )
													{
														locRef->xc1 = newVal;
														chkd.maps.curr->Redraw(false);
													}
												}
												break;
											case IDC_LOCTOP:
												{
													ChkLocation* locRef;
													int newVal;
													if ( chkd.maps.curr && chkd.maps.curr->getLocation(locRef, locProcLocIndex) && GetEditNum<int>(hWnd, IDC_LOCTOP, newVal) )
													{
														locRef->yc1 = newVal;
														chkd.maps.curr->Redraw(false);
													}
												}
												break;
											case IDC_LOCRIGHT:
												{
													ChkLocation* locRef;
													int newVal;
													if ( chkd.maps.curr && chkd.maps.curr->getLocation(locRef, locProcLocIndex) && GetEditNum<int>(hWnd, IDC_LOCRIGHT, newVal) )
													{
														locRef->xc2 = newVal;
														chkd.maps.curr->Redraw(false);
													}
												}
												break;
											case IDC_LOCBOTTOM:
												{
													ChkLocation* locRef;
													int newVal;
													if ( chkd.maps.curr && chkd.maps.curr->getLocation(locRef, locProcLocIndex) && GetEditNum<int>(hWnd, IDC_LOCBOTTOM, newVal) )
													{
														locRef->yc2 = newVal;
														chkd.maps.curr->Redraw(false);
													}
												}
												break;
										}
										break;
								}
							}
						}
						break;
				}
				return TRUE;
			}
			break;

		default:
			return FALSE;
			break;
	}
	return FALSE;
}
