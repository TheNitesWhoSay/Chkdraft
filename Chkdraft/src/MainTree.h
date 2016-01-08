#ifndef MAINTREE_H
#define MAINTREE_H
#include "Windows UI/WindowsUI.h"
#include "Mapping Core/MappingCore.h"
#include "UnitTree.h"
#include "LocationTree.h"

class MainTree : public TreeViewControl
{
	public:

		UnitTree unitTree;
		LocationTree locTree;

		void BuildMainTree();


	private:

};

#endif