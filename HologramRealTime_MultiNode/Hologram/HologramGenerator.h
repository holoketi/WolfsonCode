#ifndef __Hologram_Generator_h
#define __Hologram_Generator_h

#include <QtCore/QString>
#include <graphics/vec.h>
#include <vector>
#include <cufft.h>

#include "Hologram/fftw3.h"
#include "Hologram/complex.h"
#include "Hologram/GTensor.h"

#define MAX_GPU_ 8

#ifndef MAX_FLOAT
#define MAX_FLOAT	((float)3.40282347e+38)
#endif
#ifndef MAX_DOUBLE
#define MAX_DOUBLE	((double)1.7976931348623158e+308)
#endif
#ifndef MIN_FLOAT
#define MIN_FLOAT	((float)1.17549435e-38)
#endif
#ifndef MIN_DOUBLE
#define MIN_DOUBLE	((double)2.2250738585072014e-308)
#endif

//#define DEBUGMSG

using namespace graphics;

typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;
typedef long long longlong;
typedef unsigned long long ulonglong;

struct HologramConfig_PointCloud {

	double				offset_depth;
	graphics::vec2		pixel_pitch;
	graphics::ivec2		pixel_number;
	double				wave_length;
	HologramConfig_PointCloud():pixel_number(0) {}
};

struct HologramData_PointCloud {

	ulonglong			n_points;
	int					n_channels;
	float*				ObjPosition;
	float*				ObjIntensity;

	HologramData_PointCloud() :ObjPosition(nullptr), ObjIntensity(nullptr){ n_points = 0; n_channels = 0; }
};

class HologramGenerator {

public:

	HologramGenerator();
	~HologramGenerator();
	
	void setMode(bool isCPU) { isCPU_ = isCPU; }

	void setOffsetDepth(double depth) { config_PC_->offset_depth = depth; }
	void setPixelPitch(graphics::vec2 pp) { config_PC_->pixel_pitch = pp; }
	void setPixelNumber(graphics::ivec2 pn) { config_PC_->pixel_number = pn; }
	void setWaveLength(double wave) { config_PC_->wave_length = wave; }
	void setConfig_PointCloud(double depth, graphics::vec2 pp, graphics::ivec2 pn, double wave) {

		config_PC_->offset_depth = depth;
		config_PC_->pixel_pitch = pp;

		config_PC_->wave_length = wave;
		if (pn != config_PC_->pixel_number) {
			config_PC_->pixel_number = pn;
			initialize();
		}
	}

	void SetCurrentGPU(int id);

	bool isCPUMode() { return isCPU_; }

	double getOffsetDepth() {		return config_PC_->offset_depth;	}
	graphics::vec2 getPixelPitch() {	return config_PC_->pixel_pitch;	}
	graphics::ivec2 getPixelNumber() {	return config_PC_->pixel_number;	}
	double getWaveLength() {		return config_PC_->wave_length;	}
	HologramConfig_PointCloud* getConfig_PointCloud() {	return config_PC_;	}

	HologramData_PointCloud* getPCData() {	return data_PC_;	}
	Complex* getComplexField() { return complex_H_; }
	uchar* getNormalizedCGH() {		return holo_normalized_;	}
	float* getEncodedCGH() { return holo_encoded_GPU_host_; }
	float* getEncodedCGHR() { return holo_encoded_GPU_R_; }
	float* getEncodedCGHG() { return holo_encoded_GPU_G_; }
	float* getEncodedCGHB() { return holo_encoded_GPU_B_; }
	void getCGHfromGPU();
			
	void initialize();
	void generateHologram();

	void cudaMergeNCopytoComplex(int pnx, int pny, int num_gpu, float* src_real, float* src_imag, cufftComplex* dstR, cufftComplex* dstG, cufftComplex* dstB);

	void encoding_amplitude(Complex* src, double* dst, int nx, int ny);
	void encoding_ssb();
	//void readback_ssb();
	void readback_ssbR();
	void readback_ssbG();
	void readback_ssbB();
	void encodeSideBand_CPU(int cropx1, int cropx2, int cropy1, int cropy2, ivec2 sig_location);
	void encodeSideBand_GPU(int cropx1, int cropx2, int cropy1, int cropy2, ivec2 sig_location);

	void GetReal();

