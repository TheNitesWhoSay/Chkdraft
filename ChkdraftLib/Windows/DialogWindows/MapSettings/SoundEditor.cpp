#include "SoundEditor.h"
#include "../../../Chkdraft.h"
#include "../../../Mapping/DataFileBrowsers.h"
#include "../../../../MappingCoreLib/MappingCore.h"

enum_t(Id, u32, {
    TEXT_MAPSOUNDFILES = ID_FIRST,
    BUTTON_STOPSOUNDS,
    BUTTON_DELETESOUND,
    BUTTON_EXTRACTSOUND,
    BUTTON_PLAYSOUND,
    LB_MAPSOUNDS,
    TEXT_VIRTUALSOUNDS,
    BUTTON_PLAYVIRTUALSOUND,
    LB_VIRTUALSOUNDS,
    TEXT_SOUNDFILENAME,
    EDIT_SOUNDFILENAME,
    BUTTON_BROWSEFORSOUND,
    TEXT_COMPRESSIONLEVEL,
    DROP_COMPRESSIONQUALITY,
    CHECK_VIRTUALFILE,
    BUTTON_ADDFILE,
    CHECK_CUSTOMMPQPATH,
    TEXT_CUSTOMMPQSTRING,
    DROP_CUSTOMMPQPATH
});

SoundEditorWindow::SoundEditorWindow() : wavQuality(WavQuality::Uncompressed), selectedSoundListIndex(-1), soundListDc(std::nullopt)
{

}

SoundEditorWindow::~SoundEditorWindow()
{

}

bool SoundEditorWindow::CreateThis(HWND hParent, u64 windowId)
{
    if ( getHandle() != NULL )
        return SetParent(hParent);

    if ( ClassWindow::RegisterWindowClass(0, NULL, NULL, NULL, NULL, "SoundEditor", NULL, false) &&
         ClassWindow::CreateClassWindow(0, "SoundEditor", WS_VISIBLE|WS_CHILD, 4, 22, 592, 524, hParent, (HMENU)windowId) )
    {
        CreateSubWindows(getHandle());
        return true;
    }
    else
        return false;
}

bool SoundEditorWindow::DestroyThis()
{
    ClassWindow::DestroyThis();
    this->wavQuality = WavQuality::Uncompressed;
    this->selectedSoundListIndex = -1;
    this->soundListDc = std::nullopt;
    return true;
}

void SoundEditorWindow::RefreshWindow()
{
    soundMap.clear();
    for ( size_t i=0; i<Chk::TotalSounds; i++ )
    {
        size_t soundStringId = CM->getSoundStringId(i);
        if ( soundStringId != Chk::StringId::UnusedSound )
            soundMap.insert(std::pair<u32, u16>((u32)soundStringId, (u16)i));
    }
    for ( size_t i=0; i<CM->numTriggers(); i++ )
    {
        const Chk::Trigger & trigger = CM->getTrigger(i);
        for ( size_t actionIndex = 0; actionIndex < Chk::Trigger::MaxActions; actionIndex++ )
        {
            if ( (trigger.actions[actionIndex].actionType == Chk::Action::Type::PlaySound ||
                trigger.actions[actionIndex].actionType == Chk::Action::Type::Transmission) &&
                trigger.actions[actionIndex].soundStringId != Chk::StringId::NoString )
            {
                soundMap.insert(std::pair<u32, u16>((u32)trigger.actions[actionIndex].soundStringId, u16_max));
            }
        }
    }
    for ( size_t i=0; i<CM->numBriefingTriggers(); i++ )
    {
        const Chk::Trigger & trigger = CM->getBriefingTrigger(i);
        for ( size_t actionIndex = 0; actionIndex < Chk::Trigger::MaxActions; actionIndex++ )
        {
            if ( (trigger.actions[actionIndex].actionType == Chk::Action::Type::BriefingPlaySound ||
                trigger.actions[actionIndex].actionType == Chk::Action::Type::BriefingTransmission) &&
                trigger.actions[actionIndex].soundStringId != Chk::StringId::NoString )
            {
                soundMap.insert(std::pair<u32, u16>((u32)trigger.actions[actionIndex].soundStringId, u16_max));
            }
        }
    }

    listMapSounds.SetRedraw(false);
    listMapSounds.ClearItems();
    for ( auto pair : soundMap )
    {
        u32 soundStringId = pair.first;
        listMapSounds.AddItem(soundStringId);
    }
    listMapSounds.SetRedraw(true);

    if ( selectedSoundListIndex > 0 && selectedSoundListIndex < listMapSounds.GetNumItems() )
        listMapSounds.SetCurSel(selectedSoundListIndex);

    if ( selectedSoundListIndex == -1 )
    {
        buttonDeleteSound.DisableThis();
        buttonExtractSound.DisableThis();
        buttonPlaySound.DisableThis();
    }
    else
    {
        buttonDeleteSound.EnableThis();
        buttonExtractSound.EnableThis();
        buttonPlaySound.EnableThis();
    }

    UpdateWindowText();
    UpdateCustomStringList();
}

