#include <cuda_runtime.h>
#include <cuComplex.h>
#include <cuda.h>
#include <vector>
#include <cufft.h>
#include <device_launch_parameters.h>
#include <device_functions.h>

#include "Hologram/HologramGenerator_GPU.h"
#include "graphics/sys.h"

#include <thrust/complex.h>
#include <curand_kernel.h>

//cudaEvent_t start, stop;

__device__ float clamp(float v, float a, float b)
{
	return v < a ? a : (v > b ? b : v);
}

__global__ void grey_normalize(float* src, uchar* dst, float* min_v, float* max_v,int nx, int ny)
{
	int tid = threadIdx.x + blockIdx.x*blockDim.x;
	int wh = nx * ny;
	if (tid < wh) {
		dst[tid] = unsigned char(clamp((src[tid] - min_v[0]) / (max_v[0] - min_v[0] + 0.000001)*255.0, 0.0, 255.0));
	}
}

__device__  void exponent_complex_gen(cuComplex* val)
{
	float exp_val = exp(val->x);
	float cos_v;
	float sin_v;
	sincos(val->y, &sin_v, &cos_v);

	val->x = exp_val * cos_v;
	val->y = exp_val * sin_v;

}

__global__ void cropFringe(int nx, int ny, cufftComplex* in_filed, cufftComplex* out_filed, int cropx1, int cropx2, int cropy1, int cropy2)
{
	int tid = threadIdx.x + blockIdx.x*blockDim.x;

	if (tid < nx*ny)
	{
		int x = tid % nx;
		int y = tid / nx;

		if (x >= cropx1 && x <= cropx2 && y >= cropy1 && y <= cropy2)
			out_filed[tid] = in_filed[tid];
	}
}

__global__ void kernel_fftShift(int N, int nx, int ny, cufftComplex* input, cufftComplex* output, bool bNormailzed)
{
	int tid = threadIdx.x + blockIdx.x*blockDim.x;

	double normalF = 1.0;
	if (bNormailzed == true)
		normalF = nx * ny;

	while (tid < N)
	{
		int i = tid % nx;
		int j = tid / nx;

		int ti = i - nx / 2; if (ti < 0) ti += nx;
		int tj = j - ny / 2; if (tj < 0) tj += ny;

		int oindex = tj * nx + ti;


		output[tid].x = input[oindex].x / normalF;
		output[tid].y = input[oindex].y / normalF;

		tid += blockDim.x * gridDim.x;
	}
}

__global__ void getFringe(int nx, int ny, cufftComplex* in_filed, cufftComplex* out_filed, int sig_locationx, int sig_locationy,
	double ssx, double ssy, double ppx, double ppy, double pi)
{
	int tid = threadIdx.x + blockIdx.x*blockDim.x;

	if (tid < nx*ny)
	{
		cufftComplex shift_phase = make_cuComplex(1, 0);

		if (sig_locationy != 0)
		{
			int r = tid / nx;
			double yy = (ssy / 2.0) - (ppy)*(double)r - ppy;

			cufftComplex val = make_cuComplex(0, 0);
			if (sig_locationy == 1)
				val.y = 2.0 * pi * (yy / (4.0 * ppy));
			else
				val.y = 2.0 * pi * (-yy / (4.0 * ppy));

			exponent_complex_gen(&val);

			shift_phase = cuCmulf(shift_phase, val);
		}

		if (sig_locationx != 0)
		{
			int c = tid % nx;
			double xx = (-ssx / 2.0) - (ppx)*(double)c - ppx;

			cufftComplex val = make_cuComplex(0, 0);
			if (sig_locationx == -1)
				val.y = 2.0 * pi * (-xx / (4.0 * ppx));
			else
				val.y = 2.0 * pi * (xx / (4.0 * ppx));

			exponent_complex_gen(&val);
			shift_phase = cuCmulf(shift_phase, val);
		}

		out_filed[tid] = cuCmulf(in_filed[tid], shift_phase);
	}

}

__global__ void getRealPart(int nx, int ny, cufftComplex* in_filed, float* out_filed)
{
	int tid = threadIdx.x + blockIdx.x*blockDim.x;

	if (tid < nx*ny)
		out_filed[tid] = (float)in_filed[tid].x;
}

__global__ void kernel_Merge(int nx, int ny, int num_gpu, float* src_real, float* src_imag)
{

	int tid = threadIdx.x + blockIdx.x*blockDim.x;

	if (tid >= nx * ny)	return;

	for (int i = 1; i < num_gpu; i++)
	{
		float* ptr_real = src_real + (nx*ny * 3 * i);
		float* ptr_img = src_imag + (nx*ny * 3 * i);

		atomicAdd(&src_real[tid * 3], ptr_real[tid * 3]);
		atomicAdd(&src_real[tid * 3 + 1], ptr_real[tid * 3 + 1]);
		atomicAdd(&src_real[tid * 3 + 2], ptr_real[tid * 3 + 2]);

		atomicAdd(&src_imag[tid * 3], ptr_img[tid * 3]);
		atomicAdd(&src_imag[tid * 3 + 1], ptr_img[tid * 3 + 1]);
		atomicAdd(&src_imag[tid * 3 + 2], ptr_img[tid * 3 + 2]);

	}


}

