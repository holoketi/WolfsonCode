#ifndef __crop_ms_h
#define __crop_ms_h


#include <graphics/sys.h>

#include <graphics/Shader.h>
#include <graphics/ivec.h>
#include <graphics/vec.h>
#include <vector>
#include <graphics/texture_buffer.h>



namespace graphics {

class CropMS:  Shader {

public:

	CropMS();

	virtual void Initialize();
		
	void BeginShader();

	
	void EndShader()
	{
		Shader::EndShader();

		glActiveTextureARB(GL_TEXTURE0_ARB);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
		for (int i = 0 ; i < 25 ; i++) {
			glActiveTextureARB(GL_TEXTURE0_ARB + i);
			glBindTexture(GL_TEXTURE_2D, 0);
			
		}
		glActiveTextureARB(GL_TEXTURE0_ARB);
	}	


	void set_env(TextureBuffer* input, TextureBuffer* output)
	{
		input_ = input; 
		output_ = output;
	}


	void crop();
	
	uint result();
private:

	void InitUniformVariables();

	TextureBuffer* input_;
	TextureBuffer* output_;

};

};

#endif
