#ifndef __SceneNode_h
#define __SceneNode_h


#include "model/GFrame.h"
#include "material/GMaterial.h"
#include <graphics/Triangle.h>
#include <list>

namespace model_kernel {

const int kTopologySceneNodeType = 6;

class GeometryDatabase;
//
// GuideSet is created from CModel.
//
class SceneGraph {

public:

	friend class SceneNode;

public:

	SceneGraph();

	~SceneGraph();

	SceneNode*	CreateSceneNode();

	SceneNode*   GetSceneNodeById(long id)
	{
		SceneNodeMap::iterator i = scene_node_database_.find(id);
		if (i == scene_node_database_.end()) return 0;
		return i->second;
	}
	/** Remove the shape from shape map, ground level, and delete itself.
		@see .
		@param .
		@return . */
	void DeleteSceneNode(SceneNode* shape);
	void DeleteSceneNodeRecurse(SceneNode* g);

	RootSceneNodes::iterator FindRootSceneNode(SceneNode *g);

	/** Find the root node from which input Scene Node g came.
		@see .
		@param g: input scene node.
		@return root scene node. */
	SceneNode*	FindRootFrom(SceneNode* g) const;

	void AddRootSceneNode(SceneNode*g);

	void RemoveRootSceneNode(SceneNode *g);

	void GetRootSceneNodes(std::vector<SceneNode*>& g_level) const;

	size_t GetRootSceneCount() const;

	/** Copy input->children->children.. tree and retun the copied tree*/
	SceneNode* Copy(SceneNode* input);
	SceneNode* CopyRecurse(SceneNode* input);


	/** Check if input scene node g is root scene node.
		@see .
		@param g: input scene node.
		@return true if g is a root node */
	bool   IsRootSceneNode(SceneNode* g) const;

	SceneNodeMap& GetSceneNodeTable() { return scene_node_database_; }

	GeometryDatabase* GetGeometryDatabase() { return geometry_database_; }
	TextureDatabase* GetTextureDatabase() { return texture_database_; }
	MaterialDatabase* GetMaterialDatabase() { return material_database_; }

	void log_bounding_box();

	void Transform(const frame& f);

	/** Create Freeform Surface into the Scene Graph.
	*/
	void CreateSurface(const std::vector<vec3>& points,
		const std::vector<vec3>& normals,
		const std::vector<int>& point_indices,
		const std::vector<int>& normal_indices);



private:


	GeometryDatabase*	geometry_database_;
	TextureDatabase*	texture_database_;
	MaterialDatabase*   material_database_;
	SceneNodeMap		scene_node_database_;
	long				shape_id_;
	GMaterial*			default_material_;

	/** Check if input scene node g is root scene node.*/
	RootSceneNodes		ground_level_;
};

class GeometryDatabase {

public:
	friend class Geometry;

public:
	GeometryDatabase(SceneGraph* graph = 0);

public:

	~GeometryDatabase();

	Geometry*	CreateGeometry();

	int			GetGeometryCount() const;
	GeometryMap::iterator BeginGeometry();
	GeometryMap::iterator EndGeometry();

	void log_bounding_box();
private:

	SceneGraph*			connected_graph_;
	GeometryMap			geometry_map_;
	long				geometry_id_;
};



class GPolygon {
	
public:
	
	GPolygon(Geometry* mother = 0): connected_geometry_(mother), point_indices_(), normal_indices_(), texture_indices_(), display_list_(-1), display_list_reverse_(-1), material_semantic_(), bounding_box_() {}

	std::vector<unsigned int>& GetPointIndices() { return point_indices_; }
	std::vector<unsigned int>& GetNormalIndices() { return normal_indices_; }
	std::vector<unsigned int>& GetTextureIndices() { return texture_indices_; }

	/** After any update upon the points_, and so on, call this function
	    so that the display_list_ can be updated.
	*/
	void Update();

	/** 
	    Return display list for this geometry.
		@return display list.
	*/
	int GetDisplayList() const { return display_list_; }
	int GetDisplayListReverse() const { return display_list_reverse_; }

	const box3&	GetBoundingBox() const { return bounding_box_; }

public:

	int						polygon_type_;
	Geometry*				connected_geometry_;
	std::vector<unsigned int>			point_indices_;
	std::vector<unsigned int>			normal_indices_;
	std::vector<unsigned int>			texture_indices_;
	std::vector<unsigned int>			color_indices_;
	std::vector<unsigned int>           tangent_indices_;
	std::vector<unsigned int>		    binormal_indices_;
	std::vector<unsigned int>			faces_; // faces_[i]-gon
	int						display_list_;
	int						display_list_reverse_;
	box3					bounding_box_;

	WString				material_semantic_;
};

class Geometry {

public:
	Geometry(GeometryDatabase* db);

public:

	~Geometry();

	long GetId() const { return id_; }
	void SetId(long id) { id_ = id; }

	/** After any update upon the points_, and so on, call this function
	    so that the display_list_ can be updated.
	*/
	void Update();


	size_t GetPolygonCount() const { return polygons_.size() ; }

	GPolygon* GetPolygon(size_t i) { if (i < GetPolygonCount() && i >= 0) return polygons_[i]; return 0;}

	GPolygon* CreateNewPolygon() { GPolygon* poly = new GPolygon(this); polygons_.push_back(poly); return poly; }


