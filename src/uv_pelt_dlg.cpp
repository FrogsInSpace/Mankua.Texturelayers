#include "mods.h"		
#include "texutil.h"
#include "bmmlib.h"
#include "simpobj.h"
#include "simpmod.h"
#include "decomp.h"
#include "mapping.h"
#include "uvwgroup.h"
#include "texlay_mc_data.h"
#include "buildver.h"
#include "surf_api.h"
#include "meshadj.h"
#include "io.h"
#include <fcntl.h>
#include "load_cb.h"
#include "natural_box.h"
#include "uv_pelt_dlg.h"
#include "texlay.h"

#include "3dsmaxport.h"

#define ID_TOOL_MOVE					0x0100
#define ID_TOOL_ROTATE					0x0105
#define ID_TOOL_SCALE					0x010A
#define ID_TOOL_ADDPT					0x0110
#define ID_TOOL_DELPT					0x0120
#define ID_TOOL_PAN						0x0130
#define ID_TOOL_ZOOM					0x0140
#define ID_TOOL_ZOOMEXT					0x0150
#define ID_TOOL_SYMM					0x0160
#define ID_TOOL_SYMMPAIR				0x0165
#define ID_TOOL_ROTP90					0x0170
#define ID_TOOL_ROTN90					0x0180
#define ID_TOOL_ROT180					0x0190

#define TOP_HEIGHT		32
#define BOT_HEIGHT		1

#define TO_RGB(COLOR) RGB(int(COLOR.r*255.0f),int(COLOR.g*255.0f),int(COLOR.b*255.0f))

#define MAIN_SIZE	400.0f

HWND				UVPeltDlg::hWnd					= NULL;
HWND				UVPeltDlg::hView				= NULL;

int					UVPeltDlg::mode					= ID_TOOL_MOVE;
int					UVPeltDlg::move_axis			= 0;
int					UVPeltDlg::scale_axis			= 0;
IOffScreenBuf*		UVPeltDlg::iBuf					= NULL;
ICustToolbar*		UVPeltDlg::iToolBar				= NULL;
ICustButton*		UVPeltDlg::iAddPoint			= NULL;
ICustButton*		UVPeltDlg::iDelPoint			= NULL;
ICustButton*		UVPeltDlg::iMove				= NULL;
ICustButton*		UVPeltDlg::iRotate				= NULL;
ICustButton*		UVPeltDlg::iScale				= NULL;
ICustButton*		UVPeltDlg::iPan					= NULL;
ICustButton*		UVPeltDlg::iZoom				= NULL;
ICustButton*		UVPeltDlg::iZoomExt				= NULL;
ICustButton*		UVPeltDlg::iSymmetry			= NULL;
ICustButton*		UVPeltDlg::iSymmPair			= NULL;
ICustButton*		UVPeltDlg::iRotP90				= NULL;
ICustButton*		UVPeltDlg::iRotN90				= NULL;
ICustButton*		UVPeltDlg::iRot180				= NULL;
MouseManager		UVPeltDlg::mouseMan;
PeltMoveMode*		UVPeltDlg::moveMode				= NULL;
PeltRotateMode*		UVPeltDlg::rotateMode			= NULL;
PeltScaleMode*		UVPeltDlg::scaleMode			= NULL;
PeltAddPointMode*	UVPeltDlg::addPointMode			= NULL;
PeltDelPointMode*	UVPeltDlg::delPointMode			= NULL;
PeltPanMode*		UVPeltDlg::panMode				= NULL;
PeltZoomMode*		UVPeltDlg::zoomMode				= NULL;
PeltMiddleMode*		UVPeltDlg::middleMode			= NULL;
PeltRightMode*		UVPeltDlg::rightMode			= NULL;
HCURSOR				UVPeltDlg::selCur				= NULL;
HCURSOR				UVPeltDlg::moveCur				= NULL;
HCURSOR				UVPeltDlg::moveXCur				= NULL;
HCURSOR				UVPeltDlg::moveYCur				= NULL;
HCURSOR				UVPeltDlg::rotateCur			= NULL;
HCURSOR				UVPeltDlg::scaleCur				= NULL;
HCURSOR				UVPeltDlg::scaleXCur			= NULL;
HCURSOR				UVPeltDlg::scaleYCur			= NULL;
HCURSOR				UVPeltDlg::addPtCur				= NULL;
HCURSOR				UVPeltDlg::delPtCur				= NULL;
HCURSOR				UVPeltDlg::zoomCur				= NULL;
HCURSOR				UVPeltDlg::panCur				= NULL;

BOOL				UVPeltDlg::viewValid			= FALSE;

static HIMAGELIST hToolImages = NULL;

class DeleteResources {
	public:
		~DeleteResources() {
			if (hToolImages) ImageList_Destroy(hToolImages);			
			}
	};
static DeleteResources	theDelete;

class UVWPeltEvent : public EventUser {
public:
	UVPeltDlg *dlg;

	void Notify() {
		if (dlg) {
			dlg->mod->uvwProy[dlg->mod->current_channel]->PeltDelPoints();
			dlg->InvalidateView();
			}
		}
	void SetDlg(UVPeltDlg *d) {
		dlg=d;
		}
	};
UVWPeltEvent delEvent;

