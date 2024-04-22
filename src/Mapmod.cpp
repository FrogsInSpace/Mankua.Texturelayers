/**********************************************************************
 *<
	FILE: MultiMapMod.cpp

	DESCRIPTION:  A UVW mapping modifier

	CREATED BY: Rolf Berteig

	HISTORY: 10/21/95

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/
#include "mods.h"		
#include "uvwgroup.h"
#include "texutil.h"
#include "bmmlib.h"
#include "simpobj.h"
#include "simpmod.h"
#include "decomp.h"
#include "mapping.h"
#include "undo.h"
#include "texlay_mc_data.h"
#include "buildver.h"
#include "surf_api.h"
#include "meshadj.h"
#include "io.h"
#include <fcntl.h>
#include "load_cb.h"
#include "natural_box.h"
#include "uv_pelt_dlg.h"
#include "common.h"
#include "texlay.h"
#include "..\..\UVWFrame\src\uvwframe.h"
#include "3dsmaxport.h"

#if (MAX_RELEASE >= 4000)
static GenSubObjType SOT_Gizmo(1);
static GenSubObjType SOT_Points(14);
static GenSubObjType SOT_Faces(14);
static GenSubObjType SOT_Edges(14);
#endif


static inline Point3 pabs(Point3 p) { return Point3(fabs(p.x),fabs(p.y),fabs(p.z)); }

//--- ClassDescriptor and class vars ---------------------------------

IObjParam*          MultiMapMod::ip						= NULL;
MoveModBoxCMode*    MultiMapMod::moveMode				= NULL;
RotateModBoxCMode*  MultiMapMod::rotMode				= NULL;
UScaleModBoxCMode*  MultiMapMod::uscaleMode				= NULL;
NUScaleModBoxCMode* MultiMapMod::nuscaleMode			= NULL;
SquashModBoxCMode*  MultiMapMod::squashMode				= NULL;
SelectModBoxCMode*  MultiMapMod::selectMode				= NULL;
FaceAlignMode*      MultiMapMod::faceAlignMode			= NULL;
RegionFitMode*      MultiMapMod::regionFitMode			= NULL;
PickAcquire         MultiMapMod::pickAcquire;			
MultiMapMod*        MultiMapMod::editMod				= NULL;
														
HWND				MultiMapMod::hwnd_main				= NULL;
CListBoxHandler *	MultiMapMod::groups_menu_handler	= NULL; 
ICustEdit *			MultiMapMod::group_name_edit		= NULL;
ICustToolbar *		MultiMapMod::iToolbar				= NULL;
ICustButton *		MultiMapMod::iAdd					= NULL;
ICustButton *		MultiMapMod::iDel					= NULL;
ICustButton *		MultiMapMod::iSave					= NULL;
ICustButton *		MultiMapMod::iLoad					= NULL;
ICustButton *		MultiMapMod::iCopy					= NULL;
ICustButton *		MultiMapMod::iPaste					= NULL;
ICustButton *		MultiMapMod::iTools					= NULL;
ISpinnerControl *	MultiMapMod::mapch_spin				= NULL;
ISpinnerControl *	MultiMapMod::length_spin			= NULL;
ISpinnerControl *	MultiMapMod::width_spin				= NULL;
ISpinnerControl *	MultiMapMod::height_spin			= NULL;

HWND				MultiMapMod::hwnd_tiling			= NULL;
ISpinnerControl *	MultiMapMod::u_start_spin			= NULL;
ISpinnerControl *	MultiMapMod::u_offset_spin			= NULL;
ISpinnerControl *	MultiMapMod::v_start_spin			= NULL;
ISpinnerControl *	MultiMapMod::v_offset_spin			= NULL;
ISpinnerControl *	MultiMapMod::w_start_spin			= NULL;
ISpinnerControl *	MultiMapMod::w_offset_spin			= NULL;
ISpinnerControl *	MultiMapMod::att_all_spin			= NULL;
ISpinnerControl *	MultiMapMod::att_u_start_spin		= NULL;
ISpinnerControl *	MultiMapMod::att_u_offset_spin		= NULL;
ISpinnerControl *	MultiMapMod::att_v_start_spin		= NULL;
ISpinnerControl *	MultiMapMod::att_v_offset_spin		= NULL;
ISpinnerControl *	MultiMapMod::att_w_start_spin		= NULL;
ISpinnerControl *	MultiMapMod::att_w_offset_spin		= NULL;
ISpinnerControl *	MultiMapMod::att_ruv_spin			= NULL;
ISpinnerControl *	MultiMapMod::u_move_spin			= NULL;
ISpinnerControl *	MultiMapMod::v_move_spin			= NULL;
ISpinnerControl *	MultiMapMod::w_move_spin			= NULL;
ISpinnerControl *	MultiMapMod::u_rotate_spin			= NULL;
ISpinnerControl *	MultiMapMod::v_rotate_spin			= NULL;
ISpinnerControl *	MultiMapMod::w_rotate_spin			= NULL;
ISpinnerControl *	MultiMapMod::u_scale_spin			= NULL;
ISpinnerControl *	MultiMapMod::v_scale_spin			= NULL;
ISpinnerControl *	MultiMapMod::w_scale_spin			= NULL;

HWND				MultiMapMod::hwnd_tools				= NULL;

HWND				MultiMapMod::hwnd_spline			= NULL;
ICustButton * 		MultiMapMod::pick_spline_button		= NULL;
ISpinnerControl *	MultiMapMod::normals_start_spin		= NULL;

HWND				MultiMapMod::hwnd_free				= NULL;
ICustButton * 		MultiMapMod::pick_surface_button	= NULL;
ISpinnerControl *	MultiMapMod::free_threshold_spin	= NULL;

HWND				MultiMapMod::hwnd_pelt				= NULL;
ISpinnerControl *	MultiMapMod::pelt_rigidity_spin		= NULL;
ISpinnerControl *	MultiMapMod::pelt_stability_spin	= NULL;
ISpinnerControl *	MultiMapMod::pelt_frame_spin		= NULL;
ISpinnerControl *	MultiMapMod::pelt_iter_spin			= NULL;

HWND				MultiMapMod::hwnd_frame				= NULL;
ICustButton * 		MultiMapMod::pick_frame_button		= NULL;

HWND				MultiMapMod::hwnd_data				= NULL;

HWND				MultiMapMod::hIsNurbs				= NULL;

PickNodeMode*		MultiMapMod::pickMode				= NULL;
UVWProyector*		MultiMapMod::uvw_buffer				= NULL;
BitArray			MultiMapMod::face_buffer;
BitArray			MultiMapMod::edge_buffer;
PolyUVWData*		MultiMapMod::uvw_data_buffer		= NULL;

UVPeltDlg*			MultiMapMod::uv_pelt_dlg			= NULL;

HWND				MultiMapMod::hwnd_options			= NULL;

class UVWMapClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading = FALSE) {return new MultiMapMod(!loading);}
	const TCHAR *	ClassName() { return GetString(IDS_MM_CLASS); }
	
#if MAX_VERSION_MAJOR >= 24
	const TCHAR *	NonLocalizedClassName() { return ClassName(); }
#endif

	SClass_ID		SuperClassID() { return OSM_CLASS_ID; }
	Class_ID		ClassID() { return MULTIMAP_MOD_CID; }
	const TCHAR* 	Category() {return GetString(IDS_DC_TEXTURELAYERS);}

	// Hardwired name, used by MAX Script as unique identifier
	const TCHAR*	InternalName()				{ return _T("TextureLayers"); }
	HINSTANCE		HInstance()					{ return hInstance; }
	};

static UVWMapClassDesc mapDesc;
extern ClassDesc2* GetUVWMultiMapModDesc() {return &mapDesc;}

static void DoBoxIcon(BOOL sel,float length, PolyLineProc& lp);


/***********************************************************************************\
 **																				  **
 **		Multi Map Modifier Enum Proc											  **
 **																				  **
\***********************************************************************************/

class MyEnumProc : public DependentEnumProc 
	{
      public :
      virtual int proc(ReferenceMaker *rmaker); 
      INodeTab Nodes;              
	};

int MyEnumProc::proc(ReferenceMaker *rmaker) 
	{
	if (rmaker->SuperClassID()==BASENODE_CLASS_ID)    
		{
		Nodes.Append(1, (INode **)&rmaker);
		}
	return 0;
	}	

/***********************************************************************************\
 **																				  **
 **		Multi Map Modifier Enum Proc											  **
 **																				  **
\***********************************************************************************/

void MultiMapMod::ViewportAlign() {

	theHold.Begin();
	theHold.Put(new GroupsTableRestore(this,INVALID_GROUP));

//JW Code Change: GetActiveViewport is deprecated in 3ds Max 2013+
#if MAX_VERSION_MAJOR < 15
	ViewExp *vpt = ip->GetActiveViewport();
#else
	ViewExp *vpt = &ip->GetActiveViewExp();
#endif
	if (!vpt) return;

	// Get mod contexts and nodes for this modifier
	ModContextList mcList;
	INodeTab nodeList;
	ip->GetModContexts(mcList,nodeList);

	// Viewport tm
	Matrix3 vtm;
	vpt->GetAffineTM(vtm);

	vtm = Inverse(vtm);

	// Node tm
	Matrix3 ntm = nodeList[0]->GetObjectTM(ip->GetTime());	
	ntm.NoScale();

	// MC tm
#if MAX_VERSION_MAJOR < 24	
	Matrix3 mctm(1);
#else
	Matrix3 mctm;
#endif

	if (mcList[0]->tm) mctm = *mcList[0]->tm;

	// Compute the new destination transform for tmCont
	Matrix3 destTM = vtm * Inverse(ntm) * mctm;

	destTM.PreRotateZ(PI);

	switch (uvwProy[current_channel]->GetAxis()) {
		case 0:
			destTM.PreRotateY(-HALFPI);
			break;
		case 1:
			destTM.PreRotateX(HALFPI);
			break;
		}

	// Current val of tmCont
#if MAX_VERSION_MAJOR < 24
	Matrix3 curTM(1);
#else
	Matrix3 curTM;
#endif

	uvwProy[current_channel]->tmControl->GetValue(ip->GetTime(),&curTM,FOREVER,CTRL_RELATIVE);

	Point3 s;
	for (int i=0; i<3; i++) s[i] = Length(curTM.GetRow(i));

	// These types are aligned differently
	if (uvwProy[current_channel]->GetMappingType()==MAP_TL_CYLINDRICAL ||
		uvwProy[current_channel]->GetMappingType()==MAP_TL_SPHERICAL ||
		uvwProy[current_channel]->GetMappingType()==MAP_TL_BALL) {
		destTM.PreRotateX(-HALFPI);
		}

	// Keep position and scale the same	
	destTM.SetTrans(curTM.GetTrans());
	destTM.PreScale(s);	

	// Plug-in the new value
	SetXFormPacket pckt(destTM);
	uvwProy[current_channel]->tmControl->SetValue(0, &pckt);

	nodeList.DisposeTemporary();

// JW Code Change: only necessary with legacy GetActiveViewport()
#if MAX_VERSION_MAJOR < 15
	ip->ReleaseViewport(vpt);
#endif	
	
	ip->RedrawViews(ip->GetTime());

	theHold.Put(new GroupsTableRestore(this,INVALID_GROUP));
	theHold.Accept(_T("View Align"));
	}


static void MatrixFromNormal(Point3& normal, Matrix3& mat)
	{
	Point3 vx;
	vx.z = .0f;
	vx.x = -normal.y;
	vx.y = normal.x;	
	if ( vx.x == .0f && vx.y == .0f ) {
		vx.x = 1.0f;
		}
	mat.SetRow(0,vx);
	mat.SetRow(1,normal^vx);
	mat.SetRow(2,normal);
	mat.SetTrans(Point3(0,0,0));
	mat.NoScale();
	}

void FaceAlignMouseProc::FaceAlignMap(HWND hWnd,IPoint2 m) 
{
//JW Code Change: GetActiveViewport is deprecated in 3ds Max 2013+
#if MAX_VERSION_MAJOR < 15
	ViewExp *vpt = ip->GetViewport(hWnd);
#else
	ViewExp *vpt = &ip->GetViewExp( hWnd );
#endif
	if (!vpt) return;

	Ray ray, wray;
	float at;
	TimeValue t = ip->GetTime();	
	GeomObject *obj;
	Point3 norm, pt;
	Interval valid;

	// Get mod contexts and nodes for this modifier
	ModContextList mcList;
	INodeTab nodeList;
	ip->GetModContexts(mcList,nodeList);

	// Calculate a ray from the mouse point
	vpt->MapScreenToWorldRay(float(m.x), float(m.y),wray);

	for (int i=0; i<nodeList.Count(); i++) {
		INode *node = nodeList[i];

		// Get the object from the node
		ObjectState os = node->EvalWorldState(t);
		if (os.obj->SuperClassID()==GEOMOBJECT_CLASS_ID) {
			obj = (GeomObject*)os.obj;
		} else {
			continue;
			}

		// Back transform the ray into object space.		
		Matrix3 obtm  = node->GetObjectTM(t);
		Matrix3 iobtm = Inverse(obtm);
		ray.p   = iobtm * wray.p;
		ray.dir = VectorTransform(iobtm, wray.dir);
	
		// See if we hit the object
		if (obj->IntersectRay(t,ray,at,norm)) {
			// Calculate the hit point
			pt = ray.p + ray.dir * at;
					
			// Get the mod context tm
#if MAX_VERSION_MAJOR < 24
			Matrix3 tm(1);
#else
			Matrix3 tm;
#endif

			if (mcList[0]->tm) tm = tm * *mcList[0]->tm;
		
			// Transform the point and ray into mod context space
			pt = pt * tm;
			norm = Normalize(VectorTransform(tm,norm));
		
			// Construct the target transformation in mod context space
			Matrix3 destTM;
			MatrixFromNormal(norm,destTM);
			destTM.SetTrans(pt);
			destTM.PreRotateZ(PI);
			switch (mod->uvwProy[mod->current_channel]->GetAxis()) {
				case 0:
					destTM.PreRotateY(-HALFPI);
					break;
				case 1:
					destTM.PreRotateX(HALFPI);
					break;
				}
			// Our current transformation... gives relative TM

#if MAX_VERSION_MAJOR < 24
			Matrix3 curTM(1), relTM, id(1);
#else
			Matrix3 curTM, relTM, id;
#endif

			mod->uvwProy[mod->current_channel]->tmControl->GetValue(t,&curTM,valid,CTRL_RELATIVE);
			relTM = Inverse(curTM) * destTM;
		
			// Here's the modifications we need to make to get there
			tm.IdentityMatrix();
			tm.SetTrans(curTM.GetTrans());
			AffineParts parts;			
			decomp_affine(relTM,&parts);
			Point3 delta = destTM.GetTrans()-curTM.GetTrans();
			mod->Rotate(t,id,tm,parts.q);
			mod->Move(t,id,id,delta);

			mod->uvwProy[mod->current_channel]->valid_group = 0;

			break;
			}
		}

	nodeList.DisposeTemporary();

//JW Code Change:not necessary in 3ds Max 2013+
#if MAX_VERSION_MAJOR < 15
	ip->ReleaseViewport(vpt);
#endif
}

int FaceAlignMouseProc::proc(
		HWND hWnd, int msg, int point, int flags, IPoint2 m)
	{
	switch (msg) {
		case MOUSE_POINT:
			if (point==0) {				
				theHold.Begin();
				ip->RedrawViews(ip->GetTime(),REDRAW_BEGIN);
			} else {
				theHold.Accept(_T("Normal Align"));
				ip->RedrawViews(ip->GetTime(),REDRAW_END);
				}
			break;

		case MOUSE_MOVE: {
			theHold.Restore();
			FaceAlignMap(hWnd,m);
			ip->RedrawViews(ip->GetTime(),REDRAW_INTERACTIVE);			
			break;
			}

		case MOUSE_ABORT:
			theHold.Cancel();
			ip->RedrawViews(ip->GetTime(),REDRAW_END);
			break;

		case MOUSE_FREEMOVE:			
			SetCursor(ip->GetSysCursor(SYSCUR_SELECT));
			break;
		}
	return TRUE;
	}

void FaceAlignMode::EnterMode()
	{
	mod->EnterNormalAlign();	
	}

void FaceAlignMode::ExitMode()
	{
	mod->ExitNormalAlign();	
	}


void RegionFitMouseProc::RegionFitMap(HWND hWnd,IPoint2 m)
{
	//JW Code Change: GetActiveViewport is deprecated in 3ds Max 2013+
#if MAX_VERSION_MAJOR < 15
	ViewExp *vpt = ip->GetViewport(hWnd);
#else
	ViewExp *vpt = &ip->GetViewExp(hWnd);
#endif
	if (!vpt) return;

	// Get mod contexts and nodes for this modifier
	ModContextList mcList;
	INodeTab nodeList;
	ip->GetModContexts(mcList,nodeList);

	// Viewport tm
	Matrix3 vtm;
	vpt->GetAffineTM(vtm);
	vtm = Inverse(vtm);

	// Node tm
	Matrix3 ntm = nodeList[0]->GetObjectTM(ip->GetTime());	
	
	// MC tm

#if MAX_VERSION_MAJOR < 24
	Matrix3 mctm(1);
#else
	Matrix3 mctm;
#endif

	if (mcList[0]->tm) mctm = *mcList[0]->tm;

	// Current val of tmCont.. remove any scale

#if MAX_VERSION_MAJOR < 24
	Matrix3 ctm(1);
#else
	Matrix3 ctm;
#endif

	mod->uvwProy[mod->current_channel]->tmControl->GetValue(ip->GetTime(),&ctm,FOREVER,CTRL_RELATIVE);
	AffineParts parts;
	decomp_affine(ctm, &parts);
	parts.q.MakeMatrix(ctm);
	ctm.Translate(parts.t);
	
	// Compute the inverse world space tm for the gizmo
	Matrix3 iwtm = Inverse(ctm * Inverse(mctm) * ntm);
	
	// Calculate a ray from the two mouse points
	Ray mray, omray;
	float at;
	Point3 p1, p2;
	vpt->MapScreenToWorldRay(float(m.x), float(m.y),mray);
	vpt->MapScreenToWorldRay(float(om.x), float(om.y),omray);
	
	// Back transform the rays into gizmo space
	mray.p    = iwtm * mray.p;
	mray.dir  = VectorTransform(iwtm, mray.dir);
	omray.p   = iwtm * omray.p;
	omray.dir = VectorTransform(iwtm, omray.dir);

	// JW Code change: fix unitialized locals warning
	float dir=0.0, pnt = 0.0, odir = 0.0, opnt = 0.0;
	switch (mod->uvwProy[mod->current_channel]->GetAxis()) {
		case 0:
			dir = mray.dir.x; odir = omray.dir.x;
			pnt = mray.p.x; opnt = omray.p.x;
			break;
		case 1:
			dir = mray.dir.y; odir = omray.dir.y;
			pnt = mray.p.y; opnt = omray.p.y;
			break;
		case 2:
			dir = mray.dir.z; odir = omray.dir.z;
			pnt = mray.p.z; opnt = omray.p.z;
			break;
		}
#define EPSILON	0.001
	// Make sure we're going to hit
	if (fabs(dir)>EPSILON && fabs(odir)>EPSILON) {
	
		// Compute the point of intersection
		at = -pnt/dir;
		p1 = mray.p + at*mray.dir;
		at = -opnt/odir;
		p2 = omray.p + at*omray.dir;
		
		// Center the map in the region
		ctm.PreTranslate((p1+p2)/2.0f);

		// Compute scale factors and scale

		// JW Code Change : fix uninitialized locals warning
		float sx=0.0 ,sy = 0.0;
		switch (mod->uvwProy[mod->current_channel]->GetAxis()) {
			case 0:
				sx = (float)fabs(p1.z-p2.z);
				sy = (float)fabs(p1.y-p2.y);
				break;
			case 1:
				sx = (float)fabs(p1.x-p2.x);
				sy = (float)fabs(p1.z-p2.z);
				break;
			case 2:
				sx = (float)fabs(p1.x-p2.x);
				sy = (float)fabs(p1.y-p2.y);
				break;
			}
		
		// Scale params instead of the matrix
		TimeValue t = ip->GetTime();		
		mod->uvwProy[mod->current_channel]->SetWidth(t,sx);
		mod->uvwProy[mod->current_channel]->SetLength(t,sy);		
		/*
		if (sx>0.0f && sy>0.0f) {
			ctm.PreScale(Point3(sx,sy,1.0f));
			}
		*/

		// Plug-in the new value		
		SetXFormPacket pckt(ctm);
		mod->uvwProy[mod->current_channel]->tmControl->SetValue(0, &pckt);		
		}

	nodeList.DisposeTemporary();

//JW Code Change: not necessary in Max 2013+
#if MAX_VERSION_MAJOR < 15
	ip->ReleaseViewport(vpt);
#endif

	ip->RedrawViews(ip->GetTime());
}

int RegionFitMouseProc::proc(
		HWND hWnd, int msg, int point, int flags, IPoint2 m)
	{
	switch (msg) {
		case MOUSE_PROPCLICK:
			ip->SetStdCommandMode(CID_OBJMOVE);
			break;

		case MOUSE_POINT:
			if (point==0) {				
				om = m;
				theHold.Begin();
				ip->RedrawViews(ip->GetTime(),REDRAW_BEGIN);
			} else {
				theHold.Accept(0);
				ip->RedrawViews(ip->GetTime(),REDRAW_END);
				}
			break;

		case MOUSE_MOVE: {
			theHold.Restore();
			RegionFitMap(hWnd,m);
			ip->RedrawViews(ip->GetTime(),REDRAW_INTERACTIVE);			
			break;
			}

		case MOUSE_ABORT:
			theHold.Cancel();
			ip->RedrawViews(ip->GetTime(),REDRAW_END);
			break;

		case MOUSE_FREEMOVE:			
			SetCursor(ip->GetSysCursor(SYSCUR_SELECT));
			break;
		}
	return TRUE;
	}

void RegionFitMode::EnterMode()
	{
	mod->EnterRegionFit();	
	}

void RegionFitMode::ExitMode()
	{
	mod->ExitRegionFit();	
	}


//--- PickAcquire -------------------------------------------------------


//Win32 : static BOOL CALLBACK AcquireTypeDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
static INT_PTR CALLBACK AcquireTypeDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
	static int type = IDC_ACQUIRE_RELTL;
	static int *acq;

	switch (msg) {
		case WM_INITDIALOG: {
			acq = (int*)lParam;
			ISpinnerControl *spin = 
				SetupIntSpinner(hWnd,IDC_MM_ORCHSPIN,IDC_MM_ORCH,1,acq[0],acq[1]);
			ReleaseISpinner(spin);
			CheckDlgButton(hWnd,IDC_AQ_ANI,0);
			CheckRadioButton(hWnd,IDC_ACQUIRE_RELTL,IDC_ACQUIRE_ABSTL,IDC_ACQUIRE_RELTL);
			CenterWindow(hWnd,GetParent(hWnd));
			}
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDC_ACQUIRE_RELTL:
				case IDC_ACQUIRE_ABSTL:
					if (IsDlgButtonChecked(hWnd,IDC_ACQUIRE_RELTL)) 
						EnableWindow(GetDlgItem(hWnd,IDC_AQ_ANI),TRUE);
					else 
						EnableWindow(GetDlgItem(hWnd,IDC_AQ_ANI),FALSE);
					break;
				case IDOK: {
					ISpinnerControl *spin = 
						GetISpinner(GetDlgItem(hWnd,IDC_MM_ORCHSPIN));
					acq[0] = spin->GetIVal() - 1;
					ReleaseISpinner(spin);

					acq[1] = IsDlgButtonChecked(hWnd,IDC_AQ_ANI);

					if (IsDlgButtonChecked(hWnd,IDC_ACQUIRE_RELTL)) 
						 type = IDC_ACQUIRE_RELTL;
					else type = IDC_ACQUIRE_ABSTL;
					EndDialog(hWnd,type);
					}
					break;

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

static BOOL GetAcquireType(HWND hWnd,int &type, int *acq)
	{
	type = DialogBoxParam(
		hInstance,
		MAKEINTRESOURCE(IDD_MAP_ACQUIRETL),
		hWnd,
		AcquireTypeDlgProc,
		(LPARAM)acq);
	if (type<0) return FALSE;
	else return TRUE;
	}



MultiMapMod *PickAcquire::FindFirstMap(ReferenceTarget *ref)
	{
	MultiMapMod *mod;
	// JW Code change, explicit != NULL check, prevent C4706
	if ( (mod = GetTexLayInterface(ref)) != NULL ) {
		if (!mod->TestAFlag(A_MOD_DISABLED)) return mod;
		}
	
	for (int i=ref->NumRefs()-1; i>=0; i--) {
		ReferenceTarget *cref = ref->GetReference(i);
		if (cref) {
			// JW Code change, explicit != NULL check, prevent C4706
			if ((mod = FindFirstMap(cref)) != NULL)  return mod;			
			}
		}
	return NULL;
	}

BOOL PickAcquire::Filter(INode *node)
	{
	MultiMapMod *amod = FindFirstMap(node->GetObjectRef());
	if (amod!=mod && amod) return TRUE;
	else return FALSE;
	}

BOOL PickAcquire::HitTest(IObjParam *ip,HWND hWnd,ViewExp *vpt,IPoint2 m,int flags)	{
	INode *node = 
		ip->PickNode(hWnd,m,this);
	return node ? TRUE : FALSE;
	}
	

class PickAcquireSourceMC : public ModContextEnumProc {
	public:
		ModContext *mc;
		PickAcquireSourceMC() {mc = NULL;}
		BOOL proc(ModContext *mc) {
			this->mc = mc;
			return FALSE;
			}
	};

BOOL PickAcquire::Pick(IObjParam *ip,ViewExp *vpt)
	{
	INode *node = vpt->GetClosestHit();
	assert(node);
	MultiMapMod *amod = FindFirstMap(node->GetObjectRef());
	if (amod) {
		int type;

		int acq[2];
		acq[0] = amod->uvwProy.Count();
		acq[1] = amod->current_channel+1;

		if (!GetAcquireType(ip->GetMAXHWnd(),type,acq)) return TRUE;

		// Get our node and mod context
		ModContextList mcList;
		INodeTab nodes;
		ip->GetModContexts(mcList,nodes);
		assert(nodes.Count());
		
		// Get the source mod's mod context
		PickAcquireSourceMC pasmc;
		amod->EnumModContexts(&pasmc);
		assert(pasmc.mc);

		// Do it!
		theHold.Begin();
		AcquireMapping(mod,mcList[0],nodes[0],amod,acq[0],pasmc.mc,node,type,acq[1]);
		theHold.Accept(_T("Acquire Mapping"));

		nodes.DisposeTemporary();
		}
	return TRUE;
	}

void PickAcquire::EnterMode(IObjParam *ip)
	{
	mod->EnterAcquire();
	}

void PickAcquire::ExitMode(IObjParam *ip)
	{
	mod->ExitAcquire();
	}

void PickAcquire::AcquireMapping(
		MultiMapMod *toMod, ModContext *toMC, INode *toNode,
		MultiMapMod *fromMod, int channel, ModContext *fromMC, INode *fromNode,
		int type, int ani)
	{
	toMod->uvwProy[toMod->current_channel]->SetMappingType( fromMod->uvwProy[channel]->GetMappingType() );

	TimeValue t = ip->GetTime();

	if (ani && IDC_ACQUIRE_RELTL) {
		toMod->uvwProy[toMod->current_channel]->ReplaceReference(PBLOCK_REF,fromMod->uvwProy[channel]->pblock->Clone( DefaultRemapDir() ));

		toMod->uvwProy[toMod->current_channel]->ReplaceReference(TMCTRL_REF,fromMod->uvwProy[channel]->tmControl->Clone( DefaultRemapDir() ));

		if (fromMod->uvwProy[channel]->GetMappingType() == MAP_TL_SPLINE) {
			fromMod->uvwProy[channel]->GetSplineNode();
			if (fromMod->uvwProy[channel]->spline_node) {
				toMod->uvwProy[toMod->current_channel]->SetSplineNode( fromMod->uvwProy[channel]->spline_node );
				}
			}
		if (fromMod->uvwProy[channel]->GetMappingType() == MAP_TL_FFM) {
			toMod->uvwProy[toMod->current_channel]->ReplaceReference(GRID_REF,fromMod->uvwProy[channel]->gm->Clone());
			}

		toMod->uvwProy[toMod->current_channel]->angleCrv = fromMod->uvwProy[channel]->angleCrv;
		toMod->uvwProy[toMod->current_channel]->tileCrv = fromMod->uvwProy[channel]->tileCrv;
		toMod->uvwProy[toMod->current_channel]->wTileCrv = fromMod->uvwProy[channel]->wTileCrv;
		}
	else {
		// Build the mats

#if MAX_VERSION_MAJOR < 24
		Matrix3 fromNTM(1);
#else
		Matrix3 fromNTM;
#endif


#if MAX_VERSION_MAJOR < 24
		Matrix3 toNTM(1);
#else
		Matrix3 toNTM;
#endif

		if (type==IDC_ACQUIRE_ABSTL) {
			fromNTM = fromNode->GetObjectTM(ip->GetTime());	
			toNTM   = toNode->GetObjectTM(ip->GetTime());	
			}
		Matrix3 fromTM  = fromMod->uvwProy[channel]->CompMatrix(ip->GetTime(),fromMC,&fromNTM,FALSE,type==IDC_ACQUIRE_ABS);
		Matrix3 destTM  = fromTM * Inverse(toNTM);
		if (toMC->tm) destTM = destTM * (*toMC->tm);
		
		if (type==IDC_ACQUIRE_ABSTL) {// && toMod->TestAFlag(A_PLUGIN1)) {
			switch (toMod->uvwProy[toMod->current_channel]->GetMappingType()) {
				case MAP_TL_SPLINE:
				case MAP_TL_FFM:
				case MAP_TL_PLANAR:
				case MAP_TL_BOX:
					destTM.PreRotateZ(-PI);
					break;
				
				case MAP_TL_BALL:
				case MAP_TL_SPHERICAL:
				case MAP_TL_CYLINDRICAL:
				case MAP_TL_CYLCAP:
					destTM.PreRotateZ(-HALFPI);
					break;
				}		
			}

		// Set the TM
		SetXFormPacket pckt(destTM);
		toMod->uvwProy[toMod->current_channel]->tmControl->SetValue(0,&pckt);

		toMod->uvwProy[toMod->current_channel]->SetTileU(fromMod->uvwProy[channel]->GetTileU(t),t);
		toMod->uvwProy[toMod->current_channel]->SetTileV(fromMod->uvwProy[channel]->GetTileV(t),t);
		toMod->uvwProy[toMod->current_channel]->SetTileW(fromMod->uvwProy[channel]->GetTileW(t),t);
		toMod->uvwProy[toMod->current_channel]->SetOffsetU(fromMod->uvwProy[channel]->GetOffsetU(t),t);
		toMod->uvwProy[toMod->current_channel]->SetOffsetV(fromMod->uvwProy[channel]->GetOffsetV(t),t);
		toMod->uvwProy[toMod->current_channel]->SetOffsetW(fromMod->uvwProy[channel]->GetOffsetW(t),t);
		toMod->uvwProy[toMod->current_channel]->SetAttStatus(fromMod->uvwProy[channel]->GetAttStatus(t),t);
		toMod->uvwProy[toMod->current_channel]->SetAttGlobal(fromMod->uvwProy[channel]->GetAttGlobal(t),t);
		toMod->uvwProy[toMod->current_channel]->SetAttUStart(fromMod->uvwProy[channel]->GetAttUStart(t),t);
		toMod->uvwProy[toMod->current_channel]->SetAttVStart(fromMod->uvwProy[channel]->GetAttVStart(t),t);
		toMod->uvwProy[toMod->current_channel]->SetAttWStart(fromMod->uvwProy[channel]->GetAttWStart(t),t);
		toMod->uvwProy[toMod->current_channel]->SetAttUOffset(fromMod->uvwProy[channel]->GetAttUOffset(t),t);
		toMod->uvwProy[toMod->current_channel]->SetAttVOffset(fromMod->uvwProy[channel]->GetAttVOffset(t),t);
		toMod->uvwProy[toMod->current_channel]->SetAttWOffset(fromMod->uvwProy[channel]->GetAttWOffset(t),t);
		toMod->uvwProy[toMod->current_channel]->SetAttRad(fromMod->uvwProy[channel]->GetAttRad(t),t);
		toMod->uvwProy[toMod->current_channel]->SetLength(t,fromMod->uvwProy[channel]->GetLength(t));
		toMod->uvwProy[toMod->current_channel]->SetWidth(t,fromMod->uvwProy[channel]->GetWidth(t));
		toMod->uvwProy[toMod->current_channel]->SetHeight(t,fromMod->uvwProy[channel]->GetHeight(t));
		toMod->uvwProy[toMod->current_channel]->SetFFMThresh(fromMod->uvwProy[channel]->GetFFMThresh(t),t);
		toMod->uvwProy[toMod->current_channel]->SetAxis(fromMod->uvwProy[channel]->GetAxis(),t);
		toMod->uvwProy[toMod->current_channel]->SetReverse(fromMod->uvwProy[channel]->GetReverse(t),t);
		toMod->uvwProy[toMod->current_channel]->SetNormalize(fromMod->uvwProy[channel]->GetNormalize(t),t);
		toMod->uvwProy[toMod->current_channel]->SetNormalType(fromMod->uvwProy[channel]->GetNormalType(t),t);

		if (fromMod->uvwProy[channel]->GetMappingType() == MAP_TL_SPLINE) {
			fromMod->uvwProy[channel]->GetSplineNode();
			if (fromMod->uvwProy[channel]->spline_node) {
				toMod->uvwProy[channel]->SetSplineNode( fromMod->uvwProy[channel]->spline_node );
				}
			}
		if (fromMod->uvwProy[channel]->GetMappingType() == MAP_TL_FFM) {
			toMod->uvwProy[channel]->ReplaceReference(GRID_REF,fromMod->uvwProy[channel]->gm->Clone());
			}
		}


	toMod->LocalDataChanged();

	ip->RedrawViews(ip->GetTime());
	}

static BOOL CALLBACK AboutDlgProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
	switch (msg) {
		case WM_INITDIALOG:
			CenterWindow(hWnd, GetParent(hWnd));
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDOK:
					EndDialog(hWnd,1);
					break;
				}
			break;
		
		default:
			return FALSE;
		}
	return TRUE;
	}

//Win32 : BOOL CALLBACK TexMagicNurbsProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
INT_PTR CALLBACK TexMagicNurbsProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	//Win32 : MultiMapMod *mmmod = (MultiMapMod *)GetWindowLong(hDlg, GWL_USERDATA);
	MultiMapMod *mmmod = DLGetWindowLongPtr<MultiMapMod*>(hDlg);

	int i = 0;
	if ( !mmmod && message != WM_INITDIALOG ) return FALSE;
	switch ( message ) {
		case WM_INITDIALOG:
			{
			mmmod = (MultiMapMod *)lParam;
		 	//Win32 : SetWindowLong( hDlg, GWL_USERDATA, (LONG)mmmod );
			DLSetWindowLongPtr(hDlg, mmmod);
			}
			return TRUE;

		case WM_DESTROY:
			return FALSE;

		case WM_MOUSEACTIVATE:
			mmmod->ip->RealizeParamPanel();
			return FALSE;

		case WM_LBUTTONDOWN: case WM_LBUTTONUP:	case WM_MOUSEMOVE:
			return FALSE;
		default:
			break;
		}
	return FALSE;
	}

struct IOInfo {
	MultiMapMod * mod;
	BitArray groups;
	};

