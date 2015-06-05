#include "GridItem.h"

GridControlItem::GridControlItem() : selected(false), data(0)
{

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
		char sNum[12];
		_itoa_s(text, sNum, 10);
		this->text = sNum;
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

bool GridControlItem::getText(std::string &text)
{
	try { text = this->text; return true; }
	catch ( std::exception ) { return false; }
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
