#ifndef UNDO_H
#define UNDO_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"
#include "Selections.h"

// Cast to traverse through undo items without reading type
struct UndoComponent
{
	void* next;
};


// Undo Components

	class UndoTile
	{
		public:
			UndoTile* next;
			u16 xc;
			u16 yc;
			u16 value;

			UndoTile(UndoTile* next, u16 xc, u16 yc, u16 value)
				: next(next), xc(xc), yc(yc), value(value) { }

		private:
			UndoTile(); // Disallow ctor
	};

	class UndoUnitCreate
	{
		public:
			UndoUnitCreate* next;
			u16 unitIndex;

			UndoUnitCreate(u16 unitIndex)
				: next(nullptr), unitIndex(unitIndex) { }
			UndoUnitCreate(void* next, u16 unitIndex)
				: next((UndoUnitCreate*)next), unitIndex(unitIndex) { }

		private:
			UndoUnitCreate(); // Disallow ctor
	};

	class UndoUnitDel
	{
		public:
			UndoUnitDel* next;
			u16 unitIndex;
			ChkUnit unit;

			UndoUnitDel(u16 unitIndex, ChkUnit* unitRef)
				: next(nullptr), unitIndex(unitIndex) { unit = (*unitRef); }
			UndoUnitDel(void* next, u16 unitIndex, ChkUnit* unitRef)
				: next((UndoUnitDel*)next), unitIndex(unitIndex) { unit = (*unitRef); }

		private:
			UndoUnitDel(); // Disallow ctor
	};

	class UndoUnitMove // Also used for undoing swaps
	{
		public:
			UndoUnitMove* next;
			u16 oldIndex; // Where it was moved from before undos
			u16 newIndex; // Where it was moved to before undos

			UndoUnitMove(u16 oldIndex, u16 newIndex)
				: next(nullptr), oldIndex(oldIndex), newIndex(newIndex) { }
			UndoUnitMove(UndoUnitMove* next, u16 oldIndex, u16 newIndex)
				: next(next), oldIndex(oldIndex), newIndex(newIndex) { }
			UndoUnitMove(void* next, u16 oldIndex, u16 newIndex)
				: next((UndoUnitMove*)next), oldIndex(oldIndex), newIndex(newIndex) { }

		private:
			UndoUnitMove(); // Disallow ctor
	};

	class UndoUnitChange
	{
		public:
			UndoUnitChange* next;
			u16 unitIndex;

			u8 field;
			/** Index of data field of unit property, can be
				used in the unitFieldSize[] and unitFieldLoc[]
				arrays to get the size of the data type and its
				distance from the start of the unit structure,
				respectively */

			u32 data;

			UndoUnitChange(UndoUnitChange* next, u16 unitIndex, u8 field, u32 data)
				: next(next), unitIndex(unitIndex), field(field), data(data) { }
			UndoUnitChange(u16 unitIndex, u8 field, u32 data)
				: next(nullptr), unitIndex(unitIndex), field(field), data(data) { }

		private:
			UndoUnitChange(); // Disallow ctor
	};

	class UndoUnitMoveToHeader
	{
		public:
			void* next;
			u16 numDeletes;
			u16 numCreates;

			UndoUnitMoveToHeader(u16 numDeletes, u16 numCreates)
				: next(nullptr), numDeletes(numDeletes), numCreates(numCreates) { }
			UndoUnitMoveToHeader(void* next, u16 numDeletes, u16 numCreates)
				: next(next), numDeletes(numDeletes), numCreates(numCreates) { }

		private:
			UndoUnitMoveToHeader(); // Disallow ctor
	};

	class UndoLocationCreate
	{
		public:
			UndoLocationCreate* next;
			u16 locationIndex;

			UndoLocationCreate(u16 locationIndex)
				: next(nullptr), locationIndex(locationIndex) { }
			UndoLocationCreate(void* next, u16 locationIndex)
				: next((UndoLocationCreate*)next), locationIndex(locationIndex) { }

		private:
			UndoLocationCreate(); // Disallow ctor
	};

	class UndoLocationDel
	{
		public:
			UndoLocationDel* next;
			u16 locationIndex;
			ChkLocation location;
			bool isExtended;
			std::string locationName;

			UndoLocationDel(u16 locationIndex, ChkLocation* locationRef, bool isExtended, std::string locName)
				: next(nullptr), locationIndex(locationIndex), isExtended(isExtended), locationName(locName)
			{ location = (*locationRef); }

			UndoLocationDel(void* next, u16 locationIndex, ChkLocation* locationRef, bool isExtended, std::string locName)
				: next((UndoLocationDel*)next), locationIndex(locationIndex), isExtended(isExtended), locationName(locName)
			{ location = (*locationRef); }

		private:
			UndoLocationDel(); // Disallow ctor
	};

	class UndoLocationMove
	{
		public:
			UndoLocationMove* next;
			u16 locationIndex;
			s32 xChange;
			s32 yChange;

			UndoLocationMove(u16 locationIndex, s32 xChange, s32 yChange)
				: next(nullptr), locationIndex(locationIndex), xChange(xChange), yChange(yChange) { }

			UndoLocationMove(void* next, u16 locationIndex, s32 xChange, s32 yChange)
				: next((UndoLocationMove*)next), locationIndex(locationIndex), xChange(xChange), yChange(yChange) { }

		private:
			UndoLocationMove(); // Disallow ctor
	};

	class UndoLocationChange
	{
		public:
			UndoLocationChange* next;
			u16 locationIndex;

			u8 field;
			
			u32 data;

			UndoLocationChange(UndoLocationChange* next, u16  locationIndex, u8 field, u32 data)
				: next(next), locationIndex(locationIndex), field(field), data(data) { }
			UndoLocationChange(u16 locationIndex, u8 field, u32 data)
				: next(nullptr), locationIndex(locationIndex), field(field), data(data) { }

		private:
			UndoLocationChange(); // Disallow ctor
	};

