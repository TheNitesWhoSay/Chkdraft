#include "GridItem.h"

GridControlItem::GridControlItem() : selected(false), data(0), disabled(false)
{
    text = "";
}

void GridControlItem::SetText(const char* newText)
{
    try { this->text = newText; }
    catch ( std::exception ) { }
}

void GridControlItem::SetText(int newNumber)
{
    try { this->text = std::to_string(newNumber); }
    catch ( std::exception ) { }
}

void GridControlItem::SetData(u32 newData)
{
    this->data = newData;
}

void GridControlItem::SetSelected(bool isItemSelected)
{
    this->selected = isItemSelected;
}

void GridControlItem::SetDisabled(bool disabled)
{
    this->disabled = disabled;
}

void GridControlItem::Clear()
{
    selected = false;
    data = 0;
    text = "";
}

bool GridControlItem::getText(std::string &outText)
{
    try { outText = this->text; return true; }
    catch ( std::exception ) { return false; }
}

int GridControlItem::getTextLength()
{
    return text.length();
}

int GridControlItem::getTextNum()
{
    return std::stoi(text);
}

u32 GridControlItem::getData()
{
    return data;
}

bool GridControlItem::isSelected()
{
    return selected;
}

bool GridControlItem::isDisabled()
{
    return disabled;
}
