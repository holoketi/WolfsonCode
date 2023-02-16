#ifndef __vec_h
#define __vec_h
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


struct vec2 {
    real v[2];
    static const int n;

    inline vec2() { }
    inline vec2(real a)
    {
	 v[1 - 1] = a;  v[2 - 1] = a;  
    }

    inline vec2(real v_1  , real v_2 )
    {
	 v[1 - 1] = v_1;  v[2 - 1] = v_2; 
    }

    inline vec2(const ivec2& a)
    {
	 v[1 - 1] = a[1 - 1];  v[2 - 1] = a[2 - 1]; 
    }

    inline vec2(const vec2& a)
    {
	 v[1 - 1] = a[1 - 1];  v[2 - 1] = a[2 - 1]; 
    }

    inline vec2& operator=(const vec2& a)
    {
	 v[1 - 1] = a[1 - 1];  v[2 - 1] = a[2 - 1]; 
	return *this;
    }

    inline real& operator[] (int i) { return v[i]; }
    inline const real&  operator[] (int i) const { return v[i]; }
    inline real& operator() (int i) { return v[i % 2]; }
    inline const real&  operator() (int i) const { return v[i % 2]; }

    bool unit(); 
    real length() const; 

    inline bool is_zero() const { return (v[0] == 0.0 && v[1] == 0.0); }
    inline bool is_tiny(real tiny_tol = epsilon) const {
	return (fabs(v[0]) <= tiny_tol && fabs(v[1]) <= tiny_tol);
    }

    //
    // returns  1: this and other vectors are parallel
    //         -1: this and other vectors are anti-parallel
    //          0: this and other vectors are not parallel
    //             or at least one of the vectors is zero
    int is_parallel( 
        const vec2&,                 // other vector     
        real = angle_tolerance // optional angle tolerance (radians)
        ) const;

    //
    // Computes a, b such that this vector = a*X + b*Y
    // Returns false if unable to solve for a,b.  This happens
    // when X,Y is not really a basis.
    //
    // If X,Y is known to be an orthonormal frame,
    // then a = V*X, b = V*Y will compute
    // the same result more quickly.
    bool decompose( 
         const vec2&, // X
         const vec2&, // Y
         real&, // a
         real&  // b
         ) const;

    //
    // returns true:  this and other vectors are perpendicular
    //         false: this and other vectors are not perpendicular
    //                or at least one of the vectors is zero
    bool is_perpendicular(
        const vec2&,           // other vector     
        real = angle_tolerance // optional angle tolerance (radians)
        ) const;
 
    //
    // set this vector to be perpendicular to another vector
    bool perpendicular( // Result is not unitized. 
                         // returns false if input vector is zero
        const vec2& 
        );

    //
    // set this vector to be perpendicular to a line defined by 2 points
    bool perpendicular( 
        const vec2&, 
        const vec2& 
        );
};





//| binary op : componentwise


inline vec2 operator + (const vec2& a, const vec2& b)
{
    vec2 c;
    for(int i = 0; i < 2;++i) { c[i] = a[i] + b[i]; }
    return c;
}

inline vec2 operator + (real a, const vec2& b)
{
    vec2 c;
    for(int i = 0; i < 2;++i) { c[i] = a + b[i]; }
    return c;
}

inline vec2 operator + (const vec2& a, real b)
{
    vec2 c;
    for(int i = 0; i < 2;++i) { c[i] = a[i] + b; }
    return c;
}



inline vec2 operator - (const vec2& a, const vec2& b)
{
    vec2 c;
    for(int i = 0; i < 2;++i) { c[i] = a[i] - b[i]; }
    return c;
}

inline vec2 operator - (real a, const vec2& b)
{
    vec2 c;
    for(int i = 0; i < 2;++i) { c[i] = a - b[i]; }
    return c;
}

inline vec2 operator - (const vec2& a, real b)
{
    vec2 c;
    for(int i = 0; i < 2;++i) { c[i] = a[i] - b; }
    return c;
}



inline vec2 operator * (const vec2& a, const vec2& b)
{
    vec2 c;
    for(int i = 0; i < 2;++i) { c[i] = a[i] * b[i]; }
    return c;
}

inline vec2 operator * (real a, const vec2& b)
{
    vec2 c;
    for(int i = 0; i < 2;++i) { c[i] = a * b[i]; }
    return c;
}

