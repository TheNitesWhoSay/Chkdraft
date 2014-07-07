#ifndef LOCATION_H
#define LOCATION_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"

extern u16 locProcLocIndex;

BOOL CALLBACK LocationPropProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif