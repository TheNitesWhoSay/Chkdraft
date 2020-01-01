#ifndef WINUIENUMERATIONS_H
#define WINUIENUMERATIONS_H
#include <Windows.h>
#include "DataTypes.h"

namespace WinLib {

    enum_t(LB, u32, {
        /** Sent to the parent when an item is double clicked
            The WPARAM specifies the item index that was double clicked
            The LPARAM is the list box's handle */
        WM_DBLCLKITEM = (WM_APP+1), 

        /** Sent to the parent before several items are measured
            The WPARAM is unused
            The LPARAM is the list box's handle */
        WM_PREMEASUREITEMS,

        /** Sent to the parent after several items are measured
            The WPARAM is unused
            The LPARAM is the list box's handle */
        WM_POSTMEASUREITEMS,

        /** Sent to the parent before several items are drawn
            The WPARAM is unused
            The LPARAM is the list box's handle */
        WM_PREDRAWITEMS,

        /** Sent to the parent after several items are drawn
            The WPARAM is unused
            The LPARAM is the list box's handle */
        WM_POSTDRAWITEMS,

        /** Possibly sent by windows containing ListBoxes to signal
            other windows that a ListBox's selection changed 
            The WPARAM is unused
            The LPARAM is a (string*) to the newly selected string */
        WM_NEWSELTEXT,

        LB_LAST
    });

    enum_t(GV, u32, {
        /** Sent to the parent to request a specific row be drawn
            The wParam and lParam are the same as in WM_DRAWITEM,
            the return value is ignored */
        WM_DRAWGRIDVIEWITEM = LB::LB_LAST,

        /** Sent to the parent to allow it to draw any touchups
            after all the grid items have been drawn
            WPARAM: The HDC being drawn too
            LPARAM: Unused
            return: Ignored */
        WM_DRAWTOUCHUPS,

        /** Sent to the parent to request the position of the caret
            within the text in the specified grid view item
            WPARAM:     LOWORD(wParam) = gridItemX
                        HIWORD(wParam) = gridItemY
            LPARAM:     LOWORD(lParam) = click distance from left edge of the item
                        HIWORD(lParam) = click distance from top edge of the item
            Return: The caret position if successful otherwise -1 */
        WM_GETGRIDITEMCARETPOS,

        /** Sent when the focus is about to leave an item,
            should be used to validate and clean text
            WPARAM:     LOWORD(wParam) = gridItemX
                        HIWORD(wParam) = gridItemY
            LPARAM: Contains a pointer (string*) to the string the item
                    will be changed too, you may alter this string.
            Return: TRUE to allow the change to proceed, FALSE to prevent it */
        WM_GRIDITEMCHANGING,

        /** Sent when an item is deleted
            WPARAM:     LOWORD(wParam) = gridItemX
                        HIWORD(wParam) = gridItemY
            LPARAM: Unused
            Return: TRUE to allow the deletion, FALSE to prevent it */
        WM_GRIDITEMDELETING,

        /** Sent when a delete operation has completed
            WPARAM: Unused
            LPARAM: Unused
            Return: Unused */
        WM_GRIDDELETEFINISHED,

        /** Sent to the parent to request the drawing-width of a specific
            item (as per how it's drawn in WM_DRAWGRIDVIEWITEM),
            you must return a valid width for double-clicking columns
            to resize columns appropriately.
            WPARAM:     LOWORD(wParam) = gridItemX
                        HIWORD(wParam) = gridItemY
            LPARAM: Unused
            Return: The width if successful, 0 otherwise */
        WM_GETGRIDITEMWIDTH,

        /** Sent to the parent when the selected square has changed *
            WPARAM:     LOWORD(wParam) = gridItemX
                        HIWORD(wParam) = gridItemY
            LPARAM: Unused
            Return: Unused */
        WM_GRIDSELCHANGED,

        /** Sent to the parent when a grid item has started to be edited 
            WPARAM:     LOWORD(wParam) = gridItemX
                        HIWORD(wParam) = gridItemY
            LPARAM: Unused
            Return: Unused */
        WM_GRIDEDITSTART,

        /** Sent to the parent when a grid item has finished being edited
            WPARAM:     LOWORD(wParam) = gridItemX
                        HIWORD(wParam) = gridItemY
            LPARAM: Unused
            Return: Unused */
        WM_GRIDEDITEND,

        /** Sent to the keyReciever when the text in the edit box has changed
            WPARAM: Unused
            LPARAM: a (string*) to the new string contents
            Return: Unused */
        WM_NEWGRIDTEXT,

        GV_LAST
    });

    // First user-message used by a given set of sub-windows
    #define MSG_FIRST (GV_LAST+1)

    // First identifier used by a given set of sub-windows
    #define ID_FIRST 41001

}

#endif