//Win32 : static BOOL CALLBACK SaveTLDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) 
static INT_PTR CALLBACK SaveTLDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) 
{
	//Win32 : IOInfo * io_info = (IOInfo*)GetWindowLong(hWnd,GWL_USERDATA);
	IOInfo * io_info = DLGetWindowLongPtr<IOInfo*>(hWnd);

	switch (msg) {
	case WM_INITDIALOG: {
			io_info = (IOInfo*)lParam;

			//Win32 : SetWindowLong(hWnd,GWL_USERDATA,lParam);
			DLSetWindowLongPtr(hWnd, lParam);

			CenterWindow(hWnd, GetCOREInterface()->GetMAXHWnd());

			SendDlgItemMessage(hWnd,IDC_GROUPS,LB_RESETCONTENT,0,0);
			for ( int i=0; i<io_info->mod->uvwProy.Count(); i++ ) {
				TSTR name(io_info->mod->uvwProy[i]->descCanal);
				SendDlgItemMessage(	hWnd,IDC_GROUPS,LB_ADDSTRING,0,(LPARAM)(const TCHAR*)name  );
				}
			}
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDOK: {
					io_info->groups.ClearAll();
					for ( int i=0; i<io_info->mod->uvwProy.Count(); i++ ) {
						BOOL sel = SendDlgItemMessage(hWnd,IDC_GROUPS,LB_GETSEL,i,0);
						io_info->groups.Set(i,sel);
						}
					EndDialog(hWnd,1);
					}
					break;
				case IDCANCEL:
					EndDialog(hWnd,0);
					break;
				}
			break;

		case WM_CLOSE:
			EndDialog(hWnd,0);
			break;
		
		default:
			return FALSE;
		}
	return TRUE;
	}


//--- MultiMapMod methods ---------------------------------------------------

/*
static ParamBlockDesc2 texlay_param_blk ( texlay_params, _T("TextureLayers"),  0, &mapDesc, P_AUTO_CONSTRUCT + P_AUTO_UI, 0,
	//rollout
	IDD_TL_MAIN, IDS_PARAMETERS, 0, 0, NULL, 
	// params

	tlp_change,		_T("change"),	TYPE_BOOL, 	0,	IDS_SIMPLE,
		p_default, 	FALSE, 
		end, 

	end
	);
*/

// Este es el constructor
#pragma optimize( "", off )
MultiMapMod::MultiMapMod(BOOL create) {

//	mapDesc.MakeAutoParamBlocks(this);
//	assert(tl_pblock);

	in_modify_object = FALSE;

	updateCache = FALSE;
	pickingSpline = FALSE;
	selecting_faces = FALSE;
	preview = NO_PREVIEW;
	previewType = CUR_PREVIEW;

	show_all_gizmos = 0;
	compact_tverts = 0;
	save_uvw_channel = -1;
	
	show = SHOW_PROYS;
	isNurbs=FALSE;
	
	//if ( create ) 
		SetNumGroups(1);

	loadold = 0;

	tileDlg = NULL;
	radiusDlg = NULL;
	angleDlg = NULL;

	cut = 0;
	paste = 0;
	copy = 0;

	// Si estamos creando ponemos estos 3 flags (y CONTROL_OP por definicion)
	if (create) flags = CONTROL_CENTER|CONTROL_FIT|CONTROL_INIT;
	// Si estamos cargando? borramos todos los flags...
	else flags = 0;

	top_group = 0;
	current_channel = 0;
	inRender = FALSE;

	texlay_1_0 = FALSE;

	temp_v = NULL;
	temp_f = NULL;
	temp_num_v = 0;
	temp_num_f = 0;

	pelt_reset_layout = FALSE;
	
}
#pragma optimize( "", on )

MultiMapMod::~MultiMapMod() 
{
	for (int i=0; i<uvwProy.Count(); i++) 
	{
		if (uvwProy[i]) 
		{
			delete uvwProy[i];
			uvwProy[i] = NULL;
		}
	}
}

#define NEWMAP_CHUNKID	0x0100
#define VERSION_CHUNKID	0x0102
#define TEXLAY1_CHUNKID	0x0103
#define PREVIEW_CHUNKID	0x0105
#define PREVTYP_CHUNKID 0x010a
#define ALLGIZ_CHUNKID	0x010c
#define USEVCOL_CHUNKID	0x010e
#define CHANNUM_CHUNKID	0x0110
#define CHANACT_CHUNKID	0x0115
#define MAPTM_CHUNKID	0x0120
#define TIPO_CHUNKID	0x0140
#define SU_CHUNKID		0x0150
#define EU_CHUNKID		0x0160
#define SV_CHUNKID		0x0170
#define EV_CHUNKID		0x0180
#define SW_CHUNKID		0x0190
#define EW_CHUNKID		0x0200
#define ATTON_CHUNKID	0x0210
#define ATT_CHUNKID		0x0220
#define ASU_CHUNKID		0x0230
#define AEU_CHUNKID		0x0240
#define ASV_CHUNKID		0x0250
#define AEV_CHUNKID 	0x0260
#define ASW_CHUNKID 	0x0270
#define AEW_CHUNKID 	0x0280
#define LARGO_CHUNKID 	0x0300
#define ALTO_CHUNKID 	0x0310
#define ANCHO_CHUNKID 	0x0320
#define NORM_CHUNKID	0x0325
#define REVR_CHUNKID	0x032A
#define EJE_CHUNKID 	0x0330
#define NTYPE_CHUNKID	0x0335
#define FS_CHUNKID		0x033A
#define ARUV_CHUNKID	0x0340
#define TLUCRV_CHUNKID	0x0341
#define OFUCRV_CHUNKID	0x0342
#define TLWCRV_CHUNKID	0x0343
#define GMPTS_CHUNKID	0x0344
#define TOPOCH_CHUNKID	0x0345
#define CTVERTS_CHUNKID	0x0346

#define NAMES_CHUNKID	0x0350
#define NAMSEL_CHUNKID	0x2806
#define SPLNME_CHUNKID	0x4000

#define NAMEDSEL_STRING_CHUNK	0x2809
#define NAMEDSEL_ID_CHUNK		0x2810

IOResult MultiMapMod::LoadNamedSelChunk(ILoad *iload) {	
	IOResult res;
	DWORD ix=0;
	ULONG nb;

	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
		case NAMEDSEL_STRING_CHUNK: {
			TCHAR *name;
			res = iload->ReadWStringChunk(&name);
			TSTR *newName = new TSTR(name);
			named_face_sel.Append(1,&newName);				
			face_sel_ids.Append(1,&ix);
			ix++;
			break;
			}
		case NAMEDSEL_ID_CHUNK:
			iload->Read(&face_sel_ids[face_sel_ids.Count()-1],sizeof(DWORD), &nb);
			break;
		}
		iload->CloseChunk();
		if (res!=IO_OK) return res;
	}
	return IO_OK;
}

IOResult MultiMapMod::Load(ILoad *iload) {
	TCHAR buff[256];
	_stprintf(buff, _T(""));
	
	Modifier::Load(iload);

//	ClearAFlag(A_PLUGIN1);
	ULONG nb;
	int nc=0;
	int kc,i,j;
	int ca;

	int version = 0;

	Tab <int> intTab;
	Tab <float> fltTab;
	Tab <Matrix3> mt3Tab;

	IOResult res;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch (iload->CurChunkID()) {	
			case NAMSEL_CHUNKID:
				res = LoadNamedSelChunk(iload);
				break;
			case NEWMAP_CHUNKID:
			//	SetAFlag(A_PLUGIN1);
				break;
			case VERSION_CHUNKID:
				iload->Read(&version, sizeof(int), &nb);
				break;
			case TEXLAY1_CHUNKID:
				iload->Read(&texlay_1_0, sizeof(texlay_1_0), &nb);
				break;
			case PREVIEW_CHUNKID:
				iload->Read(&preview, sizeof(preview), &nb);
				break;
			case PREVTYP_CHUNKID:
				iload->Read(&previewType, sizeof(int), &nb);
				break;
			case USEVCOL_CHUNKID:
				break;
			case ALLGIZ_CHUNKID:
				iload->Read(&show_all_gizmos, sizeof(int), &nb);
				break;
			case CHANNUM_CHUNKID:
				iload->Read(&nc, sizeof(nc), &nb);
				SetNumGroups(nc);
				uvwProy.SetCount(nc);
				splNames.SetCount(nc);
				for (i=0; i<nc; i++) splNames[i] = new TSTR(buff);
				intTab.SetCount(nc);
				fltTab.SetCount(nc);
				mt3Tab.SetCount(nc);
				break;

			case CHANACT_CHUNKID:
				iload->Read(&ca, sizeof(ca), &nb);
				current_channel = ca;
				break;

			case MAPTM_CHUNKID:
				iload->Read(mt3Tab.Addr(0), sizeof(Matrix3)*nc, &nb);

				for (kc=0;kc<nc;kc++) {
					uvwProy[kc]->ReplaceReference( 1, NewDefaultMatrix3Controller() ); 
					SetXFormPacket pckt(mt3Tab[kc]);
					uvwProy[kc]->tmControl->SetValue(0,&pckt,TRUE,CTRL_RELATIVE);
					}
				break;

			case TIPO_CHUNKID:
				iload->Read(intTab.Addr(0), sizeof(int)*nc, &nb);
				loadold = 1;
				for (kc=0;kc<nc;kc++) {
					switch (intTab[kc]) {
						case MAP_PLANAR:		uvwProy[kc]->SetMappingType( MAP_TL_PLANAR );		break;
						case MAP_CYLINDRICAL:	uvwProy[kc]->SetMappingType( MAP_TL_CYLINDRICAL );	break;
						case MAP_BALL:			uvwProy[kc]->SetMappingType( MAP_TL_BALL );			break;
						case MAP_SPHERICAL:		uvwProy[kc]->SetMappingType( MAP_TL_SPHERICAL );	break;
						case 4:					uvwProy[kc]->SetMappingType( MAP_TL_SPLINE );		break;
						case 5:					uvwProy[kc]->SetMappingType( MAP_TL_FFM );			break;
						default:				uvwProy[kc]->SetMappingType( MAP_TL_PLANAR );		break;
						}
					}
				break;
			case SU_CHUNKID:
				iload->Read(fltTab.Addr(0), sizeof(float)*nc, &nb);
				for (kc=0;kc<nc;kc++) uvwProy[kc]->pblock->SetValue(uvw_tile_u,0,fltTab[kc]);
				break;
			case EU_CHUNKID:
				iload->Read(fltTab.Addr(0), sizeof(float)*nc, &nb);
				for (kc=0;kc<nc;kc++) uvwProy[kc]->pblock->SetValue(uvw_offset_u,0,fltTab[kc]);
				break;
			case SV_CHUNKID:
				iload->Read(fltTab.Addr(0), sizeof(float)*nc, &nb);
				for (kc=0;kc<nc;kc++) uvwProy[kc]->pblock->SetValue(uvw_tile_v,0,fltTab[kc]);
				break;
			case EV_CHUNKID:
				iload->Read(fltTab.Addr(0), sizeof(float)*nc, &nb);
				for (kc=0;kc<nc;kc++) uvwProy[kc]->pblock->SetValue(uvw_offset_v,0,fltTab[kc]);
				break;
			case SW_CHUNKID:
				iload->Read(fltTab.Addr(0), sizeof(float)*nc, &nb);
				for (kc=0;kc<nc;kc++) uvwProy[kc]->pblock->SetValue(uvw_tile_w,0,fltTab[kc]);
				break;
			case EW_CHUNKID:
				iload->Read(fltTab.Addr(0), sizeof(float)*nc, &nb);
				for (kc=0;kc<nc;kc++) uvwProy[kc]->pblock->SetValue(uvw_offset_w,0,fltTab[kc]);
				break;
			case ATTON_CHUNKID:
				iload->Read(intTab.Addr(0), sizeof(float)*nc, &nb);
				for (kc=0;kc<nc;kc++) uvwProy[kc]->pblock->SetValue(uvw_atton,0,intTab[kc]);
				break;
			case ATT_CHUNKID:
				iload->Read(fltTab.Addr(0), sizeof(float)*nc, &nb);
				for (kc=0;kc<nc;kc++) uvwProy[kc]->pblock->SetValue(uvw_att,0,fltTab[kc]);
				break;
			case ASU_CHUNKID:
				iload->Read(fltTab.Addr(0), sizeof(float)*nc, &nb);
				for (kc=0;kc<nc;kc++) uvwProy[kc]->pblock->SetValue(uvw_aus,0,fltTab[kc]);
				break;
			case AEU_CHUNKID:
				iload->Read(fltTab.Addr(0), sizeof(float)*nc, &nb);
				for (kc=0;kc<nc;kc++) uvwProy[kc]->pblock->SetValue(PB_AUE,0,fltTab[kc]);
				break;
			case ASV_CHUNKID:
				iload->Read(fltTab.Addr(0), sizeof(float)*nc, &nb);
				for (kc=0;kc<nc;kc++) uvwProy[kc]->pblock->SetValue(PB_AVS,0,fltTab[kc]);
				break;
			case AEV_CHUNKID:
				iload->Read(fltTab.Addr(0), sizeof(float)*nc, &nb);
				for (kc=0;kc<nc;kc++) uvwProy[kc]->pblock->SetValue(PB_AVE,0,fltTab[kc]);
				break;
			case ASW_CHUNKID:
				iload->Read(fltTab.Addr(0), sizeof(float)*nc, &nb);
				for (kc=0;kc<nc;kc++) uvwProy[kc]->pblock->SetValue(PB_AWS,0,fltTab[kc]);
				break;
			case AEW_CHUNKID:
				iload->Read(fltTab.Addr(0), sizeof(float)*nc, &nb);
				for (kc=0;kc<nc;kc++) uvwProy[kc]->pblock->SetValue(PB_AWE,0,fltTab[kc]);
				break;

			case LARGO_CHUNKID:
				iload->Read(fltTab.Addr(0), sizeof(float)*nc, &nb);
				for (kc=0;kc<nc;kc++) {
					uvwProy[kc]->flags = 0;
					uvwProy[kc]->pblock->SetValue(uvw_length,0,fltTab[kc]);
					}
				break;
			case ALTO_CHUNKID:
				iload->Read(fltTab.Addr(0), sizeof(float)*nc, &nb);
				for (kc=0;kc<nc;kc++) {
					uvwProy[kc]->flags = 0;
					uvwProy[kc]->pblock->SetValue(uvw_height,0,fltTab[kc]);
					}
				break;
			case ANCHO_CHUNKID:
				iload->Read(fltTab.Addr(0), sizeof(float)*nc, &nb);
				for (kc=0;kc<nc;kc++) {
					uvwProy[kc]->flags = 0;
					uvwProy[kc]->pblock->SetValue(uvw_width,0,fltTab[kc]);
					}
				break;

			case NORM_CHUNKID:
				iload->Read(intTab.Addr(0), sizeof(int)*nc, &nb);
				for (kc=0;kc<nc;kc++) uvwProy[kc]->pblock->SetValue(PB_NORMALIZE,0,intTab[kc]);
				break;
			case REVR_CHUNKID:
				iload->Read(intTab.Addr(0), sizeof(int)*nc, &nb);
				for (kc=0;kc<nc;kc++) uvwProy[kc]->pblock->SetValue(PB_REVERSE,0,intTab[kc]);
				break;
			case EJE_CHUNKID:
				iload->Read(intTab.Addr(0), sizeof(int)*nc, &nb);
				for (kc=0;kc<nc;kc++) uvwProy[kc]->pblock->SetValue(PB_AXIS,0,intTab[kc]);
				break;
			case NTYPE_CHUNKID:
				iload->Read(intTab.Addr(0), sizeof(int)*nc, &nb);
				for (kc=0;kc<nc;kc++) uvwProy[kc]->pblock->SetValue(PB_NORMALS,0,intTab[kc]);
				break;
			case FS_CHUNKID:
				iload->Read(fltTab.Addr(0), sizeof(float)*nc, &nb);
				for (kc=0;kc<nc;kc++) uvwProy[kc]->pblock->SetValue(PB_FS,0,fltTab[kc]);
				break;
			case ARUV_CHUNKID:
				iload->Read(fltTab.Addr(0), sizeof(float)*nc, &nb);
				for (kc=0;kc<nc;kc++) uvwProy[kc]->pblock->SetValue(PB_RUV,0,fltTab[kc]);
				break;
			case TOPOCH_CHUNKID:
				break;
			case CTVERTS_CHUNKID:
				iload->Read(&compact_tverts, sizeof(compact_tverts), &nb);
				break;
			case TLUCRV_CHUNKID:
				for (i=0; i<nc; i++) {
					int tCNp;
					iload->Read(&tCNp,sizeof(int),&nb);
					uvwProy[i]->tileCrv.SetNumPoints(tCNp);
					for(j=0;j<tCNp;j++) {
						Point2 ip,p,op;
						int knd;
						iload->Read(&knd,sizeof(int),&nb);
						iload->Read(&ip,sizeof(Point2),&nb);							
						iload->Read(&p, sizeof(Point2),&nb);
						iload->Read(&op,sizeof(Point2),&nb);
						uvwProy[i]->tileCrv.SetKind(j,knd);
						uvwProy[i]->tileCrv.SetIn(j,ip);
						uvwProy[i]->tileCrv.SetPoint(j,p);
						uvwProy[i]->tileCrv.SetOut(j,op);
						}
					}
				break;
			case OFUCRV_CHUNKID:
				for (i=0; i<nc; i++) {
					int angCNp;
					iload->Read(&angCNp,sizeof(int),&nb);
					uvwProy[i]->angleCrv.SetNumPoints(angCNp);
					for(j=0;j<angCNp;j++) {
						Point2 ip,p,op;
						int knd;
						iload->Read(&knd,sizeof(int),&nb);
						iload->Read(&ip,sizeof(Point2),&nb);							
						iload->Read(&p, sizeof(Point2),&nb);
						iload->Read(&op,sizeof(Point2),&nb);
						uvwProy[i]->angleCrv.SetKind(j,knd);
						uvwProy[i]->angleCrv.SetIn(j,ip);
						uvwProy[i]->angleCrv.SetPoint(j,p);
						uvwProy[i]->angleCrv.SetOut(j,op);
						}
					}
				break;
			case TLWCRV_CHUNKID:
				for (i=0; i<nc; i++) {
					int rCNp;
					iload->Read(&rCNp,sizeof(int),&nb);
					uvwProy[i]->wTileCrv.SetNumPoints(rCNp);
					for(j=0;j<rCNp;j++) {
						Point2 ip,p,op;
						int knd;
						iload->Read(&knd,sizeof(int),&nb);
						iload->Read(&ip,sizeof(Point2),&nb);							
						iload->Read(&p, sizeof(Point2),&nb);
						iload->Read(&op,sizeof(Point2),&nb);
						uvwProy[i]->wTileCrv.SetKind(j,knd);
						uvwProy[i]->wTileCrv.SetIn(j,ip);
						uvwProy[i]->wTileCrv.SetPoint(j,p);
						uvwProy[i]->wTileCrv.SetOut(j,op);
						}
					}
				break;
			case GMPTS_CHUNKID:
				for (i=0; i<nc; i++) {
					int gmNv,gmnx,gmny;
					iload->Read(&gmNv,sizeof(int),&nb);
					iload->Read(&gmnx,sizeof(int),&nb);
					iload->Read(&gmny,sizeof(int),&nb);
					uvwProy[i]->gm->nx = gmnx;
					uvwProy[i]->gm->ny = gmny;
					uvwProy[i]->gm->SetNumVerts(gmNv);
					for (j=0; j<gmNv; j++) {
						Point3 vert;
						iload->Read(&vert,sizeof(Point3),&nb);
						uvwProy[i]->gm->verts[j] = vert;
						}
					uvwProy[i]->BuildGMGrid();
					}
				break;
			}

		if (iload->CurChunkID() >= NAMES_CHUNKID && iload->CurChunkID() < 0x2000) {
			TCHAR *buf;
			res=iload->ReadCStringChunk(&buf);
			uvwProy[iload->CurChunkID()-NAMES_CHUNKID]->descCanal = TSTR(buf);
			}

		if (iload->CurChunkID() >= SPLNME_CHUNKID) {
			TCHAR *buf;
			res=iload->ReadCStringChunk(&buf);
			*splNames[iload->CurChunkID()-SPLNME_CHUNKID] = TSTR(buf);
			}

		iload->CloseChunk();
	if (res!=IO_OK)  return res;
		}
    
	MMMLoadCB* mmmcb = new MMMLoadCB( this, version );
    iload->RegisterPostLoadCallback(mmmcb);

	pelt_reset_layout = TRUE;

	return IO_OK;
	}

IOResult MultiMapMod::Save(ISave *isave) {
	Modifier::Save(isave);
	ULONG nb;

	int version = TEXLAY_VERSION;
	int canalActual = current_channel;

	// Averiguamos cuantos canales tenemos...
	int nc = uvwProy.Count();
	int ca = current_channel;

	if (1) { //TestAFlag(A_PLUGIN1)) {
		isave->BeginChunk(NEWMAP_CHUNKID);
		isave->EndChunk();

		isave->BeginChunk(VERSION_CHUNKID);
		isave->Write(&version,sizeof(int),&nb);
		isave->EndChunk();

		isave->BeginChunk(TEXLAY1_CHUNKID);
		isave->Write(&texlay_1_0,sizeof(texlay_1_0),&nb);
		isave->EndChunk();

		isave->BeginChunk(PREVIEW_CHUNKID);
		isave->Write(&preview,sizeof(preview),&nb);
		isave->EndChunk();

		isave->BeginChunk(PREVTYP_CHUNKID);
		isave->Write(&previewType,sizeof(previewType),&nb);
		isave->EndChunk();

		isave->BeginChunk(CHANNUM_CHUNKID);
		isave->Write(&nc,sizeof(nc),&nb);
		isave->EndChunk();

		isave->BeginChunk(CHANACT_CHUNKID);
		isave->Write(&canalActual,sizeof(canalActual),&nb);
		isave->EndChunk();

		isave->BeginChunk(CTVERTS_CHUNKID);
		isave->Write(&compact_tverts,sizeof(compact_tverts),&nb);
		isave->EndChunk();

		if (named_face_sel.Count()) {
			isave->BeginChunk(NAMSEL_CHUNKID);			
			for (int i=0; i<named_face_sel.Count(); i++) {
				isave->BeginChunk(NAMEDSEL_STRING_CHUNK);
				isave->WriteWString(*named_face_sel[i]);
				isave->EndChunk();

				isave->BeginChunk(NAMEDSEL_ID_CHUNK);
				isave->Write(&face_sel_ids[i],sizeof(DWORD),&nb);
				isave->EndChunk();
				}
			isave->EndChunk();
			}

		isave->BeginChunk(ALLGIZ_CHUNKID);
		isave->Write(&show_all_gizmos,sizeof(int),&nb);
		isave->EndChunk();


		}
	
	return IO_OK;
	}

#define NUMSETS_CHUNKID			0x0100
#define NUMFACES_CHUNKID		0x0150
#define NUM_EDGES_CHUNKID		0x0160
#define FACESSEL_CHUNKID		0x0200
#define FSELSET_CHUNKID			0x0250
#define TRIINFO_CHUNKID			0x0300
#define NUM_EDGE_SETS_CHUNKID	0x0400
#define EDGES_SEL_CHUNKID		0x0410
#define EDGES_SEL_SET_CHUNKID	0x0420
#define NUM_GROUPS_CHUNKID		0x0500
#define DATA_NUM_VERTS_CHUNKID	0x0510
#define DATA_VERTS_CHUNKID		0x0520
#define DATA_NUM_FACES_CHUNKID	0x0530
#define DATA_FACES_CHUNKID		0x0540

IOResult MultiMapMod::SaveLocalData(ISave *isave, LocalModData *ld) {
	TexLayMCData *mtd = (TexLayMCData *)ld;
	ULONG nb;
	BitArray set;

	int num_face_sels  = mtd->face_sel.sets.Count();
	int num_faces = mtd->face_sel[0].GetSize();

	int num_edge_sels = mtd->edge_sel.sets.Count();
	int num_edges = mtd->edge_sel[0].GetSize();

	int num_groups = mtd->group_uvw_data.Count();
	int i_g, i_f;

	isave->BeginChunk(NUMSETS_CHUNKID);
	isave->Write(&num_face_sels,sizeof(num_face_sels),&nb);
	isave->EndChunk();

	isave->BeginChunk(NUMFACES_CHUNKID);
	isave->Write(&num_faces,sizeof(num_faces),&nb);
	isave->EndChunk();

	isave->BeginChunk(NUM_EDGES_CHUNKID);
	isave->Write(&num_edges,sizeof(num_edges),&nb);
	isave->EndChunk();

	if (mtd->face_sel.Count()) {
		isave->BeginChunk(FACESSEL_CHUNKID);
		mtd->face_sel.Save(isave);
		isave->EndChunk();
		}

	if (mtd->face_sel_sets.Count()) {
		isave->BeginChunk(FSELSET_CHUNKID);
		mtd->face_sel_sets.Save(isave);
		isave->EndChunk();
		}

	isave->BeginChunk(TRIINFO_CHUNKID);
	mtd->triInfo.Save(isave);
	isave->EndChunk();

	isave->BeginChunk(NUM_EDGE_SETS_CHUNKID);
	isave->Write(&num_edge_sels,sizeof(num_edge_sels),&nb);
	isave->EndChunk();

	if (mtd->edge_sel.Count()) {
		isave->BeginChunk(EDGES_SEL_CHUNKID);
		mtd->edge_sel.Save(isave);
		isave->EndChunk();
		}

	if (mtd->edge_sel_sets.Count()) {
		isave->BeginChunk(EDGES_SEL_SET_CHUNKID);
		mtd->edge_sel_sets.Save(isave);
		isave->EndChunk();
		}

	isave->BeginChunk(NUM_GROUPS_CHUNKID);
	isave->Write(&num_groups,sizeof(num_groups),&nb);
	isave->EndChunk();

	isave->BeginChunk(DATA_NUM_VERTS_CHUNKID);
	for ( i_g=0; i_g<num_groups; i_g++ ) {
		int num_verts = mtd->group_uvw_data[i_g]->num_v;
		isave->Write(&num_verts,sizeof(num_verts),&nb);
		}
	isave->EndChunk();

	isave->BeginChunk(DATA_VERTS_CHUNKID);
	for ( i_g=0; i_g<num_groups; i_g++ ) {
		int num_verts = mtd->group_uvw_data[i_g]->num_v;
		isave->Write(mtd->group_uvw_data[i_g]->v,sizeof(Point3)*num_verts,&nb);
		}
	isave->EndChunk();

	isave->BeginChunk(DATA_NUM_FACES_CHUNKID);
	for ( i_g=0; i_g<num_groups; i_g++ ) {
		int num_faces = mtd->group_uvw_data[i_g]->num_f;
		isave->Write(&num_faces,sizeof(num_faces),&nb);
		}
	isave->EndChunk();

	isave->BeginChunk(DATA_FACES_CHUNKID);
	for ( i_g=0; i_g<num_groups; i_g++ ) {
		int num_faces = mtd->group_uvw_data[i_g]->num_f;
		for ( i_f=0; i_f<num_faces; i_f++ ) {
			int deg = mtd->group_uvw_data[i_g]->f[i_f].deg;
			int *vtx = mtd->group_uvw_data[i_g]->f[i_f].vtx;
			isave->Write(&deg,sizeof(deg),&nb);
			isave->Write(vtx,sizeof(int)*deg,&nb);
			}
		}
	isave->EndChunk();

	return IO_OK;
	}

IOResult MultiMapMod::LoadLocalData(ILoad *iload, LocalModData **pld) {
	TexLayMCData *mtd = new TexLayMCData;
	*pld = mtd;
	ULONG nb;
	IOResult res;	
	int num_face_sels = 0;
	int num_faces = 0;
	int num_edge_sels = 0;
	int num_edges = 0;
	BitArray set;
	updateCache = TRUE;
	
	// JW Code Change : fix uninitialized locals warning
	int num_groups=0;
	int i_g, i_f;

	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case NUMSETS_CHUNKID:
				iload->Read(&num_face_sels, sizeof(num_face_sels), &nb);
				break;

			case NUMFACES_CHUNKID:
				iload->Read(&num_faces, sizeof(num_faces), &nb);
				break;

			case NUM_EDGES_CHUNKID:
				iload->Read(&num_edges, sizeof(num_edges), &nb);
				break;

			case FACESSEL_CHUNKID:
				mtd->face_sel.sets.SetCount(0);
				mtd->face_sel.ids.SetCount(0);
				mtd->face_sel.names.SetCount(0);
				res = mtd->face_sel.Load(iload);
				break;

			case FSELSET_CHUNKID:
				res = mtd->face_sel_sets.Load(iload);
				break;

			case TRIINFO_CHUNKID:
				mtd->triInfo.Load(iload);
				break;

			case NUM_EDGE_SETS_CHUNKID:
				iload->Read(&num_edge_sels, sizeof(num_edge_sels), &nb);
				break;

			case EDGES_SEL_CHUNKID:
				mtd->edge_sel.sets.SetCount(0);
				mtd->edge_sel.ids.SetCount(0);
				mtd->edge_sel.names.SetCount(0);
				res = mtd->edge_sel.Load(iload);
				break;

			case EDGES_SEL_SET_CHUNKID:
				res = mtd->edge_sel_sets.Load(iload);
				break;

			case NUM_GROUPS_CHUNKID:
				iload->Read(&num_groups, sizeof(num_groups), &nb);
				mtd->group_uvw_data.SetCount(num_groups);
				for ( i_g=0; i_g<num_groups; i_g++ )
					mtd->group_uvw_data[i_g] = new PolyUVWData;
				break;

			case DATA_NUM_VERTS_CHUNKID:
				for ( i_g=0; i_g<num_groups; i_g++ ) {
					int num_verts;
					iload->Read(&num_verts,sizeof(num_verts),&nb);
					mtd->group_uvw_data[i_g]->num_v = num_verts;
					mtd->group_uvw_data[i_g]->v = new Point3[num_verts];
					}
				break;

			case DATA_VERTS_CHUNKID:
				for ( i_g=0; i_g<num_groups; i_g++ ) {
					int num_verts = mtd->group_uvw_data[i_g]->num_v;
					iload->Read(mtd->group_uvw_data[i_g]->v,sizeof(Point3)*num_verts,&nb);
					}
				break;

			case DATA_NUM_FACES_CHUNKID:
				for ( i_g=0; i_g<num_groups; i_g++ ) {
					int num_faces;
					iload->Read(&num_faces,sizeof(num_faces),&nb);
					mtd->group_uvw_data[i_g]->num_f = num_faces;
					mtd->group_uvw_data[i_g]->f = new PolyFace[num_faces];
					}
				break;

			case DATA_FACES_CHUNKID:
				for ( i_g=0; i_g<num_groups; i_g++ ) {
					int num_faces = mtd->group_uvw_data[i_g]->num_f;
					for ( i_f=0; i_f<num_faces; i_f++ ) {
						int deg;
						iload->Read(&deg,sizeof(deg),&nb);
						mtd->group_uvw_data[i_g]->f[i_f].deg = deg;
						mtd->group_uvw_data[i_g]->f[i_f].vtx = new int[deg];
						iload->Read(mtd->group_uvw_data[i_g]->f[i_f].vtx,sizeof(int)*deg,&nb);
						}
					}
				break;

			}
		iload->CloseChunk();
		if (res!=IO_OK) {
			return res;
			}
		}
	if (loadold) {
		delete *pld;
		*pld = NULL;
		}
				
	mtd->SetNumGroups(num_face_sels,num_faces,num_edges);

	return IO_OK;
	}

void MultiMapMod::ChannelChange(int newcan) {
	if (newcan >= uvwProy.Count()) return;
	current_channel = newcan;
	LocalDataChanged();
	}

void MultiMapMod::CopyLayerToBuffer() {
	if (uvw_buffer) {
		uvw_buffer->DeleteThis();
		uvw_buffer = NULL;
		}

	ModContextList mcList;
	INodeTab nodes;
	ip->GetModContexts(mcList,nodes);
	TexLayMCData *md = (TexLayMCData*) mcList[0]->localData;

	face_buffer = md->face_sel[current_channel];
	edge_buffer = md->edge_sel[current_channel];

	if ( !uvw_data_buffer )
		uvw_data_buffer = new PolyUVWData;

	uvw_data_buffer->CopyUVWData( md->group_uvw_data[current_channel] );

	uvw_buffer = (UVWProyector*)uvwProy[current_channel]->Clone();

	copied = current_channel;
	copy = 1;
	cut = 0;
	}

//No olvidar poner el undo
BOOL MultiMapMod::PasteBufferToLayer() {
	if (!uvw_buffer) 
		return FALSE;

	ModContextList mcList;
	INodeTab nodes;
	ip->GetModContexts(mcList,nodes);
	TexLayMCData *md = (TexLayMCData*) mcList[0]->localData;

	int face_sel_size = md->face_sel[current_channel].GetSize();
	int face_buffer_size = face_buffer.GetSize();
	int face_size = face_sel_size<face_buffer_size?face_sel_size:face_buffer_size;

	md->face_sel[current_channel].ClearAll();
	for ( int i_f=0; i_f<face_size; i_f++ ) {
		md->face_sel[current_channel].Set( i_f, face_buffer[i_f] );
		}

	int edge_sel_size = md->edge_sel[current_channel].GetSize();
	int edge_buffer_size = edge_buffer.GetSize();
	int edge_size = edge_sel_size<edge_buffer_size?edge_sel_size:edge_buffer_size;

	md->edge_sel[current_channel].ClearAll();
	for ( int i_e=0; i_e<edge_size; i_e++ ) {
		md->edge_sel[current_channel].Set( i_e, edge_buffer[i_e] );
		}

	theHold.Begin();
	theHold.Put( new GroupsTableRestore(this,START_EDIT) );
	theHold.Put(new GroupsTableRestore(this,PASTE_GROUP));

	uvwProy[current_channel] = (UVWProyector*)uvw_buffer->Clone();
	ReplaceReference( current_channel, uvwProy[current_channel] );

	if ( md->group_uvw_data[current_channel]->CompareFaceSize( uvw_data_buffer ) ) {
		md->group_uvw_data[current_channel]->CopyUVWData( uvw_data_buffer );
		}

	theHold.Put( new GroupsTableRestore(this,END_EDIT) );
	theHold.Accept(_T("Paste TexLay Group"));

	UpdateUIAll();
	GroupsMenuForceDrawItem(current_channel);
	
	paste = 1;
	return TRUE;
	}


void MultiMapMod::BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev)
{
	for ( int i=0; i<uvwProy.Count(); i++ )
		uvwProy[i]->tl = this;

	this->ip = ip;
	editMod  = this;

//#ifdef ALPS_PROTECTED
//
//	if ( auth_ok == false ) 
//	{
//		CommandMode * cm = ip->GetCommandMode();
//		ip->SetStdCommandMode(CID_OBJSELECT);
//		ip->SetCommandMode(cm);
//		return;
//	}
//
//#endif

	tileDlg = NULL;
	radiusDlg = NULL;
	angleDlg = NULL;

	cut = 0;
	paste = 0;

#if MAX_VERSION_MAJOR < 17
	// JW: has been deprecated in 3ds Max 2015 and up - SubObjectSelection always is enabled and can't be turned off
	ip->EnableSubObjectSelection(TRUE);
#endif

	if (isNurbs) {
		hIsNurbs = ip->AddRollupPage( hInstance, MAKEINTRESOURCE(IDD_NURBS), TexMagicNurbsProc, _T("NURBS"), (LPARAM)this);
		ip->RegisterDlgWnd( hIsNurbs );
		}

	CreateUI();

 	TimeValue t = ip->GetTime();
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_BEGIN_EDIT);
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_MOD_DISPLAY_ON);
	SetAFlag(A_MOD_BEING_EDITED);

	align_rotation = 0;
	}


