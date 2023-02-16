

#ifndef __GMaterial_h
#define __GMaterial_h

#include <string>
#include <model/kernel.h>
#include <graphics/color.h>
#include <graphics/unsigned.h>

#include <graphics/gHash.h>

using namespace graphics;

class FCDMaterial;

namespace model_kernel
{
class SceneGraph;
class TextureDatabase;
class MaterialDatabase;
class GTexture;

enum MaterialType { None, Lambert, Phong, Blinn, Anisotropic };
const char* const material_types[] = {
  "", "Lambert", "Phong", "Blinn", "Anisotropic", 0  };

class GMaterial
{
public:
	

	DECLARE_CLASS(GMaterial);

	GMaterial(MaterialDatabase* mdb, TextureDatabase* main, FCDMaterial* material);

	GMaterial(MaterialDatabase* mdb);
	GMaterial(MaterialDatabase* mdb, GTexture* tx);

	virtual void append(gHash& h) const;

	~GMaterial(void);

	MaterialType GetMaterialType(void) const;
	void SetMaterialType(MaterialType);

	// diffuse color value
	color GetDiffuse(void) const;
	void SetDiffuse(color);
	color Get_DefaultColor(void) const;

	GTexture* GetTextureDiffuse(void) const { return texture_diffuse_; }
	void SetTextureDiffuse(GTexture*);
	bool HasDiffuseTexture(void) { return has_diffuse_texture_; }

	// transparency 
	real GetTransparency(void) const;
	void SetTransparency(real);
	real Get_DefaultTransparency(void) const;

	GTexture* GetTextureTransparency(void) const { return texture_transparency_; }
	void SetTextureTransparency(GTexture*);
	bool HasTransparencyTexture(void) { return has_transparency_texture_; }

	// transparent Color
	color GetTransparent(void) const;
	void  SetTransparent(color);
	color Get_DefaultTransparent(void) const;
	
	GTexture* GetTextureTransparent(void) const { return texture_transparent_; }
	void SetTextureTransparent(GTexture*);
	bool HasTransparentTexture(void) { return has_transparent_texture_; }

	color GetTranslucency(void) const;			// not used
	void SetTranslucency(color);				// not used

	//========================================================
	//color Get_DefaultTranslucency(void) const;
	//GTexture* GetTextureTransparency(void) const { return texture_transparency_; }
	//void SetTextureTransparency(GTexture*);
	//bool HasTransparencyTexture(void) { return has_transparency_texture_; }
	//========================================================

	// ambient
	color GetAmbient(void) const;
	void SetAmbient(color);
	color Get_DefaultAmbient(void) const;

	GTexture* GetTextureAmbient(void) const { return texture_ambient_; }
	void SetTextureAmbient(GTexture*);
	bool HasAmbientTexture(void) { return has_ambient_texture_; }

	// Emission
	color GetEmission(void) const;
	void SetEmission(color);
	color Get_DefaultEmission(void) const;

	GTexture* GetTextureEmission(void) const { return texture_emission_; }
	void SetTextureEmission(GTexture*);
	bool HasEmissionTexture(void) { return has_emission_texture_; }

	// Bump Mapping
	GTexture* GetTextureBumpmapping(void) const { return texture_bumpmapping_; }
	void SetTextureBumpmapping(GTexture*);
	bool HasBumpmappingTexture(void) { return has_bumpmapping_texture_; }

	// Translucence 
	real GetTranslucence(void) const;
	void SetTranslucence(real);
	real Get_DefaultTranslucence(void) const;

	GTexture* GetTextureTranslucence(void) const { return texture_translucence_; }
	void SetTextureTranslucence(GTexture*);
	bool HasTranslucenceTexture(void) { return has_translucence_texture_; }

	// Translucence Depth
	real GetTranslucenceDepth(void) const;
	void SetTranslucenceDepth(real);
	real Get_DefaultTranslucenceDepth(void) const;

	GTexture* GetTextureTranslucenceDepth(void) const { return texture_translucence_depth_; }
	void SetTextureTranslucenceDepth(GTexture*);
	bool HasTranslucenceDepthTexture(void) { return has_translucence_depth_texture_; }
	
	// Translucence Focus
	real GetTranslucenceFocus(void) const;
	void SetTranslucenceFocus(real);
	real Get_DefaultTranslucenceFocus(void) const;

