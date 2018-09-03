#include "WavEditor.h"
#include "../../../Chkdraft.h"
#include "../../../Mapping/DatFileBrowsers.h"

enum ID {
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
};

WavEditorWindow::WavEditorWindow() : wavQuality(WavQuality::Uncompressed), selectedSoundListIndex(-1)
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
    if ( CM->GetWavs(wavMap, true) )
    {
        listMapSounds.SetRedraw(false);
        listMapSounds.ClearItems();
        for ( auto pair : wavMap )
        {
            u32 wavStringIndex = pair.first;
            listMapSounds.AddItem(wavStringIndex);
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
    }

    UpdateWindowText();
    UpdateCustomStringList();
}

void WavEditorWindow::UpdateWindowText()
{
    u32 wavStringIndex = 0;
    if ( selectedSoundListIndex >= 0 && listMapSounds.GetItemData(selectedSoundListIndex, wavStringIndex) )
    {
        u16 wavIndex = u16_max;
        auto wavEntry = wavMap.find(wavStringIndex);
        if ( wavEntry != wavMap.end() )
            wavIndex = wavEntry->second;

        WavStatus wavStatus = CM->GetWavStatus(wavStringIndex);

        std::string wavStatusString = "";
        if ( wavStatus == WavStatus::NoMatch )
            wavStatusString = ", Status: No Matching Sound File";
        else if ( wavStatus == WavStatus::NoMatchExtended )
            wavStatusString = ", Status: Extended, cannot match";
        else if ( wavStatus == WavStatus::FileInUse )
            wavStatusString = ", Status: Unknown - Map Locked";
        else if ( wavStatus == WavStatus::Unknown )
            wavStatusString = ", Status: ???";
        else if ( wavStatus == WavStatus::CurrentMatch )
            wavStatusString = ", Status: Matches File In Map";
        else if ( wavStatus == WavStatus::VirtualFile )
            wavStatusString = ", Status: Virtual File";
        else if ( wavStatus == WavStatus::PendingMatch )
            wavStatusString = ", Status: Matches Pending Asset";

        if ( wavIndex == u16_max )
        {
            chkd.mapSettingsWindow.SetWinText("Map Settings - [WAV #null, String #" + std::to_string(wavStringIndex) + wavStatusString + "]");
        }
        else
        {
            chkd.mapSettingsWindow.SetWinText("Map Settings - [WAV #" + std::to_string(wavIndex) + ", String #" + std::to_string(wavStringIndex) + wavStatusString + "]");
        }
    }
    else
        chkd.mapSettingsWindow.SetWinText("Map Settings");
}

