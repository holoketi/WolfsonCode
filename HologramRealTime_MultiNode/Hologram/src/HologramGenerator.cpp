#include	"Hologram/HologramGenerator.h"

#include	<windows.h>
#include	<fstream>
#include	<sstream>
#include	<random>
#include	<iomanip>
#include	<io.h>
#include	<direct.h>
#include	<chrono>
#include    "graphics/sys.h"
#include	"Hologram/Bitmap.h"
#include	<QtGui/QImage.h>

HologramGenerator::HologramGenerator()
{
	isCPU_ = false;

	config_PC_ = 0;
	data_PC_ = 0;

	config_PC_ = (HologramConfig_PointCloud*)new HologramConfig_PointCloud();
	data_PC_ = (HologramData_PointCloud*)new HologramData_PointCloud();

	complex_H_ = 0;
	holo_encoded_ = 0;
	holo_normalized_ = 0;

	//complex_H_GPU_ = 0;
	complex_H_GPU_R_ = 0;
	complex_H_GPU_G_ = 0;
	complex_H_GPU_B_ = 0;
	//holo_encoded_GPU_ = 0;
	holo_encoded_GPU_R_ = 0;
	holo_encoded_GPU_G_ = 0;
	holo_encoded_GPU_B_ = 0;
	holo_normalized_GPU_ = 0;
    holo_encoded_GPU_host_ = 0;
	for (int i = 0; i < MAX_GPU_; i++) {

		save_a_d_[i] = 0;
		save_b_d_[i] = 0;

		obj_position_d_[i] = 0;
		obj_intensity_d_[i] = 0;
		obj_hologram_pos_[i] = 0;

	}

	cghFieldReal_GPU_ = 0;
	cghFieldImag_GPU_ = 0;

	k_temp_d_ = 0;
	k_temp2_d_ = 0;

	reduce_source_ = 0;
	reduce_min_ = 0;
	reduce_max_ = 0;
	   	
	max_points_ = 0;
}

HologramGenerator::~HologramGenerator()
{

}

// not called yet.
void HologramGenerator::clear_Var()
{
	if (complex_H_)	free(complex_H_);
	complex_H_ = 0;

	if (holo_encoded_) free(holo_encoded_);
	holo_encoded_ = 0;

	if (holo_normalized_)	free(holo_normalized_);
	holo_normalized_ = 0;

    if (holo_encoded_GPU_host_) free(holo_encoded_GPU_host_);
    holo_encoded_GPU_host_ = 0;

	clear_GPU_Var();
}

void HologramGenerator::initialize()
{
	ivec2 pn = config_PC_->pixel_number;

	if (complex_H_)	free(complex_H_);
	complex_H_ = (Complex*)malloc(sizeof(Complex)*pn[0] * pn[1]);
	memset(complex_H_, 0.0, sizeof(Complex) *pn[0]*pn[1]);
	
	if (holo_encoded_)	free(holo_encoded_);
	holo_encoded_ = (double*)malloc(sizeof(double)*pn[0] * pn[1]);
	memset(holo_encoded_, 0.0, sizeof(double) *pn[0] * pn[1]);
	
	if (holo_normalized_)	free(holo_normalized_);
	holo_normalized_ = (uchar*)malloc(sizeof(uchar)*pn[0] * pn[1]);
	memset(holo_normalized_, 0, sizeof(uchar) *pn[0] * pn[1]);

    if (holo_encoded_GPU_host_)	free(holo_encoded_GPU_host_);
    holo_encoded_GPU_host_ = (float*)malloc(sizeof(float)*pn[0] * pn[1]);
    memset(holo_encoded_GPU_host_, 0, sizeof(float) *pn[0] * pn[1]);

	//if (holo_encoded_GPU_R_)	free(holo_encoded_GPU_R_);
	//holo_encoded_GPU_R_ = (float*)malloc(sizeof(float)*pn[0] * pn[1]);
	//memset(holo_encoded_GPU_R_, 0, sizeof(float) *pn[0] * pn[1]);
	//
	//if (holo_encoded_GPU_G_)	free(holo_encoded_GPU_G_);
	//holo_encoded_GPU_G_ = (float*)malloc(sizeof(float)*pn[0] * pn[1]);
	//memset(holo_encoded_GPU_G_, 0, sizeof(float) *pn[0] * pn[1]);
	//
	//if (holo_encoded_GPU_B_)	free(holo_encoded_GPU_B_);
	//holo_encoded_GPU_B_ = (float*)malloc(sizeof(float)*pn[0] * pn[1]);
	//memset(holo_encoded_GPU_B_, 0, sizeof(float) *pn[0] * pn[1]);

	init_GPU();

}