	void cudaCropFringe(int nx, int ny, cufftComplex* in_field, cufftComplex* out_field, int cropx1, int cropx2, int cropy1, int cropy2);
	void cudaFFT(int nx, int ny, cufftComplex* in_field, cufftComplex* output_field, int direction, bool bNormalized);
	void cudaGetFringe(int pnx, int pny, cufftComplex* in_field, cufftComplex* out_field, int sig_locationx, int sig_locationy,
		double ssx, double ssy, double ppx, double ppy, double PI);
	void cudaGetRealpart(int pnx, int pny, cufftComplex* in_field, float* out_field);

	void normalize_gpu();
	void normalize_cpu();

	void get_shift_phase_value(Complex& shift_phase_val, int idx, ivec2 sig_location);		// called from "encodeSideBand_CPU"

	// flipOption = 0 : no flip, 1: vertical flip, 2: horizontal flip
	int save(const char* fname, uint8_t bitsperpixel = 8, uchar* src = nullptr, uint px = 0, uint py = 0, int flipOption = 0);
	int saveAsImg(const char* fname, uint8_t bitsperpixel, uchar* src, int pic_width, int pic_height);
	
	void fresnelPropagation(Complex* in, Complex* out, double distance, uint nx = 0, uint ny = 0);
	void fftwShift(Complex* src, Complex* dst, int nx, int ny, int type, bool bNomalized = false);
	void exponent_complex(Complex* val);
	void fftShift(int nx, int ny, Complex* input, Complex* output);

	void saveComplexImage(QString realname, QString imgname);

	void clear_Var();
	void clear_GPU_Var();

	void writeIntensity_gray8_bmp(const char* fileName, int nx, int ny, Complex* complexvalue);
	void writeIntensity_gray8_img_bmp(const char* fileName, int nx, int ny, Complex* complexvalue);
	void writeIntensity_gray8_bmp(const char* fileName, int nx, int ny, double* intensity);
	void writeIntensity_gray8_bmp2(const char* fileName, int nx, int ny, float* intensity);

private:

	// Multi GPU Variables =============================================================
	int							num_gpu_;
	int							cur_gpu_;

	bool						isCPU_;						

	HologramConfig_PointCloud*	config_PC_;
	HologramData_PointCloud*	data_PC_;
	int							max_points_;


	//float*						holo_encoded_GPU_;
	float*						holo_encoded_GPU_R_;
	float*						holo_encoded_GPU_G_;
	float*						holo_encoded_GPU_B_;
    float*                      holo_encoded_GPU_host_;
	uchar*						holo_normalized_GPU_;
	//cufftComplex*				complex_H_GPU_;
	cufftComplex*				complex_H_GPU_R_;
	cufftComplex*				complex_H_GPU_G_;
	cufftComplex*				complex_H_GPU_B_;

	double*						holo_encoded_;
	uchar*						holo_normalized_;
	Complex*					complex_H_;


	// Point Cloud CPU variables ========================================================
	void	genCghPointCloudCPU();
	void	diffractNotEncodedRS(graphics::ivec2 pn, graphics::vec2 pp, graphics::vec2 ss, graphics::vec3 pc, double k, double amplitude, double lambda);


	// Point Cloud GPU variables ========================================================
	void	init_GPU();
	void	genCghPointCloudGPU();
	void	DeviceInitialize();
	void	point_sources_method_xy_plane_CUDA();
	//void	find_size(double& len_d, int& len, double pos_z, double pixelSize, double hcz, double Wavelength);
	void	find_size(double& len_d, int& len, double pos_z, double pixelSize, double hcz);//JH

	int*						core_cnt_;

	float*						cghFieldReal_GPU_;						//GPU Var - size = nx*ny*3*numofGPU
	float*						cghFieldImag_GPU_;						//GPU Var - size = nx*ny*3*numofGPU

	float*						save_a_d_[MAX_GPU_];					//GPU Var
	float*						save_b_d_[MAX_GPU_];					//GPU Var

	float*						obj_position_d_[MAX_GPU_];				//GPU Var
	float*						obj_intensity_d_[MAX_GPU_];				//GPU Var
	short*						obj_hologram_pos_[MAX_GPU_];			//GPU Var

	cufftComplex *		k_temp_d_;
	cufftComplex *		k_temp2_d_;
	
	GTensor *					reduce_source_;
	GTensor *					reduce_min_;
	GTensor *					reduce_max_;


};


#endif