__global__ void kernel_CopytoComplex(int nx, int ny, float* src_real, float* src_img, cufftComplex* dstR, cufftComplex* dstG, cufftComplex* dstB)
{
	int tid = threadIdx.x + blockIdx.x*blockDim.x;

	if (tid >= nx * ny)	return;

	dstR[tid].x = src_real[tid * 3];
	dstR[tid].y = src_img[tid * 3];

	dstG[tid].x = src_real[tid * 3+1];
	dstG[tid].y = src_img[tid * 3+1];

	dstB[tid].x = src_real[tid * 3+2];
	dstB[tid].y = src_img[tid * 3+2];

}




//__global__ void set_subhologram_position(int num_of_point, short* dev_mem, float* position_data, float len_d,
//	int nx, int ny, float pixelSize, float hcx, float hcy, float hcz)
//{
//	int tid = threadIdx.x + blockIdx.x*blockDim.x;
//	if (tid < num_of_point) {
//		float start_x_d = position_data[tid * 3] - len_d;
//		float start_y_d = position_data[tid * 3 + 1] - len_d;
//
//		dev_mem[tid * 2 + 0] = ceil((1 / pixelSize)*(start_x_d - hcx + 0.5*nx*pixelSize));
//		dev_mem[tid * 2 + 1] = ceil((1 / pixelSize)*(start_y_d - hcy + 0.5*ny*pixelSize));
//	}
//}

__global__ void set_subhologram_position(int num_of_point, short* dev_mem, float* position_data, float len_d,
	int nx, int ny, float pixelSize, float hcx, float hcy, float hcz)
{
	int tid = threadIdx.x + blockIdx.x*blockDim.x;
	if (tid < num_of_point) {
		float start_x_d = position_data[tid * 3] - len_d;
		float start_y_d = position_data[tid * 3 + 1] - len_d;

		dev_mem[tid * 2 + 0] = ceil((1 / pixelSize)*(/*start_x_d*/-len_d - hcx + 0.5*nx*pixelSize));
		dev_mem[tid * 2 + 1] = ceil((1 / pixelSize)*(/*start_y_d*/-len_d - hcy + 0.5*ny*pixelSize));
	}
}

//__global__ void point_sources_kernel_dp(int len, short*  start_pos, float* real_part_hologram, float* imagery_part_hologram,
//	float* pos, float* o_intensity, int nx_s, int ny_s, float pixelSize, float TWO_PI_DP, float waveLength, int pi)
//{
//	int tid = threadIdx.x + blockIdx.x*blockDim.x;
//	int which_hologram = blockIdx.y;
//
//	if (tid >= len * len) return;
//	//if (tid >= nx_s * ny_s) return;
//
//	int pi_index = pi + which_hologram;
//	int i, j;
//	i = start_pos[pi_index * 2] + tid % len;
//	j = start_pos[pi_index * 2 + 1] + tid / len;
//	//i = tid % nx_s;
//	//j = tid / nx_s;
//
//	if (i < 0 || i >= nx_s || j < 0 || j >= ny_s) return;
//
//	float x, y, z;
//	//x = (i - 1) * pixelSize - 0.5*nx_s*pixelSize;
//	//y = (ny_s - j) * pixelSize - 0.5*ny_s*pixelSize;
//
//	float x0 = 0, y0 = 0, z0 = 0;
//	x = i * pixelSize - 0.5*nx_s*pixelSize + x0;
//	y = j * pixelSize - 0.5*ny_s*pixelSize + y0;
//	z = z0;
//
//	float pcx = pos[pi_index * 3];
//	float pcy = pos[pi_index * 3 + 1];
//	float pcz = pos[pi_index * 3 + 2];
//
//	//=== check if the position z is a effective point ===================================
//	/*
//	float len_ccp, len_cp;
//
//	float n = sqrt((x - pcx)*(x - pcx) + (y - pcy)*(y - pcy));
//	float unitx = (x - pcx) / n;
//	float unity = (y - pcy) / n;
//	float ccx = x + pixelSize * unitx;
//	float ccy = y + pixelSize * unity;
//
//	float l = sqrt((x - pcx)*(x - pcx) + (y - pcy)*(y - pcy));
//	len_cp = sqrt(pcz*pcz + l * l);
//
//	float ll = sqrt((ccx - pcx)*(ccx - pcx) + (ccy - pcy)*(ccy - pcy));
//	len_ccp = sqrt(pcz*pcz + ll * ll);
//
//	if (abs(len_ccp - len_cp) > waveLength) {
//		return;
//	}*/
//
//	//=================================================================================
//	
//	float3 val;
//	val.x = o_intensity[pi_index * 3 + 0] / waveLength;
//	val.y = o_intensity[pi_index * 3 + 1] / waveLength;
//	val.z = o_intensity[pi_index * 3 + 2] / waveLength;
//
//	float oz = (int)(pcz / waveLength) * (waveLength);
//	float rx = x - pcx;
//	float ry = y - pcy;
//	float rz = z - oz;
//	const float l2 = sqrtf(rx*rx + ry*ry + rz*rz);
//
//	float sval = (float)(TWO_PI_DP / waveLength * l2);
//	float cos_v = cos(sval);
//	float sin_v = sin(sval);
//
//	int index = (j * nx_s * 3 + i * 3);
//
//	atomicAdd(&real_part_hologram[index], (val.x * cos_v));
//	atomicAdd(&real_part_hologram[index + 1], (val.y * cos_v));
//	atomicAdd(&real_part_hologram[index + 2], (val.z * cos_v));
//
//	atomicAdd(&imagery_part_hologram[index], (-val.x * sin_v));
//	atomicAdd(&imagery_part_hologram[index + 1], (-val.y * sin_v));
//	atomicAdd(&imagery_part_hologram[index + 2], (-val.z * sin_v));
//	
//}

