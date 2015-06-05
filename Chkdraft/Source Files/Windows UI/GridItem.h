#ifndef GRIDITEM_H
#define GRIDITEM_H
#include "DataTypes.h"

class GridControlItem
{
	public:
		GridControlItem();
		void SetText(const char* text);
		void SetText(int text);
		void SetData(u32 data);
		void SetSelected(bool selected);
		void ToggleSelection();

		bool getText(std::string &text);
		int getTextNum();
		u32 getData();
		bool isSelected();

	protected:

	private:
		std::string text;
		bool selected;
		u32 data;
};

#endif