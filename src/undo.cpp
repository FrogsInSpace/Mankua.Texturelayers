#include "mods.h"		
#include "texutil.h"
#include "bmmlib.h"
#include "simpobj.h"
#include "simpmod.h"
#include "decomp.h"
#include "mapping.h"
#include "uvwgroup.h"
#include "uv_pelt_dlg.h"
#include "undo.h"

PeltPointsPosRestore::PeltPointsPosRestore(UVWProyector *u, MultiMapMod *m) {
	uvp = u;
	mod = m;
	int np = uvp->frame_segments.Count();
	undo_points.SetCount( np );
	for ( int i=0; i<np; i++ )
		undo_points[i] = uvp->frame_segments[i]->start_point;
	}

PeltPointsPosRestore::~PeltPointsPosRestore() {
	undo_points.SetCount(0);
	redo_points.SetCount(0);
	}

void PeltPointsPosRestore::Restore(int isUndo) {
	int np = uvp->frame_segments.Count();
	if (isUndo) {
		redo_points.SetCount( np );
		for ( int i=0; i<np; i++ )
			redo_points[i] = uvp->frame_segments[i]->start_point;
		}
	for ( int i=0; i<np; i++ )
		uvp->frame_segments[i]->start_point = undo_points[i];

	if ( isUndo ) {
		uvp->LocalDataChanged();
		GetCOREInterface()->RedrawViews(GetCOREInterface()->GetTime());
		}
	}

void PeltPointsPosRestore::Redo() {
	int np = uvp->frame_segments.Count();
	for ( int i=0; i<np; i++ )
		uvp->frame_segments[i]->start_point = redo_points[i];

	uvp->LocalDataChanged();
	GetCOREInterface()->RedrawViews(GetCOREInterface()->GetTime());
	}

PeltFrameRestore::PeltFrameRestore(UVWProyector *u, MultiMapMod *m) {
	uvp = u;
	mod = m;
	int num_frame_segments = uvp->frame_segments.Count();
	undo_frame_segments.SetCount( num_frame_segments );
	for ( int i_fs=0; i_fs<num_frame_segments; i_fs++ ) {
		int num_bv = uvp->frame_segments[i_fs]->border_vert.Count();
		undo_frame_segments[i_fs] = new FrameSegments(num_bv);
		undo_frame_segments[i_fs]->start_point = uvp->frame_segments[i_fs]->start_point;
		undo_frame_segments[i_fs]->selected = uvp->frame_segments[i_fs]->selected; 
		undo_frame_segments[i_fs]->segment_length = uvp->frame_segments[i_fs]->segment_length; 
		for ( int i_bv=0; i_bv<num_bv; i_bv++ ) {
			undo_frame_segments[i_fs]->border_vert[i_bv] = uvp->frame_segments[i_fs]->border_vert[i_bv];
			undo_frame_segments[i_fs]->position[i_bv] = uvp->frame_segments[i_fs]->position[i_bv];
			}
		}
	}

PeltFrameRestore::~PeltFrameRestore() {
	int i_fs;

	for ( i_fs=0; i_fs<undo_frame_segments.Count(); i_fs++ ) {
		undo_frame_segments[i_fs]->border_vert.SetCount(0);
		undo_frame_segments[i_fs]->position.SetCount(0);
		delete undo_frame_segments[i_fs];
		undo_frame_segments[i_fs] = NULL;
		}
	undo_frame_segments.SetCount(0);

	for ( i_fs=0; i_fs<redo_frame_segments.Count(); i_fs++ ) {
		redo_frame_segments[i_fs]->border_vert.SetCount(0);
		redo_frame_segments[i_fs]->position.SetCount(0);
		delete redo_frame_segments[i_fs];
		redo_frame_segments[i_fs] = NULL;
		}
	redo_frame_segments.SetCount(0);
	}

