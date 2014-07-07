#include "ChkdPlugins.h"
#include "Maps.h"
#include "TextTrig.h"
extern MAPS maps;
extern HWND hMain;

LRESULT CALLBACK PluginProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_COPYDATA:
			{
				COPYDATASTRUCT* dataStruct = (COPYDATASTRUCT*)lParam;
				u32 length = (u32)dataStruct->cbData;
				const void* copyData = dataStruct->lpData;

				u32 dwData = (u32)dataStruct->dwData;
				u16 copyID = LOWORD(dwData),
					mapID = HIWORD(dwData);

				if ( copyID >= PLUGIN_MSG_START && copyID <= PLUGIN_MSG_END )
				{
					GuiMap* map = maps.GetMap(mapID);
					if ( map != nullptr )
					{
						switch ( copyID )
						{
							case UPDATE_CHK_FILE:
								{
									if ( map->Deserialize(copyData) )
									{
										map->refreshScenario();
										map->notifyChange(false);
										return TRUE;
									}
								}
								break;
							case REPLACE_TRIGGERS_BYTES:
								{
									buffer trigs;
									if ( trigs.deserialize(copyData) )
									{
										if ( map->TRIG().takeAllData(trigs) )
										{
											map->refreshScenario();
											map->notifyChange(false);
											return TRUE;
										}
									}
								}
								break;
							case REPLACE_TRIGGERS_TEXT:
								{
									char* trigText = nullptr;
									try { trigText = new char[length+1]; }
									catch ( std::bad_alloc ) {
										MessageBox(NULL, "Compilation failed, couldn't allocate space for input text.", "Error!", MB_OK);
										return FALSE;
									}
									memcpy(trigText, copyData, length);
									if ( trigText[length-2] != '\0' ) // Ensure NUL-terminated
										trigText[length-1] = '\0';
	
									buffer text("TxTr"), output("TRIG");
									text.addStr(trigText, length);
									delete[] trigText;

									bool success;
									TextTrigCompiler compiler(map, success);
									if ( success == true )
									{
										if ( compiler.CompileText(text, map) )
										{
											map->notifyChange(false);
											return TRUE;
										}
									}
									else
										MessageBox(NULL, "Compilation failed, couldn't allocate space for metadata.", "Error!", MB_OK);
								}
								break;
						}
					}
				}
			}
			break;

		case SAVE_MAP:
			{
				GuiMap* map = maps.GetMap((u16)wParam);
				if ( map != nullptr )
				{
					if ( map->SaveFile(false) )
						return maps.GetMapID(map);
				}
			}
			break;

		case COPY_CHK_FILE:
			{
				GuiMap* map = maps.GetMap((u16)lParam);
				if ( map != nullptr )
				{
					u16 mapID = maps.GetMapID(map);
					COPYDATASTRUCT copyData;
					copyData.dwData = (ULONG_PTR)MAKELONG(COPY_CHK_FILE, mapID);
					void* chkFile;
					if ( map->Serialize(chkFile) )
					{
						copyData.lpData = chkFile;
						copyData.cbData = ((u32*)chkFile)[1]+8;
						SendMessage((HWND)wParam, WM_COPYDATA, (WPARAM)hWnd, (LPARAM)&copyData);
						delete[] ((char*)chkFile);
						return mapID;
					}
				}
			}
			break;

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
			break;
	}
	return 0;
}