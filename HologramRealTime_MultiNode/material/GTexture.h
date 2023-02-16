#ifndef GTEXTURE_H_
#define GTEXTURE_H_

#include <model/kernel.h>

#include <graphics/gHash.h>

namespace model_kernel
{
	class TextureDatabase;

	class GTexture
	{
	public:

		GTexture(TextureDatabase* db, const wchar_t* file_name, WString& dae_id, real scale_x = 1.0, real scale_y = 1.0);
		GTexture(TextureDatabase* db, const wchar_t* file_name, real scale_x = 1.0, real scale_y = 1.0);
		GTexture(TextureDatabase* db, const wchar_t* file_name, unsigned int ogl_tid, int w, int h, real scale_x = 1.0, real scale_y = 1.0);
		void append(graphics::gHash& h) const;

		WString GetFileName(void) const;
		void SetFileName(const WString&);

		WString GetTextureName(void) const;
		void SetTextureName(const WString&);

		real GetScaleX(void) const;		
		void SetScaleX(real);

		real GetScaleY(void) const;
		void SetScaleY(real);

		int GetWidth(void) const;
		void SetWidth(int);

		int GetHeight(void) const;
		void SetHeight(int);

		unsigned int GetTextureId(void) const;

		void SetId(long id) { id_ = id; }
		long GetId() const { return id_; }		

		bool GetWrapS() const { return wrap_s_; }
		void SetWrapS(bool val) { wrap_s_ = val; }
		bool GetWrapT() const { return wrap_t_; }
		void SetWrapT(bool val) { wrap_t_ = val; }		

		bool HasAlphaChannel() { return true; }

		bool operator == (const GTexture& right) const;

		DECLARE_CLASS(GTexture);

	private:

		/** id within database. */
		long		 id_; 

		/** opengl texture id */
		unsigned int texture_id_;

		wchar_t		file_name_[500];
		StringType	texture_name_;

		real		scale_x_;
		real		scale_y_;

		bool		wrap_s_;
		bool		wrap_t_;

		int			width_;
		int			height_;

		TextureDatabase* connected_database_;

	};

}
#endif