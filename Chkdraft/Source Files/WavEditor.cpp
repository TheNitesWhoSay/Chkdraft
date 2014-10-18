#include "WavEditor.h"

bool WavEditorWindow::CreateThis(HWND hParent)
{
	if ( getHandle() != NULL )
		return SetParent(getHandle(), hParent) != NULL;

	if ( ClassWindow::RegisterWindowClass(NULL, NULL, NULL, NULL, NULL, "WavEditor", NULL, false) &&
		 ClassWindow::CreateClassWindow(NULL, "WavEditor", WS_VISIBLE|WS_CHILD, 4, 22, 592, 524, hParent, (HMENU)ID_WAVEDITOR) )
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
		case WM_COMMAND:
			return DefWindowProc(hWnd, msg, wParam, lParam);
			break;

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
			break;
	}
	return 0;
}

void WavEditorWindow::CreateSubWindows(HWND hWnd)
{
	textMapSoundFiles.CreateThis(hWnd, 5, 5, 100, 20, "Map Sound Files", ID_TEXT_MAPSOUNDFILES);
	buttonStopSounds.CreateThis(hWnd, 127, 3, 110, 20, "Stop Sounds", ID_BUTTON_STOPSOUNDS);
	buttonDeleteSound.CreateThis(hWnd, 242, 3, 110, 20, "Delete Selected", ID_BUTTON_DELETESOUND);
	buttonExtractSound.CreateThis(hWnd, 357, 3, 110, 20, "Extract Selected", ID_BUTTON_EXTRACTSOUND);
	buttonPlaySound.CreateThis(hWnd, 472, 3, 110, 20, "Play Selected", ID_BUTTON_PLAYSOUND);
	listMapSounds.CreateThis(hWnd, 5, 25, 582, 200, false, ID_LB_MAPSOUNDS);

	textAvailableSounds.CreateThis(hWnd, 5, 219, 200, 20, "Available MPQ sound files (Virtual Sounds)", ID_TEXT_VIRTUALSOUNDS);
	buttonPreviewPlaySound.CreateThis(hWnd, 432, 217, 150, 20, "Play Selected", ID_BUTTON_PLAYVIRTUALSOUND);
	listVirtualSounds.CreateThis(hWnd, 5, 239, 582, 200, false, ID_LB_VIRTUALSOUNDS);
	textFileName.CreateThis(hWnd, 5, 434, 100, 20, "Filename", ID_TEXT_SOUNDFILENAME);
	editFileName.CreateThis(hWnd, 140, 434, 352, 20, false, ID_EDIT_SOUNDFILENAME);
	buttonBrowse.CreateThis(hWnd, 502, 434, 80, 20, "Browse", ID_BUTTON_BROWSEFORSOUND);
	textCompressionLevel.CreateThis(hWnd, 5, 459, 100, 20, "Compression Level", ID_TEXT_COMPRESSIONLEVEL);
	const char* compressionLevels[] = { "Uncompressed", "Low Quality", "Medium Quality", "High Quality" };
	dropCompressionLevel.CreateThis(hWnd, 140, 459, 150, 200, false, ID_DROP_COMPRESSIONQUALITY, 4, compressionLevels, defaultFont);
	checkVirtualFile.CreateThis(hWnd, 300, 459, 100, 20, false, "Virtual File", ID_CHECK_VIRTUALFILE);
	buttonAddFile.CreateThis(hWnd, 502, 459, 80, 20, "Add File", ID_BUTTON_ADDFILE);

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
