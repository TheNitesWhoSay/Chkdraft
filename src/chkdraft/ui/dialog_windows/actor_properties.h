#ifndef ACTORPROPERTIES_H
#define ACTORPROPERTIES_H
#include <common_files/common_files.h>
#include <windows/windows_ui.h>
#include <mapping_core/mapping_core.h>
#include "ui/chkd_controls/player_dropdown.h"

enum class ActorListColumn { DrawListIndex, Type, UnitSpriteIndex, Priority, Name };

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
        void RefreshAnimFrame();

    protected:
        void EnableActorEditing();
        void DisableActorEditing();
        void DisableImageEditing();
        void ViewActor(const MapActor* actor);
        void UpdateActorFieldText();
        void UpdateImageFieldText();
        void RefreshActorImageList(const MapActor* actor);
        void ImageSelectionChanged();
        void LvItemChanged(NMHDR* nmhdr);

        void NotifyClosePressed();

        void ActorDirectionUpdated();
        void ActorElevationUpdated();
        void ImageIdUpdated();
        void ImageIscriptIdUpdated();
        void ImageOwnerUpdated();
        void ImageDirectionUpdated();
        void ImageXcUpdated();
        void ImageYcUpdated();
        void ImageOffsetXcUpdated();
        void ImageOffsetYcUpdated();
        void ImageBaseFrameUpdated();
        void ImageGrpFrameUpdated();

        void AnimationSelectionChanged();
        void RemappingSelectionChanged();
        void SelColorSelectionChanged();
        void DrawFuncSelectionChanged();

        void NotifyResetActorClicked();
        void NotifyPausePlayClicked();
        void NotifyAutoRefreshClicked();
        void NotifyAutoRestartClicked();
        void NotifyPrimaryImageClicked();
        void NotifyFlippedClicked();
        void NotifyRotationEnabledClicked();
        void NotifyHiddenClicked();
        void NotifyDrawIfCloakedClicked();
        void NotifyCustomColorClicked();

        virtual void NotifyButtonClicked(int idFrom, HWND hWndFrom); // Sent when a button or checkbox is clicked
        virtual void NotifyComboSelChanged(int idFrom, HWND hWndFrom);
        virtual void NotifyEditUpdated(int idFrom, HWND hWndFrom); // Sent when edit text changes, before redraw
        virtual void NotifyEditFocusLost(int idFrom, HWND hWndFrom); // Sent when focus changes or the window is hidden

        BOOL Activate(WPARAM wParam, LPARAM lParam);
        BOOL ShowWindow(WPARAM wParam, LPARAM lParam);

        BOOL DlgNotify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr);
        BOOL DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
        BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

        MapActor* getActiveActor();
        MapImage* getActiveImage();

    private:
        static constexpr std::size_t noSelectedActor = std::numeric_limits<std::size_t>::max();
        std::size_t selectedActorIndex = noSelectedActor;
        int selectedImageSlot = -1;
        std::size_t selectedImageIndex = 0;
        std::optional<WinLib::DeviceContext> imageListDc {}; // Sound list HDC for speeding up string measurement
        std::string imageStrings[10] {};

        WinLib::ButtonControl buttonResetActor;
        WinLib::ButtonControl buttonPausePlayAnimation;
        WinLib::CheckBoxControl checkAutoRefresh;
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
        WinLib::ButtonControl buttonCustomColor;
        WinLib::DropdownControl dropDrawFunction;

        ActorListColumn columnSortedBy = ActorListColumn::Priority;
        bool flipSort = false;
        bool initilizing = true;
        bool autoRefresh = true;

        MapActor lastRefreshedActorData {};
        MapImage lastRefreshedImageData {}; // Note that the animation pointer is always null

        WinLib::ListViewControl listActors;
};

#endif