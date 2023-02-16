#include <model/colladaload.h>
#define NO_LIBXML

#include <FUtils/FUtils.h>

#include <FCDocument/FCDocument.h>

#include <FCDocument\FCDLibrary.h>
#include <FCDocument\FCDGeometryMesh.h>
#include <FCDocument\FCDGeometry.h>
#include <FCDocument\FCDAnimation.h>
#include <FCDocument\FCDAnimationChannel.h>
#include <FCDocument\FCDAnimationCurve.h>
#include <FCDocument\FCDAnimationClip.h>
#include <FCDocument\FCDAnimationKey.h>
#include <FCDocument\FCDGeometryPolygons.h>
#include <FCDocument\FCDGeometrySource.h>
#include <FCDocument\FCDSceneNode.h>
#include <FCDocument\FCDLight.h>
#include <FCDocument\FCDMaterial.h>
#include <FCDocument\FCDEffect.h>
#include <FCDocument\FCDEffectProfile.h>
#include <FCDocument\FCDEffectParameter.h>
#include <FCDocument\FCDEffectStandard.h>
#include <FCDocument\FCDEntityReference.h>
#include <FCDocument\FCDGeometryPolygonsInput.h>
#include <FCDocument\FCDExtra.h>
#include <FCDocument\FCDExtra.h>
#include <FCDocument\FCDTransform.h>
#include <FCDocument\FCDGeometryInstance.h>
#include <FCDocument\FCDMaterialInstance.h>
#include <FCDocument\FCDController.h>
#include <FUtils\FUObject.h>
#include <FCollada.h>


#include <IL/il.h>
#include <IL/ilut.h>
#include <string>

