// Matrix4.h
// Copyright (c) 2009 The Foundry Visionmongers Ltd.  All Rights Reserved.

#ifndef _Matrix4_h
#define _Matrix4_h

#include "model/Vector4.h"

#include <string.h>
#include <assert.h>

#include <vector>
#ifdef FN_OS_WINDOWS
#include <float.h>
#else
#include <math.h>
#endif

#include "graphics/sys.h"
namespace user {

    class Vector3;

    class Matrix4
    {
      inline int isNaN(float lValue) const { 
#if defined(FN_OS_MAC)
        return __inline_isnanf(lValue);
#elif defined(FN_OS_WINDOWS)
        return _isnan(lValue) != 0;
#else
        return isnan(lValue);
#endif
      }
    public:
      // warning: this relies on C++ packing these together in order.

      // Matrix is stored in transposed order to allow it to be passed
      // to OpenGL unchanged:
      float
      a00, a10, a20, a30,
      a01, a11, a21, a31,
      a02, a12, a22, a32,
      a03, a13, a23, a33;

      // warning: for back compatibility the [][] operator is transposed
      // to be [col][row] order!
      float* operator[](int i) { return &a00 + i * 4; }
      const float* operator[](int i) const { return &a00 + i * 4; }
      const float* array() const { return &a00; } // for passing to OpenGL

	  void print() 
	  {
		  for (int i = 0 ; i < 4 ;++i){
			  for (int j = 0 ; j < 4 ; ++j)  {
				  LOG("%f  ", (*this)[i][j]);
			  }
			  LOG("\n");
		  }

	  }

      Matrix4() { makeIdentity(); }
	  Matrix4(const Matrix4& a)
	  : a00(a.a00), a01(a.a01), a02(a.a02), a03(a.a03),
	  a10(a.a10), a11(a.a11), a12(a.a12), a13(a.a13),
	  a20(a.a20), a21(a.a21), a22(a.a22), a23(a.a23),
	  a30(a.a30), a31(a.a31), a32(a.a32), a33(a.a33)
	  {
	  }

      Matrix4(const float array[16]) { memcpy(&a00, array, 16 * sizeof(float)); }

      Matrix4(float a, float b, float c, float d,
              float e, float f, float g, float h,
              float i, float j, float k, float l,
              float m, float n, float o, float p) {
        a00 = a;
        a01 = b;
        a02 = c;
        a03 = d;
        a10 = e;
        a11 = f;
        a12 = g;
        a13 = h;
        a20 = i;
        a21 = j;
        a22 = k;
        a23 = l;
        a30 = m;
        a31 = n;
        a32 = o;
        a33 = p;
      }

      void set(float a, float b, float c, float d,
               float e, float f, float g, float h,
               float i, float j, float k, float l,
               float m, float n, float o, float p)
      {
        a00 = a;
        a01 = b;
        a02 = c;
        a03 = d;
        a10 = e;
        a11 = f;
        a12 = g;
        a13 = h;
        a20 = i;
        a21 = j;
        a22 = k;
        a23 = l;
        a30 = m;
        a31 = n;
        a32 = o;
        a33 = p;
      }

	  Matrix4& operator =  (const Matrix4& );
      Matrix4  operator *  ( const Matrix4& ) const;
      Matrix4& operator *= ( const Matrix4& );

      Matrix4  operator +  ( const Matrix4& ) const;
      Matrix4& operator += ( const Matrix4& );
      Matrix4  operator -  ( const Matrix4& ) const;
      Matrix4& operator -= ( const Matrix4& );
      Matrix4  operator *  (float) const;
      Matrix4& operator *= (float);
      Matrix4  operator /  (float d) const { return (*this) * (1 / d); }
      Matrix4& operator /= (float d) { return (*this) *= (1 / d); }

      Vector4  operator *  (const Vector4& v) const
      {
        return Vector4(
                 a00 * v.x + a01 * v.y + a02 * v.z + a03 * v.w,
                 a10 * v.x + a11 * v.y + a12 * v.z + a13 * v.w,
                 a20 * v.x + a21 * v.y + a22 * v.z + a23 * v.w,
                 a30 * v.x + a31 * v.y + a32 * v.z + a33 * v.w
                 );
      }