__global__ void point_sources_kernel_dp(int len, short*  start_pos, float* real_part_hologram, float* imagery_part_hologram,
	float* pos, float* o_intensity, int nx_s, int ny_s, float pixelSize, float TWO_PI_DP, /*double* waveLength,*/ int pi)//JS
{
	int tid = threadIdx.x + blockIdx.x*blockDim.x;
	int which_hologram = blockIdx.y;

	if (tid >= len * len) return;
	//if (tid >= nx_s * ny_s) return;

	int pi_index = pi + which_hologram;
	int i, j;//i,j는 픽셀인덱스같음
	i = start_pos[pi_index * 2] + tid % len;
	j = start_pos[pi_index * 2 + 1] + tid / len;
	//i = tid % nx_s;
	//j = tid / nx_s;

	if (i < 0 || i >= nx_s || j < 0 || j >= ny_s) return;

	//float x0 = pixelSize / 2, y0 = pixelSize / 2, z0 = 0;
	float xx = 0, yy = 0;// , z0 = 0;
	xx = i * pixelSize - 0.5*nx_s*pixelSize;
	yy = j * pixelSize - 0.5*ny_s*pixelSize;//플립?아직안함
	//zx = z0;
	float waveLength[3] = { 660 * 1e-9 ,532 * 1e-9 ,457 * 1e-9 };

	float x = pos[pi_index * 3];
	float y = pos[pi_index * 3 + 1];
	float z = pos[pi_index * 3 + 2];

	float k;

	float p_A;
	float p_phi=0;// = 2 * pi / waveLength[0] * z;
	float p_xyz;
	//float r = sqrt(pow(x - xx, 2) + pow(y - yy, 2) + pow(z, 2));

	int index = ((nx_s - j - 1) * nx_s * 3 + i * 3);//UDflip

	for (int i = 0; i < 3; i++)
	{

		if (abs(x - xx) / (waveLength[i] * z) < (float)1 / ((float)2 * pixelSize) && abs(y - yy) / (waveLength[i] * z) < (float)1 / ((float)4 * pixelSize))//왜 한쪽은 2 한쪽은 4지
		{
			k = TWO_PI_DP / waveLength[i];
			//p_phi = k * r;
			p_A= o_intensity[pi_index * 3 + i]*cos(k*((pow(x - xx, 2) + pow(y - yy, 2)) / ((float)2 * z) + waveLength[i] * yy / ((float)4 * pixelSize)) + p_phi);
			atomicAdd(&real_part_hologram[index + i], p_A);
			//atomicAdd(&imagery_part_hologram[index + i], (CGH.imag()));
		}
	}
	/*
	float xa, ya, za;
	//x = (i - 1) * pixelSize - 0.5*nx_s*pixelSize;-
	//y = (ny_s - j) * pixelSize - 0.5*ny_s*pixelSize;

	float x0 = pixelSize / 2, y0 = pixelSize / 2, z0 = 0;
	//float x0 = 0, y0 = 0, z0 = 0;
	xa = i * pixelSize - 0.5*nx_s*pixelSize + x0;
	ya = j * pixelSize - 0.5*ny_s*pixelSize + y0;
	za = z0;

	float x = pos[pi_index * 3];
	float y = pos[pi_index * 3 + 1];
	float z = pos[pi_index * 3 + 2];

	//float nm = 1e-9;
	//float um = 1e-6;
	//float mm = 1e-3;
	//float cm = 1e-2;

	//norm_v = [0 0 - 1];

	float djh = 0 * 1e-2;

	float dx = pixelSize; //일단 임시로 x픽셀피치를 둘다적용
	float dy = pixelSize;

	float X_Shift = 0;
	//float Y_Shift = 0.5;
	float Y_Shift = -0.5;

	//thrust::complex<float> tmp(0, 0);
	thrust::complex<float> imgI(0, 1);
	//cuDoubleComplex imgI2 = make_cuDoubleComplex(0, 2);
	curandState cs;
	curand_init(pi_index, 0, 0, &cs);
	float rand = curand_uniform(&cs);
	float B = -1;
	float R;
	thrust::complex<float> i2PI = imgI * TWO_PI_DP;
	thrust::complex<float> A;
	thrust::complex<float> CGH;
	//rand = dis(gen);
	//int index = (j * nx_s * 3 + i * 3);
	int index = ((nx_s - j - 1) * nx_s * 3 + i * 3);//UDflip
	//int index = (j * nx_s * 3 + (nx_s-i-1) * 3);//LRflip

	//float Rwl = 660 * nm;
	//float Gwl = 532 * nm;
	//float Bwl = 457 * nm;

	//float lambda = Rwl; //wavelength같은것
	float waveLength[3] = { 660 * 1e-9 ,532 * 1e-9 ,457 * 1e-9 };

	float theta_x = asin((waveLength[2] / (2 * dx))); // diffraction angle
	float theta_y = asin((waveLength[2] / (2 * dy)));

	float drx = abs(djh - z)*tan(theta_x);
	float dry = abs(djh - z)*tan(theta_y);

	if (pow((xa - x) / drx, 2) + pow((ya - y) / dry, 2) <= 1)
	{
		R = (-z / abs(z))*sqrt(pow(djh - z, 2) + pow(xa - x, 2) + pow(ya - y, 2));
		for (int i = 0; i < 3; i++)
		{
			A = exp(i2PI*rand)*exp(i2PI*R / waveLength[i]) / R * o_intensity[pi_index * 3 + i];//intensity는 A에 상수값으로 곱해준다 색깔별로
			CGH = A * B*(dx*dy) / (imgI*waveLength[i])*exp(i2PI*(xa / (2 * dx)*X_Shift*(waveLength[2] / waveLength[i]) + ya / (2 * dy)*Y_Shift*(waveLength[2] / waveLength[i])));
			atomicAdd(&real_part_hologram[index + i], (CGH.real()));
			atomicAdd(&imagery_part_hologram[index + i], (CGH.imag()));
		}
	}*/
}

