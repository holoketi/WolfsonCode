#ifndef __depth_texture_buffer_h
#define __depth_texture_buffer_h

#include "graphics/sys.h"
#include "graphics/gl.h"
#include "graphics/gl_extension.h"
#include "graphics/unsigned.h"

namespace graphics {

class DepthTextureBuffer{

public:

	DepthTextureBuffer();
	virtual ~DepthTextureBuffer();

	void GenerateObjects(uint mag_filter = GL_NEAREST, uint min_filter = GL_NEAREST);
	virtual void Resize(int w, int h);

	void BeginDraw();
	void EndDraw();

	virtual void ReadData(void* ptr); // ptr should be memory-allocated before the call

	uint getFBO() { return fbo_id_; }

	int width() const { return width_; }
	int height() const { return height_; }

	uint texture_id() const { return texture_id_; }
	uint depth_texture_id() const { return depth_texture_id_;  }
protected:

	bool depth_test_;
	int	 width_;
	int  height_;
	uint fbo_id_;
	uint texture_id_;
	uint depth_texture_id_;
	uint pbo_id_;
};

};

#endif