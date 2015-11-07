#ifndef UNITINDEXMOVE_H
#define UNITINDEXMOVE_H
#include "Reversibles.h"
#include "UndoTypes.h"

class UnitIndexMove : public ReversibleAction
{
	public:
		UnitIndexMove(u16 oldIndex, u16 newIndex);
		virtual void Reverse(void *guiMap);
		virtual int32_t GetType();

	private:
		u16 oldIndex;
		u16 newIndex;
};

class UnitIndexMoveBoundary : public ReversibleAction
{
	public:
		UnitIndexMoveBoundary();
		virtual void Reverse(void *guiMap);
		virtual int32_t GetType();
};

#endif