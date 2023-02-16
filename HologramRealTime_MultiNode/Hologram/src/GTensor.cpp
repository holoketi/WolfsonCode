#include "Hologram/GTensor.h"
#include <vector>
#include <conio.h>
#include <cuda_runtime.h>

void setTensorDesc(cudnnTensorDescriptor_t& tensorDesc,
	cudnnTensorFormat_t tensorFormat,
	cudnnDataType_t dataType,
	int n,
	int c,
	int h,
	int w)
{

	const int nDims = 4;
	int dimA[nDims] = { n,c,h,w };
	int strideA[nDims] = { c*h*w, h*w, w, 1 };
	cudnnSetTensorNdDescriptor(tensorDesc,
		dataType,
		4,
		dimA,
		strideA);

}

#ifndef max
#define max(a,b) (a>b?a:b)
#endif
cudnnHandle_t* kDnnHandle[4];
bool GTensorInitialized = false;

void GTensorInitialize() {

	if (GTensorInitialized) return;
	GTensorInitialized = true;
	int gpu_cnt;
	cudaGetDeviceCount(&gpu_cnt);

	for (int i = 0; i < gpu_cnt; i++) {
		cudaSetDevice(i);

		cudnnHandle_t* handle_ = new cudnnHandle_t;
		cudnnStatus_t ret = cudnnCreate(handle_);
		kDnnHandle[i] = handle_;
	}

	cudaSetDevice(0);
}
struct GTensorWorkspaceManager {
	std::vector<void*> work_;
	std::vector<size_t> work_size_;
	std::vector<void*> temp_work_;
	std::vector<size_t> temp_work_size_;
	static GTensorWorkspaceManager* kGTensorMemManager;

	static GTensorWorkspaceManager* Instance()
	{
		if (!kGTensorMemManager) kGTensorMemManager = new GTensorWorkspaceManager();
		return kGTensorMemManager;
	}
	// max gpu count 10
	GTensorWorkspaceManager() : work_(10, 0), work_size_(10, 0)
	{
	}

	void free()
	{
		for (int i = 0; i < 10; i++) {
			if (work_[i]) {
				cudaSetDevice(i);
				cudaFree(work_[i]);
				work_[i] = 0;
			}
				
		}
	}
	void alloc_work(int gpu_id, size_t s) {
		if (max(work_size_[gpu_id], s) != work_size_[gpu_id]) {
			if (work_[gpu_id]) {
				cudaSetDevice(gpu_id);
				cudaFree(work_[gpu_id]);
				work_[gpu_id] = 0;
			}
		}
		work_size_[gpu_id] = max(work_size_[gpu_id], s);
	}
		

	void* get_work(int gpu_id) {
		if (work_[gpu_id]) return work_[gpu_id];
		cudaSetDevice(gpu_id);
		cudaMalloc(&work_[gpu_id], work_size_[gpu_id]);
		return work_[gpu_id];
	}
};

GTensorWorkspaceManager* GTensorWorkspaceManager::kGTensorMemManager;

