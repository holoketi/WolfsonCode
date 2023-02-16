#ifndef MATERIAL_DATABASE_H_
#define MATERIAL_DATABASE_H_

#include <model/kernel.h>


class FCDMaterial;

namespace model_kernel
{	
	class TextureDatabase;
	class SceneGraph;
	class GMaterial;

	class MaterialDatabase 
	{
	public:
		friend class GMaterial;

	public:
		MaterialDatabase();

	public:

		DECLARE_CLASS(MaterialDatabase);

		static MaterialDatabase*   GetInstance();
		static TextureDatabase*   GetTextureDBInstance();
		GMaterial*			CreateMaterial(FCDMaterial* material);

		GMaterial*			CreateMaterial();
		GMaterial*			CreateMaterialWithTextureFile(WString& texture_file_name);

		bool				DeleteMaterial(GMaterial* );

		GMaterial*			FindByMaterialName(WString& val);
		GMaterial*			FindByMaterialId(const long&);
		GMaterial*			GetMaterial(int id);
		GMaterial*			GetDefaultMaterial() { return default_material_; }

		int					GetMaterialCount() const;

		MaterialTable::iterator	BeginMaterial();
		MaterialTable::iterator  EndMaterial();

		void				ClearDb();
		// material을 ID 변경없이 입력하려고 시도해본다. 성공하면 true를 반환하고 해당 id가 이미 사용중이라서 변경해야 한다면 false를 반환한다.		
		bool				AddMaterial(GMaterial*);
		void				RemoveMaterial(long);

	private:

		
		GMaterial*			default_material_;
		MaterialTable		material_table_;
		long				material_id_;

		static TextureDatabase*    texture_database_;
		static MaterialDatabase*   material_db_;
	};


	void InitializeMaterialDatabase();

}


#endif