//__global__ void point_sources_kernel_dp(int len, short*  start_pos, float* real_part_hologram, float* imagery_part_hologram,
//	float* pos, float* o_intensity, int nx_s, int ny_s, float pixelSize, float TWO_PI_DP, /*double* waveLength,*/ int pi)//JH
//{
//	int tid = threadIdx.x + blockIdx.x*blockDim.x;
//	int which_hologram = blockIdx.y;
//
//	if (tid >= len * len) return;
//	//if (tid >= nx_s * ny_s) return;
//
//	int pi_index = pi + which_hologram;
//	int i, j;//i,j는 픽셀인덱스같음
//	i = start_pos[pi_index * 2] + tid % len;
//	j = start_pos[pi_index * 2 + 1] + tid / len;
//	//i = tid % nx_s;
//	//j = tid / nx_s;
//
//	if (i < 0 || i >= nx_s || j < 0 || j >= ny_s) return;
//
//	float xa, ya, za;
//	//x = (i - 1) * pixelSize - 0.5*nx_s*pixelSize;-
//	//y = (ny_s - j) * pixelSize - 0.5*ny_s*pixelSize;
//
//	float x0 = pixelSize / 2, y0 = pixelSize / 2, z0 = 0;
//	//float x0 = 0, y0 = 0, z0 = 0;
//	xa = i * pixelSize - 0.5*nx_s*pixelSize + x0;
//	ya = j * pixelSize - 0.5*ny_s*pixelSize + y0;
//	za = z0;
//
//	float x = pos[pi_index * 3];
//	float y = pos[pi_index * 3 + 1];
//	float z = pos[pi_index * 3 + 2];
//
//	//float nm = 1e-9;
//	//float um = 1e-6;
//	//float mm = 1e-3;
//	//float cm = 1e-2;
//
//	//norm_v = [0 0 - 1];
//
//	float djh = 0 * 1e-2;
//
//	float dx = pixelSize; //일단 임시로 x픽셀피치를 둘다적용
//	float dy = pixelSize;
//
//	float X_Shift = 0;
//	//float Y_Shift = 0.5;
//	float Y_Shift = -0.5;
//
//	//thrust::complex<float> tmp(0, 0);
//	thrust::complex<float> imgI(0, 1);
//	//cuDoubleComplex imgI2 = make_cuDoubleComplex(0, 2);
//	curandState cs;
//	curand_init(pi_index, 0, 0, &cs);
//	float rand = curand_uniform(&cs);
//	float B = -1;
//	float R;
//	thrust::complex<float> i2PI = imgI * TWO_PI_DP;
//	thrust::complex<float> A;
//	thrust::complex<float> CGH;
//	//rand = dis(gen);
//	//int index = (j * nx_s * 3 + i * 3);
//	int index = ((nx_s - j - 1) * nx_s * 3 + i * 3);//UDflip
//	//int index = (j * nx_s * 3 + (nx_s-i-1) * 3);//LRflip
//
//	//float Rwl = 660 * nm;
//	//float Gwl = 532 * nm;
//	//float Bwl = 457 * nm;
//
//	//float lambda = Rwl; //wavelength같은것
//	float waveLength[3] = { 660 * 1e-9 ,532 * 1e-9 ,457 * 1e-9 };
//
//	float theta_x = asin((waveLength[2] / (2 * dx))); // diffraction angle
//	float theta_y = asin((waveLength[2] / (2 * dy)));
//
//	float drx = abs(djh - z)*tan(theta_x);
//	float dry = abs(djh - z)*tan(theta_y);
//
//	//thrust::complex<float> tempjj;
//	//if (pow((xa - x) / drx, 2) + pow((ya - y) / dry, 2) <= 1)
//	//{
//	//	R = (-z / abs(z))*sqrt(pow(djh - z, 2) + pow(xa - x, 2) + pow(ya - y, 2));
//	//	A = exp(i2PI*rand) / R;
//	//	CGH = B * (dx*dy) / imgI;
//	//	tempjj = i2PI * (xa / (2 * dx)*X_Shift + ya / (2 * dy)*Y_Shift);
//	//	for (int i = 0; i < 3; i++)
//	//	{
//	//		A = A*exp(i2PI*R / waveLength[i]) * o_intensity[pi_index * 3 + i];//intensity는 A에 상수값으로 곱해준다 색깔별로
//	//		CGH = A *CGH / waveLength[i]*exp(tempjj*(waveLength[2] / waveLength[i]));
//	//		atomicAdd(&real_part_hologram[index + i], (CGH.real()));
//	//		atomicAdd(&imagery_part_hologram[index + i], (CGH.imag()));
//	//	}
//	//}
//	
//	if (pow((xa - x) / drx, 2) + pow((ya - y) / dry, 2) <= 1)
//	{
//		R = (-z / abs(z))*sqrt(pow(djh - z, 2) + pow(xa - x, 2) + pow(ya - y, 2));
//		for (int i = 0; i < 3; i++)
//		{
//			A = exp(i2PI*rand)*exp(i2PI*R / waveLength[i]) / R * o_intensity[pi_index * 3 + i];//intensity는 A에 상수값으로 곱해준다 색깔별로
//			CGH = A * B*(dx*dy) / (imgI*waveLength[i])*exp(i2PI*(xa / (2 * dx)*X_Shift*(waveLength[2] / waveLength[i]) + ya / (2 * dy)*Y_Shift*(waveLength[2] / waveLength[i])));
//			atomicAdd(&real_part_hologram[index + i], (CGH.real()));
//			atomicAdd(&imagery_part_hologram[index + i], (CGH.imag()));
//		}
//	}
//}


