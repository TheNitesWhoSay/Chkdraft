#ifndef WAVEDITOR_H
#define WAVEDITOR_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"
#include "Windows UI/WindowsUI.h"

class WavEditorWindow : public ClassWindow
{
	public:
		bool CreateThis(HWND hParent);
		void RefreshWindow();

	private:
		LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		void CreateSubWindows(HWND hWnd);
		void DisableMapSoundEditing();
		void EnableMapSoundEditing();
		void DisableSoundCustomization();
		void EnableSoundCustomization();

		TextControl textMapSoundFiles;
		ButtonControl buttonStopSounds;
		ButtonControl buttonDeleteSound;
		ButtonControl buttonExtractSound;
		ButtonControl buttonPlaySound;
		ListBoxControl listMapSounds;

		TextControl textAvailableSounds;
		ButtonControl buttonPreviewPlaySound;
		ListBoxControl listVirtualSounds;
		TextControl textFileName;
		EditControl editFileName;
		ButtonControl buttonBrowse;
		TextControl textCompressionLevel;
		DropdownControl dropCompressionLevel;
		CheckBoxControl checkVirtualFile;
		ButtonControl buttonAddFile;
};

#endif