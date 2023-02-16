#ifndef __texture_buffer_ms_h
#define __texture_buffer_ms_h

#include "graphics/sys.h"
#include "graphics/gl.h"
#include "graphics/gl_extension.h"
#include "graphics/unsigned.h"
#include "graphics/texture_buffer.h"

namespace graphics {

class TextureBufferMS: public TextureBuffer{ // multisample

public:

	TextureBufferMS();
	virtual ~TextureBufferMS();

	virtual void GenerateObjects(uint mag_filter = GL_NEAREST, uint min_filter = GL_NEAREST);
	virtual void Resize(int w, int h);


	virtual void ReadData(void* ptr); // ptr should be memory-allocated before the call

	virtual void ReadData(void* ptr, uint pbo);

	bool IsValid() const { return rbo_id_ != 0; }


protected:

	void make_temp_fbo(uint& fbo, uint& rbo, uint& tx);
	void delete_temp_fbo(uint& fbo, uint& rbo, uint& tx);


};

};

#endif