#include <QtGui/QImage>

void HologramGenerator::point_sources_method_xy_plane_CUDA()
{
	const int nx = config_PC_->pixel_number[0];
	const int ny = config_PC_->pixel_number[1];
	const int N = nx * ny;
	const int M = data_PC_->n_points;

	//if (!start)			cudaEventCreate(&start);
	//if (!stop)			cudaEventCreate(&stop);

	for (int gpu_i = 0; gpu_i < num_gpu_; gpu_i++)
	{
		SetCurrentGPU(gpu_i);

		HANDLE_ERROR(cudaMemsetAsync(save_a_d_[gpu_i], 0, sizeof(float)*N* 3));
		HANDLE_ERROR(cudaMemsetAsync(save_b_d_[gpu_i], 0, sizeof(float)*N* 3));

		HANDLE_ERROR(cudaMemcpyAsync(obj_position_d_[gpu_i], data_PC_->ObjPosition, sizeof(float)* M * 3, cudaMemcpyHostToDevice));
		HANDLE_ERROR(cudaMemcpyAsync(obj_intensity_d_[gpu_i], data_PC_->ObjIntensity, sizeof(float)* M * 3, cudaMemcpyHostToDevice));

	}

	const double pixelSize = config_PC_->pixel_pitch[0];
	const double hcx = 0.0;
	const double hcy = 0.0;
	const double hcz = 0.0;
	const double waveLength = config_PC_->wave_length;

	unsigned int nblocks;

#ifdef DEBUGMSG
	LOG("object number %d\n", M);
#endif

	int len = 0;
	double len_d;
	double pz = data_PC_->ObjPosition[2];

	//find_size(len_d, len, pz, pixelSize, hcz, waveLength);
	find_size(len_d, len, pz, pixelSize, hcz);//JH
	//LOG("Sub hologram size=%d # of points=%d\n", len, M);

	nblocks = (M + kBlockThreads - 1) / kBlockThreads;

	for (int gpu_i = 0; gpu_i < num_gpu_; gpu_i++)
	{
		SetCurrentGPU(gpu_i);
		set_subhologram_position << <nblocks, kBlockThreads, 0 >> >
			(M, obj_hologram_pos_[gpu_i], obj_position_d_[gpu_i], len_d, nx, ny, pixelSize, hcx, hcy, hcz);

	}

	int* block_threads = new int[num_gpu_];
	int* work_load = new int[num_gpu_];

	for (int gpu_i = 0; gpu_i < num_gpu_; gpu_i++)
	{
		SetCurrentGPU(gpu_i);

		block_threads[gpu_i] = 256;
		work_load[gpu_i] = int(float(len * len) / float(core_cnt_[gpu_i]) + 0.5) * core_cnt_[gpu_i];
		while (core_cnt_[gpu_i] % block_threads[gpu_i] != 0) {
			block_threads[gpu_i]--;
		}
#ifdef DEBUGMSG
		_cprintf("sub holo size %d, work_load %d, block threads %d\n", len, work_load[gpu_i], block_threads[gpu_i]);
#endif
	}

	int pi = 0;

	while (pi < M - (kPointBundle*num_gpu_))
	{
		for (int gpu_i = 0; gpu_i < num_gpu_ ; gpu_i++)
		{
			SetCurrentGPU(gpu_i);

			dim3 grid((work_load[gpu_i] + block_threads[gpu_i] - 1) / block_threads[gpu_i], kPointBundle, 1);

			//LOG("M:%d, pi:%d, grid:%d %d %d\n", M, pi, grid.x, grid.y, grid.z);

			/*point_sources_kernel_dp << < grid, block_threads[gpu_i], 0 >> >
				(len, obj_hologram_pos_[gpu_i], save_a_d_[gpu_i], save_b_d_[gpu_i], obj_position_d_[gpu_i], obj_intensity_d_[gpu_i],
					nx, ny, pixelSize, TWO_PI_DP, waveLength, pi);*/

			point_sources_kernel_dp << < grid, block_threads[gpu_i], 0 >> >
				(len, obj_hologram_pos_[gpu_i], save_a_d_[gpu_i], save_b_d_[gpu_i], obj_position_d_[gpu_i], obj_intensity_d_[gpu_i],
					nx, ny, pixelSize, TWO_PI_DP, pi);//BJJ

			pi += kPointBundle;
		}
	}
	
	if (pi < M) {

		for (int gpu_i = 0; gpu_i < num_gpu_ && pi < M; gpu_i++)
		{
			SetCurrentGPU(gpu_i);

			dim3 grid((work_load[gpu_i] + block_threads[gpu_i] - 1) / block_threads[gpu_i], kPointBundle, 1);
			if (pi + kPointBundle >= M)
				grid.y = M - pi;

			//LOG("M:%d, pi:%d, grid:%d %d %d\n", M, pi, grid.x, grid.y, grid.z);

			/*point_sources_kernel_dp << < grid, block_threads[gpu_i], 0 >> >
				(len, obj_hologram_pos_[gpu_i], save_a_d_[gpu_i], save_b_d_[gpu_i], obj_position_d_[gpu_i], obj_intensity_d_[gpu_i],
					nx, ny, pixelSize, TWO_PI_DP, waveLength, pi);*/
			point_sources_kernel_dp << < grid, block_threads[gpu_i], 0 >> >
				(len, obj_hologram_pos_[gpu_i], save_a_d_[gpu_i], save_b_d_[gpu_i], obj_position_d_[gpu_i], obj_intensity_d_[gpu_i],
					nx, ny, pixelSize, TWO_PI_DP, pi);//BJJ

			pi += kPointBundle;
		}

	}


	cudaSetDevice(0);
	HANDLE_ERROR(cudaMemcpyAsync(cghFieldReal_GPU_, save_a_d_[0], N * sizeof(float) * 3, cudaMemcpyDeviceToDevice));
	HANDLE_ERROR(cudaMemcpyAsync(cghFieldImag_GPU_, save_b_d_[0], N * sizeof(float) * 3, cudaMemcpyDeviceToDevice));

	if (num_gpu_ > 1)
	{
		for (int gpu_i = 1; gpu_i < num_gpu_; gpu_i++)
		{
			cudaSetDevice(gpu_i);

			float * ptr_real = cghFieldReal_GPU_ + (N * 3 * gpu_i);
			float * ptr_imag = cghFieldImag_GPU_ + (N * 3 * gpu_i);

			HANDLE_ERROR(cudaMemcpyPeerAsync(ptr_real, 0, save_a_d_[gpu_i], gpu_i, N * sizeof(float) * 3));
			HANDLE_ERROR(cudaMemcpyPeerAsync(ptr_imag, 0, save_b_d_[gpu_i], gpu_i, N * sizeof(float) * 3));
		}

	}

	cudaSetDevice(0);
	//HANDLE_ERROR(cudaMemsetAsync(complex_H_GPU_, 0, sizeof(cufftComplex)*N));
	HANDLE_ERROR(cudaMemsetAsync(complex_H_GPU_R_, 0, sizeof(cufftComplex)*N));
	HANDLE_ERROR(cudaMemsetAsync(complex_H_GPU_G_, 0, sizeof(cufftComplex)*N));
	HANDLE_ERROR(cudaMemsetAsync(complex_H_GPU_B_, 0, sizeof(cufftComplex)*N));

	//cudaMergeNCopytoComplex(nx, ny, num_gpu_, cghFieldReal_GPU_, cghFieldImag_GPU_, complex_H_GPU_);
	cudaMergeNCopytoComplex(nx, ny, num_gpu_, cghFieldReal_GPU_, cghFieldImag_GPU_, complex_H_GPU_R_, complex_H_GPU_G_, complex_H_GPU_B_);

	delete[] block_threads, work_load;
}






