#include "material/MaterialDatabase.h"

#undef _UNICODE
#define NO_LIBXML
#include <FUtils/FUtils.h>
#include <FUtils\FUObject.h>

#include "material/GMaterial.h"
#include "material/TextureDatabase.h"

namespace model_kernel
{
	GMaterial* kDefaultMaterial;
	MaterialDatabase*   MaterialDatabase::material_db_ = 0;
	TextureDatabase*   MaterialDatabase::texture_database_ = 0; 
	
	void InitializeMaterialDatabase()
	{
		MaterialDatabase::GetInstance();
	}

	MaterialDatabase*   MaterialDatabase::GetInstance() 
	{
		if (!material_db_) {
			texture_database_ = new TextureDatabase();
			material_db_ = new MaterialDatabase();
		} return material_db_;
	}
	
	TextureDatabase*   MaterialDatabase::GetTextureDBInstance()
	{
		if (!material_db_) {
			texture_database_ = new TextureDatabase();
			material_db_ = new MaterialDatabase();
		} return texture_database_;
	}

	MaterialDatabase::MaterialDatabase()
		: material_table_()
		, material_id_(0)
	{
		default_material_ = CreateMaterial();
		default_material_->SetDefaultMaterial();
		kDefaultMaterial = default_material_;

	}

	GMaterial*	MaterialDatabase::CreateMaterial(FCDMaterial* material)
	{
		GMaterial *g = new GMaterial(this, texture_database_, material);
		g->SetId(material_id_);
		material_table_[material_id_] = g;
		material_id_++;
		return g;
	}

	GMaterial*	MaterialDatabase::CreateMaterial()
	{
		GMaterial *g = new GMaterial(this);
		g->SetId(material_id_);
		material_table_[material_id_] = g;
		material_id_++;
		return g;
	}
	GMaterial*	MaterialDatabase::CreateMaterialWithTextureFile(WString& texture_file_name)
	{
		GTexture* tx = texture_database_->FindByTextureName(texture_file_name);
		if (!tx) {
			fstring str = FUStringConversion::ToFString(texture_file_name.c_str());
			tx = texture_database_->CreateTexture(str.c_str());
		}
		GMaterial* g = new GMaterial(this, tx);
		g->SetId(material_id_);
		g->SetMaterialName(texture_file_name);
		material_table_[material_id_] = g;
		material_id_++;
		return g;
	}

	GMaterial*			MaterialDatabase::GetMaterial(int id)
	{
		MaterialTable::iterator i = material_table_.find(id);
		if (i == material_table_.end()) return 0;
		return i->second;
	}

	GMaterial*	MaterialDatabase::FindByMaterialName(WString& val)
	{
		MaterialTable::iterator i = material_table_.begin();
		for (; i != material_table_.end(); i++) {
			if (i->second->GetMaterialName() == val) return i->second;
		}

		return 0;
	}

	GMaterial* MaterialDatabase::FindByMaterialId(const long& id)
	{
		MaterialTable::iterator i = material_table_.begin();
		for (; i != material_table_.end(); i++) 
		{
			long material_id = i->second->GetId();
			if (material_id == id) 
				return i->second;
		}

		return 0;
	}


	int	MaterialDatabase::GetMaterialCount() const
	{
		return material_table_.size();
	}

	MaterialTable::iterator	MaterialDatabase::BeginMaterial()
	{
		return material_table_.begin();
	}

	MaterialTable::iterator  MaterialDatabase::EndMaterial()
	{
		return material_table_.end();
	}

	void MaterialDatabase::ClearDb()
	{
		MaterialTable::iterator i = material_table_.begin();
		for (; i != material_table_.end(); i++) 
		{
			GMaterial* material = i->second;
			if(material)
				delete material;
		}

		material_table_.clear();
		material_id_ = 0;
	}

	bool MaterialDatabase::AddMaterial(GMaterial* material)
	{
		// 무조건 추가한다.
		material->SetId(material_id_);
		material_table_[material_id_] = material;
		material_id_++;
		return false;
/*
		const bool first = material_id_ == 0 && material->GetId() == 0;

		MaterialTable::iterator iter = material_table_.find(material->GetId());
		const bool is_in_the_db = iter != material_table_.end();
		GMaterial *material_from_db(NULL);
		if(is_in_the_db)
		{
			material_from_db = (*iter).second;				
		}

		//db에 있는 material이 지금 삽입하려고 하는 material과 같은놈인지 검사한다. 같다면 삽입할 필요없다.
		if(is_in_the_db && *material_from_db == *material)
			return true;

		if(first || !is_in_the_db)
		{
			material_table_[material->GetId()] = material;
			if(material_id_ <= material->GetId())
				material_id_ = material->GetId() + 1;
			return true;
		}
		else
		{
			material->SetId(material_id_);
			material_table_[material_id_] = material;
			material_id_++;
			return false;
		}
*/
	}

	void MaterialDatabase::RemoveMaterial(long id)
	{
		material_table_.erase(id);
	}

	bool MaterialDatabase::DeleteMaterial(GMaterial* material)
	{
		if (!material)	return false;

		material_table_.erase(material->GetId());

		delete material;

		return true;

	}

}