void HologramGenerator::generateHologram()
{
	ivec2 pn = config_PC_->pixel_number;
	memset(complex_H_, 0.0, sizeof(Complex) *pn[0] * pn[1]);
	genCghPointCloudGPU();
}


void HologramGenerator::encoding_amplitude(Complex* src, double* dst, int Nx, int Ny)
{
	for (int i = 0; i < Nx * Ny ; i++) {

		dst[i] = sqrt(src[i].a * src[i].a + src[i].b * src[i].b);

	}

	memcpy(holo_encoded_, dst, sizeof(double)*Nx*Ny);

}

void HologramGenerator::encoding_ssb()
{
	ivec2 pn = config_PC_->pixel_number;
	int pnx = pn[0];
	int pny = pn[1];

	ivec2 sig_location(0, 1);
	
	int cropx1, cropx2, cropx, cropy1, cropy2, cropy;
	if (sig_location[1] == 0) //Left or right half
	{
		cropy1 = 1;
		cropy2 = pny;

	}
	else {

		cropy = (int)floor((double)pny / 2.0);
		cropy1 = cropy - (int)floor((double)cropy / 2.0);
		cropy2 = cropy1 + cropy - 1;
	}

	if (sig_location[0] == 0) // Upper or lower half
	{
		cropx1 = 1;
		cropx2 = pnx;

	}
	else {

		cropx = (int)floor((double)pnx / 2.0);
		cropx1 = cropx - (int)floor((double)cropx / 2.0);
		cropx2 = cropx1 + cropx - 1;
	}

	cropx1 -= 1;
	cropx2 -= 1;
	cropy1 -= 1;
	cropy2 -= 1;

	encodeSideBand_GPU(cropx1, cropx2, cropy1, cropy2, sig_location);  
}

void HologramGenerator::normalize_cpu()
{
	ivec2 pn = config_PC_->pixel_number;
	int nx = pn[0];
	int ny = pn[1];

	memset(holo_normalized_, 0.0, sizeof(uchar)*nx*ny);

	double* src = holo_encoded_;
	uchar*  dst = holo_normalized_;

	double min_val, max_val;
	min_val = src[0];
	max_val = src[0];
	for (int i = 0; i < nx*ny; ++i)
	{
		if (min_val > src[i])
			min_val = src[i];
		else if (max_val < src[i])
			max_val = src[i];
	}

	for (int k = 0; k < nx*ny; k++)
		dst[k] = (uint)((src[k] - min_val) / (max_val - min_val)*255.0 );

}

int HologramGenerator::save(const char * fname, uint8_t bitsperpixel, uchar* src, uint px, uint py, int flipOption)
{
	if (fname == nullptr) return -1;

	uchar* source = src;
	ivec2 p(px, py);

	if (src == nullptr)
		source = holo_normalized_;

	graphics::ivec2 pn = config_PC_->pixel_number;
	if (px == 0 && py == 0)
		p = ivec2(pn[0], pn[1]);

	QImage img(source, px, py, px, QImage::Format::Format_Grayscale8);
	if (flipOption)
		img = img.mirrored(flipOption==2, flipOption==1);

	return img.save(fname, "jpg");
		
	//return saveAsImg(fname, bitsperpixel, source, p[0], p[1]);
}