void WavEditorWindow::UpdateCustomStringList()
{
    if ( checkCustomMpqString.isChecked() )
    {
        std::vector<StringTableNode> strList;
        if ( CM->addAllUsedStrings(strList, true, false) )
        {
            dropCustomMpqString.ClearItems();
            for ( auto str : strList )
                dropCustomMpqString.AddItem(str.string);
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
    u32 wavStringIndex = 0;
    if ( selectedSoundListIndex >= 0 && listMapSounds.GetItemData(selectedSoundListIndex, wavStringIndex) )
    {
        buffer wavBuffer("WaBu");
        RawString wavString;
        if ( CM->GetWav(wavStringIndex, wavBuffer) )
        {
            PlaySound((LPCTSTR)wavBuffer.getPtr(0), NULL, SND_ASYNC|SND_MEMORY);
        }
        else if ( CM->GetString(wavString, wavStringIndex) && CM->IsInVirtualWavList(wavString) &&
            chkd.scData.GetScAsset(wavString, wavBuffer, DatFileBrowserPtr(new ChkdDatFileBrowser()), ChkdDatFileBrowser::getDatFileDescriptors(), ChkdDatFileBrowser::getExpectedStarCraftDirectory()) )
        {
            PlaySound((LPCTSTR)wavBuffer.getPtr(0), NULL, SND_ASYNC|SND_MEMORY);
        }
        else
            Error("Failed to get WAV buffer!");
    }
    else
        Error("Failed to get string ID of WAV!");
}

void WavEditorWindow::PlayVirtualSoundButtonPressed()
{
    buffer wavBuffer("WaBu");
    int sel = 0;
    std::string wavString = "";
    if ( listVirtualSounds.GetCurSelString(wavString) )
    {
        if ( CM->IsInVirtualWavList(wavString) && chkd.scData.GetScAsset(wavString, wavBuffer) )
            PlaySound((LPCTSTR)wavBuffer.getPtr(0), NULL, SND_ASYNC | SND_MEMORY);
    }
    else
        Error("Failed to get selected virtual WAV string!");
}

void WavEditorWindow::ExtractSoundButtonPressed()
{
    if ( !listMapSounds.GetCurSel(selectedSoundListIndex) )
        selectedSoundListIndex = -1;

    RawString wavMpqPath;
    u32 wavStringIndex = 0;
    if ( listMapSounds.GetItemData(selectedSoundListIndex, wavStringIndex) && CM->GetString(wavMpqPath, wavStringIndex) )
    {
        std::string wavFileName = GetMpqFileName(wavMpqPath);
        u32 filterIndex = 0;
        std::string saveFilePath = "";
        if ( BrowseForSave(saveFilePath, filterIndex, getSoundFilters(), "", "Save Sound", false, true) )
        {
            if ( !CM->ExtractMpqAsset(wavMpqPath, saveFilePath) )
                Error("Error Extracting Asset!");
        }
        else
            Error("Error Retrieving File Name.");
    }
}

void WavEditorWindow::BrowseButtonPressed()
{
    OPENFILENAME ofn = { };
    std::vector<std::pair<std::string, std::string>> filtersAndLabels = { std::make_pair<std::string, std::string>("*.wav", "WAV File"), std::make_pair<std::string, std::string>("*.*", "All Files") };
    std::string initPath = Settings::starCraftPath + GetSystemFileSeparator() + "Maps";

    u32 filterIndex = 0;
    std::string soundFilePath;
    if ( BrowseForFile(soundFilePath, filterIndex, filtersAndLabels, initPath, "Open Sound", true, false) )
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
    if ( useVirtualFile && CM->IsInVirtualWavList(filePath) )
    {
        if ( useCustomMpqString )
            addedWav = CM->AddWav(filePath, customMpqPath, wavQuality, true);
        else
            addedWav = CM->AddWav(filePath, wavQuality, true);
    }
    else if ( !useVirtualFile && FindFile(filePath) )
    {
        if ( useCustomMpqString )
            addedWav = CM->AddWav(filePath, customMpqPath, wavQuality, false);
        else
            addedWav = CM->AddWav(filePath, wavQuality, false);
    }
    else if ( !useVirtualFile && CM->IsInVirtualWavList(filePath) )
    {
        buffer wavContents("WaCo");
        if ( chkd.scData.GetScAsset(filePath, wavContents) )
        {
            if ( useCustomMpqString )
                addedWav = CM->AddWav(customMpqPath, wavContents, wavQuality);
            else
            {
                std::string mpqFileName = GetMpqFileName(filePath);
                std::string standardWavDir = MapFile::GetStandardWavDir();
                std::string mpqFilePath = MakeMpqFilePath(standardWavDir, mpqFileName);
                addedWav = CM->AddWav(mpqFilePath, wavContents, wavQuality);
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
        u32 wavStringIndex = 0;
        if ( selectedSoundListIndex >= 0 && listMapSounds.GetItemData(selectedSoundListIndex, wavStringIndex) )
        {
            WavStatus wavStatus = CM->GetWavStatus(wavStringIndex);
            if ( wavStatus == WavStatus::PendingMatch || wavStatus == WavStatus::CurrentMatch || wavStatus == WavStatus::VirtualFile )
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
    PlaySound(NULL, NULL, 0);
}

void WavEditorWindow::DeleteSoundButtonPressed()
{
    u32 wavStringIndex = 0;
    if ( listMapSounds.GetItemData(selectedSoundListIndex, wavStringIndex) )
    {
        if ( CM->MapUsesWavString(wavStringIndex) )
        {
            WavStatus wavStatus = CM->GetWavStatus(wavStringIndex);
            if ( wavStatus == WavStatus::NoMatch || wavStatus == WavStatus::NoMatchExtended )
            {
                selectedSoundListIndex = -1;
                CM->RemoveWavByStringIndex(wavStringIndex, true);
                CM->notifyChange(false);
                CM->refreshScenario();
            }
            else
            {
                std::string warningMessage = "This WAV is currently using in triggers and/or briefing";
                if ( wavStatus == WavStatus::CurrentMatch )
                    warningMessage += " and is present in the MPQ";
                else if ( wavStatus == WavStatus::PendingMatch )
                    warningMessage += " and is valid";
                else if ( wavStatus == WavStatus::FileInUse )
                    warningMessage += " and whether it is in the map MPQ is unknown";
                else if ( wavStatus == WavStatus::VirtualFile )
                    warningMessage += " and is a valid virtual file";
                warningMessage += ", are you sure you want to remove it?";

                if ( WinLib::GetYesNo(warningMessage, "Warning!") == WinLib::PromptResult::Yes )
                {
                    selectedSoundListIndex = -1;
                    CM->RemoveWavByStringIndex(wavStringIndex, true);
                    CM->notifyChange(false);
                    CM->refreshScenario();
                }
            }
        }
        else
        {
            selectedSoundListIndex = -1;
            CM->RemoveWavByStringIndex(wavStringIndex, false);
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
            RawString str;

            if ( CM->GetString(str, (u32)mis->itemData) && str.size() > 0 &&
                GetStringDrawSize(wavListDC, mis->itemWidth, mis->itemHeight, str) )
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
                RawString str;
                if ( CM != nullptr && CM->GetString(str, (u32)pdis->itemData) && str.size() > 0 )
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
                        RGB(16, 252, 24), str);
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
    case ID::BUTTON_PLAYSOUND: PlaySoundButtonPressed(); break;
    case ID::BUTTON_PLAYVIRTUALSOUND: PlayVirtualSoundButtonPressed(); break;
    case ID::BUTTON_EXTRACTSOUND: ExtractSoundButtonPressed(); break;
    case ID::BUTTON_BROWSEFORSOUND: BrowseButtonPressed(); break;
    case ID::BUTTON_ADDFILE: AddFileButtonPressed(); break;
    case ID::CHECK_VIRTUALFILE: CheckVirtualFilePressed(); break;
    case ID::BUTTON_STOPSOUNDS: StopSoundsButtonPressed(); break;
    case ID::BUTTON_DELETESOUND: DeleteSoundButtonPressed(); break;
    case ID::CHECK_CUSTOMMPQPATH: CheckCustomMpqPathPressed(); break;
    }
}

void WavEditorWindow::NotifyComboSelChanged(int idFrom, HWND hWndFrom)
{
    switch ( idFrom )
    {
    case ID::DROP_COMPRESSIONQUALITY: CompressionDropdownChanged(); break;
    case ID::LB_MAPSOUNDS: MapSoundSelectionChanged(); break;
    case ID::LB_VIRTUALSOUNDS: VirtualSoundSelectionChanged(); break;
    }
}

void WavEditorWindow::CreateSubWindows(HWND hWnd)
{
    textMapSoundFiles.CreateThis(hWnd, 5, 5, 100, 20, "Map Sound Files", TEXT_MAPSOUNDFILES);
    buttonStopSounds.CreateThis(hWnd, 127, 3, 110, 20, "Stop Sounds", BUTTON_STOPSOUNDS);
    buttonDeleteSound.CreateThis(hWnd, 242, 3, 110, 20, "Delete Selected", BUTTON_DELETESOUND);
    buttonExtractSound.CreateThis(hWnd, 357, 3, 110, 20, "Extract Selected", BUTTON_EXTRACTSOUND);
    buttonPlaySound.CreateThis(hWnd, 472, 3, 110, 20, "Play Selected", BUTTON_PLAYSOUND);
    listMapSounds.CreateThis(hWnd, 5, 25, 582, 188, true, false, false, false, LB_MAPSOUNDS);

    textAvailableSounds.CreateThis(hWnd, 5, 219, 200, 20, "Available MPQ sound files (Virtual Sounds)", TEXT_VIRTUALSOUNDS);
    buttonPreviewPlaySound.CreateThis(hWnd, 432, 217, 150, 20, "Play Selected", BUTTON_PLAYVIRTUALSOUND);
    listVirtualSounds.CreateThis(hWnd, 5, 239, 582, 200, false, false, true, true, LB_VIRTUALSOUNDS);
    textFileName.CreateThis(hWnd, 5, 434, 100, 20, "Filename", TEXT_SOUNDFILENAME);
    editFileName.CreateThis(hWnd, 140, 434, 352, 20, false, EDIT_SOUNDFILENAME);
    buttonBrowse.CreateThis(hWnd, 502, 434, 80, 20, "Browse", BUTTON_BROWSEFORSOUND);
    textCompressionLevel.CreateThis(hWnd, 5, 459, 100, 20, "Compression Level", TEXT_COMPRESSIONLEVEL);
    const std::vector<std::string> compressionLevels = { "Uncompressed", "Low Quality", "Medium Quality", "High Quality" };
    dropCompressionLevel.CreateThis(hWnd, 140, 459, 150, 200, false, false, DROP_COMPRESSIONQUALITY, compressionLevels, defaultFont);
    dropCompressionLevel.SetSel(0);
    checkVirtualFile.CreateThis(hWnd, 300, 459, 100, 20, false, "Virtual File", CHECK_VIRTUALFILE);
    buttonAddFile.CreateThis(hWnd, 502, 459, 80, 20, "Add File", BUTTON_ADDFILE);
    textCustomMpqString.CreateThis(hWnd, 5, 484, 100, 20, "Custom MPQ Path", TEXT_CUSTOMMPQSTRING);
    dropCustomMpqString.CreateThis(hWnd, 140, 484, 150, 200, true, true, DROP_CUSTOMMPQPATH, {}, defaultFont);
    checkCustomMpqString.CreateThis(hWnd, 300, 484, 150, 20, false, "Use Custom Path", CHECK_CUSTOMMPQPATH);

    for ( s32 i=0; i<NumVirtualSounds; i++ )
        listVirtualSounds.AddString(VirtualSoundFiles[i]);

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