void PeltFrameRestore::Restore(int isUndo) {
	int i_fs;

	if ( isUndo ) {
		int num_frame_segments = uvp->frame_segments.Count();
		redo_frame_segments.SetCount( num_frame_segments );
		for ( i_fs=0; i_fs<num_frame_segments; i_fs++ ) {
			int num_bv = uvp->frame_segments[i_fs]->border_vert.Count();
			redo_frame_segments[i_fs] = new FrameSegments(num_bv);
			redo_frame_segments[i_fs]->start_point = uvp->frame_segments[i_fs]->start_point;
			redo_frame_segments[i_fs]->selected = uvp->frame_segments[i_fs]->selected; 
			redo_frame_segments[i_fs]->segment_length = uvp->frame_segments[i_fs]->segment_length; 
			for ( int i_bv=0; i_bv<num_bv; i_bv++ ) {
				redo_frame_segments[i_fs]->border_vert[i_bv] = uvp->frame_segments[i_fs]->border_vert[i_bv];
				redo_frame_segments[i_fs]->position[i_bv] = uvp->frame_segments[i_fs]->position[i_bv];
				}
			}
		}

	for ( i_fs=0; i_fs<uvp->frame_segments.Count(); i_fs++ ) {
		uvp->frame_segments[i_fs]->border_vert.SetCount(0);
		uvp->frame_segments[i_fs]->position.SetCount(0);
		delete uvp->frame_segments[i_fs];
		uvp->frame_segments[i_fs] = NULL;
		}
	uvp->frame_segments.SetCount(0);

	int num_frame_segments = undo_frame_segments.Count();
	uvp->frame_segments.SetCount( num_frame_segments );
	for ( i_fs=0; i_fs<num_frame_segments; i_fs++ ) {
		int num_bv = undo_frame_segments[i_fs]->border_vert.Count();
		uvp->frame_segments[i_fs] = new FrameSegments(num_bv);
		uvp->frame_segments[i_fs]->start_point = undo_frame_segments[i_fs]->start_point;
		uvp->frame_segments[i_fs]->selected = undo_frame_segments[i_fs]->selected; 
		uvp->frame_segments[i_fs]->segment_length = undo_frame_segments[i_fs]->segment_length; 
		for ( int i_bv=0; i_bv<num_bv; i_bv++ ) {
			uvp->frame_segments[i_fs]->border_vert[i_bv] = undo_frame_segments[i_fs]->border_vert[i_bv];
			uvp->frame_segments[i_fs]->position[i_bv] = undo_frame_segments[i_fs]->position[i_bv];
			}
		}

	if ( isUndo ) {
		uvp->LocalDataChanged();
		GetCOREInterface()->RedrawViews(GetCOREInterface()->GetTime());
		}
	}

void PeltFrameRestore::Redo() {
	int i_fs;

	for ( i_fs=0; i_fs<uvp->frame_segments.Count(); i_fs++ ) {
		uvp->frame_segments[i_fs]->border_vert.SetCount(0);
		uvp->frame_segments[i_fs]->position.SetCount(0);
		delete uvp->frame_segments[i_fs];
		uvp->frame_segments[i_fs] = NULL;
		}
	uvp->frame_segments.SetCount(0);

	int num_frame_segments = redo_frame_segments.Count();
	uvp->frame_segments.SetCount( num_frame_segments );
	for ( i_fs=0; i_fs<num_frame_segments; i_fs++ ) {
		int num_bv = redo_frame_segments[i_fs]->border_vert.Count();
		uvp->frame_segments[i_fs] = new FrameSegments(num_bv);
		uvp->frame_segments[i_fs]->start_point = redo_frame_segments[i_fs]->start_point;
		uvp->frame_segments[i_fs]->selected = redo_frame_segments[i_fs]->selected; 
		uvp->frame_segments[i_fs]->segment_length = redo_frame_segments[i_fs]->segment_length; 
		for ( int i_bv=0; i_bv<num_bv; i_bv++ ) {
			uvp->frame_segments[i_fs]->border_vert[i_bv] = redo_frame_segments[i_fs]->border_vert[i_bv];
			uvp->frame_segments[i_fs]->position[i_bv] = redo_frame_segments[i_fs]->position[i_bv];
			}
		}

	uvp->LocalDataChanged();
	GetCOREInterface()->RedrawViews(GetCOREInterface()->GetTime());
	}

// Simple Restore
MRSRestore::MRSRestore(MultiMapMod *m) {
	mod = m;
	current_group = m->current_channel;
	}

