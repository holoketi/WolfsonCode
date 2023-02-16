#ifndef RENDERSCALARBW_H_
#define RENDERSCALARBW_H_


#include "graphics/Shader.h"


namespace graphics {

class RenderScalarBW: public Shader {

public:

	RenderScalarBW();

	virtual void Initialize();

	void BeginShader();

	void SetNumberOfLegend(int n);

		// 0~1������ �����е� �ε��Ҽ�
	void SetHSVBrightness(float d);
	void SetTransparency(float d);

protected:

	int	   number_of_legend_;
	float hsv_brightness_;
	float transparency_;
};

};

#endif