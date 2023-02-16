#ifndef __AnimCharacter_h__
#define __AnimCharacter_h__

#include "model/kernel.h"
#include "model/Matrix4.h"
#include "material/GMaterial.h"
#include "model/GraphicsScene.h"
#include "model/AnimTransform.h"
#include "model/AnimationClip.h"

namespace model_kernel {


struct JointWeightPair
{
public:
	/** Default constructor: sets both the joint index and the weight to zero. */
	JointWeightPair() { jointIndex = -1; weight = 0.0f; }

	/** Constructor: sets the joint index and the weight to the given values.
		@param _jointIndex The jointIndex.
		@param _weight Its weight. */
	JointWeightPair(int _jointIndex, float _weight) { jointIndex = _jointIndex; weight = _weight; }

	JointWeightPair(const JointWeightPair& cp): jointIndex(cp.jointIndex), weight(cp.weight) {}


	/** A joint index.
		Use this index within the skin's joint list.
		Look-out for the special joint index: -1. It indicates that
		the bind-shape position should be used. */
	int jointIndex;

	/** The weight of this influence on the vertex. */
	float weight;
};

/**
	A COLLADA skin controller vertex.
	This structure contains a list of joint-weight pairs that defines
	how to modify a given mesh vertex in order to skin it properly.
	@ingroup FCDGeometry
*/
struct SkinControllerVertex
{
public:

	std::vector<struct JointWeightPair, Allocator<struct JointWeightPair> > pairs;

public:

	SkinControllerVertex(): pairs() {}
	/** Retrieve the number of joint-weight pairs defined for this vertex.
		@return The number of joint-weight pairs. */
	inline size_t GetPairCount() const { return pairs.size(); }


	/** Retrieves a joint-weight pair.
		@param index The index of the joint-weight pair.
		@return The joint-weight pair at the given index. */
	inline JointWeightPair* GetPair(size_t index) { if(index < pairs.size()) return &(pairs.at(index)); return NULL;  }
	inline const JointWeightPair* GetPair(size_t index) const { if(index < pairs.size()) return &(pairs.at(index));return NULL;  } /**< See above. */

	/** Adds a new joint-weight pair to this vertex.
		No verification will be made to ensure that the sum of the weights equal 1.0.
		@param jointIndex The index of the joint within the skin controller's joint list.
		@param weight The influence weight for this joint, on this vertex. */
	void AddPair(int jointIndex, float weight) {
		pairs.push_back(JointWeightPair(jointIndex, weight));
	}

	//DECLARE_CLASS(SkinControllerVertex);
};


class JointNode;

/**
	A COLLADA skin controller joint.
	The controller does not reference the scene nodes directly: that's the instance's job.
	Instead, the skin controllers keeps track of the sub-ids of the scene nodes and their
	bind poses.
	@ingroup FCDGeometry
*/
struct SkinControllerJoint
{
public:
	wchar_t id[100];
	JointNode*    node_;
	user::Matrix4 bindPoseInverse;

public:
	//DECLARE_CLASS(SkinControllerJoint);

	SkinControllerJoint()
		: bindPoseInverse(), node_(0)
	{
	}
	SkinControllerJoint(const SkinControllerJoint& cp)
		: bindPoseInverse(cp.bindPoseInverse), node_(cp.node_)
	{
		SetId(cp.id);
	}
	SkinControllerJoint(const WString& id_, const user::Matrix4& bposeinv)
		: bindPoseInverse(bposeinv), node_(0)
	{
		SetId(id_);
	}

	/** Retrieves the identifier of the scene node(s) representing this joint.
		@return The identifier of the joint. */
	inline const WString GetId() const { return WString(id); }

	/** Sets the identifier of the scene node(s) representing this joint.
		@param id The identifier of the joint. */
	void SetId(const WString& id_) { for (int i = 0 ; i < id_.size()+1 ; i++) id[i]=id_[i]; }
	void SetId(const wchar_t* id_) { int size = wcslen(id_)+1; for (int i = 0 ; i < size ; i++) id[i]=id_[i]; }
	
	/** Retrieves the inverse bind-pose matrix of the joint.
		@return The inverse bind-pose matrix. */
	inline const user::Matrix4& GetBindPoseInverse() const { return bindPoseInverse; }

	/** Sets the inverse bind-pose matrix of the joint.
		@param inverseBindPose The inverse bind-pose matrix. */
	inline void SetBindPoseInverse(const user::Matrix4& inverseBindPose) { bindPoseInverse = inverseBindPose; }
};

class AnimCharacter;

class CSubMesh {
	
public:
	
	CSubMesh(AnimCharacter* mother = 0): connected_geometry_(mother), point_indices_(), normal_indices_(){}

	IndexArray& GetPointIndices() { return point_indices_; }
	IndexArray& GetNormalIndices() { return normal_indices_; }
	IndexArray& GetTextureIndices() { return texture_indices_; }

	void setPointIndices(const std::vector<unsigned int>& points);