//Win32 : BOOL CALLBACK UVPeltDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
INT_PTR CALLBACK UVPeltDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	//Win32 : UVPeltDlg *dlg = (UVPeltDlg*)GetWindowLong(hWnd,GWL_USERDATA);
	UVPeltDlg *dlg = DLGetWindowLongPtr<UVPeltDlg*>(hWnd);

	switch (message) {
		case WM_INITDIALOG:
			dlg = (UVPeltDlg*)lParam;
			
			//Win32 : SetWindowLong(hWnd,GWL_USERDATA,lParam);
			DLSetWindowLongPtr(hWnd, lParam);

			SendMessage(hWnd, WM_SETICON, ICON_SMALL, GetClassLongPtr(dlg->mod->ip->GetMAXHWnd(), GCLP_HICONSM)); // mjm - 3.12.99

			CenterWindow(hWnd,GetParent(hWnd));
			SetCursor(LoadCursor(NULL,IDC_ARROW));

			delEvent.SetDlg(dlg);
			dlg->mod->ip->RegisterDeleteUser(&delEvent);

			dlg->mod->ip->RegisterDlgWnd(hWnd);
			dlg->SetupDlg(hWnd);
			return 1;
			break;


		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case 0:
					break;
				case ID_TOOL_MOVE:
				case ID_TOOL_ROTATE:
				case ID_TOOL_SCALE:
				case ID_TOOL_ADDPT:
				case ID_TOOL_DELPT:
				case ID_TOOL_PAN:
				case ID_TOOL_ZOOM:
					dlg->move_axis = dlg->iMove->GetCurFlyOff();
					if (dlg->move_axis == 0)
						dlg->iMove->SetTooltip(TRUE,GetString(IDS_TLP_MOVE));
					else if (dlg->move_axis == 1)
						dlg->iMove->SetTooltip(TRUE,GetString(IDS_TLP_MOVE_X));
					else if (dlg->move_axis == 2)
						dlg->iMove->SetTooltip(TRUE,GetString(IDS_TLP_MOVE_Y));

					dlg->scale_axis = dlg->iScale->GetCurFlyOff();
					if (dlg->scale_axis == 0)
						dlg->iScale->SetTooltip(TRUE,GetString(IDS_TLP_SCALE));
					else if (dlg->scale_axis == 1)
						dlg->iScale->SetTooltip(TRUE,GetString(IDS_TLP_SCALE_X));
					else if (dlg->scale_axis == 2)
						dlg->iScale->SetTooltip(TRUE,GetString(IDS_TLP_SCALE_Y));

					dlg->SetMode( LOWORD(wParam) );
					break;

				case ID_TOOL_ZOOMEXT:
					dlg->ZoomExtents();
					break;

				case ID_TOOL_SYMM: {
					int symm = dlg->iSymmetry->GetCurFlyOff();
					if ( dlg->iSymmetry->IsChecked() )
						dlg->mod->uvwProy[dlg->mod->current_channel]->PeltBuildUpSymmetryTables(symm,TRUE);
					else
						dlg->mod->uvwProy[dlg->mod->current_channel]->symmetry_axis = -1;
					}
					break;

				case ID_TOOL_ROTP90:
					dlg->mod->uvwProy[dlg->mod->current_channel]->PeltStepRotatePoints(-HALFPI);
					break;

				case ID_TOOL_ROTN90:
					dlg->mod->uvwProy[dlg->mod->current_channel]->PeltStepRotatePoints(HALFPI);
					break;

				case ID_TOOL_ROT180:
					dlg->mod->uvwProy[dlg->mod->current_channel]->PeltStepRotatePoints(PI);
					break;

				case ID_TOOL_SYMMPAIR:
					dlg->mod->uvwProy[dlg->mod->current_channel]->PeltMakeSymmetryPair();
					break;

				}
			return 1;

		case WM_PAINT: 
			break;

		case WM_SIZE:
			dlg->SizeDlg();
			break;

		case WM_CLOSE: {
			HWND maxHwnd = dlg->mod->ip->GetMAXHWnd();
			SetFocus(maxHwnd);
			DestroyWindow(hWnd);
			}
			break;

		case WM_DESTROY:
			dlg->EndUVPeltDlg();
			dlg->mod->ip->UnRegisterDeleteUser(&delEvent);
			break;
		}
	return 0;
	}

static LRESULT CALLBACK UVPeltViewProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) 
{
	//Win32 : UVPeltDlg *dlg = (UVPeltDlg*)GetWindowLong(hWnd,GWL_USERDATA);
	UVPeltDlg *dlg = DLGetWindowLongPtr<UVPeltDlg*>(hWnd);

	switch (msg) {
		case WM_CREATE:
			break;

		case WM_SIZE:
			if (dlg) {
				dlg->iBuf->Resize();
				dlg->InvalidateView();
				}
			break;

		case WM_PAINT:
			if (dlg) dlg->PaintView();
			break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONUP:		
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONDBLCLK:
		case WM_MBUTTONUP:		
		case WM_MOUSEMOVE:
			return dlg->mouseMan.MouseWinProc(hWnd,msg,wParam,lParam);
			break;

		default:
			return DefWindowProc(hWnd,msg,wParam,lParam);
		}	
	return 0;
	}


UVPeltDlg::UVPeltDlg( MultiMapMod *m ) {
	selecting = FALSE;
	mod = m;
	hWnd = NULL;
	hView = NULL;

	selCur		= mod->ip->GetSysCursor(SYSCUR_SELECT);
	moveCur		= mod->ip->GetSysCursor(SYSCUR_MOVE);
	rotateCur	= mod->ip->GetSysCursor(SYSCUR_ROTATE);
	scaleCur	= mod->ip->GetSysCursor(SYSCUR_USCALE);
	if (moveXCur == NULL)
		moveXCur	 = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_MOVE_X));
	if (moveYCur == NULL)
		moveYCur	 = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_MOVE_Y));
	if (scaleXCur == NULL)
		scaleXCur	 = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_SCALE_X));
	if (scaleYCur == NULL)
		scaleYCur	 = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_SCALE_Y));
	if (zoomCur == NULL)
		zoomCur		= LoadCursor(hInstance, MAKEINTRESOURCE(IDC_ZOOM));
	if (panCur == NULL)
		panCur		= LoadCursor(hInstance, MAKEINTRESOURCE(IDC_PAN));
	if (addPtCur == NULL)
		addPtCur	 = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_ADD_POINT));
	if (delPtCur == NULL)
		delPtCur	 = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_DEL_POINT));
	}

void UVPeltDlg::StartUVPeltDlg() {
	RegisterClasses();
	HWND floaterHwnd = CreateDialogParam(
						hInstance,
						MAKEINTRESOURCE(IDD_PELT_DLG),
						mod->ip->GetMAXHWnd(),
						UVPeltDlgProc,
						(LPARAM)this);

	SetFocus(floaterHwnd);

	InvalidateView();
	}