int HologramGenerator::saveAsImg(const char * fname, uint8_t bitsperpixel, uchar* src, int pic_width, int pic_height)
{
#ifdef DEBUGMSG
	LOG("Saving...%s...", fname);
#endif
	auto start = std::chrono::system_clock::now();

	int _width = pic_width, _height = pic_height;

	int _pixelbytesize = _height * _width * bitsperpixel / 8;
	int _filesize = _pixelbytesize + sizeof(bitmap);

	FILE *fp;
	fopen_s(&fp, fname, "wb");
	if (fp == nullptr) return -1;

	bitmap *pbitmap = (bitmap*)calloc(1, sizeof(bitmap));
	memset(pbitmap, 0.0, sizeof(bitmap));

	pbitmap->fileheader.signature[0] = 'B';
	pbitmap->fileheader.signature[1] = 'M';
	pbitmap->fileheader.filesize = _filesize;
	pbitmap->fileheader.fileoffset_to_pixelarray = sizeof(bitmap);

	for (int i = 0; i < 256; i++) {
		pbitmap->rgbquad[i].rgbBlue = i;
		pbitmap->rgbquad[i].rgbGreen = i;
		pbitmap->rgbquad[i].rgbRed = i;
	}

	pbitmap->bitmapinfoheader.dibheadersize = sizeof(bitmapinfoheader);
	pbitmap->bitmapinfoheader.width = _width;
	pbitmap->bitmapinfoheader.height = _height;
	pbitmap->bitmapinfoheader.planes = 1;
	pbitmap->bitmapinfoheader.bitsperpixel = bitsperpixel;
	pbitmap->bitmapinfoheader.compression = 0;
	pbitmap->bitmapinfoheader.imagesize = _pixelbytesize;
	pbitmap->bitmapinfoheader.ypixelpermeter = 0x130B;
	pbitmap->bitmapinfoheader.xpixelpermeter = 0x130B;
	pbitmap->bitmapinfoheader.numcolorspallette = 256;
	fwrite(pbitmap, 1, sizeof(bitmap), fp);

	fwrite(src, 1, _pixelbytesize, fp);
	fclose(fp);
	free(pbitmap);

	auto end = std::chrono::system_clock::now();

	auto during = ((std::chrono::duration<double>)(end - start)).count();

#ifdef DEBUGMSG
	LOG("%.5lfsec...done\n", during);
#endif

	return 1;
}


void HologramGenerator::saveComplexImage(QString realname, QString imgname)
{
	if (!complex_H_)	return;

	ivec2 pn = config_PC_->pixel_number;
	int pnx = pn[0];
	int pny = pn[1];

	uchar* realpart = (uchar*)malloc(pnx*pny * sizeof(uchar));
	uchar* imgpart = (uchar*)malloc(pnx*pny * sizeof(uchar));
	memset(realpart, 0.0, sizeof(uchar)*pnx*pny);
	memset(imgpart, 0.0, sizeof(uchar)*pnx*pny);
	   	 
	double min_real, max_real, min_img, max_img;
	min_real = complex_H_[0].a;
	max_real = complex_H_[0].a;
	min_img = complex_H_[0].b;
	max_img = complex_H_[0].b;

	for (int i = 0; i < pnx*pny; ++i)
	{
		if (min_real > complex_H_[i].a)
			min_real = complex_H_[i].a;
		else if (max_real < complex_H_[i].a)
			max_real = complex_H_[i].a;

		if (min_img > complex_H_[i].b)
			min_img = complex_H_[i].b;
		else if (max_img < complex_H_[i].b)
			max_img = complex_H_[i].b;
	}

	for (int k = 0; k < pnx*pny; k++) {

		realpart[k] = (uchar)((complex_H_[k].a - min_real) / (max_real - min_real)*255.0);
		imgpart[k] = (uchar)((complex_H_[k].b - min_img) / (max_img - min_img)*255.0);

	}
		
	QImage img(realpart, pnx, pny, pnx, QImage::Format::Format_Grayscale8);
	img = img.mirrored(false, true);
	img.save(realname, "jpg");
	QImage img2(imgpart, pnx, pny, pnx, QImage::Format::Format_Grayscale8);
	img2 = img2.mirrored(false, true);
	img2.save(imgname, "jpg");
	
	free(realpart);
	free(imgpart);

}
void HologramGenerator::writeIntensity_gray8_bmp2(const char* fileName, int nx, int ny, float* intensity)
{
	const int n = nx * ny;

	float min_val, max_val;
	min_val = intensity[0];
	max_val = intensity[0];

	for (int i = 0; i < n; ++i)
	{
		if (min_val > intensity[i])
			min_val = intensity[i];
		else if (max_val < intensity[i])
			max_val = intensity[i];
	}

	char fname[100];
	strcpy(fname, fileName);
	//strcat(fname, ".bmp");

	//LOG("minval %f, max val %f\n", min_val, max_val);
	unsigned char* cgh = (unsigned char*)malloc(sizeof(unsigned char)*n);

	for (int i = 0; i < n; ++i) {
		float val = 255.0 * ((intensity[i] - min_val) / (max_val - min_val));
		cgh[i] = val;
	}

	QImage img(cgh, nx, ny, nx, QImage::Format::Format_Grayscale8);
	img.save(QString(fname), "jpg");

	free(cgh);
}


