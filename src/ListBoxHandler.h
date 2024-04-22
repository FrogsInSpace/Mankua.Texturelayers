#pragma once


// custom drag&drop notification message
// dragged item as WPARAM
// dropped position as LPARAM
#define USER_LB_DRAGDROP	WM_USER+1
#define USER_LB_SETSEL		WM_USER+2
#define USER_LB_ACTIVATE	WM_USER+3


class CListBoxHandler
{
public:
	CListBoxHandler();
	virtual ~CListBoxHandler();

public:
	BOOL SetTarget(HINSTANCE hInstance, HWND hwndListbox);

protected:
	int					xpos;
	int					ypos;
	HWND				m_hwndListBox;
	static WNDPROC		m_pfOriginalProc;
	int					m_curSel;
	HCURSOR				m_hcurDrag;
	HINSTANCE			m_hInstance;

protected:
	HCURSOR GetDragCursor();

	//Win32 : static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LONG_PTR CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
	HWND GetHwnd();
};
