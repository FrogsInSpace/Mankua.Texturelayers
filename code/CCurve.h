#ifndef CCURVE_H
#define CCURVE_H

#include "max.h"

#ifndef MAX_RELEASE_R9
#include "max_mem.h"
#endif

#include "mods.h"	
//#include "iparamm.h"
#include "simpobj.h"

#define GRID_WID	35
#define GRID_HEI	15

class CurveDlg;

#define CCPOINT_BEZCORN		0
#define CCPOINT_BEZIER		1
#define CCPOINT_CORNER		2

static HCURSOR selCur   = NULL;
static HCURSOR moveCur  = NULL;
static HCURSOR addCur   = NULL;

class CCurvePoint
{
public:
	Point2 p;
	Point2 in;
	Point2 out;
	float du;
	int kind;


	CCurvePoint() {
		out = in = p = Point2(0.0f,0.0f);
		kind = CCPOINT_CORNER;
		}
	CCurvePoint(Point2 pto) {
		out = in = p = pto;
		kind = CCPOINT_CORNER;
		}
	};

class square
{
public:
	Point2 center;
	float t,b,l,r;
	
	square(Point2 c) {
		center = c;
		t = b = l = r = 0.0f;
		}
	void InsPt(Point2 p) {
		if (center.y + t < p.y) t = p.y - center.y;
		if (center.y - b > p.y) b = center.y - p.y;
		if (center.x + r < p.x) r = p.x - center.x;
		if (center.x - l > p.x) l = center.x - p.x;
		}
};


#define MAX_FLAG	(1<<0)
#define MIN_FLAG	(1<<1)

class ControlCurve {
	public:
		int flags;

		float max;
		float min;		// MAX and MIN values in the curve..

		Tab <CCurvePoint> points;
		BitArray selPoints;

		ControlCurve() {;}

		int NumPoints()					{	return points.Count();}

		Point2 GetPoint(int n)			{	return points[n].p;}
		Point2 GetIn(int n)				{	return points[n].in;}
		Point2 GetOut(int n)			{	return points[n].out;}
		int GetKind(int n)				{	return points[n].kind;}
		float GetDU(int n)				{	return points[n].du;}

		void SetPoint(int n, Point2 pt) {	points[n].p = pt;}
		void SetIn(int n,Point2 pt)		{	points[n].in = pt;}
		void SetOut(int n,Point2 pt)	{	points[n].out = pt;}
		void SetKind(int n,int k)		{	points[n].kind = k; }

		BOOL IsSel(int n)				{	return selPoints[n];}
		int NumSel()					{	return selPoints.NumberSet();}
		void SetSel(int n,BOOL state)	{	selPoints.Set(n,state);}
		void ClearSelect()				{	selPoints.ClearAll();}

		void SetAFlag(int mask) { flags|=mask; }
		void ClearAFlag(int mask) { flags &= ~mask; }
		int TestAFlag(int mask) { return(flags&mask?1:0); }

		void SetNumPoints(int n);

		void Select(Tab<int> &hits,BOOL toggle,BOOL subtract,BOOL all);
		void DelPoints();
		void AddPoint(Point2 pto);

		void MovePoints(Point2 pt,BOOL in, BOOL out, int inout);
		void MoveIn(Point2 pt,int inout);
		void MoveOut(Point2 pt,int inout);

		void SetFullXPoint(float x);
		void SetFullYPoint(float y);

		void SetLine();
		void SetCorner();
		void SetSmooth();

		void BuildDU();
		void BuildSmooth(int i);

		Point2 Ibp(int i,float u);
		float GetValue(float u);

		void Reset();
		void Reset(float val, float mx, float mn, int flag);
	};


class SelectMode : public MouseCallBack {
	public:
		CurveDlg *dlg;
		BOOL region, toggle, subtract;
		IPoint2 om, lm;
		SelectMode(CurveDlg *d) {dlg=d;}
		int proc(HWND hWnd, int msg, int point, int flags, IPoint2 m);
		virtual int subproc(HWND hWnd, int msg, int point, int flags, IPoint2 m)=0;
		virtual HCURSOR GetXFormCur()=0;
	};

class MoveMode : public SelectMode {
	public:				
		MoveMode(CurveDlg *d) : SelectMode(d) {}
		int subproc(HWND hWnd, int msg, int point, int flags, IPoint2 m);
		HCURSOR GetXFormCur() {return moveCur;}
	};

class AddMode : public MouseCallBack {
	public:
		CurveDlg *dlg;
		AddMode(CurveDlg *d) {dlg=d;}
		int proc(HWND hWnd, int msg, int point, int flags, IPoint2 m);
		virtual HCURSOR GetXFormCur() {return addCur;}
	};

class RightMouseMode : public MouseCallBack {
	public:
		CurveDlg *dlg;		
		RightMouseMode(CurveDlg *d) {dlg=d;}
		int proc(HWND hWnd, int msg, int point, int flags, IPoint2 m);
	};

class PanMode : public MouseCallBack {
	public:
		CurveDlg *dlg;
		IPoint2 om;
		float oxscroll, oyscroll;
		PanMode(CurveDlg *d) {dlg=d;}
		int proc(HWND hWnd, int msg, int point, int flags, IPoint2 m);
	};

#define ZOOM_XY	0
#define ZOOM_X	1
#define ZOOM_Y	2
class ZoomMode : public MouseCallBack {
	public:
		CurveDlg *dlg;
		int zoomtype;
		IPoint2 om;
		float oxzoom,oyzoom;
		float oxscroll, oyscroll;
		ZoomMode(CurveDlg *d) {dlg=d;zoomtype=ZOOM_XY;}
		int proc(HWND hWnd, int msg, int point, int flags, IPoint2 m);
	};


class CurveDlg : public ReferenceMaker, public TimeChangeCallback {
	public:
		float xzoom, yzoom, xscroll, yscroll;

		// Move only Point vector handlers???
		BOOL in;
		BOOL out;
		int inout;

		BOOL typeInsValid;

		TSTR title;

		ControlCurve *cc;
		ReferenceTarget *mmod;
		IObjParam *ip;

		HWND hMain, hView;

		IOffScreenBuf *iBuf;

		ICustToolbar *iTool;
		ICustButton *iMove;
		ICustButton *iAdd;
		ICustButton *iDel;
		ICustButton *iReset;
		ICustButton *iBez;
		ICustButton *iCorn;
		ICustButton *iLine;
		ICustButton *iPan;
		ICustButton *iZe;
		ICustButton *iZhe;
		ICustButton *iZve;
		ICustButton *iZoom;
		ICustButton *iZoomX;
		ICustButton *iZoomY;
		ISpinnerControl *iXPos;
		ISpinnerControl *iYPos;

		int mode;
		int move;

		Point2 punto;

		CurveDlg(ReferenceTarget *mod, ControlCurve *c, IObjParam *iop,HWND hParent, TSTR tit);

		BOOL IsActive();

		void SizeDlg();
		void SetupDlg(HWND hWnd);
		void RegisterClasses();
		void InvalidateView();
		void ComputeZooms(HWND hwnd,int &width, int &height);
		void PaintView();
		void DestroyDlg();
		void MaybeCloseWindow();
		void SetupTypeins();
		void InvalidateTypeins();

		Point2 PointToScreen(Point2 pt,int w,int h);
		Point2 ScreenToPoint(IPoint2 m,int w,int h);
		BOOL HitTest(Rect rect,Tab<int> &hits,BOOL selOnly, BOOL &in, BOOL &out, int &inout);

		void Select(Tab<int> &hits,BOOL toggle,BOOL subtract,BOOL all);
		void ClearSelect();
		void MovePoints(Point2 pt);

		void ZoomExtents();
		void ZoomExtentsH();
		void ZoomExtentsV();

		MouseManager mouseMan;
		void SetMode(int m);

		MoveMode *moveGMode;
		AddMode *addGMode;
		ZoomMode *zoomGMode;
		PanMode *panGMode;
		RightMouseMode *rightMode;

		void SetXPosition(float nxp);
		void SetYPosition(float nyp);

		void SetZoomType(int zt);

		void TimeChanged(TimeValue t) {InvalidateView();}

// JW Code Change: NotifyRefChanged signature changed in 3ds Max 2015+
#if MAX_VERSION_MAJOR < 17
		RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message);
#else
		RefResult NotifyRefChanged(const Interval& changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message, BOOL propagate);
#endif
		int NumRefs() {return 1;}
		RefTargetHandle GetReference(int i) {return mmod;}
		void SetReference(int i, RefTargetHandle rtarg) {mmod=(ReferenceTarget*)rtarg;}

	};

class CurveRestore : public RestoreObj {
	public:
		CurveDlg *cd;
		ControlCurve uCurve;
		ControlCurve rCurve;

		CurveRestore(CurveDlg *c) {
			cd=c;
			uCurve = *cd->cc;
			}

		void Restore(int isUndo) {
			if (isUndo) {
				rCurve = *cd->cc;
				}
			*cd->cc = uCurve;
			cd->InvalidateView();
			}

		void Redo() {
			*cd->cc = rCurve;
			cd->InvalidateView();
			}

		TSTR Description() { return TSTR(_T("Modify Curve")); }
	};



#endif //  CCURVE_H
