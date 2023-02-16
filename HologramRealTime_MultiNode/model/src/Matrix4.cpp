#include "model/Matrix4.h"
#include "model/Quaternion.h"
#include <graphics/vec.h>
#include <graphics/epsilon.h>
#include <graphics/homography.h>
#include <graphics/gHash.h>
#include <graphics/EulerAngles.h>

namespace user {


const Matrix4 Matrix4::_identity(1, 0, 0, 0,
	                             0, 1, 0, 0,
								 0, 0, 1, 0,
								 0, 0, 0, 1);

void Matrix4::translation(float x, float y, float z)
{
	makeIdentity();
	a03 = x;
	a13 = y;
	a23 = z;
}


void Matrix4::rotationX(float ang)
{
	float c = (float) cos(ang);   
	float s = (float) sin(ang);  
	makeIdentity();
	a11 = c;
	a21 = s;
	a12 = -s;
	a22 = c;
}

void Matrix4::rotationY(float ang)
{
	float c = (float) cos(ang);   
	float s = (float) sin(ang);  
	makeIdentity();
	a00 = c;   
	a02 = s;   
	a20 = -s;   
	a22 = c;  
}
void Matrix4::rotationZ(float ang)
{
	float c = (float) cos(ang);   
	float s = (float) sin(ang);  
	makeIdentity();
	a00 = c;   
	a01 = -s;   
	a10 = s;   
	a11 = c; 
}

void Matrix4::rotation(float a, float x, float y, float z)
{
	Vector3 axis(x, y, z);
	axis.normalize();

	float s = (float)sin( a );   
	float c = (float)cos( a );   
	x = axis.x, y = axis.y, z = axis.z;   

	makeIdentity();

	a00 = x*x*(1-c)+c;   
	a01 = x*y*(1-c)-(z*s);   
	a02 = x*z*(1-c)+(y*s);   
	a03 = 0;   
	a10 = y*x*(1-c)+(z*s);   
	a11 = y*y*(1-c)+c;   
	a12 = y*z*(1-c)-(x*s);   
	a13 = 0;   
	a20 = z*x*(1-c)-(y*s);   
	a21 = z*y*(1-c)+(x*s);   
	a22 = z*z*(1-c)+c;   
	a23 = 0;   
	a30 = 0;   
	a31 = 0;   
	a32 = 0;   
	a33 = 1; 
}


void Matrix4::scaling(float v)
{
	makeIdentity();
	a00 = v;   
	a11 = v;   
	a22 = v;   
}
void Matrix4::scaling(float v1, float v2, float v3)
{
	makeIdentity();
	a00 = v1;   
	a11 = v2;   
	a22 = v3;   
}

void Matrix4::transpose()
{
	float tmp;
	tmp = a01;
	a01 = a10;
	a10 = tmp;

	tmp = a02;
	a02 = a20;
	a20 = tmp;

	tmp = a03;
	a03 = a30;
	a30 = tmp;

	tmp = a12;
	a12 = a21;
	a21 = tmp;

	tmp = a13;
	a13 = a31;
	a31 = tmp;

	tmp = a23;
	a23 = a32;
	a32 = tmp;
}

void Matrix4::scale(float s)
{
	Matrix4 m;
	m.scaling(s);
	*this *= m;
}

void Matrix4::scale(float xs, float ys, float zs)
{
	Matrix4 m;
	m.scaling(xs, ys, zs);
	*this *= m;
}

Matrix4  Matrix4::operator *  ( const Matrix4& b) const
{
	Matrix4 ret;
	ret.a00 = a00*b.a00 + a01*b.a10 + a02*b.a20 + a03*b.a30;
	ret.a01 = a00*b.a01 + a01*b.a11 + a02*b.a21 + a03*b.a31;
	ret.a02 = a00*b.a02 + a01*b.a12 + a02*b.a22 + a03*b.a32;
	ret.a03 = a00*b.a03 + a01*b.a13 + a02*b.a23 + a03*b.a33;

	ret.a10 = a10*b.a00 + a11*b.a10 + a12*b.a20 + a13*b.a30;
	ret.a11 = a10*b.a01 + a11*b.a11 + a12*b.a21 + a13*b.a31;
	ret.a12 = a10*b.a02 + a11*b.a12 + a12*b.a22 + a13*b.a32;
	ret.a13 = a10*b.a03 + a11*b.a13 + a12*b.a23 + a13*b.a33;

	ret.a20 = a20*b.a00 + a21*b.a10 + a22*b.a20 + a23*b.a30;
	ret.a21 = a20*b.a01 + a21*b.a11 + a22*b.a21 + a23*b.a31;
	ret.a22 = a20*b.a02 + a21*b.a12 + a22*b.a22 + a23*b.a32;
	ret.a23 = a20*b.a03 + a21*b.a13 + a22*b.a23 + a23*b.a33;

	ret.a30 = a30*b.a00 + a31*b.a10 + a32*b.a20 + a33*b.a30;
	ret.a31 = a30*b.a01 + a31*b.a11 + a32*b.a21 + a33*b.a31;
	ret.a32 = a30*b.a02 + a31*b.a12 + a32*b.a22 + a33*b.a32;
	ret.a33 = a30*b.a03 + a31*b.a13 + a32*b.a23 + a33*b.a33;
	return ret;
}

Matrix4& Matrix4::operator *= ( const Matrix4& b)
{
	Matrix4 ret;
	ret = *this * b;
	*this = ret;
	return *this;
}

Matrix4& Matrix4::operator = ( const Matrix4& a)
{
	memcpy(&a00, &a[0][0], sizeof(float)*16);
	return *this;
}

Matrix4  Matrix4::operator +  ( const Matrix4& b) const
{
	Matrix4 ret;
	for (int i = 0 ; i < 16 ;++i)			(&ret.a00)[i] = (&a00)[i] + (&b.a00)[i];

	return ret;
}

Matrix4& Matrix4::operator += ( const Matrix4& b)
{
	for (int i = 0 ; i < 16 ;++i)		(&a00)[i] = (&a00)[i] + (&b.a00)[i];
	return *this;
}

Matrix4  Matrix4::operator -  ( const Matrix4& b) const
{
	Matrix4 ret;
	for (int i = 0 ; i < 16 ;++i)		(&ret.a00)[i] = (&a00)[i] - (&b.a00)[i];

	return ret;
}

Matrix4& Matrix4::operator -= ( const Matrix4& b)
{
	for (int i = 0 ; i < 16 ;++i)		(&a00)[i] = (&a00)[i] - (&b.a00)[i];
	return *this;
}


Matrix4  Matrix4::operator *  (float b) const
{
	Matrix4 ret;
	for (int i = 0 ; i < 16 ;++i)		(&ret.a00)[i] = (&a00)[i] * b;

	return ret;
}

Matrix4&  Matrix4::operator *= (float b)
{
	for (int i = 0 ; i < 16 ;++i)		(&a00)[i] = (&a00)[i] * b;
	return *this;
}


