#ifndef UNIT_H
#define UNIT_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"

BOOL CALLBACK UnitPropProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void EnableUnitEditing(HWND hWnd);
void DisableUnitEditing(HWND hWnd);
void SetUnitFieldText(HWND hWnd, ChkUnit* unit);

#endif