void MultiMapMod::EndEditParams(IObjParam *ip, ULONG flags,Animatable *next)
{

//#ifdef ALPS_PROTECTED
//
//	if(!auth_ok) {
//		this->ip = NULL;
//		editMod  = NULL;
//		return;
//		}
//
//#endif

	ip->UnRegisterTimeChangeCallback(this);

	if(hIsNurbs) 
	{
		ip->UnRegisterDlgWnd( hIsNurbs );
		ip->DeleteRollupPage( hIsNurbs );
		hIsNurbs = NULL;
	}

	ip->ClearPickMode();

	pickAcquire.mod = NULL;
	pickAcquire.ip  = NULL;	

	//delete pickMode;
	//pickMode = NULL;

	ip->DeleteMode(moveMode);
	ip->DeleteMode(rotMode);
	ip->DeleteMode(uscaleMode);
	ip->DeleteMode(nuscaleMode);
	ip->DeleteMode(squashMode);	
	ip->DeleteMode(selectMode);
	ip->DeleteMode(faceAlignMode);
	ip->DeleteMode(regionFitMode);

	if (moveMode) delete moveMode; 
	moveMode = NULL;
	if (rotMode) delete rotMode; 
	rotMode = NULL;
	if (uscaleMode) delete uscaleMode; 
	uscaleMode = NULL;
	if (nuscaleMode) delete nuscaleMode; 
	nuscaleMode = NULL;
	if (squashMode) delete squashMode; 
	squashMode = NULL;
	if (selectMode) delete selectMode;
	selectMode = NULL;
	if (faceAlignMode) delete faceAlignMode; 
	faceAlignMode = NULL;
	if (regionFitMode) delete regionFitMode; 
	regionFitMode = NULL;

	top_group = (int)SendDlgItemMessage( hwnd_main,IDC_TL_GROUPS_MENU,LB_GETTOPINDEX,0,0);

	CloseAllRollups();

	TimeValue t = ip->GetTime();
 	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_END_EDIT);
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_MOD_DISPLAY_OFF);
	ClearAFlag(A_MOD_BEING_EDITED);

	CloseCurveDlgs();

	if ( uv_pelt_dlg->hWnd )
		DestroyWindow(uv_pelt_dlg->hWnd);

	this->ip = NULL;
	editMod  = NULL;
}

void MultiMapMod::TimeChanged(TimeValue t) {
	if (ip)
		UpdateUIAll();
	}


void MultiMapMod::CloseCurveDlgs() {
	if (tileDlg){
		DestroyWindow(tileDlg->hMain);
		tileDlg = NULL;
		}
	if (radiusDlg){
		DestroyWindow(radiusDlg->hMain);
		radiusDlg = NULL;
		}
	if (angleDlg) {
		DestroyWindow(angleDlg->hMain);
		angleDlg = NULL;
		}
	}


void MultiMapMod::SetNodeName() {
	if (!ip) return;
	if (uvwProy[current_channel]->GetMappingType() == MAP_TL_SPLINE) {
		uvwProy[current_channel]->GetSplineNode();
		if (uvwProy[current_channel]->spline_node) {
//			SetWindowText(GetDlgItem(hwnd_main, IDC_PICK_MM_NODE), 
//				uvwProy[current_channel]->spline_node->GetName());
		} else {
//			SetWindowText(GetDlgItem(hwnd_main, IDC_PICK_MM_NODE), 
//				GetString(IDS_MTL_PICKSPLINE));
			}
		} 
	}

void MultiMapMod::PanelVisibility(int pvflags) {
	}

void MultiMapMod::EnterNormalAlign()
	{
	SendMessage(GetDlgItem(hwnd_main,IDC_MAP_NORMALALIGN),CC_COMMAND,CC_CMD_SET_STATE,1);
	}

void MultiMapMod::ExitNormalAlign()
	{
	SendMessage(GetDlgItem(hwnd_main,IDC_MAP_NORMALALIGN),CC_COMMAND,CC_CMD_SET_STATE,0);
	}

void MultiMapMod::EnterRegionFit()
	{
	SendMessage(GetDlgItem(hwnd_main,IDC_MAP_FITREGION),CC_COMMAND,CC_CMD_SET_STATE,1);
	}

void MultiMapMod::ExitRegionFit()
	{
	SendMessage(GetDlgItem(hwnd_main,IDC_MAP_FITREGION),CC_COMMAND,CC_CMD_SET_STATE,0);
	}

void MultiMapMod::EnterAcquire()
	{
	SendMessage(GetDlgItem(hwnd_main,IDC_MAP_ACQUIRE),CC_COMMAND,CC_CMD_SET_STATE,1);
	}

void MultiMapMod::ExitAcquire()
	{
	SendMessage(GetDlgItem(hwnd_main,IDC_MAP_ACQUIRE),CC_COMMAND,CC_CMD_SET_STATE,0);
	}

// Este metodo intersecta todos los intervalos de validez de
// los diferentes pblock para asi dar el intervalo de validez del
// modifier...
// Como nosotros no somos animables, podriamos devolver FOREVER...
Interval MultiMapMod::LocalValidity(TimeValue t)
	{	
	Interval valid = FOREVER;
	for (int ch=0; ch<uvwProy.Count(); ch++) {
		Interval uvIv = uvwProy[ch]->LocalValidity(t);
		valid = valid&uvIv;
		}
	return valid;
	}

// TODO:
// Implementar clone perfectamente... 
// Este es el que trabaja en el copy/paste del modifier stack????
// TLTODO:  
RefTargetHandle MultiMapMod::Clone(RemapDir& remap) {
	MultiMapMod* newmod = new MultiMapMod(FALSE);
	newmod->SetNumGroups(uvwProy.Count());
	newmod->current_channel = current_channel;
	for (int i=0; i<uvwProy.Count(); i++) {
		newmod->ReplaceReference(i,uvwProy[i]->Clone(remap)); // 1+i tl_pblock
		}
	return newmod;
	}

RefTargetHandle MultiMapMod::GetReference(int i) {
	return uvwProy[i];
	return NULL;
	}

void MultiMapMod::SetReference(int i, RefTargetHandle rtarg) {
	uvwProy[i] = (UVWProyector*)rtarg;
	}

// JW Code Change: NotifyRefChanged signature changed in 3ds Max 2015+
#if MAX_VERSION_MAJOR < 17
RefResult MultiMapMod::NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message) 
#else
RefResult MultiMapMod::NotifyRefChanged( const Interval& changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message, BOOL propagate )
#endif
{
	TimeValue t = GetCOREInterface()->GetTime();
	if ( in_modify_object ) 
		return REF_STOP;
	return REF_SUCCEED;
	}

int MultiMapMod::GetFirstParamVer() {
	return 3;
	}

int MultiMapMod::GetPBlockVersion() {
	return uvwProy[current_channel]->pblock->GetVersion();
	}


// Track View - References Methods

Animatable* MultiMapMod::SubAnim(int i)
	{
	return uvwProy[i];
	}
#if MAX_VERSION_MAJOR < 24
TSTR MultiMapMod::SubAnimName(int i)
#else
TSTR MultiMapMod::SubAnimName(int i, bool localized )
#endif
	{
	if (i >= uvwProy.Count()) return _T("");

	TCHAR buf[256];
	_stprintf(buf, _T("%s"), uvwProy[i]->descCanal );
	return TSTR(buf);
	}

int MultiMapMod::SubNumToRefNum(int subNum)
	{
	return subNum;
	}

#if (MAX_RELEASE >= 4000)
// r4. SubObjects stuff

int MultiMapMod::NumSubObjTypes() {
	return 4;
	}

ISubObjType *MultiMapMod::GetSubObjType(int i) {
	static bool initialized = false;

	if(!initialized) {
		initialized = true;
		SOT_Gizmo.SetName( GetString(IDS_DC_APPARATUS) );
		SOT_Points.SetName( GetString(IDS_DC_CONTPOINTS) );
		SOT_Faces.SetName( GetString(IDS_DC_SELFACES) );
		SOT_Edges.SetName( GetString(IDS_DC_SELEDGES) );
	}

	switch(i) {
		case 0:		return &SOT_Gizmo;
		case 1:		return &SOT_Points;
		case 2:		return &SOT_Faces;
		case 3:		return &SOT_Edges;
	}
	return NULL;
}
#endif
	
void MultiMapMod::ActivateSubobjSel(int level, XFormModes& modes )
{	
//#ifdef ALPS_PROTECTED
//	if ( auth_ok == false )	level = SEL_OBJECT;
//#endif

	switch (level) {
		case SEL_OBJECT:
			ip->DeleteMode(moveMode);
			ip->DeleteMode(rotMode);
			ip->DeleteMode(uscaleMode);
			ip->DeleteMode(nuscaleMode);
			ip->DeleteMode(squashMode);
			ip->DeleteMode(selectMode);		
			break;
		case SEL_GIZMO: // Modifier box
			modes = XFormModes(moveMode,rotMode,nuscaleMode,uscaleMode,squashMode,NULL);
			break;
		case SEL_POINTS:
			modes = XFormModes(moveMode,rotMode,nuscaleMode,uscaleMode,squashMode,selectMode);
			break;
		case SEL_FACES:
			SetupNamedSelDropDown();
			modes = XFormModes(NULL,NULL,NULL,NULL,NULL,selectMode);
		case SEL_EDGES:
			SetupNamedSelDropDown();
			modes = XFormModes(NULL,NULL,NULL,NULL,NULL,selectMode);
		}
	NotifyDependents(FOREVER,PART_DISPLAY,REFMSG_CHANGE);
	}

//	-- GetAttenuation ---------------------------------------------
float MultiMapMod::GetAtt(Point3 p, int group) {
	if (group>=uvwProy.Count())
		return 0.0f;

	if (!uvwProy[group]->attOn) 
		return 1.0f;

	return uvwProy[group]->GetAtt(p);
	}


// TODO: Que los canales nuevos tengan por valor el de el current_channel actual???
void MultiMapMod::SetNumGroups(int n) {
	TCHAR buf[256];
	int ct = uvwProy.Count();

	if ( n==ct )
		return;

	if (n!=ct) {
		if (n<ct) {
			for (int i=n; i<ct; i++) {
				uvwProy[i]->DeleteAllRefsToMe();
				uvwProy[i]->DeleteAllRefsFromMe();
				delete uvwProy[i];
				uvwProy[i] = NULL;
				}
			uvwProy.SetCount(n);
			if ( current_channel >= uvwProy.Count() )
				current_channel = uvwProy.Count()-1;
			}
		if (n>ct) {
			uvwProy.SetCount(n);
			for (int i=ct; i<uvwProy.Count(); i++) {
				uvwProy[i] = NULL;
				UVWProyector * uvwproy = new UVWProyector(TRUE);
				ReplaceReference( i, uvwproy );

				_stprintf(buf, _T("Group %d"), i+1);
				uvwProy[i]->descCanal = TSTR(buf);

				CopyPBlock(i,0);
				}
			}		
		}

	for ( int i=0; i<uvwProy.Count(); i++ )
		uvwProy[i]->tl = this;

	LocalDataChanged();
	}

//---------------------------------------------------------------------------------
// Name : GetModNodes
//
// Desc : Returns all the modifers and modcontexts used by a Node
//---------------------------------------------------------------------------------
void GetModNodes(INode *root, Modifier *fmod, Tab<INode *>& tab, Tab<ModContext *>& ModContexts)
{
    for (int i=0; i<root->NumberOfChildren(); i++) {
		INode *node = root->GetChildNode(i);
		GetModNodes(node, fmod, tab, ModContexts);

		Object *obj = node->GetObjectRef();
		if(!obj) continue;
		while (obj->SuperClassID()==GEN_DERIVOB_CLASS_ID) {
			IDerivedObject *derivObj=(IDerivedObject*) obj;

			for (int i=0; i<derivObj->NumModifiers(); i++) {
				Modifier *mod=derivObj->GetModifier(i);
				if (mod==fmod)
                {
                    ModContext * temp = derivObj->GetModContext(i);
                    ModContexts.Append(1, &temp);

                    tab.Append(1, &node);
                }
            }
			obj=derivObj->GetObjRef();
		}
	}
}


void MultiMapMod::AddNewGroup() {
	int num_group = uvwProy.Count();
	UVWProyector * uvw_group = new UVWProyector(TRUE);

	int id = GetUniqueNameID();

	theHold.Begin();
	theHold.Put( new GroupsTableRestore(this,START_EDIT) );

	theHold.Put( new GroupsTableRestore(this,NEW_GROUP,id) );

	uvw_group->descCanal.printf(_T("Group %d"),id);

	uvwProy.SetCount(num_group+1);
	uvwProy[num_group] = uvw_group;
	ReplaceReference( num_group, uvwProy[num_group] );

	current_channel = num_group;
	top_group = num_group;

	SendDlgItemMessage( hwnd_main,IDC_TL_GROUPS_MENU,LB_ADDSTRING, 0, (LPARAM)(const TCHAR*) uvw_group->descCanal );
	SendDlgItemMessage( hwnd_main,IDC_TL_GROUPS_MENU,LB_SETTOPINDEX,top_group,0);

	theHold.Put( new GroupsTableRestore(this,END_EDIT) );
	theHold.Accept(_T("Add Group"));

	LocalDataChanged();
	UpdateUIAll();
	}

void MultiMapMod::RepositionGroup(int from, int to) {
	theHold.Begin();
	theHold.Put( new GroupsTableRestore(this,START_EDIT) );

	UVWProyector * uvw_group = uvwProy[from];
	Tab<INode *> nds;
	Tab<ModContext *> mcList;
	GetModNodes(ip->GetRootNode(),this,nds,mcList);

	theHold.Put( new GroupsTableRestore(this,MOVE_GROUP,from,to) );

	if ( from<to ) {
		uvwProy.Insert(to,1,&uvw_group);
		uvwProy.Delete(from,1);
		}
	if ( from>to ) {
		uvwProy.Insert(to,1,&uvw_group);
		uvwProy.Delete(from+1,1);
		}

	for ( int i=0; i<mcList.Count(); i++ ) {
		TexLayMCData *d = (TexLayMCData*)mcList[i]->localData;
		if (!d) continue;

		BitArray * face_sel = new BitArray(d->face_sel[from]);
		BitArray * edge_sel = new BitArray(d->edge_sel[from]);
		PolyUVWData * temp_uvw_data = d->group_uvw_data[from];
	
		theHold.Put( new MoveSelectionRestore(d, from, to) );

		if ( to>from ) {
			d->face_sel.sets.Insert(to,1,&face_sel);
			d->face_sel.sets.Delete(from,1);

			d->edge_sel.sets.Insert(to,1,&edge_sel);
			d->edge_sel.sets.Delete(from,1);

			d->group_uvw_data.Insert(to,1,&temp_uvw_data);
			d->group_uvw_data.Delete(from,1);
			}
		if ( from>to ) {
			d->face_sel.sets.Insert(to,1,&face_sel);
			d->face_sel.sets.Delete(from+1,1);

			d->edge_sel.sets.Insert(to,1,&edge_sel);
			d->edge_sel.sets.Delete(from+1,1);

			d->group_uvw_data.Insert(to,1,&temp_uvw_data);
			d->group_uvw_data.Delete(from+1,1);
			}
		}

	theHold.Put( new GroupsTableRestore(this,END_EDIT) );
	theHold.Accept(_T("Move Group"));

	LocalDataChanged();
	SendDlgItemMessage(	hwnd_main,IDC_TL_GROUPS_MENU,LB_SETITEMDATA,0,0);
	GroupsMenuForceDrawItem(to);
	UpdateUIAll();
	}

void MultiMapMod::DeleteCurrentGroup() {
	if ( uvwProy.Count()<=1 )
		return;

	int del_group = current_channel;

	theHold.Begin();
	theHold.Put( new GroupsTableRestore(this,START_EDIT) );

	DeleteReference( current_channel );		//	delete uvwProy[del_group];

	theHold.Put( new GroupsTableRestore(this,DELETE_GROUP) );

	uvwProy.Delete(del_group,1);
	uvwProy.Shrink();

	if ( current_channel>=uvwProy.Count() )
		current_channel = uvwProy.Count() - 1;

	Tab<INode *> nds;
	Tab<ModContext *> mcList;
	GetModNodes(ip->GetRootNode(),this,nds,mcList);

	for ( int nd=0;nd<mcList.Count(); nd++) {
		TexLayMCData *md = (TexLayMCData*) mcList[nd]->localData;

		theHold.Put( new DeleteSelectionRestore(md, del_group) );

		md->face_sel.DeleteSet(del_group);
		md->edge_sel.DeleteSet(del_group);

		delete md->group_uvw_data[del_group];
		md->group_uvw_data[del_group] = NULL;
		md->group_uvw_data.Delete(del_group,1);
		}

	SendDlgItemMessage(hwnd_main,IDC_TL_GROUPS_MENU, LB_DELETESTRING, del_group, 0);

	theHold.Put( new GroupsTableRestore(this,END_EDIT) );
	theHold.Accept(_T("Delete Group"));

	LocalDataChanged();
	UpdateUIAll();
	}


void MultiMapMod::CopyPBlock(int dest, int ori) {
	uvwProy[dest]->SetLength(0,uvwProy[ori]->GetLength(0));
	uvwProy[dest]->SetWidth(0,uvwProy[ori]->GetWidth(0));
	uvwProy[dest]->SetHeight(0,uvwProy[ori]->GetHeight(0));
	}

int MultiMapMod::RenderBegin(TimeValue t, ULONG flags) 
	{
	inRender = TRUE;
	for (int i=0; i<uvwProy.Count(); i++) 
		uvwProy[i]->Update(t);
	LocalDataChanged();
	return 1;
	}

int MultiMapMod::RenderEnd(TimeValue t)
	{
	inRender = FALSE;
	LocalDataChanged();
	return 1;
	}

// TODO: cap->cambiar nombre a algo relativo a preview
Class_ID MultiMapMod::InputType() // { return mapObjectClassID;}
	{
//	if(preview != NO_PREVIEW) return triObjectClassID;
//	else 
		return mapObjectClassID;
	}

// --- Modify Object ---------------------------------------------------

// This method checks if the user has changed the channel or if he is in the editing the
// same channel.
// If the user has changed the channel, this methods set up all the parameters of this new
// channel.
// If the user is editing the same channel, this method saves all the info to this channel

static TriObject *GetTriObject(TimeValue t,Object *obj,Interval &valid,BOOL &needsDel)
	{	
	needsDel = FALSE;
	if (!obj) return NULL;
	ObjectState os = obj->Eval(t);
	valid &= os.Validity(t);
	if (os.obj->IsSubClassOf(triObjectClassID)) {
		return (TriObject*)os.obj;
	} else {
		if (os.obj->CanConvertToType(triObjectClassID)) {
			Object *oldObj = os.obj;
			TriObject *tobj = (TriObject*)os.obj->ConvertToType(t,triObjectClassID);			
			needsDel = (tobj != oldObj);			
			return tobj;
			}
		}
	return NULL;
	}


static int lStart[12] = {0,1,3,2,4,5,7,6,0,1,2,3};
static int lEnd[12]   = {1,3,2,0,5,7,6,4,4,5,6,7};

static void DoBoxIcon(BOOL sel,float length, PolyLineProc& lp)
	{
	Point3 pt[3];
	
	length *= 0.5f;
	Box3 box;
	box.pmin = Point3(-length,-length,-length);
	box.pmax = Point3( length, length, length);

	if (sel) //lp.SetLineColor(1.0f,1.0f,0.0f);
		 lp.SetLineColor(GetUIColor(COLOR_SEL_GIZMOS));
	else //lp.SetLineColor(0.85f,0.5f,0.0f);		
		 lp.SetLineColor(GetUIColor(COLOR_GIZMOS));

	for (int i=0; i<12; i++) {
		pt[0] = box[lStart[i]];
		pt[1] = box[lEnd[i]];
		lp.proc(pt,2);
		}
	}

// TLTODO:	Implementar UNDO / REDO
BOOL MultiMapMod::AddSpline(INode *node) {
	if (node->TestForLoop(FOREVER,this)==REF_SUCCEED) {
		TimeValue t = ip->GetTime();

		pickingSpline = TRUE;

		uvwProy[current_channel]->SetSplineNode(node,ip->GetTime());
		UpdateUIItem( -1, uvwProy[current_channel] );

		LocalDataChanged();
		SetNodeName();
		return TRUE;
	} else {
		return FALSE;
		}
	}

BOOL MultiMapMod::AddSpline(INode *node, int chann) {
	if (node->TestForLoop(FOREVER,this)==REF_SUCCEED) {
		uvwProy[chann]->SetSplineNode(node);
		return TRUE;
	} else {
		return FALSE;
		}
	}

void MultiMapMod::ShapeFFGPoints(INode *surf) {
	Interval iv = FOREVER;
	TimeValue t = ip->GetTime();
	float u,v;
	if (ip && surf) {

		ModContextList mcList;
		INodeTab nodes;
		ip->GetModContexts(mcList,nodes);

		Matrix3 mcWs = Inverse(Inverse(nodes[0]->GetObjectTM(t)) * *mcList[0]->tm);

		ObjectState os = surf->EvalWorldState(0);
		Matrix3 surfTM = surf->GetObjTMBeforeWSM(t,&iv);


#if MAX_VERSION_MAJOR < 24
		Matrix3 mTM(1);
#else
		Matrix3 mTM;
#endif

		uvwProy[current_channel]->tmControl->GetValue(t,&mTM,FOREVER,CTRL_RELATIVE);
		mTM.PreRotateZ(PI);

		// Hold for undo
		theHold.Begin();
		theHold.Put(new MoveRestore(this));

		BitArray oldsel = uvwProy[current_channel]->gm->sel;
		uvwProy[current_channel]->gm->sel.SetAll();

		uvwProy[current_channel]->gm->PlugControllers(t);
		for (int i=0; i<uvwProy[current_channel]->gm->verts.Count(); i++) {
			u = float(i%uvwProy[current_channel]->gm->nx)/float(uvwProy[current_channel]->gm->nx-1);
			v = float(i/uvwProy[current_channel]->gm->nx)/float(uvwProy[current_channel]->gm->ny-1);
			if (u == 1.0f) u = 0.999999f;
			if (v == 1.0f) v = 0.999999f;

			if (uvwProy[current_channel]->gm->GetVtCtrl(i)) {
				uvwProy[current_channel]->gm->verts[i] = ((os.obj->GetSurfacePoint(t,u,v,iv) * surfTM)
												* Inverse(mcWs)) * Inverse(mTM); 
				uvwProy[current_channel]->gm->GetVtCtrl(i)->SetValue(t,&uvwProy[current_channel]->gm->verts[i],TRUE,CTRL_ABSOLUTE);
				}
			else {
				uvwProy[current_channel]->gm->verts[i] = ((os.obj->GetSurfacePoint(t,u,v,iv) * surfTM)
												* Inverse(mcWs)) * Inverse(mTM); 
				}
			}
		uvwProy[current_channel]->gm->sel = oldsel;

		theHold.Accept(_T("Shape FFG to Surface"));
		uvwProy[current_channel]->gm->CacheVNormals();
		NotifyDependents(Interval(t,t), PART_ALL, REFMSG_MOD_DISPLAY_ON);
		LocalDataChanged();
		}
	}


void MultiMapMod::ResetNode() {
	if ( uvwProy[current_channel]->GetMappingType() == MAP_TL_SPLINE ) {
		uvwProy[current_channel]->GetSplineNode();
		if ( uvwProy[current_channel]->spline_node )
			AddSpline(uvwProy[current_channel]->spline_node);
		}
	}

//--- MultiMapMod methods -----------------------------------------------

void* MultiMapMod::GetInterface(ULONG id)
	{	
	if (id==I_TEXLAYINTERFACE) return this;
	else return Modifier::GetInterface(id);
	}

// --- Gizmo transformations ------------------------------------------

// Metodo que genera la matriz para transformar los puntos de mapping en puntos
// 
Matrix3 MultiMapMod::GenMatrix(TimeValue t,ModContext *mc)
	{

#if MAX_VERSION_MAJOR < 24
	Matrix3 tm(1);
#else
	Matrix3 tm;
#endif

	Interval valid;
	tm.SetTrans(Point3(0.5f,0.5f,0.5f));

	Point3 s;
	s.x = 0.5f;
	s.y = 0.5f;
	s.z = 1.0f;
	tm.PreScale(s);

	if (mc && mc->tm) {
		tm = tm * Inverse(*mc->tm);
		}

	return tm;
	}

#define NUM_SEGS	32

void MultiMapMod::DoMMSphericalMapIcon(int channel, TimeValue t, BOOL sel,float radius, PolyLineProc& lp)
	{
	float u, up, assu, aeeu, assv, aeev;
	Point3 pt[6];
	Point3 aspt[6];
	Point3 aept[6];
	Point3 qt[2];
	Point3 asqt[2];
	Point3 aeqt[2];
	Point3 aux[2];
	Point3 pas[6];
	Point3 pae[6];
	BOOL warning = 0;

	float atus = uvwProy[channel]->attUs;
	float atuo = uvwProy[channel]->attUo;
	float atvs = uvwProy[channel]->attVs;
	float atvo = uvwProy[channel]->attVo;

	if (atus==0.0f && atuo==0.0) atus = 1.0f;
	if (atvs==0.0f && atvo==0.0) atvs = 1.0f;

	float asr = radius;
	float aer = radius;
	if (!(uvwProy[channel]->tile_w == 0.0f && uvwProy[channel]->offset_w == 0.0f)) {
		asr = (radius*uvwProy[channel]->attWs / uvwProy[channel]->tile_w)+(uvwProy[channel]->offset_w/uvwProy[channel]->tile_w);
		aer = (radius*(uvwProy[channel]->attWs+uvwProy[channel]->attWo) / uvwProy[channel]->tile_w)+(uvwProy[channel]->offset_w/uvwProy[channel]->tile_w);
		}

	float angs = ((uvwProy[channel]->offset_u/uvwProy[channel]->tile_u) * TWOPI) + PI;
	if (angs < PI) {
		angs = PI;
		warning = 1;
		}

	if (angs > PI * 3.0f) {
		angs = PI * 3.0f;
		warning = 1;
		}

	float ange = (((uvwProy[channel]->offset_u/uvwProy[channel]->tile_u) + (1.0f/uvwProy[channel]->tile_u)) * TWOPI) + PI;
	if (ange > PI * 3.0f) {
		ange = PI * 3.0f;
		warning = 1;
		}

	if (ange < PI) {
		ange = PI;
		warning = 1;
		}

	float asuangs = angs+(ange-angs)/2.0f-atus*(ange-angs)/2.0f;
	if (asuangs < PI) asuangs = PI;
	if (asuangs > PI * 3.0f) asuangs = PI * 3.0f;

	float asuange = angs+(ange-angs)/2.0f+atus*(ange-angs)/2.0f;
	if (asuange < PI) asuange = PI;
	if (asuange > PI * 3.0f) asuange = PI * 3.0f;

	float aeuangs = angs+(ange-angs)/2.0f-(atus+atuo)*(ange-angs)/2.0f;
	if (aeuangs < PI) aeuangs = PI;
	if (aeuangs > PI * 3.0f) aeuangs = PI * 3.0f;

	float aeuange = angs+(ange-angs)/2.0f+(atus+atuo)*(ange-angs)/2.0f;
	if (aeuange < PI) aeuange = PI;
	if (aeuange > PI * 3.0f) aeuange = PI * 3.0f;

	if (uvwProy[channel]->tile_u == 1.0f && uvwProy[channel]->offset_u == 0.0f) warning = 0;

	float vangs = (- PI / 2.0f) + (uvwProy[channel]->offset_v/uvwProy[channel]->tile_v) * PI;
	if (vangs < -PI/2.0f) {
		vangs = -PI/2.0f;
		warning = 1;
		}
	if (vangs > PI/2.0f) {
		vangs = PI/2.0f;
		warning = 1;
		}

	float vange = (- PI / 2.0f) + PI / uvwProy[channel]->tile_v + (uvwProy[channel]->offset_v/uvwProy[channel]->tile_v) * PI;
	if (vange < -PI/2.0f) {
		vange = -PI/2.0f;
		warning = 1;
		}
	if (vange > PI/2.0f) {
		vange = PI/2.0f;
		warning = 1;
		}

	float asvangs = vangs+(vange-vangs)/2.0f-atvs*(vange-vangs)/2.0f;
	if (asvangs < -PI/2.0f) asvangs = -PI/2.0f;
	if (asvangs > PI/2.0f) asvangs = PI/2.0f;

	float asvange = vangs+(vange-vangs)/2.0f+atvs*(vange-vangs)/2.0f;
	if (asvange < -PI/2.0f) asvange = -PI/2.0f;
	if (asvange > PI/2.0f) asvange = PI/2.0f;

	float aevangs = vangs+(vange-vangs)/2.0f-(atvs+atvo)*(vange-vangs)/2.0f;
	if (aevangs < -PI/2.0f) aevangs = -PI/2.0f;
	if (aevangs > PI/2.0f) aevangs = PI/2.0f;

	float aevange = vangs+(vange-vangs)/2.0f+(atvs+atvo)*(vange-vangs)/2.0f;
	if (aevange < -PI/2.0f) aevange = -PI/2.0f;
	if (aevange > PI/2.0f) aevange = PI/2.0f;

	float asvangm = asvangs + (asvange-asvangs)/2.0f;
	float aevangm = aevangs + (aevange-aevangs)/2.0f;

	float vangm = vangs + (vange-vangs)/2.0f;

	Point3 ept = Point3(float(cos(angs)*cos(vange)*radius),float(sin(angs)*cos(vange)*radius),float(sin(vange) * radius));
	Point3 mpt = Point3(float(cos(angs)*cos(vangm)*radius),float(sin(angs)*cos(vangm)*radius),float(sin(vangm) * radius));
	Point3 spt = Point3(float(cos(angs)*cos(vangs)*radius),float(sin(angs)*cos(vangs)*radius),float(sin(vangs) * radius));

	Point3 asept = Point3(float(cos(asuangs)*cos(asvange)*asr),float(sin(asuangs)*cos(asvange)*asr),float(sin(asvange) * asr));
	Point3 asmpt = Point3(float(cos(asuangs)*cos(asvangm)*asr),float(sin(asuangs)*cos(asvangm)*asr),float(sin(asvangm) * asr));
	Point3 asspt = Point3(float(cos(asuangs)*cos(asvangs)*asr),float(sin(asuangs)*cos(asvangs)*asr),float(sin(asvangs) * asr));
	pas[0] = asspt;
	pas[3] = asept;

	Point3 aeept = Point3(float(cos(aeuangs)*cos(aevange)*aer),float(sin(aeuangs)*cos(aevange)*aer),float(sin(aevange) * aer));
	Point3 aempt = Point3(float(cos(aeuangs)*cos(aevangm)*aer),float(sin(aeuangs)*cos(aevangm)*aer),float(sin(aevangm) * aer));
	Point3 aespt = Point3(float(cos(aeuangs)*cos(aevangs)*aer),float(sin(aeuangs)*cos(aevangs)*aer),float(sin(aevangs) * aer));
	pae[0] = aespt;
	pae[3] = aeept;

	for (int i=1; i<=NUM_SEGS; i++) {
		u = angs + ((ange - angs)/float(NUM_SEGS))*float(i);
		assu = asuangs + ((asuange - asuangs)/float(NUM_SEGS))*float(i);
		aeeu = aeuangs + ((aeuange - aeuangs)/float(NUM_SEGS))*float(i);
		
		if (sel) lp.SetLineColor(GetUIColor(COLOR_SEL_GIZMOS));
		else lp.SetLineColor(GetUIColor(COLOR_GIZMOS));		
		if (warning) lp.SetLineColor(1.0f,0.0f,0.0f);

		// Dibuja la Isolinea U inicial
		if (i == 1) {
			qt[0] = Point3(float(cos(angs)*cos(vangs)*radius),float(sin(angs)*cos(vangs)*radius),float(sin(vangs) * radius));
			asqt[0] = Point3(float(cos(asuangs)*cos(asvangs)*asr),float(sin(asuangs)*cos(asvangs)*asr),float(sin(asvangs) * asr));
			aeqt[0] = Point3(float(cos(aeuangs)*cos(aevangs)*aer),float(sin(aeuangs)*cos(aevangs)*aer),float(sin(aevangs) * aer));
			for (int j=1; j<=NUM_SEGS/2; j++) {
				up = vangs + (vange - vangs)/float(NUM_SEGS)*float(j)*2.0f;
				assv = asvangs + (asvange - asvangs)/float(NUM_SEGS)*float(j)*2.0f;
				aeev = aevangs + (aevange - aevangs)/float(NUM_SEGS)*float(j)*2.0f;
				qt[1].x = (float)cos(angs) * (float)cos(up) * radius;
				qt[1].y = (float)sin(angs) * (float)cos(up) * radius;
				qt[1].z = (float)sin(up) * radius;

				asqt[1].x = (float)cos(asuangs) * (float)cos(assv) * asr;
				asqt[1].y = (float)sin(asuangs) * (float)cos(assv) * asr;
				asqt[1].z = (float)sin(assv) * asr;

				aeqt[1].x = (float)cos(aeuangs) * (float)cos(aeev) * aer;
				aeqt[1].y = (float)sin(aeuangs) * (float)cos(aeev) * aer;
				aeqt[1].z = (float)sin(aeev) * aer;

				if (sel) lp.SetLineColor(GetUIColor(COLOR_SEL_GIZMOS));
				else lp.SetLineColor(GetUIColor(COLOR_GIZMOS));		
				if (warning) lp.SetLineColor(1.0f,0.0f,0.0f);
				lp.proc(qt,2);
				qt[0] = qt[1];

				if (sel && uvwProy[channel]->attOn) {
					lp.SetLineColor(GetUIColor(COLOR_START_RANGE));
					lp.proc(asqt,2);
					lp.SetLineColor(GetUIColor(COLOR_END_RANGE));
					lp.proc(aeqt,2);
					}
				asqt[0] = asqt[1];
				aeqt[0] = aeqt[1];
				}
			}

		pt[0]   = ept;
		
		pt[1].x = (float)cos(u) * (float)cos(vange) * radius;
		pt[1].y = (float)sin(u) * (float)cos(vange) * radius;
		pt[1].z = (float)sin(vange) * radius;
	
		pt[2]   = mpt;

		pt[3].x = (float)cos(u) * (float)cos(vangm) * radius;
		pt[3].y = (float)sin(u) * (float)cos(vangm) * radius;
		pt[3].z = (float)sin(vangm) * radius;

		pt[4]   = spt;

		pt[5].x = (float)cos(u) * (float)cos(vangs) * radius;
		pt[5].y = (float)sin(u) * (float)cos(vangs) * radius;
		pt[5].z = (float)sin(vangs) * radius;

		// Attenuation Start
		aspt[0]   = asept;
		
		aspt[1].x = (float)cos(assu) * (float)cos(asvange) * asr;
		aspt[1].y = (float)sin(assu) * (float)cos(asvange) * asr;
		aspt[1].z = (float)sin(asvange) * asr;
	
		aspt[2]   = asmpt;

		aspt[3].x = (float)cos(assu) * (float)cos(asvangm) * asr;
		aspt[3].y = (float)sin(assu) * (float)cos(asvangm) * asr;
		aspt[3].z = (float)sin(asvangm) * asr;

		aspt[4]   = asspt;

		aspt[5].x = (float)cos(assu) * (float)cos(asvangs) * asr;
		aspt[5].y = (float)sin(assu) * (float)cos(asvangs) * asr;
		aspt[5].z = (float)sin(asvangs) * asr;

		// Attenuation End
		aept[0]   = aeept;
		
		aept[1].x = (float)cos(aeeu) * (float)cos(aevange) * aer;
		aept[1].y = (float)sin(aeeu) * (float)cos(aevange) * aer;
		aept[1].z = (float)sin(aevange) * aer;
	
		aept[2]   = aempt;

		aept[3].x = (float)cos(aeeu) * (float)cos(aevangm) * aer;
		aept[3].y = (float)sin(aeeu) * (float)cos(aevangm) * aer;
		aept[3].z = (float)sin(aevangm) * aer;

		aept[4]   = aespt;

		aept[5].x = (float)cos(aeeu) * (float)cos(aevangs) * aer;
		aept[5].y = (float)sin(aeeu) * (float)cos(aevangs) * aer;
		aept[5].z = (float)sin(aevangs) * aer;
		// Dibuja la Isolinea U final
		if (i == NUM_SEGS) {
			pas[2] = aspt[5];
			pas[5] = aspt[1];
			pae[2] = aept[5];
			pae[5] = aept[1];
			qt[0] = Point3(float(cos(u)*cos(vangs)*radius),float(sin(u)*cos(vangs)*radius),float(sin(vangs) * radius));
			asqt[0] = Point3(float(cos(assu)*cos(asvangs)*asr),float(sin(assu)*cos(asvangs)*asr),float(sin(asvangs) * asr));
			aeqt[0] = Point3(float(cos(aeeu)*cos(aevangs)*aer),float(sin(aeeu)*cos(aevangs)*aer),float(sin(aevangs) * aer));
			for (int j=1; j<=NUM_SEGS/2; j++) {
				up = vangs + (vange - vangs)/float(NUM_SEGS)*float(j)*2.0f;
				assv = asvangs + (asvange - asvangs)/float(NUM_SEGS)*float(j)*2.0f;
				aeev = aevangs + (aevange - aevangs)/float(NUM_SEGS)*float(j)*2.0f;
				qt[1].x = (float)cos(u) * (float)cos(up) * radius;
				qt[1].y = (float)sin(u) * (float)cos(up) * radius;
				qt[1].z = (float)sin(up) * radius;

				asqt[1].x = (float)cos(assu) * (float)cos(assv) * asr;
				asqt[1].y = (float)sin(assu) * (float)cos(assv) * asr;
				asqt[1].z = (float)sin(assv) * asr;
	
				aeqt[1].x = (float)cos(aeeu) * (float)cos(aeev) * aer;
				aeqt[1].y = (float)sin(aeeu) * (float)cos(aeev) * aer;
				aeqt[1].z = (float)sin(aeev) * aer;

				if (sel) lp.SetLineColor(0.0f,0.8f,0.0f);
				else lp.SetLineColor(GetUIColor(COLOR_GIZMOS));		
				if (warning) lp.SetLineColor(1.0f,0.0f,0.0f);
				lp.proc(qt,2);
				qt[0] = qt[1];
				if (sel && uvwProy[channel]->attOn) {
					lp.SetLineColor(GetUIColor(COLOR_START_RANGE));
					lp.proc(asqt,2);
					lp.SetLineColor(GetUIColor(COLOR_END_RANGE));
					lp.proc(aeqt,2);
					}
				asqt[0] = asqt[1];
				aeqt[0] = aeqt[1];
				}
			}

		// Dibuja la Isolineas V
		if (sel) lp.SetLineColor(GetUIColor(COLOR_SEL_GIZMOS));
		else lp.SetLineColor(GetUIColor(COLOR_GIZMOS));		
		if (warning) lp.SetLineColor(1.0f,0.0f,0.0f);

		lp.proc(pt,2);
		lp.proc(&pt[2],2);
		lp.proc(&pt[4],2);

		if (sel && uvwProy[channel]->attOn) {
			lp.SetLineColor(GetUIColor(COLOR_START_RANGE));
			lp.proc(aspt,2);
			lp.proc(&aspt[2],2);
			lp.proc(&aspt[4],2);
			lp.SetLineColor(GetUIColor(COLOR_END_RANGE));
			lp.proc(aept,2);
			lp.proc(&aept[2],2);
			lp.proc(&aept[4],2);
			}

		// Dibuja la Isolinea U del medio y un pipicito
		if (i == NUM_SEGS/2) {
			qt[0] = Point3(float(cos(u)*cos(vangs)*radius),float(sin(u)*cos(vangs)*radius),float(sin(vangs) * radius));
			asqt[0] = Point3(float(cos(assu)*cos(asvangs)*asr),float(sin(assu)*cos(asvangs)*asr),float(sin(asvangs) * asr));
			aeqt[0] = Point3(float(cos(aeeu)*cos(aevangs)*aer),float(sin(aeeu)*cos(aevangs)*aer),float(sin(aevangs) * aer));
			for (int j=1; j<=NUM_SEGS/2; j++) {
				up = vangs + (vange - vangs)/float(NUM_SEGS)*float(j)*2.0f;
				assv = asvangs + (asvange - asvangs)/float(NUM_SEGS)*float(j)*2.0f;
				aeev = aevangs + (aevange - aevangs)/float(NUM_SEGS)*float(j)*2.0f;
				qt[1].x = (float)cos(u) * (float)cos(up) * radius;
				qt[1].y = (float)sin(u) * (float)cos(up) * radius;
				qt[1].z = (float)sin(up) * radius;

				asqt[1].x = (float)cos(assu) * (float)cos(assv) * asr;
				asqt[1].y = (float)sin(assu) * (float)cos(assv) * asr;
				asqt[1].z = (float)sin(assv) * asr;

				aeqt[1].x = (float)cos(aeeu) * (float)cos(aeev) * aer;
				aeqt[1].y = (float)sin(aeeu) * (float)cos(aeev) * aer;
				aeqt[1].z = (float)sin(aeev) * aer;

				if (sel) lp.SetLineColor(GetUIColor(COLOR_SEL_GIZMOS));
				else lp.SetLineColor(GetUIColor(COLOR_GIZMOS));		
				if (warning) lp.SetLineColor(1.0f,0.0f,0.0f);
				lp.proc(qt,2);
				aux[0] = qt[0];
				qt[0] = qt[1];
				if (sel && uvwProy[channel]->attOn) {
					lp.SetLineColor(GetUIColor(COLOR_START_RANGE));
					lp.proc(asqt,2);
					lp.SetLineColor(GetUIColor(COLOR_END_RANGE));
					lp.proc(aeqt,2);
					}
				asqt[0] = asqt[1];
				aeqt[0] = aeqt[1];
				}
			qt[1] += (qt[1] - aux[0]) / Length(qt[1] - aux[0]) * radius * 0.2f;
			if (vangs != vange) 
				if (sel) lp.SetLineColor(GetUIColor(COLOR_SEL_GIZMOS));
				else lp.SetLineColor(GetUIColor(COLOR_GIZMOS));		
				if (warning) lp.SetLineColor(1.0f,0.0f,0.0f);
				lp.proc(qt,2);
			}

		ept = pt[1];
		mpt = pt[3];
		spt = pt[5];

		asept = aspt[1];
		asmpt = aspt[3];
		asspt = aspt[5];

		aeept = aept[1];
		aempt = aept[3];
		aespt = aept[5];
		}
	if (sel && uvwProy[channel]->attOn) {
		pas[1] = Point3(0.0f,0.0f,0.0f);
		pas[4] = Point3(0.0f,0.0f,0.0f);
		pae[1] = Point3(0.0f,0.0f,0.0f);
		pae[4] = Point3(0.0f,0.0f,0.0f);
		lp.SetLineColor(GetUIColor(COLOR_START_RANGE));
		lp.proc(pas,3);
		lp.proc(&pas[3],3);
		lp.SetLineColor(GetUIColor(COLOR_END_RANGE));
		lp.proc(pae,3);
		lp.proc(&pae[3],3);
		}
	}

