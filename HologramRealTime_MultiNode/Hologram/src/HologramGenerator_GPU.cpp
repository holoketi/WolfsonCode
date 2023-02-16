
#include	"Hologram/HologramGenerator_GPU.h"
#include	"graphics/sys.h"

void HologramGenerator::genCghPointCloudGPU()
{
	DeviceInitialize();

	point_sources_method_xy_plane_CUDA();


}

//void HologramGenerator::find_size(double& len_d, int& len, double pos_z, double pixelSize, double hcz, double Wavelength)
//{
//	double oz2 = (int)(pos_z / Wavelength) * (Wavelength);
//	double l = kSubhologramSize * Wavelength + fabs(hcz - oz2);
//	len_d = sqrt(l*l - (hcz - oz2)*(hcz - oz2));
//	len = ceil((len_d / pixelSize) * 2);
//	
//}

void HologramGenerator::find_size(double& len_d, int& len, double pos_z, double pixelSize, double hcz) //JH
{
	len_d = 2000 / 2 * pixelSize;
	len = ceil((len_d / pixelSize) * 2);
}


void HologramGenerator::SetCurrentGPU(int id)
{
	cudaSetDevice(id);
	cur_gpu_ = id;
}

void HologramGenerator::init_GPU()
{
	const int nx = config_PC_->pixel_number[0];
	const int ny = config_PC_->pixel_number[1];
	const int N = nx * ny;

	cudaGetDeviceCount(&num_gpu_);
	LOG("init_gpu %d\n", num_gpu_);

	core_cnt_ = new int[num_gpu_];

	cudaDeviceProp deviceProp;
	for (int gpu_i = 0; gpu_i < num_gpu_; gpu_i++)
	{
		SetCurrentGPU(gpu_i);

		cudaGetDeviceProperties(&deviceProp, gpu_i);
		core_cnt_[gpu_i] = _ConvertSMVer2Cores(deviceProp.major, deviceProp.minor) * deviceProp.multiProcessorCount;
		LOG("GPU core count %d\n", core_cnt_[gpu_i]);

		if (save_a_d_[gpu_i])			HANDLE_ERROR(cudaFree(save_a_d_[gpu_i]));
		if (save_b_d_[gpu_i])			HANDLE_ERROR(cudaFree(save_b_d_[gpu_i]));
		HANDLE_ERROR(cudaMalloc(&save_a_d_[gpu_i], N * sizeof(float) * 3));
		HANDLE_ERROR(cudaMalloc(&save_b_d_[gpu_i], N * sizeof(float) * 3));

	}

	SetCurrentGPU(0);

	if (cghFieldReal_GPU_)		HANDLE_ERROR(cudaFree(cghFieldReal_GPU_));
	if (cghFieldImag_GPU_)		HANDLE_ERROR(cudaFree(cghFieldImag_GPU_));
	HANDLE_ERROR(cudaMalloc(&cghFieldReal_GPU_, N * sizeof(float) * 3 * num_gpu_));
	HANDLE_ERROR(cudaMalloc(&cghFieldImag_GPU_, N * sizeof(float) * 3 * num_gpu_));

	if (k_temp_d_)		HANDLE_ERROR(cudaFree(k_temp_d_));
	if (k_temp2_d_)		HANDLE_ERROR(cudaFree(k_temp2_d_));
	//if (complex_H_GPU_)	HANDLE_ERROR(cudaFree(complex_H_GPU_));
	if (complex_H_GPU_R_)	HANDLE_ERROR(cudaFree(complex_H_GPU_R_));
	if (complex_H_GPU_G_)	HANDLE_ERROR(cudaFree(complex_H_GPU_G_));
	if (complex_H_GPU_B_)	HANDLE_ERROR(cudaFree(complex_H_GPU_B_));
	//HANDLE_ERROR(cudaMalloc((void**)&complex_H_GPU_, sizeof(cufftComplex) * N));
	HANDLE_ERROR(cudaMalloc((void**)&complex_H_GPU_R_, sizeof(cufftComplex) * N));
	HANDLE_ERROR(cudaMalloc((void**)&complex_H_GPU_G_, sizeof(cufftComplex) * N));
	HANDLE_ERROR(cudaMalloc((void**)&complex_H_GPU_B_, sizeof(cufftComplex) * N));
	HANDLE_ERROR(cudaMalloc((void**)&k_temp_d_, sizeof(cufftComplex) * N));
	HANDLE_ERROR(cudaMalloc((void**)&k_temp2_d_, sizeof(cufftComplex) * N));

	if (!reduce_source_) reduce_source_ = new GTensor(1, 1, ny, nx);
	if (!reduce_min_) reduce_min_ = new GTensor(1, 1, 1, 1);
	if (!reduce_max_) reduce_max_ = new GTensor(1, 1, 1, 1);

	//if (holo_encoded_GPU_)		HANDLE_ERROR(cudaFree(holo_encoded_GPU_));
	if (holo_encoded_GPU_R_)		HANDLE_ERROR(cudaFree(holo_encoded_GPU_R_));
	if (holo_encoded_GPU_G_)		HANDLE_ERROR(cudaFree(holo_encoded_GPU_G_));
	if (holo_encoded_GPU_B_)		HANDLE_ERROR(cudaFree(holo_encoded_GPU_B_));
	if (holo_normalized_GPU_)	HANDLE_ERROR(cudaFree(holo_normalized_GPU_));
	//HANDLE_ERROR(cudaMalloc(&holo_encoded_GPU_, N * sizeof(float)));
	HANDLE_ERROR(cudaMalloc(&holo_encoded_GPU_R_, N * sizeof(float)));
	HANDLE_ERROR(cudaMalloc(&holo_encoded_GPU_G_, N * sizeof(float)));
	HANDLE_ERROR(cudaMalloc(&holo_encoded_GPU_B_, N * sizeof(float)));
	if (holo_normalized_GPU_)	HANDLE_ERROR(cudaFree(holo_normalized_GPU_));
	HANDLE_ERROR(cudaMalloc(&holo_normalized_GPU_, N * sizeof(uchar)));


}

