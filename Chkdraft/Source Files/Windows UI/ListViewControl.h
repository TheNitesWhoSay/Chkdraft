#ifndef LISTVIEWCONTROL_H
#define LISTVIEWCONTROL_H
#include "WindowControl.h"

class ListViewControl : public WindowControl
{
	public:
		bool CreateThis(HWND hParent, int x, int y, int width, int height, u32 id);
		void AddColumn(int insertAt, const char* title, int width, int alignmentFlags);
		void AddRow(int numColumns, LPARAM lParam);
		void RemoveRow(int rowNum);
		void ChangeLParam(LPARAM oldLParam, LPARAM newLParam);
		void SetItem(int row, int column, const char* text);
		void SetItem(int row, int column, int value);
		int GetItemRow(int lParam);
		void EnableFullRowSelect();

	private:

};

#endif