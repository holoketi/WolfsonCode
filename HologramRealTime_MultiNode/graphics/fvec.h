#ifndef __fvec_h
#define __fvec_h
// Description:
//  Mathematical tools to handle n-dimensional vectors
//
// Author:
//   Myung-Joon Kim
//   Dae-Hyun Kim



#include "graphics/real.h"
#include <stdio.h>
#include "graphics/ivec.h"
#include <math.h>
#include "graphics/epsilon.h"


namespace graphics {


struct fvec2 {
    real_t v[2];
    static const int n;

    inline fvec2() { }
    inline fvec2(real_t a)
    {
	 v[0] = a;  v[1] = a;  
    }

    inline fvec2(real_t v_1  , real_t v_2 )
    {
	 v[0] = v_1;  v[1] = v_2; 
    }

    inline fvec2(const ivec2& a)
    {
		v[0] = a[0];  v[1] = a[1];
    }

    inline fvec2(const fvec2& a)
    {
		v[0] = a[0];  v[1] = a[1];
    }

    inline fvec2& operator=(const fvec2& a)
    {
	 v[0] = a[0];  v[1] = a[1];  
	return *this;
    }

    inline real_t& operator[] (int i) { return v[i]; }
    inline const real_t&  operator[] (int i) const { return v[i]; }
    inline real_t& operator() (int i) { return v[i % 2]; }
    inline const real_t&  operator() (int i) const { return v[i % 2]; }

    bool unit(); 
    real_t length() const; 

    inline bool is_zero() const { return (v[0] == 0.0 && v[1] == 0.0); }
    inline bool is_tiny(real_t tiny_tol = epsilon) const {
	return (fabs(v[0]) <= tiny_tol && fabs(v[1]) <= tiny_tol);
    }

    //
    // returns  1: this and other vectors are parallel
    //         -1: this and other vectors are anti-parallel
    //          0: this and other vectors are not parallel
    //             or at least one of the vectors is zero
    int is_parallel( 
        const fvec2&,                 // other vector     
        real_t = angle_tolerance // optional angle tolerance (radians)
        ) const;

    //
    // Computes a, b such that this vector = a*X + b*Y
    // Returns false if unable to solve for a,b.  This happens
    // when X,Y is not real_tly a basis.
    //
    // If X,Y is known to be an orthonormal frame,
    // then a = V*X, b = V*Y will compute
    // the same result more quickly.
    bool decompose( 
         const fvec2&, // X
         const fvec2&, // Y
         real_t&, // a
         real_t&  // b
         ) const;

    //
    // returns true:  this and other vectors are perpendicular
    //         false: this and other vectors are not perpendicular
    //                or at least one of the vectors is zero
    bool is_perpendicular(
        const fvec2&,           // other vector     
        real_t = angle_tolerance // optional angle tolerance (radians)
        ) const;
 
    //
    // set this vector to be perpendicular to another vector
    bool perpendicular( // Result is not unitized. 
                         // returns false if input vector is zero
        const fvec2& 
        );

