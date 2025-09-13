#include "profile_general.h"
#include <chkdraft.h>

enum_t(Id, u32, {
    CheckUseRemastered = ID_FIRST,
    GroupRemastered,
    TextScrPath,
    EditScrPath,
    BrowseScrPath,
    TextCascPath,
    EditCascPath,
    BrowseCascPath,
    TextScrDefaultSkin,
    DropScrDefaultSkin,

    GroupClassic,
    TextScPath,
    EditScPath,
    BrowseScPath,
    TextStarDatPath,
    EditStarDatPath,
    BrowseStarDatPath,
    TextBrooDatPath,
    EditBrooDatPath,
    BrowseBrooDatPath,
    TextPatchRtDatPath,
    EditPatchRtDatPath,
    BrowsePatchRtDatPath,
    TextScDefaultSkin,
    DropScDefaultSkin,

    GroupMisc,
    TextLogLevel,
    DropLogLevel,
    TextDeathTableStart,
    EditDeathTableStart,
    CheckUseDefaultDeathTableStart,
    CheckUseAddressesForMemory,
    TextMaxHistMemoryUseMb,
    EditMaxHistMemoryUseMb,
    TextMaxHistActions,
    EditMaxHistActions
});

bool ProfileGeneralWindow::CreateThis(HWND hParent, u64 windowId)
{
    if ( getHandle() != NULL )
        return SetParent(hParent);

    if ( ClassWindow::RegisterWindowClass(0, NULL, NULL, NULL, NULL, "Profile Paths", NULL, false) &&
         ClassWindow::CreateClassWindow(0, "Profile Paths", WS_CHILD, 4, 22, 592, 524, hParent, (HMENU)windowId) )
    {
        CreateSubWindows(getHandle());
        return true;
    }
    else
        return false;
}

void ProfileGeneralWindow::DestroyThis()
{
    ClassWindow::DestroyThis();
}

void ProfileGeneralWindow::DisableRemasteredGroup()
{
    groupRemastered.DisableThis();
    textScrPath.DisableThis();
    editScrPath.DisableThis();
    browseScrPath.DisableThis();
    textCascPath.DisableThis();
    editCascPath.DisableThis();
    browseCascPath.DisableThis();
    textScrDefaultSkin.DisableThis();
    dropScrDefaultSkin.DisableThis();
}

void ProfileGeneralWindow::EnableRemasteredGroup()
{
    groupRemastered.EnableThis();
    textScrPath.EnableThis();
    editScrPath.EnableThis();
    browseScrPath.EnableThis();
    textCascPath.EnableThis();
    editCascPath.EnableThis();
    browseCascPath.EnableThis();
    textScrDefaultSkin.EnableThis();
    dropScrDefaultSkin.EnableThis();
}

void ProfileGeneralWindow::DisableClassicGroup()
{
    groupClassic.DisableThis();
    textScPath.DisableThis();
    editScPath.DisableThis();
    browseScPath.DisableThis();
    textStarDatPath.DisableThis();
    editStarDatPath.DisableThis();
    browseStarDatPath.DisableThis();
    textBrooDatPath.DisableThis();
    editBrooDatPath.DisableThis();
    browseBrooDatPath.DisableThis();
    textPatchRtDatPath.DisableThis();
    editPatchRtDatPath.DisableThis();
    browsePatchRtDatPath.DisableThis();
    textScDefaultSkin.DisableThis();
    dropScDefaultSkin.DisableThis();
}

void ProfileGeneralWindow::EnableClassicGroup()
{
    groupClassic.EnableThis();
    textScPath.EnableThis();
    editScPath.EnableThis();
    browseScPath.EnableThis();
    textStarDatPath.EnableThis();
    editStarDatPath.EnableThis();
    browseStarDatPath.EnableThis();
    textBrooDatPath.EnableThis();
    editBrooDatPath.EnableThis();
    browseBrooDatPath.EnableThis();
    textPatchRtDatPath.EnableThis();
    editPatchRtDatPath.EnableThis();
    browsePatchRtDatPath.EnableThis();
    textScDefaultSkin.EnableThis();
    dropScDefaultSkin.EnableThis();
}

