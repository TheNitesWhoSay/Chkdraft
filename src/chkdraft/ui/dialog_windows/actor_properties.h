#ifndef ACTORPROPERTIES_H
#define ACTORPROPERTIES_H
#include <common_files/common_files.h>
#include <windows/windows_ui.h>
#include <mapping_core/mapping_core.h>
#include "ui/chkd_controls/player_dropdown.h"

enum class ActorListColumn;

class ActorPropertiesWindow : public WinLib::ClassDialog
{
    public:
        ActorPropertiesWindow();
        virtual ~ActorPropertiesWindow();
        bool CreateThis(HWND hParent);
        bool CreateSubWindows(HWND hWnd);
        bool DestroyThis();
        void SetListRedraw(bool redraw);
        bool AddActor(bool isUnit, bool isSpriteUnit, std::size_t unitOrSpriteType, std::size_t unitOrSpriteIndex, const MapActor & actor);
        void FocusAndSelectIndex(u16 drawListIndex);
        void DeselectIndex(u16 drawListIndex);
        void UpdateEnabledState();
        void RepopulateList();

    protected:
        void EnableActorEditing();
        void DisableActorEditing();
        void UpdateActorFieldText();
        void UpdateImageFieldText();
        void ImageSelectionChanged();
        void LvItemChanged(NMHDR* nmhdr);

        void NotifyClosePressed();

        virtual void NotifyButtonClicked(int idFrom, HWND hWndFrom); // Sent when a button or checkbox is clicked
        virtual void NotifyComboSelChanged(int idFrom, HWND hWndFrom);

        BOOL Activate(WPARAM wParam, LPARAM lParam);
        BOOL ShowWindow(WPARAM wParam, LPARAM lParam);

        BOOL DlgNotify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr);
        BOOL DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
        BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        static constexpr std::size_t noSelectedActor = std::numeric_limits<std::size_t>::max();
        std::size_t selectedActorIndex = noSelectedActor;
        int selectedImageSlot = -1;
        std::size_t selectedImageIndex = 0;
        std::optional<WinLib::DeviceContext> imageListDc {}; // Sound list HDC for speeding up string measurement
        std::string imageStrings[10] {};

        WinLib::ButtonControl buttonPausePlayAnimation;
        WinLib::CheckBoxControl checkAutoRestart;
        WinLib::EditControl editActorDirection;
        WinLib::EditControl editElevation;
        WinLib::DropdownControl dropAnimation;
        WinLib::ListBoxControl listboxImages;
        WinLib::CheckBoxControl checkPrimaryImage;

        WinLib::EditControl editImageId;
        WinLib::EditControl editIscriptId;
        WinLib::EditControl editOwner;
        WinLib::EditControl editImageDirection;
        WinLib::EditControl editImageX;
        WinLib::EditControl editImageY;
        WinLib::EditControl editOffsetX;
        WinLib::EditControl editOffsetY;
        WinLib::EditControl editBaseFrame;
        WinLib::EditControl editGrpFrame;
        WinLib::CheckBoxControl checkFlipped;
        WinLib::CheckBoxControl checkRotationEnabled;
        WinLib::CheckBoxControl checkHidden;
        WinLib::CheckBoxControl checkDrawIfCloaked;
        WinLib::DropdownControl dropRemapping;
        WinLib::DropdownControl dropSelectionColor;
        WinLib::DropdownControl dropDrawFunction;

        ActorListColumn columnSortedBy;
        bool flipSort;
        bool initilizing;

        WinLib::ListViewControl listActors;
};

#endif