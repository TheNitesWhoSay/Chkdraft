#ifndef WAVEDITOR_H
#define WAVEDITOR_H
#include "../../../CommonFiles/CommonFiles.h"
#include "../../../../WindowsLib/WindowsUi.h"
#include "../../../../MappingCoreLib/MappingCore.h"

class WavEditorWindow : public WinLib::ClassWindow
{
    public:
        WavEditorWindow();
        virtual ~WavEditorWindow();
        bool CreateThis(HWND hParent, u64 windowId);
        void RefreshWindow();

    protected:
        void UpdateWindowText();
        void UpdateCustomStringList();

        void PlaySoundButtonPressed();
        void PlayVirtualSoundButtonPressed();
        void ExtractSoundButtonPressed();
        void BrowseButtonPressed();
        void AddFileButtonPressed();
        void CheckVirtualFilePressed();
        void CompressionDropdownChanged();
        void MapSoundSelectionChanged();
        void VirtualSoundSelectionChanged();
        void StopSoundsButtonPressed();
        void DeleteSoundButtonPressed();
        void CheckCustomMpqPathPressed();

        LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

        virtual void NotifyButtonClicked(int idFrom, HWND hWndFrom); // Sent when a button or checkbox is clicked
        virtual void NotifyComboSelChanged(int idFrom, HWND hWndFrom); // Sent when combo or listbox is changed by selection

    private:
        void CreateSubWindows(HWND hWnd);
        void DisableMapSoundEditing();
        void EnableMapSoundEditing();
        void DisableSoundCustomization();
        void EnableSoundCustomization();

        WinLib::TextControl textMapSoundFiles;
        WinLib::ButtonControl buttonStopSounds;
        WinLib::ButtonControl buttonDeleteSound;
        WinLib::ButtonControl buttonExtractSound;
        WinLib::ButtonControl buttonPlaySound;
        WinLib::ListBoxControl listMapSounds;

        WinLib::TextControl textAvailableSounds;
        WinLib::ButtonControl buttonPreviewPlaySound;
        WinLib::ListBoxControl listVirtualSounds;
        WinLib::TextControl textFileName;
        WinLib::EditControl editFileName;
        WinLib::ButtonControl buttonBrowse;
        WinLib::TextControl textCompressionLevel;
        WinLib::DropdownControl dropCompressionLevel;
        WinLib::CheckBoxControl checkVirtualFile;
        WinLib::ButtonControl buttonAddFile;
        WinLib::TextControl textCustomMpqString;
        WinLib::CheckBoxControl checkCustomMpqString;
        WinLib::DropdownControl dropCustomMpqString;

        int selectedSoundListIndex;
        WavQuality wavQuality;
        HDC wavListDC; // WAV list HDC for speeding up string measurement
        std::map<u32/*stringIndex*/, u16/*wavIndex*/> wavMap;
};

#endif