inline vec2 operator * (const vec2& a, real b)
{
    vec2 c;
    for(int i = 0; i < 2;++i) { c[i] = a[i] * b; }
    return c;
}



inline vec2 operator / (const vec2& a, const vec2& b)
{
    vec2 c;
    for(int i = 0; i < 2;++i) { c[i] = a[i] / b[i]; }
    return c;
}

inline vec2 operator / (real a, const vec2& b)
{
    vec2 c;
    for(int i = 0; i < 2;++i) { c[i] = a / b[i]; }
    return c;
}

inline vec2 operator / (const vec2& a, real b)
{
    vec2 c;
    for(int i = 0; i < 2;++i) { c[i] = a[i] / b; }
    return c;
}



//| cumulative op : componentwise


inline vec2 operator += (vec2& a, const vec2& b)
{
    return a = (a + b);
}

inline vec2 operator += (vec2& a, real b)
{
    return a = (a + b);
}



inline vec2 operator -= (vec2& a, const vec2& b)
{
    return a = (a - b);
}

inline vec2 operator -= (vec2& a, real b)
{
    return a = (a - b);
}



inline vec2 operator *= (vec2& a, const vec2& b)
{
    return a = (a * b);
}

inline vec2 operator *= (vec2& a, real b)
{
    return a = (a * b);
}



inline vec2 operator /= (vec2& a, const vec2& b)
{
    return a = (a / b);
}

inline vec2 operator /= (vec2& a, real b)
{
    return a = (a / b);
}



//| logical op : componentwise


inline int operator == (const vec2& a, const vec2& b)
{
    int c = 1;
    for(int i = 0; i < 2;++i) { c = c && a[i] == b[i]; }
    return c;
}

inline int operator == (real a, const vec2& b)
{
    int c = 1;
    for(int i = 0; i < 2;++i) { c = c && a == b[i]; }
    return c;
}

inline int operator == (const vec2& a, real b)
{
    int c = 1;
    for(int i = 0; i < 2;++i) { c = c && a[i] == b; }
    return c;
}



inline int operator < (const vec2& a, const vec2& b)
{
    int c = 1;
    for(int i = 0; i < 2;++i) { c = c && a[i] < b[i]; }
    return c;
}

inline int operator < (real a, const vec2& b)
{
    int c = 1;
    for(int i = 0; i < 2;++i) { c = c && a < b[i]; }
    return c;
}

inline int operator < (const vec2& a, real b)
{
    int c = 1;
    for(int i = 0; i < 2;++i) { c = c && a[i] < b; }
    return c;
}



inline int operator <= (const vec2& a, const vec2& b)
{
    int c = 1;
    for(int i = 0; i < 2;++i) { c = c && a[i] <= b[i]; }
    return c;
}

inline int operator <= (real a, const vec2& b)
{
    int c = 1;
    for(int i = 0; i < 2;++i) { c = c && a <= b[i]; }
    return c;
}

inline int operator <= (const vec2& a, real b)
{
    int c = 1;
    for(int i = 0; i < 2;++i) { c = c && a[i] <= b; }
    return c;
}



inline int operator > (const vec2& a, const vec2& b)
{
    int c = 1;
    for(int i = 0; i < 2;++i) { c = c && a[i] > b[i]; }
    return c;
}

inline int operator > (real a, const vec2& b)
{
    int c = 1;
    for(int i = 0; i < 2;++i) { c = c && a > b[i]; }
    return c;
}

inline int operator > (const vec2& a, real b)
{
    int c = 1;
    for(int i = 0; i < 2;++i) { c = c && a[i] > b; }
    return c;
}



inline int operator >= (const vec2& a, const vec2& b)
{
    int c = 1;
    for(int i = 0; i < 2;++i) { c = c && a[i] >= b[i]; }
    return c;
}

inline int operator >= (real a, const vec2& b)
{
    int c = 1;
    for(int i = 0; i < 2;++i) { c = c && a >= b[i]; }
    return c;
}

inline int operator >= (const vec2& a, real b)
{
    int c = 1;
    for(int i = 0; i < 2;++i) { c = c && a[i] >= b; }
    return c;
}