void SoundEditorWindow::UpdateWindowText()
{
    u32 soundStringId = 0;
    if ( selectedSoundListIndex >= 0 && listMapSounds.GetItemData(selectedSoundListIndex, soundStringId) )
    {
        u16 soundIndex = u16_max;
        auto soundEntry = soundMap.find(soundStringId);
        if ( soundEntry != soundMap.end() )
            soundIndex = soundEntry->second;

        SoundStatus soundStatus = CM->getSoundStatus(soundStringId);

        std::string soundStatusString = "";
        if ( soundStatus == SoundStatus::NoMatch )
            soundStatusString = ", Status: No Matching Sound File";
        else if ( soundStatus == SoundStatus::NoMatchExtended )
            soundStatusString = ", Status: Extended, cannot match";
        else if ( soundStatus == SoundStatus::FileInUse )
            soundStatusString = ", Status: Unknown - Map Locked";
        else if ( soundStatus == SoundStatus::Unknown )
            soundStatusString = ", Status: ???";
        else if ( soundStatus == SoundStatus::CurrentMatch )
            soundStatusString = ", Status: Matches File In Map";
        else if ( soundStatus == SoundStatus::VirtualFile )
            soundStatusString = ", Status: Virtual File";
        else if ( soundStatus == SoundStatus::PendingMatch )
            soundStatusString = ", Status: Matches Pending Asset";

        if ( soundIndex == u16_max )
        {
            chkd.mapSettingsWindow.SetWinText("Map Settings - [Sound #null, String #" + std::to_string(soundStringId) + soundStatusString + "]");
        }
        else
        {
            chkd.mapSettingsWindow.SetWinText("Map Settings - [Sound #" + std::to_string(soundIndex) + ", String #" + std::to_string(soundStringId) + soundStatusString + "]");
        }
    }
    else
        chkd.mapSettingsWindow.SetWinText("Map Settings");
}

void SoundEditorWindow::UpdateCustomStringList()
{
    if ( checkCustomMpqString.isChecked() )
    {
        dropCustomMpqString.ClearItems();
        std::bitset<Chk::MaxStrings> stringIdUsed;
        CM->markValidUsedStrings(stringIdUsed, Chk::StrScope::Either, Chk::StrScope::Game);
        size_t stringCapacity = CM->getCapacity(Chk::StrScope::Game);
        for ( size_t stringId=1; stringId<stringCapacity; stringId++ )
        {
            if ( auto str = CM->getString<SingleLineChkdString>(stringId, Chk::StrScope::Game) )
                dropCustomMpqString.AddItem(*str);
        }
        dropCustomMpqString.EnableThis();
    }
    else
    {
        dropCustomMpqString.ClearItems();
        dropCustomMpqString.DisableThis();
    }
}

