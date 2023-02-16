#include "material/GMaterial.h"
#ifndef _UNICODE
#define _UNICODE
#endif

#include <IL/il.h>
#include <IL/ilut.h>

#undef _UNICODE

#include <string>

#define NO_LIBXML

#include <FUtils/FUtils.h>

#include <FCDocument/FCDocument.h>

#include <FCDocument\FCDLibrary.h>
#include <FCDocument\FCDGeometryMesh.h>
#include <FCDocument\FCDGeometry.h>
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
#include <FCDocument\FCDTexture.h>
#include <FCDocument\FCDImage.h>
#include <FCDocument\FCDEffectParameterSampler.h>

#include <FUtils\FUObject.h>
#include <FCollada.h>

#include "material/GTexture.h"
#include "material/TextureDatabase.h"
#include "material/MaterialDatabase.h"


namespace model_kernel 
{
	GMaterial::GMaterial(MaterialDatabase* mdb) : connected_material_database_(mdb)	,   user_special_(true)
	{
		SetDefaultMaterial();
	}

	GMaterial::GMaterial(MaterialDatabase* mdb, GTexture* tx) : connected_material_database_(mdb)	,   user_special_(true)
	{
		SetDefaultMaterial();
		SetMaterialName(tx->GetTextureName());
		SetTextureDiffuse(tx);
	}