#include <graphics/sys.h>
#include <FCDocument\FCDSceneNodeIterator.h>
#include <FCDocument\FCDImage.h>
#include <FCDocument\FCDMaterial.h>
#include <FCDocument\FCDSkinController.h>
#include <material/MaterialDatabase.h>
#include <material/TextureDatabase.h>
#include <model/MemManager.h>
#include <model/sync_gpu_memory.h>
#include <model/rel_mem_manager.h>
//#include <kernel/KoonKernelApi.h>
//#include <knobs/ImportedModel_Knob.h>
#include <model/Matrix4.h>
#include <model/Vector4.h>
#include <model/Vector3.h>
#include <model/AnimationChannel.h>
#include <model/AnimationCurve.h>
#include <model/AnimationKey.h>
namespace model_kernel {
	FCDVisualSceneNodeLibrary* scene_lib_;
	FCDControllerLibrary* controller_lib_;
	FCDImageLibrary* image_lib_;
	FCDMaterialLibrary* material_lib_;
	FCDEffectLibrary* effect_lib_;
	FCDAnimationLibrary* anim_lib_;
	FCDAnimationClipLibrary* anim_clip_lib_;
	FCDGeometryLibrary* geometry_lib_;
bool kColladaInitialized = false;


static inline user::Vector3 to_Vector3(const vec3& v)
{
	return user::Vector3(v[0], v[1], v[2]);
}

static user::Matrix4 convert2Matrix4(const frame& g)
{
	user::Matrix4 ret;
	ret.makeIdentity();
	ret.x_axis() = to_Vector3(g.basis[0]);
	ret.y_axis() = to_Vector3(g.basis[1]);
	ret.z_axis() = to_Vector3(g.basis[2]);
	ret.translation() = to_Vector3(g.get_origin());
	return ret;
}

static user::Matrix4 convert2Matrix4(const FMMatrix44& g)
{
	user::Matrix4 ret;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			ret[i][j] = g[i][j];
	return ret;
}

GMaterial* ColladaLoad::SearchMaterial(FCDSceneNode* vs, const wchar_t* semantic)
{
	GMaterial* material_found = 0;
	for (size_t k = 0; k < vs->GetInstanceCount(); k++) {
		FCDEntityInstance* ei = vs->GetInstance(k);


		FCDGeometryInstance* gi = (FCDGeometryInstance*)ei;


		FCDMaterialInstance* materialinstance = 0;
			

		for (size_t i =0; i<(int) gi->GetMaterialInstanceCount(); i++) {
			// look for this material in my material lib, so I store a pointer
			materialinstance=gi->GetMaterialInstance(i);

			// strings conversions
			wchar_t m[512];
			swprintf(m, L"%s", materialinstance->GetSemantic().c_str() );

			if (!wcscmp(m, semantic)) {
				
				fstring ff = FUStringConversion::ToFString(materialinstance->GetMaterial()->GetDaeId());

				material_found = material_db_->FindByMaterialName(WString(ff.c_str()));
				return material_found;
			}
		}
	}
		

	int cnt = vs->GetChildrenCount();
	for (size_t l = 0 ; l < cnt; l++) {
		FCDSceneNode* child_node = vs->GetChild(l);
		material_found = SearchMaterial(child_node, semantic);
		if (material_found) return material_found;
	}

	return material_found;
}

GMaterial* ColladaLoad::SearchMaterial(const wchar_t* semantic)
{
	FCDVisualSceneNodeLibrary* v = scene_lib_;
	GMaterial* material_found = 0;
	for (size_t i = 0 ; i < v->GetEntityCount() ; i++) {
		FCDSceneNode* vs = v->GetEntity(i);

		
		for (size_t k = 0; k < vs->GetInstanceCount(); k++) {
			FCDEntityInstance* ei = vs->GetInstance(k);


			FCDGeometryInstance* gi = (FCDGeometryInstance*)ei;


			FCDMaterialInstance* materialinstance = 0;
			

			for (size_t i =0; i<(int) gi->GetMaterialInstanceCount(); i++) {
				// look for this material in my material lib, so I store a pointer
				materialinstance=gi->GetMaterialInstance(i);

				// strings conversions
				wchar_t m[512];
				swprintf(m, L"%s", materialinstance->GetSemantic().c_str() );

				if (!wcscmp(m, semantic)) {
				
					fstring ff = FUStringConversion::ToFString(materialinstance->GetMaterial()->GetDaeId());

					material_found = material_db_->FindByMaterialName(WString(ff.c_str()));
					return material_found;
				}
			}
		}
		

		int cnt = vs->GetChildrenCount();
		for (size_t l = 0 ; l < cnt; l++) {
			FCDSceneNode* child_node = vs->GetChild(l);
			material_found = SearchMaterial(child_node, semantic);
			if (material_found) return material_found;
		}
	}
	return material_found;
}

bool		  ColladaLoad::HasParentAnyJoint(FCDSceneNode* node)
{
	for (int i = 0; i < node->GetParentCount(); i++) {
		FCDSceneNode* pa = node->GetParent(i);
		if (pa->GetJointFlag()) return true;
	}
	return false;
}

FCDSceneNode* ColladaLoad::SearchJointNodeWithSubId(FCDSceneNode* vs, const wchar_t* name)
{
	FCDSceneNode* found = 0;
	fstring ff = FUStringConversion::ToFString(vs->GetSubId().size() ? vs->GetSubId() : vs->GetDaeId());
	if (vs->GetJointFlag() && !wcscmp(ff.c_str(), name)) return vs;

	int cnt = vs->GetChildrenCount();
	for (size_t l = 0; l < cnt; l++) {
		FCDSceneNode* child_node = vs->GetChild(l);
		found = SearchJointNodeWithSubId(child_node, name);
		if (found) return found;
	}
	return found;
}

FCDSceneNode* ColladaLoad::SearchJointNodeWithId(FCDSceneNode* vs, const wchar_t* name)
{
	FCDSceneNode* found = 0;
	fstring ff = FUStringConversion::ToFString(vs->GetDaeId());
	if (vs->GetJointFlag() && !wcscmp(ff.c_str(), name)) return vs;

	int cnt = vs->GetChildrenCount();
	for (size_t l = 0; l < cnt; l++) {
		FCDSceneNode* child_node = vs->GetChild(l);
		found = SearchJointNodeWithId(child_node, name);
		if (found) return found;
	}
	return found;
}
void ColladaLoad::CreateChildJointNodes(FCDSceneNode* vs)
{
	int cnt = vs->GetChildrenCount();

	for (size_t l = 0; l < cnt; l++) {
		FCDSceneNode* child_node = vs->GetChild(l);

		if (child_node->GetJointFlag()) {
			std::vector<Transform*> m;
			GetTransform(child_node, m);
			fstring ff = FUStringConversion::ToFString(vs->GetSubId().size() ? vs->GetSubId() : vs->GetDaeId());
			fstring ff_child = FUStringConversion::ToFString(child_node->GetSubId().size() ? child_node->GetSubId() : child_node->GetDaeId());
			character_->addChild(ff.c_str(), m, ff_child.c_str());
		}
		CreateChildJointNodes(child_node);
	}
}

FCDSceneNode* ColladaLoad::SearchJointNodeWithSubId(const wchar_t* name)
{
	FCDVisualSceneNodeLibrary* v = scene_lib_;
	FCDSceneNode* found = 0;
	for (size_t i = 0; i < v->GetEntityCount(); i++) {
		FCDSceneNode* vs = v->GetEntity(i);

		fstring ff = FUStringConversion::ToFString(vs->GetSubId().size() ? vs->GetSubId() : vs->GetDaeId());
		if (vs->GetJointFlag() && !wcscmp(ff.c_str(), name)) return vs;

		int cnt = vs->GetChildrenCount();
		for (size_t l = 0; l < cnt; l++) {
			FCDSceneNode* child_node = vs->GetChild(l);
			found = SearchJointNodeWithSubId(child_node, name);
			if (found) return found;
		}
	}
	return found;
}

FCDSceneNode* ColladaLoad::SearchJointNodeWithId(const wchar_t* name)
{
	FCDVisualSceneNodeLibrary* v = scene_lib_;
	FCDSceneNode* found = 0;
	for (size_t i = 0; i < v->GetEntityCount(); i++) {
		FCDSceneNode* vs = v->GetEntity(i);

		fstring ff = FUStringConversion::ToFString(vs->GetDaeId());
		if (vs->GetJointFlag() && !wcscmp(ff.c_str(), name)) return vs;

		int cnt = vs->GetChildrenCount();
		for (size_t l = 0; l < cnt; l++) {
			FCDSceneNode* child_node = vs->GetChild(l);
			found = SearchJointNodeWithId(child_node, name);
			if (found) return found;
		}
	}
	return found;
}


void ColladaLoad::ReconstructVisualScene(FCDSceneNode* node)
{

	FCDSceneNode* sn = node;

	FCDSceneNode* vs = sn;
	SceneNode* vs_s = shape_map_[vs];

	frame f;
	vec3 scale;
	GetTransform(vs, f, scale);

	vs_s->SetTransform(f);
	vs_s->SetScale(scale);

	for (size_t k = 0; k < sn->GetInstanceCount(); k++) {
		FCDEntityInstance* ei = sn->GetInstance(k);


		FCDGeometryInstance* gi = (FCDGeometryInstance*)ei;


		FCDMaterialInstance* materialinstance = 0;
		GMaterial* material_found = 0;
		GeometryInstance* instance = vs_s->AddGeometryInstance(geom_map_[(FCDGeometry*) gi->GetEntity()]);

		if (!instance) continue;

		for (int j = 0 ; j < instance->GetGeometry()->GetPolygonCount() ; j++) {
			//polygon_count_++;
			GPolygon* polygon = instance->GetGeometry()->GetPolygon(j);
			polygon_count_ += polygon->GetPointIndices().size()/3;
			vertex_count_ += polygon->GetPointIndices().size();
			material_found = 0;

			for (size_t i =0; i<(int) gi->GetMaterialInstanceCount(); i++) {
				// look for this material in my material lib, so I store a pointer
				materialinstance=gi->GetMaterialInstance(i);

				// strings conversions
				wchar_t m[512];
				swprintf(m, L"%s", materialinstance->GetSemantic().c_str() );

				//std::string str(FUStringConversion::ToString(m)); 

				if (!wcscmp(m, polygon->material_semantic_.c_str())) {
					//std::string material_id(FUStringConversion::ToString(materialinstance->GetMaterial()->GetDaeId())); 
			
					fstring ff = FUStringConversion::ToFString(materialinstance->GetMaterial()->GetDaeId());
					material_found = material_db_->FindByMaterialName(WString(ff.c_str()));
				}
			}
			instance->AddPolygonInstance(polygon, material_found);
		}

		// look for this material_semantic in geometry_instance
	}

	int cnt = sn->GetChildrenCount();
	for (size_t l = 0 ; l < cnt; l++) {
		FCDSceneNode* child_node = sn->GetChild(l);
		vs_s->AddChild(shape_map_[child_node]);
		ReconstructVisualScene(child_node);
	}
}

void ColladaLoad::GetTransform(FCDSceneNode* vs, std::vector<Transform*>& m)
{
	for (int j = 0; j < vs->GetTransformCount(); j++) {
		FCDTransform* tr = vs->GetTransform(j);
		if (tr->GetType() == FCDTransform::TRANSLATION) {
			FCDTTranslation* trans = (FCDTTranslation*)tr;
			Translation* mtr = new Translation();
			fstring ff = FUStringConversion::ToFString(fm::string(tr->GetSubId()));
			mtr->SetSubId(WString(ff.c_str()));
			m.push_back(mtr);
			mtr->SetTranslation(trans->GetTranslation()->x, trans->GetTranslation()->y, trans->GetTranslation()->z);
		}
		else if (tr->GetType() == FCDTransform::ROTATION) {
			FCDTRotation* rot = (FCDTRotation*)tr;
			FMVector3 axis = rot->GetAxis();
			float ang = rot->GetAngle();
			Rotation* mtr = new Rotation();
			fstring ff = FUStringConversion::ToFString(fm::string(tr->GetSubId()));
			mtr->SetSubId(WString(ff.c_str()));
			m.push_back(mtr);
			mtr->SetAngleAxis(vec3(axis.x, axis.y, axis.z), ang);
		}
		else if (tr->GetType() == FCDTransform::SCALE) {
			FCDTScale* scale_ = (FCDTScale*)tr;
			Scale* mtr = new Scale();
			fstring ff = FUStringConversion::ToFString(fm::string(tr->GetSubId()));
			mtr->SetSubId(WString(ff.c_str()));
			m.push_back(mtr);
			mtr->SetScale(scale_->GetScale()->x, scale_->GetScale()->y, scale_->GetScale()->z);
		}
		else if (tr->GetType() == FCDTransform::MATRIX) {
			FCDTMatrix* mat = (FCDTMatrix*)tr;

			FCDParameterAnimatableMatrix44 matt = mat->GetTransform();
			FMMatrix44& val = (FMMatrix44)matt;

			Matrix* mtr = new Matrix();
			fstring ff = FUStringConversion::ToFString(fm::string(tr->GetSubId()));
			mtr->SetSubId(WString(ff.c_str()));
			m.push_back(mtr);
			mtr->SetTransform(convert2Matrix4(val));
		}
	}
}
void ColladaLoad::GetTransform(FCDSceneNode* vs, frame& f, vec3& scale)
{
	f.reset();
	scale = vec3(1);
	for (int j = 0 ; j < vs->GetTransformCount() ; j++) {
		FCDTransform* tr = vs->GetTransform(j);
		if (tr->GetType() == FCDTransform::TRANSLATION) {
			FCDTTranslation* trans = (FCDTTranslation*) tr;
			
			f.eye_position += vec3(trans->GetTranslation()->x, trans->GetTranslation()->y, trans->GetTranslation()->z);
			f.update();
		}
		else if (tr->GetType() == FCDTransform::ROTATION) {
			FCDTRotation* rot = (FCDTRotation*) tr;
			FMVector3 axis = rot->GetAxis();
			float ang = rot->GetAngle();
			quater q = orient(radian(ang), vec3(axis.x, axis.y, axis.z));
			f.rotate_frame_locally(q/*radian(ang), vec3(axis.x, axis.y, axis.z)*/);
		}
		else if (tr->GetType() == FCDTransform::SCALE) {
			FCDTScale* scale_ = (FCDTScale*) tr;
			scale *= vec3(scale_->GetScale()->x, scale_->GetScale()->y, scale_->GetScale()->z);
		}
		else if (tr->GetType() == FCDTransform::MATRIX) {
			FCDTMatrix* mat = (FCDTMatrix*) tr;
		
			FCDParameterAnimatableMatrix44 matt = mat->GetTransform();
			FMMatrix44& val = (FMMatrix44) matt;
			val.m;
			scale[0] *= norm(vec3(val.m[0][0], val.m[0][1], val.m[0][2]));
			scale[1] *= norm(vec3(val.m[1][0], val.m[1][1], val.m[1][2]));
			scale[2] *= norm(vec3(val.m[2][0], val.m[2][1], val.m[2][2]));

			f.basis[0] = unit(vec3(val.m[0][0], val.m[0][1], val.m[0][2]));
			f.basis[1] = unit(vec3(val.m[1][0], val.m[1][1], val.m[1][2]));
			f.basis[2] = unit(vec3(val.m[2][0], val.m[2][1], val.m[2][2]));
			f.eye_position = vec3(val.m[3][0], val.m[3][1], val.m[3][2]);
			f.update();
		}
	}
}


std::vector<FCDSceneNode*> ColladaLoad::GetRootNodes()
{
	FCDVisualSceneNodeLibrary* v = scene_lib_;
	std::vector<FCDSceneNode*> roots;

	for (size_t i = 0 ; i < v->GetEntityCount() ; i++) {
		FCDSceneNode* vs = v->GetEntity(i);
		if (vs->GetParentCount() == 0) {
			roots.push_back(vs);
		}
	}
	return roots;
}




int*	addressOfElementBuffer(RelativeMemoryManager* rm, int index)
{
	return (int*)kSyncGpuMemory->addressOfEBO(rm->address(index));
}

inline vec3 xform(const user::Matrix4& mat, const vec3& v)
{
	user::Vector3 aa(v[0],v[1],v[2]);
	user::Vector4 vv = mat.transform(aa, 1.0);
	return vec3(vv[0]/vv[3], vv[1]/vv[3], vv[2]/vv[3]);
}
inline vec3 xform_normal(const user::Matrix4& mat, const vec3& v)
{
	user::Vector3 vv(v[0],v[1],v[2]);
	vv = mat.vtransform(vv);
	return (vec3(vv[0],vv[1],vv[2]));
}

void ColladaLoad::update_material_polygon(GMaterial* mat, GPolygon* poly, user::Matrix4& cur)
{
	RelativeMemoryManager* rm = model_->getRelativeMemoryManager(mat);
	Geometry* cp = poly->connected_geometry_;
	int k = 0;
	for (int i = 0; i < poly->faces_.size(); i++) {
		int pgon = poly->faces_[i];
		std::vector<vec3> n(pgon);
		std::vector<vec3> v(pgon);
		std::vector<vec2> t(pgon);
		for (int pi = 0; pi < pgon; pi++) {
			if (cp->normals_.size()) {
				n[pi] = unit(cp->normals_[poly->GetNormalIndices()[k + pi]]);
				n[pi] = xform_normal(cur, n[pi]);
			}
			else { n[pi] = 0; }

			if (cp->textures_.size() && k + pi < poly->GetTextureIndices().size()) {
				t[pi] = (cp->textures_[poly->GetTextureIndices()[k + pi]]);
			}
			else { t[pi] = 0; }

			if (poly->GetPointIndices()[k + pi] < cp->points_.size()) {
				v[pi] = (cp->points_[poly->GetPointIndices()[k + pi]]);
				v[pi] = xform(cur, v[pi]);
			}
			else { v[pi] = 0; }
		}

		k += pgon;
		vec3 p1 = v[0];
		vec3 n1 = n[0];
		vec2 t1 = t[0];
		for (unsigned int ii = 1; ii < v.size() - 1; ii++) {
			vec3 p2 = v[ii];
			vec3 p3 = v[ii + 1];
			vec3 n2 = n[ii];
			vec3 n3 = n[ii + 1];
			vec2 t2 = t[ii];
			vec2 t3 = t[ii + 1];
			vec3 fn = unit(cross(p2 - p1, p3 - p1));
			if (apx_equal(n1, vec3(0))) {
				model_->addTriangle2Gpu(rm, p1, p2, p3, fn, fn, fn, 0, t1, t2, t3, 0, 0, 0);
			}
			else if (inner(fn, n1) > 0.00000001)
				model_->addTriangle2Gpu(rm, p1, p2, p3, n1, n2, n3, 0, t1, t2, t3, 0, 0, 0);
			else
				model_->addTriangle2Gpu(rm, p1, p3, p2, n1, n3, n2, 0, t1, t3, t2, 0, 0, 0);
		}
	}
}



void ColladaLoad::update_material_polygon(ImportedModel* model, GMaterial* mat, GPolygon* poly, user::Matrix4& cur)
{
	RelativeMemoryManager* rm = model->getRelativeMemoryManager(mat);
	Geometry* cp = poly->connected_geometry_;
	int k = 0;
	for (int i = 0; i < poly->faces_.size(); i++) {
		int pgon = poly->faces_[i];
		std::vector<vec3> n(pgon);
		std::vector<vec3> v(pgon);
		std::vector<vec2> t(pgon);
		for (int pi = 0; pi < pgon; pi++) {
			if (cp->normals_.size()) {
				n[pi] = unit(cp->normals_[poly->GetNormalIndices()[k + pi]]);
			}
			else { n[pi] = 0; }

			if (cp->textures_.size() && k + pi < poly->GetTextureIndices().size()) {
				t[pi] = (cp->textures_[poly->GetTextureIndices()[k + pi]]);
			}
			else { t[pi] = 0; }

			if (poly->GetPointIndices()[k + pi] < cp->points_.size()) {
				v[pi] = (cp->points_[poly->GetPointIndices()[k + pi]]);
			}
			else { v[pi] = 0; }
		}

		k += pgon;
		vec3 p1 = v[0];
		vec3 n1 = n[0];
		vec2 t1 = t[0];
		for (unsigned int ii = 1; ii < v.size() - 1; ii++) {
			vec3 p2 = v[ii];
			vec3 p3 = v[ii + 1];
			vec3 n2 = n[ii];
			vec3 n3 = n[ii + 1];
			vec2 t2 = t[ii];
			vec2 t3 = t[ii + 1];

			model->addTriangle2Gpu(rm, p1, p2, p3, n1, n2, n3, 0, t1, t2, t3, 0, 0, 0);

		}
	}
}
void ColladaLoad::update_material_polygon(Geometry* new_geom, GMaterial* mat, GPolygon* poly, user::Matrix4& cur)
{	
	GPolygon* new_poly = new_geom->CreateNewPolygon();
	std::map<int, int> pmap;
	std::map<int, int> nmap;
	std::map<int, int> tmap;

	for (int k = 0 ; k < poly->GetPointIndices().size(); k++) {

		Geometry* cp = poly->connected_geometry_;

		vec3 n(0), v(0);
		vec2 t(0);


		if (cp->normals_.size()) {
			n = unit(cp->normals_[poly->GetNormalIndices()[k]]);
			n = xform_normal(cur, n);
		} else { n = 0; }
		if (cp->textures_.size() && k < poly->GetTextureIndices().size()) {
			t = (cp->textures_[poly->GetTextureIndices()[k]]);
		} else { t = 0; }

		if (poly->GetPointIndices()[k] < cp->points_.size()) {
			v = (cp->points_[poly->GetPointIndices()[k]]);
			v = xform(cur, v);
		} else { v = 0; }

		std::map<int, int>::iterator found = pmap.find(poly->GetPointIndices()[k]);
		if (found != pmap.end()) {
			new_poly->GetPointIndices().push_back(found->second);
		} else {
			pmap[poly->GetPointIndices()[k]] = new_geom->points_.size();
			new_poly->GetPointIndices().push_back(new_geom->points_.size());
			new_geom->points_.push_back(v);
		}
		found = nmap.find(poly->GetNormalIndices()[k]);
		if (found != nmap.end()) {
			new_poly->GetNormalIndices().push_back(found->second);
		} else {
			nmap[poly->GetNormalIndices()[k]] = new_geom->normals_.size();
			new_poly->GetNormalIndices().push_back(new_geom->normals_.size());
			new_geom->normals_.push_back(n);
		}
		if (poly->GetTextureIndices().size()) {
			found = tmap.find(poly->GetTextureIndices()[k]);
			if (found != tmap.end()) {
				new_poly->GetTextureIndices().push_back(found->second);
			} else {
				tmap[poly->GetTextureIndices()[k]] = new_geom->textures_.size();
				new_poly->GetTextureIndices().push_back(new_geom->textures_.size());
				new_geom->textures_.push_back(t);
			}
		}
	}
	new_poly->faces_ = poly->faces_;
	new_poly->material_semantic_ = poly->material_semantic_;
}

void ColladaLoad::ExportToKoonFlat(SceneNode* corresponding, user::Matrix4 parent)
{
	SceneNode* vs_s = corresponding;

	GFrame& fr = vs_s->GetTransform();

	user::Matrix4 mat = convert2Matrix4(fr);
	user::Matrix4 user_mat = mat;
		
	user_mat.x_axis() = user_mat.x_axis()*fr.scale[0];
	user_mat.y_axis() = user_mat.y_axis()*fr.scale[1];
	user_mat.z_axis() = user_mat.z_axis()*fr.scale[2];

	user_mat = parent * user_mat;

	int cnt = vs_s->GetGeometryInstanceCount();

	for (int j = 0 ; j < cnt ; j++) {
		GeometryInstance* instance = vs_s->GetGeometryInstance(j);
		for (int k = 0 ; k < instance->GetPolygonInstanceCount(); k++) 
		{
			update_material_polygon(instance->GetPolygonInstance(k)->GetMaterial(), instance->GetPolygonInstance(k)->GetPolygon(), user_mat);
		}
	}

	for (int k = 0 ; k < vs_s->GetChildrenCount() ; k++) {
		SceneNode* nvs = vs_s->GetChild(k);

		ExportToKoonFlat(nvs, user_mat);
	}
}
void ColladaLoad::ExportToKoonFlat()
{

	std::vector<SceneNode*> roots;
	scene_graph_->GetRootSceneNodes(roots);

	for (size_t i = 0 ; i < roots.size() ; i++) {
		SceneNode* vs = roots[i];

		GFrame& tr = vs->GetTransform();

		user::Matrix4 mat = convert2Matrix4(tr);
		user::Matrix4 user_mat = mat;
		user_mat.x_axis() = user_mat.x_axis()*tr.scale[0];
		user_mat.y_axis() = user_mat.y_axis()*tr.scale[1];
		user_mat.z_axis() = user_mat.z_axis()*tr.scale[2];

		int cnt = vs->GetGeometryInstanceCount();

		for (int j = 0 ; j < cnt ; j++) {
			GeometryInstance* instance = vs->GetGeometryInstance(j);
			for (int k = 0 ; k < instance->GetPolygonInstanceCount(); k++) 
			{
				update_material_polygon(instance->GetPolygonInstance(k)->GetMaterial(), instance->GetPolygonInstance(k)->GetPolygon(), user_mat);
			}
		}

		for (int k = 0 ; k < vs->GetChildrenCount() ; k++) {
			SceneNode* nvs = vs->GetChild(k);

			ExportToKoonFlat(nvs, user_mat);
		}
	}

	//KoonKernelApi::Get()->UpdateNodeOpCorrespondence();
}


void ColladaLoad::ReconstructVisualScene()
{
	FCDVisualSceneNodeLibrary* v = scene_lib_;

	std::vector<FCDSceneNode*> nodes = GetRootNodes();

	for (size_t i = 0 ; i < nodes.size() ; i++) {

		FCDSceneNode* vs = nodes[i];
		SceneNode* vs_s = shape_map_[vs];

		scene_graph_->AddRootSceneNode(vs_s);

		frame f;
		vec3 scale;
		GetTransform(vs, f, scale);

		vs_s->SetTransform(f);
		vs_s->SetScale(scale);

		for (size_t k = 0; k < vs->GetInstanceCount(); k++) {
			FCDEntityInstance* ei = vs->GetInstance(k);


			FCDGeometryInstance* gi = (FCDGeometryInstance*)ei;


			FCDMaterialInstance* materialinstance = 0;
			GMaterial* material_found = 0;
			GeometryInstance* instance = vs_s->AddGeometryInstance(geom_map_[(FCDGeometry*) gi->GetEntity()]);


			for (int j = 0 ; j < instance->GetGeometry()->GetPolygonCount() ; j++) {
				
				//polygon_count_++;

				GPolygon* polygon = instance->GetGeometry()->GetPolygon(j);
				polygon_count_ += polygon->GetPointIndices().size()/3;
				vertex_count_ += polygon->GetPointIndices().size();
				material_found = 0;

				for (size_t i =0; i<(int) gi->GetMaterialInstanceCount(); i++) {
					// look for this material in my material lib, so I store a pointer
					materialinstance=gi->GetMaterialInstance(i);

					// strings conversions
					wchar_t m[512];
					swprintf(m, L"%s", materialinstance->GetSemantic().c_str() );

					if (!wcscmp(m, polygon->material_semantic_.c_str())) {
				
						fstring ff = FUStringConversion::ToFString(materialinstance->GetMaterial()->GetDaeId());

						material_found = material_db_->FindByMaterialName(WString(ff.c_str()));
					}
				}
				instance->AddPolygonInstance(polygon, material_found);
			}
			
		}

		int cnt = vs->GetChildrenCount();
		for (size_t l = 0 ; l < cnt; l++) {
			FCDSceneNode* child_node = vs->GetChild(l);
			vs_s->AddChild(shape_map_[child_node]);
			ReconstructVisualScene(child_node);
		}
	}

	
}



void ColladaLoad::ReconnectSceneNodes()
{
	FCDVisualSceneNodeLibrary* v = scene_lib_;
	shape_map_.clear();

	for (size_t i = 0 ; i < v->GetEntityCount() ; i++) {
		FCDSceneNode* vs = v->GetEntity(i);

		SceneNode* shape = scene_graph_->CreateSceneNode();
		shape_map_[vs] = shape;
		
		FCDSceneNodeIterator iterator(vs, FCDSceneNodeIterator::DEPTH_FIRST_PREORDER);

		for (;;) {
			FCDSceneNode* nn = iterator.Next();

			if (!nn) break;

			SceneNode* shape1 = scene_graph_->CreateSceneNode();
			shape_map_[nn] = shape1;

			if (nn->forcedEnd) {
				FCDSceneNode* found = nn->GetDocument()->FindSceneNode(nn->link);
				if (found) {
					if (!nn->AddChildNode(found)) {
						LOG("cannot connect\n");
					}
				}
			}
			//LOG("node name %s, instance count %d\n", nn->GetName().c_str(), nn->GetInstanceCount());
		}
	}
}

static void CopyKey(AnimationKey* a, FCDAnimationKey* b)
{
	switch (a->interpolation)
	{
	case Interpolation::STEP:
	case Interpolation::LINEAR: {
		AnimationKey* aa = a;
		FCDAnimationKey* bb = b;
		aa->input = bb->input;
		aa->interpolation = bb->interpolation;
		aa->output = bb->output;
	} break;
	case Interpolation::BEZIER: {
		AnimationKeyBezier* aa = (AnimationKeyBezier*)a;
		FCDAnimationKeyBezier* bb = (FCDAnimationKeyBezier*)b;
		aa->input = bb->input;
		aa->interpolation = bb->interpolation;
		aa->output = bb->output;
		aa->inTangent = graphics::vec2(bb->inTangent[0], bb->inTangent[1]);
		aa->outTangent = graphics::vec2(bb->outTangent[0], bb->outTangent[1]);
	} break;
	case Interpolation::TCB: {
		AnimationKeyTCB* aa = (AnimationKeyTCB*)a;
		FCDAnimationKeyTCB* bb = (FCDAnimationKeyTCB*)b;
		aa->input = bb->input;
		aa->interpolation = bb->interpolation;
		aa->output = bb->output;
		aa->bias = bb->bias;
		aa->continuity = bb->continuity;
		aa->easeIn = bb->easeIn;
		aa->easeOut = bb->easeOut;
		aa->tension = bb->tension;
	} break;
	}
}

void ColladaLoad::CreateChildAnimation(FCDAnimation* vs, Animation* node, AnimationClip* clip)
{
	FCDExtra* extra = vs->GetExtra();
	std::string target1, target2;
	Animation* anim = node;
	fstring ff = FUStringConversion::ToFString(vs->GetDaeId());
	anim->SetId(WString(ff.c_str()));


	for (int k = 0; k < vs->GetChannelCount(); k++) {
		FCDAnimationChannel* chan = vs->GetChannel(k);
		AnimationChannel* my_chan = anim->AddChannel();
		clip->AddClipChannel(my_chan);
		LOG("target %s\n", chan->getTarget().c_str());

		// now set target in the channel
		fm::string str(chan->getTarget().c_str());
		size_t splitIndex = str.find_first_of("/");
		fm::string main, sub;
		if (splitIndex != std::string::npos) {
			main = str.substr(0, splitIndex);
			sub = str.substr(splitIndex + 1);
		}
		else {
			main = str;
		}
		fstring ff = FUStringConversion::ToFString(main);
		FCDSceneNode* node = SearchJointNodeWithId(ff.c_str());
		if (node && node->GetSubId().size()) main = node->GetSubId();
		str = main + "/" + sub;
		ff = FUStringConversion::ToFString(str);
		my_chan->SetTarget(WString(ff.c_str()));
		fm::string new_str = chan->getTargetQualifier();
		ff = FUStringConversion::ToFString(new_str);
		my_chan->SetTargetQualifier(WString(ff.c_str()));

		for (int nn = 0; nn < chan->GetCurveCount(); nn++) {
			FCDAnimationCurve* crv = chan->GetCurve(nn);
			AnimationCurve* my_crv = my_chan->AddCurve();
			//my_crv->SetPreInfinity((Infinity::Infinity)crv->GetPreInfinity());
			//my_crv->SetPostInfinity((Infinity::Infinity)crv->GetPostInfinity());
			for (int m = 0; m < crv->GetKeyCount(); m++) {

				FCDAnimationKey* key = crv->GetKey(m);
				AnimationKey* my_key = my_crv->AddKey((Interpolation::Interpolation)key->interpolation);
				CopyKey(my_key, key);
			}
		}
	}

	for (int k = 0; k < vs->GetChildrenCount(); k++) {
		anim = node->AddChild();
		CreateChildAnimation(vs->GetChild(k), anim, clip);
	}
}
void ColladaLoad::BuildAnimations()
{
	AnimationClip* clip = new AnimationClip();
	character_->addAnimationClip(clip);
	clip->SetId(WString(L"default clip"));
	FCDAnimationLibrary* v = anim_lib_;
	for (size_t i = 0; i < v->GetEntityCount(); i++) {
		FCDAnimation* vs = v->GetEntity(i);
		FCDExtra* extra = vs->GetExtra();
		std::string target1, target2;
		Animation* anim = new Animation();
		fstring ff = FUStringConversion::ToFString(vs->GetDaeId());
		anim->SetId(WString(ff.c_str()));
		clip->AddInstanceAnimation(anim);

		for (int k = 0; k < vs->GetChannelCount(); k++) {
			FCDAnimationChannel* chan = vs->GetChannel(k);
			AnimationChannel* my_chan = anim->AddChannel();
			clip->AddClipChannel(my_chan);
			LOG("target %s\n", chan->getTarget().c_str());

			// now set target in the channel
			fm::string str(chan->getTarget().c_str());
			size_t splitIndex = str.find_first_of("/");
			fm::string main, sub;
			if (splitIndex != std::string::npos) {
				main = str.substr(0, splitIndex);
				sub = str.substr(splitIndex + 1);
			}
			else {
				main = str;
			}
			fstring ff = FUStringConversion::ToFString(main);
			FCDSceneNode* node = SearchJointNodeWithId(ff.c_str());
			if (node) main = node->GetSubId();
			str = main + "/" + sub;
			ff = FUStringConversion::ToFString(str);
			my_chan->SetTarget(WString(ff.c_str()));
			fm::string new_str = chan->getTargetQualifier();
			ff = FUStringConversion::ToFString(new_str);
			my_chan->SetTargetQualifier(WString(ff.c_str()));

			for (int nn = 0; nn < chan->GetCurveCount(); nn++) {
				FCDAnimationCurve* crv = chan->GetCurve(nn);
				AnimationCurve* my_crv = my_chan->AddCurve();
				//my_crv->SetPreInfinity((Infinity::Infinity)crv->GetPreInfinity());
				//my_crv->SetPostInfinity((Infinity::Infinity)crv->GetPostInfinity());
				for (int m = 0; m < crv->GetKeyCount(); m++) {

					FCDAnimationKey* key = crv->GetKey(m);
					AnimationKey* my_key = my_crv->AddKey((Interpolation::Interpolation)key->interpolation);
					CopyKey(my_key, key);
				}
			}
		}
		for (int k = 0; k < vs->GetChildrenCount(); k++) {
			Animation* anim_ = anim->AddChild();
			CreateChildAnimation(vs->GetChild(k), anim_, clip);
		}
	}

	FCDAnimationClipLibrary* vv = anim_clip_lib_;
	for (size_t i = 0; i < vv->GetEntityCount(); i++) {
		FCDAnimationClip* vs = vv->GetEntity(i);
		clip = new AnimationClip();
		character_->addAnimationClip(clip);
		fstring n = vs->GetName();
		clip->SetId(WString(n.c_str()));
		clip->SetStart(vs->GetStart());
		clip->SetEnd(vs->GetEnd());
		for (int k = 0; k < vs->GetAnimationCount(); k++) {
			fm::string n = vs->GetAnimationName(k);
			FCDAnimation* anim = vs->GetAnimation(k);
			fm::string daeid = anim->GetDaeId();
			fstring ff = FUStringConversion::ToFString(daeid);
			WString anim_name(ff.c_str());
			Animation* anim1 = character_->anim_clips_[0]->FindAnimationWithId(anim_name);
			clip->AddInstanceAnimation(anim1);
			for (int m = 0; m < anim1->GetChannelCount(); m++)
				clip->AddClipChannel(anim1->GetChannel(m));
		}
	}
}


void ColladaLoad::BuildJointScenes(FCDSceneNode* node)
{
	FCDSceneNode* vs = node;
	if (vs->GetJointFlag()) {
		fstring ff = FUStringConversion::ToFString(vs->GetSubId().size() ? vs->GetSubId() : vs->GetDaeId());
		user::Matrix4 umat = user::Matrix4::identity();
		character_->addJoint(ff.c_str(), umat);
	}
	int cnt = node->GetChildrenCount();
	for (size_t l = 0; l < cnt; l++) {
		FCDSceneNode* child_node = vs->GetChild(l);
		BuildJointScenes(child_node);
	}
}
void ColladaLoad::BuildJointScenes()
{
	FCDVisualSceneNodeLibrary* v = scene_lib_;

	std::vector<FCDSceneNode*> nodes = GetRootNodes();

	for (size_t i = 0; i < nodes.size(); i++) {

		FCDSceneNode* vs = nodes[i];
		if (vs->GetJointFlag()) {
			fstring ff = FUStringConversion::ToFString(vs->GetSubId().size() ? vs->GetSubId() : vs->GetDaeId());
			user::Matrix4 umat = user::Matrix4::identity();
			character_->addJoint(ff.c_str(), umat);
		}
		int cnt = vs->GetChildrenCount();
		for (size_t l = 0; l < cnt; l++) {
			FCDSceneNode* child_node = vs->GetChild(l);
			BuildJointScenes(child_node);
		}
	}

}

void ColladaLoad::BuildControllers()
{
	FCDControllerLibrary* v = controller_lib_;
	for (size_t i = 0; i < v->GetEntityCount(); i++) {
		FCDController* vs = v->GetEntity(i);
		FCDSkinController* skin = vs->GetSkinController();
		int k = skin->GetJointCount();
		int inf = skin->GetInfluenceCount();
		FCDEntity* target;
		target = skin->GetTarget();

		Geometry* geometry = geom_map_[(FCDGeometry*)target];
		std::vector<GMaterial*> mats;
		for (int k = 0; k < geometry->polygons_.size(); k++) {
			GMaterial *tmp = SearchMaterial(geometry->polygons_[k]->material_semantic_.c_str());
			mats.push_back(tmp);
		}
		character_->setPoints(geometry->points_);
		character_->setNormals(geometry->normals_);
		character_->setTextureCoords(geometry->textures_);
		character_->setSubMeshes(geometry->polygons_, mats);

		user::Matrix4 bindShapeTransform = convert2Matrix4(skin->GetBindShapeTransform());
		// append joint
		for (int k = 0; k < skin->GetJointCount(); k++) {
			FCDSkinControllerJoint* j = skin->GetJoint(k);
			fstring ff = FUStringConversion::ToFString(j->GetId());
			FMMatrix44 mat = j->GetBindPoseInverse();
			user::Matrix4 umat = convert2Matrix4(mat);
			umat = umat*bindShapeTransform;
			character_->addJoint(ff.c_str(), umat);
		}


		// append influence
		for (int k = 0; k < skin->GetInfluenceCount(); k++) {

			FCDSkinControllerVertex* v = skin->GetVertexInfluence(k);
			std::vector<JointWeightPair> vv;
			for (int m = 0; m < v->GetPairCount(); m++) {
				vv.push_back(JointWeightPair(v->GetPair(m)->jointIndex, v->GetPair(m)->weight));
			}
			character_->addSkinControllerVertex(vv);
		}

		character_->updateNormalInfluencesFromVertexInfluences();

		FCDSceneNode *root;
		// create joint structures
		for (int k = 0; k < skin->GetJointCount(); k++) {
			FCDSkinControllerJoint* j = skin->GetJoint(k);
			fstring ff = FUStringConversion::ToFString(j->GetId());
			FCDSceneNode* tmp = SearchJointNodeWithSubId(ff.c_str());
			if (tmp && !HasParentAnyJoint(tmp)) {
				root = tmp;
				break;
			}
		}

		if (!root) break;

		std::vector<Transform*> m;
		GetTransform(root, m);
		fstring ff = FUStringConversion::ToFString(root->GetSubId().size() ? root->GetSubId() : root->GetDaeId());
		character_->createRootJoint(m, ff.c_str());

		CreateChildJointNodes(root);
		character_->updateJointRelationship();
	}

	//
	// no skin controller exists! but we have the scene with JOINT node!
	//
	if (!character_->joints.size()) {
		BuildJointScenes();
		FCDSceneNode *root = 0;
		for (int k = 0; k < character_->joints.size(); k++) {
			WString t = character_->joints[k].GetId();
			fstring ff(t.c_str());
			FCDSceneNode* tmp = SearchJointNodeWithSubId(ff.c_str());
			if (tmp && !HasParentAnyJoint(tmp)) {
				root = tmp;
				break;
			}
		}

		if (root) {
			std::vector<Transform*> m;
			GetTransform(root, m);
			fstring ff = FUStringConversion::ToFString(root->GetSubId().size() ? root->GetSubId() : root->GetDaeId());
			character_->createRootJoint(m, ff.c_str());

			CreateChildJointNodes(root);
			character_->updateJointRelationship();
		}
	}

}
void ColladaLoad::BuildGeometries()
{
	geom_map_.clear();

	for (size_t i = 0 ; i < geometry_lib_->GetEntityCount(); i++) {
		FCDGeometry* g = geometry_lib_->GetEntity(i);
		fm::string str = g->GetDaeId();
		Geometry* cp = CopyGeometry(g);
		geom_map_[g] = cp;
	}

}


Geometry* ColladaLoad::CopyGeometry(FCDGeometry* a)
{
	Geometry* cp  = 0;
	cp = geom_db_->CreateGeometry();


	FCDGeometry* entity = a;
	FCDGeometryMesh* m = entity->GetMesh();

	for (size_t i = 0 ; i < m->GetPolygonsCount() ; i++) {

	FCDGeometryPolygons* p = m->GetPolygons(i);
	
	if (p->GetPrimitiveType() == FCDGeometryPolygons::LINES || p->GetPrimitiveType() == FCDGeometryPolygons::LINE_STRIPS) 
		continue;
	
	GPolygon* polygon = cp->CreateNewPolygon();
	WString tmp(p->GetMaterialSemantic().c_str());
	polygon->material_semantic_.resize(p->GetMaterialSemantic().size());

	for (int l = 0 ; l < polygon->material_semantic_.size(); l++)
		polygon->material_semantic_[l] = tmp[l];

	int fcount = p->GetFaceCount();
	polygon->faces_.resize(fcount);
	for (int mm = 0 ; mm < fcount ; mm++) {
		polygon->faces_[mm] = p->GetFaceVertexCounts()[mm];
	}

	for (int l = 0 ; l < p->GetInputCount() ; l++) {

		FCDGeometryPolygonsInput * pp = p->GetInput(l);
		FCDGeometrySource* gs = pp->GetSource();
		FUDaeGeometryInput::Semantic sem = pp->GetSemantic();

		if (sem == FUDaeGeometryInput::NORMAL) 
		{
			int index_cnt = pp->GetIndexCount();
			polygon->normal_indices_.resize(index_cnt);

			uint32* indices = pp->GetIndices();
			for (int k = 0 ; k < index_cnt; k++) {
				polygon->normal_indices_[k] = indices[k];
			}


			float* data = gs->GetData();
			int d_count = gs->GetDataCount();
			int stride = gs->GetStride();
			cp->normals_.resize(d_count/stride);
			for (int k = 0 ; k < cp->normals_.size(); k++) {
				for (int j = 0 ; j < stride; j++) {
					cp->normals_[k][j] = data[k * stride + j];
				}
			}
		}
		else if (sem == FUDaeGeometryInput::POSITION)
		{
			int index_cnt = pp->GetIndexCount();
			polygon->point_indices_.resize(index_cnt);

			uint32* indices = pp->GetIndices();
			for (int k = 0 ; k < index_cnt; k++) {
				polygon->point_indices_[k] = indices[k];
			}


			float* data = gs->GetData();
			int d_count = gs->GetDataCount();
			int stride = gs->GetStride();
			cp->points_.resize(d_count/stride);
			for (int k = 0 ; k < cp->points_.size(); k++) {
				for (int j = 0 ; j < stride; j++) {
					cp->points_[k][j] = data[k * stride + j];
				}
			}
		}
		else if (sem == FUDaeGeometryInput::TEXCOORD) 
		{
			int index_cnt = pp->GetIndexCount();
			polygon->texture_indices_.resize(index_cnt);

			uint32* indices = pp->GetIndices();
			for (int k = 0 ; k < index_cnt; k++) {
				polygon->texture_indices_[k] = indices[k];
			}


			float* data = gs->GetData();
			int d_count = gs->GetDataCount();
			int stride = gs->GetStride();
			cp->textures_.resize(d_count/stride);
			for (int k = 0 ; k < cp->textures_.size(); k++) {
				for (int j = 0 ; j < 2; j++) {
					cp->textures_[k][j] = data[k * stride + j];
				}
			}
		}
		else if (sem == FUDaeGeometryInput::UV)
		{
		}
		
	}
	}
	return cp;
}

void
ColladaLoad::BuildMaterials()
{

	TextureDatabase* tdb = scene_graph_->GetTextureDatabase();
	MaterialDatabase* mdb = scene_graph_->GetMaterialDatabase();

	int m_num_textures=(int) image_lib_->GetEntityCount();

	// Initialize DevIL. because we are going to read texture files
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);
	// opengl lower coord, opposite to directx
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT); 

	// copy textures to my structures
	FCDImage* image;
	for (int i=0; i<m_num_textures; i++) {
		image = image_lib_->GetEntity(i);
		fstring b(FUStringConversion::ToFString(image->GetDaeId().c_str()));
		GTexture* texture = tdb->CreateTexture(image->GetFilename().c_str(), WString(b.c_str()));
	}

	// Shutdown DevIL. Now it is not necessary
	ilShutDown();


	int m_num_materials=(int) material_lib_->GetEntityCount();

	// copy lights to my structures
	FCDMaterial* material;

	for (int i=0; i<m_num_materials; i++) {
		material = material_lib_->GetEntity(i);
		mdb->CreateMaterial(material);
	}
}