    //
    // set this vector to be perpendicular to a line defined by 2 points
    bool perpendicular( 
        const fvec2&, 
        const fvec2& 
        );
};





//| binary op : componentwise


inline fvec2 operator + (const fvec2& a, const fvec2& b)
{
    fvec2 c;
    for(int i = 0; i < 2;++i) { c[i] = a[i] + b[i]; }
    return c;
}

inline fvec2 operator + (real_t a, const fvec2& b)
{
    fvec2 c;
    for(int i = 0; i < 2;++i) { c[i] = a + b[i]; }
    return c;
}

inline fvec2 operator + (const fvec2& a, real_t b)
{
    fvec2 c;
    for(int i = 0; i < 2;++i) { c[i] = a[i] + b; }
    return c;
}



inline fvec2 operator - (const fvec2& a, const fvec2& b)
{
    fvec2 c;
    for(int i = 0; i < 2;++i) { c[i] = a[i] - b[i]; }
    return c;
}

inline fvec2 operator - (real_t a, const fvec2& b)
{
    fvec2 c;
    for(int i = 0; i < 2;++i) { c[i] = a - b[i]; }
    return c;
}

inline fvec2 operator - (const fvec2& a, real_t b)
{
    fvec2 c;
    for(int i = 0; i < 2;++i) { c[i] = a[i] - b; }
    return c;
}



inline fvec2 operator * (const fvec2& a, const fvec2& b)
{
    fvec2 c;
    for(int i = 0; i < 2;++i) { c[i] = a[i] * b[i]; }
    return c;
}

inline fvec2 operator * (real_t a, const fvec2& b)
{
    fvec2 c;
    for(int i = 0; i < 2;++i) { c[i] = a * b[i]; }
    return c;
}

inline fvec2 operator * (const fvec2& a, real_t b)
{
    fvec2 c;
    for(int i = 0; i < 2;++i) { c[i] = a[i] * b; }
    return c;
}



inline fvec2 operator / (const fvec2& a, const fvec2& b)
{
    fvec2 c;
    for(int i = 0; i < 2;++i) { c[i] = a[i] / b[i]; }
    return c;
}

inline fvec2 operator / (real_t a, const fvec2& b)
{
    fvec2 c;
    for(int i = 0; i < 2;++i) { c[i] = a / b[i]; }
    return c;
}

inline fvec2 operator / (const fvec2& a, real_t b)
{
    fvec2 c;
    for(int i = 0; i < 2;++i) { c[i] = a[i] / b; }
    return c;
}



//| cumulative op : componentwise


inline fvec2 operator += (fvec2& a, const fvec2& b)
{
    return a = (a + b);
}

inline fvec2 operator += (fvec2& a, real_t b)
{
    return a = (a + b);
}



inline fvec2 operator -= (fvec2& a, const fvec2& b)
{
    return a = (a - b);
}

inline fvec2 operator -= (fvec2& a, real_t b)
{
    return a = (a - b);
}



inline fvec2 operator *= (fvec2& a, const fvec2& b)
{
    return a = (a * b);
}

inline fvec2 operator *= (fvec2& a, real_t b)
{
    return a = (a * b);
}



inline fvec2 operator /= (fvec2& a, const fvec2& b)
{
    return a = (a / b);
}

inline fvec2 operator /= (fvec2& a, real_t b)
{
    return a = (a / b);
}



//| logical op : componentwise


inline int operator == (const fvec2& a, const fvec2& b)
{
    int c = 1;
    for(int i = 0; i < 2;++i) { c = c && a[i] == b[i]; }
    return c;
}

inline int operator == (real_t a, const fvec2& b)
{
    int c = 1;
    for(int i = 0; i < 2;++i) { c = c && a == b[i]; }
    return c;
}

inline int operator == (const fvec2& a, real_t b)
{
    int c = 1;
    for(int i = 0; i < 2;++i) { c = c && a[i] == b; }
    return c;
}



inline int operator < (const fvec2& a, const fvec2& b)
{
    int c = 1;
    for(int i = 0; i < 2;++i) { c = c && a[i] < b[i]; }
    return c;
}

inline int operator < (real_t a, const fvec2& b)
{
    int c = 1;
    for(int i = 0; i < 2;++i) { c = c && a < b[i]; }
    return c;
}

inline int operator < (const fvec2& a, real_t b)
{
    int c = 1;
    for(int i = 0; i < 2;++i) { c = c && a[i] < b; }
    return c;
}



inline int operator <= (const fvec2& a, const fvec2& b)
{
    int c = 1;
    for(int i = 0; i < 2;++i) { c = c && a[i] <= b[i]; }
    return c;
}

inline int operator <= (real_t a, const fvec2& b)
{
    int c = 1;
    for(int i = 0; i < 2;++i) { c = c && a <= b[i]; }
    return c;
}

inline int operator <= (const fvec2& a, real_t b)
{
    int c = 1;
    for(int i = 0; i < 2;++i) { c = c && a[i] <= b; }
    return c;
}



inline int operator > (const fvec2& a, const fvec2& b)
{
    int c = 1;
    for(int i = 0; i < 2;++i) { c = c && a[i] > b[i]; }
    return c;
}

inline int operator > (real_t a, const fvec2& b)
{
    int c = 1;
    for(int i = 0; i < 2;++i) { c = c && a > b[i]; }
    return c;
}

inline int operator > (const fvec2& a, real_t b)
{
    int c = 1;
    for(int i = 0; i < 2;++i) { c = c && a[i] > b; }
    return c;
}



inline int operator >= (const fvec2& a, const fvec2& b)
{
    int c = 1;
    for(int i = 0; i < 2;++i) { c = c && a[i] >= b[i]; }
    return c;
}

inline int operator >= (real_t a, const fvec2& b)
{
    int c = 1;
    for(int i = 0; i < 2;++i) { c = c && a >= b[i]; }
    return c;
}

inline int operator >= (const fvec2& a, real_t b)
{
    int c = 1;
    for(int i = 0; i < 2;++i) { c = c && a[i] >= b; }
    return c;
}



//| unary op : componentwise
inline fvec2 operator - (const fvec2& a)
{
    fvec2 c;
    for(int i = 0; i < 2;++i) { c[i] = - a[i]; }
    return c;
}

//| R^n -> R
inline real_t sum(const fvec2& a)
{
    real_t s = 0;
    
    s += a[1 - 1];
    
    s += a[2 - 1];
    
    return s;
}

inline real_t inner(const fvec2& a, const fvec2& b)
{
    fvec2 tmp = a * b;
    return sum(tmp);
}

inline real_t norm(const fvec2& a)
{
    return sqrt(inner(a, a));
}

inline real_t squaredNorm(const fvec2& a) {
	return inner(a, a);
}

inline fvec2 unit(const fvec2& a)
{
    real_t n = norm(a);
    if(n < epsilon)
	return 0;
    else
	return a / n;
}

inline real_t angle(const fvec2& a, const fvec2& b)
{
    real_t ang = inner(unit(a), unit(b));
    if(ang > 1 - epsilon)
	return 0;
    else if(ang < -1 + epsilon)
	return M_PI;
    else
	return acos(ang);
}

inline fvec2 proj(const fvec2& axis, const fvec2& a)
{
    fvec2 u = unit(axis);
    return inner(a, u) * u;
}

inline fvec2 absolute(const fvec2& val)
{
    return fvec2(fabs(val[0]), fabs(val[1]));
}

//| I/O
void print(const fvec2& a);

void store(FILE* fp, const fvec2& v);

int scan(FILE* fp, const fvec2& v);

int apx_equal(const fvec2& a, const fvec2& b);
int apx_equal(const fvec2& a, const fvec2& b, real_t eps);


struct fvec3 {
    real_t v[3];
    static const int n;

