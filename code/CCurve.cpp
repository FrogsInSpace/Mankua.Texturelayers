#include "CCurve.h"
#include "modsres.h"
#include "mapping.h"
#include "uvwgroup.h"
#include "texlay_mc_data.h"
#include "3dsmaxport.h"

#define ID_TOOL_MOVE	0x0100
#define ID_TOOL_ADD		0x0110
#define ID_TOOL_DEL		0x0120
#define ID_TOOL_RESET	0x0130
#define ID_TOOL_BEZ		0x0140
#define ID_TOOL_CORN	0x0150
#define ID_TOOL_LINE	0x0160
#define ID_TOOL_PAN		0x0170
#define ID_TOOL_ZE		0x0180
#define ID_TOOL_ZHE		0x0190
#define ID_TOOL_ZVE		0x0200
#define ID_TOOL_ZOOM	0x0210
#define ID_TOOL_ZOOMX	0x0220
#define ID_TOOL_ZOOMY	0x0230

#define TOOL_HEIGHT		30

class CCurveEvent : public EventUser {
public:
	CurveDlg *dlg;

	void Notify() {
		if (dlg) {
			theHold.Put( new CurveRestore(dlg) );
			dlg->cc->DelPoints();
			theHold.Accept(_T("Delete Point"));
			dlg->InvalidateView();
			dlg->ip->RedrawViews(dlg->ip->GetTime());
			}
		}
	void SetDlg(CurveDlg *d) {
		dlg=d;
		}
	};
CCurveEvent keyboardEvent;

void ControlCurve::SetNumPoints(int n) { 
	points.SetCount(n);
	selPoints.SetSize(n);
	selPoints.ClearAll();
	}

void ControlCurve::Reset(float val, float mx, float mn, int flag) {
	SetNumPoints(2);

	flags = 0;
	SetAFlag(flag);
	max = mx;
	min = mn;

	SetPoint(0,Point2(0.0f,val));
	SetIn(0,Point2(0.0f,val));
	SetOut(0,Point2(0.0f,val));
	SetKind(0,CCPOINT_CORNER);
	SetPoint(1,Point2(1.0f,val));
	SetIn(1,Point2(1.0f,val));
	SetOut(1,Point2(1.0f,val));
	SetKind(1,CCPOINT_CORNER);
	}

void ControlCurve::Reset() {
	SetNumPoints(2);

	SetPoint(0,Point2(0.0f,1.0f));
	SetIn(0,Point2(0.0f,1.0f));
	SetOut(0,Point2(0.0f,1.0f));
	SetKind(0,CCPOINT_CORNER);
	SetPoint(1,Point2(1.0f,1.0f));
	SetIn(1,Point2(1.0f,1.0f));
	SetOut(1,Point2(1.0f,1.0f));
	SetKind(1,CCPOINT_CORNER);
	}


void ControlCurve::AddPoint(Point2 pto) {
	selPoints.ClearAll();
	for (int i=1; i<NumPoints(); i++) {
		if (GetPoint(i-1).x < pto.x && GetPoint(i).x > pto.x) {
			if(TestAFlag(MIN_FLAG) && pto.y < min) pto.y = min;
			if(TestAFlag(MAX_FLAG) && pto.y > max) pto.y = max;
			CCurvePoint nPto(pto);
			points.Insert(i,1,&nPto);
			// Fix Vector Handlers of i-1 out vec
			if (GetOut(i-1).x > GetPoint(i).x) {
				Point2 vec = GetOut(i-1) - GetPoint(i-1);
				float dx = GetPoint(i).x - GetPoint(i-1).x;
				float dy = vec.y*dx/vec.x;
				vec = Point2(dx,dy);
				SetOut(i-1,GetPoint(i-1)+vec);
				}
			// Fix Vector Handlers of i+1 in vec
			if (GetIn(i+1).x < GetPoint(i).x) {
				Point2 vec = GetIn(i+1) - GetPoint(i+1);
				float dx = GetPoint(i).x - GetPoint(i+1).x;
				float dy = vec.y*dx/vec.x;
				vec = Point2(dx,dy);
				SetIn(i+1,GetPoint(i+1)+vec);
				}
			continue;
			}
		}
	selPoints.SetSize(NumPoints());
	selPoints.ClearAll();
	}

void ControlCurve::DelPoints(){	
	if ((NumPoints() - NumSel()) < 2) return; 
	for (int i=NumPoints()-1; i>=0; i--) {
		if(IsSel(i))
			points.Delete(i,1);
		}
	Point2 in = GetPoint(0);
	in.x = 0.0f;
	SetPoint(0,in);
	Point2 fin = GetPoint(NumPoints()-1);
	fin.x = 1.0f;
	SetPoint(NumPoints()-1,fin);
	selPoints.SetSize(NumPoints());
	selPoints.ClearAll();
	}

void ControlCurve::BuildDU() {
	float sum = 0.0f;
	for ( int i=0;i<NumPoints()-1;i++) {
		points[i].du = Length(points[i].p - points[i+1].p);
		sum += points[i].du;
		}
	for ( int i=0;i<NumPoints()-1;i++) {
		points[i].du /= sum;
		}
	}

void ControlCurve::Select(Tab<int> &hits,BOOL toggle,BOOL subtract,BOOL all)
	{
	for (int i=0; i<hits.Count(); i++) {
		if (toggle) SetSel(hits[i],!IsSel(hits[i]));
		else if (subtract) SetSel(hits[i],FALSE);
		else SetSel(hits[i],TRUE);
		}	
	}

void ControlCurve::SetFullXPoint(float x) 
	{
	int numSel = 0;
	int sel=0;
	for (int i=0; i<NumPoints(); i++) 
		if (IsSel(i)) {
			numSel++;
			sel = i;
			}
	if (numSel != 1) return;

	float mx = x - GetPoint(sel).x;
	if (sel == 0 || sel == NumPoints()-1) mx = 0.0f;

	SetPoint(sel, Point2(GetPoint(sel).x+mx,GetPoint(sel).y));
	SetIn(sel, Point2(GetIn(sel).x+mx,GetIn(sel).y));
	SetOut(sel, Point2(GetOut(sel).x+mx,GetOut(sel).y));
	}

void ControlCurve::SetFullYPoint(float y) 
	{
	int numSel = 0;
	int sel=0;
	for (int i=0; i<NumPoints(); i++) 
		if (IsSel(i)) {
			numSel++;
			sel = i;
			}
	if (numSel != 1) return;

	float my = y - GetPoint(sel).y;

	SetPoint(sel, Point2(GetPoint(sel).x,GetPoint(sel).y+my));
	SetIn(sel, Point2(GetIn(sel).x,GetIn(sel).y+my));
	SetOut(sel, Point2(GetOut(sel).x,GetOut(sel).y+my));
	}

void ControlCurve::MovePoints(Point2 pt,BOOL in, BOOL out, int inout)
	{
	Point2 nPt;
	if (in) {
		MoveIn(pt,inout);
		return;
		}
	if (out) {
		MoveOut(pt,inout);
		return;
		}
	for (int i=0; i < NumPoints(); i++) {
		if (IsSel(i)) {
			nPt = pt;
			square sq(GetPoint(i));
			sq.InsPt(GetIn(i));
			sq.InsPt(GetOut(i));
			if (TestAFlag(MAX_FLAG) && (GetPoint(i).y + sq.t) + nPt.y > max)
				nPt.y = max - (GetPoint(i).y + sq.t);
			if (TestAFlag(MIN_FLAG) && (GetPoint(i).y - sq.b) + nPt.y  < min)
				nPt.y = min - (GetPoint(i).y - sq.b);
			if (i == 0 || i+1 == NumPoints()) nPt.x = 0.0f;
			else if (GetIn(i).x + nPt.x < GetPoint(i-1).x
				  ||
				  GetOut(i).x + nPt.x > GetPoint(i+1).x 
				  ||
				  GetPoint(i).x + nPt.x > GetIn(i+1).x
				  ||
				  GetPoint(i).x + nPt.x < GetOut(i-1).x)
				  nPt.x = 0.0f;
			SetPoint(i,GetPoint(i)+nPt);
			SetIn(i,GetIn(i)+nPt);
			SetOut(i,GetOut(i)+nPt);
			}
		}
	}

