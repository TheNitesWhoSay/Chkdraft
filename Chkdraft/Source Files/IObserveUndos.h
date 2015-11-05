#ifndef IOBSERVEUNDOS_H
#define IOBSERVEUNDOS_H
#include "Reversibles.h"

class IObserveUndos
{
	public:
		/** Sent whenever a change is made, provided it does
			not meet the criteria for ChangesReversed */
		virtual void ChangesMade();

		/** Sent if all changes to unique types have been reversed
			since load or since the last call to ResetChangeCount */
		virtual void ChangesReversed();
};

#endif