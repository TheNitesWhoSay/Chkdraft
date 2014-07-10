#include "Location.h"
#include "Maps.h"
#include "GuiAccel.h"

extern HWND hLocation;
extern MAPS maps;

bool initializing(false);

u32 preservedStat(0);
u16 locProcLocIndex(NO_LOCATION);

void RefreshLocationElevationFlags(ChkLocation* locRef, HWND hWnd)
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

LRESULT RefreshLocationInfo(HWND hWnd)
{
	initializing = true;

	if ( !maps.curr )
	{
		locProcLocIndex = NO_LOCATION;
		initializing = false;
		EndDialog(hWnd, IDCLOSE);
		return TRUE;
	}

	locProcLocIndex = maps.curr->selections().getSelectedLocation();
	ChkLocation* locRef;
	if ( locProcLocIndex != NO_LOCATION && maps.curr->getLocation(locRef, locProcLocIndex) )
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

		if ( maps.curr->isExtendedString(locRef->stringNum) )
			SendMessage(GetDlgItem(hWnd, IDC_EXTLOCNAMESTR), BM_SETCHECK, BST_CHECKED, NULL);
		else
			SendMessage(GetDlgItem(hWnd, IDC_EXTLOCNAMESTR), BM_SETCHECK, BST_UNCHECKED, NULL);

		std::string locName;
		if ( maps.curr->getString(locName, locRef->stringNum) )
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

BOOL CALLBACK LocationPropProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
							if ( maps.curr != nullptr && maps.curr->getLocation(locRef, locProcLocIndex) )
							{
								maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_XC1, locRef->xc1);
								maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_XC2, locRef->xc2);
								maps.curr->undos().startNext(0);
								std::swap(locRef->xc1, locRef->xc2);
								RefreshLocationInfo(hWnd);
								maps.curr->Redraw(false);
							}
						}
						break;
					case IDC_INVERTY:
						if ( HIWORD(wParam) == BN_CLICKED )
						{
							ChkLocation* locRef;
							if ( maps.curr != nullptr && maps.curr->getLocation(locRef, locProcLocIndex) )
							{
								maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_YC1, locRef->yc1);
								maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_YC2, locRef->yc2);
								maps.curr->undos().startNext(0);
								std::swap(locRef->yc1, locRef->yc2);
								RefreshLocationInfo(hWnd);
								maps.curr->Redraw(false);
							}
						}
						break;
					case IDC_INVERTXY:
						if ( HIWORD(wParam) == BN_CLICKED )
						{
							ChkLocation* locRef;
							if ( maps.curr != nullptr && maps.curr->getLocation(locRef, locProcLocIndex) )
							{
								maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_XC1, locRef->xc1);
								maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_XC2, locRef->xc2);
								maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_YC1, locRef->yc1);
								maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_YC2, locRef->yc2);
								maps.curr->undos().startNext(0);
								std::swap(locRef->xc1, locRef->xc2);
								std::swap(locRef->yc1, locRef->yc2);
								RefreshLocationInfo(hWnd);
								maps.curr->Redraw(false);
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
											if ( maps.curr != nullptr && maps.curr->getLocation(locRef, locProcLocIndex) )
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
																maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_ELEVATION, locRef->elevation);
																maps.curr->undos().startNext(0);
																locRef->elevation &= (~LOC_ELEVATION_LOWGROUND);
																break;
															case IDC_MEDGROUND:
																maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_ELEVATION, locRef->elevation);
																maps.curr->undos().startNext(0);
																locRef->elevation &= (~LOC_ELEVATION_MEDGROUND);
																break;
															case IDC_HIGHGROUND:
																maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_ELEVATION, locRef->elevation);
																maps.curr->undos().startNext(0);
																locRef->elevation &= (~LOC_ELEVATION_HIGHGROUND);
																break;
															case IDC_LOWAIR:
																maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_ELEVATION, locRef->elevation);
																maps.curr->undos().startNext(0);
																locRef->elevation &= (~LOC_ELEVATION_LOWAIR);
																break;
															case IDC_MEDAIR:
																maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_ELEVATION, locRef->elevation);
																maps.curr->undos().startNext(0);
																locRef->elevation &= (~LOC_ELEVATION_MEDAIR);
																break;
															case IDC_HIGHAIR:
																maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_ELEVATION, locRef->elevation);
																maps.curr->undos().startNext(0);
																locRef->elevation &= (~LOC_ELEVATION_HIGHAIR);
																break;
															case IDC_EXTLOCNAMESTR:
																{
																	string str;
																	if ( maps.curr->getString(str, locRef->stringNum) )
																	{
																		u32 newStrNum;
																		if ( maps.curr->addString(str, newStrNum, true) )
																		{
																			u32 oldStrNum = locRef->stringNum;
																			locRef->stringNum = u16(newStrNum);
																			maps.curr->removeUnusedString(oldStrNum);
																		}
																	}
																	maps.curr->notifyChange(false);
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
																maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_ELEVATION, locRef->elevation);
																maps.curr->undos().startNext(0);
																locRef->elevation |= LOC_ELEVATION_LOWGROUND;
																break;
															case IDC_MEDGROUND:
																maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_ELEVATION, locRef->elevation);
																maps.curr->undos().startNext(0);
																locRef->elevation |= LOC_ELEVATION_MEDGROUND;
																break;
															case IDC_HIGHGROUND:
																maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_ELEVATION, locRef->elevation);
																maps.curr->undos().startNext(0);
																locRef->elevation |= LOC_ELEVATION_HIGHGROUND;
																break;
															case IDC_LOWAIR:
																maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_ELEVATION, locRef->elevation);
																maps.curr->undos().startNext(0);
																locRef->elevation |= LOC_ELEVATION_LOWAIR;
																break;
															case IDC_MEDAIR:
																maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_ELEVATION, locRef->elevation);
																maps.curr->undos().startNext(0);
																locRef->elevation |= LOC_ELEVATION_MEDAIR;
																break;
															case IDC_HIGHAIR:
																maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_ELEVATION, locRef->elevation);
																maps.curr->undos().startNext(0);
																locRef->elevation |= LOC_ELEVATION_HIGHAIR;
																break;
															case IDC_EXTLOCNAMESTR:
																{
																	string str;
																	if ( maps.curr->getString(str, locRef->stringNum) )
																	{
																		u32 newStrNum;
																		if ( maps.curr->addString(str, newStrNum, false) )
																		{
																			u32 oldStrNum = locRef->stringNum;
																			locRef->stringNum = u16(newStrNum);
																			maps.curr->removeUnusedString(oldStrNum);
																		}
																	}
																	maps.curr->notifyChange(false);
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
											if ( maps.curr != nullptr && maps.curr->getLocation(locRef, locProcLocIndex) )
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
											if ( maps.curr != nullptr && maps.curr->getLocation(locRef, locProcLocIndex) )
											{
												switch ( LOWORD(wParam) )
												{
													case IDC_RAWFLAGS:
														{
															u16 newVal;
															if ( GetEditBinaryNum(hWnd, IDC_RAWFLAGS, newVal) && preservedStat != newVal )
															{
																locRef->elevation = newVal;
																maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_ELEVATION, preservedStat);
																maps.curr->undos().startNext(0);
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
																	maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_XC1, preservedStat);
																	maps.curr->undos().startNext(0);
																}
																maps.curr->Redraw(false);
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
																	maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_YC1, preservedStat);
																	maps.curr->undos().startNext(0);
																}
																maps.curr->Redraw(false);
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
																	maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_XC2, preservedStat);
																	maps.curr->undos().startNext(0);
																}
																maps.curr->Redraw(false);
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
																	maps.curr->undos().addUndoLocationChange(locProcLocIndex, LOC_FIELD_YC2, preservedStat);
																	maps.curr->undos().startNext(0);
																}
																maps.curr->Redraw(false);
															}
														}
														break;
													case IDC_LOCATION_NAME:
														{
															std::string oldString;
															if ( maps.curr->getString(oldString, preservedStat) )
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
																		if ( maps.curr->addString(newStr, newStringNum, isExtended) )
																		{
																			locRef->stringNum = u16(newStringNum);
																			BuildLocationTree(maps.curr);
																			maps.curr->removeUnusedString(preservedStat);
																			maps.curr->Redraw(false);
																			maps.curr->notifyChange(false);
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
													if ( maps.curr && maps.curr->getLocation(locRef, locProcLocIndex) &&
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
													if ( maps.curr && maps.curr->getLocation(locRef, locProcLocIndex) && GetEditNum<int>(hWnd, IDC_LOCLEFT, newVal) )
													{
														locRef->xc1 = newVal;
														maps.curr->Redraw(false);
													}
												}
												break;
											case IDC_LOCTOP:
												{
													ChkLocation* locRef;
													int newVal;
													if ( maps.curr && maps.curr->getLocation(locRef, locProcLocIndex) && GetEditNum<int>(hWnd, IDC_LOCTOP, newVal) )
													{
														locRef->yc1 = newVal;
														maps.curr->Redraw(false);
													}
												}
												break;
											case IDC_LOCRIGHT:
												{
													ChkLocation* locRef;
													int newVal;
													if ( maps.curr && maps.curr->getLocation(locRef, locProcLocIndex) && GetEditNum<int>(hWnd, IDC_LOCRIGHT, newVal) )
													{
														locRef->xc2 = newVal;
														maps.curr->Redraw(false);
													}
												}
												break;
											case IDC_LOCBOTTOM:
												{
													ChkLocation* locRef;
													int newVal;
													if ( maps.curr && maps.curr->getLocation(locRef, locProcLocIndex) && GetEditNum<int>(hWnd, IDC_LOCBOTTOM, newVal) )
													{
														locRef->yc2 = newVal;
														maps.curr->Redraw(false);
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
