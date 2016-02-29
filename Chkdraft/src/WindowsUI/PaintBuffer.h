#ifndef PAINTBUFFER_H
#define PAINTBUFFER_H
#include <Windows.h>

class PaintBuffer
{
    public:
        PaintBuffer();
        ~PaintBuffer();
        void Clear(); // Can call to save memory, but this will automatically be done upon resize/destructor
        HDC GetPaintDc();
        HBITMAP GetPaintBitmap();
        bool SetSize(HDC parentDc, int bufferWidth, int bufferHeight);
        bool CopyTo(HDC hdcDest);

    private:
        int width;
        int height;
        HDC memDc;
        HDC parentDc;
        HBITMAP memBitmap;
};

#endif