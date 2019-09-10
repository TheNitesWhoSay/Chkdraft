#include "MainToolbar.h"

enum class Id
{
    COMBOBOX_LAYER = ID_FIRST,
    COMBOBOX_PLAYER,
    COMBOBOX_ZOOM,
    COMBOBOX_TERRAIN
};

MainToolbar::~MainToolbar()
{

}

bool MainToolbar::CreateThis(HWND hParent, u64 windowId)
{
    // Initilize primary toolbar
    if ( WindowControl::CreateControl( 0, TOOLBARCLASSNAME, "", WS_CHILD|WS_VISIBLE|TBSTYLE_TOOLTIPS,
                                       0, 0, 0, 0, hParent, (HMENU)windowId, false ) )
    {
        HWND hTool = getHandle();
        SendMessage(hTool, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
        SendMessage(hTool, TB_SETMAXTEXTROWS, 0, 0);

        // Set toolbar to default button set
        TBADDBITMAP tbab;
        tbab.hInst = HINST_COMMCTRL;
        tbab.nID = IDB_STD_SMALL_COLOR;
        SendMessage(hTool, TB_ADDBITMAP, 0, (LPARAM)&tbab);

        // Load Save-As image from resource and store the resulting index as STD_FILESAVEAS
        HIMAGELIST hImages = ImageList_Create(16, 16, 0, 1, 1);
        HBITMAP hBmp = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_SAVEAS), IMAGE_BITMAP, 16, 16, LR_DEFAULTCOLOR);
        ImageList_Add(hImages, hBmp, NULL);
        TBADDBITMAP tb; tb.hInst = NULL; tb.nID = (UINT_PTR)hBmp;
        int STD_FILESAVEAS = (int)SendMessage(hTool, TB_ADDBITMAP, 0, (LPARAM)&tb);

        icux::uistring newMap = icux::toUistring("New");
        icux::uistring open = icux::toUistring("Open");
        icux::uistring save = icux::toUistring("Save");
        icux::uistring saveAs = icux::toUistring("Save As");
        icux::uistring undo = icux::toUistring("Undo");
        icux::uistring redo = icux::toUistring("Redo");
        icux::uistring properties = icux::toUistring("Properties");
        icux::uistring deleteSelection = icux::toUistring("Delete");
        icux::uistring copy = icux::toUistring("Copy");
        icux::uistring cut = icux::toUistring("Cut");
        icux::uistring paste = icux::toUistring("Paste");

        // Add buttons to toolbar
        TBBUTTON tbb[13];
        ZeroMemory(tbb, sizeof(TBBUTTON));
        tbb[ 0].iBitmap = STD_FILENEW;    tbb[ 0].fsState = TBSTATE_ENABLED; tbb[ 0].fsStyle = TBSTYLE_BUTTON; tbb[ 0].idCommand = ID_FILE_NEW1;       tbb[ 0].iString = (INT_PTR)newMap.c_str();
        tbb[ 1].iBitmap = STD_FILEOPEN;   tbb[ 1].fsState = TBSTATE_ENABLED; tbb[ 1].fsStyle = TBSTYLE_BUTTON; tbb[ 1].idCommand = ID_FILE_OPEN1;      tbb[ 1].iString = (INT_PTR)open.c_str();
        tbb[ 2].iBitmap = STD_FILESAVE;   tbb[ 2].fsState = 0;               tbb[ 2].fsStyle = TBSTYLE_BUTTON; tbb[ 2].idCommand = ID_FILE_SAVE1;      tbb[ 2].iString = (INT_PTR)save.c_str();
        tbb[ 3].iBitmap = STD_FILESAVEAS; tbb[ 3].fsState = 0;               tbb[ 3].fsStyle = TBSTYLE_BUTTON; tbb[ 3].idCommand = ID_FILE_SAVEAS1;    tbb[ 3].iString = (INT_PTR)saveAs.c_str();
        tbb[ 4].iBitmap = 8;              tbb[ 4].fsState = 0;               tbb[ 4].fsStyle = BTNS_SEP;
        tbb[ 5].iBitmap = STD_UNDO;       tbb[ 5].fsState = 0;               tbb[ 5].fsStyle = TBSTYLE_BUTTON; tbb[ 5].idCommand = ID_EDIT_UNDO1;      tbb[ 5].iString = (INT_PTR)undo.c_str();
        tbb[ 6].iBitmap = STD_REDOW;      tbb[ 6].fsState = 0;               tbb[ 6].fsStyle = TBSTYLE_BUTTON; tbb[ 6].idCommand = ID_EDIT_REDO1;      tbb[ 6].iString = (INT_PTR)redo.c_str();
        tbb[ 7].iBitmap = STD_PROPERTIES; tbb[ 7].fsState = 0;               tbb[ 7].fsStyle = TBSTYLE_BUTTON; tbb[ 7].idCommand = ID_EDIT_PROPERTIES; tbb[ 7].iString = (INT_PTR)properties.c_str();
        tbb[ 8].iBitmap = STD_DELETE;     tbb[ 8].fsState = 0;               tbb[ 8].fsStyle = TBSTYLE_BUTTON; tbb[ 8].idCommand = ID_EDIT_DELETE;     tbb[ 8].iString = (INT_PTR)deleteSelection.c_str();
        tbb[ 9].iBitmap = 8;              tbb[ 9].fsState = 0;               tbb[ 9].fsStyle = BTNS_SEP;
        tbb[10].iBitmap = STD_COPY;       tbb[10].fsState = 0;               tbb[10].fsStyle = TBSTYLE_BUTTON; tbb[10].idCommand = ID_EDIT_COPY1;      tbb[10].iString = (INT_PTR)copy.c_str();
        tbb[11].iBitmap = STD_CUT;        tbb[11].fsState = 0;               tbb[11].fsStyle = TBSTYLE_BUTTON; tbb[11].idCommand = ID_EDIT_CUT1;       tbb[11].iString = (INT_PTR)cut.c_str();
        tbb[12].iBitmap = STD_PASTE;      tbb[12].fsState = 0;               tbb[12].fsStyle = TBSTYLE_BUTTON; tbb[12].idCommand = ID_EDIT_PASTE1;     tbb[12].iString = (INT_PTR)paste.c_str();
        SendMessage(hTool, TB_ADDBUTTONS, sizeof(tbb)/sizeof(TBBUTTON), (LPARAM)&tbb);

        // Add layer ComboBox to toolbar
        const std::vector<std::string> layerStrings = { "Terrain", "Doodads", "Fog of War", "Locations", "Units", "Sprites", "Preview Fog", "Copy\\Cut\\Paste\\Brush" };
        layerBox.CreateThis(hTool, 277, 2, 110, 200, false, false, (u64)Id::COMBOBOX_LAYER, layerStrings, defaultFont);
        layerBox.SetSel(0);
        ShowWindow(layerBox.getHandle(), SW_HIDE); // Hide until a map is open

        // Add zoom ComboBox to toolbar
        const std::vector<std::string> zoomStrings = { "400% Zoom", "300% Zoom", "200% Zoom", "150% Zoom", "100% Zoom", "66% Zoom",
                                      "50% Zoom", "33% Zoom", "25% Zoom", "10% Zoom"};
        zoomBox.CreateThis(hTool, 395, 2, 80, 200, false, false, (u64)Id::COMBOBOX_ZOOM, zoomStrings, defaultFont);
        zoomBox.SetSel(4);
        ShowWindow(zoomBox.getHandle(), SW_HIDE); // Hide until a map is open

        // Add player ComboBox to toolbar
        playerBox.CreateThis(hTool, 483, 2, 75, 200, (u64)Id::COMBOBOX_PLAYER, false);
        ShowWindow(playerBox.getHandle(), SW_HIDE); // Hide until a map is open

        // Add terrain ComboBox to toolbar
        const std::vector<std::string> terrPalette = { "Isometrical", "Rectangular", "Subtile", "Tileset Indexed", "Copy/Cut/Paste" };
        terrainBox.CreateThis(hTool, 483, 2, 100, 200, false, false, (u64)Id::COMBOBOX_TERRAIN, terrPalette, defaultFont);
        terrainBox.SetSel(0);
        ShowWindow(terrainBox.getHandle(), SW_HIDE); // Hide until a map is open

        return true;
    }
    else
        return false;
}

void MainToolbar::AutoSize()
{
    SendMessage(getHandle(), TB_AUTOSIZE, 0, 0);
}
