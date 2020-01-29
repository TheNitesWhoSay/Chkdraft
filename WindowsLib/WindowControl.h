#ifndef WINDOWCONTROL_H
#define WINDOWCONTROL_H
#include "DataTypes.h"
#include "WinUIEnums.h"
#include "WindowsItem.h"
#include <Windows.h>
#include <WindowsX.h>
#include <CommCtrl.h>
#include <string>
#include <algorithm>
#include <SimpleIcu.h>

namespace WinLib {

    class WindowControl : public WindowsItem
    {
        public:

    /* Constructor  */  WindowControl();

    /*  Destructor  */  virtual ~WindowControl(); // This will NOT remove the control, use "DestroyThis" or destroy the parent window to do so

    /*  Universal   */  /** Finds and encapsulates a control within an existing window
                            This may be useful for controls in resource-based dialogs */
                        virtual bool FindThis(HWND hParent, u32 controlId);
                        virtual bool FindThis(HWND controlHandle);

                        /** Attempts to destroy this control and reset the associated data */
                        bool DestroyThis();

        protected:

    /*    Setup     */  /** Attemps to create and encapsulate a control
                            ControlProc is used if redirectProc is set
                            The rest of the variables are used the same as in CreateWindowEx */
                        bool CreateControl( DWORD dwExStyle, const icux::uistring & lpClassName, const std::string & lpWindowName, DWORD dwStyle,
                                            int x, int y, int nWidth, int nHeight,
                                            HWND hWndParent, HMENU hMenu, bool redirectProc );

                        /** Attempts to redirect the encapsulated control's message handling to the
                            ControlProc method, which you may override to alter default behavior */
                        bool RedirectProc();

                        /** Attempts to reset the encapsulated control's message handling to the
                            original method; used to prevent calls to virtual methods during destruction */
                        bool ResetProc();

    /*  Overridden  */  /** This method is called when WM_NOTIFY is sent to the
                            control, override this to respond to notifications */
                        virtual LRESULT Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr);

                        /** This method is called when WM_COMMAND is sent to the
                            control, override this to respond to commands */
                        virtual LRESULT Command(HWND hWnd, WPARAM wParam, LPARAM lParam);

                        /** This method is where control messages are pre-processed,
                            Override this to handle control messages, return CallDefaultProc
                            to use the default behavior */
                        virtual LRESULT ControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


        private:

    /*     Data     */  //HWND controlHandle; // Handle to the control this class encapsulates
                        WNDPROC defaultProc; // Stores the default proc for the encapsulated control

    /*  Overridden  */  /** Provides standard naming for window control create methods,
                            if more or less arguments are needed you may not need to override
                            this in specific though the "bool CreateThis(args)" format is suggested
                            this will fail unless this method has been overriden */
                        virtual bool CreateThis(HWND hParent, int x, int y, int width, int height);

    /*   Internal   */  /** This method returns the value given by ControlProc with the same parameters */
                        static LRESULT CALLBACK ForwardControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    };

}

#endif