#include "GridItem.h"

GridControlItem::GridControlItem() : selected(false), data(0)
{
	text = "";
}

void GridControlItem::SetText(const char* text)
{
	try { this->text = text; }
	catch ( std::exception ) { }
}

void GridControlItem::SetText(int text)
{
	try
	{
		this->text = std::to_string(text);
	}
	catch ( std::exception ) { }
}

void GridControlItem::SetData(u32 data)
{
	this->data = data;
}

void GridControlItem::SetSelected(bool selected)
{
	this->selected = selected;
}

void GridControlItem::Clear()
{
	selected = false;
	data = 0;
	text = "";
}

bool GridControlItem::getText(std::string &text)
{
	try { text = this->text; return true; }
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