void SoundEditorWindow::PlaySoundButtonPressed() // TODO: Support for playing and stopping oggs
{
    u32 soundStringId = 0;
    if ( selectedSoundListIndex >= 0 && listMapSounds.GetItemData(selectedSoundListIndex, soundStringId) )
    {
        if ( auto soundData = CM->getSound(soundStringId) ) // Non-virtual sound
        {
#ifdef UNICODE
            PlaySoundW((LPCTSTR)&soundData.value()[0], NULL, SND_ASYNC|SND_MEMORY);
#else
            PlaySoundA((LPCTSTR)&soundData.value()[0], NULL, SND_ASYNC|SND_MEMORY);
#endif
        }
        else if ( auto soundString = CM->getString<RawString>(soundStringId) ) // Might be a virtual sound
        {
            if ( CM->isInVirtualSoundList(*soundString) ) // Is a virtual sound
            {
                if ( auto soundData = Sc::Data::GetAsset(*soundString, Sc::DataFile::BrowserPtr(new ChkdDataFileBrowser()),
                    ChkdDataFileBrowser::getDataFileDescriptors(), ChkdDataFileBrowser::getExpectedStarCraftDirectory()) )
                {
#ifdef UNICODE
                    PlaySoundW((LPCTSTR)&soundData.value()[0], NULL, SND_ASYNC|SND_MEMORY);
#else
                    PlaySoundA((LPCTSTR)&soundData.value()[0], NULL, SND_ASYNC|SND_MEMORY);
#endif
                }
                else
                    Error("Failed to get sound buffer!");
            }
            else // Valid string but neither a valid sound in the map file nor a valid virtual sound
                Error("Could not find a sound with the given name!");
        }
        else
            Error("Failed to get string value of sound!");
    }
    else
        Error("Failed to get string ID of sound!");
}

void SoundEditorWindow::PlayVirtualSoundButtonPressed()
{
    int sel = 0;
    std::string soundPath = "";
    if ( listVirtualSounds.GetCurSelString(soundPath) )
    {
        if ( auto soundData = Sc::Data::GetAsset(soundPath,
            Sc::DataFile::BrowserPtr(new ChkdDataFileBrowser()), ChkdDataFileBrowser::getDataFileDescriptors(),
            ChkdDataFileBrowser::getExpectedStarCraftDirectory()) )
        {
#ifdef UNICODE
            PlaySoundW((LPCTSTR)&soundData.value()[0], NULL, SND_ASYNC|SND_MEMORY);
#else
            PlaySoundA((LPCTSTR)&soundData.value()[0], NULL, SND_ASYNC|SND_MEMORY);
#endif
        }
        else
            Error("Selected sound not found in virtual sound list!");
    }
    else
        Error("Failed to get selected virtual sound string!");
}

void SoundEditorWindow::ExtractSoundButtonPressed()
{
    if ( !listMapSounds.GetCurSel(selectedSoundListIndex) )
        selectedSoundListIndex = -1;

    u32 soundStringId = 0;
    if ( selectedSoundListIndex >= 0 && listMapSounds.GetItemData(selectedSoundListIndex, soundStringId) )
    {
        if ( auto soundArchivePath = CM->getString<RawString>(soundStringId) )
        {
            u32 filterIndex = 0;
            std::string saveFilePath = getMpqFileName(*soundArchivePath);
            FileBrowserPtr<u32> fileBrowser = getDefaultSoundSaver();
            if ( fileBrowser->browseForSavePath(saveFilePath, filterIndex) )
            {
                SoundStatus soundStatus = CM->getSoundStatus(soundStringId);
                if ( soundStatus == SoundStatus::VirtualFile )
                {
                    if ( !Sc::Data::ExtractAsset(*soundArchivePath, saveFilePath, Sc::DataFile::BrowserPtr(new ChkdDataFileBrowser()),
                        ChkdDataFileBrowser::getDataFileDescriptors(), ChkdDataFileBrowser::getExpectedStarCraftDirectory()) )
                    {
                        Error("Error Extracting Asset!");
                    }
                }
                else if ( !CM->extractMpqAsset(*soundArchivePath, saveFilePath) )
                    Error("Error Extracting Asset!");
            }
            else
                Error("Error Retrieving File Name.");
        }
        else
            Error("Error retrieving sound string.");
    }
}

