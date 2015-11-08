#include "MapFile.h"
#include "FileIO.h"
#include <cstdio>
#include <cstdarg>

MapFile::MapFile() : SaveType(0)
{
	filePath[0] = '\0';
}

bool MapFile::LoadFile(const char* &path)
{
	if ( path != nullptr )
		strcpy_s(filePath, MAX_PATH, path); // Copy the given path to the map's stored filePath
	else if ( GetPath() )
		path = filePath; // Have the pointer point to the new filePath

	return filePath != nullptr && OpenFile();
}

bool MapFile::LoadFile()
{
	return GetPath() && OpenFile();
}

bool MapFile::SaveFile(bool SaveAs)
{
	if ( isProtected() )
		MessageBox(NULL, "Cannot save protected maps!", "Error!", MB_OK|MB_ICONEXCLAMATION);
	else
	{
		if ( SaveAs || filePath[0] == '\0' ) // SaveAs specified or filePath not yet determined
		{
			OPENFILENAME ofn = GetScSaveOfn(filePath);
			if ( GetSaveFileName(&ofn) )
			{
				SaveType = (u8)ofn.nFilterIndex;

				char* ext = strrchr(filePath, '.'); // Find the last occurrence of '.'
				if ( ext == nullptr ) // No extension specified, need to add
				{
					if ( SaveType == 1 || SaveType == 2 )
						strcat_s(filePath, ".scm");
					else if ( SaveType == 3 || SaveType == 7 )
						strcat_s(filePath, ".scx");
					else if ( SaveType >= 4 && SaveType <= 6 )
						strcat_s(filePath, ".chk");
				}
				else // Extension specified, give it precedence over filterIndex
				{
					if ( strcmp(ext, ".chk") == 0 && SaveType < 4 )
						SaveType = 5;
					else if ( strcmp(ext, ".scm") == 0 && SaveType > 1 )
						SaveType = 2;
					else if ( strcmp(ext, ".scx") == 0 )
						SaveType = 3;
					else if ( SaveType == 7 )
						SaveType = 3;
				}
			}
		}

		if ( filePath[0] != '\0' ) // Map for sure has a path
		{
			FILE* pFile(nullptr);

			if ( SaveType == 1 || SaveType == 4 ) // StarCraft Map, edit to match
			{
				TYPE().overwrite("RAWS", 4);
				VER ().overwrite(";\0", 2);
				IVER().overwrite("\12\0", 2);
				IVE2().overwrite("\13\0", 2);

				if ( MRGN().size() > 1280 ) // If there's over 64 locations
					MRGN().delRange(1280, MRGN().size()); // Remove the extras
			}
			else if ( SaveType == 2 || SaveType == 5 ) // Hybrid Map, edit to match
			{
				TYPE().overwrite("RAWS", 4);
				VER ().overwrite("?\0", 2);
				IVER().overwrite("\12\0", 2);
				IVE2().overwrite("\13\0", 2);

				if ( MRGN().size() < 5100 ) // If there's < 255 locations
					MRGN().add<u8>(0, 5100-MRGN().size()); // Add space for 255
			}
			else if ( SaveType == 3 || SaveType == 6 || SaveType == 7 ) // BroodWar Map, edit to match
			{
				TYPE().overwrite("RAWB", 4);
				VER ().overwrite("Í\0", 2);
				RemoveSection(&IVER());
				IVE2().overwrite("\13\0", 2);

				if ( MRGN().size() < 5100 ) // If there's < 255 locations
					MRGN().add<u8>(0, 5100-MRGN().size()); // Add space for 255
			}

			if ( (SaveType > 0 && SaveType <= 3) || SaveType == 7 ) // Must be packed into an MPQ
			{
				fopen_s(&pFile, filePath, "wb");
				if ( pFile != nullptr )
				{
					fclose(pFile);
					HANDLE hMpq = NULL;
					DeleteFileA("chk.tmp"); // Remove any existing chk.tmp files
					fopen_s(&pFile, "chk.tmp", "wb");
					WriteFile(pFile);
					fclose(pFile);

					hMpq = MpqOpenArchiveForUpdate(filePath, MOAU_OPEN_ALWAYS|MOAU_MAINTAIN_LISTFILE, 16);
					if ( hMpq != NULL && hMpq != INVALID_HANDLE_VALUE )
					{
						BOOL addedFile = MpqAddFileToArchive(hMpq, "chk.tmp", "staredit\\scenario.chk", MAFA_COMPRESS | MAFA_REPLACE_EXISTING);
						MpqCloseUpdatedArchive(hMpq, NULL);

						if ( addedFile == TRUE )
						{
							DeleteFileA("chk.tmp");
							return true;
						}
						else
							MessageBox(NULL, "Failed to add file!", "Error!", MB_OK|MB_ICONEXCLAMATION);
					}
					else
						MessageBox(NULL, "Failed to open for updates!", "Error!", MB_OK|MB_ICONEXCLAMATION);

					DeleteFileA("chk.tmp");
				}
				else
					MessageBox(NULL, "Failed to open file!\n\nThe file may be in use elsewhere.", "Error!", MB_OK|MB_ICONEXCLAMATION);
			}
			else // Is a chk file or unrecognized format, write out chk file
			{
				DeleteFileA(filePath); // Remove any existing files of the same name
				fopen_s(&pFile, filePath, "wb");
				WriteFile(pFile);
				fclose(pFile);
				return true;
			}
		}
	}
	return false;
}

