#include "WavEditor.h"
#include "Chkdraft.h"

enum ID {
	TEXT_MAPSOUNDFILES = ID_FIRST,
	BUTTON_STOPSOUNDS,
	BUTTON_DELETESOUND,
	BUTTON_EXTRACTSOUND,
	BUTTON_PLAYSOUND,
	LB_MAPSOUNDS,
	TEXT_VIRTUALSOUNDS,
	BUTTON_PLAYVIRTUALSOUND,
	LB_VIRTUALSOUNDS,
	TEXT_SOUNDFILENAME,
	EDIT_SOUNDFILENAME,
	BUTTON_BROWSEFORSOUND,
	TEXT_COMPRESSIONLEVEL,
	DROP_COMPRESSIONQUALITY,
	CHECK_VIRTUALFILE,
	BUTTON_ADDFILE
};

bool WavEditorWindow::CreateThis(HWND hParent, u32 windowId)
{
	if ( getHandle() != NULL )
		return SetParent(hParent);

	if ( ClassWindow::RegisterWindowClass(NULL, NULL, NULL, NULL, NULL, "WavEditor", NULL, false) &&
		 ClassWindow::CreateClassWindow(NULL, "WavEditor", WS_VISIBLE|WS_CHILD, 4, 22, 592, 524, hParent, (HMENU)windowId) )
	{
		CreateSubWindows(getHandle());
		return true;
	}
	else
		return false;
}

void WavEditorWindow::RefreshWindow()
{

}

LRESULT WavEditorWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_SHOWWINDOW:
			if ( wParam == TRUE )
			{
				RefreshWindow();
				chkd.mapSettingsWindow.SetTitle("Map Settings");
			}
			break;

		default:
			return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
			break;
	}
	return 0;
}

void WavEditorWindow::CreateSubWindows(HWND hWnd)
{
	textMapSoundFiles.CreateThis(hWnd, 5, 5, 100, 20, "Map Sound Files", TEXT_MAPSOUNDFILES);
	buttonStopSounds.CreateThis(hWnd, 127, 3, 110, 20, "Stop Sounds", BUTTON_STOPSOUNDS);
	buttonDeleteSound.CreateThis(hWnd, 242, 3, 110, 20, "Delete Selected", BUTTON_DELETESOUND);
	buttonExtractSound.CreateThis(hWnd, 357, 3, 110, 20, "Extract Selected", BUTTON_EXTRACTSOUND);
	buttonPlaySound.CreateThis(hWnd, 472, 3, 110, 20, "Play Selected", BUTTON_PLAYSOUND);
	listMapSounds.CreateThis(hWnd, 5, 25, 582, 200, false, false, false, LB_MAPSOUNDS);

	textAvailableSounds.CreateThis(hWnd, 5, 219, 200, 20, "Available MPQ sound files (Virtual Sounds)", TEXT_VIRTUALSOUNDS);
	buttonPreviewPlaySound.CreateThis(hWnd, 432, 217, 150, 20, "Play Selected", BUTTON_PLAYVIRTUALSOUND);
	listVirtualSounds.CreateThis(hWnd, 5, 239, 582, 200, false, false, false, LB_VIRTUALSOUNDS);
	textFileName.CreateThis(hWnd, 5, 434, 100, 20, "Filename", TEXT_SOUNDFILENAME);
	editFileName.CreateThis(hWnd, 140, 434, 352, 20, false, EDIT_SOUNDFILENAME);
	buttonBrowse.CreateThis(hWnd, 502, 434, 80, 20, "Browse", BUTTON_BROWSEFORSOUND);
	textCompressionLevel.CreateThis(hWnd, 5, 459, 100, 20, "Compression Level", TEXT_COMPRESSIONLEVEL);
	const char* compressionLevels[] = { "Uncompressed", "Low Quality", "Medium Quality", "High Quality" };
	dropCompressionLevel.CreateThis(hWnd, 140, 459, 150, 200, false, false, DROP_COMPRESSIONQUALITY, 4, compressionLevels, defaultFont);
	checkVirtualFile.CreateThis(hWnd, 300, 459, 100, 20, false, "Virtual File", CHECK_VIRTUALFILE);
	buttonAddFile.CreateThis(hWnd, 502, 459, 80, 20, "Add File", BUTTON_ADDFILE);

	DisableMapSoundEditing();
	DisableSoundCustomization();
	buttonPreviewPlaySound.DisableThis();

	textAvailableSounds.DisableThis();
	listVirtualSounds.DisableThis();
	textFileName.DisableThis();
	editFileName.DisableThis();
	buttonBrowse.DisableThis();
}

void WavEditorWindow::DisableMapSoundEditing()
{
	textMapSoundFiles.DisableThis();
	buttonStopSounds.DisableThis();
	buttonDeleteSound.DisableThis();
	buttonExtractSound.DisableThis();
	buttonPlaySound.DisableThis();
	listMapSounds.DisableThis();
}

void WavEditorWindow::EnableMapSoundEditing()
{
	textMapSoundFiles.EnableThis();
	buttonStopSounds.EnableThis();
	buttonDeleteSound.EnableThis();
	buttonExtractSound.EnableThis();
	buttonPlaySound.EnableThis();
	listMapSounds.EnableThis();
}

void WavEditorWindow::DisableSoundCustomization()
{
	textCompressionLevel.DisableThis();
	dropCompressionLevel.DisableThis();
	checkVirtualFile.DisableThis();
	buttonAddFile.DisableThis();
}

void WavEditorWindow::EnableSoundCustomization()
{
	textCompressionLevel.EnableThis();
	dropCompressionLevel.EnableThis();
	checkVirtualFile.EnableThis();
	buttonAddFile.EnableThis();
}
