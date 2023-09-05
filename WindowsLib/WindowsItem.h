#ifndef WINDOWSITEM_H
#define WINDOWSITEM_H
#include "DataTypes.h"
#include "DeviceContext.h"
#include <list>
#include <optional>
#include <string>
#include <Windows.h>

namespace WinLib {

    /**
        A class encapsulating an 'HWND' and various
        windows functions that act on such items
    */
    class WindowsItem
    {
        public:
            WindowsItem();
            virtual ~WindowsItem();

            void DestroyThis();

            bool operator==(HWND hWnd); // Tests whether the encapsulated handle equals this handle
            HWND getHandle();
            HWND getParent();
            DeviceContext getDeviceContext();

            int GetWinTextLen();
            std::optional<std::string> GetWinText();
            bool getWindowRect(RECT & rect);
            int Width();
            int Height();
            int Left();
            int Top();
            int Right();
            int Bottom();
            bool getClientRect(RECT & rect);
            LONG cliWidth();
            LONG cliHeight();
            bool isEnabled();

            LONG GetWinLong(int index);
            void SetWinLong(int index, LONG newLong);

            void setFont(int width, int height, const std::string & fontName, bool redrawImmediately = true);
            void setFont(HFONT hFont, bool redrawImmediately = true);
            void setDefaultFont(bool redrawImmediately = true);
            void replaceChildFonts(int width, int height, const std::string & fontName);
            void replaceChildFonts(HFONT hFont);
            void defaultChildFonts();

            void LockCursor(); // Prevents cursor from leaving this window
            void UnlockCursor(); // Globally unlocks the cursor
            void TrackMouse(DWORD hoverTime); // Causes mouse tracking messages to be sent to this window
            bool SetParent(HWND hParent);
            void SetRedraw(bool autoRedraw);
            virtual void RedrawThis();
            void RefreshFrame();

            void MoveTo(int x, int y);
            void SetPos(int x, int y, int width, int height);
            void ShowNormal();
            void Show();
            void Hide();
            void SetSmallIcon(HICON hIcon);
            void SetMedIcon(HICON hIcon);
            bool SetWinText(const std::string & newText);
            bool AddTooltip(const std::string & text);

            void FocusThis();
            void UpdateWindow();
            virtual void EnableThis();
            virtual void DisableThis();
            void SetWidth(int newWidth);
            void SetHeight(int newHeight);
            void SetSize(int newWidth, int newHeight);

        protected:
            virtual void setHandle(HWND hWnd);

            /** Attempts to register a class for creating the window
            If the class has been registered before this returns false
            Do not register different classes with the same name */
            bool RegisterWindowClass(UINT style, HICON hIcon, HCURSOR hCursor, HBRUSH hbrBackground,
                u64 menuResourceId, const std::string & lpszClassName, HICON hIconSm, WNDPROC wndProc);

            /** Returns whether the window class given by lpszClassName is registered,
            if it is, then this class name will be set to create the window with */
            bool WindowClassIsRegistered(const std::string & lpszClassName);

            /** This method is used to compare list view items for windows sort functions
                ForwardCompareLvItems must be set as the function and the class pointer as LPARAM */
            virtual int CompareLvItems(LPARAM index1, LPARAM index2);

            /** This method returns the value given by CompareLvItems with the same parameters
                This should be passed as the proc to window's built in sort items functions */
            static int CALLBACK ForwardCompareLvItems(LPARAM index1, LPARAM index2, LPARAM lParam);

            std::string & WindowClassName();

        private:
            HWND windowsItemHandle; // Handle to the window this class encapsulates
            HWND tooltipHandle; // Handle to any tooltip created for this class
            std::string windowClassName; // May contain the window class title before the window is created
            static std::list<std::string> registeredClasses; // Contains the names of all window classes registered by this class
    };

    #define MAX_LPSZMENUNAME_LENGTH 256
    #define MAX_LPSZCLASSNAME_LENGTH 256

}

#endif