 Matrix4 Matrix4::inverse(float det) const
 {
	 Matrix4 inv;
	 Matrix4& m = * const_cast<Matrix4*>(this);

	 inv[0][0]=

	 m[1][1]*m[2][2]*m[3][3] 

	 -m[1][1]*m[2][3]*m[3][2] 

	 -m[2][1]*m[1][2]*m[3][3] 

	 +m[2][1]*m[1][3]*m[3][2]

	 +m[3][1]*m[1][2]*m[2][3] 

	 -m[3][1]*m[1][3]*m[2][2];

	 inv[0][1]=

		 -m[0][1]*m[2][2]*m[3][3] 

	 +m[0][1]*m[2][3]*m[3][2] 

	 +m[2][1]*m[0][2]*m[3][3] 

	 -m[2][1]*m[0][3]*m[3][2]

	 -m[3][1]*m[0][2]*m[2][3] 

	 +m[3][1]*m[0][3]*m[2][2];

	 inv[0][2]=

		 m[0][1]*m[1][2]*m[3][3] 

	 -m[0][1]*m[1][3]*m[3][2] 

	 -m[1][1]*m[0][2]*m[3][3] 

	 +m[1][1]*m[0][3]*m[3][2]

	 +m[3][1]*m[0][2]*m[1][3] 

	 -m[3][1]*m[0][3]*m[1][2];

	 inv[0][3]=

		 -m[0][1]*m[1][2]*m[2][3] 

	 +m[0][1]*m[1][3]*m[2][2] 

	 +m[1][1]*m[0][2]*m[2][3] 

	 -m[1][1]*m[0][3]*m[2][2]

	 -m[2][1]*m[0][2]*m[1][3] 

	 +m[2][1]*m[0][3]*m[1][2];

	 inv[1][0]=

		 -m[1][0]*m[2][2]*m[3][3] 

	 +m[1][0]*m[2][3]*m[3][2] 

	 +m[2][0]*m[1][2]*m[3][3] 

	 -m[2][0]*m[1][3]*m[3][2]

	 -m[3][0]*m[1][2]*m[2][3] 

	 +m[3][0]*m[1][3]*m[2][2];

	 inv[1][1]=

		 m[0][0]*m[2][2]*m[3][3] 

	 -m[0][0]*m[2][3]*m[3][2] 

	 -m[2][0]*m[0][2]*m[3][3] 

	 +m[2][0]*m[0][3]*m[3][2]

	 +m[3][0]*m[0][2]*m[2][3] 

	 -m[3][0]*m[0][3]*m[2][2];

	 inv[1][2]=

		 -m[0][0]*m[1][2]*m[3][3] 

	 +m[0][0]*m[1][3]*m[3][2] 

	 +m[1][0]*m[0][2]*m[3][3] 

	 -m[1][0]*m[0][3]*m[3][2]

	 -m[3][0]*m[0][2]*m[1][3] 

	 +m[3][0]*m[0][3]*m[1][2];

	 inv[1][3]=

		 m[0][0]*m[1][2]*m[2][3] 

	 -m[0][0]*m[1][3]*m[2][2] 

	 -m[1][0]*m[0][2]*m[2][3] 

	 +m[1][0]*m[0][3]*m[2][2]

	 +m[2][0]*m[0][2]*m[1][3] 

	 -m[2][0]*m[0][3]*m[1][2];

	 inv[2][0]=

		 m[1][0]*m[2][1]*m[3][3] 

	 -m[1][0]*m[2][3]*m[3][1] 

	 -m[2][0]*m[1][1]*m[3][3] 

	 +m[2][0]*m[1][3]*m[3][1]

	 +m[3][0]*m[1][1]*m[2][3] 

	 -m[3][0]*m[1][3]*m[2][1];

	 inv[2][1]=

		 -m[0][0]*m[2][1]*m[3][3] 

	 +m[0][0]*m[2][3]*m[3][1] 

	 +m[2][0]*m[0][1]*m[3][3] 

	 -m[2][0]*m[0][3]*m[3][1]

	 -m[3][0]*m[0][1]*m[2][3] 

	 +m[3][0]*m[0][3]*m[2][1];

	 inv[2][2]=

		 m[0][0]*m[1][1]*m[3][3] 

	 -m[0][0]*m[1][3]*m[3][1] 

	 -m[1][0]*m[0][1]*m[3][3] 

	 +m[1][0]*m[0][3]*m[3][1]

	 +m[3][0]*m[0][1]*m[1][3] 

	 -m[3][0]*m[0][3]*m[1][1];

	 inv[2][3]=

		 -m[0][0]*m[1][1]*m[2][3] 

	 +m[0][0]*m[1][3]*m[2][1] 

	 +m[1][0]*m[0][1]*m[2][3] 

	 -m[1][0]*m[0][3]*m[2][1]

	 -m[2][0]*m[0][1]*m[1][3] 

	 +m[2][0]*m[0][3]*m[1][1];

	 inv[3][0]=

		 -m[1][0]*m[2][1]*m[3][2] 

	 +m[1][0]*m[2][2]*m[3][1] 

	 +m[2][0]*m[1][1]*m[3][2] 

	 -m[2][0]*m[1][2]*m[3][1]

	 -m[3][0]*m[1][1]*m[2][2] 

	 +m[3][0]*m[1][2]*m[2][1];

	 inv[3][1]=

		 m[0][0]*m[2][1]*m[3][2] 

	 -m[0][0]*m[2][2]*m[3][1] 

	 -m[2][0]*m[0][1]*m[3][2] 

	 +m[2][0]*m[0][2]*m[3][1]

	 +m[3][0]*m[0][1]*m[2][2] 

	 -m[3][0]*m[0][2]*m[2][1];

	 inv[3][2]=

		 -m[0][0]*m[1][1]*m[3][2] 

	 +m[0][0]*m[1][2]*m[3][1] 

	 +m[1][0]*m[0][1]*m[3][2] 

	 -m[1][0]*m[0][2]*m[3][1]

	 -m[3][0]*m[0][1]*m[1][2] 

	 +m[3][0]*m[0][2]*m[1][1];

	 inv[3][3]=

		 m[0][0]*m[1][1]*m[2][2] 

	 -m[0][0]*m[1][2]*m[2][1] 

	 -m[1][0]*m[0][1]*m[2][2] 

	 +m[1][0]*m[0][2]*m[2][1]

	 +m[2][0]*m[0][1]*m[1][2]

	 -m[2][0]*m[0][2]*m[1][1];

	 inv /= det;
	 return inv;
 }

