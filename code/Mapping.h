/**********************************************************************
 *<
	FILE: mapping.h

	DESCRIPTION:  Texture Layers that kick the native

	CREATED BY: Diego

	HISTORY: 7/01/98

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#ifndef __MAPPING__
#define __MAPPING__

#include "iparamm2.h"
#include "istdplug.h"
#include "meshadj.h"
#include "modstack.h"
#include "macrorec.h"
#include "spline3d.h"
#include "shape.h"
#include "CCurve.h"
#include "ListBoxHandler.h"
#include "tl_classes.h"
#include "uvw_data.h"

//#ifdef ALPS_PROTECTED
//
//#else
//
//#pragma message ("This plugin is not protected") 
//
//#endif

#define TL_PBLOCK_REF 0

#define PROD_CODE1_R3	12
#define PROD_CODE2_R3	9
#define PROD_CODE3_R3	11

#define MAP_TL_PLANAR			0
#define MAP_TL_CYLINDRICAL		1
#define MAP_TL_CYLCAP			2
#define MAP_TL_SPHERICAL		3
#define MAP_TL_BALL				4
#define MAP_TL_BOX				5
#define MAP_TL_FACE				6
#define MAP_TL_XYZ				7
#define MAP_TL_SPLINE			8
#define MAP_TL_FFM				9
#define MAP_TL_PELT				10
#define MAP_TL_FRAME			11
#define MAP_TL_UVWDATA			12

#define SHOW_PROYS	0
#define SHOW_SPLINE	1
#define SHOW_FFM	2

#define UNDEFINED	0xffffffff

//--- Parameter map/block descriptors -------------------------------

#define PB_US			0
#define PB_UE			1
#define PB_VS			2
#define PB_VE			3
#define PB_WS			4
#define PB_WE			5
#define PB_ATTON		6
#define PB_ATT			7
#define PB_AUS			8
#define PB_AUE			9
#define PB_AVS			10
#define PB_AVE			11
#define PB_AWS			12
#define PB_AWE			13
#define PB_RUV			14
#define PB_LENGTH		15
#define PB_WIDTH		16
#define PB_HEIGHT		17
#define PB_AXIS			18
#define PB_NORMALIZE	19
#define PB_REVERSE		20
#define PB_NORMALS		21
#define PB_FS			22

// Preview Status
#define NO_PREVIEW		0
#define YES_PREVIEW		1

// Preview Options
#define CUR_PREVIEW		0
#define ALL_PREVIEW		1

// Selection levels
#define SEL_OBJECT		0
#define SEL_GIZMO		1
#define SEL_POINTS		2
#define SEL_FACES		3
#define SEL_EDGES		4

#define TL_VERSION		1


#define MULTIMAP_MOD_CID			Class_ID(0x36833c88, 0x3d201e12)

#define CONFIGNAME _T("texlay.cod")

// Paramblock2 name
enum { uvw_proy_params, }; 
// Paramblock2 parameter list
enum {	
		uvw_tile_u,			uvw_offset_u, 
		uvw_tile_v,			uvw_offset_v,	
		uvw_tile_w,			uvw_offset_w,
		uvw_atton,			uvw_att,
		uvw_aus,			uvw_aue,
		uvw_avs,			uvw_ave,
		uvw_aws,			uvw_awe,
		uvw_aruv,	
		uvw_length,			uvw_width,			uvw_height,
		uvw_axis,		
		uvw_normlize,		uvw_reverse,
		uvw_normals,
		uvw_ffm_thresh,
		uvw_map_channel,
		uvw_move_u,			uvw_scale_u,
		uvw_move_v,			uvw_scale_v,
		uvw_move_w,			uvw_scale_w,
		uvw_pelt_rigidity,	uvw_pelt_stability,
		uvw_pelt_frame,		uvw_pelt_iter,
		uvw_pelt_border,
		uvw_frame_node,	
		uvw_rotate_u,		uvw_rotate_v,		uvw_rotate_w,
		uvw_xform_att,		uvw_spline_belt,	uvw_normals_start,
		uvw_active_status,	uvw_mapping_type,	uvw_spline_node,
		max_uvw,

	};

// UVW Proyector List of references

#define TILEU_REF		0

#define MULTIMAP_MOD_CID	Class_ID(0x36833c88, 0x3d201e12)

#define UVW_SCID			0x0001ab

#define UVW_PROY_CID	Class_ID(0x71cf469d, 0x3b565cee)
#define GRID_MESH_CID	Class_ID(0x56cd1936, 0x4adf3425)
static inline Point3 pabs(Point3 p);

#define I_MAPPINGINTERFACE	0x9836d7f1
#define I_TEXLAYINTERFACE	0x8459f4e8
#define GetTexLayInterface(anim) ((MultiMapMod*)anim->GetInterface(I_TEXLAYINTERFACE))

// UVWProyector reference:
#define PBLOCK_REF		0
#define TMCTRL_REF		1
#define SPLINE_REF		2
#define GRID_REF		3

#define NO_UVW	Point3(0,0,0)


// Flags
// Estas son las diferentes flags...
#define CONTROL_FIT			(1<<0)
#define CONTROL_CENTER		(1<<1)
#define CONTROL_ASPECT		(1<<2)
#define CONTROL_UNIFORM		(1<<3)
#define CONTROL_HOLD		(1<<4)
#define CONTROL_INIT		(1<<5)
#define CONTROL_OP			(CONTROL_FIT|CONTROL_CENTER|CONTROL_ASPECT|CONTROL_UNIFORM)
#define CONTROL_INITPARAMS	(1<<10)

#define CID_FACEALIGNMAP 	0x4f298c7a
#define CID_REGIONFIT 		0x4f298c7b

// Panel Visibility flags
#define PV_BLOCK		(1<<0)
#define PV_TIPO			(1<<1)


void ReadCfg(IObjParam *ip);
static int ACode;
static int Serial;

// TexLay Paramblock2 name
enum { texlay_params, }; 
// Paramblock2 parameter list
enum {	tlp_change };

class PickNodeMode;
class FaceAlignMode;
class RegionFitMode;
class PickAcquire;
class UVPeltDlg;
class UVWProyector;

class MultiMapMod : public Modifier, TimeChangeCallback
{	
public:
friend BOOL CALLBACK FlatMapsDlgProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );

	bool auth_ok;

		Matrix3 this_node_tm;
		BOOL in_modify_object;
	
		TCHAR save_uvw_fname[256];

		BOOL pelt_reset_layout;

		BitArray temp_face_sel;
		int temp_num_v;
		int temp_num_f;
		Point3 * temp_v;
		PolyFace * temp_f;

		int show_all_gizmos;
		BOOL compact_tverts;
		int save_uvw_channel;

		int top_group;
		int current_channel;

		// This var is TRUE if the scene comes originally from TexLay1.0 or before
		BOOL texlay_1_0;

		BOOL updateCache;
		int objType;
				
		// Loading old files stuff.
		int loadold;
		Tab <TSTR*> splNames;

		DWORD flags;
//		float aspect;
		BOOL pickingSpline;
		BOOL selecting_faces;
		BOOL selecting_edges;

		int cut;
		int copy;
		int paste;
		int cutted;
		int copied;
		BitArray baCutted;

		int preview;
		int previewType;

		static IObjParam *ip;
		
		void* GetInterface(ULONG id);

		TSTR loadFileName;

		int align_rotation;

		// El boton para picar el spline
		static PickNodeMode *pickMode;

		BOOL inRender;
		BOOL nurbs2mesh;

		Tab <UVWProyector*> uvwProy;

		static UVWProyector *uvw_buffer;
		static BitArray     face_buffer;
		static BitArray     edge_buffer;
		static PolyUVWData* uvw_data_buffer;

		CurveDlg *tileDlg;
		CurveDlg *radiusDlg;
		CurveDlg *angleDlg;

		static UVPeltDlg * uv_pelt_dlg;

		static HWND hwnd_main;
		static CListBoxHandler	*groups_menu_handler;
		static ICustEdit *group_name_edit;
		static ICustToolbar *iToolbar;
		static ICustButton *iAdd;
		static ICustButton *iDel;
		static ICustButton *iSave;
		static ICustButton *iLoad;
		static ICustButton *iCopy;
		static ICustButton *iPaste;
		static ICustButton *iTools;
		static ISpinnerControl *mapch_spin;
		static ISpinnerControl *length_spin;
		static ISpinnerControl *width_spin;
		static ISpinnerControl *height_spin;

		static HWND hwnd_tiling;
		static ISpinnerControl *u_start_spin;
		static ISpinnerControl *u_offset_spin;
		static ISpinnerControl *v_start_spin;
		static ISpinnerControl *v_offset_spin;
		static ISpinnerControl *w_start_spin;
		static ISpinnerControl *w_offset_spin;
		static ISpinnerControl *att_all_spin;
		static ISpinnerControl *att_u_start_spin;
		static ISpinnerControl *att_u_offset_spin;
		static ISpinnerControl *att_v_start_spin;
		static ISpinnerControl *att_v_offset_spin;
		static ISpinnerControl *att_w_start_spin;
		static ISpinnerControl *att_w_offset_spin;
		static ISpinnerControl *att_ruv_spin;
		static ISpinnerControl *u_move_spin;
		static ISpinnerControl *v_move_spin;
		static ISpinnerControl *w_move_spin;
		static ISpinnerControl *u_rotate_spin;
		static ISpinnerControl *v_rotate_spin;
		static ISpinnerControl *w_rotate_spin;
		static ISpinnerControl *u_scale_spin;
		static ISpinnerControl *v_scale_spin;
		static ISpinnerControl *w_scale_spin;

		static HWND hwnd_tools;

		static HWND hwnd_spline;
		static ICustButton * pick_spline_button;
		static ISpinnerControl *normals_start_spin;

		static HWND hwnd_free;
		static ICustButton * pick_surface_button;
		static ISpinnerControl *free_threshold_spin;

		static HWND hwnd_pelt;
		static ISpinnerControl *pelt_rigidity_spin;
		static ISpinnerControl *pelt_stability_spin;
		static ISpinnerControl *pelt_frame_spin;
		static ISpinnerControl *pelt_iter_spin;
	
		static HWND hwnd_frame;
		static ICustButton * pick_frame_button;

		static HWND hwnd_data;

		static HWND hIsNurbs;
		BOOL isNurbs;

		static HWND hwnd_options;

		int show;					// What panel?

		static MoveModBoxCMode *moveMode;
		static RotateModBoxCMode *rotMode;
		static UScaleModBoxCMode *uscaleMode;
		static NUScaleModBoxCMode *nuscaleMode;
		static SquashModBoxCMode *squashMode;		
		static SelectModBoxCMode *selectMode;
		static FaceAlignMode *faceAlignMode;
		static RegionFitMode *regionFitMode;
		static PickAcquire pickAcquire;
		static MultiMapMod *editMod;

		MultiMapMod(BOOL create);	
		~MultiMapMod();
				
		// From Animatable
		void DeleteThis() { delete this; }
		void GetClassName(TSTR& s) {s=GetString(IDS_TL_MODNAME);}
		virtual Class_ID ClassID() {return MULTIMAP_MOD_CID;}
		void BeginEditParams( IObjParam  *ip, ULONG flags,Animatable *prev);
		void EndEditParams( IObjParam *ip,ULONG flags,Animatable *next);

		void CreateUI();
		void InitMainRollup();
		void InitTilingRollup();
		void InitToolsRollup();
		void InitSplineRollup();
		void InitFreeRollup();
		void InitPeltRollup();
		void InitFrameRollup();
		void InitDataRollup();

		void AddNewGroup();
		void DeleteCurrentGroup();
		void DisplayOptions();
		void DoBitmapFit();

		void CloseAllRollups();
		void OpenCloseRollups();
		void UpdateUIAll();
		void UpdateUIItem(int id, UVWProyector* uvw_proy);

		void UpdatePBlockParam(int id);

		void CloseCurveDlgs();

//JW Code Change: GetObjectName singature changed with 3ds Max 2013+
#if MAX_VERSION_MAJOR < 15
		TCHAR *GetObjectName() { return GetString(IDS_DC_TEXLAYERS); }
#else
		const TCHAR *GetObjectName() { return GetString(IDS_DC_TEXLAYERS); }
#endif
		CreateMouseCallBack* GetCreateMouseCallBack() {return NULL;} 
//		BOOL AssignController(Animatable *control,int subAnim);
		int SubNumToRefNum(int subNum);
		int RenderBegin(TimeValue t, ULONG flags);
		int RenderEnd(TimeValue t);

		ChannelMask ChannelsUsed()  {return PART_GEOM|PART_TOPO|PART_SELECT|PART_SUBSEL_TYPE|PART_VERTCOLOR;}
		ChannelMask ChannelsChanged() {return TEXMAP_CHANNEL|PART_VERTCOLOR|PART_SELECT|PART_SUBSEL_TYPE; }	

		// Cambie de mapobjectclassid a triobjectclassid
		Class_ID InputType();// { return mapObjectClassID;}
		Matrix3 GenMatrix(TimeValue t,ModContext *mc);
		void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);
		Interval LocalValidity(TimeValue t);

		void ComputeObjectCRC( Mesh &mesh, float &crc_geom, int &crc_topo );
		void ComputeObjectCRC( MNMesh &mnmesh, float &crc_geom, int &crc_topo );
		void ComputeObjectCRC( PatchMesh &pmesh, float &crc_geom, int &crc_topo );

		IOResult LoadNamedSelChunk(ILoad *iload);
		IOResult Load(ILoad *iload);
		IOResult Save(ISave *isave);
		IOResult SaveLocalData(ISave *isave, LocalModData *pld);
		IOResult LoadLocalData(ILoad *iload, LocalModData **pld);

		int NumRefs() {
			if (loadold) return 0;
			return uvwProy.Count();
			}
		RefTargetHandle GetReference(int i);
		void SetReference(int i, RefTargetHandle rtarg);
		void LocalDataChanged();

		// Modifico el numero de cositos animables
		// MAX 3.0
		int NumSubs() {return uvwProy.Count();}
		Animatable* SubAnim(int i);
		TSTR SubAnimName(int i);

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

		int NumSubObjTypes();
		ISubObjType *GetSubObjType(int i);
		void ActivateSubobjSel(int level, XFormModes& modes);

		Tab<TSTR*> named_face_sel;		
		Tab<DWORD> face_sel_ids;

		Tab<TSTR*> named_edge_sel;		
		Tab<DWORD> edge_sel_ids;

		BOOL SupportsNamedSubSels() {return TRUE;}
		void ActivateSubSelSet(TSTR &setName);
		void NewSetFromCurSel(TSTR &setName);
		void RemoveSubSelSet(TSTR &setName);
		void SetupNamedSelDropDown();
		int NumNamedSelSets();
		TSTR GetNamedSelSetName(int i);
		void SetNamedSelSetName(int i,TSTR &newName);
		void NewSetByOperator(TSTR &newName,Tab<int> &sets,int op);

		// Local methods for handling named selection sets
		int FindFaceSet(TSTR &setName);		
		DWORD AddFaceSet(TSTR &setName);
		void RemoveFaceSet(TSTR &setName);
		void ClearFaceSetNames();

		int FindEdgeSet(TSTR &setName);		
		DWORD AddEdgeSet(TSTR &setName);
		void RemoveEdgeSet(TSTR &setName);
		void ClearEdgeSetNames();

		void GetEdgeSelectionSets();
		void GetFaceSelectionSets();

		void SelectSubComponent(HitRecord *hitRec, BOOL selected, BOOL all, BOOL invert);
		void ClearSelection(int selLevel);
		void SelectAll(int selLevel);
		void InvertSelection(int selLevel);
		void Move(TimeValue t, Matrix3& partm, Matrix3& tmAxis, Point3& val, BOOL localOrigin=FALSE);
		void Rotate(TimeValue t, Matrix3& partm, Matrix3& tmAxis, Quat& val, BOOL localOrigin=FALSE);
		void Scale(TimeValue t, Matrix3& partm, Matrix3& tmAxis, Point3& val, BOOL localOrigin=FALSE);

		void GetSubObjectCenters(SubObjAxisCallback *cb,TimeValue t,INode *node,ModContext *mc);
		void GetSubObjectTMs(SubObjAxisCallback *cb,TimeValue t,INode *node,ModContext *mc);

		void SwitchTileDlgCurve();
		void SwitchRadiusDlgCurve();
		void SwitchAngleDlgCurve();

		void EnterNormalAlign();
		void ExitNormalAlign();
		void EnterRegionFit();
		void ExitRegionFit();
		void EnterAcquire();
		void ExitAcquire();

		int GetFirstParamVer();
		int GetPBlockVersion();

		// Metodos para la comunicacion con el Bitmap 
		float GetAtt(Point3 p, int channel);

		// Methods involve with the channels manipulation...
		void SetNumGroups(int n);
		void CopyPBlock(int dest,int ori);
		void ChannelChange(int newcan);
		void RenameChannel(TCHAR* buf);
		void SetActive(int index);
		void CopyLayerToBuffer();
		BOOL PasteBufferToLayer();

		void LimitToSafeTile(float &val, BOOL &warning);	
		void LimitToSafeTile(float &val);	
	
		int Display(TimeValue t, INode* inode, ViewExp *vpt, int flags, ModContext *mc);
		int DoIcon(TimeValue t, PolyLineProc& lp,int channel,BOOL sel, int level, GraphicsWindow *gw = NULL, ViewExp *vpt = NULL, INode *inode = NULL, ModContext* m=NULL, int hit=0, int hitflags=0);

		void DoBoxIcon(int channel,BOOL sel,float length, PolyLineProc& lp);
		void DoMMPlanarMapIcon(int channel,TimeValue t, BOOL sel,float width, float length, float height, PolyLineProc& lp);
		void DoMMSphericalMapIcon(int channel, TimeValue t, BOOL sel,float radius, PolyLineProc& lp);
		void DoMMShrinkMapIcon(int channel, TimeValue t, BOOL sel,float radius, PolyLineProc& lp);
		void DoMMCylindricalMapIcon(int channel, TimeValue t, BOOL sel,float radius, float height, PolyLineProc& lp);
		void DoMMSplineIcon(int channel, TimeValue t, BOOL sel, PolyLineProc &lp);
		int DoFFMIcon(int channel, TimeValue t, BOOL sel, PolyLineProc& lp,int level,ViewExp *vpt = NULL,GraphicsWindow *gw = NULL, INode *inode = NULL, ModContext* mc=NULL, int hit=0, int hitflags=0);

		int HitTest(TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt, ModContext* mc);
		void ViewportAlign();
		void GetWorldBoundBox(TimeValue t,INode* inode, ViewExp *vpt, Box3& box, ModContext *mc);

		// Spline Mapping Methods

		BOOL AddSpline(INode *node);
		BOOL AddSpline(INode *node, int chann);
		void SetNodeName();
		void TextNumPoints();
		void ResetNode();
		void ShapeFFGPoints(INode *surf);

		void PanelVisibility(int pvflags);



		// hWnd: El cuadro de dialogo con las curvas de tiling U.
		// hView: La ventana view del anterior cuadro de dialogo.
		void SaveTLDlg();

		TSTR BrowseForFileName(BOOL save, BOOL &cancel, int fo);
		void SaveTL( BitArray &sel_groups );
		void LoadTL(int fo);

		void TimeChanged(TimeValue t);

		BOOL GMSetNumPoints(HWND hWnd, BOOL hold = TRUE);

		void AlignToSelection();

		// UV PELT
		void DoUVPeltEditing();
		void DestroyUVPeltDlg();
		void ResetUVPelt();
		void PeltSaveLayout();
		void PeltLoadLayout();

		// UVW Frame
		BOOL AddFrame(INode * node);

		void GroupsMenuDrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
		void GroupsMenuForceDrawItem(int index);
		void GroupsMenuUpdateList();
		void RepositionGroup(int from, int to);

		void DrawMappingTypeItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

		int GetUniqueNameID();

		void GetSaveUVWFilename();

		void SaveUVW();
		void LoadUVW();

		void SaveGroupUVW(int group);
		void LoadGroupUVW(int group);

		void CleanTempUVWData();

		BOOL GetMeshDataInAppData( int num_verts, int num_faces );
		int * GetFacesInAppData();
		float * GetVertsInAppData();
	};

class FaceAlignMouseProc : public MouseCallBack {
	public:
		MultiMapMod *mod;
		IObjParam *ip;
		FaceAlignMouseProc(MultiMapMod *m,IObjParam *i) {mod=m;ip=i;}
		int proc(HWND hWnd, int msg, int point, int flags, IPoint2 m);
		void FaceAlignMap(HWND hWnd,IPoint2 m);
	};

class FaceAlignMode : public CommandMode {
	public:
		ChangeFGObject fgProc;
		FaceAlignMouseProc proc;
		IObjParam *ip;
		MultiMapMod *mod;

		FaceAlignMode(MultiMapMod *m,IObjParam *i)
			: fgProc(m), proc(m,i) {ip=i;mod=m;}

		int Class() {return MOVE_COMMAND;}
		int ID() {return CID_FACEALIGNMAP;}
		MouseCallBack *MouseProc(int *numPoints) {*numPoints=2;return &proc;}
		ChangeForegroundCallback *ChangeFGProc() {return &fgProc;}
		BOOL ChangeFG(CommandMode *oldMode) {return oldMode->ChangeFGProc() != &fgProc;}
		void EnterMode();
		void ExitMode();
	};

class RegionFitMouseProc : public MouseCallBack {
	public:
		MultiMapMod *mod;
		IObjParam *ip;
		IPoint2 om;
		RegionFitMouseProc(MultiMapMod *m,IObjParam *i) {mod=m;ip=i;}
		int proc(HWND hWnd, int msg, int point, int flags, IPoint2 m);
		void RegionFitMap(HWND hWnd,IPoint2 m);
	};

class PickAcquire : 
			public PickModeCallback,
			public PickNodeCallback {
	public:		
		MultiMapMod *mod;
		IObjParam *ip;

		PickAcquire() {mod = NULL; ip = NULL;}

		BOOL HitTest(IObjParam *ip,HWND hWnd,ViewExp *vpt,IPoint2 m,int flags);		
		BOOL Pick(IObjParam *ip,ViewExp *vpt);		
		void EnterMode(IObjParam *ip);
		void ExitMode(IObjParam *ip);		
		BOOL RightClick(IObjParam *ip,ViewExp *vpt)	{return TRUE;}

		BOOL Filter(INode *node);

		MultiMapMod *FindFirstMap(ReferenceTarget *ref);
		void AcquireMapping(
			MultiMapMod *toMod, ModContext *toMC, INode *toNode,
			MultiMapMod *fromMod, int channel, ModContext *fromMC, INode *fromNode,
			int type, int ani);
	};

class RegionFitMode : public CommandMode {
	public:
		ChangeFGObject fgProc;
		RegionFitMouseProc proc;
		IObjParam *ip;
		MultiMapMod *mod;

		RegionFitMode(MultiMapMod *m,IObjParam *i) 
			: fgProc(m), proc(m,i) {ip=i;mod=m;}

		int Class() {return MOVE_COMMAND;}		
		int ID() {return CID_REGIONFIT;}
		MouseCallBack *MouseProc(int *numPoints) {*numPoints=2;return &proc;}
		ChangeForegroundCallback *ChangeFGProc() {return &fgProc;}
		BOOL ChangeFG(CommandMode *oldMode) {return oldMode->ChangeFGProc() != &fgProc;}
		void EnterMode();
		void ExitMode();
	};

#endif //__MAPPING__
