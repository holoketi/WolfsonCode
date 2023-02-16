

#include "model/AnimCharacter.h"
#include "model/AnimationChannel.h"
#include "model/AnimationCurve.h"

#include <random>
#include <graphics/camera.h>
#include "material/GTexture.h"

namespace model_kernel {



std::map<AnimCharacter*, unsigned int> kCharacterVBOMap;


AnimCharacter::AnimCharacter()
	:points_(), normals_(), textures_(), polygons_(), joints(), influences(), root_joint_(0), anim_clips_()
{
}

void AnimCharacter::createRootJoint(const std::vector<Transform*>& m, const wchar_t* n)
{
	root_joint_ = new JointNode(n);
	root_joint_->setTransforms(m);
}

void AnimCharacter::addChild(const wchar_t* parent, const std::vector<Transform*>& m, const wchar_t* n)
{
	JointNode* p = root_joint_->search(parent);

	if (p) p->createChild(m, n);
}

AnimCharacter::~AnimCharacter()
{
	if (polygons_.size()) {
		for (int i = 0 ;i < polygons_.size() ; i++)
			delete polygons_[i];
	}

	if (root_joint_) delete root_joint_;

	std::map<AnimCharacter*, unsigned int>::iterator f = kCharacterVBOMap.find(this);
	if (f != kCharacterVBOMap.end()) {
		glDeleteBuffersARB(1, &f->second);
		kCharacterVBOMap.erase(f);
	}
}

void AnimCharacter::setPoints(const std::vector<vec3>& pnts)
{
	points_.resize(pnts.size());

	for (int j = 0 ; j < points_.size() ; j++) {
			points_[j] = pnts[j];
	}
}

void AnimCharacter::setNormals(const std::vector<vec3>& pnts)
{
	normals_.resize(pnts.size());

	for (int j = 0 ; j < normals_.size() ; j++) {
			normals_[j] = pnts[j];
	}
}

void AnimCharacter::setTextureCoords(const std::vector<vec2>& pnts)
{
	textures_.resize(pnts.size());

	for (int j = 0 ; j < textures_.size() ; j++) {
			textures_[j] = pnts[j];
	}
}


void AnimCharacter::setSubMeshes(const std::vector<GPolygon*>& polygons, const std::vector<GMaterial*>& mats)
{
	polygons_.resize(polygons.size());
	for (int i = 0 ; i < polygons.size() ; i++) {
		polygons_[i] = new CSubMesh(this);
		polygons_[i]->setPointIndices(polygons[i]->point_indices_);
		polygons_[i]->setTextureCoordIndices(polygons[i]->texture_indices_);
		polygons_[i]->setNormalIndices(polygons[i]->normal_indices_);
		polygons_[i]->connected_geometry_ = this;
		polygons_[i]->setFaceSides(polygons[i]->faces_);
		polygons_[i]->poly_type_ = polygons[i]->polygon_type_;
		polygons_[i]->material_ = mats[i];
	}
}

void CSubMesh::setPointIndices(const std::vector<unsigned int>& points)
{
	point_indices_.resize(points.size());
	for (int i = 0 ;i < point_indices_.size() ; i++)
		point_indices_[i] = points[i];
}

void CSubMesh::setNormalIndices(const std::vector<unsigned int>& points)
{
	normal_indices_.resize(points.size());
	for (int i = 0 ;i < normal_indices_.size() ; i++)
		normal_indices_[i] = points[i];
}
void CSubMesh::setTextureCoordIndices(const std::vector<unsigned int>& points)
{
	texture_indices_.resize(points.size());
	for (int i = 0 ;i < texture_indices_.size() ; i++)
		texture_indices_[i] = points[i];
}
void CSubMesh::setFaceSides(const std::vector<unsigned int>& sides)
{
	faces_.resize(sides.size());
	for (int i = 0 ;i < faces_.size() ; i++)
		faces_[i] = sides[i];
}


void AnimCharacter::addJoint(const wchar_t* id, const user::Matrix4& bindPose)
{
	joints.push_back(SkinControllerJoint(id, bindPose));
}

void AnimCharacter::addSkinControllerVertex(const std::vector<JointWeightPair>& cv)
{
	int k = influences.size();
	influences.resize(k+1);
	for (int m = 0 ; m < cv.size() ; m++)
		influences[k].AddPair(cv[m].jointIndex, cv[m].weight);
}



JointNode::JointNode(const wchar_t* name)
	:parent_(0), transforms_()
{
	int s = wcslen(name) + 1;
	for (size_t i = 0 ; i < s ;++i)
		name_[i] = name[i];
}

user::Matrix4 JointNode::localTransform()
{
	user::Matrix4 ret = user::Matrix4::identity();

	for (int i = 0 ; i < transforms_.size() ; i++) {
		ret = ret * transforms_[i]->ToMatrix();
	}
	return ret;
}

int JointNode::transformCount() const
{
	return transforms_.size();
}

Transform* JointNode::transform(int idx)
{
	return transforms_[idx];
}


user::Matrix4 JointNode::worldTransform()
{
	user::Matrix4 ret = localTransform();

	JointNode* paren = parent();

	while(paren) {
		ret = paren->localTransform() * ret;
		paren = paren->parent();
	}

	return ret;
}

user::Matrix4 JointNode::worldTransformFromUpdatedLocal(std::map<JointNode*, user::Matrix4*>& joint_mat_map)
{
	user::Matrix4 ret = *joint_mat_map[this];

	JointNode* paren = parent();

	while(paren) {
		ret = (*joint_mat_map[paren]) * ret;
		paren = paren->parent();
	}

	return ret;
}

void JointNode::removeChild(int i)
{
	if (i < childCount()) {
		delete child(i);
		for (int j = i ; j < childCount()-1 ; j++) {
			children_[j] = children_[j+1];
		}
		children_.resize(childCount()-1);
	}
}

void JointNode::clear() 
{
	for (int i = 0 ; i < childCount() ;++i){
		delete child(i);
	}
	children_.clear();
}
void JointNode::addChild(JointNode* a)
{
	children_.push_back(a);
	a->parent(this);
}

JointNode* JointNode::root()
{
	if (parent()) {
		return parent()->root();
	}
	return this;
}
JointNode* JointNode::createChild(const std::vector<Transform*>& m, const wchar_t* n)
{
	JointNode* a = new JointNode(n);
	a->setTransforms(m);
	addChild(a);
	return a;
}


void JointNode::name(const wchar_t* n)
{
	int s = wcslen(n) + 1;
	for (size_t i = 0 ; i < s ;++i)
		name_[i] = n[i];
}

const wchar_t* JointNode::name() const
{
	return name_;
}

void JointNode::setTransforms(const std::vector<Transform*>& transforms)
{
	transforms_.resize(transforms.size());
	for (int i = 0 ; i < transforms_.size() ; i++)
		transforms_[i] = transforms[i];
}

JointNode* JointNode::search(const wchar_t* n)
{	
	if (!wcscmp(n, name_)) {
		return this;
	}

	for (int i = 0 ; i < childCount() ;++i){
		JointNode* ret = child(i)->search(n);
		if (ret) return ret;
	}
	return NULL;
}

void AnimCharacter::updateJointRelationship()
{
	for (int i = 0 ; i < joints.size() ; i++) {
		JointNode* n = root_joint_->search(joints[i].id);
		if (n) {
			joints[i].node_ = n;
		}
	}
}

static const wchar_t* MATRIX[16] = { L"(0)(0)", L"(1)(0)", L"(2)(0)", L"(3)(0)", L"(0)(1)", L"(1)(1)", L"(2)(1)", L"(3)(1)", L"(0)(2)", L"(1)(2)", L"(2)(2)", L"(3)(2)", L"(0)(3)", L"(1)(3)", L"(2)(3)", L"(3)(3)" };
static const wchar_t* XYZW[4] = { L".X", L".Y", L".Z", L".W" };
static const wchar_t* ROTATE_AXIS[4] = { L".X", L".Y", L".Z", L".ANGLE" };

static int MatrixElementIndex(const WString& element)
{
	for (int i = 0 ; i < 16 ; i++) {
		if (WString(MATRIX[i])==element) return i;
	}
	return 0;
}

static int XYZWElementIndex(const WString& element)
{
	for (int i = 0 ; i < 3 ; i++) {
		if (WString(XYZW[i])==element) return i;
	}
	return 0;
}
static int ROTATE_AXISElementIndex(const WString& element)
{
	for (int i = 0 ; i < 4 ; i++) {
		if (WString(ROTATE_AXIS[i])==element) return i;
	}
	return 0;
}

static AnimationChannel* findChannelWithTransformId(const WString& sid, std::vector<AnimationChannel*>& channels)
{
	for (int i = 0 ; i < channels.size() ; i++) {
		WString target = channels[i]->getTarget();
		size_t splitIndex =  target.find_first_of(L"/");
		WString tmp = target.substr(0, splitIndex);
		target = target.substr(splitIndex+1);
		if (target == sid) {
			return channels[i];
		}
	}
	return 0;
}

void AnimCharacter::computeTransformsAt(real t, std::vector<user::Matrix4>& updated_transforms)
{

	std::vector<user::Matrix4> transforms;
	transforms.resize(joints.size());
	updated_transforms.resize(joints.size());

	for (int i = 0 ; i < joints.size() ; i++) {
		WString target_name(joints[i].node_->name());
		std::vector<AnimationChannel*> chans;
		if (anim_clips_.size()) {
			chans = anim_clips_[0]->FindChannelByTargetName(target_name); // multiple channel
		}
		
		if (!chans.size()) {
			transforms[i] = joints[i].node_->localTransform();
		}
		else {
			user::Matrix4 my_mat = user::Matrix4::identity();
			for (int j = 0 ; j < joints[i].node_->transformCount() ; j++) {
				Transform* tr = joints[i].node_->transform(j);
				AnimationChannel* chan = findChannelWithTransformId(tr->GetSubId(), chans);
				WString element = chan?chan->getTargetQualifier():WString(L"");
				if (chan) {
					if (tr->GetType()==Transform::MATRIX) {
						user::Matrix4 cur_mat;
						if (!element.size()) {
							for (int k = 0 ; k < chan->GetCurveCount(); k++) {
								AnimationCurve* crv = chan->GetCurve(k);
								float val = crv->Evaluate((float)t);
								cur_mat[k%4][k/4] =val;
							}
							my_mat *= cur_mat;
						}
						else {
							Matrix* rot = (Matrix*) tr;
							cur_mat = rot->ToMatrix();
							float val = chan->GetCurve(0)->Evaluate((float)t);
							int k = MatrixElementIndex(element);
							cur_mat[k%4][k/4] =val;
							my_mat *= cur_mat;
						}
					}
					else if (tr->GetType() == Transform::TRANSLATION) {
						user::Matrix4 cur_mat;
						if (!element.size()) {
							graphics::vec3 tran;
							for (int k = 0 ; k < chan->GetCurveCount(); k++) {
								AnimationCurve* crv = chan->GetCurve(k);
								float val = crv->Evaluate((float)t);
								tran[i] = val;
							}
						
							cur_mat.translation(user::Vector3(tran[0], tran[1], tran[2]));
							my_mat *= cur_mat;
						} else {
							Translation* rot = (Translation*) tr;
							graphics::vec3 tran = rot->GetTranslation();
							float val = chan->GetCurve(0)->Evaluate((float)t);
							int k = XYZWElementIndex(element);
							tran[k] = val;
							cur_mat.translation(user::Vector3(tran[0], tran[1], tran[2]));
							my_mat *= cur_mat;
						}
					}
					else if (tr->GetType() == Transform::SCALE) {
						user::Matrix4 cur_mat;
						if (!element.size()) {
							graphics::vec3 tran;
							for (int k = 0 ; k < chan->GetCurveCount(); k++) {
								AnimationCurve* crv = chan->GetCurve(k);
								float val = crv->Evaluate((float)t);
								tran[i] = val;
							}
							user::Matrix4 cur_mat;
							cur_mat.scaling(user::Vector3(tran[0], tran[1], tran[2]));
							my_mat *= cur_mat;
						} else {
							Scale* sc = (Scale*) tr;
							graphics::vec3 tran = sc->GetScale();
							float val = chan->GetCurve(0)->Evaluate((float)t);
							int k = XYZWElementIndex(element);
							tran[k] = val;
							cur_mat.scaling(user::Vector3(tran[0], tran[1], tran[2]));
							my_mat *= cur_mat;
						}
					}
					else if (tr->GetType() == Transform::ROTATION) {
						user::Matrix4 cur_mat;

						if (!element.size()) {
							graphics::vec4 tran;
							for (int k = 0 ; k < chan->GetCurveCount(); k++) {
								AnimationCurve* crv = chan->GetCurve(k);
								float val = crv->Evaluate((float)t);
								tran[k] = val;
							}
						
							cur_mat.rotation(graphics::radian(tran[3]), tran[0], tran[1], tran[2]);
							my_mat *= cur_mat;
						}
						else {
							graphics::vec4 axis_angle;
						
							Rotation* rot = (Rotation*) tr;
							vec3 axis = rot->GetAngleAxis();
							axis_angle[0] = axis[0];
							axis_angle[1] = axis[1];
							axis_angle[2] = axis[2];
							axis_angle[3] = rot->GetAngle();
							float val = chan->GetCurve(0)->Evaluate((float)t);
							int k = ROTATE_AXISElementIndex(element);
							axis_angle[k] = val;
							cur_mat.rotation(graphics::radian(axis_angle[3]), axis_angle[0], axis_angle[1], axis_angle[2]);
							my_mat *= cur_mat;
						}
					}
				}
				else {
					my_mat *= tr->ToMatrix();
				}
			}
			transforms[i] = my_mat;
		}
	}

	std::map<JointNode*, user::Matrix4*> joint_mat_map;
	for (int i = 0 ; i < joints.size() ; i++) {
		joint_mat_map[joints[i].node_] = &transforms[i];
	}

	for (int i = 0 ; i < joints.size() ; i++) {
		updated_transforms[i] = joints[i].node_->worldTransformFromUpdatedLocal(joint_mat_map);
	}
}

int	 AnimCharacter::findJointIndex(const wchar_t* n)
{
	for (int i = 0 ; i < joints.size() ; i++) {
		if (!wcscmp(joints[i].node_->name(), n)) return i;
	}
	return -1;
}




void AnimCharacter::updateNormalInfluencesFromVertexInfluences()
{
	normal_influences.clear();
	std::map<int, int> i_map; // nindex, pindex map

	for (int i = 0 ; i < polygons_.size() ; i++) {
		CSubMesh* mesh = polygons_[i];

		int cur = 0;
		for (int j = 0 ; j < mesh->normal_indices_.size() ; j++) {
			i_map[mesh->normal_indices_[j]] = mesh->point_indices_[j];
		}
	}
	normal_influences.resize(normals_.size());
	for (int i = 0; i < normal_influences.size() ; i++) {
		std::map<int, int>::iterator found = i_map.find(i);
		if (found != i_map.end()) {
			for (int j = 0 ; j < influences[found->second].GetPairCount() ; j++) {
				JointWeightPair* p = influences[found->second].GetPair(j);
				normal_influences[i].AddPair(p->jointIndex, p->weight);
			}
		}
	}
}

void AnimCharacter::deleteAnimationClip()
{
	for (int i = 0 ; i < anim_clips_.size(); i++)
		delete anim_clips_[i];

	anim_clips_.clear();
}

unsigned int AnimCharacter::getVBO()
{
	std::map<AnimCharacter*, unsigned int>::iterator f = kCharacterVBOMap.find(this);
	if (f != kCharacterVBOMap.end()) {
		return f->second;
	}
	std::vector<AnimVertexBufferData> data;
	std::vector<GMaterial*> mat;
	std::vector<ivec2> range;
	getAnimVertexBufferData(0, data, mat, range);
	uint vbo;
	glGenBuffersARB(1, &vbo);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(AnimVertexBufferData) * data.size(), 0, GL_STREAM_DRAW_ARB);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, sizeof(AnimVertexBufferData) * data.size(), &data[0]);
	kCharacterVBOMap[this] = vbo;

	return vbo;
}
void AnimCharacter::getAnimVertexBufferData(real t, std::vector<AnimVertexBufferData>& data, std::vector<GMaterial*>& mats, std::vector<ivec2>& range)
{
	data.clear();
	mats.clear();
	range.clear();
	if (!joints.size()) return;

	std::vector<user::Matrix4> skin_matrices(joints.size());
	std::vector<user::Matrix4> wtrans;

	computeTransformsAt(t, wtrans);
	for (int i = 0; i < joints.size(); i++) {
		skin_matrices[i] = wtrans[i] * joints[i].bindPoseInverse;
	}


	std::vector<vec3>		tpoints_(points_.size());
	std::vector<vec3>		tnormals_(normals_.size());

	for (int i = 0; i < influences.size(); i++) {
		SkinControllerVertex& skinc = influences[i];
		vec3 v(0);
		user::Vector4 vv(points_[i][0], points_[i][1], points_[i][2], 1);
		for (int j = 0; j < skinc.pairs.size(); j++) {
			user::Vector4 vvv = skin_matrices[skinc.pairs[j].jointIndex].transform(vv);
			vec3 t(vvv[0] / vvv[3], vvv[1] / vvv[3], vvv[2] / vvv[3]);
			v += t*skinc.pairs[j].weight;
		}
		if (!skinc.pairs.size()) { tpoints_[i] = vec3(0, 0, 1000); }
		else tpoints_[i] = v;
	}
	for (int i = 0; i < normal_influences.size(); i++) {
		SkinControllerVertex& skinc = normal_influences[i];
		vec3 v(0);
		user::Vector3 vv(normals_[i][0], normals_[i][1], normals_[i][2]);
		for (int j = 0; j < skinc.pairs.size(); j++) {
			user::Vector3 vvv = skin_matrices[skinc.pairs[j].jointIndex].vtransform(vv);
			vec3 t(vvv[0], vvv[1], vvv[2]);
			v += t*skinc.pairs[j].weight;
		}

		tnormals_[i] = unit(v);
	}

	for (int i = 0; i < polygons_.size(); i++) {
		CSubMesh* mesh = polygons_[i];
		GMaterial* mat = mesh->material_;
		mats.push_back(mat);

		int save = data.size();
		int cur = 0;
		for (int j = 0; j < mesh->faces_.size(); j++) {
			int side = mesh->faces_[j];
			vec3 p0 = tpoints_[mesh->point_indices_[cur + 0]];
			vec3 n0 = tnormals_[mesh->normal_indices_[cur + 0]];
			vec2 t0(0);
			if (mat->HasDiffuseTexture()) {
				t0 = textures_[mesh->texture_indices_[cur + 0]];
			}
			
			for (unsigned int k = 1; k < side - 1; k++) {
				data.push_back(AnimVertexBufferData(p0, n0, t0));
				vec3 p1 = tpoints_[mesh->point_indices_[cur + k]];
				vec3 n1 = tnormals_[mesh->normal_indices_[cur + k]];
				vec2 t1(0);
				if (mat->HasDiffuseTexture()) {
					t1 = textures_[mesh->texture_indices_[cur + k]];
				}
				data.push_back(AnimVertexBufferData(p1, n1, t1));

				p1 = tpoints_[mesh->point_indices_[cur + k+1]];
				n1 = tnormals_[mesh->normal_indices_[cur + k+1]];
				t1(0);
				if (mat->HasDiffuseTexture()) {
					t1 = textures_[mesh->texture_indices_[cur + k+1]];
				}
				data.push_back(AnimVertexBufferData(p1, n1, t1));


			}
			cur += side;
			
		}
		range.push_back(ivec2(save, data.size()-1));
	}
}

