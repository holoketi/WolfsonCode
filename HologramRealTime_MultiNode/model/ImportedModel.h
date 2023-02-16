#ifndef __importedmodel_h
#define __importedmodel_h

#include "model/kernel.h"
#include <string>
#include <vector>
#include <list>
#include <set>
#include <graphics/virtual_model.h>
#include <graphics/unsigned.h>

class RelativeMemoryManager;
using namespace graphics;
namespace model_kernel {

class GMaterial;

class ImportedModel: public VirtualModel {

public:
	typedef std::vector<std::pair<class RelativeMemoryManager*, class GMaterial*>, Allocator<std::pair<class RelativeMemoryManager*, class GMaterial*> > > RenderModels;
	typedef std::set<class RelativeMemoryManager*, std::greater<class RelativeMemoryManager*>, Allocator<class RelativeMemoryManager*> > SceneDBType;
	typedef std::map<class GMaterial*, class RelativeMemoryManager*, std::less<class GMaterial*>, Allocator<std::pair<class GMaterial*,class RelativeMemoryManager* > > > MaterialRenderTable;

public:

	ImportedModel();
	virtual ~ImportedModel();


	RelativeMemoryManager* getRelativeMemoryManager(GMaterial* mat);
	

	std::vector<RelativeMemoryManager*> getAllRelativeMemoryManager();

	std::vector<GMaterial*> getAllMaterial();

	bool    increaseRelativeMemoryManager(RelativeMemoryManager* rm, int count_of_element);

	int		addTriangle2Gpu(RelativeMemoryManager* rm, const vec3& v1, const vec3& v2, const vec3& v3, 
											   const vec3& n1, const vec3& n2, const vec3& n3,
											   const uint  col,
											   const vec2& uv1, const vec2& uv2, const vec2& uv3,
											   const uint vertex1, const uint vertex2, const uint vertex3);

	void	deleteTriangleFromGpu(RelativeMemoryManager* rm, int index);

	std::vector<int> triangleIndexes(RelativeMemoryManager* rm);

	// access to vertex data!
	float*  getTriangleVertex(RelativeMemoryManager* rm, int tri_index, int vertex_index);



	RelativeMemoryManager* createRelativeMemoryManager();

private:
	int*	addressOfElementBuffer(RelativeMemoryManager* rm, int index);
	
	RelativeMemoryManager* createRelativeMemoryManager(GMaterial* mat);

	float*	addressOfVertexBuffer(int index);
public:

	DECLARE_CLASS(ImportedModel);

private:

	MaterialRenderTable render_table_;
};


};

#endif