void SoundEditorWindow::BrowseButtonPressed()
{
    OPENFILENAME ofn = { };
    std::vector<std::pair<std::string, std::string>> filtersAndLabels = {
        std::make_pair<std::string, std::string>("*.wav", "WAV File"),
        std::make_pair<std::string, std::string>("*.ogg", "Ogg File"),
        std::make_pair<std::string, std::string>("*.wav;*.ogg", "All StarCraft Compatible Sounds"),
        std::make_pair<std::string, std::string>("*.*", "All Files")
    };
    std::string initPath = makeSystemFilePath(Settings::starCraftPath, "Maps");

    u32 filterIndex = 2; // All StarCraft Compatible Sounds
    std::string soundFilePath;
    if ( browseForFile(soundFilePath, filterIndex, filtersAndLabels, initPath, "Open Sound", true, false) )
    {
        buttonPreviewPlaySound.DisableThis();
        listVirtualSounds.ClearSel();
        checkVirtualFile.SetCheck(false);
        checkVirtualFile.DisableThis();
        editFileName.SetWinText(soundFilePath);
    }
    else
        CHKD_ERR("Error Retrieving File Name.");
}

void SoundEditorWindow::AddFileButtonPressed()
{
    bool useVirtualFile = checkVirtualFile.isChecked();
    bool useCustomMpqString = checkCustomMpqString.isChecked();
    int sel = dropCustomMpqString.GetSel();
    std::string customMpqPath = "";
    if ( useCustomMpqString && !dropCustomMpqString.GetItemText(sel, customMpqPath) )
    {
        Error("Failed to get custom path string!");
        return;
    }

    bool addedSound = false;
    auto filePath = editFileName.GetWinText();
    if ( useVirtualFile && CM->isInVirtualSoundList(*filePath) )
    {
        if ( useCustomMpqString )
            addedSound = CM->addSound(*filePath, customMpqPath, wavQuality, true);
        else
            addedSound = CM->addSound(*filePath, wavQuality, true);
    }
    else if ( !useVirtualFile && findFile(*filePath) )
    {
        if ( useCustomMpqString )
            addedSound = CM->addSound(*filePath, customMpqPath, wavQuality, false);
        else
            addedSound = CM->addSound(*filePath, wavQuality, false);
    }
    else if ( !useVirtualFile && CM->isInVirtualSoundList(*filePath) )
    {
        if ( auto soundData = Sc::Data::GetAsset(*filePath, Sc::DataFile::BrowserPtr(new ChkdDataFileBrowser()),
            ChkdDataFileBrowser::getDataFileDescriptors(), ChkdDataFileBrowser::getExpectedStarCraftDirectory()) )
        {
            if ( useCustomMpqString )
                addedSound = CM->addSound(customMpqPath, *soundData, wavQuality);
            else
            {
                std::string mpqFileName = getMpqFileName(*filePath);
                std::string standardSoundDir = MapFile::GetStandardSoundDir();
                std::string mpqFilePath = makeMpqFilePath(standardSoundDir, mpqFileName);
                addedSound = CM->addSound(mpqFilePath, *soundData, wavQuality);
            }
        }
        else
            Error("Failed to extract virtual sound file!");
    }
    else
        Error("Sound Path Not Found!");

    if ( addedSound )
    {
        editFileName.SetText("");
        CM->notifyChange(false);
        CM->refreshScenario();
    }
}

void SoundEditorWindow::CheckVirtualFilePressed()
{
    if ( checkVirtualFile.isChecked() )
    {
        dropCompressionLevel.DisableThis();
        dropCompressionLevel.SetSel(0);
        buttonBrowse.DisableThis();
        checkCustomMpqString.SetCheck(false);
    }
    else
    {
        int sel = 0;
        switch ( wavQuality )
        {
        case WavQuality::Uncompressed: sel = 0; break;
        case WavQuality::Low: sel = 1; break;
        case WavQuality::Med: sel = 2; break;
        case WavQuality::High: sel = 3; break;
        }
        dropCompressionLevel.SetSel(sel);
        dropCompressionLevel.EnableThis();
        buttonBrowse.EnableThis();
    }
}