void MultiMapMod::DoMMShrinkMapIcon(int channel, TimeValue t, BOOL sel,float radius, PolyLineProc& lp)
	{
	Point3 pt[6];
	Point3 qt[2];
	Point3 ats[6];
	Point3 ate[6];
	Point3 sqt[2];
	Point3 eqt[2];
	Point3 aux[2];
	Point3 opt[3];
	Point3 oats[3];
	Point3 oate[3];
	Point3 pats[6];
	Point3 pate[6];
	BOOL warning = 0;

	float atus = uvwProy[channel]->attUs;
	float atuo = uvwProy[channel]->attUo;
	float atvs = uvwProy[channel]->attVs;
	float atvo = uvwProy[channel]->attVo;

	if (atus==0.0f && atuo==0.0) atus = 1.0f;
	if (atvs==0.0f && atvo==0.0) atvs = 1.0f;

	float asr,aer;
	if (!(uvwProy[channel]->attWs==0.0f && uvwProy[channel]->attWo==0.0f)) {
		asr = (radius*uvwProy[channel]->attWs / uvwProy[channel]->tile_w)+(uvwProy[channel]->offset_w/uvwProy[channel]->tile_w);
		aer = (radius*(uvwProy[channel]->attWs+uvwProy[channel]->attWo) / uvwProy[channel]->tile_w)+(uvwProy[channel]->offset_w/uvwProy[channel]->tile_w);
		}
	else {
		asr = 1.0f;
		aer = 1.0f;
		}

	float a1, a2;
	float u,v;

	float au,av;
	float avi,avf;
	float aui,auf;

	float afu,afv;
	float afvi,afvf;
	float afui,afuf;

	float tu,ou;
	float tv,ov;
	
	tu = uvwProy[channel]->tile_u;
	ou = uvwProy[channel]->offset_u;

	tv = uvwProy[channel]->tile_v;
	ov = uvwProy[channel]->offset_v;

	u = ou/tu;

	avi = (ov/tv + 0.5f/tv)-0.5f*atvs/tv;
	avf = (ov/tv + 0.5f/tv)+0.5f*atvs/tv;
	if (avi < 0.0f) avi = 0.0f;
	if (avf < 0.0f) avf = 0.0f;
	if (avi > 1.0f) avi = 1.0f;
	if (avf > 1.0f) avf = 1.0f;
		
	aui = (ou/tu + 0.5f/tu) - 0.5f*atus/tu;
	auf = (ou/tu + 0.5f/tu) + 0.5f*atus/tu;
	if (aui < 0.0f) aui = 0.0f;
	if (auf < 0.0f) auf = 0.0f;
	if (aui > 1.0f) aui = 1.0f;
	if (auf > 1.0f) auf = 1.0f;

	afvi = (ov/tv + 0.5f/tv)-0.5f*(atvs+atvo)/tv;
	afvf = (ov/tv + 0.5f/tv)+0.5f*(atvs+atvo)/tv;
	if (afvi < 0.0f) afvi = 0.0f;
	if (afvf < 0.0f) afvf = 0.0f;
	if (afvi > 1.0f) afvi = 1.0f;
	if (afvf > 1.0f) afvf = 1.0f;
		
	afui = (ou/tu + 0.5f/tu) - 0.5f*(atus+atuo)/tu;
	afuf = (ou/tu + 0.5f/tu) + 0.5f*(atus+atuo)/tu;
	if (afui < 0.0f) afui = 0.0f;
	if (afuf < 0.0f) afuf = 0.0f;
	if (afui > 1.0f) afui = 1.0f;
	if (afuf > 1.0f) afuf = 1.0f;

	BOOL showAtt = 1;
	if ((uvwProy[channel]->offset_u > uvwProy[channel]->tile_u) || (uvwProy[channel]->offset_v > uvwProy[channel]->tile_v) || (uvwProy[channel]->offset_u < -1.0f) || (uvwProy[channel]->offset_v < -1.0f))
		showAtt = 0;


	if (ou < 0.0f) {
		warning = 1;
		u = 0.0f;
		}

	for (int iv=0; iv<3; iv++) {
		v = 1.0f/tv * float(iv)/2.0f + ov/tv;
		if (ov < 0.0f) v = (1.0f/tv + ov/tv)*float(iv)/2.0f;
		if (tv-ov < 1.0f) v = (tv-ov)/tv*float(iv)/2.0f + ov/tv;

		a1 = float(atan((v-0.5f)/(u-0.5f)));
		a2 = float(HALFPI+(PI - TWOPI * u)/cos(a1));
	
		opt[iv].z = float(sin(a2));
		if (u<0.5f) opt[iv].x = float(-sqrt((1-opt[iv].z*opt[iv].z)/(1+tan(a1)*tan(a1))));
		else opt[iv].x = float(sqrt((1-opt[iv].z*opt[iv].z)/(1+tan(a1)*tan(a1))));
		opt[iv].y = float(opt[iv].x * tan(a1));

		av = avi + (avf-avi)*float(iv)/2.0f;
		au = aui;//(ou/tu + 0.5f/tu) - 0.5f*uvwProy[channel]->tile_u/tu;

		a1 = float(atan((av-0.5f)/(au-0.5f)));
		a2 = float(HALFPI+(PI - TWOPI * au)/cos(a1));
	
		oats[iv].z = float(sin(a2));
		if (au<0.5f) oats[iv].x = float(-sqrt((1-oats[iv].z*oats[iv].z)/(1+tan(a1)*tan(a1))));
		else oats[iv].x = float(sqrt((1-oats[iv].z*oats[iv].z)/(1+tan(a1)*tan(a1))));
		oats[iv].y = float(oats[iv].x * tan(a1));
		oats[iv] *= asr;
		if (iv==0) pats[0] = oats[iv];
		if (iv==2) pats[3] = oats[iv];
		
		afv = afvi + (afvf-afvi)*float(iv)/2.0f;
		afu = afui;//(ou/tu + 0.5f/tu) - 0.5f*uvwProy[channel]->tile_u/tu;

		a1 = float(atan((afv-0.5f)/(afu-0.5f)));
		a2 = float(HALFPI+(PI - TWOPI * afu)/cos(a1));
	
		oate[iv].z = float(sin(a2));
		if (afu<0.5f) oate[iv].x = float(-sqrt((1-oate[iv].z*oate[iv].z)/(1+tan(a1)*tan(a1))));
		else oate[iv].x = float(sqrt((1-oate[iv].z*oate[iv].z)/(1+tan(a1)*tan(a1))));
		oate[iv].y = float(oate[iv].x * tan(a1));
		oate[iv] *= aer;
		if (iv==0) pate[0] = oate[iv];
		if (iv==2) pate[3] = oate[iv];
		}


	for (int i=1; i<=NUM_SEGS; i++) {
		if (tu<0.0001f) continue;
		if (ou<-1.0f) continue;
		if (tu-ou < 0.0f) continue;
		if (ov<-1.0f) continue;
		if (tv-ov<0.0f) continue;

		u = ou/tu + (1.0f/tu)*float(i)/float(NUM_SEGS);
		au = aui + (auf - aui)*float(i)/float(NUM_SEGS);
		afu = afui + (afuf - afui)*float(i)/float(NUM_SEGS);

		if (tu - ou < 1.0f) {
			warning = 1;
			u = ou/tu + (tu-ou)*float(i)/float(NUM_SEGS)/tu;
			}

		if (ou < 0.0f) {
			warning = 1;
			u = (ou/tu + (1.0f/tu))*float(i)/float(NUM_SEGS);
			}

		if (u-0.5f < 0.001f) u += 0.0011f;
		if (au-0.5f < 0.001f) au += 0.0011f;
		if (afu-0.5f < 0.001f) afu += 0.0011f;

		for (int iv=0; iv<3; iv++) {
			v = 1.0f/tv * float(iv)/2.0f + ov/tv;
			if (ov < 0.0f) {v = (1.0f/tv + ov/tv)*float(iv)/2.0f;warning=1;}
			if (tv - ov < 1.0f) {v = (tv-ov)/tv*float(iv)/2.0f + ov/tv;warning=1;}

			a1 = float(atan((v-0.5f)/(u-0.5f)));
			a2 = float(HALFPI+(PI - TWOPI * u)/cos(a1));
		
			pt[iv*2] = opt[iv];
			pt[iv*2+1].z = float(sin(a2));
			if (u<0.5f) pt[iv*2+1].x = float(-sqrt((1-pt[iv*2+1].z*pt[iv*2+1].z)/(1+tan(a1)*tan(a1))));
			else pt[iv*2+1].x = float(sqrt((1-pt[iv*2+1].z*pt[iv*2+1].z)/(1+tan(a1)*tan(a1))));
			pt[iv*2+1].y = float(pt[iv*2+1].x * tan(a1));

			av = avi + (avf-avi)*float(iv)/2.0f;

			a1 = float(atan((av-0.5f)/(au-0.5f)));
			a2 = float(HALFPI+(PI - TWOPI * au)/cos(a1));

			ats[iv*2] = oats[iv];
			ats[iv*2+1].z = float(sin(a2));
			if (au<0.5f) ats[iv*2+1].x = float(-sqrt((1-ats[iv*2+1].z*ats[iv*2+1].z)/(1+tan(a1)*tan(a1))));
			else ats[iv*2+1].x = float(sqrt((1-ats[iv*2+1].z*ats[iv*2+1].z)/(1+tan(a1)*tan(a1))));
			ats[iv*2+1].y = float(ats[iv*2+1].x * tan(a1));
			ats[iv*2+1] *= asr;
			if (i==NUM_SEGS)
				if (iv==0) pats[2] = ats[iv*2+1];
				if (iv==2) pats[5] = ats[iv*2+1];

			afv = afvi + (afvf-afvi)*float(iv)/2.0f;

			a1 = float(atan((afv-0.5f)/(afu-0.5f)));
			a2 = float(HALFPI+(PI - TWOPI * afu)/cos(a1));

			ate[iv*2] = oate[iv];
			ate[iv*2+1].z = float(sin(a2));
			if (afu<0.5f) ate[iv*2+1].x = float(-sqrt((1-ate[iv*2+1].z*ate[iv*2+1].z)/(1+tan(a1)*tan(a1))));
			else ate[iv*2+1].x = float(sqrt((1-ate[iv*2+1].z*ate[iv*2+1].z)/(1+tan(a1)*tan(a1))));
			ate[iv*2+1].y = float(ate[iv*2+1].x * tan(a1));
			ate[iv*2+1] *= aer;
			if (i==NUM_SEGS)
				if (iv==0) pate[2] = ate[iv*2+1];
				if (iv==2) pate[5] = ate[iv*2+1];
			}

		// Dibujamos la Isolinea U inicial, media y final
		if (i == 1 || i == NUM_SEGS || i == NUM_SEGS/2) {
			if (i == 1) {
				qt[0] = opt[0];
				sqt[0] = oats[0];
				eqt[0] = oate[0];
				}
			else {
				qt[0] = pt[1];
				sqt[0] = ats[1];
				eqt[0] = ate[1];
				}

			for (int j=1; j<=NUM_SEGS; j++) {
				if (i == 1 ) {
					if (ou < 0.0f) u = 0.0f;
					else u = ou/tu;
					au = aui;
					afu = afui;
					}
				v = 1.0f/tv*float(j)/float(NUM_SEGS)+ov/tv;
				if (ov<0.0f) {v = (1.0f/tv+ov/tv)*float(j)/float(NUM_SEGS);warning=1;}
				if (tv-ov<1.0f) {v = (tv-ov)/tv*float(j)/float(NUM_SEGS) + ov/tv;warning=1;}

				a1 = float(atan((v-0.5f)/(u-0.5f)));
				a2 = float(HALFPI+(PI - TWOPI * u)/cos(a1));

				qt[1].z = float(sin(a2));
				if (u<0.5f) qt[1].x = float(-sqrt((1.0f-qt[1].z*qt[1].z)/(1.0f+tan(a1)*tan(a1))));
				else qt[1].x = float(sqrt((1.0f-qt[1].z*qt[1].z)/(1.0f+tan(a1)*tan(a1))));
				qt[1].y = float(qt[1].x * tan(a1));

				av = avi + (avf-avi)*float(j)/float(NUM_SEGS);

				a1 = float(atan((av-0.5f)/(au-0.5f)));
				a2 = float(HALFPI+(PI - TWOPI * au)/cos(a1));
			
				sqt[1].z = float(sin(a2));
				if (au<0.5f) sqt[1].x = float(-sqrt((1-sqt[1].z*sqt[1].z)/(1+tan(a1)*tan(a1))));
				else sqt[1].x = float(sqrt((1-sqt[1].z*sqt[1].z)/(1+tan(a1)*tan(a1))));
				sqt[1].y = float(sqt[1].x * tan(a1));
				sqt[1] *= asr;

				afv = afvi + (afvf-afvi)*float(j)/float(NUM_SEGS);

				a1 = float(atan((afv-0.5f)/(afu-0.5f)));
				a2 = float(HALFPI+(PI - TWOPI * afu)/cos(a1));
			
				eqt[1].z = float(sin(a2));
				if (afu<0.5f) eqt[1].x = float(-sqrt((1-eqt[1].z*eqt[1].z)/(1+tan(a1)*tan(a1))));
				else eqt[1].x = float(sqrt((1-eqt[1].z*eqt[1].z)/(1+tan(a1)*tan(a1))));
				eqt[1].y = float(eqt[1].x * tan(a1));
				eqt[1] *= aer;

				if (sel) lp.SetLineColor(GetUIColor(COLOR_SEL_GIZMOS));
				else lp.SetLineColor(GetUIColor(COLOR_GIZMOS));		
				if (sel && i == NUM_SEGS) lp.SetLineColor(0.0f,0.8f,0.0f);
				if (warning) lp.SetLineColor(1.0f,0.0f,0.0f);
				lp.proc(qt,2);
				if (sel && uvwProy[channel]->attOn && showAtt) {
					lp.SetLineColor(GetUIColor(COLOR_START_RANGE));
					lp.proc(sqt,2);
					lp.SetLineColor(GetUIColor(COLOR_END_RANGE));
					lp.proc(eqt,2);
					}
				if (i == NUM_SEGS/2) aux[0] = qt[0];
				qt[0] = qt[1];
				sqt[0] = sqt[1];
				eqt[0] = eqt[1];
				}
			if (i == NUM_SEGS/2 && !warning) {
				qt[1] += (qt[1] - aux[0]) / Length(qt[1] - aux[0]) * radius * 0.2f;
				if (sel) lp.SetLineColor(GetUIColor(COLOR_SEL_GIZMOS));
				else lp.SetLineColor(GetUIColor(COLOR_GIZMOS));		
				lp.proc(qt,2);
				}
			}

		// Dibuja la Isolineas V
		if (sel) lp.SetLineColor(GetUIColor(COLOR_SEL_GIZMOS));
		else lp.SetLineColor(GetUIColor(COLOR_GIZMOS));		
		if (warning) lp.SetLineColor(1.0f,0.0f,0.0f);

		lp.proc(pt,2);
		lp.proc(&pt[2],2);
		lp.proc(&pt[4],2);

		if (sel && uvwProy[channel]->attOn && showAtt) {
			lp.SetLineColor(GetUIColor(COLOR_START_RANGE));
			lp.proc(ats,2);
			lp.proc(&ats[2],2);
			lp.proc(&ats[4],2);
			lp.SetLineColor(GetUIColor(COLOR_END_RANGE));
			lp.proc(ate,2);
			lp.proc(&ate[2],2);
			lp.proc(&ate[4],2);
			}

		opt[0] = pt[1];
		opt[1] = pt[3];
		opt[2] = pt[5];

		oats[0] = ats[1];
		oats[1] = ats[3];
		oats[2] = ats[5];

		oate[0] = ate[1];
		oate[1] = ate[3];
		oate[2] = ate[5];

		}
	if (sel && uvwProy[channel]->attOn && !(uvwProy[channel]->attWs==0.0f && uvwProy[channel]->attWo==0.0f)  && showAtt) {
		pats[1] = Point3(0.0f,0.0f,0.0f);
		pats[4] = Point3(0.0f,0.0f,0.0f);
		pate[1] = Point3(0.0f,0.0f,0.0f);
		pate[4] = Point3(0.0f,0.0f,0.0f);
		lp.SetLineColor(GetUIColor(COLOR_START_RANGE));
		lp.proc(pats,3);
		lp.proc(&pats[3],3);
		lp.SetLineColor(GetUIColor(COLOR_END_RANGE));
		lp.proc(pate,3);
		lp.proc(&pate[3],3);
		}

	}


void MultiMapMod::DoMMCylindricalMapIcon(int channel, TimeValue t, BOOL sel,float radius, float height, PolyLineProc& lp)
	{
	float u,v,w;
	Point3 pt[5], opt;
	Point3 aux[2];
	Point3 ats[5], oats;
	Point3 ate[5], oate;
	Point3 pas[3], pae[3];
	BOOL warning = 0;

	float atus = uvwProy[channel]->attUs;
	float atuo = uvwProy[channel]->attUo;
	float atvs = uvwProy[channel]->attVs;
	float atvo = uvwProy[channel]->attVo;

	if (atus==0.0f && atuo==0.0) atus = 1.0f;
	if (atvs==0.0f && atvo==0.0) atvs = 1.0f;

	float asr = (radius*uvwProy[channel]->attWs / uvwProy[channel]->tile_w)+(uvwProy[channel]->offset_w/uvwProy[channel]->tile_w);
	float aer = (radius*(uvwProy[channel]->attWs+uvwProy[channel]->attWo) / uvwProy[channel]->tile_w)+(uvwProy[channel]->offset_w/uvwProy[channel]->tile_w);

	if ((uvwProy[channel]->attWs==0.0f && uvwProy[channel]->attWo==0.0f)) {
		asr = radius;
		aer = radius;
		}

	if (sel) lp.SetLineColor(GetUIColor(COLOR_SEL_GIZMOS));
	else lp.SetLineColor(GetUIColor(COLOR_GIZMOS));		
	
	height *= 0.5f;

	float angs = ((uvwProy[channel]->offset_u/uvwProy[channel]->tile_u) * TWOPI) + PI;
	if (angs < PI) {
		angs = PI;
		warning = 1;
		}

	if (angs > PI * 3.0f) {
		angs = PI * 3.0f;
		warning = 1;
		}

	float ange = (((uvwProy[channel]->offset_u/uvwProy[channel]->tile_u) + (1.0f/uvwProy[channel]->tile_u)) * TWOPI) + PI;
	if (ange > PI * 3.0f) {
		ange = PI * 3.0f;
		warning = 1;
		}

	if (ange < PI) {
		ange = PI;
		warning = 1;
		}

	if (uvwProy[channel]->tile_u == 1.0f && uvwProy[channel]->offset_u == 0.0f) warning = 0;

	float auangs = angs+(ange-angs)/2.0f-atus*(ange-angs)/2.0f;
	if (auangs < PI) auangs = PI;
	if (auangs > PI * 3.0f) auangs = PI * 3.0f;

	float auange = angs+(ange-angs)/2.0f+atus*(ange-angs)/2.0f;
	if (auange < PI) auange = PI;
	if (auange > PI * 3.0f) auange = PI * 3.0f;

	float avangs = angs+(ange-angs)/2.0f-(atus+atuo)*(ange-angs)/2.0f;
	if (avangs < PI) avangs = PI;
	if (avangs > PI * 3.0f) avangs = PI * 3.0f;

	float avange = angs+(ange-angs)/2.0f+(atus+atuo)*(ange-angs)/2.0f;
	if (avange < PI) avange = PI;
	if (avange > PI * 3.0f) avange = PI * 3.0f;

	float hs = -height + uvwProy[channel]->offset_v/uvwProy[channel]->tile_v;
	float he = -height + 2.0f * height / uvwProy[channel]->tile_v + uvwProy[channel]->offset_v/uvwProy[channel]->tile_v;
	float hmse = hs + (he-hs)/2.0f;

	float auhs = hs+(he-hs)/2.0f-atvs*(he-hs)/2.0f;
	float auhe = hs+(he-hs)/2.0f+atvs*(he-hs)/2.0f;
	float hmauhe = auhs + (auhe-auhs)/2.0f;

	float afuhs = hs+(he-hs)/2.0f-(atvs+atvo)*(he-hs)/2.0f;
	float afuhe = hs+(he-hs)/2.0f+(atvs+atvo)*(he-hs)/2.0f;
	float hmafuhe = afuhs + (afuhe-afuhs)/2.0f;


	opt = Point3(float(radius * cos(angs)),float(radius * sin(angs)),he);
	oats = Point3(float(asr*cos(auangs)),float(asr*sin(auangs)),auhe);
	oate = Point3(float(aer*cos(avangs)),float(aer*sin(avangs)),afuhe);
	pas[0] = oats;
	pae[0] = oate;
	for (int i=1; i<=NUM_SEGS; i++) {
		u = angs + ((ange - angs)/float(NUM_SEGS))*float(i);
		

		if (i == 1) {
			pt[0] = opt;
			pt[1] = opt;
			pt[1].z = hs;
			if (sel) lp.SetLineColor(GetUIColor(COLOR_SEL_GIZMOS));
			else lp.SetLineColor(GetUIColor(COLOR_GIZMOS));		
			if (warning) lp.SetLineColor(1.0f,0.0f,0.0f);
			lp.proc(pt,2);
			}

		pt[0]   = opt;
		
		pt[1].x = (float)cos(u) * radius;
		pt[1].y = (float)sin(u) * radius;
		pt[1].z = he;

		pt[2].x = pt[1].x;
		pt[2].y = pt[1].y;
		pt[2].z = hs;

		aux[0] = pt[0];
		aux[0].z = hmse;
		aux[1] = pt[1];
		aux[1].z = hmse;

		pt[3]   = opt;
		pt[3].z = hs;

		if (i == NUM_SEGS) {
			if (sel) lp.SetLineColor(0.0f,0.8f,0.0f);
			lp.proc(&pt[1],2);
			}
		if (sel) lp.SetLineColor(GetUIColor(COLOR_SEL_GIZMOS));
		else lp.SetLineColor(GetUIColor(COLOR_GIZMOS));		
		if (warning) lp.SetLineColor(1.0f,0.0f,0.0f);
		lp.proc(pt,2);
		lp.proc(&pt[2],2);
		lp.proc(aux,2);

		if (i == NUM_SEGS/2) {
			aux[0] = pt[1];
			aux[1] = pt[2];
			aux[0].z += (he-hs)*0.2f;
			lp.proc(aux,2);
			}

		if(sel && uvwProy[channel]->attOn) {

			v = auangs + ((auange - auangs)/float(NUM_SEGS))*float(i);
			w = avangs + ((avange - avangs)/float(NUM_SEGS))*float(i);

			if (i == 1) {
				ats[0] = oats;
				ats[1] = oats;
				ats[1].z = auhs;
				
				lp.SetLineColor(GetUIColor(COLOR_START_RANGE));
				lp.proc(ats,2);

				ate[0] = oate;
				ate[1] = oate;
				ate[1].z = afuhs;
				
				lp.SetLineColor(GetUIColor(COLOR_END_RANGE));
				lp.proc(ate,2);
				}

			ats[0]  = oats;

			ate[0]  = oate;

			ats[1].x= (float)cos(v) * asr;
			ats[1].y= (float)sin(v) * asr;
			ats[1].z= auhe;

			ate[1].x= (float)cos(w) * aer;
			ate[1].y= (float)sin(w) * aer;
			ate[1].z= afuhe;

			ats[2].x= ats[1].x;
			ats[2].y= ats[1].y;
			ats[2].z= auhs;

			ate[2].x= ate[1].x;
			ate[2].y= ate[1].y;
			ate[2].z= afuhs;

			ats[3]  = oats;
			ats[3].z= auhs;

			ate[3]  = oate;
			ate[3].z= afuhs;

			if (i == NUM_SEGS) {
				lp.SetLineColor(GetUIColor(COLOR_START_RANGE));
				lp.proc(&ats[1],2);
			
				if (!(uvwProy[channel]->attWs==0.0f && uvwProy[channel]->attWo==0.0f)) {
					pas[1] = Point3(0.0f,0.0f,auhe);
					pas[2] = ats[1];
					lp.proc(pas,3);
					aux[0] = pas[1];
					for(int n=0;n<3;n++) pas[n].z = auhs;
					lp.proc(pas,3);
					aux[1] = pas[1];
					lp.proc(aux,2);
					}
				

				lp.SetLineColor(GetUIColor(COLOR_END_RANGE));
				lp.proc(&ate[1],2);

				if (!(uvwProy[channel]->attWs==0.0f && uvwProy[channel]->attWo==0.0f)) {
					pae[1] = Point3(0.0f,0.0f,afuhe);
					pae[2] = ate[1];
					lp.proc(pae,3);
					aux[0] = pae[1];
					for(int n=0;n<3;n++) pae[n].z = afuhs;
					lp.proc(pae,3);
					aux[1] = pae[1];
					lp.proc(aux,2);
					}
				}

			if (i == NUM_SEGS/2) {
				lp.SetLineColor(GetUIColor(COLOR_START_RANGE));
				lp.proc(&ats[1],2);
				lp.SetLineColor(GetUIColor(COLOR_END_RANGE));
				lp.proc(&ate[1],2);
				}


			aux[0] = ats[0];
			aux[0].z = hmauhe;
			aux[1] = ats[1];
			aux[1].z = hmauhe;
			lp.SetLineColor(GetUIColor(COLOR_START_RANGE));
			lp.proc(aux,2);
			lp.proc(ats,2);
			lp.proc(&ats[2],2);

			aux[0] = ate[0];
			aux[0].z = hmafuhe;
			aux[1] = ate[1];
			aux[1].z = hmafuhe;
			lp.SetLineColor(GetUIColor(COLOR_END_RANGE));
			lp.proc(aux,2);
			lp.proc(ate,2);
			lp.proc(&ate[2],2);
			}
		opt = pt[1];
		oats= ats[1];
		oate= ate[1];
		}

	}


void MultiMapMod::DoMMPlanarMapIcon(int channel, TimeValue t, BOOL sel,float width, float length, float height, PolyLineProc& lp)
	{
	Point3 pt[6];
	Point3 ats[5];
	Point3 ate[5];
	Point3 aux[2];

	width  *= 0.5f;
	length *= 0.5f;
	height *= 0.5f;

	float atus = uvwProy[channel]->attUs;
	float atuo = uvwProy[channel]->attUo;
	float atvs = uvwProy[channel]->attVs;
	float atvo = uvwProy[channel]->attVo;

	if (atus==0.0f && atuo==0.0) atus = 1.0f;
	if (atvs==0.0f && atvo==0.0) atvs = 1.0f;

	float tus = -width + uvwProy[channel]->offset_u*(2.0f*width)/uvwProy[channel]->tile_u;
	float tue = -width + uvwProy[channel]->offset_u*(2.0f*width)/uvwProy[channel]->tile_u + 2.0f/uvwProy[channel]->tile_u;
	float tvs = -length + uvwProy[channel]->offset_v*(2.0f*length)/uvwProy[channel]->tile_v;
	float tve = -length + uvwProy[channel]->offset_v*(2.0f*length)/uvwProy[channel]->tile_v + 2.0f/uvwProy[channel]->tile_v;

	float asus = tus + (tue-tus)/2.0f - (tue-tus)*atus/2.0f;
	float asue = tus + (tue-tus)/2.0f + (tue-tus)*atus/2.0f;
	float asvs = tvs + (tve-tvs)/2.0f - (tve-tvs)*atvs/2.0f;
	float asve = tvs + (tve-tvs)/2.0f + (tve-tvs)*atvs/2.0f;

	float aeus = tus + (tue-tus)/2.0f - (tue-tus)*(atus+atuo)/2.0f;
	float aeue = tus + (tue-tus)/2.0f + (tue-tus)*(atus+atuo)/2.0f;
	float aevs = tvs + (tve-tvs)/2.0f - (tve-tvs)*(atvs+atvo)/2.0f;
	float aeve = tvs + (tve-tvs)/2.0f + (tve-tvs)*(atvs+atvo)/2.0f;

	if (sel) lp.SetLineColor(GetUIColor(COLOR_SEL_GIZMOS));
	else lp.SetLineColor(GetUIColor(COLOR_GIZMOS));		
	
	pt[0] = Point3(tus+(tue-tus)/2.0f,tve,0.0f);
	pt[1] = Point3(tus+(tue-tus)/2.0f,tve + (tve-tvs)*0.2f,0.0f);
	lp.proc(pt,2);

	pt[0].x = tus;
	pt[0].y = tvs;
	pt[0].z = 0.0f;

	pt[1].x = tue;
	pt[1].y = tvs;
	pt[1].z = 0.0f;

	pt[2].x = tue;
	pt[2].y = tve;
	pt[2].z = 0.0f;

	pt[3].x = tus;
	pt[3].y = tve;
	pt[3].z = 0.0f;

	pt[4] = pt[0];

	if (sel) {
		lp.proc(pt,2);
		lp.SetLineColor(0.0f,0.8f,0.0f);
		lp.proc(&pt[1],2);
		lp.SetLineColor(GetUIColor(COLOR_SEL_GIZMOS));
		lp.proc(&pt[2],3);
		if (uvwProy[channel]->attOn) {
			ats[0].x = asus;
			ats[0].y = asvs;
			ats[0].z = 0.0f;

			ats[1].x = asue;
			ats[1].y = asvs;
			ats[1].z = 0.0f;

			ats[2].x = asue;
			ats[2].y = asve;
			ats[2].z = 0.0f;

			ats[3].x = asus;
			ats[3].y = asve;
			ats[3].z = 0.0f;

			ats[4] = ats[0];

			ate[0].x = aeus;
			ate[0].y = aevs;
			ate[0].z = 0.0f;

			ate[1].x = aeue;
			ate[1].y = aevs;
			ate[1].z = 0.0f;

			ate[2].x = aeue;
			ate[2].y = aeve;
			ate[2].z = 0.0f;

			ate[3].x = aeus;
			ate[3].y = aeve;
			ate[3].z = 0.0f;

			ate[4] = ate[0];

			if (uvwProy[channel]->attWs || uvwProy[channel]->attWo) {
				for( int k=0; k<4; k++) { 
					aux[0]  = ate[k];
					aux[0].z= (uvwProy[channel]->offset_w + height * (uvwProy[channel]->attWs + uvwProy[channel]->attWo)) / uvwProy[channel]->tile_w;
					aux[1]  = ate[k];
					aux[1].z= (uvwProy[channel]->offset_w - height * (uvwProy[channel]->attWs + uvwProy[channel]->attWo)) / uvwProy[channel]->tile_w;
					lp.SetLineColor(GetUIColor(COLOR_END_RANGE));
					lp.proc(aux,2);

					aux[0]  = ats[k];
					aux[0].z= (uvwProy[channel]->offset_w + height * uvwProy[channel]->attWs) / uvwProy[channel]->tile_w;
					aux[1]  = ats[k];
					aux[1].z= (uvwProy[channel]->offset_w - height * uvwProy[channel]->attWs) / uvwProy[channel]->tile_w;
					lp.SetLineColor(GetUIColor(COLOR_START_RANGE));
					lp.proc(aux,2);
					}

				for( int k=0; k<5; k++ ) {
					ate[k].z = (uvwProy[channel]->offset_w + height * (uvwProy[channel]->attWs + uvwProy[channel]->attWo)) / uvwProy[channel]->tile_w;
					ats[k].z = (uvwProy[channel]->offset_w + height * uvwProy[channel]->attWs) / uvwProy[channel]->tile_w;
					}
				lp.SetLineColor(GetUIColor(COLOR_END_RANGE));
				lp.proc(ate,5);
				lp.SetLineColor(GetUIColor(COLOR_START_RANGE));
				lp.proc(ats,5);

				for( int k=0; k<5; k++) {
					ate[k].z = (uvwProy[channel]->offset_w - height * (uvwProy[channel]->attWs + uvwProy[channel]->attWo)) / uvwProy[channel]->tile_w;
					ats[k].z = (uvwProy[channel]->offset_w - height * uvwProy[channel]->attWs) / uvwProy[channel]->tile_w;
					}
				}
			lp.SetLineColor(GetUIColor(COLOR_END_RANGE));
			lp.proc(ate,5);
			lp.SetLineColor(GetUIColor(COLOR_START_RANGE));
			lp.proc(ats,5);
			}
	} else {
		lp.proc(pt,5);
		}
	}

