#include "max.h"

#ifndef MAX_RELEASE_R9
#include "max_mem.h"
#endif

#include "3d_geom.h"
#include "natural_box.h"

bool PointInBox( Point2 pt, Box2D box ) {
	if ( pt.x >= box.min.x && pt.y >= box.min.y && pt.x <= box.max.x && pt.y <= box.max.y )
		return true;
	return false;
	}

void SolveCubic(double a,double b,double c,double d,int*solutions,double *x) {
    long double a1 = b/a, a2 = c/a, a3 = d/a;
    long double Q = (a1*a1 - 3.0*a2)/9.0;
    long double R = (2.0*a1*a1*a1 - 9.0*a1*a2 + 27.0*a3)/54.0;
    double R2_Q3 = R*R - Q*Q*Q;
    double theta;

    if (R2_Q3 <= 0) {
        *solutions = 3;
        theta = acos(R/sqrt(Q*Q*Q));
        x[0] = -2.0*sqrt(Q)*cos(theta/3.0) - a1/3.0;
        x[1] = -2.0*sqrt(Q)*cos((theta+2.0*PI)/3.0) - a1/3.0;
        x[2] = -2.0*sqrt(Q)*cos((theta+4.0*PI)/3.0) - a1/3.0;
		}
	else {
        *solutions = 1;
        x[0] = pow( double(sqrt(R2_Q3)+fabs(R)), double(1.0f/3.0f));
        x[0] += Q/x[0];
        x[0] *= (R < 0.0) ? 1 : -1;
        x[0] -= a1/3.0;
		x[1] = 0.0;
		x[2] = 0.0;
		}
	}

/**********************************************************************
  routine to tri-diagonalize a real symmetric matrix
                uses Householder's method
**********************************************************************/

/*
tri_diagonalize( Cxd, d, e, A, L, tol)
Cxd - an LxL sized matrix containing the symmetric matrix to be analyzed,
      such as a covariance matrix (in C, allocate one longer, i.e., alloc length LxL+1 )
d - a length L vector that passes results to the next routine
     (in C, allocate one longer, i.e., alloc length L+1 ); needs no initialization
e - a length L vector that passes results to the next routine
     (in C, allocate one longer, i.e., alloc length L+1 ); needs no initialization
A - an LxL matrix that holds the tri-diagonalized version of Cxd upon return;
    needed to pass to the next routine (in C, allocate one longer, i.e., alloc length LxL+1 );
    needs no initialization
tol - tolerance for checking nearness to zero;
     I found 1.0e-6 to be sufficient for my applications but you may need a smaller value.
*/

void tri_diagonalize(double *Cxd, double *d,double *e, double *A, int L, double tol)
{
	int i, j, k, l;
	double f, g, h, hh;
	for (i = 0; i < L; i++) {
		for (j = 0; j <= i; j++) {
			A[i*L + j] = Cxd[i*L + j];
		}
	}
	for (i = L - 1; i > 0; i--) {
		l = i - 2;
		f = A[i*L + i - 1];
		g = 0.0;
		for (k = 0; k <= l; k++) {
			g += A[i*L + k]*A[i*L + k];
		}
		h = g + f*f;
		if (g <= tol) {
			e[i] = f;
			h = 0.0;
			d[i] = h;
			continue;
		}
		l++;
		g = sqrt(h);
		if (f >= 0.0) g = -g;
		e[i] = g;
		h = h - f*g;
		A[i*L + i - 1] = f - g;
		f = 0.0;
		for (j = 0; j <= l; j++) {
			A[j*L + i] = A[i*L + j]/h;
			g = 0.0;
			for (k = 0; k <= j; k++) {
				g += A[j*L + k]*A[i*L + k];
			}
			for (k = j + 1; k <= l; k++) {
				g += A[k*L + j]*A[i*L + k];				
			}
			e[j] = g/h;
			f += g*A[j*L + i];
		}
		hh = f/(h + h);
		for (j = 0; j <= l; j++) {
			f = A[i*L + j];
			g = e[j] - hh*f;
			e[j] = g;
			for (k = 0; k <= j; k++) {
				A[j*L + k] = A[j*L + k] - f*e[k] - g*A[i*L + k];
			}
		}
		d[i] = h;
	}
	d[0] = e[0] = 0.0;
	for (i = 0; i < L; i++) {
		l = i - 1;
		if (d[i] != 0.0) {
			for (j = 0; j <= l; j++) {
				g = 0.0;
				for (k = 0; k <= l; k++) {
					g += A[i*L + k]*A[k*L + j];
				}
				for (k = 0; k <= l; k++) {
					A[k*L + j] = A[k*L + j] - g*A[k*L + i];
				}
			}
		}
		d[i] = A[i*L + i];
		A[i*L + i] = 1.0;
		for (j = 0; j <= l; j++) {
			A[i*L + j] = A[j*L + i] = 0.0;
		}
	}
}