    inline fvec3() { }
    inline fvec3(real_t a)
    {
	 v[0] = a;  v[1] = a;  v[2] = a;  
    }

    inline fvec3(real_t v_1  , real_t v_2  , real_t v_3 )
    {
	 v[0] = v_1;  v[1] = v_2;  v[2] = v_3; 
    }

    inline fvec3(const ivec3& a)
    {
	 v[0] = a[0];  v[1] = a[1];  v[2] = a[2]; 
    }

    inline fvec3(const fvec3& a)
    {
		v[0] = a[0];  v[1] = a[1];  v[2] = a[2];
    }
	inline fvec3(const fvec2& a)
	{
		v[0] = a[0];  v[1] = a[1];  v[2] = 0.0;
	}
    inline fvec3& operator=(const fvec3& a)
    {
		v[0] = a[0];  v[1] = a[1];  v[2] = a[2];
	return *this;
    }

    inline real_t& operator[] (int i) { return v[i]; }
    inline const real_t&  operator[] (int i) const { return v[i]; }
    inline real_t& operator() (int i) { return v[i % 3]; }
    inline const real_t&  operator() (int i) const { return v[i % 3]; }
 
    inline bool is_zero() const { return (v[0] == 0.0 && v[1] == 0.0 && v[2] == 0.0); }
    inline bool is_tiny(real_t tiny_tol = epsilon) const {
	return (fabs(v[0]) <= tiny_tol && fabs(v[1]) <= tiny_tol && fabs(v[2]) <= tiny_tol );
    }

