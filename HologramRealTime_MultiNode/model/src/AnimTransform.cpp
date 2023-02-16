#include "model/AnimTransform.h"

namespace model_kernel {


//
// Transform
//


Transform::Transform()
:	sid()
{}

Transform::~Transform()
{
}

//
// TTranslation
//

Translation::Translation()
:	Transform(), translation(0)
{
}

Translation::~Translation() {}

Transform* Translation::Clone(Transform* _clone) const
{
	Translation* clone = NULL;
	if (_clone == NULL) clone = new Translation();
	else if (_clone->GetType() != Translation::GetType()) return _clone;
	else clone = (Translation*) _clone;

	clone->translation = translation;
	return clone;
}

user::Matrix4 Translation::ToMatrix() const
{
	user::Matrix4 ret = user::Matrix4::identity();
	ret.translation() = user::Vector3(translation[0], translation[1], translation[2]);
	return ret;
}

//
// TRotation
//


Rotation::Rotation()
:	Transform(), axis(1,0,0), angle(0)
{
}

Rotation::~Rotation() {}

Transform* Rotation::Clone(Transform* _clone) const
{
	Rotation* clone = NULL;
	if (_clone == NULL) clone = new Rotation();
	else if (_clone->GetType() != Rotation::GetType()) return _clone;
	else clone = (Rotation*) _clone;

	clone->angle = angle;
	clone->axis = axis;
	return clone;
}

user::Matrix4 Rotation::ToMatrix() const
{
	user::Matrix4 rot;
	rot.rotation(graphics::radian(angle),axis[0],axis[1],axis[2]);
	return rot;
}


//
// TScale
//


Scale::Scale()
:	Transform(), scale(1,1,1)
{
}

Scale::~Scale() {}

Transform* Scale::Clone(Transform* _clone) const
{
	Scale* clone = NULL;
	if (_clone == NULL) clone = new Scale();
	else if (_clone->GetType() != Scale::GetType()) return _clone;
	else clone = (Scale*) _clone;

	clone->scale = scale;
	return clone;
}

user::Matrix4 Scale::ToMatrix() const
{
	user::Matrix4 sc;
	sc.scaling(scale[0],scale[1],scale[2]);
	return sc;
}

//
// TMatrix
//


Matrix::Matrix()
	:	Transform(), transform(user::Matrix4::identity())
{
}

Matrix::~Matrix() {}

Transform* Matrix::Clone(Transform* _clone) const
{
	Matrix* clone = NULL;
	if (_clone == NULL) clone = new Matrix();
	else if (_clone->GetType() != Matrix::GetType()) return _clone;
	else clone = (Matrix*) _clone;

	clone->transform = transform;
	return clone;
}



}