	GMaterial::GMaterial(MaterialDatabase* mdb, TextureDatabase* main, FCDMaterial* material) : material_name_(), 
		connected_material_database_(mdb), user_special_(false)
	{
		// set default material if necessary
		if (material==NULL) {
			SetDefaultMaterial();
			return;
		}

		// if common profile does not exist, the set as a default materiqal
		FCDEffect* fx = material->GetEffect();
		FCDEffectProfile* profile = fx->FindProfile(FUDaeProfileType::COMMON);

		if (profile==NULL) {
			SetDefaultMaterial();
			return;
		}

		// if we arrive here, extract all information from "material"
		fstring str = FUStringConversion::ToFString(material->GetDaeId().c_str());
		material_name_.resize(str.size());
		for (int i = 0 ; i < str.size() ; i++) material_name_[i] = str[i];

		// copy properties of common profile
		FCDEffectStandard* standardProfile=dynamic_cast<FCDEffectStandard*>(profile);

		// configure alpha value in diffuse material
		transparency_=standardProfile->GetTranslucencyFactor();
		transparent_ = vec4(standardProfile->GetTranslucencyColor().x,
			standardProfile->GetTranslucencyColor().y,
			standardProfile->GetTranslucencyColor().z,
			standardProfile->GetTranslucencyColor().w);

		// ambient
		ambient_= vec4(
			standardProfile->GetAmbientColor().x,
			standardProfile->GetAmbientColor().y,
			standardProfile->GetAmbientColor().z,
			standardProfile->GetAmbientColor().w			  
			);

		// diffuse component
		diffuse_ = vec4(
			standardProfile->GetDiffuseColor().x,
			standardProfile->GetDiffuseColor().y,
			standardProfile->GetDiffuseColor().z,
			standardProfile->GetDiffuseColor().w // opaque for opengl, use transparency_ on polygon::render 		  
			);

		// specular
		float specular_factor=standardProfile->GetSpecularFactor();
		specular_ = vec4(
			specular_factor*standardProfile->GetSpecularColor().x,
			specular_factor*standardProfile->GetSpecularColor().y,
			specular_factor*standardProfile->GetSpecularColor().z,
			specular_factor*standardProfile->GetSpecularColor().w			  
			);

		// shininess
		shininess_=standardProfile->GetShininess();

		// emission
		if (standardProfile->IsEmissionFactor()) {
			emission_ = vec4(
				standardProfile->GetEmissionFactor(),
				standardProfile->GetEmissionFactor(),
				standardProfile->GetEmissionFactor(),
				standardProfile->GetEmissionFactor()
				);
		} else {
			emission_ = vec4(
				standardProfile->GetEmissionColor().x*standardProfile->GetEmissionFactor(),
				standardProfile->GetEmissionColor().y*standardProfile->GetEmissionFactor(),
				standardProfile->GetEmissionColor().z*standardProfile->GetEmissionFactor(),
				standardProfile->GetEmissionColor().w*standardProfile->GetEmissionFactor()
				);
		}

		// not used at the moment
		//transparent mode
		if (standardProfile->GetTransparencyMode()==FCDEffectStandard::A_ONE) {
			transparency_ = 1.0-transparency_;
			for (int i = 0 ;i < 3 ; i++) {
				transparent_[i] = 1.0-transparent_[3];
			}
		}

		// textures
		FCDTexture* texture;
		FCDImage *image;

		// diffusse textures
		has_diffuse_texture_=false;
		texture_diffuse_=NULL;

		// diffuse texture
		if (standardProfile->GetTextureCount(FUDaeTextureChannel::DIFFUSE)>0) {
			texture=standardProfile->GetTexture(FUDaeTextureChannel::DIFFUSE,0);
			if (texture!=NULL) {
				image=texture->GetImage();
				if (image!=NULL) {

					fstring str = FUStringConversion::ToFString(image->GetDaeId().c_str());
					//std::string a(image->GetDaeId().c_str());
					texture_diffuse_=main->FindByTextureName(WString(str.c_str()));
					if (texture_diffuse_!=NULL) {
						if (texture_diffuse_->GetTextureId()!=-1) {
							texture_diffuse_->SetWrapS(texture->GetSampler()->GetWrapS()==FUDaeTextureWrapMode::WrapMode::WRAP);
							texture_diffuse_->SetWrapT(texture->GetSampler()->GetWrapT()==FUDaeTextureWrapMode::WrapMode::WRAP);
							has_diffuse_texture_=true;
						}
					}
				}
			}
		}	

		// reflective texture
		has_reflective_texture_=false;
		texture_reflective_=NULL;
		image=NULL;

		// reflective texture
		if (standardProfile->GetTextureCount(FUDaeTextureChannel::REFLECTION)>0) {
			//float r=standardProfile->GetReflectivityFactor();
			//float r=standardProfile->GetReflectivity();
			//float r2=standardProfile->GetReflectivityFactor();
			texture=standardProfile->GetTexture(FUDaeTextureChannel::REFLECTION,0);
			if (texture!=NULL) {
				image=texture->GetImage();
				if (image!=NULL) {
					fstring str = FUStringConversion::ToFString(image->GetDaeId().c_str());
					texture_reflective_=main->FindByTextureName(WString(str.c_str()));
					if (texture_reflective_!=NULL) {
						if (texture_reflective_->GetTextureId()!=-1) {
							texture_reflective_->SetWrapS(texture->GetSampler()->GetWrapS()==FUDaeTextureWrapMode::WrapMode::WRAP);
							texture_reflective_->SetWrapT(texture->GetSampler()->GetWrapT()==FUDaeTextureWrapMode::WrapMode::WRAP);
							has_reflective_texture_=true;
						}
					}
				}
			}
		}

		// transparent textures
		has_transparent_texture_=false;
		texture_transparent_=NULL;
		image=NULL;

		//// transparent texture
		//if (standardProfile->GetTextureCount(FUDaeTextureChannel::TRANSPARENT)>0) {
		//	texture=standardProfile->GetTexture(FUDaeTextureChannel::TRANSPARENT,0);
		//	if (texture!=NULL) {
		//		image=texture->GetImage();
		//		if (image!=NULL) {
		//			texture_transparent_=m_main->SearchTextureByName(image->GetDaeId());
		//			if (texture_transparent_!=NULL)
		//				if (texture_transparent_->GetTextureId()!=-1)
		//					has_transparent_texture_=true;
		//		}
		//	}
		//}

		SetDefaultMaterial(1);

	}