int MultiMapMod::DoIcon(TimeValue t, PolyLineProc& lp, int channel, BOOL sel, int level, 
						GraphicsWindow *gw, ViewExp *vpt, INode *inode, ModContext* mc,
						int hit, int hitflags)
	{
	int type = uvwProy[channel]->GetMappingType();
	int res = 0;
	switch (type) {
		case MAP_TL_BOX:				DoBoxIcon(channel,sel,2.0f,lp);											break;
		case MAP_TL_PLANAR:				DoMMPlanarMapIcon(channel, t, sel,2.0f,2.0f,1.0f,lp);					break;
		case MAP_TL_BALL:				DoMMShrinkMapIcon(channel, t,sel,1.0f,lp);								break;
		case MAP_TL_SPHERICAL:			DoMMSphericalMapIcon(channel, t,sel,1.0f,lp);							break;
		case MAP_TL_CYLINDRICAL:		DoMMCylindricalMapIcon(channel, t,sel,1.0f,1.0f,lp);					break;
		case MAP_TL_CYLCAP:				DoMMCylindricalMapIcon(channel, t,sel,1.0f,1.0f,lp); 					break;
		case MAP_TL_FFM:				res = DoFFMIcon(channel, t,sel,lp,level,vpt,gw,inode,mc,hit,hitflags);	break;
		case MAP_TL_SPLINE:				DoMMSplineIcon(channel, t,sel,lp);										break;
		}
	return res;
	}


void MultiMapMod::DoBoxIcon(int channel, BOOL sel,float length, PolyLineProc& lp)
	{
	int lStart[12] = {0,1,3,2,4,5,7,6,0,1,2,3};
	int lEnd[12]   = {1,3,2,0,5,7,6,4,4,5,6,7};
	Point3 pt[3];
	
	length *= 0.5f;
	Box3 box;
	box.pmin = Point3(-length,-length,-length);
	box.pmax = Point3( length, length, length);

	if (sel) //lp.SetLineColor(1.0f,1.0f,0.0f);
		 lp.SetLineColor(GetUIColor(COLOR_SEL_GIZMOS));
	else //lp.SetLineColor(0.85f,0.5f,0.0f);		
		 lp.SetLineColor(GetUIColor(COLOR_GIZMOS));

	for (int i=0; i<12; i++) {
		pt[0] = box[lStart[i]];
		pt[1] = box[lEnd[i]];
		lp.proc(pt,2);
		}
	}


#define NUM_STEPS 6

void MultiMapMod::DoMMSplineIcon(int channel, TimeValue t, BOOL sel, PolyLineProc &lp) {
	if (!uvwProy[channel]->spline_node) 
		return;

	float atus = uvwProy[channel]->attUs;
	float atuo = uvwProy[channel]->attUo;
	float atvs = uvwProy[channel]->attVs;
	float atvo = uvwProy[channel]->attVo;

	if (atus==0.0f && atuo==0.0) atus = 1.0f;
	if (atvs==0.0f && atvo==0.0) atvs = 1.0f;

	Point3 pt[2];
	Point3 ats[2];
	Point3 ate[2];
	int numsegs = uvwProy[channel]->GetSplNumSegs();

	float atrs,atre;
	atrs = (uvwProy[channel]->attWs / uvwProy[channel]->tile_w)+(uvwProy[channel]->offset_w/uvwProy[channel]->tile_w);
	atre = ((uvwProy[channel]->attWs+uvwProy[channel]->attWo) / uvwProy[channel]->tile_w)+(uvwProy[channel]->offset_w/uvwProy[channel]->tile_w);

	if (uvwProy[channel]->attWs==0.0f && uvwProy[channel]->attWo==0.0f) {
		atrs = 1.0f;
		atre = 1.0f;
		}

	int i,j;
	float u,v;
	float au,av,bu,bv;

	if ( uvwProy[channel]->belt ) {
		pt[0] = uvwProy[channel]->GetPosition(t,0.5f,1.0f,0.5f);
		pt[1] = uvwProy[channel]->GetPosition(t,0.5f,1.05f,0.5f);
		if (sel) lp.SetLineColor(GetUIColor(COLOR_SEL_GIZMOS));
		else lp.SetLineColor(GetUIColor(COLOR_GIZMOS));
		lp.proc(pt,2);

		for (i=0; i<2; i++) {
			u = float(i);
			pt[0] = uvwProy[channel]->GetPosition(t,u,0.0f,0.5f);

			au = (0.5f - 0.5f * atus) + (atus * u);
			ats[0] = uvwProy[channel]->GetPosition(t,au,0.0f,0.5f);

			bu = (0.5f - 0.5f * atus - 0.5f * atuo) + ((atus + atuo) * u);
			ate[0] = uvwProy[channel]->GetPosition(t,bu,0.0f,0.5f);

			for (j=0; j<NUM_STEPS*numsegs; j++) {
				v = float(j)/float(NUM_STEPS*numsegs-1);
				pt[1] = uvwProy[channel]->GetPosition(t,u,v,0.5f);

				if (sel)	lp.SetLineColor(GetUIColor(COLOR_SEL_GIZMOS));
				else		lp.SetLineColor(GetUIColor(COLOR_GIZMOS));
				lp.proc(pt,2);

				if (sel && uvwProy[channel]->attOn) {
					ats[1] = uvwProy[channel]->GetPosition(t,au,v,0.5f);
					lp.SetLineColor(GetUIColor(COLOR_START_RANGE));
					lp.proc(ats,2);

					ate[1] = uvwProy[channel]->GetPosition(t,bu,v,0.5f);
					lp.SetLineColor(GetUIColor(COLOR_END_RANGE));
					lp.proc(ate,2);
					}

				pt[0] = pt[1];
				ats[0] = ats[1];
				ate[0] = ate[1];
				}
			}
		if (sel)	lp.SetLineColor(GetUIColor(COLOR_SEL_GIZMOS));
		else		lp.SetLineColor(GetUIColor(COLOR_GIZMOS));
		pt[0] = uvwProy[channel]->GetPosition(t,0.0f,0.0f,0.5f);
		pt[1] = uvwProy[channel]->GetPosition(t,1.0f,0.0f,0.5f);
		lp.proc(pt,2);
		pt[0] = uvwProy[channel]->GetPosition(t,0.0f,1.0f,0.5f);
		pt[1] = uvwProy[channel]->GetPosition(t,1.0f,1.0f,0.5f);
		lp.proc(pt,2);

		// Paint the Normals
		for (i=0; i<NUM_STEPS*numsegs; i++) {
			float v = float(i)/float(NUM_STEPS*numsegs-1);
			Point3 ns,ne;
			
			uvwProy[channel]->GetUVWSplinePoint(t,0.5f,v,1.0f,ns,ne);

			pt[0] = ns;
			pt[1] = ne;

			lp.SetLineColor(0.2f,0.2f,1.0f);
			lp.proc(pt,2);
			}
		}
	else {
		BOOL warning = (uvwProy[channel]->tile_u - uvwProy[channel]->offset_u < 1.0f);
		// The little line that goes over the gizmo...
		pt[0] = uvwProy[channel]->GetPosition(t,0.5f,1.0f,1.0f);
		pt[1] = uvwProy[channel]->GetPosition(t,0.5f,1.05f,1.0f);
		if (sel) lp.SetLineColor(GetUIColor(COLOR_SEL_GIZMOS));
		else lp.SetLineColor(GetUIColor(COLOR_GIZMOS));
		if ( warning )
			lp.SetLineColor(1.0f,0.0f,0.0f);
		lp.proc(pt,2);

		for (i=0; i<5; i++) {
			u = float(i)/4.0f;
			pt[0] = uvwProy[channel]->GetPosition(t,u,0.0f,1.0f);

			au = (0.5f - 0.5f * atus) + (atus * u);
			av = (0.5f - 0.5f * atvs);
			ats[0] = uvwProy[channel]->GetPosition(t,au,av,atrs);

			bu = (0.5f - 0.5f * atus - 0.5f * atuo) + ((atus + atuo) * u);
			bv = (0.5f - 0.5f * atvs - 0.5f * atvo);
			ate[0] = uvwProy[channel]->GetPosition(t,bu,bv,atre);

			for (j=0; j<NUM_STEPS*numsegs; j++) {
				v = float(j)/float(NUM_STEPS*numsegs-1);
				pt[1] = uvwProy[channel]->GetPosition(t,u,v,1.0f);


				if (sel) {
					if (i == 9) lp.SetLineColor(0.0f,0.8f,0.0f);
					else lp.SetLineColor(GetUIColor(COLOR_SEL_GIZMOS));
					}
				else lp.SetLineColor(GetUIColor(COLOR_GIZMOS));
				if (warning) lp.SetLineColor(1.0f,0.0f,0.0f);
				lp.proc(pt,2);

				if (sel && uvwProy[channel]->attOn) {
					av = (0.5f - 0.5f * atvs) + (atvs * v);
					ats[1] = uvwProy[channel]->GetPosition(t,au,av,atrs);
					lp.SetLineColor(GetUIColor(COLOR_START_RANGE));
					lp.proc(ats,2);

					bv = (0.5f - 0.5f * atvs - 0.5f * atvo) + ((atvs + atvo) * v);
					ate[1] = uvwProy[channel]->GetPosition(t,bu,bv,atre);
					lp.SetLineColor(GetUIColor(COLOR_END_RANGE));
					lp.proc(ate,2);
					}

				pt[0] = pt[1];
				ats[0] = ats[1];
				ate[0] = ate[1];
				}
			}

		for (j=0; j<6; j++) {
			v = float(j)/5.0f;
			pt[0] = uvwProy[channel]->GetPosition(t,0.0f,v,1.0f);
				
			au = (0.5f - 0.5f * atus);
			av = (0.5f - 0.5f * atvs) + (atvs * v);
			ats[0] = uvwProy[channel]->GetPosition(t,au,av,atrs);

			bu = (0.5f - 0.5f * atus - 0.5f * atuo);
			bv = (0.5f - 0.5f * atvs - 0.5f * atvo) + ((atvs + atvo) * v);
			ate[0] = uvwProy[channel]->GetPosition(t,bu,bv,atre);

			for (i=0; i<NUM_SEGS; i++) {
				u = float(i)/float(NUM_SEGS-1);
				pt[1] = uvwProy[channel]->GetPosition(t,u,v,1.0f);
				if (sel) lp.SetLineColor(GetUIColor(COLOR_SEL_GIZMOS));
				else lp.SetLineColor(GetUIColor(COLOR_GIZMOS));
				if (warning) lp.SetLineColor(1.0f,0.0f,0.0f);
				lp.proc(pt,2);
				pt[0] = pt[1];

				if (sel && uvwProy[channel]->attOn) {
					au = (0.5f - 0.5f * atus) + (atus * u);
					ats[1] = uvwProy[channel]->GetPosition(t,au,av,atrs);
					lp.SetLineColor(GetUIColor(COLOR_START_RANGE));
					lp.proc(ats,2);
					ats[0] = ats[1];

					bu = (0.5f - 0.5f * atus - 0.5f * atuo) + ((atus + atuo) * u);
					ate[1] = uvwProy[channel]->GetPosition(t,bu,bv,atre);
					lp.SetLineColor(GetUIColor(COLOR_END_RANGE));
					lp.proc(ate,2);
					ate[0] = ate[1];
					}
				}

			if (sel && uvwProy[channel]->attOn && !(uvwProy[channel]->attWs==0.0f && uvwProy[channel]->attWo==0.0f)) {
				au = (0.5f - 0.5f * atus);
				av = (0.5f - 0.5f * atvs) + (atvs * v);
				ats[0] = uvwProy[channel]->GetPosition(t,au,av,atrs);
				ats[1] = uvwProy[channel]->GetPosition(t,au,av,0.0f);
				lp.SetLineColor(GetUIColor(COLOR_START_RANGE));
				lp.proc(ats,2);

				au = (0.5f - 0.5f * atus) + (atus);
				ats[0] = uvwProy[channel]->GetPosition(t,au,av,atrs);
				lp.SetLineColor(GetUIColor(COLOR_START_RANGE));
				lp.proc(ats,2);

				bu = (0.5f - 0.5f * atus - 0.5f * atuo);
				bv = (0.5f - 0.5f * atvs - 0.5f * atvo) + ((atvs + atvo) * v);
				ate[0] = uvwProy[channel]->GetPosition(t,bu,bv,atre);
				ate[1] = uvwProy[channel]->GetPosition(t,bu,bv,0.0f);
				lp.SetLineColor(GetUIColor(COLOR_END_RANGE));
				lp.proc(ate,2);

				bu = (0.5f - 0.5f * atus - 0.5f * atuo) + ((atus + atuo));
				ate[0] = uvwProy[channel]->GetPosition(t,bu,bv,atre);
				lp.SetLineColor(GetUIColor(COLOR_END_RANGE));
				lp.proc(ate,2);
				}
			}
		// Paint the Normals
		for (i=0; i<NUM_STEPS*numsegs; i++) {
			float v = float(i)/float(NUM_STEPS*numsegs-1);
			Point3 ns,ne;
			
			uvwProy[channel]->GetUVWSplinePoint(t,0.0f,v,0.0f,ns,ne);

			pt[0] = ns;
			pt[1] = ne;

			lp.SetLineColor(0.2f,0.2f,1.0f);
			lp.proc(pt,2);

			if (i<NUM_STEPS*numsegs-1) {
				float v1 = float(i+1)/float(NUM_STEPS*numsegs-1);
				pt[0] = uvwProy[channel]->GetPosition(t,0.0f,v ,0.0f);
				pt[1] = uvwProy[channel]->GetPosition(t,0.0f,v1,0.0f);
		
				lp.SetLineColor(0.6f,0.6f,1.0f);
				lp.proc(pt,2);
				}
			}

		}
	}

int MultiMapMod::DoFFMIcon(int channel, TimeValue t, BOOL sel, PolyLineProc &lp, int level, ViewExp *vpt, 
						   GraphicsWindow *gw, INode *inode, ModContext* mc, 
						   int hit, int hitflags) {
	uvwProy[channel]->gm->Update(t);

	int res = 0;
	Point3 pt[2];
	Point3 qt[2];
	Point3 rt[2];
	Point3 aspt[2];
	Point3 asqt[2];
	Point3 asrt[2];
	Point3 aept[2];
	Point3 aeqt[2];
	Point3 aert[2];
	int i,j;

	if (gw && level == SEL_GIZMO) {
		lp.SetLineColor(GetUIColor(COLOR_SEL_GIZMOS));
		gw->setColor(LINE_COLOR,GetUIColor(COLOR_SEL_GIZMOS));
	}
	if (gw && level == SEL_OBJECT) {
		lp.SetLineColor(GetUIColor(COLOR_GIZMOS));
		gw->setColor(LINE_COLOR,GetUIColor(COLOR_GIZMOS));
		}

	// Display the Points
	Point3 p;
	if (gw)
		for (i=0; i<uvwProy[channel]->gm->verts.Count(); i++) {
			if (hit && hitflags&HIT_SELONLY   && !uvwProy[channel]->gm->sel[i]) continue;
			if (hit && hitflags&HIT_UNSELONLY &&  uvwProy[channel]->gm->sel[i]) continue;

			if (gw && level==SEL_POINTS) {
				if ( uvwProy[channel]->gm->sel[i]) //gw->setColor(LINE_COLOR, (float)1, (float)1, (float)0.0);
					gw->setColor(LINE_COLOR,GetUIColor(COLOR_SEL_GIZMOS));
				if (!uvwProy[channel]->gm->sel[i]) //gw->setColor(LINE_COLOR, (float).85, (float).5, (float)0.0);
					gw->setColor(LINE_COLOR,GetUIColor(COLOR_GIZMOS));
				}

			p = uvwProy[channel]->gm->verts[i];
			gw->marker(&p,HOLLOW_BOX_MRKR);
			if (hit && gw->checkHitCode()) {	
				gw->clearHitCode();
				vpt->LogHit(inode, mc, gw->getHitDistance(), i, NULL); 
				res = 1;
				if (hitflags&HIT_ABORTONHIT) return res;
				if (level==SEL_GIZMO) return 1;
				}
			}

	// Display the grid
	float attWs = uvwProy[channel]->GetHeight(t) * uvwProy[channel]->GetAttWStart(t);
	float attWe = uvwProy[channel]->GetHeight(t) * (uvwProy[channel]->GetAttWStart(t) + uvwProy[channel]->GetAttWOffset(t));
	BOOL attWdisplay = (level!=SEL_POINTS && uvwProy[channel]->attOn && (uvwProy[channel]->GetAttWStart(t) != 0.0f || uvwProy[channel]->GetAttWOffset(t) != 0.0f));
	int nx = uvwProy[channel]->gm->nx;
	int ny = uvwProy[channel]->gm->ny;

	if (hit && level != SEL_GIZMO) return res;

	for (i=0; i<nx; i++)
		for (j=0; j<ny; j++) {
			pt[0] = uvwProy[channel]->gm->GetVert(i,j);
			if (attWdisplay) {	
				aspt[0] = uvwProy[channel]->gm->GetVert(i,j)+uvwProy[channel]->gm->vnormals[nx*j+i]*attWs;
				asqt[0] = uvwProy[channel]->gm->GetVert(i,j)-uvwProy[channel]->gm->vnormals[nx*j+i]*attWs;
				aept[0] = uvwProy[channel]->gm->GetVert(i,j)+uvwProy[channel]->gm->vnormals[nx*j+i]*attWe;
				aeqt[0] = uvwProy[channel]->gm->GetVert(i,j)-uvwProy[channel]->gm->vnormals[nx*j+i]*attWe;
				}
			if (i < nx-1) {
				pt[1] = uvwProy[channel]->gm->GetVert(i+1,j);
				if (sel) 
					lp.SetLineColor(GetUIColor(COLOR_SEL_GIZMOS));		
				else 
					lp.SetLineColor(GetUIColor(COLOR_GIZMOS));
				lp.proc(pt,2);

				if (attWdisplay) {	
					lp.SetLineColor(GetUIColor(COLOR_START_RANGE));
					aspt[1] = uvwProy[channel]->gm->GetVert(i+1,j)+uvwProy[channel]->gm->vnormals[nx*j+i+1]*attWs;
					asqt[1] = uvwProy[channel]->gm->GetVert(i+1,j)-uvwProy[channel]->gm->vnormals[nx*j+i+1]*attWs;
					lp.proc(aspt,2);
					lp.proc(asqt,2);
					lp.SetLineColor(GetUIColor(COLOR_END_RANGE));
					aept[1] = uvwProy[channel]->gm->GetVert(i+1,j)+uvwProy[channel]->gm->vnormals[nx*j+i+1]*attWe;
					aeqt[1] = uvwProy[channel]->gm->GetVert(i+1,j)-uvwProy[channel]->gm->vnormals[nx*j+i+1]*attWe;
					lp.proc(aept,2);
					lp.proc(aeqt,2);
					}
				}
			if (j < ny-1) {
				pt[1] = uvwProy[channel]->gm->GetVert(i,j+1);
				if (sel) 
					if (i == nx-1) lp.SetLineColor(0.0f,0.8f,0.0f);
					else lp.SetLineColor(GetUIColor(COLOR_SEL_GIZMOS));		
				else {
					lp.SetLineColor(GetUIColor(COLOR_GIZMOS));
					}
				lp.proc(pt,2);

				if (attWdisplay) {
					lp.SetLineColor(GetUIColor(COLOR_START_RANGE));
					aspt[1] = uvwProy[channel]->gm->GetVert(i,j+1)+uvwProy[channel]->gm->vnormals[nx*(j+1)+i]*attWs;
					asqt[1] = uvwProy[channel]->gm->GetVert(i,j+1)-uvwProy[channel]->gm->vnormals[nx*(j+1)+i]*attWs;
					lp.proc(aspt,2);
					lp.proc(asqt,2);
					lp.SetLineColor(GetUIColor(COLOR_END_RANGE));
					aept[1] = uvwProy[channel]->gm->GetVert(i,j+1)+uvwProy[channel]->gm->vnormals[nx*(j+1)+i]*attWe;
					aeqt[1] = uvwProy[channel]->gm->GetVert(i,j+1)-uvwProy[channel]->gm->vnormals[nx*(j+1)+i]*attWe;
					lp.proc(aept,2);
					lp.proc(aeqt,2);
					}
				}
			}

	if (attWdisplay) {
		lp.SetLineColor(GetUIColor(COLOR_START_RANGE));
		aspt[0] = uvwProy[channel]->gm->GetVert(0,0)+uvwProy[channel]->gm->vnormals[0]*attWs;
		aspt[1] = uvwProy[channel]->gm->GetVert(0,0)-uvwProy[channel]->gm->vnormals[0]*attWs;
		lp.proc(aspt,2);
		aspt[0] = uvwProy[channel]->gm->GetVert(nx-1,0)+uvwProy[channel]->gm->vnormals[nx-1]*attWs;
		aspt[1] = uvwProy[channel]->gm->GetVert(nx-1,0)-uvwProy[channel]->gm->vnormals[nx-1]*attWs;
		lp.proc(aspt,2);
		aspt[0] = uvwProy[channel]->gm->GetVert(0,ny-1)+uvwProy[channel]->gm->vnormals[nx*(ny-1)]*attWs;
		aspt[1] = uvwProy[channel]->gm->GetVert(0,ny-1)-uvwProy[channel]->gm->vnormals[nx*(ny-1)]*attWs;
		lp.proc(aspt,2);
		aspt[0] = uvwProy[channel]->gm->GetVert(nx-1,ny-1)+uvwProy[channel]->gm->vnormals[nx*(ny-1)+(nx-1)]*attWs;
		aspt[1] = uvwProy[channel]->gm->GetVert(nx-1,ny-1)-uvwProy[channel]->gm->vnormals[nx*(ny-1)+(nx-1)]*attWs;
		lp.proc(aspt,2);

		lp.SetLineColor(GetUIColor(COLOR_END_RANGE));
		aept[0] = uvwProy[channel]->gm->GetVert(0,0)+uvwProy[channel]->gm->vnormals[0]*attWe;
		aept[1] = uvwProy[channel]->gm->GetVert(0,0)-uvwProy[channel]->gm->vnormals[0]*attWe;
		lp.proc(aept,2);
		aept[0] = uvwProy[channel]->gm->GetVert(nx-1,0)+uvwProy[channel]->gm->vnormals[nx-1]*attWe;
		aept[1] = uvwProy[channel]->gm->GetVert(nx-1,0)-uvwProy[channel]->gm->vnormals[nx-1]*attWe;
		lp.proc(aept,2);
		aept[0] = uvwProy[channel]->gm->GetVert(0,ny-1)+uvwProy[channel]->gm->vnormals[nx*(ny-1)]*attWe;
		aept[1] = uvwProy[channel]->gm->GetVert(0,ny-1)-uvwProy[channel]->gm->vnormals[nx*(ny-1)]*attWe;
		lp.proc(aept,2);
		aept[0] = uvwProy[channel]->gm->GetVert(nx-1,ny-1)+uvwProy[channel]->gm->vnormals[nx*(ny-1)+(nx-1)]*attWe;
		aept[1] = uvwProy[channel]->gm->GetVert(nx-1,ny-1)-uvwProy[channel]->gm->vnormals[nx*(ny-1)+(nx-1)]*attWe;
		lp.proc(aept,2);
		}

	float u = 0;
	float tileUs,tileUe,tileVs,tileVe,attUs,attUe,attVs,attVe,aeUs,aeUe,aeVs,aeVe;

	tileUs = tileUe = tileVs = tileVe = attUs = attUe = attVs = attVe = aeUs = aeUe = aeVs = aeVe = 0;

	BOOL tileDisplay  = (uvwProy[channel]->tile_u != 1.0f || uvwProy[channel]->offset_u != 0.0f ||
						 uvwProy[channel]->tile_v != 1.0f || uvwProy[channel]->offset_v != 0.0f);
	BOOL attUVDisplay = (uvwProy[channel]->attUs != 1.0f || uvwProy[channel]->attUo != 0.0f ||
						 uvwProy[channel]->attVs != 1.0f || uvwProy[channel]->attVo != 0.0f);
	
	BOOL warning = 0;
	tileUs = 1.0f * uvwProy[channel]->offset_u / uvwProy[channel]->tile_u;
	tileUe = 1.0f * (uvwProy[channel]->offset_u + 1.0f) / uvwProy[channel]->tile_u;
	tileVs = 1.0f * uvwProy[channel]->offset_v / uvwProy[channel]->tile_v;
	tileVe = 1.0f * (uvwProy[channel]->offset_v + 1.0f) / uvwProy[channel]->tile_v;

	LimitToSafeTile(tileUs,warning);
	LimitToSafeTile(tileUe,warning);
	LimitToSafeTile(tileVs,warning);
	LimitToSafeTile(tileVe,warning);

	if (tileDisplay) {
		pt[0] = uvwProy[channel]->gm->GetPosition(t,tileUs,tileVs,0.0f);
		qt[0] = uvwProy[channel]->gm->GetPosition(t,tileUs,tileVe,0.0f);
		rt[0] = uvwProy[channel]->gm->GetPosition(t,tileUs,0.5f*(tileVe+tileVs),0.0f);
		}

	if (attUVDisplay) {
		attUs = 0.5f*(tileUs+tileUe)-uvwProy[channel]->attUs*((tileUe-tileUs))/2.0f;
		attUe = 0.5f*(tileUs+tileUe)+uvwProy[channel]->attUs*((tileUe-tileUs))/2.0f;
		attVs = 0.5f*(tileVs+tileVe)-uvwProy[channel]->attVs*((tileVe-tileVs))/2.0f;
		attVe = 0.5f*(tileVs+tileVe)+uvwProy[channel]->attVs*((tileVe-tileVs))/2.0f;

		aeUs = 0.5f*(tileUs+tileUe)-(uvwProy[channel]->attUs+uvwProy[channel]->attUo)*((tileUe-tileUs))/2.0f;
		aeUe = 0.5f*(tileUs+tileUe)+(uvwProy[channel]->attUs+uvwProy[channel]->attUo)*((tileUe-tileUs))/2.0f;
		aeVs = 0.5f*(tileVs+tileVe)-(uvwProy[channel]->attVs+uvwProy[channel]->attVo)*((tileVe-tileVs))/2.0f;
		aeVe = 0.5f*(tileVs+tileVe)+(uvwProy[channel]->attVs+uvwProy[channel]->attVo)*((tileVe-tileVs))/2.0f;

		LimitToSafeTile(attUs);
		LimitToSafeTile(attUe);
		LimitToSafeTile(attVs);
		LimitToSafeTile(attVe);

		LimitToSafeTile(aeUs);
		LimitToSafeTile(aeUe);
		LimitToSafeTile(aeVs);
		LimitToSafeTile(aeVe);

		aspt[0] = uvwProy[channel]->gm->GetPosition(t,attUs,attVs,0.0f);
		asqt[0] = uvwProy[channel]->gm->GetPosition(t,attUs,attVe,0.0f);
		asrt[0] = uvwProy[channel]->gm->GetPosition(t,attUs,0.5f*(attVe+attVs),0.0f);

		aept[0] = uvwProy[channel]->gm->GetPosition(t,aeUs,aeVs,0.0f);
		aeqt[0] = uvwProy[channel]->gm->GetPosition(t,aeUs,aeVe,0.0f);
		aert[0] = uvwProy[channel]->gm->GetPosition(t,aeUs,0.5f*(aeVe+aeVs),0.0f);
		}

	for (i=1; i<=nx*2; i++) {
		u = float(i)/float(nx*2);
		if (tileDisplay) {
			pt[1] = uvwProy[channel]->gm->GetPosition(t,tileUs+(tileUe-tileUs)*u,tileVs,0.0f);
			qt[1] = uvwProy[channel]->gm->GetPosition(t,tileUs+(tileUe-tileUs)*u,tileVe,0.0f);
			rt[1] = uvwProy[channel]->gm->GetPosition(t,tileUs+(tileUe-tileUs)*u,0.5f*(tileVe+tileVs),0.0f);
			lp.SetLineColor(0.8f,0.3f,1.0f);
			lp.proc(pt,2);
			lp.proc(qt,2);
			lp.proc(rt,2);
			pt[0] = pt[1];
			qt[0] = qt[1];
			rt[0] = rt[1];
			}
		if (attUVDisplay) {
			aspt[1] = uvwProy[channel]->gm->GetPosition(t,attUs+(attUe-attUs)*u,attVs,0.0f);
			asqt[1] = uvwProy[channel]->gm->GetPosition(t,attUs+(attUe-attUs)*u,attVe,0.0f);
			asrt[1] = uvwProy[channel]->gm->GetPosition(t,attUs+(attUe-attUs)*u,0.5f*(attVe+attVs),0.0f);
			lp.SetLineColor(GetUIColor(COLOR_START_RANGE));
			lp.proc(aspt,2);
			lp.proc(asqt,2);
			lp.proc(asrt,2);
			aspt[0] = aspt[1];
			asqt[0] = asqt[1];
			asrt[0] = asrt[1];

			aept[1] = uvwProy[channel]->gm->GetPosition(t,aeUs+(aeUe-aeUs)*u,aeVs,0.0f);
			aeqt[1] = uvwProy[channel]->gm->GetPosition(t,aeUs+(aeUe-aeUs)*u,aeVe,0.0f);
			aert[1] = uvwProy[channel]->gm->GetPosition(t,aeUs+(aeUe-aeUs)*u,0.5f*(aeVe+aeVs),0.0f);
			lp.SetLineColor(GetUIColor(COLOR_END_RANGE));
			lp.proc(aept,2);
			lp.proc(aeqt,2);
			lp.proc(aert,2);
			aept[0] = aept[1];
			aeqt[0] = aeqt[1];
			aert[0] = aert[1];
			}
		}

	if (tileDisplay) {
		pt[0] = uvwProy[channel]->gm->GetPosition(t,tileUs,tileVs,0.0f);
		qt[0] = uvwProy[channel]->gm->GetPosition(t,tileUe,tileVs,0.0f);
		rt[0] = uvwProy[channel]->gm->GetPosition(t,0.5f*(tileUe+tileUs),tileVs,0.0f);
		}

	if (attUVDisplay) {
		aspt[0] = uvwProy[channel]->gm->GetPosition(t,attUs,attVs,0.0f);
		asqt[0] = uvwProy[channel]->gm->GetPosition(t,attUe,attVs,0.0f);
		asrt[0] = uvwProy[channel]->gm->GetPosition(t,0.5f*(attUe+attUs),attVs,0.0f);

		aept[0] = uvwProy[channel]->gm->GetPosition(t,aeUs,aeVs,0.0f);
		aeqt[0] = uvwProy[channel]->gm->GetPosition(t,aeUe,aeVs,0.0f);
		aert[0] = uvwProy[channel]->gm->GetPosition(t,0.5f*(aeUe+aeUs),aeVs,0.0f);
		}

	for (i=1; i<=ny*2; i++) {
		u = float(i)/float(ny*2);
		if (tileDisplay) {
			pt[1] = uvwProy[channel]->gm->GetPosition(t,tileUs,tileVs+(tileVe-tileVs)*u,0.0f);
			qt[1] = uvwProy[channel]->gm->GetPosition(t,tileUe,tileVs+(tileVe-tileVs)*u,0.0f);
			rt[1] = uvwProy[channel]->gm->GetPosition(t,0.5f*(tileUe+tileUs),tileVs+(tileVe-tileVs)*u,0.0f);
			lp.SetLineColor(0.8f,0.3f,1.0f);
			lp.proc(pt,2);
			lp.proc(qt,2);
			lp.proc(rt,2);
			pt[0] = pt[1];
			qt[0] = qt[1];
			rt[0] = rt[1];
			}

		if (attUVDisplay) {
			aspt[1] = uvwProy[channel]->gm->GetPosition(t,attUs,attVs+(attVe-attVs)*u,0.0f);
			asqt[1] = uvwProy[channel]->gm->GetPosition(t,attUe,attVs+(attVe-attVs)*u,0.0f);
			asrt[1] = uvwProy[channel]->gm->GetPosition(t,0.5f*(attUe+attUs),attVs+(attVe-attVs)*u,0.0f);
			lp.SetLineColor(GetUIColor(COLOR_START_RANGE));
			lp.proc(aspt,2);
			lp.proc(asqt,2);
			lp.proc(asrt,2);
			aspt[0] = aspt[1];
			asqt[0] = asqt[1];
			asrt[0] = asrt[1];

			aept[1] = uvwProy[channel]->gm->GetPosition(t,aeUs,aeVs+(aeVe-aeVs)*u,0.0f);
			aeqt[1] = uvwProy[channel]->gm->GetPosition(t,aeUe,aeVs+(aeVe-aeVs)*u,0.0f);
			aert[1] = uvwProy[channel]->gm->GetPosition(t,0.5f*(aeUe+aeUs),aeVs+(aeVe-aeVs)*u,0.0f);
			lp.SetLineColor(GetUIColor(COLOR_END_RANGE));
			lp.proc(aept,2);
			lp.proc(aeqt,2);
			lp.proc(aert,2);
			aept[0] = aept[1];
			aeqt[0] = aeqt[1];
			aert[0] = aert[1];
			}
		}

	if (hit && gw->checkHitCode()) {
		gw->clearHitCode();
		vpt->LogHit(inode, mc, gw->getHitDistance(), i, NULL);
		return 1;
		}

	return 0;
	}

