#include "mods.h"		
#include "texutil.h"
#include "bmmlib.h"
#include "simpobj.h"
#include "simpmod.h"
#include "decomp.h"
#include "mapping.h"
#include "uvwgroup.h"
#include "undo.h"
#include "texlay_mc_data.h"
#include "buildver.h"
#include "surf_api.h"
#include "pblock.h"
#include "meshadj.h"
#include "common.h"
#include "texlay.h"
#include "..\..\UVWFrame\code\uvwframe.h"
#include "3dsmaxport.h"

#define ID_ADD		0x2000
#define ID_DEL		0x2500
#define ID_SAVE		0x3000
#define ID_LOAD		0x3010
#define ID_COPY		0x3020
#define ID_CUT		0x3030
#define ID_PASTE	0x3040
#define ID_TOOLS	0x3050

// PICK NODE MODE

#define	THE_HOLD_TLP_BEGIN	\
	theHold.SuperBegin();	\
	theHold.Begin();

#define	THE_HOLD_TLP_ACCEPT	\
	theHold.Accept(TSTR(_T("Parameter Change")));	\
	theHold.SuperAccept(TSTR(_T("Parameter Change")));

#define THE_HOLD_TLP_CANCEL \
	theHold.Cancel();		\
	theHold.SuperCancel();

#define TEXLAY_LABEL	"T e x t u r e   L a y e r s"

class PickNodeMode : public PickModeCallback, public PickNodeCallback {
	public:		
		MultiMapMod *mm;
		
		PickNodeMode(MultiMapMod *mmm, int f=0) {mm = mmm;}
		~PickNodeMode() {}

		// --- Methods from PickModeCallback ---
		BOOL HitTest(IObjParam *ip, HWND hWnd, ViewExp *vpt,IPoint2 m, int flags);
		BOOL Pick(IObjParam *ip, ViewExp *vpt);
		void EnterMode(IObjParam *ip);
		void ExitMode(IObjParam *ip);
		BOOL RightClick(IObjParam *ip, ViewExp *vpt) { return TRUE; }
		PickNodeCallback *GetFilter() {return this;}
		BOOL Filter(INode *node);
};

// TLTODO: Que filtre solo los nurbs y los pw grids o algo asi...
BOOL PickNodeMode::Filter(INode *node) {
	if (node) {
		ObjectState os = node->EvalWorldState(0);
		if ( mm->uvwProy[mm->current_channel]->GetMappingType() == MAP_TL_SPLINE)
			if (os.obj->SuperClassID() == SHAPE_CLASS_ID) {
				SetCursor(mm->ip->GetSysCursor(SYSCUR_SELECT));
				return TRUE;
				}
		if ( mm->uvwProy[mm->current_channel]->GetMappingType() == MAP_TL_FFM)
			if (os.obj->IsParamSurface()) {
				SetCursor(mm->ip->GetSysCursor(SYSCUR_SELECT));
				return TRUE;
				}
		if ( mm->uvwProy[mm->current_channel]->GetMappingType() == MAP_TL_FRAME )
			if ( os.obj->SuperClassID()==HELPER_CLASS_ID && os.obj->ClassID()==UVWFRAME_CLASSID ) {
				SetCursor(mm->ip->GetSysCursor(SYSCUR_SELECT));
				return TRUE;
				}

		}
	return FALSE;
	}

BOOL PickNodeMode::HitTest(IObjParam *ip, HWND hWnd, ViewExp *vpt,
	IPoint2 m, int flags) {

	INode *node = mm->ip->PickNode(hWnd, m);
	if (node) {
		ObjectState os = node->EvalWorldState(0);
		if ( mm->uvwProy[mm->current_channel]->GetMappingType() == MAP_TL_SPLINE )
			if (os.obj->SuperClassID() == SHAPE_CLASS_ID) {
				SetCursor(mm->ip->GetSysCursor(SYSCUR_SELECT));
				return TRUE;
				}
		if ( mm->uvwProy[mm->current_channel]->GetMappingType() == MAP_TL_FFM )
			if (os.obj->IsParamSurface()) {
				SetCursor(mm->ip->GetSysCursor(SYSCUR_SELECT));
				return TRUE;
				}
		if ( mm->uvwProy[mm->current_channel]->GetMappingType() == MAP_TL_FRAME )
			if ( os.obj->SuperClassID()==HELPER_CLASS_ID && os.obj->ClassID()==UVWFRAME_CLASSID ) {
				SetCursor(mm->ip->GetSysCursor(SYSCUR_SELECT));
				return TRUE;
				}
		}
	return FALSE;
	}

void PickNodeMode::EnterMode(IObjParam *ip) {
	TimeValue t = ip->GetTime();

	// Get This Node tm
	ModContextList mcList;
	INodeTab nodes;
	ip->GetModContexts(mcList,nodes);
	mm->this_node_tm = Inverse(nodes[0]->GetObjectTM(t)) * *mcList[0]->tm;

	if ( mm->uvwProy[mm->current_channel]->GetMappingType() == MAP_TL_SPLINE )	
		mm->pick_spline_button->SetCheck(TRUE); 
	else if ( mm->uvwProy[mm->current_channel]->GetMappingType() == MAP_TL_FRAME )
		mm->pick_frame_button->SetCheck(TRUE); 
	else		
		mm->pick_surface_button->SetCheck(TRUE);   
	}

void PickNodeMode::ExitMode(IObjParam *ip) {	
	if (mm->uvwProy[mm->current_channel]->GetMappingType() == MAP_TL_SPLINE)	
		mm->pick_spline_button->SetCheck(FALSE);
	else if ( mm->uvwProy[mm->current_channel]->GetMappingType() == MAP_TL_FRAME )
		mm->pick_frame_button->SetCheck(FALSE);
	else
		mm->pick_surface_button->SetCheck(FALSE);  
	}

// TLTODO: return TRUE????
BOOL PickNodeMode::Pick(IObjParam *ip, ViewExp *vpt) {
	INode *node = vpt->GetClosestHit();
	if (node) {
		ObjectState os = node->EvalWorldState(0);
		if (mm->uvwProy[mm->current_channel]->GetMappingType() == MAP_TL_SPLINE)
			if (os.obj->SuperClassID() == SHAPE_CLASS_ID) {

				theHold.Begin();
				if (mm->AddSpline(node)) {
					mm->ip->SetStdCommandMode(CID_OBJMOVE);
					mm->ip->RedrawViews(mm->ip->GetTime());	
					theHold.Accept(_T("Pick Spline"));
					} 
				else {
					theHold.Cancel();
					TSTR buf = GetString(IDS_ILLEGALSPLINE);
					TSTR buft = GetString(IDS_SPLINEMAPPING);
					MessageBox(ip->GetMAXHWnd(), buf,
						buft, MB_OK|MB_ICONEXCLAMATION);
					}
				}
		if (mm->uvwProy[mm->current_channel]->GetMappingType() == MAP_TL_FFM)
			if (os.obj->IsParamSurface()) {
			//	theHold.Begin();
				mm->ShapeFFGPoints(node);
			//	theHold.Accept(_T("Shape to Surface"));
				mm->ip->SetStdCommandMode(CID_OBJMOVE);
				mm->ip->RedrawViews(mm->ip->GetTime());	
				}

		if ( mm->uvwProy[mm->current_channel]->GetMappingType() == MAP_TL_FRAME )
			if ( os.obj->SuperClassID()==HELPER_CLASS_ID && os.obj->ClassID()==UVWFRAME_CLASSID ) {
				theHold.Begin();
				if ( mm->AddFrame(node) ) {
					mm->ip->SetStdCommandMode(CID_OBJMOVE);
					mm->ip->RedrawViews(mm->ip->GetTime());	
					theHold.Accept(_T("Pick UVW Frame"));
					}
				else {
					theHold.Cancel();
					TSTR buf = GetString(IDS_ILLEGALSPLINE);
					TSTR buft = GetString(IDS_TLFR_FRAME);
					MessageBox(ip->GetMAXHWnd(), buf, buft, MB_OK|MB_ICONEXCLAMATION);
					}
				}

		}
	return TRUE;
	}

static HIMAGELIST hAboutImage = NULL;
static HIMAGELIST hmmEditList = NULL;
static HIMAGELIST hTypeImages = NULL;
static HIMAGELIST hBulbIcons = NULL;
static HIMAGELIST hMapTypesIcons = NULL;
static void LoadImages()
	{
	if (!hAboutImage) {
		HBITMAP hBitmap, hMask;
		hAboutImage = ImageList_Create(16, 16, ILC_COLOR8|ILC_MASK, 3, 0);
		hBitmap     = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_16BUTS));
		hMask       = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_16BUTS_MASK));
		ImageList_Add(hAboutImage,hBitmap,hMask);
		DeleteObject(hBitmap);
		DeleteObject(hMask);
		}
	if (!hmmEditList) {
		HBITMAP hBitmap, hMask;
		hmmEditList = ImageList_Create(14, 14, ILC_COLOR8|ILC_MASK, 3, 0);
		hBitmap     = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_TL_TOOLBAR));
		hMask       = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_TL_TOOLBAR_MASK));
		ImageList_Add(hmmEditList,hBitmap,hMask);
		DeleteObject(hBitmap);
		DeleteObject(hMask);
		}
	if (!hBulbIcons) {
		HBITMAP hBitmap, hMask;
		hBulbIcons = ImageList_Create(10, 13, TRUE, 2, 0);
		hBitmap = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_BULBS));
		hMask   = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_BULBS_MASK));
		ImageList_Add(hBulbIcons,hBitmap,hMask);
		DeleteObject(hBitmap);
		DeleteObject(hMask);
		}
	if (!hMapTypesIcons) {
		HBITMAP hBitmap, hMask;
		hMapTypesIcons = ImageList_Create(11, 13, TRUE, 5, 0);
		hBitmap = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_MAP_TYPES));
		hMask   = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_MAP_TYPES_MASK));
		ImageList_Add(hMapTypesIcons,hBitmap,hMask);
		DeleteObject(hBitmap);
		DeleteObject(hMask);
		}
	HBITMAP temp;
	hTypeImages = ImageList_Create(50, 34, ILC_COLOR8|ILC_MASK, 11, 0);
		temp = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_TYPES));
		ImageList_AddMasked(hTypeImages, temp, RGB(200,5,205));
		DeleteObject( temp );
	}	


//Win32 : BOOL CALLBACK TexLayMainDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) {
INT_PTR CALLBACK TexLayMainDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) {
	static bool spinner_holding = false;

	//Win32 : MultiMapMod *tl = (MultiMapMod*)GetWindowLong(hWnd,GWL_USERDATA);
	MultiMapMod *tl = DLGetWindowLongPtr<MultiMapMod*>(hWnd);


	int id = LOWORD(wParam);
	switch (message) 
	{
		case USER_LB_DRAGDROP:
			tl->RepositionGroup((int)wParam,(int)lParam);
			break;

		case USER_LB_SETSEL: {
			int index = SendDlgItemMessage(hWnd, IDC_TL_GROUPS_MENU, LB_GETCURSEL, 0, 0);
			if (index!=LB_ERR) {
				if ( index != tl->current_channel ) 
					tl->CloseCurveDlgs();

				if (theHold.Holding()) {
					theHold.Put(new ChannelRestore(tl));
					}

				tl->ChannelChange(index);
				InvalidateRect(GetDlgItem(hWnd,IDC_TL_GROUPS_MENU),NULL,TRUE);
				tl->LocalDataChanged();
				tl->UpdateUIAll();
				}							 
			}
			break;

		case USER_LB_ACTIVATE: {
			int index = (int)lParam;
			tl->SetActive(index);
			}
			break;

		case WM_INITDIALOG:
			tl->hwnd_main = hWnd;
			tl = (MultiMapMod*)lParam;
			
			//Win32 : SetWindowLong(hWnd,GWL_USERDATA,lParam);
			DLSetWindowLongPtr( hWnd, lParam ); 

			tl->InitMainRollup();
			SetWindowText(GetDlgItem(hWnd,IDC_TKU_ABOUT),TEXLAY_LABEL);
			break;

		case WM_DESTROY:
			break;

		case WM_NOTIFY: {
			// This is where we provide the tooltip text for the toolbar buttons...
			LPNMHDR hdr = (LPNMHDR)lParam;
			if (hdr->code == TTN_NEEDTEXT) {
				LPTOOLTIPTEXT lpttt;
				lpttt = (LPTOOLTIPTEXT)hdr;				
				switch (lpttt->hdr.idFrom) {
					case ID_ADD:
						lpttt->lpszText = GetString(IDS_ADD_GROUP);
						break;
					case ID_DEL:
						lpttt->lpszText = GetString(IDS_DEL_GROUP);
						break;
					case ID_SAVE:
						lpttt->lpszText = GetString(IDS_SAVE_ALL);
						break;
					case ID_LOAD:
						lpttt->lpszText = GetString(IDS_LOAD_ALL);
						break;
					case ID_CUT:
						lpttt->lpszText = GetString(IDS_CUT);
						break;
					case ID_COPY:
						lpttt->lpszText = GetString(IDS_COPY);
						break;
					case ID_PASTE:
						lpttt->lpszText = GetString(IDS_PASTE);
						break;
					case ID_TOOLS:
						lpttt->lpszText = GetString(IDS_TOOLS);
						break;
					};
				};
			break;
			}

		case WM_COMMAND:
			switch (LOWORD(wParam)) {

				case IDC_TL_ABOUT:
					DisplayTexLayAbout();
					break;

				case IDC_TLM_TYPES:
					if ( HIWORD(wParam) == CBN_SELCHANGE ) {
						int oldtipo = tl->uvwProy[tl->current_channel]->GetMappingType();
						int tipo = SendDlgItemMessage(hWnd,IDC_TLM_TYPES,CB_GETCURSEL,0,0);

						if ( tipo == MAP_TL_FFM && oldtipo != MAP_TL_FFM )
							int res = tl->GMSetNumPoints(hWnd,FALSE);

						THE_HOLD_TLP_BEGIN
						tl->uvwProy[tl->current_channel]->SetMappingType( tipo );
						THE_HOLD_TLP_ACCEPT
						tl->GroupsMenuForceDrawItem(tl->current_channel);
						tl->UpdateUIAll();
						tl->LocalDataChanged();
						}
					break;

				case ID_ADD:
					tl->AddNewGroup();
					break;

				case ID_DEL:
					tl->DeleteCurrentGroup();
					break;

				case ID_SAVE:
					tl->SaveTLDlg();
					break;

				case ID_LOAD:
					tl->LoadTL(0);
					break;

				case ID_COPY:
					tl->CopyLayerToBuffer();
					break;
					
				case ID_PASTE:
					if (tl->PasteBufferToLayer()) {
						tl->LocalDataChanged();
						}
					break;

				case ID_TOOLS:
					tl->DisplayOptions();
					break;
				}
			break;

		case WM_DRAWITEM: 
			LPDRAWITEMSTRUCT lpdis;
			switch (LOWORD(wParam)) {
				case IDC_TL_GROUPS_MENU: {
					lpdis = (LPDRAWITEMSTRUCT) lParam; 
					tl->GroupsMenuDrawItem(lpdis);
					}
					break;
				case IDC_TLM_TYPES: {
					lpdis = (LPDRAWITEMSTRUCT) lParam; 
					tl->DrawMappingTypeItem(lpdis);
					}
					break;
				}
			break;

		case CC_SPINNER_BUTTONDOWN:
			THE_HOLD_TLP_BEGIN
			spinner_holding = true;
			break;

		case CC_SPINNER_CHANGE: {
			if ( !spinner_holding ) {
				THE_HOLD_TLP_BEGIN
				}
			switch (LOWORD(wParam)) {
				case IDC_TL_MAPCH_SPIN:
					tl->UpdatePBlockParam(uvw_map_channel);
					tl->GroupsMenuForceDrawItem(tl->current_channel);
					break;
				case IDC_TL_LENGTH_SPIN:
					tl->UpdatePBlockParam(uvw_length);
					break;
				case IDC_TL_WIDTH_SPIN:
					tl->UpdatePBlockParam(uvw_width);
					break;
				case IDC_TL_HEIGHT_SPIN:
					tl->UpdatePBlockParam(uvw_height);
					break;
				}
			if ( !spinner_holding ) {
				THE_HOLD_TLP_ACCEPT
				}
			spinner_holding = false;
			}
			break;

		case CC_SPINNER_BUTTONUP:
			if ( HIWORD(wParam) ) {
				THE_HOLD_TLP_ACCEPT
				}
			else {
				THE_HOLD_TLP_CANCEL
				}
			spinner_holding = false;
			break;

		case WM_CUSTEDIT_ENTER:
			switch (LOWORD(wParam)) {
				case IDC_TL_GROUP_NAME: { 
					TCHAR buf[256];
					tl->group_name_edit->GetText( buf, 255 );
					tl->RenameChannel(buf);
					break;
					}
				default:
					break;
				}
			break;

	}
	return FALSE;
}