void AnimCharacter::drawSkeleton(real t)
{
	if (!joints.size()) return;

	//if (!polygons_.size()) return;

	std::vector<user::Matrix4> skin_matrices(joints.size());
	std::vector<user::Matrix4> wtrans;

	std::vector<AnimVertexBufferData> VBO_contents;

	computeTransformsAt(t, wtrans);
	

	glDisable(GL_LIGHTING);
	

	gl_color(vec3(0,0,1));
	glDisable(GL_DEPTH_TEST);
	glLineWidth(2.0);
	glPointSize(4.0);
	for (int i = 0 ; i < joints.size() ; i++) {
		JointNode* p = joints[i].node_->parent();
		user::Vector3 pos = wtrans[i].translation();
		if (p) {
			gl_color(vec3(0,0,1)*(float)i/(float)joints.size());
			int k = findJointIndex(p->name());
			user::Vector3 pos1 = wtrans[k].translation();
			glBegin(GL_LINES);
			gl_vertex(vec3(pos[0], pos[1], pos[2]));
			gl_vertex(vec3(pos1[0], pos1[1], pos1[2]));
			glEnd();
			gl_color(vec3(1));
			glBegin(GL_POINTS);
			gl_vertex(vec3(pos[0], pos[1], pos[2]));
			gl_vertex(vec3(pos1[0], pos1[1], pos1[2]));
			glEnd();

		}
	}
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

	glLineWidth(1.0);
	glPointSize(1.0);
}

}