void UVPeltDlg::EndUVPeltDlg() {
	DestroyIOffScreenBuf(iBuf); 
	iBuf = NULL;
	
	ReleaseICustToolbar(iToolBar);			iToolBar		= NULL;
	ReleaseICustButton(iAddPoint);			iAddPoint		= NULL;
	ReleaseICustButton(iDelPoint);			iDelPoint		= NULL;
	ReleaseICustButton(iMove);				iMove			= NULL;
	ReleaseICustButton(iRotate);			iRotate			= NULL;
	ReleaseICustButton(iScale);				iScale			= NULL;
	ReleaseICustButton(iPan);				iPan			= NULL;
	ReleaseICustButton(iZoom);				iZoom			= NULL;
	ReleaseICustButton(iZoomExt);			iZoomExt		= NULL;

	delete moveMode;		moveMode		= NULL;
	delete addPointMode;	addPointMode	= NULL;
	delete delPointMode;	delPointMode	= NULL;
	delete panMode;			panMode			= NULL;
	delete zoomMode;		zoomMode		= NULL;
	delete middleMode;		middleMode		= NULL;
	delete rightMode;		rightMode		= NULL;

	mouseMan.SetMouseProc(NULL,LEFT_BUTTON,0);
	mouseMan.SetMouseProc(NULL,MIDDLE_BUTTON,0);
	mouseMan.SetMouseProc(NULL,RIGHT_BUTTON,0);

	mod->ip->UnRegisterDlgWnd(hWnd);

	mod->DestroyUVPeltDlg();
	}

void UVPeltDlg::InvalidateView() {
	viewValid = FALSE;
	if (hView) {
		InvalidateRect(hView,NULL,FALSE);
		PaintView();
		}
	int sym = mod->uvwProy[mod->current_channel]->symmetry_axis;
	if ( sym==-1 ) {
		iSymmetry->SetCurFlyOff(0);
		iSymmetry->SetCheck(FALSE);
		}
	else {
		iSymmetry->SetCurFlyOff(sym);
		iSymmetry->SetCheck(TRUE);
		}
	}

#define BUT_SIZE_X		23
#define BUT_SIZE_Y		23
#define IMG_SIZE_X		16
#define IMG_SIZE_Y		16