	GTexture* GetTextureTranslucenceFocus(void) const { return texture_translucence_focus_; }
	void SetTextureTranslucenceFocus(GTexture*);
	bool HasTranslucenceFocusTexture(void) { return has_translucence_focus_texture_; }
		
	// Specular Color
	color GetSpecular(void) const;
	void SetSpecular(color);
	color Get_DefaultSpecular(void) const;

	GTexture* GetTextureSpecular(void) const { return texture_specular_; }
	void SetTextureSpecular(GTexture*);
	bool HasSpecularTexture(void) { return has_specular_texture_; }

	// Shininess - Specular Exponent
	real GetShininess(void) const;
	void SetShininess(real);
	real Get_DefaultShininess(void) const;

	GTexture* GetTextureShininess(void) const { return texture_shininess_; }
	void SetTextureShininess(GTexture*);
	bool HasShininessTexture(void) { return has_shininess_texture_; }

	// Eccentricity - in case of Blinn Type
	real GetEccentricity(void) const;
	void SetEccentricity(real);
	real Get_DefaultEccentricity(void) const;
	
	GTexture* GetTextureEccentricity(void) const { return texture_eccentricity_; }
	void SetTextureEccentricity(GTexture*);
	bool HasEccentricityTexture(void) { return has_eccentricity_texture_; }
	
	// Specular Roll Off - in case of Blinn Type
	real GetSpecularRollOff(void) const;
	void SetSpecularRollOff(real);
	real Get_DefaultSpecularRollOff(void) const;

	GTexture* GetTextureSpecularRollOff(void) const { return texture_specularrolloff_; }
	void SetTextureSpecularRollOff(GTexture*);
	bool HasSpecularRollOffTexture(void) { return has_specularrolloff_texture_; }
	
	// Angle - in case of Anisotropic
	real GetAngle(void) const;
	void SetAngle(real);
	real Get_DefaultAngle(void) const;

	GTexture* GetTextureAngle(void) const { return texture_angle_; }
	void SetTextureAngle(GTexture*);
	bool HasAngleTexture(void) { return has_angle_texture_; }

	// Spread X - in case of Anisotropic
	real GetSpreadX(void) const;
	void SetSpreadX(real);
	real Get_DefaultSpreadX(void) const;

	GTexture* GetTextureSpreadX(void) const { return texture_spreadx_; }
	void SetTextureSpreadX(GTexture*);
	bool HasSpreadXTexture(void) { return has_spreadx_texture_; }

	// Spread Y - in case of Anisotropic
	real GetSpreadY(void) const;
	void SetSpreadY(real);
	real Get_DefaultSpreadY(void) const;

	GTexture* GetTextureSpreadY(void) const { return texture_spready_; }
	void SetTextureSpreadY(GTexture*);
	bool HasSpreadYTexture(void) { return has_spready_texture_; }

	// Roughness - in case of Anisotropic
	real GetRoughness(void) const;
	void SetRoughness(real);
	real Get_DefaultRoughness(void) const;

	GTexture* GetTextureRoughness(void) const { return texture_roughness_; }
	void SetTextureRoughness(GTexture*);
	bool HasRoughnessTexture(void) { return has_roughness_texture_; }

	// Fresnel Index - in case of Anisotropic
	real GetFresnelIndex(void) const;
	void SetFresnelIndex(real);
	real Get_DefaultFresnelIndex(void) const;

	GTexture* GetTextureFresnelIndex(void) const { return texture_fresnelindex_; }
	void SetTextureFresnelIndex(GTexture*);
	bool HasFresnelIndexTexture(void) { return has_fresnelindex_texture_; }


	// Reflectivity
	real GetReflectivity(void) const;
	void SetReflectivity(real);
	real Get_DefaultReflectivity(void) const;

	GTexture* GetTextureReflectivity(void) const { return texture_reflectivity_; }
	void SetTextureReflectivity(GTexture*);
	bool HasReflectivityTexture(void) { return has_reflectivity_texture_; }

	// Reflected Color
	color GetReflective(void) const;
	void SetReflective(color);
	color Get_DefaultReflective(void) const;

	GTexture* GetTextureReflective(void) const	{	return texture_reflective_;	}
	void SetTextureReflective(GTexture* t);
	bool HasReflectiveTexture(void)	{	return has_reflective_texture_; 	}