      /*! Same as this*v. */
      Vector4 transform(const Vector4& v) const { return (*this) * v; }

      /*!
         Same as the xyz of transform(v,1). This will transform a point
         in space but \e only if this is not a perspective matrix, meaning
         the last row is 0,0,0,1.
       */
      Vector3 transform(const Vector3& v) const
      {
        return Vector3(
                 a00 * v.x + a01 * v.y + a02 * v.z + a03,
                 a10 * v.x + a11 * v.y + a12 * v.z + a13,
                 a20 * v.x + a21 * v.y + a22 * v.z + a23
                 );
      }

      /*!
         Same as the xyz of transform(v,0). This will transform a vector
         in space but \e only if this is not a perspective matrix, meaning
         the last row is 0,0,0,1.
       */
      Vector3 vtransform(const Vector3& v) const
      {
        return Vector3(
                 a00 * v.x + a01 * v.y + a02 * v.z,
                 a10 * v.x + a11 * v.y + a12 * v.z,
                 a20 * v.x + a21 * v.y + a22 * v.z
                 );
      }

      /*!
         Same as transpose().transform(v,0). If this is the inverse of
         a transform matrix, this will transform normals.
       */
      Vector3 ntransform(const Vector3& v) const
      {
        return Vector3(
                 a00 * v.x + a10 * v.y + a20 * v.z,
                 a01 * v.x + a11 * v.y + a21 * v.z,
                 a02 * v.x + a12 * v.y + a22 * v.z
                 );
      }

      /*!
         Same as this*Vector4(v.x,v.y,v.z,w). Useful for doing transforms
         when w is stored in a different location than the xyz.
       */
      Vector4 transform(const Vector3& v, float w) const
      {
        return Vector4(
                 a00 * v.x + a01 * v.y + a02 * v.z + a03 * w,
                 a10 * v.x + a11 * v.y + a12 * v.z + a13 * w,
                 a20 * v.x + a21 * v.y + a22 * v.z + a23 * w,
                 a30 * v.x + a31 * v.y + a32 * v.z + a33 * w
                 );
      }

      bool operator != ( const Matrix4& b) const { return memcmp(&a00, &b.a00, 16 * sizeof(float)) != 0; }
      bool operator == ( const Matrix4& b) const { return !memcmp(&a00, &b.a00, 16 * sizeof(float)); }

      float determinant(void) const
      {
        return
          a01 * a23 * a32 * a10 - a01 * a22 * a33 * a10 - a23 * a31 * a02 * a10 + a22 * a31 * a03 * a10
          - a00 * a23 * a32 * a11 + a00 * a22 * a33 * a11 + a23 * a30 * a02 * a11 - a22 * a30 * a03 * a11
          - a01 * a23 * a30 * a12 + a00 * a23 * a31 * a12 + a01 * a22 * a30 * a13 - a00 * a22 * a31 * a13
          - a33 * a02 * a11 * a20 + a32 * a03 * a11 * a20 + a01 * a33 * a12 * a20 - a31 * a03 * a12 * a20
          - a01 * a32 * a13 * a20 + a31 * a02 * a13 * a20 + a33 * a02 * a10 * a21 - a32 * a03 * a10 * a21
          - a00 * a33 * a12 * a21 + a30 * a03 * a12 * a21 + a00 * a32 * a13 * a21 - a30 * a02 * a13 * a21;
      }
      Matrix4 inverse(float det) const;
      Matrix4 inverse() const { return inverse(determinant()); }

      static const Matrix4 _identity;
      static const Matrix4& identity() { return _identity; }

      /*! Replace the contents with the identity. */
      void makeIdentity() { *this = _identity; }

      void scaling(float);
      void scaling(float, float, float);
      void scaling(const Vector3& v) { scaling(v.x, v.y, v.z); }
      void translation(float, float, float = 0.0f);
      void translation(const Vector3& v) { translation(v.x, v.y, v.z); }
      void rotationX(float);
      void rotationY(float);
      void rotationZ(float);
	  void euler_rotate(float x, float y, float z);
      void rotation(float a) { rotationZ(a); }
      void rotation(float a, float x, float y, float z);
      void rotation(float a, const Vector3& v) { rotation(a, v.x, v.y, v.z); }
      void projection(float lens, float minz, float maxz, bool persp = true);

