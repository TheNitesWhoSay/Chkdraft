#include "WavEditor.h"
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

WavEditorWindow::WavEditorWindow() : wavQuality(WavQuality::Uncompressed), selectedSoundListIndex(-1), wavListDC(NULL)
{

}

WavEditorWindow::~WavEditorWindow()
{

}

bool WavEditorWindow::CreateThis(HWND hParent, u64 windowId)
{
    if ( getHandle() != NULL )
        return SetParent(hParent);

    if ( ClassWindow::RegisterWindowClass(0, NULL, NULL, NULL, NULL, "WavEditor", NULL, false) &&
         ClassWindow::CreateClassWindow(0, "WavEditor", WS_VISIBLE|WS_CHILD, 4, 22, 592, 524, hParent, (HMENU)windowId) )
    {
        CreateSubWindows(getHandle());
        return true;
    }
    else
        return false;
}

void WavEditorWindow::RefreshWindow()
{
    wavMap.clear();
    for ( size_t i=0; i<Chk::TotalSounds; i++ )
    {
        size_t soundStringId = CM->triggers.getSoundStringId(i);
        if ( soundStringId != Chk::StringId::UnusedSound )
            wavMap.insert(std::pair<u32, u16>((u32)soundStringId, (u16)i));
    }
    for ( size_t i=0; i<CM->triggers.numTriggers(); i++ )
    {
        Chk::TriggerPtr trigger = CM->triggers.getTrigger(i);
        for ( size_t actionIndex = 0; actionIndex < Chk::Trigger::MaxActions; actionIndex++ )
        {
            if ( (trigger->actions[actionIndex].actionType == Chk::Action::Type::PlaySound ||
                trigger->actions[actionIndex].actionType == Chk::Action::Type::Transmission) &&
                trigger->actions[actionIndex].soundStringId != Chk::StringId::NoString )
            {
                wavMap.insert(std::pair<u32, u16>((u32)trigger->actions[actionIndex].soundStringId, u16_max));
            }
        }
    }
    for ( size_t i=0; i<CM->triggers.numBriefingTriggers(); i++ )
    {
        Chk::TriggerPtr trigger = CM->triggers.getBriefingTrigger(i);
        for ( size_t actionIndex = 0; actionIndex < Chk::Trigger::MaxActions; actionIndex++ )
        {
            if ( (trigger->actions[actionIndex].actionType == Chk::Action::Type::BriefingPlaySound ||
                trigger->actions[actionIndex].actionType == Chk::Action::Type::BriefingTransmission) &&
                trigger->actions[actionIndex].soundStringId != Chk::StringId::NoString )
            {
                wavMap.insert(std::pair<u32, u16>((u32)trigger->actions[actionIndex].soundStringId, u16_max));
            }
        }
    }

    listMapSounds.SetRedraw(false);
    listMapSounds.ClearItems();
    for ( auto pair : wavMap )
    {
        u32 wavStringId = pair.first;
        listMapSounds.AddItem(wavStringId);
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

void WavEditorWindow::UpdateWindowText()
{
    u32 wavStringId = 0;
    if ( selectedSoundListIndex >= 0 && listMapSounds.GetItemData(selectedSoundListIndex, wavStringId) )
    {
        u16 wavIndex = u16_max;
        auto wavEntry = wavMap.find(wavStringId);
        if ( wavEntry != wavMap.end() )
            wavIndex = wavEntry->second;

        SoundStatus wavStatus = CM->GetSoundStatus(wavStringId);

        std::string wavStatusString = "";
        if ( wavStatus == SoundStatus::NoMatch )
            wavStatusString = ", Status: No Matching Sound File";
        else if ( wavStatus == SoundStatus::NoMatchExtended )
            wavStatusString = ", Status: Extended, cannot match";
        else if ( wavStatus == SoundStatus::FileInUse )
            wavStatusString = ", Status: Unknown - Map Locked";
        else if ( wavStatus == SoundStatus::Unknown )
            wavStatusString = ", Status: ???";
        else if ( wavStatus == SoundStatus::CurrentMatch )
            wavStatusString = ", Status: Matches File In Map";
        else if ( wavStatus == SoundStatus::VirtualFile )
            wavStatusString = ", Status: Virtual File";
        else if ( wavStatus == SoundStatus::PendingMatch )
            wavStatusString = ", Status: Matches Pending Asset";

        if ( wavIndex == u16_max )
        {
            chkd.mapSettingsWindow.SetWinText("Map Settings - [WAV #null, String #" + std::to_string(wavStringId) + wavStatusString + "]");
        }
        else
        {
            chkd.mapSettingsWindow.SetWinText("Map Settings - [WAV #" + std::to_string(wavIndex) + ", String #" + std::to_string(wavStringId) + wavStatusString + "]");
        }
    }
    else
        chkd.mapSettingsWindow.SetWinText("Map Settings");
}

void WavEditorWindow::UpdateCustomStringList()
{
    if ( checkCustomMpqString.isChecked() )
    {
        dropCustomMpqString.ClearItems();
        std::bitset<Chk::MaxStrings> stringIdUsed;
        CM->strings.markValidUsedStrings(stringIdUsed, Chk::Scope::Either, Chk::Scope::Game);
        size_t stringCapacity = CM->strings.getCapacity(Chk::Scope::Game);
        for ( size_t stringId=1; stringId<stringCapacity; stringId++ )
        {
            SingleLineChkdStringPtr str = CM->strings.getString<SingleLineChkdString>(stringId, Chk::Scope::Game);
            if ( str != nullptr )
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

void WavEditorWindow::PlaySoundButtonPressed()
{
    u32 soundStringId = 0;
    if ( selectedSoundListIndex >= 0 && listMapSounds.GetItemData(selectedSoundListIndex, soundStringId) )
    {
        std::vector<u8> wavBuffer;
        if ( CM->GetSound(soundStringId, wavBuffer) )
        {
#ifdef UNICODE
            PlaySoundW((LPCTSTR)&wavBuffer[0], NULL, SND_ASYNC|SND_MEMORY);
#else
            PlaySoundA((LPCTSTR)&wavBuffer[0], NULL, SND_ASYNC|SND_MEMORY);
#endif
        }
        else
        {
            RawStringPtr soundString = CM->strings.getString<RawString>(soundStringId);
            if ( soundString != nullptr && CM->IsInVirtualSoundList(*soundString) &&
                Sc::Data::GetAsset(*soundString, wavBuffer, Sc::DataFile::BrowserPtr(new ChkdDataFileBrowser()),
                    ChkdDataFileBrowser::getDataFileDescriptors(), ChkdDataFileBrowser::getExpectedStarCraftDirectory()) )
            {
#ifdef UNICODE
                PlaySoundW((LPCTSTR)&wavBuffer[0], NULL, SND_ASYNC|SND_MEMORY);
#else
                PlaySoundA((LPCTSTR)&wavBuffer[0], NULL, SND_ASYNC|SND_MEMORY);
#endif
            }
            else
                Error("Failed to get WAV buffer!");
        }
    }
    else
        Error("Failed to get string ID of WAV!");
}

void WavEditorWindow::PlayVirtualSoundButtonPressed()
{
    std::vector<u8> wavBuffer;
    int sel = 0;
    std::string wavString = "";
    if ( listVirtualSounds.GetCurSelString(wavString) )
    {
        if ( CM->IsInVirtualSoundList(wavString) && Sc::Data::GetAsset(wavString, wavBuffer,
            Sc::DataFile::BrowserPtr(new ChkdDataFileBrowser()), ChkdDataFileBrowser::getDataFileDescriptors(),
            ChkdDataFileBrowser::getExpectedStarCraftDirectory()) )
        {
#ifdef UNICODE
            PlaySoundW((LPCTSTR)&wavBuffer[0], NULL, SND_ASYNC|SND_MEMORY);
#else
            PlaySoundA((LPCTSTR)&wavBuffer[0], NULL, SND_ASYNC|SND_MEMORY);
#endif
        }
    }
    else
        Error("Failed to get selected virtual WAV string!");
}

void WavEditorWindow::ExtractSoundButtonPressed()
{
    if ( !listMapSounds.GetCurSel(selectedSoundListIndex) )
        selectedSoundListIndex = -1;

    u32 soundStringId = 0;
    if ( selectedSoundListIndex >= 0 && listMapSounds.GetItemData(selectedSoundListIndex, soundStringId) )
    {
        RawStringPtr wavMpqPath = CM->strings.getString<RawString>(soundStringId);
        if ( wavMpqPath != nullptr )
        {
            u32 filterIndex = 0;
            std::string saveFilePath = getMpqFileName(*wavMpqPath);
            FileBrowserPtr<u32> fileBrowser = getDefaultSoundSaver();
            if ( fileBrowser->browseForSavePath(saveFilePath, filterIndex) )
            {
                SoundStatus soundStatus = CM->GetSoundStatus(soundStringId);
                if ( soundStatus == SoundStatus::VirtualFile )
                {
                    if ( !Sc::Data::ExtractAsset(*wavMpqPath, saveFilePath, Sc::DataFile::BrowserPtr(new ChkdDataFileBrowser()),
                        ChkdDataFileBrowser::getDataFileDescriptors(), ChkdDataFileBrowser::getExpectedStarCraftDirectory()) )
                    {
                        Error("Error Extracting Asset!");
                    }
                }
                else if ( !CM->ExtractMpqAsset(*wavMpqPath, saveFilePath) )
                    Error("Error Extracting Asset!");
            }
            else
                Error("Error Retrieving File Name.");
        }
        else
            Error("Error retrieving wav string.");
    }
}

void WavEditorWindow::BrowseButtonPressed()
{
    OPENFILENAME ofn = { };
    std::vector<std::pair<std::string, std::string>> filtersAndLabels = { std::make_pair<std::string, std::string>("*.wav", "WAV File"), std::make_pair<std::string, std::string>("*.*", "All Files") };
    std::string initPath = Settings::starCraftPath + getSystemFileSeparator() + "Maps";

    u32 filterIndex = 0;
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

void WavEditorWindow::AddFileButtonPressed()
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

    bool addedWav = false;
    std::string filePath = editFileName.GetWinText();
    if ( useVirtualFile && CM->IsInVirtualSoundList(filePath) )
    {
        if ( useCustomMpqString )
            addedWav = CM->AddSound(filePath, customMpqPath, wavQuality, true);
        else
            addedWav = CM->AddSound(filePath, wavQuality, true);
    }
    else if ( !useVirtualFile && findFile(filePath) )
    {
        if ( useCustomMpqString )
            addedWav = CM->AddSound(filePath, customMpqPath, wavQuality, false);
        else
            addedWav = CM->AddSound(filePath, wavQuality, false);
    }
    else if ( !useVirtualFile && CM->IsInVirtualSoundList(filePath) )
    {
        std::vector<u8> wavContents;
        if ( Sc::Data::GetAsset(filePath, wavContents, Sc::DataFile::BrowserPtr(new ChkdDataFileBrowser()),
            ChkdDataFileBrowser::getDataFileDescriptors(), ChkdDataFileBrowser::getExpectedStarCraftDirectory()) )
        {
            if ( useCustomMpqString )
                addedWav = CM->AddSound(customMpqPath, wavContents, wavQuality);
            else
            {
                std::string mpqFileName = getMpqFileName(filePath);
                std::string standardWavDir = MapFile::GetStandardSoundDir();
                std::string mpqFilePath = makeMpqFilePath(standardWavDir, mpqFileName);
                addedWav = CM->AddSound(mpqFilePath, wavContents, wavQuality);
            }
        }
        else
            Error("Failed to extract virtual sound file!");
    }
    else
        Error("WAV Path Not Found!");

    if ( addedWav )
    {
        editFileName.SetText("");
        CM->notifyChange(false);
        CM->refreshScenario();
    }
}

void WavEditorWindow::CheckVirtualFilePressed()
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

void WavEditorWindow::CompressionDropdownChanged()
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

void WavEditorWindow::MapSoundSelectionChanged()
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
        u32 wavStringId = 0;
        if ( selectedSoundListIndex >= 0 && listMapSounds.GetItemData(selectedSoundListIndex, wavStringId) )
        {
            SoundStatus wavStatus = CM->GetSoundStatus(wavStringId);
            if ( wavStatus == SoundStatus::PendingMatch || wavStatus == SoundStatus::CurrentMatch || wavStatus == SoundStatus::VirtualFile )
                buttonExtractSound.EnableThis();
        }
        buttonPlaySound.EnableThis();
    }

    UpdateWindowText();
}

void WavEditorWindow::VirtualSoundSelectionChanged()
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

void WavEditorWindow::StopSoundsButtonPressed()
{
#ifdef UNICODE
    PlaySoundW(NULL, NULL, 0);
#else
    PlaySoundA(NULL, NULL, 0);
#endif
}

void WavEditorWindow::DeleteSoundButtonPressed()
{
    u32 wavStringId = 0;
    if ( listMapSounds.GetItemData(selectedSoundListIndex, wavStringId) )
    {
        bool wavStringIdIsUsed = false;
        for ( size_t i=0; i<CM->triggers.numTriggers(); i++ )
        {
            Chk::TriggerPtr trigger = CM->triggers.getTrigger(i);
            for ( size_t actionIndex = 0; actionIndex < Chk::Trigger::MaxActions; actionIndex++ )
            {
                if ( (trigger->actions[actionIndex].actionType == Chk::Action::Type::PlaySound ||
                    trigger->actions[actionIndex].actionType == Chk::Action::Type::Transmission) &&
                    trigger->actions[actionIndex].soundStringId == wavStringId )
                {
                    wavStringIdIsUsed = true;
                    i = CM->triggers.numTriggers();
                    break;
                }
            }
        }
        if ( !wavStringIdIsUsed )
        {
            for ( size_t i=0; i<CM->triggers.numBriefingTriggers(); i++ )
            {
                Chk::TriggerPtr trigger = CM->triggers.getBriefingTrigger(i);
                for ( size_t actionIndex = 0; actionIndex < Chk::Trigger::MaxActions; actionIndex++ )
                {
                    if ( (trigger->actions[actionIndex].actionType == Chk::Action::Type::BriefingPlaySound ||
                        trigger->actions[actionIndex].actionType == Chk::Action::Type::BriefingTransmission) &&
                        trigger->actions[actionIndex].soundStringId == wavStringId )
                    {
                        wavStringIdIsUsed = true;
                        i = CM->triggers.numBriefingTriggers();
                        break;
                    }
                }
            }
        }


        if ( wavStringIdIsUsed )
        {
            SoundStatus wavStatus = CM->GetSoundStatus(wavStringId);
            if ( wavStatus == SoundStatus::NoMatch || wavStatus == SoundStatus::NoMatchExtended )
            {
                selectedSoundListIndex = -1;
                CM->RemoveSoundByStringId(wavStringId, true);
                CM->notifyChange(false);
                CM->refreshScenario();
            }
            else
            {
                std::string warningMessage = "This WAV is currently using in triggers and/or briefing";
                if ( wavStatus == SoundStatus::CurrentMatch )
                    warningMessage += " and is present in the MPQ";
                else if ( wavStatus == SoundStatus::PendingMatch )
                    warningMessage += " and is valid";
                else if ( wavStatus == SoundStatus::FileInUse )
                    warningMessage += " and whether it is in the map MPQ is unknown";
                else if ( wavStatus == SoundStatus::VirtualFile )
                    warningMessage += " and is a valid virtual file";
                warningMessage += ", are you sure you want to remove it?";

                if ( WinLib::GetYesNo(warningMessage, "Warning!") == WinLib::PromptResult::Yes )
                {
                    selectedSoundListIndex = -1;
                    CM->RemoveSoundByStringId(wavStringId, true);
                    CM->notifyChange(false);
                    CM->refreshScenario();
                }
            }
        }
        else
        {
            selectedSoundListIndex = -1;
            CM->RemoveSoundByStringId(wavStringId, false);
            CM->notifyChange(false);
            CM->refreshScenario();
        }
    }
}

void WavEditorWindow::CheckCustomMpqPathPressed()
{
    UpdateCustomStringList();
    checkVirtualFile.SetCheck(false);
    dropCompressionLevel.EnableThis();
}

LRESULT WavEditorWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
            wavListDC = listMapSounds.getDC();
            break;

        case WM_MEASUREITEM:
        {
            MEASUREITEMSTRUCT* mis = (MEASUREITEMSTRUCT*)lParam;
            RawStringPtr str = CM->strings.getString<RawString>((size_t)mis->itemData);
            if ( str != nullptr && GetStringDrawSize(wavListDC, mis->itemWidth, mis->itemHeight, *str) )
            {
                mis->itemWidth += 5;
                mis->itemHeight += 2;
            }
            return TRUE;
        }
        break;

        case WinLib::LB::WM_POSTMEASUREITEMS: // Release items loaded for measurement
            listMapSounds.ReleaseDC(wavListDC);
            wavListDC = NULL;
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
                RawStringPtr str = CM->strings.getString<RawString>((size_t)pdis->itemData);
                if ( CM != nullptr && str != nullptr )
                {
                    HBRUSH hBackground = CreateSolidBrush(RGB(0, 0, 0)); // Same color as in WM_CTLCOLORLISTBOX
                    if ( hBackground != NULL )
                    {
                        FillRect(pdis->hDC, &pdis->rcItem, hBackground); // Draw far background
                        DeleteObject(hBackground);
                        hBackground = NULL;
                    }
                    SetBkMode(pdis->hDC, TRANSPARENT);
                    DrawString(pdis->hDC, pdis->rcItem.left+3, pdis->rcItem.top+1, pdis->rcItem.right-pdis->rcItem.left,
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

void WavEditorWindow::NotifyButtonClicked(int idFrom, HWND hWndFrom)
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

void WavEditorWindow::NotifyComboSelChanged(int idFrom, HWND hWndFrom)
{
    switch ( idFrom )
    {
    case Id::DROP_COMPRESSIONQUALITY: CompressionDropdownChanged(); break;
    case Id::LB_MAPSOUNDS: MapSoundSelectionChanged(); break;
    case Id::LB_VIRTUALSOUNDS: VirtualSoundSelectionChanged(); break;
    }
}

void WavEditorWindow::CreateSubWindows(HWND hWnd)
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
    dropCompressionLevel.CreateThis(hWnd, 140, 459, 150, 200, false, false, Id::DROP_COMPRESSIONQUALITY, compressionLevels, defaultFont);
    dropCompressionLevel.SetSel(0);
    checkVirtualFile.CreateThis(hWnd, 300, 459, 100, 20, false, "Virtual File", Id::CHECK_VIRTUALFILE);
    buttonAddFile.CreateThis(hWnd, 502, 459, 80, 20, "Add File", Id::BUTTON_ADDFILE);
    textCustomMpqString.CreateThis(hWnd, 5, 484, 100, 20, "Custom MPQ Path", Id::TEXT_CUSTOMMPQSTRING);
    dropCustomMpqString.CreateThis(hWnd, 140, 484, 150, 200, true, true, Id::DROP_CUSTOMMPQPATH, {}, defaultFont);
    checkCustomMpqString.CreateThis(hWnd, 300, 484, 150, 20, false, "Use Custom Path", Id::CHECK_CUSTOMMPQPATH);

    size_t numVirtualSounds = Sc::Sound::virtualSoundPaths.size();
    for ( size_t i=0; i<numVirtualSounds; i++ )
        listVirtualSounds.AddString(Sc::Sound::virtualSoundPaths[i]);

    buttonPlaySound.DisableThis();
    
    buttonExtractSound.DisableThis();

    buttonPreviewPlaySound.DisableThis();
    checkVirtualFile.DisableThis();
}

void WavEditorWindow::DisableMapSoundEditing()
{
    textMapSoundFiles.DisableThis();
    buttonStopSounds.DisableThis();
    buttonDeleteSound.DisableThis();
    buttonExtractSound.DisableThis();
    buttonPlaySound.DisableThis();
    listMapSounds.DisableThis();
}

void WavEditorWindow::EnableMapSoundEditing()
{
    textMapSoundFiles.EnableThis();
    buttonStopSounds.EnableThis();
    buttonDeleteSound.EnableThis();
    buttonExtractSound.EnableThis();
    buttonPlaySound.EnableThis();
    listMapSounds.EnableThis();
}

void WavEditorWindow::DisableSoundCustomization()
{
    textCompressionLevel.DisableThis();
    dropCompressionLevel.DisableThis();
    checkVirtualFile.DisableThis();
    buttonAddFile.DisableThis();
}

void WavEditorWindow::EnableSoundCustomization()
{
    textCompressionLevel.EnableThis();
    dropCompressionLevel.EnableThis();
    checkVirtualFile.EnableThis();
    buttonAddFile.EnableThis();
}

FileBrowserPtr<u32> WavEditorWindow::getDefaultSoundBrowser()
{
    return FileBrowserPtr<u32>(new FileBrowser<u32>(getSoundFilters(), "Open Sound", true, false));
}

FileBrowserPtr<u32> WavEditorWindow::getDefaultSoundSaver()
{
    return FileBrowserPtr<u32>(new FileBrowser<u32>(getSaveSoundFilters(), "Save Sound", true, true));
}