	void GMaterial::append(graphics::gHash& h) const
	{
		h.append(type_);

		h.append(diffuse_[0]);
		h.append(diffuse_[1]);
		h.append(diffuse_[2]);
		h.append(diffuse_[3]);

		h.append(transparency_);
		h.append(transparent_[0]);
		h.append(transparent_[1]);
		h.append(transparent_[2]);
		h.append(transparent_[3]);

		h.append(ambient_[0]);
		h.append(ambient_[1]);
		h.append(ambient_[2]);
		h.append(ambient_[3]);

		h.append(emission_[0]);
		h.append(emission_[1]);
		h.append(emission_[2]);
		h.append(emission_[3]);
		
		h.append(translucence_);
		h.append(translucence_depth_);
		h.append(translucence_focus_);

		h.append(eccentricity_);
		h.append(specularrolloff_);
		h.append(angle_);
		h.append(spreadx_);
		h.append(spready_);
		h.append(roughness_);
		h.append(fresnelindex_);

		h.append(specular_[0]);
		h.append(specular_[1]);
		h.append(specular_[2]);
		h.append(specular_[3]);
		
		h.append(shininess_);

		h.append(reflectivity_);
	
		h.append(reflective_[0]);
		h.append(reflective_[1]);
		h.append(reflective_[2]);
		h.append(reflective_[3]);

		h.append(index_of_refraction_);
		h.append(limit_of_refraction_);
		h.append(light_absorbance_);
		h.append(surface_thickness_);
		h.append(shadow_attenutation_);
		h.append(chromatic_aberration_);
		h.append(limit_of_reflection_);
		h.append(specularity_reflection_);

		h.append(material_name_.c_str());

		if (has_diffuse_texture_) {
			texture_diffuse_->append(h);
		}
		if (has_transparency_texture_) {
			texture_transparency_->append(h);
		}
		if (has_transparent_texture_) {
			texture_transparent_->append(h);
		}
		if (has_ambient_texture_) {
			texture_ambient_->append(h);
		}
		if (has_emission_texture_) {
			texture_emission_->append(h);
		}
		if (has_bumpmapping_texture_) {
			texture_bumpmapping_->append(h);
		}
		if (has_translucence_texture_) {
			texture_translucence_->append(h);
		}
		if (has_translucence_depth_texture_) {
			texture_translucence_depth_->append(h);
		}
		if (has_translucence_focus_texture_) {
			texture_translucence_focus_->append(h);
		}
		if (has_eccentricity_texture_) {
			texture_eccentricity_->append(h);
		}
		if (has_specularrolloff_texture_) {
			texture_specularrolloff_->append(h);
		}
		if (has_angle_texture_) {
			texture_angle_->append(h);
		}
		if (has_spreadx_texture_) {
			texture_spreadx_->append(h);
		}
		if (has_spready_texture_) {
			texture_spready_->append(h);
		}
		if (has_roughness_texture_) {
			texture_roughness_->append(h);
		}
		if (has_fresnelindex_texture_) {
			texture_fresnelindex_->append(h);
		}
		if (has_specular_texture_) {
			texture_specular_->append(h);
		}
		if (has_shininess_texture_) {
			texture_shininess_->append(h);
		}
		if (has_reflectivity_texture_) {
			texture_reflectivity_->append(h);
		}
		if (has_reflective_texture_) {
			texture_reflective_->append(h);
		}
		if (has_index_of_refraction_texture_) {
			texture_index_of_refraction_->append(h);
		}
		if (has_limit_of_refraction_texture_) {
			texture_limit_of_refraction_->append(h);
		}
		if (has_light_absorbance_texture_) {
			texture_light_absorbance_->append(h);
		}
		if (has_surface_thickness_texture_) {
			texture_surface_thickness_->append(h);
		}
		if (has_shadow_attenutation_texture_) {
			texture_shadow_attenutation_->append(h);
		}
		if (has_limit_of_reflection_texture_) {
			texture_limit_of_reflection_->append(h);
		}
		if (has_specularity_reflection_texture_) {
			texture_specularity_reflection_->append(h);
		}
	}


	GMaterial::~GMaterial(void)
	{

	}
	MaterialType GMaterial::GetMaterialType(void) const
	{
		return type_;
	}
	void GMaterial::SetMaterialType(MaterialType t)
	{
		type_ = t;
	}

	//=Color =============================================================================
	color GMaterial::GetDiffuse(void) const
	{
		return diffuse_;
	}
	void GMaterial::SetDiffuse(color c)
	{
		diffuse_ = c;
	}
	color GMaterial::Get_DefaultColor(void) const
	{
		return vec4( 0.7f,0.7f,0.7f,1.0f );
	}