//////////////////

class UndoNode
{
	public:
		void* head; // Stack of items that get replaced with this undo
		UndoNode* next; // The next undo in 
		UndoNode* prev;
		u32 flags;
			// Type of undo contained in this node
			#define UNDO_TERRAIN		 BIT_0
			#define UNDO_UNIT_CREATE	 BIT_1
			#define UNDO_UNIT_DEL		 BIT_2
			#define UNDO_UNIT_SWAP		 BIT_3
			#define UNDO_UNIT_MOVE		 BIT_4
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

		UndoNode() : head(nullptr), next(nullptr), prev(nullptr), flags(0) { }
		~UndoNode();
};

//////////

class UNDOS
{
	public:

		UNDOS();
		~UNDOS();

		bool clipUndos(u32 type);
		bool clipRedos(u32 type);

		bool startNext(u32 type);
		
		bool addUndoTile(u16 xc, u16 yc, u16 value);
		bool addUndoUnitDel(u16 index, ChkUnit* unitRef);
		bool addUndoUnitCreate(u16 index);
		bool addUndoUnitSwap(u16 oldIndex, u16 newIndex);
		bool addUndoUnitMove(u16 oldIndex, u16 newIndex);
		bool addUndoUnitChange(u16 index, u8 field, u32 data);
		bool addUndoUnitMoveToHeader(u16 numDeletes, u16 numCreates);

		bool addUndoLocationCreate(u16 index);
		bool addUndoLocationDel(u16 index, ChkLocation* locationRef, bool isExtended, std::string locName);
		bool addUndoLocationMove(u16 locationIndex, s32 xChange, s32 yChange);
		bool addUndoLocationChange(u16 locationIndex, u8 field, u32 data);

		void doUndo(u32 type, Scenario* chk, SELECTIONS& sel);
		void doRedo(u32 type, Scenario* chk, SELECTIONS& sel);

		void flushRoots();
		void setMapId(u16 mapId);


	protected:

		void flipNode(u32 flags, Scenario* chk, SELECTIONS& sel, UndoNode* node);

		bool addToUndo(u32 type, void* undoInfo);

		UndoNode* popUndo(u32 type);
		UndoNode* popRedo(u32 type);

		bool clip(UndoNode* track, u32 type);

		void deleteNode(UndoNode* node);
		void removeNode(UndoNode* node);


	private:
		
		UndoNode* headUndo;
		UndoNode* lastUndo;
		UndoNode* headRedo;
		UndoNode* upcomingUndo; // The undo soon to be added

		u32 rootTypes;
		u16 mapId; // Id of the map this instance of UNDOS is attached to
};

#endif