int MultiMapMod::HitTest( TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt, ModContext* mc) {
	selecting_faces = FALSE;

	int savedLimits;	
	GraphicsWindow *gw = vpt->getGW();
	HitRegion hr;
	MakeHitRegion(hr,type, crossing,4,p);
	gw->setHitRegion(&hr);
	Matrix3 modmat, ntm = inode->GetObjectTM(t);
	DrawLineProc lp(gw);
		
	gw->setRndLimits(((savedLimits = gw->getRndLimits()) | GW_PICK) & ~GW_ILLUM);
	gw->clearHitCode();

	int level = 0;
	if (ip)	level = ip->GetSubObjectLevel();
	
	if (ip && ip->GetSubObjectLevel() == SEL_GIZMO) {		
		modmat = uvwProy[current_channel]->CompMatrix(t,mc,&ntm);
		gw->setTransform(modmat);
		DoIcon(t,lp,current_channel,ip&&ip->GetSubObjectLevel()==SEL_GIZMO,level,gw,vpt,inode,mc,1,flags);
		}

	if (ip && ip->GetSubObjectLevel() == SEL_POINTS && uvwProy[current_channel]->GetMappingType() == MAP_TL_FFM) {
		modmat = uvwProy[current_channel]->CompMatrix(t,mc,&ntm);
		gw->setTransform(modmat);
		int res = DoIcon(t,lp,current_channel,ip&&ip->GetSubObjectLevel()==SEL_GIZMO,level,gw,vpt,inode,mc,1,flags);
		return res;
		}

	if (ip && (ip->GetSubObjectLevel() == SEL_FACES) ) {
		int res = 0;
		MakeHitRegion(hr,type, crossing,4,p);
		gw->setHitRegion(&hr);
		Matrix3 mat = inode->GetObjectTM(t);
		gw->setTransform(mat);	
		gw->setRndLimits(((savedLimits = gw->getRndLimits()) | GW_PICK) & ~GW_ILLUM);
		if ( type==HITTYPE_POINT )
			gw->setRndLimits(gw->getRndLimits() & GW_BACKCULL);
		else
			gw->setRndLimits(gw->getRndLimits() & ~GW_BACKCULL);

		gw->clearHitCode();

		BOOL add = GetKeyState(VK_CONTROL)<0;
		BOOL sub = GetKeyState(VK_MENU)<0;
		BOOL polySelect = !(GetKeyState(VK_SHIFT)<0);

		if (add)				ip->ReplacePrompt( GetString(IDS_DC_MOUSE_ADD));
		else if (sub)			ip->ReplacePrompt( GetString(IDS_DC_MOUSE_SUBTRACT));
		else if (!polySelect)	ip->ReplacePrompt( GetString(IDS_DC_MOUSE_SELECTTRI));
		else					ip->ReplacePrompt( GetString(IDS_DC_MOUSE_SELECTFACE));

		TexLayMCData *md = (TexLayMCData*)mc->localData;
		int tipo = ((TexLayMCData*)mc->localData)->tipo;

		if (tipo == IS_MESH) {
			if (!md->GetMesh()) return 0;
			SubObjHitList hitList;
		
			Mesh &mesh = *((TexLayMCData*)mc->localData)->GetMesh();
			mesh.faceSel = ((TexLayMCData*)mc->localData)->face_sel[current_channel];

			res = mesh.SubObjectHitTest(gw, gw->getMaterial(), &hr, flags|SUBHIT_FACES|SUBHIT_SELSOLID, hitList);


// JW: with 3ds Max 2017+,  SubObjHitList use iterators ...
#if MAX_VERSION_MAJOR > 18
			MeshSubHitRec::Iterator it = hitList.begin();
			if (it != hitList.end() )
				selecting_faces = TRUE;

			for( ; it != hitList.end() ; it++ )
				vpt->LogHit(inode, mc, it->dist, it->index, NULL);
#else
			MeshSubHitRec *rec;

			rec = hitList.First();
			while (rec) {
				vpt->LogHit(inode,mc,rec->dist,rec->index,NULL);
				rec = rec->Next();
				selecting_faces = TRUE;
				}
#endif
			return res;
			}

		else if (tipo == IS_POLY) {
			if (!md->GetMNMesh()) return 0;
			SubObjHitList hitList;
		
			MNMesh &mnmesh = *((TexLayMCData*)mc->localData)->GetMNMesh();
			BitArray faceSel = ((TexLayMCData*)mc->localData)->face_sel[current_channel];
		
			mnmesh.FaceSelect(faceSel); 
			res = mnmesh.SubObjectHitTest(gw, gw->getMaterial(), &hr, flags|SUBHIT_FACES|SUBHIT_SELSOLID, hitList);


			// JW: with 3ds Max 2017+,  SubObjHitList use iterators ...
#if MAX_VERSION_MAJOR > 18
			MeshSubHitRec::Iterator it = hitList.begin();
			if (it != hitList.end())
				selecting_faces = TRUE;

			for (; it != hitList.end(); it++)
				vpt->LogHit(inode, mc, it->dist, it->index, NULL);
#else
			MeshSubHitRec *rec;

			rec = hitList.First();
			while (rec) {
				vpt->LogHit(inode,mc,rec->dist,rec->index,NULL);
				rec = rec->Next();
				selecting_faces = TRUE;
				}
#endif
			return res;
			}

		else if (tipo == IS_PATCH)	{
			if (!md->GetPatch()) return 0;

			SubPatchHitList hitList;
		
			PatchMesh &patch = *((TexLayMCData*)mc->localData)->GetPatch();
			patch.patchSel = ((TexLayMCData*)mc->localData)->face_sel[current_channel];
			res = patch.SubObjectHitTest(gw, gw->getMaterial(), &hr, flags|SUBHIT_PATCH_PATCHES|SUBHIT_SELSOLID, hitList);

			PatchSubHitRec *rec = hitList.First();
			while (rec) {
				vpt->LogHit(inode,mc,rec->dist,rec->index,NULL);
				rec = rec->Next();
				selecting_faces = TRUE;
				}
			return res;
			}
		} // SEL_FACES

	if (ip && (ip->GetSubObjectLevel() == SEL_EDGES) ) {
		int res = 0;
		MakeHitRegion(hr,type, crossing,4,p);
		gw->setHitRegion(&hr);
		Matrix3 mat = inode->GetObjectTM(t);
		gw->setTransform(mat);	
		gw->setRndLimits(((savedLimits = gw->getRndLimits()) | GW_PICK) & ~GW_ILLUM);

		if (type == HITTYPE_POINT)
			gw->setRndLimits(gw->getRndLimits() & GW_BACKCULL);
		else 
			gw->setRndLimits(gw->getRndLimits() & ~GW_BACKCULL);

		gw->clearHitCode();

		BOOL add = GetKeyState(VK_CONTROL)<0;
		BOOL sub = GetKeyState(VK_MENU)<0;

		if (add)				ip->ReplacePrompt( GetString(IDS_DC_MOUSE_EDGES_ADD));
		else if (sub)			ip->ReplacePrompt( GetString(IDS_DC_MOUSE_EDGES_SUBTRACT));
		else					ip->ReplacePrompt( GetString(IDS_DC_MOUSE_SELECTEDGE));

		TexLayMCData *md = (TexLayMCData*)mc->localData;
		int tipo = ((TexLayMCData*)mc->localData)->tipo;

		if (tipo == IS_POLY) {
			if (!md->GetMNMesh()) return 0;
			SubObjHitList hitList;
		
			MNMesh &mnmesh = *((TexLayMCData*)mc->localData)->GetMNMesh();
			BitArray edgeSel = ((TexLayMCData*)mc->localData)->edge_sel[current_channel];
			
			mnmesh.EdgeSelect(edgeSel); 

			res = mnmesh.SubObjectHitTest(gw, gw->getMaterial(), &hr, flags|SUBHIT_MNEDGES, hitList);
	
			// JW: with 3ds Max 2017+,  SubObjHitList use iterators ...
#if MAX_VERSION_MAJOR > 18
			MeshSubHitRec::Iterator it = hitList.begin();
			if (it != hitList.end())
				selecting_faces = TRUE;

			for (; it != hitList.end(); it++)
				vpt->LogHit(inode, mc, it->dist, it->index, NULL);
#else
			MeshSubHitRec *rec;

			rec = hitList.First();
			while (rec) {
				vpt->LogHit(inode,mc,rec->dist,rec->index,NULL);
				rec = rec->Next();
				selecting_faces = TRUE;
				}
#endif
			return res;
			}
		} // SEL_EDGES
	
	gw->setRndLimits(savedLimits);	
	if (gw->checkHitCode()) {
		vpt->LogHit(inode, mc, gw->getHitDistance(), 0, NULL); 
		return 1;
		}
	return 0;
	}

void MultiMapMod::GetSubObjectCenters(SubObjAxisCallback *cb,TimeValue t,INode *node,ModContext *mc)
	{
	int level = ip->GetSubObjectLevel();
	

#if MAX_VERSION_MAJOR < 24
	Matrix3 modmat(1);
#else
	Matrix3 modmat;

#endif

	Matrix3 ntm(node->GetObjectTM(t) );

	modmat = uvwProy[current_channel]->CompMatrix(t,mc,&ntm);

	if (level==SEL_GIZMO) {
		cb->Center(modmat.GetTrans(),0);	
		}
	else if ( level==SEL_POINTS )  {
		Point3 cent(0,0,0);
		int ct=0;
		for (int i=0; i<uvwProy[current_channel]->gm->verts.Count(); i++)
			if (uvwProy[current_channel]->gm->sel[i]) {
				cent += uvwProy[current_channel]->gm->verts[i];
				ct++;
				}
		if (ct) cent/= float(ct);
		cb->Center(cent*modmat,0);
		}
	else if ( level==SEL_FACES ) {
		modmat = ntm;
		// Tri Mesh
		if ( ((TexLayMCData*)mc->localData)->mesh ) {
			Mesh * mesh = ((TexLayMCData*)mc->localData)->mesh;
			BitArray face_sel = ((TexLayMCData*)mc->localData)->face_sel[current_channel];

			BOOL some_sel = FALSE;
			Box3 mesh_box;
			for ( int i_f=0; i_f<mesh->numFaces; i_f++ ) {
				if ( face_sel[i_f] ) {
					mesh_box += mesh->verts[mesh->faces[i_f].v[0]];
					mesh_box += mesh->verts[mesh->faces[i_f].v[1]];
					mesh_box += mesh->verts[mesh->faces[i_f].v[2]];
					some_sel = TRUE;
					}
				}
			if ( some_sel ) {
				Point3 center = mesh_box.Center()*modmat;
				cb->Center(center,0);
				}
			else {
				cb->Center(Point3(0,0,0)*modmat,0);
				}
			}
		// Poly Object
		if ( ((TexLayMCData*)mc->localData)->mnMesh ) {
			MNMesh * mnMesh = ((TexLayMCData*)mc->localData)->mnMesh;
			BitArray face_sel = ((TexLayMCData*)mc->localData)->face_sel[current_channel];

			BOOL some_sel = FALSE;
			Box3 mesh_box;
			for ( int i_f=0; i_f<mnMesh->numf; i_f++ ) {
				if ( face_sel[i_f] ) {
					int deg = mnMesh->f[i_f].deg;
					for ( int i_v=0; i_v<deg; i_v++ )
						mesh_box += mnMesh->v[mnMesh->f[i_f].vtx[i_v]].p;
					some_sel = TRUE;
					}
				}
			if ( some_sel ) {
				Point3 center = mesh_box.Center()*modmat;
				cb->Center(center,0);
				}
			else {
				cb->Center(Point3(0,0,0)*modmat,0);
				}
			}
		// Patch Object
		if ( ((TexLayMCData*)mc->localData)->patch ) {
			PatchMesh *patch = ((TexLayMCData*)mc->localData)->patch;
			BitArray face_sel = ((TexLayMCData*)mc->localData)->face_sel[current_channel];

			BOOL some_sel = FALSE;
			Box3 mesh_box;
			for ( int i_f=0; i_f<patch->numPatches; i_f++ ) {
				if ( face_sel[i_f] ) {
					int deg = patch->patches[i_f].type==PATCH_TRI?3:4;
					for ( int i_v=0; i_v<deg; i_v++ )
						mesh_box += patch->verts[patch->patches[i_f].v[i_v]].p;
					some_sel = TRUE;
					}
				}
			if ( some_sel ) {
				Point3 center = mesh_box.Center()*modmat;
				cb->Center(center,0);
				}
			else {
				cb->Center(Point3(0,0,0)*modmat,0);
				}
			}
		}

	else if ( level==SEL_EDGES ) {
		modmat = ntm;
		// Poly Object
		if ( ((TexLayMCData*)mc->localData)->mnMesh ) {
			MNMesh * mnMesh = ((TexLayMCData*)mc->localData)->mnMesh;
			BitArray edge_sel = ((TexLayMCData*)mc->localData)->edge_sel[current_channel];

			BOOL some_sel = FALSE;
			Box3 mesh_box;
			for ( int i_e=0; i_e<mnMesh->nume; i_e++ ) {
				if ( edge_sel[i_e] ) {
					mesh_box += mnMesh->v[mnMesh->e[i_e].v1].p;
					mesh_box += mnMesh->v[mnMesh->e[i_e].v2].p;
					some_sel = TRUE;
					}
				}
			if ( some_sel ) {
				Point3 center = mesh_box.Center()*modmat;
				cb->Center(center,0);
				}
			else {
				cb->Center(Point3(0,0,0)*modmat,0);
				}
			}
		}


	}

void MultiMapMod::GetSubObjectTMs(SubObjAxisCallback *cb,TimeValue t,INode *node,ModContext *mc) {
	Matrix3 ntm = node->GetObjectTM(t), modmat;
	modmat = uvwProy[current_channel]->CompMatrix(t,mc,&ntm);
	cb->TM(modmat,0);
	}


void MultiMapMod::Move(TimeValue t, Matrix3& partm, Matrix3& tmAxis, Point3& val, BOOL localOrigin) {
	// partm es la tm del nodo...
	if (!ip) return;
	int level = ip->GetSubObjectLevel();
	assert(uvwProy[current_channel]->tmControl);
	if (level==SEL_OBJECT || level==SEL_GIZMO) {
		if ( theHold.Holding() &&!TestAFlag(A_HELD) ) {
			theHold.Put( new MRSRestore(this) );
			SetAFlag(A_HELD);
			}
		SetXFormPacket pckt(val,partm,tmAxis);
		uvwProy[current_channel]->valid_group = 0;
		uvwProy[current_channel]->tmControl->SetValue(t,&pckt,TRUE,CTRL_RELATIVE);
		}
	else if ( level==SEL_POINTS) {
		uvwProy[current_channel]->gm->PlugControllers(t);
		// Compute a matrix to move points

#if MAX_VERSION_MAJOR < 24
		Matrix3 ctm(1);
#else
		Matrix3 ctm;
#endif

		uvwProy[current_channel]->tmControl->GetValue(t,&ctm,FOREVER,CTRL_RELATIVE);
		ctm.PreRotateZ(PI);
		Matrix3 tm  = ctm * partm * Inverse(tmAxis);
		Matrix3 itm = Inverse(tm);
		tm *= TransMatrix(val);

		// Hold for undo
		if (theHold.Holding() && !TestAFlag(A_HELD)) {
			theHold.Put(new MoveRestore(this));
			SetAFlag(A_HELD);
			}

		// Move the control points
		for (int i=0; i<uvwProy[current_channel]->gm->verts.Count(); i++)
			if (uvwProy[current_channel]->gm->sel[i]) {
				if (uvwProy[current_channel]->gm->GetVtCtrl(i)) {
					uvwProy[current_channel]->gm->verts[i] = (tm*uvwProy[current_channel]->gm->verts[i]) * itm;
					uvwProy[current_channel]->gm->GetVtCtrl(i)->SetValue(t,&uvwProy[current_channel]->gm->verts[i],TRUE,CTRL_ABSOLUTE);
					}
				else 
					uvwProy[current_channel]->gm->verts[i] = (tm*uvwProy[current_channel]->gm->verts[i]) * itm;
				}

		uvwProy[current_channel]->gm->CacheVNormals();
		uvwProy[current_channel]->valid_group = 0;
		}
	}


void MultiMapMod::Rotate(TimeValue t, Matrix3& partm, Matrix3& tmAxis, Quat& val, BOOL localOrigin) {
	if (!ip) return;
	int level = ip->GetSubObjectLevel();
	assert(uvwProy[current_channel]->tmControl);
	if (level==SEL_OBJECT || level==SEL_GIZMO) {
		if ( theHold.Holding() &&!TestAFlag(A_HELD) ) {
			theHold.Put( new MRSRestore(this) );
			SetAFlag(A_HELD);
			}
		SetXFormPacket pckt(val,localOrigin,partm,tmAxis);
		uvwProy[current_channel]->valid_group = 0;
		uvwProy[current_channel]->tmControl->SetValue(t,&pckt,TRUE,CTRL_RELATIVE);
		}
	else if ( level==SEL_POINTS) {
		uvwProy[current_channel]->gm->PlugControllers(t);
		// Compute a matrix to move points

#if MAX_VERSION_MAJOR < 24
		Matrix3 ctm(1);
#else
		Matrix3 ctm;
#endif

		uvwProy[current_channel]->tmControl->GetValue(t,&ctm,FOREVER,CTRL_RELATIVE);
		ctm.PreRotateZ(PI);
		Matrix3 tm  = ctm * partm * Inverse(tmAxis);
		Matrix3 itm = Inverse(tm);
		Matrix3 mat;
		val.MakeMatrix(mat);
		tm *= mat;

		// Hold for undo
		if (theHold.Holding() && !TestAFlag(A_HELD)) {
			theHold.Put(new MoveRestore(this));
			SetAFlag(A_HELD);
			}

		// Move the control points
		for (int i=0; i<uvwProy[current_channel]->gm->verts.Count(); i++)
			if (uvwProy[current_channel]->gm->sel[i]) {
				if (uvwProy[current_channel]->gm->GetVtCtrl(i)) {
					uvwProy[current_channel]->gm->verts[i] = (tm*uvwProy[current_channel]->gm->verts[i]) * itm;
					uvwProy[current_channel]->gm->GetVtCtrl(i)->SetValue(t,&uvwProy[current_channel]->gm->verts[i],TRUE,CTRL_ABSOLUTE);
					}
				else 
					uvwProy[current_channel]->gm->verts[i] = (tm*uvwProy[current_channel]->gm->verts[i]) * itm;
				}

		uvwProy[current_channel]->gm->CacheVNormals();
		uvwProy[current_channel]->valid_group = 0;
		}
	}

void MultiMapMod::Scale(TimeValue t, Matrix3& partm, Matrix3& tmAxis, Point3& val, BOOL localOrigin) {
	if (!ip) return;
	int level = ip->GetSubObjectLevel();
	assert(uvwProy[current_channel]->tmControl);
	if (level==SEL_OBJECT || level==SEL_GIZMO) {
		if ( theHold.Holding() &&!TestAFlag(A_HELD) ) {
			theHold.Put( new MRSRestore(this) );
			SetAFlag(A_HELD);
			}
		SetXFormPacket pckt(val,localOrigin,partm,tmAxis);
		uvwProy[current_channel]->valid_group = 0;
		uvwProy[current_channel]->tmControl->SetValue(t,&pckt,TRUE,CTRL_RELATIVE);
		}
	else if ( level==SEL_POINTS) {
		uvwProy[current_channel]->gm->PlugControllers(t);
		// Compute a matrix to move points

#if MAX_VERSION_MAJOR < 24
		Matrix3 ctm(1);
#else
		Matrix3 ctm;
#endif

		uvwProy[current_channel]->tmControl->GetValue(t,&ctm,FOREVER,CTRL_RELATIVE);
		ctm.PreRotateZ(PI);
		Matrix3 tm  = ctm * partm * Inverse(tmAxis);
		Matrix3 itm = Inverse(tm);
		tm *= ScaleMatrix(val);;

		// Hold for undo
		if (theHold.Holding() && !TestAFlag(A_HELD)) {
			theHold.Put(new MoveRestore(this));
			SetAFlag(A_HELD);
			}

		// Move the control points
		for (int i=0; i<uvwProy[current_channel]->gm->verts.Count(); i++)
			if (uvwProy[current_channel]->gm->sel[i]) {
				if (uvwProy[current_channel]->gm->GetVtCtrl(i)) {
					uvwProy[current_channel]->gm->verts[i] = (tm*uvwProy[current_channel]->gm->verts[i]) * itm;
					uvwProy[current_channel]->gm->GetVtCtrl(i)->SetValue(t,&uvwProy[current_channel]->gm->verts[i],TRUE,CTRL_ABSOLUTE);
					}
				else 
					uvwProy[current_channel]->gm->verts[i] = (tm*uvwProy[current_channel]->gm->verts[i]) * itm;
				}
		uvwProy[current_channel]->gm->CacheVNormals();
		uvwProy[current_channel]->valid_group = 0;
		}
	}

class SelRestore : public RestoreObj {
	public:		
		MultiMapMod *mod;
		BitArray undo,redo;
		SelRestore(MultiMapMod *m) {mod=m;undo=mod->uvwProy[mod->current_channel]->gm->sel;}
		void Restore(int isUndo) {
			// if we're undoing, save a redo state
			if (isUndo) redo = mod->uvwProy[mod->current_channel]->gm->sel;
			mod->uvwProy[mod->current_channel]->gm->sel = undo;
			mod->NotifyDependents(FOREVER,PART_SELECT,REFMSG_CHANGE);
			}
		void Redo() {
			mod->uvwProy[mod->current_channel]->gm->sel = redo;
			mod->NotifyDependents(FOREVER,PART_SELECT,REFMSG_CHANGE);
			}
	};

static AdjFaceList *BuildAdjFaceList(Mesh &mesh)
	{
	AdjEdgeList ae(mesh);
	return new AdjFaceList(mesh,ae);
	}

class FaceSelRestore : public RestoreObj {
public:
	BitArray usel, rsel;
	MultiMapMod *mod;
	TexLayMCData *md;
	int current_channel;

	FaceSelRestore(MultiMapMod *m, TexLayMCData *d);
	void Restore(int isUndo);
	void Redo();
	void EndHold() {}
	TSTR Description() { return TSTR(_T("TL Face Sel Restore")); 
	}
};

FaceSelRestore::FaceSelRestore(MultiMapMod *m, TexLayMCData *d) {
	mod   = m;
	md    = d;
	current_channel = mod->current_channel;
	usel  = md->face_sel[current_channel];
	}

void FaceSelRestore::Restore(int isUndo) {
	if (isUndo) {
		rsel = md->face_sel[current_channel];
		}
	md->face_sel[current_channel] = usel;
	mod->LocalDataChanged();
	}

void FaceSelRestore::Redo() {
	md->face_sel[current_channel] = rsel;
	mod->LocalDataChanged();
	}

class EdgeSelRestore : public RestoreObj {
public:
	BitArray usel, rsel;
	MultiMapMod *mod;
	TexLayMCData *md;
	int current_channel;

	EdgeSelRestore(MultiMapMod *m, TexLayMCData *d);
	void Restore(int isUndo);
	void Redo();
	void EndHold() {}
	TSTR Description() { return TSTR(_T("TL Edge Sel Restore")); 
	}
};

EdgeSelRestore::EdgeSelRestore(MultiMapMod *m, TexLayMCData *d) {
	mod   = m;
	md    = d;
	current_channel = mod->current_channel;
	usel  = md->edge_sel[current_channel];
	}

void EdgeSelRestore::Restore(int isUndo) {
	if (isUndo) {
		rsel = md->edge_sel[current_channel];
		}
	md->edge_sel[current_channel] = usel;
	mod->LocalDataChanged();
	}

void EdgeSelRestore::Redo() {
	md->edge_sel[current_channel] = rsel;
	mod->LocalDataChanged();
	}

void MultiMapMod::SelectSubComponent(HitRecord *hitRec,BOOL selected,BOOL all,BOOL invert) 	{

	if (!ip) return;
	int level = ip->GetSubObjectLevel();

	if (level == SEL_POINTS) {
		if ( hwnd_free ) {
			int allu=0,allv=0;
							
			allu = IsDlgButtonChecked(hwnd_free,IDC_TLF_ALLU);
			allv = IsDlgButtonChecked(hwnd_free,IDC_TLF_ALLV);

			if (theHold.Holding()) 
				theHold.Put(new SelRestore(this));
			while (hitRec) {
				BOOL state = selected;
				if (invert) state = !uvwProy[current_channel]->gm->sel[hitRec->hitInfo];
				if (allu || allv) {
					uvwProy[current_channel]->gm->ExpandSelection(hitRec->hitInfo,1,allu,allv);
					}
				else {
					if (state) uvwProy[current_channel]->gm->sel.Set(hitRec->hitInfo);
					else       uvwProy[current_channel]->gm->sel.Clear(hitRec->hitInfo);
					}
				if (!all) break;
				hitRec = hitRec->Next();
				}
			}
		LocalDataChanged();
		}

	else if (level == SEL_FACES) {
		theHold.Begin();
		TexLayMCData *md = NULL, *od = NULL;

		BitArray set;
		AdjFaceList *al = NULL;
		BOOL add = GetKeyState(VK_CONTROL)<0;
		BOOL sub = GetKeyState(VK_MENU)<0;
		BOOL polySelect = !(GetKeyState(VK_SHIFT)<0);

		ip->ClearCurNamedSelSet();

		ModContextList mcList;
		INodeTab nodes;
		ip->GetModContexts(mcList,nodes);
		TimeValue t = ip->GetTime();
		int nd;
		BitArray nsel;

		for (nd=0;nd<mcList.Count(); nd++) {
			TexLayMCData *md = (TexLayMCData*) mcList[nd]->localData;
			if (md == NULL) continue;
			HitRecord *hr = hitRec;
			for (; hr!=NULL; hr=hr->Next()) if (hr->modContext->localData == md) break;
			if (hr==NULL) continue;

			int tipo = md->tipo;

			if ( tipo == IS_MESH ){
				md = (TexLayMCData*)hr->modContext->localData;
				if (!md->GetMesh()) return;
				Mesh &mesh = *md->GetMesh();
				BitArray faceSel = md->face_sel[current_channel];
				set.SetSize(mesh.getNumFaces());

				AdjFaceList *al = NULL;
				if (polySelect)
					al = BuildAdjFaceList(mesh);

				if (theHold.Holding()) 
					theHold.Put (new FaceSelRestore(this,md));

				while (hr) {
					if (hr->modContext->localData == md) {
						set.ClearAll();
						if ((hr->hitInfo < mesh.numFaces) &&(md->face_sel[current_channel].GetSize() == set.GetSize())){
							mesh.PolyFromFace (hr->hitInfo, set, 45.0, FALSE, al);
							if (invert)				faceSel ^= set;
							else if (selected)		faceSel |= set;
							else					faceSel &= ~set;
							}
					if (!all) break;
						}
					hr = hr->Next();
					}
				md->face_sel[current_channel] = faceSel;
				if (al) delete al;
				}

			else if ( tipo == IS_POLY ) {
				md = (TexLayMCData*)hr->modContext->localData;
				if (!md->GetMNMesh()) return;
				MNMesh &mnmesh = *md->GetMNMesh();
				BitArray faceSel = md->face_sel[current_channel];
				set.SetSize(mnmesh.numf);

				if (theHold.Holding()) 
					theHold.Put (new FaceSelRestore(this,md));

				while (hr) {
					if (hr->modContext->localData == md) {
						BOOL state = selected;
						if ( hr->hitInfo < md->face_sel[current_channel].GetSize() ) {
							if (invert) state =			state = !faceSel[hr->hitInfo];
							if (state)					faceSel.Set(hr->hitInfo);
							else						faceSel.Clear(hr->hitInfo);
							}
						if ( !all ) break;
						}
					hr = hr->Next();
					}
				md->face_sel[current_channel] = faceSel;
				}

			else if (tipo == IS_PATCH){
				md  = (TexLayMCData*)hr->modContext->localData;
				if (!md->GetPatch()) return;
				
				PatchMesh &patch = *md->GetPatch();
				BitArray faceSel = md->face_sel[current_channel];

				if (theHold.Holding()) 
					theHold.Put (new FaceSelRestore (this,md));

				while (hr) {
					if (hr->modContext->localData == md) {
						BOOL state = selected;
						if (hr->hitInfo < md->face_sel[current_channel].GetSize())	{
							if (invert) state =			state = !faceSel[hr->hitInfo];
							if (state)					faceSel.Set(hr->hitInfo);
							else						faceSel.Clear(hr->hitInfo);
							}
						if (!all) break;
						}
					hr = hr->Next();
					}
				md->face_sel[current_channel] = faceSel;
				}
			}

		uvwProy[current_channel]->valid_group = 0;

		LocalDataChanged();
		theHold.Accept(_T("Select Faces"));
		}

	else if ( level == SEL_EDGES ) {
		theHold.Begin();
		TexLayMCData *md = NULL, *od = NULL;

		BitArray set;

		BOOL add = GetKeyState(VK_CONTROL)<0;
		BOOL sub = GetKeyState(VK_MENU)<0;

		ip->ClearCurNamedSelSet();

		ModContextList mcList;
		INodeTab nodes;
		ip->GetModContexts(mcList,nodes);
		TimeValue t = ip->GetTime();
		int nd;
		BitArray nsel;

		for (nd=0;nd<mcList.Count(); nd++) {
			TexLayMCData *md = (TexLayMCData*) mcList[nd]->localData;
			if (md == NULL) continue;
			HitRecord *hr = hitRec;
			for (; hr!=NULL; hr=hr->Next()) if (hr->modContext->localData == md) break;
			if (hr==NULL) continue;

			int tipo = md->tipo;

			if ( tipo == IS_POLY ) {
				md = (TexLayMCData*)hr->modContext->localData;
				if (!md->GetMNMesh()) return;
				MNMesh &mnmesh = *md->GetMNMesh();
				BitArray edgeSel = md->edge_sel[current_channel];
				set.SetSize(mnmesh.nume);

				if (theHold.Holding()) 
					theHold.Put (new EdgeSelRestore(this,md));

				while (hr) {
					if (hr->modContext->localData == md) {
						BOOL state = selected;
						if ( hr->hitInfo < md->edge_sel[current_channel].GetSize() ) {
							if (invert) state =			state = !edgeSel[hr->hitInfo];
							if (state)					edgeSel.Set(hr->hitInfo);
							else						edgeSel.Clear(hr->hitInfo);
							}
						if ( !all ) break;
						}
					hr = hr->Next();
					}
				md->edge_sel[current_channel] = edgeSel;
				}

			}

		uvwProy[current_channel]->valid_group = 0;

		LocalDataChanged();
		theHold.Accept(_T("Select Edges"));
		}

	}

void MultiMapMod::ClearSelection(int selLevel) {
	int level = ip->GetSubObjectLevel();
	if (level==SEL_POINTS)
		uvwProy[current_channel]->gm->sel.ClearAll();

	else if (level == SEL_FACES) {
		ModContextList list;
		INodeTab nodes;	
		ip->GetModContexts(list,nodes);
		TexLayMCData *d;
		for (int i=0; i<list.Count(); i++) {
			d = (TexLayMCData*)list[i]->localData;
			if (!d) continue;

			if (theHold.Holding()) 
				theHold.Put (new FaceSelRestore(this,d));

			BitArray faceSel = d->face_sel[current_channel];

			faceSel.ClearAll();
			d->face_sel[current_channel] = faceSel;
			}
		ip->ClearCurNamedSelSet();
		nodes.DisposeTemporary();

		uvwProy[current_channel]->valid_group = 0;
		
		LocalDataChanged();
		}

	else if ( level == SEL_EDGES ) {
		ModContextList list;
		INodeTab nodes;	
		ip->GetModContexts(list,nodes);
		TexLayMCData *d;
		for (int i=0; i<list.Count(); i++) {
			d = (TexLayMCData*)list[i]->localData;
			if (!d) continue;

			if (theHold.Holding()) 
				theHold.Put (new EdgeSelRestore(this,d));

			BitArray edgeSel = d->edge_sel[current_channel];

			edgeSel.ClearAll();
			d->edge_sel[current_channel] = edgeSel;
			}
		ip->ClearCurNamedSelSet();
		nodes.DisposeTemporary();
		
		uvwProy[current_channel]->valid_group = 0;

		LocalDataChanged();
		}
	}

void MultiMapMod::SelectAll(int selLevel) {
	int level = ip->GetSubObjectLevel();

	if (level==SEL_POINTS)
		uvwProy[current_channel]->gm->sel.SetAll();

	else if (level == SEL_FACES) {
		ModContextList list;
		INodeTab nodes;	
		ip->GetModContexts(list,nodes);
		TexLayMCData *d;
		for (int i=0; i<list.Count(); i++) {
			d = (TexLayMCData*)list[i]->localData;
			if (!d) continue;

			if (theHold.Holding()) 
				theHold.Put (new FaceSelRestore(this,d));

			BitArray faceSel = d->face_sel[current_channel];

			faceSel.SetAll();
			d->face_sel[current_channel] = faceSel;
			}
		ip->ClearCurNamedSelSet();
		nodes.DisposeTemporary();

		uvwProy[current_channel]->valid_group = 0;
		
		LocalDataChanged();
		}

	else if ( level == SEL_EDGES ) {
		ModContextList list;
		INodeTab nodes;	
		ip->GetModContexts(list,nodes);
		TexLayMCData *d;
		for (int i=0; i<list.Count(); i++) {
			d = (TexLayMCData*)list[i]->localData;
			if (!d) continue;

			if (theHold.Holding()) 
				theHold.Put (new EdgeSelRestore(this,d));

			BitArray edgeSel = d->edge_sel[current_channel];

			edgeSel.SetAll();
			d->edge_sel[current_channel] = edgeSel;
			}
		ip->ClearCurNamedSelSet();
		nodes.DisposeTemporary();
	
		uvwProy[current_channel]->valid_group = 0;
		
		LocalDataChanged();
		}

	if ( !selecting_faces ) 
		LocalDataChanged();
	}

void MultiMapMod::InvertSelection(int selLevel) {
	int level = ip->GetSubObjectLevel();

	if (level==SEL_POINTS)
		uvwProy[current_channel]->gm->sel = ~uvwProy[current_channel]->gm->sel;

	else if (level == SEL_FACES) {
		ModContextList list;
		INodeTab nodes;	
		ip->GetModContexts(list,nodes);
		TexLayMCData *d;
		for (int i=0; i<list.Count(); i++) {
			d = (TexLayMCData*)list[i]->localData;
			if (!d) continue;

			if (theHold.Holding()) 
				theHold.Put (new FaceSelRestore(this,d));

			BitArray faceSel = d->face_sel[current_channel];

			faceSel = ~faceSel;
			d->face_sel[current_channel] = faceSel;
			}
		ip->ClearCurNamedSelSet();
		nodes.DisposeTemporary();

		uvwProy[current_channel]->valid_group = 0;
		
		LocalDataChanged();
		}

	else if ( level == SEL_EDGES ) {
		ModContextList list;
		INodeTab nodes;	
		ip->GetModContexts(list,nodes);
		TexLayMCData *d;
		for (int i=0; i<list.Count(); i++) {
			d = (TexLayMCData*)list[i]->localData;
			if (!d) continue;

			if (theHold.Holding()) 
				theHold.Put (new EdgeSelRestore(this,d));

			BitArray edgeSel = d->edge_sel[current_channel];

			edgeSel =  ~edgeSel;
			d->edge_sel[current_channel] = edgeSel;
			}
		ip->ClearCurNamedSelSet();
		nodes.DisposeTemporary();

		uvwProy[current_channel]->valid_group = 0;
		
		LocalDataChanged();
		}

	if ( !selecting_faces ) 
		LocalDataChanged();	}


int MultiMapMod::Display( TimeValue t, INode* inode, ViewExp *vpt, int flags, ModContext *mc) {
	GraphicsWindow *gw = vpt->getGW();
	Matrix3 modmat, ntm = inode->GetObjectTM(t);
	DrawLineProc lp(gw);

	if (show_all_gizmos)
		for (int i=0; i<uvwProy.Count(); i++) {
			uvwProy[i]->Update(t);
			modmat = uvwProy[i]->CompMatrix(t,mc,&ntm);
			gw->setTransform(modmat);
			int level = 0;
			if (ip) level = ip->GetSubObjectLevel();
			DoIcon(t,lp,i,ip&&ip->GetSubObjectLevel()==1,level,gw,vpt,inode);
			}
	else {
		uvwProy[current_channel]->Update(t);
		modmat = uvwProy[current_channel]->CompMatrix(t,mc,&ntm);
		gw->setTransform(modmat);
		int level = 0;
		if (ip) level = ip->GetSubObjectLevel();
		DoIcon(t,lp,current_channel,ip&&ip->GetSubObjectLevel()==1,level,gw,vpt,inode);
		}
	return 0;	
	}

