#ifndef NATURAL_BOX_H
#define NATURAL_BOX_H


#include "templt.h"


bool PointInBox( Point2 pt, Box2D box );
void SolveCubic(double a,double b,double c,double d,int*solutions,double *x);
void tri_diagonalize(double *Cxd, double *d,double *e, double *A, int L, double tol);
int calc_eigenstructure(double *d, double *e, double *A, int L, double macheps);
BOOL GetNaturalBoundingBox( Tab <Point3> &border_points, Matrix3 &natural_axis, Box3 &natural_box );

#endif