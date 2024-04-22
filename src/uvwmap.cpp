#include "mods.h"
#include "iparamm.h"
#include "iparamm2.h"
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
#include "MESHDLIB.H"
#include "natural_box.h"
#include "uv_pelt_dlg.h"
#include "texlay.h"
#include "..\..\UVWFrame\src\uvwframe.h"
#include "3d_geom.h"
#include "load_cb.h"
#include <vector>


#define SYMMETRY_X		0
#define SYMMETRY_Y		1
#define SYMMETRY_KEEP	2
#define SYMMETRY_BEST	3



//***********************************************************************//
//							GRID MESH METHODS							 //
//***********************************************************************//
class GridMeshClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() { return 0; }
	void *			Create(BOOL loading = FALSE) { return new GridMesh(0,loading);}
	const TCHAR *	ClassName() { return GetString(IDS_GRIDMESH_CLASS); }

#if MAX_VERSION_MAJOR >= 24
	const TCHAR *	NonLocalizedClassName() { return ClassName(); }
#endif

	SClass_ID		SuperClassID() { return UVW_SCID; }
	Class_ID		ClassID() { return GRID_MESH_CID; }
	const TCHAR* 	Category() {return GetString(IDS_DC_TEXTURELAYERS);}

	// Hardwired name, used by MAX Script as unique identifier
	const TCHAR*	InternalName()				{ return _T("UVWGrid"); }
	HINSTANCE		HInstance()					{ return hInstance; }
	};
static GridMeshClassDesc gridmeshClassDesc;
extern ClassDesc2* GetFFGridClassDesc() {return &gridmeshClassDesc;}

GridMesh::GridMesh(int i, BOOL loading) {
	if (!loading) {
		nx = 0;
		ny = 0;
		SetNumVerts(0);
		SetNumFaces(0);
		}
	}

Class_ID GridMesh::ClassID() { 
	return GRID_MESH_CID; 
	}

SClass_ID GridMesh::SuperClassID() {
	return UVW_SCID;
	}


Point3 GridMesh::GetVert(int i) {
	return verts[i];
	}

Point3 GridMesh::GetVert(int x, int y) {
	return verts[nx * y + x];
	}

Point3 GridMesh::GetFNormal(int x, int y, int i) {
	return fnormals[2 * ((nx-1) * y + x) + i];
	}

void GridMesh::SetNumVerts(int nv) {
	vtCtrl.SetCount(nv);
	for (int i=0; i<nv; i++) vtCtrl[i] = NULL;
	verts.SetCount(nv);
	vnormals.SetCount(nv);
	uvw.SetCount(nv);
	sel.SetSize(nv);
	sel.ClearAll();
	}

void GridMesh::SetNumFaces(int numf) {
	faces.SetCount(numf);
	fnormals.SetCount(numf);
	cosangs.SetCount(numf);
	coefD.SetCount(numf);
	den.SetCount(numf);
	}

// TODO: Implementar la ubicacion con w
// Tener en cuenta la no deformacion dentro del poligono.
Point3 GridMesh::GetPosition(TimeValue t, float u, float v, float w) {
	if (u>0.999999f) u=0.999999f;
	if (v>0.999999f) v=0.999999f;
	float dx = 1.0f/(nx-1);
	float dy = 1.0f/(ny-1);

	int ux = (int)floor(u/dx);
	float px = float(u/dx - floor(u/dx));
	
	int vy = (int)floor(v/dy);
	float py = float(v/dy - floor(v/dy));

	Point3 v0, v1, v2;
	// If we are in the even face (0 2 4 ...)
	if (px <= py) {
		v0 = GetVert(ux,vy+1); 
		v1 = GetVert(ux,vy);
		v2 = GetVert(ux+1,vy+1);
		Point3 bary(px,py,1.0f-(px+py));
		py = 1.0f - py;

		v0 = v0 + vnormals[nx*(vy+1)+ux] * w;
		v1 = v1 + vnormals[nx*vy+ux] * w;
		v2 = v2 + vnormals[nx*(vy+1)+(ux+1)] * w;

		return v0 + px * (v2-v0) + py * (v1-v0);
		}

	// If we are in the odd face (1 3 5 ...)
	else { // if (px > py)
		v0 = GetVert(ux+1,vy); 
		v1 = GetVert(ux+1,vy+1);
		v2 = GetVert(ux,vy);
		Point3 bary(px,py,1.0f-(px+py));
		px = 1.0f - px;

		v0 = v0 + vnormals[nx*vy + (ux+1)] * w;
		v1 = v1 + vnormals[nx*(vy+1)+(ux+1)] * w;
		v2 = v2 + vnormals[nx*vy+ux] * w;

		return v0 + px * (v2-v0) + py * (v1-v0);
		}
	}

void GridMesh::ExpandSelection(int ind, BOOL on, BOOL allx, BOOL ally)
	{
	int ix,iy;	

	ix = ind%nx;
	iy = ind/nx;

	if (allx)
		for (int i=0; i<nx; i++)
			sel.Set(nx * iy + i, on);

	if (ally)
		for (int i=0; i<ny; i++)
			sel.Set(nx * i + ix, on);

	}

Control *GridMesh::GetVtCtrl(int i) {return vtCtrl[i]; }

void GridMesh::PlugControllers(TimeValue t) {
	BOOL notify=FALSE;
	// Plug-in controllers for selected points without controllers
	// if we're animating
	if (Animating() && t!=0) {
		SuspendAnimate();
		AnimateOff();
		for (int i=0; i<vtCtrl.Count(); i++) {		
			if (sel[i] && !GetVtCtrl(i)) {
				ReplaceReference(i,NewDefaultPoint3Controller()); 				
				theHold.Suspend();				
				GetVtCtrl(i)->SetValue(0,&GetVert(i),TRUE,CTRL_ABSOLUTE);
				theHold.Resume();
				notify = TRUE;
				}			
			}
		ResumeAnimate();
		}
	if (notify) NotifyDependents(FOREVER,0,REFMSG_SUBANIM_STRUCTURE_CHANGED);
	}

void GridMesh::CacheVNormals() {
	if (verts.Count() == 0) return;
	int nf = faces.Count();
	int nv = verts.Count();

	for (int f=0; f<nf; f++) {
		Point3 e1 = verts[faces[f].y] - verts[faces[f].x];
		Point3 e2 = verts[faces[f].z] - verts[faces[f].x];
		fnormals[f] = CrossProd(e1,e2);
		fnormals[f] = Normalize(fnormals[f]);
		coefD[f] = -DotProd(verts[faces[f].x],fnormals[f]);
		den[f] = 1.0f / float(sqrt(fnormals[f].x*fnormals[f].x + fnormals[f].y*fnormals[f].y + 
									  fnormals[f].z*fnormals[f].z));
		}

	for (int v=0; v<nv; v++) {
		Point3 vn(0,0,0);
		Point3 avn(0,0,0);
		int pvx = v%(nx);
		int pvy = v/(nx);
		if (pvx > 0) {
			if (pvy > 0) {
				avn = GetFNormal(pvx-1,pvy-1,0);
				avn += GetFNormal(pvx-1,pvy-1,1);
				vn += Normalize(avn);
				}
			if (pvy < ny-1) {
				vn += GetFNormal(pvx-1,pvy,1);
				}
			}
		if (pvx < nx-1) {
			if (pvy > 0) {
				vn += GetFNormal(pvx,pvy-1,0);
				}
			if (pvy < ny-1) {
				avn = GetFNormal(pvx,pvy,0);
				avn += GetFNormal(pvx,pvy,1);
				vn += Normalize(avn);
				}
			}
		vnormals[v] = Normalize(vn);
		}

	for ( int f=0; f<nf; f++) {
		cosangs[f].x = DotProd(fnormals[f],vnormals[faces[f].x]);
		cosangs[f].y = DotProd(fnormals[f],vnormals[faces[f].y]);
		cosangs[f].z = DotProd(fnormals[f],vnormals[faces[f].z]);
		}
	}

#define NX_CHUNKID		0x0100
#define NY_CHUNKID		0x0150
#define VERTS_CHUNKID	0x0200
#define FACES_CHUNKID	0x0250
#define UVW_CHUNKID		0x0300


IOResult GridMesh::Load(ILoad *iload) {
	ULONG nb;
	IOResult res;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch (iload->CurChunkID()) {	
			case NX_CHUNKID:
				iload->Read(&nx, sizeof(nx), &nb);
				break;
			case NY_CHUNKID:
				iload->Read(&ny, sizeof(ny), &nb);
				break;
			case VERTS_CHUNKID:
				int vc;
				iload->Read(&vc, sizeof(vc), &nb);
				SetNumVerts(vc);
				if (vc)	iload->Read(verts.Addr(0), sizeof(Point3)*verts.Count(), &nb);
				break;
			case FACES_CHUNKID:
				int fc;
				iload->Read(&fc, sizeof(fc), &nb);
				SetNumFaces(fc);

//JW: ioapi changed considerably with 3ds Max 2013+ ( V15+ ) 
#if MAX_VERSION_MAJOR < 15
				if (fc) iload->Read(faces.Addr(0), sizeof(IPoint3)*faces.Count(), &nb);
#else
				if (fc) iload->ReadVoid(faces.Addr(0), sizeof(IPoint3)*faces.Count(), &nb);
#endif
				break;
			case UVW_CHUNKID:
				int uc;
				iload->Read(&uc, sizeof(uc), &nb);
				uvw.SetCount(uc);
				if (uc) iload->Read(uvw.Addr(0), sizeof(Point3)*uvw.Count(), &nb);
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK)
			return res;
		}
	Update(0);
	return IO_OK;
	}

IOResult GridMesh::Save(ISave *isave) {
	ULONG nb;

	isave->BeginChunk(NX_CHUNKID);
	isave->Write(&nx,sizeof(nx),&nb);
	isave->EndChunk();

	isave->BeginChunk(NY_CHUNKID);
	isave->Write(&ny,sizeof(ny),&nb);
	isave->EndChunk();

	isave->BeginChunk(VERTS_CHUNKID);
	int vc = verts.Count();
	isave->Write(&vc,sizeof(vc),&nb);
	if (vc) isave->Write(verts.Addr(0),sizeof(Point3)*verts.Count(), &nb);
	isave->EndChunk();

	isave->BeginChunk(FACES_CHUNKID);
	int fc = faces.Count();
	isave->Write(&fc,sizeof(fc),&nb);

	//JW: ioapi changed considerably with 3ds Max 2013+ ( V15+ ) 
#if MAX_VERSION_MAJOR < 15
	if (fc) isave->Write(faces.Addr(0), sizeof(IPoint3)*faces.Count(), &nb);
#else
	if (fc) isave->WriteVoid(faces.Addr(0), sizeof(IPoint3)*faces.Count(), &nb);
#endif

	isave->EndChunk();

	isave->BeginChunk(UVW_CHUNKID);
	int uc = uvw.Count();
	isave->Write(&uc,sizeof(uc),&nb);
	if (uc) isave->Write(uvw.Addr(0),sizeof(Point3)*uvw.Count(), &nb);
	isave->EndChunk();

	return IO_OK;
	}

void GridMesh::DeleteThis() {
	DeleteAllRefsFromMe();
	delete this;
	}

RefTargetHandle GridMesh::Clone(RemapDir& remap) {
	int i;
	GridMesh *ncgm = new GridMesh(0);
	ncgm->nx = nx;
	ncgm->ny = ny;
	ncgm->SetNumVerts(verts.Count());
	ncgm->SetNumFaces(faces.Count());
	for (i=0; i<verts.Count(); i++) {
		if (vtCtrl[i])	ncgm->ReplaceReference(i,vtCtrl[i]->Clone(remap));
		else			ncgm->vtCtrl[i] = NULL;
		ncgm->verts[i] = verts[i];
		}
	for (i=0; i<faces.Count(); i++)
		ncgm->faces[i] = faces[i];
	ncgm->Update(0);
	return ncgm;
	}

void GridMesh::Update(TimeValue t) {
	for (int i=0; i<vtCtrl.Count(); i++)
		if (vtCtrl[i])
			vtCtrl[i]->GetValue(t,&verts[i],FOREVER,CTRL_ABSOLUTE);
	CacheVNormals();
	}


Interval GridMesh::LocalValidity(TimeValue t) {
	Interval ival = FOREVER;
	Point3 p3;
	for (int i=0; i<vtCtrl.Count(); i++)	
		if (GetVtCtrl(i)) GetVtCtrl(i)->GetValue(t,&verts[i],ival,CTRL_ABSOLUTE);
	return ival;
	}


//***********************************************************************//
//***********************************************************************//
//																		 //
//          SPLINE MAPPING METHODS                                       //
//																		 //
//***********************************************************************//
//***********************************************************************//


TLBezierSpline::TLBezierSpline() {
	num_points = 0;
	in_vec = NULL;
	knot = NULL;
	out_vec = NULL;
	}

TLBezierSpline::TLBezierSpline( int np ) {
	SetNumKnots(np);
	}

TLBezierSpline::~TLBezierSpline() {
	Clean();
	}

void TLBezierSpline::SetNumKnots( int np ) {
	num_points = np;
	in_vec = new Point3[num_points];
	knot = new Point3[num_points];
	out_vec = new Point3[num_points];
	}

void TLBezierSpline::Clean() {
	delete [] in_vec;
	delete [] knot;
	delete [] out_vec;
	}

int TLBezierSpline::NumSegments() {
	int num_segments = num_points;
	if ( !closed )
		num_segments--;
	return num_segments;
	}

Point3 TLBezierSpline::GetInVec(int i) {
	if ( i<num_points )
		return in_vec[i];
	return Point3(0,0,0);
	}

Point3 TLBezierSpline::GetKnot(int i) {
	if ( i<num_points )
		return knot[i];
	return Point3(0,0,0);
	}

Point3 TLBezierSpline::GetOutVec(int i) {
	if ( i<num_points )
		return out_vec[i];
	return Point3(0,0,0);
	}


class UVWProyClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() { return 0; }
	void *			Create(BOOL loading = FALSE) { return new UVWProyector(!loading);}
	const TCHAR *	ClassName() { return GetString(IDS_UVWPROY_CLASS); }

#if MAX_VERSION_MAJOR >= 24
	const TCHAR *	NonLocalizedClassName() { return ClassName(); }
#endif 
	SClass_ID		SuperClassID() { return UVW_SCID; }
	Class_ID		ClassID() { return UVW_PROY_CID; }
	const TCHAR* 	Category() {return GetString(IDS_DC_TEXTURELAYERS);}

	// Hardwired name, used by MAX Script as unique identifier
	const TCHAR*	InternalName()				{ return _T("UVWGroup"); }
	HINSTANCE		HInstance()					{ return hInstance; }
	};
static UVWProyClassDesc uvwProyClassDesc;
extern ClassDesc2* GetUVWProyClassDesc() {return &uvwProyClassDesc;}

#if MAX_VERSION_MAJOR < 15 // JW: end was renamed to p_end in Max2013+
#define p_end end
#endif

