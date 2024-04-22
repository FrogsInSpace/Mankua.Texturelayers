#include "mods.h"		
#include "texutil.h"
#include "bmmlib.h"
#include "simpobj.h"
#include "simpmod.h"
#include "decomp.h"
#include "mapping.h"
#include "texlay_mc_data.h"
#include "buildver.h"
#include "surf_api.h"
#include "meshadj.h"
#include "io.h"
#include <fcntl.h>
#include "load_cb.h"
#include "natural_box.h"
#include "texlay.h"

LocalModData *TexLayMCData::Clone() {
	TexLayMCData *d = new TexLayMCData;

	if ( mesh != NULL )
		d->mesh = new Mesh(*mesh);
	if ( patch != NULL )
		d->patch = new PatchMesh(*patch);
	if ( mnMesh != NULL )
		d->mnMesh = new MNMesh(*mnMesh);

	int num_groups = group_uvw_data.Count();
	d->group_uvw_data.SetCount(num_groups);
	for ( int i_g=0; i_g<num_groups; i_g++ ) {
		d->group_uvw_data[i_g] = new PolyUVWData;

		d->group_uvw_data[i_g]->num_v = group_uvw_data[i_g]->num_v;
		d->group_uvw_data[i_g]->num_f = group_uvw_data[i_g]->num_f;
		d->group_uvw_data[i_g]->v = new Point3[d->group_uvw_data[i_g]->num_v];
		d->group_uvw_data[i_g]->f = new PolyFace[d->group_uvw_data[i_g]->num_f];

		for ( int i_v=0; i_v<d->group_uvw_data[i_g]->num_v; i_v++ )
			d->group_uvw_data[i_g]->v[i_v] = group_uvw_data[i_g]->v[i_v];

		for ( int i_f=0; i_f<d->group_uvw_data[i_g]->num_f; i_f++ ) {
			int deg = group_uvw_data[i_g]->f[i_f].deg;
			d->group_uvw_data[i_g]->f[i_f].deg = deg;
			d->group_uvw_data[i_g]->f[i_f].vtx = new int[deg];
			for ( int i_v=0; i_v<deg; i_v++ )
				d->group_uvw_data[i_g]->f[i_f].vtx[i_v] = group_uvw_data[i_g]->f[i_f].vtx[i_v];
			}
	
		}	

	d->triInfo = triInfo;
	d->face_sel = face_sel;
	d->face_sel_sets  = face_sel_sets;
	d->edge_sel = edge_sel;
	d->edge_sel_sets  = edge_sel_sets;

	return d;
	}

TexLayMCData::TexLayMCData(Mesh &mesh,int numChannels, int num_faces, int num_edges) {
	SetNumGroups(numChannels,num_faces,num_edges);
	this->mesh = new Mesh(mesh);
	this->patch = NULL;
	this->mnMesh = NULL;
	}

TexLayMCData::TexLayMCData(PatchMesh &patch, int numChannels, int num_faces, int num_edges) {
	SetNumGroups(numChannels,num_faces,num_edges);
	this->mesh = NULL;
	this->patch = new PatchMesh(patch);
	this->mnMesh = NULL;
	}

TexLayMCData::TexLayMCData(MNMesh &mnMesh, int numChannels, int num_faces, int num_edges) {
	SetNumGroups(numChannels,num_faces,num_edges);
	this->mesh = NULL;
	this->patch = NULL;
	this->mnMesh = new MNMesh(mnMesh);
	}

void TexLayMCData::SetCache(Mesh &mesh) {
	if (this->mesh) this->mesh->DeleteThis();
	this->mesh = new Mesh(mesh);

	int size = mesh.numFaces;
	face_sel.SetSize(size);
	size = 0;
	edge_sel.SetSize(size);
	}

void TexLayMCData::SetCache(MNMesh &mnMesh) {
	if (this->mnMesh) delete this->mnMesh;
	this->mnMesh = new MNMesh(mnMesh);

	int size = mnMesh.numf;
	face_sel.SetSize(size);
	size = mnMesh.nume;
	edge_sel.SetSize(size);
	}

void TexLayMCData::SetCache(PatchMesh &patch) {
	if (this->patch) delete this->patch;
	this->patch = new PatchMesh(patch);

	int size = patch.numPatches;
	face_sel.SetSize(size);
	size = 0;
	edge_sel.SetSize(size);
	}

void TexLayMCData::FreeCache() {
	if (mesh) delete mesh;
	mesh = NULL;
	if (patch) delete patch;
	patch = NULL;
	if (mnMesh) delete mnMesh;
	mnMesh = NULL;

	face_sel.sets.SetCount(0);
	face_sel.ids.SetCount(0);
	face_sel.names.SetCount(0);

	edge_sel.sets.SetCount(0);
	edge_sel.ids.SetCount(0);
	edge_sel.names.SetCount(0);
	}

void TexLayMCData::SetNumGroups(int new_count, int num_faces, int num_edges) {
	int i;
	int sel_old_count = face_sel.sets.Count();

	if (sel_old_count == new_count) {
		if ( face_sel.sets.Count() != edge_sel.sets.Count() ) {
			int num_face_sets = face_sel.sets.Count();
			int num_edge_sets = edge_sel.sets.Count();

			if ( num_edge_sets>num_face_sets ) {
				edge_sel.names.SetCount(num_face_sets);
				edge_sel.sets.SetCount(num_face_sets);
				edge_sel.ids.SetCount(num_face_sets);
				}
			else {
				for ( int i=num_edge_sets; i<num_face_sets; i++ ) {
					BitArray set_edges(num_edges);
					edge_sel.AppendSet(set_edges,i);
					}
				}
			}
		}

	if ( new_count<sel_old_count ) {
		face_sel.names.SetCount(new_count);
		face_sel.sets.SetCount(new_count);
		face_sel.ids.SetCount(new_count);

		edge_sel.names.SetCount(new_count);
		edge_sel.sets.SetCount(new_count);
		edge_sel.ids.SetCount(new_count);
		}

	if ( new_count>sel_old_count) {
		for (i=sel_old_count; i<new_count; i++) {
			BitArray set_faces(num_faces);
			face_sel.AppendSet(set_faces,i);
		
			BitArray set_edges(num_edges);
			edge_sel.AppendSet(set_edges,i);
			}
		}

	int groups_old_count = group_uvw_data.Count();

	if ( new_count<groups_old_count ) {
		for (int i=new_count; i<groups_old_count; i++) {
			delete group_uvw_data[i];
			group_uvw_data[i] = NULL;
			}
		group_uvw_data.SetCount(new_count);
		}

	if ( new_count>groups_old_count) {
		group_uvw_data.SetCount(new_count);
		for (i=groups_old_count; i<new_count; i++) {
			group_uvw_data[i] = new PolyUVWData;
			}
		}

	}
