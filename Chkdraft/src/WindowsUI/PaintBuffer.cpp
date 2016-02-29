#include "PaintBuffer.h"

PaintBuffer::PaintBuffer() : width(0), height(0), memDc(NULL), parentDc(NULL), memBitmap(NULL)
{

}

PaintBuffer::~PaintBuffer()
{
    Clear();
}

void PaintBuffer::Clear()
{
    if ( memBitmap != NULL )
        DeleteObject(memBitmap);

    if ( memDc != NULL )
        DeleteDC(memDc);

    width = 0;
    height = 0;

    parentDc = NULL;
}

HDC PaintBuffer::GetPaintDc()
{
    return memDc;
}

HBITMAP PaintBuffer::GetPaintBitmap()
{
    return memBitmap;
}

bool PaintBuffer::SetSize(HDC parentDc, int bufferWidth, int bufferHeight)
{
    if ( PaintBuffer::parentDc == parentDc && PaintBuffer::width == bufferWidth && PaintBuffer::height == bufferHeight )
        return true;

    Clear();
    PaintBuffer::width = bufferWidth;
    PaintBuffer::height = bufferHeight;
    PaintBuffer::parentDc = parentDc;
    memDc = CreateCompatibleDC(parentDc);
    if ( memDc != NULL )
    {
        memBitmap = CreateCompatibleBitmap(parentDc, width, height);
        if ( memBitmap != NULL )
        {
            HGDIOBJ result = SelectObject(memDc, memBitmap);
            return result != NULL && result != HGDI_ERROR;
        }
    }
    Clear();
    return false;
}

bool PaintBuffer::CopyTo(HDC hdcDest)
{
    return BitBlt(hdcDest, 0, 0, width, height, memDc, 0, 0, SRCCOPY) != 0;
}
