#ifndef __OglRender_Shader_h
#define __OglRender_Shader_h

#include <graphics/vec.h>
#include "graphics/Shader.h"

const int MAX_NUM_LIGHTS = 8;

class OglRenderShader : public graphics::Shader {

	struct LightProperties {
		bool				isEnabled;
		bool				isLocal;
		bool				isSpot;
		GLfloat				ambient[3];
		GLfloat				diffuse_color[3];
		GLfloat				specular_color[3];
		GLfloat				position[4];
		GLfloat				halfVector[3];
		GLfloat				coneDirection[4];
		float				spotCostCutoff;
		float				spotExponent;
		float				constantAttenuation;
		float				linearAttenuation;
		float				quadraticAttenuation;
	};

public:

	OglRenderShader();

	virtual void Initialize();

	virtual void BeginShader();
	virtual void EndShader();

	void setLightsProperties(int LightIdx, bool isLocal, bool isSpot, bool isViewSpace,
		float* ambient, float* diffuse_color, float* specular_color, float* position, float* coneDirection, float spotCostCutoff, float spotExponent,
		float constantAttenuation, float linearAttenuation, float quadraticAttenuation, float* global_ambient);

	void setMaterialProperties(bool hasDiffuseTexture, bool isTransparent, bool hasBumpmapping, bool hasReflectiveTexture, bool hasTexture,
		float* emission, float* ambient, float* diffuse, float* specular, float shininess);


	void SetHasBumpMap(bool val);
	void SetBumpMapTexture(unsigned int val);

	void SetHasDiffuse(bool val);
	void SetDiffuseColorTexture(unsigned int val);
	void SetDiffuseColor(float* val);

	void SetNumberofLights(int val);

	void setMatrix(float proj[16], float modelview[16], const float model[16], float view[16]);
	void setProjandViewMat(float proj[16], float view[16]);
	void setModelMat(const float model[16]);

	unsigned int getShaderId() { return shader_id_; }

protected:

	// Material Properties
	GLboolean	isTransparent_;
	GLboolean	hasReflectiveTexture_;
	GLboolean	hasTexture_;
	GLboolean	hasBumpmapping_;
	GLuint		BumpMap_Texture_;
	GLboolean	hasDiffuseTexture_;
	GLuint		DiffuseColor_Texture_;
	GLfloat		emission_[3];
	GLfloat		ambient_[3];
	GLfloat		diffuse_[3];
	GLfloat		specular_[3];
	GLfloat		shininess_;

	GLint		num_lights_;

	LightProperties	Lights_[MAX_NUM_LIGHTS];

	GLfloat		proj_mat_[16];
	GLfloat		model_mat_[16];
	//GLfloat	normal_mat_[9];			// transpose of the inverse of the model view matrix: that's view mat
	GLfloat		view_mat_[16];

	GLfloat		global_ambient_[3];


};

extern OglRenderShader* kOglRenderShader;



#endif