//| unary op : componentwise
inline vec2 operator - (const vec2& a)
{
    vec2 c;
    for(int i = 0; i < 2;++i) { c[i] = - a[i]; }
    return c;
}

//| R^n -> R
inline real sum(const vec2& a)
{
    real s = 0;
    
    s += a[1 - 1];
    
    s += a[2 - 1];
    
    return s;
}

inline real inner(const vec2& a, const vec2& b)
{
    vec2 tmp = a * b;
    return sum(tmp);
}

inline real norm(const vec2& a)
{
    return sqrt(inner(a, a));
}

inline real squaredNorm(const vec2& a) {
	return inner(a, a);
}

inline vec2 unit(const vec2& a)
{
    real n = norm(a);
    if(n < epsilon)
	return 0;
    else
	return a / n;
}

inline real angle(const vec2& a, const vec2& b)
{
    real ang = inner(unit(a), unit(b));
    if(ang > 1 - epsilon)
	return 0;
    else if(ang < -1 + epsilon)
	return M_PI;
    else
	return acos(ang);
}

inline vec2 proj(const vec2& axis, const vec2& a)
{
    vec2 u = unit(axis);
    return inner(a, u) * u;
}

inline vec2 absolute(const vec2& val)
{
    return vec2(fabs(val[0]), fabs(val[1]));
}

//| I/O
void print(const vec2& a);

void store(FILE* fp, const vec2& v);

int scan(FILE* fp, const vec2& v);

int apx_equal(const vec2& a, const vec2& b);
int apx_equal(const vec2& a, const vec2& b, real eps);


struct vec3 {
    real v[3];
    static const int n;

    inline vec3() { }
    inline vec3(real a)
    {
	 v[1 - 1] = a;  v[2 - 1] = a;  v[3 - 1] = a;  
    }

    inline vec3(real v_1  , real v_2  , real v_3 )
    {
	 v[1 - 1] = v_1;  v[2 - 1] = v_2;  v[3 - 1] = v_3; 
    }

    inline vec3(const ivec3& a)
    {
	 v[1 - 1] = a[1 - 1];  v[2 - 1] = a[2 - 1];  v[3 - 1] = a[3 - 1]; 
    }

    inline vec3(const vec3& a)
    {
	 v[1 - 1] = a[1 - 1];  v[2 - 1] = a[2 - 1];  v[3 - 1] = a[3 - 1]; 
    }

    inline vec3& operator=(const vec3& a)
    {
	 v[1 - 1] = a[1 - 1];  v[2 - 1] = a[2 - 1];  v[3 - 1] = a[3 - 1]; 
	return *this;
    }

    inline real& operator[] (int i) { return v[i]; }
    inline const real&  operator[] (int i) const { return v[i]; }
    inline real& operator() (int i) { return v[i % 3]; }
    inline const real&  operator() (int i) const { return v[i % 3]; }
 
    inline bool is_zero() const { return (v[0] == 0.0 && v[1] == 0.0 && v[2] == 0.0); }
    inline bool is_tiny(real tiny_tol = epsilon) const {
	return (fabs(v[0]) <= tiny_tol && fabs(v[1]) <= tiny_tol && fabs(v[2]) <= tiny_tol );
    }

    bool unit(); 
    real length() const; 

    //
    // Computes a, b, c such that this vector = a*X + b*Y + c*Z
    // Returns false if unable to solve for a,b,c.  This happens
    // when X,Y,Z is not really a basis.
    //
    // If X,Y,Z is known to be an orthonormal frame,
    // then a = V*X, b = V*Y, c = V*Z will compute
    // the same result more quickly.
    bool decompose( 
         const vec3&, // X
         const vec3&, // Y
         const vec3&, // Z
         real&, // a
         real&, // b
         real&  // c
         ) const;

    //
    // returns  1: this and other vectors are parallel
    //         -1: this and other vectors are anti-parallel
    //          0: this and other vectors are not parallel
    //             or at least one of the vectors is zero
    int is_parallel( 
        const vec3&,                 // other vector     
        real = angle_tolerance // optional angle tolerance (radians)
        ) const;

    //
    // returns true:  this and other vectors are perpendicular
    //         false: this and other vectors are not perpendicular
    //                or at least one of the vectors is zero
    bool is_perpendicular(
        const vec3&,                 // other vector     
        real = angle_tolerance // optional angle tolerance (radians)
        ) const;