GTensor::GTensor()
	:N(1), C(1), H(1), W(1), reduce_workspace_size_(10, 0)
{
	GTensorInitialize();
	cudnnCreateTensorDescriptor(&tensor_desc_);
	cudnnCreateReduceTensorDescriptor(&reduce_tensor_desc_);
	cudnnSetReduceTensorDescriptor(reduce_tensor_desc_,
		CUDNN_REDUCE_TENSOR_ADD,
		CUDNN_DATA_FLOAT,
		CUDNN_PROPAGATE_NAN,
		CUDNN_REDUCE_TENSOR_NO_INDICES,
		CUDNN_32BIT_INDICES);
	cudnnCreateReduceTensorDescriptor(&min_tensor_desc_);
	cudnnSetReduceTensorDescriptor(min_tensor_desc_,
		CUDNN_REDUCE_TENSOR_MIN,
		CUDNN_DATA_FLOAT,
		CUDNN_PROPAGATE_NAN,
		CUDNN_REDUCE_TENSOR_NO_INDICES,
		CUDNN_32BIT_INDICES);
	cudnnCreateReduceTensorDescriptor(&max_tensor_desc_);
	cudnnSetReduceTensorDescriptor(max_tensor_desc_,
		CUDNN_REDUCE_TENSOR_MAX,
		CUDNN_DATA_FLOAT,
		CUDNN_PROPAGATE_NAN,
		CUDNN_REDUCE_TENSOR_NO_INDICES,
		CUDNN_32BIT_INDICES);
	cudnnCreateOpTensorDescriptor(&mul_tensor_desc_);
	cudnnSetOpTensorDescriptor(mul_tensor_desc_,
		CUDNN_OP_TENSOR_MUL,
		CUDNN_DATA_FLOAT,
		CUDNN_PROPAGATE_NAN);
	cudnnCreateOpTensorDescriptor(&add_tensor_desc_);
	cudnnSetOpTensorDescriptor(add_tensor_desc_,
		CUDNN_OP_TENSOR_ADD,
		CUDNN_DATA_FLOAT,
		CUDNN_PROPAGATE_NAN);
}
GTensor::GTensor(int n, int c, int h, int w)
	: N(n), C(c), H(h), W(w), reduce_workspace_size_(10,0)
{
	GTensorInitialize();
	cudnnCreateTensorDescriptor(&tensor_desc_);
	setTensorDesc(tensor_desc_, CUDNN_TENSOR_NCHW, CUDNN_DATA_FLOAT, N, C, H, W);
	cudnnCreateReduceTensorDescriptor(&reduce_tensor_desc_);
	cudnnSetReduceTensorDescriptor(reduce_tensor_desc_,
		CUDNN_REDUCE_TENSOR_ADD,
		CUDNN_DATA_FLOAT,
		CUDNN_PROPAGATE_NAN,
		CUDNN_REDUCE_TENSOR_NO_INDICES,
		CUDNN_32BIT_INDICES);
	cudnnCreateReduceTensorDescriptor(&min_tensor_desc_);
	cudnnSetReduceTensorDescriptor(min_tensor_desc_,
		CUDNN_REDUCE_TENSOR_MIN,
		CUDNN_DATA_FLOAT,
		CUDNN_PROPAGATE_NAN,
		CUDNN_REDUCE_TENSOR_NO_INDICES,
		CUDNN_32BIT_INDICES);
	cudnnCreateReduceTensorDescriptor(&max_tensor_desc_);
	cudnnSetReduceTensorDescriptor(max_tensor_desc_,
		CUDNN_REDUCE_TENSOR_MAX,
		CUDNN_DATA_FLOAT,
		CUDNN_PROPAGATE_NAN,
		CUDNN_REDUCE_TENSOR_NO_INDICES,
		CUDNN_32BIT_INDICES);
	cudnnCreateOpTensorDescriptor(&mul_tensor_desc_);
	cudnnSetOpTensorDescriptor(mul_tensor_desc_,
		CUDNN_OP_TENSOR_MUL,
		CUDNN_DATA_FLOAT,
		CUDNN_PROPAGATE_NAN);
	cudnnCreateOpTensorDescriptor(&add_tensor_desc_);
	cudnnSetOpTensorDescriptor(add_tensor_desc_,
		CUDNN_OP_TENSOR_ADD,
		CUDNN_DATA_FLOAT,
		CUDNN_PROPAGATE_NAN);
}

void GTensor::set_tensor(int n, int c, int h, int w)
{
	N = (n); 
	C = (c); 
	H = (h); 
	W = (w);

	setTensorDesc(tensor_desc_, CUDNN_TENSOR_NCHW, CUDNN_DATA_FLOAT, n, c, h, w);
}

GTensor::~GTensor()
{
	cudnnDestroyTensorDescriptor(tensor_desc_);
	cudnnDestroyReduceTensorDescriptor(reduce_tensor_desc_);
	cudnnDestroyReduceTensorDescriptor(min_tensor_desc_);
	cudnnDestroyReduceTensorDescriptor(max_tensor_desc_);
	cudnnDestroyOpTensorDescriptor(mul_tensor_desc_);
	cudnnDestroyOpTensorDescriptor(add_tensor_desc_);
}
void GTensor::prepare_reduction(int gpu_id_, GTensor* A)
{
	cudaSetDevice(gpu_id_);
	cudnnGetReductionWorkspaceSize(*kDnnHandle[gpu_id_],
		reduce_tensor_desc_, tensor_desc_, A->tensor_desc(), &reduce_workspace_size_[gpu_id_]);

	GTensorWorkspaceManager::Instance()->alloc_work(gpu_id_, reduce_workspace_size_[gpu_id_]);
}

void GTensor::reduce(int gpu_id_, GTensor* A, float a, float c)
{
	prepare_reduction(gpu_id_, A);
	cudnnStatus_t status = cudnnReduceTensor(*kDnnHandle[gpu_id_],
		reduce_tensor_desc_,
		NULL, 0,
		GTensorWorkspaceManager::Instance()->get_work(gpu_id_),
		reduce_workspace_size_[gpu_id_],
		&a,
		tensor_desc_,
		mem_,
		&c,
		A->tensor_desc(),
		A->mem());
	if (status != CUDNN_STATUS_SUCCESS) {
		_cprintf("Tensor::reduce CUDNN failure\nError: %s\n", cudnnGetErrorString(status));      \
	}
}

