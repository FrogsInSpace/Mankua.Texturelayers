/**********************************************************************
 *<
	FILE: uv_pelt_dlg.h

	DESCRIPTION:  Texture Layers that kick the native

	CREATED BY: Diego

	HISTORY: 7/01/98

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#ifndef __UV_PELT_DLG_H__
#define __UV_PELT_DLG_H__

#include "iparamm2.h"
#include "istdplug.h"
#include "meshadj.h"
#include "modstack.h"
#include "macrorec.h"
#include "mapping.h"

class PeltSelectMode : public MouseCallBack {
	public:
		UVPeltDlg *dlg;
		BOOL region, toggle, subtract;
		IPoint2 om, lm;
		PeltSelectMode(UVPeltDlg *d) {dlg=d;}
		int proc(HWND hWnd, int msg, int point, int flags, IPoint2 m);
		virtual int subproc(HWND hWnd, int msg, int point, int flags, IPoint2 m)=0;
		virtual HCURSOR GetXFormCur()=0;
	};

class PeltMoveMode : public PeltSelectMode {
	public:				
		UVPeltDlg *dlg;
		PeltMoveMode(UVPeltDlg *d) : PeltSelectMode(d) {dlg = d;}
		int subproc(HWND hWnd, int msg, int point, int flags, IPoint2 m);
		HCURSOR GetXFormCur();
	};

class PeltRotateMode : public PeltSelectMode {
	public:				
		UVPeltDlg *dlg;
		PeltRotateMode(UVPeltDlg *d) : PeltSelectMode(d) {dlg = d;}
		int subproc(HWND hWnd, int msg, int point, int flags, IPoint2 m);
		HCURSOR GetXFormCur();
	};

class PeltScaleMode : public PeltSelectMode {
	public:				
		UVPeltDlg *dlg;
		PeltScaleMode(UVPeltDlg *d) : PeltSelectMode(d) {dlg = d;}
		int subproc(HWND hWnd, int msg, int point, int flags, IPoint2 m);
		HCURSOR GetXFormCur();
	};

class PeltAddPointMode : public MouseCallBack {
	public:
		UVPeltDlg *dlg;
		PeltAddPointMode(UVPeltDlg *d) {dlg=d;}
		int proc(HWND hWnd, int msg, int point, int flags, IPoint2 m);
		HCURSOR GetXFormCur();
	};

class PeltDelPointMode : public MouseCallBack {
	public:
		UVPeltDlg *dlg;
		PeltDelPointMode(UVPeltDlg *d) {dlg=d;}
		int proc(HWND hWnd, int msg, int point, int flags, IPoint2 m);
		HCURSOR GetXFormCur();
	};

class PeltPanMode : public MouseCallBack {
	public:
		UVPeltDlg *dlg;
		IPoint2 om;
		int oxscroll, oyscroll;
		PeltPanMode(UVPeltDlg *d) {dlg=d;}
		int proc(HWND hWnd, int msg, int point, int flags, IPoint2 m);
		HCURSOR GetXFormCur();
	};

class PeltZoomMode : public MouseCallBack {
	public:
		UVPeltDlg *dlg;
		Point2 fixed_point;
		IPoint2 om;
		float ozoom;
		float oxscroll, oyscroll;
		PeltZoomMode(UVPeltDlg *d) {dlg=d;}
		int proc(HWND hWnd, int msg, int point, int flags, IPoint2 m);
		HCURSOR GetXFormCur();
	};

class PeltMiddleMode : public MouseCallBack {
	public:
		UVPeltDlg *dlg;
		Point2 fixed_point;
		IPoint2 om;
		float ozoom;
		float oxscroll, oyscroll;
		PeltMiddleMode(UVPeltDlg *d) {dlg=d;}
		int proc(HWND hWnd, int msg, int point, int flags, IPoint2 m);
	};

class PeltRightMode : public MouseCallBack {
	public:
		UVPeltDlg *dlg;
		PeltRightMode(UVPeltDlg *d) {dlg=d;}
		int proc(HWND hWnd, int msg, int point, int flags, IPoint2 m);
	};


class UVPeltDlg {
	public:
		MultiMapMod *mod;

		static HCURSOR selCur;
		static HCURSOR moveCur;
		static HCURSOR moveXCur;
		static HCURSOR moveYCur;
		static HCURSOR rotateCur;
		static HCURSOR scaleCur;
		static HCURSOR scaleXCur;
		static HCURSOR scaleYCur;
		static HCURSOR addPtCur;
		static HCURSOR delPtCur;
		static HCURSOR zoomCur;
		static HCURSOR panCur;

		static HWND hWnd;
		static HWND hView;

		static int move_axis;
		static int scale_axis;
		static IOffScreenBuf *iBuf;
		static ICustToolbar *iToolBar;
		static ICustButton *iMove;
		static ICustButton *iRotate;
		static ICustButton *iScale;
		static ICustButton *iAddPoint, *iDelPoint;		
		static ICustButton *iPan, *iZoom, *iZoomExt;
		static ICustButton *iSymmetry;
		static ICustButton *iSymmPair;
		static ICustButton *iRotP90, *iRotN90, *iRot180;

		static int mode;
		static MouseManager mouseMan;
		static PeltMoveMode *moveMode;
		static PeltRotateMode *rotateMode;
		static PeltScaleMode *scaleMode;
		static PeltAddPointMode *addPointMode;
		static PeltDelPointMode *delPointMode;
		static PeltPanMode *panMode;
		static PeltZoomMode *zoomMode;
		static PeltMiddleMode *middleMode;
		static PeltRightMode *rightMode;

		static BOOL viewValid;
	
		int view_h, view_w;
		BOOL selecting;
		IPoint2 lm,om;

		UVPeltDlg( MultiMapMod *m );

		void InvalidateView();
		void StartUVPeltDlg();
		void EndUVPeltDlg();
		void SetupDlg( HWND hWnd );
		void RegisterClasses();
		void SetMode(int m);

		void PaintView();
		void SizeDlg();

		IPoint2 PeltToView( Point2 pt_p );
		Point2 ViewToPelt( IPoint2 pt_v );
		Box2D ViewBoxToPeltBox( Rect rect );

		void ZoomExtents();
	};

#endif