	// Refractive Index
	real GetIndexOfRefraction(void) const;
	void SetIndexOfRefraction(real);
	real Get_DefaultIndexOfRefraction(void) const;

	GTexture* GetTextureIndexOfRefraction(void) const	{	return texture_index_of_refraction_;	}
	void SetTextureIndexOfRefraction(GTexture* t);
	bool HasIndexOfRefractionTexture(void)	{	return has_index_of_refraction_texture_; 	}

	// Refraction Limit
	real GetLimitOfRefraction(void) const;
	void SetLimitOfRefraction(real);
	real Get_DefaultLimitOfRefraction(void) const;

	GTexture* GetTextureLimitOfRefraction(void) const	{	return texture_limit_of_refraction_;	}
	void SetTextureLimitOfRefraction(GTexture* t);
	bool HasLimitOfRefractionTexture(void)	{	return has_limit_of_refraction_texture_; 	}

	// Light Absorbance
	real GetLightAbsorbance(void) const;
	void SetLightAbsorbance(real);
	real Get_DefaultLightAbsorbance(void) const;

	GTexture* GetTextureLightAbsorbance(void) const	{	return texture_light_absorbance_;	}
	void SetTextureLightAbsorbance(GTexture* t);
	bool HasLightAbsorbanceTexture(void)	{	return has_light_absorbance_texture_; 	}

	// Surface Thickness
	real GetSurfaceThickness(void) const;
	void SetSurfaceThickness(real);
	real Get_DefaultSurfaceThickness(void) const;

	GTexture* GetTextureSurfaceThickness(void) const	{	return texture_surface_thickness_;	}
	void SetTextureSurfaceThickness(GTexture* t);
	bool HasSurfaceThicknessTexture(void)	{	return has_surface_thickness_texture_; 	}

	// Shadow Attenutation
	real GetShadowAttenutation(void) const;
	void SetShadowAttenutation(real);
	real Get_DefaultShadowAttenutation(void) const;

	GTexture* GetTextureShadowAttenutation(void) const	{	return texture_shadow_attenutation_;	}
	void SetTextureShadowAttenutation(GTexture* t);
	bool HasShadowAttenutationTexture(void)	{	return has_shadow_attenutation_texture_; 	}

	// ChromaticAberration
	bool GetChromaticAberration(void) const;
	void SetChromaticAberration(bool);

	// Reflection Limit
	real GetLimitOfReflection(void) const;
	void SetLimitOfReflection(real);
	real Get_DefaultLimitOfReflection(void) const;

	GTexture* GetTextureLimitOfReflection(void) const	{	return texture_limit_of_reflection_;	}
	void SetTextureLimitOfReflection(GTexture* t);
	bool HasLimitOfReflectionTexture(void)	{	return has_limit_of_reflection_texture_; 	}

	// Reflection Specularity
	real GetSpecularityReflection(void) const;
	void SetSpecularityReflection(real);
	real Get_DefaultSpecularityReflection(void) const;

	GTexture* GetTextureSpecularityReflection(void) const	{	return texture_specularity_reflection_;	}
	void SetTextureSpecularityReflection(GTexture* t);
	bool HasSpecularityReflectionTexture(void)	{	return has_specularity_reflection_texture_; 	}

	bool IsTransparent(void);

	bool IsMasking();

	virtual GTexture* GetUserTextureDiffuse(void) const { return (GTexture*)GetTextureDiffuse(); }

	virtual GTexture* GetUserTextureReflective(void) const { return (GTexture*)GetTextureReflective(); }

	virtual GTexture* GetUserTextureTransparent(void) const { return  (GTexture*)GetTextureTransparent(); }

	virtual GTexture* GetUserTextureBumpmapping(void) const { return (GTexture*)GetTextureBumpmapping(); }

	void SetId(long id) { id_ = id; }
	long GetId() const { return id_; }

	bool UserSpecial() const { return user_special_; }


	WString GetMaterialName() { return WString(material_name_.c_str()); }
	void SetMaterialName(const WString& fname) { material_name_.resize(fname.size()); for (unsigned int i = 0 ; i < fname.size() ; i++) material_name_[i] = fname[i]; }

	void SetDefaultMaterial(int type=0);