void ControlCurve::MoveIn(Point2 pt,int inout) {
	if (inout == 0) return;
	Point2 oPt = GetIn(inout);
	Point2 nPt;
	nPt = pt;
	if (inout == 0) nPt.x = 0.0f;
	if (TestAFlag(MAX_FLAG) && GetIn(inout).y + nPt.y > max)
		nPt.y = max - GetIn(inout).y;
	if (TestAFlag(MIN_FLAG) && GetIn(inout).y + nPt.y < min)
		nPt.y = min - GetIn(inout).y;
	if (GetIn(inout).x + nPt.x < GetPoint(inout-1).x
		||
		GetIn(inout).x + nPt.x > GetPoint(inout).x)
		nPt.x = 0.0f;
	SetIn(inout,GetIn(inout)+nPt);
	if (points[inout].kind == CCPOINT_BEZIER && inout != NumPoints()-1) {
		float lii = Length(GetPoint(inout)-oPt);
		float lif = Length(GetPoint(inout)-GetIn(inout));
		Point2 vec = (GetPoint(inout)-GetIn(inout))/lif;
		float loi = Length(GetPoint(inout)-GetOut(inout));
		float lof = lif*loi/lii;
		vec = lof*vec;
		if(GetPoint(inout).x + vec.x > GetPoint(inout+1).x) {
			float dx = GetPoint(inout+1).x - GetPoint(inout).x;
			float dy = vec.y*dx/vec.x;
			vec = Point2(dx,dy);
			}
		if (TestAFlag(MAX_FLAG) && GetPoint(inout).y + vec.y > max) {
			float dy = max - GetPoint(inout).y;
			float dx = vec.x*dy/vec.y;
			vec = Point2(dx,dy);
			}
		if (TestAFlag(MIN_FLAG) && GetPoint(inout).y + vec.y < min) {
			float dy = max - GetPoint(inout).y;
			float dx = vec.x*dy/vec.y;
			vec = Point2(dx,dy);
			}
		SetOut(inout,GetPoint(inout)+vec);
		}
	}

void ControlCurve::MoveOut(Point2 pt,int inout) {
	if (inout == NumPoints()-1) return;
	Point2 oPt = GetOut(inout);
	Point2 nPt;
	nPt = pt;
	if (inout == NumPoints() - 1) nPt.x = 0.0f;
	if (TestAFlag(MAX_FLAG) && GetOut(inout).y + nPt.y > max)
		nPt.y = max - GetOut(inout).y;
	if (TestAFlag(MIN_FLAG) && GetOut(inout).y + nPt.y < min)
		nPt.y = min - GetOut(inout).y;
	if (GetOut(inout).x + nPt.x > GetPoint(inout+1).x
		||
		GetOut(inout).x + nPt.x < GetPoint(inout).x)
		nPt.x = 0.0f;
	SetOut(inout,GetOut(inout)+nPt);
	if (points[inout].kind == CCPOINT_BEZIER && inout != 0) {
		float loi = Length(GetPoint(inout)-oPt);
		float lof = Length(GetPoint(inout)-GetOut(inout));
		Point2 vec = (GetPoint(inout)-GetOut(inout))/lof;
		float lii = Length(GetPoint(inout)-GetIn(inout));
		float lif = lii*lof/loi;
		vec = lif*vec;
		if(GetPoint(inout).x + vec.x < GetPoint(inout-1).x) {
			float dx =  GetPoint(inout-1).x - GetPoint(inout).x;
			float dy = vec.y*dx/vec.x;
			vec = Point2(dx,dy);
			}
		if (TestAFlag(MAX_FLAG) && GetPoint(inout).y + vec.y > max) {
			float dy = max - GetPoint(inout).y;
			float dx = vec.x*dy/vec.y;
			vec = Point2(dx,dy);
			}
		if (TestAFlag(MIN_FLAG) && GetPoint(inout).y + vec.y < min) {
			float dy = max - GetPoint(inout).y;
			float dx = vec.x*dy/vec.y;
			vec = Point2(dx,dy);
			}
		SetIn(inout,GetPoint(inout)+vec);
		}

	}

void ControlCurve::SetLine() {
	for(int i=0; i<NumPoints();i++) {
		if (!IsSel(i)) continue;
		SetKind(i,CCPOINT_CORNER);
		SetIn(i,GetPoint(i));
		SetOut(i,GetPoint(i));
		}
	}

void ControlCurve::SetCorner() {
	BuildDU();
	for(int i=0; i<NumPoints();i++) {
		if (!IsSel(i)) continue;
		if (GetKind(i) == CCPOINT_CORNER)
			BuildSmooth(i);
		SetKind(i,CCPOINT_BEZCORN);
		}
	}

void ControlCurve::SetSmooth() {
	if (NumPoints() < 3) return;
	BuildDU();
	for(int i=0;i<NumPoints();i++) {
		if (!IsSel(i)) continue;
		BuildSmooth(i);
		SetKind(i,CCPOINT_BEZIER);
		}
	}

// We are going to build smooth curve knots only when the selection set is
// set to Bezier curve...
void ControlCurve::BuildSmooth(int i) {
	if (NumPoints()==2) return;
	points[i].kind = CCPOINT_BEZIER;
	Point2 dm,dp,m,m0;
	float am,ap;
	int l = i-1;
	int n = i+1;
	if (i==0) {
		Point2 d0,d1,m;
		float a;
		int j = i+1,k = i+2;

		d0 = GetPoint(j) - GetPoint(i);
		d1 = GetPoint(k) - GetPoint(j);

		a = GetDU(i)/(GetDU(i)+GetDU(j));
		m = (((1.0f+a)/GetDU(i))*d0 - (a/GetDU(j)) * d1) / 3.0f;

		Point2 fm = m*GetDU(i);

		if (GetPoint(i).x + fm.x > GetPoint(i+1).x) {
			float dx = GetPoint(i+1).x - GetPoint(i).x;
			float dy = fm.y*fm.x/dx;
			fm = Point2(dx,dy);
			}

		if (fm.x < 0.0f) fm.x = 0.0f;

		SetOut(i,GetPoint(i) + fm);
		return;
		}
	if (i==NumPoints()-1) {
		Point2 d0,d1,m;
		float a;
		int j = i-1,k = i-2;

		d0 = GetPoint(j) - GetPoint(k);
		d1 = GetPoint(i) - GetPoint(j);

		a = GetDU(k)/(GetDU(k)+GetDU(j));
		m = (((a-1.0f)/GetDU(k))*d0 + ((2.0f-a)/GetDU(j))*d1)/3.0f;

		Point2 fm = m*GetDU(j);

		if (GetPoint(i).x - fm.x < GetPoint(i-1).x){
			float dx = GetPoint(i).x - GetPoint(i-1).x;
			float dy = fm.y*fm.x/dx;
			fm = Point2(dx,dy);
			}

		if (fm.x < 0.0f) fm.x = 0.0f;

		SetIn(i,GetPoint(i) - fm);
		return;
		}

	dm = GetPoint(i) - GetPoint(l);
	dp = GetPoint(n) - GetPoint(i);
	
	ap = GetDU(l) / (GetDU(l) + GetDU(i));
	am = 1.0f - ap;
	m = ((am/GetDU(l))*dm + (ap/GetDU(i))*dp)/3.0f;
	float mm = (GetDU(l)+GetDU(i))/2.0f;
	Point2 fm = m*mm;
	if (GetPoint(i).x - fm.x < GetPoint(i-1).x){
		float dx = GetPoint(i).x - GetPoint(i-1).x;
		float dy = fm.y*fm.x/dx;
		fm = Point2(dx,dy);
		}
	if (GetPoint(i).x + fm.x > GetPoint(i+1).x) {
		float dx = GetPoint(i+1).x - GetPoint(i).x;
		float dy = fm.y*fm.x/dx;
		fm = Point2(dx,dy);
		}
	SetIn(i,GetPoint(i) - fm);
	SetOut(i,GetPoint(i) + fm);
	}

// TODO: Optimizar esta formula...
Point2 ControlCurve::Ibp(int i,float u) {
	int j = i+1;
	float v = 1.0f - u;
	return
		((GetPoint(i)*v + 3.0f*GetOut(i)*u)*v + 3.0f*GetIn(j)*u*u)*v + GetPoint(j)*u*u*u;
	}

#define JMAX 20
float ControlCurve::GetValue(float u) {
	
	// Extrapolamos los valores que estan fuera del rango utilizando el vector
	// Out del punto 0.
	if (u<0.0f) {
		Point2 vec = GetOut(0) - GetPoint(0);
		if (vec.x == 0.0f || vec.y == 0.0f) return GetPoint(0).y;
		return GetPoint(0).y + u * vec.y / vec.x;
		}
	if (u == 0.0f) u = 0.000001f;
	if (u>1.0f) {
		Point3 vec = GetIn(NumPoints()-1) - GetPoint(NumPoints()-1);
		if (vec.x == 0.0f || vec.y == 0.0f) return GetPoint(NumPoints()-1).y;
		return GetPoint(NumPoints()-1).y + (u - 1.0f) * vec.y/vec.x;
		}
	if (u == 1.0f) u = 0.999999f;

	float x1,x2,rtn,dx;
	float xacc = 0.000001f;
	
	float vx = u;

	for (int i=0; i<NumPoints()-1; i++)
		if (GetPoint(i).x <= u && GetPoint(i+1).x >= u) {
			float p0 = GetPoint(i).x;
			float p1 = GetOut(i).x;
			float p2 = GetIn(i+1).x;
			float p3 = GetPoint(i+1).x;

			float a = (p3 - 3.0f * p2 + 3.0f * p1 - p0);
			float b = 3.0f * (p2 - 2.0f * p1 +  p0);
			float c = 3.0f * (p1 - p0);
			float d = p0 - vx;

			for (int sb=0; sb<8; sb++) {
				x1 = float(sb) * 0.125f;
				x2 = (float(sb) + 1.0f) * 0.125f;
				if ( (d + x1*(c + x1*(b + x1*a))) * (d + x2*(c + x2*(b + x2*a))) <= 0.0f) {
					rtn = 0.5f * (x1 + x2);
					for (int jit=1; jit<=JMAX; jit++) {
						dx = (d + rtn*(c + rtn*(b + rtn*a))) / (c + rtn*(2.0f*b + 3.0f*a*rtn));
						rtn -= dx;
						if (fabs(dx) < xacc) {
							if ((x1-rtn)*(rtn-x2) < 0.0f) {
								// Error
								}

							p0 = GetPoint(i).y;
							p1 = GetOut(i).y;
							p2 = GetIn(i+1).y;
							p3 = GetPoint(i+1).y;

							a = (p3 - 3.0f * p2 + 3.0f * p1 - p0);
							b = 3.0f * (p2 - 2.0f * p1 +  p0);
							c = 3.0f * (p1 - p0);
							d = p0;


							return d + rtn * (c + rtn * (b + rtn * a));
							jit = JMAX + 1;
							}
						} // for(jit=1->JMAX)
					} // if (f(x1) * f(x2) < 0)
				} // for (sb=0->4)
			} //if (GP(i) > u)

	return 1.0f;
	}


//***********************************************************************//
//***********************************************************************//
//																		 //
//          CURVE DIALOG BOXES METHODS                                   //
//																		 //
//***********************************************************************//
//***********************************************************************//
static HIMAGELIST hToolImages = NULL;
class DeleteResources {
	public:
		~DeleteResources() {
			if (hToolImages) ImageList_Destroy(hToolImages);			
			}
	};
static DeleteResources	theDelete;


//Win32 : static BOOL CALLBACK UTileFloaterDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
static INT_PTR CALLBACK UTileFloaterDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
	//Win32 : CurveDlg *dlg = (CurveDlg*)GetWindowLong(hWnd,GWL_USERDATA);
	CurveDlg *dlg = DLGetWindowLongPtr<CurveDlg*>(hWnd);

	switch (msg) {
		case WM_INITDIALOG:
			dlg = (CurveDlg*)lParam;
			
			//Win32 : SetWindowLong(hWnd,GWL_USERDATA,lParam);
			DLSetWindowLongPtr(hWnd, lParam);

			SendMessage(hWnd, WM_SETICON, ICON_SMALL, GetClassLongPtr(dlg->ip->GetMAXHWnd(), GCLP_HICONSM)); // mjm - 3.12.99
			dlg->SetupDlg(hWnd);
			break;

		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd,&ps);
			Rect rect;
			GetClientRect(hWnd,&rect);			
			rect.top += TOOL_HEIGHT-2;
			SelectObject(hdc,GetStockObject(WHITE_BRUSH));			
			WhiteRect3D(hdc,rect,TRUE);
			EndPaint(hWnd,&ps);
			break;
			}

		case WM_SIZE:
			dlg->SizeDlg();
			dlg->InvalidateView();
			break;

		case WM_CLOSE:
			DestroyWindow(hWnd);
			break;

		case WM_DESTROY:
			dlg->DestroyDlg();
			break;

		case CC_SPINNER_CHANGE:
			switch (LOWORD(wParam)) {
				case IDC_CC_XSPIN:
					dlg->SetXPosition(dlg->iXPos->GetFVal());
					dlg->ip->RedrawViews(dlg->ip->GetTime());
					break;
				case IDC_CC_YSPIN:
					dlg->SetYPosition(dlg->iYPos->GetFVal());
					dlg->ip->RedrawViews(dlg->ip->GetTime());
					break;
				}
			UpdateWindow(hWnd);
			break;

		case CC_SPINNER_BUTTONDOWN:
		case WM_CUSTEDIT_ENTER:
		case CC_SPINNER_BUTTONUP:
			dlg->InvalidateView();
			break;


		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case ID_TOOL_MOVE:
				case ID_TOOL_ADD:
				case ID_TOOL_PAN:
					dlg->move = dlg->iMove->GetCurFlyOff();
					dlg->SetMode(LOWORD(wParam));
					break;
				case ID_TOOL_ZOOMX:
					dlg->SetZoomType(ZOOM_X);
					dlg->SetMode(LOWORD(wParam));
					break;
				case ID_TOOL_ZOOMY:
					dlg->SetZoomType(ZOOM_Y);
					dlg->SetMode(LOWORD(wParam));
					break;
				case ID_TOOL_ZOOM:
					dlg->SetZoomType(ZOOM_XY);
					dlg->SetMode(LOWORD(wParam));
					break;
				case ID_TOOL_DEL: {
					theHold.Begin();
					CurveRestore *cr;
					theHold.Put(cr=(new CurveRestore(dlg)));
					dlg->cc->DelPoints();
					theHold.Accept(_T("Delete Point"));
					dlg->InvalidateView();
					dlg->ip->RedrawViews(dlg->ip->GetTime());
					}
					break; 
				case ID_TOOL_RESET: {
					theHold.Begin();
					CurveRestore *cr;
					theHold.Put(cr=(new CurveRestore(dlg)));
					dlg->cc->Reset();
					theHold.Accept(_T("Reset Curve"));
					dlg->InvalidateView();
					dlg->ip->RedrawViews(dlg->ip->GetTime());
					}
					break;
				case ID_TOOL_BEZ: {
					theHold.Begin();
					CurveRestore *cr;
					theHold.Put(cr=(new CurveRestore(dlg)));
					dlg->cc->SetSmooth();
					theHold.Accept(_T("Set Point Kind"));
					dlg->InvalidateView();
					dlg->ip->RedrawViews(dlg->ip->GetTime());
					}
					break; 
				case ID_TOOL_CORN: {
					theHold.Begin();
					CurveRestore *cr;
					theHold.Put(cr=(new CurveRestore(dlg)));
					dlg->cc->SetCorner();
					theHold.Accept(_T("Set Point Kind"));
					dlg->InvalidateView();
					dlg->ip->RedrawViews(dlg->ip->GetTime());
					}
					break;
				case ID_TOOL_LINE: {
					theHold.Begin();
					CurveRestore *cr;
					theHold.Put(cr=(new CurveRestore(dlg)));
					dlg->cc->SetLine();
					theHold.Accept(_T("Set Point Kind"));
					dlg->InvalidateView(); 
					dlg->ip->RedrawViews(dlg->ip->GetTime());
					}
					break;
				case ID_TOOL_ZE:
					dlg->ZoomExtents();
					dlg->InvalidateView();
					break;
				case ID_TOOL_ZHE:
					dlg->ZoomExtentsH();
					dlg->InvalidateView();
					break;
				case ID_TOOL_ZVE:
					dlg->ZoomExtentsV();
					dlg->InvalidateView();
					break;					
				}
			break;


		default:
			return FALSE;
		}
	return TRUE;
	}

