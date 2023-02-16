#include "model/GFrame.h"
#include <graphics/gl.h>
namespace model_kernel {


GFrame& GFrame::operator = (const GFrame& a) {

    for(int i = 0 ; i < 3; i++)
	   basis[i] = a.basis[i];

    eye_position = a.eye_position;

	scale = a.scale;


	memcpy(worldMatrix, a.worldMatrix, sizeof(real)*16);
	memcpy(inverseWorldMatrix, a.inverseWorldMatrix, sizeof(real)*16);

    return *this;
}


frame& GFrame::operator = (const frame& a) {

    for(int i = 0 ; i < 3; i++)
	   basis[i] = a.basis[i];

    eye_position = a.eye_position;

	memcpy(worldMatrix, a.worldMatrix, sizeof(real)*16);
	memcpy(inverseWorldMatrix, a.inverseWorldMatrix, sizeof(real)*16);

    return *this;
}


box3 GFrame::Transform(const box3& input) const
{
	vec3 min_ = input.get_minimum();
	vec3 max_ = input.get_maximum();
	
	min_ = min_ * scale;
	max_ = max_ * scale;
	min_ = to_world(min_);
	max_ = to_world(max_);

	return box3(min_, max_);
}
};