void MRSRestore::Restore(int isUndo) {
	mod->uvwProy[current_group]->valid_group = 0;
	}

void MRSRestore::Redo() {
	mod->uvwProy[current_group]->valid_group = 0;
	}

// MoveSelectionRestore
MoveSelectionRestore::MoveSelectionRestore( TexLayMCData *d, int f, int t ) {
	md = d;
	from = f;
	to = t;
	}
void MoveSelectionRestore::Restore(int isUndo) {
	if ( from<to ) {
		BitArray * face_sel = new BitArray(md->face_sel[to-1]);
		BitArray * edge_sel = new BitArray(md->edge_sel[to-1]);
		PolyUVWData * temp_uvw_data = md->group_uvw_data[to-1];
		md->face_sel.sets.Insert(from,1,&face_sel);
		md->edge_sel.sets.Insert(from,1,&edge_sel);
		md->group_uvw_data.Insert(from,1,&temp_uvw_data);
		md->face_sel.sets.Delete(to,1);
		md->edge_sel.sets.Delete(to,1);
		md->group_uvw_data.Delete(to,1);
		}
	if ( from>to ) {
		BitArray * face_sel = new BitArray(md->face_sel[to]);
		BitArray * edge_sel = new BitArray(md->edge_sel[to]);
		PolyUVWData * temp_uvw_data = md->group_uvw_data[to];
		md->face_sel.sets.Insert(from+1,1,&face_sel);
		md->edge_sel.sets.Insert(from+1,1,&edge_sel);
		md->group_uvw_data.Insert(from+1,1,&temp_uvw_data);
		md->face_sel.sets.Delete(to,1);
		md->edge_sel.sets.Delete(to,1);
		md->group_uvw_data.Delete(to,1);
		}
	}
void MoveSelectionRestore::Redo() {
	if ( from<to ) {
		BitArray * face_sel = new BitArray(md->face_sel[from]);
		BitArray * edge_sel = new BitArray(md->edge_sel[from]);
		PolyUVWData * temp_uvw_data = md->group_uvw_data[from];
		md->face_sel.sets.Insert(to,1,&face_sel);
		md->edge_sel.sets.Insert(to,1,&edge_sel);
		md->group_uvw_data.Insert(to,1,&temp_uvw_data);
		md->face_sel.sets.Delete(from,1);
		md->edge_sel.sets.Delete(from,1);
		md->group_uvw_data.Delete(from,1);
		}
	if ( from>to ) {
		BitArray * face_sel = new BitArray(md->face_sel[from]);
		BitArray * edge_sel = new BitArray(md->edge_sel[from]);
		PolyUVWData * temp_uvw_data = md->group_uvw_data[from];
		md->face_sel.sets.Insert(to,1,&face_sel);
		md->edge_sel.sets.Insert(to,1,&edge_sel);
		md->group_uvw_data.Insert(to,1,&temp_uvw_data);
		md->face_sel.sets.Delete(from+1,1);
		md->edge_sel.sets.Delete(from+1,1);
		md->group_uvw_data.Delete(from+1,1);
		}
	}


// DeleteSelectionRestore
DeleteSelectionRestore::DeleteSelectionRestore( TexLayMCData *d, int g ) {
	md = d;
	group = g;
	undo_face_sel = md->face_sel[group];
	undo_edge_sel = md->edge_sel[group];
	undo_uvw_data.CopyUVWData( d->group_uvw_data[g] );
	}

void DeleteSelectionRestore::Restore(int isUndo) {
	md->face_sel.InsertSet(group,undo_face_sel);
	md->edge_sel.InsertSet(group,undo_edge_sel);
	PolyUVWData * uvw_data = new PolyUVWData;
	uvw_data->CopyUVWData( &undo_uvw_data );
	md->group_uvw_data.Insert(group,1,&uvw_data);
	}

void DeleteSelectionRestore::Redo() {
	md->face_sel.DeleteSet(group);
	md->edge_sel.DeleteSet(group);
	delete md->group_uvw_data[group];
	md->group_uvw_data[group] = NULL;
	md->group_uvw_data.Delete(group,1);
	}