void HologramGenerator::DeviceInitialize()
{
	const int M = data_PC_->n_points;
	LOG("%d\n",M);
	if (M > max_points_) {
		max_points_ = M;

		for (int gpu_i = 0; gpu_i < num_gpu_; gpu_i++)
		{
			SetCurrentGPU(gpu_i);

			if (obj_hologram_pos_[gpu_i])		HANDLE_ERROR(cudaFree(obj_hologram_pos_[gpu_i]));
			if (obj_position_d_[gpu_i])			HANDLE_ERROR(cudaFree(obj_position_d_[gpu_i]));
			if (obj_intensity_d_[gpu_i])		HANDLE_ERROR(cudaFree(obj_intensity_d_[gpu_i]));

			HANDLE_ERROR(cudaMalloc((void**)&obj_hologram_pos_[gpu_i], M * sizeof(short) * 2));
			HANDLE_ERROR(cudaMalloc((void**)&obj_position_d_[gpu_i], 3 * M * sizeof(float)));
			HANDLE_ERROR(cudaMalloc((void**)&obj_intensity_d_[gpu_i], 3 * M * sizeof(float)));
		}
	}
}

void HologramGenerator::getCGHfromGPU()
{
	ivec2 pn = config_PC_->pixel_number;
	int pnx = pn[0];
	int pny = pn[1];

	cufftComplex* temp = (cufftComplex*)malloc(sizeof(cufftComplex)*pnx*pny);
	memset(temp, 0, sizeof(cufftComplex)*pnx*pny);

	SetCurrentGPU(0);

	//HANDLE_ERROR(cudaMemcpy(temp, complex_H_GPU_, sizeof(cufftComplex) * pnx * pny, cudaMemcpyDeviceToHost));
	HANDLE_ERROR(cudaMemcpy(temp, complex_H_GPU_G_, sizeof(cufftComplex) * pnx * pny, cudaMemcpyDeviceToHost));

	for (ulonglong k = 0; k < pnx*pny; k++)
	{
		complex_H_[k].a = (float)temp[k].x;
		complex_H_[k].b = (float)temp[k].y;
	}

	free(temp);
}