    bool unit(); 
    real_t length() const; 

    //
    // Computes a, b, c such that this vector = a*X + b*Y + c*Z
    // Returns false if unable to solve for a,b,c.  This happens
    // when X,Y,Z is not real_tly a basis.
    //
    // If X,Y,Z is known to be an orthonormal frame,
    // then a = V*X, b = V*Y, c = V*Z will compute
    // the same result more quickly.
    bool decompose( 
         const fvec3&, // X
         const fvec3&, // Y
         const fvec3&, // Z
         real_t&, // a
         real_t&, // b
         real_t&  // c
         ) const;

    //
    // returns  1: this and other vectors are parallel
    //         -1: this and other vectors are anti-parallel
    //          0: this and other vectors are not parallel
    //             or at least one of the vectors is zero
    int is_parallel( 
        const fvec3&,                 // other vector     
        real_t = angle_tolerance // optional angle tolerance (radians)
        ) const;

    //
    // returns true:  this and other vectors are perpendicular
    //         false: this and other vectors are not perpendicular
    //                or at least one of the vectors is zero
    bool is_perpendicular(
        const fvec3&,                 // other vector     
        real_t = angle_tolerance // optional angle tolerance (radians)
        ) const;

    //
    // set this vector to be perpendicular to another vector
    bool perpendicular( // Result is not unitized. 
                        // returns false if input vector is zero
        const fvec3& 
        );