void UVPeltDlg::SetupDlg( HWND hWnd ) {

	this->hWnd = hWnd;
	hView = GetDlgItem(hWnd,IDC_PELT_VIEW);

	//Win32 : SetWindowLong(hView,GWL_USERDATA,(LONG)this);
	DLSetWindowLongPtr(hView, this);

	iBuf = CreateIOffScreenBuf(hView);
	viewValid    = FALSE;

	iToolBar = GetICustToolbar(GetDlgItem(hWnd,IDC_PELT_TOOLBAR));
	iToolBar->SetImage(hToolImages);

	// Move Points CLines button
	iToolBar->AddTool(ToolButtonItem(CTB_CHECKBUTTON,2, 2, 2, 2, IMG_SIZE_X, IMG_SIZE_Y, BUT_SIZE_X, BUT_SIZE_Y, ID_TOOL_MOVE));
	iMove = iToolBar->GetICustButton(ID_TOOL_MOVE);
	iMove->SetTooltip(TRUE,GetString(IDS_TLP_MOVE));
	iMove->SetHighlightColor(GREEN_WASH);
	FlyOffData fdata_m[] = {  { 2,  2,  2,  2},	{3, 3, 3, 3},	{4, 4, 4, 4}};
	iMove->SetFlyOff(3,fdata_m,mod->ip->GetFlyOffTime(),move_axis,FLY_DOWN);
	iMove->SetCurFlyOff(move_axis);

	// Rotate Points CLines button
	iToolBar->AddTool(ToolButtonItem(CTB_CHECKBUTTON,14, 14, 14, 14, IMG_SIZE_X, IMG_SIZE_Y, BUT_SIZE_X, BUT_SIZE_Y, ID_TOOL_ROTATE));
	iRotate = iToolBar->GetICustButton(ID_TOOL_ROTATE);
	iRotate->SetTooltip(TRUE,GetString(IDS_TLP_ROTATE));
	iRotate->SetHighlightColor(GREEN_WASH);

	// Scale Points CLines button
	iToolBar->AddTool(ToolButtonItem(CTB_CHECKBUTTON,15, 15, 15, 15, IMG_SIZE_X, IMG_SIZE_Y, BUT_SIZE_X, BUT_SIZE_Y, ID_TOOL_SCALE));
	iScale = iToolBar->GetICustButton(ID_TOOL_SCALE);
	iScale->SetTooltip(TRUE,GetString(IDS_TLP_SCALE));
	iScale->SetHighlightColor(GREEN_WASH);
	FlyOffData fdata_sc[] = {  { 15,  15,  15,  15},	{16, 16, 16, 16},	{17, 17, 17, 17}};
	iScale->SetFlyOff(3,fdata_sc,mod->ip->GetFlyOffTime(),scale_axis,FLY_DOWN);
	iScale->SetCurFlyOff(scale_axis);

	iToolBar->AddTool(ToolButtonItem(CTB_SEPARATOR,0, 0, 0, 0, 10, 10, 10, 10, 0));

	iToolBar->AddTool(ToolButtonItem(CTB_CHECKBUTTON, 0, 0, 0, 0, IMG_SIZE_X, IMG_SIZE_Y, BUT_SIZE_X, BUT_SIZE_Y, ID_TOOL_ADDPT));
	iAddPoint = iToolBar->GetICustButton(ID_TOOL_ADDPT);
	iAddPoint->SetTooltip(TRUE,GetString(IDS_TLP_ADDPT));
	iAddPoint->SetHighlightColor(GREEN_WASH);

	iToolBar->AddTool(ToolButtonItem(CTB_CHECKBUTTON, 1, 1, 1, 1, IMG_SIZE_X, IMG_SIZE_Y, BUT_SIZE_X, BUT_SIZE_Y, ID_TOOL_DELPT));
	iDelPoint = iToolBar->GetICustButton(ID_TOOL_DELPT);
	iDelPoint->SetTooltip(TRUE,GetString(IDS_TLP_DELPT));
	iDelPoint->SetHighlightColor(GREEN_WASH);

	iToolBar->AddTool(ToolButtonItem(CTB_SEPARATOR,0, 0, 0, 0, 10, 10, 10, 10, 0));

	// Pan
	iToolBar->AddTool(ToolButtonItem(CTB_CHECKBUTTON,5, 5, 5, 5, IMG_SIZE_X, IMG_SIZE_Y, BUT_SIZE_X, BUT_SIZE_Y, ID_TOOL_PAN));
	iPan = iToolBar->GetICustButton(ID_TOOL_PAN);
	iPan->SetTooltip(TRUE,GetString(IDS_TLP_PAN));
	iPan->SetHighlightColor(GREEN_WASH);

	// Zoom
	iToolBar->AddTool(ToolButtonItem(CTB_CHECKBUTTON,6, 6, 6, 6, IMG_SIZE_X, IMG_SIZE_Y, BUT_SIZE_X, BUT_SIZE_Y, ID_TOOL_ZOOM));
	iZoom = iToolBar->GetICustButton(ID_TOOL_ZOOM);
	iZoom->SetTooltip(TRUE,GetString(IDS_TLP_ZOOM));
	iZoom->SetHighlightColor(GREEN_WASH);

	// Zoom Extents
	iToolBar->AddTool(ToolButtonItem(CTB_PUSHBUTTON,7, 7, 7, 7, IMG_SIZE_X, IMG_SIZE_Y, BUT_SIZE_X, BUT_SIZE_Y, ID_TOOL_ZOOMEXT));
	iZoomExt = iToolBar->GetICustButton(ID_TOOL_ZOOMEXT);
	iZoomExt->SetTooltip(TRUE,GetString(IDS_TLP_ZOOMEXT));
	iZoomExt->SetHighlightColor(GREEN_WASH);

	iToolBar->AddTool(ToolButtonItem(CTB_SEPARATOR,0, 0, 0, 0, 10, 10, 10, 10, 0));

	int sym = mod->uvwProy[mod->current_channel]->symmetry_axis;
	iToolBar->AddTool(ToolButtonItem(CTB_CHECKBUTTON,0, 0, 0, 0, IMG_SIZE_X, IMG_SIZE_Y, BUT_SIZE_X, BUT_SIZE_Y, ID_TOOL_SYMM));
	iSymmetry = iToolBar->GetICustButton(ID_TOOL_SYMM);
	iSymmetry->SetTooltip(TRUE,GetString(IDS_PELT_SYMM));
	iSymmetry->SetHighlightColor(GREEN_WASH);
	FlyOffData fdata_s[] = {  { 8,  8,  8,  8},	{9, 9, 9, 9} };
	iSymmetry->SetFlyOff(2,fdata_s,mod->ip->GetFlyOffTime(),0,FLY_DOWN);
	if ( sym==-1 ) {
		iSymmetry->SetCurFlyOff(0);
		iSymmetry->SetCheck(FALSE);
		}
	else {
		iSymmetry->SetCurFlyOff(sym);
		iSymmetry->SetCheck(TRUE);
		}

	// Symmetry Pair
	iToolBar->AddTool(ToolButtonItem(CTB_PUSHBUTTON,13, 13, 13, 13, IMG_SIZE_X, IMG_SIZE_Y, BUT_SIZE_X, BUT_SIZE_Y, ID_TOOL_SYMMPAIR));
	iSymmPair = iToolBar->GetICustButton(ID_TOOL_SYMMPAIR);
	iSymmPair->SetTooltip(TRUE,GetString(IDS_TLP_SYMMPAIR));
	iSymmPair->SetHighlightColor(GREEN_WASH);

	iToolBar->AddTool(ToolButtonItem(CTB_SEPARATOR,0, 0, 0, 0, 10, 10, 10, 10, 0));

	iToolBar->AddTool(ToolButtonItem(CTB_PUSHBUTTON,10, 10, 10, 10, IMG_SIZE_X, IMG_SIZE_Y, BUT_SIZE_X, BUT_SIZE_Y, ID_TOOL_ROTP90));
	iRotP90 = iToolBar->GetICustButton(ID_TOOL_ROTP90);
	iRotP90->SetTooltip(TRUE,GetString(IDS_PELT_ROTP90));
	iRotP90->SetHighlightColor(GREEN_WASH);

	iToolBar->AddTool(ToolButtonItem(CTB_PUSHBUTTON,11, 11, 11, 11, IMG_SIZE_X, IMG_SIZE_Y, BUT_SIZE_X, BUT_SIZE_Y, ID_TOOL_ROTN90));
	iRotN90 = iToolBar->GetICustButton(ID_TOOL_ROTN90);
	iRotN90->SetTooltip(TRUE,GetString(IDS_PELT_ROTN90));
	iRotN90->SetHighlightColor(GREEN_WASH);

	iToolBar->AddTool(ToolButtonItem(CTB_PUSHBUTTON,12, 12, 12, 12, IMG_SIZE_X, IMG_SIZE_Y, BUT_SIZE_X, BUT_SIZE_Y, ID_TOOL_ROT180));
	iRot180 = iToolBar->GetICustButton(ID_TOOL_ROT180);
	iRot180->SetTooltip(TRUE,GetString(IDS_PELT_ROT180));
	iRot180->SetHighlightColor(GREEN_WASH);

	moveMode		= new PeltMoveMode(this);
	rotateMode		= new PeltRotateMode(this);
	scaleMode		= new PeltScaleMode(this);
	addPointMode	= new PeltAddPointMode(this);
	delPointMode	= new PeltDelPointMode(this);
	panMode			= new PeltPanMode(this);
	zoomMode		= new PeltZoomMode(this);
	middleMode		= new PeltMiddleMode(this);
	rightMode		= new PeltRightMode(this);

	mouseMan.SetMouseProc(rightMode,RIGHT_BUTTON,1);
	mouseMan.SetMouseProc(middleMode,MIDDLE_BUTTON,2);

	SizeDlg();
	SetMode(mode);
	}

void UVPeltDlg::SetMode(int m) {
	switch (mode) {
		case ID_TOOL_MOVE:
			iMove->SetCheck(FALSE);
			break;
		case ID_TOOL_ROTATE:
			iRotate->SetCheck(FALSE);
			break;
		case ID_TOOL_SCALE:
			iScale->SetCheck(FALSE);
			break;
		case ID_TOOL_ADDPT:
			iAddPoint->SetCheck(FALSE);
			break;
		case ID_TOOL_DELPT:
			iDelPoint->SetCheck(FALSE);
			break;
		case ID_TOOL_PAN:    
			iPan->SetCheck(FALSE);    
			break;
		case ID_TOOL_ZOOM:
			iZoom->SetCheck(FALSE);    
			break;
		}

	mode = m;

	switch (mode) {
		case ID_TOOL_MOVE:   
			iMove->SetCheck(TRUE);  
			mouseMan.SetMouseProc(moveMode, LEFT_BUTTON);
			break;
		case ID_TOOL_ROTATE:   
			iRotate->SetCheck(TRUE);  
			mouseMan.SetMouseProc(rotateMode, LEFT_BUTTON);
			break;
		case ID_TOOL_SCALE:   
			iScale->SetCheck(TRUE);  
			mouseMan.SetMouseProc(scaleMode, LEFT_BUTTON);
			break;
		case ID_TOOL_ADDPT:   
			iAddPoint->SetCheck(TRUE);  
			mouseMan.SetMouseProc(addPointMode, LEFT_BUTTON);
			break;
		case ID_TOOL_DELPT:   
			iDelPoint->SetCheck(TRUE);  
			mouseMan.SetMouseProc(delPointMode, LEFT_BUTTON);
			break;
		case ID_TOOL_PAN:    
			iPan->SetCheck(TRUE);   
			mouseMan.SetMouseProc(panMode, LEFT_BUTTON);
			break;
		case ID_TOOL_ZOOM:    
			iZoom->SetCheck(TRUE);   
			mouseMan.SetMouseProc(zoomMode, LEFT_BUTTON);
			break;
		}
	}

