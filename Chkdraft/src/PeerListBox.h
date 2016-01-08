#ifndef PEERLISTBOX_H
#define PEERLISTBOX_H
#include "Windows UI/WindowsUI.h"

/** A simple list box extension that allows certain
	messages (only WM_MOUSESCROLL at the moment) to
	be forwarded to the designated window/control */
class PeerListBox : public ListBoxControl
{
	public:
		PeerListBox();
		void SetPeer(HWND hPeer);

	protected:
		virtual LRESULT ControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:
		HWND hPeer;
};

#endif