static ParamBlockDesc2 uvw_proy_param_blk ( uvw_proy_params, _T("UVWProyector"),  0, &uvwProyClassDesc, P_AUTO_CONSTRUCT + P_AUTO_UI, 0, //P_AUTO_CONSTRUCT + P_AUTO_UI
	//rollout
	0, IDS_PARAMETERS, 0, 0, NULL, 
	// params

	uvw_length,			_T("Length"),	TYPE_FLOAT,	P_ANIMATABLE,	IDS_TL_LENGTH,
		p_default,		0.0f,
		p_range, 		-999999999.0f, 999999999.0f, 
		p_end,

	uvw_width,			_T("Width"),	TYPE_FLOAT,	P_ANIMATABLE,	IDS_TL_WIDTH,
		p_default,		0.0f,
		p_range, 		-999999999.0f, 999999999.0f, 
		p_end,

	uvw_height,			_T("Height"),	TYPE_FLOAT,	P_ANIMATABLE,	IDS_TL_HEIGHT,
		p_default,		0.0f,
		p_range, 		-999999999.0f, 999999999.0f, 
		p_end,

	uvw_tile_u,			_T("utile"),	TYPE_FLOAT,	P_ANIMATABLE,	IDS_TL_TILE_U,
		p_default,		1.0f,
		p_range, 		-999999999.0f, 999999999.0f, 
		p_end,

	uvw_offset_u,		_T("UOffset"),	TYPE_FLOAT,	P_ANIMATABLE,	IDS_TL_OFFSET_U,
		p_default,		0.0f,
		p_range, 		-999999999.0f, 999999999.0f, 
		p_end,

	uvw_tile_v,			_T("vtile"),	TYPE_FLOAT,	P_ANIMATABLE,	IDS_TL_TILE_V,
		p_default,		1.0f,
		p_range, 		-999999999.0f, 999999999.0f, 
		p_end,

	uvw_offset_v,		_T("VOffset"),	TYPE_FLOAT,	P_ANIMATABLE,	IDS_TL_OFFSET_V,
		p_default,		0.0f,
		p_range, 		-999999999.0f, 999999999.0f, 
		p_end,

	uvw_tile_w,			_T("wtile"),	TYPE_FLOAT,	P_ANIMATABLE,	IDS_TL_TILE_W,
		p_default,		1.0f,
		p_range, 		-999999999.0f, 999999999.0f, 
		p_end,

	uvw_offset_w,		_T("WOffset"),	TYPE_FLOAT,	P_ANIMATABLE,	IDS_TL_OFFSET_W,
		p_default,		0.0f,
		p_range, 		-999999999.0f, 999999999.0f, 
		p_end,

	uvw_atton,			_T("Att"),			TYPE_BOOL, 	0,	IDS_SIMPLE,
		p_default, 		FALSE, 
		p_end, 

	uvw_att,			_T("GlobalAtt"),	TYPE_FLOAT,	P_ANIMATABLE,	IDS_TL_ATT_ON,
		p_default,		0.0f,
		p_range, 		0.0f, 100.0f, 
		p_end,
	
 	uvw_aus,			_T("AttUStart"),	TYPE_FLOAT,	P_ANIMATABLE,	IDS_TL_ATT_U_S,
		p_default,		1.0f,
		p_range, 		0.0f, 999999999.0f, 
		p_end,

	uvw_aue,			_T("AttUOffset"),	TYPE_FLOAT,	P_ANIMATABLE,	IDS_TL_ATT_U_O,
		p_default,		0.0f,
		p_range, 		0.0f, 999999999.0f, 
		p_end,

	uvw_avs,			_T("AttVStart"),	TYPE_FLOAT,	P_ANIMATABLE,	IDS_TL_ATT_V_S,
		p_default,		1.0f,
		p_range, 		0.0f, 999999999.0f, 
		p_end,

	uvw_ave,			_T("AttVOffset"),	TYPE_FLOAT,	P_ANIMATABLE,	IDS_TL_ATT_V_O,
		p_default,		0.0f,
		p_range, 		0.0f, 999999999.0f, 
		p_end,

	uvw_aws,			_T("AttWStart"),	TYPE_FLOAT,	P_ANIMATABLE,	IDS_TL_ATT_W_S,
		p_default,		1.0f,
		p_range, 		0.0f, 999999999.0f, 
		p_end,

	uvw_awe,			_T("AttWOffset"),	TYPE_FLOAT,	P_ANIMATABLE,	IDS_TL_ATT_W_O,
		p_default,		0.0f,
		p_range, 		0.0f, 999999999.0f, 
		p_end,

	uvw_aruv,			_T("AttRoundUV"),	TYPE_FLOAT,	P_ANIMATABLE,	IDS_TL_ATT_RUV,
		p_default,		0.0f,
		p_range, 		0.0f, 100.0f, 
		p_end,

	uvw_axis,			_T("Axis"),		TYPE_INT, 	0,	IDS_SIMPLE,
		p_default, 		2, 
		p_end, 

	uvw_normlize,		_T("Spl_Normalize"),	TYPE_BOOL, 	0,	IDS_SIMPLE,
		p_default, 		FALSE, 
		p_end, 

	uvw_reverse,		_T("Spl_Reverse"),	TYPE_BOOL, 	0,	IDS_SIMPLE,
		p_default, 		FALSE, 
		p_end, 

	uvw_normals,		_T("Spl_Normals"),	TYPE_INT, 	0,	IDS_SIMPLE,
		p_default, 		2, 
		p_end, 

	uvw_ffm_thresh,		_T("FFM_Thresh"),	TYPE_FLOAT,	P_ANIMATABLE,	IDS_TL_FFM_THRESH,
		p_default,		0.0f,
		p_range, 		0.0f, 999999999.0f, 
		p_end,

	uvw_map_channel,	_T("MapChannel"),	TYPE_INT, 	0,	IDS_SIMPLE,
		p_default, 		1, 
		p_end, 

	uvw_move_u,			_T("umove"),	TYPE_FLOAT,	P_ANIMATABLE,	IDS_TL_U_MOVE,
		p_default,		0.0f,
		p_range, 		-999999999.0f, 999999999.0f, 
		p_end,

	uvw_rotate_u,		_T("urotate"),	TYPE_FLOAT,	P_ANIMATABLE,	IDS_TL_U_ROTATE,
		p_default,		0.0f,
		p_range, 		-999999999.0f, 999999999.0f, 
		p_end,

	uvw_scale_u,		_T("uscale"),	TYPE_FLOAT,	P_ANIMATABLE,	IDS_TL_U_SCALE,
		p_default,		1.0f,
		p_range, 		-999999999.0f, 999999999.0f, 
		p_end,

	uvw_move_v,			_T("vmove"),	TYPE_FLOAT,	P_ANIMATABLE,	IDS_TL_V_MOVE,
		p_default,		0.0f,
		p_range, 		-999999999.0f, 999999999.0f, 
		p_end,

	uvw_rotate_v,		_T("vrotatee"),	TYPE_FLOAT,	P_ANIMATABLE,	IDS_TL_V_ROTATE,
		p_default,		0.0f,
		p_range, 		-999999999.0f, 999999999.0f, 
		p_end,

	uvw_scale_v,		_T("vscale"),	TYPE_FLOAT,	P_ANIMATABLE,	IDS_TL_V_SCALE,
		p_default,		1.0f,
		p_range, 		-999999999.0f, 999999999.0f, 
		p_end,

	uvw_move_w,			_T("wmove"),	TYPE_FLOAT,	P_ANIMATABLE,	IDS_TL_W_MOVE,
		p_default,		0.0f,
		p_range, 		-999999999.0f, 999999999.0f, 
		p_end,

	uvw_rotate_w,		_T("wrotate"),	TYPE_FLOAT,	P_ANIMATABLE,	IDS_TL_W_ROTATE,
		p_default,		0.0f,
		p_range, 		-999999999.0f, 999999999.0f, 
		p_end,

	uvw_scale_w,		_T("wscale"),	TYPE_FLOAT,	P_ANIMATABLE,	IDS_TL_W_SCALE,
		p_default,		1.0f,
		p_range, 		-999999999.0f, 999999999.0f, 
		p_end,

	uvw_xform_att,		_T("xform_att"),TYPE_BOOL, 	0,	IDS_SIMPLE,
		p_default, 		FALSE, 
		p_end, 

	uvw_pelt_rigidity, 	_T("pelt_rigidity"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_TL_PELT_K, 
		p_default, 		0.05f, 
		p_range, 		0.0f,1000.0f, 
		p_end,

	uvw_pelt_stability, _T("pelt_stability"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_TL_PELT_B, 
		p_default, 		0.5f, 
		p_range, 		0.0f,1000.0f, 
		p_end,

	uvw_pelt_frame, 	_T("pelt_frame"), 	TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_TL_PELT_FRAME, 
		p_default, 		10.0f, 
		p_range, 		0.0f,1000.0f, 
		p_end,

	uvw_pelt_iter, 		_T("pelt_iter"), 	TYPE_INT, 	P_ANIMATABLE, 	IDS_TL_PELT_ITER, 
		p_default, 		500, 
		p_range, 		0,100000, 
		p_end,

	uvw_pelt_border,	_T("pelt_move_border"),	TYPE_BOOL, 	0,	IDS_SIMPLE,
		p_default, 		FALSE, 
		p_end, 

	uvw_frame_node, 	_T("frame_object"), 		TYPE_INODE, 	0,		IDS_SIMPLE,
		p_end, 

	uvw_spline_belt,	_T("spline_belt"),	TYPE_BOOL, 	0,	IDS_SIMPLE,
		p_default, 		FALSE, 
		p_end, 

	uvw_normals_start, 	_T("normals_start"), 	TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_TL_SPL_NORMALS_START, 
		p_default, 		0.0f, 
		p_range, 		-1.0f,1.0f, 
		p_end,

	uvw_active_status,	_T("active_status"),	TYPE_BOOL, 	0,	IDS_SIMPLE,
		p_default, 		TRUE, 
		p_end, 

	uvw_mapping_type,	_T("mapping_type"),	TYPE_INT, 	0,	IDS_SIMPLE,
		p_default, 		0, 
		p_end, 

	uvw_spline_node,	_T("spline_node"),	TYPE_INODE, 	0,	IDS_SIMPLE,
		p_sclassID,		SHAPE_CLASS_ID, 
		p_end, 
	p_end
	);

inline void XForm(Point3 &uvw, float &umove, float &vmove, float &wmove, float &urotate, float &vrotate, float &wrotate, float &uscale, float &vscale, float &wscale ) {
	uvw = uvw - Point3(0.5f,0.5f,0.0f);
#if MAX_VERSION_MAJOR < 24
	Matrix3 m(1);
#else
	Matrix3 m;
#endif

	m.SetTrans(Point3(0.5f,0.5f,0.0f));
	m.PreRotateX(PI*urotate/180.0f);
	m.PreRotateY(PI*vrotate/180.0f);
	m.PreRotateZ(PI*wrotate/180.0f);
	uvw = m * uvw;
	uvw.x = ( uvw.x * uscale ) + umove;
	uvw.y = ( uvw.y * vscale ) + vmove;
	uvw.z = ( uvw.z * wscale ) + wmove;
	}

inline void UnXForm(Point3 &uvw, float &umove, float &vmove, float &wmove, float &urotate, float &vrotate, float &wrotate, float &uscale, float &vscale, float &wscale ) {
	uvw.x = ( uvw.x - umove ) / uscale;
	uvw.y = ( uvw.y - vmove ) / vscale;
	uvw.z = ( uvw.z - wmove ) / wscale;


#if MAX_VERSION_MAJOR < 24
	Matrix3 m(1);
#else
	Matrix3 m;
#endif

	m.SetTrans(Point3(0.5f,0.5f,0.0f));
	m.PreRotateX(PI*urotate/180.0f);
	m.PreRotateY(PI*vrotate/180.0f);
	m.PreRotateZ(PI*wrotate/180.0f);
	m.Invert();
	uvw = m * uvw;
	
	uvw = uvw + Point3(0.5f,0.5f,0.0f);
	}

UVWProyector::UVWProyector(BOOL create) {

	pblock = NULL;
	tmControl = NULL;
	spline = NULL;
	gm = NULL;

	skip_bug = FALSE;

	id = rand();
	
	uvwProyClassDesc.MakeAutoParamBlocks(this);
	assert(pblock);

	applying_mapping = FALSE;

	valid_group = 0;

	tl = NULL;

	descCanal = _T("");

	segLens.SetCount(0);
	segPers.SetCount(0);
	splLen = 0.0f;

	if (create) flags = CONTROL_CENTER|CONTROL_FIT|CONTROL_INIT;
	else flags = 0;

	GridMesh *newgm = new GridMesh(0);
	ReplaceReference( 3, newgm );

	tileCrv.Reset(1.0f,0.0f,0.0f,MIN_FLAG);
	wTileCrv.Reset(1.0f,0.0f,0.0f,MIN_FLAG);
	angleCrv.Reset(0.0f,1.0f,-1.0f,0);

	pelt_poly_mesh = NULL;
	vert_to_mass.SetCount(0);
	masses.SetCount(0);
	border_verts.SetCount(0);
	frame_segments.SetCount(0);
	frame_segments_symmetry.SetCount(0);

	symmetry_axis = -1;

	xscroll = 0;
	yscroll = 0;
	zoom = 1.0f;

	use_old_ffm = FALSE;

	spline_node = NULL;

	current_num_faces = 0;
	current_num_edges = 0;
	current_num_verts = 0;
	current_num_sel_faces = 0;
	current_num_sel_edges = 0;

	force_apply = TRUE;
	}

UVWProyector::~UVWProyector() {
	DeleteAllRefsFromMe();

	spline = NULL;

	segLens.SetCount(0);
	segPers.SetCount(0);
	vectors.SetCount(0);
	tileG.SetCount(0);
	tGsel.SetSize(0);

	PeltDeleteUV();
	PeltDeleteFrameSegments();
	}

void UVWProyector::Update(TimeValue t) {

	force_apply = !ivalid.InInterval(t);

	ivalid = FOREVER;

	if (GetMappingType() == MAP_TL_FFM) 
		gm->Update(t);

	pblock->GetValue(uvw_map_channel,t,map_channel,ivalid);
	pblock->GetValue(uvw_length,t,gizmo_length,ivalid);
	pblock->GetValue(uvw_width,t,gizmo_width,ivalid);
	pblock->GetValue(uvw_height,t,gizmo_height,ivalid);
	pblock->GetValue(uvw_tile_u,t,tile_u,ivalid);
	pblock->GetValue(uvw_tile_v,t,tile_v,ivalid);
	pblock->GetValue(uvw_tile_w,t,tile_w,ivalid);
	pblock->GetValue(uvw_offset_u,t,offset_u,ivalid);
	pblock->GetValue(uvw_offset_v,t,offset_v,ivalid);
	pblock->GetValue(uvw_offset_w,t,offset_w,ivalid);
	pblock->GetValue(uvw_att,t,attGl,ivalid);
	pblock->GetValue(uvw_atton,t,attOn,ivalid);
	pblock->GetValue(uvw_aus,t,attUs,ivalid);
	pblock->GetValue(uvw_aue,t,attUo,ivalid);
	pblock->GetValue(uvw_avs,t,attVs,ivalid);
	pblock->GetValue(uvw_ave,t,attVo,ivalid);
	pblock->GetValue(uvw_aws,t,attWs,ivalid);
	pblock->GetValue(uvw_awe,t,attWo,ivalid);
	pblock->GetValue(uvw_aruv,t,attRad,ivalid);
	pblock->GetValue(uvw_reverse,t,reverse,ivalid);
	pblock->GetValue(uvw_normlize,t,norm,ivalid);
	pblock->GetValue(uvw_spline_belt,t,belt,ivalid);
	pblock->GetValue(uvw_move_u,t,move_u,ivalid);
	pblock->GetValue(uvw_move_v,t,move_v,ivalid);
	pblock->GetValue(uvw_move_w,t,move_w,ivalid);
	pblock->GetValue(uvw_rotate_u,t,rotate_u,ivalid);
	pblock->GetValue(uvw_rotate_v,t,rotate_v,ivalid);
	pblock->GetValue(uvw_rotate_w,t,rotate_w,ivalid);
	pblock->GetValue(uvw_scale_u,t,scale_u,ivalid);
	pblock->GetValue(uvw_scale_v,t,scale_v,ivalid);
	pblock->GetValue(uvw_scale_w,t,scale_w,ivalid);
	pblock->GetValue(uvw_xform_att,t,xform_att,ivalid);
	pblock->GetValue(uvw_normals_start,t,normals_start,ivalid);
	pblock->GetValue(uvw_active_status,t,active_status,ivalid);
	pblock->GetValue(uvw_mapping_type,t,mapping_type,ivalid);
	pblock->GetValue(uvw_spline_node,t,spline_node,ivalid);
	pblock->GetValue(uvw_frame_node,t,frame_node,ivalid);
	pblock->GetValue(uvw_pelt_border,t,pelt_border,FOREVER);

	}

Class_ID UVWProyector::ClassID() {
	return UVW_PROY_CID; 
	}

SClass_ID UVWProyector::SuperClassID() {
	return UVW_SCID;
	}

Interval UVWProyector::LocalValidity(TimeValue t) {
	if (tmControl) {

#if MAX_VERSION_MAJOR < 24
		Matrix3 tm(1);
#else
		Matrix3 tm;
#endif

		tmControl->GetValue(t,&tm,ivalid,CTRL_RELATIVE);
		}

	if (GetMappingType() == MAP_TL_SPLINE && spline_node) {
		ObjectState splOS = spline_node->EvalWorldState(t);
		Interval splIv = splOS.obj->ObjectValidity(t);
		ivalid = ivalid&splIv;
		}

	Interval gmIval = gm->LocalValidity(t);
	ivalid = ivalid&gmIval;

	return ivalid;
	}

// Bezier Simplified form
Point3 BezPoint(float k, Point3 q1, Point3 q2, Point3 q3) {
	float s = 1.0f - k;
	float k2 = k * k;
	return (3.0f*s*(k*s*q1+k2*q2)+k*k2*q3);
	}

// Which, in all the Shape object, is the nearest point to our punto?
#define JMAX 20 // MAX number of Newton Raphson iterations..
Point3 UVWProyector::NearestSplinePoint(Point3 punto, TimeValue t)
{
	if (!spline_node) return Point3(BIGFLOAT,BIGFLOAT,BIGFLOAT);
	float xacc = 0.000001f; // Newton Raphson Accuracy...
	float x1,x2;			// Newton Raphson limits...
	float rtn;				// U point in the segment.  Newton Raphson variable..
	float dx;				// Newton Raphson Delta value

	float distSqr = BIGFLOAT, auxDist;		// Distance variables...
	float near_sub_seg_dist = BIGFLOAT;
	IPoint2 near_sub_seg(-1,-1);

	BOOL nearP = FALSE;		// This method works with the tangenst, and maybe our punto
							// is not in a 'near tangent zone', so its nearest point is one
							// of the multiple 'open' points of our Bezier Shape.

	int zone = 0;			// To which zone of the spline are we nearest located.
							// 0 :  Inside the spline
							// 1 :  Before the first point of the spline.
							// 2 :  After the last point of the spline.
	
	// JW Code Change : fix uninitialized locals warning
	float prLen=0.0;
	Point3 pP;

	Point3 p0,p1,p2,p3;
	Point3 q1,q2,q3,qt;

	int num_segments = bezier_spline.NumSegments();

	int spl = 0;
	int seg = 0;
	float u = 0.0f;

	for ( int j=0; j < num_segments; j++ )
	{
		p0 = bezier_spline.GetKnot( j );
		p1 = bezier_spline.GetOutVec( j );

		if ( bezier_spline.closed && j==bezier_spline.NumSegments()-1 )
		{
			p2 = bezier_spline.GetInVec(0);
			p3 = bezier_spline.GetKnot(0);
		}
		else
		{
			p2 = bezier_spline.GetInVec(j+1);
			p3 = bezier_spline.GetKnot(j+1);
		}

		q1 = p1 - p0;
		q2 = p2 - p0;
		q3 = p3 - p0;
		qt = punto - p0;

		float a,b,c,d,e,f;
		f = 3.0f * (qt.y*q1.y + qt.x*q1.x + qt.z*q1.z);
		e = qt.y * (6.0f*q2.y-12.0f*q1.y) + qt.z * (6.0f*q2.z-12.0f*q1.z) + qt.x * (6.0f*q2.x-12.0f*q1.x) - 9.0f * (q1.x*q1.x + q1.y*q1.y + q1.z*q1.z);
		d = qt.y * (9.0f*(q1.y-q2.y) + 3.0f*q3.y) + qt.z * (9.0f*(q1.z-q2.z) + 3.0f*q3.z) + qt.x * (9.0f*(q1.x-q2.x) + 3.0f*q3.x) + 54.0f * (q1.x*q1.x + q1.y*q1.y + q1.z*q1.z) - 27.0f * (q1.y*q2.y + q1.x*q2.x + q1.z*q2.z);
		c = q1.y * (108.0f*(q2.y-q1.y) - 12.0f*q3.y) + q1.x * (108.0f*(q2.x-q1.x) - 12.0f*q3.x) + q1.z * (108.0f*(q2.z-q1.z) - 12.0f*q3.z) - 18.0f * (q2.x*q2.x + q2.y*q2.y + q2.z*q2.z);
		b = q1.x * (90.0f*q1.x - 135.0f*q2.x + 30.0f*q3.x) + q1.y * (90.0f*q1.y - 135.0f*q2.y + 30.0f*q3.y) + q1.z * (90.0f*q1.z - 135.0f*q2.z + 30.0f*q3.z) + 45.0f * (q2.x*q2.x + q2.y*q2.y + q2.z*q2.z) - 15.0f * (q2.x*q3.x + q2.y*q3.y + q2.z*q3.z);
		a = q1.x * (54.0f*q2.x - 27.0f*q1.x - 18.0f*q3.x) + q1.y * (54.0f*q2.y - 27.0f*q1.y - 18.0f*q3.y) + q1.z * (54.0f*q2.z - 27.0f*q1.z - 18.0f*q3.z) - 3.0f*(q3.x*q3.x + q3.y*q3.y + q3.z*q3.z) + 18.0f * (q2.x*q3.x + q2.y*q3.y + q2.z*q3.z) - 27.0f * (q2.x*q2.x + q2.y*q2.y + q2.z*q2.z);

		for ( int sb = 0; sb < 8; sb++ )
		{
			x1 = float(sb) * 0.125f;
			x2 = (float(sb) + 1.0f) * 0.125f;

			// Descartamos los semisegmentos donde no puede haber tangente.
			if ( ((f+x1*(e+x1*(d+x1*(c+x1*(b+x1*(a+x1))))))*(f+x2*(e+x2*(d+x2*(c+x2*(b+x2*(a+x2))))))) < 0.0f )
			{
				// Aqui adentro debemos buscar la tangente con Newton Raphson...
				rtn=0.5f*(x1+x2); //Initial Guess
				for ( int jit = 1; jit <= JMAX; jit++ )
				{
					dx=(f+rtn*(e+rtn*(d+rtn*(c+rtn*(b+rtn*(a+rtn)))))) / (e+rtn*(2.0f*d+rtn*(3.0f*c+rtn*(4.0f*b+rtn*(5.0f*a+rtn)))));
					rtn -= dx;
					if (fabs(dx) < xacc)
					{
						if ((x1-rtn)*(rtn-x2) < 0.0)
						{
							jit = JMAX + 1;
						}
					} // NR Success...
				}

				auxDist = LengthSquared(BezPoint(rtn,q1,q2,q3)-qt);

				if (auxDist < distSqr)
				{
					nearP = TRUE;
					zone = 0;
					distSqr = auxDist;
					spl = 0;
					seg = j;
					u = rtn;
				} // if (auxDist < distsqr
			} // if (f+x1.. 

			auxDist = 0.5f * ( LengthSquared(BezPoint(x1,q1,q2,q3)-qt) + LengthSquared(BezPoint(x2,q1,q2,q3)-qt) );
			if ( auxDist<near_sub_seg_dist ) 
			{
				near_sub_seg_dist = auxDist;
				near_sub_seg = IPoint2(j,sb);
			}

		} // for (sb=0->8)
	} // for(numSegments)

	//El crash esta por aqui!

	Point3 aux_tan;
	Point3 aux_nv;
	Point3 aux_pto = Isp(spl,seg,u,aux_tan,aux_nv,t);
	aux_tan = Normalize( aux_tan );
	Point3 dir = Normalize( punto - aux_pto );
	float ang = acos( DotProd(dir,aux_tan) ) - HALFPI;

	if ( fabs(ang)>0.087266 || near_sub_seg_dist<distSqr ) {

		for ( int j=0; j<num_segments; j++ ) {
			float pos = 0.0f;

			q1 = bezier_spline.GetOutVec( j ) - bezier_spline.GetKnot( j );
			if ( bezier_spline.closed && j==num_segments-1 ) {
				q2 = bezier_spline.GetInVec(0) - bezier_spline.GetKnot( j );
				q3 = bezier_spline.GetKnot(0) - bezier_spline.GetKnot( j );
				}
			else {
				q2 = bezier_spline.GetInVec(j+1) - bezier_spline.GetKnot( j );
				q3 = bezier_spline.GetKnot(j+1) - bezier_spline.GetKnot( j );
				}
			qt = punto - bezier_spline.GetKnot( j );

			Point3 sp0;
			Point3 sp1;
			int num_steps = 1024;
			for ( int i=0; i<=num_steps; i++ ) {
				float x1 = pos + float(i) / float(num_steps);//0.125f/16.0f;
				auxDist = LengthSquared(BezPoint(x1,q1,q2,q3)-qt);
				if ( auxDist<distSqr ) {
					nearP = TRUE;
					zone = 0;
					distSqr = auxDist;
					spl = 0;
					seg = j;
					u = x1;
					}
				}
			}
		}

	// Aqui extendemos el tiling mas alla de las fronteras del spline (si este
	// es abierto)
	if ( !bezier_spline.closed ) {
		Point3 vi = bezier_spline.GetKnot(0) - bezier_spline.GetOutVec( 0 );
		Point3 pu = punto - bezier_spline.GetKnot( 0 );

		Point3 vin = Normalize(vi);
		Point3 pun = Normalize(pu);
		if (DotProd(vin,pun) > 0.0f) {
			prLen = DotProd(pu,vi) / Length(vi);
			Point3 proy = vin * prLen;
			pP = pu - proy;
			auxDist = Length(pP);
			auxDist *= auxDist;
			if (auxDist < distSqr) {
				nearP = TRUE;
				zone = 1;
				distSqr = auxDist;
				u = Length(proy);
				seg = 0;
				}
			}

		Point3 vf = bezier_spline.GetKnot( num_segments ) - bezier_spline.GetInVec( num_segments );
		pu = punto - bezier_spline.GetKnot( num_segments );

		Point3 vfn = Normalize(vf);
		pun = Normalize(pu);
		if (DotProd(vfn,pun) > 0.0f) {
			prLen = DotProd(pu,vf) / Length(vf);
			Point3 proy = vfn * prLen;
			pP = pu - proy;
			auxDist = Length(pP);
			auxDist *= auxDist;
			if (auxDist < distSqr) {
				nearP = TRUE;
				zone = 2;
				distSqr = auxDist;
				u = Length(proy);
				seg = num_segments-1;
				}
			}
		}
	// JW Code Change : fix uninitialized locals warning
	float U=0.0,V=0.0,W=0.0;
	if (nearP) {
		Point3 tan,nV,nxt;
		if (zone == 0) {
			Point3 pto = Isp(spl,seg,u,tan,nV,t);

			if (norm) {
				if (seg > 0) V = segPers[seg-1] + (segPers[seg] - segPers[seg-1]) * u;
				else V = segPers[seg] * u;
				}
			else {
				V = (float(seg) + u) / float(num_segments);
				}

			tan = Normalize(tan);
			nV  = Normalize(nV);

			nxt = nV ^ tan;
			float ang = (normals_start + angleCrv.GetValue(V)) * PI;
			nV = nV * float(cos(ang)) + nxt * float(sin(ang));

			Point3 dir = punto - pto;
			W = Length(dir);

			dir /= W;

			float dp = DotProd(nV,dir);
			if (dp > 0.999999f) dp = 0.999999f;
			if (dp < -0.999999f) dp = -0.999999f;
			float u1;
			u1 = (float)acos(dp) / TWOPI;

			if ( DotProd(CrossProd(nV,dir),tan) < 0.0f ) {
				U = 1.0f - u1;
				}
			else {
				U = u1;
				}
			}
		else if (zone == 1) {
			Point3 pto = Isp(0,seg,0.0f,tan,nV,t);

			tan = Normalize(tan);
			nV	= Normalize(nV);

			V = - prLen * segPers[0] / segLens[0];

			nxt = nV ^ tan;
			float ang = (normals_start + angleCrv.GetValue(V)) * PI;
			nV = nV * float(cos(ang)) + nxt * float(sin(ang));

			W = float(sqrt(distSqr));

			Point3 dir = pP/W;

			float dp = DotProd(nV,dir);
			if (dp > 0.999999f) dp = 0.999999f;
			float u1 = (float)acos(dp) / TWOPI;

			if ( DotProd(CrossProd(nV,dir),tan) < 0.0f ) {
				U = 1.0f - u1;
				}
			else {
				U = u1;
				}

			}
		else if (zone == 2) {
			Point3 pto = Isp(0,seg,1.0f,tan,nV,t);

			tan = Normalize(tan);
			nV  = Normalize(nV);

			V = 1.0f + prLen * segPers[seg] / segLens[seg];

			nxt = nV ^ tan;
			float ang = (normals_start + angleCrv.GetValue(V)) * PI;
			nV = nV * float(cos(ang)) + nxt * float(sin(ang));

			W = float(sqrt(distSqr));

			Point3 dir = pP/W;

			float dp = DotProd(nV,dir);
			if (dp > 0.999999f) dp = 0.999999f;
			float u1 = (float)acos(dp) / TWOPI;

			if ( DotProd(CrossProd(nV,dir),tan) < 0.0f ) {
				U = 1.0f - u1;
				}
			else {
				U = u1;
				}
			}
		}
	else {
	//	return Point3(0,0,0);
		}

	if ( belt ) {
		float ang = U * TWOPI + HALFPI;
		float dist = W;

		U = 0.5f - ( dist * cos(ang) / ( 2.0f * gizmo_length ) );	
		W = 0.5f - ( dist * sin(ang) / ( 2.0f * gizmo_height ) );
		}

	if (reverse) 
		V = 1.0f - V;
	
	return Point3(U,V,W);
	}

Point3 CalcBaryCoords(Point3 p0, Point3 p1, Point3 p2, Point3 p)
{ 
	Point3 bary;

	Point3 abc = (p1-p0)^(p2-p0);
	Point3 pbc = (p1-p)^(p2-p);
	Point3 apc = (p-p0)^(p2-p0);
	Point3 abp = (p1-p0)^(p-p0);

	float a_abc = 1.0f / Length(abc);

	float a_pbc = Length(pbc);
	float a_apc = Length(apc);
	float a_abp = Length(abp);

	if (DotProd(abc,pbc) >= 0.0f ) bary.x = a_abc * a_pbc;
	else bary.x = - a_abc * a_pbc;
	if (DotProd(abc,apc) >= 0.0f ) bary.y = a_abc * a_apc;
	else bary.y = - a_abc * a_apc;
	if (DotProd(abc,abp) >= 0.0f ) bary.z = a_abc * a_abp;
	else bary.z = - a_abc * a_abp;
	
	return bary;
}

// NearestGridPoint
//  This function maps a point to a grid
Point3 UVWProyector::NearestGridPointOld(Point3 punto, TimeValue t) {
	int nv = gm->verts.Count();
	int nf = gm->faces.Count();
	float auxd,fauxd,dist = BIGFLOAT;
	float gridDist = BIGFLOAT;
	float cornerDist = BIGFLOAT;
	float borderDist = BIGFLOAT;
	float grcrDist = BIGFLOAT;
	Point3 bc;
	BOOL inGrid = FALSE;
	BOOL inCorner = FALSE;
	int face = 0;
	int hit = 0;
	int hitBorde = 0;

	Point3 uvw,uvwB,uvwGC;

	float bcmin = 0.0f - GetFFMThresh(t);
	float bcmax = 1.0f + GetFFMThresh(t);

	int nvx = gm->nx;
	int nvy = gm->ny;
	int npx = gm->nx-1;
	int npy = gm->ny-1;

	for (int f=0; f<nf; f++) {
		auxd = (DotProd(gm->fnormals[f],punto) + gm->coefD[f]) * gm->den[f];
		fauxd = float(fabs(auxd));

		if (fauxd < dist) {
			Point3 pt[3];
			pt[0] = gm->verts[gm->faces[f].x] + (auxd / gm->cosangs[f].x) * gm->vnormals[gm->faces[f].x];
			pt[1] = gm->verts[gm->faces[f].y] + (auxd / gm->cosangs[f].y) * gm->vnormals[gm->faces[f].y];
			pt[2] = gm->verts[gm->faces[f].z] + (auxd / gm->cosangs[f].z) * gm->vnormals[gm->faces[f].z];

			Point3 baryCoord = CalcBaryCoords(pt[0],pt[1],pt[2],punto);

			if (baryCoord.x >= bcmin && baryCoord.x <= bcmax)
				if (baryCoord.y >= bcmin && baryCoord.y <= bcmax)
					if (baryCoord.z >= bcmin && baryCoord.z <= bcmax) {
						dist = fauxd;
						gridDist = dist;
						bc = baryCoord;
						face = f;
						hit = 1;
						}
			if (
				(f==0 && baryCoord.x < 0.0f && baryCoord.y > 1.0f && baryCoord.z < 0.0f) ||
				(f==1 && baryCoord.x < 0.0f && baryCoord.y < 0.0f && baryCoord.z > 1.0f) ||
				(f==(npx*2 - 1) && baryCoord.x > 1.0f && baryCoord.y < 0.0f && baryCoord.z < 0.0f) ||
				(f==2*npx*(npy-1) && baryCoord.x > 1.0f && baryCoord.y < 0.0f && baryCoord.z < 0.0f) ||
				(f==2*npx*npy-1 && baryCoord.x < 0.0f && baryCoord.y > 1.0f && baryCoord.z < 0.0f) ||
				(f==2*npx*npy-2 && baryCoord.x < 0.0f && baryCoord.y < 0.0f && baryCoord.z > 1.0f)
				) {
				dist = fauxd;
				cornerDist = dist;
				bc = baryCoord;
				face = f;
				hit = 1;
				}
			}
		}


	if (hit) {
		if (gridDist < cornerDist) {
			inGrid = TRUE;
			inCorner = FALSE;
			grcrDist = gridDist;
			if (face%2) {
				Point3 v0 = gm->verts[gm->faces[face].x];
				Point3 v1 = gm->verts[gm->faces[face].y];
				Point3 v2 = gm->verts[gm->faces[face].z];
				Point3 v3 = gm->verts[gm->faces[face-1].x];
				Point3 point = bc.x * v0 + bc.y * v1 + bc.z * v2;
				Point3 pau = (1.0f - bc.y) * v0 + bc.y * v1;
				Point3 pbu = bc.y * v1 + (1.0f - bc.y) * v2;
				Point3 pcu = v2 * (1.0f - bc.y) + v3 * bc.y;
				Point3 pav = (1.0f - bc.z) * v0 + bc.z * v2;
				Point3 pbv = (1.0f - bc.z) * v1 + bc.z * v2;
				Point3 pcv = (1.0f - bc.z) * v1 + bc.z * v3;
				float urel = 1.0f - (Length(point-pau) / (Length(pcu-pbu)+Length(pau-pbu)));
				float vrel = Length(point - pav) / (Length(pav-pbv) + Length(pcv-pbv));
				uvwGC = Point3( gm->uvw[gm->faces[face].z].x * (1.0f - urel) + gm->uvw[gm->faces[face].x].x * urel,
								gm->uvw[gm->faces[face].x].y * (1.0f - vrel) + gm->uvw[gm->faces[face].y].y * vrel,
								0.0f);
				}
			else {
				Point3 v0 = gm->verts[gm->faces[face].x];
				Point3 v1 = gm->verts[gm->faces[face].y];
				Point3 v2 = gm->verts[gm->faces[face].z];
				Point3 v3 = gm->verts[gm->faces[face+1].x];
				Point3 point = bc.x * v0 + bc.y * v1 + bc.z * v2;

				Point3 pau = (1.0f - bc.y) * v0 + bc.y * v1;
				Point3 pbu = bc.y * v1 + (1.0f - bc.y) * v2;
				Point3 pcu = v2 * (1.0f - bc.y) + v3 * bc.y;

				Point3 pav = (1.0f - bc.z) * v0 + bc.z * v2;
				Point3 pbv = (1.0f - bc.z) * v1 + bc.z * v2;
				Point3 pcv = (1.0f - bc.z) * v1 + bc.z * v3;
				float urel = Length(point - pau) / (Length(pcu-pbu)+Length(pbu-pau));
				float vrel = 1.0f - (Length(point - pav) / (Length(pav-pbv) + Length(pcv-pbv)));
				uvwGC = Point3( gm->uvw[gm->faces[face].x].x * (1.0f - urel) + gm->uvw[gm->faces[face].z].x * urel,
								gm->uvw[gm->faces[face].y].y * (1.0f - vrel) + gm->uvw[gm->faces[face].x].y * vrel,
								0.0f);
				}
			}
		else {
			inGrid = FALSE;
			inCorner = TRUE;
			grcrDist = cornerDist;
			uvwGC = bc.x * gm->uvw[gm->faces[face].x] + 
					bc.y * gm->uvw[gm->faces[face].y] +
					bc.z * gm->uvw[gm->faces[face].z];
			}
		}
	
	dist = BIGFLOAT;

	int v0=0,v1 = 0,v2 = 0,v3 = 0,sector = 0;	// JW: initialized to 0
	int vv0 = 0,vv1 = 0,vv2 = 0,vv3 = 0;

	int borde = 0;
	float bvdist = BIGFLOAT;
	float auxvd = 0;

	// Revisamos los 4 bordes para ver a donde nos acercamos mas...
	// Este calculo de la distancia es hacia el borde, no hacia la generatriz de la
	// superficie que le corresponde.

	// Borde inferior
	for (int i=0; i<nvx-1; i++) {
		Point3 p0 = gm->verts[i];
		Point3 p1 = gm->verts[i+1];
		Point3 u = punto - p0;
		Point3 v = p1 - p0;

		float proy = DotProd(u,v)/LengthSquared(v);

		if (proy <= bcmax && proy >= bcmin)  {
			auxd = LengthSquared(p0+proy*v - punto);
			if (auxd < dist) {
				dist = auxd;
				v0 = i;		v1 = i+1;		v2 = v0 + nvx;		v3 = v1 + nvx;
				sector = 1;
				}
			}
		auxvd = LengthSquared(punto - p0);
		if (auxvd < bvdist) {
			bvdist = auxvd;
			borde = 1;
			vv0 = i;		vv1 = i+1;		vv2 = vv0 + nvx;		vv3 = vv1 + nvx;
			}

		if (i==nvx-2){
			auxvd = LengthSquared(punto - p1);
			if (auxvd < bvdist) {
				bvdist = auxvd;
				borde = 1;
				vv0 = i;		vv1 = i+1;		vv2 = vv0 + nvx;		vv3 = vv1 + nvx;
				}
			}

		}

	// Borde superior
	for ( int i=(nvx*(nvy-1)); i<(nvx*nvy)-1; i++) {
		Point3 p0 = gm->verts[i];
		Point3 p1 = gm->verts[i+1];
		Point3 u = punto - p0;
		Point3 v = p1 - p0;

		float proy = DotProd(u,v)/LengthSquared(v);

		if (proy <= bcmax && proy >= bcmin)  {
			auxd = LengthSquared(p0+proy*v - punto);
			if (auxd < dist) {
				dist = auxd;
				v0 = i;		v1 = i+1;		v2 = v0 - nvx;		v3 = v1 - nvx;
				sector = 1;
				}
			}
		auxvd = LengthSquared(punto - p0);
		if (auxvd < bvdist) {
			bvdist = auxvd;
			borde = 2;
			vv0 = i;		vv1 = i+1;		vv2 = vv0 - nvx;		vv3 = vv1 - nvx;
			}

		if (i==(nvx*nvy)-2){
			auxvd = LengthSquared(punto - p1);
			if (auxvd < bvdist) {
				bvdist = auxvd;
				borde = 2;
				vv0 = i;		vv1 = i+1;		vv2 = vv0 - nvx;		vv3 = vv1 - nvx;
				}
			}
		}

	// Borde izquierdo
	for ( int i=0; i<(nvx*nvy)-nvx; i=i+nvx) {
		Point3 p0 = gm->verts[i];
		Point3 p1 = gm->verts[i+nvx];
		Point3 u = punto - p0;
		Point3 v = p1 - p0;

		float proy = DotProd(u,v)/LengthSquared(v);

		if (proy <= bcmax && proy >= bcmin)  {
			auxd = LengthSquared(p0+proy*v - punto);
			if (auxd < dist) {
				dist = auxd;
				v0 = i;			v1 = i+nvx;		v2 = v0 + 1;		v3 = v1 + 1;
				sector = 2;
				}
			}
		auxvd = LengthSquared(punto - p0);
		if (auxvd < bvdist) {
			bvdist = auxvd;
			borde = 3;
			vv0 = i;		vv1 = i+nvx;		vv2 = vv0 + 1;		vv3 = vv1 + 1;
			}

		if (i==(nvx*nvy)-2*nvx){
			auxvd = LengthSquared(punto - p1);
			if (auxvd < bvdist) {
				bvdist = auxvd;
				borde = 3;
				vv0 = i;		vv1 = i+nvx;		vv2 = vv0 + 1;		vv3 = vv1 + 1;
				}
			}
		}
	// Borde derecho
	for ( int i=nvx-1; i<(nvx*nvy)-nvx; i=i+nvx) {
		Point3 p0 = gm->verts[i];
		Point3 p1 = gm->verts[i+nvx];
		Point3 u = punto - p0;
		Point3 v = p1 - p0;

		float proy = DotProd(u,v)/LengthSquared(v);

		if (proy <= bcmax && proy >= bcmin)  {
			auxd = LengthSquared(p0+proy*v - punto);
			if (auxd < dist) {
				dist = auxd;
				v0 = i;			v1 = i+nvx;		v2 = v0 - 1;		v3 = v1 - 1;
				sector = 2;
				}
			}
		auxvd = LengthSquared(punto - p0);
		if (auxvd < bvdist) {
			bvdist = auxvd;
			borde = 4;
			vv0 = i;		vv1 = i+nvx;		vv2 = vv0 - 1;		vv3 = vv1 - 1;
			}

		if (i==(nvx*nvy)-nvx-1){
			auxvd = LengthSquared(punto - p1);
			if (auxvd < bvdist) {
				bvdist = auxvd;
				borde = 4;
				vv0 = i;		vv1 = i+nvx;		vv2 = vv0 - 1;		vv3 = vv1 - 1;
				}
			}
		}

	// Analizamos si hay hit en un borde real....
	if (sector == 1) {
		Point3 p0,p1,vd0,vd1;
		p0 = gm->verts[v0];
		p1 = gm->verts[v1];

		vd0 = p0 - gm->verts[v2];
		vd1 = p1 - gm->verts[v3];

		Point3 u = punto - p0;
		Point3 v = p1 - p0;

		float proy = DotProd(u,v)/LengthSquared(v);

		uvwB.x = gm->uvw[v0].x + proy * (gm->uvw[v1].x - gm->uvw[v0].x);

		Point3 vd = vd0 + proy * (vd1 - vd0);
		float lvd = Length(vd);

		Point3 pm = p0 + proy * v;
		Point3 ppm = punto - pm;

		float lppm = Length(ppm);
		float ppmvd = DotProd(ppm,vd)/LengthSquared(vd);

		uvwB.y = gm->uvw[v0].y - ppmvd * (gm->uvw[v2].y - gm->uvw[v0].y);

		if (ppmvd < 0.0f) {
			borderDist = BIGFLOAT;
			}
		else {
			inGrid = FALSE;
			hit = 1;
			hitBorde = 1;
			borderDist = (float)sqrt(lppm*lppm - lvd*lvd*ppmvd*ppmvd);
			}
		}

	if (sector == 2) {
		if (sector == 0 && (borde == 3 || borde == 4)) {
			v0 = vv0;v1 = vv1; v2 = vv2; v3 = vv3;
			}

		Point3 p0,p1,vd0,vd1;
		p0 = gm->verts[v0];
		p1 = gm->verts[v1];

		vd0 = p0 - gm->verts[v2];
		vd1 = p1 - gm->verts[v3];

		Point3 u = punto - p0;
		Point3 v = p1 - p0;

		float proy = DotProd(u,v)/LengthSquared(v);

		uvwB.y = gm->uvw[v0].y + proy * (gm->uvw[v1].y - gm->uvw[v0].y);

		Point3 vd = vd0 + proy * (vd1 - vd0);
		float lvd = Length(vd);

		Point3 pm = p0 + proy * v;
		Point3 ppm = punto - pm;

		float lppm = Length(ppm);
		float ppmvd = DotProd(ppm,vd)/LengthSquared(vd);

		uvwB.x = gm->uvw[v0].x - ppmvd * (gm->uvw[v2].x - gm->uvw[v0].x);

		if (ppmvd < 0.0f) {
			borderDist = BIGFLOAT;
			}
		else {
			hit = 1;
			hitBorde = 1;
			inGrid = FALSE;
			borderDist = (float)sqrt(lppm*lppm - lvd*lvd*ppmvd*ppmvd);
			}
		}
	if (!hitBorde && (borde == 1 || borde == 2)) {
		v0 = vv0; v1 = vv1; v2 = vv2; v3 = vv3;

		Point3 p0,p1,vd0,vd1;
		p0 = gm->verts[v0];
		p1 = gm->verts[v1];

		vd0 = p0 - gm->verts[v2];
		vd1 = p1 - gm->verts[v3];

		Point3 u = punto - p0;
		Point3 v = p1 - p0;

		float proy = DotProd(u,v)/LengthSquared(v);

		uvwB.x = gm->uvw[v0].x + proy * (gm->uvw[v1].x - gm->uvw[v0].x);

		Point3 vd = vd0 + proy * (vd1 - vd0);
		float lvd = Length(vd);

		Point3 pm = p0 + proy * v;
		Point3 ppm = punto - pm;

		float lppm = Length(ppm);
		float ppmvd = DotProd(ppm,vd)/LengthSquared(vd);

		uvwB.y = gm->uvw[v0].y - ppmvd * (gm->uvw[v2].y - gm->uvw[v0].y);

		if (ppmvd < 0.0f) {
			borderDist = BIGFLOAT;
			}
		else {
			inGrid = FALSE;
			hit = 1;
			borderDist = (float)sqrt(lppm*lppm - lvd*lvd*ppmvd*ppmvd);
			}
		}

	if (!hitBorde && (borde == 3 || borde == 4)) {
		v0 = vv0;v1 = vv1; v2 = vv2; v3 = vv3;

		Point3 p0,p1,vd0,vd1;
		p0 = gm->verts[v0];
		p1 = gm->verts[v1];

		vd0 = p0 - gm->verts[v2];
		vd1 = p1 - gm->verts[v3];

		Point3 u = punto - p0;
		Point3 v = p1 - p0;

		float proy = DotProd(u,v)/LengthSquared(v);

		uvwB.y = gm->uvw[v0].y + proy * (gm->uvw[v1].y - gm->uvw[v0].y);

		Point3 vd = vd0 + proy * (vd1 - vd0);
		float lvd = Length(vd);

		Point3 pm = p0 + proy * v;
		Point3 ppm = punto - pm;

		float lppm = Length(ppm);
		float ppmvd = DotProd(ppm,vd)/LengthSquared(vd);

		uvwB.x = gm->uvw[v0].x - ppmvd * (gm->uvw[v2].x - gm->uvw[v0].x);

		if (ppmvd < 0.0f) {
			borderDist = BIGFLOAT;
			}
		else {
			hit = 1;
			inGrid = FALSE;
			borderDist = (float)sqrt(lppm*lppm - lvd*lvd*ppmvd*ppmvd);
			}
		}


	if (hit) {
		if (borderDist < grcrDist) {
			uvw = uvwB;
			uvw.z = borderDist/gizmo_height;
			}
		else {
			uvw = uvwGC;
			uvw.z = grcrDist/gizmo_height;
			}
		}

	if (!hit) {
		return Point3(BIGFLOAT,BIGFLOAT,BIGFLOAT);
		}
	return uvw;
	}

// NearestGridPoint
//  This function maps a point to a grid
Point3 UVWProyector::NearestGridPoint(Point3 punto, TimeValue t) {
	
	if ( use_old_ffm )
		return NearestGridPointOld(punto,t);

	int num_grid_faces = gm->faces.Count();
	float auxd,fauxd = BIGFLOAT;
	Point3 bc;
	Point3 pt[3];

	Point3 uvw_a, uvw_b;
	Point3 uvw;
	float avg_dist = BIGFLOAT;

	Point3 this_uvw;
	float this_avg_dist;

	int f;

	for ( f=0; f<num_grid_faces; f=f+2) {
		Point3 g_v[4];
		g_v[0] = gm->verts[gm->faces[f+0].x];
		g_v[1] = gm->verts[gm->faces[f+0].y];
		g_v[2] = gm->verts[gm->faces[f+0].z];
		g_v[3] = gm->verts[gm->faces[f+1].x];

		Point3 v_n[4];
		v_n[0] = gm->vnormals[gm->faces[f+0].x];
		v_n[1] = gm->vnormals[gm->faces[f+0].y];
		v_n[2] = gm->vnormals[gm->faces[f+0].z];
		v_n[3] = gm->vnormals[gm->faces[f+1].x];

		float c_v[4];
		c_v[0] = gm->cosangs[f+0].x;
		c_v[1] = gm->cosangs[f+0].y;
		c_v[2] = gm->cosangs[f+0].z;
		c_v[3] = gm->cosangs[f+1].x;

		Point3 uvw_v[4];
		uvw_v[0] = gm->uvw[gm->faces[f+0].x];
		uvw_v[1] = gm->uvw[gm->faces[f+0].y];
		uvw_v[2] = gm->uvw[gm->faces[f+0].z];
		uvw_v[3] = gm->uvw[gm->faces[f+1].x];

		for ( int i=0; i<2; i++ ) {
			int a,b,c;
			if ( i==0 ) {
				a = 0;	b = 1;	c = 2;
				}
			else {
				a = 3;	b = 2;	c = 1;
				}

			Point3 f_n = Normalize( (g_v[b]-g_v[a])^(g_v[c]-g_v[a]) );
			float coef = -DotProd(g_v[a],f_n);
			float den = 1.0f / Length(f_n);

			auxd = (DotProd(f_n,punto) + coef) * den;
			fauxd = float(fabs(auxd));

			pt[0] = g_v[a] + (auxd / c_v[a]) * v_n[a];
			pt[1] = g_v[b] + (auxd / c_v[b]) * v_n[b];
			pt[2] = g_v[c] + (auxd / c_v[c]) * v_n[c];

			bc = CalcBaryCoords(pt[0],pt[1],pt[2],punto);

			this_uvw = bc.x*uvw_v[a] + bc.y*uvw_v[b] + bc.z*uvw_v[c];
			this_avg_dist = ( Length(punto-g_v[a]) + Length(punto-g_v[b]) + Length(punto-g_v[c]) ) / 3.0f;

			if ( bc.x>=0.0  && bc.x<=1.0f && bc.y>=0.0f && bc.y<=1.0f && bc.z>=0.0f && bc.z<=1.0f ) {
				this_avg_dist = fauxd;
				}

			if ( this_avg_dist < avg_dist ) {
				uvw = this_uvw;
				uvw_a = this_uvw;
				avg_dist = this_avg_dist;
				}
			}
		}

	return uvw;
	}

static int LargestComponent(Point3 &p)
	{
	int l = 0;
	if (fabs(p.y)>fabs(p[l])) l = 1;
	if (fabs(p.z)>fabs(p[l])) l = 2;
	return l;
	}

int UVWProyector::GetSplNumSegs() {
	return bezier_spline.NumSegments();
	}

Point3 UVWProyector::GetUVWSplinePoint(TimeValue t, float u, float v, float w, Point3 &ns, Point3 &ne) {
	int i,seg=0;
	float k;

	if (reverse)
		v = 1.0f - v;

	int numsegs = GetSplNumSegs();
	if (v == 1.0f) v = 0.999999f; 

	if (norm) {
		for (i=0; i<numsegs; i++) {
			float spant;
			if (i == 0) spant = 0.0f;
			else spant = segPers[i-1];
			if (v > spant) {
				seg = i;
				}
			}
		if (numsegs!=1) {
			float spant;
			if (seg == 0) spant = 0.0f;
			else spant = segPers[seg-1];
			k = (v - spant)/(segPers[seg]-spant);
			}
		else {
			k = v;
			}
		}
	else {
		seg = int(v*numsegs);
		k = v*numsegs - float(seg);
		}

	Point3 tan,nV,pto;
	float prLen;

	if (v > 1.0f) {
		prLen = (v - 1.0f) * segLens[numsegs-1] / segPers[numsegs-1];
		pto = Isp(0,numsegs-1,1.0f,tan,nV,0);
		tan = Normalize(tan);
		nV  = Normalize(nV);
		pto = pto + tan * prLen;
		}
	else if (v < 0.0f) {
		prLen = v * segLens[0] / segPers[0];
		pto = Isp(0,0,0.0f,tan,nV,0);
		tan = Normalize(tan);
		nV  = Normalize(nV);
		pto = pto + tan * prLen;
	} else {
		pto = Isp(0,seg,k,tan,nV,0);
		tan = Normalize(tan);
		nV  = Normalize(nV);
		}

	Point3 nxt = nV ^ tan;
	float ang = (normals_start + angleCrv.GetValue(v)) * PI;
	nV = nV * float(cos(ang)) + nxt * float(sin(ang));
	nxt = nV ^ tan;

	float hv = v;
	if (reverse) hv = 1.0f - v;

	ns = pto;
	ne = ns + Normalize(nV) * gizmo_height * wTileCrv.GetValue(hv);

	if ( belt ) {
		float a = ( u - 0.5f ) * ( 2.0f * gizmo_length );
		float b = ( w - 0.5f ) * ( 2.0f * gizmo_height );
		float dist = sqrt( a*a + b*b );
		float ang = atan( b / a );
		if ( fabs(a)<0.000001f )
			ang = HALFPI;
		if ( a<0.0f ) {
			ang = PI + ang;
			}
		ang = ang - HALFPI;

		Point3 dir = nV*(float)cos(ang) + nxt*(float)sin(ang);

		pto = pto + dir * dist;
		}
	else {
		Point3 dir = nV*(float)cos(-TWOPI*u) + nxt*(float)sin(-TWOPI*u);
		dir = w * dir;
		pto = pto + dir;
		}

	return pto;
	}

Point3 UVWProyector::GetPosition(TimeValue t, float u, float v, float w, int gizmo) {
	// Primero retiro el Tiling-Offset
	// Remove tiling
	if (mapping_type == MAP_TL_SPLINE) {
		float tile = tileCrv.GetValue(v);
		float offset = (tile - 1.0f) / 2.0f;
		u = (u + offset)/tile;

		float wtile = wTileCrv.GetValue(v);
		float height = gizmo_height;
		if ( belt )
			height = 1.0f;
		w = w * height * wtile;
		}

	int uflip = 0, vflip = 0, wflip = 0;

	float utile = GetTileU(t);
	float vtile = GetTileV(t);
	float wtile = GetTileW(t);

	if (mapping_type == MAP_TL_SPHERICAL) {
		if (v < 0.001f) v = 0.001f;
		if (v > 0.999f) v = 0.999f;
		}

	if (utile < 0.0f) {
		uflip = 1;
		utile = (float)fabs(utile);
		}
	if (vtile < 0.0f) {
		vflip = 1;
		vtile = (float)fabs(vtile);
		}
	if (wtile < 0.0f) {
		wflip = 1;
		wtile = (float)fabs(wtile);
		}

	u = u + GetOffsetU(t);
	v = v + GetOffsetV(t);
	w = w + GetOffsetW(t);

	if (uflip) u = 1.0f - u;
	if (vflip) v = 1.0f - v;
	if (wflip) w = 1.0f - w;

	u = u/utile;
	v = v/vtile;
	w = w/wtile;

	// Luego paso de espacio UVW a XYZ
	Point3 p;

#if MAX_VERSION_MAJOR < 24
	Matrix3 tm(1);
#else
	Matrix3 tm;
#endif 

	switch (mapping_type) {
		case MAP_TL_SPLINE: {
			Point3 ns,ne;
			p = GetUVWSplinePoint(t,u,v,w,ns,ne);
			}
			break;
		case MAP_TL_PLANAR:
			p.x = 2.0f * u - 1.0f;
			p.y = 2.0f * v - 1.0f;
			p.z = w - 0.5f;
			if (!gizmo) {
				p.x *= gizmo_width/2.0f;
				p.y *= gizmo_length/2.0f;
				p.z *= gizmo_height;
				switch (GetAxis()) {
					case 0:
						tm.PreRotateY(-HALFPI);
						break;
					case 1:	
						tm.PreRotateX(HALFPI);
						break;
					}
				p = p*tm;
				}
			break;

		case MAP_TL_BALL: {
			if (u == 0.5f) u = 0.499999f;
			if (v == 0.5f) v = 0.499999f;
			float r = 0.5f * (float)sqrt(4.0f*u*(u-1.0f) + 4.0f*v*(v-1.0f) + 2.0f);
			float a2 = float((0.5f-r)*TWOPI - HALFPI);
			float a1 = (float)acos((u-0.5f)/r);
			
			p.z = (float)sin(a2) * w;

			float ta1 = (float)tan(a1);

			p.x = (float)sqrt((w*w - p.z*p.z)/(1.0f + ta1*ta1));
			p.y = p.x * ta1;

			if (u < 0.5f)	p.x = -(float)fabs(p.x);
			else			p.x =  (float)fabs(p.x);
			if (v < 0.5f)	p.y = -(float)fabs(p.y);
			else			p.y =  (float)fabs(p.y);


			p.x *= gizmo_width/2.0f;
			p.y *= gizmo_length/2.0f;
			p.z *= gizmo_height/2.0f;
			tm.PreRotateZ(-HALFPI);
			switch (GetAxis()) {
				case 0:
					tm.PreRotateY(-HALFPI);
					break;
				case 1:	
					tm.PreRotateX(HALFPI);
					break;
				}
			p = p*tm;
			}
			break;

		case MAP_TL_SPHERICAL: {
			float a2 = v * PI - HALFPI;
			p.z = (float)sin(a2) * w;

			float ta1 = (float)tan(u * TWOPI);

			p.x = (float)sqrt((w*w - p.z*p.z)/(1.0f + ta1*ta1));
			p.y = p.x * ta1;

			if (u>0.0f && u<=0.25f) {
				p.x = (float)-fabs(p.x);
				p.y = (float)-fabs(p.y);
				}
			if (u>0.25f && u<=0.5f) {
				p.x = (float) fabs(p.x);
				p.y = (float)-fabs(p.y);
				}
			if (u>0.5f && u<=0.75f) {
				p.x = (float) fabs(p.x);
				p.y = (float) fabs(p.y);
				}
			if (u>0.75f && u<=1.0f) {
				p.x = (float)-fabs(p.x);
				p.y = (float) fabs(p.y);
				}

			p.x *= gizmo_width/2.0f;
			p.y *= gizmo_length/2.0f;
			p.z *= gizmo_height/2.0f;
			tm.PreRotateZ(-HALFPI);
			switch (GetAxis()) {
				case 0:
					tm.PreRotateY(-HALFPI);
					break;
				case 1:	
					tm.PreRotateX(HALFPI);
					break;
				}
			p = p*tm;
			}
			break;

		case MAP_TL_CYLINDRICAL: 
		case MAP_TL_CYLCAP: {
			float ta1 = (float)tan(u * TWOPI);

			p.x = (float)sqrt((w*w)/(1.0f + ta1*ta1));
			p.y = p.x * ta1;

			if (u>0.0f && u<=0.25f) {
				p.x = (float)-fabs(p.x);
				p.y = (float)-fabs(p.y);
				}
			if (u>0.25f && u<=0.5f) {
				p.x = (float) fabs(p.x);
				p.y = (float)-fabs(p.y);
				}
			if (u>0.5f && u<=0.75f) {
				p.x = (float) fabs(p.x);
				p.y = (float) fabs(p.y);
				}
			if (u>0.75f && u<=1.0f) {
				p.x = (float)-fabs(p.x);
				p.y = (float) fabs(p.y);
				}

			p.z = v - 0.5f;
			p.x *= gizmo_width/2.0f;
			p.y *= gizmo_length/2.0f;
			p.z *= gizmo_height;

#if MAX_VERSION_MAJOR < 24
			Matrix3 tm(1);
#else
			Matrix3 tm;
#endif

			tm.PreRotateZ(-HALFPI);
			switch (GetAxis()) {
				case 0:
					tm.PreRotateY(-HALFPI);
					break;
				case 1:	
					tm.PreRotateX(HALFPI);
					break;
				}
			p = p*tm;
			}
			break;

		case MAP_TL_FFM:
			p = gm->GetPosition(t,u,v,w);
			break;
		}
	return p;
	}

static Point3 basic_tva[3] = { Point3(0.0,0.0,0.0),Point3(1.0,0.0,0.0),Point3(1.0,1.0,0.0)};
static Point3 basic_tvb[3] = { Point3(1.0,1.0,0.0),Point3(0.0,1.0,0.0),Point3(0.0,0.0,0.0)};
static int nextpt[3] = {1,2,0};
static int prevpt[3] = {2,0,1};

static void MakeFaceUV(Face *f, Point3 *tv) {
	int na,nhid,i;
	Point3 *basetv;
	/* make the invisible edge be 2->0 */
	nhid = 2;
	if (!(f->flags&EDGE_A))  nhid=0; 
	else if (!(f->flags&EDGE_B)) nhid = 1;
	else if (!(f->flags&EDGE_C)) nhid = 2;
	na = 2-nhid;
	basetv = (f->v[prevpt[nhid]]<f->v[nhid]) ? basic_tva : basic_tvb;
	for (i=0; i<3; i++) {
		tv[i] = basetv[na]; 
		na = nextpt[na];
		}
	}

Point3 UVWProyector::MapPoint(Point3 punto, Matrix3 tm, TimeValue t) {
	Point3 p = punto * tm;

	Point3 uv(0,0,0), v(0,0,0);
	float a1, a2;
	int cap = 0;
	Point3 fNormal(0,0,0);
	switch (mapping_type) {
		case MAP_TL_XYZ:
			return punto;
		case MAP_TL_PLANAR:
			return Point3((p.x+1.0f)/2.0f,(p.y+1.0f)/2.0f,p.z+0.5f);
			break;
		case MAP_TL_SPLINE:
			return NearestSplinePoint(p,t);
			break;
		case MAP_TL_FFM:
			return NearestGridPoint(p,t);
			break;
		case MAP_TL_BALL:
		case MAP_TL_SPHERICAL:			
			uv.z = Length(p);			
			if (uv.z!=0.0f) v = p/uv.z;
			else v = p;
			if (fabs(v.x)<0.0001 && fabs(v.y)<0.0001) {
				a1 = 0.0f;
			} else {				
				a1 = (float)atan2(v.y,v.x);				
				}
			a2 = (float)asin(v.z);
				
			if (mapping_type==MAP_TL_BALL) {
				float r = 0.5f-(a2+HALFPI)/TWOPI;
				uv.x = 0.5f + r * (float)cos(a1);
				uv.y = 0.5f + r * (float)sin(a1);
			} else {
				uv.x = (a1+PI)/TWOPI;
				uv.y = (a2+HALFPI)/PI;
				}
			return uv;
			break;
		case MAP_TL_CYLINDRICAL:
			uv.z = Length(Point3(p.x, p.y, 0.0f));			
			if (fabs(uv.z)<0.0001f) {				
				uv.x = 0.5f;
				uv.y = p.z+0.5f;
			} else {	
				v = Point3(p.x/uv.z, p.y/uv.z, 0.0f);
				a1 = (float)atan2(v.y,v.x);
				uv.x = (a1+PI)/TWOPI;
				uv.y = p.z+0.5f;
				}
			return uv;			
			}
	return uv;
	}

Point3 UVWProyector::MapPoint(Point3 punto, Point3 norm, Matrix3 tm, BOOL &nan) {
	Point3 p = punto * tm;
	Point3 uv(0,0,0);
	Point3 v(0,0,0);
	float a1;
	switch (mapping_type) {
		case MAP_TL_BOX:
			nan = TRUE; 			
			switch (LargestComponent(norm)) {
				case 0:
					if (norm.x<0.0f) 
						 return Point3(1.0f-(p.y+1.0f)/2.0f,(p.z+1.0f)/2.0f,(p.x+1.0f)/2.0f);
					else return Point3(     (p.y+1.0f)/2.0f,(p.z+1.0f)/2.0f,(p.x+1.0f)/2.0f);
				case 1:
					if (norm.y<0.0f) 
						 return Point3(     (p.x+1.0f)/2.0f,(p.z+1.0f)/2.0f,(p.y+1.0f)/2.0f);
					else return Point3(1.0f-(p.x+1.0f)/2.0f,(p.z+1.0f)/2.0f,(p.y+1.0f)/2.0f);
				case 2:
					if (norm.z<0.0f) 
						 return Point3(1.0f-(p.x+1.0f)/2.0f,(p.y+1.0f)/2.0f,(p.z+1.0f)/2.0f);
					else return Point3(     (p.x+1.0f)/2.0f,(p.y+1.0f)/2.0f,(p.z+1.0f)/2.0f);
				}
			break;
			case MAP_TL_CYLCAP:
				if ((LargestComponent(norm)==2)) {
					// Do a planar mapping on the cap.
					nan = TRUE; // This will cause it to get it's own vertex
					return Point3((p.x+1.0f)/2.0f,(p.y+1.0f)/2.0f,p.z+0.5f);
					}
				uv.z = Length(Point3(p.x, p.y, 0.0f));			
				if (fabs(uv.z)<0.0001f) {				
					uv.x = 0.5f;
					uv.y = p.z+0.5f;
				} else {	
					v = Point3(p.x/uv.z, p.y/uv.z, 0.0f);
					a1 = (float)atan2(v.y,v.x);
					uv.x = (a1+PI)/TWOPI;
					uv.y = p.z+0.5f;
					}
				return uv;			
				break;
		}
	return uv;
	}


// MapFace para mapping sin normales
void UVWProyector::MapFace(Mesh *mesh, int face, Matrix3 mapTM, Tab<Point3> &newUVs, Tab<Point3> &mapVerts, DWORDTab &refCts) {
	Point3 res[3];
	BOOL wrap[3] = {0,0,0};
	BOOL wrapy[3] = {0,0,0};
	int ix, j;
	
	for (j=0; j<3; j++) {
		res[j] = mapVerts[mesh->faces[face].v[j]];
		}

	// Que cuando sea belt mapping no lo arregle en Y pero si en X

	BOOL belt_mapping = mapping_type==MAP_TL_SPLINE && belt;
	// Fix up wrapping
	if ( mapping_type!=MAP_TL_PLANAR && mapping_type!=MAP_TL_XYZ ) {
		if ( !belt_mapping ) {
			if (fabs(res[0].x-res[1].x)>0.5) {
				if (res[0].x<res[1].x) wrap[1] = TRUE;
				else wrap[0] = TRUE;
				}
			if (fabs(res[0].x-res[2].x)>0.5) {
				if (res[0].x<res[2].x) wrap[2] = TRUE;
				else wrap[0] = TRUE;
				}
			if (fabs(res[1].x-res[2].x)>0.5) {
				if (res[1].x<res[2].x) wrap[2] = TRUE;
				else wrap[1] = TRUE;
				}

			for (j=0; j<3; j++) {
				if (wrap[j])
					res[j].x -= 1.0f;
				}
			}

		if (mapping_type == MAP_TL_FFM || mapping_type == MAP_TL_SPLINE) {
			if (fabs(res[0].y-res[1].y)>0.5) {
				if (res[0].y<res[1].y) wrapy[1] = TRUE;
				else wrapy[0] = TRUE;
				}
			if (fabs(res[0].y-res[2].y)>0.5) {
				if (res[0].y<res[2].y) wrapy[2] = TRUE;
				else wrapy[0] = TRUE;
				}
			if (fabs(res[1].y-res[2].y)>0.5) {
				if (res[1].y<res[2].y) wrapy[2] = TRUE;
				else wrapy[1] = TRUE;
				}

			for (j=0; j<3; j++) {
				if (wrapy[j])
					res[j].y -= 1.0f;
				}
			}
		}

	for (j=0; j<3; j++) {
		ix = mesh->tvFace[face].t[j]; 
		// If we're wrapping then we can't share the vertex.
		if ((wrapy[j] || wrap[j]) && refCts[ix]>1) {
			refCts[ix]--;
			mesh->tvFace[face].t[j] = mesh->getNumTVerts() + newUVs.Count();
			newUVs.Append(1,&res[j]);	
		} else {
			int vct=0;
			vct = mesh->getNumTVerts();
			if (ix >= vct) {
				newUVs[ix-vct] = res[j];
			} else {
				mesh->tVerts [ix] = res[j];
				}
			}
		}
	}

// MapFace para mapping con normales
void UVWProyector::MapFace(Mesh *mesh, int face, Matrix3 mapTM, Tab<Point3> &newUVs, DWORDTab &refCts, BOOL facesel) {
	Point3 res[3];
	BOOL wrap[3] = {0,0,0};
	BOOL nan[3];
	int ix, j;

	if (mapping_type == MAP_TL_FACE) {
		if (facesel) {
			MakeFaceUV(&mesh->faces[face], res);
			}
		else {
			res[0] = res[1] = res[2] = Point3(BIGFLOAT,BIGFLOAT,BIGFLOAT);
			}
		nan[0] = nan[1] = nan[2] = TRUE;
		}
	else {
		if (facesel) {
			Point3 v0 = mesh->verts[mesh->faces[face].v[0]] * mapTM;
			Point3 v1 = mesh->verts[mesh->faces[face].v[1]] * mapTM;
			Point3 v2 = mesh->verts[mesh->faces[face].v[2]] * mapTM;
			Point3 norm = Normalize((v1-v0)^(v2-v1));
			for (j=0; j<3; j++) {
				res[j] = MapPoint(mesh->verts[mesh->faces[face].v[j]],norm,mapTM,nan[j]);
				}
			}
		else {
			res[0] = res[1] = res[2] = Point3(BIGFLOAT,BIGFLOAT,BIGFLOAT);
			}
		}

	if (mapping_type == MAP_TL_CYLCAP) {
		// Fix up wrapping
		if (fabs(res[0].x-res[1].x)>0.5) {
			if (res[0].x<res[1].x) wrap[1] = TRUE;
			else wrap[0] = TRUE;
			}
		if (fabs(res[0].x-res[2].x)>0.5) {
			if (res[0].x<res[2].x) wrap[2] = TRUE;
			else wrap[0] = TRUE;
			}
		if (fabs(res[1].x-res[2].x)>0.5) {
			if (res[1].x<res[2].x) wrap[2] = TRUE;
			else wrap[1] = TRUE;
			}

		for (j=0; j<3; j++) {
			if (wrap[j])
				res[j].x -= 1.0f;
			}
		}

	for (j=0; j<3; j++) {
		ix = mesh->tvFace[face].t[j]; 
		if ((wrap[j]||nan[j]) && refCts[ix]>1) {
			refCts[ix]--;
			mesh->tvFace[face].t[j] = mesh->getNumTVerts() + newUVs.Count();
			newUVs.Append(1,&res[j]);	
		} else {
			int vct=0;
			vct = mesh->getNumTVerts();
			if (ix >= vct) {
				newUVs[ix-vct] = res[j];
			} else {
				mesh->tVerts [ix] = res[j];
				}
			}
		}
	}

BOOL UVWProyector::ApplyUVW(Mesh *mesh, PolyUVWData * uvw_data, Matrix3 mapTM, BitArray fs, TimeValue t) {
	if ( !force_apply && (valid_group==uvw_data->valid_group_key || (mapping_type==MAP_TL_UVWDATA&&uvw_data->num_v!=0&&uvw_data->num_f!=0)) ) {
		applying_mapping = FALSE;
		return TRUE;
		}
	Update(t); 
	applying_mapping = TRUE;

	float utile = GetTileU(t);
	float vtile = GetTileV(t);
	float wtile = GetTileW(t);

	float umove = GetMoveU(t);
	float vmove = GetMoveV(t);
	float wmove = GetMoveW(t);

	float urotate = GetRotateU(t);
	float vrotate = GetRotateV(t);
	float wrotate = GetRotateW(t);

	float uscale = GetScaleU(t);
	float vscale = GetScaleV(t);
	float wscale = GetScaleW(t);

	if ( mapping_type==MAP_TL_FRAME ) {
		Interval valid = FOREVER;

		if ( !frame_node ) {
			DefaultUVW(mesh, uvw_data);
			applying_mapping = FALSE;
			return FALSE;
			}

		ObjectState obst = frame_node->EvalWorldState(t);
		UVWFrameObject *frame = (UVWFrameObject*)obst.obj;
		Matrix3 ftm = frame_node->GetObjTMAfterWSM(t, &valid);

		BOOL uvwData = FALSE;
		Tab <Point3> uvw_verts;
		Tab <UVWFace> uvw_faces;

		frame->GetUVW(uvwData,uvw_verts,uvw_faces,ftm,t);

		if ( uvw_faces.Count() != mesh->numFaces ) {
			DefaultUVW(mesh, uvw_data);
			applying_mapping = FALSE;
			return FALSE;
			}

		mesh->setNumTVerts(uvw_verts.Count());
		mesh->setNumTVFaces(uvw_faces.Count());
		
		TVFace *tvFace = mesh->tvFace;
		Point3 *tVerts = mesh->tVerts;

		uvw_data->DeleteUVWData();
		uvw_data->num_v = uvw_verts.Count();
		uvw_data->num_f = uvw_faces.Count();
		uvw_data->v = new Point3[uvw_data->num_v];
		uvw_data->f = new PolyFace[uvw_data->num_f];

		for ( int i_v=0; i_v<uvw_data->num_v; i_v++ ) {
			Point3 uvw = uvw_verts[i_v];
			XForm(uvw,umove,vmove,wmove,urotate,vrotate,wrotate,uscale,vscale,wscale);
			tVerts[i_v] = uvw;
			uvw_data->v[i_v] = uvw;
			}

		for ( int i_f=0; i_f<uvw_data->num_f; i_f++ ) {
			tvFace[i_f].t[0] = uvw_faces[i_f].vtx[0];
			tvFace[i_f].t[1] = uvw_faces[i_f].vtx[1];
			tvFace[i_f].t[2] = uvw_faces[i_f].vtx[2];
			uvw_data->f[i_f].deg = 3;
			uvw_data->f[i_f].vtx = new int[3];
			uvw_data->f[i_f].vtx[0] = uvw_faces[i_f].vtx[0];
			uvw_data->f[i_f].vtx[1] = uvw_faces[i_f].vtx[1];
			uvw_data->f[i_f].vtx[2] = uvw_faces[i_f].vtx[2];
			}

		uvw_data->valid_group_key = valid_group;

		// Se esta toteando porque cuando damos un return en este if, la jodita
		// no da un map por default al auxmesh
		applying_mapping = FALSE;
		return TRUE;
		}

	int i, j;

	TVFace *tvFace2;
	Point3 *tVerts2;
	int numTVerts2;

	int newCT = 0, numTV = mesh->getNumVerts();

	Tab <Point3> mapVerts;
	Tab <Point3> newUVs;
	DWORDTab refCts;		

	mesh->setNumTVFaces(mesh->getNumFaces());
	mesh->setNumTVerts(mesh->getNumVerts());

	mapVerts.SetCount(mesh->getNumVerts());
	tvFace2 = mesh->tvFace;
	tVerts2 = mesh->tVerts;
	numTVerts2  = mesh->numTVerts;
	// Le asignamos a todas las faces los vertices UVW
	for (i=0; i<mesh->getNumFaces(); i++) {
		tvFace2[i].setTVerts(mesh->faces[i].getAllVerts());
		}

	newUVs.SetCount(newCT);

	// Init a reference count for each UV vert. The reference count is the  
	// number of times a vert is referenced by a selected face
	// refCts:  Cuantas faces referencian a un vertice
	BitArray vs;
	vs.SetSize(numTV);
	vs.ClearAll();

	int first_not_selected = -1;

	refCts.SetCount(numTV + newCT);

	for (i=0; i<numTV + newCT; i++) refCts[i] = 0;
	for (i=0; i<mesh->getNumFaces(); i++) {
		for (j=0; j<3; j++) {
			refCts[tvFace2[i].t[j]]++;
			if (fs[i]) 
				vs.Set(tvFace2[i].t[j]);
			else if ( first_not_selected == -1 )
				first_not_selected = tvFace2[i].t[j];
			}
		}

	BOOL nonesel = FALSE;
	if (vs.NumberSet() == 0) nonesel = TRUE;

	// TLTODO:  Arreglar lo de nan
	if (mapping_type != MAP_TL_BOX && mapping_type != MAP_TL_CYLCAP && mapping_type != MAP_TL_FACE) {
		for (i=0; i<mesh->getNumVerts(); i++) {
			if ( vs[i] || nonesel ) {
				mapVerts[i] = MapPoint(mesh->verts[i],mapTM,t);
				}
			else
				mapVerts[i] = NO_UVW;
			}

		// Now map the faces
		for (i=0; i<mesh->getNumFaces(); i++) {
			// Llamada para mapear un face..
			if ( fs[i] || nonesel )
				MapFace(mesh,i,mapTM,newUVs,mapVerts,refCts);
			}
		}
	else {
		for (i=0; i<mesh->getNumFaces(); i++) {
			// Llamada para mapear un face...
			BOOL mf = nonesel ? TRUE : fs[i];
			MapFace(mesh,i,mapTM,newUVs,refCts,mf);
			}
		}

	for ( int i_f=0; i_f<mesh->getNumFaces(); i_f++ ) {
		if ( !fs[i_f] && !nonesel ) {
			for ( int j=0; j<3; j++ ) {
				tvFace2[i_f].t[j] = first_not_selected;
				}
			}
		}

	// Copy in the new UVs
	if ( newUVs.Count() ) {		
		mesh->setNumTVerts(numTV+newUVs.Count(),TRUE);
		for (i=0; i<newUVs.Count(); i++) {
			mesh->tVerts [i+numTV] = newUVs[i];
			}
		}

	// Mark used faces
	int oldCT = mesh->getNumTVerts();
	BitArray used;
	used.SetSize(oldCT);
	for (i=0; i<mesh->getNumFaces(); i++) {		
		for (int j=0; j<3; j++) {
			used.Set(mesh->tvFace[i].t[j]);
			}
		}

	// Map out the unused verts
	DWORDTab vmap;
	vmap.SetCount(oldCT);
	int ct = 0;
	for (i=0; i<oldCT; i++) {		
		if (used[i]) {
			vmap[i] = ct++;
		} else {
			vmap[i] = UNDEFINED;
			}
		}
	for (i=0; i<mesh->getNumFaces(); i++) {		
		for (int j=0; j<3; j++) {
			mesh->tvFace[i].t[j] = vmap[mesh->tvFace[i].t[j]];
			}
		}

	// Compact 'em down
	int ix=0;
	for (i=0; i<oldCT; i++) if (used[i]) mesh->tVerts[ix++] = mesh->tVerts[i];			
	mesh->setNumTVerts(ct,TRUE);

	// Apply tiling
	int uflip = 0, vflip = 0, wflip = 0;

	if (utile < 0.0f) {
		uflip = 1;
		utile = (float)fabs(utile);
		}
	if (vtile < 0.0f) {
		vflip = 1;
		vtile = (float)fabs(vtile);
		}
	if (wtile < 0.0f) {
		wflip = 1;
		wtile = (float)fabs(wtile);
		}

	for (i=0; i<mesh->getNumTVerts(); i++) {
		Point3 uvw = mesh->tVerts[i];

		uvw.x = uvw.x*utile;
		uvw.y = uvw.y*vtile;
		uvw.z = uvw.z*wtile;

		if (uflip) uvw.x =  1.0f - uvw.x;
		if (vflip) uvw.y =  1.0f - uvw.y;
		if (wflip) uvw.z =  1.0f - uvw.z;

		uvw.x -= GetOffsetU(t);
		uvw.y -= GetOffsetV(t);
		uvw.z -= GetOffsetW(t);

		if (mapping_type == MAP_TL_SPLINE) {
			float tile = tileCrv.GetValue(uvw.y);
			float offset = (tile - 1.0f) / 2.0f;
			uvw.x = uvw.x * tile - offset;

			float radius = wTileCrv.GetValue(uvw.y);
			float height = gizmo_height;
			if ( belt )
				height = 1.0f;
			uvw.z = uvw.z/(radius * height);
			}

		XForm(uvw,umove,vmove,wmove,urotate,vrotate,wrotate,uscale,vscale,wscale);

		mesh->tVerts[i] = uvw;
		}

	uvw_data->DeleteUVWData();
	uvw_data->num_v = mesh->getNumTVerts();
	uvw_data->num_f = mesh->numFaces;
	uvw_data->v = new Point3[uvw_data->num_v];
	uvw_data->f = new PolyFace[uvw_data->num_f];

	for ( int i_v=0; i_v<uvw_data->num_v; i_v++ ) 
		uvw_data->v[i_v] = mesh->tVerts[i_v];

	for ( int i_f=0; i_f<uvw_data->num_f; i_f++ ) {
		uvw_data->f[i_f].deg = 3;
		uvw_data->f[i_f].vtx = new int[3];
		uvw_data->f[i_f].vtx[0] = mesh->tvFace[i_f].t[0];
		uvw_data->f[i_f].vtx[1] = mesh->tvFace[i_f].t[1];
		uvw_data->f[i_f].vtx[2] = mesh->tvFace[i_f].t[2];
		}

	uvw_data->valid_group_key = valid_group;

	applying_mapping = FALSE;

	force_apply = FALSE;

	return TRUE;
	}

void UVWProyector::DefaultUVW(Mesh *mesh, PolyUVWData * uvw_data) {
	mesh->setNumTVFaces(mesh->getNumFaces());
	mesh->setNumTVerts(mesh->getNumVerts());

	TVFace * tvFace = mesh->tvFace;
	Point3 * tVerts = mesh->tVerts;

	uvw_data->DeleteUVWData();
	uvw_data->num_v = mesh->getNumVerts();
	uvw_data->num_f = mesh->getNumFaces();
	uvw_data->v = new Point3[uvw_data->num_v];
	uvw_data->f = new PolyFace[uvw_data->num_f];

	for ( int i_v=0; i_v<uvw_data->num_v; i_v++ ) {
		tVerts[i_v] = mesh->verts[i_v];
		uvw_data->v[i_v] = mesh->verts[i_v];
		}	

	for ( int i_f=0; i_f<uvw_data->num_f; i_f++ ) {
		tvFace[i_f].setTVerts(mesh->faces[i_f].getAllVerts());
		uvw_data->f[i_f].deg = 3;
		uvw_data->f[i_f].vtx = new int[3];
		uvw_data->f[i_f].vtx[0] = mesh->faces[i_f].getVert(0);
		uvw_data->f[i_f].vtx[1] = mesh->faces[i_f].getVert(1);
		uvw_data->f[i_f].vtx[2] = mesh->faces[i_f].getVert(2);
		}

	uvw_data->valid_group_key = valid_group;
	}	

#if (MAX_RELEASE >= 4000)
// MapFace para mapping sin normales
void UVWProyector::MapPolyFace(MNMap *mapmesh, int face, Matrix3 mapTM, Tab<Point3> &newUVs, Tab<Point3> &mapVerts, Tab <int> &refCts) {
	int deg = mapmesh->f[face].deg;
	BitArray wrapx(deg);	wrapx.ClearAll();
	BitArray wrapy(deg);	wrapy.ClearAll();
	Tab <Point3> res;		res.SetCount(deg);

	int j,ix;

	for ( j=0; j<deg; j++ ) {
		res[j] = mapVerts[ mapmesh->f[face].tv[j] ];
		}

	// Fix up wrapping
	BOOL belt_mapping = mapping_type==MAP_TL_SPLINE && belt;
	if (mapping_type != MAP_TL_PLANAR && mapping_type != MAP_TL_XYZ ) {
		float min_x = res[0].x;
		float min_y = res[0].y;
		for ( int i_d=1; i_d<deg; i_d++ ) {
			float vx = res[i_d].x;
			float vy = res[i_d].y;
			if ( vx < min_x ) min_x = vx;
			if ( vy < min_y ) min_y = vy;
			}

		for ( int i_d=0; i_d<deg; i_d++ ) {
			if ( !belt_mapping ) {
				if ( res[i_d].x-min_x > 0.5f ) {
					wrapx.Set(i_d);
					res[i_d].x -= 1.0f;
					}
				}
			if ( ( mapping_type == MAP_TL_FFM || mapping_type == MAP_TL_SPLINE ) && res[i_d].y-min_y > 0.5f ) {
				wrapy.Set(i_d);
				res[i_d].y -= 1.0f;
				}
			}
		}

	for ( int i_d=0; i_d<deg; i_d++ ) {
		ix = mapmesh->f[face].tv[i_d];

		if ( ( wrapx[i_d] || wrapy[i_d] ) && refCts[ix]>1 ) {
			refCts[ix]--;
			mapmesh->f[face].tv[i_d] = mapmesh->numv + newUVs.Count();
			newUVs.Append(1,&res[i_d]);
			}
		else {
			int vct = 0;
			vct = mapmesh->numv;
			if ( ix >= vct ) {
				newUVs[ix-vct] = res[i_d];
				}
			else {
				mapmesh->v[ix] = res[i_d];
				}
			}
		}
	}

BOOL UVWProyector::ApplyUVW(MNMesh *mnmesh, PolyUVWData * uvw_data, Matrix3 mapTM, BitArray fs, TimeValue t) 
{
	if ( !force_apply && (valid_group==uvw_data->valid_group_key || (mapping_type==MAP_TL_UVWDATA&&uvw_data->num_v!=0&&uvw_data->num_f!=0)) ) {
		applying_mapping = FALSE;
		return TRUE;
		}

	Update(t);
	applying_mapping = TRUE;

	if ( mapping_type==MAP_TL_PELT && fs.NumberSet() ) {
		PeltApplyUV( t, mnmesh, uvw_data );
		applying_mapping = FALSE;
		return TRUE;
		}

	float utile = GetTileU(t);
	float vtile = GetTileV(t);
	float wtile = GetTileW(t);

	float umove = GetMoveU(t);
	float vmove = GetMoveV(t);
	float wmove = GetMoveW(t);

	float urotate = GetRotateU(t);
	float vrotate = GetRotateV(t);
	float wrotate = GetRotateW(t);

	float uscale = GetScaleU(t);
	float vscale = GetScaleV(t);
	float wscale = GetScaleW(t);

	if ( mapping_type==MAP_TL_FRAME ) {
		Interval valid = FOREVER;

		if ( !frame_node ) {
			DefaultUVW(mnmesh, uvw_data);
			applying_mapping = FALSE;
			return FALSE;
			}

		ObjectState obst = frame_node->EvalWorldState(t);
		UVWFrameObject *frame = (UVWFrameObject*)obst.obj;
		Matrix3 ftm = frame_node->GetObjTMAfterWSM(t, &valid);

		BOOL uvwData = FALSE;
		Tab <Point3> uvw_verts;
		Tab <UVWFace> uvw_faces;

		frame->GetUVW(uvwData,uvw_verts,uvw_faces,ftm,t);

		if ( uvw_faces.Count() != mnmesh->numf ) {
			DefaultUVW(mnmesh, uvw_data);
			applying_mapping = FALSE;
			return FALSE;
			}

		for ( int i_f=0; i_f<uvw_faces.Count(); i_f++ ) {
			if ( mnmesh->f[i_f].deg != uvw_faces[i_f].deg ) {
				DefaultUVW(mnmesh, uvw_data);
				applying_mapping = FALSE;
				return FALSE;
				}
			}

		MNMap *mnc = mnmesh->M(1);

		mnc->ClearFlag (MN_DEAD);
		mnc->setNumVerts( uvw_verts.Count() );
		mnc->setNumFaces( uvw_faces.Count() );

		uvw_data->DeleteUVWData();
		uvw_data->num_v = mnc->numv;
		uvw_data->num_f = mnc->numf;
		uvw_data->v = new Point3[uvw_data->num_v];
		uvw_data->f = new PolyFace[uvw_data->num_f];

		for ( int i_v=0; i_v<uvw_data->num_v; i_v++ ) {
			Point3 uvw = uvw_verts[i_v];
			XForm(uvw,umove,vmove,wmove,urotate,vrotate,wrotate,uscale,vscale,wscale);
			mnc->v[i_v] = uvw;
			uvw_data->v[i_v] = uvw;
			}

		for ( int i_f=0; i_f<uvw_data->num_f; i_f++ ) {
			int face_deg = uvw_faces[i_f].deg;
			mnc->f[i_f].SetSize( uvw_faces[i_f].deg );
			for ( int i_v=0; i_v<face_deg; i_v++ )
				mnc->f[i_f].tv[i_v] = uvw_faces[i_f].vtx[i_v];
			uvw_data->f[i_f].deg = face_deg;
			uvw_data->f[i_f].vtx = new int[face_deg];
			for ( int i_v=0; i_v<face_deg; i_v++ )
				uvw_data->f[i_f].vtx[i_v] = uvw_faces[i_f].vtx[i_v];
			}

		uvw_data->valid_group_key = valid_group;
	
		applying_mapping = FALSE;
		return TRUE;
		}

	int i,j;

	MNMap *mnc = mnmesh->M(1);

	mnc->ClearFlag (MN_DEAD);
	mnc->setNumVerts( mnmesh->VNum() );
	mnc->setNumFaces( mnmesh->FNum() );

	int numf = mnc->numf;
	int numv = mnc->numv;

	Tab<Point3> newUVWs;
	Tab<Point3> mapVerts;
	newUVWs.SetCount(0);

	mapVerts.SetCount(numv);

	Tab <int> refCts;
	refCts.SetCount( numv );

	BitArray vs;
	vs.SetSize(numv);
	vs.ClearAll();

	int first_not_selected = -1;

	for ( i=0; i<numv; i++ )
		refCts[i] = 0;
	for ( i=0; i<numf; i++ )
		for ( j=0; j<mnmesh->f[i].deg; j++ ) {
			refCts[mnmesh->f[i].vtx[j]]++;
			if ( fs[i] )
				vs.Set( mnmesh->f[i].vtx[j] );
			else
				if ( first_not_selected == -1 )
					first_not_selected = mnmesh->f[i].vtx[j];
			}

	BOOL nonesel = FALSE;
	if (vs.NumberSet() == 0) nonesel = TRUE;

	for ( int i_f=0; i_f<numf; i_f++ ) {
		mnc->f[i_f] = mnmesh->f[i_f];
		}

	if (mapping_type != MAP_TL_BOX && mapping_type != MAP_TL_CYLCAP && mapping_type != MAP_TL_FACE) {
		for ( int i_v=0; i_v<numv; i_v++ ) {
			if ( vs[i_v] || nonesel )	mapVerts[i_v] = MapPoint(mnmesh->v[i_v].p, mapTM, t);
			else						mapVerts[i_v] = NO_UVW;
			}
		for ( int i_f=0; i_f<numf; i_f++ ) {
			if ( fs[i_f] || nonesel )
				MapPolyFace(mnc,i_f,mapTM,newUVWs,mapVerts,refCts);
			}
		}
	else {
		if ( mapping_type == MAP_TL_FACE ) {
			UVWMapper mp(MAP_FACE,mapTM);
			mnmesh->ApplyMapper (mp,1);
			}
		else if ( mapping_type == MAP_TL_CYLCAP ) {
			UVWMapper mp(MAP_CYLINDRICAL,mapTM);
			mp.cap = 1;
			mnmesh->ApplyMapper (mp,1);
			}
		else {
			UVWMapper mp(MAP_BOX,mapTM);
			mnmesh->ApplyMapper (mp,1);
			}

		for ( int i_f=0; i_f<numf; i_f++ ) {
			int deg = mnc->f[i_f].deg;
			for ( int j=0; j<deg; j++ ) 
				if ( !fs[i_f] && !nonesel ) mnc->v[ mnc->f[i_f].tv[j] ] = NO_UVW;
			}
		}

	for ( int i_f=0; i_f<numf; i_f++ ) {
		if ( !fs[i_f] && !nonesel ) {
			int deg = mnc->f[i_f].deg;
			for ( int j=0; j<deg; j++ ) 
				mnc->f[i_f].tv[j] = first_not_selected;
			}
		}

	if ( newUVWs.Count() ) {
		int numv = mnc->numv;
		mnc->setNumVerts( mnc->numv + newUVWs.Count() );
		for ( i=0; i<newUVWs.Count(); i++ ) {
			mnc->v[i+numv] = newUVWs[i];
			}
		}

	// Compact'em down
	int oldCT = mnc->numv;
	BitArray used(oldCT);
	for ( i=0; i<mnc->numf; i++ ) {
		int deg = mnc->f[i].deg;
		for ( int j=0; j<deg; j++ )
			used.Set( mnc->f[i].tv[j] );
		}

	DWORDTab vmap;
	vmap.SetCount(oldCT);
	int ct = 0;
	for ( i=0; i<oldCT; i++ ) {
		if ( used[i] )	vmap[i] = ct++;
		else			vmap[i] = UNDEFINED;
		}
	for ( i=0; i<mnc->numf; i++ ) {
		int deg = mnc->f[i].deg;
		for ( int j=0; j<deg; j++ ) {
			mnc->f[i].tv[j] = vmap[mnc->f[i].tv[j]];
			}
		}

	int ix=0;
	for ( i=0; i<oldCT; i++ ) 
		if ( used[i] )	mnc->v[ix++] = mnc->v[i];
	mnc->setNumVerts(ct);

	// Apply Tiling..
	int uflip = 0, vflip = 0, wflip = 0;

	if (utile < 0.0f) {
		uflip = 1;
		utile = (float)fabs(utile);
		}
	if (vtile < 0.0f) {
		vflip = 1;
		vtile = (float)fabs(vtile);
		}
	if (wtile < 0.0f) {
		wflip = 1;
		wtile = (float)fabs(wtile);
		}

	for (i=0; i<mnc->numv; i++) {
		Point3 uvw = mnc->v[i];

		uvw.x = uvw.x*utile;
		uvw.y = uvw.y*vtile;
		uvw.z = uvw.z*wtile;

		if (uflip) uvw.x =  1.0f - uvw.x;
		if (vflip) uvw.y =  1.0f - uvw.y;
		if (wflip) uvw.z =  1.0f - uvw.z;

		uvw.x -= GetOffsetU(t);
		uvw.y -= GetOffsetV(t);
		uvw.z -= GetOffsetW(t);

		if (mapping_type == MAP_TL_SPLINE) {
			float tile = tileCrv.GetValue(uvw.y);
			float offset = (tile - 1.0f) / 2.0f;
			uvw.x = uvw.x * tile - offset;

			float radius = wTileCrv.GetValue(uvw.y);
			float height = gizmo_height;
			if ( belt )
				height = 1.0f;
			uvw.z = uvw.z/(radius * height);
			}

		XForm(uvw,umove,vmove,wmove,urotate,vrotate,wrotate,uscale,vscale,wscale);

		mnc->v[i] = uvw;
		}

	uvw_data->DeleteUVWData();
	uvw_data->num_v = mnc->numv;
	uvw_data->num_f = mnc->numf;
	uvw_data->v = new Point3[uvw_data->num_v];
	uvw_data->f = new PolyFace[uvw_data->num_f];

	for ( int i_v=0; i_v<uvw_data->num_v; i_v++ ) 
		uvw_data->v[i_v] = mnc->v[i_v];

	for ( int i_f=0; i_f<uvw_data->num_f; i_f++ ) {
		int deg = mnc->f[i_f].deg;
		uvw_data->f[i_f].deg = deg;
		uvw_data->f[i_f].vtx = new int[deg];
		for ( int i_v=0; i_v<deg; i_v++ )
			uvw_data->f[i_f].vtx[i_v] = mnc->f[i_f].tv[i_v];
		}

	uvw_data->valid_group_key = valid_group;

	applying_mapping = FALSE;

	force_apply = FALSE;

	return TRUE;
	}

void UVWProyector::DefaultUVW(MNMesh *mnmesh, PolyUVWData * uvw_data) {
	MNMap *mnc = mnmesh->M(1);

	uvw_data->DeleteUVWData();
	uvw_data->num_v = mnmesh->VNum();
	uvw_data->num_f = mnmesh->FNum();
	uvw_data->v = new Point3[uvw_data->num_v];
	uvw_data->f = new PolyFace[uvw_data->num_f];

	mnc->ClearFlag (MN_DEAD);
	mnc->setNumVerts( mnmesh->VNum() );
	mnc->setNumFaces( mnmesh->FNum() );

	for ( int i_v=0; i_v<uvw_data->num_v; i_v++ ) {
		mnc->v[i_v] = mnmesh->v[i_v].p;
		uvw_data->v[i_v] = mnmesh->v[i_v].p;
		}

	for ( int i_f=0; i_f<uvw_data->num_f; i_f++ ) {
		mnc->f[i_f] = mnmesh->f[i_f]; //TEXLAYTODO: Si se necesita llenar este mnc???
		int deg = mnmesh->f[i_f].deg;
		uvw_data->f[i_f].deg = deg;
		uvw_data->f[i_f].vtx = new int[deg];
		for ( int i_v=0; i_v<deg; i_v++ )
			uvw_data->f[i_f].vtx[i_v] = mnmesh->f[i_f].vtx[i_v];
		}

	uvw_data->valid_group_key = valid_group;
	}

#endif 

// MapPatch para mapping con normales
void UVWProyector::MapPatch(PatchMesh *mesh, int patch, Matrix3 mapTM, Tab<Point3> &newUVs, DWORDTab &refCts, BOOL facesel) {
	Point3 res[4];
	BOOL nan[4];
	BOOL wrap[4] = {0,0,0};
	int ix, j;
	
	Patch &p = mesh->patches[patch];
	TVPatch &tp = mesh->tvPatches[1][patch];

	if (mapping_type==MAP_TL_FACE) {
		if (facesel) {
			res[0] = Point3(0,0,0);
			res[1] = Point3(1,0,0);
			if (p.type == PATCH_TRI) {
				res[2] = Point3(0,1,0);
				}
			else {
				res[2] = Point3(1,1,0);
				res[3] = Point3(0,1,0);
				}
			}
		else {
			res[0] = res[1] = res[2] = res[3] = Point3(BIGFLOAT,BIGFLOAT,BIGFLOAT);
			}
		nan[0] = nan[1] = nan[2] = nan[3] = TRUE;
		}

	Point3 norm;

	if (mapping_type==MAP_TL_BOX || (mapping_type==MAP_TL_CYLCAP)) {
		if (facesel) {
			switch(p.type) {
				// Triangular patch normal is easy!
				case PATCH_TRI: {
					Point3 v0 = mesh->verts[p.v[0]].p * mapTM;
					Point3 v1 = mesh->verts[p.v[1]].p * mapTM;
					Point3 v2 = mesh->verts[p.v[2]].p * mapTM;
					norm = Normalize((v1-v0)^(v2-v1));
					break;
					}
				// Quad patch normal is a little harder -- average the four corner norms
				case PATCH_QUAD: {
					Point3 v0 = mesh->verts[p.v[0]].p * mapTM;
					Point3 v1 = mesh->verts[p.v[1]].p * mapTM;
					Point3 v2 = mesh->verts[p.v[2]].p * mapTM;
					Point3 v3 = mesh->verts[p.v[3]].p * mapTM;
					Point3 norm1 = Normalize((v1-v0)^(v2-v1));
					Point3 norm2 = Normalize((v2-v1)^(v3-v2));
					Point3 norm3 = Normalize((v3-v2)^(v0-v3));
					Point3 norm4 = Normalize((v0-v3)^(v1-v0));
					norm = Normalize((norm1 + norm2 + norm3 + norm4) / 4.0f);
					break;
					}
				}
			}
		else {
			res[0] = res[1] = res[2] = res[3] = Point3(BIGFLOAT,BIGFLOAT,BIGFLOAT);
			}

		for (j=0; j<p.type; j++) {
			res[j] = MapPoint(mesh->verts[p.v[j]].p,norm,mapTM,nan[j]);
			}
		}

	if (mapping_type == MAP_TL_CYLCAP) {
		if(p.type == PATCH_TRI) {
			if (fabs(res[0].x-res[1].x)>0.5) {
				if (res[0].x<res[1].x) wrap[1] = TRUE;
				else wrap[0] = TRUE;
				}
			if (fabs(res[0].x-res[2].x)>0.5) {
				if (res[0].x<res[2].x) wrap[2] = TRUE;
				else wrap[0] = TRUE;
				}
			if (fabs(res[1].x-res[2].x)>0.5) {
				if (res[1].x<res[2].x) wrap[2] = TRUE;
				else wrap[1] = TRUE;
				}
			}
		else {
			if (fabs(res[0].x-res[1].x)>0.5) {
				if (res[0].x<res[1].x) wrap[1] = TRUE;
				else wrap[0] = TRUE;
				}
			if (fabs(res[1].x-res[2].x)>0.5) {
				if (res[1].x<res[2].x) wrap[2] = TRUE;
				else wrap[1] = TRUE;
				}
			if (fabs(res[2].x-res[3].x)>0.5) {
				if (res[2].x<res[3].x) wrap[3] = TRUE;
				else wrap[2] = TRUE;
				}
			if (fabs(res[3].x-res[0].x)>0.5) {
				if (res[3].x<res[0].x) wrap[0] = TRUE;
				else wrap[3] = TRUE;
				}
			} // if(type
		// Fix up wrapping
		for (j=0; j<p.type; j++) {
			if (wrap[j]) res[j].x -= 1.0f;
			}
		} // if (GetMappingType

	for (j=0; j<p.type; j++) {
		ix = tp.tv[j]; 
		
		// If we're wrapping then we can't share the vertex.
		if ((wrap[j] || nan[j]) && refCts[ix]>1) {
			refCts[ix]--;
			tp.tv[j] = mesh->getNumMapVerts(1) + newUVs.Count();
			newUVs.Append(1,&res[j]);	
		} else {
			int vct = mesh->getNumMapVerts(1);
			if (ix >= vct) {
				newUVs[ix-vct] = res[j];
			} else {
				mesh->tVerts[1][ix] = res[j];
				}
			}
		}
	}


// MapPatch para mapping sin normales
void UVWProyector::MapPatch(PatchMesh *mesh, int patch, Matrix3 mapTM, Tab<Point3> &newUVs, Tab<Point3> &mapVerts, DWORDTab &refCts) {
	Point3 res[4];
	BOOL wrap[4] = {0,0,0};
	int ix, j;
	
	Patch &p = mesh->patches[patch];
	TVPatch &tp = mesh->tvPatches[1][patch];

	for (j=0; j<p.type; j++) {
		res[j] = mapVerts[p.v[j]];
		}

	BOOL belt_mapping = mapping_type==MAP_TL_SPLINE && belt;
	if ( mapping_type!=MAP_TL_PLANAR && mapping_type!=MAP_TL_XYZ ) {
		if(p.type == PATCH_TRI) {
			if (fabs(res[0].x-res[1].x)>0.5) {
				if (res[0].x<res[1].x) wrap[1] = TRUE;
				else wrap[0] = TRUE;
				}
			if (fabs(res[0].x-res[2].x)>0.5) {
				if (res[0].x<res[2].x) wrap[2] = TRUE;
				else wrap[0] = TRUE;
				}
			if (fabs(res[1].x-res[2].x)>0.5) {
				if (res[1].x<res[2].x) wrap[2] = TRUE;
				else wrap[1] = TRUE;
				}
			}
		else {
			if (fabs(res[0].x-res[1].x)>0.5) {
				if (res[0].x<res[1].x) wrap[1] = TRUE;
				else wrap[0] = TRUE;
				}
			if (fabs(res[1].x-res[2].x)>0.5) {
				if (res[1].x<res[2].x) wrap[2] = TRUE;
				else wrap[1] = TRUE;
				}
			if (fabs(res[2].x-res[3].x)>0.5) {
				if (res[2].x<res[3].x) wrap[3] = TRUE;
				else wrap[2] = TRUE;
				}
			if (fabs(res[3].x-res[0].x)>0.5) {
				if (res[3].x<res[0].x) wrap[0] = TRUE;
				else wrap[3] = TRUE;
				}
			} // if(type
		// Fix up wrapping
		for (j=0; j<p.type; j++) {
			if (wrap[j]) res[j].x -= 1.0f;
			}
		} // if (GetMappingType

	for (j=0; j<p.type; j++) {
		ix = tp.tv[j]; 
		
		// If we're wrapping then we can't share the vertex.
		if (wrap[j] && refCts[ix]>1) {
			refCts[ix]--;
			tp.tv[j] = mesh->getNumMapVerts(1) + newUVs.Count();
			newUVs.Append(1,&res[j]);	
		} else {
			int vct = mesh->getNumMapVerts(1);
			if (ix >= vct) {
				newUVs[ix-vct] = res[j];
			} else {
				mesh->tVerts[1][ix] = res[j];
				}
			}
		}
	}

BOOL UVWProyector::ApplyUVW(PatchMesh *mesh, PolyUVWData * uvw_data, Matrix3 mapTM, BitArray fs, TimeValue t) {
	
	if ( !force_apply && (valid_group==uvw_data->valid_group_key || (mapping_type==MAP_TL_UVWDATA&&uvw_data->num_v!=0&&uvw_data->num_f!=0)) ) {
		applying_mapping = FALSE;
		return TRUE;
		}
	Update(t);
	applying_mapping = TRUE;

	float utile = GetTileU(t);
	float vtile = GetTileV(t);
	float wtile = GetTileW(t);

	float umove = GetMoveU(t);
	float vmove = GetMoveV(t);
	float wmove = GetMoveW(t);

	float urotate = GetRotateU(t);
	float vrotate = GetRotateV(t);
	float wrotate = GetRotateW(t);

	float uscale = GetScaleU(t);
	float vscale = GetScaleV(t);
	float wscale = GetScaleW(t);

	if ( mapping_type==MAP_TL_FRAME ) {
		Interval valid = FOREVER;

		if ( !frame_node ) {
			DefaultUVW(mesh, uvw_data);
			applying_mapping = FALSE;
			return FALSE;
			}

		ObjectState obst = frame_node->EvalWorldState(t);
		UVWFrameObject *frame = (UVWFrameObject*)obst.obj;
		Matrix3 ftm = frame_node->GetObjTMAfterWSM(t, &valid);

		BOOL uvwData = FALSE;
		Tab <Point3> uvw_verts;
		Tab <UVWFace> uvw_faces;

		frame->GetUVW(uvwData,uvw_verts,uvw_faces,ftm,t);

		if ( uvw_faces.Count() != mesh->getNumPatches() ) {
			DefaultUVW(mesh, uvw_data);
			applying_mapping = FALSE;
			return FALSE;
			}

		mesh->setNumMapVerts(1,uvw_verts.Count());
		mesh->setNumMapPatches(1,uvw_faces.Count());

		uvw_data->DeleteUVWData();
		uvw_data->num_v = uvw_verts.Count();
		uvw_data->num_f = uvw_faces.Count();
		uvw_data->v = new Point3[uvw_data->num_v];
		uvw_data->f = new PolyFace[uvw_data->num_f];

		TVPatch *tvPatch = mesh->tvPatches[1];
		PatchTVert *tVerts = mesh->tVerts[1];

		for ( int i_v=0; i_v<uvw_data->num_v; i_v++ ) {
			Point3 uvw = uvw_verts[i_v];
			XForm(uvw,umove,vmove,wmove,urotate,vrotate,wrotate,uscale,vscale,wscale);
			tVerts[i_v] = uvw_verts[i_v];
			uvw_data->v[i_v] = uvw;
			}

		for ( int i_f=0; i_f<uvw_data->num_f; i_f++ ) {
			tvPatch[i_f].tv[0] = uvw_faces[i_f].vtx[0];
			tvPatch[i_f].tv[1] = uvw_faces[i_f].vtx[1];
			tvPatch[i_f].tv[2] = uvw_faces[i_f].vtx[2];
			tvPatch[i_f].tv[3] = uvw_faces[i_f].vtx[3];
			uvw_data->f[i_f].deg = 4;
			uvw_data->f[i_f].vtx = new int[4];
			uvw_data->f[i_f].vtx[0] = uvw_faces[i_f].vtx[0];
			uvw_data->f[i_f].vtx[1] = uvw_faces[i_f].vtx[1];
			uvw_data->f[i_f].vtx[2] = uvw_faces[i_f].vtx[2];
			uvw_data->f[i_f].vtx[3] = uvw_faces[i_f].vtx[3];
			}

		uvw_data->valid_group_key = valid_group;

		applying_mapping = FALSE;
		return TRUE;
		}

	int i, j;
	int newCT = 0, numTV = mesh->getNumVerts();
	Tab<Point3> newUVs;
	DWORDTab refCts;

	int ct = 0;
	int pct = mesh->getNumPatches();
	int vct = mesh->getNumVerts();

	mesh->setNumMapVerts(1,vct);
	mesh->setNumMapPatches(1,pct);
	ct = vct;

	for (i = 0; i < pct; i++)
		{
		int a,b,c,d;
		a  = mesh->patches[i].v[0];
		b  = mesh->patches[i].v[1];
		c  = mesh->patches[i].v[2];
		d  = mesh->patches[i].v[3];
		mesh->getMapPatch(1,i).setTVerts(a,b,c,d);
		}

	BitArray vs;
	vs.SetSize(numTV);
	vs.ClearAll();
	refCts.SetCount(numTV + newCT);
	for (i=0; i<numTV + newCT; i++) refCts[i] = 0;
	for (i=0; i<pct; i++) {
		Patch &p = mesh->patches[i];
		for (j=0; j<p.type; j++) {
			refCts[p.v[j]]++;
			if (fs[i]) vs.Set(p.v[j]);
			}
		}
	BOOL nonesel = FALSE;
	if (vs.NumberSet() == 0) nonesel = TRUE;

	Tab<Point3> mapVerts;
	mapVerts.SetCount(vct);

	if (mapping_type != MAP_TL_BOX && mapping_type != MAP_TL_CYLCAP && mapping_type != MAP_TL_FACE) {
		for (i=0; i<vct; i++) {
			PatchVert pv = mesh->getVert(i);
			if (vs[i] || nonesel)	mapVerts[i] = MapPoint(pv.p,mapTM,t);
			else					mapVerts[i] = Point3(BIGFLOAT,BIGFLOAT,BIGFLOAT);
			}

		for (i=0; i<pct; i++) {
			MapPatch(mesh,i,mapTM,newUVs,mapVerts,refCts);
			}
	} else {
		for (i=0; i<pct; i++) {
			BOOL mf = nonesel ? TRUE : fs[i];
			MapPatch(mesh,i,mapTM,newUVs,refCts,mf);
			}

		}

	if (newUVs.Count()) {
		mesh->setNumMapVerts(1,numTV+newUVs.Count(),TRUE);
		for(i=0; i<newUVs.Count(); i++) {
			mesh->tVerts[1][i+numTV] = newUVs[i];
			}
		}
	// Apply tiling
	int uflip = 0, vflip = 0, wflip = 0;

	if (utile < 0.0f) {
		uflip = 1;
		utile = (float)fabs(utile);
		}
	if (vtile < 0.0f) {
		vflip = 1;
		vtile = (float)fabs(vtile);
		}
	if (wtile < 0.0f) {
		wflip = 1;
		wtile = (float)fabs(wtile);
		}

	for (i=0; i<mesh->getNumMapVerts(1); i++) {
		Point3 uvw = mesh->tVerts[1][i];

		uvw.x = uvw.x*utile;
		uvw.y = uvw.y*vtile;
		uvw.z = uvw.z*wtile;

		if (uflip) uvw.x =  1.0f - uvw.x;
		if (vflip) uvw.y =  1.0f - uvw.y;
		if (wflip) uvw.z =  1.0f - uvw.z;

		uvw.x -= GetOffsetU(t);
		uvw.y -= GetOffsetV(t);
		uvw.z -= GetOffsetW(t);

		if (mapping_type == MAP_TL_SPLINE) {
			float tile = tileCrv.GetValue(uvw.y);
			float offset = (tile - 1.0f) / 2.0f;
			uvw.x = uvw.x * tile - offset;

			float radius = wTileCrv.GetValue(uvw.y);
			float height = gizmo_height;
			if ( belt )
				height = 1.0f;
			uvw.z = uvw.z/(radius * height);
			}
		XForm(uvw,umove,vmove,wmove,urotate,vrotate,wrotate,uscale,vscale,wscale);

		mesh->tVerts[1][i] = uvw;
		}

	TVPatch *tvPatch = mesh->tvPatches[1];

	uvw_data->DeleteUVWData();
	uvw_data->num_v = mesh->getNumMapVerts(1);
	uvw_data->num_f = mesh->getNumPatches();
	uvw_data->v = new Point3[uvw_data->num_v];
	uvw_data->f = new PolyFace[uvw_data->num_f];

	for ( int i_v=0; i_v<uvw_data->num_v; i_v++ ) {
		uvw_data->v[i_v] = mesh->tVerts[1][i_v];
		}

	for ( int i_f=0; i_f<uvw_data->num_f; i_f++ ) {
		uvw_data->f[i_f].deg = 4;
		uvw_data->f[i_f].vtx = new int[4];
		uvw_data->f[i_f].vtx[0] = tvPatch[i_f].tv[0];
		uvw_data->f[i_f].vtx[1] = tvPatch[i_f].tv[1];
		uvw_data->f[i_f].vtx[2] = tvPatch[i_f].tv[2];
		uvw_data->f[i_f].vtx[3] = tvPatch[i_f].tv[3];
		}

	uvw_data->valid_group_key = valid_group;

	applying_mapping = FALSE;

	force_apply = FALSE;

	return TRUE;
	}

void UVWProyector::DefaultUVW(PatchMesh *mesh, PolyUVWData * uvw_data) {
	mesh->setNumMapVerts(1,mesh->getNumVerts());
	mesh->setNumMapPatches(1,mesh->getNumPatches());

	uvw_data->DeleteUVWData();
	uvw_data->num_v = mesh->getNumVerts();
	uvw_data->num_f = mesh->getNumPatches();
	uvw_data->v = new Point3[uvw_data->num_v];
	uvw_data->f = new PolyFace[uvw_data->num_f];

	for ( int i_v=0; i_v<uvw_data->num_v; i_v++ ) {
		mesh->tVerts[1][i_v] = mesh->verts[i_v].p;
		uvw_data->v[i_v] = mesh->verts[i_v].p;
		}

	for ( int i_f=0; i_f<uvw_data->num_f; i_f++ ) {
		int a  = mesh->patches[i_f].v[0];
		int b  = mesh->patches[i_f].v[1];
		int c  = mesh->patches[i_f].v[2];
		int d  = mesh->patches[i_f].v[3];
		mesh->getMapPatch(1,i_f).setTVerts(a,b,c,d);
		uvw_data->f[i_f].deg = 4;
		uvw_data->f[i_f].vtx = new int[4];
		uvw_data->f[i_f].vtx[0] = mesh->patches[i_f].v[0];
		uvw_data->f[i_f].vtx[1] = mesh->patches[i_f].v[1];
		uvw_data->f[i_f].vtx[2] = mesh->patches[i_f].v[2];
		uvw_data->f[i_f].vtx[3] = mesh->patches[i_f].v[3];
		}

	uvw_data->valid_group_key = valid_group;
	}


//  TLTODO: Que haga un smooth cubico al calcular el attenuation...
//	-- GetAttenuation ---------------------------------------------
float UVWProyector::GetAtt(Point3 p) {

	if ( xform_att )
		UnXForm(p,move_u,move_v,move_w,rotate_u,rotate_v,rotate_w,scale_u,scale_v,scale_w);

	float at, tiu, tiv, tiw;
	Point3 v;
	float aw,au,av,ruv,up,vp,rus,rvs,rue,rve,ur,vr,tt,rad,rs,re,atr;

	tiu = p.x;
	tiv = p.y;
	tiw = p.z;

	if ( float(fabs(tiu * 2.0f - 1.0f)) > (attUs + attUo) && 
		 float(fabs(tiv * 2.0f - 1.0f)) > (attVs + attVo) &&
		 (attUs+attVs+attUo+attVo != 0.0f)) 
		 return 0.0f;

	// Atenuacion general
	at = 1.0f - attGl / 100.0f;
	au = av = aw = 1.0f;

	// Atenuacion en W
	if ( mapping_type==MAP_TL_PLANAR || (mapping_type==MAP_TL_SPLINE&&belt) ) {
		float adjust = 1.0f;
		tiw = tiw - GetTileW(0) / 2.0f;
		tiw *= 2.0f;
		tiw = float(fabs(tiw));
		if (attWo) aw = (tiw - (attWs + attWo)) / (0.0f - attWo);
		else aw = (tiw - attWs) / -0.0001f;
		if (aw > 1.0f) aw = 1.0f;
		if (aw < 0.0f) aw = 0.0f;
		if (attWo == 0.0f && attWs == 0.0f) aw = 1.0f;
	} else {
		if (attWo) aw = (float(tiw) - (attWs + attWo)) / (0.0f - attWo);
		else aw = (float(tiw) - attWs) / -0.0001f;
		if (aw > 1.0f) aw = 1.0f;
		if (aw < 0.0f) aw = 0.0f;
		
		if (attWo == 0.0f && attWs == 0.0f) aw = 1.0f;
		}

	// Atenuacion en U
	if (attUs + attUo != 0.0f) {
		if (attUo)	au = (float(fabs(tiu * 2.0f - 1.0f)) - (attUs + attUo)) / (0.0f - attUo);
		else au = (float(fabs(tiu * 2.0f - 1.0f)) - attUs) / -0.0001f;
		if (au > 1.0f) au = 1.0f;
		if (au < 0.0f) au = 0.0f;
		au = au*au*(3.0f-2.0f*au);
		}

	// Atenuacion en V
	if (attVs + attVo != 0.0f) {
		if (attVo) av = (float(fabs(tiv * 2.0f - 1.0f)) - (attVs + attVo)) / (0.0f - attVo);
		else av = (float(fabs(tiv * 2.0f - 1.0f)) - attVs) / -0.0001f;
		if (av > 1.0f) av = 1.0f;
		if (av < 0.0f) av = 0.0f;
		av = av*av*(3.0f-2.0f*av);
		}

	// Atenuacion en los quadrantes
	if (!(attVs + attVo ==0.0f) && !(attUs + attUo ==0.0f) && attRad > 0.01f) {
		if ((float(fabs(tiu * 2.0f - 1.0f)) > (attUs * (1.0f - attRad / 100.0f)))
			&& (float(fabs(tiv * 2.0f - 1.0f)) > (attVs * (1.0f - attRad / 100.0f)))) {
			ruv = (attRad/100.0f);
			up  = attUs * ruv;
			vp  = attVs * ruv;
			rus = up;
			rvs = vp;
			rue = (attUs + attUo) - (attUs - rus);
			rve = (attVs + attVo) - (attVs - rvs);
			ur  = (float)fabs(tiu * 2.0f - 1.0f) - attUs * (1.0f - ruv);
			vr  = (float)fabs(tiv * 2.0f - 1.0f) - attVs * (1.0f - ruv);
			tt  = float(atan(vr/ur));
			rad = (float)sqrt(ur * ur + vr * vr);
			rs  = (float)sqrt((rus*rus*rvs*rvs) / (rvs*rvs*cos(tt)*cos(tt) + rus*rus*sin(tt)*sin(tt)));
			re  = (float)sqrt((rue*rue*rve*rve) / (rve*rve*cos(tt)*cos(tt) + rue*rue*sin(tt)*sin(tt)));
			// Protejamonos de la singularidad
			if( (re - rs) < 0.01f) {
				atr = (rad/re);
				if (atr < 1.0f) atr = 0.0f;
				if (atr > 1.0f) atr = 1.0f;
			} else {
				atr = (rad-rs)/(re-rs);
				if (atr > 1.0f) atr = 1.0f;
				if (atr < 0.0f) atr = 0.0f;
				}
			atr = 1.0f - atr;
			atr = atr*atr*(3.0f-2.0f*atr);
			at = at * aw * atr;
			}
		else {
			at = at * aw * au * av;
			}
		} 
	else {
		at = at * aw * au * av;
		}
	return at;
	}


// All Own Bezier Simplified stuff...
#define USTEP 0.01f
#define NUMSTEPS 100
void UVWProyector::CacheBezier(TimeValue t) {
	GetSplineNode();
	if (!spline_node) return;

	bezier_spline.Clean();

	float s, k2, sL, k;

	segLens.SetCount(0);
	splLen = 0.0f;

	ShapeObject *shape = NULL;
	ObjectState splOS = spline_node->EvalWorldState(t);
	bezInterval = splOS.obj->ObjectValidity(t);
	BezierShape bez;
	shape = (ShapeObject*)splOS.obj;
	shape->MakeBezier(t,bez);
	int spls = bez.SplineCount();

	Spline3D *spl3D;

	if ( spls>0 ) {
		spl3D = bez.GetSpline(0);
		int num_knots = spl3D->KnotCount();
		bezier_spline.SetNumKnots( num_knots );
		bezier_spline.closed = spl3D->Closed();

		for ( int i_k=0; i_k<num_knots; i_k++ ) {
			SplineKnot knot = spl3D->GetKnot(i_k);
			bezier_spline.in_vec[i_k] = knot.InVec();
			bezier_spline.knot[i_k] = knot.Knot();
			bezier_spline.out_vec[i_k] = knot.OutVec();
			}
		}

	if ( spls>0 ) {
		spl3D = bez.GetSpline(0);
		int knotCount = spl3D->KnotCount();
		for (int j=0; j<spl3D->Segments(); j++) {
			Point3 p0,p1,p2,p3, q1, q2, q3;
			SplineKnot knot = spl3D->GetKnot(j);
			p0 = knot.Knot();
			p1 = knot.OutVec();
			int j2= (j+1) % knotCount;
			knot = spl3D->GetKnot(j2);
			p2 = knot.InVec();
			p3 = knot.Knot();

			q1 = p1 - p0;
			q2 = p2 - p0;
			q3 = p3 - p0;

			Point3 pa, pb;

			pa = Point3(0,0,0);
			k = USTEP;
			
			sL = 0.0f;
			for(int aS = 1; aS < NUMSTEPS; ++aS, k+=USTEP) {
				s = 1.0f - k;
				k2 = k * k;
				pb = (3.0f*s*(k*s*q1+k2*q2)+k*k2*q3);
				sL += Length(pb - pa);
				pa = pb;
				}
			splLen += sL;
			segLens.Append(1,&sL);
			}
		}

	float perc = 0.0f;
	segPers.SetCount(segLens.Count());
	for (int i=0; i<segLens.Count(); i++) {
		perc += segLens[i]/splLen;
		segPers[i] = perc;
		}
	RebuildFieldNormals(t);

	rebuildBezier = FALSE;
	}

Point3 BezierTan(float k, Point3 p0, Point3 p1, Point3 p2, Point3 p3) {
	float s = 1.0f - k;
	float k2 = k * k;

	return 3.0f*(s*s*(p1-p0)+2.0f*k*s*(p2-p1)+k2*(p3-p2));
	}

void UVWProyector::RebuildFieldNormals(TimeValue t) {
	GetSplineNode();
	if (spline_node) {
		Point3 v1,v2;
		v1 = Isp(0.0f,t) - Isp(0.5f,t);
		v2 = Isp(1.0f,t) - Isp(0.5f,t);
		normal = Normalize(v1 ^ v2);

		int num_knots = bezier_spline.num_points;
		int num_segments = bezier_spline.NumSegments();

		Tab <Point3> segNormals;
		segNormals.SetCount(0);
	//	Spline3D *spl3D;
	//	spl3D = bez.GetSpline(0);
		int knotCount = num_knots;//spl3D->KnotCount();
		segNormals.SetCount(num_segments);//spl3D->Segments());
		vectors.SetCount(knotCount);
		for(int j=0; j<num_segments; j++) { //spl3D->Segments(); j++) {
			// Field Normals...
			Point3 p0,p1,p2,p3;
		//	SplineKnot knot = spl3D->GetKnot(j);
			p0 = bezier_spline.GetKnot(j);// knot.Knot();
			p1 = bezier_spline.GetOutVec(j);// knot.OutVec();
			int j2= (j+1) % knotCount;
		//	knot = spl3D->GetKnot(j2);
			p2 = bezier_spline.GetInVec(j2); // knot.InVec();
			p3 = bezier_spline.GetKnot(j2); // knot.Knot();

			Point3 n1 = (p1 - p0) ^ (p3 - p0);
			Point3 n2 = (p2 - p3) ^ (p0 - p3);

			segNormals[j] = (n1 + n2);

			// Bezier Kind Normals

			Point3 bN,fS,tan;
			tan = Normalize(p1-p0);
			if (j==0) {
				// TODO:
				// Es posible que la linea cause una singularidad con este bN
				bN = Point3(0,0,1);
				vectors[0].SetVector(Normalize((tan^bN)^tan));
				}

			bN = vectors[j].GetVector();
			tan = BezierTan(1.0f/3.0f,p0,p1,p2,p3);
			vectors[j].SetOutVector(Normalize((tan^bN)^tan));

			bN = vectors[j].GetOutVector();
			tan = BezierTan(2.0f/3.0f,p0,p1,p2,p3);
			vectors[j2].SetInVector(Normalize((tan^bN)^tan));

			bN = vectors[j2].GetInVector();
			tan = p3-p2;
			vectors[j2].SetVector(Normalize((tan^bN)^tan));
			}
		}
	}


Point3 UVWProyector::Isp(int spl, int seg, float k, Point3 &tan, TimeValue t) {
	int num_knots = bezier_spline.num_points;
	if ( seg<bezier_spline.NumSegments() ) {
		Point3 p0,p1,p2,p3;

		p0 = bezier_spline.GetKnot(seg);
		p1 = bezier_spline.GetOutVec(seg);
		int next_seg = (seg+1)%num_knots;
		p2 = bezier_spline.GetInVec(next_seg);
		p3 = bezier_spline.GetKnot(next_seg);

		float s = 1.0f - k;
		float k2 = k * k;

		tan = 3.0f*(s*s*(p1-p0)+2.0f*k*s*(p2-p1)+k2*(p3-p2));
		return (( s*p0 + (3.0f*k)*p1)*s + (3.0f*k2)* p2)*s + k*k2*p3;
		}
	return Point3(0,0,0);
	}

Point3 UVWProyector::Isp(int spl, int seg, float k, Point3 &tan, Point3 &norm, TimeValue t) {
	int num_knots = bezier_spline.num_points;
	if ( seg<bezier_spline.NumSegments() ) {
		Point3 p0,p1,p2,p3;

		p0 = bezier_spline.GetKnot(seg);
		p1 = bezier_spline.GetOutVec(seg);
		int next_seg = (seg+1)%num_knots;
		p2 = bezier_spline.GetInVec(next_seg);
		p3 = bezier_spline.GetKnot(next_seg);

		float s = 1.0f - k;
		float k2 = k * k;

		tan = 3.0f*(s*s*(p1-p0)+2.0f*k*s*(p2-p1)+k2*(p3-p2));
		switch (GetNormalType(t)) {
			case 0:
				norm = CrossProd(tan,normal);
				break;
			case 1: 
				norm = (tan^normal)^tan;
				break;
			case 2: {
					int seg2 = (seg+1) % bezier_spline.NumSegments();
					if (seg2 >= vectors.Count()) seg2 = 0;
					Point3 v0 = vectors[seg].GetVector();
					Point3 v1 = vectors[seg].GetOutVector();
					Point3 v2 = vectors[seg2].GetInVector();
					Point3 v3 = vectors[seg2].GetVector();
					Point3 vector = (( s*v0 + (3.0f*k)*v1)*s + (3.0f*k2)* v2)*s + k*k2*v3;
					norm = (tan^vector)^tan;
					}
				break;
			}
		return (( s*p0 + (3.0f*k)*p1)*s + (3.0f*k2)* p2)*s + k*k2*p3;
		}
	return Point3(0,0,0);
	}


Point3 UVWProyector::Isp(int spl, int seg, float k, TimeValue t) {
	int num_knots = bezier_spline.num_points;
	if ( seg<bezier_spline.NumSegments() ) {
		Point3 p0,p1,p2,p3;

		p0 = bezier_spline.GetKnot(seg);
		p1 = bezier_spline.GetOutVec(seg);
		int next_seg = (seg+1)%num_knots;
		p2 = bezier_spline.GetInVec(next_seg);
		p3 = bezier_spline.GetKnot(next_seg);

		float s = 1.0f - k;
		float k2 = k * k;

		return (( s*p0 + (3.0f*k)*p1)*s + (3.0f*k2)* p2)*s + k*k2*p3;
		}
	return Point3(0,0,0);
	}


Point3 UVWProyector::Isp(float k, Point3 &tan, TimeValue t) {
	// Esta trampita es por ahora...
	int spl = 0;
	int num_knots = bezier_spline.num_points;
	int num_segments = bezier_spline.NumSegments();
	
	if (k>=1.0f) {
		return bezier_spline.GetKnot( bezier_spline.closed?0:bezier_spline.num_points-1 );
		}

	float lSeg = 1.0f/float(num_segments);
	int nSeg = int(k/lSeg);
	k = (k - float(nSeg)*lSeg)/lSeg;

	Point3 p0,p1,p2,p3;

	p0 = bezier_spline.GetKnot(nSeg);
	p1 = bezier_spline.GetOutVec(nSeg);
	int next_seg = (nSeg+1)%num_knots;
	p2 = bezier_spline.GetInVec(next_seg);
	p3 = bezier_spline.GetKnot(next_seg);

	float s = 1.0f - k;
	float k2 = k * k;

	tan = 3.0f*(s*s*(p1-p0)+2.0f*k*s*(p2-p1)+k2*(p3-p2));
	return (( s*p0 + (3.0f*k)*p1)*s + (3.0f*k2)* p2)*s + k*k2*p3;
	}

Point3 UVWProyector::Isp(float k, TimeValue t) {
	// Esta trampita es por ahora...
	int spl = 0;
	int num_knots = bezier_spline.num_points;
	int num_segments = bezier_spline.NumSegments();

	if (k>=1.0f) {
		return bezier_spline.GetKnot( bezier_spline.closed?0:bezier_spline.num_points-1 );
		}

	float lSeg = 1.0f/float(num_segments);
	int nSeg = int(k/lSeg);
	k = (k - float(nSeg)*lSeg)/lSeg;
	if ( nSeg<num_segments ) {
		Point3 p0,p1,p2,p3;

		p0 = bezier_spline.GetKnot(nSeg);
		p1 = bezier_spline.GetOutVec(nSeg);
		int next_seg = (nSeg+1)%num_knots;
		p2 = bezier_spline.GetInVec(next_seg);
		p3 = bezier_spline.GetKnot(next_seg);

		float s = 1.0f - k;
		float k2 = k * k;

		return (( s*p0 + (3.0f*k)*p1)*s + (3.0f*k2)* p2)*s + k*k2*p3;
		}
	return Point3(5,5,5);
	}
/*

Point3 UVWProyector::Isp(int spl, int seg, float k, Point3 &tan, TimeValue t) {
	Spline3D *spl3D;
	if(spl<bez.SplineCount()) {
		spl3D = bez.GetSpline(spl);
		int knotCount = spl3D->KnotCount();
		if(seg<spl3D->Segments()) {
			Point3 p0,p1,p2,p3;
			SplineKnot knot = spl3D->GetKnot(seg);
			p0 = knot.Knot();
			p1 = knot.OutVec();
			int j2= (seg+1) % knotCount;
			knot = spl3D->GetKnot(j2);
			p2 = knot.InVec();
			p3 = knot.Knot();

			float s = 1.0f - k;
			float k2 = k * k;

			tan = 3.0f*(s*s*(p1-p0)+2.0f*k*s*(p2-p1)+k2*(p3-p2));
			return (( s*p0 + (3.0f*k)*p1)*s + (3.0f*k2)* p2)*s + k*k2*p3;
			}
		}
	return Point3(0,0,0);
	}

Point3 UVWProyector::Isp(int spl, int seg, float k, Point3 &tan, Point3 &norm, TimeValue t) {
	Spline3D *spl3D;
	if(spl<bez.SplineCount()) {
		spl3D = bez.GetSpline(spl);
		int knotCount = spl3D->KnotCount();
		if(seg<spl3D->Segments()) {
			Point3 p0,p1,p2,p3;
			SplineKnot knot = spl3D->GetKnot(seg);
			p0 = knot.Knot();
			p1 = knot.OutVec();
			int j2= (seg+1) % knotCount;
			knot = spl3D->GetKnot(j2);
			p2 = knot.InVec();
			p3 = knot.Knot();

			float s = 1.0f - k;
			float k2 = k * k;

			tan = 3.0f*(s*s*(p1-p0)+2.0f*k*s*(p2-p1)+k2*(p3-p2));
			switch (GetNormalType(t)) {
				case 0:
					norm = CrossProd(tan,normal);
					break;
				case 1: 
					norm = (tan^normal)^tan;
					break;
				case 2: {
						int seg2 = (seg+1) % (spl3D->Segments()+1);
						if (seg2 >= vectors.Count()) seg2 = 0;
						Point3 v0 = vectors[seg].GetVector();
						Point3 v1 = vectors[seg].GetOutVector();
						Point3 v2 = vectors[seg2].GetInVector();
						Point3 v3 = vectors[seg2].GetVector();
						Point3 vector = (( s*v0 + (3.0f*k)*v1)*s + (3.0f*k2)* v2)*s + k*k2*v3;
						norm = (tan^vector)^tan;
						}
					break;
				}
			return (( s*p0 + (3.0f*k)*p1)*s + (3.0f*k2)* p2)*s + k*k2*p3;
			}
		}
	return Point3(0,0,0);
	}


Point3 UVWProyector::Isp(int spl, int seg, float k, TimeValue t) {
	Spline3D *spl3D;
	if(spl<bez.SplineCount()) {
		spl3D = bez.GetSpline(spl);
		int knotCount = spl3D->KnotCount();
		if(seg<spl3D->Segments()) {
			Point3 p0,p1,p2,p3;
			SplineKnot knot = spl3D->GetKnot(seg);
			p0 = knot.Knot();
			p1 = knot.OutVec();
			int j2= (seg+1) % knotCount;
			knot = spl3D->GetKnot(j2);
			p2 = knot.InVec();
			p3 = knot.Knot();

			float s = 1.0f - k;
			float k2 = k * k;

			return (( s*p0 + (3.0f*k)*p1)*s + (3.0f*k2)* p2)*s + k*k2*p3;
			}
		}
	return Point3(0,0,0);
	}


Point3 UVWProyector::Isp(float k, Point3 &tan, TimeValue t) {
	// Esta trampita es por ahora...
	int spl = 0;
	Spline3D *spl3D;
	spl3D = bez.GetSpline(spl);
	int numSegs = spl3D->Segments();
	int knotCount = spl3D->KnotCount();
	
	if (k>=1.0f) {
		return spl3D->GetKnot((numSegs) % knotCount).Knot();
		}

	float lSeg = 1.0f/float(numSegs);
	int nSeg = int(k/lSeg);
	k = (k - float(nSeg)*lSeg)/lSeg;

	Point3 p0,p1,p2,p3;

	SplineKnot knot = spl3D->GetKnot(nSeg);
	p0 = knot.Knot();
	p1 = knot.OutVec();
	int j2= (nSeg+1) % knotCount;
	knot = spl3D->GetKnot(j2);
	p2 = knot.InVec();
	p3 = knot.Knot();

	float s = 1.0f - k;
	float k2 = k * k;

	tan = 3.0f*(s*s*(p1-p0)+2.0f*k*s*(p2-p1)+k2*(p3-p2));
	return (( s*p0 + (3.0f*k)*p1)*s + (3.0f*k2)* p2)*s + k*k2*p3;
	}

Point3 UVWProyector::Isp(float k, TimeValue t) {
	// Esta trampita es por ahora...
	int spl = 0;
	Spline3D *spl3D;
	spl3D = bez.GetSpline(spl);
	int numSegs = spl3D->Segments();
	int knotCount = spl3D->KnotCount();
	
	if (k>=1.0f) {
		return spl3D->GetKnot((numSegs) % knotCount).Knot();
		}

	float lSeg = 1.0f/float(numSegs);
	int nSeg = int(k/lSeg);
	k = (k - float(nSeg)*lSeg)/lSeg;
	if(nSeg<spl3D->Segments()) {
		Point3 p0,p1,p2,p3;
		SplineKnot knot = spl3D->GetKnot(nSeg);
		p0 = knot.Knot();
		p1 = knot.OutVec();
		int j2= (nSeg+1) % knotCount;
		knot = spl3D->GetKnot(j2);
		p2 = knot.InVec();
		p3 = knot.Knot();

		float s = 1.0f - k;
		float k2 = k * k;

		return (( s*p0 + (3.0f*k)*p1)*s + (3.0f*k2)* p2)*s + k*k2*p3;
		}
	return Point3(5,5,5);
	}
*/

void UVWProyector::InitFFGrid(int npx, int npy, Point3 size) {
	if (npx < 2) npx = 2;
	gm->nx = npx;

	if (npy < 2) npy = 2;
	gm->ny = npy;

	gm->SetNumVerts(gm->nx * gm->ny);
	gm->SetNumFaces(2 * (gm->nx-1) * (gm->ny-1));

	int i,j;

	for (i=0; i<gm->nx; i++)
		for (j=0; j<gm->ny; j++) {
			float u = float(i)/float(gm->nx-1);
			float v = float(j)/float(gm->ny-1);

			int vert = j*(gm->nx) + i;
			gm->verts[vert] = Point3((u-0.5f)*size.x,(v-0.5f)*size.y,0.0f);
			gm->uvw[vert].x = u;
			gm->uvw[vert].y = v;
			gm->uvw[vert].z = 0.0f;
			}

	for (i=0; i<gm->nx-1; i++)
		for (j=0; j<gm->ny-1; j++) {
			int face = 2 * (j * (gm->nx-1) + i);
			gm->faces[face].x = (j+1) * (gm->nx) + i;
			gm->faces[face].y = j * (gm->nx) + i;
			gm->faces[face].z = (j+1) * (gm->nx) + i + 1;
			face++;
			gm->faces[face].x = j * (gm->nx) + i + 1;
			gm->faces[face].y = (j+1) * (gm->nx) + i + 1;
			gm->faces[face].z = j * (gm->nx) + i;
			}
		
	gm->CacheVNormals();
//	mod->NotifyDependents(FOREVER,PART_TEXMAP,REFMSG_CHANGE);
	}

void UVWProyector::BuildGMGrid() {
	gm->SetNumFaces(2 * (gm->nx-1) * (gm->ny-1));

	int i,j;

	for (i=0; i<gm->nx; i++)
		for (j=0; j<gm->ny; j++) {
			float u = float(i)/float(gm->nx-1);
			float v = float(j)/float(gm->ny-1);

			int vert = j*(gm->nx) + i;
			gm->uvw[vert].x = u;
			gm->uvw[vert].y = v;
			gm->uvw[vert].z = 0.0f;
			}

	for (i=0; i<gm->nx-1; i++)
		for (j=0; j<gm->ny-1; j++) {
			int face = 2 * (j * (gm->nx-1) + i);
			gm->faces[face].x = (j+1) * (gm->nx) + i;
			gm->faces[face].y = j * (gm->nx) + i;
			gm->faces[face].z = (j+1) * (gm->nx) + i + 1;
			face++;
			gm->faces[face].x = j * (gm->nx) + i + 1;
			gm->faces[face].y = (j+1) * (gm->nx) + i + 1;
			gm->faces[face].z = j * (gm->nx) + i;
			}
		
	gm->CacheVNormals();
//	mod->NotifyDependents(FOREVER,PART_TEXMAP,REFMSG_CHANGE);
	}


void UVWProyector::GMSetNumPoints(int npx, int npy, int shape, Point3 size, int resetTO, TimeValue t) {
	GetMappingType();
	GetSplineNode();
	if (!shape || (mapping_type == MAP_TL_SPLINE && !spline_node)) {
		InitFFGrid(npx,npy,size);
		return;
		}
	if (gm->nx == 0 || gm->ny == 0) {
		InitFFGrid(2,2,Point3(gizmo_length,GetWidth(t),0.0f));
		}

	GridMesh *newgm = new GridMesh(0);
	newgm->nx = npx;
	newgm->ny = npy;
	newgm->SetNumVerts(npx*npy);
	newgm->SetNumFaces(2 * (npx-1) * (npy-1));


	int i,j;
	float u, v, w = 0;
	for (i=0; i<npx; i++)
		for (j=0; j<npy; j++) {
			u = float(i)/float(npx-1);
			if (i == 0) u = 0.000001f;
			if (i == npx-1) u = 0.999999f;
			v = float(j)/float(npy-1);
			if (j == 0) v = 0.000001f;
			if (j == npy-1) v = 0.999999f;

			int vert = j*(newgm->nx) + i;
			switch (mapping_type) {
				case MAP_TL_PLANAR:
					w = 0.5f;
					break;
				case MAP_TL_CYLINDRICAL:
				case MAP_TL_CYLCAP:
				case MAP_TL_BALL:
				case MAP_TL_SPHERICAL:
				case MAP_TL_SPLINE:
					w = 1.0f;
					break;
				case MAP_TL_FFM:
					w = 0.0f;
					break;
				}
			newgm->verts[vert] = GetPosition(t,u,v,w);
			newgm->uvw[vert] = Point3(u,v,0.0f);
			}

	for (i=0; i<npx-1; i++)
		for (j=0; j<npy-1; j++) {
			int face = 2 * (j * (newgm->nx-1) + i);
			newgm->faces[face].x = (j+1) * (newgm->nx) + i;
			newgm->faces[face].y = j * (newgm->nx) + i;
			newgm->faces[face].z = (j+1) * (newgm->nx) + i + 1;
			face++;
			newgm->faces[face].x = j * (newgm->nx) + i + 1;
			newgm->faces[face].y = (j+1) * (newgm->nx) + i + 1;
			newgm->faces[face].z = j * (newgm->nx) + i;
			}
	
	ReplaceReference(3,newgm);

	gm->CacheVNormals();
	if (resetTO) {
		pblock->SetValue(uvw_tile_u,0,1.0f);
		pblock->SetValue(uvw_offset_u,0,0.0f);
		pblock->SetValue(uvw_tile_v,0,1.0f);
		pblock->SetValue(uvw_offset_v,0,0.0f);
		pblock->SetValue(uvw_tile_w,0,1.0f);
		pblock->SetValue(uvw_offset_w,0,0.0f);
		}
//	mod->NotifyDependents(FOREVER,PART_TEXMAP,REFMSG_CHANGE);
	}

int UVWProyector::GetMappingChannel() {
	int map_channel;
	pblock->GetValue(uvw_map_channel,0,map_channel,FOREVER);
	return map_channel;
	}
float UVWProyector::GetTileU(TimeValue t) {
	float fl;
	pblock->GetValue(uvw_tile_u,t,fl,FOREVER);
	return fl;
	}
float UVWProyector::GetTileV(TimeValue t) {
	float fl;
	pblock->GetValue(uvw_tile_v,t,fl,FOREVER);
	return fl;
	}
float UVWProyector::GetTileW(TimeValue t) {
	float fl;
	pblock->GetValue(uvw_tile_w,t,fl,FOREVER);
	return fl;
	}
float UVWProyector::GetOffsetU(TimeValue t) {
	float fl;
	pblock->GetValue(uvw_offset_u,t,fl,FOREVER);
	return fl;
	}
float UVWProyector::GetOffsetV(TimeValue t) {
	float fl;
	pblock->GetValue(uvw_offset_v,t,fl,FOREVER);
	return fl;
	}
float UVWProyector::GetOffsetW(TimeValue t) {
	float fl;
	pblock->GetValue(uvw_offset_w,t,fl,FOREVER);
	return fl;
	}
int UVWProyector::GetAttStatus(TimeValue t) {
	int in;
	pblock->GetValue(uvw_atton,t,in,FOREVER);
	return in;
	}
float UVWProyector::GetAttGlobal(TimeValue t) {
	float fl;
	pblock->GetValue(uvw_att,t,fl,FOREVER);
	return fl;
	}
float UVWProyector::GetAttUStart(TimeValue t) {
	float fl;
	pblock->GetValue(uvw_aus,t,fl,FOREVER);
	return fl;
	}
float UVWProyector::GetAttUOffset(TimeValue t) {
	float fl;
	pblock->GetValue(PB_AUE,t,fl,FOREVER);
	return fl;
	}
float UVWProyector::GetAttVStart(TimeValue t) {
	float fl;
	pblock->GetValue(PB_AVS,t,fl,FOREVER);
	return fl;
	}
float UVWProyector::GetAttVOffset(TimeValue t) {
	float fl;
	pblock->GetValue(PB_AVE,t,fl,FOREVER);
	return fl;
	}
float UVWProyector::GetAttWStart(TimeValue t) {
	float fl;
	pblock->GetValue(PB_AWS,t,fl,FOREVER);
	return fl;
	}
float UVWProyector::GetAttWOffset(TimeValue t) {
	float fl;
	pblock->GetValue(PB_AWE,t,fl,FOREVER);
	return fl;
	}
float UVWProyector::GetAttRad(TimeValue t) {
	float fl;
	pblock->GetValue(PB_RUV,t,fl,FOREVER);
	return fl;
	}
float UVWProyector::GetMoveU(TimeValue t) {
	float fl;
	pblock->GetValue(uvw_move_u,t,fl,FOREVER);
	return fl;
	}
float UVWProyector::GetMoveV(TimeValue t) {
	float fl;
	pblock->GetValue(uvw_move_v,t,fl,FOREVER);
	return fl;
	}
float UVWProyector::GetMoveW(TimeValue t) {
	float fl;
	pblock->GetValue(uvw_move_w,t,fl,FOREVER);
	return fl;
	}
float UVWProyector::GetRotateU(TimeValue t) {
	float fl;
	pblock->GetValue(uvw_rotate_u,t,fl,FOREVER);
	return fl;
	}
float UVWProyector::GetRotateV(TimeValue t) {
	float fl;
	pblock->GetValue(uvw_rotate_v,t,fl,FOREVER);
	return fl;
	}
float UVWProyector::GetRotateW(TimeValue t) {
	float fl;
	pblock->GetValue(uvw_rotate_w,t,fl,FOREVER);
	return fl;
	}
float UVWProyector::GetScaleU(TimeValue t) {
	float fl;
	pblock->GetValue(uvw_scale_u,t,fl,FOREVER);
	return fl;
	}
float UVWProyector::GetScaleV(TimeValue t) {
	float fl;
	pblock->GetValue(uvw_scale_v,t,fl,FOREVER);
	return fl;
	}
float UVWProyector::GetScaleW(TimeValue t) {
	float fl;
	pblock->GetValue(uvw_scale_w,t,fl,FOREVER);
	return fl;
	}
float UVWProyector::GetLength(TimeValue t) {
	pblock->GetValue(uvw_length,t,gizmo_length,FOREVER);
	return gizmo_length;
	}
float UVWProyector::GetWidth(TimeValue t) {
	pblock->GetValue(uvw_width,t,gizmo_width,FOREVER);
	return gizmo_width;
	}
float UVWProyector::GetHeight(TimeValue t) {
	pblock->GetValue(uvw_height,t,gizmo_height,FOREVER);
	return gizmo_height;
	}
int UVWProyector::GetNormalize(TimeValue t) {
	int in;
	pblock->GetValue(PB_NORMALIZE,t,in,FOREVER);
	return in;
	}
float UVWProyector::GetFFMThresh(TimeValue t) {
	float fl;
	pblock->GetValue(PB_FS,t,fl,FOREVER);
	return fl * 0.0001f;
	}
float UVWProyector::GetFrameSize(TimeValue t) {
	float fl;
	pblock->GetValue(uvw_pelt_frame,t,fl,FOREVER);
	return fl;
	}
float UVWProyector::GetPeltRigidity(TimeValue t) {
	float fl;
	pblock->GetValue(uvw_pelt_rigidity,t,fl,FOREVER);
	return fl;
	}
float UVWProyector::GetPeltStability(TimeValue t) {
	float fl;
	pblock->GetValue(uvw_pelt_stability,t,fl,FOREVER);
	return fl;
	}
int UVWProyector::GetPeltIter() {
	int ival;
	pblock->GetValue(uvw_pelt_iter,0,ival,FOREVER);
	return ival;
	}
int UVWProyector::GetAxis() {
	int in;
	pblock->GetValue(PB_AXIS,0,in,FOREVER);
	return in;
	}
int UVWProyector::GetReverse(TimeValue t) {
	int in;
	pblock->GetValue(PB_REVERSE,t,in,FOREVER);
	return in;
	}
int UVWProyector::GetNormalType(TimeValue t) {
	int in;
	pblock->GetValue(PB_NORMALS,t,in,FOREVER);
	return in;
	}
float UVWProyector::GetSplineNormalsStart() {
	return normals_start;
	}
BOOL UVWProyector::GetActiveStatus() {
	pblock->GetValue(uvw_active_status,0,active_status,FOREVER);
	return active_status;
	}
int UVWProyector::GetMappingType() {
	pblock->GetValue(uvw_mapping_type,0,mapping_type,FOREVER);
	return mapping_type;
	}
INode* UVWProyector::GetSplineNode() {
	pblock->GetValue(uvw_spline_node,0,spline_node,FOREVER);
	return spline_node;
	}
INode* UVWProyector::GetFrameNode() {
	pblock->GetValue(uvw_frame_node,0,frame_node,FOREVER);
	return spline_node;
	}

void UVWProyector::SetMappingChannel(int map_channel) {
	pblock->SetValue(uvw_map_channel,0,map_channel);
	}
void UVWProyector::SetTileU(float tu,TimeValue t) {
	pblock->SetValue(uvw_tile_u,t,tu);
	}
void UVWProyector::SetTileV(float tv,TimeValue t) {
	pblock->SetValue(uvw_tile_v,t,tv);
	}
void UVWProyector::SetTileW(float tw,TimeValue t) {
	pblock->SetValue(uvw_tile_w,t,tw);
	}
void UVWProyector::SetOffsetU(float ou,TimeValue t) {
	pblock->SetValue(uvw_offset_u,t,ou);
	}
void UVWProyector::SetOffsetV(float ov,TimeValue t) {
	pblock->SetValue(uvw_offset_v,t,ov);
	}
void UVWProyector::SetOffsetW(float ow,TimeValue t) {
	pblock->SetValue(uvw_offset_w,t,ow);
	}
void UVWProyector::SetAttStatus(int ats,TimeValue t) {
	pblock->SetValue(uvw_atton,t,ats);
	}
void UVWProyector::SetAttGlobal(float att,TimeValue t) {
	pblock->SetValue(uvw_att,t,att);
	}
void UVWProyector::SetAttUStart(float aus,TimeValue t) {
	pblock->SetValue(uvw_aus,t,aus);
	}
void UVWProyector::SetAttVStart(float avs,TimeValue t) {
	pblock->SetValue(PB_AVS,t,avs);
	}
void UVWProyector::SetAttWStart(float aws,TimeValue t) {
	pblock->SetValue(PB_AWS,t,aws);
	}
void UVWProyector::SetAttUOffset(float auo,TimeValue t) {
	pblock->SetValue(PB_AUE,t,auo);
	}
void UVWProyector::SetAttVOffset(float avo,TimeValue t) {
	pblock->SetValue(PB_AVE,t,avo);
	}
void UVWProyector::SetAttWOffset(float awo,TimeValue t) {
	pblock->SetValue(PB_AWE,t,awo);
	}
void UVWProyector::SetAttRad(float ard,TimeValue t) {
	pblock->SetValue(PB_RUV,t,ard);
	}
void UVWProyector::SetLength(TimeValue t,float v) {
	pblock->SetValue(uvw_length,0,v);
	}
void UVWProyector::SetWidth(TimeValue t,float v) {
	pblock->SetValue(uvw_width,0,v);
	}
void UVWProyector::SetHeight(TimeValue t,float v) {
	pblock->SetValue(uvw_height,t,v);
	}
void UVWProyector::SetNormalize(int nz,TimeValue t) {
	pblock->SetValue(PB_NORMALIZE,t,nz);
	}
void UVWProyector::SetFFMThresh(float th,TimeValue t) {
	pblock->SetValue(PB_FS,t,th);
	}
void UVWProyector::SetMoveU(float f,TimeValue t) {
	pblock->SetValue(uvw_move_u,t,f);
	}
void UVWProyector::SetMoveV(float f,TimeValue t) {
	pblock->SetValue(uvw_move_v,t,f);
	}
void UVWProyector::SetMoveW(float f,TimeValue t) {
	pblock->SetValue(uvw_move_w,t,f);
	}
void UVWProyector::SetRotateU(float f,TimeValue t) {
	pblock->SetValue(uvw_rotate_u,t,f);
	}
void UVWProyector::SetRotateV(float f,TimeValue t) {
	pblock->SetValue(uvw_rotate_v,t,f);
	}
void UVWProyector::SetRotateW(float f,TimeValue t) {
	pblock->SetValue(uvw_rotate_w,t,f);
	}
void UVWProyector::SetScaleU(float f,TimeValue t) {
	pblock->SetValue(uvw_scale_u,t,f);
	}
void UVWProyector::SetScaleV(float f,TimeValue t) {
	pblock->SetValue(uvw_scale_v,t,f);
	}
void UVWProyector::SetScaleW(float f,TimeValue t) {
	pblock->SetValue(uvw_scale_w,t,f);
	}
void UVWProyector::SetAxis(int ax,TimeValue t) {
	pblock->SetValue(PB_AXIS,t,ax);
	}
void UVWProyector::SetReverse(int rv,TimeValue t) {
	pblock->SetValue(PB_REVERSE,t,rv);
	}
void UVWProyector::SetNormalType(int nt,TimeValue t) {
	pblock->SetValue(PB_NORMALS,t,nt);
	}
void UVWProyector::SetFrameSize(float f, TimeValue t) {
	pblock->SetValue(uvw_pelt_frame,t,f);
	}
void UVWProyector::SetPeltRigidity(float f, TimeValue t) {
	pblock->SetValue(uvw_pelt_rigidity,t,f);
	}
void UVWProyector::SetPeltStability(float f, TimeValue t) {
	pblock->SetValue(uvw_pelt_stability,t,f);
	}
void UVWProyector::SetPeltIter(int i) {
	pblock->SetValue(uvw_pelt_iter,0,i);
	}
void UVWProyector::SetSplineNormalsStart(float ns, TimeValue t) {
	pblock->SetValue(uvw_normals_start,t,ns);
	}
void UVWProyector::SetActiveStatus(BOOL status) {
	active_status = status;
	pblock->SetValue(uvw_active_status,0,active_status);
	}
void UVWProyector::SetMappingType(int type) {
	mapping_type = type;
	pblock->SetValue(uvw_mapping_type,0,mapping_type);
	}
void UVWProyector::SetSplineNode(INode * node, TimeValue t) {
	spline_node = node;
	pblock->SetValue(uvw_spline_node,t,spline_node);
	skip_bug = TRUE;
	}

void UVWProyector::CopyUVW(UVWProyector * uvProy) {
	int i,j;

	// Copy References
	ReplaceReference(PBLOCK_REF,uvProy->pblock->Clone( DefaultRemapDir() ));
	ReplaceReference(TMCTRL_REF,uvProy->tmControl->Clone( DefaultRemapDir() ));
	ReplaceReference(SPLINE_REF,uvProy->spline);
	ReplaceReference(GRID_REF,uvProy->gm->Clone());

	// Copy Spline Mapping Curves
	angleCrv = uvProy->angleCrv;
	tileCrv = uvProy->tileCrv;
	wTileCrv = uvProy->wTileCrv;

	// UV Pelt Data
	current_num_faces = uvProy->current_num_faces;
	current_num_edges = uvProy->current_num_edges;
	current_num_verts = uvProy->current_num_verts;
	current_num_sel_faces = uvProy->current_num_sel_faces;
	current_num_sel_edges = uvProy->current_num_sel_edges;
	natural_axis = uvProy->natural_axis;
	inv_natural_axis = uvProy->inv_natural_axis;

	if ( pelt_poly_mesh ) {
		pelt_poly_mesh->DeleteThis();
		pelt_poly_mesh = NULL;
		}
	pelt_poly_mesh = new PeltPolyMesh( uvProy->pelt_poly_mesh->numf, uvProy->pelt_poly_mesh->numv );
	for ( i=0; i<pelt_poly_mesh->numf; i++ )
		pelt_poly_mesh->f[i] = uvProy->pelt_poly_mesh->f[i];
	for ( i=0; i<pelt_poly_mesh->numv; i++ )
		pelt_poly_mesh->v[i] = uvProy->pelt_poly_mesh->v[i];

	vert_to_mass.SetCount( uvProy->vert_to_mass.Count() );
	for ( i=0; i<uvProy->vert_to_mass.Count(); i++ )
		vert_to_mass[i] = uvProy->vert_to_mass[i];

	masses.SetCount( uvProy->masses.Count() );
	for ( i=0; i<uvProy->masses.Count(); i++ ) {
		masses[i] = new Mass;
		masses[i]->vert_id = uvProy->masses[i]->vert_id;
		masses[i]->o_pos = uvProy->masses[i]->o_pos;
		masses[i]->f_pos = uvProy->masses[i]->f_pos;
		masses[i]->vel = uvProy->masses[i]->vel;
		masses[i]->springs.SetCount( uvProy->masses[i]->springs.Count() );
		for ( j=0; j<uvProy->masses[i]->springs.Count(); j++ ) {
			masses[i]->springs[j].attached_to_mass = uvProy->masses[i]->springs[j].attached_to_mass;
			masses[i]->springs[j].attach_id = uvProy->masses[i]->springs[j].attach_id;
			masses[i]->springs[j].o_lenght = uvProy->masses[i]->springs[j].o_lenght;
			}
		}

	border_verts.SetCount( uvProy->border_verts.Count() );
	for ( i=0; i<uvProy->border_verts.Count(); i++ )
		border_verts[i] = uvProy->border_verts[i];

	frame_segments.SetCount( uvProy->frame_segments.Count() );
	for ( i=0; i<uvProy->frame_segments.Count(); i++ ) {
		frame_segments[i] = new FrameSegments( uvProy->frame_segments[i]->border_vert.Count() );
		frame_segments[i]->selected = uvProy->frame_segments[i]->selected;
		frame_segments[i]->start_point = uvProy->frame_segments[i]->start_point;
		frame_segments[i]->segment_length = uvProy->frame_segments[i]->segment_length;
		for ( j=0; j<uvProy->frame_segments[i]->border_vert.Count(); j++ ) 
			frame_segments[i]->border_vert[j] = uvProy->frame_segments[i]->border_vert[j];
		for ( j=0; j<uvProy->frame_segments[i]->position.Count(); j++ ) 
			frame_segments[i]->position[j] = uvProy->frame_segments[i]->position[j];
		}

	frame_segments_symmetry.SetCount( uvProy->frame_segments_symmetry.Count() );
	for ( i=0; i<uvProy->frame_segments_symmetry.Count(); i++ )
		frame_segments_symmetry[i] = uvProy->frame_segments_symmetry[i];

	symmetry_axis = uvProy->symmetry_axis;
	symmetry_center = uvProy->symmetry_center;
	}

#define VERSION_CHUNKID				0x0001
#define NAME_DATA_CHUNK				0x0010
#define TIPO_CHUNKID				0x0100
#define TLUCRV_CHUNKID				0x0150
#define OFUCRV_CHUNKID				0x0200
#define TLWCRV_CHUNKID				0x0250
#define NUM_BORDER_VERTS_CHUNKID	0x0300
#define BORDER_VERTS_CHUNKID		0x0310
#define NUM_FRAME_SEGMENTS_CHUNKID	0x0320
#define FRAME_SEGMENTS_CHUNKID		0x0330
#define NATAXIS_CHUNKID				0x0340
#define INV_NATAXIS_CHUNKID			0x0350
#define PELT_ZOOM_CHUNKID			0x0360
#define PELT_XSCROLL_CHUNKID		0x0370
#define PELT_YSCROLL_CHUNKID		0x0380
#define OLD_FFM_CHUNKID				0x0390
#define PELT_SYMM_COUNT_CHUNKID		0x0400
#define PELT_SYMM_TABLE_CHUNKID		0x0410
#define PELT_SYMM_AXIS				0x0420
#define PELT_SYMM_CENTER			0x0430
#define DATA_NUM_TVERTS_CHUNKID		0x0440
#define DATA_NUM_TPOLYS_CHUNKID		0x0450
#define DATA_TVERTS_CHUNKID			0x0460
#define DATA_TPOLYS_CHUNKID			0x0470

IOResult UVWProyector::Load(ILoad *iload) {
	ULONG nb;
	int j, i_fs;

	int version = 100000;
			
	int num_border_verts = 0;
	int num_frame_segments = 0;
	int num_border_verts_in_frame_segments = 0;
	int num_symmetry_pairs = 0;

	ParamBlock2PLCB* plcb = new ParamBlock2PLCB(versions, NUM_OLDVERSIONS, &uvw_proy_param_blk, this, 0);
	iload->RegisterPostLoadCallback(plcb);

	BOOL fix_mapping_type = FALSE;

	IOResult res;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case VERSION_CHUNKID:
				iload->Read(&version, sizeof(version), &nb);
				break;

			case NAME_DATA_CHUNK:
				TCHAR *buf;
				res=iload->ReadCStringChunk(&buf);
				descCanal = TSTR(buf);
				break;

			case TIPO_CHUNKID:
				iload->Read(&mapping_type, sizeof(mapping_type), &nb);
				SetMappingType(mapping_type);
				fix_mapping_type = TRUE;
				break;

			case TLUCRV_CHUNKID:
				int tCNp;
				iload->Read(&tCNp,sizeof(int),&nb);
				tileCrv.SetNumPoints(tCNp);
				for(j=0;j<tCNp;j++) {
					Point2 ip,p,op;
					int knd;
					iload->Read(&knd,sizeof(int),&nb);
					iload->Read(&ip,sizeof(Point2),&nb);							
					iload->Read(&p, sizeof(Point2),&nb);
					iload->Read(&op,sizeof(Point2),&nb);
					tileCrv.SetKind(j,knd);
					tileCrv.SetIn(j,ip);
					tileCrv.SetPoint(j,p);
					tileCrv.SetOut(j,op);
					}
				break;

			case OFUCRV_CHUNKID:
				int angCNp;
				iload->Read(&angCNp,sizeof(int),&nb);
				angleCrv.SetNumPoints(angCNp);
				for(j=0;j<angCNp;j++) {
					Point2 ip,p,op;
					int knd;
					iload->Read(&knd,sizeof(int),&nb);
					iload->Read(&ip,sizeof(Point2),&nb);							
					iload->Read(&p, sizeof(Point2),&nb);
					iload->Read(&op,sizeof(Point2),&nb);
					angleCrv.SetKind(j,knd);
					angleCrv.SetIn(j,ip);
					angleCrv.SetPoint(j,p);
					angleCrv.SetOut(j,op);
					}
				break;

			case TLWCRV_CHUNKID:
				int rCNp;
				iload->Read(&rCNp,sizeof(int),&nb);
				wTileCrv.SetNumPoints(rCNp);
				for(j=0;j<rCNp;j++) {
					Point2 ip,p,op;
					int knd;
					iload->Read(&knd,sizeof(int),&nb);
					iload->Read(&ip,sizeof(Point2),&nb);							
					iload->Read(&p, sizeof(Point2),&nb);
					iload->Read(&op,sizeof(Point2),&nb);
					wTileCrv.SetKind(j,knd);
					wTileCrv.SetIn(j,ip);
					wTileCrv.SetPoint(j,p);
					wTileCrv.SetOut(j,op);
					}
				break;

			case NUM_BORDER_VERTS_CHUNKID:
				iload->Read(&num_border_verts, sizeof(int), &nb);
				border_verts.SetCount(num_border_verts);
				break;

			case BORDER_VERTS_CHUNKID:
				iload->Read( border_verts.Addr(0), sizeof(int)*num_border_verts, &nb );
				break;

			case NUM_FRAME_SEGMENTS_CHUNKID:
				iload->Read(&num_frame_segments, sizeof(int), &nb);
				frame_segments.SetCount(num_frame_segments);
				break;

			case FRAME_SEGMENTS_CHUNKID:
				for ( i_fs=0; i_fs<num_frame_segments; i_fs++ ) {
					int num_bv;
					iload->Read(&num_bv,sizeof(num_bv),&nb);
					frame_segments[i_fs] = new FrameSegments(num_bv);
					iload->Read(&frame_segments[i_fs]->start_point,sizeof(Point3),&nb);
					iload->Read(frame_segments[i_fs]->border_vert.Addr(0) , sizeof(int)*num_bv, &nb );
					iload->Read(frame_segments[i_fs]->position.Addr(0) , sizeof(float)*num_bv, &nb );
					}
				break;

			case NATAXIS_CHUNKID:
				iload->Read(&natural_axis,sizeof(natural_axis),&nb);
				break;

			case INV_NATAXIS_CHUNKID:
				iload->Read(&inv_natural_axis,sizeof(inv_natural_axis),&nb);
				break;

			case PELT_ZOOM_CHUNKID:
				iload->Read(&zoom,sizeof(zoom),&nb);
				break;

			case PELT_XSCROLL_CHUNKID:
				iload->Read(&xscroll,sizeof(xscroll),&nb);
				break;

			case PELT_YSCROLL_CHUNKID:
				iload->Read(&yscroll,sizeof(yscroll),&nb);
				break;

			case OLD_FFM_CHUNKID:
				iload->Read(&use_old_ffm,sizeof(use_old_ffm),&nb);
				break;

			case PELT_SYMM_COUNT_CHUNKID:
				iload->Read(&num_symmetry_pairs,sizeof(num_symmetry_pairs),&nb);
				frame_segments_symmetry.SetCount(num_symmetry_pairs);
				break;

			case PELT_SYMM_TABLE_CHUNKID:
				iload->Read(frame_segments_symmetry.Addr(0),sizeof(IPoint2)*num_symmetry_pairs,&nb);
				break;

			case PELT_SYMM_AXIS:
				iload->Read(&symmetry_axis,sizeof(symmetry_axis),&nb);
				break;

			case PELT_SYMM_CENTER:
				iload->Read(&symmetry_center,sizeof(symmetry_center),&nb);
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK)
			return res;
		}

	if ( version==100000 ) {
		use_old_ffm = TRUE;
		}

	if ( fix_mapping_type ) {
		UVWGroupLoadCB* uglcb = new UVWGroupLoadCB(this,mapping_type);
		iload->RegisterPostLoadCallback(uglcb);
		}
	
	return IO_OK;
	}