//Win32 : BOOL CALLBACK TexLayTilingDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
INT_PTR CALLBACK TexLayTilingDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{

	//Win32 : MultiMapMod *tl = (MultiMapMod*)GetWindowLong(hWnd,GWL_USERDATA);
	MultiMapMod *tl = DLGetWindowLongPtr<MultiMapMod*>(hWnd);

	int id = LOWORD(wParam);
	switch (message) {
		case WM_INITDIALOG:
			tl->hwnd_tiling = hWnd;
			tl = (MultiMapMod*)lParam;
			
			//Win32 : SetWindowLong(hWnd,GWL_USERDATA,lParam);
			DLSetWindowLongPtr(hWnd, lParam);
			
			tl->InitTilingRollup();
			break;
		case WM_DESTROY:
			break;
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDC_TL_ATTON:
					THE_HOLD_TLP_BEGIN
					tl->UpdatePBlockParam(uvw_atton);
					THE_HOLD_TLP_ACCEPT
					break;
				case IDC_TL_XF_TO_ATT:
					THE_HOLD_TLP_BEGIN
					tl->UpdatePBlockParam(uvw_xform_att);
					THE_HOLD_TLP_ACCEPT
					break;
				}
			break;

		case CC_SPINNER_BUTTONDOWN:
			THE_HOLD_TLP_BEGIN
			break;

		case CC_SPINNER_CHANGE: {
			switch (LOWORD(wParam)) {
				case IDC_TL_US_SPIN:
					tl->UpdatePBlockParam(uvw_tile_u);
					break;
				case IDC_TL_UO_SPIN:
					tl->UpdatePBlockParam(uvw_offset_u);
					break;
				case IDC_TL_VS_SPIN:
					tl->UpdatePBlockParam(uvw_tile_v);
					break;
				case IDC_TL_VO_SPIN:
					tl->UpdatePBlockParam(uvw_offset_v);
					break;
				case IDC_TL_WS_SPIN:
					tl->UpdatePBlockParam(uvw_tile_w);
					break;
				case IDC_TL_WO_SPIN:
					tl->UpdatePBlockParam(uvw_offset_w);
					break;
				case IDC_TL_ATT_SPIN:
					tl->UpdatePBlockParam(uvw_att);
					break;
				case IDC_TL_AUS_SPIN:
					tl->UpdatePBlockParam(uvw_aus);
					break;
				case IDC_TL_AUO_SPIN:
					tl->UpdatePBlockParam(uvw_aue);
					break;
				case IDC_TL_AVS_SPIN:
					tl->UpdatePBlockParam(uvw_avs);
					break;
				case IDC_TL_AVO_SPIN:
					tl->UpdatePBlockParam(uvw_ave);
					break;
				case IDC_TL_AWS_SPIN:
					tl->UpdatePBlockParam(uvw_aws);
					break;
				case IDC_TL_AWO_SPIN:
					tl->UpdatePBlockParam(uvw_awe);
					break;
				case IDC_TL_RUV_SPIN:
					tl->UpdatePBlockParam(uvw_aruv);
					break;
				case IDC_TL_MOVE_U_SPIN:
					tl->UpdatePBlockParam(uvw_move_u);
					break;
				case IDC_TL_MOVE_V_SPIN:
					tl->UpdatePBlockParam(uvw_move_v);
					break;
				case IDC_TL_MOVE_W_SPIN:
					tl->UpdatePBlockParam(uvw_move_w);
					break;
				case IDC_TL_ROT_U_SPIN:
					tl->UpdatePBlockParam(uvw_rotate_u);
					break;
				case IDC_TL_ROT_V_SPIN:
					tl->UpdatePBlockParam(uvw_rotate_v);
					break;
				case IDC_TL_ROT_W_SPIN:
					tl->UpdatePBlockParam(uvw_rotate_w);
					break;
				case IDC_TL_SCALE_U_SPIN:
					tl->UpdatePBlockParam(uvw_scale_u);
					break;
				case IDC_TL_SCALE_V_SPIN:
					tl->UpdatePBlockParam(uvw_scale_v);
					break;
				case IDC_TL_SCALE_W_SPIN:
					tl->UpdatePBlockParam(uvw_scale_w);
					break;
				}
			}
			break;

		case CC_SPINNER_BUTTONUP:
			if ( HIWORD(wParam) ) {
				THE_HOLD_TLP_ACCEPT
				}
			else {
				THE_HOLD_TLP_CANCEL
				}
			break;

		case WM_CUSTEDIT_ENTER:
			THE_HOLD_TLP_BEGIN
			THE_HOLD_TLP_ACCEPT
			break;


		}
	return FALSE;
}

//Win32 : BOOL CALLBACK TexLayToolsDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
INT_PTR CALLBACK TexLayToolsDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) 
{

	//Win32 : MultiMapMod *tl = (MultiMapMod*)GetWindowLong(hWnd,GWL_USERDATA);
	MultiMapMod *tl = DLGetWindowLongPtr<MultiMapMod*>(hWnd);

	int id = LOWORD(wParam);
	switch (message) {
		case WM_INITDIALOG:
			tl->hwnd_tools = hWnd;
			tl = (MultiMapMod*)lParam;
			
			//Win32 : SetWindowLong(hWnd,GWL_USERDATA,lParam);
			DLSetWindowLongPtr(hWnd, lParam);

			tl->InitToolsRollup();
			break;
		case WM_DESTROY:
			break;
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDC_TLO_X:
				case IDC_TLO_Y:
				case IDC_TLO_Z:
					THE_HOLD_TLP_BEGIN
					tl->UpdatePBlockParam(uvw_axis);
					THE_HOLD_TLP_ACCEPT
					break;

				case IDC_MAP_FIT:
					tl->uvwProy[tl->current_channel]->valid_group = 0;
					tl->uvwProy[tl->current_channel]->flags |= CONTROL_FIT|CONTROL_HOLD;
					tl->NotifyDependents(FOREVER,PART_TEXMAP,REFMSG_CHANGE);
					tl->LocalDataChanged();
					break;

				case IDC_MAP_CENTER:
					theHold.Begin();
					theHold.Put(new GroupsTableRestore(tl,INVALID_GROUP));
					tl->uvwProy[tl->current_channel]->valid_group = 0;
					tl->uvwProy[tl->current_channel]->flags |= CONTROL_CENTER|CONTROL_HOLD;
					tl->NotifyDependents(FOREVER,PART_TEXMAP,REFMSG_CHANGE);
					tl->LocalDataChanged();
					theHold.Put(new GroupsTableRestore(tl,INVALID_GROUP));
					theHold.Accept(_T("Center"));
					break;

				case IDC_MAP_BITMAPFIT:
					tl->uvwProy[tl->current_channel]->valid_group = 0;
					tl->DoBitmapFit();
					tl->LocalDataChanged();
					break;
				
				case IDC_MAP_NORMALALIGN: {
					ICustButton * button = GetICustButton(GetDlgItem(hWnd, IDC_MAP_NORMALALIGN));
					if (button->IsChecked())
						tl->ip->SetCommandMode(tl->faceAlignMode);
					else
						tl->ip->SetStdCommandMode(CID_OBJMOVE);
					tl->LocalDataChanged();
					ReleaseICustButton(button);
					}
					break;

				case IDC_MAP_VIEWALIGN:
					tl->uvwProy[tl->current_channel]->valid_group = 0;
					tl->ViewportAlign();
					tl->LocalDataChanged();
					break;

				case IDC_MAP_FITREGION:
					tl->uvwProy[tl->current_channel]->valid_group = 0;
					if (tl->ip->GetCommandMode()->ID()==CID_REGIONFIT) {
						tl->ip->SetStdCommandMode(CID_OBJMOVE);
					} else {
						tl->ip->SetCommandMode(tl->regionFitMode);
						}
					tl->LocalDataChanged();
					break;

				case IDC_MAP_RESET:
					tl->uvwProy[tl->current_channel]->valid_group = 0;
					theHold.Begin();
					theHold.Put(new GroupsTableRestore(tl,INVALID_GROUP));
					tl->uvwProy[tl->current_channel]->ReplaceReference(1,NULL);
					tl->flags |= CONTROL_FIT|CONTROL_CENTER|CONTROL_INIT;
					theHold.Put(new GroupsTableRestore(tl,INVALID_GROUP));
					theHold.Accept(_T("Reset"));
					tl->NotifyDependents(FOREVER,PART_TEXMAP,REFMSG_CHANGE);
					tl->LocalDataChanged();
					break;

				case IDC_MAP_ACQUIRE:
					tl->uvwProy[tl->current_channel]->valid_group = 0;
					tl->ip->SetPickMode(&tl->pickAcquire);
					tl->LocalDataChanged();
					break;

				case IDC_MAP_ALIGN_SEL:
					tl->uvwProy[tl->current_channel]->valid_group = 0;
					theHold.Begin();
					theHold.Put(new GroupsTableRestore(tl,INVALID_GROUP));
					tl->AlignToSelection();
					theHold.Put(new GroupsTableRestore(tl,INVALID_GROUP));
					theHold.Accept(_T("Align to Selection"));
					tl->LocalDataChanged();
					break;

				}
			break;
		}
	return FALSE;
}

//Win32 : BOOL CALLBACK TexLaySplineDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
INT_PTR CALLBACK TexLaySplineDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	//Win32 : MultiMapMod *tl = (MultiMapMod*)GetWindowLong(hWnd,GWL_USERDATA);
	MultiMapMod *tl = DLGetWindowLongPtr<MultiMapMod*>(hWnd);

	int id = LOWORD(wParam);
	switch (message) {
		case WM_INITDIALOG:
			tl->hwnd_spline = hWnd;
			tl = (MultiMapMod*)lParam;
			
			//Win32 : SetWindowLong(hWnd,GWL_USERDATA,lParam);
			DLSetWindowLongPtr(hWnd, lParam);

			tl->InitSplineRollup();
			break;
		case WM_DESTROY:
			break;
		case WM_COMMAND:
			switch (LOWORD(wParam)) {

				case IDC_TLS_PICK_NODE:
					tl->ip->SetPickMode(tl->pickMode);
					break;

				case IDC_TLS_REVERSE:
					THE_HOLD_TLP_BEGIN
					tl->UpdatePBlockParam(uvw_reverse);
					THE_HOLD_TLP_ACCEPT
					break;

				case IDC_TLS_NORMALIZED:
					THE_HOLD_TLP_BEGIN
					tl->UpdatePBlockParam(uvw_normlize);
					THE_HOLD_TLP_ACCEPT
					break;

				case IDC_TLS_BELT:
					THE_HOLD_TLP_BEGIN
					tl->UpdatePBlockParam(uvw_spline_belt);
					THE_HOLD_TLP_ACCEPT
					break;

				case IDC_TLS_PLANE:
				case IDC_TLS_CENTER:
				case IDC_TLS_FIELD:
					THE_HOLD_TLP_BEGIN
					tl->UpdatePBlockParam(uvw_normals);
					THE_HOLD_TLP_ACCEPT
					break;

				case IDC_TLS_RESET_SPLINE:
					theHold.Begin();
					tl->ResetNode();
					theHold.Accept(0);
					break;

				case IDC_TLS_UTILE:
					tl->SwitchTileDlgCurve();
					break;

				case IDC_TLS_RADIUS:
					tl->SwitchRadiusDlgCurve();
					break;

				case IDC_TLS_ANGLE:
					tl->SwitchAngleDlgCurve();
					break;
				}
			break;

		case CC_SPINNER_BUTTONDOWN:
			THE_HOLD_TLP_BEGIN
			break;

		case CC_SPINNER_CHANGE:
			switch (LOWORD(wParam)) {
				case IDC_TLS_NORMAL_START_SPIN:
					tl->UpdatePBlockParam(uvw_normals_start);
					break;
				}
			break;

		case CC_SPINNER_BUTTONUP:
			if ( HIWORD(wParam) ) {
				THE_HOLD_TLP_ACCEPT
				}
			else {
				THE_HOLD_TLP_CANCEL
				}
			break;

		case WM_CUSTEDIT_ENTER:
			THE_HOLD_TLP_BEGIN
			THE_HOLD_TLP_ACCEPT
			break;

		}
	return FALSE;
	}