void HologramGenerator::clear_GPU_Var()
{
	for (int gpu_i = 0; gpu_i < num_gpu_; gpu_i++) 
	{
		SetCurrentGPU(gpu_i);

		if (save_a_d_[gpu_i])			cudaFree(save_a_d_[gpu_i]);
		if (save_b_d_[gpu_i])			cudaFree(save_b_d_[gpu_i]);
		if (obj_hologram_pos_[gpu_i])	cudaFree(obj_hologram_pos_[gpu_i]);
		if (obj_position_d_[gpu_i])		cudaFree(obj_position_d_[gpu_i]);
		if (obj_intensity_d_[gpu_i])	cudaFree(obj_intensity_d_[gpu_i]);
		
		save_a_d_[gpu_i] = 0;
		save_b_d_[gpu_i] = 0;

		obj_position_d_[gpu_i] = 0;
		obj_intensity_d_[gpu_i] = 0;
		obj_hologram_pos_[gpu_i] = 0;

	}

	SetCurrentGPU(0);

	if (cghFieldReal_GPU_)	cudaFree(cghFieldReal_GPU_);
	if (cghFieldImag_GPU_)	cudaFree(cghFieldImag_GPU_);
	cghFieldReal_GPU_ = 0;
	cghFieldImag_GPU_ = 0;


	if (k_temp_d_)		HANDLE_ERROR(cudaFree(k_temp_d_));
	if (k_temp2_d_)		HANDLE_ERROR(cudaFree(k_temp2_d_));
	//if (complex_H_GPU_)	HANDLE_ERROR(cudaFree(complex_H_GPU_));
	if (complex_H_GPU_R_)	HANDLE_ERROR(cudaFree(complex_H_GPU_R_));
	if (complex_H_GPU_G_)	HANDLE_ERROR(cudaFree(complex_H_GPU_G_));
	if (complex_H_GPU_B_)	HANDLE_ERROR(cudaFree(complex_H_GPU_B_));
	k_temp_d_ = 0;
	k_temp2_d_ = 0;
	//complex_H_GPU_ = 0;
	complex_H_GPU_R_ = 0;
	complex_H_GPU_G_ = 0;
	complex_H_GPU_B_ = 0;

	//if (holo_encoded_GPU_)		HANDLE_ERROR(cudaFree(holo_encoded_GPU_));
	if (holo_encoded_GPU_R_)		HANDLE_ERROR(cudaFree(holo_encoded_GPU_R_));
	if (holo_encoded_GPU_G_)		HANDLE_ERROR(cudaFree(holo_encoded_GPU_G_));
	if (holo_encoded_GPU_B_)		HANDLE_ERROR(cudaFree(holo_encoded_GPU_B_));
	if (holo_normalized_GPU_)	HANDLE_ERROR(cudaFree(holo_normalized_GPU_));
	//holo_encoded_GPU_ = 0;
	holo_encoded_GPU_R_ = 0;
	holo_encoded_GPU_G_ = 0;
	holo_encoded_GPU_B_ = 0;
	holo_normalized_GPU_ = 0;

	if (reduce_source_) delete reduce_source_;
	if (reduce_min_) delete reduce_min_;
	if (reduce_max_) delete reduce_max_;

	reduce_source_ = 0;
	reduce_min_ = 0;
	reduce_max_ = 0;

}