IOResult UVWProyector::Save(ISave *isave) {
	int a = 5,b = 7;
	int j;
	ULONG nb;

	int version = TEXLAY_VERSION;

	int num_symmetry_pairs = frame_segments_symmetry.Count();

	isave->BeginChunk(VERSION_CHUNKID);
	isave->Write(&version,sizeof(int),&nb);
	isave->EndChunk();

	isave->BeginChunk(NAME_DATA_CHUNK);
	isave->WriteCString(descCanal);
	isave->EndChunk();

//	isave->BeginChunk(TIPO_CHUNKID);
//	isave->Write(&mapping_type,sizeof(mapping_type),&nb);
//	isave->EndChunk();

	isave->BeginChunk(TLUCRV_CHUNKID);
	int tCNp = tileCrv.NumPoints();
	isave->Write(&tCNp,sizeof(int),&nb);
	for (j=0; j<tCNp; j++) {
		Point2 ip = tileCrv.GetIn(j);
		Point2 p = tileCrv.GetPoint(j);
		Point2 op = tileCrv.GetOut(j);
		int knd = tileCrv.GetKind(j);
		isave->Write(&knd,sizeof(int),&nb);
		isave->Write(&ip,sizeof(Point2),&nb);
		isave->Write(&p,sizeof(Point2),&nb);
		isave->Write(&op,sizeof(Point2),&nb);
		}
	isave->EndChunk();

	isave->BeginChunk(OFUCRV_CHUNKID);
	int angCNp = angleCrv.NumPoints();
	isave->Write(&angCNp,sizeof(int),&nb);
	for (j=0; j<angCNp; j++) {
		Point2 ip = angleCrv.GetIn(j);
		Point2 p = angleCrv.GetPoint(j);
		Point2 op = angleCrv.GetOut(j);
		int knd = angleCrv.GetKind(j);
		isave->Write(&knd,sizeof(int),&nb);
		isave->Write(&ip,sizeof(Point2),&nb);
		isave->Write(&p,sizeof(Point2),&nb);
		isave->Write(&op,sizeof(Point2),&nb);
		}
	isave->EndChunk();

	isave->BeginChunk(TLWCRV_CHUNKID);
	int rCNp = wTileCrv.NumPoints();
	isave->Write(&rCNp,sizeof(int),&nb);
	for (j=0; j<rCNp; j++) {
		Point2 ip = wTileCrv.GetIn(j);
		Point2 p = wTileCrv.GetPoint(j);
		Point2 op = wTileCrv.GetOut(j);
		int knd = wTileCrv.GetKind(j);
		isave->Write(&knd,sizeof(int),&nb);
		isave->Write(&ip,sizeof(Point2),&nb);
		isave->Write(&p,sizeof(Point2),&nb);
		isave->Write(&op,sizeof(Point2),&nb);
		}
	isave->EndChunk();

	if ( border_verts.Count() ) {
		int num_border_verts = border_verts.Count();
		
		isave->BeginChunk(NUM_BORDER_VERTS_CHUNKID);
		isave->Write(&num_border_verts,sizeof(int),&nb);
		isave->EndChunk();

		isave->BeginChunk(BORDER_VERTS_CHUNKID);
		isave->Write( border_verts.Addr(0) , sizeof(int)*num_border_verts , &nb );
		isave->EndChunk();

		int num_frame_segments = frame_segments.Count();

		isave->BeginChunk(NUM_FRAME_SEGMENTS_CHUNKID);
		isave->Write(&num_frame_segments,sizeof(num_frame_segments),&nb);
		isave->EndChunk();

		isave->BeginChunk(FRAME_SEGMENTS_CHUNKID);
		for ( int i_fs=0; i_fs<num_frame_segments; i_fs++ ) {
			int num_bv = frame_segments[i_fs]->border_vert.Count();
			isave->Write(&num_bv,sizeof(int),&nb);
			isave->Write(&frame_segments[i_fs]->start_point,sizeof(Point3),&nb);
			isave->Write(frame_segments[i_fs]->border_vert.Addr(0) , sizeof(int)*num_bv , &nb );
			isave->Write(frame_segments[i_fs]->position.Addr(0) , sizeof(float)*num_bv , &nb );
			}
		isave->EndChunk();

		isave->BeginChunk(NATAXIS_CHUNKID);
		isave->Write(&natural_axis,sizeof(natural_axis),&nb);
		isave->EndChunk();

		isave->BeginChunk(INV_NATAXIS_CHUNKID);
		isave->Write(&inv_natural_axis,sizeof(inv_natural_axis),&nb);
		isave->EndChunk();

		isave->BeginChunk(PELT_ZOOM_CHUNKID);
		isave->Write(&zoom,sizeof(zoom),&nb);
		isave->EndChunk();

		isave->BeginChunk(PELT_XSCROLL_CHUNKID);
		isave->Write(&xscroll,sizeof(xscroll),&nb);
		isave->EndChunk();

		isave->BeginChunk(PELT_YSCROLL_CHUNKID);
		isave->Write(&yscroll,sizeof(yscroll),&nb);
		isave->EndChunk();
		}

	isave->BeginChunk(OLD_FFM_CHUNKID);
	isave->Write(&use_old_ffm,sizeof(use_old_ffm),&nb);
	isave->EndChunk();

	isave->BeginChunk(PELT_SYMM_COUNT_CHUNKID);
	isave->Write(&num_symmetry_pairs,sizeof(num_symmetry_pairs),&nb);
	isave->EndChunk();

	if ( num_symmetry_pairs ) {
		isave->BeginChunk(PELT_SYMM_TABLE_CHUNKID);
		isave->Write(frame_segments_symmetry.Addr(0),sizeof(IPoint2)*num_symmetry_pairs,&nb);
		isave->EndChunk();
		}

	isave->BeginChunk(PELT_SYMM_AXIS);
	isave->Write(&symmetry_axis,sizeof(symmetry_axis),&nb);
	isave->EndChunk();

	isave->BeginChunk(PELT_SYMM_CENTER);
	isave->Write(&symmetry_center,sizeof(symmetry_center),&nb);
	isave->EndChunk();

	return IO_OK;
	}