void MultiMapMod::GetWorldBoundBox(TimeValue t,INode* inode, ViewExp *vpt, Box3& box, ModContext *mc) {
	Matrix3 modmat, ntm = inode->GetObjTMBeforeWSM(t);	
	if (show_all_gizmos) 
		for (int i=0; i<uvwProy.Count(); i++) {
			modmat = uvwProy[i]->CompMatrix(t,mc,&ntm);		
			BoxLineProc bproc(&modmat);

			int level = 0;
			if (ip) level = ip->GetSubObjectLevel();

			DoIcon(t, bproc, i,ip&&ip->GetSubObjectLevel()==1,level);
			box = bproc.Box();	
			}
	else {
		modmat = uvwProy[current_channel]->CompMatrix(t,mc,&ntm);		
		BoxLineProc bproc(&modmat);

		int level = 0;
		if (ip) level = ip->GetSubObjectLevel();

		DoIcon(t, bproc, current_channel,ip&&ip->GetSubObjectLevel()==1,level);
		box = bproc.Box();	
		}
	}

TSTR MultiMapMod::BrowseForFileName(BOOL save, BOOL &cancel,int fo) {

	static TCHAR fname[256] = {'\0'};
	OPENFILENAME ofn;
	HWND hWnd = GetActiveWindow();
	memset(&ofn,0,sizeof(OPENFILENAME));

	FilterList fl;
	fl.Append( GetString(IDS_TL_FILE));
	fl.Append( _T("*.tl"));
	TSTR title;
	if (save)
		title = GetString(IDS_TL_SAVEALLTITLE);
	else 
		title = GetString(IDS_TL_MERGETITLE);

	ofn.lStructSize     = sizeof(OPENFILENAME);
	ofn.hwndOwner       = hwnd_main;
	ofn.lpstrFilter     = fl;
	ofn.nFilterIndex	= 1;
	ofn.lpstrFile       = fname;
	ofn.nMaxFile        = 256;    
	ofn.lpstrInitialDir = ip->GetDir(APP_EXPORT_DIR);
	ofn.Flags           = OFN_HIDEREADONLY|(save? OFN_OVERWRITEPROMPT:(
							OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST));
	ofn.lpstrDefExt     = _T("tl");
	ofn.lpstrTitle      = title;
	//Win32 : ofn.hInstance		= (HINSTANCE) GetWindowLong(hWnd, GWL_HINSTANCE);
	
	if(save) {
		if(!GetSaveFileName(&ofn)) cancel = TRUE;
		}
	else {
		if(!GetOpenFileName(&ofn)) cancel = TRUE;
		}

	loadFileName=fname;
		
	return fname;
	}

int read_string(char *string,FILE *stream,int maxsize) {
	while(maxsize--) {
		fread(string,1,1,stream);
		if(*(string++)==0)
			return(1);
		}
	return(0);
	}

void MultiMapMod::SaveTLDlg() {
	ModContextList mcList;
	INodeTab nodes;
	ip->GetModContexts(mcList,nodes);

	int sel_warning = nodes.Count()>1;

	IOInfo io_info;
	io_info.mod = this;
	io_info.groups.SetSize( uvwProy.Count() );
	int res = DialogBoxParam(	hInstance,
								MAKEINTRESOURCE(IDD_IO_DLG),
								hwnd_main,
								SaveTLDlgProc,
								(LPARAM)&io_info);
	if ( res ) 
		SaveTL(io_info.groups);
	}

// Old TexLay 1.1 version -> 100300
void MultiMapMod::SaveTL( BitArray &sel_groups ) {
	TSTR fname;
	BOOL cancel = FALSE;
	TimeValue t = ip->GetTime();

	fname = BrowseForFileName(TRUE,cancel,0);

	if (cancel) 
		return;

	ModContextList mcList;
	INodeTab nodes;
	ip->GetModContexts(mcList,nodes);
	TexLayMCData *md = (TexLayMCData*)mcList[0]->localData;

	int version = 200000;
	int numcans = sel_groups.NumberSet();
	int can = 0;

	FILE *tf = _tfopen(fname, _T("wb"));
	fwrite(&version, sizeof(int), 1, tf);
	fwrite(&numcans, sizeof(int), 1, tf);
	fwrite(&can,	 sizeof(int), 1, tf);

	for (int i=0; i<uvwProy.Count(); i++) {
		if ( sel_groups[i] ) {

			// NEW IN TEXLAY 2.0
			int save_sel = 1;
			fwrite(&save_sel,sizeof(save_sel),1,tf);

			if ( save_sel ) {
				BitArray face_sel = md->face_sel[i];
				BitArray edge_sel = md->edge_sel[i];
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
				}
			// NEW IN TEXLAY 2.0			

#if MAX_VERSION_MAJOR < 24
			Matrix3 mapTM(1);
#else
			Matrix3 mapTM;
#endif

			uvwProy[i]->tmControl->GetValue(t,&mapTM,FOREVER,CTRL_RELATIVE);
			fwrite(&mapTM,sizeof(float), 12, tf);

			int intval; float fltval;

			// NEW IN TEXLAY 2.0
			intval = uvwProy[i]->GetMappingChannel();
			fwrite(&intval,sizeof(int), 1, tf);
			// NEW IN TEXLAY 2.0

			intval = uvwProy[i]->GetMappingType();
			fwrite(&intval,sizeof(int), 1, tf);

			fltval = uvwProy[i]->GetTileU(t);
			fwrite(&fltval,sizeof(float),1,tf);
			fltval = uvwProy[i]->GetOffsetU(t);
			fwrite(&fltval,sizeof(float),1,tf);
			fltval = uvwProy[i]->GetTileV(t);
			fwrite(&fltval,sizeof(float),1,tf);
			fltval = uvwProy[i]->GetOffsetV(t);
			fwrite(&fltval,sizeof(float),1,tf);
			fltval = uvwProy[i]->GetTileW(t);
			fwrite(&fltval,sizeof(float),1,tf);
			fltval = uvwProy[i]->GetOffsetW(t);
			fwrite(&fltval,sizeof(float),1,tf);

			intval = uvwProy[i]->GetAttStatus(t);
			fwrite(&intval,sizeof(int),1,tf);

			fltval = uvwProy[i]->GetAttGlobal(t);
			fwrite(&fltval,sizeof(float),1,tf);

			fltval = uvwProy[i]->GetAttUStart(t);
			fwrite(&fltval,sizeof(float),1,tf);
			fltval = uvwProy[i]->GetAttUOffset(t);
			fwrite(&fltval,sizeof(float),1,tf);
			fltval = uvwProy[i]->GetAttVStart(t);
			fwrite(&fltval,sizeof(float),1,tf);
			fltval = uvwProy[i]->GetAttVOffset(t);
			fwrite(&fltval,sizeof(float),1,tf);
			fltval = uvwProy[i]->GetAttWStart(t);
			fwrite(&fltval,sizeof(float),1,tf);
			fltval = uvwProy[i]->GetAttWOffset(t);
			fwrite(&fltval,sizeof(float),1,tf);

			fltval = uvwProy[i]->GetLength(t);
			fwrite(&fltval,sizeof(float),1,tf);
			fltval = uvwProy[i]->GetHeight(t);
			fwrite(&fltval,sizeof(float),1,tf);
			fltval = uvwProy[i]->GetWidth(t);
			fwrite(&fltval,sizeof(float),1,tf);

			intval = uvwProy[i]->GetNormalize(t);
			fwrite(&intval,sizeof(int),1,tf);
			intval = uvwProy[i]->GetReverse(t);
			fwrite(&intval,sizeof(int),1,tf);
			intval = uvwProy[i]->GetAxis();
			fwrite(&intval,sizeof(int),1,tf);
			intval = uvwProy[i]->GetNormalType(t);
			fwrite(&intval,sizeof(int),1,tf);

			fltval = uvwProy[i]->GetFFMThresh(t);
			fwrite(&fltval,sizeof(float),1,tf);
			fltval = uvwProy[i]->GetAttRad(t);
			fwrite(&fltval,sizeof(float),1,tf);

			// NEW IN TEXLAY 2.0
			fltval = uvwProy[i]->GetMoveU(t);
			fwrite(&fltval,sizeof(float),1,tf);
			fltval = uvwProy[i]->GetMoveV(t);
			fwrite(&fltval,sizeof(float),1,tf);
			fltval = uvwProy[i]->GetMoveW(t);
			fwrite(&fltval,sizeof(float),1,tf);
			fltval = uvwProy[i]->GetRotateU(t);
			fwrite(&fltval,sizeof(float),1,tf);
			fltval = uvwProy[i]->GetRotateV(t);
			fwrite(&fltval,sizeof(float),1,tf);
			fltval = uvwProy[i]->GetRotateW(t);
			fwrite(&fltval,sizeof(float),1,tf);
			fltval = uvwProy[i]->GetScaleU(t);
			fwrite(&fltval,sizeof(float),1,tf);
			fltval = uvwProy[i]->GetScaleV(t);
			fwrite(&fltval,sizeof(float),1,tf);
			fltval = uvwProy[i]->GetScaleW(t);
			fwrite(&fltval,sizeof(float),1,tf);

			fltval = uvwProy[i]->GetFrameSize(t);
			fwrite(&fltval,sizeof(float),1,tf);
			fltval = uvwProy[i]->GetPeltRigidity(t);
			fwrite(&fltval,sizeof(float),1,tf);
			fltval = uvwProy[i]->GetPeltStability(t);
			fwrite(&fltval,sizeof(float),1,tf);
			intval = uvwProy[i]->GetPeltIter();
			fwrite(&intval,sizeof(float),1,tf);
			fltval = uvwProy[i]->GetSplineNormalsStart();
			fwrite(&fltval,sizeof(float),1,tf);
			// NEW IN TEXLAY 2.0

			int tCNp = uvwProy[i]->tileCrv.NumPoints();
			fwrite(&tCNp,sizeof(int), 1, tf);
			for ( int j=0; j<tCNp; j++ ) {
				Point2 ip = uvwProy[i]->tileCrv.GetIn(j);
				Point2 p = uvwProy[i]->tileCrv.GetPoint(j);
				Point2 op = uvwProy[i]->tileCrv.GetOut(j);
				int knd = uvwProy[i]->tileCrv.GetKind(j);
				fwrite(&knd,sizeof(int),1,tf);
				fwrite(&ip,sizeof(float),2,tf);
				fwrite(&p,sizeof(float),2,tf);
				fwrite(&op,sizeof(float),2,tf);
				}

			int angCNp = uvwProy[i]->angleCrv.NumPoints();
			fwrite(&angCNp,sizeof(int), 1, tf);
			for ( int j=0; j<angCNp; j++ ) {
				Point2 ip = uvwProy[i]->angleCrv.GetIn(j);
				Point2 p = uvwProy[i]->angleCrv.GetPoint(j);
				Point2 op = uvwProy[i]->angleCrv.GetOut(j);
				int knd = uvwProy[i]->angleCrv.GetKind(j);
				fwrite(&knd,sizeof(int),1,tf);
				fwrite(&ip,sizeof(float),2,tf);
				fwrite(&p,sizeof(float),2,tf);
				fwrite(&op,sizeof(float),2,tf);
				}

			int rCNp = uvwProy[i]->wTileCrv.NumPoints();
			fwrite(&rCNp,sizeof(int), 1, tf);
			for ( int j=0; j<rCNp; j++ ) {
				Point2 ip = uvwProy[i]->wTileCrv.GetIn(j);
				Point2 p = uvwProy[i]->wTileCrv.GetPoint(j);
				Point2 op = uvwProy[i]->wTileCrv.GetOut(j);
				int knd = uvwProy[i]->wTileCrv.GetKind(j);
				fwrite(&knd,sizeof(int),1,tf);
				fwrite(&ip,sizeof(float),2,tf);
				fwrite(&p,sizeof(float),2,tf);
				fwrite(&op,sizeof(float),2,tf);
				}

			int gmNv = uvwProy[i]->gm->verts.Count();
			int gmnx = uvwProy[i]->gm->nx;
			int gmny = uvwProy[i]->gm->ny;
			fwrite(&gmNv,sizeof(int),1,tf);
			fwrite(&gmnx,sizeof(int),1,tf);
			fwrite(&gmny,sizeof(int),1,tf);
			for ( int j=0; j<gmNv; j++ ) {
				Point3 vert = uvwProy[i]->gm->verts[j];
				fwrite(&vert,sizeof(float),3,tf);
				}

			uvwProy[i]->GetSplineNode();

			int isSpline;
			if (uvwProy[i]->spline_node) {


#if MAX_VERSION_MAJOR < 15
				CStr nameNode( uvwProy[i]->spline_node->GetName() );
#else
				CStr nameNode( (const char * )uvwProy[i]->spline_node->GetName()  );
#endif
				isSpline = 1;
				fwrite(&isSpline,sizeof(int),1,tf);
				fwrite(nameNode.data(),1,nameNode.Length()+1,tf);
				}
			else {
				isSpline = 0;
				fwrite(&isSpline,sizeof(int),1,tf);
				}

			// NEW IN TEXLAY 2.0
			if ( uvwProy[i]->GetMappingType()==MAP_TL_UVWDATA ) {
				int num_v = md->group_uvw_data[i]->num_v;
				int num_f = md->group_uvw_data[i]->num_f;
				fwrite(&num_v,sizeof(num_v),1,tf);
				fwrite(md->group_uvw_data[i]->v,sizeof(Point3),num_v,tf);
				fwrite(&num_f,sizeof(num_f),1,tf);
				for ( int i_f=0; i_f<num_f; i_f++ ) {
					int deg = md->group_uvw_data[i]->f[i_f].deg;
					fwrite(&deg,sizeof(deg),1,tf);
					fwrite(md->group_uvw_data[i]->f[i_f].vtx,sizeof(int),deg,tf);
					}
				}
			// NEW IN TEXLAY 2.0
#if MAX_VERSION_MAJOR < 15
			CStr nameChn(   uvwProy[i]->descCanal  );
#else
			CStr nameChn( CStr::FromMSTR(  uvwProy[i]->descCanal  ));
#endif
			fwrite(nameChn.data(),1,nameChn.Length()+1,tf);
			}
		}
	fclose(tf);
	}

void MultiMapMod::LoadTL(int fo) {

	TSTR fname;
	BOOL cancel = FALSE;

	fname = BrowseForFileName(FALSE,cancel,fo);

	BOOL any_loaded = FALSE;

	if (!cancel) {

		theHold.SuperBegin();
		theHold.Begin();
		theHold.Put( new GroupsTableRestore(this,START_EDIT) );
		theHold.Put( new GroupsTableRestore(this,LOAD_GROUPS) );

		ModContextList mcList;
		INodeTab nodes;
		ip->GetModContexts(mcList,nodes);
		TexLayMCData *md = (TexLayMCData*)mcList[0]->localData;

		FILE *tf = _tfopen(fname, _T("rb"));
		int version;
		int numcans;
		int can;
		fread(&version, sizeof(int),1,tf);
		fread(&numcans, sizeof(int),1,tf);
		fread(&can,		sizeof(int),1,tf);

		if ( version>100250 ) {
			int start_group = uvwProy.Count();

			uvwProy.SetCount( start_group+numcans );
			for (int i=start_group; i<start_group+numcans; i++) {


				uvwProy[i] = new UVWProyector(TRUE);
				ReplaceReference( i, uvwProy[i] ); // 1+i tl_pblock

				theHold.Put( new GroupsTableRestore(this,END_EDIT) );
				theHold.Accept(_T("Add Group"));

				uvwProy[i]->flags = 0;

				any_loaded = TRUE;

				// NEW IN TEXLAY 2.0
				if ( version>=200000 ) {
					int load_sel;
					fread(&load_sel,sizeof(load_sel),1,tf);

					if ( load_sel ) {
						BitArray face_sel = md->face_sel[0];
						BitArray edge_sel = md->edge_sel[0];
						int num_faces = face_sel.GetSize();
						int num_edges = edge_sel.GetSize();

						int num_load_faces;
						fread(&num_load_faces,sizeof(num_load_faces), 1, tf);

						face_sel.ClearAll();
						for ( int i_f=0; i_f<num_load_faces; i_f++ ) {
							char bit;
							fread(&bit,sizeof(bit),1,tf);
							if ( num_load_faces==num_faces )
								face_sel.Set(i_f,(int)bit);
							}
						if ( num_load_faces==num_faces )
							md->face_sel.AppendSet(face_sel);

						int num_load_edges;
						fread(&num_load_edges,sizeof(num_load_edges), 1, tf);

						for ( int i_e=0; i_e<num_load_edges; i_e++ ) {
							char bit;
							fread(&bit,sizeof(bit),1,tf);
							if ( num_load_edges==num_edges )
								edge_sel.Set(i_e,(int)bit);
							}
						if ( num_load_edges==num_edges ) {
							edge_sel.ClearAll();
							md->edge_sel.AppendSet(edge_sel);
							}

						PolyUVWData * uvw_data = new PolyUVWData;
						md->group_uvw_data.Append(1,&uvw_data);
						}
					}
				// NEW IN TEXLAY 2.0

				Matrix3 mapTM;
				fread(&mapTM,sizeof(float),12,tf);

				uvwProy[i]->ReplaceReference( 1, NewDefaultMatrix3Controller() );
				SetXFormPacket pckt(mapTM);
				uvwProy[i]->tmControl->SetValue(0,&pckt,TRUE,CTRL_RELATIVE);

				int intval;
				float fltval;

				// NEW IN TEXLAY 2.0
				if ( version>=200000 ) {
					fread(&intval,sizeof(int), 1, tf);
					uvwProy[i]->SetMappingChannel(intval);
					}
				// NEW IN TEXLAY 2.0

				fread(&intval,sizeof(int), 1, tf);
				if (version == 100250) {
					switch (intval) {
						case MAP_PLANAR:		uvwProy[i]->SetMappingType( MAP_TL_PLANAR );		break;
						case MAP_CYLINDRICAL:	uvwProy[i]->SetMappingType( MAP_TL_CYLINDRICAL );	break;
						case MAP_BALL:			uvwProy[i]->SetMappingType( MAP_TL_BALL );			break;
						case MAP_SPHERICAL:		uvwProy[i]->SetMappingType( MAP_TL_SPHERICAL );		break;
						case 4:					uvwProy[i]->SetMappingType( MAP_TL_SPLINE );		break;
						case 5:					uvwProy[i]->SetMappingType( MAP_TL_FFM );			break;
						default:				uvwProy[i]->SetMappingType( MAP_TL_PLANAR );
						}
					}
				else 
					uvwProy[i]->SetMappingType(intval);

				fread(&fltval,sizeof(float), 1, tf);
				uvwProy[i]->SetTileU(fltval,0);
				fread(&fltval,sizeof(float), 1, tf);
				uvwProy[i]->SetOffsetU(fltval,0);
				fread(&fltval,sizeof(float), 1, tf);
				uvwProy[i]->SetTileV(fltval,0);
				fread(&fltval,sizeof(float), 1, tf);
				uvwProy[i]->SetOffsetV(fltval,0);
				fread(&fltval,sizeof(float), 1, tf);
				uvwProy[i]->SetTileW(fltval,0);
				fread(&fltval,sizeof(float), 1, tf);
				uvwProy[i]->SetOffsetW(fltval,0);

				fread(&intval,sizeof(int), 1, tf);
				uvwProy[i]->SetAttStatus(intval,0);
				
				fread(&fltval,sizeof(float), 1, tf);
				uvwProy[i]->SetAttGlobal(fltval,0);

				fread(&fltval,sizeof(float), 1, tf);
				uvwProy[i]->SetAttUStart(fltval,0);
				fread(&fltval,sizeof(float), 1, tf);
				uvwProy[i]->SetAttUOffset(fltval,0);
				fread(&fltval,sizeof(float), 1, tf);
				uvwProy[i]->SetAttVStart(fltval,0);
				fread(&fltval,sizeof(float), 1, tf);
				uvwProy[i]->SetAttVOffset(fltval,0);
				fread(&fltval,sizeof(float), 1, tf);
				uvwProy[i]->SetAttWStart(fltval,0);
				fread(&fltval,sizeof(float), 1, tf);
				uvwProy[i]->SetAttWOffset(fltval,0);

				fread(&fltval,sizeof(float), 1, tf);
				uvwProy[i]->SetLength(0,fltval);
				fread(&fltval,sizeof(float), 1, tf);
				uvwProy[i]->SetHeight(0,fltval);
				fread(&fltval,sizeof(float), 1, tf);
				uvwProy[i]->SetWidth(0,fltval);

				fread(&intval,sizeof(int), 1, tf);
				uvwProy[i]->SetNormalize(intval,0);
				fread(&intval,sizeof(int), 1, tf);
				uvwProy[i]->SetReverse(intval,0);
				fread(&intval,sizeof(int), 1, tf);
				uvwProy[i]->SetAxis(intval,0);
				fread(&intval,sizeof(int), 1, tf);
				uvwProy[i]->SetNormalType(intval,0);

				fread(&fltval,sizeof(float), 1, tf);
				uvwProy[i]->SetFFMThresh(fltval,0);
				fread(&fltval,sizeof(float), 1, tf);
				uvwProy[i]->SetAttRad(fltval,0);

				// NEW IN TEXLAY 2.0
				if ( version>=200000 ) {
					fread(&fltval,sizeof(float), 1, tf);
					uvwProy[i]->SetMoveU(fltval,0);
					fread(&fltval,sizeof(float), 1, tf);
					uvwProy[i]->SetMoveV(fltval,0);
					fread(&fltval,sizeof(float), 1, tf);
					uvwProy[i]->SetMoveW(fltval,0);
					fread(&fltval,sizeof(float), 1, tf);
					uvwProy[i]->SetRotateU(fltval,0);
					fread(&fltval,sizeof(float), 1, tf);
					uvwProy[i]->SetRotateV(fltval,0);
					fread(&fltval,sizeof(float), 1, tf);
					uvwProy[i]->SetRotateW(fltval,0);
					fread(&fltval,sizeof(float), 1, tf);
					uvwProy[i]->SetScaleU(fltval,0);
					fread(&fltval,sizeof(float), 1, tf);
					uvwProy[i]->SetScaleV(fltval,0);
					fread(&fltval,sizeof(float), 1, tf);
					uvwProy[i]->SetScaleW(fltval,0);

					fread(&fltval,sizeof(float), 1, tf);
					uvwProy[i]->SetFrameSize(fltval,0);
					fread(&fltval,sizeof(float), 1, tf);
					uvwProy[i]->SetPeltRigidity(fltval,0);
					fread(&fltval,sizeof(float), 1, tf);
					uvwProy[i]->SetPeltStability(fltval,0);
					fread(&intval,sizeof(int), 1, tf);
					uvwProy[i]->SetPeltIter(intval);
					fread(&fltval,sizeof(float), 1, tf);
					uvwProy[i]->SetSplineNormalsStart(fltval,0);
					}
				// NEW IN TEXLAY 2.0

				int tCNp;
				fread(&tCNp,sizeof(int),1,tf);
				uvwProy[i]->tileCrv.SetNumPoints(tCNp);
				for (int j=0; j<tCNp; j++) {
					Point2 ip,p,op;
					int knd;
					fread(&knd,sizeof(int),1,tf);
					fread(&ip,sizeof(float),2,tf);
					fread(&p,sizeof(float),2,tf);
					fread(&op,sizeof(float),2,tf);
					uvwProy[i]->tileCrv.SetKind(j,knd);
					uvwProy[i]->tileCrv.SetIn(j,ip);
					uvwProy[i]->tileCrv.SetPoint(j,p);
					uvwProy[i]->tileCrv.SetOut(j,op);
					}

				int angCNp;
				fread(&angCNp,sizeof(int),1,tf);
				uvwProy[i]->angleCrv.SetNumPoints(angCNp);
				for ( int j=0; j<angCNp; j++ ) {
					Point2 ip,p,op;
					int knd;
					fread(&knd,sizeof(int),1,tf);
					fread(&ip,sizeof(float),2,tf);
					fread(&p,sizeof(float),2,tf);
					fread(&op,sizeof(float),2,tf);
					uvwProy[i]->angleCrv.SetKind(j,knd);
					uvwProy[i]->angleCrv.SetIn(j,ip);
					uvwProy[i]->angleCrv.SetPoint(j,p);
					uvwProy[i]->angleCrv.SetOut(j,op);
					}

				int rCNp;
				fread(&rCNp,sizeof(int),1,tf);
				uvwProy[i]->wTileCrv.SetNumPoints(rCNp);
				for ( int j=0; j<rCNp; j++) {
					Point2 ip,p,op;
					int knd;
					fread(&knd,sizeof(int),1,tf);
					fread(&ip,sizeof(float),2,tf);
					fread(&p,sizeof(float),2,tf);
					fread(&op,sizeof(float),2,tf);
					uvwProy[i]->wTileCrv.SetKind(j,knd);
					uvwProy[i]->wTileCrv.SetIn(j,ip);
					uvwProy[i]->wTileCrv.SetPoint(j,p);
					uvwProy[i]->wTileCrv.SetOut(j,op);
					}

				int gmNv,gmnx,gmny;
				fread(&gmNv,sizeof(int),1,tf);
				fread(&gmnx,sizeof(int),1,tf);
				fread(&gmny,sizeof(int),1,tf);
				uvwProy[i]->gm->nx = gmnx;
				uvwProy[i]->gm->ny = gmny;
				uvwProy[i]->gm->SetNumVerts(gmNv);
				for ( int j=0; j<gmNv; j++) {
					Point3 vert;
					fread(&vert,sizeof(float),3,tf);
					uvwProy[i]->gm->verts[j] = vert;
					}
				if (gmNv && gmnx && gmny) uvwProy[i]->BuildGMGrid();

				int isSpline;
				fread(&isSpline,sizeof(int),1,tf);
				if (isSpline) {
					char obname[256];
					read_string(obname,tf,256);
					INode *spl = ip->GetINodeByName((TCHAR*)obname);
					if (spl) AddSpline(spl,i);
					}

				// NEW IN TEXLAY 2.0
				if ( uvwProy[i]->GetMappingType()==MAP_TL_UVWDATA ) {
					int num_v;
					int num_f;
					fread(&num_v,sizeof(num_v),1,tf);
					md->group_uvw_data[i]->num_v = num_v;
					md->group_uvw_data[i]->v = new Point3[num_v];
					fread(md->group_uvw_data[i]->v,sizeof(Point3),num_v,tf);
					fread(&num_f,sizeof(num_f),1,tf);
					md->group_uvw_data[i]->num_f = num_f;
					md->group_uvw_data[i]->f = new PolyFace[num_f];
					for ( int i_f=0; i_f<num_f; i_f++ ) {
						int deg;
						fread(&deg,sizeof(deg),1,tf);
						md->group_uvw_data[i]->f[i_f].deg = deg;
						md->group_uvw_data[i]->f[i_f].vtx = new int[deg];
						fread(md->group_uvw_data[i]->f[i_f].vtx,sizeof(int),deg,tf);
						}
					}
				// NEW IN TEXLAY 2.0

				char chname[256];
				read_string(chname,tf,256);

#if MAX_VERSION_MAJOR < 15 
				uvwProy[i]->descCanal = (TSTR)chname;
#else
			
				uvwProy[i]->descCanal =  TSTR::FromCStr( chname ); //JW: is this static conversoin costly ?
#endif
				}
			}
		
		theHold.SuperAccept(_T("Load Groups"));

		fclose(tf);
		}

	if ( any_loaded ) {
		LocalDataChanged();
		UpdateUIAll();
		GroupsMenuUpdateList();
		}
	}

void MultiMapMod::LimitToSafeTile(float &val, BOOL &warning) {
	if (val < 0.0f) {
		val = 0.0f;
		warning = 1;
		}
	if (val > 1.0f) {
		val = 1.0f;
		warning = 1;
		}
	}

void MultiMapMod::LimitToSafeTile(float &val) {
	if (val < 0.0f) {
		val = 0.0f;
		}
	if (val > 1.0f) {
		val = 1.0f;
		}
	}

int MultiMapMod::FindFaceSet(TSTR &setName) {
	for (int i=0; i<named_face_sel.Count(); i++) {
		if (setName == *named_face_sel[i]) return i;
	}
	return -1;
}

DWORD MultiMapMod::AddFaceSet(TSTR &setName) {
	DWORD id = 0;
	TSTR *name = new TSTR(setName);
	named_face_sel.Append(1,&name);
	BOOL found = FALSE;
	while (!found) {
		found = TRUE;
		for (int i=0; i<face_sel_ids.Count(); i++) {
			if (face_sel_ids[i]!=id) continue;
			id++;
			found = FALSE;
			break;
		}
	}
	face_sel_ids.Append(1,&id);
	return id;
}

void MultiMapMod::RemoveFaceSet(TSTR &setName) {
	int i = FindFaceSet(setName);
	if (i<0) return;
	delete named_face_sel[i];
	named_face_sel.Delete(i,1);
	face_sel_ids.Delete(i,1);
}

void MultiMapMod::ClearFaceSetNames() {
	for (int i=0; i<3; i++) {
		for (int j=0; j<named_face_sel.Count(); j++) {
			delete named_face_sel[j];
			named_face_sel[j] = NULL;
			}
		}
	}

int MultiMapMod::FindEdgeSet(TSTR &setName) {
	for (int i=0; i<named_edge_sel.Count(); i++) {
		if (setName == *named_edge_sel[i]) return i;
	}
	return -1;
}

DWORD MultiMapMod::AddEdgeSet(TSTR &setName) {
	DWORD id = 0;
	TSTR *name = new TSTR(setName);
	named_edge_sel.Append(1,&name);
	BOOL found = FALSE;
	while (!found) {
		found = TRUE;
		for (int i=0; i<edge_sel_ids.Count(); i++) {
			if (edge_sel_ids[i]!=id) continue;
			id++;
			found = FALSE;
			break;
			}
		}
	edge_sel_ids.Append(1,&id);
	return id;
	}

void MultiMapMod::RemoveEdgeSet(TSTR &setName) {
	int i = FindEdgeSet(setName);
	if (i<0) return;
	delete named_edge_sel[i];
	named_edge_sel.Delete(i,1);
	edge_sel_ids.Delete(i,1);
	}

void MultiMapMod::ClearEdgeSetNames() {
	for (int i=0; i<3; i++) {
		for (int j=0; j<named_edge_sel.Count(); j++) {
			delete named_edge_sel[j];
			named_edge_sel[j] = NULL;
			}
		}
	}

void MultiMapMod::LocalDataChanged() {

	//MessageBox(GetCOREInterface()->GetMAXHWnd(),"MMM LocalDataChanged","Cacat",MB_OK);

	NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
	if (ip && editMod==this) {
		TimeValue t = ip->GetTime();
		ip->RedrawViews(t);
		}

	if ( uv_pelt_dlg )
		uv_pelt_dlg->InvalidateView();
	}

void MultiMapMod::ActivateSubSelSet(TSTR &setName) {
	if ( !ip )
		return;
	int level = ip->GetSubObjectLevel();

	ModContextList mcList;
	INodeTab nodes;

	int index = -1;
	if ( level == SEL_FACES )
		index = FindFaceSet(setName);
	else if ( level == SEL_EDGES )
		index = FindEdgeSet(setName);
	if (index<0) return;

	ip->GetModContexts(mcList,nodes);

	for (int i = 0; i < mcList.Count(); i++) {
		TexLayMCData *meshData = (TexLayMCData*)mcList[i]->localData;
		if (!meshData) continue;

		BitArray *set = NULL;
		if ( level == SEL_FACES ) {
			set = meshData->face_sel_sets.GetSet(face_sel_ids[index]);
			if (set) {
				if (set->GetSize() != meshData->face_sel[current_channel].GetSize()) {
					set->SetSize(meshData->face_sel[current_channel].GetSize(),TRUE);
					}
				meshData->face_sel[current_channel] = *set;
				}
			}
		else if ( level == SEL_EDGES ) {
			set = meshData->edge_sel_sets.GetSet(edge_sel_ids[index]);
			if (set) {
				if (set->GetSize() != meshData->edge_sel[current_channel].GetSize()) {
					set->SetSize(meshData->edge_sel[current_channel].GetSize(),TRUE);
					}
				meshData->edge_sel[current_channel] = *set;
				}
			}
		}
	
	nodes.DisposeTemporary();
	LocalDataChanged ();
	ip->RedrawViews(ip->GetTime());
	}

void MultiMapMod::NewSetFromCurSel(TSTR &setName) {
	if ( !ip )
		return;
	int level = ip->GetSubObjectLevel();

	ModContextList mcList;
	INodeTab nodes;
	DWORD id = -1;
	int index = -1;

	if ( level == SEL_FACES )
		index = FindFaceSet(setName);
	else if ( level == SEL_EDGES )
		index = FindEdgeSet(setName);

	if (index<0) {
		if ( level == SEL_FACES )
			id = AddFaceSet(setName);
		else if ( level == SEL_EDGES )
			id = AddEdgeSet(setName);
		}
	else {
		if ( level == SEL_FACES )
			face_sel_ids[index];
		else if ( level == SEL_EDGES )
			edge_sel_ids[index];
		}

	ip->GetModContexts(mcList,nodes);

	for (int i = 0; i < mcList.Count(); i++) {
		TexLayMCData *meshData = (TexLayMCData*)mcList[i]->localData;
		if (!meshData) continue;
		
		if ( level == SEL_FACES ) {
			BitArray *set = NULL;
			// JW Code change, explicit != NULL check, prevent C4706
			if (index>=0 && ( (set = meshData->face_sel_sets.GetSet(id)) != NULL ))
				*set = meshData->face_sel[current_channel];
			else 
				meshData->face_sel_sets.AppendSet(meshData->face_sel[current_channel],id);
			}
		else if ( level == SEL_EDGES ) {
			BitArray *set = NULL;
			// JW Code change, explicit != NULL check, prevent C4706
			if (index>=0 && ( (set = meshData->edge_sel_sets.GetSet(id))) != NULL)
				*set = meshData->edge_sel[current_channel];
			else 
				meshData->edge_sel_sets.AppendSet(meshData->edge_sel[current_channel],id);
			}
		}
		
	nodes.DisposeTemporary();
}

void MultiMapMod::RemoveSubSelSet(TSTR &setName) {
	if ( !ip )
		return;
	int level = ip->GetSubObjectLevel();
	int index = -1;

	if ( level == SEL_FACES )
		index = FindFaceSet(setName);
	else if ( level == SEL_EDGES )
		index = FindEdgeSet(setName);

	if ( index<0 )
		return;

	ModContextList mcList;
	INodeTab nodes;
	ip->GetModContexts(mcList,nodes);

	DWORD id = -1;
	if ( level == SEL_FACES )
		id = face_sel_ids[index];
	else if ( level == SEL_EDGES )
		id = edge_sel_ids[index];

	for (int i = 0; i < mcList.Count(); i++) {
		TexLayMCData *meshData = (TexLayMCData*)mcList[i]->localData;
		if (!meshData) continue;		
		if ( level == SEL_FACES )
			meshData->face_sel_sets.RemoveSet(id);
		else if ( level == SEL_EDGES )
			meshData->edge_sel_sets.RemoveSet(id);
		}
	
	if ( level == SEL_FACES )
		RemoveFaceSet(setName);
	else if ( level == SEL_EDGES )
		RemoveEdgeSet(setName);

	ip->ClearCurNamedSelSet();
	nodes.DisposeTemporary();
}

void MultiMapMod::SetupNamedSelDropDown() {
	ip->ClearSubObjectNamedSelSets();
	int level = ip->GetSubObjectLevel();
	if ( level == SEL_FACES ) {
		for (int i=0; i<named_face_sel.Count(); i++) {
			ip->AppendSubObjectNamedSelSet(*named_face_sel[i]);
			}
		}
	else if ( level == SEL_EDGES ) {
		for (int i=0; i<named_edge_sel.Count(); i++) {
			ip->AppendSubObjectNamedSelSet(*named_edge_sel[i]);
			}
		}
}

int MultiMapMod::NumNamedSelSets() {
	if ( !ip )
		return 0;
	int level = ip->GetSubObjectLevel();
	if ( level == SEL_FACES )
		return named_face_sel.Count();
	else if ( level == SEL_EDGES )
		return named_edge_sel.Count();
	return 0;
}

TSTR MultiMapMod::GetNamedSelSetName(int i) {
	if ( !ip )
		return TSTR(_T(""));
	int level = ip->GetSubObjectLevel();
	if ( level == SEL_FACES )
		return *named_face_sel[i];
	else if ( level == SEL_EDGES )
		return *named_edge_sel[i];
	return TSTR(_T(""));
}


