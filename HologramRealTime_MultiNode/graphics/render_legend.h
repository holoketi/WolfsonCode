#ifndef __render_legend_h
#define __render_legend_h

#include "graphics/Shader.h"


namespace graphics {

class RenderLegend: public Shader {

public:

	RenderLegend();

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