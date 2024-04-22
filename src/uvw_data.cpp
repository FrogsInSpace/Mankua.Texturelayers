#include "uvw_data.h"

PolyUVWData::PolyUVWData() {
	valid_group_key = 0;
	num_v = 0;
	num_f = 0;
	v = NULL;
	f = NULL;
	}

PolyUVWData::~PolyUVWData() {
	DeleteUVWData();
	}

void PolyUVWData::DeleteUVWData() {
	if ( v ) {
		delete [] v;
		v = NULL;
		}
	num_v = 0;

	if ( f ) {
		for ( int i=0; i<num_f; i++ ) {
			delete [] f[i].vtx;
			f[i].vtx = NULL;
			f[i].deg = 0;
			}
		delete [] f;
		f = NULL;
		}
	num_f = 0;
	}

void PolyUVWData::CopyUVWData( PolyUVWData *ori_uvw_data ) {
	DeleteUVWData(); 

	num_v = ori_uvw_data->num_v;
	num_f = ori_uvw_data->num_f;
	v = new Point3[num_v];
	f = new PolyFace[num_f];

	for ( int i_v=0; i_v<num_v; i_v++ )
		v[i_v] = ori_uvw_data->v[i_v];

	for ( int i_f=0; i_f<num_f; i_f++ ) {
		int deg = ori_uvw_data->f[i_f].deg;
		f[i_f].deg = deg;
		f[i_f].vtx = new int[deg];
		for ( int i_v=0; i_v<deg; i_v++ )
			f[i_f].vtx[i_v] = ori_uvw_data->f[i_f].vtx[i_v];
		}
	}

BOOL PolyUVWData::CompareUVWData( PolyUVWData *ori_uvw_data ) {
	return TRUE;
	}

BOOL PolyUVWData::CompareFaceSize( PolyUVWData *ori_uvw_data ) {
	if ( num_f != ori_uvw_data->num_f ) {
		return FALSE;
		}

	for ( int i_f=0; i_f<num_f; i_f++ ) {
		if ( f[i_f].deg != ori_uvw_data->f[i_f].deg ) {
			return FALSE;
			}
		}
	return TRUE;
	}
