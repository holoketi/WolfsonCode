/*! \class user::Quaternion Quaternion.h

   \brief A implementation of a way of representing rotations that avoid a lot
   of the problems that the standard rotation about the axis methods have.

   Quaternions are a modification of the concept of a vector in space,
   but specially tailored for spherical space. The cool thing about
   quaternions is that they are perfectly suited to representing
   rotations and orientations of objects in three space.

   Basically, in a quaternion there are four values: a scalar part and
   a vector part. <b>q</b> = ( s, <b>v</b> ). Typically, when dealing
   with rotations, the scalar part represents the rotation about an
   arbitrary axis. The axis is represented by a unit vector in the
   vector part.

   Since the quaternion is a representation of a rotation, it can be
   converted into a Euler angle rotation matrix and a rotation matrix
   can be converted into a quaternion.
 */

#ifndef DD_IMAGE_QUATERNION_H
#define DD_IMAGE_QUATERNION_H

#include <math.h>
#include <iostream>

#include "model/Vector3.h"
#include "model/Matrix4.h"

namespace user
{   
	class  Quaternion
    {
    public:
      double  s, vx, vy, vz;

      /*! \brief Returns the additive identity for quaternions (which is all
         zeros)
       */
      static Quaternion add_identity() { return Quaternion(0,0,0,0); }

      /*! \brief Returns the multipicative identity for quaternions (which is
         1,0,0,0)
       */
      static Quaternion mult_identity() { return Quaternion(1,0,0,0); }
      /*! \brief Default constructor.
       */

      Quaternion () :
        s(1), vx(0), vy(0), vz(0)
      {}

      /*! \brief Constructor for specifying values.
       */
      Quaternion (double a, double b, double c, double d) :
        s(a), vx(b), vy(c), vz(d)
      {}

      /*! \brief Set individual quaternion values.
       */
      void set(double a, double b, double c, double d)
      {
        s = a;
        vx = b;
        vy = c;
        vz = d;
      }

      /*! \brief This constructor takes an angle in radians and a vector to
         rotate around.
       */
      Quaternion ( double sval, const Vector3 &v ) {
        s = cos ( sval / 2.0 );

        double sang = sin ( sval / 2.0 );
        vx = sang * v.x;
        vy = sang * v.y;
        vz = sang * v.z;
      }

      /*! \brief Constructor. Given a Matrix that represents a rotation,
         calculate the quaternion that is equivalent to that rotation.
       */
      Quaternion ( const Matrix4 &mat );

      /*! \brief Constructor from two vectors. The quaternion will represent
         the angle between the two vectors.
       */
      Quaternion ( const Vector3 &org_vec, const Vector3 &new_vec );

      /*! \brief Addition of two quaternions. This follows this rule:

         <tt><b> q1 + q2 </b> = ( s1 + s2, vx1 + vx2, vy1 + vy2, vz1 + vz2 )</tt>
       */
      Quaternion  operator+ ( const Quaternion& q ) const
      {
        Quaternion ret_val;

        ret_val.s  = s + q.s;
        ret_val.vx = vx + q.vx;
        ret_val.vy = vy + q.vy;
        ret_val.vz = vz + q.vz;

        return ret_val;
      }

      /*! \brief Multiplication of two quaternions. This follows this rule:

         <pre><b>q1 q2</b> = ( s1 s2 - vx1 vx2 - vy1 vy2 - vz1 vz2,
         vy1 vz2 - vy2 vz1 + s1 vx2 + s2 vx1,
         vz1 vx2 - vz2 vx1 + s1 vy2 + s2 vy1,
         vx1 vy2 - vx2 vy1 + s1 vz2 + s2 vz1 ))</pre>

         (I think this is the same as doing the two rotations one after
         another?)
       */
      Quaternion  operator* ( const Quaternion& q ) const
      {
        Quaternion ret_val;
        ret_val.s = s * q.s - ( vx * q.vx + vy * q.vy + vz * q.vz );

        ret_val.vx = vy * q.vz - q.vy * vz + s * q.vx + q.s * vx;
        ret_val.vy = vz * q.vx - q.vz * vx + s * q.vy + q.s * vy;
        ret_val.vz = vx * q.vy - q.vx * vy + s * q.vz + q.s * vz;
        return ret_val;
      }

      /*! \brief Multiplication of a quaternion by a double number. This
         follows this rule:

         <tt><b> f * q </b> = ( f * s, f * vx, f * vy, f * vz )</tt>
       */
      Quaternion  operator* ( double f ) const
      {
        return Quaternion ( f * s, f * vx, f * vy, f * vz );
      }

      /*! \brief Returns the cojungate of this quaternion. This follows this
         rule:  <tt><b> q.conjugate </b> = ( s - <b> v </b> )</tt>
       */
      Quaternion  conjugate () const
      {
        return Quaternion ( s, -vx, -vy, -vz );
      }

      /*! \brief Returns (the square of?) the magnitude of the quaternion.
         This follows this rule:

         <tt><b> q.magnitude </b> = <b> q q.conjugate </b>
         = s^2 + vx^2 + vy^2 + vz^2 </tt>

         Huh? This does not match the code...
       */
      double magnitude () const
      {
        Quaternion mult = *this * this->conjugate();
        return mult.s + mult.vx + mult.vy + mult.vz;
      }

      /*! \brief Returns the additive inverse of the quaternion. This is:
         <tt><b> q.add_inverse </b> = ( -s, -vx, -vy, -vz ) </tt>
       */
      Quaternion add_inverse  () const
      {        return Quaternion ( -this->s, -this->vx, -this->vy, -this->vz ); }

      /*! \brief Returns the multiplicative inverse of the quaternion. This
         is:
         <tt><b> q.mult_inverse </b> =
         ( 1 / <b>q.magnitude</b> ) * <b> q.conjugate </b> </tt>
       */
      Quaternion mult_inverse () const
      {        return this->conjugate() * double(1 / this->magnitude()); }

      /*! \brief Spherical linear interpolation. This method interpolates
         smoothly between two quaternions. The value t should be a number
         between 0.0 and 1.0. When t = 0.0, *this is returned. When t = 1.0,
         end_quat is returned.
       */
      Quaternion slerp ( const Quaternion& end_quat, double t ) const;

      /*! \brief Return the transformation matrix that will represent the
         the Euler angle rotations that this quaternion embodies.
       */
      Matrix4 matrix () const;

	  void euler(double& rx, double& ry, double& rz);

      friend std::ostream&  operator<< (std::ostream& o, const Quaternion& q );

    };

} // user

#endif
