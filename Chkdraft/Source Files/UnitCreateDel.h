#ifndef UNITCREATEDEL_H
#define UNITCREATEDEL_H
#include "Reversibles.h"
#include "UndoTypes.h"

class UnitCreateDel : public ReversibleAction
{
	public:
		UnitCreateDel(u16 index, ChkUnit &unit); // Undo Deletion
		UnitCreateDel(u16 index); // Undo Creation
		virtual void Reverse(void *guiMap);
		virtual int32_t GetType();

	private:
		u16 index;
		std::unique_ptr<ChkUnit> unit; // If creation, is a nullptr
};

#endif