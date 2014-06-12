/**********************************************************************\
FILE: ATT.CPP
DESCRIPTION: Attenuation mask 
CREATED BY: Diego Castaño
HISTORY: Started:  August 6 - 1999
Copyright (c) 1999, All Rights Reserved.
\**********************************************************************/

#include "att.h"
#include "mapping.h"
#include "uvwgroup.h"
#include "texlay_mc_data.h"

#include "modstack.h"
#include "iparamm2.h"
#include "meshdata.h"
#include "load_cb.h"
#include "texlay.h"
#include "common.h"

std::vector<MeshExtraData*> AttMap::mesh_datas;

int numTints = 0;

class AttMapClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading) { 	return new AttMap; }
	const TCHAR *	ClassName() { return GetString(IDS_DC_ATTMAP_CDESC); } // mjm - 2.3.99
	SClass_ID		SuperClassID() { return TEXMAP_CLASS_ID; }
	Class_ID 		ClassID() { return ATTMAP_CID; }
	const TCHAR* 	Category() { return TEXMAP_CAT_COLMOD;  }

	const TCHAR*	InternalName() { return _T("TLAttMap"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }			// returns owning module handle
	};
static AttMapClassDesc attmapCD;
ClassDesc2* GetAttMapDesc() { return &attmapCD;  }

static HIMAGELIST hAboutImage = NULL;
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
	}	


//Win32 : BOOL AttMapDlgProc::DlgProc(TimeValue t,IParamMap2 *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam){
INT_PTR AttMapDlgProc::DlgProc(TimeValue t,IParamMap2 *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam){

	switch (msg) {
		case WM_INITDIALOG: {	
			map_group_spin = GetISpinner(GetDlgItem(hWnd, IDC_ATT_GROUP_MAP_SPIN));
			map_group_spin->LinkToEdit(GetDlgItem(hWnd, IDC_ATT_GROUP_MAP), EDITTYPE_INT);
	
			LoadImages();
			ICustButton *iTmp;
			iTmp = GetICustButton(GetDlgItem(hWnd,IDC_TLA_ABOUT));
			iTmp->SetImage(hAboutImage, 0, 0, 0, 0, 16, 16);
			iTmp->SetTooltip(TRUE,_T("About Texture Layers Attenuation"));
			ReleaseICustButton(iTmp);

			UpdateUI(hWnd);
			}
			break;
		case 133:
			UpdateUI(hWnd);
			break;
		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd,&ps);
			atmap->Update(GetCOREInterface()->GetTime(),FOREVER);
			int dc;
			atmap->pblock->GetValue(attuvwn_type, 0, dc, FOREVER);
			if (dc != SHOW_CURVE) {
				IParamMap2 *map = atmap->pblock->GetMap();
				map->Enable(uvwn_center, FALSE);
				map->Enable(uvwn_start, FALSE);
				map->Enable(uvwn_offset, FALSE);
				map->Enable(uvwn_in, FALSE);
				map->Enable(uvwn_out, FALSE);
				}
			if (dc == SHOW_CURVE) DrawCurve(hWnd,hdc);
			EndPaint(hWnd,&ps);
			}
			break;
		case CC_SPINNER_CHANGE:	{
			switch (LOWORD(wParam)) {
				case IDC_ATT_GROUP_MAP_SPIN: {
					int map_group = map_group_spin->GetIVal();
					atmap->SetMapGroup( map_group );
					break;
					}
				default:
					Rect rect;
					GetClientRectP(GetDlgItem(hWnd,IDC_ATT_GRAPH),&rect);
					InvalidateRect(hWnd,&rect,FALSE);
					break;
				}
			break;
			}
		case WM_COMMAND: {
			switch (LOWORD(wParam)) {	
				case IDC_TLA_ABOUT:
					DisplayTexLayAbout();
					break;

				case IDC_ATT_UVNOT:
				case IDC_ATT_TOP:
				case IDC_ATT_BOTT:
					{
					IParamMap2 *map = atmap->pblock->GetMap();
					map->Enable(uvwn_center, FALSE);
					map->Enable(uvwn_start, FALSE);
					map->Enable(uvwn_offset, FALSE);
					map->Enable(uvwn_in, FALSE);
					map->Enable(uvwn_out, FALSE);
					InvalidateRect(hWnd,NULL,TRUE);
					}
					break;
				case IDC_ATT_CURVE:
					{
					IParamMap2 *map = atmap->pblock->GetMap();
					map->Enable(uvwn_center, TRUE);
					map->Enable(uvwn_start, TRUE);
					map->Enable(uvwn_offset, TRUE);
					map->Enable(uvwn_in, TRUE);
					map->Enable(uvwn_out, TRUE);
					InvalidateRect(hWnd,NULL,TRUE);
					}
					break;
					}
				}
			break;
			}
	return FALSE;
	}