void UVPeltDlg::RegisterClasses() {
	if (!hToolImages) {
		HBITMAP hBitmap, hMask;	
		hToolImages = ImageList_Create(16, 16, TRUE, 4, 0);
		hBitmap = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_PELT));
		hMask   = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_PELT_MASK));
		ImageList_Add(hToolImages,hBitmap,hMask);
		DeleteObject(hBitmap);
		DeleteObject(hMask);
		}

	static BOOL registered = FALSE;
	if (!registered) {
		registered = TRUE;
		WNDCLASS  wc;
		wc.style         = 0;
		wc.hInstance     = hInstance;
		wc.hIcon         = NULL;
		wc.hCursor       = NULL;
		wc.hbrBackground = NULL;
		wc.lpszMenuName  = NULL;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.lpfnWndProc   = UVPeltViewProc;
		wc.lpszClassName = GetString(IDS_PELT_VIEW_CLASS);
		RegisterClass(&wc);
		}
	}

void UVPeltDlg::PaintView() {
	UVWProyector * uvwp = mod->uvwProy[mod->current_channel];

	Color back_color(1,1,1);
	iBuf->SetBkColor( TO_RGB( back_color ) );

	PAINTSTRUCT		ps;
	BeginPaint(hView,&ps);
	EndPaint(hView,&ps);

	HPEN black_pen = CreatePen(PS_SOLID,0,RGB(0,0,0));
	HPEN blue_pen = CreatePen(PS_SOLID,0,RGB(0,0,255));
	HPEN red_pen = CreatePen(PS_SOLID,0,RGB(255,0,0));
	HPEN yellow_pen = CreatePen(PS_SOLID,0,RGB(255,255,0));
	HPEN dotted_pen = CreatePen(PS_DOT,0,RGB(0,0,0));
	HBRUSH blue_brush = CreateSolidBrush(RGB(0,0,255));
	HBRUSH yellow_brush = CreateSolidBrush(RGB(255,255,0));
	HBRUSH red_brush = CreateSolidBrush(RGB(255,0,0));

	if (!viewValid) {
		viewValid = TRUE;
		iBuf->Erase();
		HDC hdc = iBuf->GetDC();

		// Paint the mesh
		for ( int i_m=0; i_m<uvwp->masses.Count(); i_m++ ) {
			Point2 m0_pos = uvwp->masses[i_m]->f_pos;
			int num_springs = uvwp->masses[i_m]->springs.Count();
			for ( int i_s=0; i_s<num_springs; i_s++ ) {
				if ( uvwp->masses[i_m]->springs[i_s].attached_to_mass ) {
					int m_id = uvwp->masses[i_m]->springs[i_s].attach_id;
					Point2 m1_pos = uvwp->masses[m_id]->f_pos;

					IPoint2 v_m0 = PeltToView( m0_pos );	
					IPoint2 v_m1 = PeltToView( m1_pos );	

					MoveToEx(hdc, v_m0.x, v_m0.y, NULL);
					LineTo(hdc, v_m1.x, v_m1.y);
					}
				}
			}

		// Paint the lines
		SelectObject(hdc,yellow_pen);
		SelectObject(hdc,yellow_brush);
		for ( int i_p=0; i_p<uvwp->frame_segments.Count(); i_p++ ) {
			int j_p = ( i_p + 1 ) % uvwp->frame_segments.Count();

			FrameSegments * fsi = uvwp->frame_segments[i_p];
			FrameSegments * fsj = uvwp->frame_segments[j_p];
			
			Point2 pi = fsi->start_point;
			Point2 pj = fsj->start_point;

			IPoint2 vpi = PeltToView(pi);
			IPoint2 vpj = PeltToView(pj);

			MoveToEx(hdc, vpi.x, vpi.y, NULL);
			LineTo(hdc, vpj.x, vpj.y);

			for ( int i_bb=1; i_bb<fsi->border_vert.Count(); i_bb++ ) {
				Point2 p = pi + fsi->position[i_bb] * (pj-pi);
	
				IPoint2 vp = PeltToView(p);
				Ellipse( hdc, (int)vp.x-2, (int)vp.y-2, (int)vp.x+2, (int)vp.y+2 );
				}
			}

		// Paint the points
		for ( int i_p=0; i_p<uvwp->frame_segments.Count(); i_p++ ) {
			Point2 p_pt = uvwp->frame_segments[i_p]->start_point;
			IPoint2 v_pt = PeltToView( p_pt );

			if ( uvwp->frame_segments[i_p]->selected ) {
				SelectObject(hdc,red_pen);
				SelectObject(hdc,red_brush);
				}
			else {
				SelectObject(hdc,blue_pen);
				SelectObject(hdc,blue_brush);
				}
			
			Ellipse( hdc, (int)v_pt.x-3, (int)v_pt.y-3, (int)v_pt.x+3, (int)v_pt.y+3 );
			}

		// Selection box
		if ( selecting ) {
			SelectObject(hdc,dotted_pen);
			SelectObject(hdc,GetStockObject(HOLLOW_BRUSH));
			Rectangle(	hdc, lm.x, lm.y, om.x, om.y );
			}

		SelectObject(hdc,black_pen);
		SelectObject(hdc,GetStockObject(HOLLOW_BRUSH));
		Rectangle(	hdc, 0, 0, view_w, view_h );
		}

	DeleteObject(black_pen);
	DeleteObject(blue_pen);
	DeleteObject(red_pen);
	DeleteObject(yellow_pen);
	DeleteObject(dotted_pen);
	DeleteObject(blue_brush);
	DeleteObject(red_brush);
	DeleteObject(yellow_brush);
		
	iBuf->Blit();
	}