	void setNormalIndices(const std::vector<unsigned int>& points);


	void setTextureCoordIndices(const std::vector<unsigned int>& points);

	void setFaceSides(const std::vector<unsigned int>& sides);

	DECLARE_CLASS(CSubMesh);
public:

	int						poly_type_;

	AnimCharacter*			connected_geometry_;
	IndexArray				point_indices_;
	IndexArray				normal_indices_;
	IndexArray				texture_indices_;

	IndexArray				faces_;

	GMaterial*				material_;
};


class JointNode {
	
public:
	
	typedef std::vector<JointNode*, Allocator<class JointNode*> > Children;
	typedef std::vector<Transform*, Allocator<class Transform*> > Transforms;
public:

	JointNode(const wchar_t* name);

	virtual ~JointNode() { 
		for (int i = 0 ; i < transforms_.size() ; i++)
			delete transforms_[i];

		clear(); 
	}



	const wchar_t* name() const;

	void name(const wchar_t* n);

	void addChild(JointNode* a);

	JointNode* createChild(const std::vector<Transform*>& m, const wchar_t* n);

	JointNode* child(int i) { return children_[i]; }

	int childCount() const { return children_.size(); }

	void removeChild(int i);

	void parent(JointNode* p) { parent_ = p; }

	JointNode* parent() { return parent_; }

	JointNode* search(const wchar_t* name);

	JointNode* root();

	void clear() ;

	user::Matrix4 worldTransform();

	user::Matrix4 worldTransformFromUpdatedLocal(std::map<JointNode*, user::Matrix4*>& joint_mat_map);

	void setTransforms(const std::vector<Transform*>& transforms);

	user::Matrix4 localTransform();

	int transformCount() const;

    Transform* transform(int idx);

public:

	DECLARE_CLASS(JointNode);

	JointNode();


	JointNode* parent_;
	
	Children children_;

	wchar_t name_[256];

	Transforms transforms_;
};

typedef std::vector<class CSubMesh*, Allocator<class CSubMesh*> > SubMeshArray;

struct AnimVertexBufferData {
	float v[8];
	AnimVertexBufferData(float pv[8]) { memcpy(v, pv, sizeof(float) * 8); }
	AnimVertexBufferData(AnimVertexBufferData& p) { memcpy(v, p.v, sizeof(float) * 8); }
	AnimVertexBufferData(vec3& p, vec3& n, vec2& t) { v[0] = p[0]; v[1] = p[1]; v[2] = p[2]; 
												  v[3] = n[0]; v[4] = n[1]; v[5] = n[2];
												  v[6] = t[0]; v[7] = t[1]; }
};

class AnimCharacter {

public:
	typedef std::vector<class AnimationClip*, Allocator<class AnimationClip*> > AnimationClipList;
	
	AnimCharacter();
	virtual ~AnimCharacter();

	void setPoints(const std::vector<vec3>& pnts);
	void setNormals(const std::vector<vec3>& pnts);
	void setTextureCoords(const std::vector<vec2>& pnts);

	void setSubMeshes(const std::vector<GPolygon*>& polygons, const std::vector<GMaterial*>& mats);

	void addJoint(const wchar_t* id, const user::Matrix4& bindPose);

	void addSkinControllerVertex(const std::vector<JointWeightPair>& cv);

	void createRootJoint(const std::vector<Transform*>& m, const wchar_t* n);

	void addChild(const wchar_t* parent, const std::vector<Transform*>& m, const wchar_t* n);

	void updateJointRelationship();

	void drawSkeleton(real t);


	void addAnimationClip(AnimationClip* clip) { anim_clips_.push_back(clip); }

	void computeTransformsAt(real t, std::vector<user::Matrix4>& transforms);

	int	 findJointIndex(const wchar_t* n);

	void weightCompute();

	void deleteAnimationClip();

	bool hasBody() { return joints.size() != 0; }

	//void mouse_down(Camera* cam, double t, int x, int y);

	//
	// infer the normal influences from vertex influences. This should be called after influences are
	//	constructed.
	//
	void updateNormalInfluencesFromVertexInfluences();

	void getAnimVertexBufferData(real t, std::vector<AnimVertexBufferData>& data, std::vector<GMaterial*>& mats, std::vector<ivec2>& range);

	unsigned int getVBO();

	float getEndTime() { return anim_clips_[0]->GetEnd(); }
public:

	DECLARE_CLASS(AnimCharacter);

public:

	JointNode*				root_joint_;
	AnimationClipList		anim_clips_;

	PointArray				points_;
	NormalArray				normals_;
	TextureCoordinateArray	textures_;

	SubMeshArray			polygons_;

	std::vector<struct SkinControllerJoint, Allocator<struct SkinControllerJoint> > joints;
	std::vector<struct SkinControllerVertex, Allocator<struct SkinControllerVertex> > influences;
	std::vector<struct SkinControllerVertex, Allocator<struct SkinControllerVertex> > normal_influences;
};

}

#endif