void SoundEditorWindow::CompressionDropdownChanged()
{
    int selection = dropCompressionLevel.GetSel();
    switch ( selection )
    {
        case 0: wavQuality = WavQuality::Uncompressed; break;
        case 1: wavQuality = WavQuality::Low; break;
        case 2: wavQuality = WavQuality::Med; break;
        case 3: wavQuality = WavQuality::High; break;
    }

    if ( wavQuality != WavQuality::Uncompressed )
    {
        checkVirtualFile.SetCheck(false);
        checkVirtualFile.DisableThis();
    }
    else
        checkVirtualFile.EnableThis();
}

void SoundEditorWindow::MapSoundSelectionChanged()
{
    listVirtualSounds.ClearSel();
    buttonPreviewPlaySound.DisableThis();
    if ( !listMapSounds.GetCurSel(selectedSoundListIndex) )
        selectedSoundListIndex = -1;

    if ( selectedSoundListIndex == -1 )
    {
        buttonDeleteSound.DisableThis();
        buttonExtractSound.DisableThis();
        buttonPlaySound.DisableThis();
    }
    else
    {
        buttonDeleteSound.EnableThis();
        u32 soundStringId = 0;
        if ( selectedSoundListIndex >= 0 && listMapSounds.GetItemData(selectedSoundListIndex, soundStringId) )
        {
            SoundStatus soundStatus = CM->getSoundStatus(soundStringId);
            if ( soundStatus == SoundStatus::PendingMatch || soundStatus == SoundStatus::CurrentMatch || soundStatus == SoundStatus::VirtualFile )
                buttonExtractSound.EnableThis();
        }
        buttonPlaySound.EnableThis();
    }

    UpdateWindowText();
}

void SoundEditorWindow::VirtualSoundSelectionChanged()
{
    selectedSoundListIndex = -1;
    listMapSounds.ClearSel();
    buttonPlaySound.DisableThis();
    UpdateWindowText();

    std::string virtualSoundPath = "";
    if ( listVirtualSounds.GetCurSelString(virtualSoundPath) )
    {
        dropCompressionLevel.SetSel(0);
        buttonPreviewPlaySound.EnableThis();
        editFileName.SetText(virtualSoundPath);
        checkVirtualFile.EnableThis();
        checkVirtualFile.SetCheck(true);
        checkCustomMpqString.SetCheck(false);
        dropCustomMpqString.DisableThis();
    }
}

void SoundEditorWindow::StopSoundsButtonPressed()
{
#ifdef UNICODE
    PlaySoundW(NULL, NULL, 0);
#else
    PlaySoundA(NULL, NULL, 0);
#endif
}

