#include "material/GTexture.h"

#ifndef _UNICODE
#define _UNICODE
#endif

#include <IL/il.h>
#include <IL/ilut.h>

//#undef _UNICODE

#include <string>
#include <model/kernel.h>
#include <graphics/color.h>

namespace model_kernel
{

	GTexture::GTexture(TextureDatabase* db, const wchar_t* file_name, WString& tname, real scale_x, real scale_y) : connected_database_(db), 
		texture_name_(tname.c_str()), scale_x_(scale_x), scale_y_(scale_y), wrap_s_(true), wrap_t_(true)
	{

		wcscpy(file_name_, file_name);

		// Create an image container in DevIL.
		ILuint imageId;

		ilInit();
		ilGenImages(1, &imageId);
		ilBindImage(imageId);

#ifdef __APPLE__
		if(ilLoadImage(fname.c_str()) == false)
#else if _WIN32
		if(ilLoadImage(file_name) == false)
#endif
		{
			ilDeleteImages(1, &imageId);
			LOG("cannot load texture file\n");

			return;
		}
		width_ = ilGetInteger(IL_IMAGE_WIDTH);
		height_ = ilGetInteger(IL_IMAGE_HEIGHT);
		ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

		{
			glGenTextures( 1, &texture_id_ );
			glBindTexture( GL_TEXTURE_2D, texture_id_ );

			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData() );
			//glTexImage2D( GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData() );
			glBindTexture( GL_TEXTURE_2D, 0 );
		}

		ilDeleteImages(1, &imageId);


	}



	GTexture::GTexture(TextureDatabase* db, const wchar_t* file_name, real scale_x, real scale_y) : connected_database_(db), 
		texture_name_(), scale_x_(scale_x), scale_y_(scale_y), wrap_s_(true), wrap_t_(true)
	{

		wcscpy(file_name_, file_name);
		// Create an image container in DevIL.
		ILuint imageId;

		ilInit();
		ilGenImages(1, &imageId);
		ilBindImage(imageId);

#ifdef __APPLE__
		if(ilLoadImage(fname.c_str()) == false)
#else if _WIN32
		if(ilLoadImage(file_name) == false)
#endif
		{
			ilDeleteImages(1, &imageId);
			LOG("cannot load texture file\n");

			return;
			//printf("error while loading image %s\n", file_name_.c_str());
		}
		width_ = ilGetInteger(IL_IMAGE_WIDTH);
		height_ = ilGetInteger(IL_IMAGE_HEIGHT);
		ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

		{
			glGenTextures( 1, &texture_id_ );
			glBindTexture( GL_TEXTURE_2D, texture_id_ );

			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData() );
			//glTexImage2D( GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData() );
			glBindTexture( GL_TEXTURE_2D, 0 );
		}

		ilDeleteImages(1, &imageId);


	}
	GTexture::GTexture(TextureDatabase* db, const wchar_t* file_name, unsigned int ogl_tid, int w, int h, real scale_x, real scale_y)
		:connected_database_(db), texture_id_(ogl_tid), 
		texture_name_(file_name), scale_x_(scale_x), scale_y_(scale_y), wrap_s_(true), wrap_t_(true), width_(w), height_(h)
	{
		wcscpy(file_name_, file_name);
	}
	void GTexture::append(graphics::gHash& h) const
	{
		h.append((void*)file_name_, wcslen(file_name_)*sizeof(wchar_t));
		h.append(scale_x_);
		h.append(scale_y_);
	}

	unsigned int GTexture::GetTextureId(void) const
	{
		return texture_id_;
	}

	WString GTexture::GetFileName(void) const
	{
		return WString(file_name_);
	}

	void GTexture::SetFileName(const WString& file_name)
	{		
		wcscpy(file_name_, file_name.c_str());		
	}

	WString GTexture::GetTextureName(void) const
	{
		return WString(texture_name_.c_str());
	}

	void GTexture::SetTextureName(const WString& texture_name)
	{		
		texture_name_.clear();
		texture_name_.append(texture_name.c_str(), texture_name.size());
	}

	real GTexture::GetScaleX(void) const
	{
		return scale_x_;
	}
	real GTexture::GetScaleY(void) const
	{
		return scale_y_;
	}

	void GTexture::SetScaleX(real d)
	{
		scale_x_ = d;
	}

	void GTexture::SetScaleY(real d)
	{
		scale_y_ = d;
	}

	int GTexture::GetWidth(void) const
	{
		return width_;
	}

	int GTexture::GetHeight(void) const
	{
		return height_;
	}

	void GTexture::SetWidth(int i)
	{
		width_ = i;
	}

	void GTexture::SetHeight(int i)
	{
		height_ = i;
	}

	bool GTexture::operator == (const GTexture& right) const
	{
		bool b = (WString(file_name_) == WString(right.file_name_));
		b &= (texture_name_ == right.texture_name_) ;
		b &= (scale_x_ == right.scale_x_) ;
		b &= (scale_y_ == right.scale_y_) ;
		b &= (wrap_s_ == right.wrap_s_) ;
		b &= (wrap_t_ == right.wrap_t_) ;
		b &= (width_ == right.width_);
		b &= (height_ == right.height_);

		return b;
	}

}