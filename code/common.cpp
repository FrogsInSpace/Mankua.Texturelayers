#include "max.h"

#ifndef MAX_RELEASE_R9
#include "max_mem.h"
#endif

#include "mapping.h"
#include "texlay_mc_data.h"
#include "common.h"
#include "uvwgroup.h"

MultiMapModData GetMultiMapMod( INode* node, TimeValue t ) {
	MultiMapModData mmmd;
	mmmd.mod = NULL;
	mmmd.mtd = NULL;

	Object* obj;
	if (!node)
		return mmmd;

	obj = node->GetObjectRef();
	if (!obj) 
		return mmmd;

	ObjectState os = node->EvalWorldState(t);
	if (os.obj && os.obj->SuperClassID() != GEOMOBJECT_CLASS_ID)
		return mmmd;

	// For all derived objects (can be > 1)
	while (obj && (obj->SuperClassID() == GEN_DERIVOB_CLASS_ID)) {
		IDerivedObject* dobj = (IDerivedObject*)obj;
		int m;
		int numMods = dobj->NumModifiers();
		// Step through all modififers and verify the class id
		for (m=0; m<numMods; m++) {
			Modifier* mmod = dobj->GetModifier(m);
			if ( mmod && mmod->ClassID() == MULTIMAP_MOD_CID ) {
				mmmd.mod = (MultiMapMod*)mmod;
				mmmd.mtd = (TexLayMCData*)dobj->GetModContext(m)->localData;
				return mmmd;
				}
			}
		obj = dobj->GetObjRef();
		}

	return mmmd;
	}

void FindNodeByMtl( ReferenceTarget * node_mtl, Mtl * tl_mtl, BOOL &found ) {
	if ( found )
		return;
	if ( node_mtl == tl_mtl ) {
		found = TRUE;
		}
	else {
		int num_sub_refs = node_mtl->NumRefs();
		for ( int i_sr=0; i_sr<num_sub_refs; i_sr++ ) {
			ReferenceTarget * ref = node_mtl->GetReference(i_sr);
			if ( ref ) {
				FindNodeByMtl((Mtl*)ref,tl_mtl,found);
				}
			}
		}
	}

MultiMapMod * FindModifierInScene( INode * node, Mtl *mtl ) {
	if ( node->GetMtl() ) {
		BOOL found = FALSE;
		FindNodeByMtl( (ReferenceTarget*)node->GetMtl(), mtl, found );
		if ( found ) {
			MultiMapModData mmmd = GetMultiMapMod( node, 0 );
			if ( mmmd.mod ) {
				return mmmd.mod;
				}
			}
		}
	
	int num_children = node->NumberOfChildren();
	for ( int i_ch=0; i_ch<num_children; i_ch++ ) {
		INode * child_node = node->GetChildNode(i_ch);
		return FindModifierInScene(child_node,(Mtl*)mtl);
		}
	return NULL;
	}

MultiMapMod * GetModifiersMapChs(MtlBase *mtl,BitArray &mapreq, BitArray &bumpreq) {
	INode * root = GetCOREInterface()->GetRootNode();
	int topo_channel = 0;

	int num_children = root->NumberOfChildren();
	for ( int i_ch=0; i_ch<num_children; i_ch++ ) {
		INode * child_node = root->GetChildNode(i_ch);
		MultiMapMod * mod = FindModifierInScene(child_node,(Mtl*)mtl);
		if ( mod ) {
			for ( int i=0; i<mod->uvwProy.Count(); i++ ) {
				if ( mod->uvwProy[i]->GetMappingChannel() > topo_channel )
					topo_channel = mod->uvwProy[i]->GetMappingChannel();
				mapreq.Set( mod->uvwProy[i]->GetMappingChannel() );
				bumpreq.Set( mod->uvwProy[i]->GetMappingChannel() );
				}
			}
		}
	topo_channel++;
	mapreq.Set(topo_channel);
	bumpreq.Set(topo_channel);
	return NULL;
	}

//Win32 : static BOOL CALLBACK AboutDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) 
static INT_PTR CALLBACK AboutDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) 
{
	switch (msg) {
		case WM_INITDIALOG: {
			CenterWindow(hWnd,GetParent(hWnd));
			break;
			}

		case WM_CLOSE:
		case WM_COMMAND:
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
			EndDialog(hWnd,1);
			break;

			break;

		default:
			return FALSE;
		}
	return TRUE;
	}

void DisplayTexLayAbout() 
{
	DialogBoxParam(	hInstance, MAKEINTRESOURCE(IDD_ABOUT_DLG), GetCOREInterface()->GetMAXHWnd(), AboutDlgProc, 0);
}
