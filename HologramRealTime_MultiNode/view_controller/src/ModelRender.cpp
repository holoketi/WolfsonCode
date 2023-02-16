#include "view_controller/ModelRender.h"
#include "model/MemManager.h"
#include "model/sync_gpu_memory.h"
#include "model/rel_mem_manager.h"
#include "graphics/RenderEnv.h"
#include "material/GMaterial.h"
#include "material/GTexture.h"
#include "graphics/gl_stat.h"

#include <QtGui/qimage.h>

using namespace model_kernel;

ModelRender::ModelRender() : model_(0), character_(0)
{
	scale_ = 1.0;
	rotx_ = 0.0;

	timer_.start();

}

void ModelRender::render()
{
	if (!kOglRenderShader) {
		kOglRenderShader = new OglRenderShader();
		kOglRenderShader->Initialize();
	}

	graphics::gl_stat stat;
	stat.save_stat();

	if (character_ && character_->hasBody()) {
		character_vbo_ = character_->getVBO();
		std::vector<AnimVertexBufferData> data;
		float t = timer_.getElapsedTimeInSec();

		character_->getAnimVertexBufferData(t, data, character_materials_, character_range_);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, character_vbo_);
		glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, sizeof(AnimVertexBufferData) * data.size(), &data[0]);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	}

	float projMat[16];
	glGetFloatv(GL_PROJECTION_MATRIX, projMat);

	float viewMat[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, viewMat);

	kOglRenderShader->setProjandViewMat(projMat, viewMat);

	setFrontLight();

	user::Matrix4 aa = user::Matrix4::identity();
	kOglRenderShader->setModelMat(aa.array());

	//aa.rotateX(radian(rotx_));
	//aa.scale(user::Vector3(scale_, scale_, scale_));
	//kOglRenderShader->setModelMat(aa.array());

	render(true);
	render(false);

	stat.restore_stat();
}

static void draw_by_shader(GMaterial* material, bool opaque_only, RelativeMemoryManager* ret) {

	if (opaque_only) {
		if (material->IsTransparent()) return;
	}
	else { // draw only_transparent
		if (!material->IsTransparent()) return;
	}


	bool hasDiffuseTexture = material->HasDiffuseTexture();
	bool isTransparent = material->IsTransparent();
	bool hasBumpmapping = material->HasBumpmappingTexture();
	bool hasReflectiveTexture = material->HasReflectiveTexture();
	bool hasTexture = false;
	if (hasDiffuseTexture || hasReflectiveTexture || hasBumpmapping)
		hasTexture = true;

	color diffuse_c = material->GetDiffuse();
	color ambient_c = material->GetAmbient();
	color specular_c = material->GetSpecular();
	color emission_c = material->GetEmission();
	float shininess = material->GetShininess();

	float diffuse[3], ambient[3], specular[3], emission[3];

	for (int i = 0; i < 3; i++)
	{
		diffuse[i] = diffuse_c[i];
		ambient[i] = ambient_c[i];
		specular[i] = specular_c[i];
		emission[i] = emission_c[i];
	}

	// tranparency ==================================================
	if (isTransparent) {

		color blend_color(1);
		color transparent = material->GetTransparent();
		real  transparency = material->GetTransparency();
		blend_color[0] = transparency * transparent[0];
		blend_color[1] = transparency * transparent[1];
		blend_color[2] = transparency * transparent[2];
		blend_color[3] = transparency * (transparent[0] * 0.212671 +
			transparent[1] * 0.715160 +
			transparent[2] * 0.072169);

		glEnable(GL_BLEND);
		glBlendColor(blend_color[0], blend_color[1], blend_color[2], blend_color[3]);
		glBlendFunc(GL_ONE_MINUS_CONSTANT_COLOR, GL_CONSTANT_COLOR);

	}
	else {
		glDisable(GL_BLEND);
	}

	kOglRenderShader->setMaterialProperties(hasDiffuseTexture, isTransparent, hasBumpmapping, hasReflectiveTexture, hasTexture,
		emission, ambient, diffuse, specular, shininess);

	if (hasDiffuseTexture)
		kOglRenderShader->SetDiffuseColorTexture(material->GetTextureDiffuse()->GetTextureId());

	// Bump Mapping
	if (hasBumpmapping)
		kOglRenderShader->SetBumpMapTexture(material->GetTextureBumpmapping()->GetTextureId());

	kOglRenderShader->BeginShader();

	glPolygonMode(GL_FRONT, GL_FILL);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	int r = ret->getRange();
	glDrawElements(GL_TRIANGLES, r * 3, GL_UNSIGNED_INT, BUFFER_OFFSET(ret->baseAddr() * EBO_ELEM_SIZE));

	kOglRenderShader->EndShader();

	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	GLenum err = glGetError();

}