bool MapFile::GetPath()
{
	OPENFILENAME ofn = { };
	char szFileName[MAX_PATH] = { };
	char initPath[MAX_PATH] = { };

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFilter = "All Maps\0*.scm;*.scx;*.chk\0Starcraft Maps\0*.scm\0Broodwar Maps\0*.scx\0Raw Scenario Files\0*.chk\0";
	ofn.lpstrFile = szFileName;

	if ( GetRegScPath(initPath, MAX_PATH) )
	{
		strcat_s(initPath, "\\Maps");
		ofn.lpstrInitialDir = initPath;
	}
	else
		ofn.lpstrInitialDir = NULL;

	ofn.hwndOwner = NULL;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

	if ( GetOpenFileName(&ofn) == TRUE )
	{
		strcpy_s(filePath, MAX_PATH, szFileName);
		return true;
	}
	else if ( CommDlgExtendedError() != 0 )
		CHKD_ERR("Error Retrieving File Name.");

	return false;
}

bool MapFile::OpenFile()
{
	if ( filePath != nullptr )
	{
		buffer chk("oMAP");

		char* ext = strrchr(filePath, '.'); // Find the last occurrence of '.'
		if ( ext != nullptr )
		{
			if ( strcmp(ext, ".scm") == 0 || strcmp(ext, ".scx") == 0 )
			{
				HANDLE hMpq = MpqOpenArchiveForUpdate(filePath, MOAU_OPEN_EXISTING, 1000);

				if ( hMpq != NULL && hMpq != INVALID_HANDLE_VALUE )
				{
					FileToBuffer(hMpq, "staredit\\scenario.chk", chk);
					MpqCloseUpdatedArchive(hMpq, NULL);

					if ( chk.size() > 0 && ParseScenario(chk) )
					{
						if ( VER().exists() )
						{
							if ( VER().get<u16>(0) < 63 )
								SaveType = 1; // Vanilla
							else if ( VER().get<u16>(0) < 205 )
								SaveType = 2; // Hybrid
							else
								SaveType = 3; // Expansion
						}
						else if ( strcmp(ext, ".scx") == 0 )
							SaveType = 3; // Expansion
						else if ( true ) // Could search for clues to map version here
							SaveType = 6; // Otherwise set to expansion to prevent data loss

						return true;
					}
				}
				else if ( GetLastError() == ERROR_FILE_NOT_FOUND )
					CHKD_ERR("File Not Found");
				else if ( GetLastError() == MPQ_ERROR_MPQ_INVALID )
					CHKD_ERR("Invalid or corrupt MPQ file");
				else
					CHKD_ERR("%d", GetLastError());
			}
			else if ( strcmp(ext, ".chk") == 0 )
			{
				if ( FileToBuffer(filePath, chk) )
				{
					if ( ParseScenario(chk) )
					{
						if ( VER().exists() )
						{
							if ( VER().get<u16>(0) < 63 )
								SaveType = 4; // Vanilla chk
							else if ( VER().get<u16>(0) < 205 )
								SaveType = 5; // Hybrid chk
							else
								SaveType = 6; // Expansion chk
						}
						else if ( true ) // Could search for clues to map version here
							SaveType = 6; // Otherwise set to expansion to prevent data loss

						return true;
					}
					else
						CHKD_ERR("Parsing Failed!");
				}
				else
					CHKD_ERR("Error Reading CHK File");
			}
		}
	}
	return false;
}

bool MapFile::SetPath(const char* newPath)
{
	if ( strlen(newPath) < FILENAME_MAX )
	{
		strcpy_s(filePath, newPath);
		return true;
	}
	else
		return false;
}

const char* MapFile::FilePath()
{
	return filePath;
}

void MapFile::SetSaveType(u8 newSaveType)
{
	SaveType = newSaveType;
}