//Win32 : BOOL CALLBACK TexLayFreeDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
INT_PTR CALLBACK TexLayFreeDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	//Win32 : MultiMapMod *tl = (MultiMapMod*)GetWindowLong(hWnd,GWL_USERDATA);
	MultiMapMod *tl = DLGetWindowLongPtr<MultiMapMod*>(hWnd);

	int id = LOWORD(wParam);
	switch (message) {
		case WM_INITDIALOG:
			tl->hwnd_free = hWnd;
			tl = (MultiMapMod*)lParam;
			
			//Win32 : SetWindowLong(hWnd,GWL_USERDATA,lParam);
			DLSetWindowLongPtr(hWnd, lParam);
			
			tl->InitFreeRollup();
			break;
		case WM_DESTROY:
			break;
		case WM_COMMAND:
			switch (LOWORD(wParam)) {

				case IDC_TLF_PICK_SURFACE:
					tl->ip->SetPickMode(tl->pickMode);
					break;

				case IDC_TLF_SET_NUM_POINTS:
					tl->GMSetNumPoints(hWnd);
					tl->TextNumPoints();
					break;

				case 0:
					break;
				}
			break;
		case CC_SPINNER_BUTTONDOWN:
			THE_HOLD_TLP_BEGIN
			break;

/*		case CC_SPINNER_CHANGE:
			switch (LOWORD(wParam)) {
				case IDC_TLF_THRESH_SPIN:
					tl->UpdatePBlockParam(uvw_ffm_thresh);
					break;
				}
			break;*/

		case CC_SPINNER_BUTTONUP:
			if ( HIWORD(wParam) ) {
				THE_HOLD_TLP_ACCEPT
				}
			else {
				THE_HOLD_TLP_CANCEL
				}
			break;

		case WM_CUSTEDIT_ENTER:
			THE_HOLD_TLP_BEGIN
			THE_HOLD_TLP_ACCEPT
			break;

		}
	return FALSE;
	}

//Win32 : BOOL CALLBACK TexLayPeltDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) 
INT_PTR CALLBACK TexLayPeltDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) 
{
	//Win32 : MultiMapMod *tl = (MultiMapMod*)GetWindowLong(hWnd,GWL_USERDATA);
	MultiMapMod *tl = DLGetWindowLongPtr<MultiMapMod*>(hWnd);

	int id = LOWORD(wParam);
	switch (message) {
		case WM_INITDIALOG:
			tl->hwnd_pelt = hWnd;
			tl = (MultiMapMod*)lParam;
			
			//Win32 : SetWindowLong(hWnd,GWL_USERDATA,lParam);
			DLSetWindowLongPtr(hWnd, lParam);
			
			tl->InitPeltRollup();
			break;
		case WM_DESTROY:
			break;
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDC_TLP_EDIT_FRAME:
					tl->DoUVPeltEditing();
					break;
				case IDC_TLP_RESET_FRAME:
					tl->ResetUVPelt();
					break;
				case IDC_TLP_MOVE_BORDER:
					THE_HOLD_TLP_BEGIN
					tl->UpdatePBlockParam(uvw_pelt_border);
					THE_HOLD_TLP_ACCEPT
					break;
				case IDC_TLP_SAVE_FRAME:
					tl->PeltSaveLayout();
					break;
				case IDC_TLP_LOAD_FRAME:
					tl->PeltLoadLayout();
					break;
				}
			break;
		case CC_SPINNER_BUTTONDOWN:
			THE_HOLD_TLP_BEGIN
			break;

		case CC_SPINNER_CHANGE:
			switch (LOWORD(wParam)) {
				case IDC_TLP_K_SPIN:
					tl->UpdatePBlockParam(uvw_pelt_rigidity);
					break;
				case IDC_TLP_B_SPIN:
					tl->UpdatePBlockParam(uvw_pelt_stability);
					break;
				case IDC_TLP_FRAME_SPIN:
					tl->UpdatePBlockParam(uvw_pelt_frame);
					break;
				case IDC_TLP_ITER_SPIN:
					tl->UpdatePBlockParam(uvw_pelt_iter);
					break;
				}
			break;

		case CC_SPINNER_BUTTONUP:
			if ( HIWORD(wParam) ) {
				THE_HOLD_TLP_ACCEPT
				}
			else {
				THE_HOLD_TLP_CANCEL
				}
			break;

		}
	return FALSE;
	}

//Win32 : BOOL CALLBACK TexLayFrameDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) 
INT_PTR CALLBACK TexLayFrameDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) 
{
	//Win32 : MultiMapMod *tl = (MultiMapMod*)GetWindowLong(hWnd,GWL_USERDATA);
	MultiMapMod *tl = DLGetWindowLongPtr<MultiMapMod*>(hWnd);

	int id = LOWORD(wParam);
	switch (message) {
		case WM_INITDIALOG:
			tl->hwnd_frame = hWnd;
			tl = (MultiMapMod*)lParam;
			
			//Win32 : SetWindowLong(hWnd,GWL_USERDATA,lParam);
			DLSetWindowLongPtr(hWnd, lParam);
			
			tl->InitFrameRollup();
			break;
		case WM_DESTROY:
			break;
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDC_TLFR_PICK_FRAME:
					tl->ip->SetPickMode(tl->pickMode);
					break;
				}
			break;
		case CC_SPINNER_CHANGE: {
			}
			break;
		}
	return FALSE;
	}

//Win32 : BOOL CALLBACK TexLayDataDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) 
INT_PTR CALLBACK TexLayDataDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) 
{

	//Win32 : MultiMapMod *tl = (MultiMapMod*)GetWindowLong(hWnd,GWL_USERDATA);
	MultiMapMod *tl = DLGetWindowLongPtr<MultiMapMod*>(hWnd);

	int id = LOWORD(wParam);
	switch (message) {
		case WM_INITDIALOG:
			tl->hwnd_data = hWnd;
			tl = (MultiMapMod*)lParam;
			
			//Win32 : SetWindowLong(hWnd,GWL_USERDATA,lParam);
			DLSetWindowLongPtr(hWnd, lParam);
			
			tl->InitDataRollup();
			break;
		case WM_DESTROY:
			break;
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDC_TLD_SAVE:
					tl->SaveGroupUVW(tl->current_channel);
					break;
				case IDC_TLD_LOAD:
					tl->LoadGroupUVW(tl->current_channel);
					break;
				}
			break;
		case CC_SPINNER_CHANGE: {
			}
			break;
		}
	return FALSE;
	}


void MultiMapMod::CreateUI() {

	ip->RegisterTimeChangeCallback(this);

	moveMode      = new MoveModBoxCMode(this,ip);
	rotMode       = new RotateModBoxCMode(this,ip);
	uscaleMode    = new UScaleModBoxCMode(this,ip);
	nuscaleMode   = new NUScaleModBoxCMode(this,ip);
	squashMode    = new SquashModBoxCMode(this,ip);
	selectMode    = new SelectModBoxCMode(this,ip);
	faceAlignMode = new FaceAlignMode(this,ip);
	regionFitMode = new RegionFitMode(this,ip);

	pickAcquire.mod = this;
	pickAcquire.ip  = ip;
	pickMode = new PickNodeMode(this);

	UpdateUIAll();
	}

void MultiMapMod::InitMainRollup() {
	LoadImages();

	ICustButton *iTmp;
	iTmp = GetICustButton(GetDlgItem(hwnd_main,IDC_TL_ABOUT));
	iTmp->SetImage(hAboutImage, 0, 0, 0, 0, 16, 16);
	iTmp->SetTooltip(TRUE,_T("About Texture Layers"));
	ReleaseICustButton(iTmp);

	SendDlgItemMessage(hwnd_main,IDC_TLM_TYPES,CB_RESETCONTENT,0,0);
	SendDlgItemMessage(hwnd_main,IDC_TLM_TYPES,CB_ADDSTRING,0,(LPARAM)GetString(IDS_MAPTYPE_PLANAR));
	SendDlgItemMessage(hwnd_main,IDC_TLM_TYPES,CB_ADDSTRING,0,(LPARAM)GetString(IDS_MAPTYPE_CYL));
	SendDlgItemMessage(hwnd_main,IDC_TLM_TYPES,CB_ADDSTRING,0,(LPARAM)GetString(IDS_MAPTYPE_CYLCAP));
	SendDlgItemMessage(hwnd_main,IDC_TLM_TYPES,CB_ADDSTRING,0,(LPARAM)GetString(IDS_MAPTYPE_SPHERE));
	SendDlgItemMessage(hwnd_main,IDC_TLM_TYPES,CB_ADDSTRING,0,(LPARAM)GetString(IDS_MAPTYPE_SHRINK));
	SendDlgItemMessage(hwnd_main,IDC_TLM_TYPES,CB_ADDSTRING,0,(LPARAM)GetString(IDS_MAPTYPE_BOX));
	SendDlgItemMessage(hwnd_main,IDC_TLM_TYPES,CB_ADDSTRING,0,(LPARAM)GetString(IDS_MAPTYPE_FACE));
	SendDlgItemMessage(hwnd_main,IDC_TLM_TYPES,CB_ADDSTRING,0,(LPARAM)GetString(IDS_MAPTYPE_XYZ));
	SendDlgItemMessage(hwnd_main,IDC_TLM_TYPES,CB_ADDSTRING,0,(LPARAM)GetString(IDS_MAPTYPE_SPLINE));
	SendDlgItemMessage(hwnd_main,IDC_TLM_TYPES,CB_ADDSTRING,0,(LPARAM)GetString(IDS_MAPTYPE_FFM));
	SendDlgItemMessage(hwnd_main,IDC_TLM_TYPES,CB_ADDSTRING,0,(LPARAM)GetString(IDS_MAPTYPE_PELT));
	SendDlgItemMessage(hwnd_main,IDC_TLM_TYPES,CB_ADDSTRING,0,(LPARAM)GetString(IDS_MAPTYPE_FRAME));
	SendDlgItemMessage(hwnd_main,IDC_TLM_TYPES,CB_ADDSTRING,0,(LPARAM)GetString(IDS_MAPTYPE_DATA));

	group_name_edit = GetICustEdit(GetDlgItem(hwnd_main,IDC_TL_GROUP_NAME));
	group_name_edit->SetLeading(2);

	HWND hListBox = GetDlgItem(hwnd_main,IDC_TL_GROUPS_MENU);
	groups_menu_handler = new CListBoxHandler;
	groups_menu_handler->SetTarget(hInstance, hListBox);

	iToolbar = GetICustToolbar(GetDlgItem(hwnd_main,IDC_MM_TOOLBAR));
	iToolbar->SetImage(hmmEditList);
	iToolbar->AddTool(ToolButtonItem(CTB_PUSHBUTTON,2,2,2,2,14,14,22,22,ID_ADD));
	iToolbar->AddTool(ToolButtonItem(CTB_PUSHBUTTON,3,3,3,3,14,14,22,22,ID_DEL));
	iToolbar->AddTool(ToolSeparatorItem(1));
	iToolbar->AddTool(ToolButtonItem(CTB_PUSHBUTTON,0,0,0,0,14,14,22,22,ID_SAVE));
	iToolbar->AddTool(ToolButtonItem(CTB_PUSHBUTTON,1,1,1,1,14,14,22,22,ID_LOAD));
	iToolbar->AddTool(ToolSeparatorItem(1));
	iToolbar->AddTool(ToolButtonItem(CTB_PUSHBUTTON,5,5,5,5,14,14,22,22,ID_COPY));
	iToolbar->AddTool(ToolButtonItem(CTB_PUSHBUTTON,6,6,6,6,14,14,22,22,ID_PASTE));
	iToolbar->AddTool(ToolSeparatorItem(1));
	iToolbar->AddTool(ToolButtonItem(CTB_PUSHBUTTON,7,7,7,7,14,14,22,22,ID_TOOLS));

	iAdd	= iToolbar->GetICustButton(ID_ADD);
	iDel	= iToolbar->GetICustButton(ID_DEL);
	iSave	= iToolbar->GetICustButton(ID_SAVE);
	iLoad	= iToolbar->GetICustButton(ID_LOAD);
	iCopy	= iToolbar->GetICustButton(ID_COPY);
	iPaste	= iToolbar->GetICustButton(ID_PASTE);
	iTools	= iToolbar->GetICustButton(ID_TOOLS);

	mapch_spin = SetupIntSpinner(hwnd_main, IDC_TL_MAPCH_SPIN, IDC_TL_MAPCH, 0, 100, 1);
	length_spin = SetupFloatSpinner(hwnd_main, IDC_TL_LENGTH_SPIN, IDC_TL_LENGTH, -999999999.0f, 999999999.0f, 0.0f, 0.1f);
	width_spin = SetupFloatSpinner(hwnd_main, IDC_TL_WIDTH_SPIN, IDC_TL_WIDTH, -999999999.0f, 999999999.0f, 0.0f, 0.1f);
	height_spin = SetupFloatSpinner(hwnd_main, IDC_TL_HEIGHT_SPIN, IDC_TL_HEIGHT, -999999999.0f, 999999999.0f, 0.0f, 0.1f);

	GroupsMenuUpdateList();
	}

void MultiMapMod::InitTilingRollup() {
	u_start_spin = SetupFloatSpinner(hwnd_tiling, IDC_TL_US_SPIN, IDC_TL_US, -999999999.0f, 999999999.0f, 0.0f, 0.01f);
	u_offset_spin = SetupFloatSpinner(hwnd_tiling, IDC_TL_UO_SPIN, IDC_TL_UO, -999999999.0f, 999999999.0f, 0.0f, 0.01f);
	v_start_spin = SetupFloatSpinner(hwnd_tiling, IDC_TL_VS_SPIN, IDC_TL_VS, -999999999.0f, 999999999.0f, 0.0f, 0.01f);
	v_offset_spin = SetupFloatSpinner(hwnd_tiling, IDC_TL_VO_SPIN, IDC_TL_VO, -999999999.0f, 999999999.0f, 0.0f, 0.01f);
	w_start_spin = SetupFloatSpinner(hwnd_tiling, IDC_TL_WS_SPIN, IDC_TL_WS, -999999999.0f, 999999999.0f, 0.0f, 0.01f);
	w_offset_spin = SetupFloatSpinner(hwnd_tiling, IDC_TL_WO_SPIN, IDC_TL_WO, -999999999.0f, 999999999.0f, 0.0f, 0.01f);
	att_all_spin = SetupFloatSpinner(hwnd_tiling, IDC_TL_ATT_SPIN, IDC_TL_ATT, 0.0f, 100.0f, 0.0f, 0.01f);
	att_u_start_spin = SetupFloatSpinner(hwnd_tiling, IDC_TL_AUS_SPIN, IDC_TL_AUS, 0.0f, 999999999.0f, 0.0f, 0.01f);
	att_u_offset_spin = SetupFloatSpinner(hwnd_tiling, IDC_TL_AUO_SPIN, IDC_TL_AUO, 0.0f, 999999999.0f, 0.0f, 0.01f);
	att_v_start_spin = SetupFloatSpinner(hwnd_tiling, IDC_TL_AVS_SPIN, IDC_TL_AVS, 0.0f, 999999999.0f, 0.0f, 0.01f);
	att_v_offset_spin = SetupFloatSpinner(hwnd_tiling, IDC_TL_AVO_SPIN, IDC_TL_AVO, 0.0f, 999999999.0f, 0.0f, 0.01f);
	att_w_start_spin = SetupFloatSpinner(hwnd_tiling, IDC_TL_AWS_SPIN, IDC_TL_AWS, 0.0f, 999999999.0f, 0.0f, 0.01f);
	att_w_offset_spin = SetupFloatSpinner(hwnd_tiling, IDC_TL_AWO_SPIN, IDC_TL_AWO, 0.0f, 999999999.0f, 0.0f, 0.01f);
	att_ruv_spin = SetupFloatSpinner(hwnd_tiling, IDC_TL_RUV_SPIN, IDC_TL_RUV, 0.0f, 100.0f, 0.0f, 0.01f);
	u_move_spin = SetupFloatSpinner(hwnd_tiling, IDC_TL_MOVE_U_SPIN, IDC_TL_MOVE_U, -999999999.0f, 999999999.0f, 0.0f, 0.01f);
	v_move_spin = SetupFloatSpinner(hwnd_tiling, IDC_TL_MOVE_V_SPIN, IDC_TL_MOVE_V, -999999999.0f, 999999999.0f, 0.0f, 0.01f);
	w_move_spin = SetupFloatSpinner(hwnd_tiling, IDC_TL_MOVE_W_SPIN, IDC_TL_MOVE_W, -999999999.0f, 999999999.0f, 0.0f, 0.01f);
	u_scale_spin = SetupFloatSpinner(hwnd_tiling, IDC_TL_SCALE_U_SPIN, IDC_TL_SCALE_U, -999999999.0f, 999999999.0f, 0.0f, 0.01f);
	v_scale_spin = SetupFloatSpinner(hwnd_tiling, IDC_TL_SCALE_V_SPIN, IDC_TL_SCALE_V, -999999999.0f, 999999999.0f, 0.0f, 0.01f);
	w_scale_spin = SetupFloatSpinner(hwnd_tiling, IDC_TL_SCALE_W_SPIN, IDC_TL_SCALE_W, -999999999.0f, 999999999.0f, 0.0f, 0.01f);
	u_rotate_spin = SetupFloatSpinner(hwnd_tiling, IDC_TL_ROT_U_SPIN, IDC_TL_ROT_U, -999999999.0f, 999999999.0f, 0.0f, 1.0f);
	v_rotate_spin = SetupFloatSpinner(hwnd_tiling, IDC_TL_ROT_V_SPIN, IDC_TL_ROT_V, -999999999.0f, 999999999.0f, 0.0f, 1.0f);
	w_rotate_spin = SetupFloatSpinner(hwnd_tiling, IDC_TL_ROT_W_SPIN, IDC_TL_ROT_W, -999999999.0f, 999999999.0f, 0.0f, 1.0f);
	}