void GTensor::prepare_min_reduction(int gpu_id_, GTensor* A)
{
	cudaSetDevice(gpu_id_);
	cudnnGetReductionWorkspaceSize(*kDnnHandle[gpu_id_],
		min_tensor_desc_, tensor_desc_, A->tensor_desc(), &reduce_workspace_size_[gpu_id_]);

	GTensorWorkspaceManager::Instance()->alloc_work(gpu_id_, reduce_workspace_size_[gpu_id_]);
}

void GTensor::min_reduce(int gpu_id_, GTensor* A, float a, float c)
{
	prepare_min_reduction(gpu_id_, A);
	cudnnStatus_t status = cudnnReduceTensor(*kDnnHandle[gpu_id_],
		min_tensor_desc_,
		NULL, 0,
		GTensorWorkspaceManager::Instance()->get_work(gpu_id_),
		reduce_workspace_size_[gpu_id_],
		&a,
		tensor_desc_,
		mem_,
		&c,
		A->tensor_desc(),
		A->mem());
	if (status != CUDNN_STATUS_SUCCESS) {
		_cprintf("Tensor::min reduce CUDNN failure\nError: %s\n", cudnnGetErrorString(status));      \
	}
}


void GTensor::prepare_max_reduction(int gpu_id_, GTensor* A)
{
	cudaSetDevice(gpu_id_);
	cudnnGetReductionWorkspaceSize(*kDnnHandle[gpu_id_],
		max_tensor_desc_, tensor_desc_, A->tensor_desc(), &reduce_workspace_size_[gpu_id_]);

	GTensorWorkspaceManager::Instance()->alloc_work(gpu_id_, reduce_workspace_size_[gpu_id_]);
}

void GTensor::max_reduce(int gpu_id_, GTensor* A, float a, float c)
{
	prepare_max_reduction(gpu_id_, A);
	cudnnStatus_t status = cudnnReduceTensor(*kDnnHandle[gpu_id_],
		max_tensor_desc_,
		NULL, 0,
		GTensorWorkspaceManager::Instance()->get_work(gpu_id_),
		reduce_workspace_size_[gpu_id_],
		&a,
		tensor_desc_,
		mem_,
		&c,
		A->tensor_desc(),
		A->mem());
	if (status != CUDNN_STATUS_SUCCESS) {
		_cprintf("Tensor::max reduce CUDNN failure\nError: %s\n", cudnnGetErrorString(status));      \
	}
}
void GTensor::scale(int gpu_id_, float val)
{
	cudnnStatus_t status = cudnnScaleTensor(*kDnnHandle[gpu_id_],
		tensor_desc_,
		mem_,
		&val);
	if (status != CUDNN_STATUS_SUCCESS) {
		_cprintf("GTensor::scale CUDNN failure\nError: %s\n", cudnnGetErrorString(status));      \
	}
}
void GTensor::mul(int gpu_id_, GTensor* A, float a, GTensor* B, float b, float c)
{
	cudnnStatus_t status = cudnnOpTensor(*kDnnHandle[gpu_id_],
		mul_tensor_desc_,
		&a,
		A->tensor_desc(),
		A->mem(),
		&b,
		B->tensor_desc(),
		B->mem(),
		&c,
		tensor_desc_,
		mem_);
	if (status != CUDNN_STATUS_SUCCESS) {
		_cprintf("GTensor::mul CUDNN failure\nError: %s\n", cudnnGetErrorString(status));      \
	}
}

void GTensor::set(int gpu_id_, float val)
{
	cudnnStatus_t status = cudnnSetTensor(*kDnnHandle[gpu_id_],
		tensor_desc_, mem_, &val);

	if (status != CUDNN_STATUS_SUCCESS) {
		_cprintf("GTensor::set CUDNN failure\nError: %s\n", cudnnGetErrorString(status));      \
	}
}

void GTensor::add(int gpu_id_, GTensor* A, float a, GTensor* B, float b, float c)
{

	cudnnStatus_t status = cudnnOpTensor(*kDnnHandle[gpu_id_],
		add_tensor_desc_,
		&a,
		A->tensor_desc(),
		A->mem(),
		&b,
		B->tensor_desc(),
		B->mem(),
		&c,
		tensor_desc_,
		mem_);

	if (status != CUDNN_STATUS_SUCCESS) {
		_cprintf("GTensor::add CUDNN failure\nError: %s\n", cudnnGetErrorString(status));      \
	}
}
