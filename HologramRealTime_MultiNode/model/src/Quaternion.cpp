#include "model/Quaternion.h"
#include <graphics/quater.h>

using namespace graphics;
namespace user{

Quaternion::Quaternion (const Matrix4 &mat)
{
	real a = mat[0][0];
	real b = mat[1][1];
	real c = mat[2][2];
	real t = a + b + c + 1.0;

	real qx, qy, qz, qw;
	if (t > 0.0) {
		s = 0.5/sqrt(t);
		qw = 0.25/s;
		qx = (mat[2][1] - mat[1][2]) * s;
		qy = (mat[0][2] - mat[2][0]) * s;
		qz = (mat[1][0] - mat[0][1]) * s;
	}
	else if (a >= b && a >= c) {
		s = sqrt(1.0 + a - b - c) * 2.0;
		qx = 0.5/s;
		qy = (mat[0][1] + mat[1][0])/s;
		qz = (mat[0][2] + mat[2][0])/s;
		qw = (mat[1][2] + mat[2][1])/s;
	}
	else if (b >= a && b >= c) {
		s = sqrt(1.0 + b - c - a) * 2.0;
		qx = (mat[0][1] + mat[1][0])/s;
		qy = 0.5/s;
		qz = (mat[1][2] + mat[2][1])/s;
		qw = (mat[0][2] + mat[2][0])/s;
	}
	else if (c >= a && c >= b) {
		s = sqrt(1.0 + c - a - b) * 2.0;
		qx = (mat[0][2] + mat[2][0])/s;
		qy = (mat[1][2] + mat[2][1])/s;
		qz = 0.5/s;
		qw = (mat[0][1] + mat[1][0])/s;
	}

	s = qw;
	vx = qx;
	vy = qy;
	vz = qz;
}

Quaternion::Quaternion ( const Vector3 &org_vec, const Vector3 &new_vec )
{
	vec3 a(org_vec.x, org_vec.y, org_vec.z);
	vec3 b(new_vec.x, new_vec.y, new_vec.z);
	quater r = u2v_quater(a, b);
	s = r[0];
	vx = r[1];
	vy = r[2];
	vz = r[3];
}

Quaternion Quaternion::slerp ( const Quaternion& end_quat, double t ) const
{
	quater a(s, vx, vy, vz);
	quater b(end_quat.s, end_quat.vx, end_quat.vy, end_quat.vz);
	quater r = graphics::slerp(a, b, t);
	return Quaternion(r[0], r[1], r[2], r[3]);
}

Matrix4 Quaternion::matrix () const
{
	Matrix4 basis;
	basis.makeIdentity();

	real X = vx;
	real Y = vy;
	real Z = vz;
	real W = s;

	real xx      = X * X;
	real xy      = X * Y;
	real xz      = X * Z;
	real xw      = X * W;

	real yy      = Y * Y;
	real yz      = Y * Z;
	real yw      = Y * W;

	real zz      = Z * Z;
	real zw      = Z * W;

	basis[0][0]  = 1.0 - 2.0 * ( yy + zz );
	basis[0][1]  =     2.0 * ( xy - zw );
	basis[0][2]  =     2.0 * ( xz + yw );

	basis[1][0]  =     2.0 * ( xy + zw );
	basis[1][1]  = 1.0 - 2.0 * ( xx + zz );
	basis[1][2]  =     2.0 * ( yz - xw );

	basis[2][0]  =     2.0 * ( xz - yw );
	basis[2][1]  =     2.0 * ( yz + xw );
	basis[2][2]  = 1.0 - 2.0 * ( xx + yy );

	return basis;
}

void Quaternion::euler(double& rx, double& ry, double& rz)
{
	quater q(s, vx, vy, vz);
	vec3 v = graphics::euler(q);
	rx = -v[0];
	ry = -v[1];
	rz = -v[2];
	if (apx_equal(rx,0)) rx = 0;
	if (apx_equal(ry,0)) ry = 0;
	if (apx_equal(rz,0)) rz = 0;
}

}