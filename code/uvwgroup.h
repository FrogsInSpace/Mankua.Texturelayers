#ifndef __UVW_GROUP_H__
#define __UVW_GROUP_H__

#include "iparamm2.h"
#include "istdplug.h"
#include "meshadj.h"
#include "modstack.h"
#include "macrorec.h"
#include "spline3d.h"
#include "shape.h"
#include "CCurve.h"
#include "tl_classes.h"
#include "texlay_mc_data.h"

class GridMesh : public ReferenceTarget {
	public:
		int nx,ny;  // Numero de columnas y filas de puntos

		Tab <Control*> vtCtrl;
		Tab <Point3> verts;
		Tab <Point3> vnormals;
		Tab <Point3> uvw;
		BitArray sel;

		Tab <IPoint3> faces;
		Tab <Point3> fnormals;
		Tab <Point3> cosangs;
		Tab <float>  coefD;
		Tab <float>  den;

		GridMesh(int i,BOOL loading = FALSE);
		~GridMesh() {
			DeleteAllRefsFromMe();
			verts.SetCount(0);
			vnormals.SetCount(0);
			uvw.SetCount(0);
			sel.SetSize(0);
			faces.SetCount(0);
			fnormals.SetCount(0);
			cosangs.SetCount(0);
			coefD.SetCount(0);
			den.SetCount(0);
			}

		Point3 GetVert(int x, int y);
		Point3 GetVert(int i);
		Control *GetVtCtrl(int i);

		void CacheVNormals();

		void Update(TimeValue t);

		void PlugControllers(TimeValue t);

		Point3 GetPosition(TimeValue t, float u, float v, float w);

		Point3 GetFNormal(int x, int y, int i);

		void SetNumVerts(int nv);
		void SetNumFaces(int nf);
		void ExpandSelection(int ind, BOOL on, BOOL allx, BOOL ally);

		// Reference Target Methods
		Class_ID ClassID();
		SClass_ID SuperClassID();

		int NumRefs() { return vtCtrl.Count();}
        RefTargetHandle GetReference(int i) { return vtCtrl[i];}
        void SetReference(int i, RefTargetHandle rtarg) {vtCtrl[i] = (Control*)rtarg;}

// JW Code Change: NotifyRefChanged signature changed in 3ds Max 2015+
#if MAX_VERSION_MAJOR < 17
		RefResult NotifyRefChanged(Interval, RefTargetHandle, PartID&, RefMessage) { return REF_SUCCEED; }
#else
		RefResult NotifyRefChanged(const Interval&, RefTargetHandle, PartID&, RefMessage, BOOL) { return REF_SUCCEED; }
#endif

		Interval LocalValidity(TimeValue t);
        int NumSubs()  { return NumRefs();}
        Animatable* SubAnim(int i) {return GetReference(i);}

		TSTR SubAnimName(int i , bool localized=false )
		{
			TSTR str;
			str.printf(GetString(IDS_DC_POINT),i+1);
			return str;
		}

        int SubNumToRefNum(int subNum) {return subNum;}

		void DeleteThis();
		IOResult Load(ILoad *iload);
		IOResult Save(ISave *isave);

#ifndef MAX_RELEASE_R9
		RefTargetHandle Clone(RemapDir& remap = NoRemap());
#else
		RefTargetHandle Clone(RemapDir& remap = DefaultRemapDir());
#endif
};

class TLBezierSpline {
	public:
		BOOL closed;
		int num_points;
		
		Point3 * in_vec;
		Point3 * knot;
		Point3 * out_vec;

		TLBezierSpline();
		TLBezierSpline( int np );
		~TLBezierSpline();
		void SetNumKnots( int np );
		void Clean();
		int NumSegments();
		Point3 GetInVec(int i);
		Point3 GetKnot(int i);
		Point3 GetOutVec(int i);
	};

class MultiMapMod;
class UVWProyector : public ReferenceTarget {
	public:	

		BOOL skip_bug;

		TLBezierSpline bezier_spline;

		int id;

		BOOL force_apply;
		BOOL applying_mapping;

		// If a group is valid, all the data is stored in v & f, and these are
		// returned in the ApplyUVW...
		// If the group is invalid, v & f are filled by ApplyUVW and then returned
		// to the modifier
		// This is also used by MAP_TL_UVWDATA groups
		int valid_group;

		BOOL use_old_ffm;

		TSTR descCanal;

		// Temp Variables (not in pblock for faster access during rendering
		int		mapping_type;
		int		map_channel;
		BOOL	active_status;
		float	gizmo_height;
		float	gizmo_length;
		float	gizmo_width;
		float	tile_u;
		float	tile_v;
		float	tile_w;
		float	offset_u;
		float	offset_v;
		float	offset_w;
		int		attOn;
		float	attGl;
		float	attUs;
		float	attUo;
		float	attVs;
		float	attVo;
		float	attWs;
		float	attWo;
		float	attRad;
		float	move_u;
		float	move_v;
		float	move_w;
		float	rotate_u;
		float	rotate_v;
		float	rotate_w;
		float	scale_u;
		float	scale_v;
		float	scale_w;
		int		axis;
		float	fs;
		int		norm;
		int		reverse;
		int		nType;				// 0 : plane Normalization | 1 : Center Normalization | 2 : Field Normalization
		BOOL	belt;
		float	ffm_threshold;
		float	pelt_size;
		float	pelt_rigidity;
		float	pelt_stability;
		int		pelt_iterations;
		BOOL	pelt_border;
		BOOL	xform_att;
		float	normals_start;
		INode	*spline_node;
		INode	*frame_node;

		MultiMapMod * tl;

		DWORD flags;

		IParamBlock2 *pblock;

		Control* tmControl;

		Interval ivalid;

		float aspect;

		Matrix3 sTmc;

		int rebuildBezier;

		GridMesh *gm;

		INode *spline;
		Interval bezInterval;
		Tab <float> segLens;
		Tab <float> segPers;
		Tab <Vector> vectors;

		Tab <Point2> tileG;
		BitArray tGsel;

		// UV PELT DATA

		int xscroll, yscroll;
		float zoom;

		int current_num_faces;
		int current_num_edges;
		int current_num_verts;
		int current_num_sel_faces;
		int current_num_sel_edges;
		Matrix3 natural_axis;
		Matrix3 inv_natural_axis;
		PeltPolyMesh * pelt_poly_mesh;

		Tab <int> vert_to_mass;
		Tab <Mass*> masses;

		Tab <int> border_verts;
		Tab <FrameSegments*> frame_segments;
		Tab <IPoint2> frame_segments_symmetry;
		int symmetry_axis;
		Point3 symmetry_center;

		// Control Curves...
		ControlCurve tileCrv;
		ControlCurve wTileCrv;
		ControlCurve angleCrv;

		float splLen;

		Point3 normal;
		Point3 center;

		UVWProyector(BOOL loading=FALSE);
		~UVWProyector();

		void LocalDataChanged();

		int   GetMappingChannel();
		float GetLength(TimeValue t);
		float GetWidth(TimeValue t);
		float GetHeight(TimeValue t);
		float GetTileU(TimeValue t);
		float GetTileV(TimeValue t);
		float GetTileW(TimeValue t);
		float GetOffsetU(TimeValue t);
		float GetOffsetV(TimeValue t);
		float GetOffsetW(TimeValue t);
		int   GetAttStatus(TimeValue t);
		float GetAttGlobal(TimeValue t);
		float GetAttUStart(TimeValue t);
		float GetAttVStart(TimeValue t);
		float GetAttWStart(TimeValue t);
		float GetAttUOffset(TimeValue t);
		float GetAttVOffset(TimeValue t);
		float GetAttWOffset(TimeValue t);
		float GetAttRad(TimeValue t);
		float GetMoveU(TimeValue t);
		float GetMoveV(TimeValue t);
		float GetMoveW(TimeValue t);
		float GetRotateU(TimeValue t);
		float GetRotateV(TimeValue t);
		float GetRotateW(TimeValue t);
		float GetScaleU(TimeValue t);
		float GetScaleV(TimeValue t);
		float GetScaleW(TimeValue t);
		int   GetAxis();
		int   GetNormalize(TimeValue t);
		int   GetReverse(TimeValue t);
		int   GetNormalType(TimeValue t);
		float GetFFMThresh(TimeValue t);
		float GetFrameSize(TimeValue t);	//
		float GetPeltRigidity(TimeValue t);
		float GetPeltStability(TimeValue t);
		int   GetPeltIter();
		float GetSplineNormalsStart();
		int   GetMappingType();
		BOOL  GetActiveStatus();
		INode*GetSplineNode();
		INode* GetFrameNode();

