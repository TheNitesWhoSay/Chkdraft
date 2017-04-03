#include "WavEditor.h"
#include "Chkdraft.h"

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
    BUTTON_ADDFILE
};

WavEditorWindow::WavEditorWindow() : wavQuality(WavQuality::Uncompressed), selectedSoundListIndex(-1)
{

}

bool WavEditorWindow::CreateThis(HWND hParent, u32 windowId)
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
            //buttonExtractSound.EnableThis();
            //buttonPlaySound.EnableThis();
        }
    }

    UpdateWindowText();
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
        else if ( wavStatus == WavStatus::PendingMatch )
            wavStatusString = ", Status: Matches Pending Asset";

        if ( wavIndex == u16_max )
        {
            chkd.mapSettingsWindow.SetWinText((std::string("Map Settings - [WAV #null, String #") +
                std::to_string(wavStringIndex) + wavStatusString + "]").c_str());
        }
        else
        {
            chkd.mapSettingsWindow.SetWinText((std::string("Map Settings - [WAV #") +
                std::to_string(wavIndex) + ", String #" + std::to_string(wavStringIndex) + wavStatusString + "]").c_str());
        }
    }
    else
        chkd.mapSettingsWindow.SetWinText("Map Settings");
}

void WavEditorWindow::BrowseButtonPressed()
{
    OPENFILENAME ofn = { };
    char szFileName[MAX_PATH] = { };
    char initPath[MAX_PATH] = { };

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.lpstrFilter = "WAV File\0*.wav\0All Files\0*.*\0";
    ofn.lpstrFile = szFileName;

    if ( GetRegScPath(initPath, MAX_PATH) )
    {
        std::strcat(initPath, "\\Maps");
        ofn.lpstrInitialDir = initPath;
    }
    else
        ofn.lpstrInitialDir = NULL;

    ofn.hwndOwner = NULL;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

    if ( GetOpenFileName(&ofn) == TRUE )
        editFileName.SetWinText(szFileName);
    else if ( CommDlgExtendedError() != 0 )
        CHKD_ERR("Error Retrieving File Name.");
}

void WavEditorWindow::AddFileButtonPressed()
{
    std::string filePath = editFileName.GetWinText();
    if ( CM->AddWav(filePath, wavQuality) )
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
}

void WavEditorWindow::MapSoundSelectionChanged()
{
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
        //buttonExtractSound.EnableThis();
        //buttonPlaySound.EnableThis();
    }

    UpdateWindowText();
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
                warningMessage += ", are you sure you want to remove it?";

                if ( MessageBox(NULL, warningMessage.c_str(), "Warning!", MB_YESNO | MB_ICONEXCLAMATION) == IDYES )
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

        case WM_PREMEASUREITEMS: // Measuring is time sensative, load necessary items for measuring all strings once
            wavListDC = listMapSounds.getDC();
            break;

        case WM_MEASUREITEM:
        {
            MEASUREITEMSTRUCT* mis = (MEASUREITEMSTRUCT*)lParam;
            RawString str;

            if ( CM->GetString(str, mis->itemData) && str.size() > 0 &&
                GetStringDrawSize(wavListDC, mis->itemWidth, mis->itemHeight, str) )
            {
                mis->itemWidth += 5;
                mis->itemHeight += 2;
            }
            return TRUE;
        }
        break;

        case WM_POSTMEASUREITEMS: // Release items loaded for measurement
            listMapSounds.ReleaseDC(wavListDC);
            wavListDC = NULL;
            break;

        case WM_PREDRAWITEMS:
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
                if ( CM != nullptr && CM->GetString(str, pdis->itemData) && str.size() > 0 )
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

        case WM_POSTDRAWITEMS:
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
    case ID::BUTTON_BROWSEFORSOUND: BrowseButtonPressed(); break;
    case ID::BUTTON_ADDFILE: AddFileButtonPressed(); break;
    case ID::CHECK_VIRTUALFILE: CheckVirtualFilePressed(); break;
    case ID::BUTTON_DELETESOUND: DeleteSoundButtonPressed(); break;
    }
}

void WavEditorWindow::NotifyComboSelChanged(int idFrom, HWND hWndFrom)
{
    switch ( idFrom )
    {
    case ID::DROP_COMPRESSIONQUALITY: CompressionDropdownChanged(); break;
    case ID::LB_MAPSOUNDS: MapSoundSelectionChanged(); break;
    }
}

void WavEditorWindow::CreateSubWindows(HWND hWnd)
{
    textMapSoundFiles.CreateThis(hWnd, 5, 5, 100, 20, "Map Sound Files", TEXT_MAPSOUNDFILES);
    buttonStopSounds.CreateThis(hWnd, 127, 3, 110, 20, "Stop Sounds", BUTTON_STOPSOUNDS);
    buttonDeleteSound.CreateThis(hWnd, 242, 3, 110, 20, "Delete Selected", BUTTON_DELETESOUND);
    buttonExtractSound.CreateThis(hWnd, 357, 3, 110, 20, "Extract Selected", BUTTON_EXTRACTSOUND);
    buttonPlaySound.CreateThis(hWnd, 472, 3, 110, 20, "Play Selected", BUTTON_PLAYSOUND);
    listMapSounds.CreateThis(hWnd, 5, 25, 582, 188, true, false, false, LB_MAPSOUNDS);

    textAvailableSounds.CreateThis(hWnd, 5, 219, 200, 20, "Available MPQ sound files (Virtual Sounds)", TEXT_VIRTUALSOUNDS);
    buttonPreviewPlaySound.CreateThis(hWnd, 432, 217, 150, 20, "Play Selected", BUTTON_PLAYVIRTUALSOUND);
    listVirtualSounds.CreateThis(hWnd, 5, 239, 582, 200, false, false, false, LB_VIRTUALSOUNDS);
    textFileName.CreateThis(hWnd, 5, 434, 100, 20, "Filename", TEXT_SOUNDFILENAME);
    editFileName.CreateThis(hWnd, 140, 434, 352, 20, false, EDIT_SOUNDFILENAME);
    buttonBrowse.CreateThis(hWnd, 502, 434, 80, 20, "Browse", BUTTON_BROWSEFORSOUND);
    textCompressionLevel.CreateThis(hWnd, 5, 459, 100, 20, "Compression Level", TEXT_COMPRESSIONLEVEL);
    const char* compressionLevels[] = { "Uncompressed", "Low Quality", "Medium Quality", "High Quality" };
    dropCompressionLevel.CreateThis(hWnd, 140, 459, 150, 200, false, false, DROP_COMPRESSIONQUALITY, 4, compressionLevels, defaultFont);
    dropCompressionLevel.SetSel(0);
    checkVirtualFile.CreateThis(hWnd, 300, 459, 100, 20, false, "Virtual File", CHECK_VIRTUALFILE);
    buttonAddFile.CreateThis(hWnd, 502, 459, 80, 20, "Add File", BUTTON_ADDFILE);

    buttonPlaySound.DisableThis();
    buttonStopSounds.DisableThis();
    
    buttonExtractSound.DisableThis();

    textAvailableSounds.DisableThis();
    buttonPreviewPlaySound.DisableThis();
    listVirtualSounds.DisableThis();
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