void SoundEditorWindow::DeleteSoundButtonPressed()
{
    u32 soundStringId = 0;
    if ( listMapSounds.GetItemData(selectedSoundListIndex, soundStringId) )
    {
        bool soundStringIdIsUsed = false;
        for ( size_t i=0; i<CM->numTriggers(); i++ )
        {
            const Chk::Trigger & trigger = CM->getTrigger(i);
            for ( size_t actionIndex = 0; actionIndex < Chk::Trigger::MaxActions; actionIndex++ )
            {
                if ( (trigger.actions[actionIndex].actionType == Chk::Action::Type::PlaySound ||
                    trigger.actions[actionIndex].actionType == Chk::Action::Type::Transmission) &&
                    trigger.actions[actionIndex].soundStringId == soundStringId )
                {
                    soundStringIdIsUsed = true;
                    i = CM->numTriggers();
                    break;
                }
            }
        }
        if ( !soundStringIdIsUsed )
        {
            for ( size_t i=0; i<CM->numBriefingTriggers(); i++ )
            {
                const Chk::Trigger & trigger = CM->getBriefingTrigger(i);
                for ( size_t actionIndex = 0; actionIndex < Chk::Trigger::MaxActions; actionIndex++ )
                {
                    if ( (trigger.actions[actionIndex].actionType == Chk::Action::Type::BriefingPlaySound ||
                        trigger.actions[actionIndex].actionType == Chk::Action::Type::BriefingTransmission) &&
                        trigger.actions[actionIndex].soundStringId == soundStringId )
                    {
                        soundStringIdIsUsed = true;
                        i = CM->numBriefingTriggers();
                        break;
                    }
                }
            }
        }


        if ( soundStringIdIsUsed )
        {
            SoundStatus soundStatus = CM->getSoundStatus(soundStringId);
            if ( soundStatus == SoundStatus::NoMatch || soundStatus == SoundStatus::NoMatchExtended )
            {
                selectedSoundListIndex = -1;
                CM->removeSoundByStringId(soundStringId, true);
                CM->notifyChange(false);
                CM->refreshScenario();
            }
            else
            {
                std::string warningMessage = "This sound is currently using in triggers and/or briefing";
                if ( soundStatus == SoundStatus::CurrentMatch )
                    warningMessage += " and is present in the MPQ";
                else if ( soundStatus == SoundStatus::PendingMatch )
                    warningMessage += " and is valid";
                else if ( soundStatus == SoundStatus::FileInUse )
                    warningMessage += " and whether it is in the map MPQ is unknown";
                else if ( soundStatus == SoundStatus::VirtualFile )
                    warningMessage += " and is a valid virtual file";
                warningMessage += ", are you sure you want to remove it?";

                if ( WinLib::GetYesNo(warningMessage, "Warning!") == WinLib::PromptResult::Yes )
                {
                    selectedSoundListIndex = -1;
                    CM->removeSoundByStringId(soundStringId, true);
                    CM->notifyChange(false);
                    CM->refreshScenario();
                }
            }
        }
        else
        {
            selectedSoundListIndex = -1;
            CM->removeSoundByStringId(soundStringId, false);
            CM->notifyChange(false);
            CM->refreshScenario();
        }
        CM->deleteUnusedStrings(Chk::StrScope::Both);
    }
}

void SoundEditorWindow::CheckCustomMpqPathPressed()
{
    UpdateCustomStringList();
    checkVirtualFile.SetCheck(false);
    dropCompressionLevel.EnableThis();
}

LRESULT SoundEditorWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        case WM_SHOWWINDOW:
            if ( wParam == TRUE )
                RefreshWindow();
            break;

        case WM_MOUSEWHEEL:
            listMapSounds.SetTopIndex(listMapSounds.GetTopIndex()-(int((s16(HIWORD(wParam)))/WHEEL_DELTA)));
            break;

        case WinLib::LB::WM_PREMEASUREITEMS: // Measuring is time sensative, load necessary items for measuring all strings once
            soundListDc.emplace(listMapSounds.getHandle());
            break;

        case WM_MEASUREITEM:
        {
            MEASUREITEMSTRUCT* mis = (MEASUREITEMSTRUCT*)lParam;
            auto str = CM->getString<RawString>((size_t)mis->itemData);
            if ( str && GetStringDrawSize(*soundListDc, mis->itemWidth, mis->itemHeight, *str) )
            {
                mis->itemWidth += 5;
                mis->itemHeight += 2;

                if ( mis->itemHeight > 255 )
                    mis->itemHeight = 255;
            }
            return TRUE;
        }
        break;

        case WinLib::LB::WM_POSTMEASUREITEMS: // Release items loaded for measurement
            soundListDc = std::nullopt;
            break;

        case WinLib::LB::WM_PREDRAWITEMS:
            break;

        case WM_DRAWITEM:
        {
            PDRAWITEMSTRUCT pdis = (PDRAWITEMSTRUCT)lParam;
            bool isSelected = ((pdis->itemState&ODS_SELECTED) == ODS_SELECTED),
                drawSelection = ((pdis->itemAction&ODA_SELECT) == ODA_SELECT),
                drawEntire = ((pdis->itemAction&ODA_DRAWENTIRE) == ODA_DRAWENTIRE);

            if ( pdis->itemID != -1 && ( drawSelection || drawEntire ) )
            {
                WinLib::DeviceContext dc { pdis->hDC };
                auto str = CM->getString<RawString>((size_t)pdis->itemData);
                if ( CM != nullptr && str )
                {
                    dc.fillRect(pdis->rcItem, RGB(0, 0, 0)); // Same color as in WM_CTLCOLORLISTBOX
                    SetBkMode(pdis->hDC, TRANSPARENT);
                    DrawString(dc, pdis->rcItem.left+3, pdis->rcItem.top+1, pdis->rcItem.right-pdis->rcItem.left,
                        RGB(16, 252, 24), *str);
                }
                if ( isSelected )
                    DrawFocusRect(pdis->hDC, &pdis->rcItem);
            }
            return TRUE;
        }
        break;

        case WinLib::LB::WM_POSTDRAWITEMS:
            break;

        default:
            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;
    }
    return 0;
}