void MultiMapMod::InitToolsRollup() {
	SendMessage(GetDlgItem(hwnd_tools,IDC_MAP_FITREGION),CC_COMMAND,CC_CMD_SET_TYPE,CBT_CHECK);
	SendMessage(GetDlgItem(hwnd_tools,IDC_MAP_FITREGION),CC_COMMAND,CC_CMD_HILITE_COLOR,GREEN_WASH);
	SendMessage(GetDlgItem(hwnd_tools,IDC_MAP_NORMALALIGN),CC_COMMAND,CC_CMD_SET_TYPE,CBT_CHECK);
	SendMessage(GetDlgItem(hwnd_tools,IDC_MAP_NORMALALIGN),CC_COMMAND,CC_CMD_HILITE_COLOR,GREEN_WASH);
	SendMessage(GetDlgItem(hwnd_tools,IDC_MAP_ACQUIRE),CC_COMMAND,CC_CMD_SET_TYPE,CBT_CHECK);
	SendMessage(GetDlgItem(hwnd_tools,IDC_MAP_ACQUIRE),CC_COMMAND,CC_CMD_HILITE_COLOR,GREEN_WASH);
	}

void MultiMapMod::InitSplineRollup() {
	pick_spline_button = GetICustButton(GetDlgItem(hwnd_spline, IDC_TLS_PICK_NODE));
	pick_spline_button->SetText(GetString(IDS_MTL_PICKSPLINE));
	pick_spline_button->SetType(CBT_CHECK);
	pick_spline_button->SetHighlightColor(GREEN_WASH);
	pick_spline_button->SetCheckHighlight(TRUE);
	normals_start_spin = SetupFloatSpinner(hwnd_spline, IDC_TLS_NORMAL_START_SPIN, IDC_TLS_NORMAL_START, -1.0f, 1.0f, 0.0f, 0.01f);
	}

void MultiMapMod::InitFreeRollup() {
	pick_surface_button = GetICustButton(GetDlgItem(hwnd_free, IDC_TLF_PICK_SURFACE));
	pick_surface_button->SetText(GetString(IDS_TLF_PICK_SURFACE));
	pick_surface_button->SetType(CBT_CHECK);
	pick_surface_button->SetHighlightColor(GREEN_WASH);
	pick_surface_button->SetCheckHighlight(TRUE);

//	free_threshold_spin = SetupFloatSpinner(hwnd_free, IDC_TLF_THRESH_SPIN, IDC_TLF_THRESH, 0.0f, 999999999.0f, 0.0f, 0.1f);
	}

void MultiMapMod::InitPeltRollup() {
	pelt_rigidity_spin = SetupFloatSpinner(hwnd_pelt, IDC_TLP_K_SPIN, IDC_TLP_K, 0.0f, 1000.0f, 0.0f, 0.01f);
	pelt_stability_spin = SetupFloatSpinner(hwnd_pelt, IDC_TLP_B_SPIN, IDC_TLP_B, 0.0f, 1000.0f, 0.0f, 0.01f);
	pelt_frame_spin = SetupFloatSpinner(hwnd_pelt, IDC_TLP_FRAME_SPIN, IDC_TLP_FRAME, 0.0f, 1000.0f, 0.0f, 0.01f);
	pelt_iter_spin = SetupIntSpinner(hwnd_pelt, IDC_TLP_ITER_SPIN, IDC_TLP_ITER, 0, 100000, 1);
	}

void MultiMapMod::InitFrameRollup() {
	pick_frame_button = GetICustButton(GetDlgItem(hwnd_frame, IDC_TLFR_PICK_FRAME));
	pick_frame_button->SetText(GetString(IDS_TLFR_PICKFRAME));
	pick_frame_button->SetType(CBT_CHECK);
	pick_frame_button->SetHighlightColor(GREEN_WASH);
	pick_frame_button->SetCheckHighlight(TRUE);
	}

void MultiMapMod::InitDataRollup() {
	}

void MultiMapMod::UpdatePBlockParam(int id) {
	if ( !ip  )
		return;

	TimeValue t = ip->GetTime();

	float f_val;
	int i_val;
	BOOL b_val;

	switch ( id ) {

		// Main
		case uvw_map_channel:
			i_val = mapch_spin->GetIVal();
			uvwProy[current_channel]->pblock->SetValue(uvw_map_channel,t,i_val);
			break;
		case uvw_length:
			f_val = length_spin->GetFVal();							
			uvwProy[current_channel]->pblock->SetValue(uvw_length,t,f_val);
			break;
		case uvw_width:
			f_val = width_spin->GetFVal();
			uvwProy[current_channel]->pblock->SetValue(uvw_width,t,f_val);
			break;
		case uvw_height:
			f_val = height_spin->GetFVal();							
			uvwProy[current_channel]->pblock->SetValue(uvw_height,t,f_val);
			break;

		// Tiling & Attenuation
		case uvw_tile_u:
			f_val = u_start_spin->GetFVal();							
			uvwProy[current_channel]->pblock->SetValue(uvw_tile_u,t,f_val);
			break;
		case uvw_offset_u:
			f_val = u_offset_spin->GetFVal();							
			uvwProy[current_channel]->pblock->SetValue(uvw_offset_u,t,f_val);
			break;
		case uvw_tile_v:
			f_val = v_start_spin->GetFVal();							
			uvwProy[current_channel]->pblock->SetValue(uvw_tile_v,t,f_val);
			break;
		case uvw_offset_v:
			f_val = v_offset_spin->GetFVal();							
			uvwProy[current_channel]->pblock->SetValue(uvw_offset_v,t,f_val);
			break;
		case uvw_tile_w:
			f_val = w_start_spin->GetFVal();							
			uvwProy[current_channel]->pblock->SetValue(uvw_tile_w,t,f_val);
			break;
		case uvw_offset_w:
			f_val = w_offset_spin->GetFVal();							
			uvwProy[current_channel]->pblock->SetValue(uvw_offset_w,t,f_val);
			break;
		case uvw_atton:
			b_val = IsDlgButtonChecked(hwnd_tiling,IDC_TL_ATTON);
			uvwProy[current_channel]->pblock->SetValue(uvw_atton,t,b_val);
			break;
		case uvw_att:
			f_val = att_all_spin->GetFVal();							
			uvwProy[current_channel]->pblock->SetValue(uvw_att,t,f_val);
			break;
		case uvw_aus:
			f_val = att_u_start_spin->GetFVal();							
			uvwProy[current_channel]->pblock->SetValue(uvw_aus,t,f_val);
			break;
		case uvw_aue:
			f_val = att_u_offset_spin->GetFVal();							
			uvwProy[current_channel]->pblock->SetValue(uvw_aue,t,f_val);
			break;
		case uvw_avs:
			f_val = att_v_start_spin->GetFVal();							
			uvwProy[current_channel]->pblock->SetValue(uvw_avs,t,f_val);
			break;
		case uvw_ave:
			f_val = att_v_offset_spin->GetFVal();							
			uvwProy[current_channel]->pblock->SetValue(uvw_ave,t,f_val);
			break;
		case uvw_aws:
			f_val = att_w_start_spin->GetFVal();							
			uvwProy[current_channel]->pblock->SetValue(uvw_aws,t,f_val);
			break;
		case uvw_awe:
			f_val = att_w_offset_spin->GetFVal();							
			uvwProy[current_channel]->pblock->SetValue(uvw_awe,t,f_val);
			break;
		case uvw_aruv:
			f_val = att_ruv_spin->GetFVal();							
			uvwProy[current_channel]->pblock->SetValue(uvw_aruv,t,f_val);
			break;
		case uvw_move_u:
			f_val = u_move_spin->GetFVal();							
			uvwProy[current_channel]->pblock->SetValue(uvw_move_u,t,f_val);
			break;
		case uvw_move_v:
			f_val = v_move_spin->GetFVal();							
			uvwProy[current_channel]->pblock->SetValue(uvw_move_v,t,f_val);
			break;
		case uvw_move_w:
			f_val = w_move_spin->GetFVal();							
			uvwProy[current_channel]->pblock->SetValue(uvw_move_w,t,f_val);
			break;
		case uvw_rotate_u:
			f_val = u_rotate_spin->GetFVal();							
			uvwProy[current_channel]->pblock->SetValue(uvw_rotate_u,t,f_val);
			break;
		case uvw_rotate_v:
			f_val = v_rotate_spin->GetFVal();							
			uvwProy[current_channel]->pblock->SetValue(uvw_rotate_v,t,f_val);
			break;
		case uvw_rotate_w:
			f_val = w_rotate_spin->GetFVal();							
			uvwProy[current_channel]->pblock->SetValue(uvw_rotate_w,t,f_val);
			break;
		case uvw_scale_u:
			f_val = u_scale_spin->GetFVal();							
			uvwProy[current_channel]->pblock->SetValue(uvw_scale_u,t,f_val);
			break;
		case uvw_scale_v:
			f_val = v_scale_spin->GetFVal();							
			uvwProy[current_channel]->pblock->SetValue(uvw_scale_v,t,f_val);
			break;
		case uvw_scale_w:
			f_val = w_scale_spin->GetFVal();							
			uvwProy[current_channel]->pblock->SetValue(uvw_scale_w,t,f_val);
			break;
		case uvw_xform_att:
			b_val = IsDlgButtonChecked(hwnd_tiling,IDC_TL_XF_TO_ATT);
			uvwProy[current_channel]->pblock->SetValue(uvw_xform_att,t,b_val);
			break;

		case uvw_axis:
			i_val = -1;
			if ( IsDlgButtonChecked(hwnd_tools,IDC_TLO_X ) )
				i_val = 0;
			if ( IsDlgButtonChecked(hwnd_tools,IDC_TLO_Y ) )
				i_val = 1;
			if ( IsDlgButtonChecked(hwnd_tools,IDC_TLO_Z ) )
				i_val = 2;
			if ( i_val != -1 )
				uvwProy[current_channel]->pblock->SetValue(uvw_axis,t,i_val);
			break;

		case uvw_reverse:
			b_val = IsDlgButtonChecked(hwnd_spline,IDC_TLS_REVERSE);
			uvwProy[current_channel]->pblock->SetValue(uvw_reverse,t,b_val);
			break;
		case uvw_normlize:
			b_val = IsDlgButtonChecked(hwnd_spline,IDC_TLS_NORMALIZED);
			uvwProy[current_channel]->pblock->SetValue(uvw_normlize,t,b_val);
			break;
		case uvw_spline_belt:
			b_val = IsDlgButtonChecked(hwnd_spline,IDC_TLS_BELT);
			uvwProy[current_channel]->pblock->SetValue(uvw_spline_belt,t,b_val);
			break;
		case uvw_normals:
			i_val = -1;
			if ( IsDlgButtonChecked(hwnd_spline,IDC_TLS_PLANE ) )
				i_val = 0;
			if ( IsDlgButtonChecked(hwnd_spline,IDC_TLS_CENTER ) )
				i_val = 1;
			if ( IsDlgButtonChecked(hwnd_spline,IDC_TLS_FIELD ) )
				i_val = 2;
			if ( i_val != -1 )
				uvwProy[current_channel]->pblock->SetValue(uvw_normals,t,i_val);
			break;
		case uvw_normals_start:
			f_val = normals_start_spin->GetFVal();							
			uvwProy[current_channel]->pblock->SetValue(uvw_normals_start,t,f_val);
			break;

/*		case uvw_ffm_thresh:
			f_val = free_threshold_spin->GetFVal();							
			uvwProy[current_channel]->pblock->SetValue(uvw_ffm_thresh,t,f_val);
			break;*/

		case uvw_pelt_rigidity:
			f_val = pelt_rigidity_spin->GetFVal();							
			uvwProy[current_channel]->pblock->SetValue(uvw_pelt_rigidity,t,f_val);
			break;
		case uvw_pelt_stability:
			f_val = pelt_stability_spin->GetFVal();							
			uvwProy[current_channel]->pblock->SetValue(uvw_pelt_stability,t,f_val);
			break;
		case uvw_pelt_frame:
			f_val = pelt_frame_spin->GetFVal();							
			uvwProy[current_channel]->pblock->SetValue(uvw_pelt_frame,t,f_val);
			break;
		case uvw_pelt_iter:
			i_val = pelt_iter_spin->GetIVal();							
			uvwProy[current_channel]->pblock->SetValue(uvw_pelt_iter,t,i_val);
			break;
		case uvw_pelt_border:
			b_val = IsDlgButtonChecked(hwnd_pelt,IDC_TLP_MOVE_BORDER);
			uvwProy[current_channel]->pblock->SetValue(uvw_pelt_border,t,b_val);
			break;

		}


	LocalDataChanged();
	}