    //
    // set this vector to be perpendicular to a plane defined by 3 points
    // returns false if points are coincident or colinear
    bool perpendicular(
         const fvec3&, const fvec3&, const fvec3& 
         );
};





//| binary op : componentwise


inline fvec3 operator + (const fvec3& a, const fvec3& b)
{
    fvec3 c;
    for(int i = 0; i < 3;++i) { c[i] = a[i] + b[i]; }
    return c;
}

inline fvec3 operator + (real_t a, const fvec3& b)
{
    fvec3 c;
    for(int i = 0; i < 3;++i) { c[i] = a + b[i]; }
    return c;
}

inline fvec3 operator + (const fvec3& a, real_t b)
{
    fvec3 c;
    for(int i = 0; i < 3;++i) { c[i] = a[i] + b; }
    return c;
}



inline fvec3 operator - (const fvec3& a, const fvec3& b)
{
    fvec3 c;
    for(int i = 0; i < 3;++i) { c[i] = a[i] - b[i]; }
    return c;
}

inline fvec3 operator - (real_t a, const fvec3& b)
{
    fvec3 c;
    for(int i = 0; i < 3;++i) { c[i] = a - b[i]; }
    return c;
}

inline fvec3 operator - (const fvec3& a, real_t b)
{
    fvec3 c;
    for(int i = 0; i < 3;++i) { c[i] = a[i] - b; }
    return c;
}



inline fvec3 operator * (const fvec3& a, const fvec3& b)
{
    fvec3 c;
    for(int i = 0; i < 3;++i) { c[i] = a[i] * b[i]; }
    return c;
}

inline fvec3 operator * (real_t a, const fvec3& b)
{
    fvec3 c;
    for(int i = 0; i < 3;++i) { c[i] = a * b[i]; }
    return c;
}

inline fvec3 operator * (const fvec3& a, real_t b)
{
    fvec3 c;
    for(int i = 0; i < 3;++i) { c[i] = a[i] * b; }
    return c;
}



inline fvec3 operator / (const fvec3& a, const fvec3& b)
{
    fvec3 c;
    for(int i = 0; i < 3;++i) { c[i] = a[i] / b[i]; }
    return c;
}

inline fvec3 operator / (real_t a, const fvec3& b)
{
    fvec3 c;
    for(int i = 0; i < 3;++i) { c[i] = a / b[i]; }
    return c;
}

inline fvec3 operator / (const fvec3& a, real_t b)
{
    fvec3 c;
    for(int i = 0; i < 3;++i) { c[i] = a[i] / b; }
    return c;
}



//| cumulative op : componentwise


inline fvec3 operator += (fvec3& a, const fvec3& b)
{
    return a = (a + b);
}

inline fvec3 operator += (fvec3& a, real_t b)
{
    return a = (a + b);
}



inline fvec3 operator -= (fvec3& a, const fvec3& b)
{
    return a = (a - b);
}

inline fvec3 operator -= (fvec3& a, real_t b)
{
    return a = (a - b);
}



inline fvec3 operator *= (fvec3& a, const fvec3& b)
{
    return a = (a * b);
}

inline fvec3 operator *= (fvec3& a, real_t b)
{
    return a = (a * b);
}



inline fvec3 operator /= (fvec3& a, const fvec3& b)
{
    return a = (a / b);
}

inline fvec3 operator /= (fvec3& a, real_t b)
{
    return a = (a / b);
}



//| logical op : componentwise


inline int operator == (const fvec3& a, const fvec3& b)
{
    int c = 1;
    for(int i = 0; i < 3;++i) { c = c && a[i] == b[i]; }
    return c;
}

inline int operator == (real_t a, const fvec3& b)
{
    int c = 1;
    for(int i = 0; i < 3;++i) { c = c && a == b[i]; }
    return c;
}

inline int operator == (const fvec3& a, real_t b)
{
    int c = 1;
    for(int i = 0; i < 3;++i) { c = c && a[i] == b; }
    return c;
}



inline int operator < (const fvec3& a, const fvec3& b)
{
    int c = 1;
    for(int i = 0; i < 3;++i) { c = c && a[i] < b[i]; }
    return c;
}

inline int operator < (real_t a, const fvec3& b)
{
    int c = 1;
    for(int i = 0; i < 3;++i) { c = c && a < b[i]; }
    return c;
}

inline int operator < (const fvec3& a, real_t b)
{
    int c = 1;
    for(int i = 0; i < 3;++i) { c = c && a[i] < b; }
    return c;
}



inline int operator <= (const fvec3& a, const fvec3& b)
{
    int c = 1;
    for(int i = 0; i < 3;++i) { c = c && a[i] <= b[i]; }
    return c;
}

inline int operator <= (real_t a, const fvec3& b)
{
    int c = 1;
    for(int i = 0; i < 3;++i) { c = c && a <= b[i]; }
    return c;
}

inline int operator <= (const fvec3& a, real_t b)
{
    int c = 1;
    for(int i = 0; i < 3;++i) { c = c && a[i] <= b; }
    return c;
}



inline int operator > (const fvec3& a, const fvec3& b)
{
    int c = 1;
    for(int i = 0; i < 3;++i) { c = c && a[i] > b[i]; }
    return c;
}

inline int operator > (real_t a, const fvec3& b)
{
    int c = 1;
    for(int i = 0; i < 3;++i) { c = c && a > b[i]; }
    return c;
}

inline int operator > (const fvec3& a, real_t b)
{
    int c = 1;
    for(int i = 0; i < 3;++i) { c = c && a[i] > b; }
    return c;
}



inline int operator >= (const fvec3& a, const fvec3& b)
{
    int c = 1;
    for(int i = 0; i < 3;++i) { c = c && a[i] >= b[i]; }
    return c;
}

inline int operator >= (real_t a, const fvec3& b)
{
    int c = 1;
    for(int i = 0; i < 3;++i) { c = c && a >= b[i]; }
    return c;
}

inline int operator >= (const fvec3& a, real_t b)
{
    int c = 1;
    for(int i = 0; i < 3;++i) { c = c && a[i] >= b; }
    return c;
}



//| unary op : componentwise
inline fvec3 operator - (const fvec3& a)
{
    fvec3 c;
    for(int i = 0; i < 3;++i) { c[i] = - a[i]; }
    return c;
}

inline fvec3 absolute(const fvec3& val)
{
    return fvec3(fabs(val[0]), fabs(val[1]), fabs(val[2]));
}



//| R^n -> R
inline real_t sum(const fvec3& a)
{
    real_t s = 0;
    
    s += a[1 - 1];
    
    s += a[2 - 1];
    
    s += a[3 - 1];
    
    return s;
}

inline real_t inner(const fvec3& a, const fvec3& b)
{
    fvec3 tmp = a * b;
    return sum(tmp);
}

inline real_t squaredNorm(const fvec3& a) {
	return inner(a, a);
}

inline real_t norm(const fvec3& a)
{
    return sqrt(inner(a, a));
}

inline fvec3 unit(const fvec3& a)
{
    real_t n = norm(a);
    if(n < zero_epsilon)
	return 0;
    else
	return a / n;
}

inline real_t angle(const fvec3& a, const fvec3& b)
{
    real_t ang = inner(unit(a), unit(b));
    if(ang > 1 - epsilon)
	return 0;
    else if(ang < -1 + epsilon)
	return M_PI;
    else
	return acos(ang);
}

inline fvec3 proj(const fvec3& axis, const fvec3& a)
{
    fvec3 u = unit(axis);
    return inner(a, u) * u;
}

 

//| I/O
void print(const fvec3& a);

void store(FILE* fp, const fvec3& v);
int scan(FILE* fp, const fvec3& v);

int apx_equal(const fvec3& a, const fvec3& b);
int apx_equal(const fvec3& a, const fvec3& b, real_t eps);

struct fvec4 {
    real_t v[4];
    static const int n;

