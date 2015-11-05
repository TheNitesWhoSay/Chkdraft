#ifndef UNDO_H
#define UNDO_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"
#include "IObserveUndos.h"
#include "Reversibles.h"
#include "Selections.h"
#include <map>


// Cast to traverse through undo items without reading type
class UndoComponent
{
	public:
		void* next;
		UndoComponent(void* next) : next(next) { }

	private:
		UndoComponent(); // Disallow ctor
};


// Undo Components

	class UndoTile : public UndoComponent
	{
		public:
			u16 xc;
			u16 yc;
			u16 value;

			UndoTile(UndoTile* next, u16 xc, u16 yc, u16 value)
				: UndoComponent(next), xc(xc), yc(yc), value(value) { }

		private:
			UndoTile(); // Disallow ctor
	};

	class UndoUnitCreate : public UndoComponent
	{
		public:
			u16 unitIndex;

			UndoUnitCreate(u16 unitIndex)
				: UndoComponent(nullptr), unitIndex(unitIndex) { }
			UndoUnitCreate(void* next, u16 unitIndex)
				: UndoComponent((UndoUnitCreate*)next), unitIndex(unitIndex) { }

		private:
			UndoUnitCreate(); // Disallow ctor
	};

	class UndoUnitDel : public UndoComponent
	{
		public:
			u16 unitIndex;
			ChkUnit unit;

			UndoUnitDel(u16 unitIndex, ChkUnit* unitRef)
				: UndoComponent(nullptr), unitIndex(unitIndex) { unit = (*unitRef); }
			UndoUnitDel(void* next, u16 unitIndex, ChkUnit* unitRef)
				: UndoComponent((UndoUnitDel*)next), unitIndex(unitIndex) { unit = (*unitRef); }

		private:
			UndoUnitDel(); // Disallow ctor
	};

	class UndoUnitMove : public UndoComponent // Also used for undoing swaps
	{
		public:
			u16 oldIndex; // Where it was moved from before undos
			u16 newIndex; // Where it was moved to before undos

			UndoUnitMove(u16 oldIndex, u16 newIndex)
				: UndoComponent(nullptr), oldIndex(oldIndex), newIndex(newIndex) { }
			UndoUnitMove(UndoUnitMove* next, u16 oldIndex, u16 newIndex)
				: UndoComponent(next), oldIndex(oldIndex), newIndex(newIndex) { }
			UndoUnitMove(void* next, u16 oldIndex, u16 newIndex)
				: UndoComponent((UndoUnitMove*)next), oldIndex(oldIndex), newIndex(newIndex) { }

		private:
			UndoUnitMove(); // Disallow ctor
	};

	class UndoUnitChange : public UndoComponent
	{
		public:
			u16 unitIndex;
			u8 field;
			/** Index of data field of unit property, can be
				used in the unitFieldSize[] and unitFieldLoc[]
				arrays to get the size of the data type and its
				distance from the start of the unit structure,
				respectively */
			u32 data;

			UndoUnitChange(UndoUnitChange* next, u16 unitIndex, u8 field, u32 data)
				: UndoComponent(next), unitIndex(unitIndex), field(field), data(data) { }
			UndoUnitChange(u16 unitIndex, u8 field, u32 data)
				: UndoComponent(nullptr), unitIndex(unitIndex), field(field), data(data) { }

		private:
			UndoUnitChange(); // Disallow ctor
	};

	class UndoUnitMoveToHeader : public UndoComponent
	{
		public:
			u16 numDeletes;
			u16 numCreates;

			UndoUnitMoveToHeader(u16 numDeletes, u16 numCreates)
				: UndoComponent(nullptr), numDeletes(numDeletes), numCreates(numCreates) { }
			UndoUnitMoveToHeader(void* next, u16 numDeletes, u16 numCreates)
				: UndoComponent(next), numDeletes(numDeletes), numCreates(numCreates) { }

		private:
			UndoUnitMoveToHeader(); // Disallow ctor
	};

	class UndoLocationCreate : public UndoComponent
	{
		public:
			u16 locationIndex;

			UndoLocationCreate(u16 locationIndex)
				: UndoComponent(nullptr), locationIndex(locationIndex) { }
			UndoLocationCreate(void* next, u16 locationIndex)
				: UndoComponent((UndoLocationCreate*)next), locationIndex(locationIndex) { }

		private:
			UndoLocationCreate(); // Disallow ctor
	};

	class UndoLocationDel : public UndoComponent
	{
		public:
			u16 locationIndex;
			ChkLocation location;
			bool isExtended;
			std::string locationName;

			UndoLocationDel(u16 locationIndex, ChkLocation* locationRef, bool isExtended, std::string locName)
				: UndoComponent(nullptr), locationIndex(locationIndex), isExtended(isExtended), locationName(locName)
			{ location = (*locationRef); }

			UndoLocationDel(void* next, u16 locationIndex, ChkLocation* locationRef, bool isExtended, std::string locName)
				: UndoComponent((UndoLocationDel*)next), locationIndex(locationIndex), isExtended(isExtended), locationName(locName)
			{ location = (*locationRef); }

		private:
			UndoLocationDel(); // Disallow ctor
	};

	class UndoLocationMove : public UndoComponent
	{
		public:
			u16 locationIndex;
			s32 xChange;
			s32 yChange;

			UndoLocationMove(u16 locationIndex, s32 xChange, s32 yChange)
				: UndoComponent(nullptr), locationIndex(locationIndex), xChange(xChange), yChange(yChange) { }

			UndoLocationMove(void* next, u16 locationIndex, s32 xChange, s32 yChange)
				: UndoComponent((UndoLocationMove*)next), locationIndex(locationIndex), xChange(xChange), yChange(yChange) { }

		private:
			UndoLocationMove(); // Disallow ctor
	};

	class UndoLocationChange : public UndoComponent
	{
		public:
			u16 locationIndex;
			u8 field;
			u32 data;

			UndoLocationChange(UndoLocationChange* next, u16  locationIndex, u8 field, u32 data)
				: UndoComponent(next), locationIndex(locationIndex), field(field), data(data) { }
			UndoLocationChange(u16 locationIndex, u8 field, u32 data)
				: UndoComponent(nullptr), locationIndex(locationIndex), field(field), data(data) { }

		private:
			UndoLocationChange(); // Disallow ctor
	};

