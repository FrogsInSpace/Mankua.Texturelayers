#include "max.h"

#ifndef MAX_RELEASE_R9
#include "max_mem.h"
#endif

#include "ListBoxHandler.h"
#include "modsres.h"

WNDPROC CListBoxHandler::m_pfOriginalProc = NULL;

CListBoxHandler::CListBoxHandler()
{
	xpos = -1;
	m_hwndListBox = NULL;
	m_pfOriginalProc = NULL;
	m_curSel = LB_ERR;
	m_hcurDrag = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_DRAGCURSOR));
}

CListBoxHandler::~CListBoxHandler(void) 
{
	if (m_hwndListBox && m_pfOriginalProc)
		//Win32 : SetWindowLong(m_hwndListBox, GWL_WNDPROC, (LONG)CListBoxHandler::m_pfOriginalProc);
		//SetWindowLong(m_hwndListBox, GWLP_WNDPROC, (LONG)CListBoxHandler::m_pfOriginalProc);
		SetWindowLongPtr( m_hwndListBox, GWLP_WNDPROC, (LONG_PTR)CListBoxHandler::m_pfOriginalProc );
}

BOOL CListBoxHandler::SetTarget(HINSTANCE hInstance, HWND hwndListbox) 
{
	// attach only once
	if (m_hwndListBox || m_pfOriginalProc)
		return FALSE;

	if (!IsWindow(hwndListbox))
		return FALSE;
	
	m_hInstance = hInstance;

	m_hwndListBox = hwndListbox;

	//Win32 : m_pfOriginalProc = (WNDPROC)SetWindowLong(m_hwndListBox, GWL_WNDPROC, (LONG)WindowProc);
	//m_pfOriginalProc = (WNDPROC)SetWindowLong(m_hwndListBox, GWLP_WNDPROC, (LONG)WindowProc);
	m_pfOriginalProc = (WNDPROC)SetWindowLongPtr( m_hwndListBox, GWLP_WNDPROC, (LONG_PTR)WindowProc );
	
	SetProp(m_hwndListBox, "PROP_THIS", this);

	return TRUE;
	}

HWND CListBoxHandler::GetHwnd()
{
	return m_hwndListBox;
}

HCURSOR CListBoxHandler::GetDragCursor()
{
	return m_hcurDrag;
}

//Win32 : LRESULT CALLBACK CListBoxHandler::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
LONG_PTR CALLBACK CListBoxHandler::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CListBoxHandler* pThis = (CListBoxHandler*)GetProp(hwnd, "PROP_THIS");

	LRESULT result;
	int cur_sel = LB_ERR;

	switch(uMsg) {

		case WM_LBUTTONDOWN:
			pThis->xpos = LOWORD(lParam);
			pThis->ypos = HIWORD(lParam);
			result = CallWindowProc(m_pfOriginalProc, hwnd, uMsg, wParam, lParam);
			pThis->m_curSel = (int)SendMessage(pThis->GetHwnd(), LB_GETCURSEL, 0, 0);
			return result;

		case WM_MOUSEMOVE: {
			if (pThis->m_curSel != LB_ERR)
				SetCursor(pThis->GetDragCursor());
			else
				SetCursor(LoadCursor(NULL, IDC_ARROW));
			}
			break;

		case WM_LBUTTONUP: {
			HWND hwndParent = GetParent(pThis->GetHwnd());
			cur_sel = (int)SendMessage(pThis->GetHwnd(), LB_GETCURSEL, 0, 0);
			result = CallWindowProc(m_pfOriginalProc, hwnd, uMsg, wParam, lParam);

			int delta_pos = (abs(LOWORD(lParam)-pThis->xpos)+abs(HIWORD(lParam)-pThis->ypos));

			if ( delta_pos<=4 ) {
				if ( pThis->xpos>=1 && pThis->xpos<=11 ) {
					SendMessage(hwndParent, USER_LB_ACTIVATE, 0, (LPARAM)cur_sel);
					}
				else if ( pThis->m_curSel != LB_ERR && pThis->m_curSel == cur_sel ) {
					SendMessage(hwndParent, USER_LB_SETSEL, 0, (LPARAM)cur_sel);
					}
				}
			
			if ( pThis->m_curSel != LB_ERR && pThis->m_curSel != cur_sel ) {
				SendMessage(hwndParent, USER_LB_DRAGDROP, (WPARAM)pThis->m_curSel, (LPARAM)cur_sel);
			
				TCHAR buffer[1024];
				int to = cur_sel;
				int from = pThis->m_curSel;
				if ( to>from ) {
				//	uvw_groups.Insert(to,1,&uvw_group);
				//	uvw_groups.Delete(from,1);
					SendMessage(pThis->GetHwnd(), LB_GETTEXT, from, (LPARAM)buffer);
					SendMessage(pThis->GetHwnd(), LB_INSERTSTRING, to, (LPARAM)buffer);
					SendMessage(pThis->GetHwnd(), LB_DELETESTRING, from, 0);
					SendMessage(pThis->GetHwnd(), LB_SETCURSEL, to, 0);
					}
				if ( from>to ) {
				//	uvw_groups.Insert(to,1,&uvw_group);
				//	uvw_groups.Delete(from+1,1);
					SendMessage(pThis->GetHwnd(), LB_GETTEXT, from, (LPARAM)buffer);
					SendMessage(pThis->GetHwnd(), LB_INSERTSTRING, to, (LPARAM)buffer);
					SendMessage(pThis->GetHwnd(), LB_DELETESTRING, from+1, 0);
					SendMessage(pThis->GetHwnd(), LB_SETCURSEL, to, 0);
					}
			
				}

			pThis->m_curSel = LB_ERR;
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			}
			return result;

		case WM_RBUTTONDOWN:
			return FALSE;
			break;

		case WM_SETCURSOR:
			return TRUE;
		}

	return CallWindowProc(m_pfOriginalProc, hwnd, uMsg, wParam, lParam);
}