#define CLOSE_MAIN_ROLLUP						\
	delete groups_menu_handler;					\
	groups_menu_handler = NULL;					\
	ReleaseICustToolbar(iToolbar);				\
	ReleaseICustButton(iAdd);					\
	ReleaseICustButton(iDel);					\
	ReleaseICustButton(iSave);					\
	ReleaseICustButton(iLoad);					\
	ReleaseICustButton(iCopy);					\
	ReleaseICustButton(iPaste);					\
	ReleaseICustButton(iTools);					\
	ip->UnRegisterDlgWnd( hwnd_main );			\
	ip->DeleteRollupPage( hwnd_main );			\
	ReleaseISpinner(mapch_spin);				\
	ReleaseISpinner(length_spin);				\
	ReleaseISpinner(width_spin);				\
	ReleaseISpinner(height_spin);				\
	hwnd_main = NULL;							

#define CLOSE_TILING_ROLLUP						\
	ip->UnRegisterDlgWnd( hwnd_tiling );		\
	ip->DeleteRollupPage( hwnd_tiling );		\
	hwnd_tiling = NULL;							\
	ReleaseISpinner(u_start_spin);				\
	ReleaseISpinner(u_offset_spin);				\
	ReleaseISpinner(v_start_spin);				\
	ReleaseISpinner(v_offset_spin);				\
	ReleaseISpinner(w_start_spin);				\
	ReleaseISpinner(w_offset_spin);				\
	ReleaseISpinner(att_all_spin);				\
	ReleaseISpinner(att_u_start_spin);			\
	ReleaseISpinner(att_u_offset_spin);			\
	ReleaseISpinner(att_v_start_spin);			\
	ReleaseISpinner(att_v_offset_spin);			\
	ReleaseISpinner(att_w_start_spin);			\
	ReleaseISpinner(att_w_offset_spin);			\
	ReleaseISpinner(att_ruv_spin);				\
	ReleaseISpinner(u_move_spin);				\
	ReleaseISpinner(v_move_spin);				\
	ReleaseISpinner(w_move_spin);				\
	ReleaseISpinner(u_rotate_spin);				\
	ReleaseISpinner(v_rotate_spin);				\
	ReleaseISpinner(w_rotate_spin);				\
	ReleaseISpinner(u_scale_spin);				\
	ReleaseISpinner(v_scale_spin);				\
	ReleaseISpinner(w_scale_spin);

#define CLOSE_TOOLS_ROLLUP						\
	ip->UnRegisterDlgWnd( hwnd_tools );			\
	ip->DeleteRollupPage( hwnd_tools );			\
	hwnd_tools = NULL;

#define CLOSE_SPLINE_ROLLUP						\
	ip->UnRegisterDlgWnd( hwnd_spline );		\
	ip->DeleteRollupPage( hwnd_spline );		\
	hwnd_spline = NULL;							\
	ReleaseICustButton(pick_spline_button);		\
	ReleaseISpinner(normals_start_spin);

#define CLOSE_FREE_ROLLUP						\
	ip->UnRegisterDlgWnd( hwnd_free );			\
	ip->DeleteRollupPage( hwnd_free );			\
	hwnd_free = NULL;							\
	ReleaseICustButton(pick_surface_button);	\
	ReleaseISpinner(free_threshold_spin);
	
#define CLOSE_PELT_ROLLUP						\
	ip->UnRegisterDlgWnd( hwnd_pelt );			\
	ip->DeleteRollupPage( hwnd_pelt );			\
	hwnd_pelt = NULL;							\
	ReleaseISpinner(pelt_rigidity_spin);		\
	ReleaseISpinner(pelt_stability_spin);		\
	ReleaseISpinner(pelt_frame_spin);			\
	ReleaseISpinner(pelt_iter_spin);

#define CLOSE_FRAME_ROLLUP						\
	ip->UnRegisterDlgWnd( hwnd_frame );			\
	ip->DeleteRollupPage( hwnd_frame );			\
	hwnd_frame = NULL;							\
	ReleaseICustButton(pick_frame_button);

#define CLOSE_DATA_ROLLUP						\
	ip->UnRegisterDlgWnd( hwnd_data );			\
	ip->DeleteRollupPage( hwnd_data );			\
	hwnd_data = NULL;

void MultiMapMod::CloseAllRollups() {
	if ( hwnd_main ) {
		CLOSE_MAIN_ROLLUP
		}

	if ( hwnd_tiling ) {
		CLOSE_TILING_ROLLUP
		}

	if ( hwnd_tools ) {
		CLOSE_TOOLS_ROLLUP
		}

	if ( hwnd_spline ) {
		CLOSE_SPLINE_ROLLUP
		}

	if ( hwnd_free ) {
		CLOSE_FREE_ROLLUP
		}

	if ( hwnd_pelt ) {
		CLOSE_PELT_ROLLUP
		}

	if ( hwnd_frame ) {
		CLOSE_FRAME_ROLLUP
		}

	if ( hwnd_data ) {
		CLOSE_DATA_ROLLUP
		}

	}


void MultiMapMod::OpenCloseRollups() {
	int type = uvwProy[current_channel]->GetMappingType();

	if ( !hwnd_main ) {
		hwnd_main = ip->AddRollupPage( hInstance, MAKEINTRESOURCE(IDD_TL_MAIN), TexLayMainDlgProc, _T("Texture Layers Channels"), (LPARAM)this, 0 );
		ip->RegisterDlgWnd( hwnd_main );
		}

	if ( !hwnd_tiling ) {
		hwnd_tiling = ip->AddRollupPage( hInstance, MAKEINTRESOURCE(IDD_TL_TILING), TexLayTilingDlgProc, _T("Tiling & Attenuation"), (LPARAM)this, 0 );
		ip->RegisterDlgWnd( hwnd_tiling );
		}

	if ( !hwnd_tools && type<=MAP_TL_BOX ) {
		hwnd_tools = ip->AddRollupPage(		hInstance,
											MAKEINTRESOURCE(IDD_TL_TOOLS),
											TexLayToolsDlgProc,
											_T("Tools"),
											(LPARAM)this, 0 );
		ip->RegisterDlgWnd( hwnd_tools );
		}
	if ( type>MAP_TL_BOX && hwnd_tools ) {
		CLOSE_TOOLS_ROLLUP
		}

	if ( !hwnd_spline && type==MAP_TL_SPLINE ) {
		hwnd_spline = ip->AddRollupPage(	hInstance,
											MAKEINTRESOURCE(IDD_TL_SPLINE),
											TexLaySplineDlgProc,
											_T("Spline Mapping"),
											(LPARAM)this, 0 );
		ip->RegisterDlgWnd( hwnd_spline );
		}
	if ( type!=MAP_TL_SPLINE && hwnd_spline ) {
		CLOSE_SPLINE_ROLLUP
		}

	if ( !hwnd_free && type==MAP_TL_FFM ) {
		hwnd_free = ip->AddRollupPage(		hInstance,
											MAKEINTRESOURCE(IDD_TL_FREE),
											TexLayFreeDlgProc,
											_T("Free Form Mapping"),
											(LPARAM)this, 0 );
		ip->RegisterDlgWnd( hwnd_free );
		}
	if ( type!=MAP_TL_FFM && hwnd_free ) {
		CLOSE_FREE_ROLLUP
		}

	if ( !hwnd_pelt && type==MAP_TL_PELT ) {
		hwnd_pelt = ip->AddRollupPage(		hInstance,
											MAKEINTRESOURCE(IDD_TL_PELT),
											TexLayPeltDlgProc,
											_T("Pelt Mapping"),
											(LPARAM)this, 0 );
		ip->RegisterDlgWnd( hwnd_pelt );
		}
	if ( type!=MAP_TL_PELT && hwnd_pelt ) {
		CLOSE_PELT_ROLLUP
		}

	if ( !hwnd_frame && type==MAP_TL_FRAME ) {
		hwnd_frame = ip->AddRollupPage(		hInstance,
											MAKEINTRESOURCE(IDD_TL_FRAME),
											TexLayFrameDlgProc,
											_T("Frame Mapping"),
											(LPARAM)this, 0 );
		ip->RegisterDlgWnd( hwnd_frame );
		}
	if ( type!=MAP_TL_FRAME && hwnd_frame ) {
		CLOSE_FRAME_ROLLUP
		}

	if ( !hwnd_data && type==MAP_TL_UVWDATA ) {
		hwnd_data = ip->AddRollupPage(		hInstance,
											MAKEINTRESOURCE(IDD_TL_DATA),
											TexLayDataDlgProc,
											_T("UVW Data"),
											(LPARAM)this, 0 );
		ip->RegisterDlgWnd( hwnd_data );
		}
	if ( type!=MAP_TL_UVWDATA && hwnd_data ) {
		CLOSE_DATA_ROLLUP
		}
	}

// MAIN
#define UPDATE_GROUP_NAME											\
	group_name_edit->SetText( uvwProy[current_channel]->descCanal );

// MAPPING
#define UPDATE_GROUPS_MENU	\
	GroupsMenuForceDrawItem(current_channel);

#define UPDATE_TYPES_LIST	\
	SendDlgItemMessage(hwnd_main,IDC_TLM_TYPES,CB_SETCURSEL,uvwProy[current_channel]->GetMappingType(),0);

#define UPDATE_MAPCH_SPIN											\
	uvwProy[current_channel]->pblock->GetValue(uvw_map_channel,t,i_val,iv);	\
	mapch_spin->SetValue(i_val,FALSE);
	

#define UPDATE_LENGTH_SPIN													\
	uvwProy[current_channel]->pblock->GetValue(uvw_length,t,f_val,iv);		\
	length_spin->SetValue(f_val,FALSE);										\
	length_spin->SetKeyBrackets( uvwProy[current_channel]->pblock->KeyFrameAtTimeByIndex(uvwProy[current_channel]->pblock->IDtoIndex(uvw_length), t) );
		
#define UPDATE_WIDTH_SPIN													\
	uvwProy[current_channel]->pblock->GetValue(uvw_width,t,f_val,iv);		\
	if ( uvwProy[current_channel]->GetMappingType() == MAP_TL_SPLINE ) {	\
		f_val = 0.0f;														\
		width_spin->Enable(FALSE);											\
		}																	\
	else {																	\
		width_spin->Enable(TRUE);											\
		}																	\
	width_spin->SetValue(f_val,FALSE);										\
	width_spin->SetKeyBrackets( uvwProy[current_channel]->pblock->KeyFrameAtTimeByIndex(uvwProy[current_channel]->pblock->IDtoIndex(uvw_width), t) );

#define UPDATE_HEIGHT_SPIN											\
	uvwProy[current_channel]->pblock->GetValue(uvw_height,t,f_val,iv);		\
	height_spin->SetValue(f_val,FALSE);								\
	height_spin->SetKeyBrackets( uvwProy[current_channel]->pblock->KeyFrameAtTimeByIndex(uvwProy[current_channel]->pblock->IDtoIndex(uvw_height), t) );


// TILING AND ATTENUATION
#define UPDATE_U_TILE_SPIN												\
	uvwProy[current_channel]->pblock->GetValue(uvw_tile_u,t,f_val,iv);	\
	u_start_spin->SetValue(f_val,FALSE);								\
	u_start_spin->SetKeyBrackets( uvwProy[current_channel]->pblock->KeyFrameAtTimeByIndex(uvwProy[current_channel]->pblock->IDtoIndex(uvw_tile_u), t) );

#define UPDATE_U_OFFSET_SPIN											\
	uvwProy[current_channel]->pblock->GetValue(uvw_offset_u,t,f_val,iv);		\
	u_offset_spin->SetValue(f_val,FALSE);								\
	u_offset_spin->SetKeyBrackets( uvwProy[current_channel]->pblock->KeyFrameAtTimeByIndex(uvwProy[current_channel]->pblock->IDtoIndex(uvw_offset_u), t) );

#define UPDATE_V_TILE_SPIN												\
	uvwProy[current_channel]->pblock->GetValue(uvw_tile_v,t,f_val,iv);	\
	v_start_spin->SetValue(f_val,FALSE);								\
	v_start_spin->SetKeyBrackets( uvwProy[current_channel]->pblock->KeyFrameAtTimeByIndex(uvwProy[current_channel]->pblock->IDtoIndex(uvw_tile_v), t) );

#define UPDATE_V_OFFSET_SPIN											\
	uvwProy[current_channel]->pblock->GetValue(uvw_offset_v,t,f_val,iv);		\
	v_offset_spin->SetValue(f_val,FALSE);								\
	v_offset_spin->SetKeyBrackets( uvwProy[current_channel]->pblock->KeyFrameAtTimeByIndex(uvwProy[current_channel]->pblock->IDtoIndex(uvw_offset_v), t) );

#define UPDATE_W_TILE_SPIN												\
	uvwProy[current_channel]->pblock->GetValue(uvw_tile_w,t,f_val,iv);	\
	w_start_spin->SetValue(f_val,FALSE);								\
	w_start_spin->SetKeyBrackets( uvwProy[current_channel]->pblock->KeyFrameAtTimeByIndex(uvwProy[current_channel]->pblock->IDtoIndex(uvw_tile_w), t) );

#define UPDATE_W_OFFSET_SPIN											\
	uvwProy[current_channel]->pblock->GetValue(uvw_offset_w,t,f_val,iv);		\
	w_offset_spin->SetValue(f_val,FALSE);								\
	w_offset_spin->SetKeyBrackets( uvwProy[current_channel]->pblock->KeyFrameAtTimeByIndex(uvwProy[current_channel]->pblock->IDtoIndex(uvw_offset_w), t) );

#define UPDATE_ATT_ON_CHECK												\
	uvwProy[current_channel]->pblock->GetValue(uvw_atton,t,b_val,iv);	\
	CheckDlgButton(hwnd_tiling,IDC_TL_ATTON,b_val);

#define UPDATE_ATT_ALL_SPIN												\
	uvwProy[current_channel]->pblock->GetValue(uvw_att,t,f_val,iv);			\
	att_all_spin->SetValue(f_val,FALSE);								\
	att_all_spin->SetKeyBrackets( uvwProy[current_channel]->pblock->KeyFrameAtTimeByIndex(uvwProy[current_channel]->pblock->IDtoIndex(uvw_att), t) );

#define UPDATE_ATT_U_START_SPIN											\
	uvwProy[current_channel]->pblock->GetValue(uvw_aus,t,f_val,iv);			\
	att_u_start_spin->SetValue(f_val,FALSE);							\
	att_u_start_spin->SetKeyBrackets( uvwProy[current_channel]->pblock->KeyFrameAtTimeByIndex(uvwProy[current_channel]->pblock->IDtoIndex(uvw_aus), t) );

#define UPDATE_ATT_U_OFFSET_SPIN									\
	uvwProy[current_channel]->pblock->GetValue(uvw_aue,t,f_val,iv);			\
	att_u_offset_spin->SetValue(f_val,FALSE);						\
	att_u_offset_spin->SetKeyBrackets( uvwProy[current_channel]->pblock->KeyFrameAtTimeByIndex(uvwProy[current_channel]->pblock->IDtoIndex(uvw_aue), t) );