//////////////////

class UndoNode
{
	public:
		void* head; // Stack of items that get replaced with this undo
		u32 flags;
			// Type of undo contained in this node
			#define UNDO_TERRAIN		 BIT_0
			#define UNDO_UNIT_CREATE	 BIT_1
			#define UNDO_UNIT_DEL		 BIT_2
			//#define UNDO_UNIT_SWAP		 BIT_3
			//#define UNDO_UNIT_MOVE		 BIT_4
			#define UNDO_UNIT_CHANGE	 BIT_5
			#define UNDO_LOCATION_CREATE BIT_6
			#define UNDO_LOCATION_DEL	 BIT_7
			#define UNDO_LOCATION_MOVE	 BIT_8
			#define UNDO_LOCATION_CHANGE BIT_9

			// Other
			#define UNDO_CHANGE_ROOT	 BIT_31

			// Combo's/Extractors
			#define UNDO_UNIT_MOVETO (UNDO_UNIT_CREATE|UNDO_UNIT_DEL)
			#define UNDO_UNIT (UNDO_UNIT_CREATE|UNDO_UNIT_DEL|UNDO_UNIT_SWAP|UNDO_UNIT_MOVE|UNDO_UNIT_CHANGE)
			#define UNDO_LOCATION (UNDO_LOCATION_CREATE|UNDO_LOCATION_DEL|UNDO_LOCATION_MOVE|UNDO_LOCATION_CHANGE)
			#define ALL_UNDO_REDOS	  0xFFFFFFFF
			#define UNDO_TYPE		  0x7FFFFFFF

		UndoNode() : head(nullptr), flags(0) { }
		~UndoNode();
};

#define UndoPtr std::shared_ptr<UndoNode>
#define ReversiblePtr std::shared_ptr<Reversible>

//////////

class UNDOS
{
	public:

		//UNDOS(void* thisMap);
		UNDOS(IObserveUndos &observer);
		~UNDOS();

		void AddUndo(ReversiblePtr action);
		//bool submitUndo(); // Submits all additions to the current undo
		//bool startComboUndo(u32 type); // Call prior to building an undo if it uses multiple base types
		//void doUndo(u32 type, Scenario* chk, SELECTIONS& sel);
		void doUndo(int32_t type, void *obj);
		//void doRedo(u32 type, Scenario* chk, SELECTIONS& sel);
		void doRedo(int32_t type, void *obj);
		void ResetChangeCount(); // Does not trigger notifications
		//void flushRoots();

		/*bool addUndoTile(u16 xc, u16 yc, u16 value);

		bool addUndoUnitDel(u16 index, ChkUnit* unitRef);
		bool addUndoUnitCreate(u16 index);
		bool addUndoUnitSwap(u16 oldIndex, u16 newIndex);
		bool addUndoUnitMove(u16 oldIndex, u16 newIndex);
		bool addUndoUnitChange(u16 index, u8 field, u32 data);
		bool addUndoUnitMoveToHeader(u16 numDeletes, u16 numCreates);

		bool addUndoLocationCreate(u16 index);
		bool addUndoLocationDel(u16 index, ChkLocation* locationRef, bool isExtended, std::string locName);
		bool addUndoLocationMove(u16 locationIndex, s32 xChange, s32 yChange);
		bool addUndoLocationChange(u16 locationIndex, u8 field, u32 data);*/


	protected:

		//bool startNext(u32 type);
		//void flipNode(u32 flags, Scenario* chk, SELECTIONS& sel, ReversiblePtr node);
		//bool addToUndo(u32 type, void* undoInfo);
		//ReversiblePtr popUndo(u32 type);
		ReversiblePtr popUndo(int32_t type);
		//ReversiblePtr popRedo(u32 type);
		ReversiblePtr popRedo(int32_t type);
		//bool clip(std::list<ReversiblePtr> &undoRedoList, int32_t type);
		//bool clipRedos(int32_t type);
		void AdjustChangeCount(int32_t type, int32_t adjustBy);

	private:
		
		IObserveUndos &observer;
		std::list<ReversiblePtr> undos; // front = next undo
		std::list<ReversiblePtr> redos; // front = next redo
		std::map<int32_t, int32_t> changeCounters; // <type, numChanges>, 1 addition/redo = +1 change, 1 undo = -1 change
		//std::list<UndoPtr> undos; // front = nextUndo, back = lastUndo
		//std::list<UndoPtr> redos; // front = nextRedo, back = lastRedo
		//UndoPtr upcomingUndo; // The undo soon to be added
		//u32 rootTypes; // All the undoable types that have been added since ctor/flushRoots
		//void* mapPointer; // Pointer to the map this instance of UNDOS belongs to
};

#endif