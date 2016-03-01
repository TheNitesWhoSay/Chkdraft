#ifndef MDICLIENT_H
#define MDICLIENT_H
#include "WindowsItem.h"

class MdiClient : public WindowsItem
{
    public:
        MdiClient();
        bool CreateMdiClient( HANDLE hWindowMenu, UINT idFirstChild, DWORD dwStyle,
                             int X, int Y, int nWidth, int nHeight, HWND hParent, HMENU hMenu );
        HWND getActive();
        void cascade();
        void tileHorizontally();
        void tileVertically();
        void destroyActive();
        void nextMdi();
};

#endif