static void draw_char_by_shader(GMaterial* material, bool opaque_only, ivec2 ret) {

	if (opaque_only) {
		if (material->IsTransparent()) return;
	}
	else { // draw only_transparent
		if (!material->IsTransparent()) return;
	}


	bool hasDiffuseTexture = material->HasDiffuseTexture();
	bool isTransparent = material->IsTransparent();
	bool hasBumpmapping = material->HasBumpmappingTexture();
	bool hasReflectiveTexture = material->HasReflectiveTexture();
	bool hasTexture = false;
	if (hasDiffuseTexture || hasReflectiveTexture || hasBumpmapping)
		hasTexture = true;

	color diffuse_c = material->GetDiffuse();
	color ambient_c = material->GetAmbient();
	color specular_c = material->GetSpecular();
	color emission_c = material->GetEmission();
	float shininess = material->GetShininess();

	float diffuse[3], ambient[3], specular[3], emission[3];

	for (int i = 0; i < 3; i++)
	{
		diffuse[i] = diffuse_c[i];
		ambient[i] = ambient_c[i];
		specular[i] = specular_c[i];
		emission[i] = emission_c[i];
	}

	// tranparency ==================================================
	if (isTransparent) {

		color blend_color(1);
		color transparent = material->GetTransparent();
		real  transparency = material->GetTransparency();
		blend_color[0] = transparency * transparent[0];
		blend_color[1] = transparency * transparent[1];
		blend_color[2] = transparency * transparent[2];
		blend_color[3] = transparency * (transparent[0] * 0.212671 +
			transparent[1] * 0.715160 +
			transparent[2] * 0.072169);

		glEnable(GL_BLEND);
		glBlendColor(blend_color[0], blend_color[1], blend_color[2], blend_color[3]);
		glBlendFunc(GL_ONE_MINUS_CONSTANT_COLOR, GL_CONSTANT_COLOR);

	}
	else {
		glDisable(GL_BLEND);
	}

	kOglRenderShader->setMaterialProperties(hasDiffuseTexture, isTransparent, hasBumpmapping, hasReflectiveTexture, hasTexture,
		emission, ambient, diffuse, specular, shininess);

	if (hasDiffuseTexture)
		kOglRenderShader->SetDiffuseColorTexture(material->GetUserTextureDiffuse()->GetTextureId());

	// Bump Mapping
	if (hasBumpmapping)
		kOglRenderShader->SetBumpMapTexture(material->GetUserTextureBumpmapping()->GetTextureId());

	kOglRenderShader->BeginShader();

	glPolygonMode(GL_FRONT, GL_FILL);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glDrawArrays(GL_TRIANGLES, ret[0], ret[1] - ret[0] + 1);
	kOglRenderShader->EndShader();

	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	GLenum err = glGetError();

}
void ModelRender::render(bool opaque)
{

	std::vector<RelativeMemoryManager*> ret = model_->getAllRelativeMemoryManager();
	std::vector<GMaterial*> materials = model_->getAllMaterial();

	GLenum err = glGetError();

	GLuint	arrays;
	glGenVertexArrays(1, &arrays);
	glBindVertexArray(arrays);

	int position_loc = glGetAttribLocation(kOglRenderShader->getShaderId(), "vPosition");
	int normal_loc = glGetAttribLocation(kOglRenderShader->getShaderId(), "vNormal");
	int textcoord_loc = glGetAttribLocation(kOglRenderShader->getShaderId(), "in_tex_coord");

	glBindBuffer(GL_ARRAY_BUFFER, kSyncGpuMemory->vbo());
	glVertexAttribPointer(position_loc, 3, GL_FLOAT, GL_FALSE, VBO_ELEM_SIZE, 0);
	glEnableVertexAttribArray(position_loc);
	glVertexAttribPointer(normal_loc, 3, GL_FLOAT, GL_FALSE, VBO_ELEM_SIZE, NORMAL_OFFSET);
	glEnableVertexAttribArray(normal_loc);
	glVertexAttribPointer(textcoord_loc, 2, GL_FLOAT, GL_TRUE, VBO_ELEM_SIZE, TEXTURE_OFFSET);
	glEnableVertexAttribArray(textcoord_loc);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, kSyncGpuMemory->ebo());

	for (size_t i = 0; i < ret.size(); i++) {

		draw_by_shader(materials[i], opaque, ret[i]);

	}

	err = glGetError();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glDeleteVertexArrays(1, &arrays);

	if (character_ && character_->hasBody()) {

		glBindVertexArray(arrays);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, character_vbo_);
		glVertexAttribPointer(position_loc, 3, GL_FLOAT, GL_FALSE, 32, 0);
		glEnableVertexAttribArray(position_loc);
		glVertexAttribPointer(normal_loc, 3, GL_FLOAT, GL_FALSE, 32, BUFFER_OFFSET(12));
		glEnableVertexAttribArray(normal_loc);
		glVertexAttribPointer(textcoord_loc, 2, GL_FLOAT, GL_TRUE, 32, BUFFER_OFFSET(24));
		glEnableVertexAttribArray(textcoord_loc);

		//kOglRenderShader->setModelMat(character_render_instances[i].transform().array());

		for (size_t j = 0; j < character_materials_.size(); j++) {

			draw_char_by_shader(character_materials_[j],opaque,	character_range_[j]);

		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	glDeleteVertexArrays(1, &arrays);
}

void ModelRender::setFrontLight()
{
	int vport[4];
	double model[16];
	double proj[16];
	glGetIntegerv(GL_VIEWPORT, vport);
	glGetDoublev(GL_PROJECTION_MATRIX, proj);
	glGetDoublev(GL_MODELVIEW_MATRIX, model);
	double p[3];
	gluUnProject(vport[2] / 2, vport[3] / 2, 0, model, proj, vport, &p[0], &p[1], &p[2]);

	float position[4];
	position[0] = p[0];
	position[1] = p[1];
	position[2] = p[2];
	position[3] = 0.0;

	float diffuse_color[4] = { 0.8, 0.8, 0.8, 1.0 };
	float ambient[4] = { 0.2, 0.2, 0.2, 1.0 };
	float specular_color[4] = { 0.3, 0.3, 0.3, 1.0 };
	float global_ambient[4] = { 0.2, 0.2, 0.2, 1.0 };
	float tmp[3] = { 0.0, 0.0, 0.0 };

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	kOglRenderShader->SetNumberofLights(1);
	kOglRenderShader->setLightsProperties(0, false, false, false,
		ambient, diffuse_color, specular_color, position, tmp, 0, 0, 1, 0, 0, global_ambient);

}