void SoundEditorWindow::NotifyButtonClicked(int idFrom, HWND hWndFrom)
{
    switch ( idFrom )
    {
    case Id::BUTTON_PLAYSOUND: PlaySoundButtonPressed(); break;
    case Id::BUTTON_PLAYVIRTUALSOUND: PlayVirtualSoundButtonPressed(); break;
    case Id::BUTTON_EXTRACTSOUND: ExtractSoundButtonPressed(); break;
    case Id::BUTTON_BROWSEFORSOUND: BrowseButtonPressed(); break;
    case Id::BUTTON_ADDFILE: AddFileButtonPressed(); break;
    case Id::CHECK_VIRTUALFILE: CheckVirtualFilePressed(); break;
    case Id::BUTTON_STOPSOUNDS: StopSoundsButtonPressed(); break;
    case Id::BUTTON_DELETESOUND: DeleteSoundButtonPressed(); break;
    case Id::CHECK_CUSTOMMPQPATH: CheckCustomMpqPathPressed(); break;
    }
}

void SoundEditorWindow::NotifyComboSelChanged(int idFrom, HWND hWndFrom)
{
    switch ( idFrom )
    {
    case Id::DROP_COMPRESSIONQUALITY: CompressionDropdownChanged(); break;
    case Id::LB_MAPSOUNDS: MapSoundSelectionChanged(); break;
    case Id::LB_VIRTUALSOUNDS: VirtualSoundSelectionChanged(); break;
    }
}