	void GMaterial::SetTextureDiffuse(GTexture* t)
	{
		if (t==NULL)
			has_diffuse_texture_ = false;
		else
			has_diffuse_texture_ = true;
		texture_diffuse_ = t;
	}
	//= transparency =============================================================================
	real GMaterial::GetTransparency() const
	{
		return transparency_;
	}

	void GMaterial::SetTransparency(real d)
	{
		transparency_ = d;
	}
	real GMaterial::Get_DefaultTransparency(void) const
	{
		return 0.0;
	}
	void GMaterial::SetTextureTransparency(GTexture* t)
	{
		if (t==NULL)
			has_transparency_texture_ = false;
		else
			has_transparency_texture_ = true;
		texture_transparency_ = t;
	}

	//= transparent =============================================================================
	color GMaterial::GetTransparent(void) const
	{
		return transparent_;
	}

	void GMaterial::SetTransparent(color c)
	{
		transparent_ = c;
	}
	color GMaterial::Get_DefaultTransparent(void) const
	{
		return vec4( 0.0f,0.0f,0.0f,1.0f ); 
	}

	void GMaterial::SetTextureTransparent(GTexture* t)
	{
		if (t==NULL)
			has_transparent_texture_ = false;
		else
			has_transparent_texture_ = true;
		texture_transparent_ = t;
	}

	//==============================================================================
	color GMaterial::GetTranslucency(void) const
	{ 
		return transparent_; 
	}

	void GMaterial::SetTranslucency(color c)
	{
		transparent_ = c;
	}
	//==============================================================================

	//= ambient =============================================================================
	color GMaterial::GetAmbient(void) const
	{
		return ambient_;
	}
	void GMaterial::SetAmbient(color c)
	{
		ambient_ = c;
	}
	color GMaterial::Get_DefaultAmbient(void) const
	{
		return vec4( 0.2f,0.2f,0.2f,1.0f );
	}
	void GMaterial::SetTextureAmbient(GTexture* t)
	{
		if (t==NULL)
			has_ambient_texture_ = false;
		else
			has_ambient_texture_ = true;
		texture_ambient_ = t;
	}
	//= Emission =============================================================================
	color GMaterial::GetEmission(void) const
	{
		return emission_;
	}
	void GMaterial::SetEmission(color c)
	{
		emission_ = c;
	}
	color GMaterial::Get_DefaultEmission(void) const
	{
		return vec4( 0.0f,0.0f,0.0f,1.0f );
	}
	void GMaterial::SetTextureEmission(GTexture* t)
	{
		if (t==NULL)
			has_emission_texture_ = false;
		else
			has_emission_texture_ = true;

		texture_emission_ = t;
	}
	//= Bump mapping =============================================================================
	void GMaterial::SetTextureBumpmapping(GTexture* t)
	{
		if (t==NULL)
			has_bumpmapping_texture_ = false;
		else
			has_bumpmapping_texture_ = true;

		texture_bumpmapping_ = t;
	}
	//=Translucence =============================================================================	
	real GMaterial::GetTranslucence() const
	{
		return translucence_;
	}

	void GMaterial::SetTranslucence(real d)
	{
		translucence_ = d;
	}
	real GMaterial::Get_DefaultTranslucence(void) const
	{
		return 1.0;
	}
	void GMaterial::SetTextureTranslucence(GTexture* t)
	{
		if (t==NULL)
			has_translucence_texture_ = false;
		else
			has_translucence_texture_ = true;
		texture_translucence_ = t;
	}
	//=Translucence Depth =============================================================================
	real GMaterial::GetTranslucenceDepth(void) const
	{
		return translucence_depth_;
	}

	void GMaterial::SetTranslucenceDepth(real d)
	{
		translucence_depth_ = d;
	}
	real GMaterial::Get_DefaultTranslucenceDepth(void) const
	{
		return 0.0;
	}
	void GMaterial::SetTextureTranslucenceDepth(GTexture* t)
	{
		if (t==NULL)
			has_translucence_depth_texture_ = false;
		else
			has_translucence_depth_texture_ = true;
		texture_translucence_depth_ = t;
	}