#define UPDATE_ATT_V_START_SPIN										\
	uvwProy[current_channel]->pblock->GetValue(uvw_avs,t,f_val,iv);			\
	att_v_start_spin->SetValue(f_val,FALSE);						\
	att_v_start_spin->SetKeyBrackets( uvwProy[current_channel]->pblock->KeyFrameAtTimeByIndex(uvwProy[current_channel]->pblock->IDtoIndex(uvw_avs), t) );

#define UPDATE_ATT_V_OFFSET_SPIN									\
	uvwProy[current_channel]->pblock->GetValue(uvw_ave,t,f_val,iv);			\
	att_v_offset_spin->SetValue(f_val,FALSE);						\
	att_v_offset_spin->SetKeyBrackets( uvwProy[current_channel]->pblock->KeyFrameAtTimeByIndex(uvwProy[current_channel]->pblock->IDtoIndex(uvw_ave), t) );

#define UPDATE_ATT_W_START_SPIN										\
	uvwProy[current_channel]->pblock->GetValue(uvw_aws,t,f_val,iv);			\
	att_w_start_spin->SetValue(f_val,FALSE);						\
	att_w_start_spin->SetKeyBrackets( uvwProy[current_channel]->pblock->KeyFrameAtTimeByIndex(uvwProy[current_channel]->pblock->IDtoIndex(uvw_aws), t) );

#define UPDATE_ATT_W_OFFSET_SPIN									\
	uvwProy[current_channel]->pblock->GetValue(uvw_awe,t,f_val,iv);			\
	att_w_offset_spin->SetValue(f_val,FALSE);						\
	att_w_offset_spin->SetKeyBrackets( uvwProy[current_channel]->pblock->KeyFrameAtTimeByIndex(uvwProy[current_channel]->pblock->IDtoIndex(uvw_awe), t) );

#define UPDATE_ATT_RUV_SPIN											\
	uvwProy[current_channel]->pblock->GetValue(uvw_aruv,t,f_val,iv);			\
	att_ruv_spin->SetValue(f_val,FALSE);							\
	att_ruv_spin->SetKeyBrackets( uvwProy[current_channel]->pblock->KeyFrameAtTimeByIndex(uvwProy[current_channel]->pblock->IDtoIndex(uvw_aruv), t) );

#define UPDATE_AXIS_RADIO											\
	uvwProy[current_channel]->pblock->GetValue(uvw_axis,t,i_val,iv);			\
	CheckRadioButton(hwnd_tools, IDC_TLO_X, IDC_TLO_Z, IDC_TLO_X+i_val);

#define UPDATE_U_MOVE_SPIN											\
	uvwProy[current_channel]->pblock->GetValue(uvw_move_u,t,f_val,iv);			\
	u_move_spin->SetValue(f_val,FALSE);							\
	u_move_spin->SetKeyBrackets( uvwProy[current_channel]->pblock->KeyFrameAtTimeByIndex(uvwProy[current_channel]->pblock->IDtoIndex(uvw_move_u), t) );

#define UPDATE_V_MOVE_SPIN											\
	uvwProy[current_channel]->pblock->GetValue(uvw_move_v,t,f_val,iv);			\
	v_move_spin->SetValue(f_val,FALSE);							\
	v_move_spin->SetKeyBrackets( uvwProy[current_channel]->pblock->KeyFrameAtTimeByIndex(uvwProy[current_channel]->pblock->IDtoIndex(uvw_move_v), t) );

#define UPDATE_W_MOVE_SPIN											\
	uvwProy[current_channel]->pblock->GetValue(uvw_move_w,t,f_val,iv);			\
	w_move_spin->SetValue(f_val,FALSE);							\
	w_move_spin->SetKeyBrackets( uvwProy[current_channel]->pblock->KeyFrameAtTimeByIndex(uvwProy[current_channel]->pblock->IDtoIndex(uvw_move_w), t) );

#define UPDATE_U_ROTATE_SPIN											\
	uvwProy[current_channel]->pblock->GetValue(uvw_rotate_u,t,f_val,iv);			\
	u_rotate_spin->SetValue(f_val,FALSE);							\
	u_rotate_spin->SetKeyBrackets( uvwProy[current_channel]->pblock->KeyFrameAtTimeByIndex(uvwProy[current_channel]->pblock->IDtoIndex(uvw_rotate_u), t) );

#define UPDATE_V_ROTATE_SPIN											\
	uvwProy[current_channel]->pblock->GetValue(uvw_rotate_v,t,f_val,iv);			\
	v_rotate_spin->SetValue(f_val,FALSE);							\
	v_rotate_spin->SetKeyBrackets( uvwProy[current_channel]->pblock->KeyFrameAtTimeByIndex(uvwProy[current_channel]->pblock->IDtoIndex(uvw_rotate_v), t) );

#define UPDATE_W_ROTATE_SPIN											\
	uvwProy[current_channel]->pblock->GetValue(uvw_rotate_w,t,f_val,iv);			\
	w_rotate_spin->SetValue(f_val,FALSE);							\
	w_rotate_spin->SetKeyBrackets( uvwProy[current_channel]->pblock->KeyFrameAtTimeByIndex(uvwProy[current_channel]->pblock->IDtoIndex(uvw_rotate_w), t) );

#define UPDATE_U_SCALE_SPIN											\
	uvwProy[current_channel]->pblock->GetValue(uvw_scale_u,t,f_val,iv);			\
	u_scale_spin->SetValue(f_val,FALSE);							\
	u_scale_spin->SetKeyBrackets( uvwProy[current_channel]->pblock->KeyFrameAtTimeByIndex(uvwProy[current_channel]->pblock->IDtoIndex(uvw_scale_u), t) );

#define UPDATE_V_SCALE_SPIN											\
	uvwProy[current_channel]->pblock->GetValue(uvw_scale_v,t,f_val,iv);			\
	v_scale_spin->SetValue(f_val,FALSE);							\
	v_scale_spin->SetKeyBrackets( uvwProy[current_channel]->pblock->KeyFrameAtTimeByIndex(uvwProy[current_channel]->pblock->IDtoIndex(uvw_scale_v), t) );

#define UPDATE_W_SCALE_SPIN											\
	uvwProy[current_channel]->pblock->GetValue(uvw_scale_w,t,f_val,iv);			\
	w_scale_spin->SetValue(f_val,FALSE);							\
	w_scale_spin->SetKeyBrackets( uvwProy[current_channel]->pblock->KeyFrameAtTimeByIndex(uvwProy[current_channel]->pblock->IDtoIndex(uvw_scale_w), t) );

#define UPDATE_XF_ATT_CHECK												\
	uvwProy[current_channel]->pblock->GetValue(uvw_xform_att,t,b_val,iv);	\
	CheckDlgButton(hwnd_tiling,IDC_TL_XF_TO_ATT,b_val);

//static void SpinnerSetKeyBrackets( ISpinnerControl * spin, UVWProyector* uvwProy, unsigned int channel, unsigned int id, TimeValue t, float val, Interval &iv )
//{
//	uvwProy[ channel ]->pblock->GetValue( id, t, val, iv );
//	spin->SetValue( val, FALSE );
//	spin->SetKeyBrackets( uvwProy[ channel ]->pblock->KeyFrameAtTime( uvwProy[ channel ]->pblock->IDtoIndex(uvw_move_u), t) );
//}

// SPLINE MAPPING

#define UPDATE_SPL_SPLINE_BUTTON														\
	uvwProy[current_channel]->GetSplineNode();											\
	if ( uvwProy[current_channel]->spline_node )										\
		pick_spline_button->SetText( uvwProy[current_channel]->spline_node->GetName() );\
	else																				\
		pick_spline_button->SetText( GetString(IDS_MTL_PICKSPLINE) );

#define UPDATE_SPL_REVERSE_CHECK									\
	uvwProy[current_channel]->pblock->GetValue(uvw_reverse,t,b_val,iv);		\
	CheckDlgButton(hwnd_spline,IDC_TLS_REVERSE,b_val);				

#define UPDATE_SPL_NORMALIZE_CHECK									\
	uvwProy[current_channel]->pblock->GetValue(uvw_normlize,t,b_val,iv);		\
	CheckDlgButton(hwnd_spline,IDC_TLS_NORMALIZED,b_val);

#define UPDATE_SPL_BELT_CHECK									\
	uvwProy[current_channel]->pblock->GetValue(uvw_spline_belt,t,b_val,iv);		\
	CheckDlgButton(hwnd_spline,IDC_TLS_BELT,b_val);

#define UPDATE_SPL_NORMALS_RADIO									\
	uvwProy[current_channel]->pblock->GetValue(uvw_normals,t,i_val,iv);		\
	CheckRadioButton(hwnd_spline, IDC_TLS_PLANE, IDC_TLS_FIELD, IDC_TLS_PLANE+i_val);

#define UPDATE_NORMALS_START_SPIN											\
	uvwProy[current_channel]->pblock->GetValue(uvw_normals_start,t,f_val,iv);			\
	normals_start_spin->SetValue(f_val,FALSE);							\
	normals_start_spin->SetKeyBrackets( uvwProy[current_channel]->pblock->KeyFrameAtTimeByIndex(uvwProy[current_channel]->pblock->IDtoIndex(uvw_normals_start), t) );

/*// FREE FORM MAPPING
#define UPDATE_FREE_THRESH_SPIN										\
	uvwProy[current_channel]->pblock->GetValue(uvw_ffm_thresh,t,f_val,iv);	\
	free_threshold_spin->SetValue(f_val,FALSE);						\
	free_threshold_spin->SetKeyBrackets( uvwProy[current_channel]->pblock->KeyFrameAtTimeByIndex(uvwProy[current_channel]->pblock->IDtoIndex(uvw_ffm_thresh), t) );
*/
// PELT MAPPING
#define UPDATE_PELT_RIGIDITY_SPIN											\
	uvwProy[current_channel]->pblock->GetValue(uvw_pelt_rigidity,t,f_val,iv);	\
	pelt_rigidity_spin->SetValue(f_val,FALSE);						\
	pelt_rigidity_spin->SetKeyBrackets( uvwProy[current_channel]->pblock->KeyFrameAtTimeByIndex(uvwProy[current_channel]->pblock->IDtoIndex(uvw_pelt_rigidity), t) );

#define UPDATE_PELT_STABILITY_SPIN											\
	uvwProy[current_channel]->pblock->GetValue(uvw_pelt_stability,t,f_val,iv);	\
	pelt_stability_spin->SetValue(f_val,FALSE);						\
	pelt_stability_spin->SetKeyBrackets( uvwProy[current_channel]->pblock->KeyFrameAtTimeByIndex(uvwProy[current_channel]->pblock->IDtoIndex(uvw_pelt_stability), t) );

#define UPDATE_PELT_FRAME_SPIN											\
	uvwProy[current_channel]->pblock->GetValue(uvw_pelt_frame,t,f_val,iv);	\
	pelt_frame_spin->SetValue(f_val,FALSE);						\
	pelt_frame_spin->SetKeyBrackets( uvwProy[current_channel]->pblock->KeyFrameAtTimeByIndex(uvwProy[current_channel]->pblock->IDtoIndex(uvw_pelt_frame), t) );

#define UPDATE_PELT_ITER_SPIN											\
	uvwProy[current_channel]->pblock->GetValue(uvw_pelt_iter,t,i_val,iv);	\
	pelt_iter_spin->SetValue(i_val,FALSE);						\
	pelt_iter_spin->SetKeyBrackets( uvwProy[current_channel]->pblock->KeyFrameAtTimeByIndex(uvwProy[current_channel]->pblock->IDtoIndex(uvw_pelt_iter), t) );

#define UPDATE_PELT_BORDER_ON_CHECK											\
	uvwProy[current_channel]->pblock->GetValue(uvw_pelt_border,t,b_val,iv);	\
	CheckDlgButton(hwnd_pelt,IDC_TLP_MOVE_BORDER,b_val);

#define UPDATE_FRAME_NODE_BUTTON												\
	INode * frame_node = NULL;															\
	uvwProy[current_channel]->pblock->GetValue(uvw_frame_node,t,frame_node,iv);	\
	if ( frame_node )															\
		pick_frame_button->SetText( frame_node->GetName() );					\
	else																		\
		pick_frame_button->SetText( GetString(IDS_TLFR_PICKFRAME) );

void MultiMapMod::UpdateUIItem( int id, UVWProyector* uvw_proy ) {
	if (!ip)
		return;

	if ( !ip || uvw_proy!=uvwProy[current_channel] )
		return;

//#ifdef ALPS_PROTECTED
//	if ( auth_ok == false )	return;
//#endif

	TimeValue t = ip->GetTime();

	float f_val;
	int i_val;
	BOOL b_val;

	Interval iv = FOREVER;

	switch ( id ) {
		case uvw_mapping_type:
			UPDATE_GROUPS_MENU
			UPDATE_TYPES_LIST
			UpdateUIAll();
			break;
		case uvw_map_channel:
			UPDATE_MAPCH_SPIN
			UPDATE_GROUPS_MENU
			break;
		case uvw_length:
			UPDATE_LENGTH_SPIN
			break;
		case uvw_width:
			UPDATE_WIDTH_SPIN
			break;
		case uvw_height:
			UPDATE_HEIGHT_SPIN
			break;

		case uvw_tile_u:
			UPDATE_U_TILE_SPIN
			break;
		case uvw_offset_u:
			UPDATE_U_OFFSET_SPIN
			break;
		case uvw_tile_v:
			UPDATE_V_TILE_SPIN
			break;
		case uvw_offset_v:
			UPDATE_V_OFFSET_SPIN
			break;
		case uvw_tile_w:
			UPDATE_W_TILE_SPIN
			break;
		case uvw_offset_w:
			UPDATE_W_OFFSET_SPIN
			break;
		case uvw_atton:
			UPDATE_ATT_ON_CHECK;
			break;
		case uvw_att:
			UPDATE_ATT_ALL_SPIN;
			break;
		case uvw_aus:
			UPDATE_ATT_U_START_SPIN;
			break;
		case uvw_aue:
			UPDATE_ATT_U_OFFSET_SPIN;
			break;
		case uvw_avs:
			UPDATE_ATT_V_START_SPIN;
			break;
		case uvw_ave:
			UPDATE_ATT_V_OFFSET_SPIN;
			break;
		case uvw_aws:
			UPDATE_ATT_W_START_SPIN;
			break;
		case uvw_awe:
			UPDATE_ATT_W_OFFSET_SPIN;
			break;
		case uvw_aruv:
			UPDATE_ATT_RUV_SPIN;
			break;
		case uvw_move_u:
			UPDATE_U_MOVE_SPIN
			break;
		case uvw_move_v:
			UPDATE_V_MOVE_SPIN
			break;
		case uvw_move_w:
			UPDATE_W_MOVE_SPIN
			break;
		case uvw_rotate_u:
			UPDATE_U_ROTATE_SPIN
			break;
		case uvw_rotate_v:
			UPDATE_V_ROTATE_SPIN
			break;
		case uvw_rotate_w:
			UPDATE_W_ROTATE_SPIN
			break;
		case uvw_scale_u:
			UPDATE_U_SCALE_SPIN
			break;
		case uvw_scale_v:
			UPDATE_V_SCALE_SPIN
			break;
		case uvw_scale_w:
			UPDATE_W_SCALE_SPIN
			break;
		case uvw_xform_att:
			UPDATE_XF_ATT_CHECK;
			break;

		case uvw_axis:
			if ( hwnd_tools ) {
				UPDATE_AXIS_RADIO;
				}
			break;

		case -1:
			if ( hwnd_spline ) {
				UPDATE_SPL_SPLINE_BUTTON
				}
			break;
		case uvw_normlize:
			if ( hwnd_spline ) {
				UPDATE_SPL_NORMALIZE_CHECK
				}
			break;
		case uvw_reverse:
			if ( hwnd_spline ) {
				UPDATE_SPL_REVERSE_CHECK
				}
			break;
		case uvw_normals:
			if ( hwnd_spline ) {
				UPDATE_SPL_NORMALS_RADIO
				}
			break;
		case uvw_normals_start:
			if ( hwnd_spline ) {
				UPDATE_NORMALS_START_SPIN
				}
			break;
		case uvw_spline_belt:
			if ( hwnd_spline ) {
				UPDATE_SPL_BELT_CHECK
				}
			break;			

/*		case uvw_ffm_thresh:
			if ( hwnd_free ) {
				UPDATE_FREE_THRESH_SPIN
				}
			break;*/

		case uvw_pelt_rigidity:
			if ( hwnd_pelt ) {
				UPDATE_PELT_RIGIDITY_SPIN
				}
			break;

		case uvw_pelt_stability:
			if ( hwnd_pelt ) {
				UPDATE_PELT_STABILITY_SPIN
				}
			break;

		case uvw_pelt_frame:
			if ( hwnd_pelt ) {
				UPDATE_PELT_FRAME_SPIN
				}
			break;

		case uvw_pelt_iter:
			if ( hwnd_pelt ) {
				UPDATE_PELT_ITER_SPIN
				}
			break;

		case uvw_pelt_border:
			if ( hwnd_pelt ) {
				UPDATE_PELT_BORDER_ON_CHECK
				}
			break;

		case uvw_frame_node:
			if ( hwnd_frame ) {
				UPDATE_FRAME_NODE_BUTTON
				}
			break;

		default:
			break;
		}
	}