void SoundEditorWindow::CreateSubWindows(HWND hWnd)
{
    textMapSoundFiles.CreateThis(hWnd, 5, 5, 100, 20, "Map Sound Files", Id::TEXT_MAPSOUNDFILES);
    buttonStopSounds.CreateThis(hWnd, 127, 3, 110, 20, "Stop Sounds", Id::BUTTON_STOPSOUNDS);
    buttonDeleteSound.CreateThis(hWnd, 242, 3, 110, 20, "Delete Selected", Id::BUTTON_DELETESOUND);
    buttonExtractSound.CreateThis(hWnd, 357, 3, 110, 20, "Extract Selected", Id::BUTTON_EXTRACTSOUND);
    buttonPlaySound.CreateThis(hWnd, 472, 3, 110, 20, "Play Selected", Id::BUTTON_PLAYSOUND);
    listMapSounds.CreateThis(hWnd, 5, 25, 582, 188, true, false, false, false, Id::LB_MAPSOUNDS);

    textAvailableSounds.CreateThis(hWnd, 5, 219, 200, 20, "Available MPQ sound files (Virtual Sounds)", Id::TEXT_VIRTUALSOUNDS);
    buttonPreviewPlaySound.CreateThis(hWnd, 432, 217, 150, 20, "Play Selected", Id::BUTTON_PLAYVIRTUALSOUND);
    listVirtualSounds.CreateThis(hWnd, 5, 239, 582, 200, false, false, true, true, Id::LB_VIRTUALSOUNDS);
    textFileName.CreateThis(hWnd, 5, 434, 100, 20, "Filename", Id::TEXT_SOUNDFILENAME);
    editFileName.CreateThis(hWnd, 140, 434, 352, 20, false, Id::EDIT_SOUNDFILENAME);
    buttonBrowse.CreateThis(hWnd, 502, 434, 80, 20, "Browse", Id::BUTTON_BROWSEFORSOUND);
    textCompressionLevel.CreateThis(hWnd, 5, 459, 100, 20, "Compression Level", Id::TEXT_COMPRESSIONLEVEL);
    const std::vector<std::string> compressionLevels = { "Uncompressed", "Low Quality", "Medium Quality", "High Quality" };
    dropCompressionLevel.CreateThis(hWnd, 140, 459, 150, 200, false, false, Id::DROP_COMPRESSIONQUALITY, compressionLevels);
    dropCompressionLevel.SetSel(0);
    checkVirtualFile.CreateThis(hWnd, 300, 459, 100, 20, false, "Virtual File", Id::CHECK_VIRTUALFILE);
    buttonAddFile.CreateThis(hWnd, 502, 459, 80, 20, "Add File", Id::BUTTON_ADDFILE);
    textCustomMpqString.CreateThis(hWnd, 5, 484, 100, 20, "Custom MPQ Path", Id::TEXT_CUSTOMMPQSTRING);
    dropCustomMpqString.CreateThis(hWnd, 140, 484, 150, 200, true, true, Id::DROP_CUSTOMMPQPATH, {});
    checkCustomMpqString.CreateThis(hWnd, 300, 484, 150, 20, false, "Use Custom Path", Id::CHECK_CUSTOMMPQPATH);

    size_t numVirtualSounds = Sc::Sound::virtualSoundPaths.size();
    for ( size_t i=0; i<numVirtualSounds; i++ )
        listVirtualSounds.AddString(Sc::Sound::virtualSoundPaths[i]);

    buttonPlaySound.DisableThis();
    
    buttonExtractSound.DisableThis();

    buttonPreviewPlaySound.DisableThis();
    checkVirtualFile.DisableThis();
}

void SoundEditorWindow::DisableMapSoundEditing()
{
    textMapSoundFiles.DisableThis();
    buttonStopSounds.DisableThis();
    buttonDeleteSound.DisableThis();
    buttonExtractSound.DisableThis();
    buttonPlaySound.DisableThis();
    listMapSounds.DisableThis();
}

void SoundEditorWindow::EnableMapSoundEditing()
{
    textMapSoundFiles.EnableThis();
    buttonStopSounds.EnableThis();
    buttonDeleteSound.EnableThis();
    buttonExtractSound.EnableThis();
    buttonPlaySound.EnableThis();
    listMapSounds.EnableThis();
}

void SoundEditorWindow::DisableSoundCustomization()
{
    textCompressionLevel.DisableThis();
    dropCompressionLevel.DisableThis();
    checkVirtualFile.DisableThis();
    buttonAddFile.DisableThis();
}

void SoundEditorWindow::EnableSoundCustomization()
{
    textCompressionLevel.EnableThis();
    dropCompressionLevel.EnableThis();
    checkVirtualFile.EnableThis();
    buttonAddFile.EnableThis();
}

FileBrowserPtr<u32> SoundEditorWindow::getDefaultSoundBrowser()
{
    return FileBrowserPtr<u32>(new FileBrowser<u32>(getSoundFilters(), "Open Sound", true, false));
}

FileBrowserPtr<u32> SoundEditorWindow::getDefaultSoundSaver()
{
    return FileBrowserPtr<u32>(new FileBrowser<u32>(getSaveSoundFilters(), "Save Sound", true, true));
}