    inline fvec4() { }
    inline fvec4(real_t a)
    {
	 v[0] = a;  v[1] = a;  v[2] = a;  v[3] = a;  
    }

    inline fvec4(real_t v_1  , real_t v_2  , real_t v_3  , real_t v_4 )
    {
	 v[0] = v_1;  v[1] = v_2;  v[2] = v_3;  v[3] = v_4; 
    }

    inline fvec4(const ivec4& a)
    {
	 v[0] = a[0];  v[1] = a[1];  v[2] = a[2];  v[3] = a[3]; 
    }

    inline fvec4(const fvec4& a)
    {
		v[0] = a[0];  v[1] = a[1];  v[2] = a[2];  v[3] = a[3];
    }

    inline fvec4& operator=(const fvec4& a)
    {
	 v[1 - 1] = a[1 - 1];  v[2 - 1] = a[2 - 1];  v[3 - 1] = a[3 - 1];  v[4 - 1] = a[4 - 1]; 
	return *this;
    }

    inline real_t& operator[] (int i) { return v[i]; }
    inline const real_t&  operator[] (int i) const { return v[i]; }
    inline real_t& operator() (int i) { return v[i % 4]; }
    inline const real_t&  operator() (int i) const { return v[i % 4]; }

    inline bool is_zero() const { return (v[0] == 0.0 && v[1] == 0.0 && v[2] == 0.0 && v[3] == 0.0); }
    inline bool is_tiny(real_t tiny_tol = epsilon) const {
	return (fabs(v[0]) <= tiny_tol && fabs(v[1]) <= tiny_tol && fabs(v[2]) <= tiny_tol && fabs(v[3]) <= tiny_tol);
    }

