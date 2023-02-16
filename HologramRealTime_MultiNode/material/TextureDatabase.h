#ifndef TEXTUREDATABASE_H_
#define TEXTUREDATABASE_H_

#include <model/kernel.h>

namespace model_kernel
{
	class SceneGraph;

	class TextureDatabase {

	public:
		friend class GTexture;
	public:
		TextureDatabase();

	public:

		DECLARE_CLASS(TextureDatabase);

		GTexture*			CreateTexture(const wchar_t* file_name);
		GTexture*			CreateTexture(const wchar_t* file_name, WString& texture_name);
		GTexture*           CreateTexture(const wchar_t* file_name, unsigned int ogl_tid, int w, int h);
		GTexture*			FindByTextureName(WString& val);

		int					GetTextureCount() const;

		TextureTable::iterator	BeginTexture();
		TextureTable::iterator  EndTexture();

	private:

		TextureTable		texture_table_;
		long				texture_id_;
	};
}

#endif