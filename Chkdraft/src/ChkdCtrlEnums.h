#ifndef CHKDCTRLENUMS_H
#define CHKDCTRLENUMS_H

#define CHKDE_FIRST WINUI_LAST

enum CG
{
	/** Sent to the parent when a condition has been enabled
		WPARAM:	LOWORD = conditionNumber
				HIWORD = 0 if enabled, nonzero (typically 1)
				if disabled*/
	WM_CONDITIONENABLED
};

#endif