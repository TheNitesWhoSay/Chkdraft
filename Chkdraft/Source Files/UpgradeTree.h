#ifndef UPGRADETREE_H
#define UPGRADETREE_H
#include "Windows UI/WindowsUI.h"

class UpgradeTree : public TreeViewControl
{
	public:
		bool CreateThis(HWND hParent, int x, int y, int width, int height, bool hasButtons, u32 id);

	private:
		void InsertUpgrades();

};

#endif