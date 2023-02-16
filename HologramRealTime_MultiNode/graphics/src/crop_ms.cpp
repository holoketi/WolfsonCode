#include "graphics/crop_ms.h"


#include <iostream>
#include <fstream>
#include <string>
#include <graphics/texture_buffer_ms.h>


using namespace std;


namespace graphics {

#define STRINGIZE(a) #a

	CropMS::CropMS()
	: Shader()
{

}
	void CropMS::BeginShader()
{
	Shader::BeginShader();
	InitUniformVariables();
}

	void CropMS::InitUniformVariables()
{

	int loc;

	loc = glGetUniformLocationARB(shader_id_, "scene");
	glUniform1iARB(loc, 0);
	glActiveTextureARB(GL_TEXTURE0_ARB + 0);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, input_->texture_id());

}

uint CropMS::result()
{
	if (dynamic_cast<TextureBufferMS*>(input_) == 0) return input_->texture_id();
	return output_->texture_id();
}

void CropMS::crop()
{
	if (dynamic_cast<TextureBufferMS*>(input_) == 0) return;

	int h = input_->height();
	int w = input_->width();
	output_->BeginDraw();
	glViewport(0, 0, w, h);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);

	
	BeginShader();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(0, 0, 0);
	glTexCoord2f(1.0, 0.0); glVertex3f(w, 0, 0);
	glTexCoord2f(1.0, 1.0); glVertex3f(w, h, 0);
	glTexCoord2f(0.0, 1.0); glVertex3f(0, h, 0);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);

	EndShader();

	output_->EndDraw();
}
void
CropMS::Initialize()
{
	const char* shader_prog =
		STRINGIZE(
		#version 330 core \n
			 uniform sampler2DMS scene;					\n
				out vec4 colorV;
	vec4 sample(sampler2DMS s, ivec2 t) {
		\n
		return (texelFetch(s, t, 0) + texelFetch(s, t, 1) + texelFetch(s, t, 2) + texelFetch(s, t, 3))/4.0; \n
		} \n
		void main(void)													\n
		{																	\n
		   int x = int(gl_FragCoord.x);											\n
		   int y = int(gl_FragCoord.y);											\n
		   colorV = sample(scene, ivec2(x,y)); \n
		}
		);
	SetPixelShaderSource(shader_prog);
	Shader::Initialize();
}
};
