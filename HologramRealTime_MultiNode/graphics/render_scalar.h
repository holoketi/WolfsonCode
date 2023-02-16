#ifndef __render_scalar_h
#define __render_scalar_h


#include "graphics/Shader.h"


namespace graphics {

class RenderScalar: public Shader {

public:

	RenderScalar();

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