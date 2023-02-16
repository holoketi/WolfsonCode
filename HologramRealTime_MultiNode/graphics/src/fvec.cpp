#include "graphics/fvec.h"
#include "graphics/solve.h"
#include "graphics/sys.h"

namespace graphics {


const int fvec2::n = 2;

bool fvec2::unit() 
{ 
    real_t val = norm(*this);

    if (val < epsilon) return false; 
    (*this) = (*this)/val; 
    return true; 
}

real_t fvec2::length() const 
{ 
    return norm(*this); 
}

bool fvec2::decompose( // Computes a, b such that this vector = a*X + b*Y
       //
       // If X,Y is known to be an orthonormal frame,
       // then a = V*X, b = V*Y will compute
       // the same result more quickly.
       const fvec2& X,
       const fvec2& Y,
       real_t& a,
       real_t& b
       ) const
{
  int rank;
  real pivot_ratio = 0.0;
  real_t XoY = inner(X, Y);
  real aa = a;
  real bb = b;
  rank = Solve2x2( inner(X,X), XoY, inner(Y,Y), XoY,
                    inner((*this),X), inner((*this),Y), 
                    &aa, &bb, &pivot_ratio );
  a = aa;
  b = bb;
  return (rank == 2) ? true : false;
}
 
int fvec2::is_parallel( 
      // returns  1: this and other vectors are and parallel
      //         -1: this and other vectors are anti-parallel
      //          0: this and other vectors are not parallel
      //             or at least one of the vectors is zero
      const fvec2& vv,
      real_t angle_tolerance // (default=ON_DEFAULT_ANGLE_TOLERANCE) radians
      ) const
{
  int rc = 0;
  const real_t ll = norm(*this) * norm(vv);
  if ( ll > 0.0 ) {
    const real_t cos_angle = (inner(*this, vv))/ll;
    const real_t cos_tol = cos(angle_tolerance);
    if ( cos_angle >= cos_tol )
      rc = 1;
    else if ( cos_angle <= -cos_tol )
      rc = -1;
  }
  return rc;
}

bool fvec2::is_perpendicular(
      // returns true:  this and other vectors are perpendicular
      //         false: this and other vectors are not perpendicular
      //                or at least one of the vectors is zero
      const fvec2& vv,
      real_t angle_tolerance // (default=ON_DEFAULT_ANGLE_TOLERANCE) radians
      ) const
{
  bool rc = false;
  const real_t ll = norm(*this)*norm(vv);
  if ( ll > 0.0 ) {
    if ( fabs(inner(*this, vv)/ll) <= sin(angle_tolerance) )
      rc = true;
  }
  return rc;
}

// set this vector to be perpendicular to another vector
bool fvec2::perpendicular( // Result is not unitized. 
			    // returns false if input vector is zero
      const fvec2& vv
      )
{
  v[1] = vv[0];
  v[0] = -vv[1];
  return (v[0] != 0.0 || v[1] != 0.0) ? true : false;
}

// set this vector to be perpendicular to a line defined by 2 points
bool fvec2::perpendicular( 
      const fvec2& p, 
      const fvec2& q
      )
{
  return perpendicular(q-p);
}

//| I/O
void print(const fvec2& a)
{
    LOG("(%g", a[0]);
    for(int i = 1; i < 2;++i){
	LOG(" %g", a[i]);
    }
    LOG(") ");
}

void store(FILE* fp, const fvec2& v)
{
    fprintf(fp, "(%lg", v[0]);
    for(int i = 1; i < 2;++i){
	fprintf(fp, " %lg", v[i]);
    }
    fprintf(fp, ")\n");
}

int scan(FILE* fp, const fvec2& v)
{
    int a = fscanf(fp, " (");
    for(int i = 0; i < 2;++i){
	a += fscanf(fp, " %lg", &v[i]);
    }
    a += fscanf(fp, " )");
    return a;
}

int apx_equal(const fvec2& a, const fvec2& b)
{
    int c = 1;

    for (int i = 0 ; i < 2 ;++i){
	c = c && apx_equal((double)a[i], (double)b[i]);
    }

    return c;
}

int apx_equal(const fvec2& a, const fvec2& b, real_t eps)
{
    int c = 1;

    for (int i = 0 ; i < 2 ;++i){
	c = c && apx_equal((double)a[i], (double)b[i], (double)eps);
    }

    return c;
}




const int fvec3::n = 3;

bool fvec3::unit() 
{ 
    real_t val = norm(*this);

    if (val < epsilon) return false; 
    (*this) = (*this)/val; 
    return true; 
}

real_t fvec3::length() const 
{ 
    return norm(*this); 
}

bool fvec3::decompose( // Computes a, b, c such that this vector = a*X + b*Y + c*Z
       //
       // If X,Y,Z is known to be an orthonormal frame,
       // then a = V*X, b = V*Y, c = V*Z will compute
       // the same result more quickly.
       const fvec3& X,
       const fvec3& Y,
       const fvec3& Z,
       real_t& a,
       real_t& b,
       real_t& c
       ) const
{
  int rank;
  double pivot_ratio = 0.0;
  double row0[3], row1[3], row2[3];
  row0[0] = inner(X, X);   row0[1] = inner(X, Y);   row0[2] = inner(X, Z);
  row1[0] = row0[1]; row1[1] = inner(Y, Y);   row1[2] = inner(Y, Z);
  row2[0] = row0[2]; row2[1] = row1[2]; row2[2] = inner(Z, Z);
  double aa = a;
  double bb = b;
  double cc = c;
  rank = Solve3x3( row0, row1, row2, 
                    inner((*this), X), inner((*this), Y), inner((*this), Z),
                    &aa, &bb, &cc, &pivot_ratio );
  a = aa;
  b = bb;
  c = cc;
  return (rank == 3) ? true : false;
}

int fvec3::is_parallel( 
      // returns  1: this and other vectors are and parallel
      //         -1: this and other vectors are anti-parallel
      //          0: this and other vectors are not parallel
      //             or at least one of the vectors is zero
      const fvec3& vv,
      real_t angle_tolerance // (default=ON_DEFAULT_ANGLE_TOLERANCE) radians
      ) const
{
  int rc = 0;
  const real_t ll = norm(*this) * norm(vv);
  if ( ll > 0.0 ) {
    const real_t cos_angle = (inner(*this, vv))/ll;
    const real_t cos_tol = cos(angle_tolerance);
    if ( cos_angle >= cos_tol )
      rc = 1;
    else if ( cos_angle <= -cos_tol )
      rc = -1;
  }
  return rc;
}

bool fvec3::is_perpendicular(
      // returns true:  this and other vectors are perpendicular
      //         false: this and other vectors are not perpendicular
      //                or at least one of the vectors is zero
      const fvec3& vv,
      real_t angle_tolerance // (default=ON_DEFAULT_ANGLE_TOLERANCE) radians
      ) const
{
    bool rc = false;
    const real_t ll = norm(*this) * norm(vv);
    if ( ll > 0.0 ) {
	if ( fabs(inner(graphics::unit(*this), graphics::unit(vv))/ll) <= sin(angle_tolerance) )
	    rc = true;
    }
    return rc;
}


bool fvec3::perpendicular( const fvec3& vv )
{
  //bool rc = false;
    int i, j, k; 
    real_t a, b;
    k = 2;
    if ( fabs(vv[1]) > fabs(vv[0]) ) {
	if ( fabs(vv[2]) > fabs(vv[1]) ) {
	    i = 2;
	    j = 1;
	    k = 0;
	    a = vv[2];
	    b = -vv[1];
	}
	else if ( fabs(vv[2]) >= fabs(vv[0]) ){
	    i = 1;
	    j = 2;
	    k = 0;
	    a = vv[1];
	    b = -vv[2];
	}
	else {
	    // |vv[1]| > |vv[0]| > |vv[2]|
	    i = 1;
	    j = 0;
	    k = 2;
	    a = vv[1];
	    b = -vv[0];
	}
    }
    else if ( fabs(vv[2]) > fabs(vv[0]) ) {
	// |vv[2]| > |vv[0]| >= |vv[1]|
	i = 2;
	j = 0;
	k = 1;
	a = vv[2];
	b = -vv[0];
    }
    else if ( fabs(vv[2]) > fabs(vv[1]) ) {
	// |vv[0]| >= |vv[2]| > |vv[1]|
	i = 0;
	j = 2;
	k = 1;
	a = vv[0];
	b = -vv[2];
    }
    else {
	// |vv[0]| >= |vv[1]| >= |vv[2]|
	i = 0;
	j = 1;
	k = 2;
	a = vv[0];
	b = -vv[1];
    }

    v[i] = b;
    v[j] = a;
    v[k] = 0.0;
    return (a != 0.0) ? true : false;
}

bool
fvec3::perpendicular( 
      const fvec3& P0, const fvec3& P1, const fvec3& P2
      )
{
    // Find a the unit normal to a triangle defined by 3 points
    fvec3 V0, V1, V2, N0, N1, N2;

    v[0] = v[1] = v[2] = 0.0;

    V0 = P2 - P1;
    V1 = P0 - P2;
    V2 = P1 - P0;

    N0 = cross( V1, V2 );

    if (!N0.unit())
	return false;

    N1 = cross( V2, V0 );

    if (!N1.unit())
	return false;

    N2 = cross( V0, V1 );

    if (!N2.unit())
	return false;

    const real_t s0 = 1.0/V0.length();
    const real_t s1 = 1.0/V1.length();
    const real_t s2 = 1.0/V2.length();

    // choose normal with smallest total error
    const real_t e0 = s0*fabs(inner(N0,V0)) + s1*fabs(inner(N0,V1)) + s2*fabs(inner(N0,V2));
    const real_t e1 = s0*fabs(inner(N1,V0)) + s1*fabs(inner(N1,V1)) + s2*fabs(inner(N1,V2));
    const real_t e2 = s0*fabs(inner(N2,V0)) + s1*fabs(inner(N2,V1)) + s2*fabs(inner(N2,V2));

    if ( e0 <= e1 ) {
	if ( e0 <= e2 ) {
	  *this = N0;
	}
	else {
	  *this = N2;
	}
    }
    else if (e1 <= e2) {
	*this = N1;
    }
    else {
	*this = N2;
    }

    return true;
}
 

//| I/O
void print(const fvec3& a)
{
    LOG("(%f", a[0]);
    for(int i = 1; i < 3;++i){
	LOG(" %f", a[i]);
    }
    LOG(") ");
}

void store(FILE* fp, const fvec3& v)
{
    fprintf(fp, "(%lg", v[0]);
    for(int i = 1; i < 3;++i){
	fprintf(fp, " %lg", v[i]);
    }
    fprintf(fp, ")\n");
}

int scan(FILE* fp, const fvec3& v)
{
    int a = fscanf(fp, " (");
    for(int i = 0; i < 3;++i){
	a += fscanf(fp, " %lg", &v[i]);
    }
    a += fscanf(fp, " )");
    return a;
}

int apx_equal(const fvec3& a, const fvec3& b)
{
    int c = 1;

    for (int i = 0 ; i < 3 ;++i){
	c = c && apx_equal(a[i], b[i]);
    }

    return c;
}

int apx_equal(const fvec3& a, const fvec3& b, real_t eps)
{
    int c = 1;

    for (int i = 0 ; i < 3 ;++i){
	c = c && apx_equal((double)a[i], (double)b[i], (double)eps);
    }

    return c;
}



const int fvec4::n = 4;

bool fvec4::unit() 
{ 
    real_t val = norm(*this);

    if (val < epsilon) return false; 
    (*this) = (*this)/val; 
    return true; 
}

real_t fvec4::length() const 
{ 
    return norm(*this); 
}
 

//| I/O
void print(const fvec4& a)
{
    LOG("(%g", a[0]);
    for(int i = 1; i < 4;++i){
	LOG(" %g", a[i]);
    }
    LOG(") ");
}


void store(FILE* fp, const fvec4& v)
{
    fprintf(fp, "(%lg", v[0]);
    for(int i = 1; i < 4;++i){
	fprintf(fp, " %lg", v[i]);
    }
    fprintf(fp, ")\n");
}

int scan(FILE* fp, const fvec4& v)
{
    int a = fscanf(fp, " (");
    for(int i = 0; i < 4;++i){
	a += fscanf(fp, " %lg", &v[i]);
    }
    a += fscanf(fp, " )");
    return a;
}

int apx_equal(const fvec4& a, const fvec4& b)
{
    int c = 1;

    for (int i = 0 ; i < 4 ;++i){
	c = c && apx_equal(a[i], b[i]);
    }

    return c;
}

int apx_equal(const fvec4& a, const fvec4& b, real_t eps)
{
    int c = 1;

    for (int i = 0 ; i < 4 ;++i){
	c = c && apx_equal((double)a[i], (double)b[i], (double)eps);
    }

    return c;
}

fvec3 cross(const fvec3& a, const fvec3& b)
{
    fvec3 c;
    
    c(0) = a(0 + 1) * b(0 + 2) - a(0 + 2) * b(0 + 1);
    
    c(1) = a(1 + 1) * b(1 + 2) - a(1 + 2) * b(1 + 1);
    
    c(2) = a(2 + 1) * b(2 + 2) - a(2 + 2) * b(2 + 1);
    

    return c;
}

}; // namespace graphics