		void SetMappingChannel(int map_channel);
		void SetLength(TimeValue t,float v);
		void SetWidth(TimeValue t,float v);
		void SetHeight(TimeValue t,float v);
		void SetTileU(float tu,TimeValue t);
		void SetTileV(float tv,TimeValue t);
		void SetTileW(float tw,TimeValue t);
		void SetOffsetU(float ou,TimeValue t);
		void SetOffsetV(float ov,TimeValue t);
		void SetOffsetW(float ow,TimeValue t);
		void SetAttStatus(int ats,TimeValue t);
		void SetAttGlobal(float att,TimeValue t);
		void SetAttUStart(float aus,TimeValue t);
		void SetAttVStart(float avs,TimeValue t);
		void SetAttWStart(float aws,TimeValue t);
		void SetAttUOffset(float auo,TimeValue t);
		void SetAttVOffset(float auo,TimeValue t);
		void SetAttWOffset(float auo,TimeValue t);
		void SetAttRad(float ard,TimeValue t);
		void SetNormalize(int nz,TimeValue t);
		void SetFFMThresh(float th,TimeValue t);
		void SetMoveU(float f, TimeValue t);
		void SetMoveV(float f, TimeValue t);
		void SetMoveW(float f, TimeValue t);
		void SetRotateU(float f, TimeValue t);
		void SetRotateV(float f, TimeValue t);
		void SetRotateW(float f, TimeValue t);
		void SetScaleU(float f, TimeValue t);
		void SetScaleV(float f, TimeValue t);
		void SetScaleW(float f, TimeValue t);
		void SetAxis(int ax,TimeValue t);
		void SetReverse(int rv,TimeValue t);
		void SetNormalType(int nt,TimeValue t);
		void SetFrameSize(float f, TimeValue t);
		void SetPeltRigidity(float f, TimeValue t);
		void SetPeltStability(float f, TimeValue t);
		void SetPeltIter(int i);
		void SetSplineNormalsStart(float ns, TimeValue t);
		void SetMappingType(int type);
		void SetActiveStatus(BOOL status);
		void SetSplineNode(INode * node, TimeValue t=0);

		void Update(TimeValue t);

		void CacheBezier(TimeValue t);
		void RebuildFieldNormals(TimeValue t);

		void SetGMPolys(int npx, int npy);
		void CacheFFGrid(TimeValue t);
		void InitFFGrid(int npx, int npy, Point3 size);
		void BuildGMGrid();
		void GMSetNumPoints(int npx, int npy, int shape, Point3 size, int resetTO, TimeValue t);

		// Mapping Methods..
		Point3 NearestSplinePoint(Point3 punto, TimeValue t);
		Point3 NearestGridPoint(Point3 punto, TimeValue t);
		Point3 NearestGridPointOld(Point3 punto, TimeValue t);

		Point3 MapPoint(Point3 punto, Matrix3 tm, TimeValue t);
		Point3 MapPoint(Point3 punto, Point3 norm, Matrix3 tm, BOOL &nan);

		void MapFace(Mesh *mesh, int face, Matrix3 sTo, Tab<Point3> &newUVs, Tab<Point3> &mapVerts, DWORDTab &refCts);
		void MapFace(Mesh *mesh, int face, Matrix3 sTo, Tab<Point3> &newUVs, DWORDTab &refCts, BOOL facesel);
		BOOL ApplyUVW(Mesh *mesh, PolyUVWData * uvw_data, Matrix3 sTo, BitArray fs, TimeValue t);
		void DefaultUVW(Mesh *mesh, PolyUVWData * uvw_data);

		void MapPatch(PatchMesh *mesh, int patch, Matrix3 mapTM, Tab<Point3> &newUVs, Tab<Point3> &mapVerts, DWORDTab &refCts);
		void MapPatch(PatchMesh *mesh, int patch, Matrix3 mapTM, Tab<Point3> &newUVs, DWORDTab &refCts, BOOL facesel);
		BOOL ApplyUVW(PatchMesh *mesh, PolyUVWData * uvw_data, Matrix3 sTo, BitArray fs, TimeValue t);
		void DefaultUVW(PatchMesh *mesh, PolyUVWData * uvw_data);
		
		void MapPolyFace(MNMap *mapmesh, int face, Matrix3 mapTM, Tab<Point3> &newUVs, Tab<Point3> &mapVerts, Tab<int> &refCts);
		BOOL ApplyUVW(MNMesh *mnmesh, PolyUVWData * uvw_data, Matrix3 mapTM, BitArray fs, TimeValue t);
		void DefaultUVW(MNMesh *mnmesh, PolyUVWData * uvw_data);

		// UV Pelt Info
		void PeltBuildUpUVInfo( TimeValue t, MNMesh &mnMesh, BitArray fs, BitArray es );
		void PeltApplyUV( TimeValue t, MNMesh * aux_mesh, PolyUVWData * uvw_data ); 
		BOOL PeltHitTest(Box2D box, Tab<int> &hits, BOOL selOnly);
		void PeltHoldPoints();
		void PeltSelect(Tab<int> &hits, BOOL toggle, BOOL subtract, BOOL all);
		void PeltClearSelect();
		void PeltMovePoints( Point2 delta, int axis );
		void PeltRotatePoints( float ang );
		void PeltScalePoints( float scale, int axis );
		BOOL PeltHitSubPoint(Box2D box, int &fs, int &bv);
		void PeltAddPoint(int fs, int bv);
		void PeltDelSinglePoint(int fs, BOOL hold=TRUE);
		void PeltDelPoints(int point=-1);
		void PeltDeleteUV();
		void PeltDeleteFrameSegments();
		TSTR PeltBrowseForFileName(BOOL save, BOOL &cancel);
		void PeltSaveFrame(BitArray &face_sel, BitArray &edge_sel);
		void PeltLoadFrame(FILE *tf);
		void PeltBuildUpSymmetryTables(int use_symmetry, BOOL create_axis=FALSE);
		void PeltFixSymmetricSegments();
		void PeltStepRotatePoints(float ang);
		void PeltMakeSymmetryPair();

		// Inverse Mapping methods (Gizmo)
		Point3 GetUVWSplinePoint(TimeValue t, float u, float v, float w, Point3 &ns, Point3 &ne);
		Point3 GetPosition(TimeValue t, float u, float v, float w,int gizmo = 0);
		int GetSplNumSegs();

		float GetAtt(Point3 p);

		// Various Simplified Bezier Interpolation Methods...
		Point3 Isp(int spl, int seg, float k, Point3 &tan, TimeValue t);
		Point3 Isp(int spl, int seg, float k, Point3 &tan, Point3 &norm, TimeValue t);
		Point3 Isp(int spl, int seg, float k, TimeValue t);
		Point3 Isp(float k, Point3 &tan, TimeValue t);
		Point3 Isp(float k, TimeValue t);

		void CopyUVW(UVWProyector * uvProy);

		Matrix3 CompMatrix(TimeValue t,ModContext *mc, Matrix3 *ntm,BOOL applySize=TRUE, BOOL applyAxis=TRUE);
		Matrix3 CGMatrix(TimeValue t);
		void InitControl(ModContext &mc,Object *obj,int type,TimeValue t);

		// Reference Target Methods
		Class_ID ClassID();
		SClass_ID SuperClassID();

		int NumRefs() { return 4;}
        RefTargetHandle GetReference(int i);
        void SetReference(int i, RefTargetHandle rtarg);

// JW Code Change: NotifyRefChanged signature changed in 3ds Max 2015+
#if MAX_VERSION_MAJOR < 17
		RefResult NotifyRefChanged(Interval, RefTargetHandle, PartID&, RefMessage);
#else
		RefResult NotifyRefChanged( const Interval&, RefTargetHandle, PartID&, RefMessage, BOOL);
#endif

		Interval LocalValidity(TimeValue t);
		
        int NumSubs();
        Animatable* SubAnim(int i);
#if MAX_VERSION_MAJOR < 24
		TSTR SubAnimName(int i);
#else
		TSTR SubAnimName(int i, bool localized = false);
#endif

		int	NumParamBlocks() { return 1; }	
		IParamBlock2* GetParamBlock(int i) { return pblock; }
		IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock->ID() == id) ? pblock : NULL; }
		int GetParamBlockIndex(int id) {return id;}

        int SubNumToRefNum(int subNum);

		void DeleteThis() { delete this;}
		IOResult Load(ILoad *iload);
		IOResult Save(ISave *isave);

#ifndef MAX_RELEASE_R9
		RefTargetHandle Clone(RemapDir& remap = NoRemap());
#else
		RefTargetHandle Clone(RemapDir& remap = DefaultRemapDir());
#endif
	};

#endif
