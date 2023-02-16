#include	"graphics/texture_buffer_ms.h"

namespace graphics {

TextureBufferMS::TextureBufferMS()
	: TextureBuffer()
{
}


TextureBufferMS::~TextureBufferMS()
{
}

void 
TextureBufferMS::GenerateObjects(uint mag_filter, uint min_filter)
{
	glGenTextures(1, &texture_id_);

	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture_id_);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE,4, GL_RGBA8, width_, height_,GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	glGenRenderbuffersEXT(1, &rbo_id_);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, rbo_id_);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT,
		width_, height_);
	glBindRenderbufferEXT(GL_RENDERBUFFER, 0);

	glGenFramebuffersEXT(1, &fbo_id_);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo_id_);
	

	// attach the texture to FBO color attachment point
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
		GL_TEXTURE_2D_MULTISAMPLE, texture_id_, 0);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
		GL_RENDERBUFFER_EXT, rbo_id_);


	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
	
	
	
}

void
TextureBufferMS::Resize(int w, int h)
{
	if (width_ == w && height_ == h) return;

	width_ = w; height_ = h;

	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture_id_);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, width_, height_, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);


	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, rbo_id_);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT,
		width_, height_);
	glBindRenderbufferEXT(GL_RENDERBUFFER, 0);

	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void TextureBufferMS::make_temp_fbo(uint& fbo, uint& rbo, uint& tx)
{
	glGenTextures(1, &tx);

	glBindTexture(GL_TEXTURE_2D, tx);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width_, height_, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);



	//glGenRenderbuffersEXT(1, &rbo);
	//glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, rbo);
	//glRenderbufferStorageEXT(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
	//	width_, height_);
	//glBindRenderbufferEXT(GL_RENDERBUFFER, 0);

	glGenFramebuffersEXT(1, &fbo);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

	// attach the texture to FBO color attachment point
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
		GL_TEXTURE_2D, tx, 0);
	/*glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
		GL_RENDERBUFFER_EXT, rbo);*/

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
}

void TextureBufferMS::delete_temp_fbo(uint& fbo, uint& rbo, uint& tx)
{
	if (!fbo)
		glDeleteFramebuffersEXT(1, &fbo);

	if (!rbo)
		glDeleteRenderbuffersEXT(1, &rbo);

	if (!tx)
		glDeleteTextures(1, &tx);
}

void TextureBufferMS::ReadData(void* ptr, uint pbo)
{
	uint tmp_fbo = 0, tmp_rbo = 0, tmp_tx = 0;
	make_temp_fbo(tmp_fbo, tmp_rbo, tmp_tx);

	if (!pbo) glGenBuffersARB(1, &pbo);
	glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, fbo_id_);
	glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, tmp_fbo);
	glBlitFramebufferEXT(0, 0, width_, height_, 0, 0, width_, height_, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, 0);
	glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, 0);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, tmp_fbo);
	glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, pbo);
	
	glBufferDataARB(GL_PIXEL_PACK_BUFFER_ARB, width_* height_*4, 0, GL_STREAM_READ_ARB);
	glReadPixels(0, 0, width_, height_, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	GLubyte* src = (GLubyte*)glMapBufferARB(GL_PIXEL_PACK_BUFFER_ARB, GL_READ_ONLY_ARB);
	if(src)
	{
		memcpy(ptr, src, width_* height_*4);
		glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB); // release the mapped buffer
	}

	glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, 0);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	delete_temp_fbo(tmp_fbo, tmp_rbo, tmp_tx);
}

void TextureBufferMS::ReadData(void* ptr)
{
	uint tmp_fbo = 0, tmp_rbo = 0, tmp_tx = 0;
	make_temp_fbo(tmp_fbo, tmp_rbo, tmp_tx);

	if (!pbo_id_) glGenBuffersARB(1, &pbo_id_);

	glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, fbo_id_);
	glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, tmp_fbo);
	glBlitFramebufferEXT(0, 0, width_, height_, 0, 0, width_, height_, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, 0);
	glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, 0);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, tmp_fbo);
	glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, pbo_id_);
	

	glBufferDataARB(GL_PIXEL_PACK_BUFFER_ARB, width_* height_*4, 0, GL_STREAM_READ_ARB);
	glReadPixels(0, 0, width_, height_, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	GLubyte* src = (GLubyte*)glMapBufferARB(GL_PIXEL_PACK_BUFFER_ARB, GL_READ_ONLY_ARB);
	if(src)
	{
		memcpy(ptr, src, width_* height_*4);
		glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB); // release the mapped buffer
	}

	glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, 0);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	delete_temp_fbo(tmp_fbo, tmp_rbo, tmp_tx);
}


};