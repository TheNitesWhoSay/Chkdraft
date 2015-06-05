#ifndef WINDOWCLASS_H
#define WINDOWCLASS_H
#include "DataTypes.h"
#include <Windows.h>
#include <string>
#include <list>

class ClassWindow
/** A class wrapper for generic windows, purposed to forward
	message handling to the WndProc method inside descendant classes
	GWL_USERDATA and GWL_WNDPROC (or DWL_USER and DWL_DLGPROC for
	dialog windows) are used and modified by this class and should
	not be modified elsewhere */
{
	public:

/* Constructor  */	ClassWindow();

/*  Destructor  */	~ClassWindow(); 

/*  Accessors   */	HWND getHandle(); // Accessor for the window handle
					bool getWindowRect(RECT &rect);
					bool getClientRect(RECT &rect);
					LONG cliWidth();
					LONG cliHeight();
					bool operator==(HWND hWnd); // Tests whether the encapsulated handle equals this handle

/*	 Mutators	*/	void ReplaceChildFonts(HFONT hFont);
					void LockCursor(); // Prevents cursor from leaving this window
					void UnlockCursor(); // Globally unlocks the cursor
					void TrackMouse(DWORD hoverTime); // Causes mouse tracking messages to be sent to this window
					bool SetParent(HWND hParent);

					void MoveTo(int x, int y);
					void SetPos(int x, int y, int width, int height);
					void ShowNormal();
					void Show();
					void Hide();
					void SetSmallIcon(HANDLE hIcon);
					bool SetTitle(const char* newTitle);

	protected:

/*   Setup    */	/** Attempts to register a class for creating the window
						If the class has been registered before this returns false
						Do not register different classes with the same name */
					bool RegisterWindowClass( UINT style, HICON hIcon, HCURSOR hCursor, HBRUSH hbrBackground,
											  LPCTSTR lpszMenuName, LPCTSTR lpszClassName, HICON hIconSm, bool isMDIChild );

					/** Returns whether the window class given by lpszClassName is registered,
						if it is, then this class name will be set to create the window with */
					bool WindowClassIsRegistered(LPCTSTR lpszClassName);

					/** Attemps to create a window
						The variables are used the same as in CreateWindowEx */
					bool CreateClassWindow( DWORD dwExStyle, LPCSTR lpWindowName, DWORD dwStyle,
											int x, int y, int nWidth, int nHeight,
											HWND hWndParent, HMENU hMenu );

					/** Attempts to create a window as an MDI child
						The variables are used the same as in CreateMDIWindow */
					bool CreateMdiChild( LPCSTR lpWindowName, DWORD dwStyle,
										 int x, int y, int nWidth, int nHeight,
										 HWND hParent );

					/** Attempts to create a modeless dialog
						The variables are used the same as in CreateDialog */
					bool CreateModelessDialog(LPCSTR lpTemplateName, HWND hWndParent);

					/** Attempts to create a dialog box
						The variables and return value are used the same as in DialogBox */
					INT_PTR CreateDialogBox(LPCSTR lpTemplateName, HWND hWndParent);

					/** This method returns the value given by CompareLvItems with the same parameters
						This should be passed as the proc to window's built in sort items functions */
					static int CALLBACK ForwardCompareLvItems(LPARAM index1, LPARAM index2, LPARAM lParam);

					/** This method attempts to destroy an encapsulated modeless dialog */
					bool DestroyDialog();


	private:

/*     Data     */	static std::list<std::string> registeredClasses; // Contains the names of all window classes registered by this class
					HWND windowHandle; // Handle to the window this class encapsulates
					std::string windowClassName; // May contain the window class title before the window is created
					u32 windowType; // Specifies what type of window this is, see enumerated WindowTypes
					enum WindowTypes { None, Regular, MDIChild, ModelessDialog, Dialog };

/*  Overridden  */	/** Provides standard naming for class window create methods
						this will fail unless this method has been overriden */
					virtual bool CreateThis();

					/** Provides standard naming for class window destroy methods
						this will fail unless this method has been overriden */
					virtual bool DestroyThis();

					/** This method is where all windows messages are processed
						Override this in descendant classes to handle window messages */
					virtual LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

					/** This method is where all dialog messages are processed
						Override this in descendant classes to handle dialog messages */
					virtual BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

					/** This method is used to compare list view items for windows sort functions
						ForwardCompareLvItems must be set as the function and the class pointer as LPARAM */
					virtual int CompareLvItems(LPARAM index1, LPARAM index2);

/*   Internal   */	/** This method is used until WM_NCCREATE is handled, at which point
					    ForwardWndProc is used to forward window messages to the WndProc method */
					static LRESULT CALLBACK SetupWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

					/** This method is used by MDIChild windows until WM_NCCREATE is handled, at which point
						ForwardWndProc is used to forward window messages to the WndProc method */
					static LRESULT CALLBACK SetupMDIChildProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

					/** This method is used by Dialog windows until WM_INITDIALOG is handled, at which point
						ForwardDlgProc is used to forward messages to the WndProc method */
					static BOOL CALLBACK SetupDialogProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

					/** This method simply returns the value given by WndProc with the same parameters
						This replaces SetupWndProc to remove the overhead of checking for WM_NCCREATE
							and ensuring that the class pointer is set */
					static LRESULT CALLBACK ForwardWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

					/** This method simply returns the value given by DlgProc with the same parameters
						This replaces SetupDialogProc to remove the overhead of checking for WM_INITDIALOG
							and ensuring that the class pointer is set */
					static BOOL CALLBACK ForwardDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif