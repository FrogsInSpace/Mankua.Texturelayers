#ifndef __UNDO_H__
#define __UNDO_H__

#include "iparamm2.h"
#include "istdplug.h"
#include "meshadj.h"
#include "modstack.h"
#include "macrorec.h"
#include "spline3d.h"
#include "shape.h"
#include "uvwgroup.h"
#include "texlay_mc_data.h"
#include "CCurve.h"
#include "debug.h"

#define START_EDIT		0
#define DELETE_GROUP	1
#define NEW_GROUP		2
#define MOVE_GROUP		3
#define END_EDIT		4
#define PASTE_GROUP		5
#define LOAD_GROUPS		6
#define INVALID_GROUP	7

// A restore object to save the position of control points.
class MoveRestore : public RestoreObj {
	public:		
		MultiMapMod *mod;
		Tab <Point3> undo;
		Tab <Point3> redo;
		int numVerts;
		MoveRestore(MultiMapMod *m) {
			mod = m;
			numVerts=mod->uvwProy[mod->current_channel]->gm->verts.Count();
			undo.SetCount(numVerts);
			redo.SetCount(numVerts);
			for (int i=0; i<numVerts; i++) {
				undo[i] = mod->uvwProy[mod->current_channel]->gm->verts[i];
				}
			}
		void Restore(int isUndo) {
			// if we're undoing, save a redo state
			if (isUndo) {
				for (int i=0; i<numVerts; i++) {
					redo[i] = mod->uvwProy[mod->current_channel]->gm->verts[i];
					}
				}
			for (int i=0; i<numVerts; i++) {
				mod->uvwProy[mod->current_channel]->gm->verts[i] = undo[i];
				}
			mod->uvwProy[mod->current_channel]->gm->CacheVNormals();
			mod->NotifyDependents(FOREVER,PART_GEOM,REFMSG_CHANGE);
			}
		void Redo() {
			for (int i=0; i<numVerts; i++) {
				mod->uvwProy[mod->current_channel]->gm->verts[i] = redo[i];
				}
			mod->uvwProy[mod->current_channel]->gm->CacheVNormals();
			mod->NotifyDependents(FOREVER,PART_GEOM,REFMSG_CHANGE);
			}
		void EndHold() {
			mod->ClearAFlag(A_HELD);
			}
	};

class ChannelRestore : public RestoreObj {
	public:
		MultiMapMod* mod;
		int ucanal;
		int rcanal;

		ChannelRestore(MultiMapMod *m) {
			mod = m;
			ucanal = mod->current_channel;
			}

		void Restore(int isUndo) {
			rcanal = mod->current_channel;
			mod->ChannelChange(ucanal);
			}

		void Redo() {
			mod->ChannelChange(rcanal);
			}
		TSTR Description() {return TSTR(_T("Cambio de current_channel"));}
	};

class GroupsTableRestore : public RestoreObj {
	public:
		MultiMapMod *mod;
		UVWProyector *u_proy;
		UVWProyector *r_proy;
		Tab <UVWProyector*> load_groups;
		int action;
		int group;
		int param1;
		int param2;
		
		GroupsTableRestore( MultiMapMod *m, int a, int p1=0, int p2=0 ) {
			mod = m;
			action = a;
			group = mod->current_channel;
			param1 = p1; 
			param2 = p2; 

			u_proy = NULL;
			r_proy = NULL;
			if ( action==PASTE_GROUP ) {
				u_proy = mod->uvwProy[group];
				}
			if ( action==LOAD_GROUPS) { 
				param1 = mod->uvwProy.Count();
				}
			}
		~GroupsTableRestore() {
			delete u_proy;
			delete r_proy;
			for ( int i_g=0; i_g<load_groups.Count(); i_g++ )
				delete load_groups[i_g];
			}
		void Restore(int isUndo) {
			if ( action==NEW_GROUP ) {
				int count = mod->uvwProy.Count();
				delete mod->uvwProy[count-1];
				mod->uvwProy.SetCount( count-1 );
				}
			if ( action==DELETE_GROUP ) {
				UVWProyector * uvw_group = NULL;
				mod->uvwProy.Insert(group,1,&uvw_group);
				}
			if ( action==MOVE_GROUP ) {
				int from = param1;
				int to = param2;
				if ( from<to ) {
					UVWProyector * uvw_group = mod->uvwProy[to-1];
					mod->uvwProy.Insert(from,1,&uvw_group);
					mod->uvwProy.Delete(to,1);
					}
				if ( from>to ) {
					UVWProyector * uvw_group = mod->uvwProy[to];
					mod->uvwProy.Insert(from+1,1,&uvw_group);
					mod->uvwProy.Delete(to,1);
					}
				}
			if ( action==PASTE_GROUP ) {
				r_proy = mod->uvwProy[group];
				mod->uvwProy[group] = u_proy;
				}
			if ( action==START_EDIT ) {
				mod->current_channel = group;
				mod->LocalDataChanged();
				mod->GroupsMenuUpdateList();
				mod->UpdateUIAll();
				}
			if ( action==LOAD_GROUPS ) {
				int tab_size = mod->uvwProy.Count() - param1;
				load_groups.SetCount(tab_size);
				for ( int i_g=0; i_g<load_groups.Count(); i_g++ ) 
					load_groups[i_g] = mod->uvwProy[ param1+i_g ];
				mod->uvwProy.SetCount(param1);
				}
			if ( action==INVALID_GROUP ) {
				mod->uvwProy[group]->valid_group = 0;
				mod->LocalDataChanged();
				}
			}
		void Redo() {
			if ( action==NEW_GROUP ) {
				int count = mod->uvwProy.Count();
				mod->uvwProy.SetCount( count+1 );
				UVWProyector * uvw_group = new UVWProyector(TRUE);
				uvw_group->descCanal.printf(_T("Group %d"),param1);
				mod->uvwProy[count] = uvw_group;
				mod->ReplaceReference( count, mod->uvwProy[count] );
				}
			if ( action==MOVE_GROUP ) {
				int from = param1;
				int to = param2;
				if ( from<to ) {
					UVWProyector * uvw_group = mod->uvwProy[from];
					mod->uvwProy.Insert(to,1,&uvw_group);
					mod->uvwProy.Delete(from,1);
					}
				if ( from>to ) {
					UVWProyector * uvw_group = mod->uvwProy[from];
					mod->uvwProy.Insert(to,1,&uvw_group);
					mod->uvwProy.Delete(from+1,1);
					}
				}
			if ( action==INVALID_GROUP ) {
				mod->uvwProy[group]->valid_group = 0;
				mod->LocalDataChanged();
				}
			if ( action==DELETE_GROUP ) {
				mod->uvwProy.Delete(group,1);
				mod->uvwProy.Shrink();
				}
			if ( action==PASTE_GROUP ) {
				mod->uvwProy[group] = r_proy;
				}
			if ( action==END_EDIT ) {
				mod->current_channel = group;
				mod->LocalDataChanged();
				mod->GroupsMenuUpdateList();
				mod->UpdateUIAll();
				}
			if ( action==LOAD_GROUPS ) {
				mod->uvwProy.SetCount( param1+load_groups.Count() );
				for ( int i_g=0; i_g<load_groups.Count(); i_g++ )
					mod->uvwProy[ param1+i_g ] = load_groups[i_g];
				}
			}

		TSTR Description() {return TSTR(_T("Groups Table Action"));}
	};

class DeleteSelectionRestore: public RestoreObj {
	public:
		TexLayMCData *md;
		int group;
		BitArray undo_face_sel;
		BitArray undo_edge_sel;
		PolyUVWData undo_uvw_data;

		DeleteSelectionRestore( TexLayMCData *d, int g );
		void Restore(int isUndo);
		void Redo();
		TSTR Description() {return TSTR(_T("Delete Selection"));}
	};

class MoveSelectionRestore: public RestoreObj {
	public:
		TexLayMCData *md;
		int from;
		int to;

		MoveSelectionRestore( TexLayMCData *d, int f, int t );
		void Restore(int isUndo);
		void Redo();
		TSTR Description() {return TSTR(_T("Move Selection"));}
	};

class PeltPointsPosRestore : public RestoreObj {
	public:
		UVWProyector *uvp;
		MultiMapMod *mod;
		Tab <Point3> undo_points;
		Tab <Point3> redo_points;

		PeltPointsPosRestore(UVWProyector *u, MultiMapMod *m);
		~PeltPointsPosRestore();
		void Restore(int isUndo);
		void Redo();

		void EndHold() {uvp->ClearAFlag(A_HELD);}
		TSTR Description() {return TSTR(_T(GetString(IDS_PELT_MOVEPOINTS)));}
	};

class PeltFrameRestore : public RestoreObj {
	public:
		UVWProyector *uvp;
		MultiMapMod *mod;
		Tab <FrameSegments*> undo_frame_segments;
		Tab <FrameSegments*> redo_frame_segments;

		PeltFrameRestore(UVWProyector *u, MultiMapMod *m);
		~PeltFrameRestore();
		void Restore(int isUndo);
		void Redo();

		void EndHold() {uvp->ClearAFlag(A_HELD);}
		TSTR Description() {return TSTR(_T(GetString(IDS_PELT_FRAME)));}
	};

class MRSRestore : public RestoreObj {
	public:
		int current_group;
		MultiMapMod *mod;

		MRSRestore(MultiMapMod *m);
		~MRSRestore() {;}
		void Restore(int isUndo);
		void Redo();

		void EndHold() {mod->ClearAFlag(A_HELD);}
		TSTR Description() {return TSTR(_T(GetString(IDS_PELT_FRAME)));}
	};

#endif