#include "FileIO.h"

bool FileExists(const char* fileName)
{
	FILE* fileCheck = nullptr;
	if ( fopen_s(&fileCheck, fileName, "r") == 0 )
	{
		fclose(fileCheck);
		return true;
	}
	else
		return false;
}

bool OpenArchive(const char* directory, const char* fileName, MPQHANDLE &hMpq)
{
	BOOL success = FALSE;
	char filePath[MAX_PATH],
		 locateError[MAX_PATH+60],
		 retryError[MAX_PATH+76],
		 openFilter[MAX_PATH+28];

	sprintf_s(filePath, MAX_PATH, "%s\\%s", directory, fileName);
	sprintf_s(locateError, MAX_PATH+60, "Could not find %s!\n\nWould you like to locate it manually?", filePath);
	sprintf_s(retryError, MAX_PATH+76, "Failed to open %s! The file may be in use.\n\nWould you like to try again?", fileName);

	if ( FileExists(filePath) ) // File found
	{
		do { success = SFileOpenArchive((LPCSTR)filePath, 0, NULL, &hMpq); }
		while ( success == FALSE && MessageBox(NULL, retryError, "Error!", MB_YESNO|MB_ICONEXCLAMATION) == IDYES );
	}
	else // File not found
	{
		if ( MessageBox(NULL, locateError, "Error!", MB_YESNO|MB_ICONEXCLAMATION) == IDYES )
		{
			sprintf_s(openFilter, MAX_PATH+28, "%s\0*%s*;\0All Files\0*.*\0\0", fileName);

			OPENFILENAME ofn = { };
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = NULL;
			ofn.lpstrFilter = openFilter;
			ofn.lpstrFile = filePath;
			ofn.nMaxFile = MAX_PATH;
			ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	
			if ( GetOpenFileName(&ofn) )
			{
				do { success = SFileOpenArchive(filePath, 0, NULL, &hMpq); }
				while ( success == FALSE && MessageBox(NULL, retryError, "Error!", MB_YESNO|MB_ICONEXCLAMATION) == IDYES );
			}
			else
				return OpenArchive(directory, fileName, hMpq);
		}
	}
	return success == TRUE;
}

bool CloseArchive(MPQHANDLE mpq)
{
	return SFileCloseArchive(mpq) == TRUE;
}

bool FileToBuffer(HANDLE &hMpq, const char* fileName, buffer &buf)
{
	if ( hMpq == nullptr )
		CHKD_ERR("NULL MPQ file specified for opening %s", fileName);
	else
	{
		u32 bytesRead = 0;
		HANDLE openFile = NULL;
		if ( SFileGetFileInfo(hMpq, SFILE_INFO_NUM_FILES) != 0xFFFFFFFF )
		{
			if ( SFileOpenFileEx(hMpq, fileName, SFILE_SEARCH_CURRENT_ONLY, &openFile) )
			{
				u32 fileSize = (u32)SFileGetFileSize(openFile, NULL);
				if ( buf.setSize(fileSize) )
				{
					buf.sizeUsed = fileSize;
					SFileReadFile(openFile, (LPVOID)buf.data, buf.sizeUsed, (LPDWORD)(&bytesRead), NULL);
					SFileCloseFile(openFile);

					if ( buf.sizeUsed == bytesRead )
						return true;
				}
				else
					SFileCloseFile(openFile);
			}
			else
				CHKD_ERR("Failed to get %s from MPQ file", fileName);
		}
		else
			CHKD_ERR("File is already open", fileName);
	}
	return false;
}

bool FileToBuffer(MPQHANDLE &hStarDat, MPQHANDLE &hBrooDat, MPQHANDLE &hPatchRt, MPQHANDLE &hPriority, const char* fileName, buffer &buf)
{
	return ( hPriority != nullptr && FileToBuffer(hPriority, fileName, buf) )
		   || FileToBuffer(hPatchRt, fileName, buf)
		   || FileToBuffer(hBrooDat, fileName, buf)
		   || FileToBuffer(hStarDat, fileName, buf);
}

bool FileToBuffer(MPQHANDLE &hStarDat, MPQHANDLE &hBrooDat, MPQHANDLE &hPatchRt, const char* fileName, buffer &buf)
{
	return	  FileToBuffer(hPatchRt, fileName, buf)
		   || FileToBuffer(hBrooDat, fileName, buf)
		   || FileToBuffer(hStarDat, fileName, buf);
}

OPENFILENAME GetOfn(char* szFileName, char* filter, int initFilter)
{
	OPENFILENAME ofn = { };
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = szFileName;
	ofn.nFilterIndex = initFilter;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	return ofn;
}

OPENFILENAME GetScSaveOfn(char* szFileName)
{
	return GetOfn(
		szFileName,
		"Starcraft Map(*.scm)\0*.scm\0Starcraft Hybrid Map(*.scm)\0*.scm\0Broodwar Map(*.scx)\0*.scx\0Raw Starcraft Map(*.chk)\0*.chk\0Raw Starcraft Hybrid Map(*.chk)\0*.chk\0Raw Broodwar Map(*.chk)\0*.chk\0All Maps\0*.scm;*.scx;*.chk\0",
		7 );
}

bool FileToBuffer(const char* FileName, buffer &buf)
{
	FILE* pFile;
	fopen_s(&pFile, (const char*)FileName, "rb");
	
	if ( pFile != nullptr )
	{
		buf.flush();
		fseek (pFile , 0, SEEK_END);
		u32 fileSize = (u32)ftell(pFile);

		if ( buf.setSize(fileSize) )
		{
			buf.sizeUsed = fileSize;
			rewind (pFile);

			size_t lengthRead = fread(buf.data, 1, buf.sizeUsed, pFile);
			fclose(pFile);

			if ( lengthRead == buf.sizeUsed )
				return true;
		}
	}
	return false;
}

DWORD GetSubKeyString(HKEY hParentKey, const char* subKey, const char* valueName, char* data, DWORD* dataSize)
{
	HKEY hKey = NULL;

	DWORD errorCode = RegOpenKeyEx(hParentKey, subKey, 0, KEY_QUERY_VALUE, &hKey); // Open key
	if ( errorCode == ERROR_SUCCESS ) // Open key success
	{
		errorCode = RegQueryValueEx(hKey, valueName, NULL, NULL, (LPBYTE)data, dataSize); // Read Key
		RegCloseKey(hKey); // Close key
	}
	return errorCode; // Return success/error message
}

bool GetRegString(char* dest, u32 destSize, const char* subKey, const char* valueName)
{
	DWORD lpcbData = (DWORD)destSize;

	DWORD errorCode = GetSubKeyString(HKEY_CURRENT_USER, subKey, valueName, dest, &lpcbData); // Try HKCU
	if ( errorCode != ERROR_SUCCESS ) // Not found in HKCU
		errorCode = GetSubKeyString(HKEY_LOCAL_MACHINE, subKey, valueName, dest, &lpcbData); // Try HKLM

	return errorCode == ERROR_SUCCESS;
}

bool GetRegScPath(char* dest, u32 destSize)
{
	return GetRegString(dest, destSize, "SOFTWARE\\Blizzard Entertainment\\Starcraft", "InstallPath");
}
