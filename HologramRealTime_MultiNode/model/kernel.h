#ifndef __kernel_h
#define __kernel_h

#include <graphics/sys.h>
#include <graphics/vec.h>

#include <map>
#include <set>
#include <vector>
#include <string>
#include <list>
#include <functional>
#include "model/Allocator.h"


extern unsigned int kCurrentUndoContainerId;

#define KernelAlloc(size) Allocate(size, (unsigned long)kCurrentUndoContainerId)
#define DECLARE_CLASS(class_name) void* operator new(size_t size) { return ::Allocate(size, (unsigned long)kCurrentUndoContainerId); }; \
void operator delete(void* p) { ::Deallocate(p, (unsigned long)kCurrentUndoContainerId); }

void initKernel();
void beginTransaction();
void endTransaction();

typedef std::basic_string<WChar> WString;
typedef std::basic_string<WChar, std::char_traits<WChar>, Allocator<WChar> > StringType;

namespace model_kernel {

	using namespace graphics;
	class GTexture;
	class GMaterial;
	class SceneNode;
	class Geometry;
	class GPolygon;
	class GeometryInstance;
	class GPolygonInstance;

	typedef std::map<long, class GTexture*, std::less<long>, Allocator<std::pair<const long, class GTexture*> > > TextureTable;
	typedef std::map<long, class GMaterial*, std::less<long>, Allocator<std::pair<const long, class GMaterial*> > > MaterialTable;
	typedef std::map<long, class SceneNode*, std::less<long>/*, Allocator<std::pair<const long, class SceneNode*> >*/ > SceneNodeMap;
	typedef std::map<long, class Geometry*, std::less<long>/*, Allocator<std::pair<const long, class Geometry*> >*/ > GeometryMap;
	typedef std::list<class SceneNode* > RootSceneNodes;
	typedef std::vector<class GPolygon*> PolygonArray;
	typedef std::vector<class Geometry*> Geometries;
	typedef std::vector<class GeometryInstance*> GeometryInstances;
	typedef std::vector<class GPolygonInstance*> PolygonInstances;
	typedef std::vector<class SceneNode* > SceneNodeChildren;
	typedef std::vector<class SceneNode*> SceneNodeParents;

	typedef std::vector<unsigned int, Allocator<unsigned int> >		IndexArray;
	typedef std::vector<struct vec3, Allocator<struct vec3> > PointArray;
	typedef std::vector<struct vec3, Allocator<struct vec3> > NormalArray;
	typedef std::vector<struct vec2, Allocator<struct vec2> > TextureCoordinateArray;

}

#endif