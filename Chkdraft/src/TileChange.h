#ifndef TILECHANGE_H
#define TILECHANGE_H
#include "Reversibles.h"
#include "UndoTypes.h"

class TileChange : public ReversibleAction
{
	public:
		TileChange(u16 xc, u16 yc, u16 tileValue);
		virtual void Reverse(void *guiMap);
		virtual int32_t GetType();

	private:
		u16 xc;
		u16 yc;
		u16 tileValue;
};

#endif