      // destructive modifications:
      void transpose();
      void scale(float);
      void scale(float, float, float = 1);
      void scale(const Vector3& v) { scale(v.x, v.y, v.z); }
      void translate(float, float, float = 0.0f);
      void translate(const Vector3& v) { translate(v.x, v.y, v.z); }
	  void rotateX(float);
	  void rotateY(float);
	  void rotateZ(float);
	  void rotate(float a) { rotateZ(a); }
      void rotate(float a, float x, float y, float z);
      void rotate(float a, const Vector4& v) { rotate(a, v.x, v.y, v.z); }
      void skew(float a);
      void skewXY(float x, float y);

	  void unskew(float s);

      /*! Return the transformation of a 1 unit vector in x, if this is not a persxpective matrix (ie bottom row must be 0,0,0,1). */
      const Vector3& x_axis() const { return *(Vector3*)(&a00); }
      /*! Return the transformation of a 1 unit vector in y, if this is not a persxpective matrix (ie bottom row must be 0,0,0,1). */
      const Vector3& y_axis() const { return *(Vector3*)(&a01); }
      /*! Return the transformation of a 1 unit vector in z, if this is not a persxpective matrix (ie bottom row must be 0,0,0,1). */
      const Vector3& z_axis() const { return *(Vector3*)(&a02); }
      /*! Return the transformation of a 1 unit vector in x, if this is not a persxpective matrix (ie bottom row must be 0,0,0,1). */
      Vector3& x_axis() { return *(Vector3*)(&a00); }
      /*! Return the transformation of a 1 unit vector in y, if this is not a persxpective matrix (ie bottom row must be 0,0,0,1). */
      Vector3& y_axis() { return *(Vector3*)(&a01); }
      /*! Return the transformation of a 1 unit vector in z, if this is not a persxpective matrix (ie bottom row must be 0,0,0,1). */
      Vector3& z_axis() { return *(Vector3*)(&a02); }
      /*! Return the transformation of the point 0,0,0, if this is not a perspective matrix (ie bottom row is 0,0,0,1). */
      const Vector3& translation() const { return *(Vector3*)(&a03); }
	  Vector3& translation()  { return *(Vector3*)(&a03); }

      Vector3 scale() const;
      void scaleOnly();
      void rotationOnly();
      void translationOnly();
      void scaleAndRotationOnly();

	  // return flip axis: -1: no flip, 0:x, 1:y, 2:z
      bool rotationsZXY(float& rx, float& ry, float& rz) const;

      /*! Corner pinning: map 0,0,1,1 square to the four corners (anticlockwise from bottom left) */
      void mapUnitSquareToQuad( float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3 );
      /*! Corner pinning: map the four corners (anticlockwise from bottom left) to 0,0,1,1 square */
      void mapQuadToUnitSquare( float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3 );



      /*! Return whether all of the components are valid numbers. */
      bool isValid() const 
      {
        if (isNaN(a00) || isNaN(a10) || isNaN(a20) || isNaN(a30) ||
            isNaN(a01) || isNaN(a11) || isNaN(a21) || isNaN(a31) ||
            isNaN(a02) || isNaN(a12) || isNaN(a22) || isNaN(a32) ||
            isNaN(a03) || isNaN(a13) || isNaN(a23) || isNaN(a33))
          return false;
        return true;
      }
    };


    //! convert a DD::Image::Matrix4 to a std::vector<double>
    //! transposes from column-major to row-major
    inline std::vector<double> Matrix4ToVector(const Matrix4& matrix)
    {
      std::vector<double> ret(16);
      for (int i = 0; i < 16;++i){
        ret[i] = matrix[i % 4][i / 4];
      }
      return ret;
    }
    
    //! convert a std::vector<double> to a DD::Image::Matrix4
    //! transposes from row-major to column-major
    inline Matrix4 VectorToMatrix4(const std::vector<double>& matrix)
    {
      assert(matrix.size() == 16);
      Matrix4 ret;
      for (int i = 0; i < 16 ;++i){
        ret[i % 4][i / 4] = float(matrix[i]);
      }
      return ret;
    }

}

#endif

