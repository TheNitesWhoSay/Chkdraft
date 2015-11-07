#ifndef WINDOWSITEM_H
#define WINDOWSITEM_H
#include <Windows.h>
#include <string>
#include <list>

/**
	A class encapsulating an 'HWND' and various
	windows functions that act on such items
*/
class WindowsItem
{
	public:
		WindowsItem();

		bool operator==(HWND hWnd); // Tests whether the encapsulated handle equals this handle
		HWND getHandle();

		bool getWindowRect(RECT &rect);
		int Width();
		int Height();
		int Left();
		int Top();
		int Right();
		int Bottom();
		bool getClientRect(RECT &rect);
		LONG cliWidth();
		LONG cliHeight();
		HDC getDC(); // Gets the current device context
		bool isEnabled();

		void SetFont(HFONT font, bool redrawImmediately);
		void ReplaceChildFonts(HFONT hFont);
		void LockCursor(); // Prevents cursor from leaving this window
		void UnlockCursor(); // Globally unlocks the cursor
		void TrackMouse(DWORD hoverTime); // Causes mouse tracking messages to be sent to this window
		bool SetParent(HWND hParent);
		void SetRedraw(bool autoRedraw);
		void RedrawThis();

		void MoveTo(int x, int y);
		void SetPos(int x, int y, int width, int height);
		void ShowNormal();
		void Show();
		void Hide();
		void SetSmallIcon(HANDLE hIcon);
		bool SetTitle(const char* newTitle);

		bool ReleaseDC(HDC hDC);
		void FocusThis();
		void UpdateWindow();
		void DisableThis();
		void EnableThis();
		void SetWidth(int newWidth);

	protected:
		virtual void setHandle(HWND hWnd);

		/** Attempts to register a class for creating the window
		If the class has been registered before this returns false
		Do not register different classes with the same name */
		bool RegisterWindowClass(UINT style, HICON hIcon, HCURSOR hCursor, HBRUSH hbrBackground,
			LPCTSTR lpszMenuName, LPCTSTR lpszClassName, HICON hIconSm, WNDPROC wndProc);

		/** Returns whether the window class given by lpszClassName is registered,
		if it is, then this class name will be set to create the window with */
		bool WindowClassIsRegistered(LPCTSTR lpszClassName);

		/** This method is used to compare list view items for windows sort functions
			ForwardCompareLvItems must be set as the function and the class pointer as LPARAM */
		virtual int CompareLvItems(LPARAM index1, LPARAM index2);

		/** This method returns the value given by CompareLvItems with the same parameters
			This should be passed as the proc to window's built in sort items functions */
		static int CALLBACK ForwardCompareLvItems(LPARAM index1, LPARAM index2, LPARAM lParam);

		std::string &WindowClassName();

	private:
		HWND hWnd; // Handle to the window this class encapsulates
		std::string windowClassName; // May contain the window class title before the window is created
		static std::list<std::string> registeredClasses; // Contains the names of all window classes registered by this class
};

#endif