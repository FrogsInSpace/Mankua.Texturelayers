#ifndef GEOM_3D_
#define GEOM_3D_

#define BIGFLOAT	(float(999999.9e30))

// Builds a Matrix3 from a point-normal plane
Matrix3 PlaneTM( Point3 point, Point3 normal );

// Computes the plane equation from a point in the plane and
//  the normal of that plane. The normal must be sent normalized
Point4 ComputePlaneEq( Point3 normal, Point3 point );

Point4 ComputePlaneEq( Point3 a, Point3 b, Point3 c );

// Computes the intesrsection point of a line and a plane
BOOL PlaneLineIntersection( Point4 plane, Point3 from_pt, Point3 to_pt, Point3 &int_pt );

float DistancePointToPlane( Point4 plane, Point3 pt );
float SignDistancePointToPlane( Point4 plane, Point3 pt );

BOOL TwoPlanesLineDirection( Point4 plane_a, Point4 plane_b, Point3 &line_dir );

float RelationInSegment(Point3 point, Point3 start, Point3 end);

#endif