/**********************************************************************
    routine to find eigenstructure of real tri-diagonal matrix
			 uses QL algorithm
          returns  0: sucess      -1: failure to converge
**********************************************************************/
/*calc_eigenstructure( d, e, A, L, macheps )
d - vector as above; on return it holds the eigenvalues in sorted order (smallest to largest)
e - vector as above; used to pass info into this routine from the previous one
A - matrix as above;
   on return it holds the array of eigenvectors as columns in order
   corresponding to the eigenvalues in d
macheps - an iteration error tolerance parameter;
          I found 1.0e-16 to work well in my applications; you may have to adjust
          this if you have convergence problems.
 */

int calc_eigenstructure(double *d, double *e, double *A, int L, double macheps)
{
	int i, j, k, l, m;
	double b, c, f, g, h, p, r, s;

	for (i = 1; i < L; i++) e[i - 1] = e[i];
	e[L - 1] = b = f = 0.0;
	for (l = 0; l < L; l++) {
		h = macheps*(fabs(d[l]) + fabs(e[l]));
		if (b < h) b = h;
		for (m = l; m < L; m++) {
			if (fabs(e[m]) <= b) break;
		}
		j = 0;
		if (m != l) do {
			if (j++ == 30) return -1;
			p = (d[l + 1] - d[l])/(2.0*e[l]);
			r = sqrt(p*p + 1);
			h = d[l] - e[l]/(p + (p < 0.0 ? -r : r));
			for (i = l; i < L; i++) d[i] = d[i] - h;
			f += h;
			p = d[m];
			c = 1.0;
			s = 0.0;
			for (i = m - 1; i >= l; i--) {
				g = c*e[i];
				h = c*p;
				if (fabs(p) >= fabs(e[i])) {
					c = e[i]/p;
					r = sqrt(c*c + 1);
					e[i + 1] = s*p*r;
					s = c/r;
					c = 1.0/r;
				} else {
					c = p/e[i];
					r = sqrt(c*c + 1);
					e[i + 1] = s*e[i]*r;
					s = 1.0/r;
					c = c/r;
				}
				p = c*d[i] - s*g;
				d[i + 1] = h + s*(c*g + s*d[i]);
				for (k = 0; k < L; k++) {
					h = A[k*L + i + 1];
					A[k*L + i + 1] = s*A[k*L + i] + c*h;
					A[k*L + i] = c*A[k*L + i] - s*h;
				}
			}
			e[l] = s*p;
			d[l] = c*p;
		} while (fabs(e[l]) > b);
		d[l] = d[l] + f;
	}

/* order the eigenvectors  */
	for (i = 0; i < L; i++) {
		k = i;
		p = d[i];
		for (j = i + 1; j < L; j++) {
			if (d[j] < p) {
				k = j;
				p = d[j];
			}
		}
		if (k != i) {
			d[k] = d[i];
			d[i] = p;
			for (j = 0; j < L; j++) {
				p = A[j*L + i];
				A[j*L + i] = A[j*L + k];
				A[j*L + k] = p;
			}
		}
	}
	return 0;
}

