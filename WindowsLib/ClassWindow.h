#ifndef WINDOWCLASS_H
#define WINDOWCLASS_H
#include "DataTypes.h"
#include "WindowsItem.h"
#include "WinUIEnums.h"
#include "MdiClient.h"
#include <Windows.h>
#include <string>
#include <list>

namespace WinLib {

    /** A class wrapper for generic windows, purposed to forward
        message handling to the WndProc method inside descendant classes
        GWL_USERDATA and GWL_WNDPROC (or DWL_USER and DWL_DLGPROC for
        dialog windows) are used and modified by this class and should
        not be modified elsewhere */
    class ClassWindow : public WindowsItem
    {
        public:

    /* Constructor  */  ClassWindow();

    /*  Destructor  */  virtual ~ClassWindow();

        protected:

    /*     Setup    */  /** Attempts to register a class for creating the window
                            If the class has been registered before, this returns false
                            Do not register different classes with the same name */
                        bool RegisterWindowClass(UINT style, HICON hIcon, HCURSOR hCursor, HBRUSH hbrBackground,
                            u64 menuResourceId, const std::string & lpszClassName, HICON hIconSm, bool isMDIChild);

                        /** Attemps to create a window
                            The variables are used the same as in CreateWindowEx */
                        bool CreateClassWindow( DWORD dwExStyle, const std::string & lpWindowName, DWORD dwStyle,
                                                int x, int y, int nWidth, int nHeight,
                                                HWND hWndParent, HMENU hMenu );

                        void DestroyThis();

                        /** Attempts to create a window as an MDI child
                            The variables are used the same as in CreateMDIWindow */
                        bool CreateMdiChild( const std::string & windowName, DWORD dwStyle,
                                             int x, int y, int nWidth, int nHeight,
                                             HWND hParent );

                        /** This method attempts to turn the current window into a MDI frame
                            by creating a client window (out of MdiClient) and changing message
                            handling so DefFrameProc is called for the new client window */
                        bool BecomeMDIFrame(MdiClient & client, HANDLE hWindowMenu, UINT idFirstChild, DWORD dwStyle,
                            int X, int Y, int nWidth, int nHeight, HMENU hMenu);

    /*  Overridden  */  /** This method is called when WM_NOTIFY is sent to the
                            window, override this to respond to notifications */
                        virtual LRESULT Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr);

                        /** When overidden, these methods serve as easy ways to process
                        notifications/commands */
                        virtual void NotifyTreeItemSelected(LPARAM newValue); // Sent when a tree item is selected, or an already-selected item is re-clicked
                        virtual void NotifyButtonClicked(int idFrom, HWND hWndFrom); // Sent when a button or checkbox is clicked
                        virtual void NotifyEditUpdated(int idFrom, HWND hWndFrom); // Sent when edit text changes, before redraw
                        virtual void NotifyEditFocused(int idFrom, HWND hWndFrom); // Sent when an edit box receives focus
                        virtual void NotifyEditFocusLost(int idFrom, HWND hWndFrom); // Sent when focus changes or the window is hidden
                        virtual void NotifyComboSelChanged(int idFrom, HWND hWndFrom); // Sent when combo or listbox is changed by selection
                        virtual void NotifyComboEditUpdated(int idFrom, HWND hWndFrom); // Sent when combo is changed by text edits
                        virtual void NotifyComboEditFocused(int idFrom, HWND hWndFrom); // Sent when a combos edit box is focused
                        virtual void NotifyComboEditFocusLost(int idFrom, HWND hWndFrom); // Sent when focus changes or the window is hidden
                        virtual void NotifyWindowHidden(); // Sent when the window is hidden
                        virtual void NotifyWindowShown(); // Sent when the window is shown

                        virtual void HandleDroppedFile(const std::string & dropFilePath);

                        /** This method is called when WM_COMMAND is sent to the
                            window, override this to respond to commands */
                        virtual LRESULT Command(HWND hWnd, WPARAM wParam, LPARAM lParam);

                        /** This method is where all windows messages are processed
                            Override this in descendant classes to handle window messages */
                        virtual LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


        private:
            
    /*     Data     */  HWND hWndMDIClient; // Handle to the MDI client if applicable, NULL otherwise
                        enum class WindowType { None, Regular, MDIFrame, MDIChild, Dialog };
                        WindowType windowType; // Specifies what type of window this is, see enumerated WindowTypes
                        bool allowEditNotify; // Used to prevent edit update recursion
                        WNDPROC defaultProc; // Stores the default proc for the encapsulated window

    /*   Internal   */  /* Calls NotifyEditUpdated and ensures the function is not called again until it returns */
                        void SendNotifyEditUpdated(int idFrom, HWND hWndFrom);

                        /** This method is used until WM_NCCREATE is handled, at which point
                            ForwardWndProc is used to forward window messages to the WndProc method */
                        static LRESULT CALLBACK SetupWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

                        /** This method is used by MDIChild windows until WM_NCCREATE is handled, at which point
                            ForwardWndProc is used to forward window messages to the WndProc method */
                        static LRESULT CALLBACK SetupMDIChildProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

                        /** Attempts to reset the encapsulated windows's message handling to the
                            original method; used to prevent calls to virtual methods during destruction */
                        bool ResetProc();

                        /** This method simply returns the value given by WndProc with the same parameters
                            This replaces SetupWndProc to remove the overhead of checking for WM_NCCREATE
                                and ensuring that the class pointer is set */
                        static LRESULT CALLBACK ForwardWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    };

}

#endif