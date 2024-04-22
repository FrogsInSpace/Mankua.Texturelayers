/**********************************************************************
 *<
	FILE: mods.cpp

	DESCRIPTION:   DLL implementation of modifiers

	CREATED BY: Rolf Berteig (based on prim.cpp)

	HISTORY: created 30 January 1995

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#include "mods.h"
#include "buildver.h"
#include "3dsmaxport.h"

HINSTANCE hInstance;
int controlsInit = FALSE;


/** public functions **/
BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved) {

#ifndef MAX_RELEASE_R9
	hInstance = hinstDLL;

	if ( !controlsInit ) {
		controlsInit = TRUE;
		InitCustomControls(hInstance);
		InitCommonControls();
		}

	switch(fdwReason) {
		case DLL_PROCESS_ATTACH:
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
	}

#else

	if( fdwReason == DLL_PROCESS_ATTACH )
	{
		hInstance = hinstDLL;
		DisableThreadLibraryCalls(hInstance);
	}

#endif

	return(TRUE);
}


//------------------------------------------------------
// This is the interface to Jaguar:
//------------------------------------------------------

__declspec( dllexport ) const TCHAR *
LibDescription() { return
 GetString(IDS_DC_DEFTL); }

/// MUST CHANGE THIS NUMBER WHEN ADD NEW CLASS
__declspec( dllexport ) int LibNumberClasses()
{
	return 5;
}

__declspec( dllexport ) ClassDesc*
LibClassDesc(int i) {
	switch(i) {
		case 0: return GetUVWMultiMapModDesc();
		case 1: return GetTextureLayersDesc();
		case 2: return GetUVWProyClassDesc();
		case 3: return GetAttMapDesc();
		case 4: return GetFFGridClassDesc();
		default: return 0;
		}
	}

// Return version so can detect obsolete DLLs
__declspec( dllexport ) ULONG 
LibVersion() { return VERSION_3DSMAX; }

//Win32 : BOOL CALLBACK DefaultSOTProc( HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
BOOL CALLBACK DefaultSOTProc( HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	//Win32 : IObjParam *ip = (IObjParam*)GetWindowLong(hWnd,GWL_USERDATA);
	IObjParam *ip = DLGetWindowLongPtr<IObjParam*>(hWnd);

	switch (msg) {
		case WM_INITDIALOG:
			//Win32 : SetWindowLong(hWnd,GWL_USERDATA,lParam);
			DLSetWindowLongPtr(hWnd, lParam);
			break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			if (ip) ip->RollupMouseMessage(hWnd,msg,wParam,lParam);
			return FALSE;

		default:
			return FALSE;
		}
	return TRUE;
	}

TCHAR *GetString(int id)
	{
	static TCHAR buf[256];

	if (hInstance)
		return LoadString(hInstance, id, buf, sizeof(buf)) ? buf : NULL;
	return NULL;
	}
