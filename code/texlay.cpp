/**********************************************************************
 *<
	FILE: texlay.cpp

	DESCRIPTION: A compositor texture map.

	CREATED BY: Rolf Berteig

	MODIFIED BY: Diego Castaño

	HISTORY:	Original Source (composit.cpp) by Rolf Berteig

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#include <vector>
#include "mtlhdr.h"
#include "modsres.h"
#include "stdmat.h"
#include <bmmlib.h>
#include "texutil.h"
#include "modstack.h"
#include "iparamm.h"
#include "mapping.h"
#include "uvwgroup.h"
#include "texlay_mc_data.h"
#include "meshdata.h"
#include "common.h"
#include "texlay.h"
#include "3dsmaxport.h"

#define NSUBTEXS 6
#define IMAGE_W 12
#define IMAGE_H 13 

#define MAXNUMCHANS	99

extern HINSTANCE hInstance;


#define CONFIGNAME _T("texlay.cod")

class TextureLayers;
class TextureLayersDlg;

class TextureLayersDlg: public ParamDlg {
	public:
		HWND hwmedit;	 	// window handle of the materials editor dialog
		IMtlParams *ip;
		TextureLayers *theTex;	 
		HWND hPanel; 		// Rollup panel
		HWND hScroll;
		BOOL valid;
		ICustButton *iBut[NSUBTEXS];
		ICustButton *iBack[NSUBTEXS];
		ICustButton *iModAtt[NSUBTEXS];
		ISpinnerControl *iSpin[NSUBTEXS];
		TexDADMgr dadMgr;

		TextureLayersDlg(HWND hwMtlEdit, IMtlParams *imp, TextureLayers *m); 
		~TextureLayersDlg();

		BOOL PanelProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );		
		void VScroll(int code, short int cpos );
		void LoadDialog(BOOL draw);  // stuff params into dialog
		void ReloadDialog();
		void UpdateMtlDisplay();		
		void UpdateSubTexNames();
		void ActivateDlg(BOOL onOff);
		void Invalidate();
		void Destroy(HWND hWnd);
		void SetNumMaps();
		void SetUVWAttenuation(int canal);
		void DragAndDrop(int ifrom, int ito);

		// methods inherited from ParamDlg:
		Class_ID ClassID() {return TEXTURELAYERS_CID;  }
		void SetThing(ReferenceTarget *m);
		ReferenceTarget* GetThing() {return (ReferenceTarget *)theTex;}
		void DeleteThis() { delete this;  }	
		void SetTime(TimeValue t);
		int FindSubTexFromHWND(HWND hw);
	};

//#ifdef ALPS_PROTECTED
////------------------------------------------------------------------------
//// DummyParamDlg
////------------------------------------------------------------------------
//class DummyParamDlg: public ParamDlg
//{
//	Class_ID		cid;
//	ReferenceTarget*rt;
//public:
//	DummyParamDlg(Class_ID&_cid)
//	{
//		cid = _cid;
//		rt	= 0;
//	}
//
//	Class_ID ClassID()					{ return cid; }
//	void SetThing(ReferenceTarget *m)	{ rt = m;}
//	ReferenceTarget* GetThing()			{ return rt; }
//	void SetTime(TimeValue t)			{}
//	void ReloadDialog()					{}
//	void DeleteThis()					{ delete this; }
//	void ActivateDlg(BOOL onOff)		{}
//};
//
////Win32 : static BOOL CALLBACK DummyDlgProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
//static INT_PTR CALLBACK DummyDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) 
//{
//	return TRUE;
//}
//#endif

class TextureLayers: public MultiTex 
{ 
	public:			

//#ifdef ALPS_PROTECTED
//
//		bool auth_ok;
//
//#endif

		Tab<Texmap*> subTex;

		Tab<BOOL> mapOn;

		Tab<int> back;

		
		Tab <int> mod_group;	// The mapping channel to which this
								// texture is applied to is defined by the mod_group!

		Tab<INode*> surfs;		// A table that stores the different surfaces, so we can
								// turn on modifiers once render is done...

		static std::vector<MeshExtraData*> mesh_datas;

		Tab<int> canales;		// Una relacion de los canales que tiene cada uno de los objetos.

		// This are the tables for UVW attenuation cubics.

		Tab <float> atCentre;
		Tab <float> atStart;
		Tab <float> atOffset;
		Tab <float> atV1;
		Tab <float> atV2;
		Tab <int>   atNormal;
		BitArray modat;

		TimeValue curtime;

		// This plugin needs to be Thread Safe, so...
		CRITICAL_SECTION csect;

		Interval ivalid;
		int offset;				//
		int rollScroll;
		TextureLayersDlg *paramDlg;
	
		TextureLayers();
		~TextureLayers();
		ParamDlg* CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp);
		void ClampOffset();
		void Update(TimeValue t, Interval& valid);
		void Reset();
		Interval Validity(TimeValue t) { Interval v; Update(t,v); return ivalid;}
		void NotifyChanged();		
		void SetNumSubTexmaps(int n) { SetNumMaps(n); }
		void SetNumMaps(int n);

		ULONG LocalRequirements(int subMtlNum) { 
			return MTLREQ_UV;
			}

		void LocalMappingsRequired(int subMtlNum, BitArray &mapreq, BitArray &bumpreq);

		// Evaluate the color of map for the context.
		AColor MaxEvalColor(ShadeContext& sc);
		AColor EvalColor(ShadeContext& sc);
		// For Bump mapping, need a perturbation to apply to a normal.
		// Leave it up to the Texmap to determine how to do this.
		Point3 MaxEvalNormalPerturb(ShadeContext& sc);		
		Point3 EvalNormalPerturb(ShadeContext& sc);		

		float GetUVWAttenuation(int i,float v);

		// Methods to access texture maps of material
		int NumSubTexmaps() {return subTex.Count();}
		Texmap* GetSubTexmap(int i) {return subTex[i];}		
		void SetSubTexmap(int i, Texmap *m);
		TSTR GetSubTexmapSlotName(int i);		

		Class_ID ClassID() {return TEXTURELAYERS_CID;}
		SClass_ID SuperClassID() {return TEXMAP_CLASS_ID;}
		void GetClassName(TSTR& s) {s=GetString(IDS_DC_MULTICOMP);}
		void DeleteThis() {delete this;}

		int NumSubs() {return subTex.Count();}
		Animatable* SubAnim(int i) {return subTex[i];}
		TSTR SubAnimName(int i);
		int SubNumToRefNum(int subNum) {return subNum;}

		// From ref
 		int NumRefs() {return subTex.Count();}
		RefTargetHandle GetReference(int i) {return subTex[i];}
		void SetReference(int i, RefTargetHandle rtarg) {subTex[i] = (Texmap*)rtarg;}

#ifndef MAX_RELEASE_R9
		RefTargetHandle Clone(RemapDir& remap = NoRemap());
#else
		RefTargetHandle Clone(RemapDir& remap = DefaultRemapDir());
#endif

		RefResult NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, 
		   PartID& partID, RefMessage message );

		// IO
		IOResult Save(ISave *isave);
		IOResult Load(ILoad *iload);

		// This method looks for the layers modifier ...
		Modifier* GetModifier(INode* node, Class_ID modCID);
		void SetData(INode* node, ShadeContext &sc, int ch, Class_ID modCID);

		int RenderBegin(TimeValue t, ULONG flags=0);
		int RenderEnd(TimeValue t);
	};

std::vector<MeshExtraData*> TextureLayers::mesh_datas;

static HIMAGELIST hImageBack = NULL;
static HIMAGELIST hImageAtt = NULL;
static HIMAGELIST hAboutImage = NULL;

static void LoadImages()
{
	if (!hImageBack) {
		HBITMAP hBitmap, hMask;
		hImageBack = ImageList_Create(IMAGE_W, IMAGE_H, TRUE, 3, 3);
		hBitmap = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BACKS));
		hMask = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BACKS_MASK));
		ImageList_Add(hImageBack, hBitmap, hMask);
		DeleteObject(hBitmap);
		DeleteObject(hMask);
	}
	if (!hImageAtt) {
		HBITMAP hBitmap, hMask;
		hImageAtt = ImageList_Create(14, 14, ILC_COLOR8|ILC_MASK, 3, 0);
		hBitmap     = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_TEXLAY_ATT));
		hMask       = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_TEXLAY_ATT_MASK));
		ImageList_Add(hImageAtt,hBitmap,hMask);
		DeleteObject(hBitmap);
		DeleteObject(hMask);
	}
	if (!hAboutImage) {
		HBITMAP hBitmap, hMask;
		hAboutImage = ImageList_Create(16, 16, ILC_COLOR8|ILC_MASK, 3, 0);
		hBitmap     = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_16BUTS));
		hMask       = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_16BUTS_MASK));
		ImageList_Add(hAboutImage,hBitmap,hMask);
		DeleteObject(hBitmap);
		DeleteObject(hMask);
	}
}	

class TextureLayersClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading) {return new TextureLayers;}
	const TCHAR *	ClassName() {return GetString(IDS_DC_MULTICOMP);}
	SClass_ID		SuperClassID() {return TEXMAP_CLASS_ID;}
	Class_ID 		ClassID() {return TEXTURELAYERS_CID;}
	const TCHAR* 	Category() {return TEXMAP_CAT_COMP;}
	
	// Hardwired name, used by MAX Script as unique identifier
	const TCHAR*	InternalName()				{ return _T("TextureLayersComposite"); }
	HINSTANCE		HInstance()					{ return hInstance; }
	};
static TextureLayersClassDesc compCD;
ClassDesc2* GetTextureLayersDesc() {return &compCD;}

//Win32 : static BOOL CALLBACK PanelDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) 
static INT_PTR CALLBACK PanelDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) 
	{
	//Win32 : TextureLayersDlg *theDlg = (TextureLayersDlg*)GetWindowLong(hWnd,GWL_USERDATA);
	TextureLayersDlg *theDlg = DLGetWindowLongPtr<TextureLayersDlg*>(hWnd);

	if (msg==WM_INITDIALOG) {
		theDlg = (TextureLayersDlg*)lParam;
		theDlg->hPanel = hWnd;
		//Win32 : SetWindowLong(hWnd,GWL_USERDATA,lParam);
		DLSetWindowLongPtr(hWnd, lParam);
		}	
	if (theDlg) return theDlg->PanelProc(hWnd,msg,wParam,lParam);
	else return FALSE;
	}

static BOOL CALLBACK AuthDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) 
	{
	return FALSE;
	}

int TextureLayersDlg::FindSubTexFromHWND(HWND hw) {
	for (int i=0; i<6; i++) {
		if (hw == iBut[i]->GetHwnd()) return i;
		}	
	return -1;
	}

void TextureLayersDlg::DragAndDrop(int ifrom, int ito) {
	theTex->CopySubTexmap(hPanel,ifrom+theTex->offset, ito+theTex->offset);
	theTex->NotifyChanged();
	}


TextureLayersDlg::TextureLayersDlg(HWND hwMtlEdit, IMtlParams *imp, TextureLayers *m) 
	{
	dadMgr.Init(this);
	hwmedit  = hwMtlEdit;
	ip       = imp;
	hPanel   = NULL;
	theTex   = m;
	valid    = FALSE;
	for (int i=0; i<NSUBTEXS; i++) iBut[i] = NULL;
	for (int i=0; i<NSUBTEXS; i++) iBack[i] = NULL;
	for (int i=0; i<NSUBTEXS; i++) iSpin[i] = NULL;
	for (int i=0; i<NSUBTEXS; i++) iModAtt[i] = NULL;

	hPanel   = ip->AddRollupPage( 
		hInstance,
		MAKEINTRESOURCE(IDD_TEXTURELAYERSMAP),
		PanelDlgProc, 
		GetString(IDS_DC_TLPARAMS),
		(LPARAM)this);	
	
	}

void TextureLayersDlg::Destroy(HWND hWnd) {
	for (int i=0; i<NSUBTEXS; i++) {
		ReleaseICustButton(iBut[i]);
		ReleaseICustButton(iBack[i]);
		ReleaseICustButton(iModAtt[i]);
		ReleaseISpinner(iSpin[i]);
		iBut[i] = NULL; 
		iBack[i] = NULL;
		iModAtt[i] = NULL;
		iSpin[i] = NULL;
		}
	}

void TextureLayersDlg::Invalidate()
	{
	valid = FALSE;
	Rect rect;
	rect.left = rect.top = 0;
	rect.right = rect.bottom = 10;
	InvalidateRect(hPanel,&rect,FALSE);
	}

void TextureLayersDlg::ReloadDialog() 
	{
	Interval valid;
	theTex->Update(ip->GetTime(), valid);
	LoadDialog(FALSE);
	}

void TextureLayersDlg::SetTime(TimeValue t) 
	{
	Interval valid;	
	theTex->Update(ip->GetTime(), valid);
	LoadDialog(FALSE);
	InvalidateRect(hPanel,NULL,0);	
	}

TextureLayersDlg::~TextureLayersDlg() 
	{
	theTex->paramDlg = NULL;	
	//Win32 : SetWindowLong(hPanel, GWL_USERDATA, NULL);	
	DLSetWindowLongPtr(hPanel, NULL);
	hPanel =  NULL;
	}


void TextureLayersDlg::VScroll(int code, short int cpos ) {
	switch (code) {
		case SB_LINEUP: 	theTex->offset--;		break;
		case SB_LINEDOWN:	theTex->offset++;		break;
		case SB_PAGEUP:		theTex->offset -= 6;	break;
		case SB_PAGEDOWN:	theTex->offset += 6;	break;
		
		case SB_THUMBPOSITION: 
		case SB_THUMBTRACK:
			theTex->offset = cpos;
			break;
		}
	theTex->ClampOffset();
	UpdateSubTexNames();						
	LoadDialog(ip->GetTime());
	}

static int mapIDs[] = {IDC_TL_TEX1,IDC_TL_TEX2,IDC_TL_TEX3,IDC_TL_TEX4,IDC_TL_TEX5,IDC_TL_TEX6};
static int labelIDs[] = {IDC_TL_LABEL1,IDC_TL_LABEL2,IDC_TL_LABEL3,IDC_TL_LABEL4,IDC_TL_LABEL5,IDC_TL_LABEL6};
static int mapOnIDs[] = {IDC_TLON1,IDC_TLON2,IDC_TLON3,IDC_TLON4,IDC_TLON5,IDC_TLON6};
static int chanIDs[] = {IDC_EDIT_CH1,IDC_EDIT_CH2,IDC_EDIT_CH3,IDC_EDIT_CH4,IDC_EDIT_CH5,IDC_EDIT_CH6};
static int spinIDs[] = {IDC_SPIN_CH1,IDC_SPIN_CH2,IDC_SPIN_CH3,IDC_SPIN_CH4,IDC_SPIN_CH5,IDC_SPIN_CH6};
static int matspinIDs[] = {IDC_SPIN_MT1,IDC_SPIN_MT2,IDC_SPIN_MT3,IDC_SPIN_MT4,IDC_SPIN_MT5,IDC_SPIN_MT6};
static int backIDs[] = {IDC_TL_BACK1,IDC_TL_BACK2,IDC_TL_BACK3,IDC_TL_BACK4,IDC_TL_BACK5,IDC_TL_BACK6};
static int modatIDs[] = {IDC_TL_MODAT1,IDC_TL_MODAT2,IDC_TL_MODAT3,IDC_TL_MODAT4,IDC_TL_MODAT5,IDC_TL_MODAT6};

BOOL TextureLayersDlg::PanelProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) 
	{
	TSTR buf;
	int id = LOWORD(wParam);
	int code = HIWORD(wParam);
    switch (msg)    {
		case WM_INITDIALOG: {
			LoadImages();
			hScroll	= GetDlgItem(hWnd,IDC_TL_SCROLL);
			SetScrollRange(hScroll,SB_CTL,0,theTex->NumSubTexmaps()-NSUBTEXS,FALSE);
			SetScrollPos(hScroll,SB_CTL,theTex->offset,TRUE);
			EnableWindow(hScroll,theTex->NumSubTexmaps()>NSUBTEXS);

			for (int i=0; i<NSUBTEXS; i++) {
				iBut[i] = GetICustButton(GetDlgItem(hWnd,IDC_TL_TEX1+i));
				iBut[i]->SetDADMgr(&dadMgr);

				iBack[i] = GetICustButton(GetDlgItem(hWnd,IDC_TL_BACK1+i));
				FlyOffData fod[4] = { 
									{ 0,0,0,0 },
									{ 1,1,1,1 },
									{ 2,2,2,2 },
									{ 3,3,3,3 },
									};
				iBack[i]->SetFlyOff(4, fod, 500, 0, FLY_RIGHT);

				// Here we load up the images for the flyoff...

				iBack[i]->SetImage(hImageBack, 0,0,0,0, IMAGE_W, IMAGE_H);
				iBack[i]->SetDADMgr(&dadMgr);

				iModAtt[i] = GetICustButton(GetDlgItem(hWnd,IDC_TL_MODAT1+i));
				iModAtt[i]->SetType(CBT_CHECK);
				iModAtt[i]->SetText("Mod");
				iModAtt[i]->SetTooltip(TRUE, "Modifier Attenuation");

				iSpin[i] = SetupIntSpinner(hWnd,IDC_SPIN_CH1+i,IDC_EDIT_CH1+i,1,1000,1);
				}

			ICustButton *iTmp;
			iTmp = GetICustButton(GetDlgItem(hWnd,IDC_TLC_ABOUT));
			iTmp->SetImage(hAboutImage, 0, 0, 0, 0, 16, 16);
			iTmp->SetTooltip(TRUE,_T("About Texture Layers Composite"));
			ReleaseICustButton(iTmp);

			return TRUE;
			}
			
		case WM_PAINT:
			if (!valid) {
				valid = TRUE;
				ReloadDialog();
				}
			return FALSE;

		case WM_VSCROLL:
			VScroll(LOWORD(wParam),(short int)HIWORD(wParam));
			break;

		
		case WM_COMMAND: 	
			switch (id) {		
				case IDC_TLC_ABOUT:
					DisplayTexLayAbout();
					break;

				case IDC_TL_TEX1: PostMessage(hwmedit,WM_TEXMAP_BUTTON,theTex->offset,(LPARAM)theTex); break;
				case IDC_TL_TEX2: PostMessage(hwmedit,WM_TEXMAP_BUTTON,theTex->offset+1,(LPARAM)theTex); break;
				case IDC_TL_TEX3: PostMessage(hwmedit,WM_TEXMAP_BUTTON,theTex->offset+2,(LPARAM)theTex); break;
				case IDC_TL_TEX4: PostMessage(hwmedit,WM_TEXMAP_BUTTON,theTex->offset+3,(LPARAM)theTex); break;
				case IDC_TL_TEX5: PostMessage(hwmedit,WM_TEXMAP_BUTTON,theTex->offset+4,(LPARAM)theTex); break;
				case IDC_TL_TEX6: PostMessage(hwmedit,WM_TEXMAP_BUTTON,theTex->offset+5,(LPARAM)theTex); break;

				case IDC_TLON1:							
				case IDC_TLON2:							
				case IDC_TLON3:							
				case IDC_TLON4:							
				case IDC_TLON5:							
				case IDC_TLON6:
					theTex->mapOn[id-IDC_TLON1+theTex->offset] = GetCheckBox(hWnd, id);
					theTex->NotifyChanged();
					break;

				case IDC_TL_MODAT1:
				case IDC_TL_MODAT2:
				case IDC_TL_MODAT3:
				case IDC_TL_MODAT4:
				case IDC_TL_MODAT5:
				case IDC_TL_MODAT6:
					theTex->modat.Set(id-IDC_TL_MODAT1+theTex->offset, iModAtt[id-IDC_TL_MODAT1]->IsChecked() );
					break;

				case IDC_TL_BACK1:
				case IDC_TL_BACK2:
				case IDC_TL_BACK3:
				case IDC_TL_BACK4:
				case IDC_TL_BACK5:
				case IDC_TL_BACK6:
					theTex->back[id-IDC_TL_BACK1+theTex->offset] = iBack[id-IDC_TL_BACK1]->GetCurFlyOff();
					if(theTex->back[id-IDC_TL_BACK1+theTex->offset] == 3)
						SetUVWAttenuation(id-IDC_TL_BACK1+theTex->offset);
					break;
								
				case IDC_TL_SETNUM:
					SetNumMaps();
					break;
				}			
			break;
				
		case CC_SPINNER_CHANGE:
			switch(id) {
				case IDC_SPIN_CH1:
				case IDC_SPIN_CH2:
				case IDC_SPIN_CH3:
				case IDC_SPIN_CH4:
				case IDC_SPIN_CH5:
				case IDC_SPIN_CH6: {
					int map_id = id - IDC_SPIN_CH1 + theTex->offset;
					theTex->mod_group[map_id] = iSpin[id-IDC_SPIN_CH1]->GetIVal();
					break;
					}
				}
			break;


		case WM_DESTROY:
			Destroy(hWnd);
			break;
    	}
	return FALSE;
	}

void TextureLayersDlg::UpdateSubTexNames() 
	{
	for (int i=theTex->offset; i<theTex->subTex.Count(); i++) {
		if (i-theTex->offset>=6) break;

		Texmap *m = theTex->subTex[i];
		TSTR nm;
		if (m) 	nm = m->GetFullName();
		else 	nm = GetString(IDS_TL_NONE);
		TSTR buf;
		buf.printf(_T("%d:"),i+1);
		iBut[i-theTex->offset]->SetText(nm.data());
		iBack[i-theTex->offset]->SetCurFlyOff(theTex->back[i]);
		SetDlgItemText(hPanel, labelIDs[i-theTex->offset], buf);
		SetCheckBox(hPanel, mapOnIDs[i-theTex->offset], theTex->mapOn[i]);

		iModAtt[i-theTex->offset]->SetCheck( theTex->modat[i] );

		iSpin[i-theTex->offset]->SetValue(theTex->mod_group[i],FALSE);
		}
	}


void TextureLayersDlg::LoadDialog(BOOL draw) {	
	if (theTex) {		
		theTex->ClampOffset();
		
		SetScrollRange(hScroll,SB_CTL,0,theTex->subTex.Count()-6,FALSE);
		SetScrollPos(hScroll,SB_CTL,theTex->offset,TRUE);
		EnableWindow(hScroll,theTex->NumSubTexmaps()>6);

		if (theTex->subTex.Count()>6) {
			EnableWindow(GetDlgItem(hPanel,IDC_TL_UP),theTex->offset>0);
			EnableWindow(GetDlgItem(hPanel,IDC_TL_PAGEUP),theTex->offset>0);
			EnableWindow(GetDlgItem(hPanel,IDC_TL_DOWN),theTex->offset+6<theTex->subTex.Count());
			EnableWindow(GetDlgItem(hPanel,IDC_TL_PAGEDOWN),theTex->offset+6<theTex->subTex.Count());
		} else {
			EnableWindow(GetDlgItem(hPanel,IDC_TL_UP),FALSE);
			EnableWindow(GetDlgItem(hPanel,IDC_TL_PAGEUP),FALSE);
			EnableWindow(GetDlgItem(hPanel,IDC_TL_DOWN),FALSE);
			EnableWindow(GetDlgItem(hPanel,IDC_TL_PAGEDOWN),FALSE);
			}

		Interval valid;
		theTex->Update(ip->GetTime(),valid);		
		UpdateSubTexNames();
		TSTR buf;
		buf.printf(_T("%d"),theTex->subTex.Count());
		SetDlgItemText(hPanel,IDC_TL_NUMMAPS,buf);

		ShowWindow(GetDlgItem(hPanel,IDC_STATIC_AUTH),SW_HIDE);

		int i;

		for ( i=0; i<min(theTex->subTex.Count(),6); i++) {
			ShowWindow(GetDlgItem(hPanel,mapIDs[i]),SW_SHOW);
			ShowWindow(GetDlgItem(hPanel,labelIDs[i]),SW_SHOW);
			ShowWindow(GetDlgItem(hPanel,mapOnIDs[i]),SW_SHOW);
			ShowWindow(GetDlgItem(hPanel,chanIDs[i]),SW_SHOW);
			ShowWindow(GetDlgItem(hPanel,spinIDs[i]),SW_SHOW);
			ShowWindow(GetDlgItem(hPanel,matspinIDs[i]),SW_SHOW);
			ShowWindow(GetDlgItem(hPanel,backIDs[i]),SW_SHOW);
			ShowWindow(GetDlgItem(hPanel,modatIDs[i]),SW_SHOW);
			}

		for ( ; i<6; i++ ) {
			ShowWindow(GetDlgItem(hPanel,mapIDs[i]),SW_HIDE);
			ShowWindow(GetDlgItem(hPanel,labelIDs[i]),SW_HIDE);
			ShowWindow(GetDlgItem(hPanel,mapOnIDs[i]),SW_HIDE);
			ShowWindow(GetDlgItem(hPanel,chanIDs[i]),SW_HIDE);
			ShowWindow(GetDlgItem(hPanel,spinIDs[i]),SW_HIDE);
			ShowWindow(GetDlgItem(hPanel,matspinIDs[i]),SW_HIDE);
			ShowWindow(GetDlgItem(hPanel,backIDs[i]),SW_HIDE);
			ShowWindow(GetDlgItem(hPanel,modatIDs[i]),SW_HIDE);
			}
		}
	}

void TextureLayersDlg::SetThing(ReferenceTarget *m) 
	{
	assert (m->ClassID()==TEXTURELAYERS_CID);
	assert (m->SuperClassID()==TEXMAP_CLASS_ID);
	if (theTex) theTex->paramDlg = NULL;
	theTex = (TextureLayers*)m;	
	if (theTex) theTex->paramDlg = this;
	LoadDialog(TRUE);
	}

void TextureLayersDlg::UpdateMtlDisplay() {	
	ip->MtlChanged();  
	}

void TextureLayersDlg::ActivateDlg(BOOL onOff) {	
	}


//WIN32 : static BOOL CALLBACK NumMapsDlgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
static INT_PTR CALLBACK NumMapsDlgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch (msg) {
		case WM_INITDIALOG: {
			ISpinnerControl *spin = 
				SetupIntSpinner(
					hWnd,IDC_TL_NUMMAPSSPIN,IDC_TL_NUMMAPS,
					0,1000,(int)lParam);
			ReleaseISpinner(spin);
			CenterWindow(hWnd,GetParent(hWnd));
			break;
			}

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDOK: {
					ISpinnerControl *spin = 
						GetISpinner(GetDlgItem(hWnd,IDC_TL_NUMMAPSSPIN));
					EndDialog(hWnd,spin->GetIVal());
					ReleaseISpinner(spin);
					break;
					}

				case IDCANCEL:
					EndDialog(hWnd,-1);
					break;
				}
			break;

		default:
			return FALSE;
		}
	return TRUE;
	}

#define BORD_VER	6 
#define BORD_HOR	10

static void DrawCurve(HWND hWnd,HDC hdc)
	{
	float atCentre, atStart, atOffset, atV1, atV2;

	ISpinnerControl *spin;

	spin = GetISpinner(GetDlgItem(hWnd,IDC_ATT_CEN_SPIN));
	atCentre = -spin->GetFVal()/90.0f + 1.0f;
	ReleaseISpinner(spin);	

	spin = GetISpinner(GetDlgItem(hWnd,IDC_ATT_START_SPIN));
	atStart = spin->GetFVal()/90.0f;
	ReleaseISpinner(spin);

	spin = GetISpinner(GetDlgItem(hWnd,IDC_ATT_OFFSET_SPIN));
	atOffset = spin->GetFVal()/90.0f;
	ReleaseISpinner(spin);	

	spin = GetISpinner(GetDlgItem(hWnd,IDC_ATT_V1_SPIN));
	atV1 = spin->GetFVal();
	ReleaseISpinner(spin);	

	spin = GetISpinner(GetDlgItem(hWnd,IDC_ATT_V2_SPIN));
	atV2 = spin->GetFVal();
	ReleaseISpinner(spin);	

	Rect rect, orect;
	GetClientRectP(GetDlgItem(hWnd,IDC_ATT_GRAPH),&rect);
	orect = rect;

	SelectObject(hdc,GetStockObject(NULL_PEN));
	SelectObject(hdc,GetStockObject(WHITE_BRUSH));
	Rectangle(hdc,rect.left,rect.top,rect.right,rect.bottom);	
	SelectObject(hdc,GetStockObject(NULL_BRUSH));
	
	rect.left   += BORD_VER;
	rect.right  -= BORD_VER;
	rect.top    += BORD_HOR;
	rect.bottom -= BORD_HOR;
	
	SelectObject(hdc,CreatePen(PS_DOT,0,GetSysColor(COLOR_BTNFACE)));
	MoveToEx(hdc,orect.left,rect.top,NULL);
	LineTo(hdc,orect.right,rect.top);

	MoveToEx(hdc,orect.left,rect.bottom,NULL);
	LineTo(hdc,orect.right,rect.bottom);

	MoveToEx(hdc,(rect.left+rect.right)/2,orect.top,NULL);
	LineTo(hdc,(rect.left+rect.right)/2,orect.bottom);

	MoveToEx(hdc,rect.left,orect.top,NULL);
	LineTo(hdc,rect.left,orect.bottom);

	MoveToEx(hdc,rect.right,orect.top,NULL);
	LineTo(hdc,rect.right,orect.bottom);

	DeleteObject(SelectObject(hdc,GetStockObject(BLACK_PEN)));

	float p1 = atCentre - atStart - atOffset;
	float p2 = atCentre - atStart;
	float p3 = atCentre + atStart;
	float p4 = atCentre + atStart + atOffset;

	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	int at1 = int(height * (1.0f - atV1)) + BORD_HOR + orect.top;
	int at2 = int(height * (1.0f - atV2)) + BORD_HOR + orect.top;

	int pt1 = int((p1+1.0f)/2.0f * width) + BORD_VER + orect.left;
	int pt2 = int((p2+1.0f)/2.0f * width) + BORD_VER + orect.left;
	int pt3 = int((p3+1.0f)/2.0f * width) + BORD_VER + orect.left;
	int pt4 = int((p4+1.0f)/2.0f * width) + BORD_VER + orect.left;

	if (pt1 > orect.left) {
		MoveToEx(hdc,orect.left,at1,NULL);
		LineTo(hdc,pt1,at1);
		}

	MoveToEx(hdc,pt1,at1,NULL);
	int ptml=pt1,atml=at1;
	for(int i=1; i<=10; i++) {
		float u = float(i)/10.0f;
		float at = atV1 + u*u*(3.0f-2.0f*u)*(atV2-atV1);
		float pt = p1 + (p2-p1)*u;
		int atm = int(height * (1.0f - at)) + BORD_HOR + orect.top;
		int ptm = int((pt+1.0f)/2.0f * width) + BORD_VER + orect.left;
		if (ptml < orect.left && ptm > orect.left) {
			MoveToEx(hdc,orect.left,atml + (orect.left-ptml)*(atm-atml)/(ptm-ptml),NULL);
			}
		if (ptm > orect.left) {
			LineTo(hdc,ptm,atm);
			ptml = ptm;
			atml = atm;
			}
		else {
			ptml = ptm;
			atml = atm;
			}
		}

	MoveToEx(hdc,pt2,at2,NULL);
	if (pt3 < orect.right) {
		LineTo(hdc,pt3,at2);
		}
	else {
		LineTo(hdc,orect.right,at2);
		}

	MoveToEx(hdc,pt3,at2,NULL);
	for( int i=1; i<=10; i++) {
		float u = float(i)/10.0f;
		float at = atV2 - u*u*(3.0f-2.0f*u)*(atV2-atV1);
		float pt = p3 + (p4-p3)*u;
		int atm = int(height * (1.0f - at)) + BORD_HOR + orect.top;
		int ptm = int((pt+1.0f)/2.0f * width) + BORD_VER + orect.left;
		if (ptm < orect.right) {
			LineTo(hdc,ptm,atm);
			}
		else {
			int ato,pto;
			u = float(i-1)/10.0f;
			at = atV2 - u*u*(3.0f-2.0f*u)*(atV2-atV1);
			pt = p3 + (p4-p3)*u;
			ato = int(height * (1.0f - at)) + BORD_HOR + orect.top;
			pto = int((pt+1.0f)/2.0f * width) + BORD_VER + orect.left;
			if (ptm-pto != 0) {
				int atf = ato + (orect.right-pto)*(atm-ato)/(ptm-pto);
				LineTo(hdc,orect.right,atf);
				}
			i=11;
			}
		}

	if (pt4 < orect.right) {
		MoveToEx(hdc,pt4,at1,NULL);
		LineTo(hdc,orect.right,at1);
		}
	}

//Win32 : static BOOL CALLBACK UVWAttDlgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
static INT_PTR CALLBACK UVWAttDlgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static float *attVs;
	switch (msg) {
		case WM_INITDIALOG: {
			attVs = (float*)lParam;
			ISpinnerControl *spin;

			spin = GetISpinner(GetDlgItem(hWnd,IDC_ATT_CEN_SPIN));
			spin->SetLimits(0.0f,180.0f, FALSE);
			spin->SetScale(0.1f);
			spin->LinkToEdit(GetDlgItem(hWnd,IDC_ATT_CEN), EDITTYPE_FLOAT);
			spin->SetValue(attVs[0],FALSE);
			ReleaseISpinner(spin);

			spin = GetISpinner(GetDlgItem(hWnd,IDC_ATT_START_SPIN));
			spin->SetLimits(0.0f,180.0f, FALSE);
			spin->SetScale(0.1f);
			spin->LinkToEdit(GetDlgItem(hWnd,IDC_ATT_START), EDITTYPE_FLOAT);
			spin->SetValue(attVs[1],FALSE);
			ReleaseISpinner(spin);

			spin = GetISpinner(GetDlgItem(hWnd,IDC_ATT_OFFSET_SPIN));
			spin->SetLimits(0.0f,180.0f, FALSE);
			spin->SetScale(0.1f);
			spin->LinkToEdit(GetDlgItem(hWnd,IDC_ATT_OFFSET), EDITTYPE_FLOAT);
			spin->SetValue(attVs[2],FALSE);
			ReleaseISpinner(spin);

			spin = GetISpinner(GetDlgItem(hWnd,IDC_ATT_V1_SPIN));
			spin->SetLimits(0.0f,1.0f, FALSE);
			spin->SetScale(0.001f);
			spin->LinkToEdit(GetDlgItem(hWnd,IDC_ATT_V1), EDITTYPE_FLOAT);
			spin->SetValue(attVs[3],FALSE);
			ReleaseISpinner(spin);

			spin = GetISpinner(GetDlgItem(hWnd,IDC_ATT_V2_SPIN));
			spin->SetLimits(0.0f,1.0f, FALSE);
			spin->SetScale(0.001f);
			spin->LinkToEdit(GetDlgItem(hWnd,IDC_ATT_V2), EDITTYPE_FLOAT);
			spin->SetValue(attVs[4],FALSE);
			ReleaseISpinner(spin);

			CheckRadioButton(hWnd,IDC_VW,IDC_UV,IDC_VW + attVs[5]);


			CenterWindow(hWnd,GetParent(hWnd));
			break;
			}

		case CC_SPINNER_CHANGE:	{
			Rect rect;
			GetClientRectP(GetDlgItem(hWnd,IDC_ATT_GRAPH),&rect);
			InvalidateRect(hWnd,&rect,FALSE);
			break;
			}

		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd,&ps);
			DrawCurve(hWnd,hdc);
			EndPaint(hWnd,&ps);
			}
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDOK: {
					ISpinnerControl *spin;

					spin = GetISpinner(GetDlgItem(hWnd,IDC_ATT_CEN_SPIN));
					attVs[0] = spin->GetFVal();
					ReleaseISpinner(spin);

					spin = GetISpinner(GetDlgItem(hWnd,IDC_ATT_START_SPIN));
					attVs[1] = spin->GetFVal();
					ReleaseISpinner(spin);

					spin = GetISpinner(GetDlgItem(hWnd,IDC_ATT_OFFSET_SPIN));
					attVs[2] = spin->GetFVal();
					ReleaseISpinner(spin);

					spin = GetISpinner(GetDlgItem(hWnd,IDC_ATT_V1_SPIN));
					attVs[3] = spin->GetFVal();
					ReleaseISpinner(spin);

					spin = GetISpinner(GetDlgItem(hWnd,IDC_ATT_V2_SPIN));
					attVs[4] = spin->GetFVal();
					ReleaseISpinner(spin);

					if (IsDlgButtonChecked(hWnd,IDC_WU)) attVs[5] = 1.0f;
					if (IsDlgButtonChecked(hWnd,IDC_VW)) attVs[5] = 0.0f;
					if (IsDlgButtonChecked(hWnd,IDC_UV)) attVs[5] = 2.0f;

					EndDialog(hWnd,1);
					break;
					}

				case IDCANCEL:
					EndDialog(hWnd,0);
					break;

				}
			break;

		default:
			return FALSE;
		}
	return TRUE;
	}
		
void TextureLayersDlg::SetNumMaps()
	{
	int res = DialogBoxParam(
		hInstance,
		MAKEINTRESOURCE(IDD_TL_SETNUM),
		hPanel,
		NumMapsDlgProc,
		(LPARAM)theTex->subTex.Count());
	if (res>=0) {
		theTex->SetNumMaps(res);
		LoadDialog(TRUE);
		}
	}

void TextureLayersDlg::SetUVWAttenuation(int canal)
	{
	float attVs[6] = {-(theTex->atCentre[canal]-1.0f)*90.0f,
					  theTex->atStart[canal]*90.0f,
					  theTex->atOffset[canal]*90.0f,
					  theTex->atV1[canal],
					  theTex->atV2[canal],
					  float(theTex->atNormal[canal])
						};
	int res = DialogBoxParam(
		hInstance,
		MAKEINTRESOURCE(IDD_TL_SETUVWATT),
		hPanel,
		UVWAttDlgProc,
		(LPARAM)attVs);
	if(res==1) {
		theTex->atCentre[canal] = -(attVs[0]/90.0f)+1.0f;
		theTex->atStart[canal] = attVs[1]/90.0f;
		theTex->atOffset[canal] = attVs[2]/90.0f;
		theTex->atV1[canal] = attVs[3];
		theTex->atV2[canal] = attVs[4];
		theTex->atNormal[canal] = int(attVs[5]);
		}
	}

//-----------------------------------------------------------------------------
//  TextureLayers
//-----------------------------------------------------------------------------

#define TEXTURELAYERS_VERSION 2

void TextureLayers::Reset() 
	{	
	ivalid.SetEmpty();
	offset = 0;
	DeleteAllRefsFromMe();
	subTex.Resize(0);
	mapOn.Resize(0);
	mod_group.Resize(0);
	back.Resize(0);
	atCentre.Resize(0);
	atStart.Resize(0);
	atOffset.Resize(0);
	atV1.Resize(0);
	atV2.Resize(0);
	atNormal.Resize(0);
	modat.SetSize(0);
	SetNumMaps(2);
	}

void TextureLayers::ClampOffset() {
	if (offset+6>subTex.Count()) offset = subTex.Count()-6;
	if (offset<0) offset=0;
	}

void TextureLayers::NotifyChanged() {
	NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
	}

TextureLayers::TextureLayers() 
{
	surfs.SetCount(0);
	InitializeCriticalSection(&csect);
	canales.SetCount(0);
	paramDlg  = NULL;
	Reset();
	rollScroll=0;
}

TextureLayers::~TextureLayers()
{
	DeleteCriticalSection(&csect);
}

void TextureLayers::SetNumMaps(int n)
	{
	int ct = subTex.Count();
	if (n!=ct) {
		if (n<ct) {
			for (int i=n; i<ct; i++) {
				// Tell mtledit to deactivate texture map in UI
				if (subTex[i])
					subTex[i]->DeactivateMapsInTree();
				ReplaceReference(i,NULL);
				}
			}

		BitArray old_modat = modat;

		subTex.SetCount(n);
		mapOn.SetCount(n);
		mod_group.SetCount(n);
		back.SetCount(n);
		atCentre.SetCount(n);
		atStart.SetCount(n);
		atOffset.SetCount(n);
		atV1.SetCount(n);
		atV2.SetCount(n);
		atNormal.SetCount(n);
		modat.SetSize(n);

		// max9 is turning off all the modat in the bitarray resizing. This fixes that issue.
		int minitems = old_modat.GetSize()<modat.GetSize()?old_modat.GetSize():modat.GetSize();
		for ( int i=0; i<minitems; i++ )
			modat.Set( i, old_modat[i] );

		if (n>ct) {
			for (int i=ct; i<subTex.Count(); i++) {
				subTex[i] = NULL;				
				mapOn[i] = TRUE;
				mod_group[i] = i+1;
				back[i] = 0;
				atCentre[i] = 1.0f;
				atStart[i] = 0.0f;
				atOffset[i] = 1.0f;
				atV1[i] = 0.0f;
				atV2[i] = 1.0f;
				atNormal[i] = 2;
				modat.Set(i);
			}
		}
	}
}

int TextureLayers::RenderBegin(TimeValue t, ULONG flags) {
	surfs.SetCount(0);
	canales.SetCount(0);

	for(int nm=0; nm<subTex.Count(); nm++) {
		if(subTex[nm]) {
			if ((subTex[nm]->ClassID().PartA() == 0) &&
				(subTex[nm]->ClassID().PartB() == 675561916)) {

				MessageBox(NULL, _T("You have a Raytrace texture\nin your Texture Layers tree.\nRemove it to avoid unpredictable results"), _T("Texture Layers Warning"),MB_SYSTEMMODAL);
	 			mapOn[nm] = FALSE;
				return 1;
				} // if **raytrace**

			if ((subTex[nm]->ClassID().PartA() == 544696297) &&
				 (subTex[nm]->ClassID().PartB() == 1343386505)) {
				 
				MessageBox(NULL, _T("You have a Texture Layers texture\nin your Texture Layers tree.\nRemove it to avoid unpredictable results"), _T("Texture Layers Warning"),MB_SYSTEMMODAL);
	 			mapOn[nm] = FALSE;
				return 1;
				} // if **TextureMagic**
			} // if(subTex[nm])
		} // for(nm
	return 1;
	}

void TextureLayers::LocalMappingsRequired(int subMtlNum, BitArray &mapreq, BitArray &bumpreq) 
{
	GetModifiersMapChs(this,mapreq,bumpreq);
}

int TextureLayers::RenderEnd(TimeValue t) 
{
	for( int i=0; i<surfs.Count(); i++ )
	{
		for( int j=0; j<MAX_MESHMAPS; j++ )
		{
			AppDataChunk * ad = surfs[i]->GetAppDataChunk( ATTMAP_CID, TEXMAP_CLASS_ID, j);
			if(ad)
			{
				surfs[i]->RemoveAppDataChunk( ATTMAP_CID, TEXMAP_CLASS_ID, j );
			}
		}
	}

	for ( unsigned int i_d=0; i_d<mesh_datas.size(); i_d++ ) {
		mesh_datas[i_d]->DeleteThis();
		mesh_datas[i_d] = NULL;
	}

	std::vector<MeshExtraData*>().swap( mesh_datas );

	surfs.SetCount(0);
	canales.SetCount(0);
	return 1;
	}

static AColor black(0.0f,0.0f,0.0f,0.0f);

void TextureLayers::SetData(INode* node, ShadeContext &sc, int uvw_group, Class_ID modCID) 
{
	Object* obj;
	if (!node) return;
	obj = node->GetObjectRef();
	if (!obj) return;

	ObjectState os = node->EvalWorldState(curtime);
	if (os.obj && os.obj->SuperClassID() != GEOMOBJECT_CLASS_ID)
		return;

	RenderInstance *ri = (RenderInstance*)sc.globContext->GetRenderInstance(sc.NodeID());
	Mesh *mi = ri->mesh;

	int num_maps = MAX_MESHMAPS;// mi->getNumMaps();

	// Set the none_sel status of the med data!
	MultiMapModData mmmd = GetMultiMapMod( sc.Node(), curtime );
	TexLayMCData *md = mmmd.mtd;
	MultiMapMod *mmm = mmmd.mod;

	for ( int map_channel=0; map_channel<num_maps; map_channel++ ) 
	{
		MeshExtraData * med = new MeshExtraData;

		if ( md )
			med->topo_channel = md->topo_channel;

		int len = sizeof(med);
		if ( mi->mapSupport(map_channel) ) 
		{
			med->BuildMeshExtraData( mi, map_channel );
		}

		int med_id = mesh_datas.size();
		mesh_datas.push_back( med );

		void * data = MAX_malloc( sizeof(int) );
		memcpy( data, &med_id, sizeof(int) );

		node->AddAppDataChunk( ATTMAP_CID, TEXMAP_CLASS_ID, map_channel, sizeof(int), data ); 
	}

	if ( md )
	{
		int num_groups = md->face_sel.Count();
		md->none_sel.SetSize( num_groups );

		for ( int i_g=0; i_g<num_groups; i_g++ )
		{
			mmm->uvwProy[i_g]->Update(curtime);
			if ( md->face_sel[i_g].NumberSet() == 0 )
				md->none_sel.Set(i_g);
			else 
				md->none_sel.Clear(i_g);
		}
	}
}

#define SHOW_CURVE	3
#define	SHOW_BOTTOM	2
#define	SHOW_TOP	1
#define SHOW_ALL	0

float TextureLayers::GetUVWAttenuation(int i,float v) {
	float p1 = atCentre[i] - atStart[i] - atOffset[i];
	float p2 = atCentre[i] - atStart[i];
	float p3 = atCentre[i] + atStart[i];
	float p4 = atCentre[i] + atStart[i] + atOffset[i];

	if (v <= p1) return atV1[i];
	else if (v <= p2) {
		float u = (v-p1)/(p2-p1);
		return atV1[i] + u*u*(3.0f-2.0f*u)*(atV2[i]-atV1[i]);
		}
	else if (v <= p3) return atV2[i];
	else if (v <= p4) {
		float u = (v-p3)/(p4-p3);
		return atV2[i] - u*u*(3.0f-2.0f*u)*(atV2[i]-atV1[i]);
		}
	else return atV1[i];
}

AColor TextureLayers::MaxEvalColor(ShadeContext& sc) {	
	AColor c;
	if (sc.GetCache(this,c)) 
		return c; 
	if (gbufID) sc.SetGBufferID(gbufID);
	AColor res(0,0,0);	
	for (int i=0; i<subTex.Count(); i++) {
		Interval iv;

		if (!subTex[i]||!mapOn[i]) continue;
		res = CompOver(subTex[i]->EvalColor(sc),res);
		}
	sc.PutCache(this,res); 
	return res;
	}

AColor TextureLayers::EvalColor(ShadeContext& sc) 
{	
	AColor c;
	AColor res(0,0,0,0), col(0,0,0,0);

	INode * node = sc.Node();
	if ( !node )
		return MaxEvalColor(sc);

	if (sc.GetCache(this,c)) return c;
	if (gbufID) sc.SetGBufferID(gbufID);

	// Si estamos en el editor de materiales usamos el shade context original...
	MultiMapModData mmmd = GetMultiMapMod( sc.Node(), curtime );
	TexLayMCData *md = mmmd.mtd;
	MultiMapMod *mmm = mmmd.mod;

	if ( !md || !mmm )
		return MaxEvalColor(sc);

	Point3 puvw;
	float at = 1.0f;

	for (int i=0; i<subTex.Count(); i++) 
	{
		int this_mod_group = mod_group[i]-1;
		
		if ( this_mod_group>=mmm->uvwProy.Count() )
			continue;
		
		int map_channel = 1;
		if ( this_mod_group<mmm->uvwProy.Count() ) {
			map_channel = mmm->uvwProy[this_mod_group]->GetMappingChannel();
			}

		if(sc.Node() && !sc.Node()->GetAppDataChunk( ATTMAP_CID, TEXMAP_CLASS_ID, map_channel ) )
		{
			EnterCriticalSection(&csect);
			if( sc.Node() && !sc.Node()->GetAppDataChunk(ATTMAP_CID, TEXMAP_CLASS_ID, map_channel ) )
			{
				surfs.SetCount(surfs.Count()+1);
				surfs[surfs.Count()-1] = sc.Node();
				SetData(sc.Node(), sc, 0, MULTIMAP_MOD_CID);
			}
			LeaveCriticalSection(&csect);
		}

		AppDataChunk *ad = sc.Node()->GetAppDataChunk( ATTMAP_CID, TEXMAP_CLASS_ID, map_channel );
		if ( ad == NULL )
		{
			continue;
		}

		int med_id;
		memcpy( &med_id, ad->data, sizeof(int) );
		MeshExtraData * med = mesh_datas[ med_id ];

		if (!subTex[i]||!mapOn[i]) 
			continue;

		if ( med->numFaces==0 ) 
			continue;

		at = 1.0f;

		Point3 vc = sc.UVW( med->topo_channel );
		int original_face = int(vc.x);

		if ( !med->GetFaceSelection(sc.FaceNumber()))
			at = 0.0f;

		if ( !md->none_sel[this_mod_group] && !md->face_sel[this_mod_group][original_face] )
			at = 0.0f;

		if ( modat[i] && at!=0.0f ) {
			Point3 puvw = sc.UVW(map_channel);
			at = mmm->GetAtt(puvw, this_mod_group);
			if (at == 0.0f) continue;  // If the Att is 0, stop the calculations...
			}

		if ( back[i] != SHOW_ALL && at!=0.0f ) {

			Point3 uvn = Normalize( med->GetRvert(med->GetFace(sc.FaceNumber()).x) * sc.BarycentricCoords().x +
									med->GetRvert(med->GetFace(sc.FaceNumber()).y) * sc.BarycentricCoords().y +
									med->GetRvert(med->GetFace(sc.FaceNumber()).z) * sc.BarycentricCoords().z   );

			switch (back[i]) {
				case SHOW_TOP: {
					if (uvn.z < 0.0f) at = 0.0f;
					break;
					}
				case SHOW_BOTTOM: {
					if (uvn.z >= 0.0f) at = 0.0f;
					break;
					}
				case SHOW_CURVE:
					at *= GetUVWAttenuation(i,uvn[atNormal[i]]);
					break;
				}
			}

		if (at == 0.0f) continue;

		col = subTex[i]->EvalColor(sc);
		col.r *= at;
		col.g *= at;
		col.b *= at;
		col.a *= at;
		res = CompOver(col,res);
		}
	sc.PutCache(this,res); 

	return res;
	}

Point3 TextureLayers::MaxEvalNormalPerturb(ShadeContext& sc) 
	{
	Point3 p(0,0,0);
	if (gbufID) sc.SetGBufferID(gbufID);
    BOOL c = FALSE;
	for (int i=0; i<subTex.Count(); i++) {
		Interval iv;
		if (!subTex[i]||!mapOn[i]) continue;
		Point3 d = subTex[i]->EvalNormalPerturb(sc);
		if (!c) {
			p = d;
			c = 1;
			}	
		else {
			// composite perturbations using alpha -- DS 4/4/97
			AColor col = subTex[i]->EvalColor(sc);
			p = (1.0f-col.a)*p + d;
			}
		}
	return p;	
	}

Point3 TextureLayers::EvalNormalPerturb(ShadeContext& sc) {
	Point3 p(0,0,0);
    BOOL c = FALSE;

	if (gbufID) sc.SetGBufferID(gbufID);

	// Si estamos en el editor de materiales usamos el shade context original...
	MultiMapModData mmmd = GetMultiMapMod( sc.Node(), curtime );
	TexLayMCData *md = mmmd.mtd;
	MultiMapMod *mmm = mmmd.mod;

	if ( !md || !mmm )
		return MaxEvalNormalPerturb(sc);

	Point3 puvw;
	float at = 1.0f;

	for (int i=0; i<subTex.Count(); i++)
	{
		int this_mod_group = mod_group[i]-1;

		if ( this_mod_group>=mmm->uvwProy.Count() )
			continue;
		
		int map_channel = 1;
		if ( this_mod_group<mmm->uvwProy.Count() ) {
			map_channel = mmm->uvwProy[this_mod_group]->map_channel;
			}

		if(!sc.Node()->GetAppDataChunk(ATTMAP_CID, TEXMAP_CLASS_ID,map_channel))
		{
			EnterCriticalSection(&csect);
			if(!sc.Node()->GetAppDataChunk(ATTMAP_CID, TEXMAP_CLASS_ID,map_channel))
			{
				surfs.SetCount(surfs.Count()+1);
				surfs[surfs.Count()-1] = sc.Node();
				SetData(sc.Node(), sc, 0, MULTIMAP_MOD_CID);
			}
			LeaveCriticalSection(&csect);
		}

		AppDataChunk *ad = sc.Node()->GetAppDataChunk(ATTMAP_CID,TEXMAP_CLASS_ID,map_channel);

		if (ad == NULL)	continue;

		int med_id;
		memcpy( &med_id, ad->data, sizeof(int) );
		MeshExtraData * med = mesh_datas[ med_id ];

		if ( !subTex[i] || !mapOn[i] ) continue;

		at = 1.0f;

		Point3 vc = sc.UVW( med->topo_channel );
		int original_face = int(vc.x);

		if ( !md->none_sel[this_mod_group] && !md->face_sel[this_mod_group][original_face] ) 
			at = 0.0f;

		if ( modat[i] && at!=0.0f ) {
			Point3 puvw = sc.UVW(map_channel);
			at = mmm->GetAtt(puvw, this_mod_group);
			if (at == 0.0f) continue;  // If the Att is 0, stop the calculations...
			}

		if ( back[i] != SHOW_ALL && at!=0.0f )
		{
			Point3 uvn = Normalize( med->GetRvert(med->GetFace(sc.FaceNumber()).x) * sc.BarycentricCoords().x +
									med->GetRvert(med->GetFace(sc.FaceNumber()).y) * sc.BarycentricCoords().y +
									med->GetRvert(med->GetFace(sc.FaceNumber()).z) * sc.BarycentricCoords().z   );

			switch (back[i])
			{
				case SHOW_TOP:
					if (uvn.z < 0.0f) at = 0.0f;
				break;

				case SHOW_BOTTOM:
					if (uvn.z >= 0.0f) at = 0.0f;
				break;

				case SHOW_CURVE:
					at *= GetUVWAttenuation( i, uvn[atNormal[i]] );
				break;
			}
		}

		if (at == 0.0f) continue;

		Point3 d = subTex[i]->EvalNormalPerturb(sc);

		if (!c) {
			p = d * at;
			c = 1;
			}	
		else {
			// TextureLayers perturbations using alpha -- DS 4/4/97
			AColor col = subTex[i]->EvalColor(sc);
			if (col.a == 1.0f)	
				p = p * (1.0f - at) + d * at;
			else 
				p = p * (1.0f - at * col.a) + d * at * col.a;
			}
		}

	return p;
	}


RefTargetHandle TextureLayers::Clone(RemapDir &remap) 
	{
	TextureLayers *mnew = new TextureLayers();
	*((MtlBase*)mnew) = *((MtlBase*)this);  // copy superclass stuff	
	mnew->ivalid.SetEmpty();	
	mnew->subTex.SetCount(subTex.Count());
	mnew->mapOn.SetCount(subTex.Count());
	mnew->mod_group.SetCount(mod_group.Count());
	mnew->back.SetCount(back.Count());
	mnew->atCentre.SetCount(atCentre.Count());
	mnew->atStart.SetCount(atStart.Count());
	mnew->atOffset.SetCount(atOffset.Count());
	mnew->atV1.SetCount(atV1.Count());
	mnew->atV2.SetCount(atV2.Count());
	mnew->atNormal.SetCount(atNormal.Count());
	mnew->modat.SetSize(modat.GetSize());
	mnew->offset = offset;
	for (int i = 0; i<subTex.Count(); i++) {
		mnew->subTex[i] = NULL;
		if (subTex[i])
			mnew->ReplaceReference(i,remap.CloneRef(subTex[i]));
		mnew->mapOn[i] = mapOn[i];
		mnew->mod_group[i] = mod_group[i];
		mnew->back[i] = back[i];
		mnew->atCentre[i] = atCentre[i];
		mnew->atStart[i] = atStart[i];
		mnew->atOffset[i] = atOffset[i];
		mnew->atV1[i] = atV1[i];
		mnew->atV2[i] = atV2[i];
		mnew->atNormal[i] = atNormal[i];
		mnew->modat.Set(i,modat[i]);
		}
	return (RefTargetHandle)mnew;
	}

ParamDlg* TextureLayers::CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp) 
	{
	TextureLayersDlg *dm = new TextureLayersDlg(hwMtlEdit, imp, this);
	dm->LoadDialog(TRUE);	
	paramDlg = dm;
	return dm;	
	}

void TextureLayers::Update(TimeValue t, Interval& valid) 
	{
	curtime = t;
	for(int i=0; i<surfs.Count(); i++) {
		for(int j=0; j<MAX_MESHMAPS; j++) {
			AppDataChunk *ad = surfs[i]->GetAppDataChunk( ATTMAP_CID, TEXMAP_CLASS_ID, j);
			if( ad ) {
				//MeshExtraData *med = (MeshExtraData*)ad->data;
				//med->DeleteThis();
				surfs[i]->RemoveAppDataChunk( ATTMAP_CID, TEXMAP_CLASS_ID, j );
				}
			}
		}

	for ( unsigned int i_d=0; i_d<mesh_datas.size(); i_d++ ) {
		mesh_datas[i_d]->DeleteThis();
		mesh_datas[i_d] = NULL;
	}
	std::vector<MeshExtraData*>().swap( mesh_datas );

	surfs.SetCount(0);
	canales.SetCount(0);
	if (!ivalid.InInterval(t)) {
		ivalid.SetInfinite();	
		for (int i=0; i<subTex.Count(); i++) {
			if (subTex[i]) 
				subTex[i]->Update(t,ivalid);
			}
		}
	ivalid.Set(t,t);
	valid &= ivalid;
	}

void TextureLayers::SetSubTexmap(int i, Texmap *m) {
	if (i>=subTex.Count()) {
		int n = subTex.Count();
		subTex.SetCount(i+1);
		for (int j=n; j<=i; j++)
			subTex[j] = NULL;
		}
	ReplaceReference(i,m);
	if (paramDlg)
		paramDlg->UpdateSubTexNames();
	}

TSTR TextureLayers::GetSubTexmapSlotName(int i) {
	TSTR buf;
	buf.printf("%s %d:",GetString(IDS_MM_LAYNUM),i+1);
	return buf;
	}
	 
TSTR TextureLayers::SubAnimName(int i) {	
	return GetSubTexmapTVName(i);
	}

RefResult TextureLayers::NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
   PartID& partID, RefMessage message ) {
	switch (message) {
		case REFMSG_CHANGE:			
			if (paramDlg) 
				paramDlg->Invalidate();		
			ivalid.SetEmpty();
			break;
		
		case REFMSG_GET_PARAM_DIM:
			return REF_STOP; 
		
		case REFMSG_GET_PARAM_NAME: {
			GetParamName *gpn = (GetParamName*)partID;
			gpn->name= GetSubTexmapSlotName(gpn->index);			
			return REF_STOP; 
			}
		}
	return(REF_SUCCEED);
	}


#define SUBTEX_COUNT_CHUNK	0x0010
#define CHANNEL_CHUNK		0x0020
#define BACK_CHUNK			0x0030
#define CENTRE_CHUNK		0x0035
#define START_CHUNK			0x0040
#define OFFSET_CHUNK		0x0045
#define ATV1_CHUNK			0x0050
#define ATV2_CHUNK			0x0055
#define ATNORMAL_CHUNK		0x0060
#define MODAT_CHUNK			0x0070
#define USE_MOD_CHUNK		0x0090
#define MODGROUP_CHUNK		0x0100
#define MAPOFF_CHUNK		0x1000
#define MTL_HDR_CHUNK 		0x4000

IOResult TextureLayers::Save(ISave *isave) { 
	IOResult res;
	ULONG nb;

	// Save common stuff
	isave->BeginChunk(MTL_HDR_CHUNK);
	res = MtlBase::Save(isave);
	if (res!=IO_OK) return res;
	isave->EndChunk();
	
	int c = subTex.Count();

	isave->BeginChunk(SUBTEX_COUNT_CHUNK);
	isave->Write(&c,sizeof(c),&nb);
	isave->EndChunk();

	isave->BeginChunk(BACK_CHUNK);
	isave->Write(back.Addr(0),sizeof(int)*c,&nb);
	isave->EndChunk();

	isave->BeginChunk(CENTRE_CHUNK);
	isave->Write(atCentre.Addr(0),sizeof(float)*c,&nb);
	isave->EndChunk();

	isave->BeginChunk(START_CHUNK);
	isave->Write(atStart.Addr(0),sizeof(float)*c,&nb);
	isave->EndChunk();

	isave->BeginChunk(OFFSET_CHUNK);
	isave->Write(atOffset.Addr(0),sizeof(float)*c,&nb);
	isave->EndChunk();

	isave->BeginChunk(ATV1_CHUNK);
	isave->Write(atV1.Addr(0),sizeof(float)*c,&nb);
	isave->EndChunk();

	isave->BeginChunk(ATV2_CHUNK);
	isave->Write(atV2.Addr(0),sizeof(float)*c,&nb);
	isave->EndChunk();

	isave->BeginChunk(ATNORMAL_CHUNK);
	isave->Write(atNormal.Addr(0),sizeof(float)*c,&nb);
	isave->EndChunk();

	isave->BeginChunk(MODAT_CHUNK);
	modat.Save(isave);
	isave->EndChunk();

	isave->BeginChunk(MODGROUP_CHUNK);
	isave->Write(mod_group.Addr(0),sizeof(int)*c,&nb);
	isave->EndChunk();

	for (int i=0; i<subTex.Count(); i++) {
		if (mapOn[i]==0) {
			isave->BeginChunk(MAPOFF_CHUNK+i);
			isave->EndChunk();
			}
		}

	return IO_OK;
	}	
	  


IOResult TextureLayers::Load(ILoad *iload) { 
	IOResult res;	
	ULONG nb;

	while (IO_OK==(res=iload->OpenChunk())) {
		int id = iload->CurChunkID();
		if (id>=MAPOFF_CHUNK&&id<=MAPOFF_CHUNK+0x1000) {
			mapOn[id-MAPOFF_CHUNK] = FALSE; 
			}
		else
		switch(id)  {
			case SUBTEX_COUNT_CHUNK: {
				int c;
				iload->Read(&c,sizeof(c),&nb);
				SetNumMaps(c);
				break;
				}

			case CHANNEL_CHUNK:
				iload->Read(mod_group.Addr(0), sizeof(int)*mod_group.Count(), &nb);
				break;

			case BACK_CHUNK:
				iload->Read(back.Addr(0), sizeof(int)*back.Count(), &nb);
				break;

			case CENTRE_CHUNK:
				iload->Read(atCentre.Addr(0), sizeof(float)*atCentre.Count(), &nb);
				break;

			case START_CHUNK:
				iload->Read(atStart.Addr(0), sizeof(float)*atStart.Count(), &nb);
				break;

			case OFFSET_CHUNK:
				iload->Read(atOffset.Addr(0), sizeof(float)*atOffset.Count(), &nb);
				break;

			case ATV1_CHUNK:
				iload->Read(atV1.Addr(0), sizeof(float)*atV1.Count(), &nb);
				break;

			case ATV2_CHUNK:
				iload->Read(atV2.Addr(0), sizeof(float)*atV2.Count(), &nb);
				break;

			case ATNORMAL_CHUNK:
				iload->Read(atNormal.Addr(0), sizeof(float)*atNormal.Count(), &nb);
				break;

			case MODAT_CHUNK:
				modat.Load(iload);
				break;

			case MODGROUP_CHUNK:
				iload->Read(mod_group.Addr(0), sizeof(int)*mod_group.Count(), &nb);
				break;

			case MTL_HDR_CHUNK:
				res = MtlBase::Load(iload);
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}	
	return IO_OK;
	}