    bool unit(); 
    real_t length() const; 
};





//| binary op : componentwise


inline fvec4 operator + (const fvec4& a, const fvec4& b)
{
    fvec4 c;
    for(int i = 0; i < 4;++i) { c[i] = a[i] + b[i]; }
    return c;
}

inline fvec4 operator + (real_t a, const fvec4& b)
{
    fvec4 c;
    for(int i = 0; i < 4;++i) { c[i] = a + b[i]; }
    return c;
}

inline fvec4 operator + (const fvec4& a, real_t b)
{
    fvec4 c;
    for(int i = 0; i < 4;++i) { c[i] = a[i] + b; }
    return c;
}



inline fvec4 operator - (const fvec4& a, const fvec4& b)
{
    fvec4 c;
    for(int i = 0; i < 4;++i) { c[i] = a[i] - b[i]; }
    return c;
}

inline fvec4 operator - (real_t a, const fvec4& b)
{
    fvec4 c;
    for(int i = 0; i < 4;++i) { c[i] = a - b[i]; }
    return c;
}

inline fvec4 operator - (const fvec4& a, real_t b)
{
    fvec4 c;
    for(int i = 0; i < 4;++i) { c[i] = a[i] - b; }
    return c;
}



inline fvec4 operator * (const fvec4& a, const fvec4& b)
{
    fvec4 c;
    for(int i = 0; i < 4;++i) { c[i] = a[i] * b[i]; }
    return c;
}

inline fvec4 operator * (real_t a, const fvec4& b)
{
    fvec4 c;
    for(int i = 0; i < 4;++i) { c[i] = a * b[i]; }
    return c;
}

inline fvec4 operator * (const fvec4& a, real_t b)
{
    fvec4 c;
    for(int i = 0; i < 4;++i) { c[i] = a[i] * b; }
    return c;
}



inline fvec4 operator / (const fvec4& a, const fvec4& b)
{
    fvec4 c;
    for(int i = 0; i < 4;++i) { c[i] = a[i] / b[i]; }
    return c;
}

inline fvec4 operator / (real_t a, const fvec4& b)
{
    fvec4 c;
    for(int i = 0; i < 4;++i) { c[i] = a / b[i]; }
    return c;
}

inline fvec4 operator / (const fvec4& a, real_t b)
{
    fvec4 c;
    for(int i = 0; i < 4;++i) { c[i] = a[i] / b; }
    return c;
}



//| cumulative op : componentwise


inline fvec4 operator += (fvec4& a, const fvec4& b)
{
    return a = (a + b);
}

inline fvec4 operator += (fvec4& a, real_t b)
{
    return a = (a + b);
}



inline fvec4 operator -= (fvec4& a, const fvec4& b)
{
    return a = (a - b);
}

inline fvec4 operator -= (fvec4& a, real_t b)
{
    return a = (a - b);
}



inline fvec4 operator *= (fvec4& a, const fvec4& b)
{
    return a = (a * b);
}

inline fvec4 operator *= (fvec4& a, real_t b)
{
    return a = (a * b);
}



inline fvec4 operator /= (fvec4& a, const fvec4& b)
{
    return a = (a / b);
}

inline fvec4 operator /= (fvec4& a, real_t b)
{
    return a = (a / b);
}



//| logical op : componentwise


inline int operator == (const fvec4& a, const fvec4& b)
{
    int c = 1;
    for(int i = 0; i < 4;++i) { c = c && a[i] == b[i]; }
    return c;
}

inline int operator == (real_t a, const fvec4& b)
{
    int c = 1;
    for(int i = 0; i < 4;++i) { c = c && a == b[i]; }
    return c;
}

inline int operator == (const fvec4& a, real_t b)
{
    int c = 1;
    for(int i = 0; i < 4;++i) { c = c && a[i] == b; }
    return c;
}



inline int operator < (const fvec4& a, const fvec4& b)
{
    int c = 1;
    for(int i = 0; i < 4;++i) { c = c && a[i] < b[i]; }
    return c;
}

inline int operator < (real_t a, const fvec4& b)
{
    int c = 1;
    for(int i = 0; i < 4;++i) { c = c && a < b[i]; }
    return c;
}

inline int operator < (const fvec4& a, real_t b)
{
    int c = 1;
    for(int i = 0; i < 4;++i) { c = c && a[i] < b; }
    return c;
}



inline int operator <= (const fvec4& a, const fvec4& b)
{
    int c = 1;
    for(int i = 0; i < 4;++i) { c = c && a[i] <= b[i]; }
    return c;
}

inline int operator <= (real_t a, const fvec4& b)
{
    int c = 1;
    for(int i = 0; i < 4;++i) { c = c && a <= b[i]; }
    return c;
}

inline int operator <= (const fvec4& a, real_t b)
{
    int c = 1;
    for(int i = 0; i < 4;++i) { c = c && a[i] <= b; }
    return c;
}



inline int operator > (const fvec4& a, const fvec4& b)
{
    int c = 1;
    for(int i = 0; i < 4;++i) { c = c && a[i] > b[i]; }
    return c;
}

inline int operator > (real_t a, const fvec4& b)
{
    int c = 1;
    for(int i = 0; i < 4;++i) { c = c && a > b[i]; }
    return c;
}

inline int operator > (const fvec4& a, real_t b)
{
    int c = 1;
    for(int i = 0; i < 4;++i) { c = c && a[i] > b; }
    return c;
}



inline int operator >= (const fvec4& a, const fvec4& b)
{
    int c = 1;
    for(int i = 0; i < 4;++i) { c = c && a[i] >= b[i]; }
    return c;
}

inline int operator >= (real_t a, const fvec4& b)
{
    int c = 1;
    for(int i = 0; i < 4;++i) { c = c && a >= b[i]; }
    return c;
}

inline int operator >= (const fvec4& a, real_t b)
{
    int c = 1;
    for(int i = 0; i < 4;++i) { c = c && a[i] >= b; }
    return c;
}



//| unary op : componentwise
inline fvec4 operator - (const fvec4& a)
{
    fvec4 c;
    for(int i = 0; i < 4;++i) { c[i] = - a[i]; }
    return c;
}

inline fvec4 absolute(const fvec4& val)
{
    return fvec4(fabs(val[0]), fabs(val[1]), fabs(val[2]), fabs(val[3]));
}


//| R^n -> R
inline real_t sum(const fvec4& a)
{
    real_t s = 0;
    
    s += a[1 - 1];
    
    s += a[2 - 1];
    
    s += a[3 - 1];
    
    s += a[4 - 1];
    
    return s;
}

inline real_t inner(const fvec4& a, const fvec4& b)
{
    fvec4 tmp = a * b;
    return sum(tmp);
}
inline real_t squaredNorm(const fvec4& a) {
	return inner(a, a);
}
inline real_t norm(const fvec4& a)
{
    return sqrt(inner(a, a));
}

inline fvec4 unit(const fvec4& a)
{
    real_t n = norm(a);
    if(n < epsilon)
	return 0;
    else
	return a / n;
}

inline real_t angle(const fvec4& a, const fvec4& b)
{
    real_t ang = inner(unit(a), unit(b));
    if(ang > 1 - epsilon)
	return 0;
    else if(ang < -1 + epsilon)
	return M_PI;
    else
	return acos(ang);
}

inline fvec4 proj(const fvec4& axis, const fvec4& a)
{
    fvec4 u = unit(axis);
    return inner(a, u) * u;
}

 

//| I/O
void print(const fvec4& a);

void store(FILE* fp, const fvec4& v);

int scan(FILE* fp, const fvec4& v);

int apx_equal(const fvec4& a, const fvec4& b);
int apx_equal(const fvec4& a, const fvec4& b, real_t eps);

fvec3 cross(const fvec3& a, const fvec3& b);


}; //namespace graphics
#endif