RefTargetHandle UVWProyector::Clone(RemapDir& remap) {
	int i,j;

	UVWProyector *ncuvp = new UVWProyector(FALSE);
	ncuvp->valid_group = 0;
	ncuvp->descCanal = descCanal;

	ncuvp->ReplaceReference(PBLOCK_REF,	pblock->Clone(remap));
	ncuvp->ReplaceReference(TMCTRL_REF,	tmControl->Clone(remap));
	ncuvp->ReplaceReference(SPLINE_REF,	spline);
	ncuvp->ReplaceReference(GRID_REF,	gm->Clone(remap));

	ncuvp->angleCrv = angleCrv;
	ncuvp->tileCrv  = tileCrv;
	ncuvp->wTileCrv = wTileCrv;

	// UV Pelt Data
	ncuvp->current_num_faces = current_num_faces;
	ncuvp->current_num_edges = current_num_edges;
	ncuvp->current_num_verts = current_num_verts;
	ncuvp->current_num_sel_faces = current_num_sel_faces;
	ncuvp->current_num_sel_edges = current_num_sel_edges;
	ncuvp->natural_axis = natural_axis;
	ncuvp->inv_natural_axis = inv_natural_axis;

	if ( pelt_poly_mesh ) {
		ncuvp->pelt_poly_mesh = new PeltPolyMesh( pelt_poly_mesh->numv, pelt_poly_mesh->numf );
		for ( i=0; i<pelt_poly_mesh->numv; i++ )
			ncuvp->pelt_poly_mesh->v[i] = pelt_poly_mesh->v[i];
		for ( i=0; i<pelt_poly_mesh->numf; i++ )
			ncuvp->pelt_poly_mesh->f[i] = pelt_poly_mesh->f[i];
		}

	ncuvp->vert_to_mass.SetCount( vert_to_mass.Count() );
	for ( i=0; i<vert_to_mass.Count(); i++ )
		ncuvp->vert_to_mass[i] = vert_to_mass[i];

	ncuvp->masses.SetCount( masses.Count() );
	for ( i=0; i<masses.Count(); i++ ) {
		ncuvp->masses[i] = new Mass;
		ncuvp->masses[i]->vert_id = masses[i]->vert_id;
		ncuvp->masses[i]->o_pos = masses[i]->o_pos;
		ncuvp->masses[i]->f_pos = masses[i]->f_pos;
		ncuvp->masses[i]->vel = masses[i]->vel;
		ncuvp->masses[i]->springs.SetCount( masses[i]->springs.Count() );
		for ( j=0; j<masses[i]->springs.Count(); j++ ) {
			ncuvp->masses[i]->springs[j].attached_to_mass = masses[i]->springs[j].attached_to_mass;
			ncuvp->masses[i]->springs[j].attach_id = masses[i]->springs[j].attach_id;
			ncuvp->masses[i]->springs[j].o_lenght = masses[i]->springs[j].o_lenght;
			}
		}

	ncuvp->border_verts.SetCount( border_verts.Count() );
	for ( i=0; i<border_verts.Count(); i++ )
		ncuvp->border_verts[i] = border_verts[i];
	
	ncuvp->frame_segments.SetCount( frame_segments.Count() );
	for ( i=0; i<frame_segments.Count(); i++ ) {
		ncuvp->frame_segments[i] = new FrameSegments( frame_segments[i]->border_vert.Count() );
		ncuvp->frame_segments[i]->selected = frame_segments[i]->selected;
		ncuvp->frame_segments[i]->start_point = frame_segments[i]->start_point;
		ncuvp->frame_segments[i]->segment_length = frame_segments[i]->segment_length;
		for ( j=0; j<frame_segments[i]->border_vert.Count(); j++ ) 
			ncuvp->frame_segments[i]->border_vert[j] = frame_segments[i]->border_vert[j];
		for ( j=0; j<frame_segments[i]->position.Count(); j++ ) 
			ncuvp->frame_segments[i]->position[j] = frame_segments[i]->position[j];
		}

	ncuvp->frame_segments_symmetry.SetCount( frame_segments_symmetry.Count() );
	for ( i=0; i<frame_segments_symmetry.Count(); i++ )
		ncuvp->frame_segments_symmetry[i] = frame_segments_symmetry[i];
	
	ncuvp->symmetry_axis = symmetry_axis;
	ncuvp->symmetry_center = symmetry_center;

	return ncuvp;
	}