void ProfileGeneralWindow::RefreshWindow()
{
    bool prevRefreshingVal = refreshing;
    refreshing = true;

    auto & editProfile = getEditProfile();
    bool useRemastered = editProfile.useRemastered;
    checkUseRemastered.SetCheck(useRemastered);
    if ( useRemastered )
    {
        DisableClassicGroup();
        EnableRemasteredGroup();
    }
    else // use classic
    {
        DisableRemasteredGroup();
        EnableClassicGroup();
    }
    editScPath.SetText(editProfile.classic.starCraftPath);
    editStarDatPath.SetText(editProfile.classic.starDatPath);
    editBrooDatPath.SetText(editProfile.classic.brooDatPath);
    editPatchRtDatPath.SetText(editProfile.classic.patchRtPath);
    dropScDefaultSkin.SetSel(int(editProfile.classic.defaultSkin));
    editScrPath.SetText(editProfile.remastered.starCraftPath);
    editCascPath.SetText(editProfile.remastered.cascPath);
    dropScrDefaultSkin.SetSel(int(editProfile.remastered.defaultSkin));
    switch ( editProfile.logger.defaultLogLevel )
    {
        case LogLevel::Off: dropLogLevel.SetSel(0); break; // 0
        case LogLevel::Fatal: dropLogLevel.SetSel(1); break; // 100
        case LogLevel::Error: dropLogLevel.SetSel(2); break; // 200
        case LogLevel::Warn: dropLogLevel.SetSel(3); break; // 300
        case LogLevel::Info: dropLogLevel.SetSel(4); break; // 400
        case LogLevel::Debug: dropLogLevel.SetSel(5); break; // 500
        case LogLevel::Trace: dropLogLevel.SetSel(6); break; // 600
        case LogLevel::All: dropLogLevel.SetSel(7); break; // -1
        default: dropLogLevel.SetEditNum<std::uint32_t>(std::uint32_t(editProfile.logger.defaultLogLevel)); break;
    }
    editMaxHistMemoryUseMb.SetEditNum<int>(editProfile.history.maxMemoryUseMb);
    editMaxHistActions.SetEditNum<int>(editProfile.history.maxActions);
    editDeathTableStart.SetText(to_hex_string(editProfile.triggers.deathTableStart, true));
    checkUseDefaultDeathTableStart.SetCheck(editProfile.triggers.deathTableStart == 0x58A364);
    if ( editProfile.triggers.deathTableStart == 0x58A364 )
        editDeathTableStart.DisableThis();
    else
        editDeathTableStart.EnableThis();

    checkUseAddressesForMemory.SetCheck(editProfile.triggers.useAddressesForMemory);
    refreshing = prevRefreshingVal;
}

void ProfileGeneralWindow::ToggleUseRemastered()
{
    auto & editProfile = getEditProfile();
    bool useRemastered = checkUseRemastered.isChecked();
    if ( editProfile.useRemastered != useRemastered )
    {
        editProfile.useRemastered = useRemastered;
        if ( editProfile.useRemastered )
        {
            DisableClassicGroup();
            EnableRemasteredGroup();
        }
        else
        {
            DisableRemasteredGroup();
            EnableClassicGroup();
        }
        RedrawThis();
        editProfile.saveProfile();
    }
}

void ProfileGeneralWindow::BrowseScrPath()
{
    auto & editProfile = getEditProfile();
    std::string scrPath = editProfile.remastered.starCraftPath;
    bool isRemastered = true;
    bool declinedBrowse = false;
    Sc::DataFile::Browser scBrowser {};
    if ( scBrowser.findStarCraftDirectory(scrPath, isRemastered, declinedBrowse, "",
        Sc::DataFile::Browser::getNoPromptNoDefaultStarCraftBrowser()) && !isSamePath(scrPath, editProfile.remastered.starCraftPath) )
    {
        bool updateCascPath = editProfile.remastered.cascPath.empty() ||
            isSamePath(editProfile.remastered.starCraftPath, editProfile.remastered.cascPath) ||
            isInDirectory(editProfile.remastered.cascPath, editProfile.remastered.starCraftPath);

        editProfile.remastered.starCraftPath = scrPath;
        if ( updateCascPath )
            editProfile.remastered.cascPath = makeSystemFilePath(editProfile.remastered.starCraftPath, "Data");

        editProfile.saveProfile();
        RefreshWindow();
        logger.debug() << "Updated remastered path to " << editProfile.remastered.starCraftPath << std::endl;
    }
}

void ProfileGeneralWindow::BrowseCascPath()
{
    auto & editProfile = getEditProfile();
    auto browser = NoPromptFileBrowser<u32>(Sc::DataFile::getCascBuildInfoFilter(), "");
    std::string cascPath {};
    u32 filterIndex = 0;
    if ( browser.browseForOpenPath(cascPath, filterIndex) && !cascPath.empty() )
    {
        if ( cascPath.ends_with(Sc::DataFile::buildInfoFileName) )
            cascPath = makeSystemFilePath(getSystemFileDirectory(cascPath), "Data");

        editProfile.remastered.cascPath = cascPath;
        editProfile.saveProfile();
        RefreshWindow();
        logger.debug() << "Updated remastered cascPath to " << editProfile.remastered.cascPath << std::endl;
    }
}

void ProfileGeneralWindow::BrowseScPath()
{
    auto & editProfile = getEditProfile();
    std::string classicScPath = editProfile.classic.starCraftPath;
    bool isRemastered = true;
    bool declinedBrowse = false;
    Sc::DataFile::Browser scBrowser {};
    if ( scBrowser.findStarCraftDirectory(classicScPath, isRemastered, declinedBrowse, "",
        Sc::DataFile::Browser::getNoPromptNoDefaultStarCraftBrowser()) && !isSamePath(classicScPath, editProfile.classic.starCraftPath) )
    {
        bool updateStarDat = editProfile.classic.starDatPath.empty() ||
            isInDirectory(editProfile.classic.starDatPath, editProfile.classic.starCraftPath);
        bool updateBrooDat = editProfile.classic.brooDatPath.empty() ||
            isInDirectory(editProfile.classic.brooDatPath, editProfile.classic.starCraftPath);
        bool updatePatchRtDat = editProfile.classic.patchRtPath.empty() ||
            isInDirectory(editProfile.classic.patchRtPath, editProfile.classic.starCraftPath);

        if ( classicScPath != editProfile.classic.starCraftPath )
        {
            editProfile.classic.starCraftPath = classicScPath;
            if ( updateStarDat )
                editProfile.classic.starDatPath = makeSystemFilePath(editProfile.classic.starCraftPath, "StarDat.mpq");
            if ( updateBrooDat )
                editProfile.classic.brooDatPath = makeSystemFilePath(editProfile.classic.starCraftPath, "BrooDat.mpq");
            if ( updatePatchRtDat )
                editProfile.classic.patchRtPath = makeSystemFilePath(editProfile.classic.starCraftPath, "patch_rt.mpq");

            editProfile.saveProfile();
            RefreshWindow();
            logger.debug() << "Updated classic starCraftPath to " << editProfile.classic.starCraftPath << std::endl;
        }
    }
    logger.info("BrowseScPath");
}

void ProfileGeneralWindow::BrowseStarDatPath()
{
    auto & editProfile = getEditProfile();
    auto browser = NoPromptFileBrowser<u32>(Sc::DataFile::getStarDatFilter(), "");
    std::string starDatPath {};
    u32 filterIndex = 0;
    if ( browser.browseForOpenPath(starDatPath, filterIndex) && !starDatPath.empty() && !isSamePath(starDatPath, editProfile.classic.starDatPath) )
    {
        editProfile.classic.starDatPath = starDatPath;
        editProfile.saveProfile();
        RefreshWindow();
        logger.debug() << "Updated classic starDatPath to " << editProfile.classic.starDatPath << std::endl;
    }
}

void ProfileGeneralWindow::BrowseBrooDatPath()
{
    auto & editProfile = getEditProfile();
    auto browser = NoPromptFileBrowser<u32>(Sc::DataFile::getBrooDatFilter(), "");
    std::string brooDatPath {};
    u32 filterIndex = 0;
    if ( browser.browseForOpenPath(brooDatPath, filterIndex) && !brooDatPath.empty() && !isSamePath(brooDatPath, editProfile.classic.brooDatPath) )
    {
        editProfile.classic.brooDatPath = brooDatPath;
        editProfile.saveProfile();
        RefreshWindow();
        logger.debug() << "Updated classic brooDatPath to " << editProfile.classic.brooDatPath << std::endl;
    }
}

void ProfileGeneralWindow::BrowsePatchRtDatPath()
{
    auto & editProfile = getEditProfile();
    auto browser = NoPromptFileBrowser<u32>(Sc::DataFile::getPatchRtFilter(), "");
    std::string patchRtPath {};
    u32 filterIndex = 0;
    if ( browser.browseForOpenPath(patchRtPath, filterIndex) && !patchRtPath.empty() && !isSamePath(patchRtPath, editProfile.classic.patchRtPath) )
    {
        editProfile.classic.patchRtPath = patchRtPath;
        editProfile.saveProfile();
        RefreshWindow();
        logger.debug() << "Updated classic patchRtPath to " << editProfile.classic.patchRtPath << std::endl;
    }
}

void ProfileGeneralWindow::ToggleUseDefaultDeathTableStart()
{
    auto & editProfile = getEditProfile();
    bool useDefaultDeathTableStart = checkUseDefaultDeathTableStart.isChecked();
    if ( useDefaultDeathTableStart )
    {
        editDeathTableStart.DisableThis();
        if ( editProfile.triggers.deathTableStart != 0x58A364 )
        {
            editProfile.triggers.deathTableStart = 0x58A364;
            editDeathTableStart.SetText(to_hex_string(editProfile.triggers.deathTableStart, true));
            editProfile.saveProfile();
            logger.debug() << "Death table start updated to 0x" << to_hex_string(editProfile.triggers.deathTableStart) << std::endl;
        }
    }
    else
    {
        editDeathTableStart.EnableThis();
        editDeathTableStart.SetText(to_hex_string(editProfile.triggers.deathTableStart, true));
    }
}

void ProfileGeneralWindow::ToggleUseAddressesForMemory()
{
    auto & editProfile = getEditProfile();
    bool useAddressesForMemory = checkUseAddressesForMemory.isChecked();
    if ( useAddressesForMemory != editProfile.triggers.useAddressesForMemory )
    {
        editProfile.triggers.useAddressesForMemory = useAddressesForMemory;
        editProfile.saveProfile();
        logger.debug() << "Updated useAddressesForMemory to " << (editProfile.triggers.useAddressesForMemory ? "true" : "false") << std::endl;
    }
}

void ProfileGeneralWindow::ScrSkinSelUpdated()
{
    auto & editProfile = getEditProfile();
    int newSel = dropScrDefaultSkin.GetSel();
    switch ( ChkdSkin(newSel) )
    {
        case ChkdSkin::ClassicGDI:
        case ChkdSkin::Classic:
        case ChkdSkin::RemasteredSD:
        case ChkdSkin::RemasteredHD2:
        case ChkdSkin::RemasteredHD:
        case ChkdSkin::CarbotHD2:
        case ChkdSkin::CarbotHD:
            if ( editProfile.remastered.defaultSkin != ChkdSkin(newSel) )
            {
                editProfile.remastered.defaultSkin = ChkdSkin(newSel);
                editProfile.saveProfile();
                logger.debug() << "Updated remastered default skin to " << editProfile.remastered.defaultSkin << std::endl;
            }
            break;
        default:
            logger.error("Invalid remastered skin selection (" + std::to_string(newSel) + ")");
            break;
    }
}

void ProfileGeneralWindow::ScSkinSelUpdated()
{
    auto & editProfile = getEditProfile();
    int newSel = dropScDefaultSkin.GetSel();
    switch ( ChkdSkin(newSel) )
    {
        case ChkdSkin::ClassicGDI:
        case ChkdSkin::Classic:
            if ( editProfile.classic.defaultSkin != ChkdSkin(newSel) )
            {
                editProfile.classic.defaultSkin = ChkdSkin(newSel);
                editProfile.saveProfile();
                logger.debug() << "Updated classic default skin to " << editProfile.classic.defaultSkin << std::endl;
            }
            break;
        default:
            logger.error("Invalid classic skin selection (" + std::to_string(newSel) + ")");
            break;
    }
}

void ProfileGeneralWindow::LogLevelSelUpdated()
{
    auto & editProfile = getEditProfile();
    auto setLogLevel = [&](LogLevel newLogLevel) {
        if ( editProfile.logger.defaultLogLevel != newLogLevel )
        {
            editProfile.logger.defaultLogLevel = newLogLevel;
            editProfile.saveProfile();
            logger.debug() << "Updated log level to " << editProfile.logger.defaultLogLevel << std::endl;
        }
    };

    int newSel = dropLogLevel.GetSel();
    switch ( newSel )
    {
        case 0: setLogLevel(LogLevel::Off); break; // [0] Off
        case 1: setLogLevel(LogLevel::Fatal); break; // [100] Fatal
        case 2: setLogLevel(LogLevel::Error); break; // [200] Error
        case 3: setLogLevel(LogLevel::Warn); break; // [300] Warn
        case 4: setLogLevel(LogLevel::Info); break; // [400] Info
        case 5: setLogLevel(LogLevel::Debug); break; // [500] Debug
        case 6: setLogLevel(LogLevel::Trace); break; // [600] Trace
        case 7: setLogLevel(LogLevel::All); break; // [-1] All
    }
}

void ProfileGeneralWindow::NotifyButtonClicked(int idFrom, HWND hWndFrom)
{
    switch ( idFrom )
    {
        case Id::CheckUseRemastered: ToggleUseRemastered(); break;
        case Id::BrowseScrPath: BrowseScrPath(); break;
        case Id::BrowseCascPath: BrowseCascPath(); break;
        case Id::BrowseScPath: BrowseScPath(); break;
        case Id::BrowseStarDatPath: BrowseStarDatPath(); break;
        case Id::BrowseBrooDatPath: BrowseBrooDatPath(); break;
        case Id::BrowsePatchRtDatPath: BrowsePatchRtDatPath(); break;
        case Id::CheckUseDefaultDeathTableStart: ToggleUseDefaultDeathTableStart(); break;
        case Id::CheckUseAddressesForMemory: ToggleUseAddressesForMemory(); break;
    }
}

void ProfileGeneralWindow::NotifyEditUpdated(int idFrom, HWND hWndFrom)
{
    switch ( idFrom )
    {
        case Id::EditScrPath: if ( !possibleScrPathUpdate ) possibleScrPathUpdate = true; break;
        case Id::EditCascPath: if ( !possibleCascPathUpdate ) possibleCascPathUpdate = true; break;
        case Id::EditScPath: if ( !possibleScPathUpdate ) possibleScPathUpdate = true; break;
        case Id::EditStarDatPath: if ( !possibleStarDatPathUpdate ) possibleStarDatPathUpdate = true; break;
        case Id::EditBrooDatPath: if ( !possibleBrooDatPathUpdate ) possibleBrooDatPathUpdate = true; break;
        case Id::EditPatchRtDatPath: if ( !possiblePatchRtPathUpdate ) possiblePatchRtPathUpdate = true; break;
        case Id::EditMaxHistMemoryUseMb: if ( !possibleMaxHistMemoryUpdate ) possibleMaxHistMemoryUpdate = true; break;
        case Id::EditMaxHistActions: if ( !possibleMaxHistActionsUpdate ) possibleMaxHistActionsUpdate = true; break;
        case Id::EditDeathTableStart: if ( !possibleDeathTableStartUpdate ) possibleDeathTableStartUpdate = true; break;
    }
}

void ProfileGeneralWindow::NotifyEditFocusLost(int idFrom, HWND hWndFrom)
{
    switch ( idFrom )
    {
        case Id::EditScrPath: CheckReplaceScrPath(); break;
        case Id::EditCascPath: CheckReplaceCascPath(); break;
        case Id::EditScPath: CheckReplaceScPath(); break;
        case Id::EditStarDatPath: CheckReplaceStarDatPath(); break;
        case Id::EditBrooDatPath: CheckReplaceBrooDatPath(); break;
        case Id::EditPatchRtDatPath: CheckReplacePatchRtPath(); break;
        case Id::EditMaxHistMemoryUseMb: CheckReplaceMaxHistMemory(); break;
        case Id::EditMaxHistActions: CheckReplaceMaxHistActions(); break;
        case Id::EditDeathTableStart: CheckReplaceDeathTableStart(); break;
    }
}

void ProfileGeneralWindow::NotifyComboSelChanged(int idFrom, HWND hWndFrom)
{
    switch ( idFrom )
    {
        case Id::DropScrDefaultSkin: ScrSkinSelUpdated(); break;
        case Id::DropScDefaultSkin: ScSkinSelUpdated(); break;
        case Id::DropLogLevel: LogLevelSelUpdated(); break;
    }
}

void ProfileGeneralWindow::NotifyComboEditUpdated(int idFrom, HWND hWndFrom)
{
    switch ( idFrom )
    {
        case Id::DropLogLevel: if ( !this->possibleLogLevelUpdate ) this->possibleLogLevelUpdate = true; break;
    }
}

void ProfileGeneralWindow::NotifyComboEditFocusLost(int idFrom, HWND hWndFrom)
{
    switch ( idFrom )
    {
        case Id::DropLogLevel: CheckReplaceLogLevel(); break;
    }
}

void ProfileGeneralWindow::NotifyWindowHidden()
{
    CheckEditUpdates();
}

LRESULT ProfileGeneralWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        default:
            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;
    }
    return 0;
}

void ProfileGeneralWindow::CreateSubWindows(HWND hWnd)
{
    refreshing = true;
    checkUseRemastered.CreateThis(hWnd, 5, 5, 100, 20, true, "Use Remastered", Id::CheckUseRemastered);

    groupRemastered.CreateThis(hWnd, 0, 30, 275, 175, "StarCraft Remastered", Id::GroupRemastered);

    textScrPath.CreateThis(hWnd, 5, 60, 70, 20, "StarCraft Path:", Id::TextScrPath);
    editScrPath.CreateThis(hWnd, textScrPath.Right()+5, 60, 168, 20, false, Id::EditScrPath);
    browseScrPath.CreateThis(hWnd, editScrPath.Right()+2, 60, 20, 19, "...", Id::BrowseScrPath, false, false);

    textCascPath.CreateThis(hWnd, 22, 85, 70, 20, "Casc Path:", Id::TextCascPath);
    editCascPath.CreateThis(hWnd, editScrPath.Left(), 85, 168, 20, false, Id::EditCascPath);
    browseCascPath.CreateThis(hWnd, editCascPath.Right()+2, 85, 20, 19, "...", Id::BrowseCascPath, false, false);

    textScrDefaultSkin.CreateThis(hWnd, 13, 110, 70, 20, "Default Skin:", Id::TextScrDefaultSkin);
    dropScrDefaultSkin.CreateThis(hWnd, editScrPath.Left(), 110, 168, 200, false, false, Id::DropScrDefaultSkin, {
        "Classic GDI",
        "Classic OpenGL",
        "Remastered SD",
        "Remastered HD2",
        "Remastered HD",
        "Carbot HD2",
        "Carbot HD"
    });
    dropScrDefaultSkin.SetSel(1);


    groupClassic.CreateThis(hWnd, 279, 30, 275, 175, "Classic StarCraft", Id::GroupClassic);

    textScPath.CreateThis(hWnd, 284, 60, 70, 20, "StarCraft Path:", Id::TextScPath);
    editScPath.CreateThis(hWnd, textScPath.Right()+5, 60, 168, 20, false, Id::EditScPath);
    browseScPath.CreateThis(hWnd, editScPath.Right()+2, 60, 20, 19, "...", Id::BrowseScPath, false, false);

    textStarDatPath.CreateThis(hWnd, 289, 85, 70, 20, "StarDat Path:", Id::TextStarDatPath);
    editStarDatPath.CreateThis(hWnd, editScPath.Left(), 85, 168, 20, false, Id::EditStarDatPath);
    browseStarDatPath.CreateThis(hWnd, editStarDatPath.Right()+2, 85, 20, 19, "...", Id::BrowseStarDatPath, false, false);

    textBrooDatPath.CreateThis(hWnd, 286, 110, 70, 20, "BrooDat Path:", Id::TextBrooDatPath);
    editBrooDatPath.CreateThis(hWnd, editScPath.Left(), 110, 168, 20, false, Id::EditBrooDatPath);
    browseBrooDatPath.CreateThis(hWnd, editStarDatPath.Right()+2, 110, 20, 19, "...", Id::BrowseBrooDatPath, false, false);

    textPatchRtDatPath.CreateThis(hWnd, 286, 135, 70, 20, "PatchRt Path:", Id::TextPatchRtDatPath);
    editPatchRtDatPath.CreateThis(hWnd, editScPath.Left(), 135, 168, 20, false, Id::EditPatchRtDatPath);
    browsePatchRtDatPath.CreateThis(hWnd, editStarDatPath.Right()+2, 135, 20, 19, "...", Id::BrowsePatchRtDatPath, false, false);

    textScDefaultSkin.CreateThis(hWnd, 292, 160, 70, 20, "Default Skin:", Id::TextScDefaultSkin);
    dropScDefaultSkin.CreateThis(hWnd, editScPath.Left(), 160, 168, 200, false, false, Id::DropScDefaultSkin, {
        "Classic GDI",
        "Classic OpenGL"
    });
    dropScDefaultSkin.SetSel(1);

    groupClassic.DisableThis();
    textScPath.DisableThis();
    editScPath.DisableThis();
    browseScPath.DisableThis();
    
    textStarDatPath.DisableThis();
    editStarDatPath.DisableThis();
    browseStarDatPath.DisableThis();
    
    textBrooDatPath.DisableThis();
    editBrooDatPath.DisableThis();
    browseBrooDatPath.DisableThis();
    
    textPatchRtDatPath.DisableThis();
    editPatchRtDatPath.DisableThis();
    browsePatchRtDatPath.DisableThis();
    
    textScDefaultSkin.DisableThis();
    dropScDefaultSkin.DisableThis();

    groupMisc.CreateThis(hWnd, 0, 205, 554, 150, "Misc", Id::GroupMisc);
    textLogLevel.CreateThis(hWnd, 86, 230, 140, 20, "Log Level:", Id::TextLogLevel);
    dropLogLevel.CreateThis(hWnd, 142, 230, 140, 200, true, false, Id::DropLogLevel, {
        "[0] Off",
        "[100] Fatal",
        "[200] Error",
        "[300] Warn",
        "[400] Info",
        "[500] Debug",
        "[600] Trace",
        "[-1] All",
    });
    dropLogLevel.SetSel(4);
    dropLogLevel.ClearEditSel();

    textMaxHistActions.CreateThis(hWnd, 54, 280, 140, 20, "Max Hist Actions:", Id::TextMaxHistMemoryUseMb);
    editMaxHistActions.CreateThis(hWnd, 142, 280, 100, 20, false, Id::EditMaxHistActions);
    
    textMaxHistMemoryUseMb.CreateThis(hWnd, 5, 255, 140, 20, "Max Hist Memory Use (MB):", Id::TextMaxHistActions);
    editMaxHistMemoryUseMb.CreateThis(hWnd, 142, 255, 100, 20, false, Id::EditMaxHistMemoryUseMb);

    textDeathTableStart.CreateThis(hWnd, 49, 305, 140, 20, "Death Table Start:", Id::TextDeathTableStart);
    editDeathTableStart.CreateThis(hWnd, 142, 305, 140, 20, false, Id::EditDeathTableStart);
    editDeathTableStart.DisableThis();
    checkUseDefaultDeathTableStart.CreateThis(hWnd, editDeathTableStart.Right()+2, 309, 100, 20, true, "Use Default", Id::CheckUseDefaultDeathTableStart);
    checkUseDefaultDeathTableStart.FocusThis();
    
    checkUseAddressesForMemory.CreateThis(hWnd, 5, 330, 150, 20, true, "Use Addresses For Memory", Id::CheckUseAddressesForMemory);
    refreshing = false;
}

ChkdProfile & ProfileGeneralWindow::getEditProfile()
{
    for ( auto & profile : chkd.profiles.profiles )
    {
        if ( this->editProfileName == profile->profileName )
        {
            return *(profile);
        }
    }
    this->editProfileName = chkd.profiles().profileName;
    return chkd.profiles();
}

void ProfileGeneralWindow::CheckReplaceScrPath()
{
    if ( possibleScrPathUpdate )
    {
        possibleScrPathUpdate = false;
        if ( auto newScrPath = editScrPath.GetWinText() )
        {
            auto & editProfile = getEditProfile();

            bool updateCascPath = editProfile.remastered.cascPath.empty() ||
                isSamePath(editProfile.remastered.starCraftPath, editProfile.remastered.cascPath);

            if ( newScrPath.value() != editProfile.remastered.starCraftPath )
            {
                editProfile.remastered.starCraftPath = *newScrPath;
                if ( updateCascPath )
                    editProfile.remastered.cascPath = editProfile.remastered.starCraftPath;

                editProfile.saveProfile();
                RefreshWindow();
                logger.debug() << "Updated scrPath to " << editProfile.remastered.starCraftPath << std::endl;
            }
        }
    }
}

void ProfileGeneralWindow::CheckReplaceCascPath()
{
    if ( possibleCascPathUpdate )
    {
        possibleCascPathUpdate = false;
        if ( auto newCascPath = editCascPath.GetWinText() )
        {
            auto & editProfile = getEditProfile();
            if ( newCascPath.value() != editProfile.remastered.cascPath )
            {
                editProfile.remastered.cascPath = *newCascPath;
                editProfile.saveProfile();
                logger.debug() << "Updated cascPath to " << editProfile.remastered.cascPath << std::endl;
            }
        }
    }
}

void ProfileGeneralWindow::CheckReplaceScPath()
{
    if ( possibleScPathUpdate )
    {
        possibleScPathUpdate = false;
        if ( auto newScPath = editScPath.GetWinText() )
        {
            auto & editProfile = getEditProfile();

            // Update dat files if they were empty/not-set or are not in the previous sc directory
            bool updateStarDat = editProfile.classic.starDatPath.empty() ||
                isInDirectory(editProfile.classic.starDatPath, editProfile.classic.starCraftPath);
            bool updateBrooDat = editProfile.classic.brooDatPath.empty() ||
                isInDirectory(editProfile.classic.brooDatPath, editProfile.classic.starCraftPath);
            bool updatePatchRtDat = editProfile.classic.patchRtPath.empty() ||
                isInDirectory(editProfile.classic.patchRtPath, editProfile.classic.starCraftPath);

            if ( newScPath.value() != editProfile.classic.starCraftPath )
            {
                editProfile.classic.starCraftPath = *newScPath;
                if ( updateStarDat )
                    editProfile.classic.starDatPath = makeSystemFilePath(editProfile.classic.starCraftPath, "StarDat.mpq");
                if ( updateBrooDat )
                    editProfile.classic.brooDatPath = makeSystemFilePath(editProfile.classic.starCraftPath, "BrooDat.mpq");
                if ( updatePatchRtDat )
                    editProfile.classic.patchRtPath = makeSystemFilePath(editProfile.classic.starCraftPath, "patch_rt.mpq");

                editProfile.saveProfile();
                RefreshWindow();
                logger.debug() << "Updated starCraftPath to " << editProfile.classic.starCraftPath << std::endl;
            }
        }
    }
}

void ProfileGeneralWindow::CheckReplaceStarDatPath()
{
    if ( possibleStarDatPathUpdate )
    {
        possibleStarDatPathUpdate = false;
        if ( auto newStarDatPath = editStarDatPath.GetWinText() )
        {
            auto & editProfile = getEditProfile();
            if ( newStarDatPath != editProfile.classic.starDatPath )
            {
                editProfile.classic.starDatPath = *newStarDatPath;
                editProfile.saveProfile();
                logger.debug() << "Updated starDatPath to " << editProfile.classic.starDatPath << std::endl;
            }
        }
    }
}

void ProfileGeneralWindow::CheckReplaceBrooDatPath()
{
    if ( possibleBrooDatPathUpdate )
    {
        possibleBrooDatPathUpdate = false;
        if ( auto newBrooDatPath = editBrooDatPath.GetWinText() )
        {
            auto & editProfile = getEditProfile();
            if ( newBrooDatPath != editProfile.classic.brooDatPath )
            {
                editProfile.classic.brooDatPath = *newBrooDatPath;
                editProfile.saveProfile();
                logger.debug() << "Updated brooDatPath to " << editProfile.classic.brooDatPath << std::endl;
            }
        }
    }
}

void ProfileGeneralWindow::CheckReplacePatchRtPath()
{
    if ( possiblePatchRtPathUpdate )
    {
        possiblePatchRtPathUpdate = false;
        if ( auto newPatchRtPath = editPatchRtDatPath.GetWinText() )
        {
            auto & editProfile = getEditProfile();
            if ( newPatchRtPath != editProfile.classic.patchRtPath )
            {
                editProfile.classic.patchRtPath = *newPatchRtPath;
                editProfile.saveProfile();
                logger.debug() << "Updated patchRtPath to " << editProfile.classic.patchRtPath << std::endl;
            }
        }
    }
}

void ProfileGeneralWindow::CheckReplaceLogLevel()
{
    if ( possibleLogLevelUpdate )
    {
        possibleLogLevelUpdate = false;
        int newLogLevel = 0;
        if ( dropLogLevel.GetEditNum<int>(newLogLevel) )
        {
            if ( newLogLevel >= 0 )
            {
                auto & editProfile = getEditProfile();
                if ( LogLevel(newLogLevel) != editProfile.logger.defaultLogLevel )
                {
                    editProfile.logger.defaultLogLevel = LogLevel(newLogLevel);
                    editProfile.saveProfile();
                    logger.debug() << "Updated defaultLogLevel to " << editProfile.logger.defaultLogLevel << std::endl;
                }
            }
        }
    }
}

void ProfileGeneralWindow::CheckReplaceMaxHistMemory()
{
    if ( possibleMaxHistMemoryUpdate )
    {
        possibleMaxHistMemoryUpdate = false;
        int newMaxHistMemoryUseMb = 0;
        if ( editMaxHistMemoryUseMb.GetEditNum<int>(newMaxHistMemoryUseMb) )
        {
            if ( newMaxHistMemoryUseMb < 5 )
            {
                editMaxHistMemoryUseMb.SetEditNum<int>(5);
                possibleMaxHistMemoryUpdate = false;
                newMaxHistMemoryUseMb = 5;
                logger.warn("Adjusted given max hist memory use to the minimum value of 5MB");
            }

            auto & editProfile = getEditProfile();
            if ( newMaxHistMemoryUseMb != editProfile.history.maxMemoryUseMb )
            {
                editProfile.history.maxMemoryUseMb = newMaxHistMemoryUseMb;
                editProfile.saveProfile();
                logger.debug() << "Updated maxHistMemoryUse to " << editProfile.history.maxMemoryUseMb << "MB" << std::endl;
            }
        }
    }
}

void ProfileGeneralWindow::CheckReplaceMaxHistActions()
{
    if ( possibleMaxHistActionsUpdate )
    {
        possibleMaxHistActionsUpdate = false;
        int newMaxHistActions = 0;
        if ( editMaxHistActions.GetEditNum<int>(newMaxHistActions) )
        {
            if ( newMaxHistActions < 5 )
            {
                editMaxHistActions.SetEditNum<int>(5);
                possibleMaxHistActionsUpdate = false;
                newMaxHistActions = 5;
                logger.warn("Adjusted given max hist actions to the minimum value of 5");
            }

            auto & editProfile = getEditProfile();
            if ( newMaxHistActions != editProfile.history.maxActions )
            {
                editProfile.history.maxActions = newMaxHistActions;
                editProfile.saveProfile();
                logger.debug() << "Updated maxHistActions to " << editProfile.history.maxActions << std::endl;
            }
        }
    }
}

void ProfileGeneralWindow::CheckReplaceDeathTableStart()
{
    if ( possibleDeathTableStartUpdate )
    {
        possibleDeathTableStartUpdate = false;
        if ( auto rawNewDeathTableStart = editDeathTableStart.GetWinText() )
        {
            auto & editProfile = getEditProfile();
            
            if ( rawNewDeathTableStart->starts_with("0x") || rawNewDeathTableStart->starts_with("0X") )
                rawNewDeathTableStart = rawNewDeathTableStart->substr(2, rawNewDeathTableStart->size()-2);
            else if ( rawNewDeathTableStart.value()[0] == 'x' || rawNewDeathTableStart.value()[0] == 'X' )
                rawNewDeathTableStart = rawNewDeathTableStart->substr(1, rawNewDeathTableStart->size()-1);

            std::uint64_t newDeathTableStart = 0;
            try {
                auto newDtStart = std::stoll(*rawNewDeathTableStart, nullptr, 16);
                if ( newDtStart < 0 )
                {
                    logger.error() << "Invalid death table start value" << std::endl;
                    return;
                }
                else
                    newDeathTableStart = static_cast<std::uint64_t>(newDtStart);
            } catch ( ... ) {
                logger.error() << "Invalid death table start value" << std::endl;
                return;
            }

            if ( newDeathTableStart != editProfile.triggers.deathTableStart )
            {
                editProfile.triggers.deathTableStart = newDeathTableStart;
                editProfile.saveProfile();
                logger.debug() << "Updated death table start to " << to_hex_string(editProfile.triggers.deathTableStart, true) << std::endl;
            }
        }
    }
}

void ProfileGeneralWindow::CheckEditUpdates()
{
    CheckReplaceScrPath();
    CheckReplaceCascPath();
    CheckReplaceScPath();
    CheckReplaceStarDatPath();
    CheckReplaceBrooDatPath();
    CheckReplacePatchRtPath();
    CheckReplaceLogLevel();
    CheckReplaceMaxHistMemory();
    CheckReplaceMaxHistActions();
    CheckReplaceDeathTableStart();
}
