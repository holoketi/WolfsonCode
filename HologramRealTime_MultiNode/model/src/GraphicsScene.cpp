#include	"model/GraphicsScene.h"

#include	"material/TextureDatabase.h"
#include	"material/MaterialDatabase.h"

namespace model_kernel {

static std::map<SceneNode*, SceneNode*>* copy_map;
static std::set<SceneNode*>* kDeleteSet;

SceneNode::SceneNode()
: scene_graph_(0), parents_(), children_(), transform_(), geometry_instances_()
{
	if (!copy_map) {
		copy_map = new std::map<SceneNode*, SceneNode*>();
		kDeleteSet = new  std::set<SceneNode*>();
	}
}

SceneNode::SceneNode(SceneGraph* gr)
: scene_graph_(gr), parents_(), children_(), transform_(), offset_(), geometry_instances_()
{
}

int SceneNode::runtime_type() const
{
	return kTopologySceneNodeType;
}

SceneNode* SceneNode::FindRootNodeOfThis()
{
	return scene_graph_->FindRootFrom(this);
}

void 
SceneNode::GetHierarchyChainToThis(std::list<SceneNode*>& chain) 
{
	chain.push_front(this);
	if (GetParentsCount()) {
		GetParent(0)->GetHierarchyChainToThis(chain);
	}
}

void  
SceneNode::CollectTriangles(std::vector<Triangle>& triangles)
{
	std::list<SceneNode*> chain_;
	chain_.clear();

	GetHierarchyChainToThis(chain_);

	for (int i = 0 ; i < GetGeometryInstanceCount() ; i++) {
		GeometryInstance* g_inst =GetGeometryInstance(i);
		for (int j = 0 ; j < g_inst->GetPolygonInstanceCount(); j++) {
			GPolygonInstance* p_inst = g_inst->GetPolygonInstance(j);
			GPolygon* pp = p_inst->GetPolygon();
			vec3 ret;

			for (int k = 0 ; k < pp->GetPointIndices().size(); ) {
				vec3 p1 = pp->connected_geometry_->points_[pp->GetPointIndices()[k++]];
				vec3 p2 = pp->connected_geometry_->points_[pp->GetPointIndices()[k++]];
				vec3 p3 = pp->connected_geometry_->points_[pp->GetPointIndices()[k++]];
				for (std::list<SceneNode*>::reverse_iterator i = chain_.rbegin(); i != chain_.rend() ; i++)
				{
					GFrame& f = (*i)->GetTransform();
					p1 *= f.scale;
					p2 *= f.scale;
					p3 *= f.scale;

					p1 = f.to_world(p1);
					p2 = f.to_world(p2);
					p3 = f.to_world(p3);
				}
				triangles.push_back(Triangle(p1,p2,p3));
			}
		}
	}
}

bool	
SceneNode::RayIntersect(const line& world_ray, vec3& output_pnt) 
{
	std::list<SceneNode*> chain_;
	chain_.clear();

	GetHierarchyChainToThis(chain_);
	line model_ray(world_ray);

	for (std::list<SceneNode*>::iterator i = chain_.begin(); i != chain_.end() ; i++)
	{
		GFrame& f = (*i)->GetTransform();
		model_ray = f.to_model(model_ray);
		vec3 pos1 = model_ray.get_position();
		vec3 pos2 = pos1 + model_ray.get_direction();
		pos1 /= f.scale;
		pos2 /= f.scale;
		model_ray.set_position(pos1);
		model_ray.set_direction(unit(pos2-pos1));
	}
 
	bool any_hit = false;
	real depth = 1000000000000000.0;

	for (int i = 0 ; i < GetGeometryInstanceCount() ; i++) {
		GeometryInstance* g_inst =GetGeometryInstance(i);
		for (int j = 0 ; j < g_inst->GetPolygonInstanceCount(); j++) {
			GPolygonInstance* p_inst = g_inst->GetPolygonInstance(j);
			GPolygon* pp = p_inst->GetPolygon();
			vec3 ret;
			box3 bbox = pp->GetBoundingBox();
			//if (!bbox.intersect(model_ray, ret)) continue;

			for (int k = 0 ; k < pp->GetPointIndices().size(); ) {
				vec3 p1 = pp->connected_geometry_->points_[pp->GetPointIndices()[k++]];
				vec3 p2 = pp->connected_geometry_->points_[pp->GetPointIndices()[k++]];
				vec3 p3 = pp->connected_geometry_->points_[pp->GetPointIndices()[k++]];
				real t, u, v;
				if (intersect_triangle(model_ray.get_position().v, model_ray.get_direction().v, p1.v, p2.v, p3.v, &t, &u, &v)) {
					any_hit = true;
					//vec3 a = (t*p1) + (u*p2) + (v*p3);
					vec3 a = model_ray.get_position() + (model_ray.get_direction() * t);
					vec3 b = (u*p1) + (v*p2) + ((1.0-u-v)*p3);
					//LOG("pnt a %f %f %f, b %f %f %f\n", a[0], a[1], a[2], b[0], b[1], b[2]);
					real d = norm(a-model_ray.get_position());
					if (d < depth) {
						output_pnt = a;
						depth = d;
					}
				}
			}
		}
	}

	if (!any_hit) return false;
	vec3 model_pnt = output_pnt;

	for (std::list<SceneNode*>::reverse_iterator i = chain_.rbegin(); i != chain_.rend(); i++) {
		GFrame& f = (*i)->GetTransform();
		output_pnt *= f.scale;
		output_pnt = f.to_world(output_pnt);
		
	}

	return true;
}

void SceneNode::AddChild(SceneNode* c)
{
	bool found = false;
	for (unsigned int i = 0 ; i < children_.size() ; i++) {
		if (children_[i] == c) {
			found = true;
			break;
		}
	}

	if (found) return;
	children_.push_back(c);

	found = false;
	for (unsigned int i = 0 ; i < c->GetParents().size() ; i++) {
		if (c->GetParents()[i] == this) {
			found = true;
			break;
		}
	}
	if (found) return;
	c->GetParents().push_back(this);
}

void SceneNode::SetTransform(frame& f)
{
	transform_ = f;
}

void SceneNode::SetScale(vec3& s)
{
	transform_.SetScale(s);
}

void SceneNode::SetOffsetTransform(frame& f)
{
	offset_ = f;
}

void SceneNode::SetOffsetScale(vec3& s)
{
	offset_.SetScale(s);
}

void SceneNode::RemoveChild(SceneNode* c)
{
	SceneNodeChildren::iterator found = children_.begin();
	for (; found != children_.end() ; found++) {
		if (*found == c) break;
	}

	if (found != children_.end()) {
		children_.erase(found);
	}

	found = c->GetParents().begin();
	for (; found != c->GetParents().end() ; found++) {
		if (*found == this) break;
	}

	if (found != c->GetParents().end()) {
		c->GetParents().erase(found);
	}
}

box3  SceneNode::GetWorldBoundingBox(bool root ) const
{
	box3 ret;

	bool hit = false;
	for (size_t j = 0 ; j < GetGeometryInstanceCount() ; j++) {
		ret.extend(GetGeometryInstance(j)->GetGeometry()->GetBoundingBox());
		hit = true;
	}

	if (hit && !root) ret = transform_.Transform(ret);

	box3 children_box;
	hit = false;
	for (size_t i = 0; i < children_.size() ; i++) {
		SceneNode* child = children_[i];

		children_box.extend(child->GetWorldBoundingBox());
		hit = true;
	}

	if (hit && !root) children_box = transform_.Transform(children_box);

	ret.extend(children_box);

	return ret;
}

void GeometryDatabase::log_bounding_box()
{
	GeometryMap::iterator i = geometry_map_.begin();
	for (; i != geometry_map_.end() ; i++) {
		i->second->bounding_box_.print();
	}
}

SceneGraph::SceneGraph()
: scene_node_database_(), shape_id_(0), ground_level_(), default_material_(0)
{
	geometry_database_ = new GeometryDatabase(this);
	texture_database_ = MaterialDatabase::GetTextureDBInstance();
	material_database_ = MaterialDatabase::GetInstance();
	if (!copy_map) {
		copy_map = new std::map<SceneNode*, SceneNode*>();
		kDeleteSet = new  std::set<SceneNode*>();
	}
}

void SceneNode::CopyTo(SceneNode* dest)
{
	dest->transform_ = transform_;
	dest->parents_ = parents_;
	dest->children_ = children_;
	dest->scene_graph_ = scene_graph_;
	dest->geometry_instances_ = geometry_instances_;
	for (int i = 0 ; i < parents_.size() ; i++) {
		std::map<SceneNode*, SceneNode*>::iterator found = copy_map->find(parents_[i]);
		if (found != copy_map->end()) {
			dest->parents_[i] = found->second;
		}
	}

	for (int i = 0 ; i < children_.size() ; i++) {
		std::map<SceneNode*, SceneNode*>::iterator found = copy_map->find(children_[i]);
		if (found != copy_map->end()) {
			dest->children_[i] = found->second;
		}
	}
}

SceneNode* SceneGraph::Copy(SceneNode* input)
{
	SceneNode* ret = 0;
	copy_map->clear();

	if (copy_map->find(input) == copy_map->end()) {
		(*copy_map)[input] = ret =CreateSceneNode();
	}

	for (unsigned int i = 0 ; i < input->GetChildren().size(); i++) {
		CopyRecurse(input->GetChildren()[i]);
	}

	std::map<SceneNode*, SceneNode*>::iterator ci = copy_map->begin();

	for ( ; ci != copy_map->end() ; ci++) {
		SceneNode* org = ci->first;
		SceneNode* dest = ci->second;

		org->CopyTo(dest);
	}

	if (input->GetParentsCount() == 0 && ret) {
		ground_level_.push_back(ret);
	}

	return ret;
}

SceneNode* SceneGraph::CopyRecurse(SceneNode* input)
{
	SceneNode* ret;
	if (copy_map->find(input) == copy_map->end()) {
		(*copy_map)[input] = ret = CreateSceneNode();
	}

	for (unsigned int i = 0 ; i < input->GetChildren().size(); i++) {
		CopyRecurse(input->GetChildren()[i]);
	}
	return ret;
}

void SceneGraph::log_bounding_box()
{
	geometry_database_->log_bounding_box();
}

SceneGraph::~SceneGraph()
{
	for (SceneNodeMap::iterator i = scene_node_database_.begin(); i != scene_node_database_.end() ; i++) {
		delete i->second;
	}
	scene_node_database_.clear();

	delete geometry_database_;
}

void SceneGraph::DeleteSceneNodeRecurse(SceneNode* g)
{
	for (unsigned int i = 0 ; i < g->GetChildren().size(); i++) {
		DeleteSceneNodeRecurse(g->GetChildren()[i]);
	}

	scene_node_database_.erase(g->GetSceneNodeId());
	//LOG("after erasing the group from the model map\n");
	RootSceneNodes::iterator i = FindRootSceneNode(g);
	if (i != ground_level_.end())
		ground_level_.erase(FindRootSceneNode(g));

	kDeleteSet->insert(g);
}

void SceneGraph::DeleteSceneNode(SceneNode* g)
{
	kDeleteSet->clear();

	for (unsigned int i = 0 ; i < g->GetChildren().size(); i++) {
		DeleteSceneNodeRecurse(g->GetChildren()[i]);
	}

	scene_node_database_.erase(g->GetSceneNodeId());
	//LOG("after erasing the group from the model map\n");
	RootSceneNodes::iterator i = FindRootSceneNode(g);
	if (i != ground_level_.end())
		ground_level_.erase(FindRootSceneNode(g));

	kDeleteSet->insert(g);

	std::set<SceneNode*>::iterator si = kDeleteSet->begin();
	for (; si != kDeleteSet->end(); si++) {
		delete *si;
	}
}

void SceneGraph::CreateSurface(const std::vector<vec3>& points,
	const std::vector<vec3>& normals,
	const std::vector<int>& point_indices,
	const std::vector<int>& normal_indices)
{
	SceneNode* root = CreateSceneNode();
	AddRootSceneNode(root);

	SceneNode* node = CreateSceneNode();
	root->AddChild(node);

	vec3 center(0);
	for (int i = 0 ; i < points.size() ; i++) {
		center += points[i];
	}
	center = center/(real)points.size();
	node->GetTransform().set_origin(center);

	Geometry* geom = geometry_database_->CreateGeometry();

	geom->points_.resize(points.size());
	geom->normals_.resize(normals.size());

	for (int i = 0 ; i < points.size() ; i++) {
		geom->points_[i] =node->GetTransform().to_model(points[i]);
	}

	for (int i = 0 ; i < normals.size() ; i++) {
		geom->normals_[i] = normals[i];
	}

	GPolygon* poly = geom->CreateNewPolygon();


	poly->point_indices_.resize(point_indices.size());
	poly->normal_indices_.resize(normal_indices.size());

	for (int i = 0 ; i < point_indices.size() ; i++) {
		poly->point_indices_[i] = point_indices[i];
	}
	for (int i = 0 ; i < normal_indices.size() ; i++) {
		poly->normal_indices_[i] = normal_indices[i];
	}
	
	geom->Update();

	GeometryInstance* ginst = node->AddGeometryInstance(geom);

	ginst->AddPolygonInstance(poly, material_database_->GetDefaultMaterial());
}

void SceneGraph::Transform(const frame& f)
{
	RootSceneNodes::iterator i = ground_level_.begin();
	for ( ; i != ground_level_.end() ; i++) {
		(*i)->GetTransform().basis[0] = f.to_model_normal((*i)->GetTransform().basis[0]);
		(*i)->GetTransform().basis[1] = f.to_model_normal((*i)->GetTransform().basis[1]);
		(*i)->GetTransform().basis[2] = f.to_model_normal((*i)->GetTransform().basis[2]);
		(*i)->GetTransform().eye_position = f.to_model((*i)->GetTransform().eye_position);
		(*i)->GetTransform().update();
	}
}

RootSceneNodes::iterator 
SceneGraph::FindRootSceneNode(SceneNode *g)
{
	RootSceneNodes::iterator i = ground_level_.begin();
	for ( ; i != ground_level_.end() ; i++)
		if (*i == g) return i;

	return ground_level_.end();
}

// g°¡ root SceneNodeÀÎ°¡?
bool   
SceneGraph::IsRootSceneNode(SceneNode* g) const
{
	RootSceneNodes::const_iterator i = ground_level_.begin();
	for ( ; i != ground_level_.end() ; i++)
		if (*i == g) return true;

	return false;
}


SceneNode*	SceneGraph::FindRootFrom(SceneNode*g) const
{

	while (g) {
		SceneNodeParents& parents = g->GetParents();
		if (!parents.size()) return g;
		SceneNode *gp = parents[0];
		if (gp) g = gp;
		else return g;
	}

	return g;
}


void 
SceneGraph::GetRootSceneNodes(std::vector<SceneNode*>& g_level) const
{
	g_level.clear();

	for (RootSceneNodes::const_iterator i = ground_level_.begin() ; i != ground_level_.end() ; i++) 
	{
		SceneNode *g = *i;
		g_level.push_back(g);
	}
}

size_t 
SceneGraph::GetRootSceneCount() const
{
	return ground_level_.size();
}

void
SceneGraph::AddRootSceneNode(SceneNode*g)
{
	if (g->GetParents().size()) return;

	if (FindRootSceneNode(g) == ground_level_.end()) {
		//LOG("group added %p\n", g);
		ground_level_.push_back(g);
	}
}

void	
SceneGraph::RemoveRootSceneNode(SceneNode *g)
{
	if (g->GetParents().size()) return;

	ground_level_.erase(FindRootSceneNode(g));
}



SceneNode*
SceneGraph::CreateSceneNode()
{
	SceneNode *group = new SceneNode(this);
	group->SetSceneNodeId(shape_id_);
	scene_node_database_[shape_id_] = group;
	shape_id_++;
	return group;
}



GeometryDatabase::GeometryDatabase(SceneGraph* graph)
: connected_graph_(graph)
, geometry_map_()
, geometry_id_(0)
{
}
Geometry*
GeometryDatabase::CreateGeometry()
{
	Geometry *g = new Geometry(this);
	g->SetId(geometry_id_);
	geometry_map_[geometry_id_] = g;
	geometry_id_++;
	return g;
}

GeometryDatabase::~GeometryDatabase()
{
	for (GeometryMap::iterator i = geometry_map_.begin() ; i != geometry_map_.end() ; i++) {
		delete i->second;
	}
	geometry_map_.clear();
}

Geometry::~Geometry()
{
	for (int i = 0 ; i < polygons_.size() ; i++) {
		delete polygons_[i];
	}
	polygons_.clear();
}

int			GeometryDatabase::GetGeometryCount() const
{
	return geometry_map_.size();
}

GeometryMap::iterator GeometryDatabase::BeginGeometry()
{
	return geometry_map_.begin();
}

GeometryMap::iterator GeometryDatabase::EndGeometry()
{
	return geometry_map_.end();
}

Geometry::Geometry(GeometryDatabase* db)
: geometry_database_(db)
, points_()
, normals_()
, textures_()
, polygons_()
//, bounding_box_()
{
}

void Geometry::Update()
{

	bounding_box_.make_empty();
	for (int j = 0 ; j < polygons_.size() ; j++) {
		polygons_[j]->Update();	
		bounding_box_.extend(polygons_[j]->GetBoundingBox());
	}
}

void GPolygon::Update()
{
	if (display_list_<0) 
		display_list_ = glGenLists(1);

	bounding_box_.make_empty();
	glNewList (display_list_, GL_COMPILE);
	glBegin(GL_TRIANGLES);


	for (int i = 0 ; i < GetPointIndices().size(); i++) {
		if (connected_geometry_->normals_.size()) gl_normal(unit(connected_geometry_->normals_[GetNormalIndices()[i]]));
		if (connected_geometry_->textures_.size() && i < GetTextureIndices().size()) 
			gl_tex_coord(connected_geometry_->textures_[GetTextureIndices()[i]]);

		if (GetPointIndices()[i] < connected_geometry_->points_.size()) bounding_box_.extend(connected_geometry_->points_[GetPointIndices()[i]]);
		gl_vertex(connected_geometry_->points_[GetPointIndices()[i]]);
	}

	glEnd();
	glEndList();
}

SceneNode::~SceneNode()
{
	for (int i = 0 ; i < geometry_instances_.size() ; i++) 
	{
		delete geometry_instances_[i];
	}
	geometry_instances_.clear();
}
};