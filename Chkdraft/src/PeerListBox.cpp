#include "PeerListBox.h"

PeerListBox::PeerListBox() : hPeer(NULL)
{
    
}

PeerListBox::~PeerListBox()
{

}

void PeerListBox::SetPeer(HWND hPeer)
{
    this->hPeer = hPeer;
}

LRESULT PeerListBox::ControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if ( msg == WM_MOUSEWHEEL && hPeer != NULL )
        SendMessage(hPeer, WM_MOUSEWHEEL, wParam, lParam);
    
    return ListBoxControl::ControlProc(hWnd, msg, wParam, lParam);
}