// JW Code Change: NotifyRefChanged signature changed in 3ds Max 2015+
#if MAX_VERSION_MAJOR < 17
RefResult UVWProyector::NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message) 
#else
RefResult UVWProyector::NotifyRefChanged(const Interval& changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message, BOOL propagate)
#endif
{
	if ( hTarget == pblock ) {

		int ti;
		int id = pblock->LastNotifyParamID(ti);

		if ( tl && id>=0 && id<max_uvw ) {
			tl->UpdateUIItem(id,this);
			}

		if ( id!=-1 )
			valid_group = 0;

		if ( id==uvw_spline_node && applying_mapping ) {
			return REF_STOP;
			}
		if ( (id==uvw_spline_node || id==uvw_frame_node) && applying_mapping ) {
			return REF_STOP;
			}
		}

	if ( hTarget == spline ) {
		tl->UpdateUIItem(-1,this);
		valid_group = 0;
		}

	switch (message)
	{
		case REFMSG_CHANGE: {
			int id = pblock->LastNotifyParamID();
			if ( id==uvw_spline_node )
				rebuildBezier = TRUE;
			}
			break;

		case REFMSG_GET_PARAM_DIM:
		{
			GetParamDim *gpd = (GetParamDim*)partID;			
			switch (gpd->index) {
				case uvw_length:
				case uvw_width:
				case uvw_height:
					gpd->dim = stdWorldDim;
					break;
				default:
					gpd->dim = defaultDim;
					break;
			}
			return REF_STOP; 
		}

		case REFMSG_TARGET_DELETED:
			if (hTarget == spline) spline = NULL;
			NotifyDependents(FOREVER,PART_TEXMAP,REFMSG_CHANGE);
			valid_group = 0;
			break;

	}
	return REF_SUCCEED;
}

RefTargetHandle UVWProyector::GetReference(int i) {
	switch (i) {
		case PBLOCK_REF:	return pblock;
		case TMCTRL_REF:	return tmControl;
		case SPLINE_REF:	return spline;
		case GRID_REF:		return gm;
		default: 	return NULL;
		}
	}

void UVWProyector::SetReference(int i, RefTargetHandle rtarg) {
	switch (i) {
		case PBLOCK_REF:		pblock = (IParamBlock2*)rtarg; break;
		case TMCTRL_REF:		tmControl = (Control*)rtarg; break;
		case SPLINE_REF: 
			spline = (INode*)rtarg; 
			break;
		case GRID_REF:	
		//	if (gm) delete gm;
			gm = (GridMesh *)rtarg; 
			break;
		}
	}

int UVWProyector::NumSubs()  { 
	if (GetMappingType() == MAP_TL_FFM) return 3;
	else return 2;
	}


Animatable* UVWProyector::SubAnim(int i) {
	switch (i) {
		case 0:				return tmControl;
		case 1:				return pblock;
		case 2:				return gm;
		default: 	return NULL;   
		}
	}

#if MAX_VERSION_MAJOR < 24
TSTR UVWProyector::SubAnimName(int i) 
#else
TSTR UVWProyector::SubAnimName(int i, bool localized) 
#endif

{ 
	switch(i) {
		case 0:				return _T("Gizmo");
		case 1:				return GetString(IDS_TL_PARAMETERS);
		case 2:				return _T("Free form grid");
		default: 			return TSTR(_T(""));
		}
	}	
        
int UVWProyector::SubNumToRefNum(int subNum){
	if (subNum == 0) return 1;
	if (subNum == 1) return 0;
	if (subNum == 2) return 4;
	else return -1;
	}	

Matrix3 UVWProyector::CompMatrix(TimeValue t,ModContext *mc, Matrix3 *ntm,BOOL applySize, BOOL applyAxis)
	{

#if MAX_VERSION_MAJOR < 24
	Matrix3 tm(1);
#else
	Matrix3 tm;
#endif

	Interval valid;
	
	int type = GetMappingType();
	
	if (tmControl) {
		tmControl->GetValue(t,&tm,valid,CTRL_RELATIVE);
		}
	
	// Rotate icon	
	switch (type) {
		case MAP_TL_SPLINE:
		case MAP_TL_FFM:
		case MAP_TL_PLANAR:
		case MAP_TL_BOX:
			tm.PreRotateZ(PI);
			break;
		
		case MAP_TL_BALL:
		case MAP_TL_SPHERICAL:
		case MAP_TL_CYLINDRICAL:
		case MAP_TL_CYLCAP:
			tm.PreRotateZ(HALFPI);
			break;
		}

	if (applyAxis && type != MAP_TL_FFM && type != MAP_TL_SPLINE) {
		switch (GetAxis()) {
			case 0:
				tm.PreRotateY(-HALFPI);
				break;
			case 1:	
				tm.PreRotateX(HALFPI);
				break;
			}
		}

	if (applySize && type != MAP_TL_FFM && type != MAP_TL_SPLINE) {
		Point3 s;
		s.x = gizmo_width;
		s.y = gizmo_length;
		s.z = gizmo_height;
		switch (type) {
			case MAP_TL_CYLINDRICAL:
			case MAP_TL_CYLCAP:
			case MAP_TL_PLANAR:
				s.x *= 0.5f;
				s.y *= 0.5f;
				break;
			
			case MAP_TL_BALL:
			case MAP_TL_SPHERICAL:
			case MAP_TL_BOX:
				s *= 0.5f;				
				break;
			}
		tm.PreScale(s);
		}

	if (mc && mc->tm) {
		tm = tm * Inverse(*mc->tm);
		}

	if (ntm) {
		tm = tm * *ntm;
		}

	return tm;
	}

Matrix3 UVWProyector::CGMatrix(TimeValue t)
	{

#if MAX_VERSION_MAJOR < 24
	Matrix3 tm(1);
#else
	Matrix3 tm;
#endif

	Interval valid;
	
	int type = GetMappingType();

	if (tmControl) {
		tmControl->GetValue(t,&tm,valid,CTRL_RELATIVE);
		}
	
	// Rotate icon	
	if (1) {
		switch (type) {
			case MAP_TL_SPLINE:
			case MAP_TL_BOX:
			case MAP_TL_PLANAR:
			case MAP_TL_FFM:
				tm.PreRotateZ(PI);
				break;
			
			case MAP_TL_BALL:
			case MAP_TL_SPHERICAL:
			case MAP_TL_CYLINDRICAL:
			case MAP_TL_CYLCAP:
				tm.PreRotateZ(HALFPI);
				break;
			}
		}

	if (type != MAP_TL_FFM && type != MAP_TL_SPLINE) {
		switch (GetAxis()) {
			case 0:
				tm.PreRotateY(-HALFPI);
				break;
			case 1:	
				tm.PreRotateX(HALFPI);
				break;
			}
		}

	if (type != MAP_TL_FFM && type != MAP_TL_SPLINE) {
		Point3 s;
		s.x = gizmo_width;
		s.y = gizmo_length;
		s.z = gizmo_height;
		switch (type) {
			case MAP_TL_CYLINDRICAL:			
			case MAP_TL_CYLCAP:
			case MAP_TL_PLANAR:
				s.x *= 0.5f;
				s.y *= 0.5f;
				break;
			
			case MAP_TL_BALL:
			case MAP_TL_SPHERICAL:
			case MAP_TL_BOX:
				s *= 0.5f;				
				break;
			}
		tm.PreScale(s);
		}
	tm = Inverse(tm);
	return tm;
	}

void UVWProyector::InitControl( ModContext &mc,Object *obj,int type,TimeValue t ) {
	Interval valid;

	Box3 box;
	Matrix3 tm;

	valid_group = 0;

	// Si no existe un tmControl, creamos uno nuevo (default)...
	// Aqui entra la primera vez que aplicamos el modifier...
	if (tmControl==NULL) {
		ReplaceReference( 1, NewDefaultMatrix3Controller() ); 
		NotifyDependents(FOREVER,0,REFMSG_CONTROLREF_CHANGE);
		// Le damos a nuestra matriz en curzo el valor del canal...
		}


	// Inicializamos la matriz... Aqui entramos la primera vez que asignamos un UVW
	// Aqui entra la primera vez que aplicamos el modifier...
	if (flags&CONTROL_INIT) {
		SuspendAnimate();
		AnimateOff();		
		// Rotate the seem to the back
		SetXFormPacket pckt(Quat(RotateZMatrix(PI)),TRUE);
		tmControl->SetValue(0,&pckt,TRUE,CTRL_RELATIVE);
		ResumeAnimate();
		}


	if (!(flags&CONTROL_OP)) {
		flags = 0;
		return;
		}

	if (flags&CONTROL_HOLD) theHold.Begin();	

	Matrix3 oldtm = tm = Inverse(CompMatrix(t,&mc,NULL,FALSE));	
	AffineParts parts;
	decomp_affine(tm, &parts);
	parts.q.MakeMatrix(tm);
	tm.Translate(parts.t);
	

#if MAX_VERSION_MAJOR < 24
	Matrix3 mctm(1);
#else
	Matrix3 mctm;
#endif

	if (mc.tm) mctm = *mc.tm;
	tm.Scale(Point3(
		Length(mctm.GetRow(0)),
		Length(mctm.GetRow(1)),
		Length(mctm.GetRow(2))
		));

	// Hacemos un Point3 con el ancho, largo y alto del gizmo resp.
	Point3 s;
	s.x = gizmo_width;
	s.y = gizmo_length;
	s.z = gizmo_height;	
	// Ajustamos el tamaño del box al tamaño del bounding box del objeto...
	// Este TRUE es sujeto a modificacion para que se pueda aplicar a todo el objeto...
	obj->GetDeformBBox(t,box,&tm,TRUE);
	box.Scale(1.001f);  // prevent wrap-around on sides of boxes

	if (flags&CONTROL_ASPECT &&
		(type==MAP_TL_PLANAR || type==MAP_TL_CYLINDRICAL || type==MAP_TL_CYLCAP)) {
		if (type==MAP_TL_PLANAR) {			
			float w = aspect*s.y;
			s.x *= w / s.x;			
			s.z *= s.y / s.z;				
		} else
		if (type==MAP_TL_CYLINDRICAL || type == MAP_TL_CYLCAP) {
			float w = (s.x+s.y)*PI;			
			s.z *= w/(aspect*s.z);
			}
		}

	if (flags&CONTROL_UNIFORM) {
		float av = (s.x + s.y + s.z)/3.0f;
	//	s.x = s.y = s.z = av;
		}


	// Aqui entra la primera vez que aplicamos el modifier para que el
	// gizmo se ajuste al tamaño del objeto...
	if (flags&CONTROL_FIT) {
		// w es el ancho del box...
		Point3 w = box.Width();
		if (box.IsEmpty()) w = Point3(10.0f,10.0f,10.0f);

		if (type==MAP_TL_PLANAR) {
			s.x = w.x==0.0f ? 1.0f : w.x;
			s.y = w.y==0.0f ? 1.0f : w.y;
			s.z = w.z==0.0f ? 1.0f : w.z;
		} else
		if (type==MAP_TL_SPHERICAL || type==MAP_TL_BALL) {
			float max = w.x;
			if (w.y>max) max = w.y;
			if (w.z>max) max = w.z;
			if (max==0.0f) max = 1.0f;
			s.x = s.y = s.z = max;
		} else {
			if (w.x>w.y) s.x = s.y = w.x;
			else s.x = s.y = w.y;
			s.z = w.z;
			if (s.x==0.0f) s.x = 1.0f;
			if (s.y==0.0f) s.y = 1.0f;
			if (s.z==0.0f) s.z = 1.0f;			
			}
		}


	// Aqui entra la primera vez que aplicamos el modifier
	if (flags&(CONTROL_CENTER|CONTROL_FIT)) {		

		// Creamos un nuevo box...
		Box3 sbox;		
		obj->GetDeformBBox(t,sbox,&oldtm,TRUE);
		
		// Encontramos el centro del objeto
		Point3 cent = sbox.Center();
		cent = VectorTransform(Inverse(oldtm),cent);
		cent = VectorTransform(mctm,cent);

		SetXFormPacket pckt(
			cent, //sbox.Center(),

#if MAX_VERSION_MAJOR < 24
			Matrix3(1),
			Matrix3(1)
#else
			Matrix3(),
			Matrix3()
#endif
		);
			//Inverse(oldtm));
		
		SuspendAnimate();
		AnimateOff();
		tmControl->SetValue(0,&pckt,TRUE,CTRL_RELATIVE);
		ResumeAnimate();
		}


	if (flags&(CONTROL_FIT|CONTROL_ASPECT|CONTROL_INITPARAMS|CONTROL_UNIFORM)) {
		SuspendAnimate();
		AnimateOff();

		// Clear out any scale in the transform

#if MAX_VERSION_MAJOR < 24
		tm = Matrix3(1);	
#else
		tm = Matrix3();
#endif

		tmControl->GetValue(t,&tm,FOREVER,CTRL_RELATIVE);	
		decomp_affine(tm, &parts);
		parts.q.MakeMatrix(tm);
		tm.Translate(parts.t);	
		SetXFormPacket pckt(tm);		
		tmControl->SetValue(0,&pckt,TRUE,CTRL_ABSOLUTE);
		// Set the new dimensions
		SetLength(t,s.y);
		SetWidth(t,s.x);
		SetHeight(t,s.z);

		ResumeAnimate();
		}	

	if (flags&CONTROL_HOLD) theHold.Accept(0);
	flags = 0;

	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	}


void UVWProyector::PeltBuildUpUVInfo(TimeValue t, MNMesh &mnMesh, BitArray fs, BitArray es ) 
{
	float frame_size = GetFrameSize(t);

	MNMesh aux_mnmesh(mnMesh);

	int i_v, i_f, i_e;

	MNVert *v = aux_mnmesh.v;
	MNFace *f = aux_mnmesh.f;
	MNEdge *e = aux_mnmesh.e;
	int num_faces = aux_mnmesh.numf;
	int num_verts = aux_mnmesh.numv;
	int num_edges = aux_mnmesh.nume;

	if ( current_num_faces==num_faces && current_num_verts==num_verts && current_num_edges==num_edges && current_num_sel_faces==fs.NumberSet() && current_num_sel_edges==es.NumberSet() )
		return;

	// Delete all old stuff
	PeltDeleteUV();

	current_num_faces = num_faces;
	current_num_verts = num_verts;
	current_num_edges = num_edges;
	current_num_sel_faces = fs.NumberSet();
	current_num_sel_edges = es.NumberSet();

	/* This method didn't work very well... Benjamin Slack reported a bug
	Tab <IPoint2> edges_by_faces;
	edges_by_faces.SetCount(0);
	edges_by_faces.Resize( num_edges );
	for ( i_e=0; i_e<num_edges; i_e++ ) {
		if ( es[i_e] ) {
			IPoint2 e_f( e[i_e].f1, e[i_e].f2 );
			edges_by_faces.Append(1,&e_f);
			e[i_e].SetFlag( MN_SEL );
			}
		else
			e[i_e].ClearFlag( MN_SEL );
		}*/

	std::vector <IPoint2> edges_by_face_and_side;
	edges_by_face_and_side.reserve( num_edges );
	for ( i_e=0; i_e<num_edges; i_e++ ) {
		if ( es[i_e] ) {
			int i_f = e[i_e].f1;
			int deg = f[i_f].deg;
			int the_edge = -1;
			for ( int i_v=0; i_v<deg; i_v++ ) {
				if ( f[i_f].edg[i_v]==i_e ) {
					the_edge = i_v;
					}
				}
			if ( the_edge!=-1 ) {
				IPoint2 edge_info(i_f,the_edge);
				edges_by_face_and_side.push_back( edge_info );
				}
			}
		}

	for ( i_f=0; i_f<num_faces; i_f++ ) {
		if ( fs[i_f] ) 
			f[i_f].SetFlag( MN_SEL );
		else
			f[i_f].ClearFlag( MN_SEL );
		}

	aux_mnmesh.DetachFaces();

	aux_mnmesh.InvalidateTopoCache();
	aux_mnmesh.InvalidateGeomCache();
	aux_mnmesh.FillInMesh();
	aux_mnmesh.PrepForPipeline();

	e = aux_mnmesh.e;
	v = aux_mnmesh.v;
	f = aux_mnmesh.f;
	num_faces = aux_mnmesh.numf;
	num_verts = aux_mnmesh.numv;
	num_edges = aux_mnmesh.nume;

	int restored_edges = 0;

	// Restore the selected edges
	/* This method didn't work very well... Benjamin Slack reported a bug
	   and diego fixed it on Oct 8, 2004
	for ( i_e=0; i_e<num_edges; i_e++ ) {
		IPoint2 e_f( e[i_e].f1, e[i_e].f2 );
		bool in_table = false;
		for ( int i_t=0; i_t<edges_by_faces.Count(); i_t++ ) {
			if ( e_f == edges_by_faces[i_t] ) {
				in_table = true;
				edges_by_faces.Delete(i_t,1);
				break;
				}
			}

		if ( in_table ) {
			restored_edges++;
			e[i_e].SetFlag(MN_SEL);
			}
		else
			e[i_e].ClearFlag(MN_SEL);
		}*/

	for ( i_e=0; i_e<num_edges; i_e++ ) {
		e[i_e].ClearFlag(MN_SEL);
		}

	for ( i_e=0; i_e<edges_by_face_and_side.size(); i_e++ ) {
		int the_face = edges_by_face_and_side[i_e].x;
		int the_edge = edges_by_face_and_side[i_e].y;

		e[ f[the_face].edg[the_edge] ].SetFlag(MN_SEL);
		}

	aux_mnmesh.SplitFlaggedEdges(MN_SEL);

	aux_mnmesh.InvalidateTopoCache();
	aux_mnmesh.InvalidateGeomCache();
	aux_mnmesh.FillInMesh();
	aux_mnmesh.PrepForPipeline();

	e = aux_mnmesh.e;
	v = aux_mnmesh.v;
	f = aux_mnmesh.f;
	num_faces = aux_mnmesh.numf;
	num_verts = aux_mnmesh.numv;
	num_edges = aux_mnmesh.nume;

	// Store the mnmesh info
	pelt_poly_mesh = new PeltPolyMesh( num_verts, num_faces );
	for ( i_f=0; i_f<num_faces; i_f++ ) {
		pelt_poly_mesh->f[i_f].deg = f[i_f].deg;
		pelt_poly_mesh->f[i_f].vtx = new int[f[i_f].deg];
		for ( i_v=0; i_v<f[i_f].deg; i_v++ ) {
			pelt_poly_mesh->f[i_f].vtx[i_v] = f[i_f].vtx[i_v];
			}
		}
	for ( i_v=0; i_v<num_verts; i_v++ ) {
		pelt_poly_mesh->v[i_v] = v[i_v].p;
		}

	MNMeshBorder border;
	aux_mnmesh.GetBorder( border, MNM_SL_FACE );

	int good_border = -1;
	int max_num_sel_edges = 0;
	// JW Code Change : fix uninitialized locals warning
	int num_border_verts=0;

	for ( int i_b=0; i_b<border.Num(); i_b++ ) {
		int num_edges = border.Loop(i_b)->Count();

		int edge_id = (*border.Loop(i_b))[0];
		if ( f[e[edge_id].f1].GetFlag(MN_SEL) ) {
			int num_sel_edges = 0;
			for ( int i_e=0; i_e<num_edges; i_e++ ) {
				int edge_id = (*border.Loop(i_b))[i_e];
				if ( e[edge_id].GetFlag(MN_SEL) )
					num_sel_edges++;
				}
			if ( num_sel_edges > max_num_sel_edges || good_border==-1 ) {
				good_border = i_b;
				num_border_verts = num_edges;
				max_num_sel_edges = num_sel_edges;
				}
			}
		}

	if ( good_border==-1 )
		return;

	BOOL delete_frame_segments = FALSE;
	if ( border_verts.Count() == num_border_verts ) {
		int first_edge_id = (*border.Loop(good_border))[0];
		int start_bv = -1;
		for ( int i_bv=0; i_bv<num_border_verts; i_bv++ ) {
			if ( border_verts[i_bv] == e[first_edge_id].v2 ) {
				start_bv = i_bv;
				}
			}
		if ( start_bv != -1 ) {
			for ( int i_bv=0; i_bv<num_border_verts; i_bv++ ) {
				int j_bv = ( start_bv + i_bv ) % num_border_verts;
				int edge_id = (*border.Loop(good_border))[i_bv];
				if ( border_verts[j_bv] != e[edge_id].v2 ) {
					delete_frame_segments = TRUE;
					break;
					}
				}
			}
		else {
			delete_frame_segments = TRUE;
			}
		}
	else {
		delete_frame_segments = TRUE;
		}

	// Start Building the Frame Segments
	if ( delete_frame_segments ) {
		if (theHold.Holding())
			theHold.Put( new PeltFrameRestore(this,tl) );
		PeltDeleteFrameSegments();
		}

	if ( border_verts.Count() == 0 ) {
		border_verts.SetCount( num_border_verts );
		Tab <Point3> border_points;
		border_points.SetCount( num_border_verts );

		for ( int i_bv=0; i_bv<num_border_verts; i_bv++ ) {
			int edge_id = (*border.Loop(good_border))[i_bv];
			border_verts[i_bv] = e[edge_id].v2;
			border_points[i_bv] = v[border_verts[i_bv]].p;
			}

		Box3 natural_box;
		GetNaturalBoundingBox( border_points, natural_axis, natural_box );
		inv_natural_axis = Inverse(natural_axis);

		float max_dist = 0.5f * natural_box.Width().x;

		float best_border_eval = 0.0f;
		int best_border_vert = 0;
		for ( int i_bv=0; i_bv<num_border_verts; i_bv++ ) {
			int j_bv = (i_bv+num_border_verts-1)%num_border_verts;
			int last_edge_id = (*border.Loop(good_border))[j_bv];
			int this_edge_id = (*border.Loop(good_border))[i_bv];

			int last_face = e[last_edge_id].f1;
			int this_face = e[this_edge_id].f1;

			Point3 border_point = inv_natural_axis * v[ border_verts[i_bv] ].p;
			float dist = 1.0f - fabs( border_point.x ) / max_dist;

			Point3 last_edge_vec = Normalize( v[e[last_edge_id].v2].p - v[e[last_edge_id].v1].p );
			Point3 this_edge_vec = Normalize( v[e[this_edge_id].v2].p - v[e[this_edge_id].v1].p );
			Point3 edge_tangent = Normalize( last_edge_vec + this_edge_vec );
			edge_tangent = inv_natural_axis.VectorTransform( edge_tangent );

			float dp = -edge_tangent.x;

			if ( dp<0.0f )
				dp = 0.95f * dp;
			dp = fabs(dp);

			float eval = dist * dp;
			if ( eval > best_border_eval ) {
				best_border_eval = eval;
				best_border_vert = i_bv;
				}
			}

		int nail_frame_segment = -1;

		// Rotate our border verts starting from our best_border_vert
		// Build up the frame segments
		Tab <int> new_border_verts;
		Tab <int> tab_int_frame_segments;
		new_border_verts.SetCount(num_border_verts);
		tab_int_frame_segments.Resize(num_border_verts);

		int last_frame_segment_id = 0;
		for ( int i_bv=0; i_bv<num_border_verts; i_bv++ ) {
			int old_i_bv = (best_border_vert+i_bv) % num_border_verts;
			new_border_verts[i_bv] = border_verts[old_i_bv];

			int old_j_bv = (old_i_bv+num_border_verts-1)%num_border_verts;
			int last_edge_id = (*border.Loop(good_border))[old_j_bv];
			int this_edge_id = (*border.Loop(good_border))[old_i_bv];

			int last_face_id = e[last_edge_id].f1;
			int this_face_id = e[this_edge_id].f1;

			if ( last_face_id == this_face_id ) {
				tab_int_frame_segments.Append(1,&i_bv);
				}
			}
		border_verts = new_border_verts;
		tab_int_frame_segments.Shrink();

		if ( tab_int_frame_segments.Count() <3 ) {
			tab_int_frame_segments.SetCount(num_border_verts);
			for ( int i_bv=0; i_bv<num_border_verts; i_bv++ ) {
				tab_int_frame_segments[i_bv] = i_bv;
				}
			}

		Tab <int> border_verts_perimeter;
		border_verts_perimeter.SetCount( num_border_verts );

		float perimeter = 0.0f;
		Box3 border_box;
		for ( int i_bv=0; i_bv<num_border_verts; i_bv++ ) {
			int j_bv = (i_bv+1)%num_border_verts;

			Point3 i_vert = v[border_verts[i_bv]].p;
			Point3 j_vert = v[border_verts[j_bv]].p;

			float edge_length = Length( i_vert - j_vert );
		
			border_verts_perimeter[i_bv] = perimeter;
			perimeter += edge_length;

			border_box += i_vert;
			}
		Point3 center = border_box.Center();
		float radius = 0.5f * Length( border_box.Width() );

		Point3 the_x_axis(1,0,0); // = Normalize ( the_y_axis ^ the_z_axis );
		Point3 the_y_axis(0,1,0); // = Normalize( v[border_verts[0]].p - center );
		Point3 the_z_axis(0,0,1); // = Point3(0,0,1);

		int num_frame_segments = tab_int_frame_segments.Count();
		if ( tab_int_frame_segments[0] != 0 ) {
			Tab <int> new_tab_int_frame_segments;
			new_tab_int_frame_segments.SetCount(num_frame_segments);
			for ( int i_fs=0; i_fs<num_frame_segments; i_fs++ ) {
				int j_fs = (i_fs+num_frame_segments+1)%num_frame_segments;
				new_tab_int_frame_segments[j_fs] = tab_int_frame_segments[i_fs];
				}
			tab_int_frame_segments = new_tab_int_frame_segments;
			}

		frame_segments.SetCount( num_frame_segments );

		for ( int i_fs=0; i_fs<num_frame_segments; i_fs++ ) {
			int j_fs = (i_fs+1)%num_frame_segments;

			int i_fs_bv = tab_int_frame_segments[i_fs];
			int j_fs_bv = tab_int_frame_segments[j_fs];
			int num = j_fs_bv - i_fs_bv;
			if ( j_fs_bv<i_fs_bv )
				num = (num_border_verts-i_fs_bv) + j_fs_bv;

			frame_segments[i_fs] = new FrameSegments(num);
			float sum_perimeter = 0.0f;
			for ( int i=0; i<num; i++ ) {
				
				int this_bv = (i_fs_bv+i)%num_border_verts;
				int next_bv = (i_fs_bv+i+1)%num_border_verts;

				if ( 0 == this_bv ) 
					nail_frame_segment = i;
				float len = Length( v[border_verts[this_bv]].p - v[border_verts[next_bv]].p );

				frame_segments[i_fs]->border_vert[i] = this_bv;
				frame_segments[i_fs]->position[i] = sum_perimeter;
			
				sum_perimeter = sum_perimeter + len;
				}
			for ( int i=0; i<num; i++ ) {
				frame_segments[i_fs]->position[i] = frame_segments[i_fs]->position[i]/sum_perimeter;
				}
			frame_segments[i_fs]->segment_length = sum_perimeter/perimeter;
			}

		float back_rot_ang = 0.0f;
		if ( nail_frame_segment != 0 ) {
			back_rot_ang = 2.0f * PI * frame_segments[0]->segment_length * frame_segments[0]->position[nail_frame_segment];
			}
		float sum_segment_length = 0.0;
		for ( int i_fs=0; i_fs<num_frame_segments; i_fs++ ) {		
			float ang = 2.0f * PI * sum_segment_length - back_rot_ang;
			frame_segments[i_fs]->start_point = frame_size * radius * ( the_y_axis * float(cos(ang)) + the_x_axis * float(sin(ang)) );
			sum_segment_length = sum_segment_length + frame_segments[i_fs]->segment_length;
			}
	
		PeltBuildUpSymmetryTables(SYMMETRY_BEST);
		}
	// Stop Building the Frame Segments

	vert_to_mass.SetCount(num_verts);

	BitArray verts_as_masses(num_verts);
	verts_as_masses.ClearAll();
	for ( i_f=0; i_f<num_faces; i_f++ ) {
		if ( f[i_f].GetFlag(MN_SEL) ) {
			int deg = f[i_f].deg;
			for ( i_v=0; i_v<deg; i_v++ ) {
				verts_as_masses.Set(f[i_f].vtx[i_v]);
				}
			}
		}

	int num_masses = verts_as_masses.NumberSet();

	masses.SetCount( num_masses );

	int m_id = 0;
	for ( i_v=0; i_v<num_verts; i_v++ ) {
		if ( verts_as_masses[i_v] ) {
			vert_to_mass[i_v] = m_id;

			masses[m_id] = new Mass;
			masses[m_id]->vert_id = i_v;
			masses[m_id]->o_pos = inv_natural_axis * v[i_v].p;
			masses[m_id]->f_pos = inv_natural_axis * v[i_v].p;
			masses[m_id]->vel = Point3(0,0,0);
			m_id++;
			}
		else {
			vert_to_mass[i_v] = -1;
			}
		}

	int num_springs = 0;
	for ( i_e=0; i_e<num_edges; i_e++ ) {
		if ( f[e[i_e].f1].GetFlag(MN_SEL) ) {
			int v1 = e[i_e].v1;
			int v2 = e[i_e].v2;
			
			int m1 = vert_to_mass[v1];
			int m2 = vert_to_mass[v2];

			float len = Length( masses[m1]->o_pos - masses[m2]->o_pos );

			Spring spring;
			spring.attached_to_mass = true;
			spring.attach_id = m2;
			spring.o_lenght = len;
			masses[m1]->springs.Append(1,&spring,0);

			spring.attached_to_mass = true;
			spring.attach_id = m1;
			spring.o_lenght = len;
			masses[m2]->springs.Append(1,&spring,0);
			}
		}

	for ( int i_bv=0; i_bv<border_verts.Count(); i_bv++ ) {
		int h_bv = ( i_bv + border_verts.Count() - 1 ) % border_verts.Count();
		int j_bv = ( i_bv + 1 ) % border_verts.Count();

		int v0 = border_verts[h_bv];
		int v1 = border_verts[i_bv];
		int v2 = border_verts[j_bv];

		int m0 = vert_to_mass[v0];
		int m1 = vert_to_mass[v1];
		int m2 = vert_to_mass[v2];

		float len = 0.5f * ( Length(masses[m1]->o_pos-masses[m0]->o_pos) + Length(masses[m2]->o_pos-masses[m1]->o_pos) );

		Spring spring;
		spring.attached_to_mass = false;
		spring.attach_id = i_bv;
		spring.o_lenght = len;
		masses[m1]->springs.Append(1,&spring,0);
		}
	}

// PeltBuildUpSymmetryTables...
// This builds up a table of IPoints, the pair is symmetric either in the
// X or Y axis, defined by symmetry_axis, relative to a center.
// Use Symmetry has 4 options, X, Y, find the best and Keep current
// If create_axis is true, the 2 selected points are used as a symmetry axis.
void UVWProyector::PeltBuildUpSymmetryTables( int use_symmetry, BOOL create_axis ) {

	// Let's find a center for the current frame segments
	int num_frame_segments = frame_segments.Count();
	int num_sel = 0;
	Point3 sel_point;
	Point3 sel_center(0,0,0);
	for ( int i_fs=0; i_fs<num_frame_segments; i_fs++ )
		if ( frame_segments[i_fs]->selected ) {
			sel_center += frame_segments[i_fs]->start_point;
			num_sel++;
			sel_point = frame_segments[i_fs]->start_point;
			}

	if ( num_sel!=2 )
		create_axis = FALSE;

	Box3 border_box;
	for ( int i_fs=0; i_fs<num_frame_segments; i_fs++ ) {
		Point3 i_fs_pt = frame_segments[i_fs]->start_point;
		border_box += i_fs_pt;
		}

	symmetry_center = border_box.Center();

	if ( create_axis ) {
		symmetry_center = center;
		Point3 align_axis(0,1,0);
		if ( use_symmetry==SYMMETRY_Y )
			align_axis = Point3(1,0,0);
		Point3 sel_axis = Normalize(sel_point - sel_center);
		float ang = acos( DotProd(sel_axis,align_axis) );
		float dir = (align_axis^sel_axis).z;
		if ( dir<0 )
			ang = -ang;
		PeltStepRotatePoints(-ang);
		}

	// A value to create the match... just to see how alike they are...
	float sym_value_x = border_box.Width().x / 100.0f;
	float sym_value_y = border_box.Width().y / 100.0f;

	Tab <IPoint2> sym_x;
	Tab <IPoint2> sym_y;
	sym_x.Resize( num_frame_segments );
	sym_y.Resize( num_frame_segments );

	for ( int i_fs=0; i_fs<num_frame_segments; i_fs++ ) {
		// The symmetry is identified with the points sym_pos_x and sym_pos_y, which
		// are like the mirrors for the i_fs_pt.
		Point3 i_fs_pt = frame_segments[i_fs]->start_point;
		float dir_x = i_fs_pt.x - symmetry_center.x;
		float dir_y = i_fs_pt.y - symmetry_center.y;
		Point3 sym_pos_x = i_fs_pt;
		Point3 sym_pos_y = i_fs_pt;
		sym_pos_x.x = symmetry_center.x - dir_x;
		sym_pos_y.y = symmetry_center.y - dir_y;
		for ( int j_fs=i_fs+1; j_fs<num_frame_segments; j_fs++ ) {
			Point3 j_fs_pt = frame_segments[j_fs]->start_point;
			// Is it close to the mirror???
			if ( Length(j_fs_pt-sym_pos_x)<sym_value_x ) {
				int k_fs = j_fs-1;
				int i_num_steps = frame_segments[i_fs]->border_vert.Count();
				int k_num_steps = frame_segments[k_fs]->border_vert.Count();
				// Does it have the same number of border_verts?
				if ( i_num_steps == k_num_steps ) {
					// This is a GOOD symmetry!!!
					IPoint2 sym(i_fs,j_fs);
					sym_x.Append(1,&sym,0);
					}
				}
			if ( Length(j_fs_pt-sym_pos_y)<sym_value_y ) {
				int k_fs = j_fs-1;
				int i_num_steps = frame_segments[i_fs]->border_vert.Count();
				int k_num_steps = frame_segments[k_fs]->border_vert.Count();
				if ( i_num_steps == k_num_steps ) {
					IPoint2 sym(i_fs,j_fs);
					sym_y.Append(1,&sym,0);
					}
				}
			}
		}

	sym_x.Shrink();
	sym_y.Shrink();

	switch( use_symmetry ) {
		case SYMMETRY_X:
			frame_segments_symmetry = sym_x;
			symmetry_axis = 0;
			break;
		case SYMMETRY_Y:
			frame_segments_symmetry = sym_y;
			symmetry_axis = 1;
			break;
		case SYMMETRY_BEST:
			if ( sym_x.Count() >= sym_y.Count() ) {
				frame_segments_symmetry = sym_x;
				symmetry_axis = 0;
				}
			else {
				frame_segments_symmetry = sym_y;
				symmetry_axis = 1;
				}
			break;
		case SYMMETRY_KEEP:
			if ( symmetry_axis==0 ) {
				frame_segments_symmetry = sym_x;
				}
			if ( symmetry_axis==1 ) {
				frame_segments_symmetry = sym_y;
				}
			break;
		}

//	if ( frame_segments_symmetry.Count()==0 ) {
//		symmetry_axis=-1;
//		}
	
	PeltFixSymmetricSegments();
	}

// PeltFixSymmetricSegments
// After succesive Add and Delete point operations, the symmetric segments can have a 
// completely different internal spacing. This method fixes this by mixing the two
// spacings.
void UVWProyector::PeltFixSymmetricSegments() {
	// Let's make the internal segments that are between a symmetric pair
	// completely symmetric.
	if ( symmetry_axis==-1 )
		return;

	int num_frame_segments = frame_segments.Count();
	for ( int i_sp=0; i_sp<frame_segments_symmetry.Count(); i_sp++ ) {
		// Get bot Frame Segments
		int i_fs = frame_segments_symmetry[i_sp].x;
		int j_fs = (num_frame_segments+frame_segments_symmetry[i_sp].y-1)%num_frame_segments;

		// Make sure they are the same size
		if ( frame_segments[i_fs]->position.Count() == frame_segments[j_fs]->position.Count() ) {
			int num_sub_segments = frame_segments[i_fs]->position.Count();
			Tab <float> new_deltas;
			new_deltas.SetCount(num_sub_segments);
			float nd_sum = 0.0f;
			for ( int i_fss=0; i_fss<num_sub_segments; i_fss++ ) {
				int j_fss = num_sub_segments - i_fss - 1;

				// Spacing from first segment
				float pos_i0, pos_i1;
				pos_i0 = frame_segments[i_fs]->position[i_fss];
				if ( i_fss+1 == num_sub_segments )
					pos_i1 = 1.0f;
				else
					pos_i1 = frame_segments[i_fs]->position[i_fss+1];

				// Spacing from its pair
				float pos_j0, pos_j1;
				if ( j_fss+1==num_sub_segments ) 
					pos_j1 = 1.0f;
				else
					pos_j1 = frame_segments[j_fs]->position[j_fss+1];
				pos_j0 = frame_segments[j_fs]->position[j_fss];

				new_deltas[i_fss] = (pos_i1-pos_i0 + pos_j1-pos_j0) / 2.0f;
				nd_sum = nd_sum + new_deltas[i_fss];
				}

			float i_new_position = 0.0f;
			float j_new_position = 0.0f;
			for ( int i_fss=0; i_fss<num_sub_segments; i_fss++ ) {
				int j_fss = num_sub_segments - i_fss - 1;
				frame_segments[i_fs]->position[i_fss] = i_new_position;
				frame_segments[j_fs]->position[i_fss] = j_new_position;
				i_new_position = i_new_position + new_deltas[i_fss];
				j_new_position = j_new_position + new_deltas[j_fss];
				}
			}
		}
	}