void MultiMapMod::SetNamedSelSetName(int i,TSTR &newName) {
	if ( !ip )
		return;
	int level = ip->GetSubObjectLevel();
	if ( level == SEL_FACES )
		*named_face_sel[i] = newName;
	else if ( level == SEL_EDGES )
		*named_edge_sel[i] = newName;
	}

void MultiMapMod::NewSetByOperator(TSTR &newName, Tab <int> &sets, int op) {
	if ( !ip )
		return;
	int level = ip->GetSubObjectLevel();

	ModContextList mcList;
	INodeTab nodes;
	
	DWORD id = -1;
	if ( level == SEL_FACES )
		id = AddFaceSet(newName);
	else if ( level == SEL_EDGES )
		id = AddEdgeSet(newName);

	BOOL delSet = TRUE;
	ip->GetModContexts(mcList,nodes);
	for (int i = 0; i < mcList.Count(); i++) {
		TexLayMCData *meshData = (TexLayMCData*)mcList[i]->localData;
		if (!meshData) continue;
	
		BitArray bits;
		// JW Code Change : fix uninitialized locals warning
		GenericNamedSelSetList *setList=NULL;

		if ( level == SEL_FACES )
			setList = &meshData->face_sel_sets; 	
		else if ( level == SEL_EDGES )
			setList = &meshData->edge_sel_sets; 	

		bits = (*setList)[sets[0]];

		for (int i=1; i<sets.Count(); i++) {
			switch (op) {
			case NEWSET_MERGE:
				bits |= (*setList)[sets[i]];
				break;

			case NEWSET_INTERSECTION:
				bits &= (*setList)[sets[i]];
				break;

			case NEWSET_SUBTRACT:
				bits &= ~((*setList)[sets[i]]);
				break;
			}
		}
		if (bits.NumberSet()) delSet = FALSE;

		setList->AppendSet(bits,id);
		}
	if (delSet) 
		RemoveSubSelSet(newName);
	}

void MultiMapMod::AlignToSelection() {

	TimeValue t = ip->GetTime();

	SuspendAnimate();
	AnimateOff();

	ModContextList mcList;
	INodeTab nodeList;
	ip->GetModContexts(mcList,nodeList);

	Point3 center(0,0,0);
	Point3 normal(0,0,0);
	Tab <Point3> points;

	int num_mc = mcList.Count();
	for ( int i_mc=0; i_mc<num_mc; i_mc++ ) {
		TexLayMCData *md = (TexLayMCData*) mcList[i_mc]->localData;
		if (md == NULL) 
			continue;


#if MAX_VERSION_MAJOR < 24
		Matrix3 mctm(1);
#else
		Matrix3 mctm;
#endif




		if ( mcList[i_mc]->tm )
			mctm = *mcList[i_mc]->tm;
				
		BitArray faceSel = md->face_sel[current_channel];

		if ( md->tipo == IS_MESH ) {

			BitArray sel_verts(md->mesh->numVerts);
			sel_verts.ClearAll();
			for ( int i_f=0; i_f<md->mesh->numFaces; i_f++ ) {
				if ( faceSel[i_f] ) {
					sel_verts.Set( md->mesh->faces[i_f].v[0] );
					sel_verts.Set( md->mesh->faces[i_f].v[1] );
					sel_verts.Set( md->mesh->faces[i_f].v[2] );

					Point3 n;
					Point3 v0 = mctm * md->mesh->verts[ md->mesh->faces[i_f].v[0] ];
					Point3 v1 = mctm * md->mesh->verts[ md->mesh->faces[i_f].v[1] ];
					Point3 v2 = mctm * md->mesh->verts[ md->mesh->faces[i_f].v[2] ];
					n = Normalize( (v1-v0) ^ (v2-v0) );
					normal += n;
					}
				}

			for ( int i_v=0; i_v<md->mesh->numVerts; i_v++ ) {
				if ( sel_verts[i_v] ) {
					Point3 point = mctm * md->mesh->verts[i_v];
					points.Append(1,&point,md->mesh->numVerts);

					center = center + point;
					}
				}

			points.Shrink();
			}

		if ( md->tipo == IS_PATCH ) {
			BitArray sel_verts(md->patch->numVerts);
			sel_verts.ClearAll();
			for ( int i_f=0; i_f<md->patch->numPatches; i_f++ ) {
				if ( faceSel[i_f] ) {
					int deg = md->patch->patches[i_f].type==PATCH_QUAD?4:3;
					Point3 n(0,0,0);
					for ( int i=0; i<deg; i++ ) {
						sel_verts.Set( md->patch->patches[i_f].v[i] );

						int j = (i+1)%deg;
						int k = (i+2)%deg;
						Point3 vi = md->patch->verts[ md->patch->patches[i_f].v[i] ].p;
						Point3 vj = md->patch->verts[ md->patch->patches[i_f].v[j] ].p;
						Point3 vk = md->patch->verts[ md->patch->patches[i_f].v[k] ].p;

						n += Normalize( (vi-vj) ^ (vk-vj) );
						}
					normal += Normalize(n);
					}
				}

			for ( int i_v=0; i_v<md->patch->numVerts; i_v++ ) {
				if ( sel_verts[i_v] ) {
					Point3 point = mctm * md->patch->verts[i_v].p;
					points.Append(1,&point,md->patch->numVerts);

					center = center + point;
					}
				}
			points.Shrink();
			}

		if ( md->tipo == IS_POLY ) {
			BitArray sel_verts(md->mnMesh->numv);
			sel_verts.ClearAll();
			for ( int i_f=0; i_f<md->mnMesh->numf; i_f++ ) {
				if ( faceSel[i_f] ) {
					int deg = md->mnMesh->f[i_f].deg;
					Point3 n(0,0,0);
					for ( int i_v=0; i_v<deg; i_v++ ) {
						sel_verts.Set( md->mnMesh->f[i_f].vtx[i_v] );

						int j_v = (i_v+1)%deg;
						int k_v = (i_v+2)%deg;
						Point3 vi = mctm * md->mnMesh->v[ md->mnMesh->f[i_f].vtx[i_v] ].p;
						Point3 vj = mctm * md->mnMesh->v[ md->mnMesh->f[i_f].vtx[j_v] ].p;
						Point3 vk = mctm * md->mnMesh->v[ md->mnMesh->f[i_f].vtx[k_v] ].p;

						n += Normalize( (vi-vj) ^ (vk-vj) );
						}
					normal += Normalize(n);
					}
				}

			for ( int i_v=0; i_v<md->mnMesh->numv; i_v++ ) {
				if ( sel_verts[i_v] ) {
					Point3 point = mctm * md->mnMesh->v[i_v].p;
					points.Append(1,&point,md->mnMesh->numv);

					center = center + point;
					}
				}
			points.Shrink();
			}
		}

	if ( points.Count() < 3 )
		return;

	center = center / float( points.Count() );
	normal = Normalize(normal);

	Matrix3 natural_axis;
	Box3 natural_box;
	GetNaturalBoundingBox( points, natural_axis, natural_box );

	Point3 e_vec[3];
	Point3 axis[3];
	e_vec[0] = natural_axis.GetRow(0);
	e_vec[1] = natural_axis.GetRow(1);
	e_vec[2] = natural_axis.GetRow(2);

	// Let's find the axis z (normal) eigenvector
	int axis_z = 0;
	float axis_dp = fabs( DotProd( e_vec[0], normal ) );
	if ( fabs( DotProd( e_vec[1], normal ) ) > axis_dp ) {
		axis_dp = fabs( DotProd( e_vec[1], normal ) );
		axis_z = 1;
		}
	if ( fabs( DotProd( e_vec[2], normal ) ) > axis_dp ) {
		axis_z = 2;
		}

	axis[2] = normal;
	axis[0] = Normalize( normal ^ e_vec[(axis_z+1)%3] );
	axis[1] = Normalize( axis[2] ^ axis[0] );

	// Let's rotate the gizmo 
	int ar = align_rotation%4;
	for ( int i=1; i<=ar; i++ ) {
		Point3 aux = axis[0];
		axis[0] = axis[1];
		axis[1] = -aux;
		}
	align_rotation++;

	Matrix3 new_tm;
	new_tm.SetRow( 0, axis[0] );
	new_tm.SetRow( 1, axis[1] );
	new_tm.SetRow( 2, axis[2] );
	new_tm.SetRow( 3, center );

	Box3 box;

	for ( int i_p=0; i_p<points.Count(); i_p++ ) {
		Point3 p = points[i_p];
		box += p * Inverse(new_tm);
		}

	SetXFormPacket pckt(new_tm);		
	uvwProy[current_channel]->tmControl->SetValue(t,&pckt,TRUE,CTRL_ABSOLUTE);

	int type = uvwProy[current_channel]->GetMappingType();
	if ( type == MAP_TL_PLANAR || type == MAP_TL_BOX ) {
		uvwProy[current_channel]->SetWidth(t,box.Width().x);
		uvwProy[current_channel]->SetLength(t,box.Width().y);
		}
	else {
		uvwProy[current_channel]->SetWidth(t,box.Width().y);
		uvwProy[current_channel]->SetLength(t,box.Width().x);
		}

	uvwProy[current_channel]->SetHeight(t,box.Width().z);
	uvwProy[current_channel]->valid_group = 0;

	ResumeAnimate();
	}

// UV PELT
void MultiMapMod::DoUVPeltEditing() {
	if ( !uv_pelt_dlg ) {
		uv_pelt_dlg = new UVPeltDlg( this );
		uv_pelt_dlg->StartUVPeltDlg();
		}
	else {
		SetForegroundWindow(uv_pelt_dlg->hWnd);
		ShowWindow(uv_pelt_dlg->hWnd,SW_RESTORE);
		}
	}

void MultiMapMod::DestroyUVPeltDlg() {
	if ( uv_pelt_dlg ) {
		delete uv_pelt_dlg;
		uv_pelt_dlg = NULL;	
		}
	}

void MultiMapMod::ResetUVPelt() {
	pelt_reset_layout = TRUE;

	theHold.Begin();
	theHold.Put( new PeltFrameRestore(uvwProy[current_channel],this) );
	theHold.Accept(TSTR(_T("Reset Pelt Data")));

	uvwProy[current_channel]->PeltDeleteUV();
	uvwProy[current_channel]->PeltDeleteFrameSegments();
	uvwProy[current_channel]->valid_group = 0;
	
	LocalDataChanged();

	if ( ip )
		ip->RedrawViews(ip->GetTime());
	if ( uv_pelt_dlg )
		uv_pelt_dlg->InvalidateView();
	}

void MultiMapMod::PeltSaveLayout() {
	ModContextList mcList;
	INodeTab nodes;
	ip->GetModContexts(mcList,nodes);

	if ( nodes.Count() != 1 ) {
		MessageBox(hwnd_pelt, _T("Please select only one object"), _T("Texture Layers"),MB_OK);
		return;
		}

	TexLayMCData *md = (TexLayMCData*) mcList[0]->localData;
	BitArray face_sel = md->face_sel[current_channel];
	BitArray edge_sel = md->edge_sel[current_channel];

	uvwProy[current_channel]->PeltSaveFrame(face_sel,edge_sel);
	}

void MultiMapMod::PeltLoadLayout() {
	ModContextList mcList;
	INodeTab nodes;
	ip->GetModContexts(mcList,nodes);

	if ( nodes.Count() != 1 ) {
		MessageBox(hwnd_pelt,_T("Please select only one object"), _T("Texture Layers"),MB_OK);
		return;
		}

	TexLayMCData *md = (TexLayMCData*) mcList[0]->localData;
	BitArray face_sel = md->face_sel[current_channel];
	BitArray edge_sel = md->edge_sel[current_channel];

	theHold.Begin();
	theHold.Put( new PeltFrameRestore(uvwProy[current_channel],this) );
	theHold.Accept (GetString(IDS_TLP_LOAD_FRAME) );

	TSTR fname;
	BOOL cancel = FALSE;

	fname = uvwProy[current_channel]->PeltBrowseForFileName(FALSE,cancel);

	if (cancel) 
		return;

	int load_version;

	FILE *tf = _tfopen(fname, _T("rb"));
	fread(&load_version, sizeof(load_version),1,tf);

	if ( load_version>=1010 ) {
		int num_faces = face_sel.GetSize();
		int num_edges = edge_sel.GetSize();

		int num_load_faces;
		fread(&num_load_faces,sizeof(num_load_faces), 1, tf);

		if ( num_load_faces!=num_faces ) {
			fclose(tf);
			MessageBox(hwnd_pelt,_T("Pelt Info in file doesn't match this object"),_T("Texture Layers"),MB_OK);
			return;
			}

		for ( int i_f=0; i_f<num_faces; i_f++ ) {
			char bit;
			fread(&bit,sizeof(bit),1,tf);
			face_sel.Set(i_f,(int)bit);
			}
	
		int num_load_edges;
		fread(&num_load_edges,sizeof(num_load_edges), 1, tf);

		if ( num_load_edges!=num_edges ) {
			fclose(tf);
			MessageBox(hwnd_pelt,_T("Pelt Info in file doesn't match this object"),_T("Texture Layers"),MB_OK);
			return;
			}

		for ( int i_e=0; i_e<num_edges; i_e++ ) {
			char bit;
			fread(&bit,sizeof(bit),1,tf);
			edge_sel.Set(i_e,(int)bit);
			}
		}

	md->face_sel[current_channel] = face_sel;
	md->edge_sel[current_channel] = edge_sel;

	uvwProy[current_channel]->valid_group = 0;
	uvwProy[current_channel]->LocalDataChanged();

	uvwProy[current_channel]->PeltLoadFrame(tf);

	uvwProy[current_channel]->valid_group = 0;
	uvwProy[current_channel]->LocalDataChanged();

	ip->RedrawViews(ip->GetTime());
	if ( uv_pelt_dlg )
		uv_pelt_dlg->InvalidateView();
	
	fclose(tf);
	}


#define EPOLYOBJ_CLASS_ID		Class_ID(0x1bf8338d,0x192f6098)
void MultiMapMod::GetFaceSelectionSets() {
	if (!ip) return;
	int level = ip->GetSubObjectLevel();

	ModContextList mcList;
	INodeTab nodes;
	ip->GetModContexts(mcList,nodes);

	for (int i=0; i<mcList.Count(); i++) {
		TexLayMCData *tld = (TexLayMCData*)mcList[i]->localData;
		int numf = tld->face_sel[0].GetSize();
		Object *obj;
		
		// Test the modifiers!
		obj = nodes[i]->GetObjectRef();
		while (obj && (obj->SuperClassID() == GEN_DERIVOB_CLASS_ID)) {
	
			IDerivedObject* dobj = (IDerivedObject*)obj;
			int numMods = dobj->NumModifiers();
			obj = dobj->GetObjRef();
			for (int m=0; m<numMods; m++) {
				Modifier* mod = dobj->GetModifier(m);

				LocalModData *ld = NULL;
				ModContext *mc = dobj->GetModContext(m);
				ld = mc->localData;
				if (ld) {

/*
					IPatchSelectData *ipsd = (IPatchSelectData*)ld->GetInterface(I_PATCHSELECTDATA);
					if (ipsd) {
						if ( level == SEL_FACES ) {
							for ( int nss=0; nss<ipsd->GetNamedPatchSelList().Count(); nss++ ) {
								BitArray *ss = ipsd->GetNamedPatchSelList().GetSetByIndex(nss);

								BOOL get = TRUE;
								for (int mss=0; mss<named_face_sel.Count(); mss++)
									if ( *named_face_sel[mss] == *ipsd->GetNamedPatchSelList().names[nss] )
										get = FALSE;

								if (get) {
									DWORD id = AddFaceSet(*ipsd->GetNamedPatchSelList().names[nss]);
									tld->face_sel_sets.AppendSet(*ss,id,*ipsd->GetNamedPatchSelList().names[nss]);
									}
								}
							}
						else if ( level == SEL_EDGES ) {
							for ( int nss=0; nss<ipsd->GetNamedEdgeSelList().Count(); nss++ ) {
								BitArray *ss = ipsd->GetNamedEdgeSelList().GetSetByIndex(nss);

								BOOL get = TRUE;
								for (int mss=0; mss<named_edge_sel.Count(); mss++)
									if ( *named_edge_sel[mss] == *ipsd->GetNamedEdgeSelList().names[nss] )
										get = FALSE;

								if (get) {
									DWORD id = AddEdgeSet(*ipsd->GetNamedEdgeSelList().names[nss]);
									tld->edge_sel_sets.AppendSet(*ss,id,*ipsd->GetNamedEdgeSelList().names[nss]);
									}
								}
							}
						}
*/
					IMeshSelectData *imsd = GetMeshSelectDataInterface(ld);
					if (imsd) {
						for ( int nss=0; nss<imsd->GetNamedFaceSelList().Count(); nss++ ) {
							BitArray *ss = imsd->GetNamedFaceSelList().GetSetByIndex(nss);

							BOOL get = TRUE;
							for (int mss=0; mss<named_face_sel.Count(); mss++)
								if ( *named_face_sel[mss] == *imsd->GetNamedFaceSelList().names[nss] )
									get = FALSE;

							if (get) {
								DWORD id = AddFaceSet(*imsd->GetNamedFaceSelList().names[nss]);
								tld->face_sel_sets.AppendSet(*ss,id,*imsd->GetNamedFaceSelList().names[nss]);
								}
							}
						}
					}
				}
			obj = dobj->GetObjRef();
			}

		// Test the base object!
		obj = nodes[i]->GetObjectRef();
		obj = obj->FindBaseObject();

		if ( obj->ClassID() == Class_ID(EDITTRIOBJ_CLASS_ID,0) || obj->ClassID() == EPOLYOBJ_CLASS_ID ) {
			IMeshSelectData *imsd = GetMeshSelectDataInterface(obj);

			for ( int nss=0; nss<imsd->GetNamedFaceSelList().Count(); nss++ ) {
				BOOL get = TRUE;
				for (int mss=0; mss<named_face_sel.Count(); mss++)
					if ( *named_face_sel[mss] == *imsd->GetNamedFaceSelList().names[nss] )
						get = FALSE;

				if (get) {
					BitArray *ss = imsd->GetNamedFaceSelList().GetSetByIndex(nss);
					DWORD id = AddFaceSet(*imsd->GetNamedFaceSelList().names[nss]);
					tld->face_sel_sets.AppendSet(*ss,id,*imsd->GetNamedFaceSelList().names[nss]);
					}
				}
			}
		else if ( obj->ClassID() == Class_ID(PATCHOBJ_CLASS_ID,0) ) {
			IPatchSelectData *ipsd = (IPatchSelectData*)obj->GetInterface(I_PATCHSELECTDATA);

			if ( ipsd ) {
				for ( int nss=0; nss<ipsd->GetNamedPatchSelList().Count(); nss++ ) {
					BOOL get = TRUE;
					for (int mss=0; mss<named_face_sel.Count(); mss++)
						if ( *named_face_sel[mss] == *ipsd->GetNamedPatchSelList().names[nss] )
							get = FALSE;

					if (get) {
						BitArray *ss = ipsd->GetNamedPatchSelList().GetSetByIndex(nss);
						DWORD id = AddFaceSet(*ipsd->GetNamedPatchSelList().names[nss]);
						tld->face_sel_sets.AppendSet(*ss,id,*ipsd->GetNamedPatchSelList().names[nss]);
						}
					}
				}
			}
		}

	SetupNamedSelDropDown();
	}

void MultiMapMod::GetEdgeSelectionSets() {
	if (!ip) return;
	int level = ip->GetSubObjectLevel();

	ModContextList mcList;
	INodeTab nodes;
	ip->GetModContexts(mcList,nodes);

	for (int i=0; i<mcList.Count(); i++) {
		TexLayMCData *tld = (TexLayMCData*)mcList[i]->localData;
		int numf = tld->face_sel[0].GetSize();
		Object *obj;
		
		// Test the modifiers!
		obj = nodes[i]->GetObjectRef();
		while (obj && (obj->SuperClassID() == GEN_DERIVOB_CLASS_ID)) {


			IDerivedObject* dobj = (IDerivedObject*)obj;
			int numMods = dobj->NumModifiers();
			obj = dobj->GetObjRef();
			for (int m=0; m<numMods; m++) {
				Modifier* mod = dobj->GetModifier(m);

				LocalModData *ld = NULL;
				ModContext *mc = dobj->GetModContext(m);
				ld = mc->localData;
				if (ld) {
/*
					IPatchSelectData *ipsd = (IPatchSelectData*)ld->GetInterface(I_PATCHSELECTDATA);
					if (ipsd) {
						if ( level == SEL_FACES ) {
							for ( int nss=0; nss<ipsd->GetNamedPatchSelList().Count(); nss++ ) {
								BitArray *ss = ipsd->GetNamedPatchSelList().GetSetByIndex(nss);

								BOOL get = TRUE;
								for (int mss=0; mss<named_face_sel.Count(); mss++)
									if ( *named_face_sel[mss] == *ipsd->GetNamedPatchSelList().names[nss] )
										get = FALSE;

								if (get) {
									DWORD id = AddFaceSet(*ipsd->GetNamedPatchSelList().names[nss]);
									tld->face_sel_sets.AppendSet(*ss,id,*ipsd->GetNamedPatchSelList().names[nss]);
									}
								}
							}
						else if ( level == SEL_EDGES ) {
							for ( int nss=0; nss<ipsd->GetNamedEdgeSelList().Count(); nss++ ) {
								BitArray *ss = ipsd->GetNamedEdgeSelList().GetSetByIndex(nss);

								BOOL get = TRUE;
								for (int mss=0; mss<named_edge_sel.Count(); mss++)
									if ( *named_edge_sel[mss] == *ipsd->GetNamedEdgeSelList().names[nss] )
										get = FALSE;

								if (get) {
									DWORD id = AddEdgeSet(*ipsd->GetNamedEdgeSelList().names[nss]);
									tld->edge_sel_sets.AppendSet(*ss,id,*ipsd->GetNamedEdgeSelList().names[nss]);
									}
								}
							}
						}
*/
					IMeshSelectData *imsd = GetMeshSelectDataInterface(ld);
					if (imsd) {
						for ( int nss=0; nss<imsd->GetNamedEdgeSelList().Count(); nss++ ) {
							BitArray *ss = imsd->GetNamedEdgeSelList().GetSetByIndex(nss);

							BOOL get = TRUE;
							for (int mss=0; mss<named_edge_sel.Count(); mss++)
								if ( *named_edge_sel[mss] == *imsd->GetNamedEdgeSelList().names[nss] )
									get = FALSE;

							if (get) {
								DWORD id = AddEdgeSet(*imsd->GetNamedEdgeSelList().names[nss]);
								tld->edge_sel_sets.AppendSet(*ss,id,*imsd->GetNamedEdgeSelList().names[nss]);
								}
							}
						}
					}
				}
			obj = dobj->GetObjRef();
			}

		// Test the base object!
		obj = nodes[i]->GetObjectRef();
		obj = obj->FindBaseObject();

		if ( obj->ClassID() == Class_ID(EDITTRIOBJ_CLASS_ID,0) || obj->ClassID() == EPOLYOBJ_CLASS_ID ) {

			IMeshSelectData *imsd = GetMeshSelectDataInterface(obj);

			for ( int nss=0; nss<imsd->GetNamedEdgeSelList().Count(); nss++ ) {
				BOOL get = TRUE;
				for (int mss=0; mss<named_edge_sel.Count(); mss++)
					if ( *named_edge_sel[mss] == *imsd->GetNamedEdgeSelList().names[nss] )
						get = FALSE;

				if (get) {
					BitArray *ss = imsd->GetNamedEdgeSelList().GetSetByIndex(nss);
					DWORD id = AddEdgeSet(*imsd->GetNamedEdgeSelList().names[nss]);
					tld->edge_sel_sets.AppendSet(*ss,id,*imsd->GetNamedEdgeSelList().names[nss]);
					}
				}
			}
		else if ( obj->ClassID() == Class_ID(PATCHOBJ_CLASS_ID,0) ) {
			IPatchSelectData *ipsd = (IPatchSelectData*)obj->GetInterface(I_PATCHSELECTDATA);

			if ( ipsd ) {
				for ( int nss=0; nss<ipsd->GetNamedEdgeSelList().Count(); nss++ ) {
					BOOL get = TRUE;
					for (int mss=0; mss<named_edge_sel.Count(); mss++)
						if ( *named_edge_sel[mss] == *ipsd->GetNamedEdgeSelList().names[nss] )
							get = FALSE;

					if (get) {
						BitArray *ss = ipsd->GetNamedEdgeSelList().GetSetByIndex(nss);
						DWORD id = AddEdgeSet(*ipsd->GetNamedEdgeSelList().names[nss]);
						tld->edge_sel_sets.AppendSet(*ss,id,*ipsd->GetNamedEdgeSelList().names[nss]);
						}
					}
				}
			}
		}

	SetupNamedSelDropDown();
	}


// UVW Frame
BOOL MultiMapMod::AddFrame(INode * node) {
	if (node->TestForLoop(FOREVER,this)==REF_SUCCEED) {
		TimeValue t = ip->GetTime();

		uvwProy[current_channel]->pblock->SetValue(uvw_frame_node,t,node);

		LocalDataChanged();
		return TRUE;
		}
	else {
		return FALSE;
		}
	}

#define UVWVER	4
#define FLAG_DEAD		1
#define FLAG_HIDDEN		2
#define FLAG_FROZEN		4
//#define FLAG_QUAD		8
#define FLAG_SELECTED	16
#define FLAG_CURVEDMAPPING	32
#define FLAG_INTERIOR	64
#define FLAG_WEIGHTMODIFIED	128
#define FLAG_HIDDENEDGEA	256
#define FLAG_HIDDENEDGEB	512
#define FLAG_HIDDENEDGEC	1024

void MultiMapMod::LoadUVW() {
	static TCHAR fname[256] = {'\0'};
	OPENFILENAME ofn;
	memset(&ofn,0,sizeof(ofn));
	FilterList fl;
	fl.Append( _T("UVW File"));
	fl.Append( _T("*.uvw"));		
	TSTR title = _T("Load UVW");

	ofn.lStructSize     = sizeof(OPENFILENAME);
	ofn.hwndOwner       = hwnd_data;
	ofn.lpstrFilter     = fl;
	ofn.lpstrFile       = fname;
	ofn.nMaxFile        = 256;    
	ofn.lpstrInitialDir = ip->GetDir(APP_EXPORT_DIR);
	ofn.Flags           = OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST;
	ofn.lpstrDefExt     = _T("uvw");
	ofn.lpstrTitle      = title;

	if(!GetOpenFileName(&ofn)) return;

	FILE *file = _tfopen(fname,_T("rb"));

	int ver;
	int vct;
	fread(&ver, sizeof(ver), 1,file);
	fread(&ver, sizeof(ver), 1,file);
	fread(&vct, sizeof(vct), 1,file);

	temp_num_v = vct;
	temp_v = new Point3[vct];

	for ( int i_v=0; i_v<temp_num_v; i_v++ ) {
		Point3 uv;
		float inf;
		int flags;
		fread(&uv, sizeof(uv), 1,file);
		fread(&inf, sizeof(inf), 1,file);
		fread(&flags, sizeof(flags), 1,file);
		temp_v[i_v] = uv;
		}

	int fct;
	fread(&fct, sizeof(fct), 1,file);
	temp_num_f = fct;
	temp_f = new PolyFace[fct];

	temp_face_sel.SetSize(temp_num_f);

	for ( int i_f=0; i_f<temp_num_f; i_f++ ) {
		int face_index = i_f;
		int count;
		int mat_id = 0;
		int flags = 0;

		ULONG nb = 1;
		fread(&count, sizeof(count), 1,file);

		temp_f[i_f].deg = count;
		temp_f[i_f].vtx = new int[count];

		int * temp_count = new int[count];
		int * temp_count_2 = new int[count*2];

		fread(temp_f[i_f].vtx, sizeof(temp_f[i_f].vtx)*count, 1,file);
		fread(&face_index, sizeof(face_index), 1,file);
		fread(&mat_id, sizeof(mat_id), 1,file);
		fread(&flags, sizeof(flags), 1,file);
		fread(temp_count, sizeof(temp_count)*count, 1,file);
		if (flags & FLAG_CURVEDMAPPING) {
			fread(temp_count_2, sizeof(int)*count*2, 1, file);
			fread(temp_count, sizeof(int)*count, 1, file);
			fread(temp_count_2, sizeof(int)*count*2, 1, file);
			fread(temp_count, sizeof(int)*count, 1, file);
			}

		delete [] temp_count;
		delete [] temp_count_2;

		if ( flags&FLAG_SELECTED )
			temp_face_sel.Set(i_f);
		}

	fclose(file);
	}

void MultiMapMod::GetSaveUVWFilename() {
	save_uvw_fname[0] = 0;

	OPENFILENAME ofn;
	memset(&ofn,0,sizeof(ofn));
	FilterList fl;
	fl.Append( GetString(IDS_TL2_UVWFILES));
	fl.Append( _T("*.uvw"));		
	TSTR title = GetString(IDS_TL2_SAVEOBJECT);

	ofn.lStructSize     = sizeof(OPENFILENAME);
	ofn.hwndOwner       = GetCOREInterface()->GetMAXHWnd();
	ofn.lpstrFilter     = fl;
	ofn.lpstrFile       = save_uvw_fname;
	ofn.lpstrInitialDir = ip->GetDir(APP_EXPORT_DIR);
	ofn.nMaxFile        = 256;    
	ofn.Flags           = OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST;
	ofn.lpstrDefExt     = _T("uvw");
	ofn.lpstrTitle      = title;

	tryAgain:
	if (GetSaveFileName(&ofn)) {
		if (DoesFileExist(save_uvw_fname)) {
			TSTR buf1;
			TSTR buf2 = GetString(IDS_TL2_SAVEOBJECT);
			buf1.printf(GetString(IDS_TL2_FILEEXISTS),save_uvw_fname);
			if (IDYES!=MessageBox(hwnd_data,buf1,buf2,MB_YESNO|MB_ICONQUESTION)) {
				goto tryAgain;
				}
			}
		}
	}

void MultiMapMod::SaveUVW() {
	save_uvw_channel = -1;

	if ( save_uvw_fname[0]!=0 ) {

		FILE *file = _tfopen(save_uvw_fname,_T("wb"));

		int ver = -1;
		fwrite(&ver, sizeof(ver), 1,file);
		ver = UVWVER;
		fwrite(&ver, sizeof(ver), 1,file);

		fwrite(&temp_num_v, sizeof(temp_num_v), 1,file);

		for ( int i_v=0; i_v<temp_num_v; i_v++ ) {
			Point3 uvw = temp_v[i_v];
			float inf = 0.0f;
			int flags = 0;
			fwrite(&uvw, sizeof(uvw), 1,file);
			fwrite(&inf, sizeof(inf), 1,file);
			fwrite(&flags, sizeof(flags), 1,file);
			}

		fwrite(&temp_num_f, sizeof(temp_num_f), 1,file);

		for ( int i_f=0; i_f<temp_num_f; i_f++ ) {
			PolyFace &face = temp_f[i_f];

			int face_index = i_f;
			int count = face.deg;
			int mat_id = 0;
			int flags = 0;

			if ( temp_face_sel[i_f] )
				flags = flags|FLAG_SELECTED;

			ULONG nb = 1;
			fwrite(&count, sizeof(count), 1,file);
			fwrite(face.vtx, sizeof(face.vtx)*count, 1,file);
			fwrite(&face_index, sizeof(face_index), 1,file);
			fwrite(&mat_id, sizeof(mat_id), 1,file);
			fwrite(&flags, sizeof(flags), 1,file);
			fwrite(face.vtx, sizeof(face.vtx)*count, 1,file);
			}

		fclose(file);
		}

	CleanTempUVWData();
	}

void MultiMapMod::SaveGroupUVW( int group ) {
	ModContextList list;
	INodeTab nodes;	
	ip->GetModContexts(list,nodes);
	TexLayMCData *d = (TexLayMCData*)list[0]->localData;

	PolyUVWData * group_uvw_data = d->group_uvw_data[group];
	temp_face_sel = d->face_sel[group];

	if ( temp_face_sel.NumberSet() == 0 ) {
		temp_face_sel.SetAll();
		}

	temp_num_v = group_uvw_data->num_v;
	temp_num_f = group_uvw_data->num_f;
	temp_v = new Point3[temp_num_v];
	temp_f = new PolyFace[temp_num_f];

	for ( int i_v=0; i_v<temp_num_v; i_v++ ) 
		temp_v[i_v] = group_uvw_data->v[i_v];

	for ( int i_f=0; i_f<temp_num_f; i_f++ ) {
		int deg = group_uvw_data->f[i_f].deg;
		temp_f[i_f].deg = deg;
		temp_f[i_f].vtx = new int[deg];
		for ( int i_v=0; i_v<deg; i_v++ )
			temp_f[i_f].vtx[i_v] = group_uvw_data->f[i_f].vtx[i_v];
		}
	
	GetSaveUVWFilename();
	SaveUVW();
	
	CleanTempUVWData();
	}

void MultiMapMod::LoadGroupUVW( int group ) {
	ModContextList list;
	INodeTab nodes;	
	ip->GetModContexts(list,nodes);
	TexLayMCData *d = (TexLayMCData*)list[0]->localData;

	if ( nodes.Count() > 1 ) {
		MessageBox(hwnd_main,_T("Please select just one object to load the UVW file"),_T("Texture Layers"),MB_OK);
		}

	PolyUVWData * group_uvw_data = d->group_uvw_data[group];

	LoadUVW();

	int tipo = d->tipo;
	// JW Code Change : fix uninitialized locals warning
	int num_faces=0;
	if ( tipo == IS_MESH ) 
		num_faces = d->mesh->numFaces;
	if ( tipo == IS_POLY ) 
		num_faces = d->mnMesh->numf;
	if ( tipo == IS_PATCH ) 
		num_faces = d->patch->numPatches;

	BOOL good_uvw_data = TRUE;

	if ( temp_num_v==0 || temp_num_f==0 )
		good_uvw_data = FALSE;

	if ( temp_num_f!=num_faces )
		good_uvw_data = FALSE;

	if ( good_uvw_data ) {
		for ( int i_f=0; i_f<temp_num_f; i_f++ ) {
			int count = temp_f[i_f].deg;

			if ( tipo == IS_MESH && count!=3 )
				good_uvw_data = FALSE;

			if ( tipo == IS_PATCH && count!=4 )
				good_uvw_data = FALSE;

			if ( tipo == IS_POLY && count!=d->mnMesh->f[i_f].deg )
				good_uvw_data = FALSE;

			for ( int i_v=0; i_v<count; i_v++ ) {
				if ( temp_f[i_f].vtx[i_v] >= temp_num_v )
					good_uvw_data = FALSE;
				}
			}
		}

	if ( good_uvw_data ) {
		group_uvw_data->DeleteUVWData();
		group_uvw_data->num_v = temp_num_v;
		group_uvw_data->num_f = temp_num_f;
		group_uvw_data->v = new Point3[temp_num_v];
		group_uvw_data->f = new PolyFace[temp_num_f];

		for ( int i_v=0; i_v<temp_num_v; i_v++ ) {
			group_uvw_data->v[i_v] = temp_v[i_v];
			}

		for ( int i_f=0; i_f<temp_num_f; i_f++ ) {
			int deg = temp_f[i_f].deg;
			group_uvw_data->f[i_f].deg = deg;
			group_uvw_data->f[i_f].vtx = new int[deg];
			for ( int i_v=0; i_v<deg; i_v++ ) 
				group_uvw_data->f[i_f].vtx[i_v] = temp_f[i_f].vtx[i_v];
			}

		d->face_sel[group] = temp_face_sel;
		}
	else 
		MessageBox(hwnd_main,_T("Invalid data in file"),_T("Texture Layers"),MB_OK);

	CleanTempUVWData();
	LocalDataChanged();
	}

void MultiMapMod::CleanTempUVWData() {
	if ( temp_v ) {
		delete [] temp_v;
		temp_v = NULL;
		}
	temp_num_v = 0;

	if ( temp_f ) {
		for ( int i=0; i<temp_num_f; i++ ) {
			delete [] temp_f[i].vtx;
			temp_f[i].vtx = NULL;
			temp_f[i].deg = 0;
			}
		delete [] temp_f;
		temp_f = NULL;
		}
	temp_num_f = 0;

	temp_face_sel.SetSize(0);
	}