 void Matrix4::translate(float x, float y, float z)
 {
	 Matrix4 t;
	 t.translation(x, y, z);
	 *this *= t;
 }


 void Matrix4::rotateX(float a)
 {
	 Matrix4 t;
	 t.rotationX(a);
	 *this *= t;
 }

 void Matrix4::rotateY(float a)
 {
	 Matrix4 t;
	 t.rotationY(a);
	 *this *= t;
 }

 void Matrix4::rotateZ(float a)
 {
	 Matrix4 t;
	 t.rotationZ(a);
	 *this *= t;
 }


 void Matrix4::rotate(float a, float x, float y, float z)
 {
	 Matrix4 t;
	 t.rotation(a, x, y, z);
	 *this *= t;
 }

 void Matrix4::skew(float a)
 {
	 a01 = a01 + a00 * a;
	 a11 = a11 + a10 * a;
	 a21 = a21 + a20 * a;
 }

 void Matrix4::unskew(float s)
 {
	 a01 = a01 - a00 * s;
	 a11 = a11 - a10 * s;
	 a21 = a21 - a20 * s;
 }

 void Matrix4::skewXY(float x, float y)
 {
	 a01 = a01 + a00 * x;
	 a11 = a11 + a10 * x;
	 a21 = a21 + a20 * x;

	 a00 = a00 + a01 * y;
	 a10 = a10 + a11 * y;
	 a20 = a20 + a21 * y;
 }

