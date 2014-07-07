#ifndef TERRAIN_H
#define TERRAIN_H
#include "Common Files/CommonFiles.h"

void SetTileDialog(HWND hWnd);

BOOL CALLBACK SetTileProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK TerrainPaletteProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif