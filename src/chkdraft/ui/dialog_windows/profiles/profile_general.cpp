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

void ProfileGeneralWindow::RefreshWindow()
{
    auto & editProfile = getEditProfile();
    bool useRemastered = editProfile.useRemastered;
    checkUseRemastered.SetCheck(useRemastered);
    if ( useRemastered )
    {
        groupRemastered.EnableThis();
        textScrPath.EnableThis();
        editScrPath.EnableThis();
        browseScrPath.EnableThis();
        textCascPath.EnableThis();
        editCascPath.EnableThis();
        browseCascPath.EnableThis();
        textScrDefaultSkin.EnableThis();

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
    }
    else // use classic
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

        groupRemastered.DisableThis();
        textScrPath.DisableThis();
        editScrPath.DisableThis();
        browseScrPath.DisableThis();
        textCascPath.DisableThis();
        editCascPath.DisableThis();
        browseCascPath.DisableThis();
        textScrDefaultSkin.DisableThis();
    }
    editScPath.SetText(editProfile.classic.starCraftPath);
    editStarDatPath.SetText(editProfile.classic.starCraftPath);
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

    textMaxHistMemoryUseMb.CreateThis(hWnd, 54, 280, 140, 20, "Max Hist Actions:", Id::TextMaxHistMemoryUseMb);
    editMaxHistMemoryUseMb.CreateThis(hWnd, 142, 280, 100, 20, false, Id::EditMaxHistMemoryUseMb);
    
    textMaxHistActions.CreateThis(hWnd, 5, 255, 140, 20, "Max Hist Memory Use (MB):", Id::TextMaxHistActions);
    editMaxHistActions.CreateThis(hWnd, 142, 255, 100, 20, false, Id::EditMaxHistActions);

    textDeathTableStart.CreateThis(hWnd, 49, 305, 140, 20, "Death Table Start:", Id::TextDeathTableStart);
    editDeathTableStart.CreateThis(hWnd, 142, 305, 140, 20, false, Id::EditDeathTableStart);
    editDeathTableStart.DisableThis();
    checkUseDefaultDeathTableStart.CreateThis(hWnd, editDeathTableStart.Right()+2, 309, 100, 20, true, "Use Default", Id::CheckUseDefaultDeathTableStart);
    checkUseDefaultDeathTableStart.FocusThis();
    
    checkUseAddressesForMemory.CreateThis(hWnd, 5, 330, 150, 20, true, "Use Addresses For Memory", Id::CheckUseAddressesForMemory);
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