void AttMapDlgProc::UpdateUI(HWND hWnd) {
	int mod_group;
	atmap->pblock->GetValue(pb_mod_group,0,mod_group,FOREVER);
	map_group_spin->SetLimits(1, 1000, FALSE);
	map_group_spin->SetValue(mod_group,0);
	}


void AttMapDlgProc::DrawCurve(HWND hWnd,HDC hdc) {
	float atCentre = atmap->atCentre;
	float atStart = atmap->atStart;
	float atOffset = atmap->atOffset;
	float atOut = atmap->atOut;
	float atIn = atmap->atIn;

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

	int at1 = int(height * (1.0f - atOut)) + BORD_HOR + orect.top;
	int at2 = int(height * (1.0f - atIn)) + BORD_HOR + orect.top;

	int pt1 = int((p1+1.0f)/2.0f * width) + BORD_VER + orect.left;
	int pt2 = int((p2+1.0f)/2.0f * width) + BORD_VER + orect.left;
	int pt3 = int((p3+1.0f)/2.0f * width) + BORD_VER + orect.left;
	int pt4 = int((p4+1.0f)/2.0f * width) + BORD_VER + orect.left;

	MoveToEx(hdc,orect.left,at1,NULL);
	LineTo(hdc,pt1,at1);

	MoveToEx(hdc,pt1,at1,NULL);
	for(int i=1; i<=10; i++) {
		float u = float(i)/10.0f;
		float at = atOut + u*u*(3.0f-2.0f*u)*(atIn-atOut);
		float pt = p1 + (p2-p1)*u;
		int atm = int(height * (1.0f - at)) + BORD_HOR + orect.top;
		int ptm = int((pt+1.0f)/2.0f * width) + BORD_VER + orect.left;
		LineTo(hdc,ptm,atm);
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
		float at = atIn - u*u*(3.0f-2.0f*u)*(atIn-atOut);
		float pt = p3 + (p4-p3)*u;
		int atm = int(height * (1.0f - at)) + BORD_HOR + orect.top;
		int ptm = int((pt+1.0f)/2.0f * width) + BORD_VER + orect.left;
		if (ptm < orect.right) {
			LineTo(hdc,ptm,atm);
			}
		else {
			int ato,pto;
			u = float(i-1)/10.0f;
			at = atIn - u*u*(3.0f-2.0f*u)*(atIn-atOut);
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

static ParamBlockDesc2 attmap_param_blk ( attmap_params, _T("parameters"),  0, &attmapCD, P_AUTO_CONSTRUCT + P_AUTO_UI, 0, 
	//rollout
	IDD_ATT, IDS_DC_ATTPARAMS, 0, 0, NULL, 
	// params
	attmap_map,		_T("map"),		TYPE_TEXMAP,			P_OWNERS_REF,	IDS_JW_MAP1,
		p_refno,		1,
		p_subtexno,		0,		
		p_ui,			TYPE_TEXMAPBUTTON, IDC_TINT_MAP,
		end,
	attmap_map_on,	_T("map_on"), TYPE_BOOL,			0,				IDS_JW_MAP1ENABLE,
		p_default,		TRUE,
		p_ui,			TYPE_SINGLECHEKBOX, IDC_MAPON,
		end,
	pb_att_face,	_T("att_face"), TYPE_BOOL,		0,				_T("Use face selection"),
		p_default,		TRUE,
		p_ui,			TYPE_SINGLECHEKBOX, IDC_ATT_FACE,
		end,
	pb_att_mod,		_T("att_mod"), TYPE_BOOL,		0,			_T("Use modifier selection"),
		p_default,		TRUE,
		p_ui,			TYPE_SINGLECHEKBOX, IDC_ATT_MOD,
		end,
	pb_map_channel,	_T("map_channel"),   TYPE_INT,			P_ANIMATABLE,	IDS_DC_CANAL,
		p_default,		1,
		p_range,		0, 99,
		end,
	attuvwn_type,	_T("attn_type"),		TYPE_INT,			0,				_T("type"),
		p_default,		0,
		p_range,		0,	3,
		p_ui,			TYPE_RADIO, 4, IDC_ATT_UVNOT, IDC_ATT_TOP, IDC_ATT_BOTT, IDC_ATT_CURVE,
		end,
	uvwn_center,	_T("attn_center"),   TYPE_FLOAT,			P_ANIMATABLE,	_T("Ceter"),
		p_default,		0.0,
		p_range,		0.0, 180.0,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT,  IDC_ATTCEN_EDIT, IDC_ATTCEN_SPIN, 0.1f,
		end,
	uvwn_start,		_T("attn_start"),   TYPE_FLOAT,			P_ANIMATABLE,	_T("Start"),
		p_default,		0.0,
		p_range,		0.0, 180.0,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT,  IDC_ATTSTR_EDIT, IDC_ATTSTR_SPIN, 0.1f,
		end,
	uvwn_offset,	_T("attn_offset"),   TYPE_FLOAT,			P_ANIMATABLE,	_T("Offset"),
		p_default,		90.0,
		p_range,		0.0, 180.0,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT,  IDC_ATTOFF_EDIT, IDC_ATTOFF_SPIN, 0.1f,
		end,
	uvwn_in,		_T("attn_inside"),   TYPE_FLOAT,			P_ANIMATABLE,	_T("Inside"),
		p_default,		1.0,
		p_range,		0.0, 1.0,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT,  IDC_ATTIN_EDIT, IDC_ATTIN_SPIN, 0.005f,
		end,
	uvwn_out,		_T("attn_outside"),   TYPE_FLOAT,			P_ANIMATABLE,	_T("Outside"),
		p_default,		0.0,
		p_range,		0.0, 1.0,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT,  IDC_ATTOUT_EDIT, IDC_ATTOUT_SPIN, 0.005f,
		end,
	uvwn_uvwnormal,	_T("attn_axis"),		TYPE_INT,			0,				_T("Normal"),
		p_default,		0,
		p_range,		0,	2,
		p_ui,			TYPE_RADIO, 3, IDC_UV, IDC_VW, IDC_WU,
		end,
	pb_use_mod,		_T("use_mod"), TYPE_BOOL,			0,				IDS_JW_MAP1ENABLE,
		p_default,		TRUE,
		end,
	pb_mod_group,	_T("mod_group"),   TYPE_INT,			0,	IDS_DC_CANAL,
		p_default,		1,
		p_range,		0, 1000,
		end,
	end
);




//-----------------------------------------------------------------------------
//  AttMap
//-----------------------------------------------------------------------------

#define TINT_VERSION 2

#define NPARAMS 3

static int name_id[NPARAMS] = {IDS_DS_COLOR1, IDS_DS_COLOR2, IDS_DS_COLOR3};

static ParamBlockDescID pbdesc[] = {
	{ TYPE_INT,  NULL, FALSE, pb_map_channel },
	};   

static ParamVersionDesc versions[] = {
	ParamVersionDesc(pbdesc,3,1),	// Version 1 params
};


void AttMap::Init() {
	ivalid.SetEmpty();
	mapOn[0] = 1;
	}

void AttMap::Reset() {
	attmapCD.Reset(this, TRUE);	// reset all pb2's
	for (int i=0; i<NSUBTEX; i++) 
		DeleteReference(i+1);	// get rid of maps
	Init();
	}

void AttMap::NotifyChanged() {
	NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
	}

AttMap::AttMap() {
	surfs.SetCount(0);
	InitializeCriticalSection(&csect);
	for (int i=0; i<NSUBTEX; i++) subTex[i] = NULL;
	pblock = NULL;
	attmapCD.MakeAutoParamBlocks(this);	// make and intialize paramblock2
	Init();
	rollScroll=0;
	mmm = NULL;
}

AttMap::~AttMap()
{
}

static AColor white(1.0f,1.0f,1.0f,1.0f);

void AttMap::SetData( INode* node, ShadeContext &sc, Class_ID modCID )
{
	Object* obj;
	if (!node) return;
	obj = node->GetObjectRef();
	if (!obj) return;

	ObjectState os = node->EvalWorldState(curtime);
	if (os.obj && os.obj->SuperClassID() != GEOMOBJECT_CLASS_ID)
		return;

	RenderInstance *ri = (RenderInstance*)sc.globContext->GetRenderInstance(sc.NodeID());
	Mesh * mesh_instance = ri->mesh;

	int num_maps = MAX_MESHMAPS;// mi->getNumMaps();

	// Set the none_sel status of the med data!
	MultiMapModData mmmd = GetMultiMapMod( sc.Node(), curtime );
	TexLayMCData *md = mmmd.mtd;
	MultiMapMod *mmm = mmmd.mod;

	for ( int map_channel=0; map_channel<num_maps; map_channel++ )
	{
		MeshExtraData *med = new MeshExtraData;

		if (md)
			med->topo_channel = md->topo_channel;

		int len = sizeof(med);
		if ( mesh_instance->mapSupport(map_channel) )
		{
			med->BuildMeshExtraData( mesh_instance, map_channel );
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

AColor AttMap::EvalColor(ShadeContext& sc) 
{
	if (gbufID) sc.SetGBufferID(gbufID);
	AColor c;

	if ( sc.InMtlEditor() )
	{
		return mapOn[0]&&subTex[0]? subTex[0]->EvalColor(sc) : AColor(0.0f,0.0f,0.0f,0.0f);
	}

	if (!mapOn[0]) 
		return AColor(0.0f,0.0f,0.0f,0.0f);

	MultiMapModData mmmd = GetMultiMapMod( sc.Node(), curtime );
	TexLayMCData *md = mmmd.mtd;
	MultiMapMod *mmm = mmmd.mod;

	if ( !md || !mmm || mod_group>=mmm->uvwProy.Count() )
		return mapOn[0]&&subTex[0]? subTex[0]->EvalColor(sc): AColor(0.0f,0.0f,0.0f,0.0f);

	int map_channel = 1;
	if ( mod_group<mmm->uvwProy.Count() ) {
		map_channel = mmm->uvwProy[mod_group]->map_channel;
	}

	if( sc.Node()->GetAppDataChunk( ATTMAP_CID, TEXMAP_CLASS_ID, map_channel ) == 0 )
	{
		EnterCriticalSection(&csect);
		if(!sc.Node()->GetAppDataChunk( ATTMAP_CID, TEXMAP_CLASS_ID, map_channel))
		{
			SetData( sc.Node(), sc, ATTMAP_CID );
			surfs.SetCount(surfs.Count()+1);
			surfs[surfs.Count()-1] = sc.Node();
		}
		LeaveCriticalSection(&csect);
	}

	IPoint2 sc_xy = sc.ScreenCoord();

	AppDataChunk *ad = sc.Node()->GetAppDataChunk( ATTMAP_CID,TEXMAP_CLASS_ID,map_channel);
	
	if (ad == NULL) return AColor(0.0f,0.0f,0.0f,0.0f);
	
	int med_id;
	memcpy( &med_id, ad->data, sizeof(int) );
	MeshExtraData * med = mesh_datas[ med_id ];

	if ( med->numFaces==0 )	return AColor(0.0f,0.0f,0.0f,0.0f);

	float at = 1.0f;

	Point3 vc = sc.UVW( med->topo_channel );
	int original_face = int(vc.x);

	if ( !md->none_sel[mod_group] && !md->face_sel[mod_group][original_face] )
		at = 0.0f;

	if ( att_mod && at!=0.0f )
	{
		Point3 puvw = sc.UVW( map_channel );
		at = mmm->GetAtt( puvw, mod_group );
	}

	if ( att_uvwn && at!=0.0f )
	{
		Point3 uvn = Normalize( med->GetRvert(med->GetFace(sc.FaceNumber()).x) * sc.BarycentricCoords().x +
							    med->GetRvert(med->GetFace(sc.FaceNumber()).y) * sc.BarycentricCoords().y +
							    med->GetRvert(med->GetFace(sc.FaceNumber()).z) * sc.BarycentricCoords().z   );

		switch (att_uvwn)
		{
			case SHOW_TOP:
				if (uvn.z < 0.0f) 
					at = 0.0f;
			break; 

			case SHOW_BOTTOM:
				if (uvn.z > 0.0f) 
					at = 0.0f;
			break;

			case SHOW_CURVE:
				at *= GetUVWAttenuation(uvn[att_normal]);
			break;
		}
	}

	if   (at != 0.0f) c = mapOn[0] && subTex[0] ? subTex[0]->EvalColor(sc) : AColor(0.0f,0.0f,0.0f,0.0f);
	else		      c = AColor(0.0f,0.0f,0.0f,0.0f);

	c.r *= at;
	c.g *= at;
	c.b *= at;
	c.a *= at;

	return c;	
	}

float AttMap::EvalMono(ShadeContext& sc) {
	return Intens(EvalColor(sc));
	}

Point3 AttMap::EvalNormalPerturb(ShadeContext& sc) {
	if (gbufID) sc.SetGBufferID(gbufID);
	Point3 p(0,0,0);

	if ( sc.InMtlEditor() ) {
		return mapOn[0]&&subTex[0]? subTex[0]->EvalNormalPerturb(sc) : Point3(0,0,0);
		}

	if (!mapOn[0]) 
		return p;

	MultiMapModData mmmd = GetMultiMapMod( sc.Node(), curtime );
	TexLayMCData *md = mmmd.mtd;
	MultiMapMod *mmm = mmmd.mod;

	if ( !md || !mmm || mod_group>=mmm->uvwProy.Count() )
		return mapOn[0]&&subTex[0]? subTex[0]->EvalColor(sc): AColor(0.0f,0.0f,0.0f,0.0f);

	int map_channel = 1;
	if ( mod_group<mmm->uvwProy.Count() ) {
		map_channel = mmm->uvwProy[mod_group]->GetMappingChannel();
		}

	if( !sc.Node()->GetAppDataChunk( ATTMAP_CID, TEXMAP_CLASS_ID, map_channel ) )
	{
		EnterCriticalSection(&csect);
		if(!sc.Node()->GetAppDataChunk( ATTMAP_CID, TEXMAP_CLASS_ID, map_channel)) 
		{
			SetData(sc.Node(), sc, ATTMAP_CID);
			surfs.SetCount(surfs.Count()+1);
			surfs[surfs.Count()-1] = sc.Node();
		}
		LeaveCriticalSection(&csect);
	}

	float at = 1.0f;

	AppDataChunk *ad = sc.Node()->GetAppDataChunk( ATTMAP_CID,TEXMAP_CLASS_ID, map_channel);
	if (ad == NULL) 
		return AColor(0.0f,0.0f,0.0f,0.0f);
	
	int med_id;
	memcpy( &med_id, ad->data, sizeof(int) );
	MeshExtraData * med = mesh_datas[ med_id ];

	Point3 vc = sc.UVW( med->topo_channel );
	int original_face = int(vc.x);

	if ( !med->GetFaceSelection(sc.FaceNumber()) )
		at = 0.0f; 

	if ( !md->none_sel[mod_group] && !md->face_sel[mod_group][original_face] )
		at = 0.0f;

	if ( att_mod && at!=0.0f ) {
		Point3 puvw = sc.UVW( map_channel );
		at = mmm->GetAtt( puvw, mod_group );
		}

	if ( att_uvwn && at!=0.0f ) {

		Point3 uvn = Normalize( med->GetRvert(med->GetFace(sc.FaceNumber()).x) * sc.BarycentricCoords().x +
							    med->GetRvert(med->GetFace(sc.FaceNumber()).y) * sc.BarycentricCoords().y +
							    med->GetRvert(med->GetFace(sc.FaceNumber()).z) * sc.BarycentricCoords().z   );

		switch (att_uvwn) {
			case SHOW_TOP:
				if (uvn.z < 0.0f) 
					at = 0.0f;
				break; 
			case SHOW_BOTTOM:
			if (uvn.z > 0.0f) 
				at = 0.0f;
				break;
			case SHOW_CURVE:
				at *= GetUVWAttenuation(uvn[att_normal]);
				break;
			}
		}

	Point3 d;
	if (at != 0.0f)
		d = mapOn[0]&&subTex[0]? subTex[0]->EvalNormalPerturb(sc) : Point3(0,0,0);
	else 
		return Point3(0,0,0);

	p = d * at;

	return p;	
	}

RefTargetHandle AttMap::Clone(RemapDir &remap) {
	AttMap *ncam = new AttMap;
	ncam->ReplaceReference(0,remap.CloneRef(pblock));
	ncam->ReplaceReference(1,remap.CloneRef(subTex[0]));
	return ncam;
	}

ParamDlg* AttMap::CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp) {
	IAutoMParamDlg* masterDlg = attmapCD.CreateParamDlgs(hwMtlEdit, imp, this);
	attmap_param_blk.SetUserDlgProc(new AttMapDlgProc(this));
	return masterDlg;
	}

//static int pbId[NPARAMS] = { PB_COL1, PB_COL2, PB_COL3};

void AttMap::Update(TimeValue t, Interval& valid)
{	
	curtime = t;

	for ( int i=0; i < surfs.Count(); i++ )
	{
		for(int j=0; j<MAX_MESHMAPS; j++)
		{
			AppDataChunk *ad = surfs[i]->GetAppDataChunk( ATTMAP_CID, TEXMAP_CLASS_ID, j);
			if ( ad )
			{
				surfs[i]->RemoveAppDataChunk(ATTMAP_CID, TEXMAP_CLASS_ID, j);
			}
		}
	}

	for ( unsigned int i_d=0; i_d<mesh_datas.size(); i_d++ ) {
		mesh_datas[i_d]->DeleteThis();
		mesh_datas[i_d] = NULL;
	}
	std::vector<MeshExtraData*>().swap( mesh_datas );

	surfs.SetCount(0);

	if (Param1) {
		pblock->SetValue( attmap_map_on, 0, mapOn[0]);
		Param1 = FALSE;
		}
	
	if (!ivalid.InInterval(t)) {
		ivalid.SetInfinite();

		pblock->GetValue( uvwn_center, t, atCentre, ivalid);
		pblock->GetValue( uvwn_start, t, atStart, ivalid);
		pblock->GetValue( uvwn_offset, t, atOffset, ivalid);
		pblock->GetValue( uvwn_in, t, atIn, ivalid);
		pblock->GetValue( uvwn_out, t, atOut, ivalid);
		pblock->GetValue( attmap_map_on, t, mapOn[0], ivalid);
		atCentre = -(atCentre/90.0f)+1.0f;
		atStart = atStart/90.0f;
		atOffset = atOffset/90.0f;
		pblock->GetValue( pb_att_face, t, att_face, ivalid);
		pblock->GetValue( pb_att_mod, t, att_mod, ivalid);
		pblock->GetValue( attuvwn_type, t, att_uvwn, ivalid);
		pblock->GetValue( uvwn_uvwnormal, t, att_normal, ivalid);

		switch (att_normal) {
			case 0:		att_normal = 2; break;
			case 1:		att_normal = 0; break;
			case 2:		att_normal = 1; break;
			}

		pblock->GetValue( pb_mod_group, t, mod_group, ivalid );

		mod_group = mod_group - 1;

		for (int i=0; i<NSUBTEX; i++) {
			if (subTex[i]) 
				subTex[i]->Update(t,ivalid);
			}
		}
	valid &= ivalid;
	valid = Interval(t,t);
	}

RefTargetHandle AttMap::GetReference(int i) {
	switch(i) {
		case 0:	return pblock ;
		default:return subTex[i-1];
		}
	}

void AttMap::SetReference(int i, RefTargetHandle rtarg) {
	switch(i) {
		case 0:	pblock = (IParamBlock2 *)rtarg; break;
		default: subTex[i-1] = (Texmap *)rtarg; break;
		}
	}

void AttMap::SetSubTexmap(int i, Texmap *m) {
	ReplaceReference(i+1,m);
	if (i==0)
		{
		attmap_param_blk.InvalidateUI(attmap_map);
		ivalid.SetEmpty();
		}	

	}

TSTR AttMap::GetSubTexmapSlotName(int i) {
	switch(i) {
		case 0:  return TSTR(GetString(IDS_DS_MAP)); 
		default: return TSTR(_T(""));
		}
	}
	 
Animatable* AttMap::SubAnim(int i) {
	switch (i) {
		case 0: return pblock;
		default: return subTex[i-1]; 
		}
	}

TSTR AttMap::SubAnimName(int i) {
	switch (i) {
		case 0: return TSTR(GetString(IDS_TL_PARAMETERS));		
		default: return GetSubTexmapTVName(i-1);
		}
	}

RefResult AttMap::NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
   PartID& partID, RefMessage message ) {
	switch (message) {
		case REFMSG_CHANGE:
			ivalid.SetEmpty();
			if (hTarget== pblock)
				{
			// see if this message came from a changing parameter in the pblock,
			// if so, limit rollout update to the changing item and update any active viewport texture
				ParamID changing_param = pblock->LastNotifyParamID();
//			if (hTarget == pblock) 
				attmap_param_blk.InvalidateUI(changing_param);
			// notify our dependents that we've changed
				// NotifyChanged();  //DS this is redundant
				}

			break;
		}
	return(REF_SUCCEED);
	}


#define MTL_HDR_CHUNK		0x4000
#define MAPOFF_CHUNK		0x1000
#define PARAM2_CHUNK		0x1010
#define VERSION_CHUNKID		0x1020

IOResult AttMap::Save(ISave *isave) { 
	IOResult res;
	ULONG nb;

	int version = TEXLAY_VERSION;

	// Save common stuff
	isave->BeginChunk(MTL_HDR_CHUNK);
	res = MtlBase::Save(isave);
	if (res!=IO_OK) return res;
	isave->EndChunk();

	isave->BeginChunk(VERSION_CHUNKID);
	isave->Write(&version,sizeof(int),&nb);
	isave->EndChunk();

	isave->BeginChunk(PARAM2_CHUNK);
	isave->EndChunk();

	return IO_OK;
	}	
	  
IOResult AttMap::Load(ILoad *iload) { 
	ULONG nb;
	IOResult res;
	Param1 = TRUE;

	int version = 0;

	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {

			case MTL_HDR_CHUNK:
				res = MtlBase::Load(iload);
				break;

			case MAPOFF_CHUNK:
				mapOn[0] = 0; 
				break;

			case PARAM2_CHUNK:
				Param1 = FALSE; 
				break;

			case VERSION_CHUNKID:
				iload->Read(&version, sizeof(int), &nb);
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}

	ParamBlock2PLCB* plcb = new ParamBlock2PLCB(versions, 1, &attmap_param_blk, this, 0);
	iload->RegisterPostLoadCallback(plcb);

	AttMapLoadCB* attcb = new AttMapLoadCB( this, version );
	iload->RegisterPostLoadCallback(attcb);

	return IO_OK;
	}

int AttMap::RenderBegin(TimeValue t, ULONG flags) {
	surfs.SetCount(0);
	return 1;
	}

void AttMap::LocalMappingsRequired(int subMtlNum, BitArray &mapreq, BitArray &bumpreq) {
	GetModifiersMapChs(this,mapreq,bumpreq);
	}

int AttMap::RenderEnd(TimeValue t) 
{
	for(int i=0; i<surfs.Count(); i++)
	{
		for(int j=0; j<MAX_MESHMAPS; j++)
		{
			AppDataChunk *ad = surfs[i]->GetAppDataChunk( ATTMAP_CID, TEXMAP_CLASS_ID, j);
			if(ad)
			{
				surfs[i]->RemoveAppDataChunk(ATTMAP_CID, TEXMAP_CLASS_ID, j);
			}
		}
	}

	for ( unsigned int i_d=0; i_d<mesh_datas.size(); i_d++ ) {
		mesh_datas[i_d]->DeleteThis();
		mesh_datas[i_d] = NULL;
	}
	std::vector<MeshExtraData*>().swap( mesh_datas );

	surfs.SetCount(0);
	mmm = NULL;
	return 1;
	}

float AttMap::GetUVWAttenuation(float v) {
//	v = asin(v) / HALFPI;	Uncomment this if we find a way to fix all the att values in a post load (even from controllers)

	float p1 = atCentre - atStart - atOffset;
	float p2 = atCentre - atStart;
	float p3 = atCentre + atStart;
	float p4 = atCentre + atStart + atOffset;

	if (v <= p1) return atOut;
	else if (v <= p2) {
		float u = (v-p1)/(p2-p1);
		return atOut + u*u*(3.0f-2.0f*u)*(atIn-atOut);
		}
	else if (v <= p3) return atIn;
	else if (v <= p4) {
		float u = (v-p3)/(p4-p3);
		return atIn - u*u*(3.0f-2.0f*u)*(atIn-atOut);
		}
	else return atOut;
}

void AttMap::SetMapGroup( int map_group ) {
	pblock->SetValue( pb_mod_group,0,map_group );
	}
