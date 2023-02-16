
#include "graphics/sys.h"
#include "graphics/glinfo.h"
#include <model/Matrix4.h>


#include "graphics/gl_extension.h"
#include "view_controller/OglRenderShader.h"

OglRenderShader* kOglRenderShader = 0;

OglRenderShader::OglRenderShader() : Shader(), num_lights_(0)
{
}

void OglRenderShader::setProjandViewMat(float proj[16], float view[16])
{
	memcpy(proj_mat_, proj, sizeof(float) * 16);
	memcpy(view_mat_, view, sizeof(float) * 16);

}

void OglRenderShader::setModelMat(const float model[16])
{
	memcpy(model_mat_, model, sizeof(float) * 16);
}

void OglRenderShader::setLightsProperties(int LightIdx, bool isLocal, bool isSpot, bool isViewSpace,
	float* ambient, float* diffuse_color, float* specular_color, float* position, float* coneDirection, float spotCostCutoff, float spotExponent,
	float constantAttenuation, float linearAttenuation, float quadraticAttenuation, float* global_ambient)
{
	Lights_[LightIdx].isLocal = isLocal;
	Lights_[LightIdx].isSpot = isSpot;

	Lights_[LightIdx].ambient[0] = ambient[0];
	Lights_[LightIdx].ambient[1] = ambient[1];
	Lights_[LightIdx].ambient[2] = ambient[2];

	Lights_[LightIdx].diffuse_color[0] = diffuse_color[0];
	Lights_[LightIdx].diffuse_color[1] = diffuse_color[1];
	Lights_[LightIdx].diffuse_color[2] = diffuse_color[2];

	Lights_[LightIdx].specular_color[0] = specular_color[0];
	Lights_[LightIdx].specular_color[1] = specular_color[1];
	Lights_[LightIdx].specular_color[2] = specular_color[2];

	user::Matrix4 view_mat(view_mat_);
	user::Vector4 pos;
	if (Lights_[LightIdx].isLocal)
	{
		pos = user::Vector4(position[0], position[1], position[2], 1.0);

	}
	else {

		pos = user::Vector4(position[0], position[1], position[2], 0.0);
	}

	Lights_[LightIdx].position[0] = pos[0];
	Lights_[LightIdx].position[1] = pos[1];
	Lights_[LightIdx].position[2] = pos[2];
	Lights_[LightIdx].position[3] = pos[3];

	Lights_[LightIdx].coneDirection[0] = coneDirection[0];
	Lights_[LightIdx].coneDirection[1] = coneDirection[1];
	Lights_[LightIdx].coneDirection[2] = coneDirection[2];
	Lights_[LightIdx].coneDirection[3] = 0;

	Lights_[LightIdx].spotCostCutoff = spotCostCutoff;
	Lights_[LightIdx].spotExponent = spotExponent;

	Lights_[LightIdx].constantAttenuation = constantAttenuation;
	Lights_[LightIdx].linearAttenuation = linearAttenuation;
	Lights_[LightIdx].quadraticAttenuation = quadraticAttenuation;

	global_ambient_[0] = global_ambient[0];
	global_ambient_[1] = global_ambient[1];
	global_ambient_[2] = global_ambient[2];


}

void OglRenderShader::setMaterialProperties(bool hasDiffuseTexture, bool isTransparent, bool hasBumpmapping, bool hasReflectiveTexture, bool hasTexture,
	float* emission, float* ambient, float* diffuse, float* specular, float shininess)
{
	hasDiffuseTexture_ = hasDiffuseTexture;
	isTransparent_ = isTransparent;
	hasBumpmapping_ = hasBumpmapping;
	hasReflectiveTexture_ = hasReflectiveTexture;
	hasTexture_ = hasTexture;

	emission_[0] = emission[0];
	emission_[1] = emission[1];
	emission_[2] = emission[2];

	ambient_[0] = ambient[0];
	ambient_[1] = ambient[1];
	ambient_[2] = ambient[2];

	diffuse_[0] = diffuse[0];
	diffuse_[1] = diffuse[1];
	diffuse_[2] = diffuse[2];

	specular_[0] = specular[0];
	specular_[1] = specular[1];
	specular_[2] = specular[2];

	shininess_ = shininess;

}

