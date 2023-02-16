#pragma once

#include "Hologram/HologramGenerator.h"

#include <cufft.h>
#include <cuda.h>
#include <math.h>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>

const double TWO_PI_DP = 2.0*M_PI;

static int kBlockThreads = 512;
static const int kPointBundle = 64;
const double kSubhologramSize = 6;

static void handleError(cudaError_t err, const char *file, int line) {
	if (err != cudaSuccess) {
		printf("%s in %s at line %d\n", cudaGetErrorString(err), file, line);
		exit(EXIT_FAILURE);
	}
}
#define HANDLE_ERROR(err) (handleError(err, __FILE__, __LINE__))


#define HANDLE_NULL(a) { \
	if (a == NULL) { \
		printf("Host memory failed in %s at line %d\n", __FILE__, __LINE__); \
		exit(EXIT_FAILURE); \
	} \
}

// Beginning of GPU Architecture definitions
inline int _ConvertSMVer2Cores(int major, int minor) {
	// Defines for GPU Architecture types (using the SM version to determine
	// the # of cores per SM
	typedef struct {
		int SM;  // 0xMm (hexidecimal notation), M = SM Major version,
		// and m = SM minor version
		int Cores;
	} sSMtoCores;

	sSMtoCores nGpuArchCoresPerSM[] = {
		{0x30, 192},
		{0x32, 192},
		{0x35, 192},
		{0x37, 192},
		{0x50, 128},
		{0x52, 128},
		{0x53, 128},
		{0x60,  64},
		{0x61, 128},
		{0x62, 128},
		{0x70,  64},
		{0x72,  64},
		{0x75,  64},
		{-1, -1} };

	int index = 0;

	while (nGpuArchCoresPerSM[index].SM != -1) {
		if (nGpuArchCoresPerSM[index].SM == ((major << 4) + minor)) {
			return nGpuArchCoresPerSM[index].Cores;
		}

		index++;
	}

	// If we don't find the values, we default use the previous one
	// to run properly
	printf(
		"MapSMtoCores for SM %d.%d is undefined."
		"  Default to use %d Cores/SM\n",
		major, minor, nGpuArchCoresPerSM[index - 1].Cores);
	return nGpuArchCoresPerSM[index - 1].Cores;
}

void grey_normalize_api(float* src, uchar* dst, float* min_v, float* max_v, int nx, int ny);
