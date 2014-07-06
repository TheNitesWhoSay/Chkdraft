#ifndef MAPSETTINGS_H
#define MAPSETTINGS_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"

BOOL	CALLBACK MapSettingsProc	(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MapPropertiesProc	(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ForcesProc			(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK DragProc			(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK UnitSettingsProc	(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK UpgradeSettingsProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK TechSettingsProc	(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK StringPreviewProc	(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK StringGuideProc	(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK StringEditorProc	(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WavEditorProc		(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void ChangeMapSettingsTab(HWND hWnd, u32 tabID);

void CreateMapProperties(HWND hParent);
void CreateForces(HWND hParent);
void CreateUnitSettings(HWND hParent);
void CreateUpgradeSettings(HWND hParent);
void CreateTechSettings(HWND hParent);
void CreateStringEditor(HWND hParent);
void CreateWavEditor(HWND hParent);

HWND CreateButton(HWND hParent, int x, int y, int width, int height, const char* text, u32 buttonID);

#endif