    //
    // set this vector to be perpendicular to another vector
    bool perpendicular( // Result is not unitized. 
                        // returns false if input vector is zero
        const vec3& 
        );

    //
    // set this vector to be perpendicular to a plane defined by 3 points
    // returns false if points are coincident or colinear
    bool perpendicular(
         const vec3&, const vec3&, const vec3& 
         );
};





//| binary op : componentwise


inline vec3 operator + (const vec3& a, const vec3& b)
{
    vec3 c;
    for(int i = 0; i < 3;++i) { c[i] = a[i] + b[i]; }
    return c;
}

inline vec3 operator + (real a, const vec3& b)
{
    vec3 c;
    for(int i = 0; i < 3;++i) { c[i] = a + b[i]; }
    return c;
}

inline vec3 operator + (const vec3& a, real b)
{
    vec3 c;
    for(int i = 0; i < 3;++i) { c[i] = a[i] + b; }
    return c;
}



inline vec3 operator - (const vec3& a, const vec3& b)
{
    vec3 c;
    for(int i = 0; i < 3;++i) { c[i] = a[i] - b[i]; }
    return c;
}

inline vec3 operator - (real a, const vec3& b)
{
    vec3 c;
    for(int i = 0; i < 3;++i) { c[i] = a - b[i]; }
    return c;
}

inline vec3 operator - (const vec3& a, real b)
{
    vec3 c;
    for(int i = 0; i < 3;++i) { c[i] = a[i] - b; }
    return c;
}



inline vec3 operator * (const vec3& a, const vec3& b)
{
    vec3 c;
    for(int i = 0; i < 3;++i) { c[i] = a[i] * b[i]; }
    return c;
}

inline vec3 operator * (real a, const vec3& b)
{
    vec3 c;
    for(int i = 0; i < 3;++i) { c[i] = a * b[i]; }
    return c;
}

inline vec3 operator * (const vec3& a, real b)
{
    vec3 c;
    for(int i = 0; i < 3;++i) { c[i] = a[i] * b; }
    return c;
}



inline vec3 operator / (const vec3& a, const vec3& b)
{
    vec3 c;
    for(int i = 0; i < 3;++i) { c[i] = a[i] / b[i]; }
    return c;
}

inline vec3 operator / (real a, const vec3& b)
{
    vec3 c;
    for(int i = 0; i < 3;++i) { c[i] = a / b[i]; }
    return c;
}

inline vec3 operator / (const vec3& a, real b)
{
    vec3 c;
    for(int i = 0; i < 3;++i) { c[i] = a[i] / b; }
    return c;
}



//| cumulative op : componentwise


inline vec3 operator += (vec3& a, const vec3& b)
{
    return a = (a + b);
}

inline vec3 operator += (vec3& a, real b)
{
    return a = (a + b);
}



inline vec3 operator -= (vec3& a, const vec3& b)
{
    return a = (a - b);
}

inline vec3 operator -= (vec3& a, real b)
{
    return a = (a - b);
}



inline vec3 operator *= (vec3& a, const vec3& b)
{
    return a = (a * b);
}

inline vec3 operator *= (vec3& a, real b)
{
    return a = (a * b);
}



inline vec3 operator /= (vec3& a, const vec3& b)
{
    return a = (a / b);
}

inline vec3 operator /= (vec3& a, real b)
{
    return a = (a / b);
}



//| logical op : componentwise


inline int operator == (const vec3& a, const vec3& b)
{
    int c = 1;
    for(int i = 0; i < 3;++i) { c = c && a[i] == b[i]; }
    return c;
}

inline int operator == (real a, const vec3& b)
{
    int c = 1;
    for(int i = 0; i < 3;++i) { c = c && a == b[i]; }
    return c;
}

inline int operator == (const vec3& a, real b)
{
    int c = 1;
    for(int i = 0; i < 3;++i) { c = c && a[i] == b; }
    return c;
}



inline int operator < (const vec3& a, const vec3& b)
{
    int c = 1;
    for(int i = 0; i < 3;++i) { c = c && a[i] < b[i]; }
    return c;
}

inline int operator < (real a, const vec3& b)
{
    int c = 1;
    for(int i = 0; i < 3;++i) { c = c && a < b[i]; }
    return c;
}

