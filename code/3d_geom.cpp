#include "max.h"
#include "3d_geom.h"
#include "debug.h"

Matrix3 PlaneTM( Point3 point, Point3 normal ) {
	Point3 aux_axis(0,0,1);
	if ( DotProd( normal,aux_axis ) > 0.8 )
		aux_axis = Point3(0,1,0);

	Point3 z_axis = normal;
	Point3 y_axis = Normalize( normal ^ aux_axis );
	Point3 x_axis = Normalize( y_axis ^ z_axis );
	
	y_axis = Normalize( z_axis ^ x_axis );

	Matrix3 tm( x_axis, y_axis, z_axis, point );

	return tm;
	}

Point4 ComputePlaneEq( Point3 normal, Point3 point ) {
	Point4 plane;
	plane.x = normal.x;
	plane.y = normal.y;
	plane.z = normal.z;
	plane.w = -DotProd(point,normal);
	return plane;
	}

Point4 ComputePlaneEq( Point3 a, Point3 b, Point3 c ) {
	Point3 normal;
	Point3 vec_0 = Normalize( b - a );
	Point3 vec_1 = Normalize( c - a );

	normal = Normalize( vec_0 ^ vec_1 );

	Point4 plane;
	plane.x = normal.x;
	plane.y = normal.y;
	plane.z = normal.z;
	plane.w = -DotProd(a,normal);
	return plane;
	}


BOOL PlaneLineIntersection( Point4 plane, Point3 from_pt, Point3 to_pt, Point3 &int_pt ) {
	Point3 dir = Normalize( to_pt - from_pt );
	Point3 normal = Point3( plane.x, plane.y, plane.z );

	float vd = DotProd( dir, normal );

	if ( fabs(vd) < 0.000001f )
		return FALSE;

	float vo = - ( DotProd( normal, from_pt ) + plane.w );
	float t = vo / vd;

	int_pt = from_pt + t * dir;

	return TRUE;
	}

float DistancePointToPlane( Point4 plane, Point3 pt ) {
	Point3 n(plane.x,plane.y,plane.z);

	float t = -( n.x*pt.x + n.y*pt.y + n.z*pt.z + plane.w );
	Point3 ps( pt.x+n.x*t , pt.y+n.y*t , pt.z+n.z*t );

	return Length(pt-ps);// * ts;
	}

float SignDistancePointToPlane( Point4 plane, Point3 pt ) {
	Point3 n(plane.x,plane.y,plane.z);

	float t = -( n.x*pt.x + n.y*pt.y + n.z*pt.z + plane.w );
	Point3 ps( pt.x+n.x*t , pt.y+n.y*t , pt.z+n.z*t );

	float len = Length(pt-ps);
	if ( t<0.0f )
		len = -len;

	return len;
	}

BOOL TwoPlanesLineDirection( Point4 plane_a, Point4 plane_b, Point3 &line_dir ) {
	Point3 normal_a( plane_a.x, plane_a.y, plane_a.z );
	Point3 normal_b( plane_b.x, plane_b.y, plane_b.z );

	if ( DotProd( normal_a, normal_b ) > 0.999999f )
		return FALSE;
	
	line_dir = Normalize( normal_a ^ normal_b );
	return TRUE;
	}

float RelationInSegment(Point3 point, Point3 start, Point3 end) {
	Point3 vec = end-start;

	int max_comp = 0;
	if ( fabs(vec.y) > fabs(vec[max_comp]) )
		max_comp = 1;
	if ( fabs(vec.z) > fabs(vec[max_comp]) )
		max_comp = 2;

	return ( point[max_comp] - start[max_comp] ) / ( end[max_comp] - start[max_comp] );
	}

