#include	"graphics/depth_texture_buffer.h"

namespace graphics {

DepthTextureBuffer::DepthTextureBuffer()
	: width_(100), height_(100), fbo_id_(0), pbo_id_(0), texture_id_(0), depth_texture_id_(0)
{
}


DepthTextureBuffer::~DepthTextureBuffer()
{
	if (fbo_id_) {
		glDeleteFramebuffersEXT(1, &fbo_id_);
	}


	if (texture_id_)
		glDeleteTextures(1, &texture_id_);
	
	if (depth_texture_id_)
		glDeleteTextures(1, &depth_texture_id_);

	if (pbo_id_) {
		glDeleteBuffersARB(1, &pbo_id_);
	}

}

void 
DepthTextureBuffer::GenerateObjects(uint mag_filter, uint min_filter)
{
	glGenTextures(1, &texture_id_);

	glBindTexture(GL_TEXTURE_2D, texture_id_);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width_, height_, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, 0);


	glGenTextures(1, &depth_texture_id_);
	glBindTexture(GL_TEXTURE_2D, depth_texture_id_);
	glBindTexture(GL_TEXTURE_2D, depth_texture_id_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width_, height_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &fbo_id_);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_id_);
	

	// attach the texture to FBO color attachment point
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
							  texture_id_, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
							  depth_texture_id_, 0);


	//std::vector<GLenum> drawbuffer;
	//drawbuffer.push_back(GL_COLOR_ATTACHMENT0);
	//glDrawBuffers(drawbuffer.size(), &drawbuffer[0]);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void DepthTextureBuffer::ReadData(void* ptr)
{

	if (!pbo_id_) glGenBuffersARB(1, &pbo_id_);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo_id_);
	glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, pbo_id_);


	glBufferDataARB(GL_PIXEL_PACK_BUFFER_ARB, width_* height_ * 4, 0, GL_STREAM_READ_ARB);
	glReadPixels(0, 0, width_, height_, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	GLubyte* src = (GLubyte*)glMapBufferARB(GL_PIXEL_PACK_BUFFER_ARB, GL_READ_ONLY_ARB);
	if (src)
	{
		memcpy(ptr, src, width_* height_ * 4);
		glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB); // release the mapped buffer
	}

	glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, 0);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

}
void
DepthTextureBuffer::Resize(int w, int h)
{
	if (width_ == w && height_ == h) return;

	width_ = w; height_ = h;

	glBindTexture(GL_TEXTURE_2D, texture_id_);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width_, height_, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, depth_texture_id_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width_, height_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DepthTextureBuffer::BeginDraw()
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_id_);

}


void DepthTextureBuffer::EndDraw()
{
	// unbind FBO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


};