void UVPeltDlg::SizeDlg( ) {
	Rect rect;
	GetClientRect(hWnd,&rect);

	int view_left = 2;
	int view_width = rect.w() - 5;
	int view_top = TOP_HEIGHT + 2;
	int view_height = rect.h() - TOP_HEIGHT - BOT_HEIGHT - 3;

	MoveWindow( GetDlgItem(hWnd,IDC_PELT_VIEW), view_left, view_top, view_width, view_height, FALSE );

	Rect view_rect;
	GetClientRect(hView,&view_rect);
	view_w = view_rect.right - view_rect.left;
	view_h = view_rect.bottom - view_rect.top;

	InvalidateView();
	}

IPoint2 UVPeltDlg::PeltToView( Point2 pt_p ) {
	UVWProyector * uvwp = mod->uvwProy[mod->current_channel];

	IPoint2 pt_v;

	pt_v.x = float(view_w)/2.0f + float(uvwp->xscroll) + uvwp->zoom * pt_p.x * float(view_w) / MAIN_SIZE;
	pt_v.y = float(view_h)/2.0f + float(uvwp->yscroll) - uvwp->zoom * pt_p.y * float(view_w) / MAIN_SIZE;

	return pt_v;
	}

Point2 UVPeltDlg::ViewToPelt( IPoint2 pt_v ) {
	UVWProyector * uvwp = mod->uvwProy[mod->current_channel];

	Point2 pt_p;

	pt_p.x = MAIN_SIZE * ( pt_v.x - float(view_w)/2.0f - float(uvwp->xscroll) ) / ( uvwp->zoom * float(view_w) );
	pt_p.y = MAIN_SIZE * (-pt_v.y + float(view_h)/2.0f + float(uvwp->yscroll) ) / ( uvwp->zoom * float(view_w) );

	return pt_p;
	}

Box2D UVPeltDlg::ViewBoxToPeltBox( Rect rect ) {
	IPoint2 i_min( rect.left, rect.top );
	IPoint2 i_max( rect.right, rect.bottom );

	Point2 f_min = ViewToPelt(i_min);
	Point2 f_max = ViewToPelt(i_max);

	Box2D box;
	box += f_min;
	box += f_max;
	
	return box;
	}

void UVPeltDlg::ZoomExtents() {
	UVWProyector * uvwp = mod->uvwProy[mod->current_channel];

	Box2D box;
	for ( int i_fs=0; i_fs<uvwp->frame_segments.Count(); i_fs++ ) {
		Point2 p = uvwp->frame_segments[i_fs]->start_point;
		box += p;
		}
	for ( int i_m=0; i_m<uvwp->masses.Count(); i_m++ ) {
		Point2 p = uvwp->masses[i_m]->f_pos;
		box += p;
		}

	float len_x = box.max.x - box.min.x;
	float len_y = box.max.y - box.min.y;
	float zoom_x = 0.9f * MAIN_SIZE / len_x;
	float zoom_y = 0.9f * ( MAIN_SIZE * view_h ) / ( len_y * view_w );
	uvwp->zoom = zoom_x<zoom_y?zoom_x:zoom_y;

	Point2 cen = 0.5f * ( box.min + box.max );
	IPoint2 mid( view_w/2, view_h/2 );

	uvwp->xscroll = float(mid.x) - float(view_w)/2.0f - uvwp->zoom * cen.x * float(view_w) / MAIN_SIZE;
	uvwp->yscroll = float(mid.y) - float(view_h)/2.0f + uvwp->zoom * cen.y * float(view_w) / MAIN_SIZE;

	InvalidateView();
	}

// MOUSE MODES

int PeltSelectMode::proc(HWND hWnd, int msg, int point, int flags, IPoint2 m) {
	UVWProyector * uvwproy = dlg->mod->uvwProy[dlg->mod->current_channel];
	switch (msg) {
		case MOUSE_POINT:
			if (point==0) { // First click
				
				region   = FALSE;
				toggle   = flags&MOUSE_CTRL;
				subtract = flags&MOUSE_ALT;
				
				// Hit test
				Tab<int> hits;
				Rect rect;
				rect.left = m.x-3;		rect.top = m.y-3;
				rect.right = m.x+3;		rect.bottom = m.y+3;
				Box2D box = dlg->ViewBoxToPeltBox( rect );

				if ( !toggle && !subtract && uvwproy->PeltHitTest(box,hits,TRUE) ) { 
					return subproc(hWnd,msg,point,flags,m);
					}
				else { 
					if ( uvwproy->PeltHitTest(box,hits,subtract) ) {
						if (!toggle && !subtract) uvwproy->PeltClearSelect();
						uvwproy->PeltSelect(hits,toggle,subtract,FALSE);
						dlg->InvalidateView();

						return subproc(hWnd,msg,point,flags,m);
						}
					else {
						dlg->selecting = TRUE;
						dlg->lm = dlg->om = m;
						region = TRUE;
						lm = om = m;
						}
					}
				}
			else { // Second Click
				if ( region ) {
					dlg->selecting = FALSE;

					Rect rect;
					rect.left   = om.x;		rect.top    = om.y;
					rect.right  = m.x;		rect.bottom = m.y;
					rect.Rectify();					
					Box2D box = dlg->ViewBoxToPeltBox( rect );

					// Hit test
					Tab<int> hits;
					if (!toggle && !subtract) uvwproy->PeltClearSelect();
					if ( uvwproy->PeltHitTest(box,hits,subtract) ) {
						uvwproy->PeltSelect(hits,FALSE,subtract,TRUE);
						}
					else if ( !toggle && !subtract ) {
						uvwproy->LocalDataChanged();
						}
					dlg->InvalidateView();
					}
				else
					return subproc(hWnd,msg,point,flags,m);
				}
			break;

		case MOUSE_MOVE:
			SetCursor(GetXFormCur());
			if ( region ) {
				lm = m;
				dlg->lm = m;
				dlg->InvalidateView();
				}
			else
				return subproc(hWnd,msg,point,flags,m);
			break;

		case MOUSE_FREEMOVE: {
			// Hit test
			Tab<int> hits;
			Rect rect;
			rect.left = m.x-3;		rect.top = m.y-3;
			rect.right = m.x+3;		rect.bottom = m.y+3;
			Box2D box = dlg->ViewBoxToPeltBox( rect );

			if (uvwproy->PeltHitTest(box,hits,FALSE))
				if ( 1 )//dlg->mod->psel[hits[0]])
					SetCursor(GetXFormCur());
				else
					SetCursor(dlg->selCur);
			else 
				SetCursor(LoadCursor(NULL, IDC_ARROW));
			}
			break;

		case MOUSE_ABORT:
			dlg->selecting = FALSE;
			if (region) 
				InvalidateRect(hWnd,NULL,FALSE);
			else 
				return subproc(hWnd,msg,point,flags,m);
			dlg->InvalidateView();
			break;
		}
	return 1;
	}