void 
ColladaLoad::Load(const WChar* fname)
{
	scene_graph_ = new SceneGraph();
	geom_db_ = scene_graph_->GetGeometryDatabase();
	material_db_ = scene_graph_->GetMaterialDatabase();

	if (!kColladaInitialized) {
		kColladaInitialized = true;
		FCollada::Initialize();
	}

	FUObjectRef<FCDocument> doc = FCollada::NewTopDocument();
 	
	bool ret = FCollada::LoadDocumentFromFile(doc,(const wchar_t*)fname);

	if(!ret) 
	{
		LOG("File Read Failed.\n");
		exit(0);
	}

	flat_model_ = true;
	scene_lib_ = doc->GetVisualSceneLibrary();
	image_lib_ = doc->GetImageLibrary();
	material_lib_ = doc->GetMaterialLibrary();
	effect_lib_ = doc->GetEffectLibrary();
	geometry_lib_ = doc->GetGeometryLibrary();
	controller_lib_ = doc->GetControllerLibrary();
	anim_lib_ = doc->GetAnimationLibrary();
	anim_clip_lib_ = doc->GetAnimationClipLibrary();

	BuildMaterials();
	BuildGeometries();

	ReconnectSceneNodes();
	ReconstructVisualScene();

	BuildControllers();
	BuildAnimations();

	ExportToKoonFlat();



	doc->Release();
	delete scene_graph_;
	LOG("polygon count %d\n", polygon_count_);
	LOG("vertex count %d\n", vertex_count_);
}
 

ColladaLoad::ColladaLoad(ImportedModel* model, AnimCharacter* charac)
{
	polygon_count_ = 0; 
	vertex_count_ = 0;
	model_ = model;
	character_ = charac;
}



};