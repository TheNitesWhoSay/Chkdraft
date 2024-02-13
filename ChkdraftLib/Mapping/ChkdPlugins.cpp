#include "ChkdPlugins.h"
#include "../Chkdraft.h"
#include "../../MappingCoreLib/TextTrigCompiler.h"
#include "Settings.h"

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
                                    if ( map->deserialize((Chk::SerializedChk*)copyData) )
                                    {
                                        map->refreshScenario();
                                        map->notifyChange(false);
                                        return TRUE;
                                    }
                                }
                                break;
                            case REPLACE_TRIGGERS_TEXT:
                                {
                                    std::unique_ptr<char> inputText = std::unique_ptr<char>(new char[size_t(length)+1]);
                                    std::memcpy(inputText.get(), copyData, length);
                                    if ( inputText.get()[length-2] != '\0' ) // Ensure NUL-terminated
                                        inputText.get()[length-1] = '\0';

                                    std::string textBuf(inputText.get());

                                    TextTrigCompiler compiler(Settings::useAddressesForMemory, Settings::deathTableStart);
                                    if ( compiler.compileTriggers(textBuf, *map, chkd.scData, 0, map->numTriggers()) )
                                    {
                                        map->notifyChange(false);
                                        return TRUE;
                                    }
                                    else
                                        WinLib::Message("Compilation failed.", "Error!");
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

        case CLOSE_MAP:
            {
                if ( u16(wParam) == 0 && CM != nullptr )
                    chkd.maps.CloseActive();
                else
                {
                    GuiMapPtr map = chkd.maps.GetMap((u16)wParam);
                    if ( map != nullptr )
                        chkd.maps.CloseMap(map->getHandle());
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
                    std::vector<u8> chkFile = map->serialize();
                    copyData.lpData = PVOID(&chkFile[0]);
                    copyData.cbData = DWORD(chkFile.size() - sizeof(Chk::ChkHeader));
                    SendMessage((HWND)wParam, WM_COPYDATA, (WPARAM)hWnd, (LPARAM)&copyData);
                    return mapID;
                }
            }
            break;

        default:
            return DefWindowProc(hWnd, msg, wParam, lParam); // Valid occasion to use this method
            break;
    }
    return 0;
}

bool signalRecompileError(const std::string & errorMessage)
{
    logger.error(errorMessage);
    Error(errorMessage);
    return false;
}

bool runMpqRecompiler()
{
    MapFile inputMapFile(MapFile::getDefaultOpenMapBrowser());
    if ( inputMapFile.getFilePath().empty() || inputMapFile.empty() )
        return signalRecompileError("Failed to open input file");
    else if ( !inputMapFile.open(inputMapFile.getFilePath(), true, false) )
        return signalRecompileError("Failed to open input file");
    else
        logger.info("Successfully opened input file");
    
    MpqFile outputFile{};
    auto saveMapBrowser = MapFile::getDefaultSaveMapBrowser();
    std::string outputFilePath = inputMapFile.getFilePath();
    SaveType saveType = inputMapFile.getSaveType();
    if ( !saveMapBrowser->browseForSavePath(outputFilePath, saveType) )
        return signalRecompileError("Failed to get a save file path");
    if ( outputFilePath == inputMapFile.getFilePath() )
        return signalRecompileError("Input and output file paths must differ!");
    else if ( findFile(outputFilePath) && !removeFile(outputFilePath) )
        return signalRecompileError("File to overwrite could not be removed!");
    else if ( !outputFile.open(outputFilePath, false) )
        return signalRecompileError("Failed to create output file!");
    else
        logger.info("Successfully created output file");

    if ( auto scenarioFile = inputMapFile.getFile("staredit\\scenario.chk") )
    {
        if ( outputFile.addFile("staredit\\scenario.chk", *scenarioFile) )
            logger.info() << "Added scenario file" << std::endl;
        else
            return signalRecompileError("Error adding scenario file");
    }
    else
        return signalRecompileError("Error getting scenario file");

    bool warn = false;
    for ( auto soundPath : inputMapFile.soundPaths )
    {
        if ( soundPath != Chk::StringId::UnusedSound )
        {
            if ( auto soundPathStr = inputMapFile.getString<RawString>(size_t(soundPath)) )
            {
                if ( auto soundFile = inputMapFile.getFile(soundPathStr->c_str()) )
                {
                    if ( outputFile.addFile(*soundPathStr, *soundFile) )
                        logger.info() << "Added " << *soundPathStr << std::endl;
                    else
                    {
                        warn = true;
                        logger.warn() << "Failed to transfer sound to new MPQ: " << *soundPathStr << std::endl;
                    }
                }
                else
                {
                    warn = true;
                    logger.warn() << "Failed to get sound for transfer to new MPQ: " << *soundPathStr << std::endl;
                }
            }
        }
    }
    inputMapFile.close();

    outputFile.save();
    outputFile.close();

    if ( warn )
    {
        logger.info() << "Recompile success with warnings shown above." << std::endl;
        mb("Recompile success with warnings (see logs)");
    }
    else
    {
        logger.info() << "Recompile success" << std::endl;
        mb("Recompile success!");
    }
    return true;
}
