#ifndef WAVEDITOR_H
#define WAVEDITOR_H
#include "CommonFiles/CommonFiles.h"
#include "MappingCore/MappingCore.h"
#include "WindowsUI/WindowsUI.h"

class WavEditorWindow : public ClassWindow
{
    public:
        bool CreateThis(HWND hParent, u32 windowId);
        void RefreshWindow();

    protected:
        LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
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