 Vector3 Matrix4::scale() const
 {
	 return Vector3(sqrtf(a00*a00 + a10*a10 + a20*a20), sqrtf(a01*a01 + a11*a11 + a21*a21), sqrtf(a02*a02 + a12*a12 + a22*a22));
 }

 void Matrix4::scaleOnly()
 {
	 Vector3 s=scale();
	 scaling(s);
 }

 void Matrix4::rotationOnly()
 {
	 Vector3 x = x_axis();
	 Vector3 y = y_axis();
	 Vector3 z = z_axis();
	 x.normalize();
	 y.normalize();
	 z.normalize();
	 makeIdentity();
	 memcpy(&a00, x.array(), sizeof(float)*3);
	 memcpy(&a01, y.array(), sizeof(float)*3);
	 memcpy(&a02, z.array(), sizeof(float)*3);
 }

 void Matrix4::translationOnly()
 {
	 Vector3 t = translation();
	 translation(t);
 }

 void Matrix4::scaleAndRotationOnly()
 {
	 Vector3 x = x_axis();
	 Vector3 y = y_axis();
	 Vector3 z = z_axis();

	 makeIdentity();
	 memcpy(&a00, x.array(), sizeof(float)*3);
	 memcpy(&a01, y.array(), sizeof(float)*3);
	 memcpy(&a02, z.array(), sizeof(float)*3);
 }