inline int operator < (const vec3& a, real b)
{
    int c = 1;
    for(int i = 0; i < 3;++i) { c = c && a[i] < b; }
    return c;
}



inline int operator <= (const vec3& a, const vec3& b)
{
    int c = 1;
    for(int i = 0; i < 3;++i) { c = c && a[i] <= b[i]; }
    return c;
}

inline int operator <= (real a, const vec3& b)
{
    int c = 1;
    for(int i = 0; i < 3;++i) { c = c && a <= b[i]; }
    return c;
}

inline int operator <= (const vec3& a, real b)
{
    int c = 1;
    for(int i = 0; i < 3;++i) { c = c && a[i] <= b; }
    return c;
}



inline int operator > (const vec3& a, const vec3& b)
{
    int c = 1;
    for(int i = 0; i < 3;++i) { c = c && a[i] > b[i]; }
    return c;
}

inline int operator > (real a, const vec3& b)
{
    int c = 1;
    for(int i = 0; i < 3;++i) { c = c && a > b[i]; }
    return c;
}

inline int operator > (const vec3& a, real b)
{
    int c = 1;
    for(int i = 0; i < 3;++i) { c = c && a[i] > b; }
    return c;
}



inline int operator >= (const vec3& a, const vec3& b)
{
    int c = 1;
    for(int i = 0; i < 3;++i) { c = c && a[i] >= b[i]; }
    return c;
}

inline int operator >= (real a, const vec3& b)
{
    int c = 1;
    for(int i = 0; i < 3;++i) { c = c && a >= b[i]; }
    return c;
}

inline int operator >= (const vec3& a, real b)
{
    int c = 1;
    for(int i = 0; i < 3;++i) { c = c && a[i] >= b; }
    return c;
}



//| unary op : componentwise
inline vec3 operator - (const vec3& a)
{
    vec3 c;
    for(int i = 0; i < 3;++i) { c[i] = - a[i]; }
    return c;
}

inline vec3 absolute(const vec3& val)
{
    return vec3(fabs(val[0]), fabs(val[1]), fabs(val[2]));
}



//| R^n -> R
inline real sum(const vec3& a)
{
    real s = 0;
    
    s += a[1 - 1];
    
    s += a[2 - 1];
    
    s += a[3 - 1];
    
    return s;
}

inline real inner(const vec3& a, const vec3& b)
{
    vec3 tmp = a * b;
    return sum(tmp);
}

inline real squaredNorm(const vec3& a) {
	return inner(a, a);
}

inline real norm(const vec3& a)
{
    return sqrt(inner(a, a));
}

inline vec3 unit(const vec3& a)
{
    real n = norm(a);
    if(n < zero_epsilon)
	return 0;
    else
	return a / n;
}

inline real angle(const vec3& a, const vec3& b)
{
    real ang = inner(unit(a), unit(b));
    if(ang > 1 - epsilon)
	return 0;
    else if(ang < -1 + epsilon)
	return M_PI;
    else
	return acos(ang);
}

inline vec3 proj(const vec3& axis, const vec3& a)
{
    vec3 u = unit(axis);
    return inner(a, u) * u;
}

 

//| I/O
void print(const vec3& a);

void store(FILE* fp, const vec3& v);
int scan(FILE* fp, const vec3& v);

int apx_equal(const vec3& a, const vec3& b);
int apx_equal(const vec3& a, const vec3& b, real eps);

struct vec4 {
    real v[4];
    static const int n;

    inline vec4() { }
    inline vec4(real a)
    {
	 v[1 - 1] = a;  v[2 - 1] = a;  v[3 - 1] = a;  v[4 - 1] = a;  
    }

    inline vec4(real v_1  , real v_2  , real v_3  , real v_4 )
    {
	 v[1 - 1] = v_1;  v[2 - 1] = v_2;  v[3 - 1] = v_3;  v[4 - 1] = v_4; 
    }

    inline vec4(const ivec4& a)
    {
	 v[1 - 1] = a[1 - 1];  v[2 - 1] = a[2 - 1];  v[3 - 1] = a[3 - 1];  v[4 - 1] = a[4 - 1]; 
    }

