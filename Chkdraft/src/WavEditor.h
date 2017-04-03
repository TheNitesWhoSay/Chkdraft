#ifndef WAVEDITOR_H
#define WAVEDITOR_H
#include "CommonFiles/CommonFiles.h"
#include "MappingCore/MappingCore.h"
#include "WindowsUI/WindowsUI.h"

class WavEditorWindow : public ClassWindow
{
    public:
        WavEditorWindow();
        bool CreateThis(HWND hParent, u32 windowId);
        void RefreshWindow();

    protected:
        void UpdateWindowText();
        void BrowseButtonPressed();
        void AddFileButtonPressed();
        void CheckVirtualFilePressed();
        void CompressionDropdownChanged();
        void MapSoundSelectionChanged();
        void DeleteSoundButtonPressed();

        LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

        virtual void NotifyButtonClicked(int idFrom, HWND hWndFrom); // Sent when a button or checkbox is clicked
        virtual void NotifyComboSelChanged(int idFrom, HWND hWndFrom); // Sent when combo or listbox is changed by selection

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

        int selectedSoundListIndex;
        WavQuality wavQuality;
        HDC wavListDC; // WAV list HDC for speeding up string measurement
        std::map<u32/*stringIndex*/, u16/*wavIndex*/> wavMap;
};

#endif