#ifndef __ColladaLoad_h
#define __ColladaLoad_h

#include <graphics/frame.h>
#include "model/GraphicsScene.h"
#include <model/ImportedModel.h>
#include <model/AnimCharacter.h>
//#include <model/AnimCharacterPreview.h>

using namespace graphics;

class FCDSceneNode;
class FCDGeometry;
class FCDAnimation;



namespace user {
	class Matrix4;
}

namespace model_kernel {


class ColladaLoad {

public:

	ColladaLoad(ImportedModel* model = 0, AnimCharacter* charac =0);

	void Load(const WChar* name);



	long polygon_count() const { return polygon_count_; }

	long vertex_count() const { return vertex_count_; }

private:

	GMaterial* SearchMaterial(const wchar_t* semantic);
	GMaterial* SearchMaterial(FCDSceneNode* node, const wchar_t* semantic);

	FCDSceneNode* SearchJointNodeWithSubId(const wchar_t* name);
	FCDSceneNode* SearchJointNodeWithSubId(FCDSceneNode* node, const wchar_t* name);

	FCDSceneNode* SearchJointNodeWithId(const wchar_t* name);
	FCDSceneNode* SearchJointNodeWithId(FCDSceneNode* node, const wchar_t* name);

	bool		  HasParentAnyJoint(FCDSceneNode* node);

	void CreateChildAnimation(FCDAnimation* fnode, Animation* node, AnimationClip* clip);
	void CreateChildJointNodes(FCDSceneNode* node);

	void ReconstructVisualScene();
	void ReconstructVisualScene(FCDSceneNode* node);

	void ExportToKoonFlat();
	void ExportToKoonFlat(SceneNode* corr, user::Matrix4 parent);


	void ExportToCharacter(Geometry* rep);
	void ExportToCharacter(Geometry* rep, SceneNode* corr, user::Matrix4 parent);

	void ReconnectSceneNodes();

	void BuildGeometries();

	void BuildControllers();

	void BuildJointScenes();

	void BuildJointScenes(FCDSceneNode* node);

	void BuildAnimations();

	void BuildMaterials();

	std::vector<FCDSceneNode*> GetRootNodes();

	void GetTransform(FCDSceneNode* node, frame& f, vec3& scale);

	void GetTransform(FCDSceneNode* node, std::vector<Transform*>& m);

	Geometry* CopyGeometry(FCDGeometry* a);

	void update_material_polygon(ImportedModel* model, GMaterial* mat, GPolygon* poly, user::Matrix4& cur);

	void update_material_polygon(GMaterial* mat, GPolygon* poly, user::Matrix4& cur);

	void update_material_polygon(Geometry* new_geom, GMaterial* mat, GPolygon* poly, user::Matrix4& cur);
private:



	std::map<FCDGeometry*, Geometry*> geom_map_;
	std::map<FCDSceneNode*, SceneNode*> shape_map_;

	ImportedModel* model_;
	AnimCharacter* character_;

	SceneGraph* scene_graph_;
	GeometryDatabase* geom_db_;
	MaterialDatabase* material_db_;

	std::map<GPolygon*, RelativeMemoryManager*> poly_map_;

	bool flat_model_;

	long polygon_count_;
	long vertex_count_;
};


};

#endif