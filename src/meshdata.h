#ifndef __MESHDATA__
#define __MESHDATA__

class MeshExtraData 
{
public:
	int tipo;
	int topo_channel;

	BOOL noneSel;
	BitArray faceSel;
	TimeValue time;

	int numVerts;
	int numFaces;
	int numuvVerts;
	Point3 * rVerts;
	Point3 * uvVerts;
	IPoint3 * tfaces;
	IPoint3 * faces;

	MeshExtraData();
	~MeshExtraData();

	BOOL GetFaceSelection(int face);

	void DeleteThis();

	void SetFace(int i, const IPoint3 &verts) {faces[i] = verts;}
	void SetRvert(int i, const Point3 &xyz) {rVerts[i] = xyz;}
	void SetTFace( int i, const IPoint3 &xyz) {tfaces[i] = xyz;}
	void SetVert(int i, const Point3 &xyz) {uvVerts[i] = xyz;}
	Point3 & GetRvert(int i) {return rVerts[i];}
	Point3 & GetVert(int i) {return uvVerts[i];}
	IPoint3 & GetTFace(int i) {return tfaces[i];}
	IPoint3 & GetFace(int i) {return faces[i];}

	void SetNumRVerts(int num);
	void SetNumFaces(int num);
	void SetNumUVerts(int num);

	void BuildMeshExtraData( Mesh *mesh, int map_channel );
};

#endif
