#ifndef _Vector4_h
#define _Vector4_h

#include "model/Vector3.h"

namespace user
  {

    class  Vector4
    {
    public:
      float      x, y, z, w;

      /*! vector[0] is x, vector[1] is y, vector[2] is z, vector[3] is w. */
      float&     operator[] (int i) { return *((&x) + i); }

      /*! vector[0] is x, vector[1] is y, vector[2] is z, vector[3] is w. */
      const float& operator[] (int i) const { return *((&x) + i); }

      /*! Returns a ponter that can be passed to OpenGL */
      const float* array() const { return &x; }

      /*! The default constructor leaves garbage in x,y,z,w! */
      Vector4() {}

      /*! Constructor from an array of numbers. */
      Vector4(const float v[4]) : x(v[0]), y(v[1]), z(v[2]), w(v[3]) {}

      /*! Constructor from an array of numbers. */
      Vector4(const double v[4]) : x((float)v[0]), y((float)v[1]), z((float)v[2]), w((float)v[3]) {}

      /*! Constructor that sets all 4 numbers */
      Vector4(float a, float b, float c = 0, float d = 1) : x(a), y(b), z(c), w(d) {}

      /*! Change all of xyzw. */
      void set(float a, float b, float c = 0, float d = 1) { x = a;
                                                             y = b;
                                                             z = c;
                                                             w = d; }
	  Vector4(const Vector4 &v): x(v.x), y(v.y), z(v.z), w(v.w) {}
      /*! Conversion from a Vector3 */
      Vector4(const Vector3 &v, float d = 1.0) : x(v.x), y(v.y), z(v.z), w(d) {}
      /*! Conversion from a Vector3 */
      void set(const Vector3& v, float d = 1.0) { x = v.x;
                                                y = v.y;
                                                z = v.z;
                                                w = d; }
      /*! Divides xyz by w and returns that */
      Vector3 divide_w() const { return Vector3(x / w, y / w, z / w); }
      /*! Just ignores the w. This is correct for a distance where w==0. */
      Vector3 truncate_w() const { return Vector3(x, y, z); }

      /*! Multiplies all 4 numbers. This will multiply a distance. However if
         w is non-zero, the resulting 3D point will not move, because x/w
         will not change. */
      Vector4  operator *  (float d) const { return Vector4(x * d, y * d, z * d, w * d); }
      Vector4& operator *= (float d) { x *= d;
                                       y *= d;
                                       z *= d;
                                       w *= d;
                                       return *this; }
      /*! Divides all 4 numbers. */
      Vector4  operator /  (float d) const { return Vector4(x / d, y / d, z / d, w / d); }
      Vector4& operator /= (float d) { x /= d;
                                       y /= d;
                                       z /= d;
                                       w /= d;
                                       return *this; }
      /*! Component-wise multiplication, useful for colors. */
      Vector4  operator *  (const Vector4& v) const { return Vector4(x * v.x, y * v.y, z * v.z, w * v.w); }
      Vector4& operator *= (const Vector4& v) { x *= v.x;
                                                y *= v.y;
                                                z *= v.z;
                                                w *= v.w;
                                                return *this; }
      /*! Component-wise division, useful for colors. */
      Vector4  operator /  (const Vector4& v) const { return Vector4(x / v.x, y / v.y, z / v.z, w / v.w); }
      Vector4& operator /= (const Vector4& v) { x /= v.x;
                                                y /= v.y;
                                                z /= v.z;
                                                w /= v.w;
                                                return *this; }

      /*! Component-wise addition. */
      Vector4  operator +  (const Vector4& v) const { return Vector4(x + v.x, y + v.y, z + v.z, w + v.w); }
      Vector4& operator += (const Vector4& v) { x += v.x;
                                                y += v.y;
                                                z += v.z;
                                                w += v.w;
                                                return *this; }

      /*! Component-wise subtraction. */
      Vector4  operator -  () const { return Vector4(-x, -y, -z, -w); }
      Vector4  operator -  (const Vector4& v) const { return Vector4(x - v.x, y - v.y, z - v.z, w - v.w); }
      Vector4& operator -= (const Vector4& v) { x -= v.x;
                                                y -= v.y;
                                                z -= v.z;
                                                w -= v.w;
                                                return *this; }

      bool operator == (const Vector4& v) const { return IsEqual(x, v.x) && IsEqual(y, v.y) && IsEqual(z, v.z) && IsEqual(w, v.w); }
      bool operator != (const Vector4& v) const { return !IsEqual(x, v.x) || !IsEqual(y, v.y) || !IsEqual(z, v.z) || !IsEqual(w, v.w); }
      bool operator == (float d) const { return IsEqual(x, d) && IsEqual(y, d) && IsEqual(z, d) && IsEqual(w, d); }
      bool operator != (float d) const { return !IsEqual(x, d) || !IsEqual(y, d) || !IsEqual(z, d) || !IsEqual(w, d); }

      //! Fairly arbitrary operator so you can store these in ordered arrays
      bool operator< ( const Vector4& v ) const
      {
        if (x < v.x)
          return true;
        if (x > v.x)
          return false;
        if (y < v.y)
          return true;
        if (y > v.y)
          return false;
        if (z < v.z)
          return true;
        if (z > v.z)
          return false;
        return w < v.w;
      }


    };

}  

#endif