    inline vec4(const vec4& a)
    {
	 v[1 - 1] = a[1 - 1];  v[2 - 1] = a[2 - 1];  v[3 - 1] = a[3 - 1];  v[4 - 1] = a[4 - 1]; 
    }

    inline vec4& operator=(const vec4& a)
    {
	 v[1 - 1] = a[1 - 1];  v[2 - 1] = a[2 - 1];  v[3 - 1] = a[3 - 1];  v[4 - 1] = a[4 - 1]; 
	return *this;
    }

    inline real& operator[] (int i) { return v[i]; }
    inline const real&  operator[] (int i) const { return v[i]; }
    inline real& operator() (int i) { return v[i % 4]; }
    inline const real&  operator() (int i) const { return v[i % 4]; }

    inline bool is_zero() const { return (v[0] == 0.0 && v[1] == 0.0 && v[2] == 0.0 && v[3] == 0.0); }
    inline bool is_tiny(real tiny_tol = epsilon) const {
	return (fabs(v[0]) <= tiny_tol && fabs(v[1]) <= tiny_tol && fabs(v[2]) <= tiny_tol && fabs(v[3]) <= tiny_tol);
    }

    bool unit(); 
    real length() const; 
};





//| binary op : componentwise


inline vec4 operator + (const vec4& a, const vec4& b)
{
    vec4 c;
    for(int i = 0; i < 4;++i) { c[i] = a[i] + b[i]; }
    return c;
}

inline vec4 operator + (real a, const vec4& b)
{
    vec4 c;
    for(int i = 0; i < 4;++i) { c[i] = a + b[i]; }
    return c;
}

inline vec4 operator + (const vec4& a, real b)
{
    vec4 c;
    for(int i = 0; i < 4;++i) { c[i] = a[i] + b; }
    return c;
}



inline vec4 operator - (const vec4& a, const vec4& b)
{
    vec4 c;
    for(int i = 0; i < 4;++i) { c[i] = a[i] - b[i]; }
    return c;
}

inline vec4 operator - (real a, const vec4& b)
{
    vec4 c;
    for(int i = 0; i < 4;++i) { c[i] = a - b[i]; }
    return c;
}

inline vec4 operator - (const vec4& a, real b)
{
    vec4 c;
    for(int i = 0; i < 4;++i) { c[i] = a[i] - b; }
    return c;
}



inline vec4 operator * (const vec4& a, const vec4& b)
{
    vec4 c;
    for(int i = 0; i < 4;++i) { c[i] = a[i] * b[i]; }
    return c;
}

inline vec4 operator * (real a, const vec4& b)
{
    vec4 c;
    for(int i = 0; i < 4;++i) { c[i] = a * b[i]; }
    return c;
}

inline vec4 operator * (const vec4& a, real b)
{
    vec4 c;
    for(int i = 0; i < 4;++i) { c[i] = a[i] * b; }
    return c;
}



inline vec4 operator / (const vec4& a, const vec4& b)
{
    vec4 c;
    for(int i = 0; i < 4;++i) { c[i] = a[i] / b[i]; }
    return c;
}

inline vec4 operator / (real a, const vec4& b)
{
    vec4 c;
    for(int i = 0; i < 4;++i) { c[i] = a / b[i]; }
    return c;
}

inline vec4 operator / (const vec4& a, real b)
{
    vec4 c;
    for(int i = 0; i < 4;++i) { c[i] = a[i] / b; }
    return c;
}



//| cumulative op : componentwise


inline vec4 operator += (vec4& a, const vec4& b)
{
    return a = (a + b);
}

inline vec4 operator += (vec4& a, real b)
{
    return a = (a + b);
}



inline vec4 operator -= (vec4& a, const vec4& b)
{
    return a = (a - b);
}

inline vec4 operator -= (vec4& a, real b)
{
    return a = (a - b);
}



inline vec4 operator *= (vec4& a, const vec4& b)
{
    return a = (a * b);
}

inline vec4 operator *= (vec4& a, real b)
{
    return a = (a * b);
}



inline vec4 operator /= (vec4& a, const vec4& b)
{
    return a = (a / b);
}

inline vec4 operator /= (vec4& a, real b)
{
    return a = (a / b);
}



//| logical op : componentwise


inline int operator == (const vec4& a, const vec4& b)
{
    int c = 1;
    for(int i = 0; i < 4;++i) { c = c && a[i] == b[i]; }
    return c;
}