// http://www.nauticom.net/www/jdtaft/CEigenBetter.htm
BOOL GetNaturalBoundingBox( Tab <Point3> &points, Matrix3 &natural_axis, Box3 &natural_box ) 
{
	int i_p;

	Point3 center(0,0,0);
	for ( i_p=0; i_p<points.Count(); i_p++ ) {
		center = center + points[i_p];
		}
	center = center / float( points.Count() );

	double c11 = 0.0f;
	double c22 = 0.0f;
	double c33 = 0.0f;
	double c12 = 0.0f;
	double c13 = 0.0f;
	double c23 = 0.0f;

	for ( i_p=0; i_p<points.Count(); i_p++ ) {
		Point3 p = points[i_p];
		c11 = c11 + ( p.x - center.x ) * ( p.x - center.x );
		c22 = c22 + ( p.y - center.y ) * ( p.y - center.y );
		c33 = c33 + ( p.z - center.z ) * ( p.z - center.z );
		c12 = c12 + ( p.x - center.x ) * ( p.y - center.y );
		c13 = c13 + ( p.x - center.x ) * ( p.z - center.z );
		c23 = c23 + ( p.y - center.y ) * ( p.z - center.z );
		}

	c11 = c11 / float( points.Count() );
	c22 = c22 / float( points.Count() );
	c33 = c33 / float( points.Count() );
	c12 = c12 / float( points.Count() );
	c13 = c13 / float( points.Count() );
	c23 = c23 / float( points.Count() );

	int L = 3;
	double * cxd;
	cxd = (double*)malloc( (L*L+1) * sizeof(double) );
	double * d;
	d = (double*)malloc( (L+1) * sizeof(double) );
	double * e;
	e = (double*)malloc( (L+1) * sizeof(double) );
	double * A;
	A = (double*)malloc( (L*L+1) * sizeof(double) );
	double tol = 1.0e-6;
	double macheps = 1.0e-16;

	cxd[0] = c11;
	cxd[1] = c12;
	cxd[2] = c13;
	cxd[3] = c12;
	cxd[4] = c22;
	cxd[5] = c23;
	cxd[6] = c13;
	cxd[7] = c23;
	cxd[8] = c33;
	cxd[9] = 0;

	tri_diagonalize( cxd, d, e, A, L, tol);
	calc_eigenstructure( d,  e,  A, L, macheps );
	
	natural_axis.SetRow( 0, Point3( A[0],  A[3],  A[6]  ) );
	natural_axis.SetRow( 1, Point3( A[1],  A[4],  A[7]  ) );
	natural_axis.SetRow( 2, Point3( A[2],  A[5],  A[8]  ) );
	natural_axis.SetRow( 3, center );

	BitArray pairs(points.Count());
	pairs.ClearAll();
	for ( i_p=0; i_p<points.Count(); i_p++ ) {
		for ( int j_p=i_p+1; j_p<points.Count(); j_p++ ) {
			if ( points[i_p] == points[j_p] ) {
				pairs.Set(i_p);
				pairs.Set(j_p);
				}
			}
		}

	Point3 sel_edges_center(0,0,0);


	natural_box.Init();
	Point3 symmetry(0,0,0);
	for ( i_p=0; i_p<points.Count(); i_p++ ) {
		Point3 p = points[i_p] * Inverse(natural_axis);
		natural_box += p;
		symmetry += p;
		if ( pairs[i_p] ) {
			sel_edges_center += points[i_p];
			}
		}
	if ( pairs.NumberSet() )
		sel_edges_center = sel_edges_center/float( pairs.NumberSet() );

	Point3 centers_line = Normalize( center - sel_edges_center );

	float dp;
	float min_dp = fabs( DotProd( centers_line, natural_axis.GetRow(0) ) );
	int low_sym = 0;
	dp = fabs( DotProd( centers_line, natural_axis.GetRow(1) ) );
	if ( dp < min_dp ) {
		low_sym = 1;
		min_dp = dp;
		}
	dp = fabs( DotProd( centers_line, natural_axis.GetRow(2) ) );
	if ( dp < min_dp )
		low_sym = 2;

	if ( pairs.NumberSet() == 0 ) {
		Point3 box_width = natural_box.Width();
		if ( box_width.x<0.000001f )
			symmetry.x = BIGFLOAT;
		if ( box_width.y<0.000001f )
			symmetry.y = BIGFLOAT;
		if ( box_width.z<0.000001f )
			symmetry.z = BIGFLOAT;
		low_sym = fabs(symmetry[1])<fabs(symmetry[0])?1:0;
		if ( fabs(symmetry[2])<fabs(symmetry[low_sym]) )
			low_sym = 2;
		}
 
	int sym_axis_u = (low_sym+1)%3;
	int sym_axis_v = (low_sym+2)%3;

	Point3 box_width = natural_box.Width();

	int id_axis_x = low_sym;
	int id_axis_y = fabs(box_width[sym_axis_u])>fabs(box_width[sym_axis_v])?sym_axis_u:sym_axis_v;

	Point3 axis_x = natural_axis.GetRow( id_axis_x );
	Point3 axis_y = natural_axis.GetRow( id_axis_y );
	Point3 axis_z = Normalize( axis_x ^ axis_y );

	natural_axis.SetRow( 0, axis_x );
	natural_axis.SetRow( 1, axis_y );
	natural_axis.SetRow( 2, axis_z );
	natural_axis.SetRow( 3, center );

	natural_box.Init();
	for ( i_p=0; i_p<points.Count(); i_p++ ) {
		Point3 p = points[i_p] * Inverse(natural_axis);
		natural_box += p;
		}

	return TRUE;

/*
	Point3 box_width = natural_box.Width();
	int first_longest_axis = 0;
	if ( box_width.y > box_width.x )
		first_longest_axis = 1;
	if ( box_width.z > box_width[first_longest_axis] )
		first_longest_axis = 2;
	box_width[first_longest_axis] = 0.0f;
	int second_longest_axis = 0;
	if ( box_width.y > box_width.x )
		second_longest_axis = 1;
	if ( box_width.z > box_width[second_longest_axis] )
		second_longest_axis = 2;
	int third_longest_axis = 0;
	if ( ( first_longest_axis + second_longest_axis ) == 2 )
		third_longest_axis = 1;
	if ( ( first_longest_axis + second_longest_axis ) == 1 )
		third_longest_axis = 2;

	Point3 axis_x = natural_axis.GetRow(second_longest_axis);
	Point3 axis_y = natural_axis.GetRow(first_longest_axis);
	Point3 axis_z = Normalize( axis_x ^ axis_y );

	natural_axis.SetRow( 0, axis_x );
	natural_axis.SetRow( 1, axis_y );
	natural_axis.SetRow( 2, axis_z );
	natural_axis.SetRow( 3, natural_box_center );*/


	return TRUE;
	}
