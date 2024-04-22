#ifndef TL_MC_DATA_H
#define TL_MC_DATA_H

#include "mapping.h"

#define IS_MESH		1
#define IS_PATCH	2
#define IS_NURBS	3
#define IS_POLY		4

class TexLayMCData : public LocalModData
{
public:
	// These two variables are used to check if the incoming object
	//  has changed since the last time it was modified.
	float crc_geom;
	int crc_topo;

	Tab <PolyUVWData*> group_uvw_data;

	int topo_channel;

	int tipo;

	Mesh *mesh;
	PatchMesh *patch;
	MNMesh * mnMesh;

	BitArray none_sel;
	BitArray triInfo;

	GenericNamedSelSetList face_sel;
	GenericNamedSelSetList face_sel_sets;
	GenericNamedSelSetList edge_sel;
	GenericNamedSelSetList edge_sel_sets;

	TexLayMCData(Mesh &mesh, int numChannels, int num_faces, int num_edges);
	TexLayMCData(PatchMesh &patch, int numChannels, int num_faces, int num_edges);
	TexLayMCData(MNMesh &mnMesh, int numChannels, int num_faces, int num_edges);

	TexLayMCData() { 
		mesh=NULL; 
		mnMesh = NULL; 
		patch = NULL; 
		}
	~TexLayMCData() { FreeCache(); }
	LocalModData *Clone();

	Mesh *GetMesh() {return mesh;}
	MNMesh *GetMNMesh() {return mnMesh;}
	PatchMesh *GetPatch() {return patch;}

	void SetCache(Mesh &mesh);
	void SetCache(PatchMesh &patch);
	void SetCache(MNMesh &mnMesh);

	void FreeCache();

	int GetFaceSelSize(int channel) { 
		return face_sel[channel].GetSize(); }
	void SetNumGroups( int numChanns, int num_faces, int num_edges );
	};

#endif //TL_MC_DATA_H
