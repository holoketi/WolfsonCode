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
		// material�� ID ������� �Է��Ϸ��� �õ��غ���. �����ϸ� true�� ��ȯ�ϰ� �ش� id�� �̹� ������̶� �����ؾ� �Ѵٸ� false�� ��ȯ�Ѵ�.		
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