	//=Translucence Focus =============================================================================
	real GMaterial::GetTranslucenceFocus(void) const
	{
		return translucence_focus_;
	}
	void GMaterial::SetTranslucenceFocus(real d)
	{
		translucence_focus_ = d;
	}
	real GMaterial::Get_DefaultTranslucenceFocus(void) const
	{
		return 0.0;
	}
	void GMaterial::SetTextureTranslucenceFocus(GTexture* t)
	{
		if (t==NULL)
			has_translucence_focus_texture_ = false;
		else
			has_translucence_focus_texture_ = true;
		texture_translucence_focus_ = t;
	}
	//=Eccentricity =============================================================================	
	real GMaterial::GetEccentricity(void) const
	{
		return eccentricity_;
	}
	void GMaterial::SetEccentricity(real d)
	{
		eccentricity_ = d;
	}
	real GMaterial::Get_DefaultEccentricity(void) const
	{
		return 0.3;
	}
	void GMaterial::SetTextureEccentricity(GTexture* t)
	{
		if (t==NULL)
			has_eccentricity_texture_ = false;
		else
			has_eccentricity_texture_ = true;
		texture_eccentricity_ = t;
	}

	//=Specular Roll Off =============================================================================	
	real GMaterial::GetSpecularRollOff(void) const
	{
		return specularrolloff_;
	}
	void GMaterial::SetSpecularRollOff(real d)
	{
		specularrolloff_ = d;
	}
	real GMaterial::Get_DefaultSpecularRollOff(void) const
	{
		return 0.7;
	}
	void GMaterial::SetTextureSpecularRollOff(GTexture* t)
	{
		if (t==NULL)
			has_specularrolloff_texture_ = false;
		else
			has_specularrolloff_texture_ = true;
		texture_specularrolloff_ = t;
	}
	//=Angle =============================================================================	
	real GMaterial::GetAngle(void) const
	{
		return angle_;
	}
	void GMaterial::SetAngle(real d)
	{
		angle_ = d;
	}
	real GMaterial::Get_DefaultAngle(void) const
	{
		return 0.0;
	}
	void GMaterial::SetTextureAngle(GTexture* t)
	{
		if (t==NULL)
			has_angle_texture_ = false;
		else
			has_angle_texture_ = true;
		texture_angle_ = t;
	}
	//=Spread X =============================================================================	
	real GMaterial::GetSpreadX(void) const
	{
		return spreadx_;
	}
	void GMaterial::SetSpreadX(real d)
	{
		spreadx_ = d;
	}
	real GMaterial::Get_DefaultSpreadX(void) const
	{
		return 13.0;
	}
	void GMaterial::SetTextureSpreadX(GTexture* t)
	{
		if (t==NULL)
			has_spreadx_texture_ = false;
		else
			has_spreadx_texture_ = true;
		texture_spreadx_ = t;
	}
	//=Spread Y =============================================================================	
	real GMaterial::GetSpreadY(void) const
	{
		return spready_;
	}
	void GMaterial::SetSpreadY(real d)
	{
		spready_ = d;
	}
	real GMaterial::Get_DefaultSpreadY(void) const
	{
		return 3.0;
	}
	void GMaterial::SetTextureSpreadY(GTexture* t)
	{
		if (t==NULL)
			has_spready_texture_ = false;
		else
			has_spready_texture_ = true;
		texture_spready_ = t;
	}
	//=Roughness =============================================================================	
	real GMaterial::GetRoughness(void) const
	{
		return roughness_;
	}
	void GMaterial::SetRoughness(real d)
	{
		roughness_ = d;
	}
	real GMaterial::Get_DefaultRoughness(void) const
	{
		return 0.7;
	}
	void GMaterial::SetTextureRoughness(GTexture* t)
	{
		if (t==NULL)
			has_roughness_texture_ = false;
		else
			has_roughness_texture_ = true;
		texture_roughness_ = t;
	}
	//=Fresnel Index =============================================================================	
	real GMaterial::GetFresnelIndex(void) const
	{
		return fresnelindex_;
	}
	void GMaterial::SetFresnelIndex(real d)
	{
		fresnelindex_ = d;
	}
	real GMaterial::Get_DefaultFresnelIndex(void) const
	{
		return 1.33;
	}
	void GMaterial::SetTextureFresnelIndex(GTexture* t)
	{
		if (t==NULL)
			has_fresnelindex_texture_ = false;
		else
			has_fresnelindex_texture_ = true;
		texture_fresnelindex_ = t;
	}
	//=Specular =============================================================================	
	color GMaterial::GetSpecular(void) const
	{
		return specular_;
	}
	void GMaterial::SetSpecular(color c)
	{
		specular_ = c;
	}
	color GMaterial::Get_DefaultSpecular(void) const
	{
		return vec4(0.5,0.5,0.5,1.0);
	}
	void GMaterial::SetTextureSpecular(GTexture* t)
	{
		if (t==NULL)
			has_specular_texture_ = false;
		else
			has_specular_texture_ = true;
		texture_specular_ = t;
	}
	//=Shininess============================================================================	
	real GMaterial::GetShininess(void) const
	{
		return shininess_;
	}