void HologramGenerator::readback_ssbR()
{
    ivec2 pn = config_PC_->pixel_number;
    int pnx = pn[0];
    int pny = pn[1];
    cudaMemcpy(holo_encoded_GPU_host_, holo_encoded_GPU_R_, sizeof(float)*pnx*pny, cudaMemcpyKind::cudaMemcpyDeviceToHost);
}

void HologramGenerator::readback_ssbG()
{
	ivec2 pn = config_PC_->pixel_number;
	int pnx = pn[0];
	int pny = pn[1];
	cudaMemcpy(holo_encoded_GPU_host_, holo_encoded_GPU_G_, sizeof(float)*pnx*pny, cudaMemcpyKind::cudaMemcpyDeviceToHost);
}

void HologramGenerator::readback_ssbB()
{
	ivec2 pn = config_PC_->pixel_number;
	int pnx = pn[0];
	int pny = pn[1];
	cudaMemcpy(holo_encoded_GPU_host_, holo_encoded_GPU_B_, sizeof(float)*pnx*pny, cudaMemcpyKind::cudaMemcpyDeviceToHost);
}
void HologramGenerator::encodeSideBand_GPU(int cropx1, int cropx2, int cropy1, int cropy2, ivec2 sig_location)
{
	ivec2 pn = config_PC_->pixel_number;
	int pnx = pn[0];
	int pny = pn[1];

	vec2 pp = config_PC_->pixel_pitch;
	double ppx = pp[0];
	double ppy = pp[1];
	double ssx = ppx * pnx;
	double ssy = ppy * pny;

	SetCurrentGPU(0);
	cudaMemsetAsync(k_temp2_d_, 0, sizeof(cufftComplex)*pnx*pny);
	//cudaMemcpy(k_temp2_d_, complex_H_GPU_, sizeof(cufftComplex)*pnx*pny, cudaMemcpyDeviceToDevice);

	cudaMemsetAsync(k_temp_d_, 0, sizeof(cufftComplex)*pnx*pny);
	cudaFFT(pnx, pny, k_temp2_d_, k_temp_d_, -1, false);

	cudaMemsetAsync(k_temp2_d_, 0, sizeof(cufftComplex)*pnx*pny);
	cudaCropFringe(pnx, pny, k_temp_d_, k_temp2_d_, cropx1, cropx2, cropy1, cropy2);

	cudaMemsetAsync(k_temp_d_, 0, sizeof(cufftComplex)*pnx*pny);
	cudaFFT(pnx, pny, k_temp2_d_, k_temp_d_, 1, false);

	cudaMemsetAsync(k_temp2_d_, 0, sizeof(cufftComplex)*pnx*pny);
	cudaGetFringe(pnx, pny, k_temp_d_, k_temp2_d_, sig_location[0], sig_location[1], ssx, ssy, ppx, ppy, M_PI);

	//cudaMemsetAsync(holo_encoded_GPU_, 0, sizeof(float)*pnx*pny);
	//cudaGetRealpart(pnx, pny,k_temp2_d_, holo_encoded_GPU_);

}

void HologramGenerator::GetReal()
{
	ivec2 pn = config_PC_->pixel_number;
	int pnx = pn[0];
	int pny = pn[1];

	SetCurrentGPU(0);
	cudaMemsetAsync(holo_encoded_GPU_R_, 0, sizeof(float)*pnx*pny);
	cudaGetRealpart(pnx, pny, complex_H_GPU_R_, holo_encoded_GPU_R_);

	cudaMemsetAsync(holo_encoded_GPU_G_, 0, sizeof(float)*pnx*pny);
	cudaGetRealpart(pnx, pny, complex_H_GPU_G_, holo_encoded_GPU_G_);

	cudaMemsetAsync(holo_encoded_GPU_B_, 0, sizeof(float)*pnx*pny);
	cudaGetRealpart(pnx, pny, complex_H_GPU_B_, holo_encoded_GPU_B_);
}