// Move Mode
int PeltMoveMode::subproc(HWND hWnd, int msg, int point, int flags, IPoint2 m) {
	UVWProyector * uvwproy = dlg->mod->uvwProy[dlg->mod->current_channel];
	switch (msg) {
		case MOUSE_POINT:
			if ( point == 0 ) {	// First Click
				theHold.SuperBegin();
				theHold.Begin();
				om = m;
				}
			else { // Second Click
				theHold.Accept( GetString(IDS_PELT_MOVEPOINTS) );
				theHold.SuperAccept( GetString(IDS_PELT_MOVEPOINTS) );
				}
			break;

		case MOUSE_MOVE: {
			theHold.Restore();
			Point2 delta = dlg->ViewToPelt(m) - dlg->ViewToPelt(om);	
			uvwproy->PeltMovePoints( delta, dlg->move_axis );
		//	dlg->InvalidateView();
			}
			break;		

		case MOUSE_ABORT:
			theHold.Cancel();
			theHold.SuperCancel();
			uvwproy->LocalDataChanged();
			break;
		}
	return 1;
	}

HCURSOR PeltMoveMode::GetXFormCur() {
	if ( dlg->move_axis == 1 )
		return dlg->moveXCur;
	if ( dlg->move_axis == 2 )
		return dlg->moveYCur;
	return dlg->moveCur;
	}

// Rotate Mode
#define ROT_FACT	DegToRad(0.5f)
int PeltRotateMode::subproc(HWND hWnd, int msg, int point, int flags, IPoint2 m) {
	UVWProyector * uvwproy = dlg->mod->uvwProy[dlg->mod->current_channel];
	switch (msg) {
		case MOUSE_POINT:
			if ( point == 0 ) {	// First Click
				theHold.SuperBegin();
				theHold.Begin();
				om = m;
				}
			else { // Second Click
				theHold.Accept( GetString(IDS_PELT_ROTATEPOINTS));
				theHold.SuperAccept( GetString(IDS_PELT_ROTATEPOINTS));
				}
			break;

		case MOUSE_MOVE: {
			theHold.Restore();
			uvwproy->PeltRotatePoints( float(m.y-om.y) * ROT_FACT );
		//	dlg->InvalidateView();
			}
			break;		

		case MOUSE_ABORT:
			theHold.Cancel();
			theHold.SuperCancel();
			uvwproy->LocalDataChanged();
			break;
		}
	return 1;
	}

HCURSOR PeltRotateMode::GetXFormCur() {
	return dlg->rotateCur;
	}

// Scale Mode
int PeltScaleMode::subproc(HWND hWnd, int msg, int point, int flags, IPoint2 m) {
	UVWProyector * uvwproy = dlg->mod->uvwProy[dlg->mod->current_channel];
	switch (msg) {
		case MOUSE_POINT:
			if ( point == 0 ) {	// First Click
				theHold.SuperBegin();
				theHold.Begin();
				om = m;
				}
			else { // Second Click
				theHold.Accept( GetString(IDS_PELT_SCALEPOINTS) );
				theHold.SuperAccept( GetString(IDS_PELT_SCALEPOINTS) );
				}
			break;

		case MOUSE_MOVE: {
			theHold.Restore();
			Point2 delta = dlg->ViewToPelt(m) - dlg->ViewToPelt(om);
			float scale = -float(m.y-om.y);
			if ( dlg->scale_axis==1 )
				scale = float(m.x-om.x);
			uvwproy->PeltScalePoints( scale, dlg->scale_axis );
			}
			break;		

		case MOUSE_ABORT:
			theHold.Cancel();
			theHold.SuperCancel();
			uvwproy->LocalDataChanged();
			break;
		}
	return 1;
	}

HCURSOR PeltScaleMode::GetXFormCur() {
	if ( dlg->scale_axis == 1 )
		return dlg->scaleXCur;
	if ( dlg->scale_axis == 2 )
		return dlg->scaleYCur;
	return dlg->scaleCur;
	}


int PeltAddPointMode::proc(HWND hWnd, int msg, int point, int flags, IPoint2 m) {
	UVWProyector * uvwproy = dlg->mod->uvwProy[dlg->mod->current_channel];
	switch (msg) {
		case MOUSE_POINT: 
			if ( point==0 ) {
				Rect rect;
				rect.left = m.x-2;		rect.top = m.y-2;
				rect.right = m.x+2;		rect.bottom = m.y+2;
				Box2D box = dlg->ViewBoxToPeltBox( rect );

				int fs,bv;
				if ( uvwproy->PeltHitSubPoint(box,fs,bv) ) {
					uvwproy->PeltAddPoint(fs,bv);
				//	dlg->InvalidateView();
					}
				}
			break;			

		case MOUSE_MOVE:
			break;

		case MOUSE_FREEMOVE: {
			Rect rect;
			rect.left = m.x-2;		rect.top = m.y-2;
			rect.right = m.x+2;		rect.bottom = m.y+2;
			Box2D box = dlg->ViewBoxToPeltBox( rect );

			int fs,bv;
			if ( uvwproy->PeltHitSubPoint(box,fs,bv) )
				SetCursor( GetXFormCur() );
			else
				SetCursor(LoadCursor(NULL, IDC_ARROW));
			}
			break;

		case MOUSE_ABORT:
			break;
		}
	return 1;
	}

HCURSOR PeltAddPointMode::GetXFormCur() {		
	return dlg->addPtCur;
	}