 bool Matrix4::rotationsZXY(float& rx, float& ry, float& rz) const
 {
	 Matrix4 mat= *this;
	 mat.rotationOnly();
	 bool flip = false;
	 if (mat.determinant() < 0.0) {
		 flip = true;
		 mat.z_axis() *= -1;
	 }

	 HMatrix mat_type;
	 for (int a = 0 ; a < 3; a++)
		 for (int b = 0 ; b < 3 ; b++) 
			 mat_type[b][a] = mat[a][b];
	 EulerAngles outAngs = Eul_FromHMatrix(mat_type, EulOrdXYZs);
	 rx = outAngs.x;
	 ry = outAngs.y;
	 rz = outAngs.z;

	 return flip;
 }

 void Matrix4::euler_rotate(float x, float y, float z)
 {
	 EulerAngles inAngs = { 0, 0, 0, EulOrdXYZs };
	inAngs.x = x;
	inAngs.y = y;
	inAngs.z = z;

	 HMatrix R;
	 Eul_ToHMatrix(inAngs, R);
	 for (int a = 0 ; a < 3 ; a++)
		 for (int b = 0 ; b < 3 ; b++) {
			 (*this)[b][a] = R[a][b];
		 }
 }
 void Matrix4::mapUnitSquareToQuad( float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3 )
 {
	 graphics::vec2 p1(x0, y0);
	 graphics::vec2 p2(x1, y1);
	 graphics::vec2 p3(x2, y2);
	 graphics::vec2 p4(x3, y3);

	 graphics::vec2 o1(0,0);
	 graphics::vec2 o2(1,0);
	 graphics::vec2 o3(1,1);
	 graphics::vec2 o4(0,1);

	 std::vector<graphics::vec2> xy;
	 xy.push_back(p1);
	 xy.push_back(p2);
	 xy.push_back(p3);
	 xy.push_back(p4);
	 std::vector<graphics::vec2> uv;
	 uv.push_back(o1);
	 uv.push_back(o2);
	 uv.push_back(o3);
	 uv.push_back(o4);

	 graphics::Homography h(uv, xy);

	 std::vector<double>& out = h.GetMatrix();
	 makeIdentity();
	 if (!h.IsHomographyDefined()) return;
	 a00=out[0]/out[8];
	 a01=out[1]/out[8];
	 a03=out[2]/out[8];
	 a10=out[3]/out[8];
	 a11=out[4]/out[8];
	 a13=out[5]/out[8];
	 a30=out[6]/out[8];
	 a31=out[7]/out[8];
	 a33=1.0;
 }

void Matrix4::mapQuadToUnitSquare( float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3 )
{
	graphics::vec2 p1(x0, y0);
	graphics::vec2 p2(x1, y1);
	graphics::vec2 p3(x2, y2);
	graphics::vec2 p4(x3, y3);

	graphics::vec2 o1(0,0);
	graphics::vec2 o2(1,0);
	graphics::vec2 o3(1,1);
	graphics::vec2 o4(0,1);

	std::vector<graphics::vec2> xy;
	xy.push_back(p1);
	xy.push_back(p2);
	xy.push_back(p3);
	xy.push_back(p4);
	std::vector<graphics::vec2> uv;
	uv.push_back(o1);
	uv.push_back(o2);
	uv.push_back(o3);
	uv.push_back(o4);

	// this should be exchanged! malbehavior of Nuke!
	graphics::Homography h(uv, xy);

	std::vector<double>& out = h.GetMatrix();

	makeIdentity();

	if (!h.IsHomographyDefined()) return;
	a00=out[0]/out[8];
	a01=out[1]/out[8];
	a03=out[2]/out[8];
	a10=out[3]/out[8];
	a11=out[4]/out[8];
	a13=out[5]/out[8];
	a30=out[6]/out[8];
	a31=out[7]/out[8];
	a33=1.0;
}

void Matrix4::projection(float lens, float minz, float maxz, bool persp)
{

	float r = minz/(2.0*lens);
	memset(&a00, 0, sizeof(float)*16);
	if (persp) {
		a00=a11=minz/r;
		a22=-(maxz+minz)/(maxz-minz);
		a23=(-2.0*minz*maxz)/(maxz-minz);
		a32=-1.0;
	}
	else {
		a00=a11=1.0/r;
		a22=-2.0/(maxz-minz);
		a23=-(maxz+minz)/(maxz-minz);
		a33=1.0;
	}
}

}