#include "max.h" 

#ifndef MAX_RELEASE_R9
#include "max_mem.h"
#endif

#include "meshdata.h"

//--------------------------------------------------------------
// MeshExtraData
//--------------------------------------------------------------
// Toca poner un constructor y un destructor para que borre
// con delete los bitarrays

MeshExtraData::MeshExtraData()
{ 
	topo_channel = 1;
	numVerts = numFaces = numuvVerts = 0;
	rVerts	= NULL; 
	faces	= NULL; 
	tfaces  = NULL;
	uvVerts = NULL;
	faceSel.SetSize(0); 
	noneSel = TRUE;
}

MeshExtraData::~MeshExtraData()
{
	if (rVerts) {
		//delete [] rVerts;
		delete rVerts;
		rVerts = NULL;
		}
	if (faces) {
		delete [] faces;
		faces = NULL;
		}
	if (tfaces) {
		delete [] tfaces;
		tfaces = NULL;
		}
	if (uvVerts) {
		delete [] uvVerts;
		uvVerts = NULL;
		}
	numuvVerts = 0;
	numVerts = 0;
	numFaces = 0;
	faceSel.SetSize(0);
	delete this;
	}

void MeshExtraData::DeleteThis()
{
	if (rVerts) {
		delete [] rVerts;
		rVerts = NULL;
		}
	if (faces) {
		delete [] faces;
		faces = NULL;
		}
	if (tfaces) {
		delete [] tfaces;
		tfaces = NULL;
		}
	if (uvVerts) {
		delete [] uvVerts;
		uvVerts = NULL;
		}
	numuvVerts = 0;
	numVerts = 0;
	numFaces = 0;
	faceSel.SetSize(0);
	}

BOOL MeshExtraData::GetFaceSelection(int face) {
	if (noneSel) return TRUE;
	if (face > faceSel.GetSize()) return FALSE;
	return faceSel[face];
	}

void MeshExtraData::SetNumRVerts(int num) {
	if (rVerts) {
		delete [] rVerts;
		rVerts = NULL;
		}

	rVerts = new Point3[num];
	numVerts = num;
	}

void MeshExtraData::SetNumUVerts(int num) {
	if (uvVerts) {
		delete [] uvVerts;
		uvVerts = NULL;
		}

	uvVerts = new Point3[num];
	numuvVerts = num;
	}

void MeshExtraData::SetNumFaces(int num) {
	if (tfaces) {
		delete [] tfaces;
		tfaces = NULL;
		}
	if (faces) {
		delete [] faces;
		faces = NULL;
		}

	faces = new IPoint3[num];
	tfaces = new IPoint3[num];
	numFaces = num;
	}

void MeshExtraData::BuildMeshExtraData( Mesh * mesh, int map_channel )
{
	// UVW Normals setup
	Tab <Point3> norv;

	int i_tf, i_tv;

	SetNumRVerts( mesh->numVerts );
	SetNumFaces( mesh->numFaces );
	SetNumUVerts( mesh->getNumMapVerts(map_channel) );

	faceSel.SetSize( mesh->numFaces );
	faceSel.ClearAll();

	norv.SetCount( mesh->numVerts );

	for (i_tv=0; i_tv<mesh->numVerts; i_tv++) 
	{
		norv[i_tv] = Point3(0,0,0);
	}

	UVVert *mv = mesh->mapVerts( map_channel );
	TVFace *tf = mesh->mapFaces( map_channel );

	for ( i_tf = 0; i_tf < mesh->numFaces; i_tf++ )
	{
		float a0, a1, a2, dp, lenA, lenB;

		Point3 v0 = mv[tf[i_tf].getTVert(0)];
		Point3 v1 = mv[tf[i_tf].getTVert(1)];
		Point3 v2 = mv[tf[i_tf].getTVert(2)];

		lenA = Length( v1 - v0 );
		lenB = Length( v2 - v0 );

		if ( lenA == 0.0f || lenB == 0.0f )
		{
			a0 = 0.0f;
		}
		else
		{
			dp = DotProd((v1-v0)/lenA,(v2-v0)/lenB);
			if (dp>1.0f) dp = 1.0f;
			if (dp<-1.0) dp = -1.0f;
			a0 = acos(dp);
		}

		lenA = Length( v0 - v1 );
		lenB = Length( v2 - v1 );

		if ( lenA == 0.0f || lenB == 0.0f )
		{
			a1 = 0.0f;
		}
		else
		{
			dp = DotProd((v0-v1)/lenA,(v2-v1)/lenB);
			if (dp>1.0f) dp = 1.0f;
			if (dp<-1.0) dp = -1.0f;
			a1 = acos(dp);
		}

		a2 = PI - ( a0 + a1 );

		Point3 vetnorm = (v1-v0)^(v2-v0);
		norv[mesh->faces[i_tf].getVert(0)] += vetnorm * a0;
		norv[mesh->faces[i_tf].getVert(1)] += vetnorm * a1;
		norv[mesh->faces[i_tf].getVert(2)] += vetnorm * a2;
		SetFace(i_tf, IPoint3(	int(mesh->faces[i_tf].getVert(0)),
								int(mesh->faces[i_tf].getVert(1)),
								int(mesh->faces[i_tf].getVert(2))) );
	}

	for (i_tv=0; i_tv<mesh->numVerts; i_tv++) 
	{
		Point3 vnormal = Normalize(norv[i_tv]);
		SetRvert(i_tv,vnormal);
	}	

	// Build Face Selections

	for ( i_tf=0; i_tf<mesh->numFaces; i_tf++ )
	{
		if ( mv[ tf[i_tf].t[0] ] == mv[ tf[i_tf].t[1] ] && mv[ tf[i_tf].t[1] ] == mv[ tf[i_tf].t[2] ] )
			faceSel.Clear(i_tf);
		else 
			faceSel.Set(i_tf);
	}

	if ( faceSel.NumberSet() == 0 ) 
		noneSel = TRUE;
	else
		noneSel = FALSE;
}