	const box3&	GetBoundingBox() const { return bounding_box_; }

private:

	long					id_;


	GeometryDatabase*		geometry_database_;

public:

	std::vector<struct vec3>	points_;
	std::vector<struct vec4>    colors_;
	std::vector<struct vec3>	normals_;
	std::vector<struct vec2>	textures_;
	std::vector<struct vec3>    tangents_;
	std::vector<struct vec3>    binormals_;

	PolygonArray			polygons_;
	box3					bounding_box_;

};


class GPolygonInstance {
public:
	GPolygonInstance(GPolygon* poly, GMaterial* mat) : polygon_(poly), material_(mat) {}

	GPolygonInstance(const GPolygonInstance& cpy) : polygon_(cpy.polygon_), material_(cpy.material_) {}

	GPolygon* GetPolygon() { return polygon_; }
	GMaterial* GetMaterial() { return material_; }

public:

	GMaterial* material_;
	GPolygon*  polygon_;
};

class GeometryInstance {
public:
	GeometryInstance(Geometry* geom)
		: bound_geometry_(geom), polygon_instances_()
	{
	}

	GeometryInstance(const GeometryInstance& cpy)
		: bound_geometry_(cpy.bound_geometry_), polygon_instances_(cpy.polygon_instances_) 
	{
	}

	~GeometryInstance() 
	{
		for (int i = 0 ; i < polygon_instances_.size() ; i++) {
			delete polygon_instances_[i];
		}
		polygon_instances_.clear();
	}
	Geometry* GetGeometry() { return bound_geometry_; }
	const Geometry* GetGeometry() const { return bound_geometry_; }

	void AddPolygonInstance(GPolygon* polygon, GMaterial* mat)
	{
		bool found =false;
		for (uint i = 0; i < bound_geometry_->GetPolygonCount(); i++) {
			if (bound_geometry_->polygons_[i] == polygon) found = true;
		}
		if (!found) return;
		polygon_instances_.push_back(new GPolygonInstance(polygon, mat));
	}

	size_t GetPolygonInstanceCount() { return polygon_instances_.size(); }
	GPolygonInstance* GetPolygonInstance(size_t idx) { return polygon_instances_[idx]; }

public:

	Geometry* bound_geometry_;

	PolygonInstances polygon_instances_;

private:
	GeometryInstance() {}
};

class SceneNode {

public:

	SceneNode(SceneGraph* gr);

public:

	~SceneNode();
	void AddChild(SceneNode* shape);

	void RemoveChild(SceneNode* shape);

	const SceneNodeChildren& GetChildren() const { return children_; }
	const SceneNodeParents& GetParents() const { return parents_; }

	SceneNodeChildren& GetChildren()  { return children_; }
	SceneNodeParents& GetParents()  { return parents_; }

	size_t GetParentsCount() const { return parents_.size(); }
	size_t GetChildrenCount() const { return children_.size(); }

	SceneNode* GetChild(size_t index) { return index < children_.size() ? children_[index]: 0; }
	SceneNode* GetParent(size_t index) { return index < parents_.size() ? parents_[index]: 0; }

	SceneNode* FindRootNodeOfThis();

	void GetHierarchyChainToThis(std::list<SceneNode*>& chain) ;


	/** Compute ray surface intersection */
	bool	RayIntersect(const line& wolrd_ray, vec3& output_pnt) ;

	virtual int runtime_type() const;

	long GetSceneNodeId() const { return id_; }

	void SetSceneNodeId(long id) { id_ = id; }

	bool IsRootNode() const { return GetParentsCount() == 0; }

	SceneGraph* GetSceneGraph() const  { return scene_graph_; }

	void CopyTo(SceneNode* dest);

	GeometryInstance* AddGeometryInstance(Geometry* a) 
	{ 
		if (!a) return 0;
		for (uint i = 0 ; i < geometry_instances_.size(); i++)
			if (geometry_instances_[i]->bound_geometry_ == a) return 0;

		GeometryInstance* inst = new GeometryInstance(a);
		geometry_instances_.push_back(inst); 
		return inst;
	}

	void SetTransform(frame& f);

	void SetScale(vec3& s);

	void SetOffsetTransform(frame& f);

	void SetOffsetScale(vec3& s);

	size_t GetGeometryInstanceCount() const { return geometry_instances_.size(); }
	GeometryInstance* GetGeometryInstance(int i) { return geometry_instances_[i]; }
	const GeometryInstance* GetGeometryInstance(int i) const { return geometry_instances_[i]; }

	GeometryInstances& GetGeometryInstances() { return geometry_instances_; }


	GFrame& GetTransform() { return transform_; }

	const GFrame& GetTransform() const { return transform_; }

	GFrame& GetOffsetTransform() { return offset_; }

	const GFrame& GetOffsetTransform() const { return offset_; }
	box3  GetWorldBoundingBox(bool root = false) const;

	void  CollectTriangles(std::vector<Triangle>& triangles);

	long GetId() const { return id_; }


protected:
	
	long					id_;

	SceneNodeParents		parents_;
	SceneNodeChildren		children_;
	SceneGraph*				scene_graph_;

	GFrame					transform_;
	GFrame					offset_; //

	GeometryInstances       geometry_instances_;

private:
	// cannot be called
	SceneNode();
};


};

#endif