void MultiMapMod::UpdateUIAll() {
	if (!ip) 
		return;

	OpenCloseRollups();

	TimeValue t = ip->GetTime();
	Interval iv = FOREVER;

	int i_val;
	float f_val;
	BOOL b_val;

//	UPDATE MAIN ROLLUP
	if ( hwnd_main ) {
		if ( uvwProy[current_channel]->GetMappingType()==MAP_TL_SPLINE ) {
			SetWindowText(GetDlgItem(hwnd_main,IDC_TXT_HEIGHT),"Radius");
		//	EnableWindow(GetDlgItem(hwnd_main,IDC_TXT_WIDTH),FALSE);
			}
		else {
			SetWindowText(GetDlgItem(hwnd_main,IDC_TXT_HEIGHT),"Height");
		//	EnableWindow(GetDlgItem(hwnd_main,IDC_TXT_WIDTH),TRUE);
			}
		UPDATE_TYPES_LIST
		UPDATE_GROUP_NAME
		UPDATE_MAPCH_SPIN
		UPDATE_LENGTH_SPIN
		UPDATE_WIDTH_SPIN
		UPDATE_HEIGHT_SPIN
		}

//	UPDATE TILING ROLLUP
	if ( hwnd_tiling ) {
		UPDATE_U_TILE_SPIN
		UPDATE_U_OFFSET_SPIN
		UPDATE_V_TILE_SPIN
		UPDATE_V_OFFSET_SPIN
		UPDATE_W_TILE_SPIN
		UPDATE_W_OFFSET_SPIN
		UPDATE_ATT_ON_CHECK
		UPDATE_ATT_ALL_SPIN
		UPDATE_ATT_U_START_SPIN
		UPDATE_ATT_U_OFFSET_SPIN
		UPDATE_ATT_V_START_SPIN
		UPDATE_ATT_V_OFFSET_SPIN
		UPDATE_ATT_W_START_SPIN
		UPDATE_ATT_W_OFFSET_SPIN
		UPDATE_ATT_RUV_SPIN
		UPDATE_U_MOVE_SPIN
		UPDATE_V_MOVE_SPIN
		UPDATE_W_MOVE_SPIN
		UPDATE_U_ROTATE_SPIN
		UPDATE_V_ROTATE_SPIN
		UPDATE_W_ROTATE_SPIN
		UPDATE_U_SCALE_SPIN
		UPDATE_V_SCALE_SPIN
		UPDATE_W_SCALE_SPIN
		UPDATE_XF_ATT_CHECK
		}

//	UPDATE TOOLS ROLLUP
	if ( hwnd_tools ) {
		UPDATE_AXIS_RADIO
		}

//	SPLINE ROLLUP
	if ( hwnd_spline ) {
		UPDATE_SPL_SPLINE_BUTTON
		UPDATE_SPL_REVERSE_CHECK
		UPDATE_SPL_NORMALIZE_CHECK
		UPDATE_SPL_BELT_CHECK
		UPDATE_SPL_NORMALS_RADIO
		UPDATE_NORMALS_START_SPIN
		}

// FREE FORM ROLLUP
	if ( hwnd_free ) {
		TextNumPoints();
//		UPDATE_FREE_THRESH_SPIN
		}

// PELT ROLLUP
	if ( hwnd_pelt ) {
		UPDATE_PELT_RIGIDITY_SPIN
		UPDATE_PELT_STABILITY_SPIN
		UPDATE_PELT_FRAME_SPIN
		UPDATE_PELT_ITER_SPIN
		UPDATE_PELT_BORDER_ON_CHECK
		}

// FRAME ROLLUP
	if ( hwnd_frame ) {
		UPDATE_FRAME_NODE_BUTTON
		}

	}


// DISPLAY OPTIONS

//Win32 : static BOOL CALLBACK MapOptionsDlgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) 
static INT_PTR CALLBACK MapOptionsDlgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) 
{
	//Win32 : MultiMapMod *tl = (MultiMapMod*)GetWindowLong(hWnd,GWL_USERDATA);
	MultiMapMod *tl = DLGetWindowLongPtr<MultiMapMod*>(hWnd);

	switch (msg) {
		case WM_INITDIALOG: {
			tl = (MultiMapMod*)lParam;
			
			//Win32 : SetWindowLong(hWnd,GWL_USERDATA,lParam);
			DLSetWindowLongPtr(hWnd, lParam);

			tl->hwnd_options = hWnd;

			CheckDlgButton(hWnd,IDC_DO_GIZMO,tl->show_all_gizmos);
			CheckDlgButton(hWnd,IDC_TLO_COMPACT_TVERTS,tl->compact_tverts);

			SetupIntSpinner(hWnd,IDC_TLO_CHANNEL_SPIN,IDC_TLO_CHANNEL,1,99,1);

			CenterWindow(hWnd,GetCOREInterface()->GetMAXHWnd());
			break;
			}

		case WM_COMMAND:
			switch (LOWORD(wParam)) {

				case IDC_DO_GIZMO:
					tl->show_all_gizmos = IsDlgButtonChecked(hWnd,IDC_DO_GIZMO);
					tl->LocalDataChanged();
					tl->ip->RedrawViews(tl->ip->GetTime());
					break;

				case IDC_TLO_COMPACT_TVERTS:
					tl->compact_tverts = IsDlgButtonChecked(hWnd,IDC_TLO_COMPACT_TVERTS);
					tl->LocalDataChanged();
					break;

				case IDC_TLO_SS_EDGES:
					tl->GetEdgeSelectionSets();
					break;

				case IDC_TLO_SS_FACES:
					tl->GetFaceSelectionSets();
					break;

				case IDC_TLO_SAVE_CH_UVW: {
					ISpinnerControl * spin = GetISpinner(GetDlgItem(hWnd,IDC_TLO_CHANNEL_SPIN));
					int uvw_ch = spin->GetIVal();
					ReleaseISpinner(spin);
					tl->save_uvw_channel = uvw_ch;
					tl->GetSaveUVWFilename();
					tl->LocalDataChanged();
					}
					break;

				case IDC_CLOSE:
				case IDOK:
					tl->hwnd_options = NULL;
					EndDialog(hWnd,1);
					break;
				}
			break;

		case WM_DESTROY:
			tl->hwnd_options = NULL;
			break;

		default:
			return FALSE;
		}
	return TRUE;
	}

void MultiMapMod::DisplayOptions() {
	if ( !hwnd_options )
		CreateDialogParam(	hInstance,
							MAKEINTRESOURCE(IDD_MAPOPTIONS),
							hwnd_main,
							MapOptionsDlgProc,
							(LPARAM)this);
	}


// DO BITMAP FIT

void MultiMapMod::DoBitmapFit() {
	BitmapInfo bi;
	TheManager->SelectFileInputEx(&bi, hwnd_tools, GetString(IDS_TL_SELECTIMAGE));
	if (bi.Name()[0]) {		
		TheManager->GetImageInfo(&bi);
		uvwProy[current_channel]->aspect = bi.Aspect() * float(bi.Width())/float(bi.Height());
		uvwProy[current_channel]->flags |= CONTROL_ASPECT|CONTROL_HOLD;
		NotifyDependents(FOREVER,PART_TEXMAP,REFMSG_CHANGE);
		PanelVisibility(PV_BLOCK);
		}
	}


// SET NUMBER OF CHANNELS

static BOOL CALLBACK NumChannsDlgProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
	switch (msg) {
		case WM_INITDIALOG: {
			ISpinnerControl *spin = 
				SetupIntSpinner(
					hWnd,IDC_MM_NUMCHSPIN,IDC_MM_NUMCH,
					1,1000,(int)lParam);
			ReleaseISpinner(spin);
			CenterWindow(hWnd,GetParent(hWnd));
			break;
			}

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDOK: {
					ISpinnerControl *spin = 
						GetISpinner(GetDlgItem(hWnd,IDC_MM_NUMCHSPIN));
					EndDialog(hWnd,spin->GetIVal());
					ReleaseISpinner(spin);
					break;
					}

				case IDCANCEL:
					ISpinnerControl *spin = 
						GetISpinner(GetDlgItem(hWnd,IDC_MM_NUMCHSPIN));
					EndDialog(hWnd,-1);
					ReleaseISpinner(spin);
					break;
				}
			break;

		default:
			return FALSE;
		}
	return TRUE;
	}


// TLTODO: Si el cuadro de dialogo esta abierto, y el usuario vuelve a picar
//		   el boton que lo activa, que este cuadro pase al frente.
void MultiMapMod::SwitchTileDlgCurve() {
	if (!tileDlg) {
		TSTR title = _T("U Tile Control Curve");
		tileDlg = new CurveDlg(this,&uvwProy[current_channel]->tileCrv,ip,hwnd_main,title); 
		return;
		}
	else {
		if (tileDlg->IsActive()) {
			}
		else {
			delete tileDlg;
			tileDlg = NULL;
			SwitchTileDlgCurve();
			}
		}
	}

void MultiMapMod::SwitchRadiusDlgCurve() {
	if (!radiusDlg) {
		TSTR title = _T("W Tile Control Curve");
		radiusDlg = new CurveDlg(this,&uvwProy[current_channel]->wTileCrv,ip,hwnd_main,title); 
		return;
		}
	else {
		if (radiusDlg->IsActive()) {
			}
		else {
			delete radiusDlg;
			radiusDlg = NULL;
			SwitchRadiusDlgCurve();
			}
		}
	}

void MultiMapMod::SwitchAngleDlgCurve() {
	if (!angleDlg) {
		TSTR title = _T("U Offset Control Curve");
		angleDlg = new CurveDlg(this,&uvwProy[current_channel]->angleCrv,ip,hwnd_main,title); 
		return;
		}
	else {
		if (angleDlg->IsActive()) {
			}
		else {
			delete angleDlg;
			angleDlg = NULL;
			SwitchAngleDlgCurve();
			}
		}
	}

//WIN32 : static BOOL CALLBACK GMNumPointsDlgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) 
static INT_PTR CALLBACK GMNumPointsDlgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) 
{
	static int *dim;
	switch (msg) {
		case WM_INITDIALOG: {
			dim = (int*)lParam;
			ISpinnerControl *spin;

			CheckDlgButton(hWnd,IDC_TL_SHAPE,dim[2]);
			CheckDlgButton(hWnd,IDC_TL_RESTO,dim[3]);

			spin = GetISpinner(GetDlgItem(hWnd,IDC_TL_GMROW_SPIN));
			spin->SetLimits(2,1000, FALSE);
			spin->LinkToEdit(GetDlgItem(hWnd,IDC_TL_GMROW), EDITTYPE_INT);
			spin->SetValue(dim[0],FALSE);
			ReleaseISpinner(spin);

			spin = GetISpinner(GetDlgItem(hWnd,IDC_TL_GMCOL_SPIN));
			spin->SetLimits(2,100, FALSE);
			spin->LinkToEdit(GetDlgItem(hWnd,IDC_TL_GMCOL), EDITTYPE_INT);
			spin->SetValue(dim[1],FALSE);
			ReleaseISpinner(spin);

			if (!dim[4]) {
				CheckDlgButton(hWnd,IDC_TL_SHAPE,0);
				CheckDlgButton(hWnd,IDC_TL_RESTO,0);
				EnableWindow(GetDlgItem(hWnd,IDC_TL_SHAPE),FALSE);
				EnableWindow(GetDlgItem(hWnd,IDC_TL_RESTO),FALSE);
				}

			CenterWindow(hWnd,GetParent(hWnd));
			break;
			}

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDOK: {
					ISpinnerControl *spin;

					spin = GetISpinner(GetDlgItem(hWnd,IDC_TL_GMROW_SPIN));
					dim[0] = spin->GetIVal();
					ReleaseISpinner(spin);

					spin = GetISpinner(GetDlgItem(hWnd,IDC_TL_GMCOL_SPIN));
					dim[1] = spin->GetIVal();
					ReleaseISpinner(spin);

					dim[2] = IsDlgButtonChecked(hWnd,IDC_TL_SHAPE);
					dim[3] = IsDlgButtonChecked(hWnd,IDC_TL_RESTO);

					EndDialog(hWnd,1);
					break;
					}

				case IDCANCEL:
					EndDialog(hWnd,-1);
					break;

				case IDC_TL_SHAPE:
					if (!IsDlgButtonChecked(hWnd,IDC_TL_SHAPE)) {
						CheckDlgButton(hWnd,IDC_TL_RESTO,0);
						EnableWindow(GetDlgItem(hWnd,IDC_TL_RESTO),FALSE);
						}
					else 
						EnableWindow(GetDlgItem(hWnd,IDC_TL_RESTO),TRUE);
					break;
				}
			break;

		default:
			return FALSE;
		}
	return TRUE;
	}