void OglRenderShader::SetHasDiffuse(bool val)
{
	hasDiffuseTexture_ = val;
}
void OglRenderShader::SetDiffuseColorTexture(GLuint val)
{
	DiffuseColor_Texture_ = val;
}
void OglRenderShader::SetDiffuseColor(float* val)
{
	diffuse_[0] = val[0];
	diffuse_[1] = val[1];
	diffuse_[2] = val[2];

}

void OglRenderShader::SetHasBumpMap(bool val)
{
	hasBumpmapping_ = val;
}

void OglRenderShader::SetBumpMapTexture(GLuint val)
{
	BumpMap_Texture_ = val;
}

void OglRenderShader::SetNumberofLights(int val)
{
	num_lights_ = val;
}

void OglRenderShader::EndShader()
{
	Shader::EndShader();
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glBindTexture(GL_TEXTURE_2D, 0);

}

void OglRenderShader::BeginShader()
{
	Shader::BeginShader();
	GLenum err = glGetError();

	int loc;
	loc = glGetUniformLocation(shader_id_, "ProjMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, proj_mat_);
	if (loc == -1) 	LOG("proj_mat_ loc:%d\n", loc);

	loc = glGetUniformLocation(shader_id_, "ModelMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, model_mat_);
	loc = glGetUniformLocation(shader_id_, "ViewMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, view_mat_);

	loc = glGetUniformLocation(shader_id_, "global_ambient");
	glUniform3fv(loc, 1, global_ambient_);

	loc = glGetUniformLocation(shader_id_, "hasBump");
	glUniform1i(loc, hasBumpmapping_);

	loc = glGetUniformLocation(shader_id_, "Material.hasDiffuseTexture");
	glUniform1i(loc, hasDiffuseTexture_);

	loc = glGetUniformLocation(shader_id_, "Material.isTransparent");
	glUniform1i(loc, isTransparent_);

	loc = glGetUniformLocation(shader_id_, "Material.hasBumpmapping");
	glUniform1i(loc, hasBumpmapping_);

	loc = glGetUniformLocation(shader_id_, "Material.hasReflectiveTexture");
	glUniform1i(loc, hasReflectiveTexture_);

	loc = glGetUniformLocation(shader_id_, "Material.hasTexture");
	glUniform1i(loc, hasTexture_);

	loc = glGetUniformLocation(shader_id_, "Material.BumpMap");
	glUniform1i(loc, 1);
	glActiveTexture(GL_TEXTURE1_ARB);
	glBindTexture(GL_TEXTURE_2D, BumpMap_Texture_);

	loc = glGetUniformLocation(shader_id_, "Material.DiffuseTexture");
	glUniform1i(loc, 0);
	glActiveTexture(GL_TEXTURE0_ARB);
	glBindTexture(GL_TEXTURE_2D, DiffuseColor_Texture_);

	loc = glGetUniformLocation(shader_id_, "Material.emission");
	glUniform3fv(loc, 1, emission_);

	loc = glGetUniformLocation(shader_id_, "Material.ambient");
	glUniform3fv(loc, 1, ambient_);

	loc = glGetUniformLocation(shader_id_, "Material.diffuse");
	glUniform3fv(loc, 1, diffuse_);

	loc = glGetUniformLocation(shader_id_, "Material.specular");
	glUniform3fv(loc, 1, specular_);

	loc = glGetUniformLocation(shader_id_, "Material.shininess");
	glUniform1f(loc, shininess_);

	loc = glGetUniformLocation(shader_id_, "num_lights");
	glUniform1i(loc, num_lights_);


	for (int k = 0; k < num_lights_; k++)
	{
		std::string number = std::to_string(k);
		loc = glGetUniformLocationARB(shader_id_, ("Lights[" + number + "].isEnabled").c_str());
		glUniform1i(loc, true);

		loc = glGetUniformLocationARB(shader_id_, ("Lights[" + number + "].isLocal").c_str());
		glUniform1i(loc, Lights_[k].isLocal);

		loc = glGetUniformLocationARB(shader_id_, ("Lights[" + number + "].isSpot").c_str());
		glUniform1i(loc, Lights_[k].isSpot);

		loc = glGetUniformLocationARB(shader_id_, ("Lights[" + number + "].ambient").c_str());
		glUniform3fv(loc, 1, Lights_[k].ambient);

		loc = glGetUniformLocationARB(shader_id_, ("Lights[" + number + "].diffuse_color").c_str());
		glUniform3fv(loc, 1, Lights_[k].diffuse_color);

		loc = glGetUniformLocationARB(shader_id_, ("Lights[" + number + "].specular_color").c_str());
		glUniform3fv(loc, 1, Lights_[k].specular_color);

		loc = glGetUniformLocationARB(shader_id_, ("Lights[" + number + "].position").c_str());
		glUniform4fv(loc, 1, Lights_[k].position);

		loc = glGetUniformLocationARB(shader_id_, ("Lights[" + number + "].coneDirection").c_str());
		glUniform4fv(loc, 1, Lights_[k].coneDirection);

		loc = glGetUniformLocationARB(shader_id_, ("Lights[" + number + "].spotCostCutoff").c_str());
		glUniform1f(loc, Lights_[k].spotCostCutoff);

		loc = glGetUniformLocationARB(shader_id_, ("Lights[" + number + "].spotExponent").c_str());
		glUniform1f(loc, Lights_[k].spotExponent);

		loc = glGetUniformLocationARB(shader_id_, ("Lights[" + number + "].constantAttenuation").c_str());
		glUniform1f(loc, Lights_[k].constantAttenuation);

		loc = glGetUniformLocationARB(shader_id_, ("Lights[" + number + "].linearAttenuation").c_str());
		glUniform1f(loc, Lights_[k].linearAttenuation);

		loc = glGetUniformLocationARB(shader_id_, ("Lights[" + number + "].quadraticAttenuation").c_str());
		glUniform1f(loc, Lights_[k].quadraticAttenuation);

	}

}

void OglRenderShader::Initialize()
{
	const char* vertex_prog =
		"#version 430 core																	\n"
		"uniform mat4 ProjMatrix;															\n"
		"uniform mat4 ModelMatrix;																\n"
		"uniform mat4 ViewMatrix;															\n"
		"uniform bool hasBump;																\n"
		"																					\n"
		"in vec4 vPosition;																	\n"
		"in vec3 vNormal;																	\n"
		"in vec2 in_tex_coord;																\n"
		"																					\n"
		"out vec2 tex_coord;																\n"
		"out vec3 normal_VSp;																\n"
		"out vec3 vertex_VSp;																\n"
		"out vec3 bump_tangent;																\n"
		"out vec3 bump_binormal;															\n"
		"out vec3 bump_normal;																\n"
		"																					\n"
		"vec3 get_tangent(mat4 MVMatrix){																\n"
		"	vec3 c1 = cross( (MVMatrix * vec4(vNormal,0)).xyz, vec3(0.0, 0.0, 1.0) );					\n"
		"	vec3 c2 = cross( (MVMatrix * vec4(vNormal,0)).xyz, vec3(0.0, 1.0, 0.0) );					\n"
		"	vec3 t;																			\n"
		"	if( length(c1)>length(c2) )														\n"
		"		t = c1;																		\n"
		"	else																			\n"
		"		t = c2;																		\n"
		"	return normalize(t);															\n"
		"}																					\n"
		"																					\n"
		"void main()																		\n"
		"{																					\n"
		"   mat4 MVMatrix = ViewMatrix*ModelMatrix; \n"
		"	gl_Position = ProjMatrix * MVMatrix * vPosition;								\n"
		"	tex_coord = in_tex_coord;														\n"
		"	normal_VSp = normalize( (MVMatrix * vec4(vNormal,0)).xyz);								\n"
		"	vertex_VSp = (MVMatrix * vPosition).xyz;										\n"
		"																					\n"
		"	if (hasBump){																	\n"
		"		bump_tangent = get_tangent(MVMatrix);												\n"
		"		bump_binormal = normalize(cross((MVMatrix * vec4(vNormal,0)).xyz, bump_tangent));		\n"
		"		bump_normal = normalize((MVMatrix * vec4(vNormal,0)).xyz);							\n"
		"	}																				\n"
		"}\0";

	const char* pixel_prog =
		"#version 430 core																					\n"
		"struct LightProperties {																			\n"
		"	bool		isEnabled;																			\n"
		"	bool		isLocal;																			\n"
		"	bool		isSpot;																				\n"
		"	vec3		ambient;																			\n"
		"	vec3		diffuse_color;																		\n"
		"	vec3		specular_color;																		\n"
		"	vec4		position;																			\n"
		"	vec3		halfVector;																			\n"
		"	vec4		coneDirection;																		\n"
		"	float		spotCostCutoff;																		\n"
		"	float		spotExponent;																		\n"
		"	float		constantAttenuation;																\n"
		"	float		linearAttenuation;																	\n"
		"	float		quadraticAttenuation;																\n"
		"};																									\n"
		"																									\n"
		"struct MatrialProperties {																			\n"
		"	bool		hasDiffuseTexture;																	\n"
		"	bool		isTransparent;																		\n"
		"	bool		hasBumpmapping;																		\n"
		"	bool		hasReflectiveTexture;																\n"
		"	bool		hasTexture;																			\n"
		"	vec3		emission;																			\n"
		"	vec3		ambient;																			\n"
		"	vec3		diffuse;																			\n"
		"	vec3		specular;																			\n"
		"	float		shininess;																			\n"
		"	sampler2D	DiffuseTexture;																		\n"
		"	sampler2D	BumpMap;																			\n"
		"};																									\n"
		"																									\n"
		"const int MaxLight = 8;																			\n"
		"uniform int num_lights;																			\n"
		"uniform LightProperties Lights[MaxLight];															\n"
		"uniform MatrialProperties Material;																\n"

		"uniform mat4 ViewMatrix;																			\n"
		"uniform vec3 global_ambient;																		\n"
		"                                                                                                   \n"
		"in vec2 tex_coord;																					\n"
		"in vec3 normal_VSp;																				\n"
		"in vec3 vertex_VSp;																				\n"
		"in vec3 bump_tangent;																				\n"
		"in vec3 bump_binormal;																				\n"
		"in vec3 bump_normal;																				\n"
		"                                                                                                   \n"
		"out vec4 fColor;																					\n"
		"																									\n"
		"vec3 mypow(vec3 a, float b)																		\n"
		"{ return vec3(pow(a[0], b), pow(a[1], b), pow(a[2], b)); }											\n"
		"																									\n"
		"void ComputeTangentSP(in vec3 light_VSp, in vec3 view_VSp, inout vec3 L, inout vec3 E, inout vec3 N)\n"
		"{																									\n"
		"	N = texture(Material.BumpMap, tex_coord).rgb * 2.0 - 1.0;										\n"
		"	L.x = dot(bump_tangent, light_VSp);																\n"
		"	L.y = dot(bump_binormal, light_VSp);															\n"
		"	L.z = dot(bump_normal, light_VSp);												   				\n"
		"	L = normalize(L);																				\n"
		"	E.x = dot(bump_tangent, view_VSp);																\n"
		"	E.y = dot(bump_binormal, view_VSp);																\n"
		"	E.z = dot(bump_normal, view_VSp);																\n"
		"}																									\n"
		"																									\n"
		"void main()																						\n"
		"{																									\n"
		"	vec3 diffuseColor = Material.diffuse;															\n"
		"	if (Material.hasDiffuseTexture == true)															\n"
		"		diffuseColor = texture(Material.DiffuseTexture, tex_coord).rgb;								\n"
		"	//	diffuseColor = mypow(texture(Material.DiffuseTexture, tex_coord).rgb, 2.2);					\n"
		"																									\n"
		"	vec3 ambientLight = vec3(0.0);																	\n"
		"	vec3 diffuseLight = vec3(0.0);																	\n"
		"	vec3 specularLight = vec3(0.0);																	\n"
		"																									\n"
		"	for (int k = 0; k<num_lights; k++)																\n"
		"	{																								\n"
		"		if (Lights[0].isEnabled == false)	continue;												\n"
		"																									\n"
		"		vec3 L, E, N, light_VSp, view_VSp, light_pos, halfV;										\n"
		"		float attenuation = 1.0;																	\n"
		"		view_VSp = -normalize(vertex_VSp);															\n"
		"       light_pos = (ViewMatrix*Lights[k].position).xyz; \n"
		"		if (Lights[k].isLocal == false)																\n"
		"       {																							\n"
		"			light_VSp = normalize(light_pos);														\n"
		"			halfV = light_VSp;																		\n"
		"																									\n"
		"		}else{																						\n"
		"																									\n"
		"			light_VSp = light_pos - vertex_VSp;														\n"
		"			float len = length(light_VSp);															\n"
		"			light_VSp = normalize(light_VSp);														\n"
		"																									\n"
		"			attenuation = 1.0 / (Lights[k].constantAttenuation + Lights[k].linearAttenuation*len + Lights[k].quadraticAttenuation*len*len);	\n"
		"																									\n"
		"			if (Lights[k].isSpot == true) {															\n"
		"																									\n"
		"				vec3 coneDir = normalize((ViewMatrix*Lights[k].coneDirection).xyz);									\n"
		"				float spotDot = max(dot(-light_VSp, coneDir), 0.0);									\n"
		"				if (spotDot >= Lights[k].spotCostCutoff)  {											\n"
		"					spotDot = (spotDot - Lights[k].spotCostCutoff) / (1.0 - Lights[k].spotCostCutoff); \n"
		"					if (spotDot != 0.0 && Lights[k].spotExponent > 0.0)								\n"
		"						attenuation *= pow(spotDot, Lights[k].spotExponent);						\n"
		"					else                                                                            \n"
		"						attenuation *= spotDot;														\n"
		"				} else 																				\n"
		"					attenuation = 0.0;																\n"
		"			}																						\n"
		"			halfV = normalize(light_VSp + view_VSp);												\n"
		"		}																							\n"
		"																									\n"
		"		if (Material.hasBumpmapping == true)	{													\n"
		"			ComputeTangentSP(light_VSp, view_VSp, L, E, N);											\n"
		"		} else{																						\n"
		"			L = light_VSp;																			\n"
		"			E = view_VSp;																			\n"
		"			N = normalize(normal_VSp);																\n"
		"		}																							\n"
		"		float NdotL = 0.0, NdotH = 0.0;																\n"
		"		NdotL = max(dot(N, L), 0.0);																\n"
		"		//NdotH = max(dot(N, halfV), 0.0);															\n"
		"		NdotH = max(dot(reflect(-L, N), E), 0.0);													\n"
		"		if (NdotL >= 0.0 && NdotH != 0.0 && Material.shininess > 0.0)								\n"
		"			NdotH = pow(NdotH, Material.shininess);													\n"
		"		ambientLight += Lights[k].ambient * attenuation;											\n"
		"		diffuseLight += Lights[k].diffuse_color *  NdotL * attenuation;								\n"
		"		specularLight += Lights[k].specular_color * NdotH * attenuation;							\n"
		"	}																								\n"
		"																									\n"
		"	vec3 scatteredLight = ambientLight * Material.ambient + diffuseLight * diffuseColor;			\n"
		"	vec3 reflectedLight = specularLight * Material.specular;										\n"
		"	vec3 sceneColor = Material.emission + global_ambient * Material.ambient;						\n"
		"	vec3 linear_color = sceneColor + scatteredLight + reflectedLight;								\n"
		"	fColor.rgb = min(linear_color, vec3(1.0));														\n"
		"	fColor.w = 1;																					\n"
		"}\0";

	SetVertexShaderSource(vertex_prog);
	SetPixelShaderSource(pixel_prog);

	Shader::Initialize();

}