	void GMaterial::SetShininess(real d)
	{
		shininess_ = d;
	}
	real GMaterial::Get_DefaultShininess(void) const
	{
		return 32.0;
	}
	void GMaterial::SetTextureShininess(GTexture* t)
	{
		if (t==NULL)
			has_shininess_texture_ = false;
		else
			has_shininess_texture_ = true;
		texture_shininess_ = t;
	}

	//=Reflectivity =============================================================================	
	real GMaterial::GetReflectivity(void) const
	{
		return reflectivity_;
	}

	void GMaterial::SetReflectivity(real d)
	{
		reflectivity_ = d;
	}
	real GMaterial::Get_DefaultReflectivity(void) const
	{
		return 0.5;
	}
	void GMaterial::SetTextureReflectivity(GTexture* t)
	{
		if (t==NULL)
			has_reflectivity_texture_ = false;
		else
			has_reflectivity_texture_ = true;
		texture_reflectivity_ = t;
	}
	//=Reflective =============================================================================	
	color GMaterial::GetReflective(void) const
	{
		return reflective_;
	}
	void GMaterial::SetReflective(color c)
	{
		reflective_ = c;
	}
	color GMaterial::Get_DefaultReflective(void) const
	{
		//return vec4(0.99,0.99,0.99,1.0);
		return vec4(0.2,0.2,0.2,1.0);
	}
	void GMaterial::SetTextureReflective(GTexture* t)
	{
		if (t==NULL)
			has_reflective_texture_ = false;
		else
			has_reflective_texture_ = true;
		texture_reflective_ = t;
	}
	//=Index Of Refraction =============================================================================	
	real GMaterial::GetIndexOfRefraction(void) const
	{
		return index_of_refraction_;
	}
	void GMaterial::SetIndexOfRefraction(real d)
	{
		index_of_refraction_ = d;
	}
	real GMaterial::Get_DefaultIndexOfRefraction(void) const
	{
		return 1.0;
	}
	void GMaterial::SetTextureIndexOfRefraction(GTexture* t)
	{
		if (t==NULL)
			has_index_of_refraction_texture_ = false;
		else
			has_index_of_refraction_texture_ = true;
		texture_index_of_refraction_ = t;
	}

	//=Limit Of Refraction =============================================================================	
	real GMaterial::GetLimitOfRefraction(void) const
	{
		return limit_of_refraction_;
	}
	void GMaterial::SetLimitOfRefraction(real d)
	{
		limit_of_refraction_ = d;
	}
	real GMaterial::Get_DefaultLimitOfRefraction(void) const
	{
		return 10.0;
	}
	void GMaterial::SetTextureLimitOfRefraction(GTexture* t)
	{
		if (t==NULL)
			has_limit_of_refraction_texture_ = false;
		else
			has_limit_of_refraction_texture_ = true;
		texture_limit_of_refraction_ = t;
	}

