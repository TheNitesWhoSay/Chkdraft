#ifndef PLUGINS_H
#define PLUGINS_H
#include <Windows.h>

#define PLUGIN_MSG_START        (WM_APP+200)
#define PLUGIN_MSG_END          (WM_APP+205)

/** Basic messages, get a handle to CHKDraft with the above
    messages, then you can use the messages that follow */

    #define SAVE_MAP            (PLUGIN_MSG_START+0)
        /** This will cause CHKDraft to save the specified map

            wParam: mapID, if NULL, the current map is saved
            lParam: NULL
            return: The saved mapID if successful, 0 otherwise */

    #define CLOSE_MAP            (PLUGIN_MSG_START+5)
        /** This will cause CHKDraft to close the specified map

            wParam: mapID, if NULL, the current map (if any) is closed
            lParam: NULL
            return: The closed mapID if successful, 0 otherwise */


/** Getting a scenario file, modifying it, and returning it is one of
    the most powerful ways to interface with CHKDraft, include mapping
    core in your project and use its methods or parse and edit the scenario
    file yourself after using this message to get a chk file */

    #define COPY_CHK_FILE           (PLUGIN_MSG_START+1)
        /** This will cause CHKDraft to send you WM_COPYDATA
            containing a chk file (as specified in Scenario::Serialize)
            in lpData, its size in cbData, and MAKELONG(COPY_CHK_FILE, mapID)
            as dwData

            wParam: HWND to the window WM_COPYDATA with the CHK file will be
                    sent to
            lParam: mapID, if NULL, the currently focused map will be used
            return: The copied mapID if successful, 0 otherwise */

    #define UPDATE_CHK_FILE         (PLUGIN_MSG_START+2)
        /** Send this as part of WM_COPYDATA to update and refresh
            the map; it is recommeded to return the scenario file
            ASAP after retrieving it so changes aren't made and lost
            in CHKDraft

            wParam: set to sending handle
            lParam: COPYDATASTRUCT with lpData = chk file, size
                    as cbData and dwData as described below

                    dwData = (LPARAM)MAKELONG(UPDATE_CHK_FILE, mapID);
                    if mapID is 0 the current map is the one updated.
            return: The updated mapID if successful, 0 otherwise */


/** To replace triggers in the given form send one of the following
    as part of the dwData in the COPYDATASTRUCT of WM_COPYDATA
    where lpData is the trigger bytes, cbData the length, and
    dwData = (ULONG_PTR)MAKELONG(REPLACE_TRIGGERS_(form), mapID);
    Chkdrafts WM_COPYDATA returns TRUE if the operation succeeded,
    FALSE otherwise */

    #define REPLACE_TRIGGERS_BYTES  (PLUGIN_MSG_START+3)
    #define REPLACE_TRIGGERS_TEXT   (PLUGIN_MSG_START+4)



LRESULT CALLBACK PluginProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool runMpqRecompiler();

#endif