int PeltDelPointMode::proc(HWND hWnd, int msg, int point, int flags, IPoint2 m) {
	UVWProyector * uvwproy = dlg->mod->uvwProy[dlg->mod->current_channel];
	switch (msg) {
		case MOUSE_POINT:
			if (point==0) { // First click
				Tab<int> hits;
				BOOL subtract = FALSE;
				Rect rect;
				rect.left = m.x-3;		rect.top = m.y-3;
				rect.right = m.x+3;		rect.bottom = m.y+3;
				Box2D box = dlg->ViewBoxToPeltBox( rect );

				if ( uvwproy->PeltHitTest(box,hits,subtract) ) {
					uvwproy->PeltDelPoints(hits[0]);
				//	dlg->InvalidateView();
					}
				}
			break;			

		case MOUSE_MOVE:
			break;

		case MOUSE_FREEMOVE: {
			Tab<int> hits;
			BOOL subtract = FALSE;
			Rect rect;
			rect.left = m.x-3;		rect.top = m.y-3;
			rect.right = m.x+3;		rect.bottom = m.y+3;
			Box2D box = dlg->ViewBoxToPeltBox( rect );

			if ( uvwproy->PeltHitTest(box,hits,subtract) )
				SetCursor( GetXFormCur() );
			else
				SetCursor(LoadCursor(NULL, IDC_ARROW));
			}
			break;

		case MOUSE_ABORT:
			break;
		}
	return 1;
	}

HCURSOR PeltDelPointMode::GetXFormCur() {		
	return dlg->delPtCur;
	}

#define ZOOM_FACT	0.01f

int PeltZoomMode::proc(HWND hWnd, int msg, int point, int flags, IPoint2 m) {
	UVWProyector * uvwproy = dlg->mod->uvwProy[dlg->mod->current_channel];
	switch (msg) {
		case MOUSE_POINT:
			if (point==0) {
				om = m;
				ozoom = uvwproy->zoom;
				oxscroll = uvwproy->xscroll;
				oyscroll = uvwproy->yscroll;

				fixed_point = dlg->ViewToPelt( m );
				}
			break;

		case MOUSE_MOVE: {
			IPoint2 delta = om-m;
			float z;
			if (delta.y<0)
				 z = (1.0f/(1.0f-ZOOM_FACT*delta.y));
			else 
				z = (1.0f+ZOOM_FACT*delta.y);

			float new_zoom = ozoom * z;

			int new_xscroll = float(om.x) - float(dlg->view_w)/2.0f - new_zoom * fixed_point.x * float(dlg->view_w) / MAIN_SIZE;
			int new_yscroll = float(om.y) - float(dlg->view_h)/2.0f + new_zoom * fixed_point.y * float(dlg->view_w) / MAIN_SIZE;

			uvwproy->zoom = new_zoom;
			uvwproy->xscroll = new_xscroll;
			uvwproy->yscroll = new_yscroll;
			dlg->InvalidateView();
			SetCursor(GetXFormCur());
			}
			break;

		case MOUSE_ABORT:
			uvwproy->zoom = ozoom;
			uvwproy->xscroll = oxscroll;
			uvwproy->yscroll = oyscroll;
			dlg->InvalidateView();
			break;

		case MOUSE_FREEMOVE:
			SetCursor(GetXFormCur());
			break;		
		}
	return 1;
	}

HCURSOR PeltZoomMode::GetXFormCur() {		
	return dlg->zoomCur;
	}

int PeltPanMode::proc(HWND hWnd, int msg, int point, int flags, IPoint2 m) {
	UVWProyector * uvwproy = dlg->mod->uvwProy[dlg->mod->current_channel];
	switch (msg) {
		case MOUSE_POINT:
			if (point==0) {
				om = m;
				oxscroll = uvwproy->xscroll;
				oyscroll = uvwproy->yscroll;
				}
			break;

		case MOUSE_MOVE: {
			IPoint2 delta = m-om;
			uvwproy->xscroll = oxscroll + delta.x;
			uvwproy->yscroll = oyscroll + delta.y;
			dlg->InvalidateView();
			SetCursor(GetPanCursor());
			}
			break;

		case MOUSE_ABORT:
			uvwproy->xscroll = oxscroll;
			uvwproy->yscroll = oyscroll;
			dlg->InvalidateView();
			break;

		case MOUSE_FREEMOVE:
			SetCursor(GetPanCursor());
			break;		
		}
	return 1;
	}

HCURSOR PeltPanMode::GetXFormCur() {		
	return dlg->panCur;
	}

int PeltMiddleMode::proc(HWND hWnd, int msg, int point, int flags, IPoint2 m) {
	UVWProyector * uvwproy = dlg->mod->uvwProy[dlg->mod->current_channel];
	static int modeType = 0;
	switch (msg) {
		case MOUSE_POINT:
			if (point==0) {
				BOOL ctrl = flags & MOUSE_CTRL;
				BOOL alt = flags & MOUSE_ALT;

				om = m;
				if ( alt && ctrl ) {
					modeType = ID_TOOL_ZOOM;
					ozoom = uvwproy->zoom;

					fixed_point = dlg->ViewToPelt( m );
					}
				else {
					modeType = ID_TOOL_PAN;
					}

				oxscroll = uvwproy->xscroll;
				oyscroll = uvwproy->yscroll;

				SetCursor(GetPanCursor());
				}
			break;

		case MOUSE_MOVE: {
			if ( modeType == ID_TOOL_PAN ) {
				IPoint2 delta = m-om;
				uvwproy->xscroll = oxscroll + delta.x;
				uvwproy->yscroll = oyscroll + delta.y;

				SetCursor(GetPanCursor());
				}
			else {
				IPoint2 delta = om-m;
				float z;
				if (delta.y<0)
					 z = (1.0f/(1.0f-ZOOM_FACT*delta.y));
				else 
					z = (1.0f+ZOOM_FACT*delta.y);

				float new_zoom = ozoom * z;

				int new_xscroll = float(om.x) - float(dlg->view_w)/2.0f - new_zoom * fixed_point.x * float(dlg->view_w) / MAIN_SIZE;
				int new_yscroll = float(om.y) - float(dlg->view_h)/2.0f + new_zoom * fixed_point.y * float(dlg->view_w) / MAIN_SIZE;

				uvwproy->zoom = new_zoom;
				uvwproy->xscroll = new_xscroll;
				uvwproy->yscroll = new_yscroll;

				SetCursor(dlg->zoomCur);
				}
			dlg->InvalidateView();
			}
			break;

		case MOUSE_ABORT:
			if ( modeType == ID_TOOL_ZOOM )
				uvwproy->zoom = ozoom;
			uvwproy->xscroll = oxscroll;
			uvwproy->yscroll = oyscroll;
			dlg->InvalidateView();
			break;
		}
	return 1;
	}

int PeltRightMode::proc(HWND hWnd, int msg, int point, int flags, IPoint2 m)
	{
	switch (msg) {
		case MOUSE_POINT:
		case MOUSE_PROPCLICK:
			break;
		}
	return 1;
	}