	//=Light Absorbance =============================================================================	
	real GMaterial::GetLightAbsorbance(void) const
	{
		return light_absorbance_;
	}
	void GMaterial::SetLightAbsorbance(real d)
	{
		light_absorbance_ = d;
	}
	real GMaterial::Get_DefaultLightAbsorbance(void) const
	{
		return 0.0;
	}
	void GMaterial::SetTextureLightAbsorbance(GTexture* t)
	{
		if (t==NULL)
			has_light_absorbance_texture_ = false;
		else
			has_light_absorbance_texture_ = true;
		texture_light_absorbance_ = t;
	}

	//=Surface Thickness=============================================================================	
	real GMaterial::GetSurfaceThickness(void) const
	{
		return surface_thickness_;
	}
	void GMaterial::SetSurfaceThickness(real d)
	{
		surface_thickness_ = d;
	}
	real GMaterial::Get_DefaultSurfaceThickness(void) const
	{
		return 0.0;
	}
	void GMaterial::SetTextureSurfaceThickness(GTexture* t)
	{
		if (t==NULL)
			has_surface_thickness_texture_ = false;
		else
			has_surface_thickness_texture_ = true;
		texture_surface_thickness_ = t;
	}

	//=Shadow Attenutation=============================================================================	
	real GMaterial::GetShadowAttenutation(void) const
	{
		return shadow_attenutation_;
	}
	void GMaterial::SetShadowAttenutation(real d)
	{
		shadow_attenutation_ = d;
	}
	real GMaterial::Get_DefaultShadowAttenutation(void) const
	{
		return 1.0;
		//return 0.5;
	}
	void GMaterial::SetTextureShadowAttenutation(GTexture* t)
	{
		if (t==NULL)
			has_shadow_attenutation_texture_ = false;
		else
			has_shadow_attenutation_texture_ = true;
		texture_shadow_attenutation_ = t;
	}

	//=ChromaticAberration=============================================================================	
	bool GMaterial::GetChromaticAberration(void) const
	{
		return chromatic_aberration_;
	}
	void GMaterial::SetChromaticAberration(bool d)
	{
		chromatic_aberration_ = d;
	}

	//=Limit Of Reflection=============================================================================	
	real GMaterial::GetLimitOfReflection(void) const
	{
		return limit_of_reflection_;
	}
	void GMaterial::SetLimitOfReflection(real d)
	{
		limit_of_reflection_ = d;
	}
	real GMaterial::Get_DefaultLimitOfReflection(void) const
	{
		return 5.0;
	}
	void GMaterial::SetTextureLimitOfReflection(GTexture* t)
	{
		if (t==NULL)
			has_limit_of_reflection_texture_ = false;
		else
			has_limit_of_reflection_texture_ = true;
		texture_limit_of_reflection_ = t;
	}

	//=Specularity Reflection============================================================================	
	real GMaterial::GetSpecularityReflection(void) const
	{
		return specularity_reflection_;
	}
	void GMaterial::SetSpecularityReflection(real d)
	{
		specularity_reflection_ = d;
	}
	real GMaterial::Get_DefaultSpecularityReflection(void) const
	{
		return 1.0;
	}
	void GMaterial::SetTextureSpecularityReflection(GTexture* t)
	{
		if (t==NULL)
			has_specularity_reflection_texture_ = false;
		else
			has_specularity_reflection_texture_ = true;
		texture_specularity_reflection_ = t;
	}


	// if diffuse texture has a alfa channel or opacity is less than 1, this will return true
	bool GMaterial::IsTransparent() {

		// if texture has alpha channel
		/*if ((has_diffuse_texture_==true) && (texture_diffuse_->HasAlphaChannel())) 
			return true;*/

		color t = transparent_ * transparency_;

			if (t[0] >0.0 || t[1] >0.0 || t[2] >0.0 || t[3] >0.0)
				return true;


		// in other case, false
		return false;
	}