void UVWProyector::PeltApplyUV( TimeValue t, MNMesh * aux_mesh, PolyUVWData * uvw_data ) {
	float k = 1.0f / GetPeltRigidity(t);
	float b = GetPeltStability(t);
	int iter = GetPeltIter();

	if ( aux_mesh->numf != pelt_poly_mesh->numf ) {
		// Borrar el uvw_data???
		return;
		}

	int num_bv = 0;
	for ( int i_fs=0; i_fs<frame_segments.Count(); i_fs++ )
		num_bv += frame_segments[i_fs]->border_vert.Count();

	if ( border_verts.Count() != num_bv )
		return;

	int num_border_verts = border_verts.Count();
	int num_frame_segments = frame_segments.Count();
	float global_scale = 0.033f;

	int i_m,i_v,i_f;
	Tab <Point3> frame_points;
	frame_points.SetCount( num_border_verts );

	for ( int i_fs=0; i_fs<frame_segments.Count(); i_fs++ ) {
		int j_fs = (i_fs+1)%num_frame_segments;
		Point3 i_point = frame_segments[i_fs]->start_point;
		Point3 j_point = frame_segments[j_fs]->start_point;
		Point3 segment_vec = j_point - i_point;

		for ( int i=0; i<frame_segments[i_fs]->position.Count(); i++ ) {
			int i_bv = frame_segments[i_fs]->border_vert[i];
			float pos = frame_segments[i_fs]->position[i];
			frame_points[i_bv] = i_point + pos * segment_vec;
			}
		}

	for ( int i_m=0; i_m<masses.Count(); i_m++ ) {
		int vert_id = masses[i_m]->vert_id;
		Point3 v = pelt_poly_mesh->v[vert_id];
		masses[i_m]->o_pos = inv_natural_axis * v;
		masses[i_m]->f_pos = inv_natural_axis * v;
		masses[i_m]->vel = Point3(0,0,0);
		}

	if ( pelt_border ) {
		for ( int i_bv=0; i_bv<num_border_verts; i_bv++ ) {
			int mass_id = vert_to_mass[ border_verts[i_bv] ];
			masses[mass_id]->f_pos = frame_points[i_bv];
			}
		}

	for ( int it=0; it<iter; it++ ) {
		for ( i_m=0; i_m<masses.Count(); i_m++ )
			masses[i_m]->o_pos = masses[i_m]->f_pos;
		for ( i_m=0; i_m<masses.Count(); i_m++ ) {
			Point3 sum_f(0,0,0);
			Point3 mass_pos = masses[i_m]->o_pos;
			for ( int i_s=0; i_s<masses[i_m]->springs.Count(); i_s++ ) {
				Point3 attach_pos(0,0,0);
				if ( masses[i_m]->springs[i_s].attached_to_mass ) {
					attach_pos = masses[ masses[i_m]->springs[i_s].attach_id ]->o_pos;
					}
				else {
					attach_pos = frame_points[ masses[i_m]->springs[i_s].attach_id ];
					}
				Point3 s_dir = attach_pos - mass_pos;
				float s_len = Length( s_dir );
				if ( s_len!=0.0f )
					s_dir = s_dir / s_len;

				float o_s_len = masses[i_m]->springs[i_s].o_lenght;

				sum_f = sum_f + s_dir * k * ( s_len - o_s_len );
				}

			Point3 force = sum_f - b * masses[i_m]->vel;

			masses[i_m]->vel = masses[i_m]->vel + force * global_scale;
			masses[i_m]->f_pos = mass_pos + masses[i_m]->vel * global_scale;
			}
		if ( pelt_border ) {
			for ( int i_bv=0; i_bv<num_border_verts; i_bv++ ) {
				int mass_id = vert_to_mass[ border_verts[i_bv] ];
				masses[mass_id]->f_pos = frame_points[i_bv];
				}
			}
		}

	Box3 uvw_box;
	uvw_box.Init();
	for ( i_m=0; i_m<masses.Count(); i_m++ ) {
		uvw_box += masses[i_m]->f_pos;
		}

	Point3 uvw_w = uvw_box.Width();
	float pelt_aspect = uvw_w.x / uvw_w.y;
	// If the uvw box is larger than taller		-
	if ( pelt_aspect>1.0f ) 
		uvw_w.y = uvw_w.y * pelt_aspect;
	if ( pelt_aspect<1.0f )
		uvw_w.x = uvw_w.x / pelt_aspect;

	MNMap *mac = aux_mesh->M(1);
	
	mac->setNumVerts( pelt_poly_mesh->numv );
	mac->setNumFaces( pelt_poly_mesh->numf );
	for ( i_v=0; i_v<pelt_poly_mesh->numv; i_v++ ) {
		mac->v[i_v] = Point3(0,0,0);
		}
	for ( i_f=0; i_f<pelt_poly_mesh->numf; i_f++ ) {
		int mesh_deg = aux_mesh->f[i_f].deg;
		int pelt_deg = pelt_poly_mesh->f[i_f].deg;
		if ( mesh_deg != pelt_deg ) {
			//MessageBox(GetCOREInterface()->GetMAXHWnd(),"Different DEG","REMOVE",MB_OK);
			return;
			}
		mac->f[i_f].SetSize( pelt_deg );
		for ( i_v=0; i_v<pelt_deg; i_v++ )
			mac->f[i_f].tv[i_v] = pelt_poly_mesh->f[i_f].vtx[i_v];
		}

	float umove = GetMoveU(t);
	float vmove = GetMoveV(t);
	float wmove = GetMoveW(t);

	float urotate = GetRotateU(t);
	float vrotate = GetRotateV(t);
	float wrotate = GetRotateW(t);

	float uscale = GetScaleU(t);
	float vscale = GetScaleV(t);
	float wscale = GetScaleW(t);

	float w_x = uvw_box.Width().x / uvw_w.x;	// 0.0 <-> 1.0
	float w_y = uvw_box.Width().y / uvw_w.y;	// 0.0 <-> 1.0

	for ( i_m=0; i_m<masses.Count(); i_m++ ) {
		int vert_id = masses[i_m]->vert_id;
		
		Point3 uv = masses[i_m]->f_pos;
		uv.x = ( uv.x - uvw_box.pmin.x ) / ( uvw_w.x );
		uv.y = ( uv.y - uvw_box.pmin.y ) / ( uvw_w.y );
		uv.z = 0.0f;

		uv.x = 0.5f - 0.5f*w_x + uv.x;
		uv.y = 0.5f - 0.5f*w_y + uv.y;

		XForm(uv,umove,vmove,wmove,urotate,vrotate,wrotate,uscale,vscale,wscale);
		
		mac->v[vert_id] = uv;
		}

	uvw_data->DeleteUVWData();
	uvw_data->num_v = mac->numv;
	uvw_data->num_f = mac->numf;
	uvw_data->v = new Point3[uvw_data->num_v];
	uvw_data->f = new PolyFace[uvw_data->num_f];

	for ( i_v=0; i_v<uvw_data->num_v; i_v++ ) {
		uvw_data->v[i_v] = mac->v[i_v];
		}

	for ( i_f=0; i_f<uvw_data->num_f; i_f++ ) {
		int deg = mac->f[i_f].deg;
		uvw_data->f[i_f].deg = deg;
		uvw_data->f[i_f].vtx = new int[deg];
		for ( int i_v=0; i_v<deg; i_v++ )
			uvw_data->f[i_f].vtx[i_v] = mac->f[i_f].tv[i_v];
		}

	uvw_data->valid_group_key = valid_group;
}

BOOL UVWProyector::PeltHitTest(Box2D box, Tab<int> &hits, BOOL selOnly) {
	for ( int i=0; i<frame_segments.Count(); i++) {

		Point2 pt = frame_segments[i]->start_point;

		if ( selOnly && !frame_segments[i]->selected ) continue;

		if ( PointInBox( pt, box ) ) {
			hits.Append(1,&i,10);
			}
		}

	return hits.Count();
	}
		
void UVWProyector::PeltSelect(Tab<int> &hits, BOOL toggle, BOOL subtract, BOOL all) {
	for (int i=0; i<hits.Count(); i++) {
		FrameSegments * fs = frame_segments[ hits[i] ];
		if ( toggle && hits[i] ) {
			fs->selected = !fs->selected;
			}
		else if (subtract) {
			fs->selected = false;
			}
		else {
			fs->selected = true;
			}
		if (!all) 
			break;
		}
	}

void UVWProyector::PeltHoldPoints()	{
	if (theHold.Holding() && !TestAFlag(A_HELD)) {

		SetAFlag(A_HELD);

		theHold.Put(new PeltPointsPosRestore(this,tl));
		}
	}

void UVWProyector::PeltMovePoints( Point2 delta, int move_axis ) {
	PeltHoldPoints();

	if ( move_axis == 1 )
		delta.y = 0.0f;
	if ( move_axis == 2 )
		delta.x = 0.0f;

	Point3 p3_delta(delta);

	BitArray already_moved(frame_segments.Count());
	already_moved.ClearAll();

	for ( int i=0; i<frame_segments.Count(); i++ ) {
		if ( !already_moved[i] && frame_segments[i]->selected ) {
			int symmetry_pair = -1;
			if ( symmetry_axis!=-1 ) {
				for ( int i_sp=0; i_sp<frame_segments_symmetry.Count(); i_sp++ ) {
					if ( i==frame_segments_symmetry[i_sp].x )
						symmetry_pair = frame_segments_symmetry[i_sp].y;
					else if ( i==frame_segments_symmetry[i_sp].y )
						symmetry_pair = frame_segments_symmetry[i_sp].x;
					}
				}
			frame_segments[i]->start_point = frame_segments[i]->start_point + p3_delta;
			already_moved.Set(i);
			if ( symmetry_pair!=-1 ) {
				float dir = frame_segments[i]->start_point[symmetry_axis] - symmetry_center[symmetry_axis];
				frame_segments[symmetry_pair]->start_point = frame_segments[i]->start_point;
				frame_segments[symmetry_pair]->start_point[symmetry_axis] = symmetry_center[symmetry_axis] - dir;
				already_moved.Set(symmetry_pair);
				}
			}
		}	

	LocalDataChanged();
	GetCOREInterface()->RedrawViews(GetCOREInterface()->GetTime());
	}

void UVWProyector::PeltRotatePoints( float ang ) {
	PeltHoldPoints();

	Point3 cent(0.0f,0.0f,0.0f);
	int ct = 0;
	for ( int i=0; i<frame_segments.Count(); i++ ) {
		if ( frame_segments[i]->selected ) {
			cent += frame_segments[i]->start_point;
			ct++;
			}
		}
	if ( !ct ) return;
	cent /= float(ct);

	Matrix3 mat;
	mat.SetRotateZ(ang);
	mat.Translate(cent);

	Matrix3 inv_mat;
	inv_mat.SetTranslate(cent);
	inv_mat.Invert();
	
	BitArray already_moved(frame_segments.Count());
	already_moved.ClearAll();

	for ( int i=0; i<frame_segments.Count(); i++ ) {
		if ( !already_moved[i] && frame_segments[i]->selected ) {
			int symmetry_pair = -1;
			if ( symmetry_axis!=-1 ) {
				for ( int i_sp=0; i_sp<frame_segments_symmetry.Count(); i_sp++ ) {
					if ( i==frame_segments_symmetry[i_sp].x )
						symmetry_pair = frame_segments_symmetry[i_sp].y;
					else if ( i==frame_segments_symmetry[i_sp].y )
						symmetry_pair = frame_segments_symmetry[i_sp].x;
					}
				}
			Point3 p = frame_segments[i]->start_point;
			frame_segments[i]->start_point = (p * inv_mat) * mat;
			already_moved.Set(i);
			if ( symmetry_pair!=-1 ) {
				float dir = frame_segments[i]->start_point[symmetry_axis] - symmetry_center[symmetry_axis];
				frame_segments[symmetry_pair]->start_point = frame_segments[i]->start_point;
				frame_segments[symmetry_pair]->start_point[symmetry_axis] = symmetry_center[symmetry_axis] - dir;
				already_moved.Set(symmetry_pair);
				}
			}
		}	

	LocalDataChanged();
	GetCOREInterface()->RedrawViews(GetCOREInterface()->GetTime());
	}

void UVWProyector::PeltScalePoints( float scale, int scale_axis ) {
	PeltHoldPoints();

	Point3 cent(0.0f,0.0f,0.0f);
	int ct = 0;
	for ( int i=0; i<frame_segments.Count(); i++ ) {
		if ( frame_segments[i]->selected ) {
			cent += frame_segments[i]->start_point;
			ct++;
			}
		}
	if ( !ct ) return;
	cent /= float(ct);

	Point3 scale_p( 1.0f+scale/100.0f, 1.0f+scale/100.0f, 0.0f );

	if ( scale_axis == 1 )
		scale_p.y = 1.0f;
	if ( scale_axis == 2 )
		scale_p.x = 1.0f;

	Matrix3 mat;
	mat.IdentityMatrix();
	mat.Scale(scale_p);
	mat.Translate(cent);

	Matrix3 inv_mat;
	inv_mat.SetTranslate(cent);
	inv_mat.Invert();

	BitArray already_moved(frame_segments.Count());
	already_moved.ClearAll();

	for ( int i=0; i<frame_segments.Count(); i++ ) {
		if ( !already_moved[i] && frame_segments[i]->selected ) {
			int symmetry_pair = -1;
			if ( symmetry_axis!=-1 ) {
				for ( int i_sp=0; i_sp<frame_segments_symmetry.Count(); i_sp++ ) {
					if ( i==frame_segments_symmetry[i_sp].x )
						symmetry_pair = frame_segments_symmetry[i_sp].y;
					else if ( i==frame_segments_symmetry[i_sp].y )
						symmetry_pair = frame_segments_symmetry[i_sp].x;
					}
				}
			Point3 p = frame_segments[i]->start_point;
			frame_segments[i]->start_point = ( p * inv_mat ) * mat;
			already_moved.Set(i);
			if ( symmetry_pair!=-1 ) {
				float dir = frame_segments[i]->start_point[symmetry_axis] - symmetry_center[symmetry_axis];
				frame_segments[symmetry_pair]->start_point = frame_segments[i]->start_point;
				frame_segments[symmetry_pair]->start_point[symmetry_axis] = symmetry_center[symmetry_axis] - dir;
				already_moved.Set(symmetry_pair);
				}
			}
		}	

	LocalDataChanged();
	GetCOREInterface()->RedrawViews(GetCOREInterface()->GetTime());
	}

void UVWProyector::PeltStepRotatePoints(float ang) {
	PeltHoldPoints();

	int num_frame_segments = frame_segments.Count();
	Box3 border_box;
	for ( int i_fs=0; i_fs<num_frame_segments; i_fs++ ) {
		Point3 i_fs_pt = frame_segments[i_fs]->start_point;
		border_box += i_fs_pt;
		}
	Point3 center = border_box.Center();


#if MAX_VERSION_MAJOR < 24
	Matrix3 rot_m(1);
#else
	Matrix3 rot_m;
#endif

	rot_m.SetTrans(center);
	Matrix3 i_m = Inverse(rot_m);
	rot_m.PreRotateZ(ang);

	for ( int i_fs=0; i_fs<num_frame_segments; i_fs++ ) {
		Point3 p = frame_segments[i_fs]->start_point;
		p = i_m * p;
		p = rot_m * p;
		frame_segments[i_fs]->start_point = p;
		}

	if ( symmetry_axis!=-1 && ( ang==HALFPI || ang==-HALFPI ) ) {
		this->PeltBuildUpSymmetryTables((symmetry_axis+1)%2);
		}

	LocalDataChanged();
	GetCOREInterface()->RedrawViews(GetCOREInterface()->GetTime());
	}

// PeltMakeSymmetryPair
//  The user can select 2 points and manually insert them in the symmetric 
// table by calling this function.
void UVWProyector::PeltMakeSymmetryPair() {
	if ( symmetry_axis==-1 )
		return;

	IPoint2 pair(-1,-1);
	for ( int i_fs=0; i_fs<frame_segments.Count(); i_fs++ ) {
		if ( frame_segments[i_fs]->selected ) {
			if ( pair.x!=-1 && pair.y!=-1 ) {
				MessageBox(GetCOREInterface()->GetMAXHWnd(),_T("Please Select just 2 Points"),_T("Texture Layers Pelt"),MB_OK);
				return;
				}
			if ( pair.x==-1 )
				pair.x = i_fs;
			else 
				pair.y = i_fs;
			}
		}

	if ( pair.x==-1 || pair.y==-1 )
		return;

	// Delete the symmetry points that use any of the two selected points
	for ( int i_sp=frame_segments_symmetry.Count()-1; i_sp>=0; i_sp-- ) {
		IPoint2 fsp =  frame_segments_symmetry[i_sp];
		if ( fsp.x == pair.x || fsp.x == pair.y || fsp.y==pair.x || fsp.y==pair.y ) {
			frame_segments_symmetry.Delete(i_sp,1);
			}
		}
	frame_segments_symmetry.Append(1,&pair,0);
	frame_segments_symmetry.Shrink();

	// Make the two points symmetric
	Point3 p0 = frame_segments[pair.x]->start_point;
	Point3 p1 = frame_segments[pair.y]->start_point;

	int other_axis = (symmetry_axis+1)%2;
	float cen = 0.5f * ( p0[other_axis] + p1[other_axis] );
	float dir_0 = p0[symmetry_axis] - symmetry_center[symmetry_axis];
	float dir_1 = symmetry_center[symmetry_axis] - p1[symmetry_axis];
	float dir = 0.5f * ( dir_0 + dir_1 );
	p0[symmetry_axis] = symmetry_center[symmetry_axis] + dir;
	p1[symmetry_axis] = symmetry_center[symmetry_axis] - dir;
	p0[other_axis] = cen;
	p1[other_axis] = cen;

	frame_segments[pair.x]->start_point = p0;
	frame_segments[pair.y]->start_point = p1;

	LocalDataChanged();
	GetCOREInterface()->RedrawViews(GetCOREInterface()->GetTime());
	}

void UVWProyector::PeltClearSelect() {
	for ( int i=0; i<frame_segments.Count(); i++) {
		frame_segments[i]->selected = false;
		}
	}

BOOL UVWProyector::PeltHitSubPoint(Box2D box, int &fs, int &bv) {
	for ( int i=0; i<frame_segments.Count(); i++) {
		int j = (i+1)%frame_segments.Count();
		FrameSegments * fsi = frame_segments[i];
		FrameSegments * fsj = frame_segments[j];

		Point2 pti = fsi->start_point;
		Point2 ptj = fsj->start_point;

		for ( int i_pb=1; i_pb<fsi->border_vert.Count(); i_pb++ ) {
			Point2 pt = pti + fsi->position[i_pb] * ( ptj - pti );

			if ( PointInBox( pt, box ) ) {
				fs = i;
				bv = i_pb;
				return TRUE;
				}
			}
		}
	return FALSE;
	}

void UVWProyector::PeltAddPoint(int i_fs, int bv) {
	theHold.Begin();
	theHold.Put( new PeltFrameRestore(this,tl) );
	theHold.Accept( GetString(IDS_PELT_FRAME) );

	Tab <IPoint2> add_fs;
	add_fs.SetCount(2);
	add_fs[0] = IPoint2(i_fs,bv);
	add_fs[1] = IPoint2(-1,-1);

	IPoint2 f_fs(i_fs,bv);
	IPoint2 s_fs(-1,-1);

	// Lets see if this point has a symmetry segment....
	int num_frame_segments = frame_segments.Count();
	int symmetry_pair = -1;
	if ( symmetry_axis!=-1 ) {
		for ( int i_sp=0; i_sp<frame_segments_symmetry.Count(); i_sp++ ) {
			if ( i_fs==frame_segments_symmetry[i_sp].x )
				symmetry_pair = frame_segments_symmetry[i_sp].y;
			else if( i_fs==frame_segments_symmetry[i_sp].y )
				symmetry_pair = frame_segments_symmetry[i_sp].x;
			}
		}
	
	IPoint2 this_fs(i_fs,bv);
	IPoint2 symm_fs(0,0);
	
	// Now let's see if this is indeed a symmetry point (same number of border_verts)
	if ( symmetry_pair!=-1 ) {
		int o_fs = (num_frame_segments+symmetry_pair-1)%num_frame_segments;
		int o_bv;
		int i_num_bv = frame_segments[i_fs]->border_vert.Count();
		int o_num_bv = frame_segments[i_fs]->border_vert.Count();
		if ( i_num_bv==o_num_bv ) {
			o_bv = i_num_bv - bv;
			symm_fs = IPoint2(o_fs,o_bv);
			}
		else {
			symmetry_pair = -1;
			}
		}

	// If there is a symmetry, let's organize the fs from greater to lower...
	if ( symmetry_pair!=-1) {
		// We add the points in reversed order, so lets order them as fs - bv
		add_fs[0] = symm_fs; // First Frame Segment... this should be the greatest
		add_fs[1] = this_fs; // Second Frame Segment... this should be the lowest

		IPoint2 aux_fs = this_fs;
		if ( this_fs.x>symm_fs.x ) {
			add_fs[0] = this_fs;
			add_fs[1] = symm_fs;
			}
		else if ( this_fs.x==symm_fs.x ) {
			if ( this_fs.y>symm_fs.y ) {
				add_fs[0] = this_fs;
				add_fs[1] = symm_fs;
				}
			else if ( this_fs.y==symm_fs.y ) {
				// If it is not a good symmetry, just make it invalid!
				add_fs[1] = IPoint2(-1,-1);
				}
			}
		}

	for ( int j_fs=0; j_fs<2; j_fs++ ) {
		int i_fs = add_fs[j_fs].x;
		int bv = add_fs[j_fs].y;

		if ( i_fs!=-1 && bv!=-1 ) {
			FrameSegments * old_fs = frame_segments[i_fs];
			int new_num_points = old_fs->border_vert.Count() - bv;
			int old_num_points = old_fs->border_vert.Count() - new_num_points;

			int fsn = (i_fs+1)%frame_segments.Count();
			Point3 pi = frame_segments[i_fs]->start_point;
			Point3 pj = frame_segments[fsn]->start_point;
			Point3 pt = pi + frame_segments[i_fs]->position[bv] * ( pj - pi );

			float bv_pos = frame_segments[i_fs]->position[bv];

			FrameSegments * new_fs = new FrameSegments( new_num_points );
			new_fs->start_point = pt;
			new_fs->selected = false;
			new_fs->segment_length = 0.0;

			int o_id = bv;
			for ( int i=0; i<old_num_points; i++ ) {
				old_fs->position[i] = old_fs->position[i] / bv_pos;
				}

			for ( int i=0; i<new_num_points; i++ ) {
				new_fs->border_vert[i] = old_fs->border_vert[o_id];
				new_fs->position[i] = ( old_fs->position[o_id] - bv_pos ) / ( 1.0f - bv_pos );
				o_id++;
				}

			old_fs->position.SetCount(bv);
			old_fs->border_vert.SetCount(bv);

			frame_segments.Insert(i_fs+1,1,&new_fs);
			}
		}

	PeltBuildUpSymmetryTables(SYMMETRY_KEEP);

	LocalDataChanged();
	GetCOREInterface()->RedrawViews(GetCOREInterface()->GetTime());
	}

// This CompTable doesn't follow the 3dsmax tables rule, it returns the
// numbers inverted, so it can sort from greater to lower!
static int CompTable( const void *elem1, const void *elem2 ) {
    int *a = (int*)elem1;
    int *b = (int*)elem2;
	if ( *a > *b )	return -1;
	if ( *a < *b )	return 1;
	return 0;
	}

void UVWProyector::PeltDelPoints(int point) {
	int num_segments = frame_segments.Count();
	int num_selected = 0;
	for ( int i_fs=0; i_fs<num_segments; i_fs++ ) {
		if ( frame_segments[i_fs]->selected )
			num_selected++;
		}

	if ( num_segments-num_selected<3 ) 
		return;

	theHold.Begin();
	theHold.Put( new PeltFrameRestore(this,tl) );
	theHold.Accept( GetString(IDS_PELT_FRAME) );

	Tab <int> delete_points;
	delete_points.Resize(num_segments);
	BitArray to_delete(num_segments);
	to_delete.ClearAll();
	for ( int i_fs=0; i_fs<num_segments; i_fs++ ) {
		// Get All selected points, and the current point if needed...
		if ( point==i_fs || (point==-1&&frame_segments[i_fs]->selected) ) {
			// Get the symmetry pairs of this point
			int symmetry_pair = -1;
			if ( symmetry_axis!=-1 ) {
				for ( int i_sp=0; i_sp<frame_segments_symmetry.Count(); i_sp++ ) {
					if ( i_fs==frame_segments_symmetry[i_sp].x )
						symmetry_pair = frame_segments_symmetry[i_sp].y;
					else if( i_fs==frame_segments_symmetry[i_sp].y )
						symmetry_pair = frame_segments_symmetry[i_sp].x;
					}
				}
			delete_points.Append(1,&i_fs,0);
			to_delete.Set(i_fs);
			if ( symmetry_pair!=-1 && !to_delete[symmetry_pair] ) {
				delete_points.Append(1,&symmetry_pair,0);
				to_delete.Set(symmetry_pair);
				}
			}
		}

	// Organize our delete points from greater to lower
	delete_points.Shrink();
	delete_points.Sort(CompTable);

	if ( (frame_segments.Count()-delete_points.Count())<3 ) {
		MessageBox(GetCOREInterface()->GetMAXHWnd(),_T("You must have at least 3 Frame Segments!"),_T("Texture Layers Pelt"),MB_OK);
		}

	for ( int i=0; i<delete_points.Count(); i++ ) {
		PeltDelSinglePoint(delete_points[i],FALSE);
		}

	PeltBuildUpSymmetryTables(SYMMETRY_KEEP);

	LocalDataChanged();
	GetCOREInterface()->RedrawViews(GetCOREInterface()->GetTime());
	}

void UVWProyector::PeltDelSinglePoint(int fs, BOOL hold) {

	if ( hold ) {
		theHold.Begin();
		theHold.Put( new PeltFrameRestore(this,tl) );
		theHold.Accept( GetString(IDS_PELT_FRAME) );
		}

	int fs0 = (fs-1+frame_segments.Count())%frame_segments.Count();
	int fs2 = (fs+1)%frame_segments.Count();

	FrameSegments * fsa = frame_segments[fs0];
	FrameSegments * fse = frame_segments[fs];
	FrameSegments * fsn = frame_segments[fs2];

	Point3 pa = fsa->start_point;
	Point3 pe = fse->start_point;
	Point3 pn = fsn->start_point;

	float len_0 = Length( pe - pa );
	float len_1 = Length( pn - pe );

	float u_0 = len_0 / ( len_0 + len_1 );
	float u_1 = len_1 / ( len_0 + len_1 );

	int a_num_points = fsa->border_vert.Count();
	int e_num_points = fse->border_vert.Count();

	fsa->border_vert.SetCount( a_num_points + e_num_points );
	fsa->position.SetCount( a_num_points + e_num_points );

	for ( int i=0; i<a_num_points; i++ ) {
		fsa->position[i] = u_0 * fsa->position[i];
		}
	for ( int i=0; i<e_num_points; i++ ) {
		fsa->position[a_num_points+i] = u_0 + ( u_1 * fse->position[i] );
		fsa->border_vert[a_num_points+i] = fse->border_vert[i];
		}

	frame_segments.Delete(fs,1);

	if ( hold ) {
		LocalDataChanged();
		GetCOREInterface()->RedrawViews(GetCOREInterface()->GetTime());
		}
	}

void UVWProyector::PeltDeleteUV() {
	if ( pelt_poly_mesh ) {
		pelt_poly_mesh->DeleteThis();
		pelt_poly_mesh = NULL;
		}

	for ( int i=0; i<masses.Count(); i++ ) {
		masses[i]->springs.SetCount(0);
		delete masses[i];
		masses[i] = NULL;
		}
	masses.SetCount(0);
	vert_to_mass.SetCount(0);

	current_num_faces = 0;
	current_num_edges = 0;
	current_num_verts = 0;
	current_num_sel_faces = 0;
	current_num_sel_edges = 0;
	}

void UVWProyector::PeltDeleteFrameSegments() {
	for ( int i=0; i<frame_segments.Count(); i++ ) {
		frame_segments[i]->border_vert.SetCount(0);
		frame_segments[i]->position.SetCount(0);
		delete frame_segments[i];
		frame_segments[i] = NULL;
		}
	frame_segments.SetCount(0);
	border_verts.SetCount(0);
	}

TSTR UVWProyector::PeltBrowseForFileName(BOOL save, BOOL &cancel) {

	static TCHAR fname[256] = {'\0'};
	OPENFILENAME ofn;
	HWND hWnd = GetActiveWindow();
	memset(&ofn,0,sizeof(OPENFILENAME));

	FilterList fl;
	fl.Append( GetString(IDS_PELT_FRAME_FILE));
	fl.Append( _T("*.upf"));
	TSTR title;
	if (save)
		title = GetString(IDS_TLP_SAVE_FRAME);
	else 
		title = GetString(IDS_TLP_LOAD_FRAME);

	ofn.lStructSize     = sizeof(OPENFILENAME);
	ofn.hwndOwner       = tl->hwnd_main;
	ofn.lpstrFilter     = fl;
	ofn.nFilterIndex	= 1;
	ofn.lpstrFile       = fname;
	ofn.nMaxFile        = 256;    
	ofn.lpstrInitialDir = tl->ip->GetDir(APP_EXPORT_DIR);
	ofn.Flags           = OFN_HIDEREADONLY|(save? OFN_OVERWRITEPROMPT:(
							OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST));
	ofn.lpstrDefExt     = _T("upf");
	ofn.lpstrTitle      = title;
	//Win32 : ofn.hInstance		= (HINSTANCE) GetWindowLong(hWnd, GWL_HINSTANCE);
	
	if(save) {
		if(!GetSaveFileName(&ofn)) cancel = TRUE;
		}
	else {
		if(!GetOpenFileName(&ofn)) cancel = TRUE;
		}
	
	return fname;
	}
		
// Original Pelt File Version				1000
#define CURRENT_PELT_FILE_VERSION			1010	// Includes edge & face selection!

void UVWProyector::PeltSaveFrame(BitArray &face_sel, BitArray &edge_sel) {
	TSTR fname;
	BOOL cancel = FALSE;

	fname = PeltBrowseForFileName(TRUE,cancel);

	if (cancel) 
		return;

	int current_version = CURRENT_PELT_FILE_VERSION;

	FILE *tf = _tfopen(fname, _T("wb") );
	
	fwrite(&current_version, sizeof(current_version), 1, tf);
	
	// Start new Stuff Version 1010
	int num_faces = face_sel.GetSize();
	int num_edges = edge_sel.GetSize();

	fwrite(&num_faces,sizeof(num_faces), 1, tf);
	for ( int i_f=0; i_f<num_faces; i_f++ ) {
		char bit = face_sel[i_f]?1:0;
		fwrite(&bit,sizeof(bit),1,tf);
		}

	fwrite(&num_edges,sizeof(num_edges), 1, tf);
	for ( int i_e=0; i_e<num_edges; i_e++ ) {
		char bit = edge_sel[i_e]?1:0;
		fwrite(&bit,sizeof(bit),1,tf);
		}
	// End new Stuff Version 1010	

	int num_border_verts = border_verts.Count();
	fwrite(&num_border_verts, sizeof(num_border_verts), 1, tf);
	for ( int i_bv=0; i_bv<num_border_verts; i_bv++ ) {
		int bv = border_verts[i_bv];
		fwrite(&bv, sizeof(bv), 1, tf);
		}

	int num_frame_segments = frame_segments.Count();
	fwrite(&num_frame_segments, sizeof(num_frame_segments), 1, tf);
	for ( int i_fs=0; i_fs<num_frame_segments; i_fs++ ) {
		Point3 sp = frame_segments[i_fs]->start_point;
		int num_bv = frame_segments[i_fs]->border_vert.Count();
		fwrite(&sp, sizeof(sp), 1, tf);
		fwrite(&num_bv, sizeof(num_bv), 1, tf);
		for ( int i_bv=0; i_bv<num_bv; i_bv++ ) {
			int bv = frame_segments[i_fs]->border_vert[i_bv];
			float pos = frame_segments[i_fs]->position[i_bv];
			fwrite(&bv, sizeof(bv), 1, tf);
			fwrite(&pos, sizeof(pos), 1, tf);
			}
		}

	fclose(tf);
	}

void UVWProyector::PeltLoadFrame(FILE *tf) {

	Tab <int> new_border_verts;
	
	int num_border_verts;
	fread(&num_border_verts, sizeof(num_border_verts), 1, tf);

	if ( num_border_verts != border_verts.Count() ) {
		fclose(tf);
		return;
		}

	new_border_verts.SetCount( num_border_verts );
	for ( int i_bv=0; i_bv<num_border_verts; i_bv++ ) {
		int bv;
		fread(&bv, sizeof(bv), 1, tf);
		new_border_verts[i_bv] = bv;
		}

	BOOL good_border_verts = TRUE;
	int start_bv = -1;
	for ( int i_bv=0; i_bv<num_border_verts; i_bv++ ) {
		if ( border_verts[i_bv] == new_border_verts[0] ) {
			start_bv = i_bv;
			}
		}
	if ( start_bv != -1 ) {
		for ( int i_bv=0; i_bv<num_border_verts; i_bv++ ) {
			int j_bv = ( start_bv + i_bv ) % num_border_verts;
			if ( border_verts[j_bv] != new_border_verts[i_bv] ) {
				good_border_verts = FALSE;
				break;
				}
			}
		}

	if ( !good_border_verts ) {
		fclose(tf);
		return;
		}

	border_verts = new_border_verts;

	// Delete Old Pelt Frames
	for ( int i_fs=0; i_fs<frame_segments.Count(); i_fs++ ) {
		frame_segments[i_fs]->border_vert.SetCount(0);
		frame_segments[i_fs]->position.SetCount(0);
		delete frame_segments[i_fs];
		frame_segments[i_fs] = NULL;
		}
	frame_segments.SetCount(0);

	int num_frame_segments;
	fread(&num_frame_segments, sizeof(num_frame_segments), 1, tf);
	frame_segments.SetCount( num_frame_segments );
	for ( int i_fs=0; i_fs<num_frame_segments; i_fs++ ) {
		Point3 sp;
		int num_bv; 
		fread(&sp, sizeof(sp), 1, tf);
		fread(&num_bv, sizeof(num_bv), 1, tf);
		frame_segments[i_fs] = new FrameSegments(num_bv);
		frame_segments[i_fs]->start_point = sp;
		for ( int i_bv=0; i_bv<num_bv; i_bv++ ) {
			int bv;
			float pos;
			fread(&bv, sizeof(bv), 1, tf);
			fread(&pos, sizeof(pos), 1, tf);
			frame_segments[i_fs]->border_vert[i_bv] = bv;
			frame_segments[i_fs]->position[i_bv] = pos;
			}
		}
	}

void UVWProyector::LocalDataChanged() {
	valid_group = 0;
	NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
	} 
