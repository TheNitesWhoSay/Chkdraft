#include "grid_item.h"

namespace WinLib {

    GridControlItem::GridControlItem() : selected(false), data(0), disabled(false)
    {
        text = "";
    }

    GridControlItem::~GridControlItem()
    {

    }

    void GridControlItem::SetText(const std::string & newText)
    {
        try { this->text = newText; }
        catch ( ... ) { }
    }

    void GridControlItem::SetText(int newNumber)
    {
        try { this->text = std::to_string(newNumber); }
        catch ( ... ) { }
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

    const std::string & GridControlItem::getText()
    {
        return this->text;
    }

    size_t GridControlItem::getTextLength()
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

}
