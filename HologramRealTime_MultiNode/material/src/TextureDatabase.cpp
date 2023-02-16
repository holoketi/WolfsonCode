#include "material/TextureDatabase.h"

#include "material/GTexture.h"

namespace model_kernel
{
	TextureDatabase::TextureDatabase()
		: texture_table_()
		, texture_id_(0)
	{
	}

	GTexture* TextureDatabase::CreateTexture(const wchar_t* file_name)
	{
		GTexture *g = new GTexture(this, file_name);
		g->SetId(texture_id_);
		texture_table_[texture_id_] = g;
		texture_id_++;
		return g;
	}

	GTexture* TextureDatabase::CreateTexture(const wchar_t* file_name, unsigned int ogl_tid, int w, int h)
	{
		GTexture *g = new GTexture(this, file_name, ogl_tid, w, h);
		g->SetId(texture_id_);
		texture_table_[texture_id_] = g;
		texture_id_++;
		return g;
	}
	GTexture* TextureDatabase::CreateTexture(const wchar_t* file_name, WString& texture_name)
	{
		GTexture *g = new GTexture(this, file_name, texture_name);
		g->SetId(texture_id_);
		texture_table_[texture_id_] = g;
		texture_id_++;
		return g;
	}

	GTexture* TextureDatabase::FindByTextureName(WString& val)
	{
		TextureTable::iterator i = texture_table_.begin();
		for (; i != texture_table_.end(); i++) {
			if (i->second->GetTextureName() == val) return i->second;
		}

		return 0;
	}

	int	TextureDatabase::GetTextureCount() const
	{
		return texture_table_.size();
	}

	TextureTable::iterator TextureDatabase::BeginTexture()
	{
		return texture_table_.begin();
	}

	TextureTable::iterator TextureDatabase::EndTexture()
	{
		return texture_table_.end();
	}
}