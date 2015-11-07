#ifndef CONDITIONSGRID_H
#define CONDITIONSGRID_H
#include "Windows UI/WindowsUI.h"
#include "Common Files/CommonFiles.h"
#include "Suggestions.h"

class IConditionGridUser // ~Interface
{
public:
	virtual void ConditionEnableToggled(u8 conditionNum) = 0;
};

class ConditionsGrid : public GridViewControl
{
	public:
		CheckBoxControl checkEnabled[16];

		ConditionsGrid(IConditionGridUser &user);
		bool CreateThis(HWND hParent, int x, int y, int width, int height, u32 id);
		void SetEnabledCheck(u8 conditionId, bool enabled);

		Suggestions &GetSuggestions();
		bool HasUpDownedThisEdit();

		virtual bool allowKeyNavDuringEdit(GVKey key); // Checks if navigation is allowed by the specified key
		virtual bool allowSel(int x, int y);
		virtual bool allowSel(int xStart, int xEnd, int yStart, int yEnd);
		virtual void adjustSel(int &xStart, int &xEnd, int &yStart, int &yEnd);
		virtual void CellClicked(int x, int y);

	protected:
		bool CreateSubWindows(HWND hWnd);
		virtual void StartEditing(int xClick, int yClick, char initChar);
		virtual void KeyDown(WPARAM wParam);
		virtual void EditTextChanged(std::string &str);

		virtual LRESULT Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr);
		virtual LRESULT Command(HWND hWnd, WPARAM wParam, LPARAM lParam);
		virtual LRESULT ControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:
		Suggestions suggestions;
		IConditionGridUser& user;
		bool hasUpDownedThisEdit;
		bool startedByClick;
		bool ignoreChange;

		BOOL ProcessTrack(NMHEADER* nmHeader);
		BOOL ProcessTrackW(NMHEADERW* nmHeader);

		ConditionsGrid(); // Disallow ctor
};

#endif