inline int operator == (real a, const vec4& b)
{
    int c = 1;
    for(int i = 0; i < 4;++i) { c = c && a == b[i]; }
    return c;
}

inline int operator == (const vec4& a, real b)
{
    int c = 1;
    for(int i = 0; i < 4;++i) { c = c && a[i] == b; }
    return c;
}



inline int operator < (const vec4& a, const vec4& b)
{
    int c = 1;
    for(int i = 0; i < 4;++i) { c = c && a[i] < b[i]; }
    return c;
}

inline int operator < (real a, const vec4& b)
{
    int c = 1;
    for(int i = 0; i < 4;++i) { c = c && a < b[i]; }
    return c;
}

inline int operator < (const vec4& a, real b)
{
    int c = 1;
    for(int i = 0; i < 4;++i) { c = c && a[i] < b; }
    return c;
}



inline int operator <= (const vec4& a, const vec4& b)
{
    int c = 1;
    for(int i = 0; i < 4;++i) { c = c && a[i] <= b[i]; }
    return c;
}

inline int operator <= (real a, const vec4& b)
{
    int c = 1;
    for(int i = 0; i < 4;++i) { c = c && a <= b[i]; }
    return c;
}

inline int operator <= (const vec4& a, real b)
{
    int c = 1;
    for(int i = 0; i < 4;++i) { c = c && a[i] <= b; }
    return c;
}



inline int operator > (const vec4& a, const vec4& b)
{
    int c = 1;
    for(int i = 0; i < 4;++i) { c = c && a[i] > b[i]; }
    return c;
}

inline int operator > (real a, const vec4& b)
{
    int c = 1;
    for(int i = 0; i < 4;++i) { c = c && a > b[i]; }
    return c;
}

inline int operator > (const vec4& a, real b)
{
    int c = 1;
    for(int i = 0; i < 4;++i) { c = c && a[i] > b; }
    return c;
}



inline int operator >= (const vec4& a, const vec4& b)
{
    int c = 1;
    for(int i = 0; i < 4;++i) { c = c && a[i] >= b[i]; }
    return c;
}

inline int operator >= (real a, const vec4& b)
{
    int c = 1;
    for(int i = 0; i < 4;++i) { c = c && a >= b[i]; }
    return c;
}

inline int operator >= (const vec4& a, real b)
{
    int c = 1;
    for(int i = 0; i < 4;++i) { c = c && a[i] >= b; }
    return c;
}



//| unary op : componentwise
inline vec4 operator - (const vec4& a)
{
    vec4 c;
    for(int i = 0; i < 4;++i) { c[i] = - a[i]; }
    return c;
}

inline vec4 absolute(const vec4& val)
{
    return vec4(fabs(val[0]), fabs(val[1]), fabs(val[2]), fabs(val[3]));
}


//| R^n -> R
inline real sum(const vec4& a)
{
    real s = 0;
    
    s += a[1 - 1];
    
    s += a[2 - 1];
    
    s += a[3 - 1];
    
    s += a[4 - 1];
    
    return s;
}

inline real inner(const vec4& a, const vec4& b)
{
    vec4 tmp = a * b;
    return sum(tmp);
}
inline real squaredNorm(const vec4& a) {
	return inner(a, a);
}
inline real norm(const vec4& a)
{
    return sqrt(inner(a, a));
}

inline vec4 unit(const vec4& a)
{
    real n = norm(a);
    if(n < epsilon)
	return 0;
    else
	return a / n;
}

inline real angle(const vec4& a, const vec4& b)
{
    real ang = inner(unit(a), unit(b));
    if(ang > 1 - epsilon)
	return 0;
    else if(ang < -1 + epsilon)
	return M_PI;
    else
	return acos(ang);
}

inline vec4 proj(const vec4& axis, const vec4& a)
{
    vec4 u = unit(axis);
    return inner(a, u) * u;
}

 

//| I/O
void print(const vec4& a);

void store(FILE* fp, const vec4& v);

int scan(FILE* fp, const vec4& v);

int apx_equal(const vec4& a, const vec4& b);
int apx_equal(const vec4& a, const vec4& b, real eps);

vec3 cross(const vec3& a, const vec3& b);


}; //namespace graphics
#endif