void HologramGenerator::writeIntensity_gray8_bmp(const char* fileName, int nx, int ny, double* intensity)
{
	const int n = nx*ny;

	double min_val, max_val;
	min_val = intensity[0];
	max_val = intensity[0];

	for (int i = 0; i < n; ++i)
	{
		if (min_val > intensity[i])
			min_val = intensity[i];
		else if (max_val < intensity[i])
			max_val = intensity[i];
	}

	char fname[100];
	strcpy(fname, fileName);
	//strcat(fname, ".bmp");

	//LOG("minval %f, max val %f\n", min_val, max_val);
	unsigned char* cgh = (unsigned char*)malloc(sizeof(unsigned char)*n);

	for (int i = 0; i < n; ++i){
		double val = 255.0 * ((intensity[i] - min_val) / (max_val - min_val));
		cgh[i] = val;
	}

	QImage img(cgh, nx, ny, nx, QImage::Format::Format_Grayscale8);
	img.save(QString(fname), "jpg");

	free(cgh);
}




void HologramGenerator::writeIntensity_gray8_bmp(const char* fileName, int nx, int ny, Complex* complexvalue)
{
	const int n = nx*ny;

	double* intensity = (double*)malloc(sizeof(double)*n);
	for (int i = 0; i < n; i++)
		intensity[i] = complexvalue[i].a;
		//intensity[i] = complexvalue[i].mag2();

	double min_val, max_val;
	min_val = intensity[0];
	max_val = intensity[0];

	for (int i = 0; i < n; ++i)
	{
		if (min_val > intensity[i])
			min_val = intensity[i];
		else if (max_val < intensity[i])
			max_val = intensity[i];
	}

	char fname[100];
	strcpy(fname, fileName);
	//strcat(fname, ".bmp");

	//LOG("minval %e, max val %e\n", min_val, max_val);

	unsigned char* cgh = (unsigned char*)malloc(sizeof(unsigned char)*n);

	for (int i = 0; i < n; ++i) {
		double val = (intensity[i] - min_val) / (max_val - min_val);
		//val = pow(val, 1.0 / 1.5);
		val = val * 255.0;
		unsigned char v = (uchar)val;

		cgh[i] = v;
	}

	QImage img(cgh, nx, ny, nx, QImage::Format::Format_Grayscale8);
	img.save(QString(fname), "jpg");


	free(intensity);
	free(cgh);
}
void HologramGenerator::writeIntensity_gray8_img_bmp(const char* fileName, int nx, int ny, Complex* complexvalue)
{
	const int n = nx * ny;

	double* intensity = (double*)malloc(sizeof(double)*n);
	for (int i = 0; i < n; i++)
		intensity[i] = complexvalue[i].b;
	//intensity[i] = complexvalue[i].mag2();

	double min_val, max_val;
	min_val = intensity[0];
	max_val = intensity[0];

	for (int i = 0; i < n; ++i)
	{
		if (min_val > intensity[i])
			min_val = intensity[i];
		else if (max_val < intensity[i])
			max_val = intensity[i];
	}

	char fname[100];
	strcpy(fname, fileName);
	//strcat(fname, ".bmp");

	//LOG("minval %e, max val %e\n", min_val, max_val);

	unsigned char* cgh = (unsigned char*)malloc(sizeof(unsigned char)*n);

	for (int i = 0; i < n; ++i) {
		double val = (intensity[i] - min_val) / (max_val - min_val);
		//val = pow(val, 1.0 / 1.5);
		val = val * 255.0;
		unsigned char v = (uchar)val;

		cgh[i] = v;
	}

	QImage img(cgh, nx, ny, QImage::Format::Format_Grayscale8);
	img.save(QString(fname), "jpg");


	free(intensity);
	free(cgh);
}
