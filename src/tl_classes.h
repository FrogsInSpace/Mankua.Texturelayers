#ifndef __TL_CLASSES_H__
#define __TL_CLASSES_H__

#include "max.h"

class Vector {
	public:
		Point3 vector;
		Point3 inVector;
		Point3 outVector;

		void SetVector(Point3 v)	{ vector = v;}
		void SetInVector(Point3 v)	{ inVector = v;}
		void SetOutVector(Point3 v)	{ outVector = v;}
		Point3 GetVector()			{ return vector;}
		Point3 GetInVector()		{ return inVector;}
		Point3 GetOutVector()		{ return outVector;}
	};

class PolyFace {
public:
	int deg;
	int * vtx;

	PolyFace() {
		deg = 0;
		vtx = NULL;
		}
	};

class PeltPolyMesh {
public:
	int numf;
	int numv;
	Point3 * v;
	PolyFace * f;

	PeltPolyMesh( int num_verts, int num_faces ) {
		numv = num_verts;
		v = new Point3[num_verts];
		numf = num_faces;
		f = new PolyFace[num_faces];
		}
	void DeleteThis() {
		delete [] v;
		for ( int i_f=0; i_f<numf; i_f++ ) {
			delete [] f[i_f].vtx;
			f[i_f].deg = 0;
			}
		delete [] f;
		numv = 0;
		numf = 0;
		}
	};

class Spring {
public:
	bool attached_to_mass;		// Is it atached to a max, or a frame point?
	int attach_id;				// The id of the attach, either mass or frame

	float o_lenght;				// Original Length of the spring
	};

class Mass {
public:
	int vert_id;				// The vertex id in poly mesh of this mass

	Point3 o_pos;				// The positions and velocities, one original and one final
	Point3 f_pos;				//  one final to always compute a iteration with 
	Point3 vel;					//  original pos and vel, and set to finals. 

	Tab <Spring> springs;		// The springs that go out from this mass
	};

class FrameSegments {
public:
	bool selected;
	Point3 start_point;
	float segment_length;
	Tab <int> border_vert;
	Tab <float> position;

	FrameSegments( int num ) {
		selected = false;
		border_vert.SetCount( num );
		position.SetCount( num );
		}
	~FrameSegments() {
		border_vert.SetCount( 0 );
		position.SetCount( 0 );
		}
	};

class RemapDirUVWProy : public RemapDir {
	RefTargetHandle FindMapping(RefTargetHandle from) { return NULL;}
	RefTargetHandle CloneRef(RefTargetHandle oldTarg) { return NULL;}
	void AddEntry(RefTargetHandle hfrom, RefTargetHandle hto) {;}
	void PatchPointer(RefTargetHandle* patchThis, RefTargetHandle oldTarg) {;}
	void Backpatch() {;}
	void Clear() {;}
	void ClearBackpatch() {;}
	void DeleteThis() {;}
	void AddPostPatchProc(PostPatchProc* proc, bool toDelete) {;}
	};

#endif