static LRESULT CALLBACK UTileViewProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
	{
	//Win32 : CurveDlg *dlg = (CurveDlg*)GetWindowLong(hWnd,GWL_USERDATA);
	CurveDlg *dlg = DLGetWindowLongPtr<CurveDlg*>(hWnd);
	
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
		case WM_MOUSEMOVE:
			return dlg->mouseMan.MouseWinProc(hWnd,msg,wParam,lParam);

		default:
			return DefWindowProc(hWnd,msg,wParam,lParam);
		}	
	return 0;
	}

CurveDlg::CurveDlg(ReferenceTarget *mod,ControlCurve *c, IObjParam *iop,HWND hParent, TSTR tit) {
	title = tit;
	xzoom = 0.9f;
	yzoom = 0.9f;
	xscroll = 0.0f;
	yscroll = 0.0f;
	move = ID_TOOL_MOVE;
	cc   = c;
	mmod = mod;
	ip   = iop;
	move = 0;
	punto = Point2(0.0f,0.0f);
	selCur   = ip->GetSysCursor(SYSCUR_SELECT);
	moveCur	 = ip->GetSysCursor(SYSCUR_MOVE);
	addCur	 = ip->GetSysCursor(SYSCUR_SELECT);
	RegisterClasses();
	CreateDialogParam(
		hInstance,
		MAKEINTRESOURCE(IDD_SM_UTILE),
		hParent,
		UTileFloaterDlgProc,
		(LPARAM)this);
	}

BOOL CurveDlg::IsActive() {
	if (hMain) return 1;
	else return 0;
	}

//JW Code Change: NotifyRefChanged Signuature changed with Max2015+
#if MAX_VERSION_MAJOR < 17
RefResult CurveDlg::NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, PartID& partID,  RefMessage message)
#else
RefResult CurveDlg::NotifyRefChanged( const Interval& changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message, BOOL propagate)
#endif
	{
	switch (message) {
		case REFMSG_CHANGE:
			InvalidateView();			
			break;
		
		case REFMSG_REF_DELETED:
			MaybeCloseWindow();
			break;
		}
	return REF_SUCCEED;
	}

void CurveDlg::InvalidateView()
	{
	InvalidateTypeins();
	if (hView) {
		InvalidateRect(hView,NULL,TRUE);
		}

	MultiMapMod *mm = (MultiMapMod*)mmod;

	mm->uvwProy[mm->current_channel]->valid_group = 0;
	mm->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	}

void CurveDlg::InvalidateTypeins()
	{
	typeInsValid = FALSE;	
	}


void CurveDlg::RegisterClasses()
	{
	if (!hToolImages) {
		HBITMAP hBitmap, hMask;	
		hToolImages = ImageList_Create(16, 15, TRUE, 4, 0);
		hBitmap = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_UTILETOOL));
		hMask   = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_MASK_UTILETOOL));
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
		wc.hbrBackground = NULL; //(HBRUSH)GetStockObject(WHITE_BRUSH);	
		wc.lpszMenuName  = NULL;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.lpfnWndProc   = UTileViewProc;
		wc.lpszClassName = _T("UTileView");
		RegisterClass(&wc);
		}
	}

// Esta funcion es llamada en el WM_INITDIALOG de la funcion que
// maneja el dialogo flotante
void CurveDlg::SetupDlg(HWND hWnd)
	{
	this->hMain = hWnd;

	keyboardEvent.SetDlg(this);
	ip->RegisterDeleteUser(&keyboardEvent);
	
	moveGMode = new MoveMode(this);
	addGMode  = new AddMode(this);
	rightMode = new RightMouseMode(this);
	zoomGMode = new ZoomMode(this);
	panGMode  = new PanMode(this);

	typeInsValid = TRUE;

	mouseMan.SetMouseProc(rightMode,RIGHT_BUTTON,1);

	iXPos = GetISpinner(GetDlgItem(hWnd,IDC_CC_XSPIN));
	iXPos->LinkToEdit(GetDlgItem(hWnd,IDC_CC_X),EDITTYPE_FLOAT);
	iXPos->SetLimits(-9999999, 9999999, FALSE);
	iXPos->SetAutoScale();
	
	iYPos = GetISpinner(GetDlgItem(hWnd,IDC_CC_YSPIN));
	iYPos->LinkToEdit(GetDlgItem(hWnd,IDC_CC_Y),EDITTYPE_FLOAT);
	iYPos->SetLimits(-9999999, 9999999, FALSE);
	iYPos->SetAutoScale();


	iTool = GetICustToolbar(GetDlgItem(hWnd,IDC_SM_TOOLBAR));
	iTool->SetBottomBorder(TRUE);	
	iTool->SetImage(hToolImages);
	iTool->AddTool(ToolSeparatorItem(5));
	iTool->AddTool(
		ToolButtonItem(CTB_CHECKBUTTON,
			0, 0, 1, 1, 16, 15, 23, 22, ID_TOOL_MOVE));
	iTool->AddTool(
		ToolButtonItem(CTB_CHECKBUTTON,
			2, 2, 3, 3, 16, 15, 23, 22, ID_TOOL_ADD));
	iTool->AddTool(
		ToolButtonItem(CTB_PUSHBUTTON,
			4, 4, 4, 4, 16, 15, 23, 22, ID_TOOL_DEL));
	iTool->AddTool(ToolSeparatorItem(7));
	iTool->AddTool(
		ToolButtonItem(CTB_PUSHBUTTON,
			11, 11, 11, 11, 16, 15, 23, 22, ID_TOOL_BEZ));
	iTool->AddTool(
		ToolButtonItem(CTB_PUSHBUTTON,
			12, 12, 12, 12, 16, 15, 23, 22, ID_TOOL_CORN));
	iTool->AddTool(
		ToolButtonItem(CTB_PUSHBUTTON,
			13, 13, 13, 13, 16, 15, 23, 22, ID_TOOL_LINE));
	iTool->AddTool(ToolSeparatorItem(8));
	iTool->AddTool(
		ToolButtonItem(CTB_PUSHBUTTON,
			5, 5, 5, 5, 16, 15, 23, 22, ID_TOOL_RESET));
	iTool->AddTool(ToolSeparatorItem(10));
	iTool->AddTool(
		ToolButtonItem(CTB_CHECKBUTTON,
			6, 6, 6, 6, 16, 15, 23, 22, ID_TOOL_PAN));
	iTool->AddTool(ToolSeparatorItem(3));
	iTool->AddTool(
		ToolButtonItem(CTB_PUSHBUTTON,
			8, 8, 8, 8, 16, 15, 23, 22, ID_TOOL_ZE));
	iTool->AddTool(
		ToolButtonItem(CTB_PUSHBUTTON,
			9, 9, 9, 9, 16, 15, 23, 22, ID_TOOL_ZHE));
	iTool->AddTool(
		ToolButtonItem(CTB_PUSHBUTTON,
			10, 10, 10, 10, 16, 15, 23, 22, ID_TOOL_ZVE));
	iTool->AddTool(ToolSeparatorItem(3));
	iTool->AddTool(
		ToolButtonItem(CTB_CHECKBUTTON,
			7, 7, 7, 7, 16, 15, 23, 22, ID_TOOL_ZOOM));
	iTool->AddTool(
		ToolButtonItem(CTB_CHECKBUTTON,
			14, 14, 14, 14, 16, 15, 23, 22, ID_TOOL_ZOOMX));
	iTool->AddTool(
		ToolButtonItem(CTB_CHECKBUTTON,
			15, 15, 15, 15, 16, 15, 23, 22, ID_TOOL_ZOOMY));

	iMove    = iTool->GetICustButton(ID_TOOL_MOVE);
	iAdd     = iTool->GetICustButton(ID_TOOL_ADD);
	iDel     = iTool->GetICustButton(ID_TOOL_DEL);
	iReset	 = iTool->GetICustButton(ID_TOOL_RESET);
	iBez	 = iTool->GetICustButton(ID_TOOL_BEZ);
	iCorn	 = iTool->GetICustButton(ID_TOOL_CORN);
	iLine	 = iTool->GetICustButton(ID_TOOL_LINE);
	iPan	 = iTool->GetICustButton(ID_TOOL_PAN);
	iZe 	 = iTool->GetICustButton(ID_TOOL_ZE);
	iZhe	 = iTool->GetICustButton(ID_TOOL_ZHE);
	iZve	 = iTool->GetICustButton(ID_TOOL_ZVE);
	iZoom	 = iTool->GetICustButton(ID_TOOL_ZOOM);
	iZoomX	 = iTool->GetICustButton(ID_TOOL_ZOOMX);
	iZoomY	 = iTool->GetICustButton(ID_TOOL_ZOOMY);

	iMove->SetHighlightColor(GREEN_WASH);
	iAdd->SetHighlightColor(GREEN_WASH);
	iPan->SetHighlightColor(GREEN_WASH);
	iZoom->SetHighlightColor(GREEN_WASH);
	iZoomX->SetHighlightColor(GREEN_WASH);
	iZoomY->SetHighlightColor(GREEN_WASH);

	iMove->SetTooltip(TRUE,GetString(IDS_CC_MOVE));
	iAdd->SetTooltip(TRUE,GetString(IDS_CC_ADD));
	iDel->SetTooltip(TRUE,GetString(IDS_CC_DEL));
	iReset->SetTooltip(TRUE,GetString(IDS_CC_RESET));
	iBez->SetTooltip(TRUE,GetString(IDS_CC_BEZ));
	iCorn->SetTooltip(TRUE,GetString(IDS_CC_CORN));
	iLine->SetTooltip(TRUE,GetString(IDS_CC_LINE));
	iPan->SetTooltip(TRUE,GetString(IDS_CC_PAN));
	iZe->SetTooltip(TRUE,GetString(IDS_CC_ZE));
	iZhe->SetTooltip(TRUE,GetString(IDS_CC_ZHE));
	iZve->SetTooltip(TRUE,GetString(IDS_CC_ZVE));
	iZoom->SetTooltip(TRUE,GetString(IDS_CC_ZOOM));
	iZoomX->SetTooltip(TRUE,GetString(IDS_CC_ZOOMX));
	iZoomY->SetTooltip(TRUE,GetString(IDS_CC_ZOOMY));

	FlyOffData fdata1[] = {
		{ 0,  0,  1,  1},
		{ 16, 16, 16, 16},
		{ 17, 17, 17, 17}};
	iMove->SetFlyOff(3,fdata1,ip->GetFlyOffTime(),move,FLY_DOWN);


	hView = GetDlgItem(hMain,IDC_SM_VIEW);
	SetWindowText(hMain,title);

	//Win32 : SetWindowLong(hView,GWL_USERDATA,(LONG)this);
	DLSetWindowLongPtr(hView, this);

	iBuf = CreateIOffScreenBuf(hView);
	iBuf->SetBkColor(RGB(225,225,225));

	SizeDlg();
	}

void CurveDlg::ComputeZooms(HWND hwnd, int &width, int &height)
	{
 	Rect rect;
	GetClientRect(hwnd,&rect);	
	width = rect.w()-1;
	height = rect.h()-1;
	}

void CurveDlg::SetZoomType(int zt) {
	zoomGMode->zoomtype = zt;
	}

Point2 CurveDlg::ScreenToPoint(IPoint2 m, int w, int h) {
	int wd = w - GRID_WID;
	return Point2(	(m.x-xscroll-wd/2.0f-GRID_WID)/(wd*xzoom) + 0.5f , 
					-2.0f*(m.y-yscroll-h/2.0f)/(h*yzoom));
	}

Point2 CurveDlg::PointToScreen(Point2 pt,int w,int h)
	{	
	int wd = w - GRID_WID;
	return Point2(	xscroll + wd/2.0f + wd*xzoom*(pt.x - 0.5f) + GRID_WID, 
					yscroll + h/2.0f - h*yzoom*pt.y/2.0f);
	}

void CurveDlg::SetupTypeins()
	{
	if (typeInsValid) return;
	typeInsValid = TRUE;

	Point2 xy(0.0f,0.0f);
	int numSel = 0;
	int numPoint = 0;

	for (int i=0; i<cc->NumPoints(); i++) {
		if (cc->IsSel(i)) {
			numSel++;
			numPoint = i;
			}
		}

	if (numSel == 1) {
		// Cuadramos los limites...
		float x0,x1,xl,xr;

		if (numPoint == 0) { 
			if ( (cc->GetOut(numPoint).x - cc->GetPoint(numPoint).x) >
				 (cc->GetPoint(numPoint+1).x -cc->GetIn(numPoint+1).x) )
				 xr = (cc->GetOut(numPoint).x - cc->GetPoint(numPoint).x);
			else xr = (cc->GetPoint(numPoint+1).x - cc->GetIn(numPoint+1).x);

			x0 = 0.0f;
			x1 = cc->GetPoint(numPoint+1).x - xr;
		} else if (numPoint == cc->NumPoints()-1) {
			if ( (cc->GetOut(numPoint-1).x - cc->GetPoint(numPoint-1).x) > 
				 (cc->GetPoint(numPoint).x - cc->GetIn(numPoint).x) )
				 xl = (cc->GetOut(numPoint-1).x - cc->GetPoint(numPoint-1).x);
			else xl = (cc->GetPoint(numPoint).x - cc->GetIn(numPoint).x);

			x0 = cc->GetPoint(numPoint-1).x + xl;
			x1 = 1.0f;
		} else {
			if ( (cc->GetOut(numPoint-1).x - cc->GetPoint(numPoint-1).x) > 
				 (cc->GetPoint(numPoint).x - cc->GetIn(numPoint).x) )
				 xl = (cc->GetOut(numPoint-1).x - cc->GetPoint(numPoint-1).x);
			else xl = (cc->GetPoint(numPoint).x - cc->GetIn(numPoint).x);

			if ( (cc->GetOut(numPoint).x - cc->GetPoint(numPoint).x) >
				 (cc->GetPoint(numPoint+1).x - cc->GetIn(numPoint+1).x) )
				 xr = (cc->GetOut(numPoint).x - cc->GetPoint(numPoint).x);
			else xr = (cc->GetPoint(numPoint+1).x - cc->GetIn(numPoint+1).x);

			x0 = cc->GetPoint(numPoint-1).x + xl;
			x1 = cc->GetPoint(numPoint+1).x - xr;
			}

		// TLTODO: Pilas que no vallamos a mover el point 0 a algo dif de 0.0f...
		iXPos->Enable();
		iYPos->Enable();

		iXPos->SetIndeterminate(FALSE);
		iYPos->SetIndeterminate(FALSE);

		iXPos->SetLimits(x0,x1,FALSE);
		iXPos->SetValue(cc->GetPoint(numPoint).x,FALSE);
		iYPos->SetValue(cc->GetPoint(numPoint).y,FALSE);

		float yt,yb;
		
		square sq(cc->GetPoint(numPoint));
		sq.InsPt(cc->GetIn(numPoint));
		sq.InsPt(cc->GetOut(numPoint));
		
		yt = 999999999.0f;
		yb = -999999999.0f;

		if (cc->TestAFlag(MAX_FLAG)) yt = cc->max - sq.t;
		if (cc->TestAFlag(MIN_FLAG)) yb = cc->min + sq.b;

		iYPos->SetLimits(yb,yt,FALSE);

	} else {
		iXPos->SetIndeterminate(TRUE);
		iYPos->SetIndeterminate(TRUE);

		iXPos->Disable();
		iYPos->Disable();
		}
	}


#define NUMSEGS		15
#define FONTSIZE	14
void CurveDlg::PaintView()
	{
	PAINTSTRUCT		ps;
	BeginPaint(hView,&ps);
	EndPaint(hView,&ps);
	TimeValue t = ip->GetTime();
	int i;

	int width,height;
	ComputeZooms(hView,width,height);

	SetupTypeins();

	iBuf->Erase();

	HDC hdc = iBuf->GetDC();

	HPEN gPen		= CreatePen(PS_SOLID,2,RGB(100,100,100));
	HPEN unselPen	= CreatePen(PS_SOLID,1,RGB(30,30,220));
	HPEN linePen	= CreatePen(PS_SOLID,1,RGB(255,0,0));
	HPEN blkPen		= CreatePen(PS_SOLID,1,RGB(0,0,0));
	HPEN gydPen		= CreatePen(PS_SOLID,1,RGB(198,170,123));
	HPEN gyPen		= CreatePen(PS_SOLID,1,RGB(140,117,66));
	HPEN gy0Pen		= CreatePen(PS_SOLID,3,RGB(140,117,66));
	HPEN grayPen	= CreatePen(PS_SOLID,1,RGB(198,198,198));
	HPEN papaPen	= CreatePen(PS_SOLID,1,RGB(165,165,165));
	HPEN greenPen	= CreatePen(PS_DOT,1,RGB(0,101,99));

	HBRUSH whtBrush	= CreateSolidBrush(RGB(255,255,255));
	HBRUSH blkBrush = CreateSolidBrush(RGB(0,0,0));
	HBRUSH grayBrush= CreateSolidBrush(RGB(198,198,198));
	HBRUSH papaBrush= CreateSolidBrush(RGB(165,165,165));

	HFONT hFont		= CreateFont(FONTSIZE,0,0,0,FW_NORMAL,0,0,0,0,0,0,0, VARIABLE_PITCH | FF_MODERN, _T(""));

	SetTextColor( hdc, RGB(0,0,0));
	SetBkColor(hdc,RGB(198,198,198));
	SelectObject(hdc,hFont);

	// Paint X Grid...

	Point2 gxv = PointToScreen(Point2(0.0f,0.0f),width,height);
	SelectObject(hdc,papaPen);
	SelectObject(hdc,papaBrush);
	if (gxv.x > 0.0f) {
		Rectangle(hdc,0,0,(int)gxv.x,height);
		SelectObject(hdc,greenPen);
		MoveToEx(hdc,(int)gxv.x, 0, NULL);
		LineTo(hdc,(int)gxv.x, height);
		}

	gxv = PointToScreen(Point2(1.0f,0.0f),width,height);
	SelectObject(hdc,papaPen);
	SelectObject(hdc,papaBrush);
	if (gxv.x < (float)width){
		Rectangle(hdc,(int)gxv.x,0,width,height);
		SelectObject(hdc,greenPen);
		MoveToEx(hdc,(int)gxv.x, 0, NULL);
		LineTo(hdc,(int)gxv.x, height);
		}


	
	// Paint Y Grid...
	float vmin = -2.0f*(height-yscroll-height/2.0f)/(height*yzoom);
	float vmax = -2.0f*(-yscroll-height/2.0f)/(height*yzoom);

	int mnd = int(height/(FONTSIZE*4));
	float minStep = 2.0f/(yzoom*mnd);
	float step = float(pow((float)10,(float)ceil(log10(minStep)))/10.0f);
	int nd = int(2/(yzoom*step))+1;
	for (i=0; i<=nd; i++) {
		float v = float(ceil((vmin + (vmax-vmin)*i/nd)/(step)) * (step));

		SelectObject(hdc,gydPen);
		Point2 gp0 = PointToScreen(Point2(0.0f,v),width,height);
		Point2 gp1 = PointToScreen(Point2(1.0f,v),width,height);
		MoveToEx(hdc,GRID_WID, (int)gp0.y, NULL);
		LineTo(hdc, width, (int)gp1.y);
		}

	mnd = int(height/(FONTSIZE+5));
	minStep = 2.0f/(yzoom*mnd);
	step = float(pow((float)10,(float)ceil(log10(minStep))));
	nd= int(2/(yzoom*step))+1;
	for (i=0; i<=nd; i++) {
		float v = float(ceil((vmin + (vmax-vmin)*i/nd)/step) * step);
	
		if (v==0.0f) SelectObject(hdc,gy0Pen);
		else SelectObject(hdc,gyPen);
		Point2 gp = PointToScreen(Point2(0.0f,v),width,height);
		MoveToEx(hdc,GRID_WID, (int)gp.y, NULL);
		LineTo(hdc, width, (int)gp.y);
		}


	// Pintamos la linea
	SelectObject(hdc,linePen);
	Point2 sp = PointToScreen(cc->GetPoint(0),width,height);
	MoveToEx(hdc,(int)sp.x, (int)sp.y, NULL);
	Point2 ep;
	for(i=0; i<cc->NumPoints()-1; i++) {
		for(int j=0; j<=NUMSEGS; j++) {
			float u = float(j)/float(NUMSEGS);
			Point2 ep = PointToScreen(cc->Ibp(i,u),width,height);	
			LineTo(hdc,(int)ep.x, (int)ep.y);
			sp = ep;
			}
		}

	// Pintamos los Vertices
	for(i=0; i<cc->NumPoints(); i++) {
		Point2 sp = PointToScreen(cc->GetPoint(i),width,height);	
		if (cc->IsSel(i)) {
			SelectObject(hdc,blkPen);
			SelectObject(hdc,whtBrush);
			Rectangle(hdc,
				(int)sp.x-3,(int)sp.y-3,
				(int)sp.x+3,(int)sp.y+3);
			if (cc->GetKind(i) == CCPOINT_BEZIER || cc->GetKind(i) == CCPOINT_BEZCORN){
				SelectObject(hdc,blkPen);
				Point2 inP = PointToScreen(cc->GetIn(i),width,height);
				Point2 outP = PointToScreen(cc->GetOut(i),width,height);
				MoveToEx(hdc,(int)inP.x, (int)inP.y, NULL);
				LineTo(hdc,(int)sp.x, (int)sp.y);
				LineTo(hdc,(int)outP.x, (int)outP.y);
				SelectObject(hdc,blkPen);
				SelectObject(hdc,blkBrush);
				if (i != 0)
					Rectangle(hdc,
						(int)inP.x-2,(int)inP.y-2,
						(int)inP.x+2,(int)inP.y+2);	
				if (i != cc->NumPoints()-1)
					Rectangle(hdc,
						(int)outP.x-2,(int)outP.y-2,
						(int)outP.x+2,(int)outP.y+2);	
				}
			}
		else{
			SelectObject(hdc,blkPen);
			SelectObject(hdc,blkBrush);
			Rectangle(hdc,
				(int)sp.x-3,(int)sp.y-3,
				(int)sp.x+3,(int)sp.y+3);	
			}
			
		}

	// Pintamos la regleta Y
	SelectObject(hdc,grayPen);
	SelectObject(hdc,grayBrush);
	Rectangle(hdc,0,0,GRID_WID,height);

	TSTR buf;
	for (i=0; i<=nd; i++) {
		float v = float(ceil((vmin + (vmax-vmin)*i/nd)/step) * step);
		buf.printf(_T("%.1f"),v);
		Point2 py = PointToScreen(Point2(0.0f,v),width,height);
		TextOut(hdc, 2, int(py.y-float(FONTSIZE)/2.0f), buf, buf.length());
		}


	iBuf->Blit();

	SelectObject(hdc,GetStockObject(WHITE_PEN));
	DeleteObject(unselPen);
	DeleteObject(linePen);
	DeleteObject(blkPen);
	DeleteObject(gydPen);
	DeleteObject(gyPen);
	DeleteObject(gy0Pen);
	DeleteObject(grayPen);
	DeleteObject(papaPen);
	DeleteObject(greenPen);
	DeleteObject(whtBrush);
	DeleteObject(blkBrush);
	DeleteObject(grayBrush);
	DeleteObject(papaBrush);
	DeleteObject(hFont);
//	mmod->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	}

void CurveDlg::ZoomExtents() {
	xzoom = 0.9f;
	xscroll = 0.0f;
	ZoomExtentsV();
	}

void CurveDlg::ZoomExtentsH() {
	xzoom = 0.9f;
	xscroll = 0.0f;
	}

void CurveDlg::ZoomExtentsV() {
	float maxy;
	float miny;

	maxy = cc->max;
	miny = cc->min;

	int width,height;
	ComputeZooms(hView,width,height);

	Point2 auxp;

	for(int i=0; i<cc->NumPoints()-1; i++) {
		if (cc->GetPoint(i).y > maxy) maxy = cc->GetPoint(i).y;
		if (cc->GetPoint(i).y < miny) miny = cc->GetPoint(i).y;
		if (cc->GetIn(i).y > maxy) maxy = cc->GetIn(i).y;
		if (cc->GetIn(i).y < miny) miny = cc->GetIn(i).y;
		if (cc->GetOut(i).y > maxy) maxy = cc->GetOut(i).y;
		if (cc->GetOut(i).y < miny) miny = cc->GetOut(i).y;
		for(int j=0; j<=NUMSEGS; j++) {
			float u = float(j)/float(NUMSEGS);
			auxp = cc->Ibp(i,u);
			if (auxp.y > maxy) maxy = auxp.y;
			if (auxp.y < miny) miny = auxp.y;
			}
		}

	if (cc->GetPoint(cc->NumPoints()-1).y > maxy) maxy = cc->GetPoint(cc->NumPoints()-1).y;
	if (cc->GetPoint(cc->NumPoints()-1).y < miny) miny = cc->GetPoint(cc->NumPoints()-1).y;
	if (cc->GetIn(cc->NumPoints()-1).y > maxy) maxy = cc->GetIn(cc->NumPoints()-1).y;
	if (cc->GetIn(cc->NumPoints()-1).y < miny) miny = cc->GetIn(cc->NumPoints()-1).y;
	if (cc->GetOut(cc->NumPoints()-1).y > maxy) maxy = cc->GetOut(cc->NumPoints()-1).y;
	if (cc->GetOut(cc->NumPoints()-1).y < miny) miny = cc->GetOut(cc->NumPoints()-1).y;
	


	yzoom = 1.8f/(maxy-miny);
	yscroll = 0.95f*height - float(height)/2.0f + float(height)*miny*yzoom/2.0f;
	}

void CurveDlg::MaybeCloseWindow()
	{
//	CheckForNonNoiseDlg check(cont);
//	cont->EnumDependents(&check);
//	if (!check.non) {
//		PostMessage(hWnd,WM_CLOSE,0,0);
//		}
	}

void CurveDlg::SetMode(int m)
	{
	switch (mode) {
		case ID_TOOL_MOVE:    iMove->SetCheck(FALSE);   break;
		case ID_TOOL_ADD:	  iAdd->SetCheck(FALSE);    break;
		case ID_TOOL_PAN:	  iPan->SetCheck(FALSE);    break;
		case ID_TOOL_ZOOM:	  iZoom->SetCheck(FALSE);   break;
		case ID_TOOL_ZOOMX:	  iZoomX->SetCheck(FALSE);   break;
		case ID_TOOL_ZOOMY:	  iZoomY->SetCheck(FALSE);   break;
		}

	mode = m;

	switch (mode) {
		case ID_TOOL_MOVE:   
			iMove->SetCheck(TRUE);  
			mouseMan.SetMouseProc(moveGMode, LEFT_BUTTON);
			break;
		case ID_TOOL_ADD:   
			iAdd->SetCheck(TRUE);  
			mouseMan.SetMouseProc(addGMode, LEFT_BUTTON);
			break;
		case ID_TOOL_ZOOM:
			iZoom->SetCheck(TRUE);
			mouseMan.SetMouseProc(zoomGMode, LEFT_BUTTON);
			break;
		case ID_TOOL_ZOOMX:
			iZoomX->SetCheck(TRUE);
			mouseMan.SetMouseProc(zoomGMode, LEFT_BUTTON);
			break;
		case ID_TOOL_ZOOMY:
			iZoomY->SetCheck(TRUE);
			mouseMan.SetMouseProc(zoomGMode, LEFT_BUTTON);
			break;
		case ID_TOOL_PAN:
			iPan->SetCheck(TRUE);
			mouseMan.SetMouseProc(panGMode, LEFT_BUTTON);
			break;
		}
	}

void CurveDlg::DestroyDlg()
	{
	ReleaseICustToolbar(iTool);
	iTool = NULL;

	ReleaseICustButton(iMove);			iMove		= NULL;
	ReleaseICustButton(iAdd);			iAdd		= NULL;
	ReleaseICustButton(iDel);			iDel		= NULL;
	ReleaseICustButton(iReset);			iReset		= NULL;
	ReleaseICustButton(iBez);			iBez		= NULL;
	ReleaseICustButton(iCorn);			iCorn		= NULL;
	ReleaseICustButton(iLine);			iLine		= NULL;
	ReleaseICustButton(iPan);			iPan		= NULL;
	ReleaseICustButton(iZe);			iZe			= NULL;
	ReleaseICustButton(iZhe);			iZhe		= NULL;
	ReleaseICustButton(iZve);			iZve		= NULL;
	ReleaseICustButton(iZoom);			iZoom		= NULL;
	ReleaseICustButton(iZoomX);			iZoomX		= NULL;
	ReleaseICustButton(iZoomY);			iZoomY		= NULL;
	ReleaseISpinner(iXPos);				iXPos		= NULL;
	ReleaseISpinner(iYPos);				iYPos		= NULL;

	
	delete moveGMode;					moveGMode = NULL;
	delete addGMode;					addGMode = NULL;
	delete rightMode;					rightMode = NULL;
	delete zoomGMode;					zoomGMode = NULL;
	delete panGMode;					panGMode = NULL;

	DestroyIOffScreenBuf(iBuf); 

	iBuf   = NULL;
	hMain = NULL;
	hView = NULL;

	mouseMan.SetMouseProc(NULL,LEFT_BUTTON,0);
	mouseMan.SetMouseProc(NULL,RIGHT_BUTTON,0);

	ip->UnRegisterDeleteUser(&keyboardEvent);
	}

static void SetWindowYPos(HWND hWnd,int y)
	{
	Rect rect;
	GetClientRectP(hWnd,&rect);
	SetWindowPos(hWnd,NULL,rect.left,y,0,0,SWP_NOSIZE|SWP_NOZORDER);
	}

#define GRID_SIZE	35
#define GRID_HEI	15
#define SPINS_HEI	22
void CurveDlg::SizeDlg()
	{
	Rect rect;
	GetClientRect(hMain,&rect);

	MoveWindow(GetDlgItem(hMain,IDC_SM_TOOLBAR),
		0, 0, rect.w()-1, TOOL_HEIGHT, TRUE);
	MoveWindow(GetDlgItem(hMain,IDC_SM_VIEW),
		2, TOOL_HEIGHT, rect.w()-2, rect.h()-TOOL_HEIGHT-2-SPINS_HEI,FALSE);

	int ys = rect.h() - 21;
	int yl = rect.h() - 19;
	
	SetWindowYPos(GetDlgItem(hMain,IDC_CC_XLABEL),yl);
	SetWindowYPos(GetDlgItem(hMain,IDC_CC_YLABEL),yl);

	SetWindowYPos(GetDlgItem(hMain,IDC_CC_X),ys);
	SetWindowYPos(GetDlgItem(hMain,IDC_CC_Y),ys);
	
	SetWindowYPos(GetDlgItem(hMain,IDC_CC_XSPIN),ys);
	SetWindowYPos(GetDlgItem(hMain,IDC_CC_YSPIN),ys);

	InvalidateRect(hMain,NULL,TRUE);
	ZoomExtents();
	}

BOOL CurveDlg::HitTest(Rect rect,Tab<int> &hits,BOOL selOnly,BOOL &in,BOOL &out,int &inout)
	{
	in = 0;
	out = 0;
	Point2 pt,opt,ipt;
	int width,height;
	TimeValue t = ip->GetTime();
	ComputeZooms(hView,width,height);	
	for (int i=0; i<cc->NumPoints(); i++) {
		if (cc->IsSel(i) && cc->GetKind(i) != CCPOINT_CORNER) {
			opt = PointToScreen(cc->GetOut(i),width,height);
			ipt = PointToScreen(cc->GetIn(i),width,height);
			IPoint2 iopt(int(opt.x),int(opt.y));
			IPoint2 iipt(int(ipt.x),int(ipt.y));
			if (rect.Contains(iopt) && i != cc->NumPoints()-1) {
				out = TRUE;
				inout = i;
				return 0;
				}
			if (rect.Contains(iipt) && i != 0) {
				in = TRUE;
				inout = i;
				return 0;
				}
			}
		if (selOnly && !cc->IsSel(i)) continue;
		pt = PointToScreen(cc->GetPoint(i),width,height);
		IPoint2 inpt(int(pt.x),int(pt.y));

		if (rect.Contains(inpt)) {
			hits.Append(1,&i,10);
			}
		}
	return hits.Count();
	}

void CurveDlg::Select(Tab<int> &hits,BOOL toggle,BOOL subtract,BOOL all)
	{
	cc->Select(hits,toggle,subtract,all);
	}

void CurveDlg::ClearSelect()
	{
	cc->ClearSelect();
	}

void CurveDlg::MovePoints(Point2 pt)
	{
	cc->MovePoints(pt,in,out,inout);
	InvalidateView();
	}

void CurveDlg::SetXPosition(float nxp) {
	cc->SetFullXPoint(nxp);
	InvalidateView();
	}

void CurveDlg::SetYPosition(float nyp) {
	cc->SetFullYPoint(nyp);
	InvalidateView();
	}



//***********************************************************************//
//***********************************************************************//
//																		 //
//          DIALOG MOUSE PROCS											 //
//																		 //
//***********************************************************************//
//***********************************************************************//

int SelectMode::proc(HWND hWnd, int msg, int point, int flags, IPoint2 m)
	{
	switch (msg) {
		case MOUSE_POINT:
			if (point==0) {
				// First click
				
				region   = FALSE;
				toggle   = flags&MOUSE_CTRL;
				subtract = flags&MOUSE_ALT;
	
				// Hit test
				Tab<int> hits;
				Rect rect;
				rect.left = m.x-2;
				rect.right = m.x+2;
				rect.top = m.y-2;
				rect.bottom = m.y+2;
				
				// First hit test sel only
				if (!toggle && !subtract && dlg->HitTest(rect,hits,TRUE,dlg->in,dlg->out,dlg->inout)) {
					return subproc(hWnd,msg,point,flags,m);
				} else
					// Next hit test everything
					if (dlg->HitTest(rect,hits,subtract,dlg->in,dlg->out,dlg->inout)) {
						if (!toggle && !subtract) dlg->ClearSelect();
						dlg->Select(hits,toggle,subtract,FALSE);
						dlg->InvalidateView();
						if (toggle || subtract) return FALSE;
						return subproc(hWnd,msg,point,flags,m);
					} else if(dlg->in||dlg->out) {
						return subproc(hWnd,msg,point,flags,m);
					} else {
						region = TRUE;
						lm = om = m;
						XORDottedRect(hWnd,om,m);
						}				
			} else {
				// Second click
				if (region) {
					Rect rect;
					rect.left   = om.x;
					rect.top    = om.y;
					rect.right  = m.x;
					rect.bottom = m.y;
					rect.Rectify();					
					Tab<int> hits;
					if (!toggle && !subtract) dlg->ClearSelect();
					if (dlg->HitTest(rect,hits,subtract,dlg->in,dlg->out,dlg->inout)) {						
						dlg->Select(hits,FALSE,subtract,TRUE);											
						}
					dlg->InvalidateView();
				} else {
					return subproc(hWnd,msg,point,flags,m);
					}
				}
			break;			

		case MOUSE_MOVE:
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			if (region) {
				XORDottedRect(hWnd,om,lm);
				XORDottedRect(hWnd,om,m);
				lm = m;
			} else {
				SetCursor(GetXFormCur());
				return subproc(hWnd,msg,point,flags,m);
				}
			break;

		case MOUSE_FREEMOVE: {
			Tab<int> hits;
			Rect rect;
			rect.left = m.x-2;
			rect.right = m.x+2;
			rect.top = m.y-2;
			rect.bottom = m.y+2;
			if (dlg->HitTest(rect,hits,FALSE,dlg->in,dlg->out,dlg->inout)) {
				if (dlg->cc->IsSel(hits[0])) {
					SetCursor(GetXFormCur());
				}else {
					SetCursor(selCur);
					}
			}else if(dlg->in || dlg->out) {
				SetCursor(GetXFormCur());
			} else {
				SetCursor(LoadCursor(NULL, IDC_ARROW));
				}
			return subproc(hWnd,msg,point,flags,m);
			}

		case MOUSE_ABORT:
			if (region) {
				InvalidateRect(hWnd,NULL,FALSE);
			} else {
				return subproc(hWnd,msg,point,flags,m);
				}
			break;
		}
	return 1;
	}

int AddMode::proc(HWND hWnd, int msg, int point, int flags, IPoint2 m)
	{
	switch (msg) {
		case MOUSE_POINT:
			if(point==0) {
				theHold.Begin();
				CurveRestore *cr;
				theHold.Put(cr=(new CurveRestore(dlg)));

				int width, height;
				dlg->ComputeZooms(dlg->hView,width,height);
				dlg->cc->AddPoint(dlg->ScreenToPoint(m,width,height));

				theHold.Accept(_T("Add Curve Point"));
				dlg->InvalidateView();
				dlg->ip->RedrawViews(dlg->ip->GetTime());
				}
			break;	
		case MOUSE_FREEMOVE: 
			SetCursor(GetXFormCur());
			break;
		}
	return 1;
	}


int MoveMode::subproc(HWND hWnd, int msg, int point, int flags, IPoint2 m)
	{
	switch (msg) {
		case MOUSE_POINT:
			if (flags) {
				theHold.Begin();
				CurveRestore *cr;
				theHold.Put(cr=(new CurveRestore(dlg)));
				}
			else theHold.Accept(_T("Modify Curve"));
			if (point==0) {
				om = m;
			} else {
				dlg->ip->RedrawViews(dlg->ip->GetTime());
				}
			break;

		case MOUSE_MOVE: {
			int width, height;
			IPoint2 delta = m-om;
			om = m;
			if (dlg->move==1) {
				delta.y = 0;
			} else if (dlg->move==2) {
				delta.x = 0;
				}
			dlg->ComputeZooms(dlg->hView,width,height);
			Point2 mv;
			mv.x = float(delta.x)/(float(width-GRID_WID)*dlg->xzoom);
			mv.y = 2.0f * float(-delta.y)/(float(height)*dlg->yzoom);
			dlg->MovePoints(mv);
			UpdateWindow(hWnd);
			dlg->ip->RedrawViews(dlg->ip->GetTime());
			break;		
			}

		case MOUSE_ABORT:
			theHold.Cancel();
			dlg->ip->RedrawViews(dlg->ip->GetTime());
			break;
		}
	return 1;
	}

int PanMode::proc(HWND hWnd, int msg, int point, int flags, IPoint2 m)
	{
	switch (msg) {
		case MOUSE_POINT:
			if (point==0) {
				om = m;
				oxscroll = dlg->xscroll;
				oyscroll = dlg->yscroll;
				}
			break;

		case MOUSE_MOVE: {
			IPoint2 delta = m-om;
			dlg->xscroll = oxscroll + float(delta.x);
			dlg->yscroll = oyscroll + float(delta.y);
			dlg->InvalidateView();
			SetCursor(GetPanCursor());
			break;
			}

		case MOUSE_ABORT:
			dlg->xscroll = oxscroll;
			dlg->yscroll = oyscroll;
			dlg->InvalidateView();
			break;

		case MOUSE_FREEMOVE:
			SetCursor(GetPanCursor());
			break;		
		}
	return 1;
	}

#define ZOOM_FACT	0.01f
int ZoomMode::proc(HWND hWnd, int msg, int point, int flags, IPoint2 m)
	{
	switch (msg) {
		case MOUSE_POINT:
			if (point==0) {
				om = m;
				oxzoom = dlg->xzoom;
				oyzoom = dlg->yzoom;
				oxscroll = dlg->xscroll;
				oyscroll = dlg->yscroll;
				}
			break;

		case MOUSE_MOVE: {
			IPoint2 delta = om-m;
			float z;
			switch (zoomtype) {
				case ZOOM_XY:
					if (delta.y<0)
						 z = (1.0f/(1.0f-ZOOM_FACT*delta.y));
					else z = (1.0f+ZOOM_FACT*delta.y);
					dlg->xzoom = oxzoom * z;
					dlg->yzoom = oyzoom * z;
					dlg->xscroll = oxscroll*z;
					dlg->yscroll = oyscroll*z;
					break;
				case ZOOM_X:
					if (delta.x<0)
						 z = (1.0f/(1.0f-ZOOM_FACT*delta.x));
					else z = (1.0f+ZOOM_FACT*delta.x);
					dlg->xzoom = oxzoom / z;
					dlg->xscroll = oxscroll/z;
					break;
				case ZOOM_Y:
					if (delta.y<0)
						 z = (1.0f/(1.0f-ZOOM_FACT*delta.y));
					else z = (1.0f+ZOOM_FACT*delta.y);
					dlg->yzoom = oyzoom * z;
					dlg->yscroll = oyscroll*z;
					break;
				}
			dlg->InvalidateView();
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			break;
			}

		case MOUSE_ABORT:
			dlg->xzoom = oxzoom;
			dlg->yzoom = oyzoom;
			dlg->xscroll = oxscroll;
			dlg->yscroll = oyscroll;
			dlg->InvalidateView();
			break;

		case MOUSE_FREEMOVE:
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			break;		
		}
	return 1;
	}


int RightMouseMode::proc(HWND hWnd, int msg, int point, int flags, IPoint2 m)
	{
	switch (msg) {
		case MOUSE_POINT:
		case MOUSE_PROPCLICK:
			dlg->SetMode(ID_TOOL_PAN);
			break;
		}
	return 1;
	}
