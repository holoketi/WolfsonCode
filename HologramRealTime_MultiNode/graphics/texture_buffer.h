#ifndef __texture_buffer_h
#define __texture_buffer_h

#include "graphics/sys.h"
#include "graphics/gl.h"
#include "graphics/gl_extension.h"
#include "graphics/unsigned.h"

namespace graphics {

class TextureBuffer{

public:

	TextureBuffer();
	virtual ~TextureBuffer();

	virtual void GenerateObjects(uint mag_filter = GL_NEAREST, uint min_filter = GL_NEAREST);
	virtual void Resize(int w, int h);

	void BeginDraw();
	void EndDraw();

	virtual void ReadData(void* ptr); // ptr should be memory-allocated before the call

	virtual void ReadData(void* ptr, uint pbo);

	virtual void ReadDepth(void* ptr);

	void getLinearDepth(int width, int height, void* depth);


	uint getFBO() { return fbo_id_; }

	int width() const { return width_; }
	int height() const { return height_; }

	uint texture_id() const { return texture_id_; }
protected:

	bool depth_test_;
	int	 width_;
	int  height_;
	uint rbo_id_;
	uint fbo_id_;
	uint texture_id_;
	uint pbo_id_;
};

};

#endif