void HologramGenerator::normalize_gpu()
{
	ivec2 pn = config_PC_->pixel_number;
	int pnx = pn[0];
	int pny = pn[1];
	int N = pnx * pny;

	SetCurrentGPU(0);

	//reduce_source_->set_mem(holo_encoded_GPU_);
	float* min_v_mem;
	float* max_v_mem;
	cudaMalloc(&min_v_mem, sizeof(float));
	cudaMalloc(&max_v_mem, sizeof(float));
	reduce_min_->set_mem(min_v_mem);
	reduce_max_->set_mem(max_v_mem);
	reduce_source_->min_reduce(0, reduce_min_, 1.0);
	reduce_source_->max_reduce(0, reduce_max_, 1.0);

	dim3 grid((N + kBlockThreads - 1) / kBlockThreads, 1, 1);
	//grey_normalize << < grid, kBlockThreads, 0 >> > (holo_encoded_GPU_, holo_normalized_GPU_, min_v_mem, max_v_mem, pnx, pny);
	cudaMemcpy(holo_normalized_, holo_normalized_GPU_, N * sizeof(uchar), cudaMemcpyDeviceToHost);

	cudaFree(min_v_mem);
	cudaFree(max_v_mem);

}
void grey_normalize_api(float* src, uchar* dst, float* min_v, float* max_v, int nx, int ny)
{
    int N = nx * ny;
    dim3 grid((N + kBlockThreads - 1) / kBlockThreads, 1, 1);
    grey_normalize << < grid, kBlockThreads, 0 >> > (src, dst, min_v, max_v, nx, ny);

}
void HologramGenerator::cudaCropFringe(int nx, int ny, cufftComplex* in_field, cufftComplex* out_field, int cropx1, int cropx2, int cropy1, int cropy2)
{
	unsigned int nblocks = (nx*ny + kBlockThreads - 1) / kBlockThreads;

	cropFringe << < nblocks, kBlockThreads, 0 >> > (nx, ny, in_field, out_field, cropx1, cropx2, cropy1, cropy2);
}

