#ifndef MAPFILE_H
#define MAPFILE_H
#include "Scenario.h"
#include <cstdio>

class MapFile : public Scenario // MapFile is a scenario file + the stuff here
{
	public:
		MapFile();

		bool LoadFile(const char* &path); // If you're not providing a path, pass in nullptr
		bool LoadFile();

		bool SaveFile(bool SaveAs);
		bool GetPath();
		
		const char* FilePath();

	protected:
		bool OpenFile();

	private:
		char filePath[FILENAME_MAX];
		u8 SaveType;
};

#endif