	void GMaterial::SetDefaultMaterial(int type) {

		if ( type == 0 )
		{
			material_name_.clear();

			diffuse_ = Get_DefaultColor(); 
			texture_diffuse_ = NULL;
			has_diffuse_texture_=false;

			//translucency_ = Get_DefaultTranslucency(); 
			transparent_ = Get_DefaultTransparent();	
			texture_transparent_ = NULL;
			has_transparent_texture_ = false;
		
			transparency_ = Get_DefaultTransparency();

			ambient_ = Get_DefaultAmbient();

			emission_ = Get_DefaultEmission();

			shininess_ = Get_DefaultShininess();

			specular_ = Get_DefaultSpecular();

			texture_reflective_ = NULL;
			has_reflective_texture_=false;

		}

		type_ = Phong;

		texture_ambient_ = NULL;
		has_ambient_texture_ = false;

		texture_emission_ = NULL;
		has_emission_texture_ = false;

		texture_shininess_ = NULL;
		has_shininess_texture_ = false;

		texture_transparency_ = NULL;
		has_transparency_texture_=false;
		
		texture_bumpmapping_ = NULL;
		has_bumpmapping_texture_ = false;

		translucence_ = Get_DefaultTranslucence();
		texture_translucence_ = NULL;
		has_translucence_texture_ = false;
		
		translucence_depth_ = Get_DefaultTranslucenceDepth();
		texture_translucence_depth_ = NULL;
		has_translucence_depth_texture_ = false;
			
		translucence_focus_ = Get_DefaultTranslucenceFocus();
		texture_translucence_focus_ = NULL;
		has_translucence_focus_texture_ = false;
		
		eccentricity_ = Get_DefaultEccentricity();
		texture_eccentricity_ = NULL;
		has_eccentricity_texture_ = false;

		specularrolloff_ = Get_DefaultSpecularRollOff();
		texture_specularrolloff_ = NULL;
		has_specularrolloff_texture_ = false;
		
		angle_ = Get_DefaultAngle();
		texture_angle_ = NULL;
		has_angle_texture_ = false;

		spreadx_ = Get_DefaultSpreadX();
		texture_spreadx_ = NULL;
		has_spreadx_texture_ = false;
		
		spready_ = Get_DefaultSpreadY();
		texture_spready_ = NULL;
		has_spready_texture_ = false;
		
		roughness_ = Get_DefaultRoughness();
		texture_roughness_ = NULL;
		has_roughness_texture_ = false;

		fresnelindex_ = Get_DefaultFresnelIndex();
		texture_fresnelindex_ = NULL;
		has_fresnelindex_texture_ = false;
		
		texture_specular_ = NULL;
		has_specular_texture_ = false;
		
		reflectivity_ = Get_DefaultReflectivity();
		texture_reflectivity_ = NULL;
		has_reflectivity_texture_ = false;
		
		reflective_ =  Get_DefaultReflective();

		index_of_refraction_ = Get_DefaultIndexOfRefraction();
		texture_index_of_refraction_ = NULL;
		has_index_of_refraction_texture_ = false;
		
		limit_of_refraction_ = Get_DefaultLimitOfRefraction();
		texture_limit_of_refraction_ = NULL;
		has_limit_of_refraction_texture_ = false;
		
		light_absorbance_ = Get_DefaultLightAbsorbance();
		texture_light_absorbance_ = NULL;
		has_light_absorbance_texture_ = false;
		
		surface_thickness_ = Get_DefaultSurfaceThickness();
		texture_surface_thickness_ = NULL;
		has_surface_thickness_texture_ = false;
		
		shadow_attenutation_ = Get_DefaultShadowAttenutation();
		texture_shadow_attenutation_ = NULL;
		has_shadow_attenutation_texture_ = false;

		chromatic_aberration_ = false;

		limit_of_reflection_ = Get_DefaultLimitOfReflection();
		texture_limit_of_reflection_ = NULL;
		has_limit_of_reflection_texture_ = false;

		specularity_reflection_ = Get_DefaultSpecularityReflection();
		texture_specularity_reflection_ = NULL;
		has_specularity_reflection_texture_ = false;


	}

	

	bool GMaterial::IsMasking() {
		//return ((this->GetTransparentMode()==GMaterial::RGB_ZERO) && (this->HasTransparentTexture()));
		return false;
	}

	bool GMaterial::operator == (const GMaterial& right) const
	{
		graphics::gHash a;
		append(a);
		graphics::gHash b;
		right.append(b);
		if (a.value() == b.value()) return true;
		return false;
	}

}