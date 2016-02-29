#ifndef MAPFILE_H
#define MAPFILE_H
#include "Scenario.h"
#include <cstdio>
#include <time.h>

enum class SaveType;
class MapFile;
using MapFilePtr = std::shared_ptr<MapFile>;

class MapFile : public Scenario // MapFile is a scenario file + the stuff here
{
	public:
		MapFile();

        virtual bool SaveFile(bool SaveAs);

		bool LoadFile(const char* &path); // If you're not providing a path, pass in nullptr
		bool LoadFile();

		bool GetPath();
		bool SetPath(const char* newPath);
		const char* FilePath();
		void SetSaveType(SaveType newSaveType);

		std::string GetFileName();
		std::string GetFilePath();

	protected:
		bool OpenFile();

	private:
		char filePath[FILENAME_MAX];
		SaveType saveType;
};

/** The types of files a map can be saved as, one
    through the number of selectable save types should
    correspond to the selectable types in save dialogs */
enum class SaveType
{
    CustomFilter = 0,
    StarCraftScm = 1,
    HybridScm = 2,
    ExpansionScx = 3,
    StarCraftChk = 4,
    HybridChk = 5,
    ExpansionChk = 6,
    AllMaps = 7,
    Unknown // Have this higher than all other SaveTypes
};

#endif