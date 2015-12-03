#include "ChkdPlugins.h"
#include "Chkdraft.h"
#include "TextTrigCompiler.h"

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
					GuiMapPtr map = chkd.maps.GetMap(mapID);
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
									char* inputText = nullptr;
									try { inputText = new char[length+1]; }
									catch ( std::bad_alloc ) {
										MessageBox(NULL, "Compilation failed, couldn't allocate space for input text.", "Error!", MB_OK);
										return FALSE;
									}
									std::memcpy(inputText, copyData, length);
									if ( inputText[length-2] != '\0' ) // Ensure NUL-terminated
										inputText[length-1] = '\0';
	
									buffer textBuf("TxTr");
									textBuf.addStr(inputText, length);
									delete[] inputText;

									TextTrigCompiler compiler;
									if ( compiler.CompileTriggers(textBuf, map) )
									{
										map->notifyChange(false);
										return TRUE;
									}
									else
										MessageBox(NULL, "Compilation failed.", "Error!", MB_OK);
								}
								break;
						}
					}
				}
			}
			break;

		case SAVE_MAP:
			{
				GuiMapPtr map = chkd.maps.GetMap((u16)wParam);
				if ( map != nullptr )
				{
					if ( map->SaveFile(false) )
						return chkd.maps.GetMapID(map);
				}
			}
			break;

		case COPY_CHK_FILE:
			{
				GuiMapPtr map = chkd.maps.GetMap((u16)lParam);
				if ( map != nullptr )
				{
					u16 mapID = chkd.maps.GetMapID(map);
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
			return DefWindowProc(hWnd, msg, wParam, lParam); // Valid occasion to use this method
			break;
	}
	return 0;
}
