#pragma once

#include <model/kernel.h>
#include <graphics/vec.h>
#include <graphics/frame.h>


using namespace graphics;

namespace model_kernel {

struct GFrame: public frame {

	vec3 scale;

	GFrame(): frame(), scale(1)
	{
	}

	GFrame(const vec3 &x, const vec3 &y, const vec3 &z, const vec3 &org)
	: frame(org, x, y, z), scale(1)
	{}

	GFrame(const vec3 &dir, const vec3 &up)
		: frame(dir, up), scale(1)
	{
	}

	GFrame(const GFrame &val)
		: frame(val), scale(val.scale)
	{
	}

	virtual frame& operator = (const frame& a) ;
	 virtual GFrame& operator = (const GFrame& a) ;

	 

	 void SetScale(vec3& s) { scale = s; }

	 /** Transform the input bounding box using this */
	 box3 Transform(const box3& input) const;

};
};