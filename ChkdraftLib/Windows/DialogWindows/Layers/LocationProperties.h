#ifndef LOCATIONPROPERITES_H
#define LOCATIONPROPERITES_H
#include "../../../CommonFiles/CommonFiles.h"
#include "../../../../WindowsLib/WindowsUi.h"
#include "../../../../MappingCoreLib/MappingCore.h"

class LocationWindow : public WinLib::ClassDialog
{
    public:
        LocationWindow();
        virtual ~LocationWindow();
        bool CreateThis(HWND hParent);
        bool DestroyThis();
        void RefreshLocationInfo();

    protected:
        void RefreshLocationElevationFlags();

        void InvertXc();
        void InvertYc();
        void InvertXY();

        void NotifyLowGroundClicked();
        void NotifyMedGroundClicked();
        void NotifyHighGroundClicked();
        void NotifyLowAirClicked();
        void NotifyMedAirClicked();
        void NotifyHighAirClicked();
        void NotifyLocNamePropertiesClicked();

        void RawFlagsUpdated();
        void LocationLeftUpdated();
        void LocationTopUpdated();
        void LocationRightUpdated();
        void LocationBottomUpdated();

        void LocationNameFocusLost();
        void RawFlagsFocusLost();
        void LocationLeftFocusLost();
        void LocationTopFocusLost();
        void LocationRightFocusLost();
        void LocationBottomFocusLost();

        virtual void NotifyButtonClicked(int idFrom, HWND hWndFrom); // Sent when a button or checkbox is clicked
        virtual void NotifyEditUpdated(int idFrom, HWND hWndFrom); // Sent when edit text changes, before redraw
        virtual void NotifyEditFocused(int idFrom, HWND hWndFrom); // Sent when an edit box receives focus
        virtual void NotifyEditFocusLost(int idFrom, HWND hWndFrom); // Sent when focus changes or the window is hidden
        BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        bool refreshing;
        u32 preservedStat;
        u16 currentLocationId;

        WinLib::EditControl editLocName, editLocLeft, editLocTop, editLocRight, editLocBottom, editRawFlags;
        WinLib::CheckBoxControl checkLowGround, checkMedGround, checkHighGround, checkLowAir, checkMedAir, checkHighAir;
        WinLib::ButtonControl buttonLocNameProperties, buttonInvertX, buttonInvertY, buttonInvertXY;
};

#endif