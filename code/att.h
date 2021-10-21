#ifndef _ATTMAP_H_
#define _ATTMAP_H_

#include "mtlhdr.h"
#include "modsres.h"
#include "stdmat.h"
#include <bmmlib.h>
#include "texutil.h"
#include "mapping.h"
#include "meshdata.h"
#include "texlay.h"

#include <vector>

extern HINSTANCE hInstance;

static LRESULT CALLBACK CurveWndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );

#define NSUBTEX 1    // number of texture map slots

#define SHOW_CURVE	3
#define	SHOW_BOTTOM	2
#define	SHOW_TOP	1
#define SHOW_ALL	0

//*****  Paramblock 2 enums  ******

enum { attmap_params };  // pblock ID
// rgbtints param IDs
enum 
{ 
	attmap_map,
	attmap_map_on, // main grad params 
	pb_att_face,
	pb_att_mod,
	pb_map_channel,
	attuvwn_type,
	uvwn_center,uvwn_start,uvwn_offset,uvwn_in,uvwn_out,
	uvwn_uvwnormal,
	pb_use_mod,	pb_mod_group,
};

//*****                      *******

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
//// Win32 : static BOOL CALLBACK DummyDlgProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
//static INT_PTR CALLBACK DummyDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) 
//{ 
//	return TRUE;
//}
//#endif

//--------------------------------------------------------------
// AttMap: A Composite texture map
//--------------------------------------------------------------
class AttMap: public MultiTex { 
	friend class TintPostLoad;
	Texmap* subTex[NSUBTEX];  // More refs
	Interval ivalid;
	BOOL rollScroll;
	public:

		MultiMapMod * mmm;

		TimeValue curtime;

		int mod_group;

		float atCentre;
		float atStart;
		float atOffset;
		float atIn;
		float atOut;

		int att_mod;
		int att_uvwn;
		int att_face;

		int att_normal;

		Tab <INode*> surfs;		// Tabla con los objetos que tienen aplicado el aatmap

		static std::vector<MeshExtraData*> mesh_datas;

		BOOL Param1;
		BOOL mapOn[NSUBTEX];
		IParamBlock2 *pblock;   // ref #0

		AttMap();
		~AttMap();
		ParamDlg* CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp);
		void Update(TimeValue t, Interval& valid);
		void Init();
		void Reset();

		// This plugin needs to be Thread Safe, so...
		CRITICAL_SECTION csect;

		Interval Validity(TimeValue t) { Interval v; Update(t,v); return ivalid; }

		void NotifyChanged();

		// Evaluate the color of map for the context.
		AColor EvalColor(ShadeContext& sc);
		float EvalMono(ShadeContext& sc);
		AColor EvalFunction(ShadeContext& sc, float u, float v, float du, float dv);

		void LocalMappingsRequired(int subMtlNum, BitArray &mapreq, BitArray &bumpreq);
		
		// For Bump mapping, need a perturbation to apply to a normal.
		// Leave it up to the Texmap to determine how to do this.
		Point3 EvalNormalPerturb(ShadeContext& sc);

		// Methods to access texture maps of material
		int NumSubTexmaps() { return NSUBTEX; }
		Texmap* GetSubTexmap(int i) { return subTex[i]; }
		void SetSubTexmap(int i, Texmap *m);

#if MAX_VERSION_MAJOR < 24
		TSTR GetSubTexmapSlotName(int i);
#else
		TSTR GetSubTexmapSlotName(int i,  bool localized = false);
#endif

		Class_ID ClassID() {	return ATTMAP_CID; }
		SClass_ID SuperClassID() { return TEXMAP_CLASS_ID; }

#if MAX_VERSION_MAJOR < 24
		void GetClassName(TSTR& s) { s= GetString(IDS_DC_ATTMAP); }  
#else
		void GetClassName(TSTR& s, bool localized = false) { s = GetString(IDS_DC_ATTMAP); }
#endif
		void DeleteThis() { delete this; }	

		int NumSubs() { return 1+NSUBTEX; }  
		Animatable* SubAnim(int i);

#if MAX_VERSION_MAJOR < 24
		TSTR SubAnimName(int i);
#else
		TSTR SubAnimName(int i, bool localized = false );
#endif

		int SubNumToRefNum(int subNum) { return subNum; }

		// From ref
 		int NumRefs() { return 1+NSUBTEX; }
		RefTargetHandle GetReference(int i);
		void SetReference(int i, RefTargetHandle rtarg);

#ifndef MAX_RELEASE_R9
		RefTargetHandle Clone(RemapDir& remap = NoRemap());
#else
		RefTargetHandle Clone(RemapDir& remap = DefaultRemapDir());
#endif

// JW Code Change: NotifyRefChanged signature changed in 3ds Max 2015+
#if MAX_VERSION_MAJOR < 17
		RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message);
#else
		RefResult NotifyRefChanged(const Interval& changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message, BOOL propagate);
#endif

		// IO
		IOResult Save(ISave *isave);
		IOResult Load(ILoad *iload);
// JBW: direct ParamBlock access is added
		int	NumParamBlocks() { return 1; }					// return number of ParamBlocks in this instance
		IParamBlock2* GetParamBlock(int i) { return pblock; } // return i'th ParamBlock
		IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock->ID() == id) ? pblock : NULL; } // return id'd ParamBlock

		void SetData(INode* node, ShadeContext &sc, Class_ID modCID);
		float GetUVWAttenuation(float v);

		int RenderBegin(TimeValue t, ULONG flags=0);
		int RenderEnd(TimeValue t);

		void SetMapGroup( int map_group );
	};

//****************************************************************************************
//			DIALOG STUFF
//****************************************************************************************

#define BORD_VER	6 
#define BORD_HOR	10

// Authorization Stuff    //


//dialog stuff to get the Set Ref button
class AttMapDlgProc : public ParamMap2UserDlgProc {
//public ParamMapUserDlgProc {
	public:
		AttMap *atmap;	
		
		ISpinnerControl * map_group_spin;

		AttMapDlgProc(AttMap *m) {atmap = m;}	
		//Win32 : BOOL DlgProc(TimeValue t,IParamMap2 *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);		
		INT_PTR DlgProc(TimeValue t,IParamMap2 *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);		
		void DeleteThis() {delete this;}
		void SetThing(ReferenceTarget *m) {
			atmap = (AttMap*)m;
			}
		void DrawCurve(HWND hWnd, HDC hdc);
		void UpdateUI(HWND hWnd);
	};

#endif