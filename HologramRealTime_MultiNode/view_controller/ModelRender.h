#pragma once

#include	"view_controller/OglRenderShader.h"
#include	"model/ImportedModel.h"
#include	"model/AnimCharacter.h"
#include	"material/GMaterial.h"
#include	"graphics/Timer.h"
#include	"graphics/camera.h"

class ModelRender {

public:

	ModelRender();
	
	void setModel(model_kernel::ImportedModel* m) { model_ = m; }
	void setCharacter(model_kernel::AnimCharacter* c) { character_ = c; }

	void setFrontLight();
	void render();
	void render(bool opaque);

	void makeLarger() { scale_ += 5.0; }
	void makeSmaller() { scale_ -= 5.0; }

	void rotPlus() { rotx_ += 5.0; }
	void rotMinus() { rotx_ -= 5.0; }

	float scale_;
	float rotx_;

private:

	model_kernel::ImportedModel* model_;
	model_kernel::AnimCharacter* character_;

	unsigned int character_vbo_;
	std::vector<model_kernel::GMaterial*> character_materials_;
	std::vector<ivec2> character_range_;

	graphics::Timer timer_;


};