void HologramGenerator::cudaFFT(int nx, int ny, cufftComplex* in_field, cufftComplex* output_field, int direction, bool bNormalized)
{
	unsigned int nblocks = (nx*ny + kBlockThreads - 1) / kBlockThreads;
	int N = nx * ny;
	kernel_fftShift << <nblocks, kBlockThreads, 0 >> > (N, nx, ny, in_field, output_field, false);

	cufftHandle plan;

	// fft
	if (cufftPlan2d(&plan, ny, nx, CUFFT_C2C) != CUFFT_SUCCESS)
	{
		//LOG("FAIL in creating cufft plan");
		return;
	};

	cufftResult result;

	if (direction == -1)
		result = cufftExecC2C(plan, output_field, in_field, CUFFT_FORWARD);
	else
		result = cufftExecC2C(plan, output_field, in_field, CUFFT_INVERSE);

	if (result != CUFFT_SUCCESS)
	{
		//LOG("------------------FAIL: execute cufft, code=%s", result);
		return;
	}

	if (cudaDeviceSynchronize() != cudaSuccess) {
		//LOG("Cuda error: Failed to synchronize\n");
		return;
	}

	kernel_fftShift << < nblocks, kBlockThreads, 0 >> > (N, nx, ny, in_field, output_field, bNormalized);

	cufftDestroy(plan);
}

void HologramGenerator::cudaGetFringe(int pnx, int pny, cufftComplex* in_field, cufftComplex* out_field, int sig_locationx, int sig_locationy,
	double ssx, double ssy, double ppx, double ppy, double PI)
{
	unsigned int nblocks = (pnx*pny + kBlockThreads - 1) / kBlockThreads;

	getFringe << < nblocks, kBlockThreads, 0 >> > (pnx, pny, in_field, out_field, sig_locationx, sig_locationy, ssx, ssy, ppx, ppy, PI);
}

void HologramGenerator::cudaGetRealpart(int pnx, int pny, cufftComplex* in_field, float* out_field)
{
	unsigned int nblocks = (pnx*pny + kBlockThreads - 1) / kBlockThreads;

	getRealPart << < nblocks, kBlockThreads, 0 >> > (pnx, pny, in_field, out_field);
	   
}

void HologramGenerator::cudaMergeNCopytoComplex(int pnx, int pny, int num_gpu, float* src_real, float* src_imag, cufftComplex* dstR, cufftComplex* dstG, cufftComplex* dstB)
{
	unsigned int nblocks = (pnx*pny + kBlockThreads - 1) / kBlockThreads;

	kernel_Merge << < nblocks, kBlockThreads, 0 >> > (pnx, pny, num_gpu, src_real, src_imag);


	kernel_CopytoComplex << < nblocks, kBlockThreads, 0 >> > (pnx, pny, src_real, src_imag, dstR, dstG, dstB);
}