	bool operator == (const GMaterial& right) const;
private:

	long	id_;

	MaterialType	type_;					// NEW - Lambert, Phong, Blinn, Anisotropic

	color			diffuse_;				// Color
	GTexture*		texture_diffuse_;		// pointer to diffuse texture
	bool			has_diffuse_texture_;

	// Transparency 
	real			transparency_;			
	GTexture*		texture_transparency_;
	bool			has_transparency_texture_;
	
	// Transparent Color
	color			transparent_;			
	GTexture*		texture_transparent_;
	bool			has_transparent_texture_;

	//=====================================
	//color			translucency_;		
	//GTexture*		texture_transparency_;
	//bool			has_transparency_texture_;
	//=====================================

	color			ambient_;				// Ambient Color
	GTexture*		texture_ambient_;
	bool			has_ambient_texture_;

	color			emission_;				// emission
	GTexture*		texture_emission_;
	bool			has_emission_texture_;

	GTexture*		texture_bumpmapping_;		// Bump mapping
	bool			has_bumpmapping_texture_;
	
	real			translucence_;			// NEW - Translucence
	GTexture*		texture_translucence_;
	bool			has_translucence_texture_;

	real			translucence_depth_;	// NEW - Translucence Depth
	GTexture*		texture_translucence_depth_;
	bool			has_translucence_depth_texture_;

	real			translucence_focus_;	// NEW - Trnaslucence Focus
	GTexture*		texture_translucence_focus_;
	bool			has_translucence_focus_texture_;

	real			shininess_;				// Shininess - specular exponent
	GTexture*		texture_shininess_;
	bool			has_shininess_texture_;

	color			specular_;				// Specular Color
	GTexture*		texture_specular_;
	bool			has_specular_texture_;

	real			reflectivity_;			// Reflectivity
	GTexture*		texture_reflectivity_;
	bool			has_reflectivity_texture_;

	color			reflective_;			// Reflected Color
	GTexture*		texture_reflective_;
	bool			has_reflective_texture_;

	real			eccentricity_;			// NEW - Eccentricity
	GTexture*		texture_eccentricity_;
	bool			has_eccentricity_texture_;

	real			specularrolloff_;		// NEW - Specular Roll Off
	GTexture*		texture_specularrolloff_;
	bool			has_specularrolloff_texture_;

	real			angle_;					// NEW - Angle
	GTexture*		texture_angle_;
	bool			has_angle_texture_;

	real			spreadx_;				// NEW - Spread X
	GTexture*		texture_spreadx_;
	bool			has_spreadx_texture_;

	real			spready_;				// NEW - Spread y
	GTexture*		texture_spready_;
	bool			has_spready_texture_;

	real			roughness_;				// NEW - Roughness
	GTexture*		texture_roughness_;
	bool			has_roughness_texture_;

	real			fresnelindex_;			// NEW - Fresnel Index
	GTexture*		texture_fresnelindex_;
	bool			has_fresnelindex_texture_;

	real			index_of_refraction_;	// Refractive Index
	GTexture*		texture_index_of_refraction_;
	bool			has_index_of_refraction_texture_;

	real			limit_of_refraction_;	// NEW - Refraction Limit
	GTexture*		texture_limit_of_refraction_;
	bool			has_limit_of_refraction_texture_;

	real			light_absorbance_;		// NEW - Light Absorbance
	GTexture*		texture_light_absorbance_;
	bool			has_light_absorbance_texture_;

	real			surface_thickness_;		// NEW - Surface Thickness
	GTexture*		texture_surface_thickness_;
	bool			has_surface_thickness_texture_;

	real			shadow_attenutation_;	// NEW - Shadow Attenuation
	GTexture*		texture_shadow_attenutation_;
	bool			has_shadow_attenutation_texture_;

	bool			chromatic_aberration_;	// NEW - Chromatic Aberration

	real			limit_of_reflection_;	// NEW - Reflection Limit
	GTexture*		texture_limit_of_reflection_;
	bool			has_limit_of_reflection_texture_;

	real			specularity_reflection_;// NEW - Reflection Specularity
	GTexture*		texture_specularity_reflection_;
	bool			has_specularity_reflection_texture_;


	StringType material_name_;


	bool	user_special_;

	MaterialDatabase* connected_material_database_;
};
}
#endif