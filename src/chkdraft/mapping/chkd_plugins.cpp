#include "chkd_plugins.h"
#include "chkdraft/chkdraft.h"
#include <mapping_core/text_trig_compiler.h>
#include "chkd_profiles.h"

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

                                    TextTrigCompiler compiler(chkd.profiles().triggers.useAddressesForMemory, chkd.profiles().triggers.deathTableStart);
                                    if ( compiler.compileTriggers(textBuf, (Scenario &)*map, *chkd.scData, 0, map->numTriggers()) )
                                        return TRUE;
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
    SaveType saveType = inputMapFile->saveType;
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
    for ( auto soundPath : inputMapFile->soundPaths )
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
                else if ( !inputMapFile.isInVirtualSoundList(*soundPathStr) )
                {
                    warn = true;
                    logger.warn() << "Failed to get sound for transfer to new MPQ: " << *soundPathStr << std::endl;
                }
            }
        }
    }
    for ( auto potentialSoundPath : inputMapFile->strings )
    {
        if ( potentialSoundPath && !potentialSoundPath->empty() )
        {
            if ( inputMapFile.findFile(potentialSoundPath->str) )
            {
                if ( auto soundFile = inputMapFile.getFile(potentialSoundPath->str) )
                {
                    if ( outputFile.addFile(potentialSoundPath->str, *soundFile) )
                        logger.info() << "Added " << potentialSoundPath->str << std::endl;
                    else if ( !inputMapFile.isInVirtualSoundList(potentialSoundPath->str) )
                    {
                        warn = true;
                        logger.warn() << "Failed to transfer sound to new MPQ: " << potentialSoundPath->str << std::endl;
                    }
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

void repairSounds()
{
    if ( CM == nullptr )
        return;
    
    auto edit = CM->create_action(ActionDescriptor::RepairSounds);
    std::set<u32> knownSoundPaths {};
    bool isNew = CM->getFilePath().empty();

    bool warn = false;
    if ( !CM->hasSection(Chk::SectionName::WAV) )
    {
        CM->addSaveSection(Chk::SectionName::WAV);
        logger.info() << "Found the WAV section was missing, added the WAV section to the map." << std::endl;
    }

    // Look for any strings to delete
    for ( size_t i=0; i<Chk::TotalSounds; ++i )
    {
        auto soundStringId = CM->read.soundPaths[i];
        if ( soundStringId >= CM->read.strings.size() || !CM->read.strings[soundStringId] || CM->read.strings[soundStringId]->empty() )
        {
            edit->soundPaths[i] = Chk::StringId::UnusedSound;
        }
        else if ( soundStringId != Chk::StringId::UnusedSound )
        {
            if ( soundStringId >= CM->read.strings.size() )
            {
                edit->soundPaths[i] = Chk::StringId::UnusedSound;
                CM->deleteString(soundStringId, Chk::Scope::Game, true);
                auto foundStr = CM->getString<ChkdString>(soundStringId);
                logger.info()
                    << "Sound [" << i << "] string [" << soundStringId << "] \"" << (foundStr ? *foundStr : ChkdString(""))
                    << "\" had a duplicated WAV section entry which was removed." << std::endl;
            }
            else
            {
                auto soundStatus = CM->getSoundStatus(soundStringId);
                switch ( soundStatus )
                {
                    case SoundStatus::PendingMatch:
                    case SoundStatus::CurrentMatch:
                    case SoundStatus::VirtualFile:
                        if ( knownSoundPaths.find(soundStringId) == knownSoundPaths.end() )
                            knownSoundPaths.insert(soundStringId); // Sound is fine, no action needed
                        else // Sound has a duplicate entry
                        {
                            edit->soundPaths[i] = Chk::StringId::UnusedSound;
                            logger.info()
                                << "Sound [" << i << "] string [" << soundStringId << "] \""
                                << "\" was removed not refer to a valid string so it was removed from the WAV section." << std::endl;
                        }
                        break;

                    case SoundStatus::FileInUse:
                        warn = true;
                        logger.warn() << "Map file in use or does not exist, sound could not be examined!" << std::endl;
                        break;
                    case SoundStatus::Unknown:
                        warn = true;
                        logger.warn() << "An unknown sound status was encountered, no action will be taken." << std::endl;
                        break;

                    case SoundStatus::NoMatch:
                    case SoundStatus::NoMatchExtended:
                        {
                            edit->soundPaths[i] = Chk::StringId::UnusedSound;
                            CM->deleteString(soundStringId, Chk::Scope::Game, true);
                            auto foundStr = CM->getString<ChkdString>(soundStringId);
                            logger.info()
                                << "Sound [" << i << "] string [" << soundStringId << "] \"" << (foundStr ? *foundStr : ChkdString(""))
                                << "\" does not refer to a valid sound so it was removed from the WAV section." << std::endl;
                        }
                        break;
                }
            }
        }
    }

    for ( size_t i=0; i<CM->read.strings.size(); ++i )
    {
        auto & potentialSoundPath = CM->read.strings[i];
        if ( knownSoundPaths.find(u32(i)) == knownSoundPaths.end() && potentialSoundPath && !potentialSoundPath->empty() )
        {
            switch ( CM->getSoundStatus(i) )
            {
                case SoundStatus::PendingMatch:
                case SoundStatus::CurrentMatch:
                case SoundStatus::VirtualFile:
                    {
                        auto result = ((Scenario &)*CM).addSound(i);
                        if ( result < Chk::TotalSounds )
                            logger.info() << "Sound string [" << i << "] was added to the WAV section successfully at index [" << result << "]" << std::endl;
                        else
                        {
                            warn = true;
                            logger.warn() << "Sound string [" << i << "] could not be added to the WAV section as the WAV section is full!" << std::endl;
                        }
                    }
                    break;

                case SoundStatus::FileInUse:
                    if ( !isNew )
                    {
                        warn = true;
                        logger.warn() << "Map file in use or does not exist, sound could not be examined!" << std::endl;
                    }
                    break;
                case SoundStatus::Unknown:
                    warn = true;
                    logger.warn() << "An unknown sound status was encountered, no action will be taken." << std::endl;
                    break;

                case SoundStatus::NoMatch:
                case SoundStatus::NoMatchExtended:
                    break; // Just a regular string, do nothing
            }
        }
    }
    CM->refreshScenario();

    if ( warn )
    {
        logger.info() << "Sound repair succeeded with warnings shown above." << std::endl;
        mb("Sound repair success with warnings (see logs)");
    }
    else
    {
        logger.info() << "Sound repair succeeded" << std::endl;
        mb("Sound repair success!");
    }
}

void repairStrings(bool compatibilityMode)
{
    if ( CM == nullptr || CM->isProtected() )
        return;

    if ( compatibilityMode )
    {
        auto result = getYesNoCancel(
            "Unlike the minimal string repair, which only handles integrity issues with StarCraft and Chkdraft, "
            "maximum compatibility will intrusively add strings that aren't strictly necessary in order to maximize compatibility "
            "with staredit and various versions of SCMDraft, are you sure you want to continue?",
            "Max Compatibility String Repair");

        if ( result != PromptResult::Yes )
        {
            logger.info() << "String repair cancelled.\n";
            return;
        }
    }

    auto & map = *CM;
    auto edit = map.create_action(ActionDescriptor::RepairStrings);
    auto stringsBackup = map.copyStrings(Chk::Scope::Game);
    auto editorStringsBackup = map.copyStrings(Chk::Scope::Editor);
    auto locationsBackup = map->locations;
    auto triggersBackup = map->triggers;
    auto briefingTriggersBackup = map->briefingTriggers;
    auto scenarioPropertiesBackup = map->scenarioProperties;
    auto forceBackup = map->forces;
    auto soundPathsBackup = std::make_unique<u32[]>(Chk::TotalSounds);
    std::memcpy(&soundPathsBackup[0], &map->soundPaths[0], Chk::TotalSounds*sizeof(u32));
    auto origUnitSettingsBackup = map->origUnitSettings;
    auto unitSettingsBackup = map->unitSettings;
    auto switchesBackup = std::make_unique<u32[]>(Chk::TotalSwitches);
    std::memcpy(&switchesBackup[0], &map->switchNames[0], Chk::TotalSwitches*sizeof(u32));
    auto ostrBackup = map->editorStringOverrides;
    auto triggerExtensionsBackup = map->triggerExtensions;
    auto triggerGroupingsBackup = map->triggerGroupings;

    bool lessThanStandardAtStart = map->strings.size() < 1024;

    if ( !map.isRemastered() && map.hasSection(Chk::SectionName::STRx) ) // Remastered string section in non-remastered map
    {
        logger.warn() << "Remastered string section found in non-remastered map, replacing with classic string section.\n";
        map.removeSaveSection(Chk::SectionName::STRx);
        map.addSaveSection(Chk::SectionName::STR);
    }

    if ( map.isRemastered() && map.hasSection(Chk::SectionName::STR) ) // Classic string section in remastered map
    {
        logger.warn() << "Classic string section found in remastered map, replacing with remastered string section.\n";
        map.removeSaveSection(Chk::SectionName::STR);
        map.addSaveSection(Chk::SectionName::STRx);
    }

    if ( !map.hasSection(Chk::SectionName::STR) && !map.hasSection(Chk::SectionName::STRx) ) // No string section
    {
        logger.warn() << "No string section found, adding\n";
        map.addSaveSection(map.isRemastered() ? Chk::SectionName::STRx : Chk::SectionName::STR);
    }

    if ( map->strings.empty() )
    {
        logger.warn() << "String header NUL was missing, adding string header NUL\n";
        edit->strings.append(std::nullopt);
    }
    else if ( map->strings.front() )
    {
        logger.warn() << "Header was not null, moving to back and swapping\n";
        edit->strings.append(std::nullopt);
        edit->strings.swap(std::size_t(0), map->strings.size()-1);
    }

    for ( size_t i=0; i<map->locations.size(); ++i )
    {
        const auto & location = map->locations[i];
        if ( location.stringId >= map->strings.size() )
        {
            logger.warn() << "Location Index:" << i << " had an out-of-bounds string, the string has been cleared.\n";
            edit->locations[i].stringId = Chk::StringId::NoString;
        }
        else if ( map->locations[i].stringId != Chk::StringId::NoString && (!map->strings[location.stringId] || map->strings[location.stringId]->empty()) )
        {
            logger.warn() << "Location Index:" << i << " referred to an empty string, the string has been cleared.\n";
            edit->locations[i].stringId = Chk::StringId::NoString;
        }

        if ( compatibilityMode &&
            (map->locations[i].stringId == Chk::StringId::NoString ||
                !map->strings[map->locations[i].stringId] ||
                map->strings[map->locations[i].stringId]->empty()) &&
            (map.triggerLocationUsed(i) || !location.isBlank()) )
        {
            logger.warn() << "Location Index:" << i << " was in-use but lacked a string, setting it to \"Location " + std::to_string(i) + "\".\n";
            map.setLocationName<RawString>(i, "Location " + std::to_string(i), Chk::Scope::Game, true);
        }
    }

    for ( size_t triggerIndex=0; triggerIndex<map->triggers.size(); ++triggerIndex )
    {
        const auto & trigger = map->triggers[triggerIndex];
        for ( size_t actionIndex=0; actionIndex<Chk::Trigger::MaxActions; ++actionIndex )
        {
            auto & action = trigger.actions[actionIndex];
            bool usesString = Chk::Action::actionUsesStringArg[action.actionType];
            bool usesSoundString = Chk::Action::actionUsesSoundArg[action.actionType];
            if ( action.stringId >= map->strings.size() )
            {
                logger.warn() << "Trigger " << triggerIndex << " action " << actionIndex << " had an out-of-bounds string, the string has been cleared.\n";
                edit->triggers[triggerIndex].actions[actionIndex].stringId = 0;
            }
            else if ( !usesString && action.stringId != 0 )
            {
                logger.warn() << "Trigger " << triggerIndex << " action " << actionIndex << " has no stringArg but stringId was set, clearing\n";
                edit->triggers[triggerIndex].actions[actionIndex].stringId = 0;
            }

            if ( compatibilityMode && usesString && (action.stringId == 0 || !map->strings[action.stringId] || map->strings[action.stringId]->empty() ) )
            {
                logger.warn() << "Trigger " << triggerIndex << " action " << actionIndex << " was in-use but lacked a string, setting it to \"TODO\"\n";
                edit->triggers[triggerIndex].actions[actionIndex].stringId = u32(map.addString<RawString>("TODO", Chk::Scope::Game, true));
            }
            
            if ( action.soundStringId >= map->strings.size() )
            {
                logger.warn() << "Trigger " << triggerIndex << " action " << actionIndex
                    << " had an out-of-bounds sound-string, the sound-string has been cleared.\n";
                edit->triggers[triggerIndex].actions[actionIndex].soundStringId = 0;
            }
            else if ( !usesSoundString && action.soundStringId != 0 )
            {
                logger.warn() << "Trigger " << triggerIndex << " action " << actionIndex << " has no soundStringArg but soundStringId was set, clearing\n";
                edit->triggers[triggerIndex].actions[actionIndex].soundStringId = 0;
            }

            if ( compatibilityMode && usesSoundString &&
                (action.soundStringId == 0 || !map->strings[action.soundStringId] || map->strings[action.soundStringId]->empty() ) )
            {
                logger.warn() << "Trigger " << triggerIndex << " action " << actionIndex << " was in-use but lacked a sound string, setting it to \"TODO\"\n";
                edit->triggers[triggerIndex].actions[actionIndex].soundStringId = u32(map.addString<RawString>("TODO", Chk::Scope::Game, true));
            }
        }
    }

    for ( size_t briefingTriggerIndex=0; briefingTriggerIndex<map->briefingTriggers.size(); ++briefingTriggerIndex )
    {
        auto & briefingTrigger = map->briefingTriggers[briefingTriggerIndex];
        for ( size_t actionIndex=0; actionIndex<Chk::Trigger::MaxActions; ++actionIndex )
        {
            auto & briefingAction = briefingTrigger.actions[actionIndex];
            bool usesString = Chk::Action::briefingActionUsesStringArg[briefingAction.actionType];
            bool usesSoundString = Chk::Action::briefingActionUsesSoundArg[briefingAction.actionType];
            if ( briefingAction.stringId >= map->strings.size() )
            {
                logger.warn() << "BriefingTrigger " << briefingTriggerIndex << " action " << actionIndex
                    << " had an out-of-bounds string, the string has been cleared.\n";
            }
            else if ( !usesString && briefingAction.stringId != 0 )
            {
                logger.warn() << "BriefingTrigger " << briefingTriggerIndex << " action " << actionIndex
                    << " has no stringArg but stringId was set, clearing\n";
                edit->briefingTriggers[briefingTriggerIndex].actions[actionIndex].stringId = 0;
            }

            if ( compatibilityMode && usesString &&
                (briefingAction.stringId == 0 || !map->strings[briefingAction.stringId] || map->strings[briefingAction.stringId]->empty() ) )
            {
                logger.warn() << "BriefingTrigger " << briefingTriggerIndex << " action " << actionIndex
                    << " was in-use but lacked a string, setting it to \"TODO\"\n";
                edit->briefingTriggers[briefingTriggerIndex].actions[actionIndex].stringId = u32(map.addString<RawString>("TODO", Chk::Scope::Game, true));
            }
            
            if ( briefingAction.soundStringId >= map->strings.size() )
            {
                logger.warn() << "BriefingTrigger " << briefingTriggerIndex << " action " << actionIndex
                    << " had an out-of-bounds sound-string, the sound-string has been cleared.\n";
                edit->briefingTriggers[briefingTriggerIndex].actions[actionIndex].soundStringId = 0;
            }
            else if ( !usesSoundString && briefingAction.soundStringId != 0 )
            {
                logger.warn() << "BriefingTrigger " << briefingTriggerIndex << " action " << actionIndex
                    << " has no soundStringArg but soundStringId was set, clearing\n";
                edit->briefingTriggers[briefingTriggerIndex].actions[actionIndex].soundStringId = 0;
            }

            if ( compatibilityMode && usesSoundString &&
                (briefingAction.soundStringId == 0 || !map->strings[briefingAction.soundStringId] || map->strings[briefingAction.soundStringId]->empty() ) )
            {
                logger.warn() << "BriefingTrigger " << briefingTriggerIndex << " action " << actionIndex
                    << " was in-use but lacked a sound string, setting it to \"TODO\"\n";
                edit->briefingTriggers[briefingTriggerIndex].actions[actionIndex].soundStringId = u32(map.addString<RawString>("TODO", Chk::Scope::Game, true));
            }
        }
    }

    if ( map->scenarioProperties.scenarioNameStringId >= map->strings.size() )
    {
        logger.warn() << "Scenario name had an out-of-bounds string, the string has been cleared.\n";
        edit->scenarioProperties.scenarioNameStringId = Chk::StringId::NoString;
    }
    else if ( map->scenarioProperties.scenarioNameStringId != Chk::StringId::NoString &&
        (!map->strings[map->scenarioProperties.scenarioNameStringId] || map->strings[map->scenarioProperties.scenarioNameStringId]->empty()) )
    {
        logger.warn() << "Scenario name referred to an empty string, the string has been cleared.\n";
        edit->scenarioProperties.scenarioNameStringId = Chk::StringId::NoString;
    }

    if ( compatibilityMode && (map->scenarioProperties.scenarioNameStringId == Chk::StringId::NoString || !map->strings[map->scenarioProperties.scenarioNameStringId] || map->strings[map->scenarioProperties.scenarioNameStringId]->empty()) )
    {
        logger.warn() << "Scenario had no name, setting to \"Untitled Scenario\".\n";
        edit->scenarioProperties.scenarioNameStringId = u16(map.addString<RawString>("Untitled Scenario", Chk::Scope::Game, true));
    }

    if ( map->scenarioProperties.scenarioDescriptionStringId >= map->strings.size() )
    {
        logger.warn() << "Scenario name had an out-of-bounds string, the string has been cleared.\n";
        edit->scenarioProperties.scenarioDescriptionStringId = Chk::StringId::NoString;
    }
    else if ( map->scenarioProperties.scenarioDescriptionStringId != Chk::StringId::NoString &&
        (!map->strings[map->scenarioProperties.scenarioDescriptionStringId] || map->strings[map->scenarioProperties.scenarioDescriptionStringId]->empty()) )
    {
        logger.warn() << "Scenario name referred to an empty string, the string has been cleared.\n";
        edit->scenarioProperties.scenarioDescriptionStringId = Chk::StringId::NoString;
    }

    if ( compatibilityMode && (map->scenarioProperties.scenarioDescriptionStringId == Chk::StringId::NoString || !map->strings[map->scenarioProperties.scenarioDescriptionStringId] || map->strings[map->scenarioProperties.scenarioDescriptionStringId]->empty()) )
    {
        logger.warn() << "Scenario had no description, setting to \"Untitled Scenario\".\n";
        edit->scenarioProperties.scenarioDescriptionStringId = u16(map.addString<RawString>("Untitled Scenario", Chk::Scope::Game, true));
    }

    for ( size_t i=0; i<Chk::TotalForces; ++i )
    {
        if ( map->forces.forceString[i] >= map->strings.size() )
        {
            logger.warn() << "Force " << (i+1) << "'s name had an out-of-bounds string, the string has been cleared.\n";
            edit->forces.forceString[i] = Chk::StringId::NoString;
        }
        else if ( map->forces.forceString[i] != Chk::StringId::NoString &&
            (!map->strings[map->forces.forceString[i]] || map->strings[map->forces.forceString[i]]->empty()) )
        {
            logger.warn() << "Force " << (i+1) << "'s name referred to an empty string, the string has been cleared.\n";
            edit->forces.forceString[i] = Chk::StringId::NoString;
        }

        if ( compatibilityMode && (map->forces.forceString[i] == Chk::StringId::NoString || !map->strings[map->forces.forceString[i]] || map->strings[map->forces.forceString[i]]->empty()) )
        {
            logger.warn() << "Force " << (i+1) << " had no name, setting to \"Force " << (i+1) << "\".\n";
            edit->forces.forceString[i] = u16(map.addString<RawString>("Force" + std::to_string(i+1), Chk::Scope::Game, true));
        }
    }

    for ( size_t i=0; i<Chk::TotalSounds; ++i )
    {
        if ( map->soundPaths[i] >= map->strings.size() )
        {
            logger.warn() << "Sound Index " << i << " had an out-of-bounds string, the string has been cleared.\n";
            edit->soundPaths[i] = Chk::StringId::NoString;
        }
        else if ( map->soundPaths[i] != Chk::StringId::NoString &&
            (!map->strings[map->soundPaths[i]] || map->strings[map->soundPaths[i]]->empty()) )
        {
            logger.warn() << "Sound Index " << i << " referred to an empty string, the string has been cleared.\n";
            edit->soundPaths[i] = Chk::StringId::NoString;
        }
    }
    
    for ( size_t i=0; i<Sc::Unit::TotalTypes; ++i )
    {
        if ( map->origUnitSettings.nameStringId[i] >= map->strings.size() )
        {
            logger.warn() << "Unit Index " << i << "'s original name had an out-of-bounds string, the string has been cleared.\n";
            edit->origUnitSettings.nameStringId[i] = Chk::StringId::NoString;
        }
        else if ( map->origUnitSettings.nameStringId[i] != Chk::StringId::NoString &&
            (!map->strings[map->origUnitSettings.nameStringId[i]] || map->strings[map->origUnitSettings.nameStringId[i]]->empty()) )
        {
            logger.warn() << "Unit Index " << i << "'s original name referred to an empty string, the string has been cleared.\n";
            edit->origUnitSettings.nameStringId[i] = Chk::StringId::NoString;
        }
        
        if ( map->unitSettings.nameStringId[i] >= map->strings.size() )
        {
            logger.warn() << "Unit Index " << i << "'s expansion name had an out-of-bounds string, the string has been cleared.\n";
            edit->unitSettings.nameStringId[i] = Chk::StringId::NoString;
        }
        else if ( map->unitSettings.nameStringId[i] != Chk::StringId::NoString &&
            (!map->strings[map->unitSettings.nameStringId[i]] || map->strings[map->unitSettings.nameStringId[i]]->empty()) )
        {
            logger.warn() << "Unit Index " << i << "'s expansion name referred to an empty string, the string has been cleared.\n";
            edit->unitSettings.nameStringId[i] = Chk::StringId::NoString;
        }
    }

    for ( size_t i=0; i<Chk::TotalSwitches; ++i )
    {
        if ( map->switchNames[i] >= map->strings.size() )
        {
            logger.warn() << "Switch " << (i+1) << " used an out-of-bounds string, the string has been cleared.\n";
            edit->switchNames[i] = Chk::StringId::NoString;
        }
        else if ( map->switchNames[i] != Chk::StringId::NoString &&
            (!map->strings[map->switchNames[i]] || map->strings[map->switchNames[i]]->empty()) )
        {
            logger.warn() << "Switch " << (i+1) << " referred to an empty string, the string has been cleared.\n";
            edit->switchNames[i] = Chk::StringId::NoString;
        }

        if ( compatibilityMode && map.triggerSwitchUsed(i) &&
            (map->switchNames[i] == Chk::StringId::NoString || !map->strings[map->switchNames[i]] || map->strings[map->switchNames[i]]->empty()) )
        {
            logger.warn() << "Switch " << (i+1) << " was in use but had no name, setting to \"Switch " << (i+1) << "\".\n";
            edit->switchNames[i] = u32(map.addString<RawString>("Switch " + std::to_string(i+1), Chk::Scope::Game, true));
        }
    }

    if ( map->editorStringOverrides.scenarioName != Chk::StringId::NoString && map->editorStringOverrides.scenarioName >= map->editorStrings.size() )
    {
        logger.warn() << "Editor-string override for scenario name had an out-of-bounds string, the string has been cleared.\n";
        edit->editorStringOverrides.scenarioName = Chk::StringId::NoString;
    }
    else if ( map->editorStringOverrides.scenarioName != Chk::StringId::NoString &&
        (!map->editorStrings[map->editorStringOverrides.scenarioName] || map->editorStrings[map->editorStringOverrides.scenarioName]->empty()) )
    {
        logger.warn() << "Editor-string override for scenario name referred to an empty string, the string has been cleared.\n";
        edit->editorStringOverrides.scenarioName = Chk::StringId::NoString;
    }

    if ( map->editorStringOverrides.scenarioDescription != Chk::StringId::NoString &&
        map->editorStringOverrides.scenarioDescription >= map->editorStrings.size() )
    {
        logger.warn() << "Editor-string override for scenario description had an out-of-bounds string, the string has been cleared.\n";
        edit->editorStringOverrides.scenarioDescription = Chk::StringId::NoString;
    }
    else if ( map->editorStringOverrides.scenarioDescription != Chk::StringId::NoString &&
        (!map->editorStrings[map->editorStringOverrides.scenarioDescription] || map->editorStrings[map->editorStringOverrides.scenarioDescription]->empty()) )
    {
        logger.warn() << "Editor-string override for scenario description referred to an empty string, the string has been cleared.\n";
        edit->editorStringOverrides.scenarioDescription = Chk::StringId::NoString;
    }

    for ( size_t i=0; i<Chk::TotalForces; ++i )
    {
        if ( map->editorStringOverrides.forceName[i] != Chk::StringId::NoString && map->editorStringOverrides.forceName[i] >= map->editorStrings.size() )
        {
            logger.warn() << "Editor-string override for force " << (i+1) << "'s name had an out-of-bounds string, the string has been cleared.\n";
            edit->editorStringOverrides.forceName[i] = Chk::StringId::NoString;
        }
        else if ( map->editorStringOverrides.forceName[i] != Chk::StringId::NoString &&
            (!map->editorStrings[map->editorStringOverrides.forceName[i]] || map->editorStrings[map->editorStringOverrides.forceName[i]]->empty()) )
        {
            logger.warn() << "Editor-string override for force " << (i+1) << "'s name referred to an empty string, the string has been cleared.\n";
            edit->editorStringOverrides.forceName[i] = Chk::StringId::NoString;
        }
    }

    for ( size_t i=0; i<Sc::Unit::TotalTypes; ++i )
    {
        if ( map->editorStringOverrides.unitName[i] != Chk::StringId::NoString && map->editorStringOverrides.unitName[i] >= map->editorStrings.size() )
        {
            logger.warn() << "Editor-string override for unit index " << i << "'s original name had an out-of-bounds string, the string has been cleared.\n";
            edit->editorStringOverrides.unitName[i] = Chk::StringId::NoString;
        }
        else if ( map->editorStringOverrides.unitName[i] != Chk::StringId::NoString &&
            (!map->editorStrings[map->editorStringOverrides.unitName[i]] || map->editorStrings[map->editorStringOverrides.unitName[i]]->empty()) )
        {
            logger.warn() << "Editor-string override for unit index " << i << "'s original name referred to an empty string, the string has been cleared.\n";
            edit->editorStringOverrides.unitName[i] = Chk::StringId::NoString;
        }
    }

    for ( size_t i=0; i<Sc::Unit::TotalTypes; ++i )
    {
        if ( map->editorStringOverrides.expUnitName[i] != Chk::StringId::NoString && map->editorStringOverrides.expUnitName[i] >= map->editorStrings.size() )
        {
            logger.warn() << "Editor-string override for unit index " << i << "'s expansion name had an out-of-bounds string, the string has been cleared.\n";
            edit->editorStringOverrides.expUnitName[i] = Chk::StringId::NoString;
        }
        else if ( map->editorStringOverrides.expUnitName[i] != Chk::StringId::NoString &&
            (!map->editorStrings[map->editorStringOverrides.expUnitName[i]] || map->editorStrings[map->editorStringOverrides.expUnitName[i]]->empty()) )
        {
            logger.warn() << "Editor-string override for unit index " << i << "'s expansion name referred to an empty string, the string has been cleared.\n";
            edit->editorStringOverrides.expUnitName[i] = Chk::StringId::NoString;
        }
    }

    for ( size_t i=0; i<Chk::TotalSounds; ++i )
    {
        if ( map->editorStringOverrides.soundPath[i] != Chk::StringId::NoString && map->editorStringOverrides.soundPath[i] >= map->editorStrings.size() )
        {
            logger.warn() << "Editor-string override for sound index " << i << " had an out-of-bounds string, the string has been cleared.\n";
            edit->editorStringOverrides.soundPath[i] = Chk::StringId::NoString;
        }
        else if ( map->editorStringOverrides.soundPath[i] != Chk::StringId::NoString &&
            (!map->editorStrings[map->editorStringOverrides.soundPath[i]] || map->editorStrings[map->editorStringOverrides.soundPath[i]]->empty()) )
        {
            logger.warn() << "Editor-string override for sound index " << i << " referred to an empty string, the string has been cleared.\n";
            edit->editorStringOverrides.soundPath[i] = Chk::StringId::NoString;
        }
    }

    for ( size_t i=0; i<Chk::TotalSwitches; ++i )
    {
        if ( map->editorStringOverrides.switchName[i] != Chk::StringId::NoString && map->editorStringOverrides.switchName[i] >= map->editorStrings.size() )
        {
            logger.warn() << "Editor-string override for switch " << (i+1) << " had an out-of-bounds string, the string has been cleared.\n";
            edit->editorStringOverrides.switchName[i] = Chk::StringId::NoString;
        }
        else if ( map->editorStringOverrides.switchName[i] != Chk::StringId::NoString &&
            (!map->editorStrings[map->editorStringOverrides.switchName[i]] || map->editorStrings[map->editorStringOverrides.switchName[i]]->empty()) )
        {
            logger.warn() << "Editor-string override for switch " << (i+1) << " referred to an empty string, the string has been cleared.\n";
            edit->editorStringOverrides.switchName[i] = Chk::StringId::NoString;
        }
    }

    for ( size_t i=0; i<Chk::TotalLocations; ++i )
    {
        if ( map->editorStringOverrides.locationName[i] != Chk::StringId::NoString && map->editorStringOverrides.locationName[i] >= map->editorStrings.size() )
        {
            logger.warn() << "Editor-string override for location index " << i << " had an out-of-bounds string, the string has been cleared.\n";
            edit->editorStringOverrides.locationName[i] = Chk::StringId::NoString;
        }
        else if ( map->editorStringOverrides.locationName[i] != Chk::StringId::NoString &&
            (!map->editorStrings[map->editorStringOverrides.locationName[i]] || map->editorStrings[map->editorStringOverrides.locationName[i]]->empty()) )
        {
            logger.warn() << "Editor-string override for location index " << i << " referred to an empty string, the string has been cleared.\n";
            edit->editorStringOverrides.locationName[i] = Chk::StringId::NoString;
        }
    }

    for ( size_t i=0; i<map->triggerExtensions.size(); ++i )
    {
        const auto & triggerExtension = map->triggerExtensions[i];

        if ( triggerExtension.commentStringId != Chk::StringId::NoString && triggerExtension.commentStringId >= map->editorStrings.size() )
        {
            logger.warn() << "Trigger extension comment for extension index " << i << " had an out-of-bounds string, the string has been cleared.\n";
            edit->triggerExtensions[i].commentStringId = Chk::StringId::NoString;
        }
        else if ( triggerExtension.commentStringId != Chk::StringId::NoString &&
            (!map->editorStrings[triggerExtension.commentStringId] || map->editorStrings[triggerExtension.commentStringId]->empty()) )
        {
            logger.warn() << "Trigger extension comment for extension index " << i << " referred to an empty string, the string has been cleared.\n";
            edit->triggerExtensions[i].commentStringId = Chk::StringId::NoString;
        }

        if ( triggerExtension.notesStringId != Chk::StringId::NoString && triggerExtension.notesStringId >= map->editorStrings.size() )
        {
            logger.warn() << "Trigger extension notes for extension index " << i << " had an out-of-bounds string, the string has been cleared.\n";
            edit->triggerExtensions[i].notesStringId = Chk::StringId::NoString;
        }
        else if ( triggerExtension.notesStringId != Chk::StringId::NoString &&
            (!map->editorStrings[triggerExtension.notesStringId] || map->editorStrings[triggerExtension.notesStringId]->empty()) )
        {
            logger.warn() << "Trigger extension notes for extension index " << i << " referred to an empty string, the string has been cleared.\n";
            edit->triggerExtensions[i].notesStringId = Chk::StringId::NoString;
        }
    }

    for ( size_t i=0; i<map->triggerGroupings.size(); ++i )
    {
        const auto & triggerGrouping = map->triggerGroupings[i];

        if ( triggerGrouping.commentStringId != Chk::StringId::NoString && triggerGrouping.commentStringId >= map->editorStrings.size() )
        {
            logger.warn() << "Trigger grouping comment for grouping index " << i << " had an out-of-bounds string, the string has been cleared.\n";
            edit->triggerGroupings[i].commentStringId = Chk::StringId::NoString;
        }
        else if ( triggerGrouping.commentStringId != Chk::StringId::NoString &&
            (!map->editorStrings[triggerGrouping.commentStringId] || map->editorStrings[triggerGrouping.commentStringId]->empty()) )
        {
            logger.warn() << "Trigger grouping comment for grouping index " << i << " referred to an empty string, the string has been cleared.\n";
            edit->triggerGroupings[i].commentStringId = Chk::StringId::NoString;
        }

        if ( triggerGrouping.notesStringId != Chk::StringId::NoString && triggerGrouping.notesStringId >= map->editorStrings.size() )
        {
            logger.warn() << "Trigger grouping notes for grouping index " << i << " had an out-of-bounds string, the string has been cleared.\n";
            edit->triggerGroupings[i].notesStringId = Chk::StringId::NoString;
        }
        else if ( triggerGrouping.notesStringId != Chk::StringId::NoString &&
            (!map->editorStrings[triggerGrouping.notesStringId] || map->editorStrings[triggerGrouping.notesStringId]->empty()) )
        {
            logger.warn() << "Trigger grouping notes for grouping index " << i << " referred to an empty string, the string has been cleared.\n";
            edit->triggerGroupings[i].notesStringId = Chk::StringId::NoString;
        }
    }
    
    logger.info() << "Defragmenting strings...\n";
    map.defragment(Chk::Scope::Both, true);
    logger.info() << "Defragmenting complete\n";
    if ( map->strings.size() < 1024 )
    {
        if ( lessThanStandardAtStart ) // Only warn if capacity was less than 1024 when you started, since defragmenting may have taken it down
            logger.warn() << "String capacity less than 1024, expanding to 1024\n";

        edit->strings.reserve(1024);
        while ( map->strings.size() < 1024 )
            edit->strings.append(std::nullopt);
    }

    bool serializable = true;
    try {
        std::vector<u8> stringBytesTest {};
        if ( map.isRemastered() )
            map.syncStringsToBytes(stringBytesTest);
        else
            map.syncRemasteredStringsToBytes(stringBytesTest);
    } catch ( const std::exception & e ) {
        logger.error("Serialization failed ", e);
        serializable = false;
    }

    if ( !serializable )
    {
        logger.error() << "String section capacity exceeded, restoring map and aborting!\n";
        map.swapStrings(stringsBackup, Chk::Scope::Game);
        map.swapStrings(editorStringsBackup, Chk::Scope::Editor);
        edit->locations = locationsBackup;
        edit->triggers = triggersBackup;
        edit->briefingTriggers = briefingTriggersBackup;
        edit->scenarioProperties = scenarioPropertiesBackup;
        edit->forces = forceBackup;
        for ( std::size_t i=0; i<Chk::TotalSounds; ++i )
            edit->soundPaths[i] = soundPathsBackup[i];
        edit->origUnitSettings = origUnitSettingsBackup;
        edit->unitSettings = unitSettingsBackup;
        for ( std::size_t i=0; i<Chk::TotalSwitches; ++i )
            edit->soundPaths[i] = soundPathsBackup[i];
        edit->editorStringOverrides = ostrBackup;
        edit->triggerExtensions = triggerExtensionsBackup;
        edit->triggerGroupings = triggerGroupingsBackup;
    }
    
    edit->strTailData.reset();
    CM->refreshScenario();
}

struct PalettedTile
{
    u8 px[32*32] {};

    inline bool operator==(const PalettedTile & other) const { return std::memcmp(px, other.px, 32*32) == 0; }
};

std::vector<PalettedTile> getPalettedMegatiles(const Sc::Terrain::Tiles & tiles)
{
    std::size_t megaTileCount = tiles.tileGraphics.size();
    std::vector<PalettedTile> wpeTiles(megaTileCount, PalettedTile{});
    for ( std::size_t i=0; i<megaTileCount; ++i )
    {
        const Sc::Terrain::TileGraphicsEx & tileGraphics = tiles.tileGraphics[i];
        for ( std::size_t yMiniTile = 0; yMiniTile < 4; yMiniTile++ )
        {
            s64 yMiniOffset = yMiniTile*8;
            for ( std::size_t xMiniTile = 0; xMiniTile < 4; xMiniTile++ )
            {
                const Sc::Terrain::TileGraphicsEx::MiniTileGraphics & miniTileGraphics = tileGraphics.miniTileGraphics[yMiniTile][xMiniTile];
                bool flipped = miniTileGraphics.isFlipped();
                size_t vr4Index = size_t(miniTileGraphics.vr4Index());
                if ( vr4Index < tiles.miniTilePixels.size() )
                {
                    const Sc::Terrain::MiniTilePixels & miniTilePixels = tiles.miniTilePixels[vr4Index];
                    s64 xMiniOffset = xMiniTile*8;
                    for ( s64 yMiniPixel = yMiniOffset < 0 ? -yMiniOffset : 0; yMiniPixel < 8; yMiniPixel++ )
                    {
                        for ( s64 xMiniPixel = xMiniOffset < 0 ? -xMiniOffset : 0; xMiniPixel < 8; xMiniPixel++ )
                        {
                            const u8 & wpeIndex = miniTilePixels.wpeIndex[yMiniPixel][flipped ? 7-xMiniPixel : xMiniPixel];
                            wpeTiles[i].px[size_t((yMiniOffset+yMiniPixel)*32 + (xMiniOffset+xMiniPixel))] = wpeIndex;
                        }
                    }
                }
            }
        }
    }
    return wpeTiles;
}

struct TileAlternative
{
    std::uint16_t altMtxmTileValue = 0;
    std::uint64_t matchScore = 0;

    inline bool operator<(const TileAlternative & other) { return matchScore > other.matchScore || (matchScore == other.matchScore && altMtxmTileValue < other.altMtxmTileValue); }
};

std::uint16_t getMegaTileIndex(const Sc::Terrain::Tiles & tiles, std::uint16_t mtxmTileValue)
{
    std::uint16_t tileGroupIndex = Sc::Terrain::getTileGroup(mtxmTileValue);
    std::uint16_t tileGroupMemberIndex = Sc::Terrain::getSubtileValue(mtxmTileValue);
    if ( tileGroupIndex >= tiles.tileGroups.size() )
        throw std::invalid_argument("Megatile could not be found for the given tileset data and mtxm index");

    const Sc::Terrain::TileGroup & tileGroup = tiles.tileGroups[tileGroupIndex];
    std::uint16_t megaTileIndex = tileGroup.megaTileIndex[tileGroupMemberIndex];
    return megaTileIndex;
}

bool renderTile(const Sc::Terrain::Tiles & tiles, std::uint16_t mtxmTileValue, std::array<Sc::SystemColor, 32*32> & tileColors)
{
    tileColors.fill(Sc::SystemColor{});
    std::uint16_t megaTileIndex = getMegaTileIndex(tiles, mtxmTileValue);
    if ( megaTileIndex == 0 )
        return true;
    else
    {
        if ( megaTileIndex >= tiles.tileGraphics.size() )
            return false;

        Sc::Terrain::TileGraphicsEx tileGraphics = tiles.tileGraphics[megaTileIndex];
        for ( std::size_t y=0; y<4; ++y )
        {
            s64 yMiniOffset = y*8;
            for ( std::size_t x=0; x<4; ++x )
            {
                Sc::Terrain::TileGraphicsEx::MiniTileGraphics miniTileGraphics = tileGraphics.miniTileGraphics[y][x];
                bool flipped = miniTileGraphics.isFlipped();
                size_t vr4Index = size_t(miniTileGraphics.vr4Index());

                if ( vr4Index < tiles.miniTilePixels.size() )
                {
                    const Sc::Terrain::MiniTilePixels & miniTilePixels = tiles.miniTilePixels[vr4Index];
                    s64 xMiniOffset = x*8;
                    for ( s64 yMiniPixel = yMiniOffset < 0 ? -yMiniOffset : 0; yMiniPixel < 8; yMiniPixel++ )
                    {
                        for ( s64 xMiniPixel = xMiniOffset < 0 ? -xMiniOffset : 0; xMiniPixel < 8; xMiniPixel++ )
                        {
                            const u8 & wpeIndex = miniTilePixels.wpeIndex[yMiniPixel][flipped ? 7-xMiniPixel : xMiniPixel];
                            tileColors[size_t((yMiniOffset+yMiniPixel)*32 + (xMiniOffset+xMiniPixel))] = tiles.systemColorPalette[wpeIndex];
                        }
                    }
                }
            }
        }
    }
    return true;
}

std::uint32_t calcTileColorDifference(const Sc::Terrain::Tiles & tiles, const std::vector<PalettedTile> & palettedMegaTiles, std::uint16_t mtxmTileValue, std::uint16_t otherMtxmTileValue)
{
    std::array<Sc::SystemColor, 32*32> tileColors {};
    std::array<Sc::SystemColor, 32*32> otherTileColors {};
    if ( !renderTile(tiles, mtxmTileValue, tileColors) || !renderTile(tiles, otherMtxmTileValue, otherTileColors) )
        throw std::runtime_error("Failed to render tiles");

    double colorDifferenceSum = 0;
    for ( std::size_t y=0; y<32; ++y )
    {
        for ( std::size_t x=0; x<32; ++x )
        {
            std::size_t i = y*32+x;
            Sc::SystemColor pxColor = tileColors[i];
            Sc::SystemColor otherPxColor = otherTileColors[i];
            colorDifferenceSum += std::sqrt(
                std::pow(double(pxColor.red) - double(otherPxColor.red), 2) +
                std::pow(double(pxColor.green) - double(otherPxColor.green), 2) +
                std::pow(double(pxColor.blue) - double(otherPxColor.blue), 2));
        }
    }
    return static_cast<std::uint32_t>(colorDifferenceSum);
}

void removeRemasteredDoodads(const Sc::Data & scData, Scenario & map)
{
    Sc::Terrain::Tileset tilesetIndex = Sc::Terrain::Tileset(map.getTileset() % Sc::Terrain::NumTilesets);
    const Sc::Terrain::Tiles & tiles = scData.terrain.get(tilesetIndex);

    std::vector<std::size_t> removedDoodadIndexes {};
    std::ptrdiff_t i = static_cast<std::ptrdiff_t>(map->doodads.size())-1;
    for ( ; i>=0; --i )
    {
        if ( tiles.isRemasteredDoodad(tilesetIndex, map->doodads[i].type) )
            removedDoodadIndexes.push_back(static_cast<std::size_t>(i));
    }

    if ( !removedDoodadIndexes.empty() )
    {
        auto edit = map.create_action(ActionDescriptor::DowngradeFromRemastered);
        edit->doodads.remove(removedDoodadIndexes);
    }
}

std::size_t downgradeSectionTiles(const Sc::Data & scData, Scenario & map, const std::vector<PalettedTile> & palettedMegaTiles, SectionName sectionName)
{
    std::size_t tileChangeCount = 0;
    std::vector<std::uint16_t> tileValues {}; 
    switch ( sectionName )
    {
        case SectionName::TILE: tileValues = map->editorTiles; break;
        case SectionName::MTXM: tileValues = map->tiles; break;
        default: throw std::invalid_argument("Only valid section names for tile downgrading are TILE and MTXM"); break;
    }

    Sc::Terrain::Tileset tilesetIndex = Sc::Terrain::Tileset(map.getTileset() % Sc::Terrain::NumTilesets);
    const Sc::Terrain::Tiles & tiles = scData.terrain.get(tilesetIndex);
    std::size_t totalTileValues = tileValues.size();

    std::vector<TileAlternative> replacementTiles {};
    replacementTiles.reserve(65536);
    for ( std::size_t tileValueIndex=0; tileValueIndex<totalTileValues; ++tileValueIndex )
    {
        std::uint16_t & tileValue = tileValues[tileValueIndex];
        if ( !Sc::Terrain::isRemasteredTile(tilesetIndex, tileValue) )
            continue; // Not remastered, nothing to downgrade

        std::uint16_t tileGroupIndex = Sc::Terrain::getTileGroup(tileValue);
        std::uint16_t tileGroupMemberIndex = Sc::Terrain::getSubtileValue(tileValue);
        if ( tileGroupIndex >= tiles.tileGroups.size() )
            continue; // Out of bounds tile group regardless of the version

        const Sc::Terrain::TileGroup & tileGroup = tiles.tileGroups[tileGroupIndex];
        std::uint16_t megaTileIndex = tileGroup.megaTileIndex[tileGroupMemberIndex];
        if ( megaTileIndex == 0 )
        {
            tileValue = 0; // Null tile, make sure it's using tileValue 0
            continue; // Move on to the next tile
        }

        constexpr std::uint64_t lowByte = 0xFF;
        std::uint64_t maxMatchScore = 0;
        replacementTiles.clear();
        for ( const auto & classicMtxmTileRange : Sc::Terrain::classicMtxmTileRanges[tilesetIndex] )
        {
            for ( std::size_t classicMtxmTileValue = classicMtxmTileRange.begin; classicMtxmTileValue < classicMtxmTileRange.end; ++classicMtxmTileValue )
            {
                std::uint16_t classicTileGroupIndex = Sc::Terrain::getTileGroup(classicMtxmTileValue);
                std::uint16_t classicTileGroupMemberIndex = Sc::Terrain::getSubtileValue(classicMtxmTileValue);
                if ( classicTileGroupIndex >= tiles.tileGroups.size() )
                    break;

                const Sc::Terrain::TileGroup & classicTileGroup = tiles.tileGroups[classicTileGroupIndex];
                std::uint16_t classicMegaTileIndex = classicTileGroup.megaTileIndex[classicTileGroupMemberIndex];
                
                bool unbuildableMatch = tileGroup.isBuildable() == classicTileGroup.isBuildable();
                bool creepMatch = tileGroup.isCreep() == classicTileGroup.isCreep();
                bool temporaryCreepMatch = tileGroup.isTemporaryCreep() == classicTileGroup.isTemporaryCreep();
                bool recedingCreepMatch = tileGroup.isRecedingCreep() == classicTileGroup.isRecedingCreep();
                bool megaTileMatch = megaTileIndex == classicMegaTileIndex;
                bool providesCoverMatch = tileGroup.providesCover() == classicTileGroup.providesCover();
                bool blocksViewMatch = tileGroup.blocksView() == classicTileGroup.blocksView();
                bool groupWalkableMatch = tileGroup.groupWalkable() == classicTileGroup.groupWalkable();
                bool groupUnwalkableMatch = tileGroup.groupUnwalkable() == classicTileGroup.groupUnwalkable();
                bool groupMidGroundMatch = tileGroup.groupMidGround() == classicTileGroup.groupMidGround();
                bool groupHighGroundMatch = tileGroup.groupHighGround() == classicTileGroup.groupHighGround();
                
                constexpr std::uint64_t true_ = 1;
                constexpr std::uint64_t false_ = 0;
                std::uint64_t matchScore = 0;
                matchScore |= unbuildableMatch ? true_ << std::uint64_t(63) : false_;
                matchScore |= creepMatch ? true_ << std::uint64_t(62) : false_;
                matchScore |= temporaryCreepMatch ? true_ <<std::uint64_t(61) : false_;
                matchScore |= recedingCreepMatch ? true_<<std::uint64_t(60) : false_;
                matchScore |= megaTileMatch ? true_ << std::uint64_t(59) : false_;
                if ( (matchScore & 0xF800000000000000) < (maxMatchScore & 0xF800000000000000) )
                    continue; // Match cannot be better than the max match so far

                // Scan mini-tile flags
                std::uint64_t walkabilityMatchCount = 0;
                std::uint64_t elevationMatchScore = 0;
                std::uint64_t rampMatchCount = 0;
                for ( std::size_t yMiniTile=0; yMiniTile<4; ++yMiniTile )
                {
                    for ( std::size_t xMiniTile=0; xMiniTile<4; ++xMiniTile )
                    {
                        Sc::Terrain::TileFlags::MiniTileFlags miniTileFlags = tiles.tileFlags[megaTileIndex].miniTileFlags[yMiniTile][xMiniTile];
                        Sc::Terrain::TileFlags::MiniTileFlags classicMiniTileFlags = tiles.tileFlags[classicMegaTileIndex].miniTileFlags[yMiniTile][xMiniTile];

                        bool walkabilityMatch = miniTileFlags.isWalkable() == classicMiniTileFlags.isWalkable();
                        bool rampMatch = miniTileFlags.isRamp() == classicMiniTileFlags.isRamp();
                        walkabilityMatchCount += walkabilityMatch ? 1 : 0;
                        rampMatchCount += rampMatch ? 1 : 0;

                        Sc::Terrain::TileElevation tileElevation = miniTileFlags.getElevation();
                        Sc::Terrain::TileElevation classicTileElevation = classicMiniTileFlags.getElevation();

                        if ( tileElevation == classicTileElevation )
                            elevationMatchScore += 2;
                        else
                            elevationMatchScore += std::uint64_t(2-std::abs(int(tileElevation)-int(classicTileElevation)));
                    }
                }
                
                matchScore |= walkabilityMatchCount << std::uint64_t(54);
                matchScore |= providesCoverMatch ? true_ << std::uint64_t(53) : false_;
                matchScore |= blocksViewMatch ? true_ << std::uint64_t(52) : false_;
                matchScore |= elevationMatchScore << std::uint64_t(44);
                if ( (matchScore & 0xFFFFF00000000000) < (maxMatchScore & 0xFFFFF00000000000) ) // Check including minitile properties and more group flags
                    continue; // Match cannot be better than the max match so far
                
                // Perform palette-index check to see if these tiles share the same exact graphics
                bool sameGraphics = palettedMegaTiles[megaTileIndex] == palettedMegaTiles[classicMegaTileIndex];
                matchScore |= sameGraphics ? true_ << std::uint64_t(43) : false_;
                // (the next 2 bits of matchScore are presently unused)
                
                if ( (matchScore & 0xFFFFF80000000000) < (maxMatchScore & 0xFFFFF80000000000) ) // Check including paletted check for graphical equivalence
                    continue; // Match cannot be better than the max match so far

                // At this point in the matchScore the full-tile graphics must be considered, do not perform any more short-circuiting till the end
                // matchScore |= std::uint64_t(graphicsMatchScore) << std::uint64_t(8)

                maxMatchScore = std::max(maxMatchScore, matchScore); // The lowByte is not considered part of the score till after graphics is applied

                // Apply the lowest 8-bytes to the score then add the score to the matches
                matchScore |= rampMatchCount << std::uint64_t(4);
                matchScore |= groupWalkableMatch ? true_ << std::uint64_t(3) : false_;
                matchScore |= groupUnwalkableMatch ? true_ << std::uint64_t(2) : false_;
                matchScore |= groupMidGroundMatch ? true_ << std::uint64_t(1) : false_;
                matchScore |= groupHighGroundMatch ? true_ << std::uint64_t(0) : false_;

                replacementTiles.emplace_back(std::uint16_t(classicMtxmTileValue), matchScore);
            }
        }

        if ( replacementTiles.empty() )
        {
            tileValue = 0;
            continue;
        }
        
        std::sort(replacementTiles.begin(), replacementTiles.end());

        constexpr std::uint64_t preGraphicsMatchScoreMask = 0xFFFFFF0000000000; // Sheer off the low-byte
        std::uint64_t highestScore = (replacementTiles[0].matchScore & preGraphicsMatchScoreMask);
        if ( replacementTiles.size() >= 2 && highestScore == (replacementTiles[1].matchScore & preGraphicsMatchScoreMask) ) // Multiple tiles scored equal
        {
            // Resolve color differences for the replacement tiles with relevant match scores
            std::size_t i=0;
            for ( ; i<replacementTiles.size(); ++i )
            {
                if ( (replacementTiles[i].matchScore & preGraphicsMatchScoreMask) < highestScore )
                    break; // This tile and those after it all score too low to be affected by graphic evaluation

                std::uint32_t colorDifference = calcTileColorDifference(tiles, palettedMegaTiles, tileValue, replacementTiles[i].altMtxmTileValue);
                std::uint32_t colorScore = std::numeric_limits<std::uint32_t>::max()-colorDifference;
                replacementTiles[i].matchScore |= std::uint64_t(colorScore) << std::uint64_t(9);
            }
            if ( i == replacementTiles.size() )
                std::sort(replacementTiles.begin(), replacementTiles.end()); // Sort entire vector again
            else
                std::sort(replacementTiles.begin(), std::next(replacementTiles.begin(), std::ptrdiff_t(i))); // Sort just the portion of the vector that was updated
        }

        std::uint16_t newTileValue = replacementTiles[0].altMtxmTileValue;
        if ( tileValue != newTileValue )
        {
            tileValue = replacementTiles[0].altMtxmTileValue;
            ++tileChangeCount;
        }
    }

    if ( tileChangeCount > 0 )
    {
        auto edit = map.create_action(ActionDescriptor::DowngradeFromRemastered);
        switch ( sectionName )
        {
            case SectionName::TILE: edit->editorTiles = tileValues; break;
            case SectionName::MTXM: edit->tiles = tileValues; break;
            default: throw std::logic_error("Unreachable"); break;
        }
    }
    return tileChangeCount;
}

void downgradeRemasteredMap()
{
    if ( CM == nullptr )
        return;

    GuiMap & map = *CM;

    if ( !chkd.scData )
    {
        logger.error() << "Can only downgrade tiles when remastered data is loaded." << std::endl;
        mb("Can only downgrade tiles when remastered data is loaded.");
    }

    const auto & scData = *chkd.scData;
    if ( !scData.isRemastered() )
    {
        logger.error() << "Can only downgrade tiles when remastered data is loaded." << std::endl;
        mb("Can only downgrade tiles when remastered data is loaded.");
        return;
    }

    Sc::Terrain::Tileset tilesetIndex = Sc::Terrain::Tileset(map.getTileset() % Sc::Terrain::NumTilesets);
    std::vector<PalettedTile> palettedMegaTiles = getPalettedMegatiles(scData.terrain.get(tilesetIndex));

    auto edit = map.create_action(ActionDescriptor::DowngradeFromRemastered);
    removeRemasteredDoodads(scData, map);
    downgradeSectionTiles(scData, map, palettedMegaTiles, SectionName::TILE);
    downgradeSectionTiles(scData, map, palettedMegaTiles, SectionName::MTXM);
}