BOOL MultiMapMod::GMSetNumPoints(HWND hWnd,BOOL hold) {
	int npx = uvwProy[current_channel]->gm->nx;
	int npy = uvwProy[current_channel]->gm->ny;

	int gizmo = 1;
	if (uvwProy[current_channel]->GetMappingType() == MAP_TL_FACE ||
		uvwProy[current_channel]->GetMappingType() == MAP_TL_BOX ||
		uvwProy[current_channel]->GetMappingType() == MAP_TL_XYZ ||
		uvwProy[current_channel]->GetMappingType() == MAP_TL_UVWDATA)
		gizmo = 0;

	
	int dim[5] = {npx,npy,1,1,gizmo};
	int res = DialogBoxParam(
		hInstance,
		MAKEINTRESOURCE(IDC_TL_SETNUMPOINTS),
		ip->GetMAXHWnd(),
		GMNumPointsDlgProc,
		(LPARAM)dim);

	if (res==1 && ( dim[2] || dim[0] != npx || dim[1] != npy ) ) {

		uvwProy[current_channel]->use_old_ffm = FALSE;
	
		if (hold) theHold.Begin();
		Point3 size;
		size.x = uvwProy[current_channel]->GetWidth(0);
		size.y = uvwProy[current_channel]->GetLength(0);
		size.z = uvwProy[current_channel]->GetHeight(0);
		uvwProy[current_channel]->GMSetNumPoints(dim[0],dim[1],dim[2],size,dim[3],0);
		if (hold) theHold.Accept(_T("Set Number FFG Points"));
		LocalDataChanged();
		return TRUE;
		}

	return FALSE;
	}

void MultiMapMod::TextNumPoints() {
	if ( hwnd_free ) {
		TSTR buf;
		buf.printf(_T("%d×%d"), uvwProy[current_channel]->gm->nx, uvwProy[current_channel]->gm->ny);
		SetWindowText(GetDlgItem(hwnd_free,IDC_TLF_NUMPOINTS),buf);
		}
	}

void MultiMapMod::GroupsMenuUpdateList() 
{
	TSTR max;

	SendDlgItemMessage(hwnd_main,IDC_TL_GROUPS_MENU,LB_RESETCONTENT,0,0);

	for (int i=0; i<uvwProy.Count(); i++) {
		TSTR name(uvwProy[i]->descCanal);
		SendDlgItemMessage(	hwnd_main,IDC_TL_GROUPS_MENU,LB_ADDSTRING,0,(LPARAM)(TCHAR*)name);
		SendDlgItemMessage(	hwnd_main,IDC_TL_GROUPS_MENU,LB_SETITEMHEIGHT,i,MAKELPARAM(16,0));
		if (name.Length()>max.Length()) 
			max = name;
	}

	SendDlgItemMessage(	hwnd_main,IDC_TL_GROUPS_MENU,LB_SETCURSEL,0,(LPARAM)current_channel);
	SendDlgItemMessage( hwnd_main,IDC_TL_GROUPS_MENU,LB_SETTOPINDEX,top_group,0);
/*	
	HWND hCont = GetDlgItem(hwnd_main,IDC_TL_GROUPS_MENU);
	HDC hdc = GetDC(hCont);
	SIZE size;
	GetTextExtentPoint(hdc,max,max.Length(),&size);
	SendDlgItemMessage(hwnd_main,IDC_TL_GROUPS_MENU,LB_SETHORIZONTALEXTENT,size.cx+12,0);
	ReleaseDC(hCont,hdc);*/			
}

#define MAPCH_SIZE	20
void MultiMapMod::GroupsMenuDrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	HWND hListBox = groups_menu_handler->GetHwnd();
	DRAWITEMSTRUCT *di = (DRAWITEMSTRUCT*)lpDrawItemStruct;
	int index = di->itemID;
	if (index>=0) {

		HDC hdc = di->hDC;
		BOOL sel = di->itemState;
		RECT rect = di->rcItem;

		HFONT hFont = CreateFont(14,0,0,0,0,0,0,0,0,0,0,0, VARIABLE_PITCH|FF_SWISS, _T(""));
		HPEN white_pen = CreatePen(PS_SOLID,0,RGB(255,255,255));
		HBRUSH white_brush = CreateSolidBrush(RGB(255,255,255));
		HPEN gray_pen = CreatePen(PS_SOLID,0,RGB(128,128,128));
		HPEN blue_pen = CreatePen(PS_SOLID,0,RGB(0,0,255));
		HPEN lblue_pen = CreatePen(PS_SOLID,0,RGB(200,200,255));
		HBRUSH lblue_brush = CreateSolidBrush(RGB(200,200,255));

		SelectObject(hdc,hFont);
		SelectObject(hdc,GetStockObject(HOLLOW_BRUSH));

		// Paint the background in the whole rectangle
		if ( index==current_channel ) {
			SelectObject(hdc,lblue_pen);
			SelectObject(hdc,lblue_brush);
			Rectangle(hdc,rect.left,rect.top,rect.right,rect.bottom);
			}
		else {
			SelectObject(hdc,white_pen);
			SelectObject(hdc,white_brush);
			Rectangle(hdc,rect.left,rect.top,rect.right,rect.bottom);
			}

		if ( uvwProy[index]->GetActiveStatus() )
			ImageList_Draw(hBulbIcons,0,hdc, rect.left+2, rect.top+1, ILD_NORMAL);
		else
			ImageList_Draw(hBulbIcons,1,hdc, rect.left+2, rect.top+1, ILD_NORMAL);

		int map_type = uvwProy[index]->GetMappingType();
		ImageList_Draw(hMapTypesIcons,map_type,hdc, rect.left+14, rect.top+1, ILD_NORMAL);

		TSTR name = uvwProy[index]->descCanal;
		SetBkMode(hdc, TRANSPARENT); 
		TextOut(hdc,rect.left+27,rect.top+1,name,name.Length());

		// Paint the background in the whole rectangle
		if ( index==current_channel ) {
			SelectObject(hdc,lblue_pen);
			SelectObject(hdc,lblue_brush);
			Rectangle(hdc,rect.right-MAPCH_SIZE-5,rect.top,rect.right,rect.bottom);
			}
		else {
			SelectObject(hdc,white_pen);
			SelectObject(hdc,white_brush);
			Rectangle(hdc,rect.right-MAPCH_SIZE-5,rect.top,rect.right,rect.bottom);
			}

		TSTR map_ch_txt;
		map_ch_txt.printf(_T("%d"),uvwProy[index]->GetMappingChannel());
		SetBkMode(hdc, TRANSPARENT); 
		TextOut(hdc,rect.right-MAPCH_SIZE,rect.top+1,map_ch_txt,map_ch_txt.Length());

		SelectObject(hdc,gray_pen);
		MoveToEx(hdc,rect.left,rect.bottom-1,NULL);
		LineTo(hdc,rect.right,rect.bottom-1);

		DeleteObject(hFont);
		DeleteObject(white_pen);
		DeleteObject(white_brush);
		DeleteObject(gray_pen);
		DeleteObject(blue_pen);
		DeleteObject(lblue_pen);
		DeleteObject(lblue_brush);
		}
	}

void MultiMapMod::GroupsMenuForceDrawItem(int group) {
	HWND hCont = GetDlgItem(hwnd_main,IDC_TL_GROUPS_MENU);

	RECT rect;
	SendMessage( hCont, LB_GETITEMRECT, group, (LPARAM)&rect );

	HDC hdc = GetDC(hCont);
	DRAWITEMSTRUCT di;
	di.CtlID = IDC_TL_GROUPS_MENU;
	di.itemID = group;
	di.hDC = hdc;
	di.itemState = TRUE;
	di.rcItem = rect;
	di.itemState = 0;

	SendMessage( hwnd_main, WM_DRAWITEM, IDC_TL_GROUPS_MENU, (LPARAM)&di);
	
	ReleaseDC(hCont,hdc);
	}


void MultiMapMod::RenameChannel(TCHAR *buf) {
	uvwProy[current_channel]->descCanal = TSTR(buf);
	GroupsMenuForceDrawItem(current_channel);
	}

void MultiMapMod::SetActive(int index) {
	BOOL status = uvwProy[index]->GetActiveStatus();
	uvwProy[index]->SetActiveStatus(!status);
	GroupsMenuForceDrawItem(index);

	LocalDataChanged();
	ip->RedrawViews(ip->GetTime());
	}

void MultiMapMod::DrawMappingTypeItem(LPDRAWITEMSTRUCT lpDrawItemStruct) {
	DRAWITEMSTRUCT *di = (DRAWITEMSTRUCT*)lpDrawItemStruct;
	int index = di->itemID;
	if (index>=0) {
		HDC hdc = di->hDC;
		RECT rect = di->rcItem;
		TSTR map_type;
		SetBkMode(hdc, TRANSPARENT); 

		HPEN sel_pen = CreatePen(PS_SOLID,0,GetSysColor(COLOR_HIGHLIGHT));
		HBRUSH sel_brush = CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT));
		HPEN white_pen = CreatePen(PS_SOLID,0,GetSysColor(COLOR_INFOBK));
		HBRUSH white_brush = CreateSolidBrush(GetSysColor(COLOR_INFOBK));
		if ( di->itemState&ODS_SELECTED  ) {
			SelectObject(hdc,sel_pen);
			SelectObject(hdc,sel_brush);
			Rectangle(hdc,rect.left,rect.top,rect.right,rect.bottom);
			SetTextColor(hdc,GetSysColor(COLOR_HIGHLIGHTTEXT));
			}
		else {
			SelectObject(hdc,white_pen);
			SelectObject(hdc,white_brush);
			Rectangle(hdc,rect.left,rect.top,rect.right,rect.bottom);
			SetTextColor(hdc,GetSysColor(COLOR_BTNTEXT));
			}

		switch ( index ) {
			case 0:		// IDS_MAPTYPE_PLANAR:
				ImageList_Draw(hMapTypesIcons,0,hdc, rect.left+2, rect.top+1, ILD_NORMAL);
				map_type.printf(GetString(IDS_MAPTYPE_PLANAR));
				TextOut(hdc,rect.left+14,rect.top+1,map_type,map_type.Length());
				break;
			case 1:		// IDS_MAPTYPE_CYL:
				ImageList_Draw(hMapTypesIcons,1,hdc, rect.left+2, rect.top+1, ILD_NORMAL);
				map_type.printf(GetString(IDS_MAPTYPE_CYL));
				TextOut(hdc,rect.left+14,rect.top+1,map_type,map_type.Length());
				break;
			case 2:		// IDS_MAPTYPE_CYLCAP:
				ImageList_Draw(hMapTypesIcons,2,hdc, rect.left+2, rect.top+1, ILD_NORMAL);
				map_type.printf(GetString(IDS_MAPTYPE_CYLCAP));
				TextOut(hdc,rect.left+14,rect.top+1,map_type,map_type.Length());
				break;
			case 3:		// IDS_MAPTYPE_SPHERE:
				ImageList_Draw(hMapTypesIcons,3,hdc, rect.left+2, rect.top+1, ILD_NORMAL);
				map_type.printf(GetString(IDS_MAPTYPE_SPHERE));
				TextOut(hdc,rect.left+14,rect.top+1,map_type,map_type.Length());
				break;
			case 4:		// IDS_MAPTYPE_SHRINK:
				ImageList_Draw(hMapTypesIcons,4,hdc, rect.left+2, rect.top+1, ILD_NORMAL);
				map_type.printf(GetString(IDS_MAPTYPE_SHRINK));
				TextOut(hdc,rect.left+14,rect.top+1,map_type,map_type.Length());
				break;
			case 5:		// IDS_MAPTYPE_BOX:
				ImageList_Draw(hMapTypesIcons,5,hdc, rect.left+2, rect.top+1, ILD_NORMAL);
				map_type.printf(GetString(IDS_MAPTYPE_BOX));
				TextOut(hdc,rect.left+14,rect.top+1,map_type,map_type.Length());
				break;
			case 6:		// IDS_MAPTYPE_FACE:
				ImageList_Draw(hMapTypesIcons,6,hdc, rect.left+2, rect.top+1, ILD_NORMAL);
				map_type.printf(GetString(IDS_MAPTYPE_FACE));
				TextOut(hdc,rect.left+14,rect.top+1,map_type,map_type.Length());
				break;
			case 7:		// IDS_MAPTYPE_XYZ:
				ImageList_Draw(hMapTypesIcons,7,hdc, rect.left+2, rect.top+1, ILD_NORMAL);
				map_type.printf(GetString(IDS_MAPTYPE_XYZ));
				TextOut(hdc,rect.left+14,rect.top+1,map_type,map_type.Length());
				break;
			case 8:		// IDS_MAPTYPE_SPLINE:
				ImageList_Draw(hMapTypesIcons,8,hdc, rect.left+2, rect.top+1, ILD_NORMAL);
				map_type.printf(GetString(IDS_MAPTYPE_SPLINE));
				TextOut(hdc,rect.left+14,rect.top+1,map_type,map_type.Length());
				break;
			case 9:		// IDS_MAPTYPE_FFM:
				ImageList_Draw(hMapTypesIcons,9,hdc, rect.left+2, rect.top+1, ILD_NORMAL);
				map_type.printf(GetString(IDS_MAPTYPE_FFM));
				TextOut(hdc,rect.left+14,rect.top+1,map_type,map_type.Length());
				break;
			case 10:	// IDS_MAPTYPE_PELT:
				ImageList_Draw(hMapTypesIcons,10,hdc, rect.left+2, rect.top+1, ILD_NORMAL);
				map_type.printf(GetString(IDS_MAPTYPE_PELT));
				TextOut(hdc,rect.left+14,rect.top+1,map_type,map_type.Length());
				break;
			case 11:	// IDS_MAPTYPE_FRAME:
				ImageList_Draw(hMapTypesIcons,11,hdc, rect.left+2, rect.top+1, ILD_NORMAL);
				map_type.printf(GetString(IDS_MAPTYPE_FRAME));
				TextOut(hdc,rect.left+14,rect.top+1,map_type,map_type.Length());
				break;
			case 12:	// IDS_MAPTYPE_DATA:
				ImageList_Draw(hMapTypesIcons,12,hdc, rect.left+2, rect.top+1, ILD_NORMAL);
				map_type.printf(GetString(IDS_MAPTYPE_DATA));
				TextOut(hdc,rect.left+14,rect.top+1,map_type,map_type.Length());
				break;
			}
		}
	}

int MultiMapMod::GetUniqueNameID() {
	int max_id = uvwProy.Count();
	for ( int i=0; i<uvwProy.Count(); i++ ) {
		TSTR group_name( uvwProy[i]->descCanal );
		int pos = strcspn( group_name.data(), "Group ");
		if ( pos==0 ) {
			group_name = group_name.remove(0,6);
			int num = atoi( group_name.data() );
			if ( num > max_id )
				max_id = num;
			}
		}
	return max_id+1;
	}